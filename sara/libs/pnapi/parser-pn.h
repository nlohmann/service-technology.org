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
     PLACENAME = 258,
     TRANSITIONNAME = 259,
     IDENTIFIER = 260,
     WEIGHT = 261,
     K_MODEL = 262,
     K_DUMMY = 263,
     K_OUTPUTS = 264,
     K_GRAPH = 265,
     K_MARKING = 266,
     K_END = 267,
     NEWLINE = 268,
     OPENBRACE = 269,
     CLOSEBRACE = 270,
     LPAR = 271,
     RPAR = 272
   };
#endif
/* Tokens.  */
#define PLACENAME 258
#define TRANSITIONNAME 259
#define IDENTIFIER 260
#define WEIGHT 261
#define K_MODEL 262
#define K_DUMMY 263
#define K_OUTPUTS 264
#define K_GRAPH 265
#define K_MARKING 266
#define K_END 267
#define NEWLINE 268
#define OPENBRACE 269
#define CLOSEBRACE 270
#define LPAR 271
#define RPAR 272




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 81 "parser-pn.yy"
{
  char * yt_str;
  unsigned int yt_uInt;
}
/* Line 1489 of yacc.c.  */
#line 88 "parser-pn.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE pnapi_pn_yylval;

