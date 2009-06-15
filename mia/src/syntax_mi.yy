%token KW_STATE KW_PROG COLON DOT COMMA ARROW NUMBER NAME TGT_NAME MPP_NAME

%defines
%name-prefix="mi_"

%{
#include <string>
#include <map>

#define YYDEBUG 1
#define YYERROR_VERBOSE 1

std::map<unsigned int, std::string> id2marking;
std::string temp;

/// the current NAME token as string
extern std::string NAME_token;

extern int mi_lex();
extern int mi_error(const char *);
extern const char* mi_text;
%}

%union {
  unsigned int val;
}

%type <val> NUMBER

%%

markings:
  marking
| markings marking
;

marking:
  NUMBER COLON places
    { id2marking[$1] = temp; temp.clear(); }
;

places:
  place
| places COMMA place;

place:
  /* empty */
| NAME COLON NUMBER
    { 
      temp += NAME_token;
      if ($3 > 1) {
        temp += ":" + std::string(mi_text);
      }
    }
;
