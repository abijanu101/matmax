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
customs return CUSTOMS;

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

"//".* return COMMENT;

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

tran return TRAN;
apply return APPLY;
sum return SUM;
min return MIN;
max return MAX;
hstack return HSTACK;
vstack return VSTACK;
"=" return ASSIGN;

write return WRITE;
stdout return STDOUT;
stdin return STDIN;

\"([^\"]|\\\")*\" {
    yylval.ascii_string = strndup(yytext + 1, yyleng - 2);
    return STRING_LITERAL;
}
\'([^\']|\\.)\' {
    if (yyleng == 3) {
        yylval.ascii_char = yytext[1];
        return CHAR_LITERAL;
    }

    if (yytext[1] != '\\')
        return INVALID_TOKEN;

    switch (yytext[2]) {
        case 't': 
            yylval.ascii_char = '\t';
            break;
        case 'n': 
            yylval.ascii_char = '\n';
            break;
        case 'r': 
            yylval.ascii_char = '\r';
            break;
        default: 
            yylval.ascii_char = yytext[2];
    }
    return CHAR_LITERAL;
}
(\+|-)?[0-9]+\.[0-9]+([eE](\+|-)?[0-9]+)? {
    yylval.floating = atof(yytext);
    return DOUBLE_LITERAL;
}
(\+|-)[0-9]+ {
    yylval.signed_integer = atoi(yytext);
    return INT_LITERAL;
}
[0-9]+ {
    yylval.unsigned_integer = atoi(yytext);
    return UINT_LITERAL;
}
[a-zA-Z_][a-zA-Z0-9_]* {
    yylval.ascii_string = strndup(yytext, yyleng);
    return IDENTIFIER;
}

[ \t\r\n]+    { /* ignore whitespace */ }
. {
    fprintf(stderr, "Unexpected character: %s\n", yytext);
    return INVALID_TOKEN;
}

%%

