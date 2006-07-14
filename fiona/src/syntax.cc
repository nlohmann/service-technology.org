/* A Bison parser, made by GNU Bison 1.875b.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003 Free Software Foundation, Inc.

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
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Written by Richard Stallman by simplifying the original so called
   ``semantic'' parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

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




/* Copy the first part of user declarations.  */
#line 1 "syntax.yy"

/* Prologue: Syntax and usage of the prologue.
 * Bison Declarations: Syntax and usage of the Bison declarations section.
 * Grammar Rules: Syntax and usage of the grammar rules section.
 * Epilogue: Syntax and usage of the epilogue.  */

// options for Bison
#define YYDEBUG 1
#define YYERROR_VERBOSE 0  // for verbose error messages


// to avoid the message "parser stack overflow"
#define YYMAXDEPTH 1000000
#define YYINITDEPTH 10000


// from flex
extern char* yytext;
extern int yylex();


// defined in "debug.h"
extern int yyerror(const char *);

//using namespace std;

#include "dimensions.h"
#include "graph.h"
#include "symboltab.h"
#include "owfnTransition.h"
#include "owfnPlace.h"
#include "owfn.h"
#include "formula.h"

#include "main.h"

#include<stdio.h>
#include<limits.h>


/* list of places and multiplicities to become arcs */
class arc_list {
 public:
	PlSymbol * place;
	unsigned int nu;
    arc_list    * next;
};   


int CurrentCapacity;
owfnPlace *P;
owfnTransition *T;
Symbol * S;
PlSymbol * PS;
TrSymbol * TS;
SymbolTab * GlobalTable;
SymbolTab * LocalTable;
oWFN * PN;					// the petri net

placeType type = INTERNAL;		/* type of place */



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

#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 85 "syntax.yy"
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
/* Line 191 of yacc.c.  */
#line 209 "syntax.cc"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 214 of yacc.c.  */
#line 221 "syntax.cc"

#if ! defined (yyoverflow) || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# if YYSTACK_USE_ALLOCA
#  define YYSTACK_ALLOC alloca
# else
#  ifndef YYSTACK_USE_ALLOCA
#   if defined (alloca) || defined (_ALLOCA_H)
#    define YYSTACK_ALLOC alloca
#   else
#    ifdef __GNUC__
#     define YYSTACK_ALLOC __builtin_alloca
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
# else
#  if defined (__STDC__) || defined (__cplusplus)
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   define YYSIZE_T size_t
#  endif
#  define YYSTACK_ALLOC malloc
#  define YYSTACK_FREE free
# endif
#endif /* ! defined (yyoverflow) || YYERROR_VERBOSE */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE))				\
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  register YYSIZE_T yyi;		\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (0)
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
    while (0)

#endif

#if defined (__STDC__) || defined (__cplusplus)
   typedef signed char yysigned_char;
#else
   typedef short yysigned_char;
#endif

/* YYFINAL -- State number of the termination state. */
#define YYFINAL  9
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   98

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  30
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  30
/* YYNRULES -- Number of rules. */
#define YYNRULES  57
/* YYNRULES -- Number of states. */
#define YYNSTATES  104

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   284

