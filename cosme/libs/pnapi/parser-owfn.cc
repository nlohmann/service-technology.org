/* A Bison parser, made by GNU Bison 2.4.2.  */

/* Skeleton implementation for Bison LALR(1) parsers in C++
   
      Copyright (C) 2002-2010 Free Software Foundation, Inc.
   
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


/* First part of user declarations.  */

/* Line 310 of lalr1.cc  */
#line 41 "parser-owfn.yy"


#include "formula.h"
#include "marking.h"
#include "parser-owfn-wrapper.h"
#include "petrinet.h"

#include <sstream>



/* Line 310 of lalr1.cc  */
#line 51 "parser-owfn.cc"


#include "parser-owfn.h"

/* User implementation prologue.  */


/* Line 316 of lalr1.cc  */
#line 60 "parser-owfn.cc"

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* FIXME: INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#define YYUSE(e) ((void) (e))

/* Enable debugging if requested.  */
#if YYDEBUG

/* A pseudo ostream that takes yydebug_ into account.  */
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)	\
do {							\
  if (yydebug_)						\
    {							\
      *yycdebug_ << Title << ' ';			\
      yy_symbol_print_ ((Type), (Value), (Location));	\
      *yycdebug_ << std::endl;				\
    }							\
} while (false)

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug_)				\
    yy_reduce_print_ (Rule);		\
} while (false)

# define YY_STACK_PRINT()		\
do {					\
  if (yydebug_)				\
    yystack_print_ ();			\
} while (false)

#else /* !YYDEBUG */

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_REDUCE_PRINT(Rule)
# define YY_STACK_PRINT()

#endif /* !YYDEBUG */

#define yyerrok		(yyerrstatus_ = 0)
#define yyclearin	(yychar = yyempty_)

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)


/* Line 379 of lalr1.cc  */
#line 11 "parser-owfn.yy"
namespace pnapi { namespace parser { namespace owfn { namespace yy {

/* Line 379 of lalr1.cc  */
#line 128 "parser-owfn.cc"
#if YYERROR_VERBOSE

  /* Return YYSTR after stripping away unnecessary quotes and
     backslashes, so that it's suitable for yyerror.  The heuristic is
     that double-quoting is unnecessary unless the string contains an
     apostrophe, a comma, or backslash (other than backslash-backslash).
     YYSTR is taken from yytname.  */
  std::string
  BisonParser::yytnamerr_ (const char *yystr)
  {
    if (*yystr == '"')
      {
        std::string yyr = "";
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
              yyr += *yyp;
              break;

            case '"':
              return yyr;
            }
      do_not_strip_quotes: ;
      }

    return yystr;
  }

#endif

  /// Build a parser object.
  BisonParser::BisonParser (Parser& parser__yyarg)
    :
#if YYDEBUG
      yydebug_ (false),
      yycdebug_ (&std::cerr),
#endif
      parser_ (parser__yyarg)
  {
  }

  BisonParser::~BisonParser ()
  {
  }

#if YYDEBUG
  /*--------------------------------.
  | Print this symbol on YYOUTPUT.  |
  `--------------------------------*/

  inline void
  BisonParser::yy_symbol_value_print_ (int yytype,
			   const semantic_type* yyvaluep, const location_type* yylocationp)
  {
    YYUSE (yylocationp);
    YYUSE (yyvaluep);
    switch (yytype)
      {
         default:
	  break;
      }
  }


  void
  BisonParser::yy_symbol_print_ (int yytype,
			   const semantic_type* yyvaluep, const location_type* yylocationp)
  {
    *yycdebug_ << (yytype < yyntokens_ ? "token" : "nterm")
	       << ' ' << yytname_[yytype] << " ("
	       << *yylocationp << ": ";
    yy_symbol_value_print_ (yytype, yyvaluep, yylocationp);
    *yycdebug_ << ')';
  }
#endif

  void
  BisonParser::yydestruct_ (const char* yymsg,
			   int yytype, semantic_type* yyvaluep, location_type* yylocationp)
  {
    YYUSE (yylocationp);
    YYUSE (yymsg);
    YYUSE (yyvaluep);

    YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

    switch (yytype)
      {
  
	default:
	  break;
      }
  }

  void
  BisonParser::yypop_ (unsigned int n)
  {
    yystate_stack_.pop (n);
    yysemantic_stack_.pop (n);
    yylocation_stack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  BisonParser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  BisonParser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  BisonParser::debug_level_type
  BisonParser::debug_level () const
  {
    return yydebug_;
  }

  void
  BisonParser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif

  int
  BisonParser::parse ()
  {
    /// Lookahead and lookahead in internal form.
    int yychar = yyempty_;
    int yytoken = 0;

    /* State.  */
    int yyn;
    int yylen = 0;
    int yystate = 0;

    /* Error handling.  */
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// Semantic value of the lookahead.
    semantic_type yylval;
    /// Location of the lookahead.
    location_type yylloc;
    /// The locations where the error started and ended.
    location_type yyerror_range[2];

    /// $$.
    semantic_type yyval;
    /// @$.
    location_type yyloc;

    int yyresult;

    YYCDEBUG << "Starting parse" << std::endl;


    /* Initialize the stacks.  The initial state will be pushed in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystate_stack_ = state_stack_type (0);
    yysemantic_stack_ = semantic_stack_type (0);
    yylocation_stack_ = location_stack_type (0);
    yysemantic_stack_.push (yylval);
    yylocation_stack_.push (yylloc);

    /* New state.  */
  yynewstate:
    yystate_stack_.push (yystate);
    YYCDEBUG << "Entering state " << yystate << std::endl;

    /* Accept?  */
    if (yystate == yyfinal_)
      goto yyacceptlab;

    goto yybackup;

    /* Backup.  */
  yybackup:

    /* Try to take a decision without lookahead.  */
    yyn = yypact_[yystate];
    if (yyn == yypact_ninf_)
      goto yydefault;

    /* Read a lookahead token.  */
    if (yychar == yyempty_)
      {
	YYCDEBUG << "Reading a token: ";
	yychar = yylex (&yylval, &yylloc, parser_);
      }


    /* Convert token to internal form.  */
    if (yychar <= yyeof_)
      {
	yychar = yytoken = yyeof_;
	YYCDEBUG << "Now at end of input." << std::endl;
      }
    else
      {
	yytoken = yytranslate_ (yychar);
	YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
      }

    /* If the proper action on seeing token YYTOKEN is to reduce or to
       detect an error, take that action.  */
    yyn += yytoken;
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yytoken)
      goto yydefault;

    /* Reduce or error.  */
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
	if (yyn == 0 || yyn == yytable_ninf_)
	goto yyerrlab;
	yyn = -yyn;
	goto yyreduce;
      }

    /* Shift the lookahead token.  */
    YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

    /* Discard the token being shifted.  */
    yychar = yyempty_;

    yysemantic_stack_.push (yylval);
    yylocation_stack_.push (yylloc);

    /* Count tokens shifted since error; after three, turn off error
       status.  */
    if (yyerrstatus_)
      --yyerrstatus_;

    yystate = yyn;
    goto yynewstate;

  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[yystate];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;

  /*-----------------------------.
  | yyreduce -- Do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    /* If YYLEN is nonzero, implement the default value of the action:
       `$$ = $1'.  Otherwise, use the top of the stack.

       Otherwise, the following line sets YYVAL to garbage.
       This behavior is undocumented and Bison
       users should not rely upon it.  */
    if (yylen)
      yyval = yysemantic_stack_[yylen - 1];
    else
      yyval = yysemantic_stack_[0];

