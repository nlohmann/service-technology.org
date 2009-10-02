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
     LPAR = 258,
     RPAR = 259,
     NUMBER = 260,
     SEMICOLON = 261,
     ADD = 262,
     MULT = 263,
     IDENT = 264,
     FIDENT = 265,
     CIDENT = 266,
     MINUS = 267,
     MEQ = 268,
     GT = 269,
     LT = 270,
     COMMA = 271,
     COLON = 272,
     MUNBOUNDED = 273,
     FM = 274,
     KEY_FINALMARKING = 275,
     KEY_PLACE = 276,
     KEY_INTERNAL = 277,
     KEY_INPUT = 278,
     KEY_OUTPUT = 279,
     KEY_TERMS = 280,
     KEY_BOUNDS = 281,
     KEY_SYNCHRONOUS = 282
   };
#endif
/* Tokens.  */
#define LPAR 258
#define RPAR 259
#define NUMBER 260
#define SEMICOLON 261
#define ADD 262
#define MULT 263
#define IDENT 264
#define FIDENT 265
#define CIDENT 266
#define MINUS 267
#define MEQ 268
#define GT 269
#define LT 270
#define COMMA 271
#define COLON 272
#define MUNBOUNDED 273
#define FM 274
#define KEY_FINALMARKING 275
#define KEY_PLACE 276
#define KEY_INTERNAL 277
#define KEY_INPUT 278
#define KEY_OUTPUT 279
#define KEY_TERMS 280
#define KEY_BOUNDS 281
#define KEY_SYNCHRONOUS 282




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 68 "syntax_mp.yy"
{
  int yt_int;
  std::string* yt_string;
  EventTerm* yt_term;
}
/* Line 1489 of yacc.c.  */
#line 109 "syntax_mp.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE mp_yylval;

