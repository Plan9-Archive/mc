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

#include "parse.h"

static Stab *findstab(Stab *st, char *pkg)
{
    Node *n;
    Stab *s;

    if (!pkg) {
        if (!st->name)
            return st;
        else
            return NULL;
    }

    n = mkname(-1, pkg);
    if (getns(st, n)) {
        s = getns(st, n);
    } else {
        s = mkstab();
        s->name = n;
        putns(st, s);
    }
    return s;
}

int loaduse(FILE *f, Stab *st)
{
    char *pkg;
    Stab *s;
    Node *dcl;
    Type *t;
    int c;

    if (fgetc(f) != 'U')
        return 0;
    pkg = rdstr(f);
    /* if the package names match up, or the usefile has no declared
     * package, then we simply add to the current stab. Otherwise,
     * we add a new stab under the current one */
    if (st->name) {
        if (pkg && !strcmp(pkg, namestr(st->name))) {
            s = st;
        } else {
            s = findstab(st, pkg);
        }
    } else {
        if (pkg) {
            s = findstab(st, pkg);
        } else {
            s = st;
        }
    }
    while ((c = fgetc(f)) != 'Z') {
        switch(c) {
            case 'G':
            case 'D':
                dcl = symunpickle(f);
                putdcl(s, dcl);
                break;
            case 'T':
                t = tyunpickle(f);
                assert(t->type == Tyname || t->type == Tygeneric);
                puttype(s, t->name, t);
                break;
            case EOF:
                break;
        }
    }
    return 1;
}

void readuse(Node *use, Stab *st)
{
    size_t i;
    FILE *fd;
    char *p, *q;

    /* local (quoted) uses are always relative to the cwd */
    fd = NULL;
    if (use->use.islocal) {
        fd = fopen(use->use.name, "r");
    /* nonlocal (barename) uses are always searched on the include path */
    } else {
        for (i = 0; i < nincpaths; i++) {
            p = strjoin(incpaths[i], "/");
            q = strjoin(p, use->use.name);
            fd = fopen(q, "r");
            if (fd) {
                free(p);
                free(q);
                break;
            }
        }
    }
    if (!fd)
        fatal(use->line, "Could not open %s", use->use.name);

    if (!loaduse(fd, st))
        die("Could not load usefile %s", use->use.name);
}

/* Usefile format:
 * U<pkgname>
 * T<pickled-type>
 * D<picled-decl>
 * G<pickled-decl><pickled-initializer>
 * Z
 */
void writeuse(FILE *f, Node *file)
{
    Stab *st;
    void **k;
    Type *t;
    Node *s;
    size_t i, n;

    st = file->file.exports;
    wrbyte(f, 'U');
    if (st->name)
        wrstr(f, namestr(st->name));
    else
        wrstr(f, NULL);

    k = htkeys(st->ty, &n);
    for (i = 0; i < n; i++) {
        t = gettype(st, k[i]);
        assert(t->type == Tyname || t->type == Tygeneric);
        wrbyte(f, 'T');
        typickle(t, f);
    }
    free(k);
    k = htkeys(st->dcl, &n);
    for (i = 0; i < n; i++) {
        s = getdcl(st, k[i]);
        if (s->decl.isgeneric)
            wrbyte(f, 'G');
        else
            wrbyte(f, 'D');
        sympickle(s, f);
    }
    free(k);
    wrbyte(f, 'Z');
}
