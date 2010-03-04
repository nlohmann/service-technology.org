/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.3"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0

/* Substitute the variable and function names.  */
#define yyparse pnapi_owfn_yyparse
#define yylex   pnapi_owfn_yylex
#define yyerror pnapi_owfn_yyerror
#define yylval  pnapi_owfn_yylval
#define yychar  pnapi_owfn_yychar
#define yydebug pnapi_owfn_yydebug
#define yynerrs pnapi_owfn_yynerrs


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
     KEY_CONSTRAIN = 266,
     KEY_INITIALMARKING = 267,
     KEY_FINALMARKING = 268,
     KEY_NOFINALMARKING = 269,
     KEY_FINALCONDITION = 270,
     KEY_TRANSITION = 271,
     KEY_CONSUME = 272,
     KEY_PRODUCE = 273,
     KEY_PORT = 274,
     KEY_PORTS = 275,
     KEY_ROLES = 276,
     KEY_ALL_PLACES_EMPTY = 277,
     KEY_COST = 278,
     KEY_ALL_OTHER_PLACES_EMPTY = 279,
     KEY_ALL_OTHER_INTERNAL_PLACES_EMPTY = 280,
     KEY_ALL_OTHER_EXTERNAL_PLACES_EMPTY = 281,
     KEY_MAX_UNIQUE_EVENTS = 282,
     KEY_ON_LOOP = 283,
     KEY_MAX_OCCURRENCES = 284,
     KEY_TRUE = 285,
     KEY_FALSE = 286,
     LCONTROL = 287,
     RCONTROL = 288,
     COMMA = 289,
     COLON = 290,
     SEMICOLON = 291,
     IDENT = 292,
     NUMBER = 293,
     NEGATIVE_NUMBER = 294,
     LPAR = 295,
     RPAR = 296,
     OP_NOT = 297,
     OP_AND = 298,
     OP_OR = 299,
     OP_LE = 300,
     OP_GE = 301,
     OP_LT = 302,
     OP_GT = 303,
     OP_NE = 304,
     OP_EQ = 305
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
#define KEY_CONSTRAIN 266
#define KEY_INITIALMARKING 267
#define KEY_FINALMARKING 268
#define KEY_NOFINALMARKING 269
#define KEY_FINALCONDITION 270
#define KEY_TRANSITION 271
#define KEY_CONSUME 272
#define KEY_PRODUCE 273
#define KEY_PORT 274
#define KEY_PORTS 275
#define KEY_ROLES 276
#define KEY_ALL_PLACES_EMPTY 277
#define KEY_COST 278
#define KEY_ALL_OTHER_PLACES_EMPTY 279
#define KEY_ALL_OTHER_INTERNAL_PLACES_EMPTY 280
#define KEY_ALL_OTHER_EXTERNAL_PLACES_EMPTY 281
#define KEY_MAX_UNIQUE_EVENTS 282
#define KEY_ON_LOOP 283
#define KEY_MAX_OCCURRENCES 284
#define KEY_TRUE 285
#define KEY_FALSE 286
#define LCONTROL 287
#define RCONTROL 288
#define COMMA 289
#define COLON 290
#define SEMICOLON 291
#define IDENT 292
#define NUMBER 293
#define NEGATIVE_NUMBER 294
#define LPAR 295
#define RPAR 296
#define OP_NOT 297
#define OP_AND 298
#define OP_OR 299
#define OP_LE 300
#define OP_GE 301
#define OP_LT 302
#define OP_GT 303
#define OP_NE 304
#define OP_EQ 305




/* Copy the first part of user declarations.  */
#line 20 "parser-owfn.yy"


#include "parser.h"
#include <string>
#include <sstream>

#undef yylex
#undef yyparse
#undef yyerror

#define yyerror pnapi::parser::error
#define yylex pnapi::parser::owfn::lex
#define yylex_destory pnapi::parser::owfn::lex_destroy
#define yyparse pnapi::parser::owfn::parse


using namespace pnapi;
using namespace pnapi::parser::owfn;



/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 1
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 63 "parser-owfn.yy"
{
  int yt_int;
  pnapi::formula::Formula * yt_formula;
  char * yt_str;
}
/* Line 187 of yacc.c.  */
#line 231 "parser-owfn.cc"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 244 "parser-owfn.cc"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int i)
#else
static int
YYID (i)
    int i;
