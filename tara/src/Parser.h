
#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <iostream>
#include <stdio.h>

/** simple wrapper class for bison/yacc parser
 * yy_parse, yy_in, yy_lex_destroy are passed as (function) pointers
 */
class Parser {
    public:
        Parser( int (*_yy_parse)(),  FILE** _file, int (*_yy_lex_destroy)())
            : yy_parse(_yy_parse), file(_file), yy_lex_destroy(_yy_lex_destroy) { };

       int parse(const char* filename);

       /// Parser object for Lola output
       static Parser lola;

       /// Parser object for costfunction
       static Parser costfunction;
    private:

       /// pointer to parse function from yacc/bison file
       int (*yy_parse) ();

       /// The file pointer from yacc/bison file
       FILE** file;

       /// the lex destroy function
       int (*yy_lex_destroy)();

};

#endif
