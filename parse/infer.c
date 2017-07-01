#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <inttypes.h>
#include <inttypes.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>

#include "util.h"
#include "parse.h"

typedef struct Inferstate Inferstate;
struct Inferstate {
	/* tracking where we are in the inference */
	int ingeneric;
	int inaggr;
	int innamed;
	int indentdepth;
	Type *ret;
	Srcloc *usrc;
	size_t nusrc;

	/* post-inference checking/unification */
	Htab *delayed;
	Node **postcheck;
	size_t npostcheck;
	Stab **postcheckscope;
	size_t npostcheckscope;

	/* type params bound at the current point */
	Htab **tybindings;
	size_t ntybindings;

	/* generic declarations to be specialized */
	Node **genericdecls;
	size_t ngenericdecls;
	Node **impldecl;
	size_t nimpldecl;

	/* specializations of generics */
	Node **specializations;
	size_t nspecializations;
	Stab **specializationscope;
	size_t nspecializationscope;
	Htab *seqbase;
};

static void infernode(Inferstate *st, Node **np, Type *ret, int *sawret);
static void inferexpr(Inferstate *st, Node **np, Type *ret, int *sawret);
static void inferdecl(Inferstate *st, Node *n);
static void typesub(Inferstate *st, Node *n, int noerr);
static void tybind(Inferstate *st, Type *t);
static Type *tyfix(Inferstate *st, Node *ctx, Type *orig, int noerr);
static void bind(Inferstate *st, Node *n);
static void tyunbind(Inferstate *st, Type *t);
static void unbind(Inferstate *st, Node *n);
static Type *unify(Inferstate *st, Node *ctx, Type *a, Type *b);
static Type *tf(Inferstate *st, Type *t);

static void ctxstrcall(char *buf, size_t sz, Inferstate *st, Node *n)
{
	char *p, *end, *sep, *t;
	size_t nargs, i;
	Node **args;
	Type *et;

	args = n->expr.args;
	nargs = n->expr.nargs;
	p = buf;
	end = buf + sz;
	sep = "";

	if (exprop(args[0]) == Ovar)
		p += bprintf(p, end - p, "%s(", namestr(args[0]->expr.args[0]));
	else
		p += bprintf(p, end - p, "<e>(");
	for (i = 1; i < nargs; i++) {
		et = tyfix(st, NULL, exprtype(args[i]), 1);
		if (et != NULL)
			t = tystr(et);
		else
			t = strdup("?");

		if (exprop(args[i]) == Ovar)
			p += bprintf(p, end - p, "%s%s:%s", sep, namestr(args[i]->expr.args[0]), t);
		else
			p += bprintf(p, end - p, "%s<e%zd>:%s", sep, i, t);
		sep = ", ";
		free(t);
	}
	if (exprtype(args[0])->nsub)
		t = tystr(tyfix(st, NULL, exprtype(args[0])->sub[0], 1));
	else
		t = strdup("unknown");
	p += bprintf(p, end - p, "): %s", t);
	free(t);
}

static char *nodetystr(Inferstate *st, Node *n)
{
	Type *t;

	t = NULL;
	if (n->type == Nexpr && exprtype(n) != NULL)
		t = tyfix(st, NULL, exprtype(n), 1);
	else if (n->type == Ndecl && decltype(n) != NULL)
		t = tyfix(st, n, decltype(n), 1);

	if (t && tybase(t)->type != Tyvar)
		return tystr(t);
	else
		return strdup("unknown");
}

static void marksrc(Inferstate *st, Type *t, Srcloc l)
{
	t = tf(st, t);
	if (t->tid >= st->nusrc) {
		st->usrc = zrealloc(st->usrc, st->nusrc*sizeof(Srcloc), (t->tid + 1)*sizeof(Srcloc));
		st->nusrc = t->tid + 1;
	}
	if (st->usrc[t->tid].line <= 0)
		st->usrc[t->tid] = l;
}

static char *srcstr(Inferstate *st, Type *ty)
{
	char src[128];
	Srcloc l;
	char *s;

	src[0] = 0;
	if (st->nusrc > ty->tid && st->usrc[ty->tid].line > 0) {
		l = st->usrc[ty->tid];
		s = tystr(ty);
		snprintf(src, sizeof src, "\n\t%s from %s:%d", s, fname(l), lnum(l));
		free(s);
	}
	return strdup(src);
}

/* Tries to give a good string describing the context
 * for the sake of error messages. */
static char *ctxstr(Inferstate *st, Node *n)
{
	char *t, *t1, *t2, *t3;
	char *s, *d;
	size_t nargs;
	Node **args;
	char buf[512];

	switch (n->type) {
	default: s = strdup(nodestr[n->type]); break;
	case Ndecl:
		 d = declname(n);
		 t = nodetystr(st, n);
		 bprintf(buf, sizeof buf, "%s:%s", d, t);
		 s = strdup(buf);
		 free(t);
		 break;
	case Nname:	s = strdup(namestr(n));	break;
	case Nexpr:
		args = n->expr.args;
		nargs = n->expr.nargs;
		t1 = NULL;
		t2 = NULL;
		t3 = NULL;
		if (exprop(n) == Ovar)
			d = namestr(args[0]);
		else
			d = opstr[exprop(n)];
		t = nodetystr(st, n);
		if (nargs >= 1)
			t1 = nodetystr(st, args[0]);
		if (nargs >= 2)
			t2 = nodetystr(st, args[1]);
		if (nargs >= 3)
			t3 = nodetystr(st, args[2]);

		switch (opclass[exprop(n)]) {
		case OTpre:	bprintf(buf, sizeof buf, "%s<e:%s>", oppretty[exprop(n)], t1);	break;
		case OTpost:	bprintf(buf, sizeof buf, "<e:%s>%s", t1, oppretty[exprop(n)]);	break;
		case OTzarg:	bprintf(buf, sizeof buf, "%s", oppretty[exprop(n)]);	break;
		case OTbin:
			bprintf(buf, sizeof buf, "<e1:%s> %s <e2:%s>", t1, oppretty[exprop(n)], t2);
			break;
		case OTmisc:
			switch (exprop(n)) {
			case Ovar:	bprintf(buf, sizeof buf, "%s:%s", namestr(args[0]), t);	break;
			case Ocall:	ctxstrcall(buf, sizeof buf, st, n);	break;
			case Oidx:
				if (exprop(args[0]) == Ovar)
					bprintf(buf, sizeof buf, "%s[<e1:%s>]", namestr(args[0]->expr.args[0]), t2);
				else
					bprintf(buf, sizeof buf, "<sl:%s>[<e1%s>]", t1, t2);
				break;
			case Oslice:
				if (exprop(args[0]) == Ovar)
					bprintf(buf, sizeof buf, "%s[<e1:%s>:<e2:%s>]", namestr(args[0]->expr.args[0]), t2, t3);
				else
					bprintf( buf, sizeof buf, "<sl:%s>[<e1%s>:<e2:%s>]", t1, t2, t3);
				break;
			case Omemb:
				bprintf(buf, sizeof buf, "<%s>.%s", t1, namestr(args[1]));
				break;
			default: 
				bprintf(buf, sizeof buf, "%s:%s", d, t);
				break;
			}
			break;
		default: bprintf(buf, sizeof buf, "%s", d); break;
		}
		free(t);
		free(t1);
		free(t2);
		free(t3);
		s = strdup(buf);
		break;
	}
	return s;
}

static void addspecialization(Inferstate *st, Node *n, Stab *stab)
{
	Node *dcl;

	dcl = decls[n->expr.did];
	lappend(&st->specializationscope, &st->nspecializationscope, stab);
	lappend(&st->specializations, &st->nspecializations, n);
	lappend(&st->genericdecls, &st->ngenericdecls, dcl);
}

static void additerspecializations(Inferstate *st, Node *n, Stab *stab)
{
	Trait *tr;
	Type *ty;
	size_t i;

	tr = traittab[Tciter];
	ty = exprtype(n->iterstmt.seq);
	if (!ty->traits || !bshas(ty->traits, Tciter))
		return;
	if (ty->type == Tyslice || ty->type == Tyarray || ty->type == Typtr)
		return;
	for (i = 0; i < tr->nproto; i++) {
		ty = exprtype(n->iterstmt.seq);
		if (hthas(tr->proto[i]->decl.impls, ty))
			continue;
		lappend(&st->specializationscope, &st->nspecializationscope, stab);
		lappend(&st->specializations, &st->nspecializations, n);
		lappend(&st->genericdecls, &st->ngenericdecls, tr->proto[i]);
	}
}

static void delayedcheck(Inferstate *st, Node *n, Stab *s)
{
	lappend(&st->postcheck, &st->npostcheck, n);
	lappend(&st->postcheckscope, &st->npostcheckscope, s);
}

static void typeerror(Inferstate *st, Type *a, Type *b, Node *ctx, char *msg)
{
	char *t1, *t2, *s1, *s2, *c;

	t1 = tystr(tyfix(st, NULL, a, 1));
	t2 = tystr(tyfix(st, NULL, b, 1));
	s1 = srcstr(st, a);
	s2 = srcstr(st, b);
	c = ctxstr(st, ctx);
	if (msg)
		fatal(ctx, "type \"%s\" incompatible with \"%s\" near %s: %s%s%s", t1, t2, c, msg, s1, s2);
	else
		fatal(ctx, "type \"%s\" incompatible with \"%s\" near %s%s%s", t1, t2, c, s1, s2);
	free(t1);
	free(t2);
	free(s1);
	free(s2);
	free(c);
}

/* Set a scope's enclosing scope up correctly.
 * We don't do this in the parser for some reason. */
static void setsuper(Stab *st, Stab *super)
{
	Stab *s;

	/* verify that we don't accidentally create loops */
	for (s = super; s; s = s->super)
		assert(s->super != st);
	st->super = super;
}

/* If the current environment binds a type,
 * we return true */
static int isbound(Inferstate *st, Type *t)
{
	ssize_t i;

	for (i = st->ntybindings - 1; i >= 0; i--) {
		if (htget(st->tybindings[i], t->pname))
			return 1;
	}
	return 0;
}

/* Checks if a type that directly contains itself.
 * Recursive types that contain themselves through
 * pointers or slices are fine, but any other self-inclusion
 * would lead to a value of infinite size */
static int occurs_rec(Inferstate *st, Type *sub, Bitset *bs)
{
	size_t i;

	if (bshas(bs, sub->tid))
		return 1;
	bsput(bs, sub->tid);
	switch (sub->type) {
	case Typtr:
	case Tyslice: 
		break;
	case Tystruct:
		for (i = 0; i < sub->nmemb; i++)
			if (occurs_rec(st, decltype(sub->sdecls[i]), bs))
				return 1;
		break;
	case Tyunion:
		for (i = 0; i < sub->nmemb; i++) {
			if (!sub->udecls[i]->etype)
				continue;
			if (occurs_rec(st, sub->udecls[i]->etype, bs))
				return 1;
		}
		break;
	default:
		for (i = 0; i < sub->nsub; i++)
			if (occurs_rec(st, sub->sub[i], bs))
				return 1;
		break;
	}
	bsdel(bs, sub->tid);
	return 0;
}

