/*****************************************************************************\
 Marlene -- synthesizing behavioral adapters

 Copyright (C) 2009  Christian Gierds <gierds@informatik.hu-berlin.de>

 Marlene is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Marlene is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Marlene.  If not, see <http://www.gnu.org/licenses/>. 
\*****************************************************************************/

%{
// options for Bison
#define YYDEBUG 1
#define YYERROR_VERBOSE 0  // for verbose error messages


// to avoid the message "parser stack overflow"
#define YYMAXDEPTH 1000000
#define YYINITDEPTH 10000

#include <iostream>
#include <cstdlib>
#include <string>
#include <list>
#include "adapter.h"
#include "config.h"
#include "verbose.h"

// from flex
extern char* adapt_rules_yytext;
extern int adapt_rules_yylineno;
extern int adapt_rules_yylex();
extern int adapt_rules_yyerror(const char*);

RuleSet * workingSet = NULL;
RuleSet::AdapterRule::cfMode cfmodus = RuleSet::AdapterRule::AR_NORMAL;

int adapt_rules_yyerror(const char* msg)
{
  message("error while reading adapter rules\n%s\nerror in line %d, token last read '%s'", msg, adapt_rules_yylineno, adapt_rules_yytext);
  exit(EXIT_FAILURE);
}

%}

// Bison options
%defines
%name-prefix="adapt_rules_yy"

%token RULE_HIDDEN RULE_OBSERVABLE RULE_CONTROLLABLE
%token NAME VALUE ARROW COMMA SEMICOLON DMINUS
%token LBRACE RBRACE

%token_table

%{
typedef std::list< unsigned int > uilist;
%}

%union {
  char *str;
  int ival;
  uilist * channelList;
}

/* the types of the non-terminal symbols */
%type <str> NAME
%type <ival> VALUE
%type <ival> opt_costvalue
%type <channelList> channel_list
%type <channelList> opt_channel_list


%%

/* adapter rules consist of rules for partial communication flow (hidden,
** observable and controllable) and rules for total communication flow
** (called standard rules or total rules) */

adapt_rules:
    RULE_HIDDEN 
    { cfmodus = RuleSet::AdapterRule::AR_HIDDEN; }
    partial_rule
    SEMICOLON adapt_rules
|
    RULE_OBSERVABLE 
    { cfmodus = RuleSet::AdapterRule::AR_OBSERVABLE; }
    partial_rule
    SEMICOLON adapt_rules
|
    RULE_CONTROLLABLE 
    { cfmodus = RuleSet::AdapterRule::AR_CONTROLLABLE; }
    partial_rule
    SEMICOLON adapt_rules
|
    total_rule
    SEMICOLON adapt_rules
|
    /* empty */
;

/* partial rules cannot have optional channel lists */

partial_rule:
    channel_list 
    ARROW channel_list
    {
        std::pair< std::list<unsigned int>, std::list<unsigned int> > rulepair(*$1, *$3);
        std::list< unsigned int > * slist = new std::list< unsigned int >();
        RuleSet::AdapterRule * rule = new RuleSet::AdapterRule(rulepair, *slist, cfmodus);
        workingSet->_adapterRules.push_back(rule);
        delete $1;
        delete $3;
	delete slist;
    }
|
    channel_list 
    DMINUS channel_list
    ARROW channel_list
    {
        std::pair< std::list<unsigned int>, std::list<unsigned int> > rulepair(*$1, *$5);
        RuleSet::AdapterRule * rule = new RuleSet::AdapterRule(rulepair, *$3, cfmodus);
        workingSet->_adapterRules.push_back(rule);
        delete $1;
        delete $3;
        delete $5;
    }
;

/* total rules can have optional channel lists */

total_rule:
    opt_channel_list 
    ARROW opt_channel_list
    opt_costvalue
    {
        std::pair< std::list<unsigned int>, std::list<unsigned int> > rulepair(*$1, *$3);
        std::list< unsigned int > * slist = new std::list< unsigned int >();
        RuleSet::AdapterRule * rule = new RuleSet::AdapterRule(rulepair, *slist, RuleSet::AdapterRule::AR_NORMAL, $4);
	workingSet->_adapterRules.push_back(rule);
        delete $1;
        delete $3;
	delete slist;
    }
|    
    opt_channel_list 
    DMINUS channel_list
    ARROW opt_channel_list
    opt_costvalue
    {
        std::pair< std::list<unsigned int>, std::list<unsigned int> > rulepair(*$1, *$5);
        RuleSet::AdapterRule * rule = new RuleSet::AdapterRule(rulepair, *$3, RuleSet::AdapterRule::AR_NORMAL, $6);
        workingSet->_adapterRules.push_back(rule);
        delete $1;
        delete $3;
        delete $5;
    }
;

opt_channel_list:
    channel_list
    {
        $$ = $1;
    }
|
    /* empty */
    {
        $$ = new std::list< unsigned int>();
    }
;

channel_list:
    NAME
    {
        $$ = new std::list< unsigned int>();
        $$->push_back(workingSet->getIdForMessage(std::string($1)));
        free $1;
    }
|
    NAME COMMA channel_list
    {
        $$ = $3;
        $$->push_back(workingSet->getIdForMessage(std::string($1)));
        free $1;
    }
;

opt_costvalue:
    /* empty */
    {
        $$ = 0;
    }
|
    LBRACE VALUE RBRACE
    {
        $$ = $2;
    }
