
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison LALR(1) parsers in C++
   
      Copyright (C) 2002, 2003, 2004, 2005, 2006, 2007, 2008 Free Software
   Foundation, Inc.
   
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

/* Line 311 of lalr1.cc  */
#line 41 "parser-owfn.yy"


#include "formula.h"
#include "marking.h"
#include "parser-owfn-wrapper.h"
#include "petrinet.h"

#include <sstream>



/* Line 311 of lalr1.cc  */
#line 53 "parser-owfn.cc"


#include "parser-owfn.h"

/* User implementation prologue.  */


/* Line 317 of lalr1.cc  */
#line 62 "parser-owfn.cc"

#ifndef YY_
# if YYENABLE_NLS
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


/* Line 380 of lalr1.cc  */
#line 11 "parser-owfn.yy"
namespace pnapi { namespace parser { namespace owfn { namespace yy {

/* Line 380 of lalr1.cc  */
#line 130 "parser-owfn.cc"
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

/* Line 678 of lalr1.cc  */
#line 96 "parser-owfn.yy"
    { 
    parser_.net_.setConstraintLabels(parser_.constrains_); 
  }
    break;

  case 3:

/* Line 678 of lalr1.cc  */
#line 103 "parser-owfn.yy"
    { 
    // clear stringstream
    parser_.nodeName_.str("");
    parser_.nodeName_.clear();

    parser_.nodeName_ << (yysemantic_stack_[(1) - (1)].yt_str);
    free((yysemantic_stack_[(1) - (1)].yt_str)); 
  }
    break;

  case 4:

/* Line 678 of lalr1.cc  */
#line 112 "parser-owfn.yy"
    { 
    // clear stringstream
    parser_.nodeName_.str("");
    parser_.nodeName_.clear();

    parser_.nodeName_ << (yysemantic_stack_[(1) - (1)].yt_int); 
  }
    break;

  case 20:

/* Line 678 of lalr1.cc  */
#line 161 "parser-owfn.yy"
    { parser_.labelType_ = Label::INPUT; }
    break;

  case 22:

/* Line 678 of lalr1.cc  */
#line 165 "parser-owfn.yy"
    { parser_.labelType_ = Label::OUTPUT; }
    break;

  case 24:

/* Line 678 of lalr1.cc  */
#line 169 "parser-owfn.yy"
    { parser_.labelType_ = Label::SYNCHRONOUS; }
    break;

  case 26:

/* Line 678 of lalr1.cc  */
#line 174 "parser-owfn.yy"
    { parser_.capacity_ = 0; }
    break;

  case 27:

/* Line 678 of lalr1.cc  */
#line 175 "parser-owfn.yy"
    { parser_.capacity_ = 0; }
    break;

  case 29:

/* Line 678 of lalr1.cc  */
#line 180 "parser-owfn.yy"
    { parser_.capacity_ = 1; }
    break;

  case 30:

/* Line 678 of lalr1.cc  */
#line 181 "parser-owfn.yy"
    { parser_.capacity_ = (yysemantic_stack_[(3) - (2)].yt_int); }
    break;

  case 32:

/* Line 678 of lalr1.cc  */
#line 187 "parser-owfn.yy"
    {
    parser_.check(parser_.labels_[parser_.nodeName_.str()] == NULL, "node name already used");
    parser_.check(parser_.places_[parser_.nodeName_.str()] == NULL, "node name already used");
    parser_.label_ = &(parser_.port_->addLabel(parser_.nodeName_.str(), parser_.labelType_));
    parser_.labels_[parser_.nodeName_.str()] = parser_.label_;
  }
    break;

  case 34:

/* Line 678 of lalr1.cc  */
#line 195 "parser-owfn.yy"
    {
    parser_.check(parser_.labels_[parser_.nodeName_.str()] == NULL, "node name already used");
    parser_.check(parser_.places_[parser_.nodeName_.str()] == NULL, "node name already used");
    parser_.label_ = &(parser_.port_->addLabel(parser_.nodeName_.str(), parser_.labelType_));
    parser_.labels_[parser_.nodeName_.str()] = parser_.label_;
  }
    break;

  case 42:

/* Line 678 of lalr1.cc  */
#line 215 "parser-owfn.yy"
    {
    if(parser_.place_ != NULL) // is only NULL when parsing interface
    {
      parser_.place_->setMaxOccurrence((yysemantic_stack_[(3) - (3)].yt_int));
    }
  }
    break;

  case 44:

/* Line 678 of lalr1.cc  */
#line 223 "parser-owfn.yy"
    {
    if(parser_.place_ != NULL) // is only NULL when parsing interface
    {
      parser_.place_->setMaxOccurrence((yysemantic_stack_[(3) - (3)].yt_int));
    }
  }
    break;

  case 46:

/* Line 678 of lalr1.cc  */
#line 234 "parser-owfn.yy"
    { parser_.port_ = &(parser_.net_.getInterface().addPort(parser_.nodeName_.str())); }
    break;

  case 48:

/* Line 678 of lalr1.cc  */
#line 237 "parser-owfn.yy"
    { parser_.port_ = &(parser_.net_.getInterface().addPort(parser_.nodeName_.str())); }
    break;

  case 56:

/* Line 678 of lalr1.cc  */
#line 257 "parser-owfn.yy"
    { parser_.capacity_ = 0; }
    break;