static int occursin(Inferstate *st, Type *a, Type *b)
{
	Bitset *bs;
	int r;

	bs = mkbs();
	bsput(bs, b->tid);
	r = occurs_rec(st, a, bs);
	bsfree(bs);
	return r;
}

static int occurs(Inferstate *st, Type *t)
{
	Bitset *bs;
	int r;

	bs = mkbs();
	r = occurs_rec(st, t, bs);
	bsfree(bs);
	return r;
}

static int needfreshenrec(Inferstate *st, Type *t, Bitset *visited)
{
	size_t i;

	if (bshas(visited, t->tid))
		return 0;
	bsput(visited, t->tid);
	switch (t->type) {
	case Typaram: return 1;
	case Tygeneric: return 1;
	case Tyname:
		for (i = 0; i < t->narg; i++)
			if (needfreshenrec(st, t->arg[i], visited))
				return 1;
		return needfreshenrec(st, t->sub[0], visited);
	case Tystruct:
		for (i = 0; i < t->nmemb; i++)
			if (needfreshenrec(st, decltype(t->sdecls[i]), visited))
				return 1;
		break;
	case Tyunion:
		for (i = 0; i < t->nmemb; i++)
			if (t->udecls[i]->etype && needfreshenrec(st, t->udecls[i]->etype, visited))
				return 1;
		break;
	default:
		for (i = 0; i < t->nsub; i++)
			if (needfreshenrec(st, t->sub[i], visited))
				return 1;
		break;
	}
	return 0;
}

static int needfreshen(Inferstate *st, Type *t)
{
	Bitset *visited;
	int ret;

	visited = mkbs();
	ret = needfreshenrec(st, t, visited);
	bsfree(visited);
	return ret;
}

/* Freshens the type of a declaration. */
static Type *tyfreshen(Inferstate *st, Tysubst *subst, Type *t)
{
	char *from, *to;

	if (!needfreshen(st, t)) {
		if (debugopt['u'])
			indentf(st->indentdepth, "%s isn't generic: skipping freshen\n", tystr(t));
		return t;
	}

	from = tystr(t);
	tybind(st, t);
	if (!subst) {
		subst = mksubst();
		t = tyspecialize(t, subst, st->delayed, st->seqbase);
		substfree(subst);
	} else {
		t = tyspecialize(t, subst, st->delayed, st->seqbase);
	}
	tyunbind(st, t);
	if (debugopt['u']) {
		to = tystr(t);
		indentf(st->indentdepth, "Freshen %s => %s\n", from, to);
		free(to);
	}
	free(from);

	return t;
}

/* Resolves a type and all its subtypes recursively. */
static void tyresolve(Inferstate *st, Type *t)
{
	size_t i;
	Type *base;

	if (t->resolved)
		return;
	/* type resolution should never throw errors about non-generics
	 * showing up within a generic type, so we push and pop a generic
	 * around resolution */
	st->ingeneric++;
	t->resolved = 1;
	/* Walk through aggregate type members */
	if (t->type == Tystruct) {
		st->inaggr++;
		for (i = 0; i < t->nmemb; i++)
			infernode(st, &t->sdecls[i], NULL, NULL);
		st->inaggr--;
	} else if (t->type == Tyunion) {
		st->inaggr++;
		for (i = 0; i < t->nmemb; i++) {
			t->udecls[i]->utype = t;
			t->udecls[i]->utype = tf(st, t->udecls[i]->utype);
			if (t->udecls[i]->etype) {
				tyresolve(st, t->udecls[i]->etype);
				t->udecls[i]->etype = tf(st, t->udecls[i]->etype);
			}
		}
		st->inaggr--;
	} else if (t->type == Tyarray) {
		if (!st->inaggr && !t->asize)
			lfatal(t->loc, "unsized array type outside of struct");
		infernode(st, &t->asize, NULL, NULL);
	} else if (t->type == Typaram && st->innamed) {
		if (!isbound(st, t))
			lfatal(
					t->loc, "type parameter %s is undefined in generic context", tystr(t));
	}

	if (t->type == Tyname || t->type == Tygeneric) {
		tybind(st, t);
		st->innamed++;
	}
	for (i = 0; i < t->nsub; i++)
		t->sub[i] = tf(st, t->sub[i]);
	base = tybase(t);
	/* no-ops if base == t */
	if (t->traits && base->traits)
		bsunion(t->traits, base->traits);
	else if (base->traits)
		t->traits = bsdup(base->traits);
	if (occurs(st, t))
		lfatal(t->loc, "type %s includes itself", tystr(t));
	st->ingeneric--;
	if (t->type == Tyname || t->type == Tygeneric) {
		tyunbind(st, t);
		st->innamed--;
	}
}

Type *tysearch(Type *t)
{
	while (tytab[t->tid])
		t = tytab[t->tid];
	return t;
}

/* Look up the best type to date in the unification table, returning it */
static Type *tylookup(Type *t)
{
	Type *lu;
	Stab *ns;

	assert(t != NULL);
	lu = NULL;
	while (1) {
		if (!tytab[t->tid] && t->type == Tyunres) {
			ns = curstab();
			if (t->name->name.ns) {
				ns = getns(file, t->name->name.ns);
			}
			if (!ns)
				fatal(t->name, "could not resolve namespace \"%s\"",
						t->name->name.ns);
			if (!(lu = gettype(ns, t->name)))
				fatal(t->name, "could not resolve type %s", tystr(t));
			tytab[t->tid] = lu;
		}

		if (!tytab[t->tid])
			break;
		/* compress paths: shift the link up one level */
		if (tytab[tytab[t->tid]->tid])
			tytab[t->tid] = tytab[tytab[t->tid]->tid];
		t = tytab[t->tid];
	}
	return t;
}

static Type *tysubstmap(Inferstate *st, Tysubst *subst, Type *t, Type *orig)
{
	size_t i;

	for (i = 0; i < t->ngparam; i++) {
		substput(subst, t->gparam[i], tf(st, orig->arg[i]));
	}
	t = tyfreshen(st, subst, t);
	return t;
}

static Type *tysubst(Inferstate *st, Type *t, Type *orig)
{
	Tysubst *subst;

	subst = mksubst();
	t = tysubstmap(st, subst, t, orig);
	substfree(subst);
	return t;
}


/* find the most accurate type mapping we have (ie,
 * the end of the unification chain */
static Type *tf(Inferstate *st, Type *orig)
{
	int isgeneric;
	Type *t;

	assert(orig != NULL);
	t = tylookup(orig);
	isgeneric = t->type == Tygeneric;
	st->ingeneric += isgeneric;
	tyresolve(st, t);
	/* If this is an instantiation of a generic type, we want the params to
	 * match the instantiation */
	if (orig->type == Tyunres && t->type == Tygeneric) {
		if (t->ngparam != orig->narg) {
			lfatal(orig->loc, "%s incompatibly specialized with %s, declared on %s:%d",
					tystr(orig), tystr(t), file->file.files[t->loc.file], t->loc.line);
		}
		t = tysubst(st, t, orig);
	}
	st->ingeneric -= isgeneric;
	return t;
}

/* set the type of any typable node */
static void settype(Inferstate *st, Node *n, Type *t)
{
	t = tf(st, t);
	switch (n->type) {
	case Nexpr:	n->expr.type = t;	break;
	case Ndecl:	n->decl.type = t;	break;
	case Nlit:	n->lit.type = t;	break;
	case Nfunc:	n->func.type = t;	break;
	default: die("untypable node %s", nodestr[n->type]); break;
	}
	if (t->type != Tyvar)
		marksrc(st, t, n->loc);
}

/* Gets the type of a literal value */
static Type *littype(Node *n)
{
	Type *t;

	t = NULL;
	if (!n->lit.type) {
		switch (n->lit.littype) {
		case Lvoid:	t = mktype(n->loc, Tyvoid);	break;
		case Lchr:	t = mktype(n->loc, Tychar);	break;
		case Lbool:	t = mktype(n->loc, Tybool);	break;
		case Lint:	t = mktylike(n->loc, Tyint);	break;
		case Lflt:	t = mktylike(n->loc, Tyflt64);	break;
		case Lstr:	t = mktyslice(n->loc, mktype(n->loc, Tybyte));	break;
		case Llbl:	t = mktyptr(n->loc, mktype(n->loc, Tyvoid));	break;
		case Lfunc:	t = n->lit.fnval->func.type;	break;
		}
		n->lit.type = t;
	}
	return n->lit.type;
}

static Type *delayeducon(Inferstate *st, Type *fallback)
{
	Type *t;
	char *from, *to;

	if (fallback->type != Tyunion)
		return fallback;
	t = mktylike(fallback->loc, fallback->type);
	htput(st->delayed, t, fallback);
	if (debugopt['u']) {
		from = tystr(t);
		to = tystr(fallback);
		indentf(st->indentdepth, "Delay %s -> %s\n", from, to);
		free(from);
		free(to);
	}
	return t;
}

/* Finds the type of any typable node */
static Type *type(Inferstate *st, Node *n)
{
	Type *t;

	switch (n->type) {
	case Nlit:	t = littype(n);	break;
	case Nexpr:	t = n->expr.type;	break;
	case Ndecl:	t = decltype(n);	break;
	case Nfunc:	t = n->func.type;	break;
	default:
		t = NULL;
		die("untypeable node %s", nodestr[n->type]);
		break;
	};
	return tf(st, t);
}

static Ucon *uconresolve(Inferstate *st, Node *n)
{
	Ucon *uc;
	Node **args;
	Stab *ns;

	args = n->expr.args;
	ns = curstab();
	if (args[0]->name.ns)
		ns = getns(file, args[0]->name.ns);
	if (!ns)
		fatal(n, "no namespace %s\n", args[0]->name.ns);
	uc = getucon(ns, args[0]);
	if (!uc)
		fatal(n, "no union constructor `%s", ctxstr(st, args[0]));
	if (!uc->etype && n->expr.nargs > 1)
		fatal(n, "nullary union constructor `%s passed arg ", ctxstr(st, args[0]));
	else if (uc->etype && n->expr.nargs != 2)
		fatal(n, "union constructor `%s needs arg ", ctxstr(st, args[0]));
	return uc;
}

