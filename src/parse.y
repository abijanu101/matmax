%{
    #include <stdio.h>

    extern int yylex(void);
    void yyerror(const char*);

%}

%define parse.error verbose

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

/* Needed by numerical.matm ("sum A col") -- add "col" / "row" to lex.l.
   Harmless if the lexer never returns them. */
%token COL ROW

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

/* ------------------------------------------------------------------ */
/* 1. Program                                                          */
/* ------------------------------------------------------------------ */

root
    : program
    ;

program
    : item_list
    | /* empty */                       /* allows empty source files */
    ;

item_list
    : item_list item
    | item
    ;

item
    : matrix_decl
    | statement
    ;

/* The samples write "types: [...]", "if cond: stmt", "for i inrange n: stmt".
   The colon is optional so both the formal CFG style and the sample style work. */
opt_colon
    : /* empty */
    | COLON
    ;

/* ------------------------------------------------------------------ */
/* 2. Matrix definition                                                */
/* ------------------------------------------------------------------ */

matrix_decl
    : MATRIX IDENTIFIER LBRACE
          types_clause ops_clause subops_clause customs_clause_opt
      RBRACE SEMICOLON
    ;

/* types: which cell types the matrix can hold */
types_clause
    : TYPES opt_colon LBRACKET type_list RBRACKET
    ;

type_list
    : type_list COMMA type_name
    | type_name
    ;

type_name
    : INT | UINT | DOUBLE | CHAR | STRING | FILETYPE
    | IDENTIFIER                        /* a user matrix name */
    ;

/* ops: which operations are allowed on the matrix.
   IDENTIFIER covers user-defined ops such as inv / derivative. */
ops_clause
    : OPS opt_colon LBRACKET op_list RBRACKET
    ;

op_list
    : op_list COMMA op_name
    | op_name
    ;

op_name
    : PLUS | MINUS | MULTIPLY | DIVIDE | TRAN
    | IDENTIFIER
    ;

/* ------------------------------------------------------------------ */
/* 3. Subops                                                           */
/* ------------------------------------------------------------------ */

/* subops: how two single cells combine */
subops_clause
    : SUBOPS opt_colon LBRACKET subop_list RBRACKET
    | SUBOPS opt_colon DEFAULT          /* shorthand: default rules for everything */
    ;

subop_list
    : subop_list subop
    | subop
    ;

subop
    /* result type found automatically */
    : type_pat bin_op type_pat TO INFER COLON DEFAULT SEMICOLON
    /* result type given by user */
    | type_pat bin_op type_pat TO INFER COLON type_name SEMICOLON
    /* custom rule with its own code; optional modifier name, e.g. "commute" */
    | type_pat bin_op type_pat TO type_name COLON custom_modifier_opt
          LPAREN param_list RPAREN block
    ;

custom_modifier_opt
    : /* empty */
    | IDENTIFIER
    ;

type_pat
    : ALL                               /* any type */
    | type_name                         /* one type */
    | LBRACKET type_list RBRACKET       /* a group of types */
    ;

bin_op
    : PLUS | MINUS | MULTIPLY | DIVIDE
    ;

/* customs: user-defined operators (unary or binary) with their own code
     derivative Polynomial >> infer: Polynomial: (p) { ... }
     mul PolynomialMatrix int >> PolynomialMatrix: (M, k) { ... }          */
customs_clause_opt
    : /* empty */
    | CUSTOMS opt_colon LBRACKET custom_list RBRACKET
    ;

custom_list
    : custom_list custom
    | custom
    ;

custom
    : IDENTIFIER type_pat TO custom_result COLON LPAREN param_list RPAREN block
    | IDENTIFIER type_pat type_pat TO custom_result COLON LPAREN param_list RPAREN block
    ;

custom_result
    : type_name
    | INFER COLON type_name
    ;

param_list
    : param_list COMMA IDENTIFIER
    | IDENTIFIER
    ;

/* ------------------------------------------------------------------ */
/* 4. Statements                                                       */
/* ------------------------------------------------------------------ */

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

/* declare variables, optionally with an initial value:
     MixedMatrix A, B, C;      int a = 7;      file out = stdout;      */
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

/* assignment:  x = e;   M[i, j] = e;   s += e;
   (lex.l has no "+=" token, so it arrives as PLUS ASSIGN) */
assign_stmt
    : lvalue ASSIGN expr SEMICOLON
    | lvalue PLUS ASSIGN expr SEMICOLON
    ;

lvalue
    : IDENTIFIER
    | IDENTIFIER LBRACKET index COMMA index RBRACKET
    ;

/* fill a matrix */
init_stmt
    : INIT IDENTIFIER init_value SEMICOLON
    ;

init_value
    : ENUMERATE                         /* 0, 1, 2, 3... */
    | RANDOM                            /* random values */
    | ZEROES                            /* all zeros */
    | expr                              /* one value in every cell */
    ;

/* run a function on every cell */
apply_stmt
    : APPLY IDENTIFIER region_opt TO INFER COLON LPAREN IDENTIFIER RPAREN block
    ;

/* print a result */
write_stmt
    : WRITE IDENTIFIER expr SEMICOLON
    ;

/* ------------------------------------------------------------------ */
/* 5. Blocks and control flow                                          */
/* ------------------------------------------------------------------ */

block
    : LBRACE stmt_list RBRACE
    ;

stmt_list
    : stmt_list statement
    | /* empty */
    ;

body
    : statement                         /* single statement, no braces */
    | block                             /* or a block with braces */
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

/* ------------------------------------------------------------------ */
/* 6. Expressions (lowest to highest precedence)                       */
/* ------------------------------------------------------------------ */

expr
    : cmp_expr
    ;

/* comparisons are non-associative: a < b < c is a syntax error */
cmp_expr
    : add_expr
    | add_expr cmp_op add_expr
    ;

cmp_op
    : EQ | GT | GTE | LT | LTE
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

/* matrix commands (vstack / hstack use parentheses + comma to avoid ambiguity) */
prefix_expr
    : TRAN postfix
    | SUM postfix region_opt
    | MIN postfix region_opt
    | MAX postfix region_opt
    | VSTACK LPAREN postfix COMMA postfix RPAREN
    | HSTACK LPAREN postfix COMMA postfix RPAREN
    ;

region_opt
    : ALL                               /* whole matrix */
    | COL                               /* per column */
    | ROW                               /* per row */
    | /* empty */
    ;

/* indexing / slicing:  A[1, 2]   A[1:3, 0:2] */
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
    | LBRACKET expr_list RBRACKET                       /* list of values */
    | ZEROES LPAREN expr COMMA expr RPAREN              /* zero matrix: rows, cols */
    | IDENTIFIER LPAREN arg_list_opt RPAREN             /* function call */
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
