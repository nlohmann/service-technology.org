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

  case 3:

/* Line 677 of lalr1.cc  */
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

/* Line 677 of lalr1.cc  */
#line 112 "parser-owfn.yy"
    { 
    // clear stringstream
    parser_.nodeName_.str("");
    parser_.nodeName_.clear();

    parser_.nodeName_ << (yysemantic_stack_[(1) - (1)].yt_int); 
  }
    break;

  case 10:

/* Line 677 of lalr1.cc  */
#line 139 "parser-owfn.yy"
    { parser_.labelType_ = Label::INPUT; }
    break;

  case 13:

/* Line 677 of lalr1.cc  */
#line 144 "parser-owfn.yy"
    { parser_.labelType_ = Label::OUTPUT; }
    break;

  case 16:

/* Line 677 of lalr1.cc  */
#line 149 "parser-owfn.yy"
    { parser_.labelType_ = Label::SYNCHRONOUS; }
    break;

  case 18:

/* Line 677 of lalr1.cc  */
#line 154 "parser-owfn.yy"
    { parser_.capacity_ = 0; }
    break;

  case 19:

/* Line 677 of lalr1.cc  */
#line 155 "parser-owfn.yy"
    { parser_.capacity_ = 0; }
    break;

  case 21:

/* Line 677 of lalr1.cc  */
#line 160 "parser-owfn.yy"
    { parser_.capacity_ = 1; }
    break;

  case 22:

/* Line 677 of lalr1.cc  */
#line 161 "parser-owfn.yy"
    { parser_.capacity_ = (yysemantic_stack_[(3) - (2)].yt_int); }
    break;

  case 24:

/* Line 677 of lalr1.cc  */
#line 167 "parser-owfn.yy"
    {
    parser_.check(parser_.labels_[parser_.nodeName_.str()] == NULL, "node name already used");
    parser_.check(parser_.places_[parser_.nodeName_.str()] == NULL, "node name already used");
    parser_.label_ = &(parser_.port_->addLabel(parser_.nodeName_.str(), parser_.labelType_));
    parser_.labels_[parser_.nodeName_.str()] = parser_.label_;
  }
    break;

  case 26:

/* Line 677 of lalr1.cc  */
#line 175 "parser-owfn.yy"
    {
    parser_.check(parser_.labels_[parser_.nodeName_.str()] == NULL, "node name already used");
    parser_.check(parser_.places_[parser_.nodeName_.str()] == NULL, "node name already used");
    parser_.label_ = &(parser_.port_->addLabel(parser_.nodeName_.str(), parser_.labelType_));
    parser_.labels_[parser_.nodeName_.str()] = parser_.label_;
  }
    break;

  case 34:

/* Line 677 of lalr1.cc  */
#line 195 "parser-owfn.yy"
    {
    if(parser_.place_ != NULL) // is only NULL when parsing interface
    {
      parser_.place_->setMaxOccurrence((yysemantic_stack_[(3) - (3)].yt_int));
    }
  }
    break;

  case 36:

/* Line 677 of lalr1.cc  */
#line 203 "parser-owfn.yy"
    {
    if(parser_.place_ != NULL) // is only NULL when parsing interface
    {
      parser_.place_->setMaxOccurrence((yysemantic_stack_[(3) - (3)].yt_int));
    }
  }
    break;

  case 38:

/* Line 677 of lalr1.cc  */
#line 214 "parser-owfn.yy"
    { parser_.port_ = &(parser_.net_.getInterface().addPort(parser_.nodeName_.str())); }
    break;

  case 40:

/* Line 677 of lalr1.cc  */
#line 217 "parser-owfn.yy"
    { parser_.port_ = &(parser_.net_.getInterface().addPort(parser_.nodeName_.str())); }
    break;

  case 42:

/* Line 677 of lalr1.cc  */
#line 223 "parser-owfn.yy"
    { parser_.capacity_ = 0; }
    break;

  case 43:

/* Line 677 of lalr1.cc  */
#line 224 "parser-owfn.yy"
    { parser_.capacity_ = 0; }
    break;

  case 45:

/* Line 677 of lalr1.cc  */
#line 230 "parser-owfn.yy"
    {
    parser_.check(parser_.labels_[parser_.nodeName_.str()] == NULL, "node name already used");
    parser_.check(parser_.places_[parser_.nodeName_.str()] == NULL, "node name already used");
    parser_.place_ = &(parser_.net_.createPlace(parser_.nodeName_.str(), 0, parser_.capacity_));
    parser_.places_[parser_.nodeName_.str()] = parser_.place_;
  }
    break;

  case 47:

/* Line 677 of lalr1.cc  */
#line 238 "parser-owfn.yy"
    {
    parser_.check(parser_.labels_[parser_.nodeName_.str()] == NULL, "node name already used");
    parser_.check(parser_.places_[parser_.nodeName_.str()] == NULL, "node name already used");
    parser_.place_ = &(parser_.net_.createPlace(parser_.nodeName_.str(), 0, parser_.capacity_));
    parser_.places_[parser_.nodeName_.str()] = parser_.place_;
  }
    break;

  case 51:

/* Line 677 of lalr1.cc  */
#line 255 "parser-owfn.yy"
    { parser_.net_.addRole(parser_.nodeName_.str()); }
    break;

  case 52:

/* Line 677 of lalr1.cc  */
#line 257 "parser-owfn.yy"
    { parser_.net_.addRole(parser_.nodeName_.str()); }
    break;

  case 59:

/* Line 677 of lalr1.cc  */
#line 279 "parser-owfn.yy"
    { parser_.port_ = &(parser_.net_.getInterface().addPort(parser_.nodeName_.str())); }
    break;

  case 61:

/* Line 677 of lalr1.cc  */
#line 282 "parser-owfn.yy"
    { parser_.port_ = &(parser_.net_.getInterface().addPort(parser_.nodeName_.str())); }
    break;

  case 63:

/* Line 677 of lalr1.cc  */
#line 288 "parser-owfn.yy"
    {
    Label * l = parser_.labels_[parser_.nodeName_.str()];
    parser_.check(l != NULL, "unknown interface label");
    parser_.check((l->getPort().getName() == ""), "interface label already assigned to port '" + l->getPort().getName() +"'");
     
    Label::Type t = l->getType();
    parser_.net_.getInterface().getPort()->removeLabel(parser_.nodeName_.str());
    parser_.labels_[parser_.nodeName_.str()] = &(parser_.port_->addLabel(parser_.nodeName_.str(), t));
  }
    break;

  case 64:

/* Line 677 of lalr1.cc  */
#line 298 "parser-owfn.yy"
    {
    Label * l = parser_.labels_[parser_.nodeName_.str()];
    parser_.check(l != NULL, "unknown interface label");
    parser_.check((l->getPort().getName() == ""), "interface label already assigned to port '" + l->getPort().getName() +"'");
     
    Label::Type t = l->getType();
    parser_.net_.getInterface().getPort()->removeLabel(parser_.nodeName_.str());
    parser_.labels_[parser_.nodeName_.str()] = &(parser_.port_->addLabel(parser_.nodeName_.str(), t));
  }
    break;

  case 65:

/* Line 677 of lalr1.cc  */
#line 316 "parser-owfn.yy"
    { parser_.markInitial_ = true; }
    break;

  case 66:

/* Line 677 of lalr1.cc  */
#line 317 "parser-owfn.yy"
    { parser_.markInitial_ = false; }
    break;

  case 71:

/* Line 677 of lalr1.cc  */
#line 328 "parser-owfn.yy"
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

  case 72:

/* Line 677 of lalr1.cc  */
#line 342 "parser-owfn.yy"
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

  case 73:

/* Line 677 of lalr1.cc  */
#line 359 "parser-owfn.yy"
    { parser_.finalMarking_ = new Marking(parser_.net_, true); }
    break;

  case 74:

/* Line 677 of lalr1.cc  */
#line 361 "parser-owfn.yy"
    {
    delete parser_.finalMarking_;
    parser_.finalMarking_ = NULL;
  }
    break;

  case 75:

/* Line 677 of lalr1.cc  */
#line 366 "parser-owfn.yy"
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

  case 76:

/* Line 677 of lalr1.cc  */
#line 379 "parser-owfn.yy"
    {
    parser_.net_.getFinalCondition().addMarking(*(parser_.finalMarking_));
    delete parser_.finalMarking_;
    parser_.finalMarking_ = new Marking(parser_.net_, true);
  }
    break;

  case 77:

/* Line 677 of lalr1.cc  */
#line 385 "parser-owfn.yy"
    {
    parser_.net_.getFinalCondition().addMarking(*(parser_.finalMarking_));
    delete parser_.finalMarking_;
    parser_.finalMarking_ = new Marking(parser_.net_, true);
  }
    break;

  case 78:

/* Line 677 of lalr1.cc  */
#line 393 "parser-owfn.yy"
    { (yyval.yt_formula) = new formula::FormulaTrue(); }
    break;

  case 79:

/* Line 677 of lalr1.cc  */
#line 394 "parser-owfn.yy"
    { (yyval.yt_formula) = new formula::FormulaFalse(); }
    break;

  case 80:

/* Line 677 of lalr1.cc  */
#line 395 "parser-owfn.yy"
    { (yyval.yt_formula) = (yysemantic_stack_[(3) - (2)].yt_formula); }
    break;

  case 81:

