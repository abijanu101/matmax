/*
 * matmax parser + syntax-directed translation (SDT) to an AST.
 *
 * Every grammar rule has an action { ... } that runs when bison REDUCES
 * that rule. The action builds an AST node out of the values of the
 * symbols on the right-hand side ($1, $2, ...) and stores it in $$.
 * Bison parses bottom-up, so the children always exist before the parent.
 *
 * Rules of thumb used everywhere below:
 *   1. Punctuation ( ; , : { } ( ) [ ] >> ) never becomes a node.
 *   2. Single-child chains (expr -> cmp_expr -> add_expr -> ... -> primary)
 *      just pass the child up:  { $$ = $1; }
 *   3. Left-recursive lists become ONE node with N children:
 *         first item   -> create the list node with 1 child
 *         every other  -> ast_add() the new item to the existing list
 *   4. Operators/keywords become the node's label (BINOP "+", BUILTIN "sum").
 *   5. IDENTIFIER and STRING_LITERAL strings are strdup'd by the lexer.
 *      ast_new() makes its own copy, so we free() the lexer's copy.
 */

%code requires {
    /* goes into parse.h, so the lexer also knows what an ASTNode is */
    #include "ast.h"
}

%{
#include <stdio.h>
#include <stdlib.h>
#include "ast.h"

extern int yylex(void);
extern int yylineno;
void yyerror(const char *s);

ASTNode *ast_root = NULL;   /* the finished tree, read by main.c */
%}

%define parse.error verbose

