
%{
#include "parse.h"   /* bison -d -o build/parse.c writes build/parse.h */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
%}

%option noyywrap
%option nounput
%option noinput
%option yylineno

%%

matrix      return MATRIX;
types       return TYPES;
ops         return OPS;
subops      return SUBOPS;
infer       return INFER;
default     return DEFAULT;
customs     return CUSTOMS;

all         return ALL;
file        return FILETYPE;
int         return INT;
uint        return UINT;
double      return DOUBLE;
char        return CHAR;
string      return STRING;

for         return FOR;
inrange     return INRANGE;
if          return IF;
ret         return RET;

init        return INIT;
zeros       return ZEROES;
enumerate   return ENUMERATE;
random      return RANDOM;

tran        return TRAN;
apply       return APPLY;
sum         return SUM;
min         return MIN;
max         return MAX;
len         return LEN;
rows        return ROWS;
cols        return COLS;
col         return COL;
row         return ROW;
hstack      return HSTACK;
vstack      return VSTACK;

write       return WRITE;
stdout      return STDOUT;
stdin       return STDIN;

"//".*      { }

">>"        return TO;
"=="        return EQ;
">="        return GTE;
"<="        return LTE;

","         return COMMA;
":"         return COLON;
";"         return SEMICOLON;
"["         return LBRACKET;
"]"         return RBRACKET;
"("         return LPAREN;
")"         return RPAREN;
"{"         return LBRACE;
"}"         return RBRACE;

"+"         return PLUS;
"-"         return MINUS;
"*"         return MULTIPLY;
"/"         return DIVIDE;
"%"         return MOD;
">"         return GT;
"<"         return LT;
"="         return ASSIGN;

\"([^\\\"]|\\.)*\" {
    yylval.ascii_string = strndup(yytext + 1, yyleng - 2);
    return STRING_LITERAL;
}

\'([^\\']|\\.)\' {
    if (yyleng == 3) {
        yylval.ascii_char = yytext[1];
        return CHAR_LITERAL;
    }

    switch (yytext[2]) {
        case 't': yylval.ascii_char = '\t'; break;
        case 'n': yylval.ascii_char = '\n'; break;
        case 'r': yylval.ascii_char = '\r'; break;
        default:  yylval.ascii_char = yytext[2]; break;
    }

    return CHAR_LITERAL;
}

[+-]?[0-9]+\.[0-9]+([eE][+-]?[0-9]+)? {
    yylval.floating = atof(yytext);
    return DOUBLE_LITERAL;
}

[+-][0-9]+ {
    yylval.signed_integer = atoi(yytext);
    return INT_LITERAL;
}

[0-9]+ {
    yylval.unsigned_integer = (unsigned int)atoi(yytext);
    return UINT_LITERAL;
}

[a-zA-Z_][a-zA-Z0-9_]* {
    yylval.ascii_string = strndup(yytext, yyleng);
    return IDENTIFIER;
}

[ \t\r\n]+  { }

. {
    fprintf(stderr, "Unexpected character: %s\n", yytext);
    return INVALID_TOKEN;
}

%%
