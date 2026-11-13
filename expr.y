%{
    #include "defs.h"
    #include<bits/stdc++.h>
    using namespace std;

    extern FILE* yyin;
    extern int yylex();
    extern int qidx;
    void yyerror(const char* msg);
    // int yydebug;
%}

%union{
    int ival;
    double fval;
    char* str;
    addr* node;
    nextlist* nlist;
    boollist* blist;
}



%start PROG
%token<str> ID ERROR
%token<ival> INTCONST 
%token<fval> FLTCONST
%token RATOP TO IF ELSE AND OR EQ NE LT LE GT GE FOR STEP DOWNTO


%left OR
%right AND
%right '!'

%type<nlist> LIST BLOCK STMT N
%type<blist> BOOL
%type ASGN  
%type<node> EXPR TERM FACTOR REAL COMPLEX REALCONST RATIONAL RCOMP 
%type<ival> RELOP M


%% 
PROG    : LIST {backpatch($1,qidx);}
        ;

BLOCK   : '{' LIST '}' {$$ = NULL;backpatch($2,qidx);}
        ;

LIST    : LIST M STMT {
                backpatch($1,$2);
                $$ = $3;
                }
        | {$$ = NULL;}
        ;

STMT    : ASGN {$$ = NULL;}
        | IF '(' BOOL ')' M BLOCK {
                backpatch($3->truelist,$5);
                $$ = merge($3->falselist,$6);
        }
        | IF '(' BOOL ')' M BLOCK N ELSE M BLOCK {
                backpatch($3->truelist,$5);
                backpatch($3->falselist,$9);
                nextlist* temp = merge($6,$7);
                $$ = merge(temp,$10);
        }
        | FOR '(' ID '=' EXPR { // now represents id
                 assign($3,$5);
                 $<node>$ = create_id($3);
                }
         M TO EXPR { // BOOL, M to capture qidx
                int target_type = fetch_type($3);
                addr* end = convert($9,target_type);

                nextlist* tlist = makelist(qidx);
                nextlist *flist = makelist(qidx+1);

                $<blist>$ = make_blist(tlist,flist);
                create_expr($<node>6,end,3);
                create_jmp(NULL);

                }
         M STEP EXPR { // UPDATE
                int target_type = fetch_type($3);
                addr* step = convert($13,target_type);

                addr* temp = binary($<node>6,step,0);
                assign($3,temp);

                $<nlist>$ = makelist(qidx);
                create_jmp(NULL);
                }
         ')' M BLOCK {
                $$ = merge($<blist>10->falselist,$17); // can exit in two ways -> by BOOL being false or through BLOCK

                nextlist* curr = makelist(qidx);
                create_jmp(NULL);

                // backpatch all jmps
                backpatch($<blist>10->truelist,$16);
                backpatch($<nlist>14,$7);
                backpatch(curr,$11);

                }
        | FOR '(' ID '=' EXPR { // now represents id
                 assign($3,$5);
                 $<node>$ = create_id($3);
                }
         M DOWNTO EXPR { // BOOL, M to capture qidx
                int target_type = fetch_type($3);
                addr* end = convert($9,target_type);

                nextlist* tlist = makelist(qidx);
                nextlist *flist = makelist(qidx+1);

                $<blist>$ = make_blist(tlist,flist);
                create_expr($<node>6,end,5);
                create_jmp(NULL);

                }
         M STEP EXPR { // UPDATE
                int target_type = fetch_type($3);
                addr* step = convert($13,target_type);

                addr* temp = binary($<node>6,step,1);
                assign($3,temp);

                $<nlist>$ = makelist(qidx);
                create_jmp(NULL);
                }
         ')' M BLOCK {
                $$ = merge($<blist>10->falselist,$17); // can exit in two ways -> by BOOL being false or through BLOCK

                nextlist* curr = makelist(qidx);
                create_jmp(NULL);

                // backpatch all jmps
                backpatch($<blist>10->truelist,$16);
                backpatch($<nlist>14,$7);
                backpatch(curr,$11);

                }
        ;

N       : {$$ = makelist(qidx);create_jmp(NULL);}
        ;

BOOL    : EXPR RELOP EXPR {

                        int target_type = max($1->type,$3->type);
                        // convert before using qidx
                        addr* x = convert($1,target_type);
                        addr* y = convert($3,target_type);

                        nextlist* tlist = makelist(qidx);
                        nextlist *flist = makelist(qidx+1);
                        $$ = make_blist(tlist,flist);
                        create_expr(x,y,$2);
                        create_jmp(NULL);
                }
        | BOOL OR  M BOOL {
                        backpatch($1->falselist,$3);
                        nextlist *final_tlist = merge($1->truelist,$4->truelist);
                        $$ = make_blist(final_tlist,$4->falselist);
                }
        | BOOL AND M BOOL {
                        backpatch($1->truelist,$3);
                        nextlist* final_flist = merge($1->falselist,$4->falselist);
                        $$ = make_blist($4->truelist,final_flist);
                }
        | '!' BOOL {$$ = make_blist($2->falselist,$2->truelist);}
        | '(' BOOL ')' {$$ = $2;}
        ;

M       : {$$ = qidx;}
        ;

RELOP   : EQ {$$ = 0;}
        | NE {$$ = 1;}
        | LT {$$ = 2;}
        | LE {$$ = 3;}
        | GT {$$ = 4;}
        | GE {$$ = 5;}
        ;


ASGN    : ID '=' EXPR ';' {assign($1,$3);}
        | ID '[' EXPR ']' '=' EXPR ';' {assign_arr($1,$3,$6);}
        | ID '=' ID '[' EXPR TO EXPR ']' ';' {band_copy_nooff($1,$3,$5,$7);}
        | ID '+' EXPR '=' ID '[' EXPR TO EXPR ']' ';' {band_copy($1,$3,$5,$7,$9);}
        ;

EXPR    : EXPR '+' TERM {$$ = binary($1,$3,0);}
        | EXPR '-' TERM {$$ = binary($1,$3,1);}
        | TERM {$$ = $1;}
        ;

TERM    : TERM '*' FACTOR {$$ = binary($1,$3,2);}
        | TERM '/' FACTOR {$$ = binary($1,$3,3);}
        | TERM '%' FACTOR {$$ = binary($1,$3,4);}
        | FACTOR {$$ = $1;}
        ;

FACTOR  : ID {$$ = create_id($1);}
        | '(' EXPR ')' {$$ = $2;}
        | REAL {$$ = $1;}
        | COMPLEX {$$ = $1;}
        | ID '[' EXPR ']' {$$ = create_arr($1,$3);}
        ;

REAL    : REALCONST {$$ = $1;}
        | RATIONAL {$$ = $1;}
        ;
REALCONST : INTCONST {$$ = create_int($1);}
        | FLTCONST {$$ = create_flt($1);}
        ;
RATIONAL    : RCOMP RATOP RCOMP {$$ = create_rtl($1,$3);}
            ;

COMPLEX : RCOMP '@' RCOMP {$$ = create_cpx($1,$3);}
        ;

RCOMP   : REALCONST {$$ = $1;}
        | ID {$$ = create_id($1);}
        | '(' EXPR ')' {$$ = $2;}
        | ID '[' EXPR ']' {$$ = create_arr($1,$3);}
        ;


%%

void yyerror( const char* msg){
    fprintf(stderr, "Error encountered: %s\n",msg);
    exit(1);
}

int main(int argc, char* argv[]){
    if(argc > 1)yyin = (FILE*)fopen("r",argv[1]);
//     yydebug = 1;
    init();
    yyparse();
    final_print();
    fclose(yyin);
    return 0;
}