/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     KEY_SAFE = 258,
     KEY_PLACE = 259,
     KEY_INTERNAL = 260,
     KEY_INPUT = 261,
     KEY_OUTPUT = 262,
     KEY_MARKING = 263,
     KEY_FINALMARKING = 264,
     KEY_FINALCONDITION = 265,
     KEY_TRANSITION = 266,
     KEY_CONSUME = 267,
     KEY_PRODUCE = 268,
     KEY_ALL_OTHER_PLACES_EMPTY = 269,
     KEY_ALL_OTHER_INTERNAL_PLACES_EMPTY = 270,
     KEY_ALL_OTHER_EXTERNAL_PLACES_EMPTY = 271,
     KEY_MAX_UNIQUE_EVENTS = 272,
     KEY_ON_LOOP = 273,
     KEY_MAX_OCCURRENCES = 274,
     KEY_TRUE = 275,
     KEY_FALSE = 276,
     LCONTROL = 277,
     RCONTROL = 278,
     COMMA = 279,
     COLON = 280,
     SEMICOLON = 281,
     IDENT = 282,
     NUMBER = 283,
     NEGATIVE_NUMBER = 284,
     LPAR = 285,
     RPAR = 286,
     OP_OR = 287,
     OP_AND = 288,
     OP_NOT = 289,
     OP_LE = 290,
     OP_GE = 291,
     OP_LT = 292,
     OP_GT = 293,
     OP_NE = 294,
     OP_EQ = 295
   };
#endif
/* Tokens.  */
#define KEY_SAFE 258
#define KEY_PLACE 259
#define KEY_INTERNAL 260
#define KEY_INPUT 261
#define KEY_OUTPUT 262
#define KEY_MARKING 263
#define KEY_FINALMARKING 264
#define KEY_FINALCONDITION 265
#define KEY_TRANSITION 266
#define KEY_CONSUME 267
#define KEY_PRODUCE 268
#define KEY_ALL_OTHER_PLACES_EMPTY 269
#define KEY_ALL_OTHER_INTERNAL_PLACES_EMPTY 270
#define KEY_ALL_OTHER_EXTERNAL_PLACES_EMPTY 271
#define KEY_MAX_UNIQUE_EVENTS 272
#define KEY_ON_LOOP 273
#define KEY_MAX_OCCURRENCES 274
#define KEY_TRUE 275
#define KEY_FALSE 276
#define LCONTROL 277
#define RCONTROL 278
#define COMMA 279
#define COLON 280
#define SEMICOLON 281
#define IDENT 282
#define NUMBER 283
#define NEGATIVE_NUMBER 284
#define LPAR 285
#define RPAR 286
#define OP_OR 287
#define OP_AND 288
#define OP_NOT 289
#define OP_LE 290
#define OP_GE 291
#define OP_LT 292
#define OP_GT 293
#define OP_NE 294
#define OP_EQ 295




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 93 "syntax_owfn.yy"
{
    kc::casestring yt_casestring;
}
/* Line 1529 of yacc.c.  */
#line 133 "syntax_owfn.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE owfn_yylval;

