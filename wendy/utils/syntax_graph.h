
/* A Bison parser, made by GNU Bison 2.4.1.  */

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
     KW_STATE = 258,
     KW_PROG = 259,
     KW_LOWLINK = 260,
     KW_SCC = 261,
     COLON = 262,
     COMMA = 263,
     ARROW = 264,
     INDENT = 265,
     BANG = 266,
     STAR = 267,
     QUESTION = 268,
     NUMBER = 269,
     NAME = 270,
     MESSAGE = 271,
     PATHARROW = 272
   };
#endif
/* Tokens.  */
#define KW_STATE 258
#define KW_PROG 259
#define KW_LOWLINK 260
#define KW_SCC 261
#define COLON 262
#define COMMA 263
#define ARROW 264
#define INDENT 265
#define BANG 266
#define STAR 267
#define QUESTION 268
#define NUMBER 269
#define NAME 270
#define MESSAGE 271
#define PATHARROW 272




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1676 of yacc.c  */
#line 33 "syntax_graph.yy"

  unsigned int val;
  char *name;



/* Line 1676 of yacc.c  */
#line 93 "syntax_graph.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE graph_lval;


