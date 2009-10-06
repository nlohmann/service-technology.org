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

/* All symbols defined below should begin with lp_yy or YY, to avoid
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



/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum lp_yytokentype {
     VAR = 258,
     CONS = 259,
     INTCONS = 260,
     VARIABLECOLON = 261,
     INF = 262,
     SEC_INT = 263,
     SEC_BIN = 264,
     SEC_SEC = 265,
     SEC_SOS = 266,
     SOSDESCR = 267,
     SEC_FREE = 268,
     SIGN = 269,
     AR_M_OP = 270,
     RE_OPLE = 271,
     RE_OPGE = 272,
     END_C = 273,
     COMMA = 274,
     COLON = 275,
     MINIMISE = 276,
     MAXIMISE = 277,
     UNDEFINED = 278
   };
#endif
/* Tokens.  */
#define VAR 258
#define CONS 259
#define INTCONS 260
#define VARIABLECOLON 261
#define INF 262
#define SEC_INT 263
#define SEC_BIN 264
#define SEC_SEC 265
#define SEC_SOS 266
#define SOSDESCR 267
#define SEC_FREE 268
#define SIGN 269
#define AR_M_OP 270
#define RE_OPLE 271
#define RE_OPGE 272
#define END_C 273
#define COMMA 274
#define COLON 275
#define MINIMISE 276
#define MAXIMISE 277
#define UNDEFINED 278




/* Copy the first part of user declarations.  */


#include <string.h>
#include <ctype.h>

#include "lpkit.h"
#include "yacc_read.h"

#ifdef FORTIFY
# include "lp_fortify.h"
#endif

static int HadVar0, HadVar1, HadVar2, HasAR_M_OP, do_add_row, Had_lineair_sum0, HadSign;
static char *Last_var = NULL, *Last_var0 = NULL;
static REAL f, f0, f1;
static int x;
static int state, state0;
static int Sign;
static int isign, isign0;      /* internal_sign variable to make sure nothing goes wrong */
                /* with lookahead */
static int make_neg;   /* is true after the relational operator is seen in order */
                /* to remember if lin_term stands before or after re_op */
static int Within_int_decl = FALSE; /* TRUE when we are within an int declaration */
static int Within_bin_decl = FALSE; /* TRUE when we are within an bin declaration */
static int Within_sec_decl = FALSE; /* TRUE when we are within a sec declaration */
static int Within_sos_decl = FALSE; /* TRUE when we are within a sos declaration */
static int Within_sos_decl1;
static int Within_free_decl = FALSE; /* TRUE when we are within a free declaration */
static short SOStype0; /* SOS type */
static short SOStype; /* SOS type */
static int SOSNr;
static int SOSweight = 0; /* SOS weight */

static int HadConstraint;
static int HadVar;
static int Had_lineair_sum;

extern FILE *lp_yyin;

#define YY_FATAL_ERROR lex_fatal_error

/* let's please C++ users */
#ifdef __cplusplus
extern "C" {
#endif

static int wrap(void)
{
  return(1);
}

static int __WINAPI lp_input_lp_yyin(void *fpin, char *buf, int max_size)
{
  int result;

  if ( (result = fread( (char*)buf, sizeof(char), max_size, (FILE *) fpin)) < 0)
    YY_FATAL_ERROR( "read() in flex scanner failed");

  return(result);
}

static read_modeldata_func *lp_input;

#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) result = lp_input((void *) lp_yyin, buf, max_size);

#ifdef __cplusplus
};
#endif

#define lp_yywrap wrap
#define lp_yyerror read_error

#include "lp_rlp.h"



/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define lp_yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */


#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 lp_yytype_uint8;
#else
typedef unsigned char lp_yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 lp_yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char lp_yytype_int8;
#else
typedef short int lp_yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 lp_yytype_uint16;
#else
typedef unsigned short int lp_yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 lp_yytype_int16;
#else
typedef short int lp_yytype_int16;
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

#if ! defined lp_yyoverflow || YYERROR_VERBOSE

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
#endif /* ! defined lp_yyoverflow || YYERROR_VERBOSE */


