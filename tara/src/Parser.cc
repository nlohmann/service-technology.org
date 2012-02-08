#include "Parser.h"
#include "verbose.h"

int Parser::parse(const char* filename) {

    // TODO: check if file exists

    *(this->file)=fopen(filename, "r");
    int ret=this->yy_parse();
    
    // TODO: line below causes seg-fault :-(
    // Do we need to do this cleanup stuff or not?
    // this->yy_lex_destroy();

    return ret;
}

extern int graph_parse();
extern int graph_lex_destroy();
extern FILE* graph_in;
Parser Parser::lola=Parser(graph_parse,&graph_in,graph_lex_destroy);

extern int costfunction_parse();
extern int costfunction_lex_destroy();
extern FILE* costfunction_in;
Parser Parser::costfunction=Parser(costfunction_parse,&costfunction_in,costfunction_lex_destroy);