static void putbindingsrec(Inferstate *st, Htab *bt, Type *t, Bitset *visited)
{
	size_t i;

	if (bshas(visited, t->tid))
		return;
	bsput(visited, t->tid);
	switch (t->type) {
	case Typaram:
		if (hthas(bt, t->pname))
			unify(st, NULL, htget(bt, t->pname), t);
		else if (!isbound(st, t))
			htput(bt, t->pname, t);
		break;
	case Tygeneric:
		for (i = 0; i < t->ngparam; i++)
			putbindingsrec(st, bt, t->gparam[i], visited);
		break;
	case Tyname:
		for (i = 0; i < t->narg; i++)
			putbindingsrec(st, bt, t->arg[i], visited);
		break;
	case Tyunres:
		for (i = 0; i < t->narg; i++)
			putbindingsrec(st, bt, t->arg[i], visited);
		break;
	case Tystruct:
		for (i = 0; i < t->nmemb; i++)
			putbindingsrec(st, bt, t->sdecls[i]->decl.type, visited);
		break;
	case Tyunion:
		for (i = 0; i < t->nmemb; i++)
			if (t->udecls[i]->etype)
				putbindingsrec(st, bt, t->udecls[i]->etype, visited);
		break;
	default:
		for (i = 0; i < t->nsub; i++)
			putbindingsrec(st, bt, t->sub[i], visited);
		break;
	}
}

/* Binds the type parameters present in the
 * current type into the type environment */
static void putbindings(Inferstate *st, Htab *bt, Type *t)
{
	Bitset *visited;

	if (!t)
		return;
	visited = mkbs();
	putbindingsrec(st, bt, t, visited);
	bsfree(visited);
}

static void tybind(Inferstate *st, Type *t)
{
	Htab *bt;
	char *s;

	if (debugopt['u']) {
		s = tystr(t);
		indentf(st->indentdepth, "Binding %s\n", s);
		free(s);
	}
	bt = mkht(strhash, streq);
	lappend(&st->tybindings, &st->ntybindings, bt);
	putbindings(st, bt, t);
}

/* Binds the type parameters in the
 * declaration into the type environment */
static void bind(Inferstate *st, Node *n)
{
	Htab *bt;

	assert(n->type == Ndecl);
	if (!n->decl.isgeneric)
		return;
	if (!n->decl.init)
		fatal(n, "generic %s has no initializer", n->decl);

	st->ingeneric++;
	bt = mkht(strhash, streq);
	lappend(&st->tybindings, &st->ntybindings, bt);

	putbindings(st, bt, n->decl.type);
	putbindings(st, bt, n->decl.init->expr.type);
}

/* Rolls back the binding of type parameters in
 * the type environment */
static void unbind(Inferstate *st, Node *n)
{
	if (!n->decl.isgeneric)
		return;
	htfree(st->tybindings[st->ntybindings - 1]);
	lpop(&st->tybindings, &st->ntybindings);
	st->ingeneric--;
}

static void tyunbind(Inferstate *st, Type *t)
{
	if (t->type != Tygeneric)
		return;
	htfree(st->tybindings[st->ntybindings - 1]);
	lpop(&st->tybindings, &st->ntybindings);
}

/* Constrains a type to implement the required constraints. On
 * type variables, the constraint is added to the required
 * constraint list. Otherwise, the type is checked to see
 * if it has the required constraint */
static void constrain(Inferstate *st, Node *ctx, Type *a, Trait *c)
{
	if (a->type == Tyvar) {
		if (!a->traits)
			a->traits = mkbs();
		settrait(a, c);
	} else if (!a->traits || !bshas(a->traits, c->uid)) {
		fatal(ctx, "%s needs %s near %s", tystr(a), namestr(c->name), ctxstr(st, ctx));
	}
}

/* does b satisfy all the constraints of a? */
static int checktraits(Type *a, Type *b)
{
	/* a has no traits to satisfy */
	if (!a->traits)
		return 1;
	/* b satisfies no traits; only valid if a requires none */
	if (!b->traits)
		return bscount(a->traits) == 0;
	/* if a->traits is a subset of b->traits, all of
	 * a's constraints are satisfied by b. */
	return bsissubset(a->traits, b->traits);
}

static void verifytraits(Inferstate *st, Node *ctx, Type *a, Type *b)
{
	size_t i, n;
	Srcloc l;
	char *sep;
	char traitbuf[64], abuf[64], bbuf[64];
	char asrc[64], bsrc[64];

	if (!checktraits(a, b)) {
		sep = "";
		n = 0;
		for (i = 0; bsiter(a->traits, &i); i++) {
			if (!b->traits || !bshas(b->traits, i))
				n += bprintf(traitbuf + n, sizeof(traitbuf) - n, "%s%s", sep,
						namestr(traittab[i]->name));
			sep = ",";
		}
		tyfmt(abuf, sizeof abuf, a);
		tyfmt(bbuf, sizeof bbuf, b);
		bsrc[0] = 0;
		if (st->nusrc > b->tid && st->usrc[b->tid].line > 0) {
			l = st->usrc[b->tid];
			snprintf(bsrc, sizeof asrc, "\n\t%s from %s:%d", bbuf, fname(l), lnum(l));
		}
		fatal(ctx, "%s missing traits %s for %s near %s%s%s",
				bbuf, traitbuf, abuf, ctxstr(st, ctx),
				srcstr(st, a), srcstr(st, b));
	}
}

/* Merges the constraints on types */
static void mergetraits(Inferstate *st, Node *ctx, Type *a, Type *b)
{
	if (b->type == Tyvar) {
		/* make sure that if a = b, both have same traits */
		if (a->traits && b->traits)
			bsunion(b->traits, a->traits);
		else if (a->traits)
			b->traits = bsdup(a->traits);
		else if (b->traits)
			a->traits = bsdup(b->traits);
	} else {
		verifytraits(st, ctx, a, b);
	}
}

/* Computes the 'rank' of the type; ie, in which
 * direction should we unify. A lower ranked type
 * should be mapped to the higher ranked (ie, more
 * specific) type. */
static int tyrank(Inferstate *st, Type *t)
{
	/* plain tyvar */
	if (t->type == Tyvar) {
		if (hthas(st->seqbase, t))
			return 1;
		else
			return 0;
	}
	/* concrete type */
	return 2;
}

static void unionunify(Inferstate *st, Node *ctx, Type *u, Type *v)
{
	size_t i, j;
	int found;

	if (u->nmemb != v->nmemb)
		fatal(ctx, "can't unify %s and %s near %s%s%s\n",
			tystr(u), tystr(v), ctxstr(st, ctx),
			srcstr(st, u), srcstr(st, v));

	for (i = 0; i < u->nmemb; i++) {
		found = 0;
		for (j = 0; j < v->nmemb; j++) {
			if (!nameeq(u->udecls[i]->name, v->udecls[j]->name))
				continue;
			found = 1;
			if (u->udecls[i]->etype == NULL && v->udecls[j]->etype == NULL)
				continue;
			else if (u->udecls[i]->etype && v->udecls[j]->etype)
				unify(st, ctx, u->udecls[i]->etype, v->udecls[j]->etype);
			else
				fatal(ctx, "can't unify %s and %s near %s%s%s",
					tystr(u), tystr(v), ctxstr(st, ctx),
					srcstr(st, u), srcstr(st, v));
		}
		if (!found)
			fatal(ctx, "can't unify %s and %s near %s%s%s",
				tystr(u), tystr(v), ctxstr(st, ctx),
				srcstr(st, u), srcstr(st, v));
	}
}

static void structunify(Inferstate *st, Node *ctx, Type *u, Type *v)
{
	size_t i, j;
	int found;
	char *ud, *vd;

	if (u->nmemb != v->nmemb)
		fatal(ctx, "can't unify %s and %s near %s%s%s",
			tystr(u), tystr(v), ctxstr(st, ctx),
			srcstr(st, u), srcstr(st, v));

	for (i = 0; i < u->nmemb; i++) {
		found = 0;
		for (j = 0; j < v->nmemb; j++) {
			ud = namestr(u->sdecls[i]->decl.name);
			vd = namestr(v->sdecls[j]->decl.name);
			if (strcmp(ud, vd) == 0) {
				found = 1;
				unify(st, ctx, type(st, u->sdecls[i]), type(st, v->sdecls[j]));
			}
		}
		/* we had at least one missing member */
		if (!found)
			fatal(ctx, "can't unify %s and %s near %s%s%s",
				tystr(u), tystr(v), ctxstr(st, ctx),
				srcstr(st, u), srcstr(st, v));
	}
}

static void membunify(Inferstate *st, Node *ctx, Type *u, Type *v)
{
	if (hthas(st->delayed, u))
		u = htget(st->delayed, u);
	u = tybase(u);
	if (hthas(st->delayed, v))
		v = htget(st->delayed, v);
	v = tybase(v);
	if (u->type == Tyunion && v->type == Tyunion && u != v)
		unionunify(st, ctx, u, v);
	else if (u->type == Tystruct && v->type == Tystruct && u != v)
		structunify(st, ctx, u, v);
}

static Type *basetype(Inferstate *st, Type *a)
{
	Type *t;

	t = htget(st->seqbase, a);
        while (!t && a->type == Tyname) {
            a = a->sub[0];
            t = htget(st->seqbase, a);
        }
	if (!t && (a->type == Tyslice || a->type == Tyarray || a->type == Typtr))
		t = a->sub[0];
	if (t)
		t = tf(st, t);
	return t;
}

static void checksize(Inferstate *st, Node *ctx, Type *a, Type *b)
{
	if (a->asize)
		a->asize = fold(a->asize, 1);
	if (b->asize)
		b->asize = fold(b->asize, 1);
	if (a->asize && exprop(a->asize) != Olit)
		lfatal(ctx->loc, "%s: array size is not constant near %s",
				tystr(a), ctxstr(st, ctx));
	if (b->asize && exprop(b->asize) != Olit)
		lfatal(ctx->loc, "%s: array size is not constant near %s",
				tystr(b), ctxstr(st, ctx));
	if (!a->asize)
		a->asize = b->asize;
	else if (!b->asize)
		b->asize = a->asize;
	else if (a->asize && b->asize)
		if (!litvaleq(a->asize->expr.args[0], b->asize->expr.args[0]))
			lfatal(ctx->loc, "array size of %s does not match %s near %s",
				tystr(a), tystr(b), ctxstr(st, ctx));
}

static int hasargs(Type *t)
{
	return t->type == Tyname && t->narg > 0;
}

