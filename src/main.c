#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "ast.h"

extern int yyparse(void);
extern FILE* yyin;

/*
 * Usage:
 *   main.out program.matm          parse and print the AST as text
 *   main.out program.matm --dot    parse and print the AST as Graphviz DOT
 */
int main(int argc, char **argv) {
    if (argc < 2 || argc > 3 || (argc == 3 && strcmp(argv[2], "--dot") != 0)) {
        fprintf(stderr, "Usage: %s <source-file> [--dot]\n", argv[0]);
        return 1;
    }
    int want_dot = (argc == 3);

    yyin = fopen(argv[1], "r");
    if (!yyin) {
        fprintf(stderr, "Error: could not open input file '%s': %s\n",
                argv[1], strerror(errno));
        return 1;
    }

    int result = yyparse();      /* runs the lexer + parser + SDT actions */
    fclose(yyin);

    if (result != 0 || !ast_root)
        return 1;                /* yyerror already printed the message  */

    if (want_dot) ast_to_dot(ast_root, stdout);
    else          ast_print(ast_root, 0);

    ast_free(ast_root);
    return 0;
}