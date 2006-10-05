/* DEFINITIONS */

/* flex options */
%option noyywrap
%option yylineno
%option nodefault
%option debug

%s COMMENT

%{
// c-code (wird übernommen)

#include "syntax_wrap.h"			// list of all tokens used
#include <string>

using namespace std;

extern int yyerror(const char *msg);
void setlval();

%}


%%
 /* RULES */

"{$"		{ return lcontrol; }
"$}"		{ return rcontrol; }

"{"            { BEGIN(COMMENT); }
<COMMENT>"}"   { BEGIN(INITIAL); }
<COMMENT>[^}]* {}

MAX_UNIQUE_EVENTS		{ return key_max_unique_events; }
ON_LOOP				{ return key_on_loop; }
MAX_OCCURENCES			{ return key_max_occurences; }
TRUE				{ return key_true; }
FALSE				{ return key_false; }

SAFE             						{ return key_safe;}
PLACE            						{ return key_place; }
INTERNAL		 						{ return key_internal; }
INPUT			 						{ return key_input; }
OUTPUT			 						{ return key_output; }
TRANSITION       						{ return key_transition; }
INITIALMARKING          				{ return key_marking; }
FINALMARKING          					{ return key_finalmarking; }
FINALCONDITION     						{ return key_finalcondition; }
CONSUME          						{ return key_consume; }
PRODUCE          						{ return key_produce; }
ALL_OTHER_PLACES_EMPTY           { return key_all_other_places_empty; }
ALL_OTHER_INTERNAL_PLACES_EMPTY  { return key_all_other_internal_places_empty; }
ALL_OTHER_EXTERNAL_PLACES_EMPTY  { return key_all_other_external_places_empty; }
AND		 								{ return op_and;}
OR		 								{ return op_or;}
NOT		 								{ return op_not;}
\>		 								{ return op_gt;}
\<		 								{ return op_lt;}
\>=		 								{ return op_ge;}
\<=		 								{ return op_le;}
=		 								{ return op_eq;}
\<\>									{ return op_ne;}
\#		 								{ return op_ne;}
\:              						{ return colon; }
\;              						{ return semicolon; }
,               						{ return comma; }
\(		 								{ return lpar;}
\)		 								{ return rpar;}
[0-9][0-9]*     						{ setlval(); return number; }
[^,;:()\t \n\r\{\}=][^,;:()\t \n\r\{\}=]*		{ setlval(); return ident; }
[\n\r]            						{ break; }
[ \t]           						{ break; }
.										{ yyerror("lexical error"); }


%%
// USER CODE

// pass token string as attribute to bison
void setlval() {
  yylval.str = strdup(yytext);
}




