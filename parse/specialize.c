#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "parse.h"

static Node *specializenode(Node *g, Htab *tsmap);

void addtraits(Type *t, Bitset *traits)
{
    size_t b;

    if (traits)
        for (b = 0; bsiter(traits, &b); b++)
            settrait(t, traittab[b]);
}

/*
 * Duplicates the type 't', with all bound type
 * parameters substituted with the substitions
 * described in 'tsmap'
 *
 * Returns a fresh type with all unbound type
 * parameters (type schemes in most literature)
 * replaced with type variables that we can unify
 * against */
Type *tyspecialize(Type *t, Htab *tsmap, Htab *delayed)
{
    Type *ret, *tmp;
    size_t i;
    Type **subst;

    t = tysearch(t);
    if (hthas(tsmap, t))
        return htget(tsmap, t);
    switch (t->type) {
        case Typaram:
            ret = mktyvar(t->loc);
            addtraits(ret, t->traits);
            htput(tsmap, t, ret);
            break;
        case Tyname:
            if (t->narg)
                subst = t->arg;
            else
                subst = t->param;
            for (i = 0; i < t->nparam; i++) {
                if (subst[i]->type != Typaram || hthas(tsmap, subst[i]))
                    continue;
                tmp = mktyvar(subst[i]->loc);
                addtraits(tmp, subst[i]->traits);
                htput(tsmap, subst[i], tmp);
            }
            ret = mktyname(t->loc, t->name, t->param, t->nparam, tyspecialize(t->sub[0], tsmap, delayed));
            ret->issynth = 1;
            htput(tsmap, t, ret);
            for (i = 0; i < t->nparam; i++)
                lappend(&ret->arg, &ret->narg, tyspecialize(subst[i], tsmap, delayed));
            break;
        case Tystruct:
            ret = tydup(t);
            htput(tsmap, t, ret);
            pushstab(NULL);
            for (i = 0; i < t->nmemb; i++)
                ret->sdecls[i] = specializenode(t->sdecls[i], tsmap);
            popstab();
            break;
        case Tyunion:
            ret = tydup(t);
            htput(tsmap, t, ret);
            for (i = 0; i < t->nmemb; i++) {
                tmp = NULL;
                if (ret->udecls[i]->etype)
                    tmp = tyspecialize(t->udecls[i]->etype, tsmap, delayed);
                ret->udecls[i] = mkucon(t->loc, t->udecls[i]->name, ret, tmp);
                ret->udecls[i]->utype = ret;
                ret->udecls[i]->id = i;
                ret->udecls[i]->synth = 1;
            }
            break;
        case Tyvar:
            if (delayed && hthas(delayed, t)) {
                ret = tydup(t);
                tmp = htget(delayed, t);
                htput(delayed, ret, tyspecialize(tmp, tsmap, delayed));
            } else {
                ret = t;
            }
            break;
        default:
            if (t->nsub > 0) {
                ret = tydup(t);
                htput(tsmap, t, ret);
                for (i = 0; i < t->nsub; i++)
                    ret->sub[i] = tyspecialize(t->sub[i], tsmap, delayed);
            } else {
                ret = t;
            }
            break;
    }
    return ret;
}

/* Checks if the type 't' is generic, and if it is
 * substitutes the types. This is here for efficiency,
 * so we don't gratuitously duplicate types */
static Type *tysubst(Type *t, Htab *tsmap)
{
    if (hasparams(t))
        return tyspecialize(t, tsmap, NULL);
    else
        return t;
}

/* 
 * Fills the substitution map with a mapping from
 * the type parameter 'from' to it's substititon 'to'
 */
static void fillsubst(Htab *tsmap, Type *to, Type *from)
{
    size_t i;

    if (from->type == Typaram) {
        if (debugopt['S'])
            printf("mapping %s => %s\n", tystr(from), tystr(to));
        htput(tsmap, from, to);
        return;
    }
    assert(to->nsub == from->nsub);
    for (i = 0; i < to->nsub; i++)
        fillsubst(tsmap, to->sub[i], from->sub[i]);
    if (to->type == Tyname && to->nparam > 0) {
        assert(to->nparam == to->narg);
        for (i = 0; i < to->nparam; i++)
            fillsubst(tsmap, to->arg[i], to->param[i]);
    }
}

/*
 * Fixes up nodes. This involves fixing up the
 * declaration identifiers once we specialize
 */
