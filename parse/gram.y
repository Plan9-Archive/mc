%{
#define YYERROR_VERBOSE

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

void yyerror(const char *s);
int yylex(void);
Op binop(int toktype);
Stab *curscope;
%}

%token<tok> TError
%token<tok> TPlus    /* + */
%token<tok> TMinus   /* - */
%token<tok> TStar    /* * */
%token<tok> TDiv     /* / */
%token<tok> TInc     /* ++ */
%token<tok> TDec     /* -- */
%token<tok> TMod     /* % */
%token<tok> TAsn     /* = */
%token<tok> TAddeq   /* += */
%token<tok> TSubeq   /* -= */
%token<tok> TMuleq   /* *= */
%token<tok> TDiveq   /* /= */
%token<tok> TModeq   /* %= */
%token<tok> TBoreq   /* |= */
%token<tok> TBxoreq  /* ^= */
%token<tok> TBandeq  /* &= */
%token<tok> TBsleq   /* <<= */
%token<tok> TBsreq   /* >>= */

%token<tok> TBor     /* | */
%token<tok> TBxor    /* ^ */
%token<tok> TBand    /* & */
%token<tok> TBsl     /* << */
%token<tok> TBsr     /* >> */
%token<tok> TBnot    /* ~ */

%token<tok> TEq      /* == */
%token<tok> TGt      /* > */
%token<tok> TLt      /* < */
%token<tok> TGe      /* >= */
%token<tok> TLe      /* <= */
%token<tok> TNe      /* != */

%token<tok> TLor     /* || */
%token<tok> TLand    /* && */
%token<tok> TLnot    /* ! */

%token<tok> TObrace  /* { */
%token<tok> TCbrace  /* } */
%token<tok> TOparen  /* ( */
%token<tok> TCparen  /* ) */
%token<tok> TOsqbrac /* [ */
%token<tok> TCsqbrac /* ] */
%token<tok> TAt      /* @ */

%token<tok> TType            /* type */
%token<tok> TFor             /* for */
%token<tok> TWhile           /* while */
%token<tok> TIf              /* if */
%token<tok> TElse            /* else */
%token<tok> TElif            /* else */
%token<tok> TMatch           /* match */
%token<tok> TDefault /* default */
%token<tok> TGoto            /* goto */

%token<tok><tok> TIntlit
%token<tok><tok> TStrlit
%token<tok><tok> TFloatlit
%token<tok><tok> TChrlit
%token<tok><tok> TBoollit

%token<tok> TEnum    /* enum */
%token<tok> TStruct  /* struct */
%token<tok> TUnion   /* union */

%token<tok> TConst   /* const */
%token<tok> TVar             /* var */
%token<tok> TExtern  /* extern */

%token<tok> TExport  /* export */
%token<tok> TProtect /* protect */

%token<tok> TEllipsis        /* ... */
%token<tok> TEndln           /* ; or \n */
%token<tok> TEndblk  /* ;; */
%token<tok> TColon   /* : */
%token<tok> TDot             /* . */
%token<tok> TComma   /* , */
%token<tok> TRet             /* -> */
%token<tok> TUse             /* use */
%token<tok> TPkg             /* pkg */
%token<tok><tok> TSizeof  /* sizeof */

%token<tok> TIdent
%token<tok> TEof

%start module

%type <ty> type structdef uniondef enumdef compoundtype functype funcsig

%type <tok> asnop cmpop addop mulop shiftop

%type <node> exprln retexpr expr atomicexpr literal asnexpr lorexpr landexpr borexpr
%type <node> bandexpr cmpexpr addexpr mulexpr shiftexpr prefixexpr postfixexpr
%type <node> funclit arraylit arglist name
%type <node> decl declvariants declbody declcore structelt enumelt unionelt

%type <nodelist> argdefs structbody enumbody unionbody

%union {
    struct {
        Node **nodes;
        size_t nnodes;
    } nodelist;
    struct {
        Type **types;
        size_t ntypes;
    } tylist;
    Node *node;
    Tok  *tok;
    Type *ty;
}


