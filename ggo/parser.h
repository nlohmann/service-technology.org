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
     KEY_PACKAGE = 258,
     KEY_VERSION = 259,
     KEY_PURPOSE = 260,
     KEY_USAGE = 261,
     KEY_DESCRIPTION = 262,
     KEY_ARGS = 263,
     KEY_OPTION = 264,
     KEY_FLAG = 265,
     KEY_SECTION = 266,
     KEY_SECTIONDESC = 267,
     KEY_TEXT = 268,
     KEY_TYPESTR = 269,
     KEY_REQUIRED = 270,
     KEY_OPTIONAL = 271,
     KEY_DEPENDON = 272,
     KEY_DEFAULT = 273,
     KEY_HIDDEN = 274,
     KEY_ARGOPTIONAL = 275,
     KEY_DETAILS = 276,
     KEY_VALUES = 277,
     KEY_MULTIPLE = 278,
     KEY_STRING = 279,
     KEY_INT = 280,
     KEY_SHORT = 281,
     KEY_LONG = 282,
     KEY_FLOAT = 283,
     KEY_DOUBLE = 284,
     KEY_LONGDOUBLE = 285,
     KEY_LONGLONG = 286,
     KEY_ON = 287,
     KEY_OFF = 288,
     ASSIGN = 289,
     COMMA = 290,
     CONTROL = 291,
     STRING = 292,
     SHORT = 293
   };
#endif
/* Tokens.  */
#define KEY_PACKAGE 258
#define KEY_VERSION 259
#define KEY_PURPOSE 260
#define KEY_USAGE 261
#define KEY_DESCRIPTION 262
#define KEY_ARGS 263
#define KEY_OPTION 264
#define KEY_FLAG 265
#define KEY_SECTION 266
#define KEY_SECTIONDESC 267
#define KEY_TEXT 268
#define KEY_TYPESTR 269
#define KEY_REQUIRED 270
#define KEY_OPTIONAL 271
#define KEY_DEPENDON 272
#define KEY_DEFAULT 273
#define KEY_HIDDEN 274
#define KEY_ARGOPTIONAL 275
#define KEY_DETAILS 276
#define KEY_VALUES 277
#define KEY_MULTIPLE 278
#define KEY_STRING 279
#define KEY_INT 280
#define KEY_SHORT 281
#define KEY_LONG 282
#define KEY_FLOAT 283
#define KEY_DOUBLE 284
#define KEY_LONGDOUBLE 285
#define KEY_LONGLONG 286
#define KEY_ON 287
#define KEY_OFF 288
#define ASSIGN 289
#define COMMA 290
#define CONTROL 291
#define STRING 292
#define SHORT 293




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

