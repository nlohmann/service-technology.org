
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
     KEY_INITIALNODES = 262,
     KEY_GLOBALFORMULA = 263,
     KEY_TAULOOPS = 264,
     COMMA = 265,
     COLON = 266,
     SEMICOLON = 267,
     IDENT = 268,
     ARROW = 269,
     NUMBER = 270,
     KEY_TRUE = 271,
     KEY_FALSE = 272,
     KEY_FINAL = 273,
     BIT_F = 274,
     BIT_S = 275,
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
#define KEY_INITIALNODES 262
#define KEY_GLOBALFORMULA 263
#define KEY_TAULOOPS 264
#define COMMA 265
#define COLON 266
#define SEMICOLON 267
#define IDENT 268
#define ARROW 269
#define NUMBER 270
#define KEY_TRUE 271
#define KEY_FALSE 272
#define KEY_FINAL 273
#define BIT_F 274
#define BIT_S 275
#define LPAR 276
#define RPAR 277
#define OP_OR 278
#define OP_AND 279
#define OP_NOT 280




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1676 of yacc.c  */
#line 75 "og_syntax.yy"

    char *str;
    int value;
    Formula *f;
//    Node *n;   



/* Line 1676 of yacc.c  */
#line 111 "og_syntax.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE og_yylval;