%%

module  : file
        ;

file    : toplev
        | file toplev
        ;

toplev
        : decl
        | use
        | package
        | typedef
        | TEndln
        ;

decl    : declvariants TEndln
        ;

use     : TUse TIdent TEndln
        | TUse TStrlit TEndln
        ;

package : TPkg TIdent TAsn pkgbody TEndblk {}
        ;


pkgbody : pkgitem
        | pkgbody pkgitem
        ;

pkgitem : decl
        | type
        | visdef
        | TEndln
        ;

visdef  : TExport TColon
        | TProtect TColon
        ;


declvariants
        : TVar declbody         {$2->decl.isconst = 0; $$ = $2;}
        | TConst declbody       {$2->decl.isconst = 1; $$ = $2;}
        | TExtern TVar declbody   {$3->decl.isconst = 0; $$ = $3;}
        | TExtern TConst declbody {$3->decl.isconst = 0; $$ = $3;}
        ;

declbody: declcore TAsn expr {$$ = $1; $1->decl.init = $3;}
        | declcore
        ;

declcore: name {$$ = mkdecl(line, mksym(line, $1, mktyvar(line)));}
        | name TColon type {$$ = mkdecl(line, mksym(line, $1, $3));}
        ;

name    : TIdent {$$ = mkname(line, $1->str);}
        | TIdent TDot name {$$ = $3; setns($3, $1->str);}
        ;

typedef : TType TIdent TAsn type TEndln
        | TType TIdent TEndln
        ;

type    : structdef
        | uniondef
        | enumdef
        | compoundtype
        ;

compoundtype
        : functype   {$$ = $1;}
        | type TOsqbrac TComma TCsqbrac {$$ = mktyslice(line, $1);}
        | type TOsqbrac expr TCsqbrac {$$ = mktyarray(line, $1, $3);}
        | type TStar {$$ = mktyptr(line, $1);}
        | name       {$$ = mktynamed(line, $1);}
        | TAt TIdent {$$ = mktyparam(line, $2->str);}
        ;

functype: TOparen funcsig TCparen {$$ = $2;}
        ;

funcsig : argdefs {$$ = mktyfunc(line, $1.nodes, $1.nnodes, mktyvar(line));}
        | argdefs TRet type {$$ = mktyfunc(line, $1.nodes, $1.nnodes, $3);}
        ;

argdefs : declcore {$$.nodes = NULL; $$.nnodes = 0; nlappend(&$$.nodes, &$$.nnodes, $1);}
        | argdefs TComma declcore {nlappend(&$$.nodes, &$$.nnodes, $3);}
        ;

structdef
        : TStruct structbody TEndblk {$$ = mktystruct($1->line, $2.nodes, $2.nnodes);}
        ;

structbody
        : structelt {$$.nnodes = 0; nlappend(&$$.nodes, &$$.nnodes, $1);}
        | structbody structelt {if ($2) {nlappend(&$$.nodes, &$$.nnodes, $2);}}
        ;

structelt
        : declcore TEndln {$$ = $1;}
        | visdef TEndln {$$ = NULL;}
        ;

uniondef
        : TUnion unionbody TEndblk {$$ = mktyunion(line, $2.nodes, $2.nnodes);}
        ;

unionbody
        : unionelt {$$.nnodes = 0; nlappend(&$$.nodes, &$$.nnodes, $1);}
        | unionbody unionelt {if ($2) {nlappend(&$$.nodes, &$$.nnodes, $2);}}
        ;

unionelt
        : TIdent type TEndln {$$ = NULL; die("unionelt impl");}
        | visdef TEndln {$$ = NULL;}
        ;

enumdef : TEnum enumbody TEndblk {$$ = mktyenum($1->line, $2.nodes, $2.nnodes);}
        ;