    {
      slice<location_type, location_stack_type> slice (yylocation_stack_, yylen);
      YYLLOC_DEFAULT (yyloc, slice, yylen);
    }
    YY_REDUCE_PRINT (yyn);
    switch (yyn)
      {
	  case 2:

/* Line 677 of lalr1.cc  */
#line 96 "parser-owfn.yy"
    { 
    parser_.net_.setConstraintLabels(parser_.constrains_); 
  }
    break;

  case 4:

/* Line 677 of lalr1.cc  */
#line 104 "parser-owfn.yy"
    { 
    // clear stringstream
    parser_.nodeName_.str("");
    parser_.nodeName_.clear();

    parser_.nodeName_ << (yysemantic_stack_[(1) - (1)].yt_str);
    free((yysemantic_stack_[(1) - (1)].yt_str)); 
  }
    break;

  case 5:

/* Line 677 of lalr1.cc  */
#line 113 "parser-owfn.yy"
    { 
    // clear stringstream
    parser_.nodeName_.str("");
    parser_.nodeName_.clear();

    parser_.nodeName_ << (yysemantic_stack_[(1) - (1)].yt_int); 
  }
    break;

  case 21:

/* Line 677 of lalr1.cc  */
#line 162 "parser-owfn.yy"
    { parser_.labelType_ = Label::INPUT; }
    break;

  case 23:

/* Line 677 of lalr1.cc  */
#line 166 "parser-owfn.yy"
    { parser_.labelType_ = Label::OUTPUT; }
    break;

  case 25:

/* Line 677 of lalr1.cc  */
#line 170 "parser-owfn.yy"
    { parser_.labelType_ = Label::SYNCHRONOUS; }
    break;

  case 27:

/* Line 677 of lalr1.cc  */
#line 175 "parser-owfn.yy"
    { parser_.capacity_ = 0; }
    break;

  case 28:

/* Line 677 of lalr1.cc  */
#line 176 "parser-owfn.yy"
    { parser_.capacity_ = 0; }
    break;

  case 30:

/* Line 677 of lalr1.cc  */
#line 181 "parser-owfn.yy"
    { parser_.capacity_ = 1; }
    break;

  case 31:

/* Line 677 of lalr1.cc  */
#line 182 "parser-owfn.yy"
    { parser_.capacity_ = (yysemantic_stack_[(3) - (2)].yt_int); }
    break;

  case 33:

/* Line 677 of lalr1.cc  */
#line 188 "parser-owfn.yy"
    {
    // parser_.check(parser_.labels_[parser_.nodeName_.str()] == NULL, "node name already used");
    // parser_.check(parser_.places_[parser_.nodeName_.str()] == NULL, "node name already used");
    try
    {
      parser_.label_ = &(parser_.port_->addLabel(parser_.nodeName_.str(), parser_.labelType_));
    }
    catch(exception::Error e)
    {
      parser_.rethrow(e);
    }

    parser_.labels_[parser_.nodeName_.str()] = parser_.label_;
  }
    break;

  case 35:

/* Line 677 of lalr1.cc  */
#line 204 "parser-owfn.yy"
    {
    // parser_.check(parser_.labels_[parser_.nodeName_.str()] == NULL, "node name already used");
    // parser_.check(parser_.places_[parser_.nodeName_.str()] == NULL, "node name already used");
    try
    {
      parser_.label_ = &(parser_.port_->addLabel(parser_.nodeName_.str(), parser_.labelType_));
    }
    catch(exception::Error e)
    {
      parser_.rethrow(e);
    }

    parser_.labels_[parser_.nodeName_.str()] = parser_.label_;
  }
    break;

  case 43:

/* Line 677 of lalr1.cc  */
#line 232 "parser-owfn.yy"
    {
    if(parser_.place_ != NULL) // is only NULL when parsing interface
    {
      parser_.place_->setMaxOccurrence((yysemantic_stack_[(3) - (3)].yt_int));
    }
  }
    break;

  case 45:

/* Line 677 of lalr1.cc  */
#line 240 "parser-owfn.yy"
    {
    if(parser_.place_ != NULL) // is only NULL when parsing interface
    {
      parser_.place_->setMaxOccurrence((yysemantic_stack_[(3) - (3)].yt_int));
    }
  }
    break;

  case 47:

/* Line 677 of lalr1.cc  */
#line 251 "parser-owfn.yy"
    { parser_.port_ = &(parser_.net_.getInterface().addPort(parser_.nodeName_.str())); }
    break;

  case 49:

/* Line 677 of lalr1.cc  */
#line 254 "parser-owfn.yy"
    { parser_.port_ = &(parser_.net_.getInterface().addPort(parser_.nodeName_.str())); }
    break;

  case 57:

/* Line 677 of lalr1.cc  */
#line 274 "parser-owfn.yy"
    { parser_.capacity_ = 0; }
    break;

  case 58:

/* Line 677 of lalr1.cc  */
#line 275 "parser-owfn.yy"
    { parser_.capacity_ = 0; }
    break;

  case 60:

/* Line 677 of lalr1.cc  */
#line 281 "parser-owfn.yy"
    {
    // parser_.check(parser_.labels_[parser_.nodeName_.str()] == NULL, "node name already used");
    // parser_.check(parser_.places_[parser_.nodeName_.str()] == NULL, "node name already used");
    try
    {
      parser_.place_ = &(parser_.net_.createPlace(parser_.nodeName_.str(), 0, parser_.capacity_));
    }
    catch(exception::Error e)
    {
      parser_.rethrow(e);
    }

    parser_.places_[parser_.nodeName_.str()] = parser_.place_;
  }
    break;

  case 62:

/* Line 677 of lalr1.cc  */
#line 297 "parser-owfn.yy"
    {
    // parser_.check(parser_.labels_[parser_.nodeName_.str()] == NULL, "node name already used");
    // parser_.check(parser_.places_[parser_.nodeName_.str()] == NULL, "node name already used");
    try
    {
      parser_.place_ = &(parser_.net_.createPlace(parser_.nodeName_.str(), 0, parser_.capacity_));
    }
    catch(exception::Error e)
    {
      parser_.rethrow(e);
    }

    parser_.places_[parser_.nodeName_.str()] = parser_.place_;
  }
    break;

  case 65:

/* Line 677 of lalr1.cc  */
#line 321 "parser-owfn.yy"
    { parser_.net_.addRole(parser_.nodeName_.str()); }
    break;

  case 66:

/* Line 677 of lalr1.cc  */
#line 323 "parser-owfn.yy"
    { parser_.net_.addRole(parser_.nodeName_.str()); }
    break;

  case 79:

/* Line 677 of lalr1.cc  */
#line 357 "parser-owfn.yy"
    { parser_.port_ = &(parser_.net_.getInterface().addPort(parser_.nodeName_.str())); }
    break;

  case 81:

/* Line 677 of lalr1.cc  */
#line 360 "parser-owfn.yy"
    { parser_.port_ = &(parser_.net_.getInterface().addPort(parser_.nodeName_.str())); }
    break;

  case 83:

/* Line 677 of lalr1.cc  */
#line 366 "parser-owfn.yy"
    {
    Label * l = parser_.labels_[parser_.nodeName_.str()];
    parser_.check(l != NULL, "unknown interface label");
    parser_.check((l->getPort().getName() == ""), "interface label already assigned to port '" + l->getPort().getName() +"'");
     
    Label::Type t = l->getType();
    parser_.net_.getInterface().getPort()->removeLabel(parser_.nodeName_.str());
    parser_.labels_[parser_.nodeName_.str()] = &(parser_.port_->addLabel(parser_.nodeName_.str(), t));
  }
    break;

  case 84:

/* Line 677 of lalr1.cc  */
#line 376 "parser-owfn.yy"
    {
    Label * l = parser_.labels_[parser_.nodeName_.str()];
    parser_.check(l != NULL, "unknown interface label");
    parser_.check((l->getPort().getName() == ""), "interface label already assigned to port '" + l->getPort().getName() +"'");
     
    Label::Type t = l->getType();
    parser_.net_.getInterface().getPort()->removeLabel(parser_.nodeName_.str());
    parser_.labels_[parser_.nodeName_.str()] = &(parser_.port_->addLabel(parser_.nodeName_.str(), t));
  }
    break;

  case 85:

/* Line 677 of lalr1.cc  */
#line 394 "parser-owfn.yy"
    { parser_.markInitial_ = true; }
    break;

  case 86:

/* Line 677 of lalr1.cc  */
#line 395 "parser-owfn.yy"
    { parser_.markInitial_ = false; }
    break;

  case 91:

/* Line 677 of lalr1.cc  */
#line 406 "parser-owfn.yy"
    {
    parser_.place_ = parser_.places_[parser_.nodeName_.str()];
    parser_.check(parser_.place_ != NULL, "unknown place");
  }
    break;

  case 93:

/* Line 677 of lalr1.cc  */
#line 415 "parser-owfn.yy"
    {      
    if(parser_.markInitial_)
    {  
      parser_.place_->setTokenCount((yysemantic_stack_[(2) - (2)].yt_int));
    }
    else
    {
      (*(parser_.finalMarking_))[*parser_.place_] = (yysemantic_stack_[(2) - (2)].yt_int);
    }
  }
    break;

  case 94:

/* Line 677 of lalr1.cc  */
#line 426 "parser-owfn.yy"
    {
    if(parser_.markInitial_)
    {  
      parser_.place_->setTokenCount(1);
    }
    else
    {
      (*(parser_.finalMarking_))[*parser_.place_] = 1;
    }
  }
    break;

  case 95:

/* Line 677 of lalr1.cc  */
#line 440 "parser-owfn.yy"
    { parser_.finalMarking_ = new Marking(parser_.net_, true); }
    break;

  case 96:

/* Line 677 of lalr1.cc  */
#line 442 "parser-owfn.yy"
    {
    delete parser_.finalMarking_;
    parser_.finalMarking_ = NULL;
  }
    break;

  case 97:

/* Line 677 of lalr1.cc  */
#line 447 "parser-owfn.yy"
    {
    parser_.net_.getFinalCondition() = (*(yysemantic_stack_[(1) - (1)].yt_formula));
    if(parser_.wildcardGiven_)
    {
      parser_.wildcardGiven_ = false;
      parser_.net_.getFinalCondition().allOtherPlacesEmpty(parser_.net_);
    }
    delete (yysemantic_stack_[(1) - (1)].yt_formula); 
  }
    break;

  case 98:

/* Line 677 of lalr1.cc  */
#line 460 "parser-owfn.yy"
    {
    parser_.net_.getFinalCondition().addMarking(*(parser_.finalMarking_));
    delete parser_.finalMarking_;
    parser_.finalMarking_ = new Marking(parser_.net_, true);
  }
    break;

  case 99:

/* Line 677 of lalr1.cc  */
#line 466 "parser-owfn.yy"
    {
    parser_.net_.getFinalCondition().addMarking(*(parser_.finalMarking_));
    delete parser_.finalMarking_;
    parser_.finalMarking_ = new Marking(parser_.net_, true);
  }
    break;

  case 100:

/* Line 677 of lalr1.cc  */
#line 474 "parser-owfn.yy"
    { (yyval.yt_formula) = new formula::FormulaTrue(); }
    break;

  case 101:

/* Line 677 of lalr1.cc  */
#line 475 "parser-owfn.yy"
    { (yyval.yt_formula) = new formula::FormulaFalse(); }
    break;

  case 102:

/* Line 677 of lalr1.cc  */
#line 476 "parser-owfn.yy"
    { (yyval.yt_formula) = (yysemantic_stack_[(3) - (2)].yt_formula); }
    break;

  case 104:

/* Line 677 of lalr1.cc  */
#line 482 "parser-owfn.yy"
    {
    parser_.net_.getFinalCondition() = (*(yysemantic_stack_[(3) - (2)].yt_formula));
    delete (yysemantic_stack_[(3) - (2)].yt_formula);
  }
    break;

  case 105:

/* Line 677 of lalr1.cc  */
#line 489 "parser-owfn.yy"
    { (yyval.yt_formula) = (yysemantic_stack_[(3) - (2)].yt_formula); }
    break;

  case 106:

/* Line 677 of lalr1.cc  */
#line 490 "parser-owfn.yy"
    { (yyval.yt_formula) = new formula::FormulaTrue(); }
    break;

  case 107:

/* Line 677 of lalr1.cc  */
#line 491 "parser-owfn.yy"
    { (yyval.yt_formula) = new formula::FormulaFalse(); }
    break;

  case 108:

/* Line 677 of lalr1.cc  */
#line 493 "parser-owfn.yy"
    { 
    parser_.wildcardGiven_ = true;
    (yyval.yt_formula) = new formula::FormulaTrue();
  }
    break;

  case 109:

/* Line 677 of lalr1.cc  */
#line 498 "parser-owfn.yy"
    { 
    (yyval.yt_formula) = new formula::Negation(*(yysemantic_stack_[(2) - (2)].yt_formula));
    delete (yysemantic_stack_[(2) - (2)].yt_formula);
  }
    break;

  case 110:

/* Line 677 of lalr1.cc  */
#line 503 "parser-owfn.yy"
    {
    (yyval.yt_formula) = new formula::Disjunction(*(yysemantic_stack_[(3) - (1)].yt_formula), *(yysemantic_stack_[(3) - (3)].yt_formula));
    delete (yysemantic_stack_[(3) - (1)].yt_formula);
    delete (yysemantic_stack_[(3) - (3)].yt_formula);
  }
    break;

  case 111:

/* Line 677 of lalr1.cc  */
#line 509 "parser-owfn.yy"
    {
    (yyval.yt_formula) = new formula::Conjunction(*(yysemantic_stack_[(3) - (1)].yt_formula), *(yysemantic_stack_[(3) - (3)].yt_formula));
    delete (yysemantic_stack_[(3) - (1)].yt_formula);
    delete (yysemantic_stack_[(3) - (3)].yt_formula);
  }
    break;

  case 112:

/* Line 677 of lalr1.cc  */
#line 515 "parser-owfn.yy"
    {
    parser_.wildcardGiven_ = true;
    (yyval.yt_formula) = (yysemantic_stack_[(3) - (1)].yt_formula);
  }
    break;

  case 113:

/* Line 677 of lalr1.cc  */
#line 520 "parser-owfn.yy"
    {
    (yyval.yt_formula) = (yysemantic_stack_[(3) - (1)].yt_formula); // obsolete; kept due to compatibility
  }
    break;

  case 114:

/* Line 677 of lalr1.cc  */
#line 524 "parser-owfn.yy"
    {
    (yyval.yt_formula) = (yysemantic_stack_[(3) - (1)].yt_formula); // obsolete; kept due to compatibility
  }
    break;

  case 115:

/* Line 677 of lalr1.cc  */
#line 528 "parser-owfn.yy"
    {
    parser_.place_ = parser_.places_[parser_.nodeName_.str()];
    parser_.check(parser_.place_ != NULL, "unknown place");
  }
    break;

  case 116:

/* Line 677 of lalr1.cc  */
#line 533 "parser-owfn.yy"
    {
    (yyval.yt_formula) = (yysemantic_stack_[(3) - (3)].yt_formula);
  }
    break;

  case 117:

/* Line 677 of lalr1.cc  */
#line 540 "parser-owfn.yy"
    { 
    (yyval.yt_formula) = new formula::FormulaEqual(*parser_.place_, (yysemantic_stack_[(2) - (2)].yt_int));
  }
    break;

  case 118:

/* Line 677 of lalr1.cc  */
#line 544 "parser-owfn.yy"
    {
    (yyval.yt_formula) = new formula::Negation(formula::FormulaEqual(*parser_.place_, (yysemantic_stack_[(2) - (2)].yt_int)));
  }
    break;

  case 119:

/* Line 677 of lalr1.cc  */
#line 548 "parser-owfn.yy"
    {
    (yyval.yt_formula) = new formula::FormulaLess(*parser_.place_, (yysemantic_stack_[(2) - (2)].yt_int));
  }
    break;

  case 120:

/* Line 677 of lalr1.cc  */
#line 552 "parser-owfn.yy"
    {
    (yyval.yt_formula) = new formula::FormulaGreater(*parser_.place_, (yysemantic_stack_[(2) - (2)].yt_int));
  }
    break;

  case 121:

/* Line 677 of lalr1.cc  */
#line 556 "parser-owfn.yy"
    {
    (yyval.yt_formula) = new formula::FormulaGreaterEqual(*parser_.place_, (yysemantic_stack_[(2) - (2)].yt_int));
  }
    break;

  case 122:

/* Line 677 of lalr1.cc  */
#line 560 "parser-owfn.yy"
    {
    (yyval.yt_formula) = new formula::FormulaLessEqual(*parser_.place_, (yysemantic_stack_[(2) - (2)].yt_int));
  }
    break;

  case 123:

/* Line 677 of lalr1.cc  */
#line 568 "parser-owfn.yy"
    { (yyval.yt_formula) = (yysemantic_stack_[(3) - (2)].yt_formula); }
    break;

  case 124:

/* Line 677 of lalr1.cc  */
#line 569 "parser-owfn.yy"
    { (yyval.yt_formula) = new formula::FormulaTrue(); }
    break;

  case 125:

/* Line 677 of lalr1.cc  */
#line 570 "parser-owfn.yy"
    { (yyval.yt_formula) = new formula::FormulaFalse(); }
    break;

  case 126:

/* Line 677 of lalr1.cc  */
#line 572 "parser-owfn.yy"
    { 
    (yyval.yt_formula) = new formula::Negation(*(yysemantic_stack_[(2) - (2)].yt_formula));
    delete (yysemantic_stack_[(2) - (2)].yt_formula);
  }
    break;

  case 127:

/* Line 677 of lalr1.cc  */
#line 577 "parser-owfn.yy"
    {
    (yyval.yt_formula) = new formula::Disjunction(*(yysemantic_stack_[(3) - (1)].yt_formula), *(yysemantic_stack_[(3) - (3)].yt_formula));
    delete (yysemantic_stack_[(3) - (1)].yt_formula);
    delete (yysemantic_stack_[(3) - (3)].yt_formula);
  }
    break;

  case 128:

/* Line 677 of lalr1.cc  */
#line 583 "parser-owfn.yy"
    {
    (yyval.yt_formula) = new formula::Conjunction(*(yysemantic_stack_[(3) - (1)].yt_formula), *(yysemantic_stack_[(3) - (3)].yt_formula));
    delete (yysemantic_stack_[(3) - (1)].yt_formula);
    delete (yysemantic_stack_[(3) - (3)].yt_formula);
  }
    break;

  case 129:

/* Line 677 of lalr1.cc  */
#line 589 "parser-owfn.yy"
    {
    parser_.place_ = parser_.places_[parser_.nodeName_.str()];
    if(parser_.place_ == NULL)
    {
      parser_.place_ = parser_.places_[parser_.nodeName_.str()] = &(parser_.net_.createPlace(parser_.nodeName_.str()));
    }
  }
    break;

  case 130:

/* Line 677 of lalr1.cc  */
#line 597 "parser-owfn.yy"
    {
    (yyval.yt_formula) = (yysemantic_stack_[(3) - (3)].yt_formula);
  }
    break;

  case 133:

/* Line 677 of lalr1.cc  */
#line 617 "parser-owfn.yy"
    {
    // parser_.check(parser_.labels_[parser_.nodeName_.str()] == NULL, "node name already used");
    // parser_.check(!(parser_.net_.containsNode(parser_.nodeName_.str())), "node name already used");
    try
    {
      parser_.transition_ = &(parser_.net_.createTransition(parser_.nodeName_.str()));
    }
    catch(exception::Error e)
    {
      parser_.rethrow(e);
    }
  }
    break;

  case 134:

/* Line 677 of lalr1.cc  */
#line 630 "parser-owfn.yy"
    {
    parser_.transition_->setCost((yysemantic_stack_[(4) - (4)].yt_int));
  }
    break;

  case 135:

/* Line 677 of lalr1.cc  */
#line 635 "parser-owfn.yy"
    { 
    parser_.target_ = reinterpret_cast<Node * *>(&(parser_.transition_));
    parser_.source_ = reinterpret_cast<Node * *>(&(parser_.place_)); 
    parser_.placeSet_.clear();
    parser_.placeSetType_ = true;
  }
    break;

  case 136:

/* Line 677 of lalr1.cc  */
#line 642 "parser-owfn.yy"
    { 
    parser_.source_ = reinterpret_cast<Node * *>(&(parser_.transition_));
    parser_.target_ = reinterpret_cast<Node * *>(&(parser_.place_)); 
    parser_.placeSet_.clear();
    parser_.placeSetType_ = false;
  }
    break;

  case 138:

/* Line 677 of lalr1.cc  */
#line 652 "parser-owfn.yy"
    { (yyval.yt_int) = 0;  }
    break;

  case 139:

/* Line 677 of lalr1.cc  */
#line 653 "parser-owfn.yy"
    { (yyval.yt_int) = (yysemantic_stack_[(3) - (2)].yt_int); }
    break;

  case 142:

/* Line 677 of lalr1.cc  */
#line 663 "parser-owfn.yy"
    {
    parser_.check(parser_.transition_->getPetriNet().isRoleSpecified(parser_.nodeName_.str()), "role has not been specified");
    parser_.transition_->addRole(parser_.nodeName_.str());
  }
    break;

  case 143:

/* Line 677 of lalr1.cc  */
#line 668 "parser-owfn.yy"
    {
    parser_.check(parser_.transition_->getPetriNet().isRoleSpecified(parser_.nodeName_.str()), "role has not been specified");
    parser_.transition_->addRole(parser_.nodeName_.str());
  }
    break;

  case 147:

/* Line 677 of lalr1.cc  */
#line 682 "parser-owfn.yy"
    {
    parser_.place_ = parser_.places_[parser_.nodeName_.str()];
    parser_.label_ = parser_.labels_[parser_.nodeName_.str()];
    parser_.check(!((parser_.place_ == NULL) && (parser_.label_ == NULL)), "unknown place or interface label");
    parser_.check(parser_.placeSet_.find(parser_.nodeName_.str()) == parser_.placeSet_.end(), parser_.placeSetType_ ? "place already in preset" : "place already in postset");
    parser_.check(!(parser_.placeSetType_ && (parser_.label_ != NULL) && (parser_.label_->getType() != Label::INPUT)), "can only consume from places and input labels");
    parser_.check(!(!parser_.placeSetType_ && (parser_.label_ != NULL) && (parser_.label_->getType() != Label::OUTPUT)), "can only produce to places and output labels");
  }
    break;

  case 149:

/* Line 677 of lalr1.cc  */
#line 695 "parser-owfn.yy"
    {
    if(parser_.place_ != NULL)
    {
      parser_.net_.createArc(**(parser_.source_), **(parser_.target_), (yysemantic_stack_[(2) - (2)].yt_int));
    }
    else
    {
      parser_.transition_->addLabel(*(parser_.label_), (yysemantic_stack_[(2) - (2)].yt_int));
    }

    parser_.placeSet_.insert(parser_.nodeName_.str());
  }
    break;

  case 150:

/* Line 677 of lalr1.cc  */
#line 708 "parser-owfn.yy"
    {
    if(parser_.place_ != NULL)
    {
      parser_.net_.createArc(**(parser_.source_), **(parser_.target_));
    }
    else
    {
      parser_.transition_->addLabel(*(parser_.label_));
    }

    parser_.placeSet_.insert(parser_.nodeName_.str());
  }
    break;

  case 154:

/* Line 677 of lalr1.cc  */
#line 730 "parser-owfn.yy"
    {
    parser_.label_ = parser_.labels_[parser_.nodeName_.str()];
    parser_.check(parser_.label_ != NULL, "unknown label");
    parser_.check(parser_.label_->getType() == Label::SYNCHRONOUS, "can only synchronize with synchronous labels");
    parser_.transition_->addLabel(*(parser_.label_));
  }
    break;

  case 155:

/* Line 677 of lalr1.cc  */
#line 737 "parser-owfn.yy"
    {
    parser_.label_ = parser_.labels_[parser_.nodeName_.str()];
    parser_.check(parser_.label_ != NULL, "unknown label");
    parser_.check(parser_.label_->getType() == Label::SYNCHRONOUS, "can only synchronize with synchronous labels");
    parser_.transition_->addLabel(*(parser_.label_));
  }
    break;

  case 158:

/* Line 677 of lalr1.cc  */
#line 752 "parser-owfn.yy"
    { parser_.constrains_[parser_.transition_].insert(parser_.nodeName_.str()); }
    break;

  case 159:

/* Line 677 of lalr1.cc  */
#line 754 "parser-owfn.yy"
    { parser_.constrains_[parser_.transition_].insert(parser_.nodeName_.str()); }
    break;



/* Line 677 of lalr1.cc  */
#line 1247 "parser-owfn.cc"
	default:
          break;
      }
    YY_SYMBOL_PRINT ("-> $$ =", yyr1_[yyn], &yyval, &yyloc);

