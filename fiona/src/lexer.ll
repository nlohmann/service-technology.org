/* DEFINITIONS */

/* flex options */
%option noyywrap
%option yylineno
%option nodefault
%option debug



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
"{"[^\n]*"}"    						{ break; }
[^,;:()\t \n\{\}][^,;:()\t \n\{\}]*		{ setlval(); return ident; }
[\n\r]            						{ yylineno++; break; }
[ \t]           						{ break; }
.										{ yyerror("lexical error"); }


%%
// USER CODE

// pass token string as attribute to bison
void setlval() {
  yylval.str = new char[strlen(yytext)+1];
  strcpy(yylval.str,yytext);
}