static void fixup(Node *n)
{
    size_t i;
    Node *d;
    Stab *ns;

    if (!n)
        return;
    switch (n->type) {
        case Nfile:
        case Nuse:
            die("Node %s not allowed here\n", nodestr[n->type]);
            break;
        case Nexpr:
            fixup(n->expr.idx);
            for (i = 0; i < n->expr.nargs; i++)
                fixup(n->expr.args[i]);
            if (n->expr.op == Ovar) {
                ns = curstab();
                if (n->expr.args[0]->name.ns)
                    ns = getns_str(ns, n->expr.args[0]->name.ns);
                if (!ns)
                    fatal(n, "No namespace %s\n", n->expr.args[0]->name.ns);
                d = getdcl(ns, n->expr.args[0]);
                if (!d)
                    die("Missing decl %s", namestr(n->expr.args[0]));
                if (d->decl.isgeneric)
                    d = specializedcl(d, n->expr.type, &n->expr.args[0]);
                n->expr.did = d->decl.did;
            }
            break;
        case Nlit:
            switch (n->lit.littype) {
                case Lfunc:     fixup(n->lit.fnval);          break;
                case Lchr: case Lint: case Lflt:
                case Lstr: case Llbl: case Lbool:
                    break;
            }
            break;
        case Nifstmt:
            fixup(n->ifstmt.cond);
            fixup(n->ifstmt.iftrue);
            fixup(n->ifstmt.iffalse);
            break;
        case Nloopstmt:
            fixup(n->loopstmt.init);
            fixup(n->loopstmt.cond);
            fixup(n->loopstmt.step);
            fixup(n->loopstmt.body);
            break;
        case Niterstmt:
            fixup(n->iterstmt.elt);
            fixup(n->iterstmt.seq);
            fixup(n->iterstmt.body);
            break;
        case Nmatchstmt:
            fixup(n->matchstmt.val);
            for (i = 0; i < n->matchstmt.nmatches; i++)
                fixup(n->matchstmt.matches[i]);
            break;
        case Nmatch:
            /* patterns are evaluated in their block's scope */
            pushstab(n->match.block->block.scope);
            fixup(n->match.pat);
            popstab();
            fixup(n->match.block);
            break;
        case Nblock:
            pushstab(n->block.scope);
            for (i = 0; i < n->block.nstmts; i++)
                fixup(n->block.stmts[i]);
            popstab();
            break;
        case Ndecl:
            fixup(n->decl.init);
            break;
        case Nfunc:
            pushstab(n->func.scope);
            fixup(n->func.body);
            popstab();
            break;
        case Nnone: case Nname:
            break;
        case Nimpl:
            die("trait/impl not implemented");
            break;
    }
}


/*
 * Duplicates a node, replacing all things that
 * need to be specialized to make it concrete
 * instead of generic, and returns it.
 */