enumbody: enumelt {$$.nnodes = 0; nlappend(&$$.nodes, &$$.nnodes, $1);}
        | enumbody enumelt {if ($2) {nlappend(&$$.nodes, &$$.nnodes, $2);}}
        ;

enumelt : TIdent TEndln {$$ = NULL; die("enumelt impl");}
        | TIdent TAsn exprln {$$ = NULL; die("enumelt impl");}
        ;

retexpr : TRet exprln {$$ = mkexpr(line, Oret, $2, NULL);}
        | exprln
        ;

exprln  : expr TEndln
        ;

expr    : asnexpr{dump($1, stdout);}
        ;

asnexpr : lorexpr asnop asnexpr {$$ = mkexpr($1->line, binop($2->type), $1, $3, NULL);}
        | lorexpr
        ;

asnop   : TAsn
        | TAddeq        /* += */
        | TSubeq        /* -= */
        | TMuleq        /* *= */
        | TDiveq        /* /= */
        | TModeq        /* %= */
        | TBoreq        /* |= */
        | TBxoreq       /* ^= */
        | TBandeq       /* &= */
        | TBsleq        /* <<= */
        | TBsreq        /* >>= */
        ;

lorexpr : lorexpr TLor landexpr {$$ = mkexpr($1->line, binop($2->type), $1, $3, NULL);}
        | landexpr
        ;

landexpr: landexpr TLand borexpr {$$ = mkexpr($1->line, binop($2->type), $1, $3, NULL);}
        | borexpr
        ;

borexpr : borexpr TBor bandexpr {$$ = mkexpr($1->line, binop($2->type), $1, $3, NULL);}
        | bandexpr
        ;

bandexpr: bandexpr TBand cmpexpr {$$ = mkexpr($1->line, binop($2->type), $1, $3, NULL);}
        | cmpexpr
        ;

cmpexpr : cmpexpr cmpop addexpr {$$ = mkexpr($1->line, binop($2->type), $1, $3, NULL);}
        | addexpr
        ;

cmpop   : TEq | TGt | TLt | TGe | TLe | TNe ;

addexpr : addexpr addop mulexpr {$$ = mkexpr($1->line, binop($2->type), $1, $3, NULL);}
        | mulexpr
        ;

addop   : TPlus | TMinus ;

mulexpr : mulexpr mulop shiftexpr {$$ = mkexpr($1->line, binop($2->type), $1, $3, NULL);}
        | shiftexpr
        ;

mulop   : TStar | TDiv | TMod
        ;

shiftexpr
        : shiftexpr shiftop prefixexpr {$$ = mkexpr($1->line, binop($2->type), $1, $3, NULL);}
        | prefixexpr
        ;

shiftop : TBsl | TBsr;

prefixexpr
        : TInc postfixexpr {$$ = mkexpr($1->line, Opreinc, $2, NULL);}
        | TDec postfixexpr {$$ = mkexpr($1->line, Opredec, $2, NULL);}
        | TStar postfixexpr {$$ = mkexpr($1->line, Oderef, $2, NULL);}
        | TBand postfixexpr {$$ = mkexpr($1->line, Oaddr, $2, NULL);}
        | TLnot postfixexpr {$$ = mkexpr($1->line, Olnot, $2, NULL);}
        | TBnot postfixexpr {$$ = mkexpr($1->line, Obnot, $2, NULL);}
        | TMinus postfixexpr {$$ = mkexpr($1->line, Oneg, $2, NULL);}
        | TPlus postfixexpr {$$ = $2;}
        | postfixexpr
        ;

postfixexpr
        : postfixexpr TDot TIdent {
                $$ = mkexpr($1->line, Omemb, $1, mkname($3->line, $3->str), NULL);
            }
        | postfixexpr TInc {$$ = mkexpr($1->line, Opostinc, $1, NULL);}
        | postfixexpr TDec {$$ = mkexpr($1->line, Opostdec, $1, NULL);}
        | postfixexpr TOsqbrac expr TCsqbrac {$$ = mkexpr($1->line, Oidx, $1, $3);}
        | postfixexpr TOsqbrac expr TComma expr TCsqbrac {
                $$ = mkexpr($1->line, Oslice, $1, $3, $5, NULL);
            }
        | postfixexpr TOparen arglist TCparen {$$ = mkexpr($1->line, Ocall, $1, $3);}
        | atomicexpr
        ;