  case 57:

/* Line 678 of lalr1.cc  */
#line 258 "parser-owfn.yy"
    { parser_.capacity_ = 0; }
    break;

  case 59:

/* Line 678 of lalr1.cc  */
#line 264 "parser-owfn.yy"
    {
    parser_.check(parser_.labels_[parser_.nodeName_.str()] == NULL, "node name already used");
    parser_.check(parser_.places_[parser_.nodeName_.str()] == NULL, "node name already used");
    parser_.place_ = &(parser_.net_.createPlace(parser_.nodeName_.str(), 0, parser_.capacity_));
    parser_.places_[parser_.nodeName_.str()] = parser_.place_;
  }
    break;

  case 61:

/* Line 678 of lalr1.cc  */
#line 272 "parser-owfn.yy"
    {
    parser_.check(parser_.labels_[parser_.nodeName_.str()] == NULL, "node name already used");
    parser_.check(parser_.places_[parser_.nodeName_.str()] == NULL, "node name already used");
    parser_.place_ = &(parser_.net_.createPlace(parser_.nodeName_.str(), 0, parser_.capacity_));
    parser_.places_[parser_.nodeName_.str()] = parser_.place_;
  }
    break;

  case 64:

/* Line 678 of lalr1.cc  */
#line 288 "parser-owfn.yy"
    { parser_.net_.addRole(parser_.nodeName_.str()); }
    break;

  case 65:

/* Line 678 of lalr1.cc  */
#line 290 "parser-owfn.yy"
    { parser_.net_.addRole(parser_.nodeName_.str()); }
    break;

  case 78:

/* Line 678 of lalr1.cc  */
#line 324 "parser-owfn.yy"
    { parser_.port_ = &(parser_.net_.getInterface().addPort(parser_.nodeName_.str())); }
    break;

  case 80:

/* Line 678 of lalr1.cc  */
#line 327 "parser-owfn.yy"
    { parser_.port_ = &(parser_.net_.getInterface().addPort(parser_.nodeName_.str())); }
    break;

  case 82:

/* Line 678 of lalr1.cc  */
#line 333 "parser-owfn.yy"
    {
    Label * l = parser_.labels_[parser_.nodeName_.str()];
    parser_.check(l != NULL, "unknown interface label");
    parser_.check((l->getPort().getName() == ""), "interface label already assigned to port '" + l->getPort().getName() +"'");
     
    Label::Type t = l->getType();
    parser_.net_.getInterface().getPort()->removeLabel(parser_.nodeName_.str());
    parser_.labels_[parser_.nodeName_.str()] = &(parser_.port_->addLabel(parser_.nodeName_.str(), t));
  }
    break;

  case 83:

/* Line 678 of lalr1.cc  */
#line 343 "parser-owfn.yy"
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

/* Line 678 of lalr1.cc  */
#line 361 "parser-owfn.yy"
    { parser_.markInitial_ = true; }
    break;

  case 85:

/* Line 678 of lalr1.cc  */
#line 362 "parser-owfn.yy"
    { parser_.markInitial_ = false; }
    break;

  case 90:

/* Line 678 of lalr1.cc  */
#line 373 "parser-owfn.yy"
    { 
    Place * p = parser_.places_[parser_.nodeName_.str()];
    parser_.check(p != NULL, "unknown place");      
      
    if(parser_.markInitial_)
    {  
      p->setTokenCount((yysemantic_stack_[(3) - (3)].yt_int));
    }
    else
    {
      (*(parser_.finalMarking_))[*p] = (yysemantic_stack_[(3) - (3)].yt_int);
    }
  }
    break;

  case 91:

/* Line 678 of lalr1.cc  */
#line 387 "parser-owfn.yy"
    { 
    Place * p = parser_.places_[parser_.nodeName_.str()];
    parser_.check(p != NULL, "unknown place");
    
    if(parser_.markInitial_)
    {  
      p->setTokenCount(1);
    }
    else
    {
      (*(parser_.finalMarking_))[*p] = 1;
    }
  }
    break;

  case 92:

/* Line 678 of lalr1.cc  */
#line 404 "parser-owfn.yy"
    { parser_.finalMarking_ = new Marking(parser_.net_, true); }
    break;

  case 93:

/* Line 678 of lalr1.cc  */
#line 406 "parser-owfn.yy"
    {
    delete parser_.finalMarking_;
    parser_.finalMarking_ = NULL;
  }
    break;

  case 94:

/* Line 678 of lalr1.cc  */
#line 411 "parser-owfn.yy"
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

  case 95:

/* Line 678 of lalr1.cc  */
#line 424 "parser-owfn.yy"
    {
    parser_.net_.getFinalCondition().addMarking(*(parser_.finalMarking_));
    delete parser_.finalMarking_;
    parser_.finalMarking_ = new Marking(parser_.net_, true);
  }
    break;

  case 96:

/* Line 678 of lalr1.cc  */
#line 430 "parser-owfn.yy"
    {
    parser_.net_.getFinalCondition().addMarking(*(parser_.finalMarking_));
    delete parser_.finalMarking_;
    parser_.finalMarking_ = new Marking(parser_.net_, true);
  }
    break;

  case 97:

/* Line 678 of lalr1.cc  */
#line 438 "parser-owfn.yy"
    { (yyval.yt_formula) = new formula::FormulaTrue(); }
    break;

