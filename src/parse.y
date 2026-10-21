
%{
#include <stdio.h>

extern int yylex(void);
void yyerror(const char *s);
%}

%define parse.error verbose

%union {
    char *ascii_string;
    char ascii_char;
    int signed_integer;
    unsigned int unsigned_integer;
    double floating;
}

%token MATRIX TYPES OPS SUBOPS INFER DEFAULT CUSTOMS
%token ALL FILETYPE INT UINT DOUBLE CHAR STRING
%token FOR INRANGE IF RET
%token INIT ZEROES ENUMERATE RANDOM
%token ASSIGN TRAN APPLY SUM MIN MAX LEN ROWS COLS HSTACK VSTACK
%token COL ROW
%token WRITE STDOUT STDIN

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

root
    : program
    ;

program
    : item_list
    | /* empty */
    ;

item_list
    : item_list item
    | item
    ;

item
    : matrix_decl
    | statement
    ;

opt_colon
    : /* empty */
    | COLON
    ;

matrix_decl
    : MATRIX IDENTIFIER LBRACE
      types_clause ops_clause subops_clause customs_clause_opt
      RBRACE SEMICOLON
    ;

types_clause
    : TYPES opt_colon LBRACKET type_list RBRACKET
    ;

type_list
    : type_list COMMA type_name
    | type_name
    ;

type_name
    : INT
    | UINT
    | DOUBLE
    | CHAR
    | STRING
    | FILETYPE
    | IDENTIFIER
    ;

ops_clause
    : OPS opt_colon LBRACKET op_list RBRACKET
    ;

op_list
    : op_list COMMA op_name
    | op_name
    ;

op_name
    : PLUS
    | MINUS
    | MULTIPLY
    | DIVIDE
    | TRAN
    | IDENTIFIER
    ;

subops_clause
    : SUBOPS opt_colon LBRACKET subop_list RBRACKET
    | SUBOPS opt_colon DEFAULT
    ;

subop_list
    : subop_list subop
    | subop
    ;

subop
    : type_pat bin_op type_pat TO INFER COLON DEFAULT SEMICOLON
    | type_pat bin_op type_pat TO INFER COLON type_name SEMICOLON
    | type_pat bin_op type_pat TO type_name COLON
      custom_modifier_opt LPAREN param_list RPAREN block
    ;

custom_modifier_opt
    : /* empty */
    | IDENTIFIER
    ;

type_pat
    : ALL
    | type_name
    | LBRACKET type_list RBRACKET
    ;

bin_op
    : PLUS
    | MINUS
    | MULTIPLY
    | DIVIDE
    ;

customs_clause_opt
    : /* empty */
    | CUSTOMS opt_colon LBRACKET custom_list RBRACKET
    ;

custom_list
    : custom_list custom
    | custom
    ;

custom
    : IDENTIFIER type_pat TO custom_result COLON
      LPAREN param_list RPAREN block
    | IDENTIFIER type_pat type_pat TO custom_result COLON
      LPAREN param_list RPAREN block
    ;

custom_result
    : type_name
    | INFER COLON type_name
    ;

param_list
    : param_list COMMA IDENTIFIER
    | IDENTIFIER
    ;

statement
    : decl
    | assign_stmt
    | init_stmt
    | apply_stmt
    | write_stmt
    | if_stmt
    | for_stmt
    | ret_stmt
    ;

decl
    : type_name decl_list SEMICOLON
    ;

decl_list
    : decl_list COMMA decl_item
    | decl_item
    ;

decl_item
    : IDENTIFIER
    | IDENTIFIER ASSIGN expr
    ;

assign_stmt
    : lvalue ASSIGN expr SEMICOLON
    | lvalue PLUS ASSIGN expr SEMICOLON
    ;

lvalue
    : IDENTIFIER
    | IDENTIFIER LBRACKET index COMMA index RBRACKET
    ;

init_stmt
    : INIT IDENTIFIER init_value SEMICOLON
    ;

init_value
    : ENUMERATE
    | RANDOM
    | ZEROES
    | expr
    ;

apply_stmt
    : APPLY IDENTIFIER region_opt TO INFER COLON
      LPAREN IDENTIFIER RPAREN block
    ;

write_stmt
    : WRITE IDENTIFIER expr SEMICOLON
    ;

block
    : LBRACE stmt_list RBRACE
    ;

stmt_list
    : stmt_list statement
    | /* empty */
    ;

body
    : statement
    | block
    ;

if_stmt
    : IF expr opt_colon body
    ;

for_stmt
    : FOR IDENTIFIER INRANGE expr opt_colon body
    ;

ret_stmt
    : RET expr SEMICOLON
    ;

expr
    : cmp_expr
    ;

cmp_expr
    : add_expr
    | add_expr cmp_op add_expr
    ;

cmp_op
    : EQ
    | GT
    | GTE
    | LT
    | LTE
    ;

add_expr
    : add_expr PLUS mul_expr
    | add_expr MINUS mul_expr
    | mul_expr
    ;

mul_expr
    : mul_expr MULTIPLY unary
    | mul_expr DIVIDE unary
    | mul_expr MOD unary
    | unary
    ;

unary
    : MINUS unary
    | prefix_expr
    | postfix
    ;

prefix_expr
    : TRAN postfix
    | SUM postfix region_opt
    | MIN postfix region_opt
    | MAX postfix region_opt
    | LEN LPAREN expr RPAREN
    | ROWS LPAREN expr RPAREN
    | COLS LPAREN expr RPAREN
    | VSTACK LPAREN expr COMMA expr RPAREN
    | HSTACK LPAREN expr COMMA expr RPAREN
    ;

region_opt
    : ALL
    | COL
    | ROW
    | /* empty */
    ;

postfix
    : primary
    | postfix LBRACKET index COMMA index RBRACKET
    ;

index
    : expr
    | expr COLON expr
    ;

primary
    : INT_LITERAL
    | UINT_LITERAL
    | DOUBLE_LITERAL
    | STRING_LITERAL
    | CHAR_LITERAL
    | IDENTIFIER
    | STDOUT
    | STDIN
    | LPAREN expr RPAREN
    | LBRACKET expr_list RBRACKET
    | ZEROES LPAREN expr COMMA expr RPAREN
    | IDENTIFIER LPAREN arg_list_opt RPAREN
    ;

expr_list
    : expr_list COMMA expr
    | expr
    ;

arg_list_opt
    : expr_list
    | /* empty */
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Syntax error: %s\n", s);
}
