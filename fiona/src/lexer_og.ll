/*****************************************************************************
 * Copyright 2005, 2006 Jan Bretschneider                                    *
 *                                                                           *
 * This file is part of Fiona.                                               *
 *                                                                           *
 * Fiona is free software; you can redistribute it and/or modify it          *
 * under the terms of the GNU General Public License as published by the     *
 * Free Software Foundation; either version 2 of the License, or (at your    *
 * option) any later version.                                                *
 *                                                                           *
 * Fiona is distributed in the hope that it will be useful, but WITHOUT      *
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or     *
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for  *
 * more details.                                                             *
 *                                                                           *
 * You should have received a copy of the GNU General Public License along   *
 * with Fiona; if not, write to the Free Software Foundation, Inc., 51       *
 * Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.                      *
 *****************************************************************************/

/*!
 * \file    lexer_og.ll
 *
 * \brief   the lexer for the OG file format
 *
 * \author  responsible: Jan Bretschneider <bretschn@informatik.hu-berlin.de>
 *
 * \note    This file is part of the tool Fiona and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 */

/* DEFINITIONS */

/* flex options */
%option outfile="lex.yy.c"
%option prefix="og_yy"
%option noyywrap
%option yylineno
%option nodefault
%option debug

%s COMMENT

%{
// c-code (wird übernommen)

#include "syntax_og.h"            // list of all tokens used
#include <cstring>
using namespace std;

extern int og_yyerror(const char *msg);
static void setlval();
%}


%%
 /* RULES */

"{"            { BEGIN(COMMENT); }
<COMMENT>"}"   { BEGIN(INITIAL); }
<COMMENT>[^}]* {                 }

NODES         { return key_nodes;       }
INITIALNODE   { return key_initialnode; }
TRANSITIONS   { return key_transitions; }

TRUE          { return key_true;  }
FALSE         { return key_false; }
\*            { return op_and;    }
\+            { return op_or;     }
\(            { return lpar;      }
\)            { return rpar;      }

\:            { return colon;     }
\;            { return semicolon; }
,             { return comma;     }
->            { return arrow;     }

[^,;:()\t \n\r\{\}=]+  { setlval(); return ident;  }

[\n\r]        { break; }
[ \t]         { break; }
.             { og_yyerror("lexical error"); }


%%
// USER CODE

// pass token string as attribute to bison
static void setlval() {
  og_yylval.str = strdup(og_yytext);
}




