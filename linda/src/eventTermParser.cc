#include "eventTermParser.h"

/// the parser vector
std::vector<int*>* term_vec = 0;
/// Parser variables
extern int et_yylineno;
extern int et_yydebug;
extern int et_yy_flex_debug;
extern FILE* et_yyin;
extern int et_yyerror();
extern int et_yyparse();

#ifdef YY_FLEX_HAS_YYLEX_DESTROY
extern int et_yylex_destroy();
#endif

/// Convenience function to initialize the parser for event term files.
void initialize_et_parser() {
	et_yylineno = 1;
	et_yydebug = 0;
	et_yy_flex_debug = 0;
}

bool EventTermParser::initialize() {
	initialize_et_parser();
	return true;
}

std::vector<int*>* EventTermParser::parseFile(std::string filename) {

	et_yyin = fopen(filename.c_str(), "r");
	if (!et_yyin) {
		abort(1, "cannot open ET file '%s' for reading", filename.c_str());
	}

	// Initialize the vector that stores the parsed events
	term_vec = new std::vector<int*>();

	// Parse the file
	et_yyparse();
	fclose(et_yyin);

	return term_vec;
}