arglist : asnexpr
        | arglist TComma asnexpr
        ;

atomicexpr
        : TIdent        {$$ = mkexpr(line, Ovar, mkname(line, $1->str), NULL);}
        | literal
        | TOparen expr TCparen {$$ = $2;}
        | TSizeof atomicexpr {$$ = mkexpr($1->line, Osize, $2, NULL);}
        ;

literal : funclit       {$$ = $1;}
        | arraylit      {$$ = $1;}
        | TStrlit       {$$ = mkstr($1->line, $1->str);}
        | TIntlit       {$$ = mkint($1->line, strtol($1->str, NULL, 0));}
        | TChrlit       {$$ = mkchar($1->line, *$1->str);} /* FIXME: expand escapes, unicode  */
        | TFloatlit     {$$ = mkfloat($1->line, strtod($1->str, NULL));}
        | TBoollit      {$$ = mkbool($1->line, !strcmp($1->str, "true"));}
        ;

funclit : TObrace params TEndln blockbody TCbrace {$$ = NULL; die("unimpl funclit");}
        ;

params  : declcore
        | params TComma declcore
        ;

arraylit : TOsqbrac arraybody TCsqbrac {$$ = NULL; die("Unimpl arraylit");}
         ;

arraybody
        : expr
        | arraybody TComma expr
        ;

stmt    : retexpr
        | label
        | ifstmt
        ;

ifstmt  : TIf exprln blockbody elifblocks TElse block
        | TIf exprln blockbody elifblocks TEndblk
        | TIf exprln blockbody TElse block
        | TIf exprln block
        ;

elifblocks
        : TElif exprln blockbody
        | elifblocks TElif exprln blockbody
        ;

block   : blockbody TEndblk
        ;

blockbody
        : stmt
        | blockbody stmt
        ;

label   : TColon TIdent
        ;

%%

void yyerror(const char *s)
{
    fprintf(stderr, "%d: %s", line, s);
    if (curtok->str)
        fprintf(stderr, " near %s", curtok->str);
    fprintf(stderr, "\n");
}

Op binop(int tt)
{
    Op o;
    switch (tt) {
        case TPlus:     o = Oadd;       break;
        case TMinus:    o = Osub;       break;
        case TStar:     o = Omul;       break;
        case TDiv:      o = Odiv;       break;
        case TMod:      o = Omod;       break;
        case TAsn:      o = Oasn;       break;
        case TAddeq:    o = Oaddeq;     break;
        case TSubeq:    o = Osubeq;     break;
        case TMuleq:    o = Omuleq;     break;
        case TDiveq:    o = Odiveq;     break;
        case TModeq:    o = Omodeq;     break;
        case TBoreq:    o = Oboreq;     break;
        case TBxoreq:   o = Obxoreq;    break;
        case TBandeq:   o = Obandeq;    break;
        case TBsleq:    o = Obsleq;     break;
        case TBsreq:    o = Obsreq;     break;
        case TBor:      o = Obor;       break;
        case TBxor:     o = Obxor;      break;
        case TBand:     o = Oband;      break;
        case TBsl:      o = Obsl;       break;
        case TBsr:      o = Obsr;       break;
        case TEq:       o = Oeq;        break;
        case TGt:       o = Ogt;        break;
        case TLt:       o = Olt;        break;
        case TGe:       o = Oge;        break;
        case TLe:       o = Ole;        break;
        case TNe:       o = One;        break;
        case TLor:      o = Olor;       break;
        case TLand:     o = Oland;      break;
        default:
            die("Unimplemented binop\n");
            break;
    }
    return o;
}

