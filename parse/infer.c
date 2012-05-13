#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>

#include "parse.h"

static void infernode(Node *n, Type *ret, int *sawret);
static void inferexpr(Node *n, Type *ret, int *sawret);

static void setsuper(Stab *st, Stab *super)
{
    Stab *s;
    
    /* verify that we don't accidentally create loops */
    for (s = super; s; s = s->super)
        assert(s->super != st);
    st->super = super;
}

static void tyresolve(Type *t)
{
    int i, nn;
    Node **n;

    nn = t->nmemb;
    switch (t->type) {
        case Tystruct: n = t->sdecls; break;
        case Tyunion: n = t->udecls; break;
        case Tyenum: n = t->edecls; break;
        default: return;
    }
    for (i = 0; i < nn; i++)
        infernode(n[i], NULL, NULL);
}

/* find the most accurate type mapping */
static Type *tf(Type *t)
{
    char buf[1024];
    Type *lu;
    assert(t != NULL);

    while (1) {
        if (!tytab[t->tid] && t->type == Tyname) {
            if (!(lu = gettype(curstab(), t->name)))
                fatal(t->name->line, "Could not find type %s", t->name->name.parts[t->name->name.nparts - 1]);
            tytab[t->tid] = lu;
            tyresolve(lu);
        }

        printf("%s => ", tyfmt(buf, 1024, t));
        if (!tytab[t->tid])
            break;
        t = tytab[t->tid];
    }
    printf("nil\n");
    return t;
}

/* does b satisfy all the constraints of a? */
static int cstrcheck(Type *a, Type *b)
{
    Bitset *s;
    int n;

    /* a has no cstrs to satisfy */
    if (!a->cstrs)
        return 1;
    /* b satisfies no cstrs; only valid if a requires none */
    if (!b->cstrs)
        return bscount(a->cstrs) == 0;
    /* if b->cstrs \ a->cstrs == 0, then all of
     * a's constraints are satisfied. */
    s = dupbs(b->cstrs);
    bsdiff(s, a->cstrs);
    n = bscount(s);
    delbs(s);

    return n == 0;
}

static void loaduses(Node *n)
{
    int i;
    /* uses only allowed at top level. Do we want to keep it this way? */
    for (i = 0; i < n->file.nuses; i++)
        fprintf(stderr, "INTERNAL: implement use loading\n");
        /* readuse(n->file.uses[i], n->file.globls); */
}

/* a => b */
static void settype(Node *n, Type *t)
{
    t = tf(t);
    switch (n->type) {
        case Nexpr:     n->expr.type = t;       break;
        case Ndecl:     n->decl.sym->type = t;  break;
        case Nlit:      n->lit.type = t;        break;
        case Nfunc:     n->func.type = t;       break;
        default:
            die("can't set type of %s", nodestr(n->type));
            break;
    }

}

static Type *littype(Node *n)
{
    switch (n->lit.littype) {
        case Lchr:      return mkty(n->line, Tychar);                           break;
        case Lbool:     return mkty(n->line, Tybool);                           break;
        case Lint:      return tylike(mktyvar(n->line), Tyint);                 break;
        case Lflt:      return tylike(mktyvar(n->line), Tyfloat32);             break;
        case Lstr:      return mktyslice(n->line, mkty(n->line, Tychar));       break;
        case Lfunc:
            return n->lit.fnval->func.type;
            break;
        case Larray:    return NULL; break;
    };
    return NULL;
}

static Type *type(Node *n)
{
    Type *t;

    switch (n->type) {
      case Nlit:        t = littype(n);         break;
      case Nexpr:       t = n->expr.type;       break;
      case Ndecl:       t = decltype(n);        break;
      case Nfunc:       t = n->func.type;       break;
      default:
        t = NULL;
        die("untypeable %s", nodestr(n->type));
        break;
    };
    return tf(t);
}

static char *ctxstr(Node *n)
{
    char *s;
    switch (n->type) {
        case Nexpr:     s = opstr(exprop(n)); break;
        default:        s = nodestr(n->type); break;
    }
    return s;
}

