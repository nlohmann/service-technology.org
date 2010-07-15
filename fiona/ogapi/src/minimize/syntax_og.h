
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
     KEY_NODES = 258,
     KEY_INTERFACE = 259,
     KEY_INPUT = 260,
     KEY_OUTPUT = 261,
     KEY_SYNCHRONOUS = 262,
     COMMA = 263,
     COLON = 264,
     DOUBLECOLON = 265,
     SEMICOLON = 266,
     IDENT = 267,
     ARROW = 268,
     NUMBER = 269,
     KEY_TRUE = 270,
     KEY_FALSE = 271,
     KEY_FINAL = 272,
     BIT_F = 273,
     BIT_S = 274,
     BIT_T = 275,
     LPAR = 276,
     RPAR = 277,
     OP_OR = 278,
     OP_AND = 279,
     OP_NOT = 280
   };
#endif
/* Tokens.  */
#define KEY_NODES 258
#define KEY_INTERFACE 259
#define KEY_INPUT 260
#define KEY_OUTPUT 261
#define KEY_SYNCHRONOUS 262
#define COMMA 263
#define COLON 264
#define DOUBLECOLON 265
#define SEMICOLON 266
#define IDENT 267
#define ARROW 268
#define NUMBER 269
#define KEY_TRUE 270
#define KEY_FALSE 271
#define KEY_FINAL 272
#define BIT_F 273
#define BIT_S 274
#define BIT_T 275
#define LPAR 276
#define RPAR 277
#define OP_OR 278
#define OP_AND 279
#define OP_NOT 280




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1676 of yacc.c  */
#line 49 "syntax_og.yy"

    char *str;
    unsigned int value;
    Formula *f;
    Node *n;



/* Line 1676 of yacc.c  */
#line 111 "syntax_og.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE og_yylval;


