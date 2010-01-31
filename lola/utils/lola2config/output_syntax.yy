%token_table
%defines
%name-prefix="output_"


%{
#define YYERROR_VERBOSE
#define YYMAXDEPTH 65535

#include <libconfig.h++>
using namespace libconfig;

extern int output_lex();
extern int output_error(const char *);

extern Config *cfg;
Setting *currentSetting = NULL;
Setting *currentSetting2 = NULL;
Setting *currentSetting3 = NULL;
Setting *currentSetting4 = NULL;
Setting *result = NULL;
%}

%union {
  int val;
  char *name;
}

%token KW_STATE KW_PROG KW_PATH_EXPRESSION LPAREN RPAREN DOT HASH
%token COLON COMMA ARROW INDENT BANG STAR QUESTION MSG_NODEADLOCK MSG_BOUNDED
%token MESSAGE PATHARROW MSG_DEADLOCK KW_PATH MSG_UNBOUNDED KW_GENERATOR
%token KW_PLACES KW_TRANSITIONS KW_SIGNIFICATPLACES MARKER KW_STATES KW_EDGES KW_HASHTABLEENTRIES
%token KW_GENERATORS KW_GROUPS KW_SYMMETRIES KW_DEADBRANCHES KW_COMPUTING_SYMM
%token <val> NUMBER
%token <name> NAME

%%

output:
  net_info
    { result = &cfg->getRoot().add("result", Setting::TypeGroup); }
  symm_info
  result artifact net_footer
| generators
;

result:
  /* empty */
| MSG_DEADLOCK
    { result->add("deadlock", Setting::TypeBoolean) = true; }
| MSG_NODEADLOCK
    { result->add("deadlock", Setting::TypeBoolean) = false; }
| MSG_BOUNDED
    { result->add("unbounded", Setting::TypeBoolean) = false; }
| MSG_UNBOUNDED
    { result->add("unbounded", Setting::TypeBoolean) = true; }
;

artifact:
  /* empty */
| KW_PATH
    { currentSetting = &result->add("path", Setting::TypeList); }
  path
| KW_PATH_EXPRESSION
    { currentSetting = &result->add("path", Setting::TypeList); }
  path_loop
| graph
;

/****************************************************************************/

path:
  NAME { currentSetting->add(Setting::TypeString) = $1; }
| path NAME { currentSetting->add(Setting::TypeString) = $2; }
;

path_loop:
  LPAREN
    { currentSetting = &result->add("loop", Setting::TypeList); }
  path RPAREN
| path LPAREN { currentSetting = &result->add("loop", Setting::TypeList); }
  path RPAREN
;

/****************************************************************************/

graph:
  state
| graph state
;

state:
  KW_STATE marking
| KW_STATE state_marker NUMBER successors
| KW_STATE state_marker NUMBER marking successors
;

marking:
  NAME COLON NUMBER
| marking COMMA NAME COLON NUMBER
;

state_marker:
  /* empty */
| BANG
| STAR
;

successors:
  /* empty */
| NAME PATHARROW NUMBER successors
| NAME ARROW NUMBER successors
| NAME ARROW QUESTION successors
;

/****************************************************************************/

net_info:
  NUMBER KW_PLACES NUMBER KW_TRANSITIONS
    {
        Setting &s = cfg->getRoot().add("net", Setting::TypeGroup);
        s.add("places", Setting::TypeInt) = $1;
        s.add("transitions", Setting::TypeInt) = $3;
    }
| NUMBER KW_PLACES NUMBER KW_TRANSITIONS NUMBER KW_SIGNIFICATPLACES
    {
        Setting &s = cfg->getRoot().add("net", Setting::TypeGroup);
        s.add("places", Setting::TypeInt) = $1;
        s.add("transitions", Setting::TypeInt) = $3;
        s.add("significant_places", Setting::TypeInt) = $5;
    }
;

/****************************************************************************/

symm_info:
  /* empty */
| KW_COMPUTING_SYMM NUMBER KW_GENERATORS NUMBER KW_GROUPS
  NUMBER KW_SYMMETRIES NUMBER KW_DEADBRANCHES
    {
        Setting &s = cfg->getRoot().add("symmetries", Setting::TypeGroup);
        s.add("nogenerators", Setting::TypeInt) = $2;
        s.add("groups", Setting::TypeInt) = $4;
        s.add("symmetries", Setting::TypeInt) = $6;
        s.add("dead_branches", Setting::TypeInt) = $8;
        currentSetting = &s.add("generators", Setting::TypeList);
    }
  generators
;

generators:
  KW_GENERATOR HASH NAME
    {
        if (currentSetting == NULL) {
            Setting &s = cfg->getRoot().add("symmetries", Setting::TypeGroup);
            currentSetting = &s.add("generators", Setting::TypeList);
        }
        std::string name($3);
        int family = atoi(name.substr(0, name.find_first_of(".")).c_str());
        int number = atoi(name.substr(name.find_first_of(".")+1, name.length()).c_str());
        currentSetting2 = &currentSetting->add(Setting::TypeGroup);
        currentSetting2->add("family", Setting::TypeInt) = family;
        currentSetting2->add("number", Setting::TypeInt) = number;
        currentSetting3 = &currentSetting2->add("cycles", Setting::TypeList);
    }
  automorphisms
| generators KW_GENERATOR HASH NAME
    {
        if (currentSetting == NULL) {
            Setting &s = cfg->getRoot().add("symmetries", Setting::TypeGroup);
            currentSetting = &s.add("generators", Setting::TypeList);
        }
        std::string name($4);
        int family = atoi(name.substr(0, name.find_first_of(".")).c_str());
        int number = atoi(name.substr(name.find_first_of(".")+1, name.length()).c_str());
        currentSetting2 = &currentSetting->add(Setting::TypeGroup);
        currentSetting2->add("family", Setting::TypeInt) = family;
        currentSetting2->add("number", Setting::TypeInt) = number;
        currentSetting3 = &currentSetting2->add("cycles", Setting::TypeList);
    }
  automorphisms
;

automorphisms:
  LPAREN
    { currentSetting4 = &currentSetting3->add(Setting::TypeList); }
  placelist RPAREN
| automorphisms LPAREN
    { currentSetting4 = &currentSetting3->add(Setting::TypeList); }
  placelist RPAREN
;

placelist:
  NAME           { currentSetting4->add(Setting::TypeString) = $1; }
| placelist NAME { currentSetting4->add(Setting::TypeString) = $2; }
;

/****************************************************************************/

net_footer:
  /* empty */
| MARKER NUMBER KW_STATES COMMA NUMBER KW_EDGES COMMA NUMBER KW_HASHTABLEENTRIES
    {
        Setting &s = cfg->getRoot().add("statistics", Setting::TypeGroup);
        s.add("states", Setting::TypeInt) = $2;
        s.add("edges", Setting::TypeInt) = $5;
        s.add("hash_table_entries", Setting::TypeInt) = $8;
    }
;