static void mergecstrs(Node *ctx, Type *a, Type *b)
{
    if (b->type == Tyvar) {
        /* make sure that if a = b, both have same cstrs */
        if (a->cstrs && b->cstrs)
            bsunion(b->cstrs, a->cstrs);
        else if (a->cstrs)
            b->cstrs = dupbs(a->cstrs);
        else if (b->cstrs)
            a->cstrs = dupbs(b->cstrs);
    } else {
        if (!cstrcheck(a, b))
            fatal(ctx->line, "%s incompatible with %s near %s", tystr(a), tystr(b), ctxstr(ctx));
    }
}

static Type *unify(Node *ctx, Type *a, Type *b)
{
    Type *t;
    int i;

    /* a ==> b */
    a = tf(a);
    b = tf(b);
    if (b->type == Tyvar) {
        t = a;
        a = b;
        b = t;
    }

    mergecstrs(ctx, a, b);
    if (a->type != b->type) {
        if (a->type == Tyvar)
            tytab[a->tid] = b;
        else
            fatal(ctx->line, "%s incompatible with %s near %s", tystr(a), tystr(b), ctxstr(ctx));
        return b;
    } else {
        for (i = 0; i < b->nsub; i++) {
            /* types must have same arity */
            if (i >= a->nsub)
                fatal(ctx->line, "%s incompatible with %s near %s", tystr(a), tystr(b), ctxstr(ctx));

            /* FIXME: recurse properly.
               mergecstrs(ctx, a, b);
               unify(ctx, a->sub[i], b->sub[i]);
               */
        }
        return b;
    }
}

static void unifycall(Node *n)
{
    int i;
    Type *ft;

    inferexpr(n->expr.args[0], NULL, NULL);
    ft = type(n->expr.args[0]);
    for (i = 1; i < n->expr.nargs; i++) {
        inferexpr(n->expr.args[i], NULL, NULL);
        unify(n, ft->sub[i], type(n->expr.args[i]));
    }
    settype(n, ft->sub[0]);
}