  case 98:

/* Line 678 of lalr1.cc  */
#line 439 "parser-owfn.yy"
    { (yyval.yt_formula) = new formula::FormulaFalse(); }
    break;

  case 99:

/* Line 678 of lalr1.cc  */
#line 440 "parser-owfn.yy"
    { (yyval.yt_formula) = (yysemantic_stack_[(3) - (2)].yt_formula); }
    break;

  case 100:

/* Line 678 of lalr1.cc  */
#line 444 "parser-owfn.yy"
    { (yyval.yt_formula) = (yysemantic_stack_[(3) - (2)].yt_formula); }
    break;

  case 101:

/* Line 678 of lalr1.cc  */
#line 445 "parser-owfn.yy"
    { (yyval.yt_formula) = new formula::FormulaTrue(); }
    break;

  case 102:

/* Line 678 of lalr1.cc  */
#line 446 "parser-owfn.yy"
    { (yyval.yt_formula) = new formula::FormulaFalse(); }
    break;

  case 103:

/* Line 678 of lalr1.cc  */
#line 448 "parser-owfn.yy"
    { 
    parser_.wildcardGiven_ = true;
    (yyval.yt_formula) = new formula::FormulaTrue();
  }
    break;

  case 104:

/* Line 678 of lalr1.cc  */
#line 453 "parser-owfn.yy"
    { 
    (yyval.yt_formula) = new formula::Negation(*(yysemantic_stack_[(2) - (2)].yt_formula));
    delete (yysemantic_stack_[(2) - (2)].yt_formula);
  }
    break;

  case 105:

/* Line 678 of lalr1.cc  */
#line 458 "parser-owfn.yy"
    {
    (yyval.yt_formula) = new formula::Disjunction(*(yysemantic_stack_[(3) - (1)].yt_formula), *(yysemantic_stack_[(3) - (3)].yt_formula));
    delete (yysemantic_stack_[(3) - (1)].yt_formula);
    delete (yysemantic_stack_[(3) - (3)].yt_formula);
  }
    break;

  case 106:

/* Line 678 of lalr1.cc  */
#line 464 "parser-owfn.yy"
    {
    (yyval.yt_formula) = new formula::Conjunction(*(yysemantic_stack_[(3) - (1)].yt_formula), *(yysemantic_stack_[(3) - (3)].yt_formula));
    delete (yysemantic_stack_[(3) - (1)].yt_formula);
    delete (yysemantic_stack_[(3) - (3)].yt_formula);
  }
    break;

  case 107:

/* Line 678 of lalr1.cc  */
#line 470 "parser-owfn.yy"
    {
    parser_.wildcardGiven_ = true;
    (yyval.yt_formula) = (yysemantic_stack_[(3) - (1)].yt_formula);
  }
    break;

  case 108:

/* Line 678 of lalr1.cc  */
#line 475 "parser-owfn.yy"
    {
    (yyval.yt_formula) = (yysemantic_stack_[(3) - (1)].yt_formula); // obsolete; kept due to compatibility
  }
    break;

  case 109:

/* Line 678 of lalr1.cc  */
#line 479 "parser-owfn.yy"
    {
    (yyval.yt_formula) = (yysemantic_stack_[(3) - (1)].yt_formula); // obsolete; kept due to compatibility
  }
    break;

  case 110:

/* Line 678 of lalr1.cc  */
#line 483 "parser-owfn.yy"
    {
    Place * p = parser_.places_[parser_.nodeName_.str()];
    parser_.check(p != NULL, "unknown place");
    
    (yyval.yt_formula) = new formula::FormulaEqual(*p, (yysemantic_stack_[(3) - (3)].yt_int));
  }
    break;

  case 111:

/* Line 678 of lalr1.cc  */
#line 490 "parser-owfn.yy"
    {
    Place * p = parser_.places_[parser_.nodeName_.str()];
    parser_.check(p != NULL, "unknown place");

    (yyval.yt_formula) = new formula::Negation(formula::FormulaEqual(*p, (yysemantic_stack_[(3) - (3)].yt_int)));
  }
    break;

  case 112:

/* Line 678 of lalr1.cc  */
#line 497 "parser-owfn.yy"
    {
    Place * p = parser_.places_[parser_.nodeName_.str()];
    parser_.check(p != NULL, "unknown place");

    (yyval.yt_formula) = new formula::FormulaLess(*p, (yysemantic_stack_[(3) - (3)].yt_int));
  }
    break;

  case 113:

/* Line 678 of lalr1.cc  */
#line 504 "parser-owfn.yy"
    {
    Place * p = parser_.places_[parser_.nodeName_.str()];
    parser_.check(p != NULL, "unknown place");

    (yyval.yt_formula) = new formula::FormulaGreater(*p, (yysemantic_stack_[(3) - (3)].yt_int));
  }
    break;

  case 114:

/* Line 678 of lalr1.cc  */
#line 511 "parser-owfn.yy"
    {
    Place * p = parser_.places_[parser_.nodeName_.str()];
    parser_.check(p != NULL, "unknown place");

    (yyval.yt_formula) = new formula::FormulaGreaterEqual(*p, (yysemantic_stack_[(3) - (3)].yt_int));
  }
    break;

  case 115:

/* Line 678 of lalr1.cc  */
#line 518 "parser-owfn.yy"
    {
    Place * p = parser_.places_[parser_.nodeName_.str()];
    parser_.check(p != NULL, "unknown place");

    (yyval.yt_formula) = new formula::FormulaLessEqual(*p, (yysemantic_stack_[(3) - (3)].yt_int));
  }
    break;

