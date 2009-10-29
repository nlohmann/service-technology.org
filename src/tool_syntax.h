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
     SSHORT = 258,
     SSTRING = 259,
     KEY_PACKAGE = 260,
     KEY_VERSION = 261,
     KEY_PURPOSE = 262,
     KEY_USAGE = 263,
     KEY_DESCRIPTION = 264,
     KEY_ARGS = 265,
     KEY_OPTION = 266,
     KEY_FLAG = 267,
     KEY_SECTION = 268,
     KEY_SECTIONDESC = 269,
     KEY_TEXT = 270,
     KEY_TYPESTR = 271,
     KEY_REQUIRED = 272,
     KEY_OPTIONAL = 273,
     KEY_DEPENDON = 274,
     KEY_DEFAULT = 275,
     KEY_HIDDEN = 276,
     KEY_ARGOPTIONAL = 277,
     KEY_DETAILS = 278,
     KEY_VALUES = 279,
     KEY_MULTIPLE = 280,
     KEY_STRING = 281,
     KEY_INT = 282,
     KEY_SHORT = 283,
     KEY_LONG = 284,
     KEY_FLOAT = 285,
     KEY_DOUBLE = 286,
     KEY_LONGDOUBLE = 287,
     KEY_LONGLONG = 288,
     KEY_ON = 289,
     KEY_OFF = 290,
     ASSIGN = 291,
     COMMA = 292,
     CONTROL_IN = 293,
     CONTROL_OUT = 294
   };
#endif
/* Tokens.  */
#define SSHORT 258
#define SSTRING 259
#define KEY_PACKAGE 260
#define KEY_VERSION 261
#define KEY_PURPOSE 262
#define KEY_USAGE 263
#define KEY_DESCRIPTION 264
#define KEY_ARGS 265
#define KEY_OPTION 266
#define KEY_FLAG 267
#define KEY_SECTION 268
#define KEY_SECTIONDESC 269
#define KEY_TEXT 270
#define KEY_TYPESTR 271
#define KEY_REQUIRED 272
#define KEY_OPTIONAL 273
#define KEY_DEPENDON 274
#define KEY_DEFAULT 275
#define KEY_HIDDEN 276
#define KEY_ARGOPTIONAL 277
#define KEY_DETAILS 278
#define KEY_VALUES 279
#define KEY_MULTIPLE 280
#define KEY_STRING 281
#define KEY_INT 282
#define KEY_SHORT 283
#define KEY_LONG 284
#define KEY_FLOAT 285
#define KEY_DOUBLE 286
#define KEY_LONGDOUBLE 287
#define KEY_LONGLONG 288
#define KEY_ON 289
#define KEY_OFF 290
#define ASSIGN 291
#define COMMA 292
#define CONTROL_IN 293
#define CONTROL_OUT 294




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 31 "tool_syntax.yy"
{
  char * long_;
  char short_;
}
/* Line 1489 of yacc.c.  */
#line 132 "tool_syntax.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

