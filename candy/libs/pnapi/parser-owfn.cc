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
#define yyparse pnapi_owfn_parse
#define yylex   pnapi_owfn_lex
#define yyerror pnapi_owfn_error
#define yylval  pnapi_owfn_lval
#define yychar  pnapi_owfn_char
#define yydebug pnapi_owfn_debug
#define yynerrs pnapi_owfn_nerrs


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
     KEY_MARKING = 267,
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
#define KEY_MARKING 267
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




/* Copy the first part of user declarations.  */
#line 20 "parser-owfn.yy"


#include <string>
#include "parser.h"

#undef yylex
#undef yyparse
#undef yyerror

#define yyerror pnapi::parser::error

#define yylex pnapi::parser::owfn::lex
#define yyparse pnapi::parser::owfn::parse



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
#line 58 "parser-owfn.yy"
{
  int yt_int;
  std::string * yt_string;
  pnapi::parser::owfn::Node * yt_node;
}
/* Line 187 of yacc.c.  */
#line 224 "parser-owfn.cc"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 237 "parser-owfn.cc"

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
#define YYFINAL  19
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   190

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  50
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  45
/* YYNRULES -- Number of rules.  */
#define YYNRULES  98
/* YYNRULES -- Number of states.  */
#define YYNSTATES  174

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
       0,     0,     3,     7,     9,    11,    17,    21,    26,    28,
      32,    33,    37,    38,    42,    43,    46,    48,    52,    55,
      56,    59,    63,    64,    66,    70,    73,    74,    78,    79,
      84,    89,    94,    99,   104,   105,   108,   110,   113,   118,
     120,   124,   126,   130,   132,   134,   137,   143,   147,   148,
     150,   154,   156,   159,   160,   168,   172,   173,   177,   181,
     182,   184,   188,   192,   194,   195,   199,   200,   204,   207,
     211,   212,   214,   218,   222,   224,   228,   230,   232,   235,
     239,   241,   245,   247,   251,   253,   255,   257,   260,   264,
     268,   272,   276,   280,   284,   288,   292,   296,   300
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      51,     0,    -1,    53,    86,    77,    -1,    36,    -1,    37,
      -1,     4,    71,     5,    59,    35,    -1,     5,    54,    66,
      -1,    57,    55,    56,    74,    -1,    58,    -1,     7,    59,
      35,    -1,    -1,     8,    59,    35,    -1,    -1,     6,    59,
      35,    -1,    -1,    59,    35,    -1,    60,    -1,    59,    35,
      60,    -1,    61,    62,    -1,    -1,     3,    34,    -1,     3,
      37,    34,    -1,    -1,    63,    -1,    62,    33,    63,    -1,
      52,    64,    -1,    -1,    31,    65,    32,    -1,    -1,    26,
      49,    37,    65,    -1,    27,    49,    29,    65,    -1,    27,
      49,    30,    65,    -1,    28,    49,    37,    65,    -1,    28,
      49,    38,    65,    -1,    -1,    20,    67,    -1,    68,    -1,
      67,    68,    -1,    52,    34,    69,    35,    -1,    70,    -1,
      69,    33,    70,    -1,    52,    -1,    55,    56,    74,    -1,
      72,    -1,    73,    -1,    72,    73,    -1,    19,    52,    55,
      56,    74,    -1,    10,    75,    35,    -1,    -1,    76,    -1,
      75,    33,    76,    -1,    52,    -1,    77,    78,    -1,    -1,
      16,    52,    79,    80,    81,    84,    85,    -1,    22,    37,
      35,    -1,    -1,    17,    82,    35,    -1,    18,    82,    35,
      -1,    -1,    83,    -1,    82,    33,    83,    -1,    52,    34,
      37,    -1,    52,    -1,    -1,     9,    75,    35,    -1,    -1,
      11,    75,    35,    -1,    87,    90,    -1,    12,    88,    35,
      -1,    -1,    89,    -1,    88,    33,    89,    -1,    52,    34,
      37,    -1,    52,    -1,    13,    92,    35,    -1,    91,    -1,
      14,    -1,    15,    35,    -1,    15,    94,    35,    -1,    93,
      -1,    92,    35,    93,    -1,    88,    -1,    39,    94,    40,
      -1,    29,    -1,    30,    -1,    21,    -1,    41,    94,    -1,
      94,    43,    94,    -1,    94,    42,    94,    -1,    94,    42,
      23,    -1,    94,    42,    24,    -1,    94,    42,    25,    -1,
      52,    49,    37,    -1,    52,    48,    37,    -1,    52,    46,
      37,    -1,    52,    47,    37,    -1,    52,    45,    37,    -1,
      52,    44,    37,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,    88,    88,    92,    93,   102,   104,   109,   111,   115,
     116,   120,   121,   125,   126,   130,   134,   135,   139,   143,
     144,   145,   149,   150,   151,   155,   158,   160,   163,   165,
     166,   167,   168,   169,   173,   174,   178,   179,   183,   187,
     188,   192,   196,   197,   201,   202,   206,   211,   212,   216,
     217,   221,   230,   231,   235,   240,   241,   245,   249,   253,
     254,   255,   259,   260,   264,   265,   269,   270,   279,   283,
     287,   288,   289,   293,   294,   298,   299,   303,   304,   305,
     309,   310,   314,   318,   319,   320,   321,   322,   323,   324,
     325,   327,   329,   331,   332,   333,   334,   335,   336
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "KEY_SAFE", "KEY_INTERFACE", "KEY_PLACE",
  "KEY_INTERNAL", "KEY_INPUT", "KEY_OUTPUT", "KEY_SYNCHRONIZE",
  "KEY_SYNCHRONOUS", "KEY_CONSTRAIN", "KEY_MARKING", "KEY_FINALMARKING",
  "KEY_NOFINALMARKING", "KEY_FINALCONDITION", "KEY_TRANSITION",
  "KEY_CONSUME", "KEY_PRODUCE", "KEY_PORT", "KEY_PORTS",
  "KEY_ALL_PLACES_EMPTY", "KEY_COST", "KEY_ALL_OTHER_PLACES_EMPTY",
  "KEY_ALL_OTHER_INTERNAL_PLACES_EMPTY",
  "KEY_ALL_OTHER_EXTERNAL_PLACES_EMPTY", "KEY_MAX_UNIQUE_EVENTS",
  "KEY_ON_LOOP", "KEY_MAX_OCCURRENCES", "KEY_TRUE", "KEY_FALSE",
  "LCONTROL", "RCONTROL", "COMMA", "COLON", "SEMICOLON", "IDENT", "NUMBER",
  "NEGATIVE_NUMBER", "LPAR", "RPAR", "OP_NOT", "OP_AND", "OP_OR", "OP_LE",
  "OP_GE", "OP_LT", "OP_GT", "OP_NE", "OP_EQ", "$accept", "petrinet",
  "node_name", "places_ports", "typed_places", "input_places",
  "output_places", "internal_places", "lola_places", "places",
  "capacity_place_list", "capacity", "place_list", "place",
  "controlcommands", "commands", "ports", "port_list", "port_definition",
  "port_participants", "port_participant", "interface", "port_list_new",
  "port_definition_new", "synchronous", "labels", "label", "transitions",
  "transition", "transition_cost", "preset_arcs", "postset_arcs", "arcs",
  "arc", "synchronize", "constrain", "markings", "initial", "marking_list",
  "marking", "final", "condition", "finalmarkings", "finalmarking",
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
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    50,    51,    52,    52,    53,    53,    54,    54,    55,
      55,    56,    56,    57,    57,    58,    59,    59,    60,    61,
      61,    61,    62,    62,    62,    63,    64,    64,    65,    65,
      65,    65,    65,    65,    66,    66,    67,    67,    68,    69,
      69,    70,    71,    71,    72,    72,    73,    74,    74,    75,
      75,    76,    77,    77,    78,    79,    79,    80,    81,    82,
      82,    82,    83,    83,    84,    84,    85,    85,    86,    87,
      88,    88,    88,    89,    89,    90,    90,    91,    91,    91,
      92,    92,    93,    94,    94,    94,    94,    94,    94,    94,
      94,    94,    94,    94,    94,    94,    94,    94,    94
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     3,     1,     1,     5,     3,     4,     1,     3,
       0,     3,     0,     3,     0,     2,     1,     3,     2,     0,
       2,     3,     0,     1,     3,     2,     0,     3,     0,     4,
       4,     4,     4,     4,     0,     2,     1,     2,     4,     1,
       3,     1,     3,     1,     1,     2,     5,     3,     0,     1,
       3,     1,     2,     0,     7,     3,     0,     3,     3,     0,
       1,     3,     3,     1,     0,     3,     0,     3,     2,     3,
       0,     1,     3,     3,     1,     3,     1,     1,     2,     3,
       1,     3,     1,     3,     1,     1,     1,     2,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,    10,    14,     0,     0,    19,     0,    12,     0,    43,
      44,     0,    19,    34,    10,     8,     0,    16,    22,     1,
      70,    53,     0,     0,     3,     4,    10,    19,    48,    19,
      45,    20,     0,     0,     0,     6,    12,    19,    26,    18,
      23,    74,     0,    71,     2,    70,    77,     0,    68,    76,
       9,    12,     0,     0,    42,     0,    21,    13,     0,    35,
      36,    48,    17,    28,    25,     0,     0,     0,    69,     0,
      52,    82,     0,    80,    86,    84,    85,    78,     0,     0,
       0,     0,    48,    11,    51,     0,    49,    19,     0,    37,
       7,     0,     0,     0,     0,    24,    73,    72,    56,    70,
       0,    87,     0,     0,     0,     0,     0,     0,    79,     0,
       0,    46,     0,    47,    41,     0,    39,     0,     0,     0,
      27,     0,     0,    81,    83,    98,    97,    95,    96,    94,
      93,    90,    91,    92,    89,    88,    50,     0,    38,    28,
      28,    28,    28,    28,     0,    59,     0,    40,    29,    30,
      31,    32,    33,    55,    63,     0,    60,    59,    64,     0,
       0,    57,     0,     0,    66,    62,    61,    58,     0,     0,
      54,    65,     0,    67
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     3,    80,     4,    13,     7,    28,    14,    15,    16,
      17,    18,    39,    40,    64,    94,    35,    59,    60,   115,
     116,     8,     9,    10,    54,    85,    86,    44,    70,   122,
     146,   158,   155,   156,   164,   170,    21,    22,    71,    43,
      48,    49,    72,    73,    81
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -134
static const yytype_int16 yypact[] =
{
      41,     0,     7,     8,    13,    17,    14,    47,    24,    52,
    -134,    38,    17,    56,    72,  -134,    59,  -134,    14,  -134,
      14,  -134,    84,    69,  -134,  -134,    72,    17,   104,    17,
    -134,  -134,    83,    76,    14,  -134,    47,    29,    88,   115,
    -134,   113,    27,  -134,   118,    14,  -134,    66,  -134,  -134,
      21,    47,   102,    14,  -134,   106,  -134,    21,   116,    14,
    -134,   104,  -134,   101,  -134,    14,   112,    14,  -134,    14,
    -134,   119,   120,  -134,  -134,  -134,  -134,  -134,    79,    79,
      77,    35,   104,    21,  -134,    31,  -134,    15,    14,  -134,
    -134,   109,   110,   111,   121,  -134,  -134,  -134,   134,     1,
      70,  -134,   124,   125,   127,   128,   129,   130,  -134,    44,
      79,  -134,    14,  -134,  -134,    51,  -134,   131,   114,   108,
    -134,   132,   153,  -134,  -134,  -134,  -134,  -134,  -134,  -134,
    -134,  -134,  -134,  -134,  -134,  -134,  -134,    14,  -134,   101,
     101,   101,   101,   101,   136,    14,   154,  -134,  -134,  -134,
    -134,  -134,  -134,  -134,   139,    97,  -134,    14,   165,   138,
      14,  -134,   100,    14,   166,  -134,  -134,  -134,   103,    14,
    -134,  -134,   107,  -134
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -134,  -134,    -6,  -134,  -134,    -3,   -30,  -134,  -134,     4,
     -35,  -134,  -134,   117,  -134,   -51,  -134,  -134,   122,  -134,
      39,  -134,  -134,   169,   -56,  -133,    67,  -134,  -134,  -134,
    -134,  -134,    23,    25,  -134,  -134,  -134,  -134,   163,   123,
    -134,  -134,  -134,    85,   -75
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -76
static const yytype_int16 yytable[] =
{
      26,   -75,    62,   100,   101,    90,    61,     5,    19,    23,
      11,    36,    38,    12,    41,    62,    33,   -75,    11,     6,
      11,    82,    62,    51,    11,    20,   111,    -5,    58,    29,
     168,    52,    11,    55,   134,   135,   172,    24,    25,    41,
     -19,   -15,   -19,   -19,   -19,     1,     2,    84,    62,   -15,
      24,    25,    62,    58,   -19,    27,   -19,   -19,   -19,    38,
      67,    41,    68,    98,   112,    74,   113,   131,   132,   133,
     108,     6,    31,    75,    76,    32,    34,   109,   110,     5,
      24,    25,   114,    78,   137,    79,   138,    74,   148,   149,
     150,   151,   152,    41,    37,    75,    76,    45,    46,    47,
      74,    77,    24,    25,    50,    78,    84,    79,    75,    76,
     124,    57,   109,   110,    53,    24,    25,    56,    78,    63,
      79,   102,   103,   104,   105,   106,   107,    91,    92,    93,
     160,   114,   161,   160,    69,   167,   112,    83,   171,   154,
     112,    87,   173,   140,   141,   142,   143,    66,    65,    96,
      88,   154,    67,   120,   154,    99,   121,    84,   117,   118,
     119,   125,   126,    84,   127,   128,   129,   130,   139,   144,
     145,   153,   157,   159,   163,   165,   147,   169,    30,   136,
     162,    89,    95,    42,   123,   166,     0,     0,     0,     0,
      97
};

static const yytype_int16 yycheck[] =
{
       6,     0,    37,    78,    79,    61,    36,     7,     0,     5,
       3,    14,    18,     6,    20,    50,    12,    16,     3,    19,
       3,    51,    57,    26,     3,    12,    82,    12,    34,     5,
     163,    27,     3,    29,   109,   110,   169,    36,    37,    45,
      33,    12,    35,    36,    37,     4,     5,    53,    83,    20,
      36,    37,    87,    59,    33,     8,    35,    36,    37,    65,
      33,    67,    35,    69,    33,    21,    35,    23,    24,    25,
      35,    19,    34,    29,    30,    37,    20,    42,    43,     7,
      36,    37,    88,    39,    33,    41,    35,    21,   139,   140,
     141,   142,   143,    99,    35,    29,    30,    13,    14,    15,
      21,    35,    36,    37,    35,    39,   112,    41,    29,    30,
      40,    35,    42,    43,    10,    36,    37,    34,    39,    31,
      41,    44,    45,    46,    47,    48,    49,    26,    27,    28,
      33,   137,    35,    33,    16,    35,    33,    35,    35,   145,
      33,    35,    35,    29,    30,    37,    38,    34,    33,    37,
      34,   157,    33,    32,   160,    35,    22,   163,    49,    49,
      49,    37,    37,   169,    37,    37,    37,    37,    37,    37,
      17,    35,    18,    34,     9,    37,   137,    11,     9,   112,
     157,    59,    65,    20,    99,   160,    -1,    -1,    -1,    -1,
      67
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     4,     5,    51,    53,     7,    19,    55,    71,    72,
      73,     3,     6,    54,    57,    58,    59,    60,    61,     0,
      12,    86,    87,    59,    36,    37,    52,     8,    56,     5,
      73,    34,    37,    59,    20,    66,    55,    35,    52,    62,
      63,    52,    88,    89,    77,    13,    14,    15,    90,    91,
      35,    55,    59,    10,    74,    59,    34,    35,    52,    67,
      68,    56,    60,    31,    64,    33,    34,    33,    35,    16,
      78,    88,    92,    93,    21,    29,    30,    35,    39,    41,
      52,    94,    56,    35,    52,    75,    76,    35,    34,    68,
      74,    26,    27,    28,    65,    63,    37,    89,    52,    35,
      94,    94,    44,    45,    46,    47,    48,    49,    35,    42,
      43,    74,    33,    35,    52,    69,    70,    49,    49,    49,
      32,    22,    79,    93,    40,    37,    37,    37,    37,    37,
      37,    23,    24,    25,    94,    94,    76,    33,    35,    37,
      29,    30,    37,    38,    37,    17,    80,    70,    65,    65,
      65,    65,    65,    35,    52,    82,    83,    18,    81,    34,
      33,    35,    82,     9,    84,    37,    83,    35,    75,    11,
      85,    35,    75,    35
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
#line 88 "parser-owfn.yy"
    { node = new Node((yyvsp[(1) - (3)].yt_node), (yyvsp[(2) - (3)].yt_node), (yyvsp[(3) - (3)].yt_node)); }
    break;

  case 3:
#line 92 "parser-owfn.yy"
    { (yyval.yt_node) = new Node((yyvsp[(1) - (1)].yt_string)); }
    break;

  case 4:
#line 93 "parser-owfn.yy"
    { (yyval.yt_node) = new Node((yyvsp[(1) - (1)].yt_int)); }
    break;

  case 5:
#line 103 "parser-owfn.yy"
    { (yyval.yt_node) = new Node((yyvsp[(2) - (5)].yt_node), new Node(INTERNAL, (yyvsp[(4) - (5)].yt_node))); }
    break;

  case 6:
#line 105 "parser-owfn.yy"
    { (yyval.yt_node) = new Node((yyvsp[(2) - (3)].yt_node), (yyvsp[(3) - (3)].yt_node)); }
    break;

  case 7:
#line 110 "parser-owfn.yy"
    { (yyval.yt_node) = new Node((yyvsp[(1) - (4)].yt_node), (yyvsp[(2) - (4)].yt_node), (yyvsp[(3) - (4)].yt_node), (yyvsp[(4) - (4)].yt_node)); }
    break;

  case 8:
#line 111 "parser-owfn.yy"
    { (yyval.yt_node) = (yyvsp[(1) - (1)].yt_node);                       }
    break;

  case 9:
#line 115 "parser-owfn.yy"
    { (yyval.yt_node) = new Node(INPUT, (yyvsp[(2) - (3)].yt_node)); }
    break;

  case 10:
#line 116 "parser-owfn.yy"
    { (yyval.yt_node) = new Node();          }
    break;

  case 11:
#line 120 "parser-owfn.yy"
    { (yyval.yt_node) = new Node(OUTPUT, (yyvsp[(2) - (3)].yt_node)); }
    break;

  case 12:
#line 121 "parser-owfn.yy"
    { (yyval.yt_node) = new Node();           }
    break;

  case 13:
#line 125 "parser-owfn.yy"
    { (yyval.yt_node) = new Node(INTERNAL, (yyvsp[(2) - (3)].yt_node)); }
    break;

  case 14:
#line 126 "parser-owfn.yy"
    { (yyval.yt_node) = new Node();             }
    break;

  case 15:
#line 130 "parser-owfn.yy"
    { (yyval.yt_node) = new Node(INTERNAL, (yyvsp[(1) - (2)].yt_node)); }
    break;

  case 16:
#line 134 "parser-owfn.yy"
    { (yyval.yt_node) = new Node((yyvsp[(1) - (1)].yt_node));     }
    break;

  case 17:
#line 135 "parser-owfn.yy"
    { (yyval.yt_node) = (yyvsp[(1) - (3)].yt_node)->addChild((yyvsp[(3) - (3)].yt_node)); }
    break;

  case 18:
#line 139 "parser-owfn.yy"
    { (yyval.yt_node) = new Node(CAPACITY, (yyvsp[(1) - (2)].yt_node), (yyvsp[(2) - (2)].yt_node)); }
    break;

  case 19:
#line 143 "parser-owfn.yy"
    { (yyval.yt_node) = new Node(0);  }
    break;

  case 20:
#line 144 "parser-owfn.yy"
    { (yyval.yt_node) = new Node(1);  }
    break;

  case 21:
#line 145 "parser-owfn.yy"
    { (yyval.yt_node) = new Node((yyvsp[(2) - (3)].yt_int)); }
    break;

  case 22:
#line 149 "parser-owfn.yy"
    { (yyval.yt_node) = new Node();       }
    break;

  case 23:
#line 150 "parser-owfn.yy"
    { (yyval.yt_node) = new Node((yyvsp[(1) - (1)].yt_node));     }
    break;

  case 24:
#line 151 "parser-owfn.yy"
    { (yyval.yt_node) = (yyvsp[(1) - (3)].yt_node)->addChild((yyvsp[(3) - (3)].yt_node)); }
    break;

  case 25:
#line 155 "parser-owfn.yy"
    { (yyval.yt_node) = new Node(PLACE, (yyvsp[(1) - (2)].yt_node)); }
    break;

  case 34:
#line 173 "parser-owfn.yy"
    { (yyval.yt_node) = new Node(); }
    break;

  case 35:
#line 174 "parser-owfn.yy"
    { (yyval.yt_node) = (yyvsp[(2) - (2)].yt_node);         }
    break;

  case 36:
#line 178 "parser-owfn.yy"
    { (yyval.yt_node) = new Node((yyvsp[(1) - (1)].yt_node));     }
    break;

  case 37:
#line 179 "parser-owfn.yy"
    { (yyval.yt_node) = (yyvsp[(1) - (2)].yt_node)->addChild((yyvsp[(2) - (2)].yt_node)); }
    break;

  case 38:
#line 183 "parser-owfn.yy"
    { (yyval.yt_node) = new Node(PORT, (yyvsp[(1) - (4)].yt_node), (yyvsp[(3) - (4)].yt_node)); }
    break;

  case 39:
#line 187 "parser-owfn.yy"
    { (yyval.yt_node) = new Node((yyvsp[(1) - (1)].yt_node));     }
    break;

  case 40:
#line 188 "parser-owfn.yy"
    { (yyval.yt_node) = (yyvsp[(1) - (3)].yt_node)->addChild((yyvsp[(3) - (3)].yt_node)); }
    break;

  case 41:
#line 192 "parser-owfn.yy"
    { (yyval.yt_node) = new Node(PORT_PLACE, (yyvsp[(1) - (1)].yt_node)); }
    break;

  case 42:
#line 196 "parser-owfn.yy"
    { (yyval.yt_node) = new Node((yyvsp[(1) - (3)].yt_node), (yyvsp[(2) - (3)].yt_node), (yyvsp[(3) - (3)].yt_node)); }
    break;

  case 43:
#line 197 "parser-owfn.yy"
    { (yyval.yt_node) = (yyvsp[(1) - (1)].yt_node);                   }
    break;

  case 44:
#line 201 "parser-owfn.yy"
    { (yyval.yt_node) = new Node((yyvsp[(1) - (1)].yt_node));     }
    break;

  case 45:
#line 202 "parser-owfn.yy"
    { (yyval.yt_node) = (yyvsp[(1) - (2)].yt_node)->addChild((yyvsp[(2) - (2)].yt_node)); }
    break;

  case 46:
#line 207 "parser-owfn.yy"
    { (yyval.yt_node) = new Node(PORT, (yyvsp[(2) - (5)].yt_node), new Node((yyvsp[(3) - (5)].yt_node), (yyvsp[(4) - (5)].yt_node), (yyvsp[(5) - (5)].yt_node))); }
    break;

  case 47:
#line 211 "parser-owfn.yy"
    { (yyval.yt_node) = new Node(SYNCHRONOUS, (yyvsp[(2) - (3)].yt_node)); }
    break;

  case 48:
#line 212 "parser-owfn.yy"
    { (yyval.yt_node) = new Node();                }
    break;

  case 49:
#line 216 "parser-owfn.yy"
    { (yyval.yt_node) = new Node((yyvsp[(1) - (1)].yt_node));     }
    break;

  case 50:
#line 217 "parser-owfn.yy"
    { (yyval.yt_node) = (yyvsp[(1) - (3)].yt_node)->addChild((yyvsp[(3) - (3)].yt_node)); }
    break;

  case 51:
#line 221 "parser-owfn.yy"
    { (yyval.yt_node) = new Node(LABEL, (yyvsp[(1) - (1)].yt_node)); }
    break;

  case 52:
#line 230 "parser-owfn.yy"
    { (yyval.yt_node) = (yyvsp[(1) - (2)].yt_node)->addChild((yyvsp[(2) - (2)].yt_node)); }
    break;

  case 53:
#line 231 "parser-owfn.yy"
    { (yyval.yt_node) = new Node();       }
    break;

  case 54:
#line 236 "parser-owfn.yy"
    { (yyval.yt_node) = new Node(TRANSITION, (yyvsp[(2) - (7)].yt_node), (yyvsp[(3) - (7)].yt_node), (yyvsp[(4) - (7)].yt_node), (yyvsp[(5) - (7)].yt_node), (yyvsp[(6) - (7)].yt_node), (yyvsp[(7) - (7)].yt_node)); }
    break;

  case 55:
#line 240 "parser-owfn.yy"
    { (yyval.yt_node) = new Node((yyvsp[(2) - (3)].yt_int)); }
    break;

  case 56:
#line 241 "parser-owfn.yy"
    { (yyval.yt_node) = new Node(0);  }
    break;

  case 57:
#line 245 "parser-owfn.yy"
    { (yyval.yt_node) = new Node(PRESET, (yyvsp[(2) - (3)].yt_node)); }
    break;

  case 58:
#line 249 "parser-owfn.yy"
    { (yyval.yt_node) = new Node(POSTSET, (yyvsp[(2) - (3)].yt_node)); }
    break;

  case 59:
#line 253 "parser-owfn.yy"
    { (yyval.yt_node) = new Node();       }
    break;

  case 60:
#line 254 "parser-owfn.yy"
    { (yyval.yt_node) = new Node((yyvsp[(1) - (1)].yt_node));     }
    break;

  case 61:
#line 255 "parser-owfn.yy"
    { (yyval.yt_node) = (yyvsp[(1) - (3)].yt_node)->addChild((yyvsp[(3) - (3)].yt_node)); }
    break;

  case 62:
#line 259 "parser-owfn.yy"
    { (yyval.yt_node) = new Node(ARC, (yyvsp[(1) - (3)].yt_node), (yyvsp[(3) - (3)].yt_int)); }
    break;

  case 63:
#line 260 "parser-owfn.yy"
    { (yyval.yt_node) = new Node(ARC, (yyvsp[(1) - (1)].yt_node), 1);  }
    break;

  case 64:
#line 264 "parser-owfn.yy"
    { (yyval.yt_node) = new Node(); }
    break;

  case 65:
#line 265 "parser-owfn.yy"
    { (yyval.yt_node) = (yyvsp[(2) - (3)].yt_node);         }
    break;

  case 66:
#line 269 "parser-owfn.yy"
    { (yyval.yt_node) = new Node();              }
    break;

  case 67:
#line 270 "parser-owfn.yy"
    { (yyval.yt_node) = new Node(CONSTRAIN, (yyvsp[(2) - (3)].yt_node)); }
    break;

  case 68:
#line 279 "parser-owfn.yy"
    { (yyval.yt_node) = new Node((yyvsp[(1) - (2)].yt_node), (yyvsp[(2) - (2)].yt_node)); }
    break;

  case 69:
#line 283 "parser-owfn.yy"
    { (yyval.yt_node) = new Node(INITIALMARKING, (yyvsp[(2) - (3)].yt_node)); }
    break;

  case 70:
#line 287 "parser-owfn.yy"
    { (yyval.yt_node) = new Node();       }
    break;

  case 71:
#line 288 "parser-owfn.yy"
    { (yyval.yt_node) = new Node((yyvsp[(1) - (1)].yt_node));     }
    break;

  case 72:
#line 289 "parser-owfn.yy"
    { (yyval.yt_node) = (yyvsp[(1) - (3)].yt_node)->addChild((yyvsp[(3) - (3)].yt_node)); }
    break;

  case 73:
#line 293 "parser-owfn.yy"
    { (yyval.yt_node) = new Node(MARK, (yyvsp[(1) - (3)].yt_node), (yyvsp[(3) - (3)].yt_int)); }
    break;

  case 74:
#line 294 "parser-owfn.yy"
    { (yyval.yt_node) = new Node(MARK, (yyvsp[(1) - (1)].yt_node), 1);  }
    break;

  case 75:
#line 298 "parser-owfn.yy"
    { (yyval.yt_node) = (yyvsp[(2) - (3)].yt_node);                      }
    break;

  case 76:
#line 299 "parser-owfn.yy"
    { (yyval.yt_node) = new Node(CONDITION, (yyvsp[(1) - (1)].yt_node)); }
    break;

  case 77:
#line 303 "parser-owfn.yy"
    { (yyval.yt_node) = new Node(FORMULA_FALSE); }
    break;

  case 78:
#line 304 "parser-owfn.yy"
    { (yyval.yt_node) = new Node(FORMULA_TRUE);  }
    break;

  case 79:
#line 305 "parser-owfn.yy"
    { (yyval.yt_node) = (yyvsp[(2) - (3)].yt_node);                      }
    break;

  case 80:
#line 309 "parser-owfn.yy"
    { (yyval.yt_node) = new Node((yyvsp[(1) - (1)].yt_node));     }
    break;

  case 81:
#line 310 "parser-owfn.yy"
    { (yyval.yt_node) = (yyvsp[(1) - (3)].yt_node)->addChild((yyvsp[(3) - (3)].yt_node)); }
    break;

  case 82:
#line 314 "parser-owfn.yy"
    { (yyval.yt_node) = new Node(FINALMARKING, (yyvsp[(1) - (1)].yt_node)); }
    break;

  case 83:
#line 318 "parser-owfn.yy"
    { (yyval.yt_node) = (yyvsp[(2) - (3)].yt_node);                            }
    break;

  case 84:
#line 319 "parser-owfn.yy"
    { (yyval.yt_node) = new Node(FORMULA_TRUE);        }
    break;

  case 85:
#line 320 "parser-owfn.yy"
    { (yyval.yt_node) = new Node(FORMULA_FALSE);       }
    break;

  case 86:
#line 321 "parser-owfn.yy"
    { (yyval.yt_node) = new Node(FORMULA_APE);         }
    break;

  case 87:
#line 322 "parser-owfn.yy"
    { (yyval.yt_node) = new Node(FORMULA_NOT, (yyvsp[(2) - (2)].yt_node));     }
    break;

  case 88:
#line 323 "parser-owfn.yy"
    { (yyval.yt_node) = new Node(FORMULA_OR, (yyvsp[(1) - (3)].yt_node), (yyvsp[(3) - (3)].yt_node));  }
    break;

  case 89:
#line 324 "parser-owfn.yy"
    { (yyval.yt_node) = new Node(FORMULA_AND, (yyvsp[(1) - (3)].yt_node), (yyvsp[(3) - (3)].yt_node)); }
    break;

  case 90:
#line 326 "parser-owfn.yy"
    { (yyval.yt_node) = new Node(FORMULA_AAOPE, (yyvsp[(1) - (3)].yt_node));   }
    break;

  case 91:
#line 328 "parser-owfn.yy"
    { (yyval.yt_node) = new Node(FORMULA_AAOIPE, (yyvsp[(1) - (3)].yt_node));  }
    break;

  case 92:
#line 330 "parser-owfn.yy"
    { (yyval.yt_node) = new Node(FORMULA_AAOEPE, (yyvsp[(1) - (3)].yt_node));  }
    break;

  case 93:
#line 331 "parser-owfn.yy"
    { (yyval.yt_node) = new Node(FORMULA_EQ, (yyvsp[(1) - (3)].yt_node), (yyvsp[(3) - (3)].yt_int));  }
    break;

  case 94:
#line 332 "parser-owfn.yy"
    { (yyval.yt_node) = new Node(FORMULA_NE, (yyvsp[(1) - (3)].yt_node), (yyvsp[(3) - (3)].yt_int));  }
    break;

  case 95:
#line 333 "parser-owfn.yy"
    { (yyval.yt_node) = new Node(FORMULA_LT, (yyvsp[(1) - (3)].yt_node), (yyvsp[(3) - (3)].yt_int));  }
    break;

  case 96:
#line 334 "parser-owfn.yy"
    { (yyval.yt_node) = new Node(FORMULA_GT, (yyvsp[(1) - (3)].yt_node), (yyvsp[(3) - (3)].yt_int));  }
    break;

  case 97:
#line 335 "parser-owfn.yy"
    { (yyval.yt_node) = new Node(FORMULA_GE, (yyvsp[(1) - (3)].yt_node), (yyvsp[(3) - (3)].yt_int));  }
    break;

  case 98:
#line 336 "parser-owfn.yy"
    { (yyval.yt_node) = new Node(FORMULA_LE, (yyvsp[(1) - (3)].yt_node), (yyvsp[(3) - (3)].yt_int));  }
    break;


/* Line 1267 of yacc.c.  */
#line 2053 "parser-owfn.cc"
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