#define YYTRANSLATE(YYX) 						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
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
      25,    26,    27,    28,    29
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned char yyprhs[] =
{
       0,     0,     3,     5,     6,     7,    17,    21,    25,    29,
      31,    32,    37,    38,    39,    44,    45,    46,    51,    52,
      53,    57,    60,    65,    66,    69,    73,    77,    79,    81,
      83,    85,    86,    88,    92,    96,    97,    99,   103,   107,
     109,   112,   121,   123,   125,   126,   128,   132,   136,   140,
     144,   148,   151,   155,   159,   163,   167,   171
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const yysigned_char yyrhs[] =
{
      31,     0,    -1,    32,    -1,    -1,    -1,     4,    36,     8,
      33,    34,    50,    16,    35,    54,    -1,     9,    52,    16,
      -1,    10,    59,    16,    -1,    41,    37,    39,    -1,    43,
      -1,    -1,     6,    38,    45,    16,    -1,    -1,    -1,     7,
      40,    45,    16,    -1,    -1,    -1,     5,    42,    45,    16,
      -1,    -1,    -1,    44,    45,    16,    -1,    46,    47,    -1,
      45,    16,    46,    47,    -1,    -1,     3,    15,    -1,     3,
      18,    15,    -1,    47,    14,    48,    -1,    48,    -1,    49,
      -1,    17,    -1,    18,    -1,    -1,    51,    -1,    50,    14,
      51,    -1,    49,    15,    18,    -1,    -1,    53,    -1,    52,
      14,    53,    -1,    49,    15,    18,    -1,    55,    -1,    54,
      55,    -1,    11,    56,    12,    57,    16,    13,    57,    16,
      -1,    17,    -1,    18,    -1,    -1,    58,    -1,    58,    14,
      57,    -1,    49,    15,    18,    -1,    28,    59,    29,    -1,
      59,    19,    59,    -1,    59,    20,    59,    -1,    21,    59,
      -1,    49,    26,    18,    -1,    49,    27,    18,    -1,    49,
      23,    18,    -1,    49,    22,    18,    -1,    49,    24,    18,
      -1,    49,    25,    18,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short yyrline[] =
{
       0,   112,   112,   116,   117,   116,   207,   208,   218,   219,
     222,   222,   223,   226,   226,   227,   230,   230,   231,   234,
     234,   236,   237,   240,   241,   242,   247,   248,   251,   267,
     268,   271,   272,   273,   276,   287,   288,   289,   292,   304,
     305,   308,   373,   374,   378,   379,   380,   386,   400,   403,
     406,   409,   412,   421,   430,   439,   448,   457
};
#endif

#if YYDEBUG || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "key_safe", "key_place", "key_internal", 
  "key_input", "key_output", "key_marking", "key_finalmarking", 
  "key_finalcondition", "key_transition", "key_consume", "key_produce", 
  "comma", "colon", "semicolon", "ident", "number", "op_and", "op_or", 
  "op_not", "op_gt", "op_lt", "op_ge", "op_le", "op_eq", "op_ne", "lpar", 
  "rpar", "$accept", "input", "net", "@1", "@2", "final", "place_area", 
  "place_area_input", "@3", "place_area_output", "@4", 
  "place_area_internal", "@5", "place_area_lola", "@6", "placelists", 
  "capacity", "placelist", "place", "nodeident", "markinglist", "marking", 
  "finalmarkinglist", "finalmarking", "transitionlist", "transition", 
  "tname", "arclist", "arc", "statepredicate", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    30,    31,    33,    34,    32,    35,    35,    36,    36,
      38,    37,    37,    40,    39,    39,    42,    41,    41,    44,
      43,    45,    45,    46,    46,    46,    47,    47,    48,    49,
      49,    50,    50,    50,    51,    52,    52,    52,    53,    54,
      54,    55,    56,    56,    57,    57,    57,    58,    59,    59,
      59,    59,    59,    59,    59,    59,    59,    59
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     1,     0,     0,     9,     3,     3,     3,     1,
       0,     4,     0,     0,     4,     0,     0,     4,     0,     0,
       3,     2,     4,     0,     2,     3,     3,     1,     1,     1,
       1,     0,     1,     3,     3,     0,     1,     3,     3,     1,
       2,     8,     1,     1,     0,     1,     3,     3,     3,     3,
       3,     2,     3,     3,     3,     3,     3,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       0,    18,     0,     2,    16,     0,    12,     9,    23,     1,
      23,     3,    10,    15,     0,     0,     0,     0,     4,    23,
      13,     8,    24,     0,    23,    29,    30,    21,    27,    28,
      17,    31,     0,    23,    25,     0,     0,     0,     0,    32,
      11,     0,    22,    26,     0,     0,     0,    23,    34,    33,
      35,     0,     0,     0,     0,    36,     0,     0,     0,     0,
       0,     5,    39,     0,     0,     6,    51,     0,     0,     0,
       0,     0,     0,     0,     7,     0,     0,    42,    43,     0,
      40,    38,    37,    48,    55,    54,    56,    57,    52,    53,
      49,    50,    44,     0,     0,    45,     0,     0,    44,    47,
      44,    46,     0,    41
};

/* YYDEFGOTO[NTERM-NUM]. */
static const yysigned_char yydefgoto[] =
{
      -1,     2,     3,    18,    31,    52,     5,    13,    19,    21,
      33,     6,    10,     7,     8,    15,    16,    27,    28,    58,
      38,    39,    54,    55,    61,    62,    79,    94,    95,    59
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -54
static const yysigned_char yypact[] =
{
      17,     9,     6,   -54,   -54,     5,    57,   -54,    25,   -54,
      25,   -54,   -54,    59,    29,    51,    28,    52,   -54,    25,
     -54,   -54,   -54,    54,     2,   -54,   -54,    56,   -54,   -54,
      13,    28,    55,    25,   -54,    28,    28,    58,    23,   -54,
      13,    61,    56,   -54,    60,    28,    50,    32,   -54,   -54,
      28,    15,    63,    64,    41,   -54,    15,    15,    27,    22,
      44,    63,   -54,    62,    28,   -54,    45,   -11,    65,    67,
      68,    69,    70,    71,   -54,    15,    15,   -54,   -54,    78,
     -54,   -54,   -54,   -54,   -54,   -54,   -54,   -54,   -54,   -54,
     -54,   -54,    28,    66,    75,    79,    74,    81,    28,   -54,
      28,   -54,    80,   -54
};

/* YYPGOTO[NTERM-NUM].  */
static const yysigned_char yypgoto[] =
{
     -54,   -54,   -54,   -54,   -54,   -54,   -54,   -54,   -54,   -54,
     -54,   -54,   -54,   -54,   -54,    -8,   -23,    37,    39,   -16,
     -54,    53,   -54,    31,   -54,    36,   -54,   -42,   -54,   -53
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -24
static const yysigned_char yytable[] =
{
      29,    35,    17,    66,    67,    14,     9,    35,    75,    76,
     -20,    32,   -19,    11,     4,    37,    14,    35,    83,    29,
      29,     1,    90,    91,    35,    41,   -19,   -19,    14,    37,
     -23,   -23,    25,    26,    53,    14,    56,    45,    74,    46,
     -14,    75,    76,    57,    22,    25,    26,    23,    53,    68,
      69,    70,    71,    72,    73,    64,   101,    65,   102,    50,
      51,    77,    78,    12,    75,    76,    20,    24,    30,    34,
      36,    40,    42,    44,    60,    43,    93,    47,    48,    63,
      81,    96,    93,    84,    93,    85,    86,    87,    88,    89,
      92,    97,    99,    98,   100,    82,   103,    80,    49
};

static const unsigned char yycheck[] =
{
      16,    24,    10,    56,    57,     3,     0,    30,    19,    20,
       8,    19,     3,     8,     5,    31,     3,    40,    29,    35,
      36,     4,    75,    76,    47,    33,    17,    18,     3,    45,
      17,    18,    17,    18,    50,     3,    21,    14,    16,    16,
       8,    19,    20,    28,    15,    17,    18,    18,    64,    22,
      23,    24,    25,    26,    27,    14,    98,    16,   100,     9,
      10,    17,    18,     6,    19,    20,     7,    16,    16,    15,
      14,    16,    35,    15,    11,    36,    92,    16,    18,    15,
      18,    15,    98,    18,   100,    18,    18,    18,    18,    18,
      12,    16,    18,    14,    13,    64,    16,    61,    45
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,     4,    31,    32,     5,    36,    41,    43,    44,     0,
      42,     8,     6,    37,     3,    45,    46,    45,    33,    38,
       7,    39,    15,    18,    16,    17,    18,    47,    48,    49,
      16,    34,    45,    40,    15,    46,    14,    49,    50,    51,
      16,    45,    47,    48,    15,    14,    16,    16,    18,    51,
       9,    10,    35,    49,    52,    53,    21,    28,    49,    59,
      11,    54,    55,    15,    14,    16,    59,    59,    22,    23,
      24,    25,    26,    27,    16,    19,    20,    17,    18,    56,
      55,    18,    53,    29,    18,    18,    18,    18,    18,    18,
      59,    59,    12,    49,    57,    58,    15,    16,    14,    18,
      13,    57,    57,    16
};

#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# if defined (__STDC__) || defined (__cplusplus)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# endif
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrlab1


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
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");\
      YYERROR;							\
    }								\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

/* YYLLOC_DEFAULT -- Compute the default location (before the actions
   are run).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)         \
  Current.first_line   = Rhs[1].first_line;      \
  Current.first_column = Rhs[1].first_column;    \
  Current.last_line    = Rhs[N].last_line;       \
  Current.last_column  = Rhs[N].last_column;
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
} while (0)

# define YYDSYMPRINT(Args)			\
do {						\
  if (yydebug)					\
    yysymprint Args;				\
} while (0)

# define YYDSYMPRINTF(Title, Token, Value, Location)		\
do {								\
  if (yydebug)							\
    {								\
      YYFPRINTF (stderr, "%s ", Title);				\
      yysymprint (stderr, 					\
                  Token, Value);	\
      YYFPRINTF (stderr, "\n");					\
    }								\
} while (0)

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (cinluded).                                                   |
`------------------------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_stack_print (short *bottom, short *top)
#else
static void
yy_stack_print (bottom, top)
    short *bottom;
    short *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (/* Nothing. */; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_reduce_print (int yyrule)
#else
static void
yy_reduce_print (yyrule)
    int yyrule;
#endif
{
  int yyi;
  unsigned int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %u), ",
             yyrule - 1, yylno);
  /* Print the symbols being reduced, and their result.  */
  for (yyi = yyprhs[yyrule]; 0 <= yyrhs[yyi]; yyi++)
    YYFPRINTF (stderr, "%s ", yytname [yyrhs[yyi]]);
  YYFPRINTF (stderr, "-> %s\n", yytname [yyr1[yyrule]]);
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (Rule);		\
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YYDSYMPRINT(Args)
# define YYDSYMPRINTF(Title, Token, Value, Location)
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
   SIZE_MAX < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#if YYMAXDEPTH == 0
# undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  register const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  register char *yyd = yydest;
  register const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

#endif /* !YYERROR_VERBOSE */



#if YYDEBUG
/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yysymprint (FILE *yyoutput, int yytype, YYSTYPE *yyvaluep)
#else
static void
yysymprint (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  if (yytype < YYNTOKENS)
    {
      YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
# ifdef YYPRINT
      YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
    }
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  switch (yytype)
    {
      default:
        break;
    }
  YYFPRINTF (yyoutput, ")");
}

#endif /* ! YYDEBUG */
/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yydestruct (int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yytype, yyvaluep)
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  switch (yytype)
    {

      default:
        break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM);
# else
int yyparse ();
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
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



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM)
# else
int yyparse (YYPARSE_PARAM)
  void *YYPARSE_PARAM;
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  short	yyssa[YYINITDEPTH];
  short *yyss = yyssa;
  register short *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;



#define YYPOPSTACK   (yyvsp--, yyssp--)

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* When reducing, the number of symbols on the RHS of the reduced
     rule.  */
  int yylen;

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
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyoverflowlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	short *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyoverflowlab;
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

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

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
      YYDSYMPRINTF ("Next token is", yytoken, &yylval, &yylloc);
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

  /* Shift the lookahead token.  */
  YYDPRINTF ((stderr, "Shifting token %s, ", yytname[yytoken]));

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;


  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
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
#line 112 "syntax.yy"
    { 
}
    break;

  case 3:
#line 116 "syntax.yy"
    {LocalTable = (SymbolTab *) 0;}
    break;

  case 4:
#line 117 "syntax.yy"
    {
			unsigned int i,h,j;
			Symbol * ss;
			
			// Create array of places
			PN->Places = new owfnPlace * [PlaceTable->card + 10];
			
			i = 0;
		  
			for(h = 0; h < PlaceTable->size; h++) {
				for(ss = PlaceTable->table[h]; ss; ss = ss->next) {
					PN->addPlace(i++, ((PlSymbol *) ss)->place);
				}
			}
			
			PlaceTable->card = i;
			
			PN->CurrentMarking = new unsigned int [PlaceTable->card + 10];
			PN->FinalMarking = new unsigned int [PlaceTable->card + 10];
	
		  	for (i = 0; i < PlaceTable->card; i++) {
		  		PN->FinalMarking[i] = 0;
		  	}
		  
			
		}
    break;

  case 5:
#line 145 "syntax.yy"
    {
			unsigned int i,h,j;
			Symbol * ss;
			
			// Create array of transitions 
			PN->Transitions = new owfnTransition * [TransitionTable -> card+10];
			i = 0;
			for(h = 0; h < TransitionTable->size; h++) {
				for(ss = TransitionTable -> table[h];ss;ss = ss -> next) {
					PN->addTransition(i++, ((TrSymbol *) ss)->transition);
				}
			}
			TransitionTable->card = i;
			
			// Create arc list of places pass 1 (count nr of arcs)
			for(i = 0; i < TransitionTable->card; i++) {
				for(j = 0; j < PN->Transitions[i]->NrOfArriving; j++) {
					PN->Transitions[i]->ArrivingArcs[j]->pl->NrOfLeaving++;
				}
				for(j=0;j < PN->Transitions[i]->NrOfLeaving;j++) {
					PN->Transitions[i]->LeavingArcs[j]->pl->NrOfArriving++;
				}
			}
		
			// pass 2 (allocate arc arrays)
			for(i = 0; i < PlaceTable->card; i++) {
				PN->Places[i]->ArrivingArcs = new Arc * [PN->Places[i]->NrOfArriving+10];
				PN->Places[i]->NrOfArriving = 0;
				PN->Places[i]->LeavingArcs = new Arc * [PN->Places[i]->NrOfLeaving+10];
				PN->Places[i]->NrOfLeaving = 0;
			}
		
			// pass 3 (fill in arcs)
			for(i = 0; i < TransitionTable->card; i++) {
				for(j=0;j < PN->Transitions[i]->NrOfLeaving;j++) {
					owfnPlace * pl;
					pl = PN->Transitions[i]->LeavingArcs[j]->pl;
					pl->ArrivingArcs[pl->NrOfArriving] = PN->Transitions[i]->LeavingArcs[j];
					pl->NrOfArriving++;
					
					if (pl->type == OUTPUT) {
						PN->commDepth += PN->Transitions[i]->LeavingArcs[j]->Multiplicity;
					}
				}
				for(j = 0;j < PN->Transitions[i]->NrOfArriving; j++) {
					owfnPlace * pl;
					pl = PN->Transitions[i]->ArrivingArcs[j]->pl;
					pl->LeavingArcs[pl->NrOfLeaving] = PN->Transitions[i]->ArrivingArcs[j];
					pl->NrOfLeaving ++;
					
					if (pl->type == INPUT) {
						PN->commDepth += PN->Transitions[i]->ArrivingArcs[j]->Multiplicity;
					}
				}
			}
			for(i = 0; i < TransitionTable->card; i++)
			{
			}
			PN->startOfEnabledList = PN->Transitions[0];
		}
    break;

  case 7:
#line 208 "syntax.yy"
    {
	formula * F;
	F = yyvsp[-1].form;
	F = F -> posate();
	F = F -> merge();
	F -> setstatic();
	PN -> FinalCondition = F;
}
    break;

  case 10:
#line 222 "syntax.yy"
    {type = INPUT; }
    break;

  case 13:
#line 226 "syntax.yy"
    {type = OUTPUT; }
    break;

  case 16:
#line 230 "syntax.yy"
    {type = INTERNAL; }
    break;

  case 19:
#line 234 "syntax.yy"
    {type = INTERNAL; }
    break;

  case 23:
#line 240 "syntax.yy"
    { CurrentCapacity = CAPACITY;}
    break;

  case 24:
#line 241 "syntax.yy"
    {CurrentCapacity = 1;}
    break;

  case 25:
#line 242 "syntax.yy"
    { 
				sscanf(yyvsp[-1].str, "%u", &CurrentCapacity);
			}
    break;

  case 28:
#line 251 "syntax.yy"
    {
	if(PlaceTable->lookup(yyvsp[0].str)) {
		yyerror("Place name used twice");
	}
	P = new owfnPlace(yyvsp[0].str, type, PN);
	if (type == INPUT) {
		PN->placeInputCnt++;
	} else if (type == OUTPUT) {
		PN->placeOutputCnt++;
	} 
	PS = new PlSymbol(P);
	P->capacity = CurrentCapacity;
	P->nrbits = CurrentCapacity > 0 ? logzwo(CurrentCapacity) : 32;
}
    break;

  case 29:
#line 267 "syntax.yy"
    { yyval.str = yyvsp[0].str;}
    break;

  case 30:
#line 268 "syntax.yy"
    {yyval.str = yyvsp[0].str; }
    break;

  case 34:
#line 276 "syntax.yy"
    {
	unsigned int i;
	PS = (PlSymbol *) PlaceTable->lookup(yyvsp[-2].str);
	if(!PS) {
		yyerror("place does not exist");
	}
	sscanf(yyvsp[0].str,"%u",&i);
	*(PS->place) += i;
}
    break;

  case 38:
#line 292 "syntax.yy"
    {
	unsigned int i;
	PS = (PlSymbol *) PlaceTable->lookup(yyvsp[-2].str);
	if(!PS) {
		yyerror("place does not exist");
	}
	sscanf(yyvsp[0].str,"%u",&i);
	PN->FinalMarking[PS->place->index] = i;
}
    break;

  case 41:
#line 308 "syntax.yy"
    {
	unsigned int card;
	unsigned int i;
	arc_list * current;
	/* 1. Transition anlegen */
	if(TransitionTable -> lookup(yyvsp[-6].str)) {
		yyerror("transition name used twice");
	}
	TS = new TrSymbol(yyvsp[-6].str);
	T = TS->transition = new owfnTransition(TS->name);	// counter++ in PN
								// KS: macht Konstruktor von owfnTransition
	/* 2. Inliste eintragen */

	/* Anzahl der Boegen */
	for(card = 0, current = yyvsp[-4].al; current; card++, current = current->next);
		
	T->ArrivingArcs = new  Arc * [card+10];
	/* Schleife ueber alle Boegen */
	for(current = yyvsp[-4].al;current;current = current -> next) {
	/* gibt es Bogen schon? */

		for(i = 0; i < T->NrOfArriving;i++) {
			if(current->place -> place == T->ArrivingArcs[i]->pl) {
				/* Bogen existiert, nur Vielfachheit addieren */
				*(T->ArrivingArcs[i]) += current->nu;
				break;
			}
		}

		if(i >= T->NrOfArriving) {
			T->ArrivingArcs[T->NrOfArriving] = new Arc(T,current->place->place,true,current->nu);
			T->NrOfArriving++;
			current -> place -> place -> references ++;
		}
	}

	/* 2. Outliste eintragen */

	/* Anzahl der Boegen */
	for(card = 0, current = yyvsp[-1].al; current; card++, current = current->next);

	T->LeavingArcs = new  Arc * [card+10];
	/* Schleife ueber alle Boegen */
	for(current = yyvsp[-1].al; current; current = current->next) {
		/* gibt es Bogen schon? */

		for(i = 0; i < T->NrOfLeaving; i++) {
			if(current->place->place == T->LeavingArcs[i]->pl) {
				/* Bogen existiert, nur Vielfachheit addieren */
				*(T->LeavingArcs[i]) += current->nu;
				break;
			}
		}

		if(i >= T->NrOfLeaving) {
			T->LeavingArcs[T->NrOfLeaving] = new Arc(T,current->place->place, false, current->nu);
			T->NrOfLeaving++;
			current->place->place->references++;
		}
	}

}
    break;

  case 44:
#line 378 "syntax.yy"
    { yyval.al = (arc_list *) 0;}
    break;

  case 45:
#line 379 "syntax.yy"
    {yyval.al = yyvsp[0].al;}
    break;

  case 46:
#line 380 "syntax.yy"
    {
			yyvsp[-2].al-> next = yyvsp[0].al;
			yyval.al = yyvsp[-2].al;
		}
    break;

  case 47:
#line 386 "syntax.yy"
    {
	unsigned int i;
	PS = (PlSymbol *) PlaceTable -> lookup(yyvsp[-2].str);
	if(!PS) {
		yyerror("place does not exist");
	}
	yyval.al = new arc_list;
	yyval.al->place = PS;
	yyval.al->next = (arc_list *)  0;
	sscanf(yyvsp[0].str,"%u",&i);
	yyval.al->nu = i;
    }
    break;

  case 48:
#line 400 "syntax.yy"
    {
	yyval.form = yyvsp[-1].form;
}
    break;

  case 49:
#line 403 "syntax.yy"
    {
	yyval.form = new binarybooleanformula(conj,yyvsp[-2].form,yyvsp[0].form);
}
    break;

  case 50:
#line 406 "syntax.yy"
    {
	yyval.form = new binarybooleanformula(disj,yyvsp[-2].form,yyvsp[0].form);
}
    break;

  case 51:
#line 409 "syntax.yy"
    {
	yyval.form = new unarybooleanformula(neg,yyvsp[0].form);
}
    break;

  case 52:
#line 412 "syntax.yy"
    {
	unsigned int i;
	PS = (PlSymbol *) PlaceTable->lookup(yyvsp[-2].str);
	if(!PS) {
		yyerror("place does not exist");
	}
	sscanf(yyvsp[0].str,"%u",&i);
	yyval.form = new atomicformula(eq,PS->place,i);
}
    break;

  case 53:
#line 421 "syntax.yy"
    {
	unsigned int i;
	PS = (PlSymbol *) PlaceTable->lookup(yyvsp[-2].str);
	if(!PS) {
		yyerror("place does not exist");
	}
	sscanf(yyvsp[0].str,"%u",&i);
	yyval.form = new atomicformula(neq,PS->place,i);
}
    break;

  case 54:
#line 430 "syntax.yy"
    {
	unsigned int i;
	PS = (PlSymbol *) PlaceTable->lookup(yyvsp[-2].str);
	if(!PS) {
		yyerror("place does not exist");
	}
	sscanf(yyvsp[0].str,"%u",&i);
	yyval.form = new atomicformula(lt,PS->place,i);
}
    break;

  case 55:
#line 439 "syntax.yy"
    {
	unsigned int i;
	PS = (PlSymbol *) PlaceTable->lookup(yyvsp[-2].str);
	if(!PS) {
		yyerror("place does not exist");
	}
	sscanf(yyvsp[0].str,"%u",&i);
	yyval.form = new atomicformula(gt,PS->place,i);
}
    break;

  case 56:
#line 448 "syntax.yy"
    {
	unsigned int i;
	PS = (PlSymbol *) PlaceTable->lookup(yyvsp[-2].str);
	if(!PS) {
		yyerror("place does not exist");
	}
	sscanf(yyvsp[0].str,"%u",&i);
	yyval.form = new atomicformula(geq,PS->place,i);
}
    break;

  case 57:
#line 457 "syntax.yy"
    {
	unsigned int i;
	PS = (PlSymbol *) PlaceTable->lookup(yyvsp[-2].str);
	if(!PS) {
		yyerror("place does not exist");
	}
	sscanf(yyvsp[0].str,"%u",&i);
	yyval.form = new atomicformula(leq,PS->place,i);
}
    break;


    }

