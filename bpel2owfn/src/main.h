#ifndef MAIN_H
#define MAIN_H

#include <cstdio>
#include <iostream>
#include <fstream>


/* defined by Bison */
extern int yyparse();
extern int yydebug;


/* defined by flex */
extern int yylineno;
extern char *yytext;
extern int yy_flex_debug;
extern FILE *yyin;


/// function to display error messages from lexic and syntactical analysis
int yyerror(const char* msg);


#endif