%union {
    char *ascii_string;
    char ascii_char;
    int signed_integer;
    unsigned int unsigned_integer;
    double floating;

    struct ASTNode *node;   /* value of almost every non-terminal          */
    const char *op_text;    /* constant operator text like "+" or "==",
                               used by bin_op / cmp_op, never freed         */
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

/* which %union field each non-terminal's value ($$) lives in */
%type <node> program item_list item
%type <node> matrix_decl types_clause type_list type_name
%type <node> ops_clause op_list op_name
%type <node> subops_clause subop_list subop type_pat
%type <node> customs_clause_opt custom_list custom custom_result param_list
%type <node> statement decl decl_list decl_item assign_stmt lvalue
%type <node> init_stmt init_value apply_stmt write_stmt
%type <node> block stmt_list body if_stmt for_stmt ret_stmt
%type <node> expr cmp_expr add_expr mul_expr unary prefix_expr region_opt
%type <node> postfix index primary expr_list arg_list_opt
%type <op_text> bin_op cmp_op
%type <ascii_string> custom_modifier_opt

/* On a syntax error bison throws away half-built pieces; these tell it
   how to free them so nothing leaks. (Not called on a successful parse.) */
%destructor { ast_free($$); } <node>
%destructor { free($$); }     <ascii_string>

%start root

%%

/* ================================================================ */
/*  Program                                                         */
/* ================================================================ */

root
    : program                       { ast_root = $1; }
    ;

program
    : item_list                     { $$ = $1; }
    | %empty                        { $$ = ast_new(N_PROGRAM, NULL); }
    ;

item_list
    : item_list item                { $$ = ast_add($1, $2); }
    | item                          { $$ = ast_make(N_PROGRAM, NULL, 1, $1); }
    ;

item
    : matrix_decl                   { $$ = $1; }
    | statement                     { $$ = $1; }
    ;

opt_colon
    : %empty
    | COLON
    ;

/* ================================================================ */
/*  Matrix type definition                                          */
/*                                                                  */
/*  matrix A { types.. ops.. subops.. customs.. };                  */
/*       ==>  MATRIX_DEF(A)                                         */
/*              |- TYPE_LIST                                        */
/*              |- OP_LIST                                          */
/*              |- SUBOP_LIST                                       */
/*              `- CUSTOM_LIST   (empty when there is no customs)   */
/* ================================================================ */

matrix_decl
    : MATRIX IDENTIFIER LBRACE
      types_clause ops_clause subops_clause customs_clause_opt
      RBRACE SEMICOLON
        {
            $$ = ast_make(N_MATRIX_DEF, $2, 4, $4, $5, $6, $7);
            free($2);
        }
    ;

types_clause
    : TYPES opt_colon LBRACKET type_list RBRACKET   { $$ = $4; }
    ;

type_list
    : type_list COMMA type_name     { $$ = ast_add($1, $3); }
    | type_name                     { $$ = ast_make(N_TYPE_LIST, NULL, 1, $1); }
    ;

type_name
    : INT                           { $$ = ast_new(N_TYPE, "int"); }
    | UINT                          { $$ = ast_new(N_TYPE, "uint"); }
    | DOUBLE                        { $$ = ast_new(N_TYPE, "double"); }
    | CHAR                          { $$ = ast_new(N_TYPE, "char"); }
    | STRING                        { $$ = ast_new(N_TYPE, "string"); }
    | FILETYPE                      { $$ = ast_new(N_TYPE, "file"); }
    | IDENTIFIER                    { $$ = ast_new(N_TYPE, $1); free($1); }
    ;

ops_clause
    : OPS opt_colon LBRACKET op_list RBRACKET       { $$ = $4; }
    ;

op_list
    : op_list COMMA op_name         { $$ = ast_add($1, $3); }
    | op_name                       { $$ = ast_make(N_OP_LIST, NULL, 1, $1); }
    ;

op_name
    : PLUS                          { $$ = ast_new(N_OP, "+"); }
    | MINUS                         { $$ = ast_new(N_OP, "-"); }
    | MULTIPLY                      { $$ = ast_new(N_OP, "*"); }
    | DIVIDE                        { $$ = ast_new(N_OP, "/"); }
    | TRAN                          { $$ = ast_new(N_OP, "tran"); }
    | IDENTIFIER                    { $$ = ast_new(N_OP, $1); free($1); }
    ;

subops_clause
    : SUBOPS opt_colon LBRACKET subop_list RBRACKET { $$ = $4; }
    | SUBOPS opt_colon DEFAULT      { $$ = ast_new(N_SUBOP_LIST, "default"); }
    ;

subop_list
    : subop_list subop              { $$ = ast_add($1, $2); }
    | subop                         { $$ = ast_make(N_SUBOP_LIST, NULL, 1, $1); }
    ;

/*  L op R >> infer: default;   ==>  SUBOP(op): L, R, INFER_DEFAULT
 *  L op R >> infer: T;         ==>  SUBOP(op): L, R, INFER(T)
 *  L op R >> T: mod (a,b) {..} ==>  SUBOP(op): L, R, TYPE(T), FUNC(mod)  */
subop
    : type_pat bin_op type_pat TO INFER COLON DEFAULT SEMICOLON
        {
            $$ = ast_make(N_SUBOP, $2, 3, $1, $3,
                          ast_new(N_INFER_DEFAULT, NULL));
        }
    | type_pat bin_op type_pat TO INFER COLON type_name SEMICOLON
        {
            $$ = ast_make(N_SUBOP, $2, 3, $1, $3,
                          ast_make(N_INFER, NULL, 1, $7));
        }
    | type_pat bin_op type_pat TO type_name COLON
      custom_modifier_opt LPAREN param_list RPAREN block
        {
            $$ = ast_make(N_SUBOP, $2, 4, $1, $3, $5,
                          ast_make(N_FUNC, $7, 2, $9, $11));
            free($7);   /* free(NULL) is fine when there is no modifier */
        }
    ;

custom_modifier_opt
    : %empty                        { $$ = NULL; }
    | IDENTIFIER                    { $$ = $1; }
    ;

/* a type pattern is one of: ALL, a single TYPE, or a TYPE_LIST */
type_pat
    : ALL                           { $$ = ast_new(N_ALL, NULL); }
    | type_name                     { $$ = $1; }
    | LBRACKET type_list RBRACKET   { $$ = $2; }
    ;

bin_op
    : PLUS                          { $$ = "+"; }
    | MINUS                         { $$ = "-"; }
    | MULTIPLY                      { $$ = "*"; }
    | DIVIDE                        { $$ = "/"; }
    ;

customs_clause_opt
    : %empty                        { $$ = ast_new(N_CUSTOM_LIST, NULL); }
    | CUSTOMS opt_colon LBRACKET custom_list RBRACKET { $$ = $4; }
    ;

custom_list
    : custom_list custom            { $$ = ast_add($1, $2); }
    | custom                        { $$ = ast_make(N_CUSTOM_LIST, NULL, 1, $1); }
    ;

/*  name T >> R: (p) {..}     ==>  CUSTOM(name): ARG_TYPES(T),   R, FUNC
 *  name T U >> R: (p,q) {..} ==>  CUSTOM(name): ARG_TYPES(T,U), R, FUNC  */
custom
    : IDENTIFIER type_pat TO custom_result COLON
      LPAREN param_list RPAREN block
        {
            $$ = ast_make(N_CUSTOM, $1, 3,
                          ast_make(N_ARG_TYPES, NULL, 1, $2),
                          $4,
                          ast_make(N_FUNC, NULL, 2, $7, $9));
            free($1);
        }
    | IDENTIFIER type_pat type_pat TO custom_result COLON
      LPAREN param_list RPAREN block
        {
            $$ = ast_make(N_CUSTOM, $1, 3,
                          ast_make(N_ARG_TYPES, NULL, 2, $2, $3),
                          $5,
                          ast_make(N_FUNC, NULL, 2, $8, $10));
            free($1);
        }
    ;

custom_result
    : type_name                     { $$ = $1; }
    | INFER COLON type_name         { $$ = ast_make(N_INFER, NULL, 1, $3); }
    ;

param_list
    : param_list COMMA IDENTIFIER
        { $$ = ast_add($1, ast_new(N_ID, $3)); free($3); }
    | IDENTIFIER
        { $$ = ast_make(N_PARAMS, NULL, 1, ast_new(N_ID, $1)); free($1); }
    ;

/* ================================================================ */
/*  Statements                                                      */
/* ================================================================ */

statement
    : decl                          { $$ = $1; }
    | assign_stmt                   { $$ = $1; }
    | init_stmt                     { $$ = $1; }
    | apply_stmt                    { $$ = $1; }
    | write_stmt                    { $$ = $1; }
    | if_stmt                       { $$ = $1; }
    | for_stmt                      { $$ = $1; }
    | ret_stmt                      { $$ = $1; }
    ;

/*  MixedMatrix A, B = x;  ==>  DECL: TYPE(MixedMatrix), DECL_VAR(A), DECL_VAR(B): x
 *  decl_list is reduced before decl, so the DECL node already holds the
 *  variables when we get here; we put the type in front of them.        */
decl
    : type_name decl_list SEMICOLON { $$ = ast_prepend($2, $1); }
    ;

decl_list
    : decl_list COMMA decl_item     { $$ = ast_add($1, $3); }
    | decl_item                     { $$ = ast_make(N_DECL, NULL, 1, $1); }
    ;

decl_item
    : IDENTIFIER
        { $$ = ast_new(N_DECL_VAR, $1); free($1); }
    | IDENTIFIER ASSIGN expr
        { $$ = ast_make(N_DECL_VAR, $1, 1, $3); free($1); }
    ;

assign_stmt
    : lvalue ASSIGN expr SEMICOLON      { $$ = ast_make(N_ASSIGN, "=",  2, $1, $3); }
    | lvalue PLUS ASSIGN expr SEMICOLON { $$ = ast_make(N_ASSIGN, "+=", 2, $1, $4); }
    ;

lvalue
    : IDENTIFIER
        { $$ = ast_new(N_ID, $1); free($1); }
    | IDENTIFIER LBRACKET index COMMA index RBRACKET
        { $$ = ast_make(N_INDEX, NULL, 3, ast_new(N_ID, $1), $3, $5); free($1); }
    ;

init_stmt
    : INIT IDENTIFIER init_value SEMICOLON
        { $$ = ast_make(N_INIT, $2, 1, $3); free($2); }
    ;

init_value
    : ENUMERATE                     { $$ = ast_new(N_INIT_MODE, "enumerate"); }
    | RANDOM                        { $$ = ast_new(N_INIT_MODE, "random"); }
    | ZEROES                        { $$ = ast_new(N_INIT_MODE, "zeros"); }
    | expr                          { $$ = $1; }
    ;

/*  apply A all >> infer:(old) {..}  ==>  APPLY(A): REGION(all), FUNC: PARAMS(old), BLOCK
 *  Same FUNC shape as in subops and customs, so they can be handled alike. */
apply_stmt
    : APPLY IDENTIFIER region_opt TO INFER COLON
      LPAREN IDENTIFIER RPAREN block
        {
            ASTNode *params = ast_make(N_PARAMS, NULL, 1, ast_new(N_ID, $8));
            $$ = ast_make(N_APPLY, $2, 2, $3,
                          ast_make(N_FUNC, NULL, 2, params, $10));
            free($2);
            free($8);
        }
    ;

write_stmt
    : WRITE IDENTIFIER expr SEMICOLON
        { $$ = ast_make(N_WRITE, $2, 1, $3); free($2); }
    ;

block
    : LBRACE stmt_list RBRACE       { $$ = $2; }
    ;

stmt_list
    : stmt_list statement           { $$ = ast_add($1, $2); }
    | %empty                        { $$ = ast_new(N_BLOCK, NULL); }
    ;

/* a body is ALWAYS a BLOCK in the AST, even a one-line body like
   "if x: ret 1;", so later stages only have to handle one case */
body
    : statement                     { $$ = ast_make(N_BLOCK, NULL, 1, $1); }
    | block                         { $$ = $1; }
    ;

if_stmt
    : IF expr opt_colon body        { $$ = ast_make(N_IF, NULL, 2, $2, $4); }
    ;

for_stmt
    : FOR IDENTIFIER INRANGE expr opt_colon body
        { $$ = ast_make(N_FOR, $2, 2, $4, $6); free($2); }
    ;

ret_stmt
    : RET expr SEMICOLON            { $$ = ast_make(N_RETURN, NULL, 1, $2); }
    ;

/* ================================================================ */
/*  Expressions                                                     */
/*  The grammar levels (cmp > add > mul > unary) already encode     */
/*  precedence, so the tree shape is right; we just label nodes.   */
/* ================================================================ */

expr
    : cmp_expr                      { $$ = $1; }
    ;

cmp_expr
    : add_expr                      { $$ = $1; }
    | add_expr cmp_op add_expr      { $$ = ast_make(N_BINOP, $2, 2, $1, $3); }
    ;

cmp_op
    : EQ                            { $$ = "=="; }
    | GT                            { $$ = ">"; }
    | GTE                           { $$ = ">="; }
    | LT                            { $$ = "<"; }
    | LTE                           { $$ = "<="; }
    ;

add_expr
    : add_expr PLUS mul_expr        { $$ = ast_make(N_BINOP, "+", 2, $1, $3); }
    | add_expr MINUS mul_expr       { $$ = ast_make(N_BINOP, "-", 2, $1, $3); }
    | mul_expr                      { $$ = $1; }
    ;

mul_expr
    : mul_expr MULTIPLY unary       { $$ = ast_make(N_BINOP, "*", 2, $1, $3); }
    | mul_expr DIVIDE unary         { $$ = ast_make(N_BINOP, "/", 2, $1, $3); }
    | mul_expr MOD unary            { $$ = ast_make(N_BINOP, "%", 2, $1, $3); }
    | unary                         { $$ = $1; }
    ;

unary
    : MINUS unary                   { $$ = ast_make(N_NEG, NULL, 1, $2); }
    | prefix_expr                   { $$ = $1; }
    | postfix                       { $$ = $1; }
    ;

prefix_expr
    : TRAN postfix                  { $$ = ast_make(N_BUILTIN, "tran", 1, $2); }
    | SUM postfix region_opt        { $$ = ast_make(N_BUILTIN, "sum",  2, $2, $3); }
    | MIN postfix region_opt        { $$ = ast_make(N_BUILTIN, "min",  2, $2, $3); }
    | MAX postfix region_opt        { $$ = ast_make(N_BUILTIN, "max",  2, $2, $3); }
    | LEN LPAREN expr RPAREN        { $$ = ast_make(N_BUILTIN, "len",  1, $3); }
    | ROWS LPAREN expr RPAREN       { $$ = ast_make(N_BUILTIN, "rows", 1, $3); }
    | COLS LPAREN expr RPAREN       { $$ = ast_make(N_BUILTIN, "cols", 1, $3); }
    | VSTACK LPAREN expr COMMA expr RPAREN
                                    { $$ = ast_make(N_BUILTIN, "vstack", 2, $3, $5); }
    | HSTACK LPAREN expr COMMA expr RPAREN
                                    { $$ = ast_make(N_BUILTIN, "hstack", 2, $3, $5); }
    ;

/* no region written ("sum A") is stored as REGION(all), since the
   samples use "sum A" and "sum A all" for the same thing */
region_opt
    : ALL                           { $$ = ast_new(N_REGION, "all"); }
    | COL                           { $$ = ast_new(N_REGION, "col"); }
    | ROW                           { $$ = ast_new(N_REGION, "row"); }
    | %empty                        { $$ = ast_new(N_REGION, "all"); }
    ;

postfix
    : primary                       { $$ = $1; }
    | postfix LBRACKET index COMMA index RBRACKET
                                    { $$ = ast_make(N_INDEX, NULL, 3, $1, $3, $5); }
    ;

index
    : expr                          { $$ = $1; }
    | expr COLON expr               { $$ = ast_make(N_RANGE, NULL, 2, $1, $3); }
    ;

primary
    : INT_LITERAL                   { $$ = ast_int($1); }
    | UINT_LITERAL                  { $$ = ast_uint($1); }
    | DOUBLE_LITERAL                { $$ = ast_double($1); }
    | STRING_LITERAL                { $$ = ast_new(N_STRING, $1); free($1); }
    | CHAR_LITERAL                  { $$ = ast_char($1); }
    | IDENTIFIER                    { $$ = ast_new(N_ID, $1); free($1); }
    | STDOUT                        { $$ = ast_new(N_STREAM, "stdout"); }
    | STDIN                         { $$ = ast_new(N_STREAM, "stdin"); }
    | LPAREN expr RPAREN            { $$ = $2; }   /* brackets vanish */
    | LBRACKET expr_list RBRACKET   { $$ = $2; }   /* already a MATRIX_LIT */
    | ZEROES LPAREN expr COMMA expr RPAREN
                                    { $$ = ast_make(N_BUILTIN, "zeros", 2, $3, $5); }
    | IDENTIFIER LPAREN arg_list_opt RPAREN
        {
            /* reuse the list node that holds the arguments,
               just relabel it as a CALL and give it the name */
            $$ = $3;
            $$->kind = N_CALL;
            $$->text = $1;      /* take the lexer's string, no copy needed */
        }
    ;

/* expr_list is used by [a, b, c] and by f(a, b, c).
   It always builds a MATRIX_LIT; a function call relabels it to CALL. */
expr_list
    : expr_list COMMA expr          { $$ = ast_add($1, $3); }
    | expr                          { $$ = ast_make(N_MATRIX_LIT, NULL, 1, $1); }
    ;

arg_list_opt
    : expr_list                     { $$ = $1; }
    | %empty                        { $$ = ast_new(N_MATRIX_LIT, NULL); }
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Line %d: syntax error: %s\n", yylineno, s);
}