    yypop_ (yylen);
    yylen = 0;
    YY_STACK_PRINT ();

    yysemantic_stack_.push (yyval);
    yylocation_stack_.push (yyloc);

    /* Shift the result of the reduction.  */
    yyn = yyr1_[yyn];
    yystate = yypgoto_[yyn - yyntokens_] + yystate_stack_[0];
    if (0 <= yystate && yystate <= yylast_
	&& yycheck_[yystate] == yystate_stack_[0])
      yystate = yytable_[yystate];
    else
      yystate = yydefgoto_[yyn - yyntokens_];
    goto yynewstate;

  /*------------------------------------.
  | yyerrlab -- here on detecting error |
  `------------------------------------*/
  yyerrlab:
    /* If not already recovering from an error, report this error.  */
    if (!yyerrstatus_)
      {
	++yynerrs_;
	error (yylloc, yysyntax_error_ (yystate, yytoken));
      }

    yyerror_range[0] = yylloc;
    if (yyerrstatus_ == 3)
      {
	/* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

	if (yychar <= yyeof_)
	  {
	  /* Return failure if at end of input.  */
	  if (yychar == yyeof_)
	    YYABORT;
	  }
	else
	  {
	    yydestruct_ ("Error: discarding", yytoken, &yylval, &yylloc);
	    yychar = yyempty_;
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
    if (false)
      goto yyerrorlab;

    yyerror_range[0] = yylocation_stack_[yylen - 1];
    /* Do not reclaim the symbols of the rule which action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    yystate = yystate_stack_[0];
    goto yyerrlab1;

  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;	/* Each real token shifted decrements this.  */

    for (;;)
      {
	yyn = yypact_[yystate];
	if (yyn != yypact_ninf_)
	{
	  yyn += yyterror_;
	  if (0 <= yyn && yyn <= yylast_ && yycheck_[yyn] == yyterror_)
	    {
	      yyn = yytable_[yyn];
	      if (0 < yyn)
		break;
	    }
	}

	/* Pop the current state because it cannot handle the error token.  */
	if (yystate_stack_.height () == 1)
	YYABORT;

	yyerror_range[0] = yylocation_stack_[0];
	yydestruct_ ("Error: popping",
		     yystos_[yystate],
		     &yysemantic_stack_[0], &yylocation_stack_[0]);
	yypop_ ();
	yystate = yystate_stack_[0];
	YY_STACK_PRINT ();
      }

    yyerror_range[1] = yylloc;
    // Using YYLLOC is tempting, but would change the location of
    // the lookahead.  YYLOC is available though.
    YYLLOC_DEFAULT (yyloc, (yyerror_range - 1), 2);
    yysemantic_stack_.push (yylval);
    yylocation_stack_.push (yyloc);

    /* Shift the error token.  */
    YY_SYMBOL_PRINT ("Shifting", yystos_[yyn],
		     &yysemantic_stack_[0], &yylocation_stack_[0]);

    yystate = yyn;
    goto yynewstate;

    /* Accept.  */
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;

    /* Abort.  */
  yyabortlab:
    yyresult = 1;
    goto yyreturn;

  yyreturn:
    if (yychar != yyempty_)
      yydestruct_ ("Cleanup: discarding lookahead", yytoken, &yylval, &yylloc);

    /* Do not reclaim the symbols of the rule which action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    while (yystate_stack_.height () != 1)
      {
	yydestruct_ ("Cleanup: popping",
		   yystos_[yystate_stack_[0]],
		   &yysemantic_stack_[0],
		   &yylocation_stack_[0]);
	yypop_ ();
      }

    return yyresult;
  }

  // Generate an error message.
  std::string
  BisonParser::yysyntax_error_ (int yystate, int tok)
  {
    std::string res;
    YYUSE (yystate);
#if YYERROR_VERBOSE
    int yyn = yypact_[yystate];
    if (yypact_ninf_ < yyn && yyn <= yylast_)
      {
	/* Start YYX at -YYN if negative to avoid negative indexes in
	   YYCHECK.  */
	int yyxbegin = yyn < 0 ? -yyn : 0;

	/* Stay within bounds of both yycheck and yytname.  */
	int yychecklim = yylast_ - yyn + 1;
	int yyxend = yychecklim < yyntokens_ ? yychecklim : yyntokens_;
	int count = 0;
	for (int x = yyxbegin; x < yyxend; ++x)
	  if (yycheck_[x + yyn] == x && x != yyterror_)
	    ++count;

	// FIXME: This method of building the message is not compatible
	// with internationalization.  It should work like yacc.c does it.
	// That is, first build a string that looks like this:
	// "syntax error, unexpected %s or %s or %s"
	// Then, invoke YY_ on this string.
	// Finally, use the string as a format to output
	// yytname_[tok], etc.
	// Until this gets fixed, this message appears in English only.
	res = "syntax error, unexpected ";
	res += yytnamerr_ (yytname_[tok]);
	if (count < 5)
	  {
	    count = 0;
	    for (int x = yyxbegin; x < yyxend; ++x)
	      if (yycheck_[x + yyn] == x && x != yyterror_)
		{
		  res += (!count++) ? ", expecting " : " or ";
		  res += yytnamerr_ (yytname_[x]);
		}
	  }
      }
    else
#endif
      res = YY_("syntax error");
    return res;
  }


  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
  const short int BisonParser::yypact_ninf_ = -178;
  const short int
  BisonParser::yypact_[] =
  {
        61,    17,    13,   107,    48,    27,  -178,    34,  -178,   115,
      42,    -8,    34,    50,    73,   120,  -178,  -178,  -178,  -178,
    -178,   122,   122,  -178,    31,  -178,  -178,  -178,  -178,    95,
    -178,  -178,  -178,    34,  -178,  -178,  -178,  -178,  -178,  -178,
      34,    58,  -178,    83,  -178,    74,     8,    34,  -178,    95,
    -178,  -178,  -178,  -178,  -178,  -178,    39,  -178,   134,  -178,
     122,   122,    34,   108,   126,    93,    95,    95,    95,  -178,
     -21,  -178,  -178,  -178,  -178,   118,    34,    34,  -178,   104,
      34,   139,  -178,   116,   148,   156,   157,   161,   163,  -178,
    -178,  -178,  -178,    70,  -178,    34,  -178,  -178,   162,    13,
     160,    34,   166,   167,    34,  -178,   126,    84,  -178,  -178,
      74,  -178,   169,    13,  -178,  -178,  -178,  -178,  -178,  -178,
     171,    34,  -178,  -178,  -178,   197,    13,  -178,   174,    13,
      13,  -178,   162,   159,   164,   165,   177,   118,    34,  -178,
     173,  -178,  -178,   143,   189,  -178,  -178,    34,   118,    34,
     197,   175,   158,   152,  -178,  -178,  -178,   112,    34,  -178,
    -178,  -178,    79,  -178,  -178,   178,  -178,   174,  -178,  -178,
    -178,    84,    84,    84,  -178,  -178,    34,  -178,   127,    34,
    -178,  -178,  -178,  -178,    22,    22,  -178,    56,   182,   199,
     118,  -178,  -178,  -178,    84,    84,  -178,  -178,   187,   186,
      91,  -178,   134,  -178,    65,    22,  -178,    34,   206,  -178,
    -178,  -178,     4,  -178,  -178,  -178,  -178,  -178,  -178,  -178,
    -178,   131,  -178,   187,    34,  -178,    34,  -178,  -178,   132,
    -178,   191,    34,   209,   190,  -178,  -178,  -178,  -178,    34,
     137,   215,    34,   218,  -178,   140,    34,  -178,    34,  -178,
    -178,   151,  -178,    34,  -178,  -178
  };

