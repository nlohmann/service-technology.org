/*****************************************************************************\
 SA2IG -- compiling Wendy SAs to Fiona IGs

 Copyright (C) 2009  Christian Sura <christian.sura@uni-rostock.de>

 SA2IG is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 SA2IG is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with SA2IG.  If not, see <http://www.gnu.org/licenses/>. 
\*****************************************************************************/

%error-verbose
%token_table
%defines


%{
#include "config.h"
#include <iostream>
#include <map>
#include <set>

// from flex
extern char* yytext;
extern int yylex();
extern int yyerror(char const *msg);

// from main
extern std::ostream * myOut;

// data structures
// recent event
char event;
// map from event to prefix
std::map<std::string, char> events;
// output events (since input and output has to be switched)
std::set<std::string> outputs;
// initial node
unsigned int initialNode;
// finalNodes
std::map<unsigned int, bool> finalNodes;
// recent node
unsigned int recentNode;
// successors
std::map<unsigned int, std::map<std::string, unsigned int> > succ;


%}


%token KEY_NODES KEY_INITIAL KEY_FINAL
%token KEY_INTERFACE KEY_INPUT KEY_OUTPUT KEY_SYNCHRONOUS
%token COLON COMMA SEMICOLON IDENT ARROW NUMBER

%union {
    char *str;
    unsigned int value;
}

%type <value> NUMBER
%type <str>   IDENT

%start sa


%%


sa:
  KEY_INTERFACE
  {
    (*myOut) << "INTERFACE\n  INPUT\n    ";
    event = '!';
  }
  input
  {  event = '?'; }
  output
  { event = '#'; }
  synchronous KEY_NODES nodes
  {
    (*myOut) << ";\n  OUTPUT\n    ";

    std::string delim = ""; // delimeter
    for(std::set<std::string>::iterator it = outputs.begin();
         it != outputs.end(); ++it)
    {
      (*myOut) << delim << *it;
      delim = ", ";
    }

    (*myOut) << ";\n\nNODES";

    // print nodes
    delim = "\n";
    for(std::map<unsigned int, std::map<std::string, unsigned int> >::iterator it = succ.begin();
         it != succ.end(); ++it)
    {
      if(finalNodes[it->first])
        (*myOut) << delim << "  " << it->first << " : finalnode";
      else
        (*myOut) << delim << "  " << it->first;
      delim = ",\n";
    }

    (*myOut) << ";\n\nINITIALNODE\n  " << initialNode << ";\n\nTRANSITIONS";

    // print successors
    delim = "\n";
    for(std::map<unsigned int, std::map<std::string, unsigned int> >::iterator it = succ.begin();
         it != succ.end(); ++it)
    {
      for(std::map<std::string, unsigned int>::iterator s = it->second.begin();
           s != it->second.end(); ++s)
      {
        (*myOut) << delim << "  " << it->first << " -> " << s->second
                 << " : " << events[s->first] << s->first;
        delim = ",\n";
      }
    }

    (*myOut) << ";\n" << std::endl << std::flush;
  }
;


input:
  /* empty */
| KEY_INPUT identlist SEMICOLON
;


output:
  /* empty */
| KEY_OUTPUT identlist SEMICOLON
;


synchronous:
  /* empty */
| KEY_SYNCHRONOUS identlist SEMICOLON
;


identlist:
  /* empty */
| IDENT
  {
    events[$1] = event;
    switch(event)
    {
      case '!' : outputs.insert($1); break;
      case '?' : (*myOut) << $1; break;
      case '#' : break;
      default : assert(false);
    }
    free($1);
  }
| identlist COMMA IDENT
  {
    events[$3] = event;
    switch(event)
    {
      case '!' : outputs.insert($3); break;
      case '?' : (*myOut) << ", " << $3; break;
      case '#' : break;
      default : assert(false);
    }
    free($3);
  }
;


nodes:
  node
| nodes node
;


node:
  NUMBER
  {
    recentNode = $1;
    succ[$1]; // add node to node list
  }
  annotation successors
;


annotation:
  /* empty */
| COLON KEY_INITIAL { initialNode = recentNode; }
| COLON KEY_FINAL   { finalNodes[recentNode] = true; }
| COLON KEY_INITIAL COMMA KEY_FINAL
  {
    initialNode = recentNode;
    finalNodes[recentNode] = true;
  }
;


successors:
  /* empty */
| successors IDENT ARROW NUMBER
  {
    succ[recentNode][$2] = $4; 
    free($2);
  }
;


%%