#if (! defined lp_yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union lp_yyalloc
{
  lp_yytype_int16 lp_yyss;
  YYSTYPE lp_yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union lp_yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (lp_yytype_int16) + sizeof (YYSTYPE)) \
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
	  YYSIZE_T lp_yyi;				\
	  for (lp_yyi = 0; lp_yyi < (Count); lp_yyi++)	\
	    (To)[lp_yyi] = (From)[lp_yyi];		\
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
	YYSIZE_T lp_yynewbytes;						\
	YYCOPY (&lp_yyptr->Stack, Stack, lp_yysize);				\
	Stack = &lp_yyptr->Stack;						\
	lp_yynewbytes = lp_yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	lp_yyptr += lp_yynewbytes / sizeof (*lp_yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   111

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  24
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  56
/* YYNRULES -- Number of rules.  */
#define YYNRULES  88
/* YYNRULES -- Number of states.  */
#define YYNSTATES  122

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   278

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? lp_yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const lp_yytype_uint8 lp_yytranslate[] =
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
      15,    16,    17,    18,    19,    20,    21,    22,    23
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const lp_yytype_uint8 lp_yyprhs[] =
{
       0,     0,     3,     4,     5,    10,    13,    16,    18,    21,
      23,    25,    27,    29,    31,    34,    36,    37,    41,    42,
      43,    44,    53,    55,    56,    57,    63,    65,    67,    69,
      70,    74,    75,    78,    80,    83,    86,    88,    89,    93,
      95,    97,   100,   102,   104,   106,   108,   110,   112,   114,
     116,   118,   120,   122,   125,   127,   129,   131,   133,   135,
     136,   140,   141,   147,   149,   152,   154,   155,   159,   161,
     162,   167,   169,   172,   174,   176,   178,   182,   184,   186,
     188,   189,   191,   193,   196,   200,   203,   206,   209
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const lp_yytype_int8 lp_yyrhs[] =
{
      26,     0,    -1,    -1,    -1,    27,    28,    31,    57,    -1,
      22,    29,    -1,    21,    29,    -1,    29,    -1,    30,    18,
      -1,    25,    -1,    45,    -1,    25,    -1,    32,    -1,    33,
      -1,    32,    33,    -1,    35,    -1,    -1,     6,    34,    35,
      -1,    -1,    -1,    -1,    42,    36,    51,    37,    43,    38,
      39,    18,    -1,    25,    -1,    -1,    -1,    51,    40,    52,
      41,    56,    -1,    25,    -1,    43,    -1,    45,    -1,    -1,
       7,    44,    56,    -1,    -1,    46,    47,    -1,    48,    -1,
      47,    48,    -1,    54,    49,    -1,    53,    -1,    -1,    55,
      50,     3,    -1,    16,    -1,    17,    -1,    54,    53,    -1,
       7,    -1,     5,    -1,     4,    -1,    25,    -1,    14,    -1,
      25,    -1,    15,    -1,    25,    -1,    25,    -1,    58,    -1,
      60,    -1,    58,    60,    -1,     8,    -1,     9,    -1,    10,
      -1,    11,    -1,    13,    -1,    -1,    59,    61,    64,    -1,
      -1,    63,    65,    70,    67,    18,    -1,    62,    -1,    64,
      62,    -1,    25,    -1,    -1,    12,    66,    76,    -1,    25,
      -1,    -1,    16,     5,    68,    69,    -1,    25,    -1,    20,
       5,    -1,    25,    -1,    71,    -1,    77,    -1,    71,    72,
      77,    -1,    25,    -1,    19,    -1,    25,    -1,    -1,    25,
      -1,    25,    -1,     3,    73,    -1,     6,    74,    78,    -1,
      53,    75,    -1,    76,    79,    -1,     3,    73,    -1,     6,
      74,    53,    75,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const lp_yytype_uint16 lp_yyrline[] =
{
       0,    88,    88,    92,    92,   114,   118,   122,   125,   136,
     137,   167,   168,   171,   172,   176,   178,   177,   187,   192,
     199,   186,   239,   246,   256,   245,   281,   291,   297,   299,
     298,   306,   306,   324,   325,   329,   365,   370,   369,   382,
     382,   385,   387,   398,   398,   401,   406,   413,   417,   423,
     441,   442,   445,   446,   449,   449,   449,   449,   449,   454,
     453,   461,   461,   483,   484,   487,   489,   488,   496,   505,
     503,   519,   523,   530,   531,   534,   535,   540,   541,   544,
     570,   588,   610,   625,   627,   632,   634,   639,   641
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const lp_yytname[] =
{
  "$end", "error", "$undefined", "VAR", "CONS", "INTCONS",
  "VARIABLECOLON", "INF", "SEC_INT", "SEC_BIN", "SEC_SEC", "SEC_SOS",
  "SOSDESCR", "SEC_FREE", "SIGN", "AR_M_OP", "RE_OPLE", "RE_OPGE", "END_C",
  "COMMA", "COLON", "MINIMISE", "MAXIMISE", "UNDEFINED", "$accept",
  "EMPTY", "inputfile", "@1", "objective_function", "real_of",
  "lineair_sum", "constraints", "x_constraints", "constraint", "@2",
  "real_constraint", "@3", "@4", "@5", "optionalrange", "@6", "@7",
  "x_lineair_sum2", "x_lineair_sum3", "@8", "x_lineair_sum", "@9",
  "x_lineair_sum1", "x_lineair_term", "x_lineair_term1", "@10", "RE_OP",
  "cons_term", "REALCONS", "x_SIGN", "optional_AR_M_OP", "RHS_STORE",
  "int_bin_sec_sos_free_declarations", "real_int_bin_sec_sos_free_decls",
  "SEC_INT_BIN_SEC_SOS_FREE", "int_bin_sec_sos_free_declaration", "@11",
  "xx_int_bin_sec_sos_free_declaration", "@12",
  "x_int_bin_sec_sos_free_declaration", "optionalsos", "@13",
  "optionalsostype", "@14", "optionalSOSweight", "vars", "x_vars",
  "optionalcomma", "variable", "variablecolon", "sosweight", "sosdescr",
  "onevarwithoptionalweight", "INTCONSorVARIABLE",
  "x_onevarwithoptionalweight", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const lp_yytype_uint16 lp_yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const lp_yytype_uint8 lp_yyr1[] =
{
       0,    24,    25,    27,    26,    28,    28,    28,    29,    30,
      30,    31,    31,    32,    32,    33,    34,    33,    36,    37,
      38,    35,    39,    40,    41,    39,    42,    42,    43,    44,
      43,    46,    45,    47,    47,    48,    49,    50,    49,    51,
      51,    52,    52,    53,    53,    54,    54,    55,    55,    56,
      57,    57,    58,    58,    59,    59,    59,    59,    59,    61,
      60,    63,    62,    64,    64,    65,    66,    65,    67,    68,
      67,    69,    69,    70,    70,    71,    71,    72,    72,    73,
      74,    75,    76,    77,    77,    78,    78,    79,    79
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const lp_yytype_uint8 lp_yyr2[] =
{
       0,     2,     0,     0,     4,     2,     2,     1,     2,     1,
       1,     1,     1,     1,     2,     1,     0,     3,     0,     0,
       0,     8,     1,     0,     0,     5,     1,     1,     1,     0,
       3,     0,     2,     1,     2,     2,     1,     0,     3,     1,
       1,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     2,     1,     1,     1,     1,     1,     0,
       3,     0,     5,     1,     2,     1,     0,     3,     1,     0,
       4,     1,     2,     1,     1,     1,     3,     1,     1,     1,
       0,     1,     1,     2,     3,     2,     2,     2,     4
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const lp_yytype_uint8 lp_yydefact[] =
{
       3,     0,    31,     1,    31,    31,     9,     2,     7,     0,
      10,     2,     6,     5,    16,    29,    11,     2,    12,    13,
      15,    18,    27,    28,     8,    46,    45,     2,    33,     2,
      31,     2,    54,    55,    56,    57,    58,    50,     4,    51,
      59,    52,    26,    14,     0,    34,    44,    43,    48,    47,
      35,    36,    37,    17,    49,    30,    53,    61,    39,    40,
      19,     0,    63,     2,    60,    31,    38,    66,    65,     2,
      64,    20,     2,     2,    80,    73,     2,     2,    75,     2,
      82,    67,    79,    83,     2,     0,    68,     0,    78,    77,
       0,    22,     0,    23,     2,     0,    84,    69,    62,    76,
      21,     2,    81,    85,     2,    80,    86,     2,    42,    24,
       0,    87,     0,     0,    71,    70,     2,    41,     2,    72,
      25,    88
};

/* YYDEFGOTO[NTERM-NUM].  */
static const lp_yytype_int8 lp_yydefgoto[] =
{
      -1,     6,     1,     2,     7,     8,     9,    17,    18,    19,
      30,    20,    44,    65,    79,    92,   101,   116,    21,    22,
      31,    23,    11,    27,    28,    50,    61,    60,   109,    51,
      29,    52,    55,    38,    39,    40,    41,    57,    62,    63,
      64,    69,    72,    87,   107,   115,    76,    77,    90,    83,
      84,   103,    81,    78,    96,   106
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -84
static const lp_yytype_int8 lp_yypact[] =
{
     -84,     2,    42,   -84,   -11,   -11,   -84,    32,   -84,    -5,
     -84,     3,   -84,   -84,   -84,   -84,     9,    40,    27,   -84,
     -84,   -84,   -84,   -84,   -84,   -84,   -84,    41,   -84,     4,
      -2,   -84,   -84,   -84,   -84,   -84,   -84,   -84,   -84,    40,
     -84,   -84,   -84,   -84,    51,   -84,   -84,   -84,   -84,   -84,
     -84,   -84,   -84,   -84,   -84,   -84,   -84,   -84,   -84,   -84,
     -84,    37,   -84,    33,     0,    45,   -84,   -84,   -84,    77,
     -84,   -84,   -84,   -84,   -84,   -84,    38,    55,   -84,    51,
     -84,   -84,   -84,   -84,    81,    56,   -84,    57,   -84,   -84,
      77,   -84,    64,   -84,   -84,    78,   -84,   -84,   -84,   -84,
     -84,    14,   -84,   -84,   -84,   -84,   -84,    70,   -84,   -84,
      81,   -84,    81,    86,   -84,   -84,   -84,   -84,   -84,   -84,
     -84,   -84
};

/* YYPGOTO[NTERM-NUM].  */
static const lp_yytype_int8 lp_yypgoto[] =
{
     -84,    -7,   -84,   -84,   -84,    84,   -84,   -84,   -84,    75,
     -84,    62,   -84,   -84,   -84,   -84,   -84,   -84,   -84,    30,
     -84,    74,   -84,   -84,    69,   -84,   -84,    19,   -84,   -83,
       1,   -84,   -17,   -84,   -84,   -84,    65,   -84,    39,   -84,
     -84,   -84,   -84,   -84,   -84,   -84,   -84,   -84,   -84,    -3,
       5,   -13,    22,    17,   -84,   -84
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -75
static const lp_yytype_int8 lp_yytable[] =
{
      16,    94,     3,   -61,    26,    15,   -61,    -2,    46,    47,
      37,    42,   -61,    24,    -2,    -2,   -61,    25,   -61,    48,
      26,   108,    49,    42,    54,   -26,   -26,   117,    25,   118,
     -31,   -31,   -31,    14,    15,   -31,   -31,   -31,    14,    15,
      66,   -31,   -31,    -2,    -2,    67,   -31,   -31,    32,    33,
      34,    35,    15,    36,    85,    25,    68,   -32,   -32,   -32,
      -2,    97,    75,     4,     5,    80,    82,    58,    59,    86,
      89,   -74,    91,   -74,    88,    98,    10,    80,    10,    10,
      73,   104,   100,    74,   105,    46,    47,   102,    12,    13,
     113,   119,    53,    43,    26,    71,    45,    82,    93,   120,
     114,   111,   110,    70,    56,   121,    95,    99,     0,    54,
     112,   102
};

static const lp_yytype_int8 lp_yycheck[] =
{
       7,    84,     0,     3,    11,     7,     6,    18,     4,     5,
      17,    18,    12,    18,    16,    17,    16,    14,    18,    15,
      27,     7,    29,    30,    31,    16,    17,   110,    14,   112,
       3,     4,     5,     6,     7,     3,     4,     5,     6,     7,
       3,    14,    15,    16,    17,    12,    14,    15,     8,     9,
      10,    11,     7,    13,    16,    14,    63,    16,    17,    18,
      18,     5,    69,    21,    22,    72,    73,    16,    17,    76,
      77,    16,    79,    18,    19,    18,     2,    84,     4,     5,
       3,     3,    18,     6,     6,     4,     5,    94,     4,     5,
      20,     5,    30,    18,   101,    65,    27,   104,    79,   116,
     107,   104,   101,    64,    39,   118,    84,    90,    -1,   116,
     105,   118
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const lp_yytype_uint8 lp_yystos[] =
{
       0,    26,    27,     0,    21,    22,    25,    28,    29,    30,
      45,    46,    29,    29,     6,     7,    25,    31,    32,    33,
      35,    42,    43,    45,    18,    14,    25,    47,    48,    54,
      34,    44,     8,     9,    10,    11,    13,    25,    57,    58,
      59,    60,    25,    33,    36,    48,     4,     5,    15,    25,
      49,    53,    55,    35,    25,    56,    60,    61,    16,    17,
      51,    50,    62,    63,    64,    37,     3,    12,    25,    65,
      62,    43,    66,     3,     6,    25,    70,    71,    77,    38,
      25,    76,    25,    73,    74,    16,    25,    67,    19,    25,
      72,    25,    39,    51,    53,    76,    78,     5,    18,    77,
      18,    40,    25,    75,     3,     6,    79,    68,     7,    52,
      54,    73,    74,    20,    25,    69,    41,    53,    53,     5,
      56,    75
};

#define lp_yyerrok		(lp_yyerrstatus = 0)
#define lp_yyclearin	(lp_yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto lp_yyacceptlab
#define YYABORT		goto lp_yyabortlab
#define YYERROR		goto lp_yyerrorlab


/* Like YYERROR except do call lp_yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto lp_yyerrlab

#define YYRECOVERING()  (!!lp_yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (lp_yychar == YYEMPTY && lp_yylen == 1)				\
    {								\
      lp_yychar = (Token);						\
      lp_yylval = (Value);						\
      lp_yytoken = YYTRANSLATE (lp_yychar);				\
      YYPOPSTACK (1);						\
      goto lp_yybackup;						\
    }								\
  else								\
    {								\
      lp_yyerror (YY_("syntax error: cannot back up")); \
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


/* YYLEX -- calling `lp_yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX lp_yylex (YYLEX_PARAM)
#else
# define YYLEX lp_yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (lp_yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (lp_yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      lp_yy_symbol_print (stderr,						  \
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
lp_yy_symbol_value_print (FILE *lp_yyoutput, int lp_yytype, YYSTYPE const * const lp_yyvaluep)
#else
static void
lp_yy_symbol_value_print (lp_yyoutput, lp_yytype, lp_yyvaluep)
    FILE *lp_yyoutput;
    int lp_yytype;
    YYSTYPE const * const lp_yyvaluep;
#endif
{
  if (!lp_yyvaluep)
    return;
# ifdef YYPRINT
  if (lp_yytype < YYNTOKENS)
    YYPRINT (lp_yyoutput, lp_yytoknum[lp_yytype], *lp_yyvaluep);
# else
  YYUSE (lp_yyoutput);
# endif
  switch (lp_yytype)
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
lp_yy_symbol_print (FILE *lp_yyoutput, int lp_yytype, YYSTYPE const * const lp_yyvaluep)
#else
static void
lp_yy_symbol_print (lp_yyoutput, lp_yytype, lp_yyvaluep)
    FILE *lp_yyoutput;
    int lp_yytype;
    YYSTYPE const * const lp_yyvaluep;
#endif
{
  if (lp_yytype < YYNTOKENS)
    YYFPRINTF (lp_yyoutput, "token %s (", lp_yytname[lp_yytype]);
  else
    YYFPRINTF (lp_yyoutput, "nterm %s (", lp_yytname[lp_yytype]);

  lp_yy_symbol_value_print (lp_yyoutput, lp_yytype, lp_yyvaluep);
  YYFPRINTF (lp_yyoutput, ")");
}

/*------------------------------------------------------------------.
| lp_yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
lp_yy_stack_print (lp_yytype_int16 *bottom, lp_yytype_int16 *top)
#else
static void
lp_yy_stack_print (bottom, top)
    lp_yytype_int16 *bottom;
    lp_yytype_int16 *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (lp_yydebug)							\
    lp_yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
lp_yy_reduce_print (YYSTYPE *lp_yyvsp, int lp_yyrule)
#else
static void
lp_yy_reduce_print (lp_yyvsp, lp_yyrule)
    YYSTYPE *lp_yyvsp;
    int lp_yyrule;
#endif
{
  int lp_yynrhs = lp_yyr2[lp_yyrule];
  int lp_yyi;
  unsigned long int lp_yylno = lp_yyrline[lp_yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     lp_yyrule - 1, lp_yylno);
  /* The symbols being reduced.  */
  for (lp_yyi = 0; lp_yyi < lp_yynrhs; lp_yyi++)
    {
      fprintf (stderr, "   $%d = ", lp_yyi + 1);
      lp_yy_symbol_print (stderr, lp_yyrhs[lp_yyprhs[lp_yyrule] + lp_yyi],
		       &(lp_yyvsp[(lp_yyi + 1) - (lp_yynrhs)])
		       		       );
      fprintf (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (lp_yydebug)				\
    lp_yy_reduce_print (lp_yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int lp_yydebug;
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

# ifndef lp_yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define lp_yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
lp_yystrlen (const char *lp_yystr)
#else
static YYSIZE_T
lp_yystrlen (lp_yystr)
    const char *lp_yystr;
#endif
{
  YYSIZE_T lp_yylen;
  for (lp_yylen = 0; lp_yystr[lp_yylen]; lp_yylen++)
    continue;
  return lp_yylen;
}
#  endif
# endif

# ifndef lp_yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define lp_yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
lp_yystpcpy (char *lp_yydest, const char *lp_yysrc)
#else
static char *
lp_yystpcpy (lp_yydest, lp_yysrc)
    char *lp_yydest;
    const char *lp_yysrc;
#endif
{
  char *lp_yyd = lp_yydest;
  const char *lp_yys = lp_yysrc;

  while ((*lp_yyd++ = *lp_yys++) != '\0')
    continue;

  return lp_yyd - 1;
}
#  endif
# endif

# ifndef lp_yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for lp_yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from lp_yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
lp_yytnamerr (char *lp_yyres, const char *lp_yystr)
{
  if (*lp_yystr == '"')
    {
      YYSIZE_T lp_yyn = 0;
      char const *lp_yyp = lp_yystr;

      for (;;)
	switch (*++lp_yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++lp_yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (lp_yyres)
	      lp_yyres[lp_yyn] = *lp_yyp;
	    lp_yyn++;
	    break;

	  case '"':
	    if (lp_yyres)
	      lp_yyres[lp_yyn] = '\0';
	    return lp_yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! lp_yyres)
    return lp_yystrlen (lp_yystr);

  return lp_yystpcpy (lp_yyres, lp_yystr) - lp_yyres;
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
lp_yysyntax_error (char *lp_yyresult, int lp_yystate, int lp_yychar)
{
  int lp_yyn = lp_yypact[lp_yystate];

  if (! (YYPACT_NINF < lp_yyn && lp_yyn <= YYLAST))
    return 0;
  else
    {
      int lp_yytype = YYTRANSLATE (lp_yychar);
      YYSIZE_T lp_yysize0 = lp_yytnamerr (0, lp_yytname[lp_yytype]);
      YYSIZE_T lp_yysize = lp_yysize0;
      YYSIZE_T lp_yysize1;
      int lp_yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *lp_yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int lp_yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *lp_yyfmt;
      char const *lp_yyf;
      static char const lp_yyunexpected[] = "syntax error, unexpected %s";
      static char const lp_yyexpecting[] = ", expecting %s";
      static char const lp_yyor[] = " or %s";
      char lp_yyformat[sizeof lp_yyunexpected
		    + sizeof lp_yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof lp_yyor - 1))];
      char const *lp_yyprefix = lp_yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int lp_yyxbegin = lp_yyn < 0 ? -lp_yyn : 0;

      /* Stay within bounds of both lp_yycheck and lp_yytname.  */
      int lp_yychecklim = YYLAST - lp_yyn + 1;
      int lp_yyxend = lp_yychecklim < YYNTOKENS ? lp_yychecklim : YYNTOKENS;
      int lp_yycount = 1;

      lp_yyarg[0] = lp_yytname[lp_yytype];
      lp_yyfmt = lp_yystpcpy (lp_yyformat, lp_yyunexpected);

      for (lp_yyx = lp_yyxbegin; lp_yyx < lp_yyxend; ++lp_yyx)
	if (lp_yycheck[lp_yyx + lp_yyn] == lp_yyx && lp_yyx != YYTERROR)
	  {
	    if (lp_yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		lp_yycount = 1;
		lp_yysize = lp_yysize0;
		lp_yyformat[sizeof lp_yyunexpected - 1] = '\0';
		break;
	      }
	    lp_yyarg[lp_yycount++] = lp_yytname[lp_yyx];
	    lp_yysize1 = lp_yysize + lp_yytnamerr (0, lp_yytname[lp_yyx]);
	    lp_yysize_overflow |= (lp_yysize1 < lp_yysize);
	    lp_yysize = lp_yysize1;
	    lp_yyfmt = lp_yystpcpy (lp_yyfmt, lp_yyprefix);
	    lp_yyprefix = lp_yyor;
	  }

      lp_yyf = YY_(lp_yyformat);
      lp_yysize1 = lp_yysize + lp_yystrlen (lp_yyf);
      lp_yysize_overflow |= (lp_yysize1 < lp_yysize);
      lp_yysize = lp_yysize1;

      if (lp_yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (lp_yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *lp_yyp = lp_yyresult;
	  int lp_yyi = 0;
	  while ((*lp_yyp = *lp_yyf) != '\0')
	    {
	      if (*lp_yyp == '%' && lp_yyf[1] == 's' && lp_yyi < lp_yycount)
		{
		  lp_yyp += lp_yytnamerr (lp_yyp, lp_yyarg[lp_yyi++]);
		  lp_yyf += 2;
		}
	      else
		{
		  lp_yyp++;
		  lp_yyf++;
		}
	    }
	}
      return lp_yysize;
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
lp_yydestruct (const char *lp_yymsg, int lp_yytype, YYSTYPE *lp_yyvaluep)
#else
static void
lp_yydestruct (lp_yymsg, lp_yytype, lp_yyvaluep)
    const char *lp_yymsg;
    int lp_yytype;
    YYSTYPE *lp_yyvaluep;
#endif
{
  YYUSE (lp_yyvaluep);

  if (!lp_yymsg)
    lp_yymsg = "Deleting";
  YY_SYMBOL_PRINT (lp_yymsg, lp_yytype, lp_yyvaluep, lp_yylocationp);

  switch (lp_yytype)
    {

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int lp_yyparse (void *YYPARSE_PARAM);
#else
int lp_yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int lp_yyparse (void);
#else
int lp_yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The look-ahead symbol.  */
int lp_yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE lp_yylval;

/* Number of syntax errors so far.  */
int lp_yynerrs;



/*----------.
| lp_yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
lp_yyparse (void *YYPARSE_PARAM)
#else
int
lp_yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
lp_yyparse (void)
#else
int
lp_yyparse ()

#endif
#endif
{
  
  int lp_yystate;
  int lp_yyn;
  int lp_yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int lp_yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int lp_yytoken = 0;
#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char lp_yymsgbuf[128];
  char *lp_yymsg = lp_yymsgbuf;
  YYSIZE_T lp_yymsg_alloc = sizeof lp_yymsgbuf;
#endif

  /* Three stacks and their tools:
     `lp_yyss': related to states,
     `lp_yyvs': related to semantic values,
     `lp_yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow lp_yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  lp_yytype_int16 lp_yyssa[YYINITDEPTH];
  lp_yytype_int16 *lp_yyss = lp_yyssa;
  lp_yytype_int16 *lp_yyssp;

  /* The semantic value stack.  */
  YYSTYPE lp_yyvsa[YYINITDEPTH];
  YYSTYPE *lp_yyvs = lp_yyvsa;
  YYSTYPE *lp_yyvsp;



#define YYPOPSTACK(N)   (lp_yyvsp -= (N), lp_yyssp -= (N))

  YYSIZE_T lp_yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE lp_yyval;


  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int lp_yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  lp_yystate = 0;
  lp_yyerrstatus = 0;
  lp_yynerrs = 0;
  lp_yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  lp_yyssp = lp_yyss;
  lp_yyvsp = lp_yyvs;

  goto lp_yysetstate;

/*------------------------------------------------------------.
| lp_yynewstate -- Push a new state, which is found in lp_yystate.  |
`------------------------------------------------------------*/
 lp_yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  lp_yyssp++;

 lp_yysetstate:
  *lp_yyssp = lp_yystate;

  if (lp_yyss + lp_yystacksize - 1 <= lp_yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T lp_yysize = lp_yyssp - lp_yyss + 1;

#ifdef lp_yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *lp_yyvs1 = lp_yyvs;
	lp_yytype_int16 *lp_yyss1 = lp_yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if lp_yyoverflow is a macro.  */
	lp_yyoverflow (YY_("memory exhausted"),
		    &lp_yyss1, lp_yysize * sizeof (*lp_yyssp),
		    &lp_yyvs1, lp_yysize * sizeof (*lp_yyvsp),

		    &lp_yystacksize);

	lp_yyss = lp_yyss1;
	lp_yyvs = lp_yyvs1;
      }
#else /* no lp_yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto lp_yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= lp_yystacksize)
	goto lp_yyexhaustedlab;
      lp_yystacksize *= 2;
      if (YYMAXDEPTH < lp_yystacksize)
	lp_yystacksize = YYMAXDEPTH;

      {
	lp_yytype_int16 *lp_yyss1 = lp_yyss;
	union lp_yyalloc *lp_yyptr =
	  (union lp_yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (lp_yystacksize));
	if (! lp_yyptr)
	  goto lp_yyexhaustedlab;
	YYSTACK_RELOCATE (lp_yyss);
	YYSTACK_RELOCATE (lp_yyvs);

#  undef YYSTACK_RELOCATE
	if (lp_yyss1 != lp_yyssa)
	  YYSTACK_FREE (lp_yyss1);
      }
# endif
#endif /* no lp_yyoverflow */

      lp_yyssp = lp_yyss + lp_yysize - 1;
      lp_yyvsp = lp_yyvs + lp_yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) lp_yystacksize));

      if (lp_yyss + lp_yystacksize - 1 <= lp_yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", lp_yystate));

  goto lp_yybackup;

/*-----------.
| lp_yybackup.  |
`-----------*/
lp_yybackup:

  /* Do appropriate processing given the current state.  Read a
     look-ahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to look-ahead token.  */
  lp_yyn = lp_yypact[lp_yystate];
  if (lp_yyn == YYPACT_NINF)
    goto lp_yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
  if (lp_yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      lp_yychar = YYLEX;
    }

  if (lp_yychar <= YYEOF)
    {
      lp_yychar = lp_yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      lp_yytoken = YYTRANSLATE (lp_yychar);
      YY_SYMBOL_PRINT ("Next token is", lp_yytoken, &lp_yylval, &lp_yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  lp_yyn += lp_yytoken;
  if (lp_yyn < 0 || YYLAST < lp_yyn || lp_yycheck[lp_yyn] != lp_yytoken)
    goto lp_yydefault;
  lp_yyn = lp_yytable[lp_yyn];
  if (lp_yyn <= 0)
    {
      if (lp_yyn == 0 || lp_yyn == YYTABLE_NINF)
	goto lp_yyerrlab;
      lp_yyn = -lp_yyn;
      goto lp_yyreduce;
    }

  if (lp_yyn == YYFINAL)
    YYACCEPT;

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (lp_yyerrstatus)
    lp_yyerrstatus--;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", lp_yytoken, &lp_yylval, &lp_yylloc);

  /* Discard the shifted token unless it is eof.  */
  if (lp_yychar != YYEOF)
    lp_yychar = YYEMPTY;

  lp_yystate = lp_yyn;
  *++lp_yyvsp = lp_yylval;

  goto lp_yynewstate;


/*-----------------------------------------------------------.
| lp_yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
lp_yydefault:
  lp_yyn = lp_yydefact[lp_yystate];
  if (lp_yyn == 0)
    goto lp_yyerrlab;
  goto lp_yyreduce;


/*-----------------------------.
| lp_yyreduce -- Do a reduction.  |
`-----------------------------*/
lp_yyreduce:
  /* lp_yyn is the number of a rule to reduce with.  */
  lp_yylen = lp_yyr2[lp_yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  lp_yyval = lp_yyvsp[1-lp_yylen];


  YY_REDUCE_PRINT (lp_yyn);
  switch (lp_yyn)
    {
        case 3:

    {
  isign = 0;
  make_neg = 0;
  Sign = 0;
  HadConstraint = FALSE;
  HadVar = HadVar0 = FALSE;
}
    break;

  case 5:

    {
  set_obj_dir(TRUE);
}
    break;

  case 6:

    {
  set_obj_dir(FALSE);
}
    break;

  case 8:

    {
  add_row();
  HadConstraint = FALSE;
  HadVar = HadVar0 = FALSE;
  isign = 0;
  make_neg = 0;
}
    break;

  case 16:

    {
  if(!add_constraint_name(Last_var))
    YYABORT;
  HadConstraint = TRUE;
}
    break;

  case 18:

    {
  HadVar1 = HadVar0;
  HadVar0 = FALSE;
}
    break;

  case 19:

    {
  if(!store_re_op((char *) lp_yytext, HadConstraint, HadVar, Had_lineair_sum))
    YYABORT;
  make_neg = 1;
  f1 = 0;
}
    break;

  case 20:

    {
  Had_lineair_sum0 = Had_lineair_sum;
  Had_lineair_sum = TRUE;
  HadVar2 = HadVar0;
  HadVar0 = FALSE;
  do_add_row = FALSE;
  if(HadConstraint && !HadVar ) {
    /* it is a range */
    /* already handled */
  }
  else if(!HadConstraint && HadVar) {
    /* it is a bound */

    if(!store_bounds(TRUE))
      YYABORT;
  }
  else {
    /* it is a row restriction */
    if(HadConstraint && HadVar)
      store_re_op("", HadConstraint, HadVar, Had_lineair_sum); /* makes sure that data stored in temporary buffers is treated correctly */
    do_add_row = TRUE;
  }
}
    break;

  case 21:

    {
  if((!HadVar) && (!HadConstraint)) {
    lp_yyerror("parse error");
    YYABORT;
  }
  if(do_add_row)
    add_row();
  HadConstraint = FALSE;
  HadVar = HadVar0 = FALSE;
  isign = 0;
  make_neg = 0;
  null_tmp_store(TRUE);
}
    break;

  case 22:

    {
  if((!HadVar1) && (Had_lineair_sum0))
    if(!negate_constraint())
      YYABORT;
}
    break;

  case 23:

    {
  make_neg = 0;
  isign = 0;
  if(HadConstraint)
    HadVar = Had_lineair_sum = FALSE;
  HadVar0 = FALSE;
  if(!store_re_op((char *) ((*lp_yytext == '<') ? ">" : (*lp_yytext == '>') ? "<" : lp_yytext), HadConstraint, HadVar, Had_lineair_sum))
    YYABORT;
}
    break;

  case 24:

    {
  f -= f1;
}
    break;

  case 25:

    {
  if((HadVar1) || (!HadVar2) || (HadVar0)) {
    lp_yyerror("parse error");
    YYABORT;
  }

  if(HadConstraint && !HadVar ) {
    /* it is a range */
    /* already handled */
    if(!negate_constraint())
      YYABORT;
  }
  else if(!HadConstraint && HadVar) {
    /* it is a bound */

    if(!store_bounds(TRUE))
      YYABORT;
  }
}
    break;

  case 26:

    {
  /* to allow a range */
  /* constraint: < max */
  if(!HadConstraint) {
    lp_yyerror("parse error");
    YYABORT;
  }
  Had_lineair_sum = FALSE;
}
    break;

  case 27:

    {
  Had_lineair_sum = TRUE;
}
    break;

  case 29:

    {
  isign = Sign;
}
    break;

  case 31:

    {
  state = state0 = 0;
}
    break;

  case 32:

    {
  if (state == 1) {
    /* RHS_STORE */
    if (    (isign0 || !make_neg)
        && !(isign0 && !make_neg)) /* but not both! */
      f0 = -f0;
    if(make_neg)
      f1 += f0;
    if(!rhs_store(f0, HadConstraint, HadVar, Had_lineair_sum))
      YYABORT;
  }
}
    break;

  case 35:

    {
  if ((HadSign || state == 1) && (state0 == 1)) {
    /* RHS_STORE */
    if (    (isign0 || !make_neg)
        && !(isign0 && !make_neg)) /* but not both! */
      f0 = -f0;
    if(make_neg)
      f1 += f0;
    if(!rhs_store(f0, HadConstraint, HadVar, Had_lineair_sum))
      YYABORT;
  }
  if (state == 1) {
    f0 = f;
    isign0 = isign;
  }
  if (state == 2) {
    if((HadSign) || (state0 != 1)) {
     isign0 = isign;
     f0 = 1.0;
    }
    if (    (isign0 || make_neg)
        && !(isign0 && make_neg)) /* but not both! */
      f0 = -f0;
    if(!var_store(Last_var, f0, HadConstraint, HadVar, Had_lineair_sum)) {
      lp_yyerror("var_store failed");
      YYABORT;
    }
    HadConstraint |= HadVar;
    HadVar = HadVar0 = TRUE;
  }
  state0 = state;
}
    break;

  case 36:

    {
  state = 1;
}
    break;

  case 37:

    {
  if ((HasAR_M_OP) && (state != 1)) {
    lp_yyerror("parse error");
    YYABORT;
  }
}
    break;

  case 38:

    {
  state = 2;
}
    break;

  case 42:

    {
  isign = Sign;
}
    break;

  case 45:

    {
  isign = 0;
  HadSign = FALSE;
}
    break;

  case 46:

    {
  isign = Sign;
  HadSign = TRUE;
}
    break;

  case 47:

    {
  HasAR_M_OP = FALSE;
}
    break;

  case 48:

    {
  HasAR_M_OP = TRUE;
}
    break;

  case 49:

    {
  if (    (isign || !make_neg)
      && !(isign && !make_neg)) /* but not both! */
    f = -f;
  if(!rhs_store(f, HadConstraint, HadVar, Had_lineair_sum))
    YYABORT;
  isign = 0;
}
    break;

  case 59:

    {
  Within_sos_decl1 = Within_sos_decl;
}
    break;

  case 61:

    {
  if((!Within_int_decl) && (!Within_sec_decl) && (!Within_sos_decl1) && (!Within_free_decl)) {
    lp_yyerror("parse error");
    YYABORT;
  }
  SOStype = SOStype0;
  check_int_sec_sos_free_decl(Within_int_decl, Within_sec_decl, Within_sos_decl1 = (Within_sos_decl1 ? 1 : 0), Within_free_decl);
}
    break;

  case 62:

    {
  if((Within_sos_decl1) && (SOStype == 0))
  {
    lp_yyerror("Unsupported SOS type (0)");
    YYABORT;
  }
}
    break;

  case 66:

    {
  FREE(Last_var0);
  Last_var0 = strdup(Last_var);
}
    break;

  case 68:

    {
  if(Within_sos_decl1) {
    set_sos_type(SOStype);
    set_sos_weight((double) SOSweight, 1);
  }
}
    break;

  case 69:

    {
  if((Within_sos_decl1) && (!SOStype))
  {
    set_sos_type(SOStype = (short) (f + .1));
  }
  else
  {
    lp_yyerror("SOS type not expected");
    YYABORT;
  }
}
    break;

  case 71:

    {
  set_sos_weight((double) SOSweight, 1);
}
    break;

  case 72:

    {
  set_sos_weight(f, 1);
}
    break;

  case 79:

    {
  if(Within_sos_decl1 == 1)
  {
    char buf[16];

    SOSweight++;
    sprintf(buf, "SOS%d", SOSweight);
    storevarandweight(buf);

    check_int_sec_sos_free_decl(Within_int_decl, Within_sec_decl, 2, Within_free_decl);
    Within_sos_decl1 = 2;
    SOSNr = 0;
  }

  storevarandweight(Last_var);

  if(Within_sos_decl1 == 2)
  {
    SOSNr++;
    set_sos_weight((double) SOSNr, 2);
  }
}
    break;

  case 80:

    {
  if(!Within_sos_decl1) {
    lp_yyerror("parse error");
    YYABORT;
  }
  if(Within_sos_decl1 == 1) {
    FREE(Last_var0);
    Last_var0 = strdup(Last_var);
  }
  if(Within_sos_decl1 == 2)
  {
    storevarandweight(Last_var);
    SOSNr++;
    set_sos_weight((double) SOSNr, 2);
  }
}
    break;

  case 81:

    {
  if(Within_sos_decl1 == 1)
  {
    char buf[16];

    SOSweight++;
    sprintf(buf, "SOS%d", SOSweight);
    storevarandweight(buf);

    check_int_sec_sos_free_decl(Within_int_decl, Within_sec_decl, 2, Within_free_decl);
    Within_sos_decl1 = 2;
    SOSNr = 0;

    storevarandweight(Last_var0);
    SOSNr++;
  }

  set_sos_weight(f, 2);
}
    break;

  case 82:

    { /* SOS name */
  if(Within_sos_decl1 == 1)
  {
    storevarandweight(Last_var0);
    set_sos_type(SOStype);
    check_int_sec_sos_free_decl(Within_int_decl, Within_sec_decl, 2, Within_free_decl);
    Within_sos_decl1 = 2;
    SOSNr = 0;
    SOSweight++;
  }
}
    break;


/* Line 1267 of yacc.c.  */

      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", lp_yyr1[lp_yyn], &lp_yyval, &lp_yyloc);

  YYPOPSTACK (lp_yylen);
  lp_yylen = 0;
  YY_STACK_PRINT (lp_yyss, lp_yyssp);

  *++lp_yyvsp = lp_yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  lp_yyn = lp_yyr1[lp_yyn];

  lp_yystate = lp_yypgoto[lp_yyn - YYNTOKENS] + *lp_yyssp;
  if (0 <= lp_yystate && lp_yystate <= YYLAST && lp_yycheck[lp_yystate] == *lp_yyssp)
    lp_yystate = lp_yytable[lp_yystate];
  else
    lp_yystate = lp_yydefgoto[lp_yyn - YYNTOKENS];

  goto lp_yynewstate;


/*------------------------------------.
| lp_yyerrlab -- here on detecting error |
`------------------------------------*/
lp_yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!lp_yyerrstatus)
    {
      ++lp_yynerrs;
#if ! YYERROR_VERBOSE
      lp_yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T lp_yysize = lp_yysyntax_error (0, lp_yystate, lp_yychar);
	if (lp_yymsg_alloc < lp_yysize && lp_yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T lp_yyalloc = 2 * lp_yysize;
	    if (! (lp_yysize <= lp_yyalloc && lp_yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      lp_yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (lp_yymsg != lp_yymsgbuf)
	      YYSTACK_FREE (lp_yymsg);
	    lp_yymsg = (char *) YYSTACK_ALLOC (lp_yyalloc);
	    if (lp_yymsg)
	      lp_yymsg_alloc = lp_yyalloc;
	    else
	      {
		lp_yymsg = lp_yymsgbuf;
		lp_yymsg_alloc = sizeof lp_yymsgbuf;
	      }
	  }

	if (0 < lp_yysize && lp_yysize <= lp_yymsg_alloc)
	  {
	    (void) lp_yysyntax_error (lp_yymsg, lp_yystate, lp_yychar);
	    lp_yyerror (lp_yymsg);
	  }
	else
	  {
	    lp_yyerror (YY_("syntax error"));
	    if (lp_yysize != 0)
	      goto lp_yyexhaustedlab;
	  }
      }
#endif
    }



  if (lp_yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
	 error, discard it.  */

      if (lp_yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (lp_yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  lp_yydestruct ("Error: discarding",
		      lp_yytoken, &lp_yylval);
	  lp_yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
     token.  */
  goto lp_yyerrlab1;


/*---------------------------------------------------.
| lp_yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
lp_yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label lp_yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto lp_yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (lp_yylen);
  lp_yylen = 0;
  YY_STACK_PRINT (lp_yyss, lp_yyssp);
  lp_yystate = *lp_yyssp;
  goto lp_yyerrlab1;


/*-------------------------------------------------------------.
| lp_yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
lp_yyerrlab1:
  lp_yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      lp_yyn = lp_yypact[lp_yystate];
      if (lp_yyn != YYPACT_NINF)
	{
	  lp_yyn += YYTERROR;
	  if (0 <= lp_yyn && lp_yyn <= YYLAST && lp_yycheck[lp_yyn] == YYTERROR)
	    {
	      lp_yyn = lp_yytable[lp_yyn];
	      if (0 < lp_yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (lp_yyssp == lp_yyss)
	YYABORT;


      lp_yydestruct ("Error: popping",
		  lp_yystos[lp_yystate], lp_yyvsp);
      YYPOPSTACK (1);
      lp_yystate = *lp_yyssp;
      YY_STACK_PRINT (lp_yyss, lp_yyssp);
    }

  if (lp_yyn == YYFINAL)
    YYACCEPT;

  *++lp_yyvsp = lp_yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", lp_yystos[lp_yyn], lp_yyvsp, lp_yylsp);

  lp_yystate = lp_yyn;
  goto lp_yynewstate;


/*-------------------------------------.
| lp_yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
lp_yyacceptlab:
  lp_yyresult = 0;
  goto lp_yyreturn;

/*-----------------------------------.
| lp_yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
lp_yyabortlab:
  lp_yyresult = 1;
  goto lp_yyreturn;

#ifndef lp_yyoverflow
/*-------------------------------------------------.
| lp_yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
lp_yyexhaustedlab:
  lp_yyerror (YY_("memory exhausted"));
  lp_yyresult = 2;
  /* Fall through.  */
#endif

lp_yyreturn:
  if (lp_yychar != YYEOF && lp_yychar != YYEMPTY)
     lp_yydestruct ("Cleanup: discarding lookahead",
		 lp_yytoken, &lp_yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (lp_yylen);
  YY_STACK_PRINT (lp_yyss, lp_yyssp);
  while (lp_yyssp != lp_yyss)
    {
      lp_yydestruct ("Cleanup: popping",
		  lp_yystos[*lp_yyssp], lp_yyvsp);
      YYPOPSTACK (1);
    }
#ifndef lp_yyoverflow
  if (lp_yyss != lp_yyssa)
    YYSTACK_FREE (lp_yyss);
#endif
#if YYERROR_VERBOSE
  if (lp_yymsg != lp_yymsgbuf)
    YYSTACK_FREE (lp_yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (lp_yyresult);
}





static void lp_yy_delete_allocated_memory(void)
{
  /* free memory allocated by flex. Otherwise some memory is not freed.
     This is a bit tricky. There is not much documentation about this, but a lot of
     reports of memory that keeps allocated */

  /* If you get errors on this function call, just comment it. This will only result
     in some memory that is not being freed. */

# if defined YY_CURRENT_BUFFER
    /* flex defines the macro YY_CURRENT_BUFFER, so you should only get here if lp_rlp.h is
       generated by flex */
    /* lex doesn't define this macro and thus should not come here, but lex doesn't has
       this memory leak also ...*/

    lp_yy_delete_buffer(YY_CURRENT_BUFFER); /* comment this line if you have problems with it */
    lp_yy_init = 1; /* make sure that the next time memory is allocated again */
    lp_yy_start = 0;
# endif

  FREE(Last_var);
  FREE(Last_var0);
}

static int parse(void)
{
  return(lp_yyparse());
}

lprec *read_lp1(lprec *lp, void *userhandle, read_modeldata_func read_modeldata, int verbose, char *lp_name)
{
  lp_yyin = (FILE *) userhandle;
  lp_yyout = NULL;
  lp_yylineno = 1;
  lp_input = read_modeldata;
  return(yacc_read(lp, verbose, lp_name, &lp_yylineno, parse, lp_yy_delete_allocated_memory));
}

lprec * __WINAPI read_lp(FILE *filename, int verbose, char *lp_name)
{
  return(read_lp1(NULL, filename, lp_input_lp_yyin, verbose, lp_name));
}

lprec * __WINAPI read_lpex(void *userhandle, read_modeldata_func read_modeldata, int verbose, char *lp_name)
{
  return(read_lp1(NULL, userhandle, read_modeldata, verbose, lp_name));
}

lprec *read_LP1(lprec *lp, char *filename, int verbose, char *lp_name)
{
  FILE *fpin;

  if((fpin = fopen(filename, "r")) != NULL) {
    lp = read_lp1(lp, fpin, lp_input_lp_yyin, verbose, lp_name);
    fclose(fpin);
  }
  else
    lp = NULL;
  return(lp);
}

lprec * __WINAPI read_LP(char *filename, int verbose, char *lp_name)
{
  return(read_LP1(NULL, filename, verbose, lp_name));
}

MYBOOL __WINAPI LP_readhandle(lprec **lp, FILE *filename, int verbose, char *lp_name)
{
  if(lp != NULL)
    *lp = read_lp1(*lp, filename, lp_input_lp_yyin, verbose, lp_name);

  return((lp != NULL) && (*lp != NULL));
}

