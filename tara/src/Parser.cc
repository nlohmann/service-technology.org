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
