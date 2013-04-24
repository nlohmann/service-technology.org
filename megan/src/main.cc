#include <cstdio>       /* for FILE io */
#include "ast-system-k.h"          /* for kc namespace */
#include "ast-system-yystype.h"    /* for flex/bison bridge */
#include "scanner.h"    /* for yyin, yylex_destroy */
#include "parser.h"     /* for yyparse */
#include "ast-system-unpk.h"       /* for unparsers */
#include "ast-system-rk.h"         /* for rewriters */

extern int yyerror(const char *);
kc::property_set root;

void printer(const char *s, kc::uview v)
{
    fprintf(stdout, "%s", s);
}

int main(int argc, char* argv[]) {
    // initialize global variables
    yyin = stdin;
    yyout = stdout;

    // process command line parameters for input and output
    if (argc > 2) {
        yyerror("wrong number of parameters");
    }
    if (argc > 1) {
        yyin = fopen(argv[1], "r");
        if (yyin == NULL) {
            yyerror("cannot open file for reading");
        }
    }

    // start the parser
    yyparse();

    root = root->rewrite(kc::arrows);
    root = root->rewrite(kc::simplify);

    root->print();

    root->unparse(printer, kc::lola);
    //fprintf(yyout, "\n");

    // tidy up
    fclose(yyin);
    yylex_destroy();

    return 0;
}
