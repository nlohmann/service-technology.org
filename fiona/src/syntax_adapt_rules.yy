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
#include "options.h"
#include "main.h"

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

// defined in "debug.h"
extern int adapt_rules_yyerror(const char*);


namespace
{
    list< string > channelList;	   // a list of names for input/output places (channels)
    adapterRule currentRule;	  // current read rule
    unsigned int currentRuleCost; // costs of current read rule
}


int adapt_rules_yyerror(const char* msg)
{
  cerr << msg << endl;
  cerr << "error in line " << adapt_rules_yylineno << ": token last read: `" << adapt_rules_yytext << "'" << endl;
  setExitCode(EC_PARSE_ERROR);
}

%}

// Bison options
%name-prefix="adapt_rules_yy"

%token RULE_HIDDEN RULE_OBSERVABLE RULE_CONTROLLABLE
%token NAME NUMBER ARROW_BEGIN ARROW_END COMMA SEMICOLON

%token_table

%union {
  char *str;
  unsigned int value;
}

/* the types of the non-terminal symbols */
%type <str> NAME
%type <str> NUMBER


%%

/* adapter rules consist of rules for partial communication flow (hidden,
** observable and controllable) and rules for total communication flow
** (called standard rules or total rules) */

adapt_rules:
    RULE_HIDDEN partial_rule
    {
        hiddenRules.push_back(currentRule);

        for ( list< string >::iterator channel = currentRule.first.begin(); channel != currentRule.first.end(); channel++) {
            if ( ruleTypePerChannel.find(*channel) != ruleTypePerChannel.end() &&
                 ruleTypePerChannel[*channel] != ADAPT_HIDDEN ) {
                string error = "Channel " + string(*channel) + " was used in different rule types!";
                adapt_rules_yyerror(error.c_str());
            }
            ruleTypePerChannel[*channel] = ADAPT_HIDDEN;
        }

        for ( list< string >::iterator channel = currentRule.second.begin(); channel != currentRule.second.end(); channel++) {
            if ( ruleTypePerChannel.find(*channel) != ruleTypePerChannel.end() &&
                 ruleTypePerChannel[*channel] != ADAPT_HIDDEN ) {
                string error = "Channel " + string(*channel) + " was used in different rule types!";
                adapt_rules_yyerror(error.c_str());
            }
            ruleTypePerChannel[*channel] = ADAPT_HIDDEN;
        }

        currentRule.first  = list< string >();
        currentRule.second = list< string >();
    }
    SEMICOLON adapt_rules
|
    RULE_OBSERVABLE partial_rule
    {
        observableRules.push_back(currentRule);

        for ( list< string >::iterator channel = currentRule.first.begin(); channel != currentRule.first.end(); channel++) {
            if ( ruleTypePerChannel.find(*channel) != ruleTypePerChannel.end() &&
                 ruleTypePerChannel[*channel] != ADAPT_OBSERVABLE ) {
                string error = "Channel " + string(*channel) + " was used in different rule types!";
                adapt_rules_yyerror(error.c_str());
            }
            ruleTypePerChannel[*channel] = ADAPT_OBSERVABLE;
        }

        for ( list< string >::iterator channel = currentRule.second.begin(); channel != currentRule.second.end(); channel++) {
            if ( ruleTypePerChannel.find(*channel) != ruleTypePerChannel.end() &&
                 ruleTypePerChannel[*channel] != ADAPT_OBSERVABLE ) {
                string error = "Channel " + string(*channel) + " was used in different rule types!";
                adapt_rules_yyerror(error.c_str());
            }
            ruleTypePerChannel[*channel] = ADAPT_OBSERVABLE;
        }

        currentRule.first  = list< string >();
        currentRule.second = list< string >();
    }
    SEMICOLON adapt_rules