#endif
{
  return i;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   209

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  51
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  63
/* YYNRULES -- Number of rules.  */
#define YYNRULES  122
/* YYNRULES -- Number of states.  */
#define YYNSTATES  214

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   305

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     4,     9,    10,    18,    19,    25,    30,
      33,    34,    35,    40,    41,    42,    47,    48,    49,    54,
      57,    62,    63,    66,    70,    71,    72,    76,    77,    83,
      85,    87,    88,    92,    93,    98,   103,   108,   109,   115,
     116,   122,   123,   126,   127,   133,   134,   141,   143,   147,
     151,   153,   154,   161,   162,   170,   171,   172,   177,   179,
     183,   184,   188,   190,   194,   196,   198,   199,   202,   203,
     204,   205,   221,   222,   226,   227,   231,   233,   237,   239,
     241,   242,   244,   248,   252,   254,   255,   256,   261,   262,
     263,   268,   269,   270,   277,   278,   280,   284,   288,   290,
     291,   296,   298,   300,   304,   306,   309,   313,   317,   319,
     321,   323,   326,   330,   334,   338,   342,   346,   350,   354,
     358,   362,   366
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      52,     0,    -1,    -1,    53,    54,   104,    89,    -1,    -1,
       4,    79,     5,    86,    55,    64,    36,    -1,    -1,     5,
      56,    86,    57,    74,    -1,    62,    58,    60,    83,    -1,
      64,    36,    -1,    -1,    -1,     7,    59,    64,    36,    -1,
      -1,    -1,     8,    61,    64,    36,    -1,    -1,    -1,     6,
      63,    64,    36,    -1,    65,    66,    -1,    64,    36,    65,
      66,    -1,    -1,     3,    35,    -1,     3,    38,    35,    -1,
      -1,    -1,    69,    67,    70,    -1,    -1,    66,    34,    69,
      68,    70,    -1,    37,    -1,    38,    -1,    -1,    32,    71,
      33,    -1,    -1,    27,    50,    38,    71,    -1,    28,    50,
      30,    71,    -1,    28,    50,    31,    71,    -1,    -1,    29,
      50,    38,    72,    71,    -1,    -1,    29,    50,    39,    73,
      71,    -1,    -1,    20,    75,    -1,    -1,    69,    35,    76,
      78,    36,    -1,    -1,    75,    69,    35,    77,    78,    36,
      -1,    69,    -1,    78,    34,    69,    -1,    58,    60,    83,
      -1,    80,    -1,    -1,    19,    69,    81,    58,    60,    83,
      -1,    -1,    80,    19,    69,    82,    58,    60,    83,    -1,
      -1,    -1,    10,    84,    85,    36,    -1,    69,    -1,    85,
      34,    69,    -1,    -1,    21,    87,    36,    -1,    88,    -1,
      87,    34,    88,    -1,    37,    -1,    38,    -1,    -1,    89,
      90,    -1,    -1,    -1,    -1,    16,    69,    94,    91,    95,
      17,    92,    98,    36,    18,    93,    98,    36,   100,   102,
      -1,    -1,    23,    38,    36,    -1,    -1,    21,    96,    36,
      -1,    97,    -1,    96,    34,    97,    -1,    37,    -1,    38,
      -1,    -1,    99,    -1,    98,    34,    99,    -1,    69,    35,
      38,    -1,    69,    -1,    -1,    -1,     9,   101,    85,    36,
      -1,    -1,    -1,    11,   103,    85,    36,    -1,    -1,    -1,
      12,   105,   107,    36,   106,   109,    -1,    -1,   108,    -1,
     107,    34,   108,    -1,    69,    35,    38,    -1,    69,    -1,
      -1,    13,   110,   111,    36,    -1,   112,    -1,   107,    -1,
     111,    36,   107,    -1,    14,    -1,    15,    36,    -1,    15,
     113,    36,    -1,    40,   113,    41,    -1,    30,    -1,    31,
      -1,    22,    -1,    42,   113,    -1,   113,    44,   113,    -1,
     113,    43,   113,    -1,   113,    43,    24,    -1,   113,    43,
      25,    -1,   113,    43,    26,    -1,    69,    50,    38,    -1,
      69,    49,    38,    -1,    69,    47,    38,    -1,    69,    48,
      38,    -1,    69,    46,    38,    -1,    69,    45,    38,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,    83,    83,    83,   106,   105,   111,   111,   115,   116,
     119,   121,   121,   124,   126,   126,   129,   131,   131,   135,
     136,   139,   141,   142,   145,   148,   147,   155,   154,   164,
     173,   183,   185,   188,   190,   191,   192,   194,   193,   197,
     196,   203,   205,   209,   209,   210,   210,   214,   223,   237,
     238,   242,   242,   244,   244,   248,   250,   250,   255,   261,
     273,   275,   279,   280,   284,   285,   292,   294,   299,   306,
     313,   298,   323,   324,   327,   329,   333,   334,   338,   339,
     342,   344,   345,   349,   360,   373,   375,   375,   379,   381,
     381,   391,   392,   391,   395,   397,   398,   402,   416,   433,
     433,   436,   449,   455,   464,   465,   466,   470,   471,   472,
     473,   478,   483,   489,   495,   500,   504,   508,   515,   522,
     529,   536,   543
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "KEY_SAFE", "KEY_INTERFACE", "KEY_PLACE",
  "KEY_INTERNAL", "KEY_INPUT", "KEY_OUTPUT", "KEY_SYNCHRONIZE",
  "KEY_SYNCHRONOUS", "KEY_CONSTRAIN", "KEY_INITIALMARKING",
  "KEY_FINALMARKING", "KEY_NOFINALMARKING", "KEY_FINALCONDITION",
  "KEY_TRANSITION", "KEY_CONSUME", "KEY_PRODUCE", "KEY_PORT", "KEY_PORTS",
  "KEY_ROLES", "KEY_ALL_PLACES_EMPTY", "KEY_COST",
  "KEY_ALL_OTHER_PLACES_EMPTY", "KEY_ALL_OTHER_INTERNAL_PLACES_EMPTY",
  "KEY_ALL_OTHER_EXTERNAL_PLACES_EMPTY", "KEY_MAX_UNIQUE_EVENTS",
  "KEY_ON_LOOP", "KEY_MAX_OCCURRENCES", "KEY_TRUE", "KEY_FALSE",
  "LCONTROL", "RCONTROL", "COMMA", "COLON", "SEMICOLON", "IDENT", "NUMBER",
  "NEGATIVE_NUMBER", "LPAR", "RPAR", "OP_NOT", "OP_AND", "OP_OR", "OP_LE",
  "OP_GE", "OP_LT", "OP_GT", "OP_NE", "OP_EQ", "$accept", "petrinet", "@1",
  "places_ports", "@2", "@3", "typed_places", "input_places", "@4",
  "output_places", "@5", "internal_places", "@6", "places", "capacity",
  "place_list", "@7", "@8", "node_name", "controlcommands", "commands",
  "@9", "@10", "ports", "port_list", "@11", "@12", "port_participants",
  "interface", "port_list_new", "@13", "@14", "synchronous", "@15",
  "labels", "roles", "role_names", "role_name", "transitions",
  "transition", "@16", "@17", "@18", "transition_cost", "transition_roles",
  "transition_role_names", "transition_role_name", "arcs", "arc",
  "synchronize", "@19", "constrain", "@20", "markings", "@21", "@22",
  "marking_list", "marking", "final", "@23", "finalmarkings", "condition",
  "formula", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    51,    53,    52,    55,    54,    56,    54,    57,    57,
      58,    59,    58,    60,    61,    60,    62,    63,    62,    64,
      64,    65,    65,    65,    66,    67,    66,    68,    66,    69,
      69,    70,    70,    71,    71,    71,    71,    72,    71,    73,
      71,    74,    74,    76,    75,    77,    75,    78,    78,    79,
      79,    81,    80,    82,    80,    83,    84,    83,    85,    85,
      86,    86,    87,    87,    88,    88,    89,    89,    91,    92,
      93,    90,    94,    94,    95,    95,    96,    96,    97,    97,
      98,    98,    98,    99,    99,   100,   101,   100,   102,   103,
     102,   105,   106,   104,   107,   107,   107,   108,   108,   110,
     109,   109,   111,   111,   112,   112,   112,   113,   113,   113,
     113,   113,   113,   113,   113,   113,   113,   113,   113,   113,
     113,   113,   113
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     4,     0,     7,     0,     5,     4,     2,
       0,     0,     4,     0,     0,     4,     0,     0,     4,     2,
       4,     0,     2,     3,     0,     0,     3,     0,     5,     1,
       1,     0,     3,     0,     4,     4,     4,     0,     5,     0,
       5,     0,     2,     0,     5,     0,     6,     1,     3,     3,
       1,     0,     6,     0,     7,     0,     0,     4,     1,     3,
       0,     3,     1,     3,     1,     1,     0,     2,     0,     0,
       0,    15,     0,     3,     0,     3,     1,     3,     1,     1,
       0,     1,     3,     3,     1,     0,     0,     4,     0,     0,
       4,     0,     0,     6,     0,     1,     3,     3,     1,     0,
       4,     1,     1,     3,     1,     2,     3,     3,     1,     1,
       1,     2,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     0,     1,    10,     6,     0,    11,     0,    13,
       0,    50,    60,    91,    66,    21,    29,    30,    51,    14,
      55,    60,     0,     0,    16,    94,     3,     0,     0,    24,
      10,    21,    56,    49,     4,    53,    64,    65,     0,    62,
      17,    41,    10,     0,    98,     0,    95,     0,    67,    22,
       0,    12,    19,    25,    13,     0,     0,    21,    10,     0,
      61,    21,     0,     7,    13,    21,     0,     0,    92,    72,
      23,    24,     0,    31,    55,    15,    58,     0,     0,    13,
      63,     0,     0,    42,    55,    97,    96,     0,     0,    68,
      20,    27,    33,    26,    52,     0,    57,    21,    55,    18,
      43,     0,     8,    99,   104,     0,    93,   101,     0,    74,
      31,     0,     0,     0,     0,    59,    54,     0,    45,    94,
     110,   108,   109,   105,     0,     0,     0,     0,    73,     0,
       0,    28,     0,     0,     0,    32,    47,     0,     0,   102,
       0,     0,   111,     0,     0,     0,     0,     0,     0,   106,
       0,     0,    78,    79,     0,    76,    69,    33,    33,    33,
      37,    39,     0,    44,     0,    94,   107,   122,   121,   119,
     120,   118,   117,   114,   115,   116,   113,   112,     0,    75,
      80,    34,    35,    36,    33,    33,    48,    46,   103,    77,
      84,     0,    81,    38,    40,     0,     0,     0,    83,    82,
      70,    80,     0,    85,    86,    88,     0,    89,    71,     0,
       0,    87,     0,    90
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,     6,    57,    12,    41,     9,    15,    20,
      31,    42,    61,    28,    29,    52,    73,   110,   126,    93,
     114,   184,   185,    63,    83,   117,   138,   137,    10,    11,
      30,    58,    33,    56,    77,    24,    38,    39,    26,    48,
     109,   180,   201,    89,   130,   154,   155,   191,   192,   205,
     206,   208,   210,    14,    25,    87,    45,    46,   106,   119,
     140,   107,   127
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -198
static const yytype_int16 yypact[] =
{
    -198,    23,    90,  -198,    49,  -198,    19,  -198,   114,    45,
      62,    38,    55,  -198,  -198,    79,  -198,  -198,  -198,  -198,
      69,    55,   114,   118,    24,   114,    84,    34,    72,   114,
      98,    79,  -198,  -198,  -198,  -198,  -198,  -198,   -26,  -198,
    -198,   111,    98,    92,    80,    52,  -198,   114,  -198,  -198,
     115,    -1,   119,  -198,    45,   128,   114,    79,    98,   118,
    -198,    79,   114,  -198,    45,     4,   127,   114,  -198,    87,
    -198,   114,   114,   134,    69,    -1,  -198,    93,   131,    45,
    -198,   132,   135,   114,    69,  -198,  -198,    78,   133,  -198,
     119,  -198,    75,  -198,  -198,   114,  -198,     8,    69,    -1,
    -198,   138,  -198,  -198,  -198,    76,  -198,  -198,   139,   148,
     134,   126,   129,   136,   141,  -198,  -198,   114,  -198,   114,
    -198,  -198,  -198,  -198,    95,    95,    74,    30,  -198,   121,
     160,  -198,   140,   130,   124,  -198,  -198,   100,   114,   146,
     145,   -15,  -198,   144,   147,   149,   151,   152,   153,  -198,
      59,    95,  -198,  -198,   104,  -198,  -198,    75,    75,    75,
    -198,  -198,   114,  -198,   105,     3,  -198,  -198,  -198,  -198,
    -198,  -198,  -198,  -198,  -198,  -198,  -198,  -198,   121,  -198,
     114,  -198,  -198,  -198,    75,    75,  -198,  -198,   146,  -198,
     157,   108,  -198,  -198,  -198,   156,   114,   165,  -198,  -198,
    -198,   114,   109,   175,  -198,   184,   114,  -198,  -198,   112,
     114,  -198,   113,  -198
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -198,  -198,  -198,  -198,  -198,  -198,  -198,   -24,  -198,     1,
    -198,  -198,  -198,   -19,   -50,   125,  -198,  -198,    -8,    89,
    -114,  -198,  -198,  -198,  -198,  -198,  -198,    63,  -198,  -198,
    -198,  -198,   -52,  -198,  -197,   176,  -198,   150,  -198,  -198,
    -198,  -198,  -198,  -198,  -198,  -198,    22,     2,     9,  -198,
    -198,  -198,  -198,  -198,  -198,  -198,  -115,   137,  -198,  -198,
    -198,  -198,   -73
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -101
static const yytype_int16 yytable[] =
{
      18,    71,    27,  -100,   139,    43,    54,    27,    59,   209,
      60,    27,    55,   212,    35,    71,    -9,    44,    64,  -100,
      -5,    53,    94,     3,    -9,    71,   166,    27,   150,   151,
      40,    13,   102,   -21,    79,   -21,   -21,   -21,    78,    69,
      16,    17,    81,   181,   182,   183,   116,    71,    76,    71,
     188,   141,   142,    19,    82,    74,     7,    22,   -21,    44,
     -21,   -21,   -21,    53,    91,    84,   149,    21,     8,    49,
     193,   194,    50,   150,   151,   101,    23,   176,   177,    32,
      98,   120,    27,   173,   174,   175,    67,   115,    68,   121,
     122,   103,   104,   105,     4,     5,    16,    17,   120,   124,
      47,   125,   111,   112,   113,     7,   121,   122,    51,   136,
      88,    44,   123,    16,    17,    66,   124,   120,   125,   143,
     144,   145,   146,   147,   148,   121,   122,    95,    65,    96,
     136,    62,    16,    17,   162,   124,   163,   125,   178,   162,
     179,   187,   196,   196,   197,   203,    95,    95,   211,   213,
      70,    16,    17,    72,   186,    36,    37,    44,   152,   153,
     158,   159,   160,   161,    75,    85,    92,    97,    99,   129,
     100,   108,   190,   118,   135,   128,   132,   156,   157,   133,
      67,   165,   167,   200,   204,   168,   134,   169,   190,   170,
     171,   172,   195,   190,   198,   207,    90,    34,    76,   131,
     189,   164,    76,   202,    86,   199,     0,     0,     0,    80
};

static const yytype_int16 yycheck[] =
{
       8,    51,     3,     0,   119,    24,    30,     3,    34,   206,
      36,     3,    31,   210,    22,    65,    12,    25,    42,    16,
      12,    29,    74,     0,    20,    75,    41,     3,    43,    44,
       6,    12,    84,    34,    58,    36,    37,    38,    57,    47,
      37,    38,    61,   157,   158,   159,    98,    97,    56,    99,
     165,   124,   125,     8,    62,    54,     7,    19,    34,    67,
      36,    37,    38,    71,    72,    64,    36,     5,    19,    35,
     184,   185,    38,    43,    44,    83,    21,   150,   151,    10,
      79,    22,     3,    24,    25,    26,    34,    95,    36,    30,
      31,    13,    14,    15,     4,     5,    37,    38,    22,    40,
      16,    42,    27,    28,    29,     7,    30,    31,    36,   117,
      23,   119,    36,    37,    38,    35,    40,    22,    42,    45,
      46,    47,    48,    49,    50,    30,    31,    34,    36,    36,
     138,    20,    37,    38,    34,    40,    36,    42,    34,    34,
      36,    36,    34,    34,    36,    36,    34,    34,    36,    36,
      35,    37,    38,    34,   162,    37,    38,   165,    37,    38,
      30,    31,    38,    39,    36,    38,    32,    36,    36,    21,
      35,    38,   180,    35,    33,    36,    50,    17,    38,    50,
      34,    36,    38,    18,     9,    38,    50,    38,   196,    38,
      38,    38,    35,   201,    38,    11,    71,    21,   206,   110,
     178,   138,   210,   201,    67,   196,    -1,    -1,    -1,    59
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    52,    53,     0,     4,     5,    54,     7,    19,    58,
      79,    80,    56,    12,   104,    59,    37,    38,    69,     8,
      60,     5,    19,    21,    86,   105,    89,     3,    64,    65,
      81,    61,    10,    83,    86,    69,    37,    38,    87,    88,
       6,    57,    62,    64,    69,   107,   108,    16,    90,    35,
      38,    36,    66,    69,    58,    64,    84,    55,    82,    34,
      36,    63,    20,    74,    58,    36,    35,    34,    36,    69,
      35,    65,    34,    67,    60,    36,    69,    85,    64,    58,
      88,    64,    69,    75,    60,    38,   108,   106,    23,    94,
      66,    69,    32,    70,    83,    34,    36,    36,    60,    36,
      35,    69,    83,    13,    14,    15,   109,   112,    38,    91,
      68,    27,    28,    29,    71,    69,    83,    76,    35,   110,
      22,    30,    31,    36,    40,    42,    69,   113,    36,    21,
      95,    70,    50,    50,    50,    33,    69,    78,    77,   107,
     111,   113,   113,    45,    46,    47,    48,    49,    50,    36,
      43,    44,    37,    38,    96,    97,    17,    38,    30,    31,
      38,    39,    34,    36,    78,    36,    41,    38,    38,    38,
      38,    38,    38,    24,    25,    26,   113,   113,    34,    36,
      92,    71,    71,    71,    72,    73,    69,    36,   107,    97,
      69,    98,    99,    71,    71,    35,    34,    36,    38,    99,
      18,    93,    98,    36,     9,   100,   101,    11,   102,    85,
     103,    36,    85,    36
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *bottom, yytype_int16 *top)
#else
static void
yy_stack_print (bottom, top)
    yytype_int16 *bottom;
    yytype_int16 *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      fprintf (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      fprintf (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

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



/* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;
#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  yytype_int16 yyssa[YYINITDEPTH];
  yytype_int16 *yyss = yyssa;
  yytype_int16 *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     look-ahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to look-ahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 83 "parser-owfn.yy"
    { 
      // clean up from previous parsing
      port_ = "";
      places_.clear();
      synchronousLabels_.clear();
      constrains_.clear();
      placeSet_.clear();
    }
    break;

  case 3:
#line 92 "parser-owfn.yy"
    { 
      pnapi_owfn_yynet.setConstraintLabels(constrains_); 
      pnapi_owfn_yynet.setSynchronousLabels(synchronousLabels_);
    }
    break;

  case 4:
#line 106 "parser-owfn.yy"
    {
     placeType_ = Node::INTERNAL;
     port_ = "";
    }
    break;

  case 6:
#line 111 "parser-owfn.yy"
    {placeType_ = Node::INTERNAL;}
    break;

  case 11:
#line 121 "parser-owfn.yy"
    {placeType_ = Node::INPUT;}
    break;

  case 14:
#line 126 "parser-owfn.yy"
    {placeType_ = Node::OUTPUT;}
    break;

  case 17:
#line 131 "parser-owfn.yy"
    {placeType_ = Node::INTERNAL;}
    break;

  case 19:
#line 135 "parser-owfn.yy"
    { capacity_ = 0; }
    break;

  case 20:
#line 136 "parser-owfn.yy"
    { capacity_ = 0; }
    break;

  case 22:
#line 141 "parser-owfn.yy"
    {capacity_ = 1; }
    break;

  case 23:
#line 142 "parser-owfn.yy"
    {capacity_ = (yyvsp[(2) - (3)].yt_int);}
    break;

  case 25:
#line 148 "parser-owfn.yy"
    {
     check(places_[nodeName_.str()] == 0, "node name already used");
     place_ = & pnapi_owfn_yynet.createPlace(nodeName_.str(), placeType_, 0, capacity_, port_);
     places_[nodeName_.str()] = place_;
    }
    break;

  case 27:
#line 155 "parser-owfn.yy"
    {
     check(places_[nodeName_.str()] == 0, "node name already used");
     place_ = & pnapi_owfn_yynet.createPlace(nodeName_.str(), placeType_, 0, capacity_, port_);
     places_[nodeName_.str()] = place_;
    }
    break;

  case 29:
#line 165 "parser-owfn.yy"
    { 
      // clear stringstream
      nodeName_.str("");
      nodeName_.clear();

      nodeName_ << (yyvsp[(1) - (1)].yt_str);
      free((yyvsp[(1) - (1)].yt_str)); 
    }
    break;

  case 30:
#line 174 "parser-owfn.yy"
    { 
      // clear stringstream
      nodeName_.str("");
      nodeName_.clear();

      nodeName_ << (yyvsp[(1) - (1)].yt_int); 
    }
    break;

  case 37:
#line 194 "parser-owfn.yy"
    { place_->setMaxOccurrence((yyvsp[(3) - (3)].yt_int)); }
    break;

  case 39:
#line 197 "parser-owfn.yy"
    { place_->setMaxOccurrence((yyvsp[(3) - (3)].yt_int)); }
    break;

  case 43:
#line 209 "parser-owfn.yy"
    { port_ = nodeName_.str(); }
    break;

  case 45:
#line 210 "parser-owfn.yy"
    { port_ = nodeName_.str(); }
    break;

  case 47:
#line 215 "parser-owfn.yy"
    {
      Place* p = places_[nodeName_.str()];
      check(p != 0, "unknown place");
      check(p->getType() != Place::INTERNAL, "interface place expected");
      check(p->getPort().empty(), "place already assigned to port '" + p->getPort() +"'");
       
      p->setPort(port_);
    }
    break;

  case 48:
#line 224 "parser-owfn.yy"
    {
      Place* p = places_[nodeName_.str()];
      check(p != 0, "unknown place");
      check(p->getType() != Place::INTERNAL, "interface place expected");
      check(p->getPort().empty(), "place already assigned to port '" + p->getPort() +"'");

      p->setPort(port_);
    }
    break;

  case 51:
#line 242 "parser-owfn.yy"
    { port_ = nodeName_.str(); }
    break;

  case 53:
#line 244 "parser-owfn.yy"
    { port_ = nodeName_.str(); }
    break;

  case 56:
#line 250 "parser-owfn.yy"
    { labels_.clear(); checkLabels_ = false; }
    break;

  case 57:
#line 251 "parser-owfn.yy"
    { synchronousLabels_ = labels_; }
    break;

  case 58:
#line 256 "parser-owfn.yy"
    { 
      check(!(checkLabels_ && (synchronousLabels_.find(nodeName_.str()) == synchronousLabels_.end())),
             "undeclared label");
      labels_.insert(nodeName_.str()); 
    }
    break;

  case 59:
#line 262 "parser-owfn.yy"
    {
      check(!(checkLabels_ && (synchronousLabels_.find(nodeName_.str()) == synchronousLabels_.end())),
             "undeclared label"); 
      labels_.insert(nodeName_.str()); 
    }
    break;

  case 64:
#line 284 "parser-owfn.yy"
    {pnapi_owfn_yynet.addRole((yyvsp[(1) - (1)].yt_str));}
    break;

  case 65:
#line 285 "parser-owfn.yy"
    {pnapi_owfn_yynet.addRole((char*) (yyvsp[(1) - (1)].yt_int));}
    break;

  case 68:
#line 299 "parser-owfn.yy"
    { 
      check(!pnapi_owfn_yynet.containsNode(nodeName_.str()), "node name already used");
      transition_ = & pnapi_owfn_yynet.createTransition(nodeName_.str()); 
      transition_->setCost((yyvsp[(3) - (3)].yt_int));
    }
    break;

  case 69:
#line 306 "parser-owfn.yy"
    { 
      target_ = (Node**)(&transition_); source_ = (Node**)(&place_); 
      placeSet_.clear();
      placeSetType_ = true;
    }
    break;

  case 70:
#line 313 "parser-owfn.yy"
    { 
      source_ = (Node**)(&transition_); target_ = (Node**)(&place_); 
      placeSet_.clear();
      placeSetType_ = false;
    }
    break;

  case 72:
#line 323 "parser-owfn.yy"
    { (yyval.yt_int) = 0;  }
    break;

  case 73:
#line 324 "parser-owfn.yy"
    { (yyval.yt_int) = (yyvsp[(2) - (3)].yt_int); }
    break;

  case 78:
#line 338 "parser-owfn.yy"
    {check(transition_->getPetriNet().isRoleSpecified((yyvsp[(1) - (1)].yt_str)),"role has not been specified");transition_->addRole((yyvsp[(1) - (1)].yt_str));}
    break;

  case 79:
#line 339 "parser-owfn.yy"
    {check(transition_->getPetriNet().isRoleSpecified((char*) (yyvsp[(1) - (1)].yt_int)),"role has not been specified");transition_->addRole((char*) (yyvsp[(1) - (1)].yt_int));}
    break;

  case 83:
#line 350 "parser-owfn.yy"
    {
      place_ = places_[nodeName_.str()];
      check(place_ != 0, "unknown place");
      check(placeSet_.find(place_) == placeSet_.end(), placeSetType_ ? "place already in preset" : "place already in postset");
      check(!(placeSetType_ && (place_->getType() == Place::OUTPUT)), "output places are not allowed in preset");
      check(!(!placeSetType_ && (place_->getType() == Place::INPUT)), "input places are not allowed in postset");
      
      pnapi_owfn_yynet.createArc(**source_, **target_, (yyvsp[(3) - (3)].yt_int));
      placeSet_.insert(place_);
    }
    break;

  case 84:
#line 361 "parser-owfn.yy"
    {
      place_ = places_[nodeName_.str()];
      check(place_ != 0, "unknown place");
      check(placeSet_.find(place_) == placeSet_.end(), placeSetType_ ? "place already in preset" : "place already in postset");
      check(!(placeSetType_ && (place_->getType() == Place::OUTPUT)), "output places are not allowed in preset");
      check(!(!placeSetType_ && (place_->getType() == Place::INPUT)), "input places are not allowed in postset");
      
      pnapi_owfn_yynet.createArc(**source_, **target_);
      placeSet_.insert(place_);
    }
    break;

  case 86:
#line 375 "parser-owfn.yy"
    { labels_.clear(); checkLabels_ = true; }
    break;

  case 87:
#line 376 "parser-owfn.yy"
    { transition_->setSynchronizeLabels(labels_); }
    break;

  case 89:
#line 381 "parser-owfn.yy"
    { labels_.clear(); checkLabels_ = false; }
    break;

  case 90:
#line 382 "parser-owfn.yy"
    { constrains_[transition_] = labels_; }
    break;

  case 91:
#line 391 "parser-owfn.yy"
    { markInitial_ = true; }
    break;

  case 92:
#line 392 "parser-owfn.yy"
    {markInitial_ = false; }
    break;

  case 97:
#line 403 "parser-owfn.yy"
    { 
      Place* p = places_[nodeName_.str()];
      check(p != 0, "unknown place");      
      
      if(markInitial_)
      {  
        p->mark((yyvsp[(3) - (3)].yt_int));
      }
      else
      {
        (*finalMarking_)[*p] = (yyvsp[(3) - (3)].yt_int);
      }
    }
    break;

  case 98:
#line 417 "parser-owfn.yy"
    { 
      Place* p = places_[nodeName_.str()];
      check(p != 0, "unknown place");
      
      if(markInitial_)
      {  
        p->mark(1);
      }
      else
      {
        (*finalMarking_)[*p] = 1;
      }
    }
    break;

  case 99:
#line 433 "parser-owfn.yy"
    { finalMarking_ = new Marking(pnapi_owfn_yynet, true); }
    break;

  case 100:
#line 435 "parser-owfn.yy"
    { delete finalMarking_; finalMarking_ = NULL; }
    break;

  case 101:
#line 437 "parser-owfn.yy"
    {
      pnapi_owfn_yynet.finalCondition() = (*(yyvsp[(1) - (1)].yt_formula));
      if(wildcardGiven_)
      {
        wildcardGiven_ = false;
        pnapi_owfn_yynet.finalCondition().allOtherPlacesEmpty(pnapi_owfn_yynet);
      }
      delete (yyvsp[(1) - (1)].yt_formula); 
    }
    break;

  case 102:
#line 450 "parser-owfn.yy"
    {
      pnapi_owfn_yynet.finalCondition().addMarking(*finalMarking_);
      delete finalMarking_;
      finalMarking_ = new Marking(pnapi_owfn_yynet, true);
    }
    break;

  case 103:
#line 456 "parser-owfn.yy"
    {
      pnapi_owfn_yynet.finalCondition().addMarking(*finalMarking_);
      delete finalMarking_;
      finalMarking_ = new Marking(pnapi_owfn_yynet, true);
    }
    break;

  case 104:
#line 464 "parser-owfn.yy"
    { (yyval.yt_formula) = new formula::FormulaTrue(); }
    break;

  case 105:
#line 465 "parser-owfn.yy"
    { (yyval.yt_formula) = new formula::FormulaFalse(); }
    break;

  case 106:
#line 466 "parser-owfn.yy"
    { (yyval.yt_formula) = (yyvsp[(2) - (3)].yt_formula); }
    break;

  case 107:
#line 470 "parser-owfn.yy"
    { (yyval.yt_formula) = (yyvsp[(2) - (3)].yt_formula); }
    break;

  case 108:
#line 471 "parser-owfn.yy"
    { (yyval.yt_formula) = new formula::FormulaTrue(); }
    break;

  case 109:
#line 472 "parser-owfn.yy"
    { (yyval.yt_formula) = new formula::FormulaFalse(); }
    break;

  case 110:
#line 474 "parser-owfn.yy"
    { 
      wildcardGiven_ = true;
      (yyval.yt_formula) = new formula::FormulaTrue();
    }
    break;

  case 111:
#line 479 "parser-owfn.yy"
    { 
      (yyval.yt_formula) = new formula::Negation(*(yyvsp[(2) - (2)].yt_formula));
      delete (yyvsp[(2) - (2)].yt_formula);
    }
    break;

  case 112:
#line 484 "parser-owfn.yy"
    {
      (yyval.yt_formula) = new formula::Disjunction(*(yyvsp[(1) - (3)].yt_formula), *(yyvsp[(3) - (3)].yt_formula));
      delete (yyvsp[(1) - (3)].yt_formula);
      delete (yyvsp[(3) - (3)].yt_formula);
    }
    break;

  case 113:
#line 490 "parser-owfn.yy"
    {
      (yyval.yt_formula) = new formula::Conjunction(*(yyvsp[(1) - (3)].yt_formula), *(yyvsp[(3) - (3)].yt_formula));
      delete (yyvsp[(1) - (3)].yt_formula);
      delete (yyvsp[(3) - (3)].yt_formula);
    }
    break;

  case 114:
#line 496 "parser-owfn.yy"
    {
      wildcardGiven_ = true;
      (yyval.yt_formula) = (yyvsp[(1) - (3)].yt_formula);
    }
    break;

  case 115:
#line 501 "parser-owfn.yy"
    {
      (yyval.yt_formula) = (yyvsp[(1) - (3)].yt_formula); // obsolete; kept due to compatibility
    }
    break;

  case 116:
#line 505 "parser-owfn.yy"
    {
      (yyval.yt_formula) = (yyvsp[(1) - (3)].yt_formula); // obsolete; kept due to compatibility
    }
    break;

  case 117:
#line 509 "parser-owfn.yy"
    {
      Place* p = places_[nodeName_.str()];
      check(p != 0, "unknown place");
      
      (yyval.yt_formula) = new formula::FormulaEqual(*p, (yyvsp[(3) - (3)].yt_int));
    }
    break;

  case 118:
#line 516 "parser-owfn.yy"
    {
      Place* p = places_[nodeName_.str()];
      check(p != 0, "unknown place");

      (yyval.yt_formula) = new formula::Negation(formula::FormulaEqual(*p, (yyvsp[(3) - (3)].yt_int)));
    }
    break;

  case 119:
#line 523 "parser-owfn.yy"
    {
      Place* p = places_[nodeName_.str()];
      check(p != 0, "unknown place");

      (yyval.yt_formula) = new formula::FormulaLess(*p, (yyvsp[(3) - (3)].yt_int));
    }
    break;

  case 120:
#line 530 "parser-owfn.yy"
    {
      Place* p = places_[nodeName_.str()];
      check(p != 0, "unknown place");

      (yyval.yt_formula) = new formula::FormulaGreater(*p, (yyvsp[(3) - (3)].yt_int));
    }
    break;

  case 121:
#line 537 "parser-owfn.yy"
    {
      Place* p = places_[nodeName_.str()];
      check(p != 0, "unknown place");

      (yyval.yt_formula) = new formula::FormulaGreaterEqual(*p, (yyvsp[(3) - (3)].yt_int));
    }
    break;

  case 122:
#line 544 "parser-owfn.yy"
    {
      Place* p = places_[nodeName_.str()];
      check(p != 0, "unknown place");

      (yyval.yt_formula) = new formula::FormulaLessEqual(*p, (yyvsp[(3) - (3)].yt_int));
    }
    break;


/* Line 1267 of yacc.c.  */
#line 2180 "parser-owfn.cc"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



