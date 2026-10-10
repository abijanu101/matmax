%{
    #include <stdio.h>

    extern int yylex(void);
    void yyerror(const char*);

%}

%token MATRIX TYPES OPS SUBOPS INFER DEFAULT
%token ALL FILETYPE INT UINT DOUBLE CHAR STRING
%token FOR INRANGE IF RET
%token INIT ZEROES ENUMERATE RANDOM
%token ASSIGN TRAN APPLY SUM MIN MAX HSTACK VSTACK WRITE

%token EQ GT GTE LT LTE 
%token PLUS MINUS MULTIPLY DIVIDE MOD
%token COMMA COLON SEMICOLON LBRACKET RBRACKET TO
%token LPAREN RPAREN LBRACE RBRACE

%token STRING_LITERAL CHAR_LITERAL
%token DOUBLE_LITERAL INT_LITERAL

%token IDENTIFIER


%start root

%%

root: {}
    ;

%%


void yyerror(const char *s) {
    fprintf(stderr, "Syntax error: %s\n", s);
}