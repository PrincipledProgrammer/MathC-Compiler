#ifndef DEFS_H
#define DEFS_H

#include "symtutil.h"
#include<bits/stdc++.h>
using namespace std;

typedef struct _some{
    int category; // var(0) , const(1), temp(2) 
    int type; // int(0) , rtl(1) , flt(2) , cpx(3), array(4x) of ele type x
    int ref; // stref(var), value(const) , tempno(temp)
    float fref; // value for fltconst
} addr;

typedef struct _some1{
    char op[16];
    addr *result;
    addr *arg1;
    addr *arg2;
} Quad;

typedef struct _some2{
    int size;
    int arr[MAXN];
} nextlist;

typedef struct _some3{
    nextlist *truelist;
    nextlist *falselist;
} boollist;

void init();
void final_print();
void assign(char *id, addr *expr);
void assign_arr(char *id,addr* lidx , addr *expr);

addr *convert(addr *a, int type);
addr *binary(addr *a, addr *b, int op);
addr *create_id(char *id);
addr *create_int(int);
addr *create_flt(double);
addr *create_rtl(addr* a,addr* b);
addr* create_cpx(addr* a,addr*b);

addr *create_arr(char *id, addr *expr);

void band_copy_nooff(char* idlhs,char*idrhs,addr* start,addr*end);
void band_copy(char*idlhs,addr* offset,char*idrhs,addr*start,addr*end);

nextlist *makelist(int ins);
nextlist *merge(nextlist *x, nextlist *y);
void backpatch(nextlist *l, int idx);
boollist *make_blist(nextlist *truel, nextlist *falsel);

void create_jmp(addr* next );
void create_expr(addr *expr1, addr *expr2, int relop);

#endif