static Node *specializenode(Node *n, Htab *tsmap)
{
    Node *r;
    size_t i;

    if (!n)
        return NULL;
    r = mknode(n->loc, n->type);
    switch (n->type) {
        case Nfile:
        case Nuse:
            die("Node %s not allowed here\n", nodestr[n->type]);
            break;
        case Nexpr:
            r->expr.op = n->expr.op;
            r->expr.type = tysubst(n->expr.type, tsmap);
            r->expr.isconst = n->expr.isconst;
            r->expr.nargs = n->expr.nargs;
            r->expr.idx = specializenode(n->expr.idx, tsmap);
            r->expr.args = xalloc(n->expr.nargs * sizeof(Node*));
            for (i = 0; i < n->expr.nargs; i++)
                r->expr.args[i] = specializenode(n->expr.args[i], tsmap);
            break;
        case Nname:
            if (n->name.ns)
                r->name.ns = strdup(n->name.ns);
            r->name.name = strdup(n->name.name);
            break;
        case Nlit:
            r->lit.littype = n->lit.littype;
            r->lit.type = tysubst(n->expr.type, tsmap);
            switch (n->lit.littype) {
                case Lchr:      r->lit.chrval = n->lit.chrval;       break;
                case Lint:      r->lit.intval = n->lit.intval;       break;
                case Lflt:      r->lit.fltval = n->lit.fltval;       break;
                case Lstr:      r->lit.strval = n->lit.strval;       break;
                case Llbl:      r->lit.lblval = n->lit.lblval;       break;
                case Lbool:     r->lit.boolval = n->lit.boolval;     break;
                case Lfunc:     r->lit.fnval = specializenode(n->lit.fnval, tsmap);       break;
            }
            break;
        case Nifstmt:
            r->ifstmt.cond = specializenode(n->ifstmt.cond, tsmap);
            r->ifstmt.iftrue = specializenode(n->ifstmt.iftrue, tsmap);
            r->ifstmt.iffalse = specializenode(n->ifstmt.iffalse, tsmap);
            break;
        case Nloopstmt:
            r->loopstmt.init = specializenode(n->loopstmt.init, tsmap);
            r->loopstmt.cond = specializenode(n->loopstmt.cond, tsmap);
            r->loopstmt.step = specializenode(n->loopstmt.step, tsmap);
            r->loopstmt.body = specializenode(n->loopstmt.body, tsmap);
            break;
        case Niterstmt:
            r->iterstmt.elt = specializenode(n->iterstmt.elt, tsmap);
            r->iterstmt.seq = specializenode(n->iterstmt.seq, tsmap);
            r->iterstmt.body = specializenode(n->iterstmt.body, tsmap);
            break;
        case Nmatchstmt:
            r->matchstmt.val = specializenode(n->matchstmt.val, tsmap);
            r->matchstmt.nmatches = n->matchstmt.nmatches;
            r->matchstmt.matches = xalloc(n->matchstmt.nmatches * sizeof(Node*));
            for (i = 0; i < n->matchstmt.nmatches; i++)
                r->matchstmt.matches[i] = specializenode(n->matchstmt.matches[i], tsmap);
            break;
        case Nmatch:
            r->match.pat = specializenode(n->match.pat, tsmap);
            r->match.block = specializenode(n->match.block, tsmap);
            break;
        case Nblock:
            r->block.scope = mkstab();
            r->block.scope->super = curstab();
            pushstab(r->block.scope);
            r->block.nstmts = n->block.nstmts;
            r->block.stmts = xalloc(sizeof(Node *)*n->block.nstmts);
            for (i = 0; i < n->block.nstmts; i++)
                r->block.stmts[i] = specializenode(n->block.stmts[i], tsmap);
            popstab();
            break;
        case Ndecl:
            r->decl.did = ndecls;
            /* sym */
            r->decl.name = specializenode(n->decl.name, tsmap);
            r->decl.type = tysubst(n->decl.type, tsmap);

            /* symflags */
            r->decl.isconst = n->decl.isconst;
            r->decl.isgeneric = n->decl.isgeneric;
            r->decl.isextern = n->decl.isextern;
            if (curstab())
                putdcl(curstab(), r);

            /* init */
            r->decl.init = specializenode(n->decl.init, tsmap);
            lappend(&decls, &ndecls, r);
            break;
        case Nfunc:
            r->func.scope = mkstab();
            r->func.scope->super = curstab();
            pushstab(r->func.scope);
            r->func.type = tysubst(n->func.type, tsmap);
            r->func.nargs = n->func.nargs;
            r->func.args = xalloc(sizeof(Node *)*n->func.nargs);
            for (i = 0; i < n->func.nargs; i++)
                r->func.args[i] = specializenode(n->func.args[i], tsmap);
            r->func.body = specializenode(n->func.body, tsmap);
            popstab();
            break;
        case Nimpl:
            die("trait/impl not implemented");
        case Nnone:
            die("Nnone should not be seen as node type!");
            break;
    }
    return r;
}
Node *genericname(Node *n, Type *t)
{
    char buf[1024];
    char *p;
    char *end;
    Node *name;

    if (!n->decl.isgeneric)
        return n->decl.name;
    p = buf;
    end = buf + sizeof buf;
    p += snprintf(p, end - p, "%s", n->decl.name->name.name);
    p += snprintf(p, end - p, "$");
    p += tyidfmt(p, end - p, t);
    name = mkname(n->loc, buf);
    if (n->decl.name->name.ns)
        setns(name, n->decl.name->name.ns);
    return name;
}

/*
 * Takes a generic declaration, and creates a specialized
 * duplicate of it with type 'to'. It also generates
 * a name for this specialized node, and returns it in '*name'.
 */
Node *specializedcl(Node *g, Type *to, Node **name)
{
    extern int stabstkoff;
    Node *d, *n;
    Htab *tsmap;
    Stab *st;

    assert(g->type == Ndecl);
    assert(g->decl.isgeneric);
    
    n = genericname(g, to);
    *name = n;
    if (n->name.ns)
        st = getns_str(curstab(), n->name.ns);
    else
        st = file->file.globls;
    if (!st)
        fatal(n, "Can't find symbol table for %s.%s", n->name.ns, n->name.name);
    d = getdcl(st, n);
    if (debugopt['S'])
        printf("depth[%d] specializing [%d]%s => %s\n", stabstkoff, g->loc.line, namestr(g->decl.name), namestr(n));
    if (d)
        return d;
    if (g->decl.trait) {
        printf("%s\n", namestr(n));
        fatal(g, "No trait implemented for for %s:%s", namestr(g->decl.name), tystr(to));
    }
    /* namespaced names need to be looked up in their correct
     * context. */
    if (n->name.ns)
        pushstab(st);

    /* specialize */
    tsmap = mkht(tyhash, tyeq);
    fillsubst(tsmap, to, g->decl.type);

    d = mkdecl(g->loc, n, tysubst(g->decl.type, tsmap));
    d->decl.isconst = g->decl.isconst;
    d->decl.isextern = g->decl.isextern;
    d->decl.init = specializenode(g->decl.init, tsmap);
    putdcl(st, d);

    fixup(d);

    lappend(&file->file.stmts, &file->file.nstmts, d);
    if (d->decl.name->name.ns)
        popstab();
    return d;
}