/* Unifies two types, or errors if the types are not unifiable. */
static Type *unify(Inferstate *st, Node *ctx, Type *u, Type *v)
{
	Type *t, *r;
	Type *a, *b;
	Type *ea, *eb;
	char *from, *to;
	size_t i;

	/* a ==> b */
	a = tf(st, u);
	b = tf(st, v);
	if (a->tid == b->tid)
		return a;

	/* we unify from lower to higher ranked types */
	if (tyrank(st, b) < tyrank(st, a)) {
		t = a;
		a = b;
		b = t;
	}

	if (debugopt['u']) {
		from = tystr(a);
		to = tystr(b);
		indentf(st->indentdepth, "Unify %s => %s\n", from, to);
		indentf(st->indentdepth + 1, "indexes: %s => %s\n",
			tystr(htget(st->seqbase, a)), tystr(htget(st->seqbase, b)));
		free(from);
		free(to);
	}

	/* Disallow recursive types */
	if (a->type == Tyvar && b->type != Tyvar) {
		if (occursin(st, a, b))
			fatal(ctx, "%s occurs within %s, leading to infinite type near %s\n",
				tystr(a), tystr(b), ctxstr(st, ctx));
	}

	r = NULL;
	if (a->type == Tyvar || tyeq(a, b)) {
		tytab[a->tid] = b;
		if (ctx) {
			marksrc(st, a, ctx->loc);
			marksrc(st, b, ctx->loc);
		}
	}
	if (a->type == Tyvar) {
		ea = basetype(st, a);
		eb = basetype(st, b);
		if (ea && eb)
			unify(st, ctx, ea, eb);
		r = b;
	}

	if (a->type == Tyarray && b->type == Tyarray) {
		checksize(st, ctx, a, b);
	}

	/* if the tyrank of a is 0 (ie, a raw tyvar), just unify.
	 * Otherwise, match up subtypes. */
	if (a->type == b->type && a->type != Tyvar) {
		if (a->type == Tyname)
			if (!nameeq(a->name, b->name))
				typeerror(st, a, b, ctx, "incompatible types");
		if (hasargs(a) && hasargs(b)) {
			/* Only Tygeneric and Tyname should be able to unify. And they
			 * should have the same names for this to be true. */
			if (!nameeq(a->name, b->name))
				typeerror(st, a, b, ctx, NULL);
			if (a->narg != b->narg)
				typeerror(st, a, b, ctx, "incompatible parameter lists");
			for (i = 0; i < a->narg; i++)
				unify(st, ctx, a->arg[i], b->arg[i]);
			r = b;
		}
		if (a->nsub != b->nsub) {
			verifytraits(st, ctx, a, b);
			if (tybase(a)->type == Tyfunc)
				typeerror(st, a, b, ctx, "function arity mismatch");
			else
				typeerror(st, a, b, ctx, "subtype counts incompatible");
		}
		for (i = 0; i < b->nsub; i++)
			unify(st, ctx, a->sub[i], b->sub[i]);
		r = b;
	} else if (a->type != Tyvar) {
		typeerror(st, a, b, ctx, NULL);
	}
	mergetraits(st, ctx, a, b);
	if (a->isreflect || b->isreflect) {
		tagreflect(r);
		tagreflect(a);
		tagreflect(b);
	}
	membunify(st, ctx, a, b);

	/* if we have delayed types for a tyvar, transfer it over. */
	if (a->type == Tyvar && b->type == Tyvar) {
		if (hthas(st->delayed, a) && !hthas(st->delayed, b))
			htput(st->delayed, b, htget(st->delayed, a));
		else if (hthas(st->delayed, b) && !hthas(st->delayed, a))
			htput(st->delayed, a, htget(st->delayed, b));
	} else if (hthas(st->delayed, a)) {
		unify(st, ctx, htget(st->delayed, a), tybase(b));
	}

	return r;
}

/* Applies unifications to function calls.
 * Funciton application requires a slightly
 * different approach to unification. */
static void unifycall(Inferstate *st, Node *n)
{
	size_t i;
	Type *ft;
	char *ret, *ctx;

	ft = type(st, n->expr.args[0]);

	if (ft->type == Tyvar) {
		/* the first arg is the function itself, so it shouldn't be counted */
		ft = mktyfunc(n->loc, &n->expr.args[1], n->expr.nargs - 1, mktyvar(n->loc));
		unify(st, n, ft, type(st, n->expr.args[0]));
	} else if (tybase(ft)->type != Tyfunc) {
		fatal(n, "calling uncallable type %s", tystr(ft));
	}
	/* first arg: function itself */
	for (i = 1; i < n->expr.nargs; i++) {
		if (i == ft->nsub)
			fatal(n, "%s arity mismatch (expected %zd args, got %zd)",
					ctxstr(st, n->expr.args[0]), ft->nsub - 1, n->expr.nargs - 1);

		if (ft->sub[i]->type == Tyvalist) {
			break;
		}
		unify(st, n->expr.args[0], ft->sub[i], type(st, n->expr.args[i]));
	}
	if (i < ft->nsub && ft->sub[i]->type != Tyvalist)
		fatal(n, "%s arity mismatch (expected %zd args, got %zd)",
				ctxstr(st, n->expr.args[0]), ft->nsub - 1, i - 1);
	if (debugopt['u']) {
		ret = tystr(ft->sub[0]);
		ctx = ctxstr(st, n->expr.args[0]);
		indentf(st->indentdepth, "Call of %s returns %s\n", ctx, ret);
		free(ctx);
		free(ret);
	}

	settype(st, n, ft->sub[0]);
}

static void unifyparams(Inferstate *st, Node *ctx, Type *a, Type *b)
{
	size_t i;

	/* The only types with unifiable params are Tyunres and Tyname.
	 * Tygeneric should always be freshened, and no other types have
	 * parameters attached.
	 *
	 * FIXME: Is it possible to have parameterized typarams? */
	if (a->type != Tyunres && a->type != Tyname)
		return;
	if (b->type != Tyunres && b->type != Tyname)
		return;

	if (a->narg != b->narg)
		fatal(ctx, "mismatched arg list sizes: %s with %s near %s", tystr(a), tystr(b),
				ctxstr(st, ctx));
	for (i = 0; i < a->narg; i++)
		unify(st, ctx, a->arg[i], b->arg[i]);
}

static void loaduses(Node *n)
{
	size_t i;

	/* uses only allowed at top level. Do we want to keep it this way? */
	for (i = 0; i < n->file.nuses; i++)
		readuse(n->file.uses[i], n->file.globls, Visintern);
}

static Type *initvar(Inferstate *st, Node *n, Node *s)
{
	Type *t, *param;
	Tysubst *subst;

	if (s->decl.ishidden)
		fatal(n, "attempting to refer to hidden decl %s", ctxstr(st, n));

	param = n->expr.param;
	if (s->decl.isgeneric) {
		subst = mksubst();
		if (param)
			substput(subst, s->decl.trait->param, param);
		t = tysubstmap(st, subst, tf(st, s->decl.type), s->decl.type);
		if (s->decl.trait && !param) {
			param = substget(subst, s->decl.trait->param);
			if (!param)
				fatal(n, "ambiguous trait decl %s", ctxstr(st, s));
		}
		substfree(subst);
	} else {
		t = s->decl.type;
	}
	n->expr.did = s->decl.did;
	n->expr.isconst = s->decl.isconst;
	if (param) {
		n->expr.param = param;
		delayedcheck(st, n, curstab());
	}
	if (s->decl.isgeneric && !st->ingeneric) {
		t = tyfreshen(st, NULL, t);
		addspecialization(st, n, curstab());
		if (t->type == Tyvar) {
			settype(st, n, mktyvar(n->loc));
			delayedcheck(st, n, curstab());
		} else {
			settype(st, n, t);
		}
	} else {
		settype(st, n, t);
	}
	return t;
}

/* Finds out if the member reference is actually
 * referring to a namespaced name, instead of a struct
 * member. If it is, it transforms it into the variable
 * reference we should have, instead of the Omemb expr
 * that we do have */
static Node *checkns(Inferstate *st, Node *n, Node **ret)
{
	Node *var, *name, *nsname;
	Node **args;
	Stab *stab;
	Node *s;

	/* check that this is a namespaced declaration */
	if (n->type != Nexpr)
		return n;
	if (exprop(n) != Omemb)
		return n;
	if (!n->expr.nargs)
		return n;
	args = n->expr.args;
	if (args[0]->type != Nexpr || exprop(args[0]) != Ovar)
		return n;
	name = args[0]->expr.args[0];
	stab = getns(file, namestr(name));
	if (!stab)
		return n;

	/* substitute the namespaced name */
	nsname = mknsname(n->loc, namestr(name), namestr(args[1]));
	s = getdcl(stab, args[1]);
	if (!s)
		fatal(n, "undeclared var %s.%s", nsname->name.ns, nsname->name.name);
	var = mkexpr(n->loc, Ovar, nsname, NULL);
	var->expr.idx = n->expr.idx;
	initvar(st, var, s);
	*ret = var;
	return var;
}

static void inferstruct(Inferstate *st, Node *n, int *isconst)
{
	size_t i;

	*isconst = 1;
	/* we want to check outer nodes before inner nodes when unifying nested structs */
	delayedcheck(st, n, curstab());
	for (i = 0; i < n->expr.nargs; i++) {
		infernode(st, &n->expr.args[i], NULL, NULL);
		if (!n->expr.args[i]->expr.isconst)
			*isconst = 0;
	}
	settype(st, n, mktyvar(n->loc));
}

static int64_t arraysize(Inferstate *st, Node *n)
{
	int64_t sz, off, i;
	Node **args, *idx;

	sz = 0;
	args = n->expr.args;
	for (i = 0; i < n->expr.nargs; i++) {
		if (args[i]->expr.idx) {
			args[i]->expr.idx = fold(args[i]->expr.idx, 1);
			idx = args[i]->expr.idx;
			if (exprop(idx) != Olit)
				fatal(idx, "nonconstant array initializer index near %s\n",
					ctxstr(st, idx));
			if (idx->expr.args[0]->lit.littype == Lchr)
				off = idx->expr.args[0]->lit.chrval;
			else if (idx->expr.args[0]->lit.littype == Lint)
				off = idx->expr.args[0]->lit.intval;
			else
				fatal(idx, "noninteger array initializer index near %s\n",
					ctxstr(st, idx));
			if (off >= sz)
				sz = off + 1;
		} else {
			sz++;
		}
	}
	return sz;
}

static void inferarray(Inferstate *st, Node *n, int *isconst)
{
	size_t i;
	Type *t;
	Node *len;

	*isconst = 1;
	len = mkintlit(n->loc, arraysize(st, n));
	t = mktyarray(n->loc, mktyvar(n->loc), len);
	for (i = 0; i < n->expr.nargs; i++) {
		infernode(st, &n->expr.args[i], NULL, NULL);
		unify(st, n, t->sub[0], type(st, n->expr.args[i]));
		if (!n->expr.args[i]->expr.isconst)
			*isconst = 0;
	}
	settype(st, n, t);
}

static void infertuple(Inferstate *st, Node *n, int *isconst)
{
	Type **types;
	size_t i;

	*isconst = 1;
	types = xalloc(sizeof(Type *) * n->expr.nargs);
	for (i = 0; i < n->expr.nargs; i++) {
		infernode(st, &n->expr.args[i], NULL, NULL);
		n->expr.isconst = n->expr.isconst && n->expr.args[i]->expr.isconst;
		types[i] = type(st, n->expr.args[i]);
	}
	*isconst = n->expr.isconst;
	settype(st, n, mktytuple(n->loc, types, n->expr.nargs));
}

