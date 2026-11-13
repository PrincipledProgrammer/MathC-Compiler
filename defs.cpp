#include "defs.h"
#include <bits/stdc++.h>
using namespace std;

int tno = 1;
int qidx = 0;

const char *pprnt[4][4] = {{"","int2rtl","int2flt","int2cpx"}, {"rtl2int","","rtl2flt","rtl2cpx"}, {"flt2int","flt2rtl","","flt2cpx"}, {"cpx2int","cpx2rtl","cpx2flt"}};
const char *opprnt[4][5] = {{"iadd","isub","imul","idiv","irem"}, {"radd","rsub","rmul","rdiv",""}, {"fadd","fsub","fmul","fdiv",""}, {"cadd","csub","cmul","cdiv",""}};
const char *strprnt[4] = {"istr", "rstr", "fstr", "cstr"};
const char *getarr[4] = {"igetarr", "rgetarr", "fgetarr", "cgetarr"};
const char *strarr[4] = {"istrarr", "rstrarr", "fstrarr", "cstrarr"};
const char typestr[4] = {'i', 'r', 'f', 'c'};
const char *cmpstr[6] = {"eq", "ne", "lt", "le", "gt", "ge"};
extern void yyerror( const char *);

int width[4] = {4,8,8,16};
addr *width_vec[4];

Quad *QTable[MAXN];

void init(){

    for (int i = 0; i < 4;i++){
        width_vec[i] = new addr();
        width_vec[i]->category = 1;
        width_vec[i]->ref = width[i];
        width_vec[i]->type = 0;
    }
}

char *print_addr(const addr *a)
{
    char *prnt = (char *)malloc(64);
    if (!prnt)
    {
        return NULL;
    }

    if (a == NULL)
    {
        prnt[0] = '\0';
        return prnt;
    }

    if (a->category == 0)
    {
        const char *name = fetch_name(a->ref);
        if (name)
        {
            snprintf(prnt, 64, "%s", name);
        }
        else
        {
            prnt[0] = '\0';
        }
    }
    else if (a->category == 1)
    {
        if (a->type == 0)
        {
            snprintf(prnt, 64, "%d", a->ref);
        }
        else
        {
            snprintf(prnt, 64, "%f", a->fref); 
        }
    }
    else
    {
        snprintf(prnt, 64, "t%d", a->ref);
    }

    return prnt;
}

void add_Quad(char op[16], addr* arg1, addr*arg2,addr* result){
    QTable[qidx] = new Quad();
    Quad *temp = QTable[qidx];
    temp->arg1 = arg1;
    temp->arg2 = arg2;
    temp->result = result;
    strcpy(temp->op, op);
    ++qidx;
}

addr* create_temp(int t){
    addr *curr = new addr();
    curr->category = 2;
    curr->ref = tno++;
    curr->type = t;
    return curr;
}

void assign(char *id, addr *expr){
    int t = fetch_type(id);
    if(t != -1){
        if(t > 10){
            add(id, expr->type);
        }
    }
    else{
        add(id, expr->type);
    }
    char op[16];
    strcpy(op, strprnt[expr->type]);
    add_Quad(op, expr, NULL, create_id(id));
}

