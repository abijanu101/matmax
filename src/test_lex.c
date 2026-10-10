#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "../build/parse.h"

extern int yylex(void);
extern int yyparse(void);
extern FILE* yyin;
extern char* yytext;

const char *token_name(int token) {
    switch (token) {
        case MATRIX: return "MATRIX";
        case TYPES: return "TYPES";
        case OPS: return "OPS";
        case SUBOPS: return "SUBOPS";
        case INFER: return "INFER";
        case DEFAULT: return "DEFAULT";
        case CUSTOMS: return "CUSTOMS";

        case ALL: return "ALL";
        case FILETYPE: return "FILETYPE";
        case INT: return "INT";
        case UINT: return "UINT";
        case DOUBLE: return "DOUBLE";
        case CHAR: return "CHAR";
        case STRING: return "STRING";
        
        case COMMENT: return "COMMENT";

        case FOR: return "FOR";
        case INRANGE: return "INRANGE";
        case IF: return "IF";
        case RET: return "RET";

        case INIT: return "INIT";
        case ZEROES: return "ZEROES";
        case ENUMERATE: return "ENUMERATE";
        case RANDOM: return "RANDOM";

        case ASSIGN: return "ASSIGN";
        case TRAN: return "TRAN";
        case APPLY: return "APPLY";
        case SUM: return "SUM";
        case MIN: return "MIN";
        case MAX: return "MAX";
        case HSTACK: return "HSTACK";
        case VSTACK: return "VSTACK";
        case WRITE: return "WRITE";
        case STDOUT: return "STDOUT";
        case STDIN: return "STDIN";

        case EQ: return "EQ";
        case GT: return "GT";
        case GTE: return "GTE";
        case LT: return "LT";
        case LTE: return "LTE";
        case PLUS: return "PLUS";
        case MINUS: return "MINUS";
        case MULTIPLY: return "MULTIPLY";
        case DIVIDE: return "DIVIDE";
        case MOD: return "MOD";

        case COMMA: return "COMMA";
        case COLON: return "COLON";
        case SEMICOLON: return "SEMICOLON";
        case LBRACKET: return "LBRACKET";
        case RBRACKET: return "RBRACKET";
        case TO: return "TO";
        case LPAREN: return "LPAREN";
        case RPAREN: return "RPAREN";
        case LBRACE: return "LBRACE";
        case RBRACE: return "RBRACE";

        case STRING_LITERAL: return "STRING_LITERAL";
        case CHAR_LITERAL: return "CHAR_LITERAL";
        case DOUBLE_LITERAL: return "DOUBLE_LITERAL";
        case INT_LITERAL: return "INT_LITERAL";
        case UINT_LITERAL: return "UINT_LITERAL";
        case IDENTIFIER: return "IDENTIFIER";
        case INVALID_TOKEN: return "INVALID_TOKEN";

        default: return "UNKNOWN_TOKEN";
    }
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <source-file>\n", argv[0]);
        return 1;
    }

    yyin = fopen(argv[1], "r");
    if (!yyin) {
        fprintf(stderr, "Error: could not open input file '%s': %s\n",
                argv[1], strerror(errno));
        return 1;
    }

    int token;
    while ((token = yylex()))
        printf("%s{%s} \n", token_name(token), yytext);
    
    printf("\nWarning: The tokens in the test file are getting string names from a hard-coded mapping, if you see UNKNOWN_TOKENs that means this file is out of date.\n");
    fclose(yyin);
    return 0;
}
