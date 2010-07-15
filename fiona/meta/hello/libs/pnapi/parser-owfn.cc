
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
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
#define YYBISON_VERSION "2.4.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0

/* Substitute the variable and function names.  */
#define yyparse         pnapi_owfn_yyparse
#define yylex           pnapi_owfn_yylex
#define yyerror         pnapi_owfn_yyerror
#define yylval          pnapi_owfn_yylval
#define yychar          pnapi_owfn_yychar
#define yydebug         pnapi_owfn_yydebug
#define yynerrs         pnapi_owfn_yynerrs


/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
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



/* Line 189 of yacc.c  */
#line 103 "parser-owfn.cc"

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
     KEY_ALL_PLACES_EMPTY = 276,
     KEY_COST = 277,
     KEY_ALL_OTHER_PLACES_EMPTY = 278,
     KEY_ALL_OTHER_INTERNAL_PLACES_EMPTY = 279,
     KEY_ALL_OTHER_EXTERNAL_PLACES_EMPTY = 280,
     KEY_MAX_UNIQUE_EVENTS = 281,
     KEY_ON_LOOP = 282,
     KEY_MAX_OCCURRENCES = 283,
     KEY_TRUE = 284,
     KEY_FALSE = 285,
     LCONTROL = 286,
     RCONTROL = 287,
     COMMA = 288,
     COLON = 289,
     SEMICOLON = 290,
     IDENT = 291,
     NUMBER = 292,
     NEGATIVE_NUMBER = 293,
     LPAR = 294,
     RPAR = 295,
     OP_NOT = 296,
     OP_AND = 297,
     OP_OR = 298,
     OP_LE = 299,
     OP_GE = 300,
     OP_LT = 301,
     OP_GT = 302,
     OP_NE = 303,
     OP_EQ = 304
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
#define KEY_ALL_PLACES_EMPTY 276
#define KEY_COST 277
#define KEY_ALL_OTHER_PLACES_EMPTY 278
#define KEY_ALL_OTHER_INTERNAL_PLACES_EMPTY 279
#define KEY_ALL_OTHER_EXTERNAL_PLACES_EMPTY 280
#define KEY_MAX_UNIQUE_EVENTS 281
#define KEY_ON_LOOP 282
#define KEY_MAX_OCCURRENCES 283
#define KEY_TRUE 284
#define KEY_FALSE 285
#define LCONTROL 286
#define RCONTROL 287
#define COMMA 288
#define COLON 289
#define SEMICOLON 290
#define IDENT 291
#define NUMBER 292
#define NEGATIVE_NUMBER 293
#define LPAR 294
#define RPAR 295
#define OP_NOT 296
#define OP_AND 297
#define OP_OR 298
#define OP_LE 299
#define OP_GE 300
#define OP_LT 301
#define OP_GT 302
#define OP_NE 303
#define OP_EQ 304




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 63 "parser-owfn.yy"

  int yt_int;
  pnapi::formula::Formula * yt_formula;
  char * yt_str;