/* Line 677 of lalr1.cc  */
#line 399 "parser-owfn.yy"
    { (yyval.yt_formula) = (yysemantic_stack_[(3) - (2)].yt_formula); }
    break;

  case 82:

/* Line 677 of lalr1.cc  */
#line 400 "parser-owfn.yy"
    { (yyval.yt_formula) = new formula::FormulaTrue(); }
    break;

  case 83:

/* Line 677 of lalr1.cc  */
#line 401 "parser-owfn.yy"
    { (yyval.yt_formula) = new formula::FormulaFalse(); }
    break;

  case 84:

/* Line 677 of lalr1.cc  */
#line 403 "parser-owfn.yy"
    { 
    parser_.wildcardGiven_ = true;
    (yyval.yt_formula) = new formula::FormulaTrue();
  }
    break;

  case 85:

/* Line 677 of lalr1.cc  */
#line 408 "parser-owfn.yy"
    { 
    (yyval.yt_formula) = new formula::Negation(*(yysemantic_stack_[(2) - (2)].yt_formula));
    delete (yysemantic_stack_[(2) - (2)].yt_formula);
  }
    break;

  case 86:

/* Line 677 of lalr1.cc  */
#line 413 "parser-owfn.yy"
    {
    (yyval.yt_formula) = new formula::Disjunction(*(yysemantic_stack_[(3) - (1)].yt_formula), *(yysemantic_stack_[(3) - (3)].yt_formula));
    delete (yysemantic_stack_[(3) - (1)].yt_formula);
    delete (yysemantic_stack_[(3) - (3)].yt_formula);
  }
    break;

  case 87:

/* Line 677 of lalr1.cc  */
#line 419 "parser-owfn.yy"
    {
    (yyval.yt_formula) = new formula::Conjunction(*(yysemantic_stack_[(3) - (1)].yt_formula), *(yysemantic_stack_[(3) - (3)].yt_formula));
    delete (yysemantic_stack_[(3) - (1)].yt_formula);
    delete (yysemantic_stack_[(3) - (3)].yt_formula);
  }
    break;

  case 88:

/* Line 677 of lalr1.cc  */
#line 425 "parser-owfn.yy"
    {
    parser_.wildcardGiven_ = true;
    (yyval.yt_formula) = (yysemantic_stack_[(3) - (1)].yt_formula);
  }
    break;

  case 89:

/* Line 677 of lalr1.cc  */
#line 430 "parser-owfn.yy"
    {
    (yyval.yt_formula) = (yysemantic_stack_[(3) - (1)].yt_formula); // obsolete; kept due to compatibility
  }
    break;

  case 90:

/* Line 677 of lalr1.cc  */
#line 434 "parser-owfn.yy"
    {
    (yyval.yt_formula) = (yysemantic_stack_[(3) - (1)].yt_formula); // obsolete; kept due to compatibility
  }
    break;

  case 91:

/* Line 677 of lalr1.cc  */
#line 438 "parser-owfn.yy"
    {
    Place * p = parser_.places_[parser_.nodeName_.str()];
    parser_.check(p != NULL, "unknown place");
    
    (yyval.yt_formula) = new formula::FormulaEqual(*p, (yysemantic_stack_[(3) - (3)].yt_int));
  }
    break;

  case 92:

/* Line 677 of lalr1.cc  */
#line 445 "parser-owfn.yy"
    {
    Place * p = parser_.places_[parser_.nodeName_.str()];
    parser_.check(p != NULL, "unknown place");

    (yyval.yt_formula) = new formula::Negation(formula::FormulaEqual(*p, (yysemantic_stack_[(3) - (3)].yt_int)));
  }
    break;

  case 93:

/* Line 677 of lalr1.cc  */
#line 452 "parser-owfn.yy"
    {
    Place * p = parser_.places_[parser_.nodeName_.str()];
    parser_.check(p != NULL, "unknown place");

    (yyval.yt_formula) = new formula::FormulaLess(*p, (yysemantic_stack_[(3) - (3)].yt_int));
  }
    break;

  case 94:

/* Line 677 of lalr1.cc  */
#line 459 "parser-owfn.yy"
    {
    Place * p = parser_.places_[parser_.nodeName_.str()];
    parser_.check(p != NULL, "unknown place");

    (yyval.yt_formula) = new formula::FormulaGreater(*p, (yysemantic_stack_[(3) - (3)].yt_int));
  }
    break;

  case 95:

/* Line 677 of lalr1.cc  */
#line 466 "parser-owfn.yy"
    {
    Place * p = parser_.places_[parser_.nodeName_.str()];
    parser_.check(p != NULL, "unknown place");

    (yyval.yt_formula) = new formula::FormulaGreaterEqual(*p, (yysemantic_stack_[(3) - (3)].yt_int));
  }
    break;

  case 96:

/* Line 677 of lalr1.cc  */
#line 473 "parser-owfn.yy"
    {
    Place * p = parser_.places_[parser_.nodeName_.str()];
    parser_.check(p != NULL, "unknown place");

    (yyval.yt_formula) = new formula::FormulaLessEqual(*p, (yysemantic_stack_[(3) - (3)].yt_int));
  }
    break;

  case 99:

/* Line 677 of lalr1.cc  */
#line 494 "parser-owfn.yy"
    {
    parser_.check(parser_.labels_[parser_.nodeName_.str()] == NULL, "node name already used");
    parser_.check(!(parser_.net_.containsNode(parser_.nodeName_.str())), "node name already used");
    parser_.transition_ = &(parser_.net_.createTransition(parser_.nodeName_.str())); 
    parser_.transition_->setCost((yysemantic_stack_[(3) - (3)].yt_int));
  }
    break;

  case 100:

/* Line 677 of lalr1.cc  */
#line 502 "parser-owfn.yy"
    { 
    parser_.target_ = reinterpret_cast<Node * *>(&(parser_.transition_));
    parser_.source_ = reinterpret_cast<Node * *>(&(parser_.place_)); 
    parser_.placeSet_.clear();
    parser_.placeSetType_ = true;
  }
    break;

  case 101:

/* Line 677 of lalr1.cc  */
#line 509 "parser-owfn.yy"
    { 
    parser_.source_ = reinterpret_cast<Node * *>(&(parser_.transition_));
    parser_.target_ = reinterpret_cast<Node * *>(&(parser_.place_)); 
    parser_.placeSet_.clear();
    parser_.placeSetType_ = false;
  }
    break;

  case 103:

/* Line 677 of lalr1.cc  */
#line 519 "parser-owfn.yy"
    { (yyval.yt_int) = 0;  }
    break;

  case 104:

/* Line 677 of lalr1.cc  */
#line 520 "parser-owfn.yy"
    { (yyval.yt_int) = (yysemantic_stack_[(3) - (2)].yt_int); }
    break;

  case 107:

/* Line 677 of lalr1.cc  */
#line 530 "parser-owfn.yy"
    {
    parser_.check(parser_.transition_->getPetriNet().isRoleSpecified(parser_.nodeName_.str()), "role has not been specified");
    parser_.transition_->addRole(parser_.nodeName_.str());
  }
    break;

  case 108:

/* Line 677 of lalr1.cc  */
#line 535 "parser-owfn.yy"
    {
    parser_.check(parser_.transition_->getPetriNet().isRoleSpecified(parser_.nodeName_.str()), "role has not been specified");
    parser_.transition_->addRole(parser_.nodeName_.str());
  }
    break;

  case 112:

/* Line 677 of lalr1.cc  */
#line 549 "parser-owfn.yy"
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

  case 113:

/* Line 677 of lalr1.cc  */
#line 569 "parser-owfn.yy"
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

  case 117:

/* Line 677 of lalr1.cc  */
#line 598 "parser-owfn.yy"
    {
    parser_.label_ = parser_.labels_[parser_.nodeName_.str()];
    parser_.check(parser_.label_ != NULL, "unknown label");
    parser_.check(parser_.label_->getType() == Label::SYNCHRONOUS, "can only synchronize with synchronous labels");
    parser_.transition_->addLabel(*(parser_.label_));
  }
    break;

  case 118:

/* Line 677 of lalr1.cc  */
#line 605 "parser-owfn.yy"
    {
    parser_.label_ = parser_.labels_[parser_.nodeName_.str()];
    parser_.check(parser_.label_ != NULL, "unknown label");
    parser_.check(parser_.label_->getType() == Label::SYNCHRONOUS, "can only synchronize with synchronous labels");
    parser_.transition_->addLabel(*(parser_.label_));
  }
    break;

  case 121:

/* Line 677 of lalr1.cc  */
#line 620 "parser-owfn.yy"
    { parser_.constrains_[parser_.transition_].insert(parser_.nodeName_.str()); }
    break;

  case 122:

/* Line 677 of lalr1.cc  */
#line 622 "parser-owfn.yy"
    { parser_.constrains_[parser_.transition_].insert(parser_.nodeName_.str()); }
    break;