  case 118:

/* Line 678 of lalr1.cc  */
#line 539 "parser-owfn.yy"
    {
    parser_.check(parser_.labels_[parser_.nodeName_.str()] == NULL, "node name already used");
    parser_.check(!(parser_.net_.containsNode(parser_.nodeName_.str())), "node name already used");
    parser_.transition_ = &(parser_.net_.createTransition(parser_.nodeName_.str())); 
    parser_.transition_->setCost((yysemantic_stack_[(3) - (3)].yt_int));
  }
    break;

  case 119:

/* Line 678 of lalr1.cc  */
#line 547 "parser-owfn.yy"
    { 
    parser_.target_ = reinterpret_cast<Node * *>(&(parser_.transition_));
    parser_.source_ = reinterpret_cast<Node * *>(&(parser_.place_)); 
    parser_.placeSet_.clear();
    parser_.placeSetType_ = true;
  }
    break;

  case 120:

/* Line 678 of lalr1.cc  */
#line 554 "parser-owfn.yy"
    { 
    parser_.source_ = reinterpret_cast<Node * *>(&(parser_.transition_));
    parser_.target_ = reinterpret_cast<Node * *>(&(parser_.place_)); 
    parser_.placeSet_.clear();
    parser_.placeSetType_ = false;
  }
    break;

  case 122:

/* Line 678 of lalr1.cc  */
#line 564 "parser-owfn.yy"
    { (yyval.yt_int) = 0;  }
    break;

  case 123:

/* Line 678 of lalr1.cc  */
#line 565 "parser-owfn.yy"
    { (yyval.yt_int) = (yysemantic_stack_[(3) - (2)].yt_int); }
    break;

  case 126:

/* Line 678 of lalr1.cc  */
#line 575 "parser-owfn.yy"
    {
    parser_.check(parser_.transition_->getPetriNet().isRoleSpecified(parser_.nodeName_.str()), "role has not been specified");
    parser_.transition_->addRole(parser_.nodeName_.str());
  }
    break;

  case 127:

/* Line 678 of lalr1.cc  */
#line 580 "parser-owfn.yy"
    {
    parser_.check(parser_.transition_->getPetriNet().isRoleSpecified(parser_.nodeName_.str()), "role has not been specified");
    parser_.transition_->addRole(parser_.nodeName_.str());
  }
    break;

  case 131:

/* Line 678 of lalr1.cc  */
#line 594 "parser-owfn.yy"
    {
    parser_.place_ = parser_.places_[parser_.nodeName_.str()];
    parser_.label_ = parser_.labels_[parser_.nodeName_.str()];
    parser_.check(!((parser_.place_ == NULL) && (parser_.label_ == NULL)), "unknown place");
    parser_.check(parser_.placeSet_.find(parser_.nodeName_.str()) == parser_.placeSet_.end(), parser_.placeSetType_ ? "place already in preset" : "place already in postset");
    parser_.check(!(parser_.placeSetType_ && (parser_.label_ != NULL) && (parser_.label_->getType() != Label::INPUT)), "can only consume from places and input labels");
    parser_.check(!(!parser_.placeSetType_ && (parser_.label_ != NULL) && (parser_.label_->getType() != Label::OUTPUT)), "can only produce to places and output labels");

    if(parser_.place_ != NULL)
    {
      parser_.net_.createArc(**(parser_.source_), **(parser_.target_), (yysemantic_stack_[(3) - (3)].yt_int));
    }
    else
    {
      parser_.transition_->addLabel(*(parser_.label_), (yysemantic_stack_[(3) - (3)].yt_int));
    }

    parser_.placeSet_.insert(parser_.nodeName_.str());
  }
    break;

