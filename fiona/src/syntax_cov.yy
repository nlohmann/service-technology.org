/*****************************************************************************\
 Copyright (c) 2008  Robert Danitz, Christian Gierds, Niels Lohmann,
                     Peter Massuthe, Richard Mu"ller, Janine Ott,
                     Jan Su"rmeli, Daniela Weinberg, Karsten Wolf,
                     Martin Znamirowski

 Copyright (c) 2007  Jan Bretschneider, Robert Danitz, Christian Gierds,
                     Kathrin Kaschner, Leonard Kern, Niels Lohmann,
                     Peter Massuthe, Daniela Weinberg, Karsten Wolf

 Copyright (c) 2006  Kathrin Kaschner, Peter Massuthe, Daniela Weinberg,
                     Karsten Wolf

 This file is part of Fiona, see <http://service-technology.org/fiona>.

 Fiona is free software; you can redistribute it and/or modify it under the
 terms of the GNU General Public License as published by the Free Software
 Foundation; either version 3 of the License, or (at your option) any later
 version.
 
 Fiona is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License along with
 Fiona (see file COPYING). If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/


/*!
 * \file    syntax_cov.yy
 *
 * \brief   the parser for the file format specifying the set of open net 
 *          nodes to be covered 
 *          (needed for operating guidelines with global constraint)
 *
 * \author  responsible: Robert Danitz <danitz@informatik.hu-berlin.de> 
 *
 */

%{
/* Prologue: Syntax and usage of the prologue.
 * Bison Declarations: Syntax and usage of the Bison declarations section.
 * Grammar Rules: Syntax and usage of the grammar rules section.
 * Epilogue: Syntax and usage of the epilogue.  */

// options for Bison
#define YYDEBUG 1
#define YYERROR_VERBOSE 0  // for verbose error messages


// to avoid the message "parser stack overflow"
#define YYMAXDEPTH 1000000
#define YYINITDEPTH 10000


// from flex
extern char* cov_yytext;
extern int cov_yylex();


#include "mynew.h"
#include "debug.h"

#include "ConstraintOG.h"

extern AnnotatedGraph* OGToParse;

set<string>* PlacesToCover;
set<string>* TransitionsToCover;

using namespace std;

%}

// Bison options
%name-prefix="cov_yy"

// the terminal symbols (tokens)

%token key_places
%token key_transitions
%token comma colon semicolon ident
%token key_true key_false


// Bison generates a list of all used tokens in file "syntax_covog.h" (for flex)
%token_table


%union {
    char * str;
}

/* the types of the non-terminal symbols */
%type <str> ident


%%

/* Grammar rules */

constraint: places transitions
| transitions places
;

places: key_places places_list semicolon
;

places_list: places_list comma place 
| place 
| /* empty */
;

place: ident
    {
        PlacesToCover->insert($1);
        free($1);
    }
;

transitions: key_transitions transitions_list semicolon
;

transitions_list: transitions_list comma transition
| transition
| /* empty */
;

transition: ident
    {
        TransitionsToCover->insert($1);
        free($1);
    }
;