  /* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
     doesn't specify something else to do.  Zero means the default is an
     error.  */
  const unsigned char
  BisonParser::yydefact_[] =
  {
         0,    14,    69,     0,     0,     0,     3,     0,     6,     9,
      11,     0,    59,     0,    77,    67,   124,   125,   103,     4,
       5,     0,     0,   129,     0,     1,    85,   131,    47,    29,
      21,    23,    25,     0,    20,    15,    16,    17,    18,    19,
       0,    10,    30,     0,    60,    57,    29,     0,     7,    29,
      72,    73,    74,    75,    70,    71,     0,   126,     0,   104,
       0,     0,    88,     2,    51,     0,    29,    29,    29,    65,
       0,    49,    12,    13,    31,    37,     0,    59,    68,     0,
      78,     0,   123,     0,     0,     0,     0,     0,     0,   130,
     128,   127,    91,     0,    89,     0,   132,    52,    53,     8,
       0,    32,     0,     0,     0,    64,    51,    39,    61,    62,
      58,    79,     0,    76,   122,   121,   119,   120,   118,   117,
      94,     0,    86,   133,    54,    55,    22,    33,    27,    24,
      26,    66,    53,     0,     0,     0,     0,    37,     0,    81,
       0,    92,    90,     0,   138,    56,    48,    32,    37,     0,
      55,     0,     0,     0,    38,    63,    83,     0,     0,    93,
      95,   100,     0,    87,    97,     0,   134,    28,    34,    35,
      50,    39,    39,    39,    43,    45,     0,    80,     0,    88,
     108,   106,   107,   101,     0,     0,   115,     0,     0,   140,
      37,    40,    41,    42,    39,    39,    84,    82,    98,     0,
       0,   109,     0,   102,     0,     0,   139,     0,     0,    36,
      44,    46,    88,   105,   116,   112,   113,   114,   111,   110,
     142,     0,   135,    99,     0,   141,   144,   143,   147,     0,
     145,   150,     0,     0,     0,   148,   146,   136,   149,   144,
       0,   151,   153,   156,   154,     0,     0,   137,     0,   152,
     158,     0,   155,     0,   157,   159
  };

