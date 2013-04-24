/* A Bison parser, made by GNU Bison 2.7.12-4996.  */

/* Bison interface for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2013 Free Software Foundation, Inc.
   
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

#ifndef YY_YY_PARSER_H_INCLUDED
# define YY_YY_PARSER_H_INCLUDED
/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     _REACH = 258,
     _STRUCT = 259,
     _CTL = 260,
     _LTL = 261,
     _BOUND = 262,
     _MARKING = 263,
     _DEADLOCK = 264,
     _TRUE = 265,
     _FALSE = 266,
     _TILDE = 267,
     _QUESTIONMARK = 268,
     _LIVENESS0 = 269,
     _LIVENESS1 = 270,
     _LIVENESS2 = 271,
     _LIVENESS3 = 272,
     _LIVENESS4 = 273,
     _COMMA = 274,
     _COLON = 275,
     _LPAREN = 276,
     _RPAREN = 277,
     _EOL = 278,
     _XOR = 279,
     _OR = 280,
     _AND = 281,
     _IMPLY = 282,
     _EQUIV = 283,
     _NEQ = 284,
     _EQ = 285,
     _GEQ = 286,
     _GE = 287,
     _LEQ = 288,
     _LE = 289,
     _MINUS = 290,
     _PLUS = 291,
     _DIV = 292,
     _MULT = 293,
     _NEGATION = 294,
     _NEXTSTATE = 295,
     _EVENTUALLY = 296,
     _ALWAYS = 297,
     _WUNTIL = 298,
     _SUNTIL = 299,
     _EXPATH = 300,
     _ALLPATH = 301,
     _IMPOSSIBILITY = 302,
     _INVARIANT = 303,
     _INTEGER = 304,
     _IDENTIFIER = 305,
     _QUOTED = 306
   };
#endif


#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */

#endif /* !YY_YY_PARSER_H_INCLUDED  */
