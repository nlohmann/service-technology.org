/*****************************************************************************\
 ProductOG -- Computing product OGs

 Copyright (C) 2009  Christian Sura <christian.sura@uni-rostock.de>

 ProductOG is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 ProductOG is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with ProductOG.  If not, see <http://www.gnu.org/licenses/>. 
\*****************************************************************************/


%option outfile="lex.yy.c"
%option noyywrap
%option yylineno
%option nodefault
%option nounput

%{

#include <cstring>
#include <iostream>
#include <string>
#include "syntax-og.h"
#include "config.h"

int yyerror(char const *msg);

%}

%s COMMENT
%s BITS

whitespace     [\n\r\t ]
identifier     [^,;:()\t \n\r\{\}=~]+
number         [0-9]+


%%


"{"                                     { BEGIN(COMMENT);      }
<COMMENT>"}"                            { BEGIN(INITIAL);      }
<COMMENT>[^}]*                          { /* ignore */         }

"NODES"                                 { return KEY_NODES;            }

"INTERFACE"                             { return KEY_INTERFACE;        }
"INPUT"                                 { return KEY_INPUT;            }
"OUTPUT"                                { return KEY_OUTPUT;           }
"SYNCHRONOUS"                           { return KEY_SYNCHRONOUS;      }

<BITS>"F"                               { BEGIN(INITIAL); return BIT_F; }
<BITS>"S"                               { BEGIN(INITIAL); return BIT_S; }
<BITS>"T"                               { BEGIN(INITIAL); return BIT_T; }
"TRUE"                                  { return KEY_TRUE;             }
"true"                                  { return KEY_TRUE;             }
"FALSE"                                 { return KEY_FALSE;            }
"false"                                 { return KEY_FALSE;            }
"FINAL"                                 { return KEY_FINAL;            }
"final"                                 { return KEY_FINAL;            }
"~"                                     { return OP_NOT;               }
"*"                                     { return OP_AND;               }
"+"                                     { return OP_OR;                }
"("                                     { return LPAR;                 }
")"                                     { return RPAR;                 }

":"                                     { return COLON;                }
"::"                                    { BEGIN(BITS); return DOUBLECOLON; }
";"                                     { return SEMICOLON;            }
","                                     { return COMMA;                }
"->"                                    { return ARROW;                }

{number}       { yylval.value = atoi(yytext); return NUMBER;           }
{identifier}   { yylval.str = strdup(yytext); return IDENT;            }

{whitespace}                            { /* do nothing */             }

.                                       { yyerror("lexical error"); }

%%

int yyerror(char const *msg) {
    std::cerr << PACKAGE << ": " << yylineno
              << ": ERROR near '" << yytext 
              << "': " << msg << std::endl;
    exit(EXIT_FAILURE);
}
