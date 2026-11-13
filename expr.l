%{
    #include "defs.h"
    #include "y.tab.h"
    #include<bits/stdc++.h>
    using namespace std;

%}

D [0-9]
validname   [a-zA-Z_][a-zA-Z0-9_]*
intconst    {D}+
exp         [eE]D+
fltconst    {D}+"."({D}+)?({exp})?
ws          [ \t\n\r]
%%

"for"   {return FOR;}
"step"  {return STEP;}
"downto"    {return DOWNTO;}
"if"    {return IF;}
"else"  {return ELSE;}
"to"    {return TO;}
{validname}  {yylval.str = strdup(yytext);return ID;}
"="     {return yytext[0];}
";"     {return yytext[0];}
"+"     {return yytext[0];}
"-"     {return yytext[0];}
"*"     {return yytext[0];}
"/"     {return yytext[0];}
"%"     {return yytext[0];}
"("     {return yytext[0];}
")"     {return yytext[0];}
"["     {return yytext[0];}
"]"     {return yytext[0];}
"{"     {return yytext[0];}
"}"     {return yytext[0];}
"//"    {return RATOP;}
"||"    {return OR;}
"&&"    {return AND;}
"!"     {return yytext[0];}    
"=="    {return EQ;}
"!="    {return NE;}
"<"     {return LT;}
"<="    {return LE;}
">"     {return GT;}
">="    {return GE;}
"@"     {return yytext[0];}
{intconst}  {yylval.ival = atoi(yytext);return INTCONST;}
{fltconst}  {yylval.fval = atof(yytext);return FLTCONST;}
{ws}        {}
.       {yylval.str = strdup(yytext);return ERROR;}
%%

int yywrap(){return 1;}
