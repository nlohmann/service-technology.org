/*****************************************************************************\
 Tara -- 

 Copyright (C) 2009  Niels Lohmann <niels.lohmann@uni-rostock.de>

 Tara is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Tara is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Tara.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/


/*
NOTE: This syntax-scanner does not recognize semantic errors!
Wrong Input causes undefined behaviour and not necessarily an error message.
*/

%token CF_RESET CF_COLON CF_NUMBER CF_NAME

%expect 0
%defines
%name-prefix="costfunction_"

%{
#include <map>
#include <string>
#include <stdio.h>
#include <pnapi/pnapi.h>
#include "Tara.h"


extern int costfunction_lex();
extern int costfunction_error(const char *);


%}

%union {
  unsigned int val;
  char* str;
}

%type <val> CF_NUMBER
%type <str> CF_NAME
%%

empty: costs | ;

costs: 
     cost
   | cost costs
;

cost:
   CF_NAME CF_COLON CF_NUMBER {
       pnapi::Transition* t=Tara::net->findTransition($1);
       if(t!=NULL) {
           Tara::partialCostFunction[t]=$3;
	   if (Tara::highestTransitionCosts < $3) Tara::highestTransitionCosts = $3;
	}
       free($1); //free string
   }
   | CF_NAME CF_COLON CF_NUMBER CF_RESET {
       pnapi::Transition* t=Tara::net->findTransition($1);
       if(t!=NULL) {
           Tara::partialCostFunction[t] = $3;
           Tara::resetMap[t] = true;
	   if (Tara::highestTransitionCosts < $3) Tara::highestTransitionCosts = $3;
	}
       free($1); //free string
   }
;