  /* YYPGOTO[NTERM-NUM].  */
  const short int
  BisonParser::yypgoto_[] =
  {
      -178,  -178,    -7,  -178,   192,  -178,  -178,  -178,  -178,     9,
    -178,    10,  -178,    19,  -178,   125,   -45,    87,  -178,  -178,
    -134,  -163,  -178,  -178,  -178,  -178,  -178,   124,    99,    86,
     -12,   168,  -178,  -178,    -3,  -178,  -178,  -178,  -178,  -178,
    -178,  -178,  -178,  -178,    80,  -178,  -178,  -178,  -177,   119,
    -178,  -178,  -178,  -178,  -178,  -178,  -178,  -127,  -178,    35,
     -15,  -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,
    -178,     3,    11,  -178,  -178,  -178,  -178,  -178,  -178
  };

  /* YYDEFGOTO[NTERM-NUM].  */
  const short int
  BisonParser::yydefgoto_[] =
  {
        -1,     4,    23,     5,    34,     8,    41,     9,    35,    97,
      66,   124,    67,   145,    68,   100,    12,   128,   148,   190,
     108,   136,   194,   195,    10,    64,   106,    98,   125,   146,
      13,    45,    75,   137,    39,    70,    14,    15,    54,    55,
      48,    80,   138,   158,   157,    27,    62,   143,    93,    94,
     120,   141,   163,   179,   199,   164,     6,   187,   202,    89,
      24,    58,    63,    96,   144,   189,   226,   239,   166,   208,
     221,   229,   230,   231,   235,   243,   245,   247,   251
  };