addr *convert(addr *a, int type){
    if(a->type == type)
        return a;
    addr *a1 = create_temp(type);
    char op[16];
    strcpy(op, pprnt[a->type][type]);

    add_Quad(op, a, NULL, a1);
    return a1;
}
addr *binary(addr *a, addr *b, int op){
    int target_type = max(a->type%10,b->type%10);
    if(op == 4)target_type = 0;

    a = convert(a,target_type);
    b = convert(b, target_type);
    addr *next = create_temp(target_type);
    char op_str[16];
    strcpy(op_str, opprnt[target_type][op]);
    add_Quad(op_str, a, b, next);
    return next;
}
addr *create_id(char *id){
    addr* next = new addr();
    next->category = 0;
    if(search(id)){
        next->ref = fetch(id);
        next->type = fetch_type(id);
    }
    else{
        next->ref = -1;
        next->type = 0;
    }
    return next;
}
addr *create_int(int val){
    addr *temp = new addr();
    temp->category = 1;
    temp->ref = val;
    temp->type = 0;
    return temp;
}
addr *create_flt(double val){
    addr *temp = new addr();
    temp->category = 1;
    temp->fref = val;
    temp->type = 2;
    return temp;
}
addr *create_rtl(addr *a, addr *b){
    int target_type = 0;
    a = convert(a, target_type);
    b = convert(b, target_type);

    addr *temp = create_temp(1);
    char op_str[16];
    strcpy(op_str, "mkrtl");
    add_Quad(op_str, a, b, temp);
    return temp;
}
addr *create_cpx(addr *a, addr *b){
    int target_type = 2;
    a = convert(a, target_type);
    b = convert(b, target_type);

    addr *temp = create_temp(3);
    char op_str[16];
    strcpy(op_str, "mkcpx");
    add_Quad(op_str, a, b, temp);
    return temp;
}

void final_print(){
    cout << "\t Operation \t Arg1 \t Arg2 \t Result\n";

    for (int i = 0; i < qidx;i++){
        if(QTable[i] != NULL)printf("%d\t%s\t\t%s\t\t%s\t\t%s\n", i + 1000,QTable[i]->op, print_addr(QTable[i]->arg1), print_addr(QTable[i]->arg2), print_addr(QTable[i]->result));
    }
}

void assign_arr(char* id,addr* lidx,addr* expr){
    lidx = convert(lidx, 0);
    int t = fetch_type(id);
    int target_type;
    if(t > 10){
       target_type = t % 10;
       expr = convert(expr, target_type);
    }
    else{
        target_type = expr->type;
    }
    lidx = binary(lidx, width_vec[target_type], 2);
    char op[16];
    strcpy(op, strarr[target_type]);
    add(id, 40 + target_type);
    add_Quad(op, expr, lidx, create_id(id));
}

addr* create_arr(char* id,addr* expr){

    addr *next = NULL;
    expr = convert(expr, 0);
    int target_type = 0;
    int t = fetch_type(id);
    if(t < 10){
        next = new addr();
        next->category = 1;
        next->ref = 0;
        next->type = 0;
    }
    else{
        t = t % 10;
        expr = binary(expr, width_vec[t], 2);
        next = create_temp(t);
        char op[16];
        strcpy(op, getarr[t]);
        add_Quad(op, create_id(id), expr, next);
    }

    return next;
}