|
    RULE_CONTROLLABLE partial_rule
    {
        controllableRules.push_back(currentRule);

        for ( list< string >::iterator channel = currentRule.first.begin(); channel != currentRule.first.end(); channel++) {
            if ( ruleTypePerChannel.find(*channel) != ruleTypePerChannel.end() &&
                 ruleTypePerChannel[*channel] != ADAPT_CONTROLLABLE ) {
                string error = "Channel " + string(*channel) + " was used in different rule types!";
                adapt_rules_yyerror(error.c_str());
            }
            ruleTypePerChannel[*channel] = ADAPT_CONTROLLABLE;
        }

        for ( list< string >::iterator channel = currentRule.second.begin(); channel != currentRule.second.end(); channel++) {
            if ( ruleTypePerChannel.find(*channel) != ruleTypePerChannel.end() &&
                 ruleTypePerChannel[*channel] != ADAPT_CONTROLLABLE ) {
                string error = "Channel " + string(*channel) + " was used in different rule types!";
                adapt_rules_yyerror(error.c_str());
            }
            ruleTypePerChannel[*channel] = ADAPT_CONTROLLABLE;
        }

        currentRule.first  = list< string >();
        currentRule.second = list< string >();
    }
    SEMICOLON adapt_rules
|
    total_rule
    {
        totalRules.push_back( make_pair(currentRule, currentRuleCost) );

        for ( list< string >::iterator channel = currentRule.first.begin(); channel != currentRule.first.end(); channel++) {
            if ( ruleTypePerChannel.find(*channel) != ruleTypePerChannel.end() &&
                 ruleTypePerChannel[*channel] != ADAPT_TOTAL ) {
                string error = "Channel " + string(*channel) + " was used in different rule types!";
                adapt_rules_yyerror(error.c_str());
            }
            ruleTypePerChannel[*channel] = ADAPT_TOTAL;
        }

        for ( list< string >::iterator channel = currentRule.second.begin(); channel != currentRule.second.end(); channel++) {
            if ( ruleTypePerChannel.find(*channel) != ruleTypePerChannel.end() &&
                 ruleTypePerChannel[*channel] != ADAPT_TOTAL ) {
                string error = "Channel " + string(*channel) + " was used in different rule types!";
                adapt_rules_yyerror(error.c_str());
            }
            ruleTypePerChannel[*channel] = ADAPT_TOTAL;
        }

        currentRule.first  = list< string >();
        currentRule.second = list< string >();
    }
    SEMICOLON adapt_rules
|
    /* empty */
;

/* partial rules cannot have optional channel lists */

partial_rule:
    channel_list 
    {
        currentRule.first = channelList;

        map< string, bool > seen;
        for ( list< string >::iterator channel = channelList.begin(); channel != channelList.end(); channel++) {
            if (!seen[*channel]) {
                seen[*channel] = true;
                consumeRulesPerChannel[*channel] += 1;
            }
        }

        channelList.clear();
    }
    ARROW_END channel_list
    {
        currentRule.second = channelList;

        map< string, bool > seen;
        for ( list< string >::iterator channel = channelList.begin(); channel != channelList.end(); channel++) {
            if (!seen[*channel]) {
                seen[*channel] = true;
                //produceRulesPerChannel[*channel] += 1;
            }
        }

        channelList.clear();
    }
;

/* total rules can have optional channel lists */

total_rule:
    opt_channel_list 
    {
        currentRule.first = channelList;

        map< string, bool > seen;
        for ( list< string >::iterator channel = channelList.begin(); channel != channelList.end(); channel++) {
            if (!seen[*channel]) {
                seen[*channel] = true;
                consumeRulesPerChannel[*channel] += 1;
            }
        }

        channelList.clear();
    }
    ARROW_END opt_channel_list
    {
        currentRule.second = channelList;
        currentRuleCost = 0;

        map< string, bool > seen;
        for ( list< string >::iterator channel = channelList.begin(); channel != channelList.end(); channel++) {
            if (!seen[*channel]) {
                seen[*channel] = true;
                //produceRulesPerChannel[*channel] += 1;
            }
        }

        channelList.clear();
    }
|
    opt_channel_list 
    {
        currentRule.first = channelList;

        map< string, bool > seen;
        for ( list< string >::iterator channel = channelList.begin(); channel != channelList.end(); channel++) {
            if (!seen[*channel]) {
                seen[*channel] = true;
                consumeRulesPerChannel[*channel] += 1;
            }
        }

        channelList.clear();
    }
    ARROW_BEGIN NUMBER ARROW_END opt_channel_list
    {
        currentRule.second = channelList;
        sscanf($4,"%u",&currentRuleCost);
        free($4);

        map< string, bool > seen;
        for ( list< string >::iterator channel = channelList.begin(); channel != channelList.end(); channel++) {
            if (!seen[*channel]) {
                seen[*channel] = true;
                //produceRulesPerChannel[*channel] += 1;
            }
        }

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