static void inferucon(Inferstate *st, Node *n, int *isconst)
{
	Ucon *uc;
	Type *t;

	*isconst = 1;
	uc = uconresolve(st, n);
	t = tysubst(st, tf(st, uc->utype), uc->utype);
	uc = tybase(t)->udecls[uc->id];
	if (uc->etype) {
		inferexpr(st, &n->expr.args[1], NULL, NULL);
		unify(st, n, uc->etype, type(st, n->expr.args[1]));
		*isconst = n->expr.args[1]->expr.isconst;
	}
	settype(st, n, delayeducon(st, t));
}

static void inferpat(Inferstate *st, Node **np, Node *val, Node ***bind, size_t *nbind)
{
	size_t i;
	Node **args;
	Node *s, *n;
	Stab *ns;
	Type *t;

	n = *np;
	n = checkns(st, n, np);
	args = n->expr.args;
	for (i = 0; i < n->expr.nargs; i++)
		if (args[i]->type == Nexpr)
			inferpat(st, &args[i], val, bind, nbind);
	switch (exprop(n)) {
	case Otup:
	case Ostruct:
	case Oarr:
	case Olit:
	case Omemb:
		infernode(st, np, NULL, NULL);
		break;
		/* arithmetic expressions just need to be constant */
	case Oneg:
	case Oadd:
	case Osub:
	case Omul:
	case Odiv:
	case Obsl:
	case Obsr:
	case Oband:
	case Obor:
	case Obxor:
	case Obnot:
		infernode(st, np, NULL, NULL);
		if (!n->expr.isconst)
			fatal(n, "matching against non-constant expression near %s", ctxstr(st, n));
		break;
	case Oucon:
		inferucon(st, n, &n->expr.isconst);
		break;
	case Ovar:
		ns = curstab();
		if (args[0]->name.ns)
			ns = getns(file, args[0]->name.ns);
		s = getdcl(ns, args[0]);
		if (s && !s->decl.ishidden) {
			if (s->decl.isgeneric)
				t = tysubst(st, s->decl.type, s->decl.type);
			else if (s->decl.isconst)
				t = s->decl.type;
			else
				fatal(n, "pattern shadows variable declared on %s:%d near %s",
						fname(s->loc), lnum(s->loc), ctxstr(st, s));
		} else {
			t = mktyvar(n->loc);
			s = mkdecl(n->loc, n->expr.args[0], t);
			s->decl.init = val;
			settype(st, n, t);
			lappend(bind, nbind, s);
		}
		settype(st, n, t);
		n->expr.did = s->decl.did;
		n->expr.isconst = s->decl.isconst;
		break;
	case Oaddr:
		infernode(st, np, NULL, NULL);
		break;
	case Ogap:
		infernode(st, np, NULL, NULL);	break;
	default: fatal(n, "invalid pattern"); break;
	}
}

void addbindings(Inferstate *st, Node *n, Node **bind, size_t nbind)
{
	size_t i;

	/* order of binding shouldn't matter, so push them into the block
	 * in reverse order. */
	for (i = 0; i < nbind; i++) {
		putdcl(n->block.scope, bind[i]);
		linsert(&n->block.stmts, &n->block.nstmts, 0, bind[i]);
	}
}

static void infersub(Inferstate *st, Node *n, Type *ret, int *sawret, int *exprconst)
{
	Node **args;
	size_t i, nargs;
	int isconst;

	/* Ovar has no subexpressions */
	if (exprop(n) == Ovar)
		return;
	args = n->expr.args;
	nargs = n->expr.nargs;
	isconst = 1;
	for (i = 0; i < nargs; i++) {
		/* Nlit, Nvar, etc should not be inferred as exprs */
		if (args[i]->type == Nexpr) {
			/* Omemb can sometimes resolve to a namespace. We have to check
			 * this. Icky. */
			checkns(st, args[i], &args[i]);
			inferexpr(st, &args[i], ret, sawret);
			isconst = isconst && args[i]->expr.isconst;
		}
	}
	if (opispure[exprop(n)])
		n->expr.isconst = isconst;
	*exprconst = n->expr.isconst;
}

static void inferexpr(Inferstate *st, Node **np, Type *ret, int *sawret)
{
	Node **args;
	size_t i, nargs;
	Node *s, *n;
	Type *t, *b;
	int isconst;
	Stab *ns;

	n = *np;
	assert(n->type == Nexpr);
	args = n->expr.args;
	nargs = n->expr.nargs;
	infernode(st, &n->expr.idx, NULL, NULL);
	n = checkns(st, n, np);
	switch (exprop(n)) {
		/* all operands are same type */
	case Oadd: /* @a + @a -> @a */
	case Osub: /* @a - @a -> @a */
	case Omul: /* @a * @a -> @a */
	case Odiv: /* @a / @a -> @a */
	case Oneg: /* -@a -> @a */
	case Oaddeq:	/* @a += @a -> @a */
	case Osubeq:	/* @a -= @a -> @a */
	case Omuleq:	/* @a *= @a -> @a */
	case Odiveq:	/* @a /= @a -> @a */
		infersub(st, n, ret, sawret, &isconst);
		t = type(st, args[0]);
		constrain(st, n, type(st, args[0]), traittab[Tcnum]);
		isconst = args[0]->expr.isconst;
		for (i = 1; i < nargs; i++) {
			isconst = isconst && args[i]->expr.isconst;
			t = unify(st, n, t, type(st, args[i]));
		}
		n->expr.isconst = isconst;
		settype(st, n, t);
		break;
	case Omod:	/* @a % @a -> @a */
	case Obor:	/* @a | @a -> @a */
	case Oband:	/* @a & @a -> @a */
	case Obxor:	/* @a ^ @a -> @a */
	case Obsl:	/* @a << @a -> @a */
	case Obsr:	/* @a >> @a -> @a */
	case Obnot:	/* ~@a -> @a */
	case Opreinc:	/* ++@a -> @a */
	case Opredec:	/* --@a -> @a */
	case Opostinc:	/* @a++ -> @a */
	case Opostdec:	/* @a-- -> @a */
	case Omodeq:	/* @a %= @a -> @a */
	case Oboreq:	/* @a |= @a -> @a */
	case Obandeq:	/* @a &= @a -> @a */
	case Obxoreq:	/* @a ^= @a -> @a */
	case Obsleq:	/* @a <<= @a -> @a */
	case Obsreq:	/* @a >>= @a -> @a */
		infersub(st, n, ret, sawret, &isconst);
		t = type(st, args[0]);
		constrain(st, n, type(st, args[0]), traittab[Tcnum]);
		constrain(st, n, type(st, args[0]), traittab[Tcint]);
		isconst = args[0]->expr.isconst;
		for (i = 1; i < nargs; i++) {
			isconst = isconst && args[i]->expr.isconst;
			t = unify(st, n, t, type(st, args[i]));
		}
		n->expr.isconst = isconst;
		settype(st, n, t);
		break;
	case Oasn: /* @a = @a -> @a */
		infersub(st, n, ret, sawret, &isconst);
		t = type(st, args[0]);
		for (i = 1; i < nargs; i++)
			t = unify(st, n, t, type(st, args[i]));
		settype(st, n, t);
		if (args[0]->expr.isconst)
			fatal(n, "attempting to assign constant \"%s\"", ctxstr(st, args[0]));
		break;

		/* operands same type, returning bool */
	case Olor:  /* @a || @b -> bool */
	case Oland: /* @a && @b -> bool */
	case Oeq:   /* @a == @a -> bool */
	case One:   /* @a != @a -> bool */
	case Ogt:   /* @a > @a -> bool */
	case Oge:   /* @a >= @a -> bool */
	case Olt:   /* @a < @a -> bool */
	case Ole:   /* @a <= @b -> bool */
		infersub(st, n, ret, sawret, &isconst);
		t = type(st, args[0]);
		for (i = 1; i < nargs; i++)
			unify(st, n, t, type(st, args[i]));
		settype(st, n, mktype(Zloc, Tybool));
		break;

	case Olnot: /* !bool -> bool */
		infersub(st, n, ret, sawret, &isconst);
		t = unify(st, n, type(st, args[0]), mktype(Zloc, Tybool));
		settype(st, n, t);
		break;

		/* reach into a type and pull out subtypes */
	case Oaddr: /* &@a -> @a* */
		infersub(st, n, ret, sawret, &isconst);
		settype(st, n, mktyptr(n->loc, type(st, args[0])));
		break;
	case Oderef: /* *@a* ->  @a */
		infersub(st, n, ret, sawret, &isconst);
		t = unify(st, n, type(st, args[0]), mktyptr(n->loc, mktyvar(n->loc)));
		settype(st, n, t->sub[0]);
		break;
	case Oidx: /* @a[@b::tcint] -> @a */
		infersub(st, n, ret, sawret, &isconst);
		b = mktyvar(n->loc);
		t = mktyvar(n->loc);
		htput(st->seqbase, t, b);
		unify(st, n, type(st, args[0]), t);
		constrain(st, n, type(st, args[0]), traittab[Tcidx]);
		constrain(st, n, type(st, args[1]), traittab[Tcint]);
		settype(st, n, b);
		break;
	case Oslice: /* @a[@b::tcint,@b::tcint] -> @a[,] */
		infersub(st, n, ret, sawret, &isconst);
		b = mktyvar(n->loc);
		t = mktyvar(n->loc);
		htput(st->seqbase, t, b);
		unify(st, n, type(st, args[0]), t);
		constrain(st, n, type(st, args[1]), traittab[Tcint]);
		constrain(st, n, type(st, args[2]), traittab[Tcint]);
		settype(st, n, mktyslice(n->loc, b));
		break;

		/* special cases */
	case Omemb: /* @a.Ident -> @b, verify type(@a.Ident)==@b later */
		infersub(st, n, ret, sawret, &isconst);
		settype(st, n, mktyvar(n->loc));
		delayedcheck(st, n, curstab());
		break;
	case Osize: /* sizeof(@a) -> size */
		infersub(st, n, ret, sawret, &isconst);
		settype(st, n, mktylike(n->loc, Tyuint));
		break;
	case Ocall: /* (@a, @b, @c, ... -> @r)(@a, @b, @c, ...) -> @r */
		infersub(st, n, ret, sawret, &isconst);
		unifycall(st, n);
		break;
	case Ocast: /* (@a : @b) -> @b */
		infersub(st, n, ret, sawret, &isconst);
		delayedcheck(st, n, curstab());
		break;
	case Oret: /* -> @a -> void */
		infersub(st, n, ret, sawret, &isconst);
		if (sawret)
			*sawret = 1;
		if (!ret)
			fatal(n, "returns are not valid near %s", ctxstr(st, n));
		t = unify(st, n, ret, type(st, args[0]));
		settype(st, n, t);
		break;
	case Obreak:
	case Ocontinue:
		/* nullary: nothing to infer. */
		settype(st, n, mktype(Zloc, Tyvoid));
		break;
	case Ojmp: /* goto void* -> void */
		if (args[0]->type == Nlit && args[0]->lit.littype == Llbl)
			args[0] = getlbl(curstab(), args[0]->loc, args[0]->lit.lblname);
		infersub(st, n, ret, sawret, &isconst);
		settype(st, n, mktype(Zloc, Tyvoid));
		break;
	case Ovar: /* a:@a -> @a */
		infersub(st, n, ret, sawret, &isconst);
		/* if we created this from a namespaced var, the type should be
		 * set, and the normal lookup is expected to fail. Since we're
		 * already done with this node, we can just return. */
		if (n->expr.type)
			return;
		ns = curstab();
		if (args[0]->name.ns)
			ns = getns(file, args[0]->name.ns);
		s = getdcl(ns, args[0]);
		if (!s)
			fatal(n, "undeclared var %s", ctxstr(st, args[0]));
		if (n->expr.param && !s->decl.trait)
			fatal(n, "var %s must refer to a trait decl", ctxstr(st, args[0]));
		initvar(st, n, s);
		break;
	case Ogap: /* _ -> @a */
		if (n->expr.type)
			return;
		n->expr.type = mktyvar(n->loc);
		break;
	case Oucon:	inferucon(st, n, &n->expr.isconst);	break;
	case Otup:	infertuple(st, n, &n->expr.isconst);	break;
	case Ostruct:	inferstruct(st, n, &n->expr.isconst);	break;
	case Oarr:	inferarray(st, n, &n->expr.isconst);	break;
	case Olit: /* <lit>:@a::tyclass -> @a */
		   infersub(st, n, ret, sawret, &isconst);
		   switch (args[0]->lit.littype) {
		   case Lfunc:
			   infernode(st, &args[0]->lit.fnval, NULL, NULL);
			   /* FIXME: env capture means this is non-const */
			   n->expr.isconst = 1;
			   break;
		   case Llbl:
			   s = getlbl(curstab(), args[0]->loc, args[0]->lit.lblname);
			   if (!s)
				   fatal(n, "unable to find label %s in function scope\n", args[0]->lit.lblname);
			   *np = s;
			   break;
		   default: 
			   n->expr.isconst = 1;
			   break;
		   }
		   settype(st, n, type(st, args[0]));
		   break;
	case Oundef:
		   infersub(st, n, ret, sawret, &isconst);
		   settype(st, n, mktype(n->loc, Tyvoid));
		   break;
	case Odef:
	case Odead:	n->expr.type = mktype(n->loc, Tyvoid);	break;
	case Obad:
	case Ocjmp:
	case Ovjmp:
	case Oset:
	case Oslbase:
	case Osllen:
	case Outag:
	case Ocallind:
	case Oblit:
	case Oclear:
	case Oudata:
	case Otrunc:
	case Oswiden:
	case Ozwiden:
	case Oint2flt:
	case Oflt2int:
	case Oflt2flt:
	case Ofadd:
	case Ofsub:
	case Ofmul:
	case Ofdiv:
	case Ofneg:
	case Ofeq:
	case Ofne:
	case Ofgt:
	case Ofge:
	case Oflt:
	case Ofle:
	case Oueq:
	case Oune:
	case Ougt:
	case Ouge:
	case Oult:
	case Oule:
	case Otupget:
	case Numops:
		die("Should not see %s in fe", opstr[exprop(n)]);
		break;
	}
}