  /* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule which
     number is the opposite.  If zero, do what YYDEFACT says.  */
  const signed char BisonParser::yytable_ninf_ = -97;
  const short int
  BisonParser::yytable_[] =
  {
        28,    77,   198,   155,   -96,    44,    56,    57,   191,   192,
     193,    11,    53,   104,   168,   105,    11,    65,    36,    37,
     -96,   101,   101,   101,    50,    51,    69,    42,    38,    33,
      43,   210,   211,    71,    52,   223,     7,    81,    73,    26,
      79,    19,    20,    78,   180,    90,    91,   -29,    25,   -29,
     -29,   -29,   181,   182,    77,    92,   209,   200,   201,    19,
      20,    40,   184,    29,   185,     1,     2,    59,    77,   109,
      44,    19,    20,   112,    60,    61,     3,   218,   219,    33,
      82,   147,    60,    61,   147,   147,    46,   180,   123,   215,
     216,   217,   203,    47,   127,   181,   182,   131,    11,   204,
     205,   180,    19,    20,   121,   184,   122,   185,    76,   181,
     182,   133,   134,   135,    92,   183,    19,    20,    74,   184,
      29,   185,    30,    31,    95,    32,    49,    30,    31,    99,
      32,   156,   213,    30,   204,   205,    33,    16,    17,   111,
     127,    33,   169,    18,    19,    20,   176,    21,   177,    22,
     107,   156,    16,    17,   114,   186,   160,   161,   162,    19,
      20,   176,    21,   197,    22,   224,   232,   225,   233,   196,
      31,   232,    92,   241,   248,   113,   249,   186,   186,    83,
      84,    85,    86,    87,    88,   253,   115,   254,   172,   173,
     174,   175,   102,   103,   116,   117,   126,   186,   186,   118,
     220,   119,   129,   130,   139,    92,   140,    32,   149,   151,
     154,   159,   165,   171,   152,   153,   188,   227,   206,   228,
     207,   121,   212,   222,   242,   228,   234,   237,   238,   246,
     132,   150,   228,    72,   167,   244,   170,   214,   178,   250,
     142,   252,   240,   236,     0,   110,   255
  };

  /* YYCHECK.  */
  const short int
  BisonParser::yycheck_[] =
  {
         7,    46,   179,   137,     0,    12,    21,    22,   171,   172,
     173,     3,    15,    34,   148,    36,     3,    29,     9,     9,
      16,    66,    67,    68,    15,    15,    33,    35,     9,    21,
      38,   194,   195,    40,    15,   212,    19,    49,    41,    12,
      47,    37,    38,    46,    22,    60,    61,    34,     0,    36,
      37,    38,    30,    31,    99,    62,   190,   184,   185,    37,
      38,    19,    40,     5,    42,     4,     5,    36,   113,    76,
      77,    37,    38,    80,    43,    44,    15,   204,   205,    21,
      41,   126,    43,    44,   129,   130,    36,    22,    95,    24,
      25,    26,    36,    20,   101,    30,    31,   104,     3,    43,
      44,    22,    37,    38,    34,    40,    36,    42,    34,    30,
      31,    27,    28,    29,   121,    36,    37,    38,    35,    40,
       5,    42,     7,     8,    16,    10,     6,     7,     8,    36,
      10,   138,    41,     7,    43,    44,    21,    30,    31,    35,
     147,    21,   149,    36,    37,    38,    34,    40,    36,    42,
      32,   158,    30,    31,    38,   162,    13,    14,    15,    37,
      38,    34,    40,    36,    42,    34,    34,    36,    36,   176,
       8,    34,   179,    36,    34,    36,    36,   184,   185,    45,
      46,    47,    48,    49,    50,    34,    38,    36,    30,    31,
      38,    39,    67,    68,    38,    38,    36,   204,   205,    38,
     207,    38,    36,    36,    35,   212,    35,    10,    34,    50,
      33,    38,    23,    38,    50,    50,    38,   224,    36,   226,
      21,    34,    36,    17,     9,   232,    35,    18,    38,    11,
     106,   132,   239,    41,   147,   242,   150,   202,   158,   246,
     121,   248,   239,   232,    -1,    77,   253
  };

