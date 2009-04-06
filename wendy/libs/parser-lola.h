
/* A Bison parser, made by GNU Bison 2.4.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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
     KEY_MARKING = 266,
     KEY_FINALMARKING = 267,
     KEY_NOFINALMARKING = 268,
     KEY_FINALCONDITION = 269,
     KEY_TRANSITION = 270,
     KEY_CONSUME = 271,
     KEY_PRODUCE = 272,
     KEY_PORT = 273,
     KEY_PORTS = 274,
     KEY_ALL_PLACES_EMPTY = 275,
     KEY_ALL_OTHER_PLACES_EMPTY = 276,
     KEY_ALL_OTHER_INTERNAL_PLACES_EMPTY = 277,
     KEY_ALL_OTHER_EXTERNAL_PLACES_EMPTY = 278,
     KEY_MAX_UNIQUE_EVENTS = 279,
     KEY_ON_LOOP = 280,
     KEY_MAX_OCCURRENCES = 281,
     KEY_TRUE = 282,
     KEY_FALSE = 283,
     LCONTROL = 284,
     RCONTROL = 285,
     COMMA = 286,
     COLON = 287,
     SEMICOLON = 288,
     IDENT = 289,
     NUMBER = 290,
     NEGATIVE_NUMBER = 291,
     LPAR = 292,
     RPAR = 293,
     OP_NOT = 294,
     OP_AND = 295,
     OP_OR = 296,
     OP_LE = 297,
     OP_GE = 298,
     OP_LT = 299,
     OP_GT = 300,
     OP_NE = 301,
     OP_EQ = 302
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
#define KEY_MARKING 266
#define KEY_FINALMARKING 267
#define KEY_NOFINALMARKING 268
#define KEY_FINALCONDITION 269
#define KEY_TRANSITION 270
#define KEY_CONSUME 271
#define KEY_PRODUCE 272
#define KEY_PORT 273
#define KEY_PORTS 274
#define KEY_ALL_PLACES_EMPTY 275
#define KEY_ALL_OTHER_PLACES_EMPTY 276
#define KEY_ALL_OTHER_INTERNAL_PLACES_EMPTY 277
#define KEY_ALL_OTHER_EXTERNAL_PLACES_EMPTY 278
#define KEY_MAX_UNIQUE_EVENTS 279
#define KEY_ON_LOOP 280
#define KEY_MAX_OCCURRENCES 281
#define KEY_TRUE 282
#define KEY_FALSE 283
#define LCONTROL 284
#define RCONTROL 285
#define COMMA 286
#define COLON 287
#define SEMICOLON 288
#define IDENT 289
#define NUMBER 290
#define NEGATIVE_NUMBER 291
#define LPAR 292
#define RPAR 293
#define OP_NOT 294
#define OP_AND 295
#define OP_OR 296
#define OP_LE 297
#define OP_GE 298
#define OP_LT 299
#define OP_GT 300
#define OP_NE 301
#define OP_EQ 302




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1740 of yacc.c  */
#line 58 "parser-lola.yy"

  int yt_int;
  std::string * yt_string;
  pnapi::parser::owfn::Node * yt_node;



/* Line 1740 of yacc.c  */
#line 154 "parser-lola.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE pnapi_lola_lval;