  case 132:

/* Line 678 of lalr1.cc  */
#line 614 "parser-owfn.yy"
    {
    parser_.place_ = parser_.places_[parser_.nodeName_.str()];
    parser_.label_ = parser_.labels_[parser_.nodeName_.str()];
    parser_.check(!((parser_.place_ == NULL) && (parser_.label_ == NULL)), "unknown place");
    parser_.check(parser_.placeSet_.find(parser_.nodeName_.str()) == parser_.placeSet_.end(), parser_.placeSetType_ ? "place already in preset" : "place already in postset");
    parser_.check(!(parser_.placeSetType_ && (parser_.label_ != NULL) && (parser_.label_->getType() != Label::INPUT)), "can only consume from places and input labels");
    parser_.check(!(!parser_.placeSetType_ && (parser_.label_ != NULL) && (parser_.label_->getType() != Label::OUTPUT)), "can only produce to places and output labels");

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

  case 136:

/* Line 678 of lalr1.cc  */
#line 643 "parser-owfn.yy"
    {
    parser_.label_ = parser_.labels_[parser_.nodeName_.str()];
    parser_.check(parser_.label_ != NULL, "unknown label");
    parser_.check(parser_.label_->getType() == Label::SYNCHRONOUS, "can only synchronize with synchronous labels");
    parser_.transition_->addLabel(*(parser_.label_));
  }
    break;

  case 137:

/* Line 678 of lalr1.cc  */
#line 650 "parser-owfn.yy"
    {
    parser_.label_ = parser_.labels_[parser_.nodeName_.str()];
    parser_.check(parser_.label_ != NULL, "unknown label");
    parser_.check(parser_.label_->getType() == Label::SYNCHRONOUS, "can only synchronize with synchronous labels");
    parser_.transition_->addLabel(*(parser_.label_));
  }
    break;

  case 140:

/* Line 678 of lalr1.cc  */
#line 665 "parser-owfn.yy"
    { parser_.constrains_[parser_.transition_].insert(parser_.nodeName_.str()); }
    break;

  case 141:

/* Line 678 of lalr1.cc  */
#line 667 "parser-owfn.yy"
    { parser_.constrains_[parser_.transition_].insert(parser_.nodeName_.str()); }
    break;



/* Line 678 of lalr1.cc  */
#line 1112 "parser-owfn.cc"
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
  const short int BisonParser::yypact_ninf_ = -145;
  const short int
  BisonParser::yypact_[] =
  {
        16,    -9,     3,    28,    37,   -13,  -145,   113,    32,   -24,
     -13,    22,    50,    40,  -145,  -145,  -145,  -145,  -145,  -145,
      78,  -145,  -145,  -145,   -13,  -145,  -145,  -145,  -145,  -145,
    -145,   -13,    70,  -145,    65,  -145,    82,    15,   -13,  -145,
      78,  -145,  -145,  -145,  -145,  -145,  -145,   -13,    96,   124,
     123,    78,    78,    78,  -145,    54,  -145,  -145,  -145,  -145,
     128,   -13,   -13,  -145,   129,   -13,   130,   132,   104,  -145,
     -13,  -145,  -145,   155,     3,   133,   -13,   134,   135,   -13,
    -145,   124,   108,  -145,  -145,    82,  -145,   137,     3,   136,
     -13,  -145,   142,  -145,   158,     3,  -145,   139,     3,     3,
    -145,   155,   125,   126,   138,   144,   128,   -13,  -145,  -145,
    -145,    91,   140,  -145,  -145,  -145,   -13,   128,   -13,   158,
     141,   127,    71,  -145,  -145,  -145,   107,   -13,  -145,  -145,
      56,  -145,  -145,   146,   159,   139,  -145,  -145,  -145,   108,
     108,   108,  -145,  -145,   -13,  -145,   110,   -13,  -145,  -145,
    -145,  -145,    77,    77,    79,    33,  -145,   -13,   166,   128,
    -145,  -145,  -145,   108,   108,  -145,  -145,   150,   149,    89,
    -145,   148,   151,   152,   153,   154,   157,  -145,    42,    77,
    -145,   111,  -145,  -145,  -145,  -145,     7,  -145,  -145,  -145,
    -145,  -145,  -145,  -145,  -145,  -145,  -145,  -145,  -145,   -13,
    -145,   -13,   150,  -145,   162,   114,  -145,   160,   -13,   169,
    -145,  -145,  -145,   -13,   115,   184,   -13,   188,  -145,   119,
     -13,  -145,   -13,  -145,  -145,   120,  -145,   -13,  -145,  -145
  };

  /* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
     doesn't specify something else to do.  Zero means the default is an
     error.  */
  const unsigned char
  BisonParser::yydefact_[] =
  {
         0,    13,    68,     0,     0,     0,     5,     8,    10,     0,
      58,     0,    76,    66,     1,    84,   116,     3,     4,    46,
      28,    20,    22,    24,     0,    19,    14,    15,    16,    17,
      18,     0,     9,    29,     0,    59,    56,    28,     0,     6,
      28,    71,    72,    73,    74,    69,    70,    87,     2,    50,
       0,    28,    28,    28,    64,     0,    48,    11,    12,    30,
      36,     0,    58,    67,     0,    77,     0,    91,     0,    88,
       0,   117,    51,    52,     7,     0,    31,     0,     0,     0,
      63,    50,    38,    60,    61,    57,    78,     0,    75,     0,
       0,    85,   122,    53,    54,    21,    32,    26,    23,    25,
      65,    52,     0,     0,     0,     0,    36,     0,    80,    90,
      89,     0,     0,   118,    55,    47,    31,    36,     0,    54,
       0,     0,     0,    37,    62,    82,     0,     0,    92,    97,
       0,    86,    94,     0,   124,    27,    33,    34,    49,    38,
      38,    38,    42,    44,     0,    79,     0,    87,   103,   101,
     102,    98,     0,     0,     0,     0,   123,     0,     0,    36,
      39,    40,    41,    38,    38,    83,    81,    95,     0,     0,
     104,     0,     0,     0,     0,     0,     0,    99,     0,     0,
     126,     0,   119,    35,    43,    45,    87,   100,   115,   114,
     112,   113,   111,   110,   107,   108,   109,   106,   105,     0,
     125,   128,    96,   127,   132,     0,   129,     0,     0,     0,
     131,   130,   120,   128,     0,   133,   135,   138,   136,     0,
       0,   121,     0,   134,   140,     0,   137,     0,   139,   141
  };

  /* YYPGOTO[NTERM-NUM].  */
  const short int
  BisonParser::yypgoto_[] =
  {
      -145,  -145,    -5,  -145,   168,  -145,  -145,  -145,  -145,    76,
    -145,    88,  -145,    90,  -145,   109,   -36,    85,  -145,  -145,
    -104,  -110,  -145,  -145,  -145,  -145,  -145,   121,   103,    86,
      -8,   145,  -145,  -145,   -10,  -145,  -145,  -145,  -145,  -145,
    -145,  -145,  -145,  -145,    83,  -145,  -145,  -145,  -143,   116,
    -145,  -145,  -145,  -145,  -144,  -145,  -145,  -145,  -145,  -145,
    -145,  -145,  -145,    -4,     4,  -145,  -145,  -145,  -145
  };

  /* YYDEFGOTO[NTERM-NUM].  */
  const short int
  BisonParser::yydefgoto_[] =
  {
        -1,     3,   154,     4,    25,     6,    32,     7,    26,    72,
      51,    93,    52,   114,    53,    75,    10,    97,   117,   159,
      83,   105,   163,   164,     8,    49,    81,    73,    94,   115,
      11,    36,    60,   106,    30,    55,    12,    13,    45,    46,
      39,    65,   107,   127,   126,    16,    47,   111,    68,    69,
     131,   147,   168,   132,   155,    48,    71,   134,   201,   213,
     113,   158,   181,   205,   206,   217,   219,   221,   225
  };

  /* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule which
     number is the opposite.  If zero, do what YYDEFACT says.  */
  const signed char BisonParser::yytable_ninf_ = -94;
  const short int
  BisonParser::yytable_[] =
  {
        19,    62,   124,    44,   167,    35,     9,   -93,   169,   170,
       5,    33,    50,   136,    34,    76,    76,    76,     9,    54,
       1,     2,    58,   -93,    17,    18,    56,    63,    14,   160,
     161,   162,    66,    64,   197,   198,    24,   -28,    62,   -28,
     -28,   -28,    67,   202,    17,    18,    40,    21,    22,    15,
      23,    31,    62,   184,   185,   183,    84,    35,    37,   116,
      87,    24,   116,   116,   148,    92,   194,   195,   196,   177,
      38,    96,   149,   150,   100,    20,   178,   179,   148,    17,
      18,     9,   152,    27,   153,    67,   149,   150,    79,    41,
      80,    24,   151,    17,    18,    28,   152,    29,   153,   148,
      59,    42,   125,    43,   128,   129,   130,   149,   150,   142,
     143,    96,    70,   137,    17,    18,    61,   152,    20,   153,
      21,    22,   125,    23,   171,   172,   173,   174,   175,   176,
     187,    21,   178,   179,    24,   102,   103,   104,    90,   165,
      91,   144,    67,   145,   144,   199,   166,   200,   208,   208,
     209,   215,   180,   222,   227,   223,   228,   140,   141,    74,
      82,    77,    78,    22,    86,   112,    88,    89,    23,    95,
      98,    99,   108,   118,   109,   120,   121,   123,   133,   139,
     157,    67,   156,   182,    90,   186,   188,   212,   122,   189,
     190,   191,   192,   216,   203,   193,   204,   207,   210,   220,
      57,   135,   101,   204,   119,   138,   110,    85,   204,   214,
     146,   218,   211,     0,     0,   224,     0,   226,     0,     0,
       0,     0,   229
  };

  /* YYCHECK.  */
  const short int
  BisonParser::yycheck_[] =
  {
         5,    37,   106,    13,   147,    10,     3,     0,   152,   153,
      19,    35,    20,   117,    38,    51,    52,    53,     3,    24,
       4,     5,    32,    16,    37,    38,    31,    37,     0,   139,
     140,   141,    40,    38,   178,   179,    21,    34,    74,    36,
      37,    38,    47,   186,    37,    38,     6,     7,     8,    12,
      10,    19,    88,   163,   164,   159,    61,    62,    36,    95,
      65,    21,    98,    99,    22,    70,    24,    25,    26,    36,
      20,    76,    30,    31,    79,     5,    43,    44,    22,    37,
      38,     3,    40,     7,    42,    90,    30,    31,    34,    13,
      36,    21,    36,    37,    38,     7,    40,     7,    42,    22,
      35,    13,   107,    13,    13,    14,    15,    30,    31,    38,
      39,   116,    16,   118,    37,    38,    34,    40,     5,    42,
       7,     8,   127,    10,    45,    46,    47,    48,    49,    50,
      41,     7,    43,    44,    21,    27,    28,    29,    34,   144,
      36,    34,   147,    36,    34,    34,    36,    36,    34,    34,
      36,    36,   157,    34,    34,    36,    36,    30,    31,    36,
      32,    52,    53,     8,    35,    23,    36,    35,    10,    36,
      36,    36,    35,    34,    38,    50,    50,    33,    38,    38,
      21,   186,    36,    17,    34,    36,    38,    18,    50,    38,
      38,    38,    38,     9,   199,    38,   201,    35,    38,    11,
      32,   116,    81,   208,   101,   119,    90,    62,   213,   213,
     127,   216,   208,    -1,    -1,   220,    -1,   222,    -1,    -1,
      -1,    -1,   227
  };

  /* STOS_[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
  const unsigned char
  BisonParser::yystos_[] =
  {
         0,     4,     5,    52,    54,    19,    56,    58,    75,     3,
      67,    81,    87,    88,     0,    12,    96,    37,    38,    53,
       5,     7,     8,    10,    21,    55,    59,    60,    62,    64,
      85,    19,    57,    35,    38,    53,    82,    36,    20,    91,
       6,    60,    62,    64,    85,    89,    90,    97,   106,    76,
      81,    61,    63,    65,    53,    86,    53,    55,    85,    35,
      83,    34,    67,    85,    53,    92,    81,    53,    99,   100,
      16,   107,    60,    78,    36,    66,    67,    66,    66,    34,
      36,    77,    32,    71,    53,    82,    35,    53,    36,    35,
      34,    36,    53,    62,    79,    36,    53,    68,    36,    36,
      53,    78,    27,    28,    29,    72,    84,    93,    35,    38,
     100,    98,    23,   111,    64,    80,    67,    69,    34,    79,
      50,    50,    50,    33,    71,    53,    95,    94,    13,    14,
      15,   101,   104,    38,   108,    68,    71,    53,    80,    38,
      30,    31,    38,    39,    34,    36,    95,   102,    22,    30,
      31,    36,    40,    42,    53,   105,    36,    21,   112,    70,
      72,    72,    72,    73,    74,    53,    36,    99,   103,   105,
     105,    45,    46,    47,    48,    49,    50,    36,    43,    44,
      53,   113,    17,    71,    72,    72,    36,    41,    38,    38,
      38,    38,    38,    38,    24,    25,    26,   105,   105,    34,
      36,   109,    99,    53,    53,   114,   115,    35,    34,    36,
      38,   115,    18,   110,   114,    36,     9,   116,    53,   117,
      11,   118,    34,    36,    53,   119,    53,    34,    36,    53
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
         0,    51,    52,    53,    53,    54,    54,    55,    56,    56,
      57,    57,    57,    58,    58,    59,    59,    59,    59,    59,
      61,    60,    63,    62,    65,    64,    66,    66,    67,    67,
      67,    68,    69,    68,    70,    68,    71,    71,    72,    72,
      72,    72,    73,    72,    74,    72,    76,    75,    77,    75,
      78,    78,    79,    79,    80,    80,    81,    81,    82,    83,
      82,    84,    82,    85,    86,    86,    87,    87,    88,    88,
      89,    89,    89,    89,    89,    90,    91,    91,    93,    92,
      94,    92,    95,    95,    97,    98,    96,    99,    99,    99,
     100,   100,   102,   101,   101,   103,   103,   104,   104,   104,
     105,   105,   105,   105,   105,   105,   105,   105,   105,   105,
     105,   105,   105,   105,   105,   105,   106,   106,   108,   109,
     110,   107,   111,   111,   112,   112,   113,   113,   114,   114,
     114,   115,   115,   116,   116,   117,   117,   117,   118,   118,
     119,   119
  };

  /* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
  const unsigned char
  BisonParser::yyr2_[] =
  {
         0,     2,     3,     1,     1,     2,     3,     3,     1,     2,
       0,     2,     2,     0,     2,     1,     1,     1,     1,     1,
       0,     4,     0,     4,     0,     4,     2,     4,     0,     2,
       3,     0,     0,     3,     0,     5,     0,     3,     0,     4,
       4,     4,     0,     5,     0,     5,     0,     6,     0,     7,
       0,     1,     0,     1,     0,     1,     2,     4,     0,     0,
       3,     0,     5,     3,     1,     3,     1,     3,     0,     2,
       1,     1,     1,     1,     1,     3,     0,     2,     0,     5,
       0,     6,     1,     3,     0,     0,     6,     0,     1,     3,
       3,     1,     0,     4,     1,     1,     3,     1,     2,     3,
       3,     1,     1,     1,     2,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     0,     2,     0,     0,
       0,    15,     0,     3,     0,     3,     1,     3,     0,     1,
       3,     3,     1,     0,     3,     0,     1,     3,     0,     3,
       1,     3
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
  "marking", "final", "$@16", "finalmarkings", "condition", "formula",
  "transitions", "transition", "$@17", "$@18", "$@19", "transition_cost",
  "transition_roles", "transition_role_names", "arcs", "arc",
  "synchronize", "synchronize_labels", "constrain", "constrain_labels", 0
  };
#endif

#if YYDEBUG
  /* YYRHS -- A `-1'-separated list of the rules' RHS.  */
  const BisonParser::rhs_number_type
  BisonParser::yyrhs_[] =
  {
        52,     0,    -1,    54,    96,   106,    -1,    37,    -1,    38,
      -1,     4,    56,    -1,     5,    87,    91,    -1,     5,    81,
      36,    -1,    58,    -1,    75,    57,    -1,    -1,    57,    55,
      -1,    57,    85,    -1,    -1,    58,    59,    -1,    60,    -1,
      62,    -1,    64,    -1,    85,    -1,    55,    -1,    -1,     7,
      61,    66,    36,    -1,    -1,     8,    63,    66,    36,    -1,
      -1,    10,    65,    66,    36,    -1,    67,    68,    -1,    66,
      36,    67,    68,    -1,    -1,     3,    35,    -1,     3,    38,
      35,    -1,    -1,    -1,    53,    69,    71,    -1,    -1,    68,
      34,    53,    70,    71,    -1,    -1,    32,    72,    33,    -1,
      -1,    27,    50,    38,    72,    -1,    28,    50,    30,    72,
      -1,    28,    50,    31,    72,    -1,    -1,    29,    50,    38,
      73,    72,    -1,    -1,    29,    50,    39,    74,    72,    -1,
      -1,    19,    53,    76,    78,    79,    80,    -1,    -1,    75,
      19,    53,    77,    78,    79,    80,    -1,    -1,    60,    -1,
      -1,    62,    -1,    -1,    64,    -1,    67,    82,    -1,    81,
      36,    67,    82,    -1,    -1,    -1,    53,    83,    71,    -1,
      -1,    82,    34,    53,    84,    71,    -1,    21,    86,    36,
      -1,    53,    -1,    86,    34,    53,    -1,    88,    -1,    81,
      36,    85,    -1,    -1,    88,    89,    -1,    90,    -1,    60,
      -1,    62,    -1,    64,    -1,    85,    -1,     6,    81,    36,
      -1,    -1,    20,    92,    -1,    -1,    53,    35,    93,    95,
      36,    -1,    -1,    92,    53,    35,    94,    95,    36,    -1,
      53,    -1,    95,    34,    53,    -1,    -1,    -1,    12,    97,
      99,    36,    98,   101,    -1,    -1,   100,    -1,    99,    34,
     100,    -1,    53,    35,    38,    -1,    53,    -1,    -1,    13,
     102,   103,    36,    -1,   104,    -1,    99,    -1,   103,    36,
      99,    -1,    14,    -1,    15,    36,    -1,    15,   105,    36,
      -1,    40,   105,    41,    -1,    30,    -1,    31,    -1,    22,
      -1,    42,   105,    -1,   105,    44,   105,    -1,   105,    43,
     105,    -1,   105,    43,    24,    -1,   105,    43,    25,    -1,
     105,    43,    26,    -1,    53,    50,    38,    -1,    53,    49,
      38,    -1,    53,    47,    38,    -1,    53,    48,    38,    -1,
      53,    46,    38,    -1,    53,    45,    38,    -1,    -1,   106,
     107,    -1,    -1,    -1,    -1,    16,    53,   111,   108,   112,
      17,   109,   114,    36,    18,   110,   114,    36,   116,   118,
      -1,    -1,    23,    38,    36,    -1,    -1,    21,   113,    36,
      -1,    53,    -1,   113,    34,    53,    -1,    -1,   115,    -1,
     114,    34,   115,    -1,    53,    35,    38,    -1,    53,    -1,
      -1,     9,   117,    36,    -1,    -1,    53,    -1,   117,    34,
      53,    -1,    -1,    11,   119,    36,    -1,    53,    -1,   119,
      34,    53,    -1
  };

  /* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
     YYRHS.  */
  const unsigned short int
  BisonParser::yyprhs_[] =
  {
         0,     0,     3,     7,     9,    11,    14,    18,    22,    24,
      27,    28,    31,    34,    35,    38,    40,    42,    44,    46,
      48,    49,    54,    55,    60,    61,    66,    69,    74,    75,
      78,    82,    83,    84,    88,    89,    95,    96,   100,   101,
     106,   111,   116,   117,   123,   124,   130,   131,   138,   139,
     147,   148,   150,   151,   153,   154,   156,   159,   164,   165,
     166,   170,   171,   177,   181,   183,   187,   189,   193,   194,
     197,   199,   201,   203,   205,   207,   211,   212,   215,   216,
     222,   223,   230,   232,   236,   237,   238,   245,   246,   248,
     252,   256,   258,   259,   264,   266,   268,   272,   274,   277,
     281,   285,   287,   289,   291,   294,   298,   302,   306,   310,
     314,   318,   322,   326,   330,   334,   338,   339,   342,   343,
     344,   345,   361,   362,   366,   367,   371,   373,   377,   378,
     380,   384,   388,   390,   391,   395,   396,   398,   402,   403,
     407,   409
  };

  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
  const unsigned short int
  BisonParser::yyrline_[] =
  {
         0,    95,    95,   102,   111,   128,   129,   133,   137,   138,
     141,   143,   144,   147,   149,   153,   154,   155,   156,   157,
     161,   161,   165,   165,   169,   169,   174,   175,   178,   180,
     181,   184,   187,   186,   195,   194,   204,   206,   209,   211,
     212,   213,   215,   214,   223,   222,   234,   233,   237,   236,
     241,   243,   246,   248,   251,   253,   257,   258,   261,   264,
     263,   272,   271,   283,   287,   289,   295,   296,   299,   301,
     305,   306,   307,   308,   309,   313,   317,   319,   324,   323,
     327,   326,   332,   342,   361,   362,   361,   365,   367,   368,
     372,   386,   404,   403,   410,   423,   429,   438,   439,   440,
     444,   445,   446,   447,   452,   457,   463,   469,   474,   478,
     482,   489,   496,   503,   510,   517,   532,   534,   539,   547,
     554,   538,   564,   565,   568,   570,   574,   579,   586,   588,
     589,   593,   613,   635,   637,   640,   642,   649,   658,   660,
     664,   666
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
  const int BisonParser::yylast_ = 222;
  const int BisonParser::yynnts_ = 69;
  const int BisonParser::yyempty_ = -2;
  const int BisonParser::yyfinal_ = 14;
  const int BisonParser::yyterror_ = 1;
  const int BisonParser::yyerrcode_ = 256;
  const int BisonParser::yyntokens_ = 51;

  const unsigned int BisonParser::yyuser_token_number_max_ = 305;
  const BisonParser::token_number_type BisonParser::yyundef_token_ = 2;


/* Line 1054 of lalr1.cc  */
#line 11 "parser-owfn.yy"
} } } } // pnapi::parser::owfn::yy

/* Line 1054 of lalr1.cc  */
#line 1771 "parser-owfn.cc"


