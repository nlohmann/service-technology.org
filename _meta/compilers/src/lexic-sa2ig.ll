/*****************************************************************************\
 SA2IG -- compiling Wendy SAs to FIONA IGs

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

%option outfile="lex.yy.c"
%option noyywrap
%option yylineno
%option nodefault
%option nounput

%{

#include <cstring>
#include <iostream>

#include "syntax-sa2ig.h"
#include "config.h"

int yyerror(char const *msg);

extern std::ostream * myOut;

%}

%s COMMENT

whitespace     [\n\r\t ]
identifier     [^,;:()\t \n\r\{\}=]+
number         [0-9]+


%%


"{"                                     { (*myOut) << "{"; BEGIN(COMMENT);      }
<COMMENT>"}"                            { (*myOut) << "}\n\n"; BEGIN(INITIAL);      }
<COMMENT>[^}]*                          { (*myOut) << yytext; /* copy comment */  }

"NODES"                                 { return KEY_NODES;            }
"INITIAL"                               { return KEY_INITIAL;          }
"FINAL"                                 { return KEY_FINAL;            }

"INTERFACE"                             { return KEY_INTERFACE;        }
"INPUT"                                 { return KEY_INPUT;            }
"OUTPUT"                                { return KEY_OUTPUT;           }
"SYNCHRONOUS"                           { return KEY_SYNCHRONOUS;      }

":"                                     { return COLON;                }
";"                                     { return SEMICOLON;            }
","                                     { return COMMA;                }
"->"                                    { return ARROW;                }

{number}             { yylval.value = atoi(yytext); return NUMBER;     }
{identifier}         { yylval.str = strdup(yytext); return IDENT;      }

{whitespace}                            { /* do nothing */             }

.                                       { yyerror("lexical error"); }

%%

int yyerror(char const *msg) {
    std::cerr << PACKAGE << ": " << yylineno
              << ": ERROR near '" << yytext 
              << "': " << msg << std::endl;
    exit(EXIT_FAILURE);
}

