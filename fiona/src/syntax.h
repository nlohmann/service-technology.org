/* A Bison parser, made by GNU Bison 2.1.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005 Free Software Foundation, Inc.

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

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     key_safe = 258,
     key_place = 259,
     key_internal = 260,
     key_input = 261,
     key_output = 262,
     key_marking = 263,
     key_finalmarking = 264,
     key_finalcondition = 265,
     key_transition = 266,
     key_consume = 267,
     key_produce = 268,
     comma = 269,
     colon = 270,
     semicolon = 271,
     ident = 272,
     number = 273,
     op_and = 274,
     op_or = 275,
     op_not = 276,
     op_gt = 277,
     op_lt = 278,
     op_ge = 279,
     op_le = 280,
     op_eq = 281,
     op_ne = 282,
     lpar = 283,
     rpar = 284
   };
#endif
/* Tokens.  */
#define key_safe 258
#define key_place 259
#define key_internal 260
#define key_input 261
#define key_output 262
#define key_marking 263
#define key_finalmarking 264
#define key_finalcondition 265
#define key_transition 266
#define key_consume 267
#define key_produce 268
#define comma 269
#define colon 270
#define semicolon 271
#define ident 272
#define number 273
#define op_and 274
#define op_or 275
#define op_not 276
#define op_gt 277
#define op_lt 278
#define op_ge 279
#define op_le 280
#define op_eq 281
#define op_ne 282
#define lpar 283
#define rpar 284




#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 92 "syntax.yy"
typedef union YYSTYPE {
	char * str;
	int value;
	int * exl;
	arc_list * al;
	IdList * idl;
	owfnPlace * pl;
	owfnTransition * tr;
	TrSymbol * ts;
	formula * form;
} YYSTYPE;
/* Line 1447 of yacc.c.  */
#line 108 "syntax.h"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;