static void inferexpr(Node *n, Type *ret, int *sawret)
{
    Node **args;
    Sym *s;
    int nargs;
    Type *t;
    int i;

    assert(n->type == Nexpr);
    args = n->expr.args;
    nargs = n->expr.nargs;
    for (i = 0; i < nargs; i++)
        /* Nlit, Nvar, etc should not be inferred as exprs */
        if (args[i]->type == Nexpr)
            inferexpr(args[i], ret, sawret);
    switch (exprop(n)) {
        /* all operands are same type */
        case Oadd:      /* @a + @a -> @a */
        case Osub:      /* @a - @a -> @a */
        case Omul:      /* @a * @a -> @a */
        case Odiv:      /* @a / @a -> @a */
        case Omod:      /* @a % @a -> @a */
        case Oneg:      /* -@a -> @a */
        case Obor:      /* @a | @a -> @a */
        case Oband:     /* @a & @a -> @a */
        case Obxor:     /* @a ^ @a -> @a */
        case Obsl:      /* @a << @a -> @a */
        case Obsr:      /* @a >> @a -> @a */
        case Obnot:     /* ~@a -> @a */
        case Opreinc:   /* ++@a -> @a */
        case Opredec:   /* --@a -> @a */
        case Opostinc:  /* @a++ -> @a */
        case Opostdec:  /* @a-- -> @a */
        case Oasn:      /* @a = @a -> @a */
        case Oaddeq:    /* @a += @a -> @a */
        case Osubeq:    /* @a -= @a -> @a */
        case Omuleq:    /* @a *= @a -> @a */
        case Odiveq:    /* @a /= @a -> @a */
        case Omodeq:    /* @a %= @a -> @a */
        case Oboreq:    /* @a |= @a -> @a */
        case Obandeq:   /* @a &= @a -> @a */
        case Obxoreq:   /* @a ^= @a -> @a */
        case Obsleq:    /* @a <<= @a -> @a */
        case Obsreq:    /* @a >>= @a -> @a */
            t = type(args[0]);
            for (i = 1; i < nargs; i++)
                t = unify(n, t, type(args[i]));
            settype(n, tf(t));
            break;

        /* operands same type, returning bool */
        case Olor:      /* @a || @b -> bool */
        case Oland:     /* @a && @b -> bool */
        case Olnot:     /* !@a -> bool */
        case Oeq:       /* @a == @a -> bool */
        case One:       /* @a != @a -> bool */
        case Ogt:       /* @a > @a -> bool */
        case Oge:       /* @a >= @a -> bool */
        case Olt:       /* @a < @a -> bool */
        case Ole:       /* @a <= @b -> bool */
            t = type(args[0]);
            for (i = 1; i < nargs; i++)
                unify(n, t, type(args[i]));
            settype(n, mkty(-1, Tybool));
            break;

        /* reach into a type and pull out subtypes */
        case Oaddr:     /* &@a -> @a* */
            settype(n, mktyptr(n->line, type(args[0])));
            break;
        case Oderef:    /* *@a* ->  @a */
            t = unify(n, type(args[0]), mktyptr(n->line, mktyvar(n->line)));
            settype(n, t);
            break;
        case Oidx:      /* @a[@b::tcint] -> @a */
            die("inference of indexes not done yet");
            break;
        case Oslice:    /* @a[@b::tcint,@b::tcint] -> @a[,] */
            die("inference of slices not done yet");
            break;

        /* special cases */
        case Omemb:     /* @a.Ident -> @b, verify type(@a.Ident)==@b later */
            die("members not done yet");
            break;
        case Osize:     /* sizeof @a -> size */
            die("inference of sizes not done yet");
            break;
        case Ocall:     /* (@a, @b, @c, ... -> @r)(@a,@b,@c, ... -> @r) -> @r */
            unifycall(n);
            break;
        case Ocast:     /* cast(@a, @b) -> @b */
            die("casts not implemented");
            break;
        case Oret:      /* -> @a -> void */
            if (sawret)
                *sawret = 1;
            if (!ret)
                fatal(n->line, "Not allowed to return value here");
            if (nargs)
                t = unify(n, type(args[0]), ret);
            else
                t =  unify(n, mkty(-1, Tyvoid), ret);
            settype(n, t);
            break;
        case Ojmp:     /* goto void* -> void */
            settype(n, mkty(-1, Tyvoid));
            break;
        case Ovar:      /* a:@a -> @a */
            s = getdcl(curstab(), args[0]);
            if (!s)
                fatal(n->line, "Undeclared var %s", args[0]->name.parts[args[0]->name.nparts - 1]);
            else
                settype(n, s->type);
            n->expr.did = s->id;
            break;
        case Olit:      /* <lit>:@a::tyclass -> @a */
            switch (args[0]->lit.littype) {
                case Lfunc: infernode(args[0]->lit.fnval, NULL, NULL); break;
                case Larray: die("array types not implemented yet"); break;
                default: break;
            }
            settype(n, type(args[0]));
            break;
        case Olbl:      /* :lbl -> void* */
            settype(n, mktyptr(n->line, mkty(-1, Tyvoid)));
        case Obad: case Numops: case Ocjmp:
        case Oload: case Ostor:
            die("Should not see %s in fe", opstr(exprop(n)));
            break;
    }
}

static void inferfunc(Node *n)
{
    int i;
    int sawret;

    sawret = 0;
    for (i = 0; i < n->func.nargs; i++)
        infernode(n->func.args[i], NULL, NULL);
    infernode(n->func.body, n->func.type->sub[0], &sawret);
    if (!sawret)
        unify(n, type(n)->sub[0], mkty(-1, Tyvoid));
    else
        printf("SAWRET!!!\n");
}

static void inferdecl(Node *n)
{
    Type *t;

    printf("====== decl %s\n", n->decl.sym->name->name.parts[n->decl.sym->name->name.nparts-1]);
    t = decltype(n);
    settype(n, t);
    if (n->decl.init) {
        inferexpr(n->decl.init, NULL, NULL);
        unify(n, type(n), type(n->decl.init));
    }
}

static void inferstab(Stab *s)
{
    void **k;
    int n, i;
    Type *t;

    k = htkeys(s->ty, &n);
    for (i = 0; i < n; i++) {
        t = tf(gettype(s, k[i]));
        updatetype(s, k[i], t);
    }
}