  /* STOS_[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
  const unsigned char
  BisonParser::yystos_[] =
  {
         0,     4,     5,    15,    52,    54,   107,    19,    56,    58,
      75,     3,    67,    81,    87,    88,    30,    31,    36,    37,
      38,    40,    42,    53,   111,     0,    12,    96,    53,     5,
       7,     8,    10,    21,    55,    59,    60,    62,    64,    85,
      19,    57,    35,    38,    53,    82,    36,    20,    91,     6,
      60,    62,    64,    85,    89,    90,   111,   111,   112,    36,
      43,    44,    97,   113,    76,    81,    61,    63,    65,    53,
      86,    53,    55,    85,    35,    83,    34,    67,    85,    53,
      92,    81,    41,    45,    46,    47,    48,    49,    50,   110,
     111,   111,    53,    99,   100,    16,   114,    60,    78,    36,
      66,    67,    66,    66,    34,    36,    77,    32,    71,    53,
      82,    35,    53,    36,    38,    38,    38,    38,    38,    38,
     101,    34,    36,    53,    62,    79,    36,    53,    68,    36,
      36,    53,    78,    27,    28,    29,    72,    84,    93,    35,
      35,   102,   100,    98,   115,    64,    80,    67,    69,    34,
      79,    50,    50,    50,    33,    71,    53,    95,    94,    38,
      13,    14,    15,   103,   106,    23,   119,    68,    71,    53,
      80,    38,    30,    31,    38,    39,    34,    36,    95,   104,
      22,    30,    31,    36,    40,    42,    53,   108,    38,   116,
      70,    72,    72,    72,    73,    74,    53,    36,    99,   105,
     108,   108,   109,    36,    43,    44,    36,    21,   120,    71,
      72,    72,    36,    41,   110,    24,    25,    26,   108,   108,
      53,   121,    17,    99,    34,    36,   117,    53,    53,   122,
     123,   124,    34,    36,    35,   125,   123,    18,    38,   118,
     122,    36,     9,   126,    53,   127,    11,   128,    34,    36,
      53,   129,    53,    34,    36,    53
  };

#if YYDEBUG
  /* TOKEN_NUMBER_[YYLEX-NUM] -- Internal symbol number corresponding
     to YYLEX-NUM.  */
  const unsigned short int
  BisonParser::yytoken_number_[] =
  {
         0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305
  };
#endif

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
  const unsigned char
  BisonParser::yyr1_[] =
  {
         0,    51,    52,    52,    53,    53,    54,    54,    55,    56,
      56,    57,    57,    57,    58,    58,    59,    59,    59,    59,
      59,    61,    60,    63,    62,    65,    64,    66,    66,    67,
      67,    67,    68,    69,    68,    70,    68,    71,    71,    72,
      72,    72,    72,    73,    72,    74,    72,    76,    75,    77,
      75,    78,    78,    79,    79,    80,    80,    81,    81,    82,
      83,    82,    84,    82,    85,    86,    86,    87,    87,    88,
      88,    89,    89,    89,    89,    89,    90,    91,    91,    93,
      92,    94,    92,    95,    95,    97,    98,    96,    99,    99,
      99,   101,   100,   102,   102,   104,   103,   103,   105,   105,
     106,   106,   106,   107,   107,   108,   108,   108,   108,   108,
     108,   108,   108,   108,   108,   109,   108,   110,   110,   110,
     110,   110,   110,   111,   111,   111,   111,   111,   111,   112,
     111,   113,   113,   115,   116,   117,   118,   114,   119,   119,
     120,   120,   121,   121,   122,   122,   122,   124,   123,   125,
     125,   126,   126,   127,   127,   127,   128,   128,   129,   129
  };

  /* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
  const unsigned char
  BisonParser::yyr2_[] =
  {
         0,     2,     3,     1,     1,     1,     2,     3,     3,     1,
       2,     0,     2,     2,     0,     2,     1,     1,     1,     1,
       1,     0,     4,     0,     4,     0,     4,     2,     4,     0,
       2,     3,     0,     0,     3,     0,     5,     0,     3,     0,
       4,     4,     4,     0,     5,     0,     5,     0,     6,     0,
       7,     0,     1,     0,     1,     0,     1,     2,     4,     0,
       0,     3,     0,     5,     3,     1,     3,     1,     3,     0,
       2,     1,     1,     1,     1,     1,     3,     0,     2,     0,
       5,     0,     6,     1,     3,     0,     0,     6,     0,     1,
       3,     0,     3,     2,     0,     0,     4,     1,     1,     3,
       1,     2,     3,     2,     3,     3,     1,     1,     1,     2,
       3,     3,     3,     3,     3,     0,     3,     2,     2,     2,
       2,     2,     2,     3,     1,     1,     2,     3,     3,     0,
       3,     0,     2,     0,     0,     0,     0,    16,     0,     3,
       0,     3,     1,     3,     0,     1,     3,     0,     3,     2,
       0,     0,     3,     0,     1,     3,     0,     3,     1,     3
  };

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
  /* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
     First, the terminals, then, starting at \a yyntokens_, nonterminals.  */
  const char*
  const BisonParser::yytname_[] =
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
  "OP_GE", "OP_LT", "OP_GT", "OP_NE", "OP_EQ", "$accept", "petrinet",
  "node_name", "interface", "interface_place", "interface_ports",
  "port_list_tail", "interface_ports_list", "interface_ports_list_element",
  "input_places", "$@1", "output_places", "$@2", "synchronous", "$@3",
  "interface_labels", "capacity", "interface_label_list", "$@4", "$@5",
  "controlcommands", "commands", "$@6", "$@7", "port_list", "$@8", "$@9",
  "port_input_places", "port_output_places", "port_synchronous", "places",
  "place_list", "$@10", "$@11", "roles", "role_names", "typed_interface",
  "typed_interface_list", "typed_interface_list_element",
  "internal_places", "ports", "port_list2", "$@12", "$@13",
  "port_participants", "markings", "$@14", "$@15", "marking_list",
  "marking", "$@16", "marking_tail", "final", "$@17", "finalmarkings",
  "condition", "finalcondition_only", "formula", "$@18",
  "formula_proposition", "fo_formula", "$@19", "transitions", "transition",
  "$@20", "$@21", "$@22", "$@23", "transition_cost", "transition_roles",
  "transition_role_names", "arcs", "arc", "$@24", "arc_tail",
  "synchronize", "synchronize_labels", "constrain", "constrain_labels", 0
  };
#endif

#if YYDEBUG
  /* YYRHS -- A `-1'-separated list of the rules' RHS.  */
  const BisonParser::rhs_number_type
  BisonParser::yyrhs_[] =
  {
        52,     0,    -1,    54,    96,   113,    -1,   107,    -1,    37,
      -1,    38,    -1,     4,    56,    -1,     5,    87,    91,    -1,
       5,    81,    36,    -1,    58,    -1,    75,    57,    -1,    -1,
      57,    55,    -1,    57,    85,    -1,    -1,    58,    59,    -1,
      60,    -1,    62,    -1,    64,    -1,    85,    -1,    55,    -1,
      -1,     7,    61,    66,    36,    -1,    -1,     8,    63,    66,
      36,    -1,    -1,    10,    65,    66,    36,    -1,    67,    68,
      -1,    66,    36,    67,    68,    -1,    -1,     3,    35,    -1,
       3,    38,    35,    -1,    -1,    -1,    53,    69,    71,    -1,
      -1,    68,    34,    53,    70,    71,    -1,    -1,    32,    72,
      33,    -1,    -1,    27,    50,    38,    72,    -1,    28,    50,
      30,    72,    -1,    28,    50,    31,    72,    -1,    -1,    29,
      50,    38,    73,    72,    -1,    -1,    29,    50,    39,    74,
      72,    -1,    -1,    19,    53,    76,    78,    79,    80,    -1,
      -1,    75,    19,    53,    77,    78,    79,    80,    -1,    -1,
      60,    -1,    -1,    62,    -1,    -1,    64,    -1,    67,    82,
      -1,    81,    36,    67,    82,    -1,    -1,    -1,    53,    83,
      71,    -1,    -1,    82,    34,    53,    84,    71,    -1,    21,
      86,    36,    -1,    53,    -1,    86,    34,    53,    -1,    88,
      -1,    81,    36,    85,    -1,    -1,    88,    89,    -1,    90,
      -1,    60,    -1,    62,    -1,    64,    -1,    85,    -1,     6,
      81,    36,    -1,    -1,    20,    92,    -1,    -1,    53,    35,
      93,    95,    36,    -1,    -1,    92,    53,    35,    94,    95,
      36,    -1,    53,    -1,    95,    34,    53,    -1,    -1,    -1,
      12,    97,    99,    36,    98,   103,    -1,    -1,   100,    -1,
      99,    34,   100,    -1,    -1,    53,   101,   102,    -1,    35,
      38,    -1,    -1,    -1,    13,   104,   105,    36,    -1,   106,
      -1,    99,    -1,   105,    36,    99,    -1,    14,    -1,    15,
      36,    -1,    15,   108,    36,    -1,    15,    36,    -1,    15,
     111,    36,    -1,    40,   108,    41,    -1,    30,    -1,    31,
      -1,    22,    -1,    42,   108,    -1,   108,    44,   108,    -1,
     108,    43,   108,    -1,   108,    43,    24,    -1,   108,    43,
      25,    -1,   108,    43,    26,    -1,    -1,    53,   109,   110,
      -1,    50,    38,    -1,    49,    38,    -1,    47,    38,    -1,
      48,    38,    -1,    46,    38,    -1,    45,    38,    -1,    40,
     111,    41,    -1,    30,    -1,    31,    -1,    42,   111,    -1,
     111,    44,   111,    -1,   111,    43,   111,    -1,    -1,    53,
     112,   110,    -1,    -1,   113,   114,    -1,    -1,    -1,    -1,
      -1,    16,    53,   115,   119,   116,   120,    17,   117,   122,
      36,    18,   118,   122,    36,   126,   128,    -1,    -1,    23,
      38,    36,    -1,    -1,    21,   121,    36,    -1,    53,    -1,
     121,    34,    53,    -1,    -1,   123,    -1,   122,    34,   123,
      -1,    -1,    53,   124,   125,    -1,    35,    38,    -1,    -1,
      -1,     9,   127,    36,    -1,    -1,    53,    -1,   127,    34,
      53,    -1,    -1,    11,   129,    36,    -1,    53,    -1,   129,
      34,    53,    -1
  };