static void inferfunc(Inferstate *st, Node *n)
{
	size_t i;
	int sawret;

	sawret = 0;
	for (i = 0; i < n->func.nargs; i++)
		infernode(st, &n->func.args[i], NULL, NULL);
	infernode(st, &n->func.body, n->func.type->sub[0], &sawret);
	/* if there's no return stmt in the function, assume void ret */
	if (!sawret)
		unify(st, n, type(st, n)->sub[0], mktype(Zloc, Tyvoid));
}

static void specializeimpl(Inferstate *st, Node *n)
{
	Node *dcl, *proto, *name, *sym;
	Tysubst *subst;
	Type *ty;
	Trait *t;
	size_t i, j;

	t = gettrait(curstab(), n->impl.traitname);
	if (!t)
		fatal(n, "no trait %s\n", namestr(n->impl.traitname));
	n->impl.trait = t;

	dcl = NULL;
	if (n->impl.naux != t->naux)
		fatal(n, "%s incompatibly specialized with %zd types instead of %zd types",
			namestr(n->impl.traitname), n->impl.naux, t->naux);
	n->impl.type = tf(st, n->impl.type);
	for (i = 0; i < n->impl.naux; i++)
		n->impl.aux[i] = tf(st, n->impl.aux[i]);
	for (i = 0; i < n->impl.ndecls; i++) {
		/* look up the prototype */
		proto = NULL;
		dcl = n->impl.decls[i];

		/*
		   since the decls in an impl are not installed in a namespace,
		   their names are not updated when we call updatens() on the
		   symbol table. Because we need to do namespace dependent
		   comparisons for specializing, we need to set the namespace
		   here.
		*/
		if (file->file.globls->name)
			setns(dcl->decl.name, file->file.globls->name);
		for (j = 0; j < t->nproto; j++) {
			if (nsnameeq(dcl->decl.name, t->proto[j]->decl.name)) {
				proto = t->proto[j];
				break;
			}
		}
		if (!proto)
			fatal(n, "declaration %s missing in %s, near %s", namestr(dcl->decl.name),
					namestr(t->name), ctxstr(st, n));

		/* infer and unify types */
		verifytraits(st, n, t->param, n->impl.type);
		subst = mksubst();
		substput(subst, t->param, n->impl.type);
		for (j = 0; j < t->naux; j++)
			substput(subst, t->aux[j], n->impl.aux[j]);
		ty = tyspecialize(type(st, proto), subst, st->delayed, NULL);
		substfree(subst);

		inferdecl(st, dcl);
		unify(st, n, type(st, dcl), ty);

		/* and put the specialization into the global stab */
		name = genericname(proto, ty);
		sym = getdcl(file->file.globls, name);
		if (sym)
			fatal(n, "trait %s already specialized with %s on %s:%d",
				namestr(t->name), tystr(n->impl.type),
				fname(sym->loc), lnum(sym->loc));
		dcl->decl.name = name;
		putdcl(file->file.globls, dcl);
		htput(proto->decl.impls, n->impl.type, dcl);
		dcl->decl.isconst = 1;
		if (ty->type == Tygeneric || hasparams(ty)) {
			dcl->decl.isgeneric = 1;
			lappend(&proto->decl.gimpl, &proto->decl.ngimpl, dcl);
			lappend(&proto->decl.gtype, &proto->decl.ngtype, ty);
		}
		if (debugopt['S'])
			printf("specializing trait [%d]%s:%s => %s:%s\n", n->loc.line,
					namestr(proto->decl.name), tystr(type(st, proto)), namestr(name),
					tystr(ty));
		dcl->decl.vis = t->vis;
		lappend(&st->impldecl, &st->nimpldecl, dcl);
	}
}

static void inferdecl(Inferstate *st, Node *n)
{
	Type *t;

	t = tf(st, decltype(n));
	if (t->type == Tygeneric && !n->decl.isgeneric) {
		t = tyfreshen(st, NULL, t);
		unifyparams(st, n, t, decltype(n));
	}
	settype(st, n, t);
	if (n->decl.init) {
		inferexpr(st, &n->decl.init, NULL, NULL);
		unify(st, n, type(st, n), type(st, n->decl.init));
		if (n->decl.isconst && !n->decl.init->expr.isconst)
			fatal(n, "non-const initializer for \"%s\"", ctxstr(st, n));
	}
}

static void inferstab(Inferstate *st, Stab *s)
{
	void **k;
	size_t n, i;
	Node *dcl;
	Type *t;

	k = htkeys(s->dcl, &n);
	for (i = 0; i < n; i++) {
		dcl = htget(s->dcl, k[i]);
		tf(st, type(st, dcl));
	}
	free(k);

	k = htkeys(s->ty, &n);
	for (i = 0; i < n; i++) {
		t = gettype(s, k[i]);
		if (!t)
			fatal(k[i], "undefined type %s", namestr(k[i]));
		t = tysearch(t);
		tybind(st, t);
		tyresolve(st, t);
		tyunbind(st, t);
		updatetype(s, k[i], t);
	}
	free(k);
}

static void infernode(Inferstate *st, Node **np, Type *ret, int *sawret)
{
	size_t i, nbound;
	Node **bound, *n, *pat;
	Type *t, *b;

	n = *np;
	if (!n)
		return;
	if (n->inferred)
		return;
	n->inferred = 1;
	switch (n->type) {
	case Nfile:
		pushstab(n->file.globls);
		inferstab(st, n->file.globls);
		for (i = 0; i < n->file.nstmts; i++)
			infernode(st, &n->file.stmts[i], NULL, sawret);
		popstab();
		break;
	case Ndecl:
		if (debugopt['u'])
			indentf(st->indentdepth, "--- infer %s ---\n", declname(n));
		st->indentdepth++;
		bind(st, n);
		inferdecl(st, n);
		if (type(st, n)->type == Typaram && !st->ingeneric)
			fatal(n, "generic type %s in non-generic near %s", tystr(type(st, n)),
					ctxstr(st, n));
		unbind(st, n);
		st->indentdepth--;
		if (debugopt['u'])
			indentf(st->indentdepth, "--- done ---\n");
		break;
	case Nblock:
		setsuper(n->block.scope, curstab());
		pushstab(n->block.scope);
		inferstab(st, n->block.scope);
		for (i = 0; i < n->block.nstmts; i++)
			infernode(st, &n->block.stmts[i], ret, sawret);
		popstab();
		break;
	case Nifstmt:
		infernode(st, &n->ifstmt.cond, NULL, sawret);
		infernode(st, &n->ifstmt.iftrue, ret, sawret);
		infernode(st, &n->ifstmt.iffalse, ret, sawret);
		unify(st, n, type(st, n->ifstmt.cond), mktype(n->loc, Tybool));
		break;
	case Nloopstmt:
		setsuper(n->loopstmt.scope, curstab());
		pushstab(n->loopstmt.scope);
		infernode(st, &n->loopstmt.init, ret, sawret);
		infernode(st, &n->loopstmt.cond, NULL, sawret);
		infernode(st, &n->loopstmt.step, ret, sawret);
		infernode(st, &n->loopstmt.body, ret, sawret);
		unify(st, n, type(st, n->loopstmt.cond), mktype(n->loc, Tybool));
		popstab();
		break;
	case Niterstmt:
		bound = NULL;
		nbound = 0;

		inferpat(st, &n->iterstmt.elt, NULL, &bound, &nbound);
		addbindings(st, n->iterstmt.body, bound, nbound);

		infernode(st, &n->iterstmt.seq, NULL, sawret);
		infernode(st, &n->iterstmt.body, ret, sawret);

		b = mktyvar(n->loc);
		t = mktyvar(n->loc);
		htput(st->seqbase, t, b);
		constrain(st, n, type(st, n->iterstmt.seq), traittab[Tciter]);
		unify(st, n, type(st, n->iterstmt.seq), t);
		unify(st, n, type(st, n->iterstmt.elt), b);
		break;
	case Nmatchstmt:
		infernode(st, &n->matchstmt.val, NULL, sawret);
		for (i = 0; i < n->matchstmt.nmatches; i++) {
			infernode(st, &n->matchstmt.matches[i], ret, sawret);
			pat = n->matchstmt.matches[i]->match.pat;
			unify(st, pat, type(st, n->matchstmt.val),
					type(st, n->matchstmt.matches[i]->match.pat));
		}
		break;
	case Nmatch:
		bound = NULL;
		nbound = 0;
		inferpat(st, &n->match.pat, NULL, &bound, &nbound);
		addbindings(st, n->match.block, bound, nbound);
		infernode(st, &n->match.block, ret, sawret);
		break;
	case Nexpr:
		inferexpr(st, np, ret, sawret);
		break;
	case Nfunc:
		setsuper(n->func.scope, curstab());
		if (st->ntybindings > 0)
			for (i = 0; i < n->func.nargs; i++)
				putbindings(st, st->tybindings[st->ntybindings - 1],
						n->func.args[i]->decl.type);
		pushstab(n->func.scope);
		inferstab(st, n->func.scope);
		inferfunc(st, n);
		popstab();
		break;
	case Nimpl:
		specializeimpl(st, n);
		break;
	case Nlit:
	case Nname:
	case Nuse:
		break;
	case Nnone:
		die("Nnone should not be seen as node type!");
		break;
	}
}

