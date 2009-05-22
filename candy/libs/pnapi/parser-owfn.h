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
     KEY_INTERFACE = 259,
     KEY_PLACE = 260,
     KEY_INTERNAL = 261,
     KEY_INPUT = 262,
     KEY_OUTPUT = 263,
     KEY_SYNCHRONIZE = 264,
     KEY_SYNCHRONOUS = 265,
     KEY_CONSTRAIN = 266,
     KEY_MARKING = 267,
     KEY_FINALMARKING = 268,
     KEY_NOFINALMARKING = 269,
     KEY_FINALCONDITION = 270,
     KEY_TRANSITION = 271,
     KEY_CONSUME = 272,
     KEY_PRODUCE = 273,
     KEY_PORT = 274,
     KEY_PORTS = 275,
     KEY_ALL_PLACES_EMPTY = 276,
     KEY_COST = 277,
     KEY_ALL_OTHER_PLACES_EMPTY = 278,
     KEY_ALL_OTHER_INTERNAL_PLACES_EMPTY = 279,
     KEY_ALL_OTHER_EXTERNAL_PLACES_EMPTY = 280,
     KEY_MAX_UNIQUE_EVENTS = 281,
     KEY_ON_LOOP = 282,
     KEY_MAX_OCCURRENCES = 283,
     KEY_TRUE = 284,
     KEY_FALSE = 285,
     LCONTROL = 286,
     RCONTROL = 287,
     COMMA = 288,
     COLON = 289,
     SEMICOLON = 290,
     IDENT = 291,
     NUMBER = 292,
     NEGATIVE_NUMBER = 293,
     LPAR = 294,
     RPAR = 295,
     OP_NOT = 296,
     OP_AND = 297,
     OP_OR = 298,
     OP_LE = 299,
     OP_GE = 300,
     OP_LT = 301,
     OP_GT = 302,
     OP_NE = 303,
     OP_EQ = 304
   };
#endif
/* Tokens.  */
#define KEY_SAFE 258
#define KEY_INTERFACE 259
#define KEY_PLACE 260
#define KEY_INTERNAL 261
#define KEY_INPUT 262
#define KEY_OUTPUT 263
#define KEY_SYNCHRONIZE 264
#define KEY_SYNCHRONOUS 265
#define KEY_CONSTRAIN 266
#define KEY_MARKING 267
#define KEY_FINALMARKING 268
#define KEY_NOFINALMARKING 269
#define KEY_FINALCONDITION 270
#define KEY_TRANSITION 271
#define KEY_CONSUME 272
#define KEY_PRODUCE 273
#define KEY_PORT 274
#define KEY_PORTS 275
#define KEY_ALL_PLACES_EMPTY 276
#define KEY_COST 277
#define KEY_ALL_OTHER_PLACES_EMPTY 278
#define KEY_ALL_OTHER_INTERNAL_PLACES_EMPTY 279
#define KEY_ALL_OTHER_EXTERNAL_PLACES_EMPTY 280
#define KEY_MAX_UNIQUE_EVENTS 281
#define KEY_ON_LOOP 282
#define KEY_MAX_OCCURRENCES 283
#define KEY_TRUE 284
#define KEY_FALSE 285
#define LCONTROL 286
#define RCONTROL 287
#define COMMA 288
#define COLON 289
#define SEMICOLON 290
#define IDENT 291
#define NUMBER 292
#define NEGATIVE_NUMBER 293
#define LPAR 294
#define RPAR 295
#define OP_NOT 296
#define OP_AND 297
#define OP_OR 298
#define OP_LE 299
#define OP_GE 300
#define OP_LT 301
#define OP_GT 302
#define OP_NE 303
#define OP_EQ 304




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 58 "parser-owfn.yy"
{
  int yt_int;
  std::string * yt_string;
  pnapi::parser::owfn::Node * yt_node;
}
/* Line 1489 of yacc.c.  */
#line 153 "parser-owfn.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE pnapi_owfn_lval;