  /* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
     YYRHS.  */
  const unsigned short int
  BisonParser::yyprhs_[] =
  {
         0,     0,     3,     7,     9,    11,    13,    16,    20,    24,
      26,    29,    30,    33,    36,    37,    40,    42,    44,    46,
      48,    50,    51,    56,    57,    62,    63,    68,    71,    76,
      77,    80,    84,    85,    86,    90,    91,    97,    98,   102,
     103,   108,   113,   118,   119,   125,   126,   132,   133,   140,
     141,   149,   150,   152,   153,   155,   156,   158,   161,   166,
     167,   168,   172,   173,   179,   183,   185,   189,   191,   195,
     196,   199,   201,   203,   205,   207,   209,   213,   214,   217,
     218,   224,   225,   232,   234,   238,   239,   240,   247,   248,
     250,   254,   255,   259,   262,   263,   264,   269,   271,   273,
     277,   279,   282,   286,   289,   293,   297,   299,   301,   303,
     306,   310,   314,   318,   322,   326,   327,   331,   334,   337,
     340,   343,   346,   349,   353,   355,   357,   360,   364,   368,
     369,   373,   374,   377,   378,   379,   380,   381,   398,   399,
     403,   404,   408,   410,   414,   415,   417,   421,   422,   426,
     429,   430,   431,   435,   436,   438,   442,   443,   447,   449
  };

  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
  const unsigned short int
  BisonParser::yyrline_[] =
  {
         0,    95,    95,    99,   103,   112,   129,   130,   134,   138,
     139,   142,   144,   145,   148,   150,   154,   155,   156,   157,
     158,   162,   162,   166,   166,   170,   170,   175,   176,   179,
     181,   182,   185,   188,   187,   204,   203,   221,   223,   226,
     228,   229,   230,   232,   231,   240,   239,   251,   250,   254,
     253,   258,   260,   263,   265,   268,   270,   274,   275,   278,
     281,   280,   297,   296,   316,   320,   322,   328,   329,   332,
     334,   338,   339,   340,   341,   342,   346,   350,   352,   357,
     356,   360,   359,   365,   375,   394,   395,   394,   398,   400,
     401,   406,   405,   414,   426,   440,   439,   446,   459,   465,
     474,   475,   476,   480,   481,   489,   490,   491,   492,   497,
     502,   508,   514,   519,   523,   528,   527,   539,   543,   547,
     551,   555,   559,   568,   569,   570,   571,   576,   582,   589,
     588,   610,   612,   617,   630,   635,   642,   616,   652,   653,
     656,   658,   662,   667,   674,   676,   677,   682,   681,   694,
     708,   722,   724,   727,   729,   736,   745,   747,   751,   753
  };

  // Print the state stack on the debug stream.
  void
  BisonParser::yystack_print_ ()
  {
    *yycdebug_ << "Stack now";
    for (state_stack_type::const_iterator i = yystate_stack_.begin ();
	 i != yystate_stack_.end (); ++i)
      *yycdebug_ << ' ' << *i;
    *yycdebug_ << std::endl;
  }

  // Report on the debug stream that the rule \a yyrule is going to be reduced.
  void
  BisonParser::yy_reduce_print_ (int yyrule)
  {
    unsigned int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    /* Print the symbols being reduced, and their result.  */
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
	       << " (line " << yylno << "):" << std::endl;
    /* The symbols being reduced.  */
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
		       yyrhs_[yyprhs_[yyrule] + yyi],
		       &(yysemantic_stack_[(yynrhs) - (yyi + 1)]),
		       &(yylocation_stack_[(yynrhs) - (yyi + 1)]));
  }
#endif // YYDEBUG

  /* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
  BisonParser::token_number_type
  BisonParser::yytranslate_ (int t)
  {
    static
    const token_number_type
    translate_table[] =
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
    if ((unsigned int) t <= yyuser_token_number_max_)
      return translate_table[t];
    else
      return yyundef_token_;
  }

  const int BisonParser::yyeof_ = 0;
  const int BisonParser::yylast_ = 246;
  const int BisonParser::yynnts_ = 79;
  const int BisonParser::yyempty_ = -2;
  const int BisonParser::yyfinal_ = 25;
  const int BisonParser::yyterror_ = 1;
  const int BisonParser::yyerrcode_ = 256;
  const int BisonParser::yyntokens_ = 51;

  const unsigned int BisonParser::yyuser_token_number_max_ = 305;
  const BisonParser::token_number_type BisonParser::yyundef_token_ = 2;


/* Line 1053 of lalr1.cc  */
#line 11 "parser-owfn.yy"
} } } } // pnapi::parser::owfn::yy

/* Line 1053 of lalr1.cc  */
#line 1931 "parser-owfn.cc"