static void infernode(Node *n, Type *ret, int *sawret)
{
    int i;

    if (!n)
        return;
    switch (n->type) {
        case Nfile:
            pushstab(n->file.globls);
            inferstab(n->file.globls);
            for (i = 0; i < n->file.nstmts; i++)
                infernode(n->file.stmts[i], NULL, sawret);
            popstab();
            break;
        case Ndecl:
            inferdecl(n);
            break;
        case Nblock:
            setsuper(n->block.scope, curstab());
            pushstab(n->block.scope);
            inferstab(n->block.scope);
            for (i = 0; i < n->block.nstmts; i++)
                infernode(n->block.stmts[i], ret, sawret);
            popstab();
            break;
        case Nifstmt:
            infernode(n->ifstmt.cond, NULL, sawret);
            infernode(n->ifstmt.iftrue, ret, sawret);
            infernode(n->ifstmt.iffalse, ret, sawret);
            constrain(type(n->ifstmt.cond), cstrtab[Tctest]);
            break;
        case Nloopstmt:
            infernode(n->loopstmt.init, ret, sawret);
            infernode(n->loopstmt.cond, NULL, sawret);
            infernode(n->loopstmt.step, ret, sawret);
            infernode(n->loopstmt.body, ret, sawret);
            constrain(type(n->loopstmt.cond), cstrtab[Tctest]);
            break;
        case Nexpr:
            inferexpr(n, ret, sawret);
            break;
        case Nfunc:
            setsuper(n->func.scope, curstab());
            pushstab(n->func.scope);
            inferstab(n->block.scope);
            inferfunc(n);
            popstab();
            break;
        case Nname:
        case Nlit:
        case Nuse:
        case Nlbl:
            break;
        case Nnone:
            die("Nnone should not be seen as node type!");
            break;
    }
}

static void infercompn(Node *n)
{
}

static void checkcast(Node *n)
{
}

/* returns the final type for t, after all unifications
 * and default constraint selections */
static Type *tyfin(Node *ctx, Type *t)
{
    static Type *tyint;
    int i;
    char buf[1024];

    if (!tyint)
        tyint = mkty(-1, Tyint);

    t = tf(t);
    if (t->type == Tyvar) {
        if (hascstr(t, cstrtab[Tcint]) && cstrcheck(t, tyint))
            return tyint;
    } else {
        for (i = 0; i < t->nsub; i++)
            t->sub[i] = tyfin(ctx, t->sub[i]);
    }
    if (t->type == Tyvar)
         fatal(t->line, "underconstrained type %s near %s", tyfmt(buf, 1024, t), ctxstr(ctx));
    return t;
}

static void typesub(Node *n)
{
    int i;

    if (!n)
        return;
    switch (n->type) {
        case Nfile:
            for (i = 0; i < n->file.nstmts; i++)
                typesub(n->file.stmts[i]);
            break;
        case Ndecl:
            settype(n, tyfin(n, type(n)));
            if (n->decl.init)
                typesub(n->decl.init);
            break;
        case Nblock:
            for (i = 0; i < n->block.nstmts; i++)
                typesub(n->block.stmts[i]);
            break;
        case Nifstmt:
            typesub(n->ifstmt.cond);
            typesub(n->ifstmt.iftrue);
            typesub(n->ifstmt.iffalse);
            break;
        case Nloopstmt:
            typesub(n->loopstmt.cond);
            typesub(n->loopstmt.init);
            typesub(n->loopstmt.step);
            typesub(n->loopstmt.body);
            break;
        case Nexpr:
            settype(n, tyfin(n, type(n)));
            for (i = 0; i < n->expr.nargs; i++)
                typesub(n->expr.args[i]);
            break;
        case Nfunc:
            settype(n, tyfin(n, n->func.type));
            for (i = 0; i < n->func.nargs; i++)
                typesub(n->func.args[i]);
            typesub(n->func.body);
            break;
        case Nlit:
            settype(n, tyfin(n, type(n)));
            switch (n->lit.littype) {
                case Lfunc:     typesub(n->lit.fnval); break;
                case Larray:    typesub(n->lit.arrval); break;
                default:        break;
            }
            break;
        case Nname:
        case Nuse:
        case Nlbl:
            break;
        case Nnone:
            die("Nnone should not be seen as node type!");
            break;
    }
}

void infer(Node *file)
{
    assert(file->type == Nfile);

    loaduses(file);
    infernode(file, NULL, NULL);
    infercompn(file);
    checkcast(file);
    typesub(file);
}