/* Line 677 of lalr1.cc  */
#line 1110 "parser-owfn.cc"
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
  const short int BisonParser::yypact_ninf_ = -133;
  const short int
  BisonParser::yypact_[] =
  {
        70,     5,    59,     7,    -1,  -133,   106,    20,    27,    45,
      31,    38,   106,    32,    81,   108,  -133,  -133,  -133,    38,
    -133,  -133,  -133,    38,  -133,   115,   106,  -133,   100,   127,
    -133,   130,   121,   106,   137,    27,   106,   149,   131,   106,
     108,   133,    38,  -133,  -133,  -133,  -133,    43,   134,   106,
     106,  -133,    16,   106,  -133,   115,   135,    22,  -133,   106,
    -133,    43,  -133,   138,    27,    51,   139,    38,   108,    71,
    -133,  -133,   130,   106,  -133,   136,   106,  -133,   140,   106,
    -133,   150,   106,   134,   106,   115,  -133,    43,   141,    27,
     124,   126,   132,   146,   134,  -133,  -133,   145,  -133,  -133,
      98,   147,  -133,   138,  -133,  -133,  -133,    43,   115,   148,
     129,   123,  -133,  -133,   106,  -133,  -133,  -133,    92,  -133,
    -133,   151,   163,   134,  -133,    71,    71,    71,  -133,  -133,
    -133,    58,   106,   106,  -133,  -133,  -133,  -133,   109,   109,
      40,   -22,  -133,   106,   171,  -133,  -133,  -133,  -133,    71,
      71,   106,  -133,    84,   155,   156,   -26,  -133,   153,   158,
     159,   161,   162,   165,  -133,    79,   109,  -133,   102,  -133,
    -133,  -133,  -133,  -133,    23,  -133,  -133,  -133,  -133,  -133,
    -133,  -133,  -133,  -133,  -133,  -133,  -133,   106,  -133,   106,
     155,  -133,   166,   114,  -133,   167,   106,   175,  -133,  -133,
    -133,   106,   118,   185,   106,   195,  -133,   119,   106,  -133,
     106,  -133,  -133,   122,  -133,   106,  -133,  -133
  };

  /* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
     doesn't specify something else to do.  Zero means the default is an
     error.  */
  const unsigned char
  BisonParser::yydefact_[] =
  {
         0,     9,    55,     0,     0,    10,     0,     0,    12,     8,
       0,    20,    44,     0,    49,     9,     1,    65,    97,    20,
       3,     4,    38,    20,    13,    15,     0,    21,     0,     0,
      45,    42,    20,     0,    57,    12,    68,     2,     0,    23,
       9,     0,    20,    16,     7,    40,    22,    56,    28,     0,
      44,    51,     0,     0,     6,    15,    72,     0,    69,     0,
      98,    11,    24,    18,    12,    20,     0,    20,     9,    30,
      46,    47,    43,     0,    50,     0,    58,    53,     0,     0,
      66,   103,    23,    28,     0,    15,     5,    14,     0,    12,
       0,     0,     0,     0,    28,    52,    59,     0,    71,    70,
       0,     0,    99,    19,    25,    26,    39,    17,    15,     0,
       0,     0,    29,    48,     0,    61,    73,    78,     0,    67,
      75,     0,   105,    28,    41,    30,    30,    30,    34,    36,
      63,     0,     0,    68,    84,    82,    83,    79,     0,     0,
       0,     0,   104,     0,     0,    27,    31,    32,    33,    30,
      30,     0,    60,     0,    76,     0,     0,    85,     0,     0,
       0,     0,     0,     0,    80,     0,     0,   107,     0,   100,
      35,    37,    64,    62,    68,    81,    96,    95,    93,    94,
      92,    91,    88,    89,    90,    87,    86,     0,   106,   109,
      77,   108,   113,     0,   110,     0,     0,     0,   112,   111,
     101,   109,     0,   114,   116,   119,   117,     0,     0,   102,
       0,   115,   121,     0,   118,     0,   120,   122
  };

  /* YYPGOTO[NTERM-NUM].  */
  const short int
  BisonParser::yypgoto_[] =
  {
      -133,  -133,    -6,  -133,  -133,   -11,  -133,   -30,  -133,   -53,
    -133,   -29,   -16,   125,  -133,  -133,   -75,   -43,  -133,  -133,
    -133,  -133,  -133,    17,   160,  -133,  -133,   143,  -133,  -133,
    -133,  -133,  -133,  -133,  -133,    80,  -133,  -133,  -133,  -132,
     142,  -133,  -133,  -133,  -133,  -129,  -133,  -133,  -133,  -133,
    -133,  -133,  -133,  -133,    10,    18,  -133,  -133,  -133,  -133
  };

  /* YYDEFGOTO[NTERM-NUM].  */
  const short int
  BisonParser::yydefgoto_[] =
  {
        -1,     3,   140,     4,     7,     8,    19,    25,    42,    44,
      67,    38,    12,    63,    83,   123,    70,    93,   149,   150,
       9,    40,    68,    13,    31,    48,    94,    34,    52,    14,
      15,    54,    76,   114,   132,   131,    18,    36,   100,    57,
      58,   119,   133,   155,   120,   141,    37,    60,   122,   189,
     201,   102,   144,   168,   193,   194,   205,   207,   209,   213
  };

  /* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule which
     number is the opposite.  If zero, do what YYDEFACT says.  */
  const signed char BisonParser::yytable_ninf_ = -75;
  const short int
  BisonParser::yytable_[] =
  {
        22,   154,    77,    39,    35,    55,    30,    16,   104,   156,
     157,    17,     5,    66,   164,   175,    50,   165,   166,   113,
      45,   165,   166,   -74,     6,    23,    39,    51,    29,    64,
      56,    50,   106,    62,    85,    24,   185,   186,    88,   -74,
      41,    10,   190,    71,    30,    82,    10,    75,   145,    50,
      73,    39,    74,    81,    10,   124,    79,    89,    80,   108,
      20,    21,    10,   -49,    26,    11,    27,    95,    32,    28,
      97,    82,    33,    56,     1,     2,    62,   -20,   105,   -20,
     -20,   -20,   146,   147,   148,   158,   159,   160,   161,   162,
     163,    82,   151,   -20,   152,   -20,   -20,   -20,    90,    91,
      92,   134,    33,   182,   183,   184,   170,   171,   130,   135,
     136,   116,   117,   118,   134,     5,    20,    21,   151,   138,
     173,   139,   135,   136,    10,    43,   130,    56,   137,    20,
      21,   134,   138,   -54,   139,    46,   187,   167,   188,   135,
     136,   -54,   -54,    20,    21,   172,    20,    21,   196,   138,
     197,   139,   196,   210,   203,   211,   215,    53,   216,   126,
     127,   128,   129,    47,    49,    59,    69,    61,    56,    65,
      78,    96,    84,   101,   109,    87,   110,   107,    98,   112,
     115,   191,   111,   192,   143,   121,   125,   142,   169,    79,
     192,   176,   174,   200,   204,   192,   177,   178,   206,   179,
     180,   195,   212,   181,   214,   198,   208,   103,    86,   217,
      72,   202,   153,     0,   199,     0,     0,     0,     0,     0,
       0,    99
  };

  /* YYCHECK.  */
  const short int
  BisonParser::yycheck_[] =
  {
         6,   133,    55,    19,    15,    35,    12,     0,    83,   138,
     139,    12,     7,    42,    36,    41,    32,    43,    44,    94,
      26,    43,    44,     0,    19,     5,    42,    33,    11,    40,
      36,    47,    85,    39,    64,     8,   165,   166,    67,    16,
      23,     3,   174,    49,    50,    61,     3,    53,   123,    65,
      34,    67,    36,    59,     3,   108,    34,    68,    36,    89,
      37,    38,     3,    12,    19,     6,    35,    73,    36,    38,
      76,    87,    21,    79,     4,     5,    82,    34,    84,    36,
      37,    38,   125,   126,   127,    45,    46,    47,    48,    49,
      50,   107,    34,    34,    36,    36,    37,    38,    27,    28,
      29,    22,    21,    24,    25,    26,   149,   150,   114,    30,
      31,    13,    14,    15,    22,     7,    37,    38,    34,    40,
      36,    42,    30,    31,     3,    10,   132,   133,    36,    37,
      38,    22,    40,    12,    42,    35,    34,   143,    36,    30,
      31,    20,    21,    37,    38,   151,    37,    38,    34,    40,
      36,    42,    34,    34,    36,    36,    34,    20,    36,    30,
      31,    38,    39,    36,    34,    16,    32,    36,   174,    36,
      35,    35,    34,    23,    50,    36,    50,    36,    38,    33,
      35,   187,    50,   189,    21,    38,    38,    36,    17,    34,
     196,    38,    36,    18,     9,   201,    38,    38,   204,    38,
      38,    35,   208,    38,   210,    38,    11,    82,    65,   215,
      50,   201,   132,    -1,   196,    -1,    -1,    -1,    -1,    -1,
      -1,    79
  };

  /* STOS_[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
  const unsigned char
  BisonParser::yystos_[] =
  {
         0,     4,     5,    52,    54,     7,    19,    55,    56,    71,
       3,     6,    63,    74,    80,    81,     0,    12,    87,    57,
      37,    38,    53,     5,     8,    58,    19,    35,    38,    74,
      53,    75,    36,    21,    78,    56,    88,    97,    62,    63,
      72,    74,    59,    10,    60,    53,    35,    36,    76,    34,
      63,    53,    79,    20,    82,    58,    53,    90,    91,    16,
      98,    36,    53,    64,    56,    36,    62,    61,    73,    32,
      67,    53,    75,    34,    36,    53,    83,    60,    35,    34,
      36,    53,    63,    65,    34,    58,    78,    36,    62,    56,
      27,    28,    29,    68,    77,    53,    35,    53,    38,    91,
      89,    23,   102,    64,    67,    53,    60,    36,    58,    50,
      50,    50,    33,    67,    84,    35,    13,    14,    15,    92,
      95,    38,    99,    66,    60,    38,    30,    31,    38,    39,
      53,    86,    85,    93,    22,    30,    31,    36,    40,    42,
      53,    96,    36,    21,   103,    67,    68,    68,    68,    69,
      70,    34,    36,    86,    90,    94,    96,    96,    45,    46,
      47,    48,    49,    50,    36,    43,    44,    53,   104,    17,
      68,    68,    53,    36,    36,    41,    38,    38,    38,    38,
      38,    38,    24,    25,    26,    96,    96,    34,    36,   100,
      90,    53,    53,   105,   106,    35,    34,    36,    38,   106,
      18,   101,   105,    36,     9,   107,    53,   108,    11,   109,
      34,    36,    53,   110,    53,    34,    36,    53
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
         0,    51,    52,    53,    53,    54,    54,    55,    55,    56,
      57,    56,    58,    59,    58,    60,    61,    60,    62,    62,
      63,    63,    63,    64,    65,    64,    66,    64,    67,    67,
      68,    68,    68,    68,    69,    68,    70,    68,    72,    71,
      73,    71,    74,    74,    75,    76,    75,    77,    75,    78,
      78,    79,    79,    80,    80,    81,    81,    82,    82,    84,
      83,    85,    83,    86,    86,    88,    89,    87,    90,    90,
      90,    91,    91,    93,    92,    92,    94,    94,    95,    95,
      95,    96,    96,    96,    96,    96,    96,    96,    96,    96,
      96,    96,    96,    96,    96,    96,    96,    97,    97,    99,
     100,   101,    98,   102,   102,   103,   103,   104,   104,   105,
     105,   105,   106,   106,   107,   107,   108,   108,   108,   109,
     109,   110,   110
  };

  /* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
  const unsigned char
  BisonParser::yyr2_[] =
  {
         0,     2,     3,     1,     1,     6,     4,     3,     1,     0,
       0,     4,     0,     0,     4,     0,     0,     4,     2,     4,
       0,     2,     3,     0,     0,     3,     0,     5,     0,     3,
       0,     4,     4,     4,     0,     5,     0,     5,     0,     6,
       0,     7,     2,     4,     0,     0,     3,     0,     5,     0,
       3,     1,     3,     4,     2,     0,     3,     0,     2,     0,
       5,     0,     6,     1,     3,     0,     0,     6,     0,     1,
       3,     3,     1,     0,     4,     1,     1,     3,     1,     2,
       3,     3,     1,     1,     1,     2,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     0,     2,     0,
       0,     0,    15,     0,     3,     0,     3,     1,     3,     0,
       1,     3,     3,     1,     0,     3,     0,     1,     3,     0,
       3,     1,     3
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
  "node_name", "interface", "interface_ports", "input_places", "$@1",
  "output_places", "$@2", "synchronous", "$@3", "interface_labels",
  "capacity", "interface_label_list", "$@4", "$@5", "controlcommands",
  "commands", "$@6", "$@7", "port_list", "$@8", "$@9", "places",
  "place_list", "$@10", "$@11", "roles", "role_names", "typed_places",
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
        52,     0,    -1,    54,    87,    97,    -1,    37,    -1,    38,
      -1,     4,    55,     5,    74,    36,    78,    -1,     5,    80,
      78,    82,    -1,    56,    58,    60,    -1,    71,    -1,    -1,
      -1,     7,    57,    62,    36,    -1,    -1,    -1,     8,    59,
      62,    36,    -1,    -1,    -1,    10,    61,    62,    36,    -1,
      63,    64,    -1,    62,    36,    63,    64,    -1,    -1,     3,
      35,    -1,     3,    38,    35,    -1,    -1,    -1,    53,    65,
      67,    -1,    -1,    64,    34,    53,    66,    67,    -1,    -1,
      32,    68,    33,    -1,    -1,    27,    50,    38,    68,    -1,
      28,    50,    30,    68,    -1,    28,    50,    31,    68,    -1,
      -1,    29,    50,    38,    69,    68,    -1,    -1,    29,    50,
      39,    70,    68,    -1,    -1,    19,    53,    72,    56,    58,
      60,    -1,    -1,    71,    19,    53,    73,    56,    58,    60,
      -1,    63,    75,    -1,    74,    36,    63,    75,    -1,    -1,
      -1,    53,    76,    67,    -1,    -1,    75,    34,    53,    77,
      67,    -1,    -1,    21,    79,    36,    -1,    53,    -1,    79,
      34,    53,    -1,    81,    56,    58,    60,    -1,    74,    36,
      -1,    -1,     6,    74,    36,    -1,    -1,    20,    83,    -1,
      -1,    53,    35,    84,    86,    36,    -1,    -1,    83,    53,
      35,    85,    86,    36,    -1,    53,    -1,    86,    34,    53,
      -1,    -1,    -1,    12,    88,    90,    36,    89,    92,    -1,
      -1,    91,    -1,    90,    34,    91,    -1,    53,    35,    38,
      -1,    53,    -1,    -1,    13,    93,    94,    36,    -1,    95,
      -1,    90,    -1,    94,    36,    90,    -1,    14,    -1,    15,
      36,    -1,    15,    96,    36,    -1,    40,    96,    41,    -1,
      30,    -1,    31,    -1,    22,    -1,    42,    96,    -1,    96,
      44,    96,    -1,    96,    43,    96,    -1,    96,    43,    24,
      -1,    96,    43,    25,    -1,    96,    43,    26,    -1,    53,
      50,    38,    -1,    53,    49,    38,    -1,    53,    47,    38,
      -1,    53,    48,    38,    -1,    53,    46,    38,    -1,    53,
      45,    38,    -1,    -1,    97,    98,    -1,    -1,    -1,    -1,
      16,    53,   102,    99,   103,    17,   100,   105,    36,    18,
     101,   105,    36,   107,   109,    -1,    -1,    23,    38,    36,
      -1,    -1,    21,   104,    36,    -1,    53,    -1,   104,    34,
      53,    -1,    -1,   106,    -1,   105,    34,   106,    -1,    53,
      35,    38,    -1,    53,    -1,    -1,     9,   108,    36,    -1,
      -1,    53,    -1,   108,    34,    53,    -1,    -1,    11,   110,
      36,    -1,    53,    -1,   110,    34,    53,    -1
  };

  /* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
     YYRHS.  */
  const unsigned short int
  BisonParser::yyprhs_[] =
  {
         0,     0,     3,     7,     9,    11,    18,    23,    27,    29,
      30,    31,    36,    37,    38,    43,    44,    45,    50,    53,
      58,    59,    62,    66,    67,    68,    72,    73,    79,    80,
      84,    85,    90,    95,   100,   101,   107,   108,   114,   115,
     122,   123,   131,   134,   139,   140,   141,   145,   146,   152,
     153,   157,   159,   163,   168,   171,   172,   176,   177,   180,
     181,   187,   188,   195,   197,   201,   202,   203,   210,   211,
     213,   217,   221,   223,   224,   229,   231,   233,   237,   239,
     242,   246,   250,   252,   254,   256,   259,   263,   267,   271,
     275,   279,   283,   287,   291,   295,   299,   303,   304,   307,
     308,   309,   310,   326,   327,   331,   332,   336,   338,   342,
     343,   345,   349,   353,   355,   356,   360,   361,   363,   367,
     368,   372,   374
  };

  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
  const unsigned short int
  BisonParser::yyrline_[] =
  {
         0,    95,    95,   102,   111,   128,   129,   133,   134,   137,
     139,   139,   142,   144,   144,   147,   149,   149,   154,   155,
     158,   160,   161,   164,   167,   166,   175,   174,   184,   186,
     189,   191,   192,   193,   195,   194,   203,   202,   214,   213,
     217,   216,   223,   224,   227,   230,   229,   238,   237,   248,
     250,   254,   256,   262,   263,   266,   268,   272,   274,   279,
     278,   282,   281,   287,   297,   316,   317,   316,   320,   322,
     323,   327,   341,   359,   358,   365,   378,   384,   393,   394,
     395,   399,   400,   401,   402,   407,   412,   418,   424,   429,
     433,   437,   444,   451,   458,   465,   472,   487,   489,   494,
     502,   509,   493,   519,   520,   523,   525,   529,   534,   541,
     543,   544,   548,   568,   590,   592,   595,   597,   604,   613,
     615,   619,   621
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
  const int BisonParser::yylast_ = 221;
  const int BisonParser::yynnts_ = 60;
  const int BisonParser::yyempty_ = -2;
  const int BisonParser::yyfinal_ = 16;
  const int BisonParser::yyterror_ = 1;
  const int BisonParser::yyerrcode_ = 256;
  const int BisonParser::yyntokens_ = 51;

  const unsigned int BisonParser::yyuser_token_number_max_ = 305;
  const BisonParser::token_number_type BisonParser::yyundef_token_ = 2;


/* Line 1053 of lalr1.cc  */
#line 11 "parser-owfn.yy"
} } } } // pnapi::parser::owfn::yy

/* Line 1053 of lalr1.cc  */
#line 1749 "parser-owfn.cc"