/* returns the final type for t, after all unifications
 * and default constraint selections */
static Type *tyfix(Inferstate *st, Node *ctx, Type *orig, int noerr)
{
	static Type *tyint, *tyflt;
	Type *t, *delayed, *base;
	char *from, *to;
	size_t i;
	char buf[1024];

	if (!tyint)
		tyint = mktype(Zloc, Tyint);
	if (!tyflt)
		tyflt = mktype(Zloc, Tyflt64);

	t = tysearch(tf(st, orig));
	base = htget(st->seqbase, orig);
	if (orig->type == Tyvar && hthas(st->delayed, orig)) {
		delayed = htget(st->delayed, orig);
		if (t->type == Tyvar) {
			/* tyvar is guaranteed to unify error-free */
			unify(st, ctx, t, delayed);
			t = tf(st, t);
		} else if (tybase(t)->type != delayed->type && !noerr) {
			fatal(ctx, "type %s not compatible with %s near %s\n", tystr(t),
					tystr(delayed), ctxstr(st, ctx));
		}
	}
	if (t->type == Tyvar) {
		if (hastrait(t, traittab[Tcint]) && checktraits(t, tyint))
			t = tyint;
		if (hastrait(t, traittab[Tcfloat]) && checktraits(t, tyflt))
			t = tyflt;
	} else if (!t->fixed) {
		t->fixed = 1;
		if (t->type == Tyarray) {
			typesub(st, t->asize, noerr);
		} else if (t->type == Tystruct) {
			st->inaggr++;
			for (i = 0; i < t->nmemb; i++)
				typesub(st, t->sdecls[i], noerr);
			st->inaggr--;
		} else if (t->type == Tyunion) {
			for (i = 0; i < t->nmemb; i++) {
				if (t->udecls[i]->etype) {
					tyresolve(st, t->udecls[i]->etype);
					t->udecls[i]->etype =
						tyfix(st, ctx, t->udecls[i]->etype, noerr);
				}
			}
		} else if (t->type == Tyname) {
			for (i = 0; i < t->narg; i++)
				t->arg[i] = tyfix(st, ctx, t->arg[i], noerr);
		}
		for (i = 0; i < t->nsub; i++)
			t->sub[i] = tyfix(st, ctx, t->sub[i], noerr);
	}

	if (t->type == Tyvar && !noerr) {
		if (debugopt['T'])
			dump(file, stdout);
		fatal(ctx, "underconstrained type %s near %s", tyfmt(buf, 1024, t), ctxstr(st, ctx));
	}

	if (debugopt['u'] && !tyeq(orig, t)) {
		from = tystr(orig);
		to = tystr(t);
		indentf(st->indentdepth, "subst %s => %s\n", from, to);
		free(from);
		free(to);
	}
	if (base)
		htput(st->seqbase, t, base);
	return t;
}

static void checkcast(Inferstate *st, Node *n, Node ***rem, size_t *nrem, Stab ***remscope, size_t *nremscope)
{
	/* FIXME: actually verify the casts. Right now, it's ok to leave this
	 * unimplemented because bad casts get caught by the backend. */
}

static void infercompn(Inferstate *st, Node *n, Node ***rem, size_t *nrem, Stab ***remscope, size_t *nremscope)
{
	Node *aggr;
	Node *memb;
	Node **nl;
	Type *t;
	size_t i;
	int found;

	aggr = n->expr.args[0];
	memb = n->expr.args[1];

	found = 0;
	t = tybase(tf(st, type(st, aggr)));
	/* all array-like types have a fake "len" member that we emulate */
	if (t->type == Tyslice || t->type == Tyarray) {
		if (!strcmp(namestr(memb), "len")) {
			constrain(st, n, type(st, n), traittab[Tcnum]);
			constrain(st, n, type(st, n), traittab[Tcint]);
			found = 1;
		}
	/* 
 	 * otherwise, we search aggregate types for the member, and unify
	 * the expression with the member type; ie:
	 *
	 *	 x: aggrtype	y : memb in aggrtype
	 *	 ---------------------------------------
	 *			   x.y : membtype
	 */
	} else {
		if (tybase(t)->type == Typtr)
			t = tybase(tf(st, t->sub[0]));
		if (tybase(t)->type == Tyvar) {
			if (!rem)
				fatal(n, "underspecified type defined on %s:%d used near %s",
						fname(t->loc), lnum(t->loc), ctxstr(st, n));
			lappend(rem, nrem, n);
			lappend(remscope, nremscope, curstab());
			return;
		} else if (tybase(t)->type != Tystruct) {
			fatal(n, "type %s does not support member operators near %s",
					tystr(t), ctxstr(st, n));
		}
		nl = t->sdecls;
		for (i = 0; i < t->nmemb; i++) {
			if (!strcmp(namestr(memb), declname(nl[i]))) {
				unify(st, n, type(st, n), decltype(nl[i]));
				found = 1;
				break;
			}
		}
	}
	if (!found)
		fatal(aggr, "type %s has no member \"%s\" near %s", tystr(type(st, aggr)),
				ctxstr(st, memb), ctxstr(st, aggr));
}

static void checkstruct(Inferstate *st, Node *n, Node ***rem, size_t *nrem, Stab ***remscope, size_t *nremscope)
{
	Type *t, *et;
	Node *val, *name;
	size_t i, j;

	t = tybase(tf(st, n->lit.type));
	/*
	 * If we haven't inferred the type, and it's inside another struct,
	 * we'll eventually get to it.
	 *
	 * If, on the other hand, it is genuinely underspecified, we'll give
	 * a better error on it later.
	 */
	if (t->type == Tyvar)
		return;
	if (t->type != Tystruct)
		fatal(n, "struct literal is used as non struct type %s", tystr(n->lit.type));
	for (i = 0; i < n->expr.nargs; i++) {
		val = n->expr.args[i];
		name = val->expr.idx;

		et = NULL;
		for (j = 0; j < t->nmemb; j++) {
			if (!strcmp(namestr(t->sdecls[j]->decl.name), namestr(name))) {
				et = type(st, t->sdecls[j]);
				break;
			}
		}

		if (!et) {
			if (rem) {
				lappend(rem, nrem, n);
				lappend(remscope, nremscope, curstab());
				return;
			} else {
				fatal(n, "could not find member %s in struct %s, near %s",
						namestr(name), tystr(t), ctxstr(st, n));
			}
		}

		unify(st, val, et, type(st, val));
	}
}

static void checkvar(Inferstate *st, Node *n, Node ***rem, size_t *nrem, Stab ***remscope, size_t *nremscope)
{
	Node *proto, *dcl;
	Type *ty;

	proto = decls[n->expr.did];
	ty = NULL;
	dcl = NULL;
	if (n->expr.param)
		dcl = htget(proto->decl.impls, tf(st, n->expr.param));
	if (dcl)
		ty = dcl->decl.type;
	if (!ty)
		ty = tyfreshen(st, NULL, type(st, proto));
	unify(st, n, type(st, n), ty);
}

static void postcheckpass(Inferstate *st, Node ***rem, size_t *nrem, Stab ***remscope, size_t *nremscope)
{
	size_t i;
	Node *n;

	for (i = 0; i < st->npostcheck; i++) {
		n = st->postcheck[i];
		pushstab(st->postcheckscope[i]);
		switch (exprop(n)) {
		case Omemb:	infercompn(st, n, rem, nrem, remscope, nremscope);	break;
		case Ocast:	checkcast(st, n, rem, nrem, remscope, nremscope);	break;
		case Ostruct:	checkstruct(st, n, rem, nrem, remscope, nremscope);	break;
		case Ovar:	checkvar(st, n, rem, nrem, remscope, nremscope);	break;
		default:	die("should not see %s in postcheck\n", opstr[exprop(n)]);
		}
		popstab();
	}
}

static void postcheck(Inferstate *st)
{
	size_t nrem, nremscope;
	Stab **remscope;
	Node **rem;

	while (1) {
		remscope = NULL;
		nremscope = 0;
		rem = NULL;
		nrem = 0;
		postcheckpass(st, &rem, &nrem, &remscope, &nremscope);
		if (nrem == st->npostcheck) {
			break;
		}
		st->postcheck = rem;
		st->npostcheck = nrem;
		st->postcheckscope = remscope;
		st->npostcheckscope = nremscope;
	}
	postcheckpass(st, NULL, NULL, NULL, NULL);
}

/* After inference, replace all
 * types in symbol tables with
 * the final computed types */
static void stabsub(Inferstate *st, Stab *s)
{
	void **k;
	size_t n, i;
	Type *t;
	Node *d;

	k = htkeys(s->ty, &n);
	for (i = 0; i < n; i++) {
		t = tysearch(gettype(s, k[i]));
		updatetype(s, k[i], t);
		tyfix(st, k[i], t, 0);
	}
	free(k);

	k = htkeys(s->dcl, &n);
	for (i = 0; i < n; i++) {
		d = getdcl(s, k[i]);
		d->decl.type = tyfix(st, d, d->decl.type, 0);
		if (!d->decl.isconst && !d->decl.isgeneric)
			continue;
		if (d->decl.trait)
			continue;
		if (!d->decl.isimport && !d->decl.isextern && !d->decl.init)
			fatal(d, "non-extern constant \"%s\" has no initializer", ctxstr(st, d));
	}
	free(k);
}

