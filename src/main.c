#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

extern int yylex(void);
extern int yyparse(void);
extern FILE* yyin;

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

    int lastval = -1;
    do {
        lastval = yylex();
        printf("%d ", lastval);
    } while (lastval != 0);
    fclose(yyin);
    printf("Scan completed\n");
    return 0;
}