/* Line 214 of yacc.c  */
#line 245 "parser-owfn.cc"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 257 "parser-owfn.cc"

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
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
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
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
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
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   198

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  50
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  57
/* YYNRULES -- Number of rules.  */
#define YYNRULES  110
/* YYNRULES -- Number of states.  */
#define YYNSTATES  195

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   304

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
      45,    46,    47,    48,    49
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     4,     9,    10,    17,    18,    23,    28,
      31,    32,    33,    38,    39,    40,    45,    46,    47,    52,
      55,    60,    61,    64,    68,    69,    70,    74,    75,    81,
      83,    85,    86,    90,    91,    96,   101,   106,   107,   113,
     114,   120,   121,   124,   125,   131,   132,   139,   141,   145,
     149,   151,   152,   159,   160,   168,   169,   170,   175,   177,
     181,   182,   185,   186,   187,   188,   203,   204,   208,   209,
     211,   215,   219,   221,   222,   223,   228,   229,   230,   235,
     236,   237,   244,   245,   247,   251,   255,   257,   258,   263,
     265,   267,   271,   273,   276,   280,   284,   286,   288,   290,
     293,   297,   301,   305,   309,   313,   317,   321,   325,   329,
     333
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      51,     0,    -1,    -1,    52,    53,    97,    85,    -1,    -1,
       4,    78,     5,    54,    63,    35,    -1,    -1,     5,    55,
      56,    73,    -1,    61,    57,    59,    82,    -1,    63,    35,
      -1,    -1,    -1,     7,    58,    63,    35,    -1,    -1,    -1,
       8,    60,    63,    35,    -1,    -1,    -1,     6,    62,    63,
      35,    -1,    64,    65,    -1,    63,    35,    64,    65,    -1,
      -1,     3,    34,    -1,     3,    37,    34,    -1,    -1,    -1,
      68,    66,    69,    -1,    -1,    65,    33,    68,    67,    69,
      -1,    36,    -1,    37,    -1,    -1,    31,    70,    32,    -1,
      -1,    26,    49,    37,    70,    -1,    27,    49,    29,    70,
      -1,    27,    49,    30,    70,    -1,    -1,    28,    49,    37,
      71,    70,    -1,    -1,    28,    49,    38,    72,    70,    -1,
      -1,    20,    74,    -1,    -1,    68,    34,    75,    77,    35,
      -1,    -1,    74,    68,    34,    76,    77,    35,    -1,    68,
      -1,    77,    33,    68,    -1,    57,    59,    82,    -1,    79,
      -1,    -1,    19,    68,    80,    57,    59,    82,    -1,    -1,
      79,    19,    68,    81,    57,    59,    82,    -1,    -1,    -1,
      10,    83,    84,    35,    -1,    68,    -1,    84,    33,    68,
      -1,    -1,    85,    86,    -1,    -1,    -1,    -1,    16,    68,
      90,    87,    17,    88,    91,    35,    18,    89,    91,    35,
      93,    95,    -1,    -1,    22,    37,    35,    -1,    -1,    92,
      -1,    91,    33,    92,    -1,    68,    34,    37,    -1,    68,
      -1,    -1,    -1,     9,    94,    84,    35,    -1,    -1,    -1,
      11,    96,    84,    35,    -1,    -1,    -1,    12,    98,   100,
      35,    99,   102,    -1,    -1,   101,    -1,   100,    33,   101,
      -1,    68,    34,    37,    -1,    68,    -1,    -1,    13,   103,
     104,    35,    -1,   105,    -1,   100,    -1,   104,    35,   100,
      -1,    14,    -1,    15,    35,    -1,    15,   106,    35,    -1,
      39,   106,    40,    -1,    29,    -1,    30,    -1,    21,    -1,
      41,   106,    -1,   106,    43,   106,    -1,   106,    42,   106,
      -1,   106,    42,    23,    -1,   106,    42,    24,    -1,   106,
      42,    25,    -1,    68,    49,    37,    -1,    68,    48,    37,
      -1,    68,    46,    37,    -1,    68,    47,    37,    -1,    68,
      45,    37,    -1,    68,    44,    37,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,    83,    83,    83,   106,   105,   111,   111,   115,   116,
     119,   121,   121,   124,   126,   126,   129,   131,   131,   135,
     136,   139,   141,   142,   145,   148,   147,   155,   154,   164,
     173,   184,   186,   189,   191,   192,   193,   195,   194,   198,
     197,   204,   206,   210,   210,   211,   211,   215,   224,   238,
     239,   243,   243,   245,   245,   249,   251,   251,   256,   262,
     275,   277,   282,   288,   295,   281,   305,   306,   309,   311,
     312,   316,   327,   340,   342,   342,   346,   348,   348,   358,
     359,   358,   362,   364,   365,   369,   383,   400,   400,   403,
     411,   417,   426,   427,   428,   432,   433,   434,   435,   439,
     444,   450,   456,   461,   466,   471,   478,   485,   492,   499,
     506
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
  "KEY_ALL_PLACES_EMPTY", "KEY_COST", "KEY_ALL_OTHER_PLACES_EMPTY",
  "KEY_ALL_OTHER_INTERNAL_PLACES_EMPTY",
  "KEY_ALL_OTHER_EXTERNAL_PLACES_EMPTY", "KEY_MAX_UNIQUE_EVENTS",
  "KEY_ON_LOOP", "KEY_MAX_OCCURRENCES", "KEY_TRUE", "KEY_FALSE",
  "LCONTROL", "RCONTROL", "COMMA", "COLON", "SEMICOLON", "IDENT", "NUMBER",
  "NEGATIVE_NUMBER", "LPAR", "RPAR", "OP_NOT", "OP_AND", "OP_OR", "OP_LE",
  "OP_GE", "OP_LT", "OP_GT", "OP_NE", "OP_EQ", "$accept", "petrinet",
  "$@1", "places_ports", "$@2", "$@3", "typed_places", "input_places",
  "$@4", "output_places", "$@5", "internal_places", "$@6", "places",
  "capacity", "place_list", "$@7", "$@8", "node_name", "controlcommands",
  "commands", "$@9", "$@10", "ports", "port_list", "$@11", "$@12",
  "port_participants", "interface", "port_list_new", "$@13", "$@14",
  "synchronous", "$@15", "labels", "transitions", "transition", "$@16",
  "$@17", "$@18", "transition_cost", "arcs", "arc", "synchronize", "$@19",
  "constrain", "$@20", "markings", "$@21", "$@22", "marking_list",
  "marking", "final", "$@23", "finalmarkings", "condition", "formula", 0
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
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    50,    52,    51,    54,    53,    55,    53,    56,    56,
      57,    58,    57,    59,    60,    59,    61,    62,    61,    63,
      63,    64,    64,    64,    65,    66,    65,    67,    65,    68,
      68,    69,    69,    70,    70,    70,    70,    71,    70,    72,
      70,    73,    73,    75,    74,    76,    74,    77,    77,    78,
      78,    80,    79,    81,    79,    82,    83,    82,    84,    84,
      85,    85,    87,    88,    89,    86,    90,    90,    91,    91,
      91,    92,    92,    93,    94,    93,    95,    96,    95,    98,
      99,    97,   100,   100,   100,   101,   101,   103,   102,   102,
     104,   104,   105,   105,   105,   106,   106,   106,   106,   106,
     106,   106,   106,   106,   106,   106,   106,   106,   106,   106,
     106
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     4,     0,     6,     0,     4,     4,     2,
       0,     0,     4,     0,     0,     4,     0,     0,     4,     2,
       4,     0,     2,     3,     0,     0,     3,     0,     5,     1,
       1,     0,     3,     0,     4,     4,     4,     0,     5,     0,
       5,     0,     2,     0,     5,     0,     6,     1,     3,     3,
       1,     0,     6,     0,     7,     0,     0,     4,     1,     3,
       0,     2,     0,     0,     0,    14,     0,     3,     0,     1,
       3,     3,     1,     0,     0,     4,     0,     0,     4,     0,
       0,     6,     0,     1,     3,     3,     1,     0,     4,     1,
       1,     3,     1,     2,     3,     3,     1,     1,     1,     2,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     0,     1,    10,     6,     0,    11,     0,    13,
       0,    50,    16,    79,    60,    21,    29,    30,    51,    14,
      55,     4,     0,     0,    17,    41,    10,     0,    24,    82,
       3,     0,    10,    21,    56,    49,    21,    53,    22,     0,
      21,     0,     7,    13,    21,    19,    25,    86,     0,    83,
       0,    61,    12,    13,     0,     0,     0,    10,    23,     0,
       0,    42,    55,    24,     0,    31,     0,     0,    80,    66,
      55,    15,    58,     0,    21,    13,    18,    43,     0,     8,
      20,    27,    33,    26,    85,    84,     0,     0,    62,    52,
       0,    57,    55,     0,    45,    31,     0,     0,     0,     0,
      87,    92,     0,    81,    89,     0,     0,    59,    54,    47,
       0,     0,    28,     0,     0,     0,    32,    82,    98,    96,
      97,    93,     0,     0,     0,     0,    67,    63,     0,    44,
       0,    33,    33,    33,    37,    39,    90,     0,     0,    99,
       0,     0,     0,     0,     0,     0,    94,     0,     0,    68,
      48,    46,    34,    35,    36,    33,    33,    82,    95,   110,
     109,   107,   108,   106,   105,   102,   103,   104,   101,   100,
      72,     0,    69,    38,    40,    91,     0,     0,     0,    71,
      70,    64,    68,     0,    73,    74,    76,     0,    77,    65,
       0,     0,    75,     0,    78
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,     6,    36,    12,    25,     9,    15,    20,
      33,    26,    40,    27,    28,    45,    65,    95,   124,    83,
      99,   155,   156,    42,    61,    93,   111,   110,    10,    11,
      32,    57,    35,    55,    73,    30,    51,   106,   149,   182,
      88,   171,   172,   186,   187,   189,   191,    14,    29,    86,
      48,    49,   103,   117,   137,   104,   125
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -185
static const yytype_int16 yypact[] =
{
    -185,    30,   125,  -185,     3,  -185,    20,  -185,    98,    32,
      40,    48,    13,  -185,  -185,    73,  -185,  -185,  -185,  -185,
      70,  -185,    98,   -28,  -185,    31,    97,    51,    98,    98,
     102,    96,    97,    73,  -185,  -185,    73,  -185,  -185,   104,
      73,    98,  -185,    32,     5,   110,  -185,   113,   -22,  -185,
      98,  -185,    25,    32,   121,    98,   122,    97,  -185,   123,
     114,    98,    70,    98,    98,   128,   124,    98,  -185,   138,
      70,    25,  -185,    -9,    63,    32,    25,  -185,   129,  -185,
     110,  -185,    74,  -185,  -185,  -185,    82,   127,  -185,  -185,
      98,  -185,    70,    98,  -185,   128,   116,   117,   118,   130,
    -185,  -185,    86,  -185,  -185,   133,   153,  -185,  -185,  -185,
      39,    98,  -185,   134,   107,   108,  -185,    98,  -185,  -185,
    -185,  -185,   103,   103,   106,    36,  -185,  -185,    98,  -185,
      56,    74,    74,    74,  -185,  -185,   139,   140,    41,  -185,
     136,   141,   143,   144,   145,   147,  -185,    69,   103,    98,
    -185,  -185,  -185,  -185,  -185,    74,    74,     2,  -185,  -185,
    -185,  -185,  -185,  -185,  -185,  -185,  -185,  -185,  -185,  -185,
     142,    78,  -185,  -185,  -185,   139,   148,    98,   159,  -185,
    -185,  -185,    98,    79,   177,  -185,   176,    98,  -185,  -185,
      84,    98,  -185,    93,  -185
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -185,  -185,  -185,  -185,  -185,  -185,  -185,    -3,  -185,   -38,
    -185,  -185,  -185,    37,   -40,   126,  -185,  -185,    -8,    95,
     -68,  -185,  -185,  -185,  -185,  -185,  -185,    77,  -185,  -185,
    -185,  -185,   -35,  -185,  -184,  -185,  -185,  -185,  -185,  -185,
    -185,     9,    15,  -185,  -185,  -185,  -185,  -185,  -185,  -185,
    -116,   131,  -185,  -185,  -185,  -185,   -79
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -89
static const yytype_int16 yytable[] =
{
      18,   136,   -88,   190,    63,    62,    38,   193,    23,    39,
       7,    67,    63,    68,    37,    70,    23,    -9,   -88,    24,
      46,    47,     8,    43,    90,    -9,    91,    79,    23,    53,
       3,    63,    13,    60,    63,    89,    63,    92,    16,    17,
      19,   175,    69,   138,   139,    21,   -21,    72,   -21,   -21,
     -21,    41,    31,    78,    75,    46,    81,   108,   -21,    47,
     -21,   -21,   -21,   152,   153,   154,    23,    22,   168,   169,
      54,   146,   128,    56,   129,    -5,    23,    59,   147,   148,
      34,   158,   107,   147,   148,   109,    44,   173,   174,   128,
     118,   151,   165,   166,   167,   100,   101,   102,   119,   120,
      96,    97,    98,   109,     7,    16,    17,   118,   122,    47,
     123,   177,   177,   178,   184,   119,   120,    90,    50,   192,
     150,   121,    16,    17,   118,   122,    90,   123,   194,     4,
       5,    52,   119,   120,    16,    17,   132,   133,    58,    16,
      17,   170,   122,    64,   123,   134,   135,    66,    77,    47,
     140,   141,   142,   143,   144,   145,    71,    74,    76,    82,
      87,    84,   116,    94,   105,   113,   114,   115,   126,   170,
     127,   131,    67,   159,   170,   157,   176,   181,   160,    72,
     161,   162,   163,    72,   164,   179,   185,   188,   130,    80,
     112,   183,   180,     0,     0,     0,     0,     0,    85
};

static const yytype_int16 yycheck[] =
{
       8,   117,     0,   187,    44,    43,    34,   191,     3,    37,
       7,    33,    52,    35,    22,    53,     3,    12,    16,     6,
      28,    29,    19,    26,    33,    20,    35,    62,     3,    32,
       0,    71,    12,    41,    74,    70,    76,    75,    36,    37,
       8,   157,    50,   122,   123,     5,    33,    55,    35,    36,
      37,    20,    15,    61,    57,    63,    64,    92,    33,    67,
      35,    36,    37,   131,   132,   133,     3,    19,   147,   148,
      33,    35,    33,    36,    35,    12,     3,    40,    42,    43,
      10,    40,    90,    42,    43,    93,    35,   155,   156,    33,
      21,    35,    23,    24,    25,    13,    14,    15,    29,    30,
      26,    27,    28,   111,     7,    36,    37,    21,    39,   117,
      41,    33,    33,    35,    35,    29,    30,    33,    16,    35,
     128,    35,    36,    37,    21,    39,    33,    41,    35,     4,
       5,    35,    29,    30,    36,    37,    29,    30,    34,    36,
      37,   149,    39,    33,    41,    37,    38,    34,    34,   157,
      44,    45,    46,    47,    48,    49,    35,    35,    35,    31,
      22,    37,    32,    34,    37,    49,    49,    49,    35,   177,
      17,    37,    33,    37,   182,    35,    34,    18,    37,   187,
      37,    37,    37,   191,    37,    37,     9,    11,   111,    63,
      95,   182,   177,    -1,    -1,    -1,    -1,    -1,    67
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    51,    52,     0,     4,     5,    53,     7,    19,    57,
      78,    79,    55,    12,    97,    58,    36,    37,    68,     8,
      59,     5,    19,     3,     6,    56,    61,    63,    64,    98,
      85,    63,    80,    60,    10,    82,    54,    68,    34,    37,
      62,    20,    73,    57,    35,    65,    68,    68,   100,   101,
      16,    86,    35,    57,    63,    83,    63,    81,    34,    63,
      68,    74,    59,    64,    33,    66,    34,    33,    35,    68,
      59,    35,    68,    84,    35,    57,    35,    34,    68,    82,
      65,    68,    31,    69,    37,   101,    99,    22,    90,    82,
      33,    35,    59,    75,    34,    67,    26,    27,    28,    70,
      13,    14,    15,   102,   105,    37,    87,    68,    82,    68,
      77,    76,    69,    49,    49,    49,    32,   103,    21,    29,
      30,    35,    39,    41,    68,   106,    35,    17,    33,    35,
      77,    37,    29,    30,    37,    38,   100,   104,   106,   106,
      44,    45,    46,    47,    48,    49,    35,    42,    43,    88,
      68,    35,    70,    70,    70,    71,    72,    35,    40,    37,
      37,    37,    37,    37,    37,    23,    24,    25,   106,   106,
      68,    91,    92,    70,    70,   100,    34,    33,    35,    37,
      92,    18,    89,    91,    35,     9,    93,    94,    11,    95,
      84,    96,    35,    84,    35
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
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
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
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
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


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

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
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

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
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
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

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
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

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
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

/* Line 1455 of yacc.c  */
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

/* Line 1455 of yacc.c  */
#line 92 "parser-owfn.yy"
    { 
      pnapi_owfn_yynet.setConstraintLabels(constrains_); 
      pnapi_owfn_yynet.setSynchronousLabels(synchronousLabels_);
    }
    break;

  case 4:

/* Line 1455 of yacc.c  */
#line 106 "parser-owfn.yy"
    {
     placeType_ = Node::INTERNAL;
     port_ = "";
    }
    break;

  case 6:

/* Line 1455 of yacc.c  */
#line 111 "parser-owfn.yy"
    {placeType_ = Node::INTERNAL;}
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 121 "parser-owfn.yy"
    {placeType_ = Node::INPUT;}
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 126 "parser-owfn.yy"
    {placeType_ = Node::OUTPUT;}
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 131 "parser-owfn.yy"
    {placeType_ = Node::INTERNAL;}
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 135 "parser-owfn.yy"
    { capacity_ = 0; }
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 136 "parser-owfn.yy"
    { capacity_ = 0; }
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 141 "parser-owfn.yy"
    {capacity_ = 1; }
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 142 "parser-owfn.yy"
    {capacity_ = (yyvsp[(2) - (3)].yt_int);}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 148 "parser-owfn.yy"
    {
     check(places_[nodeName_.str()] == 0, "node name already used");
     place_ = & pnapi_owfn_yynet.createPlace(nodeName_.str(), placeType_, 0, capacity_, port_);
     places_[nodeName_.str()] = place_;
    }
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 155 "parser-owfn.yy"
    {
     check(places_[nodeName_.str()] == 0, "node name already used");
     place_ = & pnapi_owfn_yynet.createPlace(nodeName_.str(), placeType_, 0, capacity_, port_);
     places_[nodeName_.str()] = place_;
    }
    break;

  case 29:

/* Line 1455 of yacc.c  */
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

/* Line 1455 of yacc.c  */
#line 174 "parser-owfn.yy"
    { 
      // clear stringstream
      nodeName_.str("");
      nodeName_.clear();

      nodeName_ << (yyvsp[(1) - (1)].yt_int); 
    }
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 195 "parser-owfn.yy"
    { place_->setMaxOccurrence((yyvsp[(3) - (3)].yt_int)); }
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 198 "parser-owfn.yy"
    { place_->setMaxOccurrence((yyvsp[(3) - (3)].yt_int)); }
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 210 "parser-owfn.yy"
    { port_ = nodeName_.str(); }
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 211 "parser-owfn.yy"
    { port_ = nodeName_.str(); }
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 216 "parser-owfn.yy"
    {
      Place* p = places_[nodeName_.str()];
      check(p != 0, "unknown place");
      check(p->getType() != Place::INTERNAL, "interface place expectd");
      check(p->getPort().empty(), "place already assigned to port '" + p->getPort() +"'");
       
      p->setPort(port_);
    }
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 225 "parser-owfn.yy"
    {
      Place* p = places_[nodeName_.str()];
      check(p != 0, "unknown place");
      check(p->getType() != Place::INTERNAL, "interface place expectd");
      check(p->getPort().empty(), "place already assigned to port '" + p->getPort() +"'");

      p->setPort(port_);
    }
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 243 "parser-owfn.yy"
    { port_ = nodeName_.str(); }
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 245 "parser-owfn.yy"
    { port_ = nodeName_.str(); }
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 251 "parser-owfn.yy"
    { labels_.clear(); checkLabels_ = false; }
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 252 "parser-owfn.yy"
    { synchronousLabels_ = labels_; }
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 257 "parser-owfn.yy"
    { 
      check(!(checkLabels_ && (synchronousLabels_.find(nodeName_.str()) == synchronousLabels_.end())),
             "undeclared label");
      labels_.insert(nodeName_.str()); 
    }
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 263 "parser-owfn.yy"
    {
      check(!(checkLabels_ && (synchronousLabels_.find(nodeName_.str()) == synchronousLabels_.end())),
             "undeclared label"); 
      labels_.insert(nodeName_.str()); 
    }
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 282 "parser-owfn.yy"
    { 
      check(!pnapi_owfn_yynet.containsNode(nodeName_.str()), "node name already used");
      transition_ = & pnapi_owfn_yynet.createTransition(nodeName_.str()); 
      transition_->setCost((yyvsp[(3) - (3)].yt_int));
    }
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 288 "parser-owfn.yy"
    { 
      target_ = (Node**)(&transition_); source_ = (Node**)(&place_); 
      placeSet_.clear();
      placeSetType_ = true;
    }
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 295 "parser-owfn.yy"
    { 
      source_ = (Node**)(&transition_); target_ = (Node**)(&place_); 
      placeSet_.clear();
      placeSetType_ = false;
    }
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 305 "parser-owfn.yy"
    { (yyval.yt_int) = 0;  }
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 306 "parser-owfn.yy"
    { (yyval.yt_int) = (yyvsp[(2) - (3)].yt_int); }
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 317 "parser-owfn.yy"
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

  case 72:

/* Line 1455 of yacc.c  */
#line 328 "parser-owfn.yy"
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

  case 74:

/* Line 1455 of yacc.c  */
#line 342 "parser-owfn.yy"
    { labels_.clear(); checkLabels_ = true; }
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 343 "parser-owfn.yy"
    { transition_->setSynchronizeLabels(labels_); }
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 348 "parser-owfn.yy"
    { labels_.clear(); checkLabels_ = false; }
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 349 "parser-owfn.yy"
    { constrains_[transition_] = labels_; }
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 358 "parser-owfn.yy"
    { markInitial_ = true; }
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 359 "parser-owfn.yy"
    {markInitial_ = false; }
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 370 "parser-owfn.yy"
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

  case 86:

/* Line 1455 of yacc.c  */
#line 384 "parser-owfn.yy"
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

  case 87:

/* Line 1455 of yacc.c  */
#line 400 "parser-owfn.yy"
    { finalMarking_ = new Marking(pnapi_owfn_yynet, true); }
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 402 "parser-owfn.yy"
    { delete finalMarking_; finalMarking_ = NULL; }
    break;

  case 89:

/* Line 1455 of yacc.c  */
#line 404 "parser-owfn.yy"
    {
      pnapi_owfn_yynet.finalCondition() = (*(yyvsp[(1) - (1)].yt_formula));
      delete (yyvsp[(1) - (1)].yt_formula); 
    }
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 412 "parser-owfn.yy"
    {
      pnapi_owfn_yynet.finalCondition().addMarking(*finalMarking_);
      delete finalMarking_;
      finalMarking_ = new Marking(pnapi_owfn_yynet, true);
    }
    break;

  case 91:

/* Line 1455 of yacc.c  */
#line 418 "parser-owfn.yy"
    {
      pnapi_owfn_yynet.finalCondition().addMarking(*finalMarking_);
      delete finalMarking_;
      finalMarking_ = new Marking(pnapi_owfn_yynet, true);
    }
    break;

  case 92:

/* Line 1455 of yacc.c  */
#line 426 "parser-owfn.yy"
    { (yyval.yt_formula) = new formula::FormulaTrue(); }
    break;

  case 93:

/* Line 1455 of yacc.c  */
#line 427 "parser-owfn.yy"
    { (yyval.yt_formula) = new formula::FormulaFalse(); }
    break;

  case 94:

/* Line 1455 of yacc.c  */
#line 428 "parser-owfn.yy"
    { (yyval.yt_formula) = (yyvsp[(2) - (3)].yt_formula); }
    break;

  case 95:

/* Line 1455 of yacc.c  */
#line 432 "parser-owfn.yy"
    { (yyval.yt_formula) = (yyvsp[(2) - (3)].yt_formula); }
    break;

  case 96:

/* Line 1455 of yacc.c  */
#line 433 "parser-owfn.yy"
    { (yyval.yt_formula) = new formula::FormulaTrue(); }
    break;

  case 97:

/* Line 1455 of yacc.c  */
#line 434 "parser-owfn.yy"
    { (yyval.yt_formula) = new formula::FormulaFalse(); }
    break;

  case 98:

/* Line 1455 of yacc.c  */
#line 436 "parser-owfn.yy"
    { 
      (yyval.yt_formula) = new formula::Conjunction(formula::ALL_PLACES_EMPTY); 
    }
    break;

  case 99:

/* Line 1455 of yacc.c  */
#line 440 "parser-owfn.yy"
    { 
      (yyval.yt_formula) = new formula::Negation(*(yyvsp[(2) - (2)].yt_formula));
      delete (yyvsp[(2) - (2)].yt_formula);
    }
    break;

  case 100:

/* Line 1455 of yacc.c  */
#line 445 "parser-owfn.yy"
    {
      (yyval.yt_formula) = new formula::Disjunction(*(yyvsp[(1) - (3)].yt_formula), *(yyvsp[(3) - (3)].yt_formula));
      delete (yyvsp[(1) - (3)].yt_formula);
      delete (yyvsp[(3) - (3)].yt_formula);
    }
    break;

  case 101:

/* Line 1455 of yacc.c  */
#line 451 "parser-owfn.yy"
    {
      (yyval.yt_formula) = new formula::Conjunction(*(yyvsp[(1) - (3)].yt_formula), *(yyvsp[(3) - (3)].yt_formula));
      delete (yyvsp[(1) - (3)].yt_formula);
      delete (yyvsp[(3) - (3)].yt_formula);
    }
    break;

  case 102:

/* Line 1455 of yacc.c  */
#line 457 "parser-owfn.yy"
    {
      (yyval.yt_formula) = new formula::Conjunction(*(yyvsp[(1) - (3)].yt_formula), formula::ALL_OTHER_PLACES_EMPTY);
      delete (yyvsp[(1) - (3)].yt_formula);
    }
    break;

  case 103:

/* Line 1455 of yacc.c  */
#line 462 "parser-owfn.yy"
    {
      (yyval.yt_formula) = new formula::Conjunction(*(yyvsp[(1) - (3)].yt_formula), formula::ALL_OTHER_INTERNAL_PLACES_EMPTY);
      delete (yyvsp[(1) - (3)].yt_formula);
    }
    break;

  case 104:

/* Line 1455 of yacc.c  */
#line 467 "parser-owfn.yy"
    {
      (yyval.yt_formula) = new formula::Conjunction(*(yyvsp[(1) - (3)].yt_formula), formula::ALL_OTHER_EXTERNAL_PLACES_EMPTY);
      delete (yyvsp[(1) - (3)].yt_formula);
    }
    break;

  case 105:

/* Line 1455 of yacc.c  */
#line 472 "parser-owfn.yy"
    {
      Place* p = places_[nodeName_.str()];
      check(p != 0, "unknown place");
      
      (yyval.yt_formula) = new formula::FormulaEqual(*p, (yyvsp[(3) - (3)].yt_int));
    }
    break;

  case 106:

/* Line 1455 of yacc.c  */
#line 479 "parser-owfn.yy"
    {
      Place* p = places_[nodeName_.str()];
      check(p != 0, "unknown place");

      (yyval.yt_formula) = new formula::Negation(formula::FormulaEqual(*p, (yyvsp[(3) - (3)].yt_int)));
    }
    break;

  case 107:

/* Line 1455 of yacc.c  */
#line 486 "parser-owfn.yy"
    {
      Place* p = places_[nodeName_.str()];
      check(p != 0, "unknown place");

      (yyval.yt_formula) = new formula::FormulaLess(*p, (yyvsp[(3) - (3)].yt_int));
    }
    break;

  case 108:

/* Line 1455 of yacc.c  */
#line 493 "parser-owfn.yy"
    {
      Place* p = places_[nodeName_.str()];
      check(p != 0, "unknown place");

      (yyval.yt_formula) = new formula::FormulaGreater(*p, (yyvsp[(3) - (3)].yt_int));
    }
    break;

  case 109:

/* Line 1455 of yacc.c  */
#line 500 "parser-owfn.yy"
    {
      Place* p = places_[nodeName_.str()];
      check(p != 0, "unknown place");

      (yyval.yt_formula) = new formula::FormulaGreaterEqual(*p, (yyvsp[(3) - (3)].yt_int));
    }
    break;

  case 110:

/* Line 1455 of yacc.c  */
#line 507 "parser-owfn.yy"
    {
      Place* p = places_[nodeName_.str()];
      check(p != 0, "unknown place");

      (yyval.yt_formula) = new formula::FormulaLessEqual(*p, (yyvsp[(3) - (3)].yt_int));
    }
    break;



/* Line 1455 of yacc.c  */
#line 2279 "parser-owfn.cc"
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
      /* If just tried and failed to reuse lookahead token after an
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

  /* Else will try to reuse lookahead token after shifting the error
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

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
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