static void checkrange(Inferstate *st, Node *n)
{
	Type *t;
	int64_t sval;
	uint64_t uval;
	static const int64_t svranges[][2] = {
		/* signed ints */
		[Tyint8] = {-128LL, 127LL}, [Tyint16] = {-32768LL, 32767LL},
		/* FIXME: this has been doubled allow for uints... */
		[Tyint32] = {-2147483648LL, 2 * 2147483647LL},
		[Tyint] = {-2147483648LL, 2 * 2147483647LL},
		[Tyint64] = {-9223372036854775808ULL, 9223372036854775807LL},
	};

	static const uint64_t uvranges[][2] = {
		[Tybyte] = {0, 255ULL}, [Tyuint8] = {0, 255ULL}, [Tyuint16] = {0, 65535ULL},
		[Tyuint32] = {0, 4294967295ULL}, [Tyuint64] = {0, 18446744073709551615ULL},
		[Tychar] = {0, 4294967295ULL},
	};

	/* signed types */
	t = type(st, n);
	if (t->type >= Tyint8 && t->type <= Tyint64) {
		sval = n->lit.intval;
		if (sval < svranges[t->type][0] || sval > svranges[t->type][1])
			fatal(n, "literal value %lld out of range for type \"%s\"", sval, tystr(t));
	} else if ((t->type >= Tybyte && t->type <= Tyint64) || t->type == Tychar) {
		uval = n->lit.intval;
		if (uval < uvranges[t->type][0] || uval > uvranges[t->type][1])
			fatal(n, "literal value %llu out of range for type \"%s\"", uval, tystr(t));
	}
}

static int initcompatible(Type *t)
{
	if (t->type != Tyfunc)
		return 0;
	if (t->nsub != 1)
		return 0;
	if (tybase(t->sub[0])->type != Tyvoid)
		return 0;
	return 1;
}

static int maincompatible(Type *t)
{
	if (t->nsub > 2)
		return 0;
	if (tybase(t->sub[0])->type != Tyvoid)
		return 0;
	if (t->nsub == 2) {
		t = tybase(t->sub[1]);
		if (t->type != Tyslice)
			return 0;
		t = tybase(t->sub[0]);
		if (t->type != Tyslice)
			return 0;
		t = tybase(t->sub[0]);
		if (t->type != Tybyte)
			return 0;
	}
	return 1;
}

static void verifyop(Inferstate *st, Node *n)
{
	Type *ty;

	ty = exprtype(n);
	switch (exprop(n)) {
	case Ostruct:
		if (tybase(ty)->type != Tystruct)
			fatal(n, "wrong type for struct literal: %s\n", tystr(ty));
		break;
	case Oarr:
		if (tybase(ty)->type != Tyarray)
			fatal(n, "wrong type for struct literal: %s\n", tystr(ty));
		break;
	default:
		break;
	}
}

/* After type inference, replace all types
 * with the final computed type */
static void typesub(Inferstate *st, Node *n, int noerr)
{
	size_t i;

	if (!n)
		return;
	switch (n->type) {
	case Nfile:
		pushstab(n->file.globls);
		stabsub(st, n->file.globls);
		for (i = 0; i < n->file.nstmts; i++)
			typesub(st, n->file.stmts[i], noerr);
		popstab();
		break;
	case Ndecl:
		settype(st, n, tyfix(st, n, type(st, n), noerr));
		if (n->decl.init)
			typesub(st, n->decl.init, noerr);
		if (streq(declname(n), "main"))
			if (!maincompatible(tybase(decltype(n))))
				fatal(n, "main must be (->void) or (byte[:][:] -> void), got %s",
						tystr(decltype(n)));
		if (streq(declname(n), "__init__"))
			if (!initcompatible(tybase(decltype(n))))
				fatal(n, "__init__ must be (->void), got %s", tystr(decltype(n)));
		break;
	case Nblock:
		pushstab(n->block.scope);
		for (i = 0; i < n->block.nstmts; i++)
			typesub(st, n->block.stmts[i], noerr);
		popstab();
		break;
	case Nifstmt:
		typesub(st, n->ifstmt.cond, noerr);
		typesub(st, n->ifstmt.iftrue, noerr);
		typesub(st, n->ifstmt.iffalse, noerr);
		break;
	case Nloopstmt:
		typesub(st, n->loopstmt.cond, noerr);
		typesub(st, n->loopstmt.init, noerr);
		typesub(st, n->loopstmt.step, noerr);
		typesub(st, n->loopstmt.body, noerr);
		break;
	case Niterstmt:
		typesub(st, n->iterstmt.elt, noerr);
		typesub(st, n->iterstmt.seq, noerr);
		typesub(st, n->iterstmt.body, noerr);
		additerspecializations(st, n, curstab());
		break;
	case Nmatchstmt:
		typesub(st, n->matchstmt.val, noerr);
		for (i = 0; i < n->matchstmt.nmatches; i++) {
			typesub(st, n->matchstmt.matches[i], noerr);
		}
		break;
	case Nmatch:
		typesub(st, n->match.pat, noerr);
		typesub(st, n->match.block, noerr);
		break;
	case Nexpr:
		settype(st, n, tyfix(st, n, type(st, n), 0));
		typesub(st, n->expr.idx, noerr);
		if (exprop(n) == Ocast && exprop(n->expr.args[0]) == Olit &&
				n->expr.args[0]->expr.args[0]->lit.littype == Lint) {
			settype(st, n->expr.args[0], exprtype(n));
			settype(st, n->expr.args[0]->expr.args[0], exprtype(n));
		}
		for (i = 0; i < n->expr.nargs; i++)
			typesub(st, n->expr.args[i], noerr);
                if (!noerr)
                    verifyop(st, n);
		break;
	case Nfunc:
		pushstab(n->func.scope);
		settype(st, n, tyfix(st, n, n->func.type, 0));
		for (i = 0; i < n->func.nargs; i++)
			typesub(st, n->func.args[i], noerr);
		typesub(st, n->func.body, noerr);
		popstab();
		break;
	case Nlit:
		settype(st, n, tyfix(st, n, type(st, n), 0));
		switch (n->lit.littype) {
		case Lfunc:	typesub(st, n->lit.fnval, noerr);	break;
		case Lint: checkrange(st, n);
		default: break;
		}
		break;
	case Nimpl: putimpl(curstab(), n);
	case Nname:
	case Nuse: break;
	case Nnone:	die("Nnone should not be seen as node type!");	break;
	}
}

static Type *itertype(Inferstate *st, Node *n, Type *ret)
{
	Type *it, *val, *itp, *valp, *fn;

	it = exprtype(n);
	itp = mktyptr(n->loc, it);
	val = basetype(st, it);
	if (!val)
		die("FAIL! %s", tystr(it));
	valp = mktyptr(n->loc, val);
	fn = mktyfunc(n->loc, NULL, 0, ret);
	lappend(&fn->sub, &fn->nsub, itp);
	lappend(&fn->sub, &fn->nsub, valp);
	return fn;
}

/* Take generics and build new versions of them
 * with the type parameters replaced with the
 * specialized types */
static void specialize(Inferstate *st, Node *f)
{
	Node *d, *n, *name;
	Type *ty, *it;
	size_t i;
	Trait *tr;

	for (i = 0; i < st->nimpldecl; i++) {
		d = st->impldecl[i];
		lappend(&file->file.stmts, &file->file.nstmts, d);
		typesub(st, d, 0);
	}

	for (i = 0; i < st->nspecializations; i++) {
		pushstab(st->specializationscope[i]);
		n = st->specializations[i];
		if (n->type == Nexpr) {
			d = specializedcl(st->genericdecls[i], n->expr.type, &name);
			n->expr.args[0] = name;
			n->expr.did = d->decl.did;

			/* we need to sub in default types in the specialization, so call
			 * typesub on the specialized function */
			typesub(st, d, 0);
		} else if (n->type == Niterstmt) {
			tr = traittab[Tciter];
			assert(tr->nproto == 2);
			ty = exprtype(n->iterstmt.seq);

			it = itertype(st, n->iterstmt.seq, mktype(n->loc, Tybool));
			d = specializedcl(tr->proto[0], it, &name);
			htput(tr->proto[0]->decl.impls, ty, d);

			it = itertype(st, n->iterstmt.seq, mktype(n->loc, Tyvoid));
			d = specializedcl(tr->proto[1], it, &name);
			htput(tr->proto[1]->decl.impls, ty, d);
		} else {
			die("unknown node for specialization\n");
		}
		popstab();
	}
}

void applytraits(Inferstate *st, Node *f)
{
	size_t i;
	Node *impl, *n;
	Trait *tr;
	Type *ty;
	Stab *ns;

	tr = NULL;
	pushstab(f->file.globls);
	/* for now, traits can only be declared globally */
	for (i = 0; i < nimpltab; i++) {
		impl = impltab[i];
		tr = impl->impl.trait;
		if (!tr) {
			n = impl->impl.traitname;
			ns = file->file.globls;
			if (n->name.ns)
				ns = getns(file, n->name.ns);
			if (ns)
				tr = gettrait(ns, n);
			if (!tr)
				fatal(impl, "trait %s does not exist near %s",
						namestr(impl->impl.traitname), ctxstr(st, impl));
			if (tr->naux != impl->impl.naux)
				fatal(impl, "incompatible implementation of %s: mismatched aux types",
						namestr(impl->impl.traitname), ctxstr(st, impl));
		}
		ty = tf(st, impl->impl.type);
		settrait(ty, tr);
		if (tr->uid == Tciter) {
			htput(st->seqbase, tf(st, impl->impl.type), tf(st, impl->impl.aux[0]));
		}
	}
	popstab();
}

void verify(Inferstate *st, Node *f)
{
	Node *n;
	size_t i;

	pushstab(f->file.globls);
	/* for now, traits can only be declared globally */
	for (i = 0; i < f->file.nstmts; i++) {
		if (f->file.stmts[i]->type == Nimpl) {
			n = f->file.stmts[i];
			/* we merge, so we need to get it back again when error checking */
			if (n->impl.isproto)
				fatal(n, "missing implementation for prototype '%s %s'",
					namestr(n->impl.traitname), tystr(n->impl.type));
		}
	}
}

void infer(Node *file)
{
	Inferstate st = {
		0,
	};

	assert(file->type == Nfile);
	st.delayed = mkht(tyhash, tyeq);
	st.seqbase = mkht(tyhash, tyeq);
	/* set up the symtabs */
	loaduses(file);
	// mergeexports(&st, file);

	/* do the inference */
	applytraits(&st, file);
	infernode(&st, &file, NULL, NULL);
	postcheck(&st);

	/* and replace type vars with actual types */
	typesub(&st, file, 0);
	specialize(&st, file);
	verify(&st, file);
}
