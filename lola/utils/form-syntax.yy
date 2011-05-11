%token COMMA NUMBER NAME EQSIGN EOL REACHABLE INVARIANT POPEN PCLOSE BOPEN BCLOSE BROPEN BRCLOSE NOTSIGN ANDSIGN ORSIGN STARSIGN DOTSIGN CONTAINS EQUALS

%defines

%{
#include <config.h>
#include <cstdio>
#include <string>
#include <cstring>
#include <map>

#define YYDEBUG 1
#define YYERROR_VERBOSE 1

FILE *outfile = NULL;
std::string currentPlace = "";
std::map<unsigned int, unsigned int> mentionedColors;
std::string currentComparison = "";
int instances = 0;
int multiplicity = 0;
int instanceCount = 0;

extern int yylex();
extern int yyerror(const char *);
extern const char* yytext;
%}

%union {
  unsigned int val;
  char *str;
}

%type <val> NUMBER
%type <str> NAME

%%

formulas:
    formula formulas
|   formula
;

formula:
    NAME {
        outfile = fopen((std::string($1) + ".task").c_str(), "w+");
    }
    EQSIGN 
    {
        fprintf(outfile, "FORMULA ");
    }
    quantifier subformula EOL
    {
        fprintf(outfile, ")\n");
        fclose(outfile);
    }
;

quantifier:
    REACHABLE
    {
        fprintf(outfile, "(");
    }
|   INVARIANT
    {
        fprintf(outfile, "(NOT ");
    }
;

subformula:
    atomic
|   POPEN
    {
        fprintf(outfile, "(");
    }
    subformula
    {
        fprintf(outfile, ")");
    }
    PCLOSE andorformula
|   NOTSIGN POPEN
    {
        fprintf(outfile, "(NOT ");
    }
    subformula
    {
        fprintf(outfile, ")");
    }
    PCLOSE andorformula
;

optnot:
    /* empty */
|   NOTSIGN
    {
        fprintf(outfile, "NOT ");
    }
;

andorformula:
    andor optnot POPEN
    {
        fprintf(outfile, "(");
    }
    subformula
    PCLOSE
    {
        fprintf(outfile, ")");
    }
    andorformula
|   /* empty */
;

andor:
    ANDSIGN
    {
        fprintf(outfile, " AND ");
    }
|   ORSIGN
    {
        fprintf(outfile, " OR ");
    }
;

atomic:
    placename containsequals BOPEN BCLOSE
    {
        if (instances > 0) {
        for (int i = 1; i <= instances; ++i) {
            if (instanceCount++ > 0) {
                fprintf(outfile, " AND ");
            }
            fprintf(outfile, "(%s.%d = 0)", currentPlace.c_str(), i);
        }
        mentionedColors.clear();
        instanceCount = 0;
    } else {
        fprintf(outfile, "(%s = 0)", currentPlace.c_str());
    }
    }
|   placename containsequals BOPEN tokens BCLOSE
    {
        if (instances > 0) {
        for (int i = 1; i <= instances; ++i) {
            if (mentionedColors[i] == 0) {
                if (instanceCount++ > 0) {
                    fprintf(outfile, " AND ");
                }
                fprintf(outfile, "(%s.%d = 0)", currentPlace.c_str(), i);
            }
        }
        mentionedColors.clear();
        instanceCount = 0;
    }}
;

placename:
    NAME { currentPlace = $1; }
;

containsequals:
    CONTAINS { currentComparison = ">="; }
|   EQUALS   { currentComparison = "="; }
;

tokens:
    token
|   token COMMA tokens
;

token:
    NUMBER STARSIGN BROPEN DOTSIGN BRCLOSE
    {
        // immediately print low-level token
        fprintf(outfile, "(%s %s %d)", currentPlace.c_str(), currentComparison.c_str(), $1);
    }
|   NUMBER STARSIGN BROPEN { multiplicity = $1; } integers BRCLOSE
;

integers:
    NUMBER
    {
        if (instanceCount++ > 0) {
            fprintf(outfile, " AND ");
        }
        fprintf(outfile, "(%s.%d %s %d)", currentPlace.c_str(), $1, currentComparison.c_str(), multiplicity);
        mentionedColors[$1]++;
    }
|   integers COMMA NUMBER
    {
        if (instanceCount++ > 0) {
            fprintf(outfile, " AND ");
        }
        fprintf(outfile, "(%s.%d %s %d)", currentPlace.c_str(), $3, currentComparison.c_str(), multiplicity);
        mentionedColors[$3]++;
    }
;


%%

int main(int argc, char* argv[]) {
    if (argc == 2 and !strcmp(argv[1], "--help")) {
        printf("usage:\n\tform < properties.formula\n\tform [instancenumber] < properties.formula\n\n");
        return 0;
    }

    if (argc == 2 and !strcmp(argv[1], "--version")) {
        printf("%s\nFormula translator for SUMo 2011 Model Checking Contest\n\n", PACKAGE_STRING);
        return 0;
    }

    instances = (argc == 2) ? atoi(argv[1]) : 0;

    return yyparse();
}
