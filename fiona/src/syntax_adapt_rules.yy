%{
// options for Bison
#define YYDEBUG 1
#define YYERROR_VERBOSE 0  // for verbose error messages


// to avoid the message "parser stack overflow"
#define YYMAXDEPTH 1000000
#define YYINITDEPTH 10000


#include <string>
#include <iostream>
#include <set>
#include <map>
#include <list>

#include "adapter.h"

using std::cerr;
using std::endl;
using std::string;
using std::set;
using std::map;
using std::list;
using std::pair;


// from flex
extern char* adapt_rules_yytext;
extern int adapt_rules_yylineno;
extern int adapt_rules_yylex();


namespace
{
    list< string > channelList;	// a list of names for input/output places (channels)
    adapterRule currentRule;	// current read rule
}


int adapt_rules_yyerror(const char* msg)
{
  cerr << msg << endl;
  cerr << "error in line " << adapt_rules_yylineno << ": token last read: `" << adapt_rules_yytext << "'" << endl;
  exit(1);
}

%}

// Bison options
%name-prefix="adapt_rules_yy"

%token NAME ARROW COMMA SEMICOLON

%token_table

%union {
  char *str;
}

/* the types of the non-terminal symbols */
%type <str> NAME


%%

/*
 *
 * adapt_rules: ( adapt_rule ; )*
 *
 */
adapt_rules:
    adapt_rule
    {
        adapter_rules.push_back(currentRule);
        currentRule.first = list< string >();
        currentRule.second = list< string >();
    }
    SEMICOLON adapt_rules
|
    /* empty */
;

adapt_rule:
    opt_channel_list 
    {
        currentRule.first = channelList;
        {
            map< string, bool > seen;
            for ( list< string >::iterator channel = channelList.begin(); channel != channelList.end(); channel++)
            {
                if (!seen[*channel])
                {
                    // count the number of rules in which current channel occurs
                    seen[*channel] = true;
                    rulesPerChannel[*channel] += 1;
                }
            }
        }
        channelList.clear();
    }
    ARROW opt_channel_list
    {
        currentRule.second = channelList;
        channelList.clear();
    }
;

opt_channel_list:
    channel_list
|
    /* empty */
;

channel_list:
    NAME
    {
        channelList.push_back($1);
    }
|
    NAME COMMA channel_list
    {
        channelList.push_back($1);
    }
;