void band_copy_nooff(char *idlhs, char *idrhs, addr *start, addr *end){

    int tl = fetch_type(idlhs);
    int tr = fetch_type(idrhs);
    if (tr < 10)
    {
        char *str = (char *)malloc(50);
        if(tr == -1)snprintf(str, 50, "%s is not defined, Band copy stopped.\n", idrhs);
        else
            snprintf(str, 50, "%s is defined with type %d\n", idrhs, tr);
        yyerror(str);
    }
    else
    {
        if (tl < 10)
        {
            add(idlhs, tr);
            tl = tr;
        }
    }
    tl = tl % 10;
    tr = tr % 10;
    char op_str[16];

    addr *Doffset = create_temp(0);
    strcpy(op_str, "iset");
    add_Quad(op_str, create_int(0), NULL, Doffset);

    start = convert(start, 0);
    addr *Soffset = binary(start, width_vec[tr], 2);
    end = convert(end, 0);
    addr *Slast = binary(end, width_vec[tr], 2);

    int BEGIN = qidx;
    strcpy(op_str, "jmpgt");
    add_Quad(op_str, Soffset, Slast, NULL);

    addr *Sele = create_temp(tr);
    strcpy(op_str, getarr[tr]);
    add_Quad(op_str, create_id(idrhs),Soffset, Sele);

    Sele = convert(Sele, tl);

    strcpy(op_str, strarr[tl]);
    add_Quad(op_str, Sele, Doffset, create_id(idlhs));

    strcpy(op_str, opprnt[0][0]);
    add_Quad(op_str, Doffset, width_vec[tl], Doffset);
    add_Quad(op_str, Soffset, width_vec[tr], Soffset);

    strcpy(op_str, "jmp");
    add_Quad(op_str, NULL, NULL, create_int(BEGIN + 1000));
    QTable[BEGIN]->result = create_int(qidx + 1000);
}
void band_copy(char *idlhs, addr *offset, char *idrhs, addr *start, addr *end){

    int tl = fetch_type(idlhs);
    int tr = fetch_type(idrhs);
    if(tr < 10){
        char *str = (char *)malloc(50);
        snprintf(str, 50, "%s is not defined, Band copy stopped.\n", idrhs);
        yyerror(str);
    }
    else{
        if(tl < 10){
            add(idlhs, tr);
            tl = tr;
        }
    }
    tl = tl % 10;
    tr = tr % 10;

    offset = convert(offset, 0);
    addr *Doffset = binary(offset, width_vec[tl], 2);
    start = convert(start, 0);
    addr *Soffset = binary(start, width_vec[tr], 2);
    end = convert(end, 0);
    addr *Slast = binary(end, width_vec[tr], 2);
    
    int BEGIN = qidx;
    char op_str[16];
    strcpy(op_str, "jmpgt");
    add_Quad(op_str, Soffset, Slast, NULL);

    addr *Sele = create_temp(tr);
    strcpy(op_str, getarr[tr]);
    add_Quad(op_str, create_id(idrhs), Soffset, Sele);

    Sele = convert(Sele, tl);

    strcpy(op_str, strarr[tl]);
    add_Quad(op_str, Sele, Doffset, create_id(idlhs));

    strcpy(op_str, opprnt[0][0]);
    add_Quad(op_str, Doffset, width_vec[tl], Doffset);
    add_Quad(op_str, Soffset, width_vec[tr], Soffset);

    strcpy(op_str, "jmp");
    add_Quad(op_str, NULL, NULL, create_int(BEGIN+1000));
    QTable[BEGIN]->result = create_int(qidx + 1000);
}

nextlist *makelist(int ins){
    nextlist *temp = new nextlist();
    temp->size = 0;
    temp->arr[0] = ins;
    temp->size++;
    return temp;
}

nextlist *merge(nextlist *x, nextlist *y){
    nextlist *temp = new nextlist();
    temp->size = 0;

    if(x != NULL){
        for (int i = 0; i < x->size;i++){
            temp->arr[temp->size++] = x->arr[i];
        }
    }

    if(y != NULL){
        for (int i = 0; i < y->size;i++){
            temp->arr[temp->size++] = y->arr[i];
        }
    }
    return temp;
}

void backpatch(nextlist *l, int idx){
    if(l == NULL)
        return;
    addr *temp = create_int(idx+1000);
    for (int i = 0; i < l->size;i++){
        int curr = l->arr[i];
        QTable[curr]->result = temp;
    }
}

boollist *make_blist(nextlist *truel, nextlist *falsel){
    boollist *curr = new boollist();
    curr->truelist = truel;
    curr->falselist = falsel;
    return curr;
}

void create_jmp(addr *next = NULL){
    QTable[qidx] = new Quad();
    strcpy(QTable[qidx]->op, "jmp");
    QTable[qidx]->arg1 = NULL;
    QTable[qidx]->arg2 = NULL;
    QTable[qidx]->result = next;
    qidx++;
}

void create_expr(addr *expr1, addr *expr2, int relop){

    int target_type = max(expr1->type, expr2->type);
    QTable[qidx] = new Quad();

    snprintf(QTable[qidx]->op, 16, "jmp%c%s",typestr[target_type], cmpstr[relop]);

    QTable[qidx]->arg1 = expr1;
    QTable[qidx]->arg2 = expr2;
    QTable[qidx]->result = NULL;
    qidx++;
}