/* Line 999 of yacc.c.  */
#line 1599 "syntax.cc"

  yyvsp -= yylen;
  yyssp -= yylen;


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
#if YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (YYPACT_NINF < yyn && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  int yytype = YYTRANSLATE (yychar);
	  const char* yyprefix;
	  char *yymsg;
	  int yyx;

	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  int yyxbegin = yyn < 0 ? -yyn : 0;

	  /* Stay within bounds of both yycheck and yytname.  */
	  int yychecklim = YYLAST - yyn;
	  int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
	  int yycount = 0;

	  yyprefix = ", expecting ";
	  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	      {
		yysize += yystrlen (yyprefix) + yystrlen (yytname [yyx]);
		yycount += 1;
		if (yycount == 5)
		  {
		    yysize = 0;
		    break;
		  }
	      }
	  yysize += (sizeof ("syntax error, unexpected ")
		     + yystrlen (yytname[yytype]));
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "syntax error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[yytype]);

	      if (yycount < 5)
		{
		  yyprefix = ", expecting ";
		  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
		    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
		      {
			yyp = yystpcpy (yyp, yyprefix);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yyprefix = " or ";
		      }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    yyerror ("syntax error; also virtual memory exhausted");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror ("syntax error");
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      /* Return failure if at end of input.  */
      if (yychar == YYEOF)
        {
	  /* Pop the error token.  */
          YYPOPSTACK;
	  /* Pop the rest of the stack.  */
	  while (yyss < yyssp)
	    {
	      YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
	      yydestruct (yystos[*yyssp], yyvsp);
	      YYPOPSTACK;
	    }
	  YYABORT;
        }

      YYDSYMPRINTF ("Error: discarding", yytoken, &yylval, &yylloc);
      yydestruct (yytoken, &yylval);
      yychar = YYEMPTY;

    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*----------------------------------------------------.
| yyerrlab1 -- error raised explicitly by an action.  |
`----------------------------------------------------*/
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

      YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
      yydestruct (yystos[yystate], yyvsp);
      yyvsp--;
      yystate = *--yyssp;

      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;


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
/*----------------------------------------------.
| yyoverflowlab -- parser overflow comes here.  |
`----------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}



