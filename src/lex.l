%{
    #include "parse.h"
%}

%option noyywrap
%option nounput
%option noinput

%%

matrix return MATRIX;
types return TYPES;
ops return OPS;
subops return SUBOPS;
infer return INFER;
default return DEFAULT;

all return ALL;
file return FILETYPE;
int return INT;
uint return UINT;
double return DOUBLE;
char return CHAR;
string return STRING;

for return FOR;
inrange return INRANGE;
if return IF;
ret return RET;

init return INIT;
zeroes return ZEROES;
enumerate return ENUMERATE;
random return RANDOM;

"," return COMMA;
":" return COLON;
">>" return TO;
";" return SEMICOLON;
"[" return LBRACKET;
"]" return RBRACKET;
"(" return LPAREN;
")" return RPAREN;
"{" return LBRACE;
"}" return RBRACE;

tran return TRAN;
apply return APPLY;
sum return SUM;
min return MIN;
max return MAX;
hstack return HSTACK;
vstack return VSTACK;
write return WRITE;
"=" return ASSIGN;

"+" return PLUS;
"-" return MINUS;
"*" return MULTIPLY;
"/" return DIVIDE;
"%" return MOD;
"==" return EQ;
">" return GT;
">=" return GTE;
"<" return LT;
"<=" return LTE;


"\""[^\"]*"\"" return STRING_LITERAL;
"\'"[^\']"\'" return CHAR_LITERAL;
(\+|-)?[0-9]+\.[0-9]+(e(\+|-)?[0-9]+)? return DOUBLE_LITERAL;
[0-9]+ return INT_LITERAL;

[a-zA-Z_][a-zA-Z0-9_]* return IDENTIFIER;

[ \t\r\n]+    { /* ignore whitespace */ }
. {
    fprintf(stderr, "Unexpected character: %s\n", yytext);
}

%%

