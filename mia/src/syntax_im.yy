/*****************************************************************************\
 Mia -- calculating migration information

 Copyright (C) 2009  Niels Lohmann <niels.lohmann@uni-rostock.de>

 Mia is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Mia is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Mia.  If not, see <http://www.gnu.org/licenses/>. 
\*****************************************************************************/

%token KW_INTERFACE COMMA RBRACKET LBRACKET NUMBER NAME

%defines
%name-prefix="im_"

%{
#include <cstdio>
#include <string>
#include <vector>
#include <map>

#define YYDEBUG 1
#define YYERROR_VERBOSE 1

/// the tuples of the source (innermarking, mppstate, interfacemarking)
std::map<unsigned, std::vector<std::vector<unsigned int> > > tuples_source;

std::vector<unsigned int> currentTuple;

unsigned int stat_tupleCount = 0;

// mappings for the port name/id management
std::map<unsigned int, std::string> id2name;
std::map<std::string, unsigned int> name2id;

unsigned int currentPos;

/// the number of interface places
unsigned int interfaceLength = 1;

/// the current NAME token as string
extern std::string NAME_token;

extern int im_lex();
extern int im_error(const char *);
%}

%union {
  unsigned int val;
}

%type <val> NUMBER

%%

im:
  KW_INTERFACE LBRACKET interface RBRACKET
    { currentTuple = std::vector<unsigned int>(interfaceLength, 0); }
  tuples
;

interface:
  NAME
    { id2name[interfaceLength] = NAME_token;
      name2id[NAME_token] = interfaceLength;
      ++interfaceLength; }
| interface COMMA NAME
    { id2name[interfaceLength] = NAME_token;
      name2id[NAME_token] = interfaceLength;
      ++interfaceLength; }
;

tuples:
  tuple
| tuples tuple;

tuple:
  NUMBER NUMBER 
    { currentTuple[0] = $2; currentPos = 1; }
  LBRACKET vector RBRACKET
    { tuples_source[$1].push_back(currentTuple); ++stat_tupleCount; }
;

vector:
  NUMBER
    { currentTuple[currentPos++] = $1; }
| vector COMMA NUMBER
    { currentTuple[currentPos++] = $3; }
;
