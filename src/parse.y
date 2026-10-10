%{
    #include <stdio.h>

    extern int yylex(void);
    void yyerror(const char*);

%}

%union {
    char* ascii_string;
    char ascii_char;
    int signed_integer;
    unsigned int unsigned_integer;
    double floating;
}

%token MATRIX TYPES OPS SUBOPS INFER DEFAULT CUSTOMS
%token ALL FILETYPE INT UINT DOUBLE CHAR STRING
%token FOR INRANGE IF RET
%token INIT ZEROES ENUMERATE RANDOM
%token ASSIGN TRAN APPLY SUM MIN MAX HSTACK VSTACK
%token WRITE STDOUT STDIN

%token COMMENT

%token EQ GT GTE LT LTE 
%token PLUS MINUS MULTIPLY DIVIDE MOD
%token COMMA COLON SEMICOLON LBRACKET RBRACKET TO
%token LPAREN RPAREN LBRACE RBRACE

%token <ascii_string> STRING_LITERAL
%token <ascii_char> CHAR_LITERAL
%token <floating> DOUBLE_LITERAL
%token <signed_integer> INT_LITERAL
%token <unsigned_integer> UINT_LITERAL

%token <ascii_string> IDENTIFIER
%token INVALID_TOKEN

%start root

%%

root: {}
    ;

%%


void yyerror(const char *s) {
    fprintf(stderr, "Syntax error: %s\n", s);
}