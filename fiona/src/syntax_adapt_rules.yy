/*****************************************************************************\
 Copyright (c) 2008  Robert Danitz, Christian Gierds, Niels Lohmann,
                     Peter Massuthe, Richard Mu"ller, Janine Ott,
                     Jan Su"rmeli, Daniela Weinberg, Karsten Wolf,
                     Martin Znamirowski

 Copyright (c) 2007  Jan Bretschneider, Robert Danitz, Christian Gierds,
                     Kathrin Kaschner, Leonard Kern, Niels Lohmann,
                     Peter Massuthe, Daniela Weinberg, Karsten Wolf

 Copyright (c) 2006  Kathrin Kaschner, Peter Massuthe, Daniela Weinberg,
                     Karsten Wolf

 This file is part of Fiona, see <http://service-technology.org/fiona>.

 Fiona is free software; you can redistribute it and/or modify it under the
 terms of the GNU General Public License as published by the Free Software
 Foundation; either version 3 of the License, or (at your option) any later
 version.
 
 Fiona is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License along with
 Fiona (see file COPYING). If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

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
#include <cstdlib>

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

