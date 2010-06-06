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
#include <utility> // make_pair
#include "config.h"
#include "verbose.h"
#include "markings.h"

// from flex
extern char* marking_information_yytext;
extern int marking_information_yylineno;
extern int marking_information_yylex();
extern int marking_information_yyerror(const char*);

int marking_information_yyerror(const char* msg)
{
  message("error while reading marking information\n%s\nerror in line %d, token last read '%s'", msg, marking_information_yylineno, marking_information_yytext);
  exit(EXIT_FAILURE);
}

// the actual marking information object ... set to "this" before parsing.
MarkingInformation * mi = NULL;
Marking * marking = NULL;
%}

// Bison options
%defines
%name-prefix="marking_information_yy"

%token NAME VALUE COMMA COLON

%token_table

%union {
  char *str;
  int ival;
}

/* the types of the non-terminal symbols */
%type <str> NAME
%type <ival> VALUE


%%

marking_information:
                   marking marking_information
                   {
                     if (mi == NULL)
                     {
                       message("marking information not set");
                       exit(1);
                     }
                   }
                   |
                   /* empty */
                   ;

marking:
       VALUE COLON opt_place_list
       {
         mi->markings[mi->getIDForMarking($1)] = marking;
         marking = NULL;
       }
       ;

opt_place_list:
              place_list
              |
              /* empty */
              ;

place_list:
          NAME COLON VALUE COMMA place_list
          {
            if (marking == NULL)
            {
              marking = new Marking();
            }
            std::string name($1);
            unsigned int id = marking->getIDForPlace(name);
            marking->marking.push_back(std::make_pair(id, $3));
            free $1;
          }
          |
          NAME COMMA place_list
          {
            if (marking == NULL)
            {
              marking = new Marking();
            }
            std::string name($1);
            unsigned int id = marking->getIDForPlace(name);
            marking->marking.push_back(std::make_pair(id, 1));
            free $1;
          }
          |
          NAME COLON VALUE
          {
            if (marking == NULL)
            {
              marking = new Marking();
            }
            std::string name($1);
            unsigned int id = marking->getIDForPlace(name);
            marking->marking.push_back(std::make_pair(id, $3));
            free $1;
          }
          |
          NAME
          {
            if (marking == NULL)
            {
              marking = new Marking();
            }
            std::string name($1);
            unsigned int id = marking->getIDForPlace(name);
            marking->marking.push_back(std::make_pair(id, 1));
            free $1;
          }
          ;


