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
#include <string>



/* Line 310 of lalr1.cc  */
#line 52 "parser-owfn.cc"


#include "parser-owfn.h"

/* User implementation prologue.  */


/* Line 316 of lalr1.cc  */
#line 61 "parser-owfn.cc"

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
#line 129 "parser-owfn.cc"
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
#line 97 "parser-owfn.yy"
    { 
    parser_.net_.setConstraintLabels(parser_.constrains_); 
  }
    break;

  case 4:

/* Line 677 of lalr1.cc  */
#line 105 "parser-owfn.yy"
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
#line 114 "parser-owfn.yy"
    { 
    // clear stringstream
    parser_.nodeName_.str("");
    parser_.nodeName_.clear();

    parser_.nodeName_ << (yysemantic_stack_[(1) - (1)].yt_int); 
  }
    break;

  case 21:

/* Line 677 of lalr1.cc  */
#line 163 "parser-owfn.yy"
    { parser_.labelType_ = Label::INPUT; }
    break;

  case 23:

/* Line 677 of lalr1.cc  */
#line 167 "parser-owfn.yy"
    { parser_.labelType_ = Label::OUTPUT; }
    break;

  case 25:

/* Line 677 of lalr1.cc  */
#line 171 "parser-owfn.yy"
    { parser_.labelType_ = Label::SYNCHRONOUS; }
    break;

  case 27:

/* Line 677 of lalr1.cc  */
#line 176 "parser-owfn.yy"
    { parser_.capacity_ = 0; }
    break;

  case 28:

/* Line 677 of lalr1.cc  */
#line 177 "parser-owfn.yy"
    { parser_.capacity_ = 0; }
    break;

  case 30:

/* Line 677 of lalr1.cc  */
#line 182 "parser-owfn.yy"
    { parser_.capacity_ = 1; }
    break;

  case 31:

/* Line 677 of lalr1.cc  */
#line 183 "parser-owfn.yy"
    { parser_.capacity_ = (yysemantic_stack_[(3) - (2)].yt_int); }
    break;

  case 33:

/* Line 677 of lalr1.cc  */
#line 189 "parser-owfn.yy"
    {
    // parser_.check(parser_.labels_[parser_.nodeName_.str()] == NULL, "node name already used");
    // parser_.check(parser_.places_[parser_.nodeName_.str()] == NULL, "node name already used");
    try
    {
      if(parser_.port_ == NULL)
      {
        parser_.label_ = &(parser_.net_.getInterface().addLabel(parser_.nodeName_.str(), parser_.labelType_));
      }
      else
      {
        parser_.label_ = &(parser_.port_->addLabel(parser_.nodeName_.str(), parser_.labelType_));
      }
    }
    catch(exception::Error & e)
    {
      parser_.rethrow(e);
    }

    parser_.labels_[parser_.nodeName_.str()] = parser_.label_;
  }
    break;

  case 35:

/* Line 677 of lalr1.cc  */
#line 212 "parser-owfn.yy"
    {
    // parser_.check(parser_.labels_[parser_.nodeName_.str()] == NULL, "node name already used");
    // parser_.check(parser_.places_[parser_.nodeName_.str()] == NULL, "node name already used");
    try
    {
      if(parser_.port_ == NULL)
      {
        parser_.label_ = &(parser_.net_.getInterface().addLabel(parser_.nodeName_.str(), parser_.labelType_));
      }
      else
      {
        parser_.label_ = &(parser_.port_->addLabel(parser_.nodeName_.str(), parser_.labelType_));
      }
    }
    catch(exception::Error & e)
    {
      parser_.rethrow(e);
    }

    parser_.labels_[parser_.nodeName_.str()] = parser_.label_;
  }
    break;

  case 43:

/* Line 677 of lalr1.cc  */
#line 247 "parser-owfn.yy"
    {
    if(parser_.place_ != NULL) // is only NULL when parsing interface
    {
      parser_.place_->setMaxOccurrence((yysemantic_stack_[(3) - (3)].yt_int));
    }
  }
    break;

  case 45:

/* Line 677 of lalr1.cc  */
#line 255 "parser-owfn.yy"
    {
    if(parser_.place_ != NULL) // is only NULL when parsing interface
    {
      parser_.place_->setMaxOccurrence((yysemantic_stack_[(3) - (3)].yt_int));
    }
  }
    break;

  case 47:

/* Line 677 of lalr1.cc  */
#line 266 "parser-owfn.yy"
    {
    try
    {
      parser_.port_ = &(parser_.net_.getInterface().addPort(parser_.nodeName_.str()));
    }
    catch(exception::Error & e)
    {
      parser_.rethrow(e);
    }
  }
    break;

  case 49:

/* Line 677 of lalr1.cc  */
#line 278 "parser-owfn.yy"
    {
    try
    {
      parser_.port_ = &(parser_.net_.getInterface().addPort(parser_.nodeName_.str()));
    }
    catch(exception::Error & e)
    {
      parser_.rethrow(e);
    }
  }
    break;

  case 57:

/* Line 677 of lalr1.cc  */
#line 307 "parser-owfn.yy"
    { parser_.capacity_ = 0; }
    break;

  case 58:

/* Line 677 of lalr1.cc  */
#line 308 "parser-owfn.yy"
    { parser_.capacity_ = 0; }
    break;

  case 60:

/* Line 677 of lalr1.cc  */
#line 314 "parser-owfn.yy"
    {
    // parser_.check(parser_.labels_[parser_.nodeName_.str()] == NULL, "node name already used");
    // parser_.check(parser_.places_[parser_.nodeName_.str()] == NULL, "node name already used");
    try
    {
      parser_.place_ = &(parser_.net_.createPlace(parser_.nodeName_.str(), 0, parser_.capacity_));
    }
    catch(exception::Error & e)
    {
      parser_.rethrow(e);
    }

    parser_.places_[parser_.nodeName_.str()] = parser_.place_;
  }
    break;

  case 62:

/* Line 677 of lalr1.cc  */
#line 330 "parser-owfn.yy"
    {
    // parser_.check(parser_.labels_[parser_.nodeName_.str()] == NULL, "node name already used");
    // parser_.check(parser_.places_[parser_.nodeName_.str()] == NULL, "node name already used");
    try
    {
      parser_.place_ = &(parser_.net_.createPlace(parser_.nodeName_.str(), 0, parser_.capacity_));
    }
    catch(exception::Error & e)
    {
      parser_.rethrow(e);
    }

    parser_.places_[parser_.nodeName_.str()] = parser_.place_;
  }
    break;

  case 65:

/* Line 677 of lalr1.cc  */
#line 354 "parser-owfn.yy"
    { parser_.net_.addRole(parser_.nodeName_.str()); }
    break;

  case 66:

/* Line 677 of lalr1.cc  */
#line 356 "parser-owfn.yy"
    { parser_.net_.addRole(parser_.nodeName_.str()); }
    break;

  case 76:

/* Line 677 of lalr1.cc  */
#line 379 "parser-owfn.yy"
    { parser_.labelType_ = Label::INPUT; }
    break;

  case 78:

/* Line 677 of lalr1.cc  */
#line 383 "parser-owfn.yy"
    { parser_.labelType_ = Label::OUTPUT; }
    break;

  case 80:

/* Line 677 of lalr1.cc  */
#line 387 "parser-owfn.yy"
    { parser_.labelType_ = Label::SYNCHRONOUS; }
    break;

  case 82:

/* Line 677 of lalr1.cc  */
#line 392 "parser-owfn.yy"
    { parser_.capacity_ = 0; }
    break;

  case 83:

/* Line 677 of lalr1.cc  */
#line 393 "parser-owfn.yy"
    { parser_.capacity_ = 0; }
    break;

  case 85:

/* Line 677 of lalr1.cc  */
#line 399 "parser-owfn.yy"
    {
    parser_.check(parser_.labels_[parser_.nodeName_.str()] == NULL, std::string("label name '") + parser_.nodeName_.str() + "' already used");
    parser_.check(parser_.places_[parser_.nodeName_.str()] == NULL, std::string("node name '") + parser_.nodeName_.str() + "' already used");
    parser_.check(parser_.labelTypes_.find(parser_.nodeName_.str()) == parser_.labelTypes_.end(), std::string("label name '") + parser_.nodeName_.str() + "' already used");

    parser_.labelTypes_[parser_.nodeName_.str()] = parser_.labelType_;
  }
    break;

  case 87:

/* Line 677 of lalr1.cc  */
#line 408 "parser-owfn.yy"
    {
    parser_.check(parser_.labels_[parser_.nodeName_.str()] == NULL, std::string("label name '") + parser_.nodeName_.str() + "' already used");
    parser_.check(parser_.places_[parser_.nodeName_.str()] == NULL, std::string("node name '") + parser_.nodeName_.str() + "' already used");
    parser_.check(parser_.labelTypes_.find(parser_.nodeName_.str()) == parser_.labelTypes_.end(), std::string("label name '") + parser_.nodeName_.str() + "' already used");

    parser_.labelTypes_[parser_.nodeName_.str()] = parser_.labelType_;
  }
    break;

  case 90:

/* Line 677 of lalr1.cc  */
#line 424 "parser-owfn.yy"
    {
    try
    {
      PNAPI_FOREACH(l, parser_.labelTypes_)
      {
        parser_.labels_[l->first] = &(parser_.net_.getInterface().addLabel(l->first, l->second));
      }
    }
    catch(exception::Error & e)
    {
      parser_.rethrow(e);
    }
    parser_.labelTypes_.clear();
  }
    break;

  case 92:

/* Line 677 of lalr1.cc  */
#line 443 "parser-owfn.yy"
    {
    try
    {
      parser_.port_ = &(parser_.net_.getInterface().addPort(parser_.nodeName_.str()));
    }
    catch(exception::Error & e)
    {
      parser_.rethrow(e);
    }
  }
    break;

  case 94:

/* Line 677 of lalr1.cc  */
#line 455 "parser-owfn.yy"
    {
    try
    {
      parser_.port_ = &(parser_.net_.getInterface().addPort(parser_.nodeName_.str()));
    }
    catch(exception::Error & e)
    {
      parser_.rethrow(e);
    }
  }
    break;

  case 96:

/* Line 677 of lalr1.cc  */
#line 470 "parser-owfn.yy"
    {
    parser_.label_ = parser_.net_.getInterface().findLabel(parser_.nodeName_.str());
    parser_.check(parser_.label_ == NULL,
                  ((parser_.label_) ? ("interface label already assigned to port '" + parser_.label_->getPort().getName() +"'") : ""));
    parser_.check(parser_.labelTypes_.find(parser_.nodeName_.str()) != parser_.labelTypes_.end(),
                  std::string("unknown label '") + parser_.nodeName_.str() + "'");

    try
    {
      parser_.labels_[parser_.nodeName_.str()] = &(parser_.port_->addLabel(parser_.nodeName_.str(), parser_.labelTypes_[parser_.nodeName_.str()]));
    }
    catch(exception::Error & e)
    {
      parser_.rethrow(e);
    }
  }
    break;

  case 97:

/* Line 677 of lalr1.cc  */
#line 487 "parser-owfn.yy"
    {
    parser_.label_ = parser_.net_.getInterface().findLabel(parser_.nodeName_.str());
    parser_.check(parser_.label_ == NULL,
                  (parser_.label_) ? ("interface label already assigned to port '" + parser_.label_->getPort().getName() +"'") : "");
    parser_.check(parser_.labelTypes_.find(parser_.nodeName_.str()) != parser_.labelTypes_.end(),
                  std::string("unknown label '") + parser_.nodeName_.str() + "'");

    try
    {
      parser_.labels_[parser_.nodeName_.str()] = &(parser_.port_->addLabel(parser_.nodeName_.str(), parser_.labelTypes_[parser_.nodeName_.str()]));
    }
    catch(exception::Error & e)
    {
      parser_.rethrow(e);
    }
  }
    break;

  case 98:

/* Line 677 of lalr1.cc  */
#line 512 "parser-owfn.yy"
    { parser_.markInitial_ = true; }
    break;

  case 99:

/* Line 677 of lalr1.cc  */
#line 513 "parser-owfn.yy"
    { parser_.markInitial_ = false; }
    break;

  case 104:

/* Line 677 of lalr1.cc  */
#line 524 "parser-owfn.yy"
    {
    parser_.place_ = parser_.places_[parser_.nodeName_.str()];
    parser_.check(parser_.place_ != NULL, "unknown place");
  }
    break;

  case 106:

/* Line 677 of lalr1.cc  */
#line 533 "parser-owfn.yy"
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

  case 107:

/* Line 677 of lalr1.cc  */
#line 544 "parser-owfn.yy"
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

  case 108:

/* Line 677 of lalr1.cc  */
#line 558 "parser-owfn.yy"
    { parser_.finalMarking_ = new Marking(parser_.net_, true); }
    break;

  case 109:

/* Line 677 of lalr1.cc  */
#line 560 "parser-owfn.yy"
    {
    delete parser_.finalMarking_;
    parser_.finalMarking_ = NULL;
  }
    break;

  case 110:

/* Line 677 of lalr1.cc  */
#line 565 "parser-owfn.yy"
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

  case 111:

/* Line 677 of lalr1.cc  */
#line 578 "parser-owfn.yy"
    {
    parser_.net_.getFinalCondition().addMarking(*(parser_.finalMarking_));
    delete parser_.finalMarking_;
    parser_.finalMarking_ = new Marking(parser_.net_, true);
  }
    break;

  case 112:

/* Line 677 of lalr1.cc  */
#line 584 "parser-owfn.yy"
    {
    parser_.net_.getFinalCondition().addMarking(*(parser_.finalMarking_));
    delete parser_.finalMarking_;
    parser_.finalMarking_ = new Marking(parser_.net_, true);
  }
    break;

  case 113:

/* Line 677 of lalr1.cc  */
#line 592 "parser-owfn.yy"
    { (yyval.yt_formula) = new formula::FormulaTrue(); }
    break;

  case 114:

/* Line 677 of lalr1.cc  */
#line 593 "parser-owfn.yy"
    { (yyval.yt_formula) = new formula::FormulaFalse(); }
    break;

  case 115:

/* Line 677 of lalr1.cc  */
#line 594 "parser-owfn.yy"
    { (yyval.yt_formula) = (yysemantic_stack_[(3) - (2)].yt_formula); }
    break;

  case 117:

/* Line 677 of lalr1.cc  */
#line 600 "parser-owfn.yy"
    {
    parser_.net_.getFinalCondition() = (*(yysemantic_stack_[(3) - (2)].yt_formula));
    delete (yysemantic_stack_[(3) - (2)].yt_formula);
  }
    break;

  case 118:

/* Line 677 of lalr1.cc  */
#line 607 "parser-owfn.yy"
    { (yyval.yt_formula) = (yysemantic_stack_[(3) - (2)].yt_formula); }
    break;

  case 119:

/* Line 677 of lalr1.cc  */
#line 608 "parser-owfn.yy"
    { (yyval.yt_formula) = new formula::FormulaTrue(); }
    break;

  case 120:

/* Line 677 of lalr1.cc  */
#line 609 "parser-owfn.yy"
    { (yyval.yt_formula) = new formula::FormulaFalse(); }
    break;

  case 121:

/* Line 677 of lalr1.cc  */
#line 611 "parser-owfn.yy"
    { 
    parser_.wildcardGiven_ = true;
    (yyval.yt_formula) = new formula::FormulaTrue();
  }
    break;

  case 122:

/* Line 677 of lalr1.cc  */
#line 616 "parser-owfn.yy"
    { 
    (yyval.yt_formula) = new formula::Negation(*(yysemantic_stack_[(2) - (2)].yt_formula));
    delete (yysemantic_stack_[(2) - (2)].yt_formula);
  }
    break;

  case 123:

/* Line 677 of lalr1.cc  */
#line 621 "parser-owfn.yy"
    {
    (yyval.yt_formula) = new formula::Disjunction(*(yysemantic_stack_[(3) - (1)].yt_formula), *(yysemantic_stack_[(3) - (3)].yt_formula));
    delete (yysemantic_stack_[(3) - (1)].yt_formula);
    delete (yysemantic_stack_[(3) - (3)].yt_formula);
  }
    break;

  case 124:

/* Line 677 of lalr1.cc  */
#line 627 "parser-owfn.yy"
    {
    (yyval.yt_formula) = new formula::Conjunction(*(yysemantic_stack_[(3) - (1)].yt_formula), *(yysemantic_stack_[(3) - (3)].yt_formula));
    delete (yysemantic_stack_[(3) - (1)].yt_formula);
    delete (yysemantic_stack_[(3) - (3)].yt_formula);
  }
    break;

  case 125:

/* Line 677 of lalr1.cc  */
#line 633 "parser-owfn.yy"
    {
    parser_.wildcardGiven_ = true;
    (yyval.yt_formula) = (yysemantic_stack_[(3) - (1)].yt_formula);
  }
    break;

  case 126:

/* Line 677 of lalr1.cc  */
#line 638 "parser-owfn.yy"
    {
    (yyval.yt_formula) = (yysemantic_stack_[(3) - (1)].yt_formula); // obsolete; kept due to compatibility
  }
    break;

  case 127:

/* Line 677 of lalr1.cc  */
#line 642 "parser-owfn.yy"
    {
    (yyval.yt_formula) = (yysemantic_stack_[(3) - (1)].yt_formula); // obsolete; kept due to compatibility
  }
    break;

  case 128:

/* Line 677 of lalr1.cc  */
#line 646 "parser-owfn.yy"
    {
    parser_.place_ = parser_.places_[parser_.nodeName_.str()];
    parser_.check(parser_.place_ != NULL, "unknown place");
  }
    break;

  case 129:

/* Line 677 of lalr1.cc  */
#line 651 "parser-owfn.yy"
    {
    (yyval.yt_formula) = (yysemantic_stack_[(3) - (3)].yt_formula);
  }
    break;

  case 130:

/* Line 677 of lalr1.cc  */
#line 658 "parser-owfn.yy"
    { 
    (yyval.yt_formula) = new formula::FormulaEqual(*parser_.place_, (yysemantic_stack_[(2) - (2)].yt_int));
  }
    break;

  case 131:

/* Line 677 of lalr1.cc  */
#line 662 "parser-owfn.yy"
    {
    (yyval.yt_formula) = new formula::Negation(formula::FormulaEqual(*parser_.place_, (yysemantic_stack_[(2) - (2)].yt_int)));
  }
    break;

  case 132:

/* Line 677 of lalr1.cc  */
#line 666 "parser-owfn.yy"
    {
    (yyval.yt_formula) = new formula::FormulaLess(*parser_.place_, (yysemantic_stack_[(2) - (2)].yt_int));
  }
    break;

  case 133:

/* Line 677 of lalr1.cc  */
#line 670 "parser-owfn.yy"
    {
    (yyval.yt_formula) = new formula::FormulaGreater(*parser_.place_, (yysemantic_stack_[(2) - (2)].yt_int));
  }
    break;

  case 134:

/* Line 677 of lalr1.cc  */
#line 674 "parser-owfn.yy"
    {
    (yyval.yt_formula) = new formula::FormulaGreaterEqual(*parser_.place_, (yysemantic_stack_[(2) - (2)].yt_int));
  }
    break;

  case 135:

/* Line 677 of lalr1.cc  */
#line 678 "parser-owfn.yy"
    {
    (yyval.yt_formula) = new formula::FormulaLessEqual(*parser_.place_, (yysemantic_stack_[(2) - (2)].yt_int));
  }
    break;

  case 136:

/* Line 677 of lalr1.cc  */
#line 686 "parser-owfn.yy"
    { (yyval.yt_formula) = (yysemantic_stack_[(3) - (2)].yt_formula); }
    break;

  case 137:

/* Line 677 of lalr1.cc  */
#line 687 "parser-owfn.yy"
    { (yyval.yt_formula) = new formula::FormulaTrue(); }
    break;

  case 138:

/* Line 677 of lalr1.cc  */
#line 688 "parser-owfn.yy"
    { (yyval.yt_formula) = new formula::FormulaFalse(); }
    break;

  case 139:

/* Line 677 of lalr1.cc  */
#line 690 "parser-owfn.yy"
    { 
    (yyval.yt_formula) = new formula::Negation(*(yysemantic_stack_[(2) - (2)].yt_formula));
    delete (yysemantic_stack_[(2) - (2)].yt_formula);
  }
    break;

  case 140:

/* Line 677 of lalr1.cc  */
#line 695 "parser-owfn.yy"
    {
    (yyval.yt_formula) = new formula::Disjunction(*(yysemantic_stack_[(3) - (1)].yt_formula), *(yysemantic_stack_[(3) - (3)].yt_formula));
    delete (yysemantic_stack_[(3) - (1)].yt_formula);
    delete (yysemantic_stack_[(3) - (3)].yt_formula);
  }
    break;

  case 141:

/* Line 677 of lalr1.cc  */
#line 701 "parser-owfn.yy"
    {
    (yyval.yt_formula) = new formula::Conjunction(*(yysemantic_stack_[(3) - (1)].yt_formula), *(yysemantic_stack_[(3) - (3)].yt_formula));
    delete (yysemantic_stack_[(3) - (1)].yt_formula);
    delete (yysemantic_stack_[(3) - (3)].yt_formula);
  }
    break;

  case 142:

/* Line 677 of lalr1.cc  */
#line 707 "parser-owfn.yy"
    {
    parser_.place_ = parser_.places_[parser_.nodeName_.str()];
    if(parser_.place_ == NULL)
    {
      parser_.place_ = parser_.places_[parser_.nodeName_.str()] = &(parser_.net_.createPlace(parser_.nodeName_.str()));
    }
  }
    break;

  case 143:

/* Line 677 of lalr1.cc  */
#line 715 "parser-owfn.yy"
    {
    (yyval.yt_formula) = (yysemantic_stack_[(3) - (3)].yt_formula);
  }
    break;

  case 146:

/* Line 677 of lalr1.cc  */
#line 735 "parser-owfn.yy"
    {
    // parser_.check(parser_.labels_[parser_.nodeName_.str()] == NULL, "node name already used");
    // parser_.check(!(parser_.net_.containsNode(parser_.nodeName_.str())), "node name already used");
    try
    {
      parser_.transition_ = &(parser_.net_.createTransition(parser_.nodeName_.str()));
    }
    catch(exception::Error & e)
    {
      parser_.rethrow(e);
    }
  }
    break;

  case 147:

/* Line 677 of lalr1.cc  */
#line 748 "parser-owfn.yy"
    {
    parser_.transition_->setCost((yysemantic_stack_[(4) - (4)].yt_int));
  }
    break;

  case 148:

/* Line 677 of lalr1.cc  */
#line 753 "parser-owfn.yy"
    { 
    parser_.target_ = reinterpret_cast<Node * *>(&(parser_.transition_));
    parser_.source_ = reinterpret_cast<Node * *>(&(parser_.place_)); 
    parser_.placeSet_.clear();
    parser_.placeSetType_ = true;
  }
    break;

  case 149:

/* Line 677 of lalr1.cc  */
#line 760 "parser-owfn.yy"
    { 
    parser_.source_ = reinterpret_cast<Node * *>(&(parser_.transition_));
    parser_.target_ = reinterpret_cast<Node * *>(&(parser_.place_)); 
    parser_.placeSet_.clear();
    parser_.placeSetType_ = false;
  }
    break;

  case 151:

/* Line 677 of lalr1.cc  */
#line 770 "parser-owfn.yy"
    { (yyval.yt_int) = 0;  }
    break;

  case 152:

/* Line 677 of lalr1.cc  */
#line 771 "parser-owfn.yy"
    { (yyval.yt_int) = (yysemantic_stack_[(3) - (2)].yt_int); }
    break;

  case 155:

/* Line 677 of lalr1.cc  */
#line 781 "parser-owfn.yy"
    {
    parser_.check(parser_.transition_->getPetriNet().isRoleSpecified(parser_.nodeName_.str()), "role has not been specified");
    parser_.transition_->addRole(parser_.nodeName_.str());
  }
    break;

  case 156:

/* Line 677 of lalr1.cc  */
#line 786 "parser-owfn.yy"
    {
    parser_.check(parser_.transition_->getPetriNet().isRoleSpecified(parser_.nodeName_.str()), "role has not been specified");
    parser_.transition_->addRole(parser_.nodeName_.str());
  }
    break;

  case 160:

/* Line 677 of lalr1.cc  */
#line 800 "parser-owfn.yy"
    {
    parser_.place_ = parser_.places_[parser_.nodeName_.str()];
    parser_.label_ = parser_.labels_[parser_.nodeName_.str()];
    parser_.check(!((parser_.place_ == NULL) && (parser_.label_ == NULL)), "unknown place or interface label");
    parser_.check(parser_.placeSet_.find(parser_.nodeName_.str()) == parser_.placeSet_.end(), parser_.placeSetType_ ? "place already in preset" : "place already in postset");
    parser_.check(!(parser_.placeSetType_ && (parser_.label_ != NULL) && (parser_.label_->getType() != Label::INPUT)), "can only consume from places and input labels");
    parser_.check(!(!parser_.placeSetType_ && (parser_.label_ != NULL) && (parser_.label_->getType() != Label::OUTPUT)), "can only produce to places and output labels");
  }
    break;

  case 162:

/* Line 677 of lalr1.cc  */
#line 813 "parser-owfn.yy"
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

  case 163:

/* Line 677 of lalr1.cc  */
#line 826 "parser-owfn.yy"
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

  case 167:

/* Line 677 of lalr1.cc  */
#line 848 "parser-owfn.yy"
    {
    parser_.label_ = parser_.labels_[parser_.nodeName_.str()];
    parser_.check(parser_.label_ != NULL, "unknown label");
    parser_.check(parser_.label_->getType() == Label::SYNCHRONOUS, "can only synchronize with synchronous labels");
    parser_.transition_->addLabel(*(parser_.label_));
  }
    break;

  case 168:

/* Line 677 of lalr1.cc  */
#line 855 "parser-owfn.yy"
    {
    parser_.label_ = parser_.labels_[parser_.nodeName_.str()];
    parser_.check(parser_.label_ != NULL, "unknown label");
    parser_.check(parser_.label_->getType() == Label::SYNCHRONOUS, "can only synchronize with synchronous labels");
    parser_.transition_->addLabel(*(parser_.label_));
  }
    break;

  case 171:

/* Line 677 of lalr1.cc  */
#line 870 "parser-owfn.yy"
    { parser_.constrains_[parser_.transition_].insert(parser_.nodeName_.str()); }
    break;

  case 172:

/* Line 677 of lalr1.cc  */
#line 872 "parser-owfn.yy"
    { parser_.constrains_[parser_.transition_].insert(parser_.nodeName_.str()); }
    break;



/* Line 677 of lalr1.cc  */
#line 1393 "parser-owfn.cc"
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
  const short int BisonParser::yypact_ninf_ = -197;
  const short int
  BisonParser::yypact_[] =
  {
        94,     2,    19,   112,    37,    48,  -197,    10,  -197,    54,
      59,   -10,    10,    31,    51,    81,  -197,  -197,  -197,  -197,
    -197,   134,   134,  -197,   -24,  -197,  -197,  -197,  -197,    87,
    -197,  -197,  -197,    10,  -197,  -197,  -197,  -197,  -197,  -197,
      10,     3,  -197,    61,  -197,    72,    11,    10,  -197,    87,
    -197,  -197,  -197,  -197,  -197,  -197,  -197,  -197,  -197,   -28,
    -197,   113,  -197,   134,   134,    10,   123,   130,   111,    87,
      87,    87,  -197,    29,  -197,  -197,  -197,  -197,   124,    10,
      10,  -197,   132,    10,   160,    87,    87,    87,  -197,   159,
     162,   163,   164,   165,   166,  -197,  -197,  -197,  -197,    78,
    -197,    10,  -197,  -197,   197,    19,   170,    10,   171,   172,
      10,  -197,   130,   103,  -197,  -197,    72,  -197,   135,    19,
      10,   173,   174,   175,  -197,  -197,  -197,  -197,  -197,  -197,
     177,    10,  -197,  -197,  -197,   203,    19,  -197,   180,    19,
      19,  -197,   197,   167,   168,   176,   182,   124,    10,  -197,
    -197,   187,    19,    19,    19,   178,  -197,  -197,   131,   200,
    -197,  -197,    10,   124,    10,   203,   186,    52,   151,  -197,
    -197,  -197,   117,    10,   124,    10,    10,  -197,  -197,  -197,
      85,  -197,  -197,   189,  -197,   180,  -197,  -197,  -197,   103,
     103,   103,  -197,  -197,    10,  -197,   141,  -197,  -197,   187,
      10,  -197,  -197,  -197,  -197,    98,    98,  -197,    90,   193,
     204,   124,  -197,  -197,  -197,   103,   103,  -197,  -197,   124,
     196,   195,    67,  -197,   113,  -197,    55,    98,  -197,    10,
     215,  -197,  -197,  -197,  -197,     1,  -197,  -197,  -197,  -197,
    -197,  -197,  -197,  -197,   144,  -197,   196,    10,  -197,    10,
    -197,  -197,   145,  -197,   198,    10,   216,   199,  -197,  -197,
    -197,  -197,    10,   148,   226,    10,   225,  -197,   149,    10,
    -197,    10,  -197,  -197,   152,  -197,    10,  -197,  -197
  };

  /* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
     doesn't specify something else to do.  Zero means the default is an
     error.  */
  const unsigned char
  BisonParser::yydefact_[] =
  {
         0,    14,    69,     0,     0,     0,     3,     0,     6,     9,
      11,     0,    59,     0,    90,    67,   137,   138,   116,     4,
       5,     0,     0,   142,     0,     1,    98,   144,    47,    29,
      21,    23,    25,     0,    20,    15,    16,    17,    18,    19,
       0,    10,    30,     0,    60,    57,    29,     0,     7,    29,
      76,    78,    80,    75,    70,    72,    73,    74,    71,     0,
     139,     0,   117,     0,     0,   101,     2,    51,     0,    29,
      29,    29,    65,     0,    49,    12,    13,    31,    37,     0,
      59,    68,     0,    91,     0,    29,    29,    29,   136,     0,
       0,     0,     0,     0,     0,   143,   141,   140,   104,     0,
     102,     0,   145,    52,    53,     8,     0,    32,     0,     0,
       0,    64,    51,    39,    61,    62,    58,    92,     0,    89,
      84,     0,     0,     0,   135,   134,   132,   133,   131,   130,
     107,     0,    99,   146,    54,    55,    22,    33,    27,    24,
      26,    66,    53,     0,     0,     0,     0,    37,     0,    94,
      85,    82,    77,    79,    81,     0,   105,   103,     0,   151,
      56,    48,    32,    37,     0,    55,     0,     0,     0,    38,
      63,    96,     0,     0,    37,     0,    84,   106,   108,   113,
       0,   100,   110,     0,   147,    28,    34,    35,    50,    39,
      39,    39,    43,    45,     0,    93,     0,    86,    87,    83,
     101,   121,   119,   120,   114,     0,     0,   128,     0,     0,
     153,    37,    40,    41,    42,    39,    39,    97,    95,    37,
     111,     0,     0,   122,     0,   115,     0,     0,   152,     0,
       0,    36,    44,    46,    88,   101,   118,   129,   125,   126,
     127,   124,   123,   155,     0,   148,   112,     0,   154,   157,
     156,   160,     0,   158,   163,     0,     0,     0,   161,   159,
     149,   162,   157,     0,   164,   166,   169,   167,     0,     0,
     150,     0,   165,   171,     0,   168,     0,   170,   172
  };

  /* YYPGOTO[NTERM-NUM].  */
  const short int
  BisonParser::yypgoto_[] =
  {
      -197,  -197,    -7,  -197,   202,  -197,  -197,  -197,  -197,   229,
    -197,   230,  -197,   232,  -197,   121,   -35,    82,  -197,  -197,
    -145,  -147,  -197,  -197,  -197,  -197,  -197,   133,   104,    84,
     -22,   179,  -197,  -197,     8,  -197,  -197,  -197,  -197,  -197,
    -197,  -197,  -197,  -197,  -197,   108,    71,  -197,  -197,  -197,
    -197,  -197,  -197,  -197,    77,  -197,  -197,  -197,  -194,   120,
    -197,  -197,  -197,  -197,  -197,  -197,  -197,  -196,  -197,    28,
     -18,  -197,  -197,  -197,  -197,  -197,  -197,  -197,  -197,  -197,
    -197,    -9,    -1,  -197,  -197,  -197,  -197,  -197,  -197
  };

  /* YYDEFGOTO[NTERM-NUM].  */
  const short int
  BisonParser::yydefgoto_[] =
  {
        -1,     4,    23,     5,    34,     8,    41,     9,    35,   103,
      69,   134,    70,   160,    71,   106,    12,   138,   163,   211,
     114,   146,   215,   216,    10,    67,   112,   104,   135,   161,
      13,    45,    78,   147,    39,    73,    14,    15,    54,    55,
      85,    56,    86,    57,    87,   121,   151,   174,   219,    58,
      48,    83,   148,   173,   172,    27,    65,   158,    99,   100,
     130,   156,   181,   200,   221,   182,     6,   208,   224,    95,
      24,    61,    66,   102,   159,   210,   249,   262,   184,   230,
     244,   252,   253,   254,   258,   266,   268,   270,   274
  };

  /* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule which
     number is the opposite.  If zero, do what YYDEFACT says.  */
  const signed char BisonParser::yytable_ninf_ = -110;
  const short int
  BisonParser::yytable_[] =
  {
        28,  -109,   170,    59,    60,    44,   220,    68,    29,   222,
     223,    80,    62,    88,    11,    63,    64,  -109,   186,    63,
      64,     7,    11,    53,    33,    42,    72,    84,    43,   197,
     241,   242,    33,    74,   107,   107,   107,    25,    19,    20,
      82,   246,   212,   213,   214,    96,    97,    19,    20,    76,
     120,   120,   120,   -29,    81,   -29,   -29,   -29,    98,    29,
      26,    30,    31,   110,    32,   111,   231,    46,   232,   233,
      80,    47,   115,    44,   234,    33,   118,   201,    40,   238,
     239,   240,   190,   191,    80,   202,   203,    49,    50,    51,
      11,    52,    19,    20,   133,   205,    77,   206,     1,     2,
     137,   162,    33,   141,   162,   162,    79,   201,   236,     3,
     226,   227,   131,   150,   132,   202,   203,   176,   176,   176,
     201,   204,    19,    20,    98,   205,   225,   206,   202,   203,
     143,   144,   145,   226,   227,    19,    20,    30,   205,   101,
     206,   171,    16,    17,   178,   179,   180,   105,    18,    19,
      20,   194,    21,   195,    22,   137,   113,   187,    89,    90,
      91,    92,    93,    94,    16,    17,   171,   117,   198,   150,
     149,    19,    20,   207,    21,   194,    22,   218,   247,   255,
     248,   256,   255,   271,   264,   272,   276,   217,   277,   192,
     193,   108,   109,    98,   122,   123,   119,   124,   207,   207,
     125,   126,   127,   128,   129,    31,   136,   139,   140,   152,
     153,   154,   155,    32,   164,   169,   177,   166,   167,   207,
     207,   175,   243,   183,   189,   229,   168,   209,    98,   228,
     131,   235,   245,   257,   260,   265,   269,   261,    36,    37,
     250,    38,   251,    75,   185,   142,   165,   199,   251,   188,
     196,   157,   237,   263,   259,   251,     0,     0,   267,   116,
       0,     0,   273,     0,   275,     0,     0,     0,     0,   278
  };

  /* YYCHECK.  */
  const short int
  BisonParser::yycheck_[] =
  {
         7,     0,   147,    21,    22,    12,   200,    29,     5,   205,
     206,    46,    36,    41,     3,    43,    44,    16,   163,    43,
      44,    19,     3,    15,    21,    35,    33,    49,    38,   174,
     226,   227,    21,    40,    69,    70,    71,     0,    37,    38,
      47,   235,   189,   190,   191,    63,    64,    37,    38,    41,
      85,    86,    87,    34,    46,    36,    37,    38,    65,     5,
      12,     7,     8,    34,    10,    36,   211,    36,   215,   216,
     105,    20,    79,    80,   219,    21,    83,    22,    19,    24,
      25,    26,    30,    31,   119,    30,    31,     6,     7,     8,
       3,    10,    37,    38,   101,    40,    35,    42,     4,     5,
     107,   136,    21,   110,   139,   140,    34,    22,    41,    15,
      43,    44,    34,   120,    36,    30,    31,   152,   153,   154,
      22,    36,    37,    38,   131,    40,    36,    42,    30,    31,
      27,    28,    29,    43,    44,    37,    38,     7,    40,    16,
      42,   148,    30,    31,    13,    14,    15,    36,    36,    37,
      38,    34,    40,    36,    42,   162,    32,   164,    45,    46,
      47,    48,    49,    50,    30,    31,   173,    35,   175,   176,
      35,    37,    38,   180,    40,    34,    42,    36,    34,    34,
      36,    36,    34,    34,    36,    36,    34,   194,    36,    38,
      39,    70,    71,   200,    86,    87,    36,    38,   205,   206,
      38,    38,    38,    38,    38,     8,    36,    36,    36,    36,
      36,    36,    35,    10,    34,    33,    38,    50,    50,   226,
     227,    34,   229,    23,    38,    21,    50,    38,   235,    36,
      34,    36,    17,    35,    18,     9,    11,    38,     9,     9,
     247,     9,   249,    41,   162,   112,   142,   176,   255,   165,
     173,   131,   224,   262,   255,   262,    -1,    -1,   265,    80,
      -1,    -1,   269,    -1,   271,    -1,    -1,    -1,    -1,   276
  };

  /* STOS_[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
  const unsigned char
  BisonParser::yystos_[] =
  {
         0,     4,     5,    15,    52,    54,   117,    19,    56,    58,
      75,     3,    67,    81,    87,    88,    30,    31,    36,    37,
      38,    40,    42,    53,   121,     0,    12,   106,    53,     5,
       7,     8,    10,    21,    55,    59,    60,    62,    64,    85,
      19,    57,    35,    38,    53,    82,    36,    20,   101,     6,
       7,     8,    10,    85,    89,    90,    92,    94,   100,   121,
     121,   122,    36,    43,    44,   107,   123,    76,    81,    61,
      63,    65,    53,    86,    53,    55,    85,    35,    83,    34,
      67,    85,    53,   102,    81,    91,    93,    95,    41,    45,
      46,    47,    48,    49,    50,   120,   121,   121,    53,   109,
     110,    16,   124,    60,    78,    36,    66,    67,    66,    66,
      34,    36,    77,    32,    71,    53,    82,    35,    53,    36,
      67,    96,    96,    96,    38,    38,    38,    38,    38,    38,
     111,    34,    36,    53,    62,    79,    36,    53,    68,    36,
      36,    53,    78,    27,    28,    29,    72,    84,   103,    35,
      53,    97,    36,    36,    36,    35,   112,   110,   108,   125,
      64,    80,    67,    69,    34,    79,    50,    50,    50,    33,
      71,    53,   105,   104,    98,    34,    67,    38,    13,    14,
      15,   113,   116,    23,   129,    68,    71,    53,    80,    38,
      30,    31,    38,    39,    34,    36,   105,    71,    53,    97,
     114,    22,    30,    31,    36,    40,    42,    53,   118,    38,
     126,    70,    72,    72,    72,    73,    74,    53,    36,    99,
     109,   115,   118,   118,   119,    36,    43,    44,    36,    21,
     130,    71,    72,    72,    71,    36,    41,   120,    24,    25,
      26,   118,   118,    53,   131,    17,   109,    34,    36,   127,
      53,    53,   132,   133,   134,    34,    36,    35,   135,   133,
      18,    38,   128,   132,    36,     9,   136,    53,   137,    11,
     138,    34,    36,    53,   139,    53,    34,    36,    53
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
      88,    89,    89,    89,    89,    89,    91,    90,    93,    92,
      95,    94,    96,    96,    97,    98,    97,    99,    97,   100,
     101,   101,   103,   102,   104,   102,   105,   105,   107,   108,
     106,   109,   109,   109,   111,   110,   112,   112,   114,   113,
     113,   115,   115,   116,   116,   116,   117,   117,   118,   118,
     118,   118,   118,   118,   118,   118,   118,   118,   119,   118,
     120,   120,   120,   120,   120,   120,   121,   121,   121,   121,
     121,   121,   122,   121,   123,   123,   125,   126,   127,   128,
     124,   129,   129,   130,   130,   131,   131,   132,   132,   132,
     134,   133,   135,   135,   136,   136,   137,   137,   137,   138,
     138,   139,   139
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
       2,     1,     1,     1,     1,     1,     0,     4,     0,     4,
       0,     4,     2,     4,     0,     0,     3,     0,     5,     3,
       0,     2,     0,     5,     0,     6,     1,     3,     0,     0,
       6,     0,     1,     3,     0,     3,     2,     0,     0,     4,
       1,     1,     3,     1,     2,     3,     2,     3,     3,     1,
       1,     1,     2,     3,     3,     3,     3,     3,     0,     3,
       2,     2,     2,     2,     2,     2,     3,     1,     1,     2,
       3,     3,     0,     3,     0,     2,     0,     0,     0,     0,
      16,     0,     3,     0,     3,     1,     3,     0,     1,     3,
       0,     3,     2,     0,     0,     3,     0,     1,     3,     0,
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
  "node_name", "interface", "interface_place", "interface_ports",
  "port_list_tail", "interface_ports_list", "interface_ports_list_element",
  "input_places", "$@1", "output_places", "$@2", "synchronous", "$@3",
  "interface_labels", "capacity", "interface_label_list", "$@4", "$@5",
  "controlcommands", "commands", "$@6", "$@7", "port_list", "$@8", "$@9",
  "port_input_places", "port_output_places", "port_synchronous", "places",
  "place_list", "$@10", "$@11", "roles", "role_names", "typed_interface",
  "typed_interface_list", "typed_interface_list_element", "input_places_2",
  "$@12", "output_places_2", "$@13", "synchronous_2", "$@14",
  "interface_labels_2", "interface_label_list_2", "$@15", "$@16",
  "internal_places", "ports", "port_list2", "$@17", "$@18",
  "port_participants", "markings", "$@19", "$@20", "marking_list",
  "marking", "$@21", "marking_tail", "final", "$@22", "finalmarkings",
  "condition", "finalcondition_only", "formula", "$@23",
  "formula_proposition", "fo_formula", "$@24", "transitions", "transition",
  "$@25", "$@26", "$@27", "$@28", "transition_cost", "transition_roles",
  "transition_role_names", "arcs", "arc", "$@29", "arc_tail",
  "synchronize", "synchronize_labels", "constrain", "constrain_labels", 0
  };
#endif

#if YYDEBUG
  /* YYRHS -- A `-1'-separated list of the rules' RHS.  */
  const BisonParser::rhs_number_type
  BisonParser::yyrhs_[] =
  {
        52,     0,    -1,    54,   106,   123,    -1,   117,    -1,    37,
      -1,    38,    -1,     4,    56,    -1,     5,    87,   101,    -1,
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
      -1,    81,    36,    85,    -1,    -1,    88,    89,    -1,   100,
      -1,    90,    -1,    92,    -1,    94,    -1,    85,    -1,    -1,
       7,    91,    96,    36,    -1,    -1,     8,    93,    96,    36,
      -1,    -1,    10,    95,    96,    36,    -1,    67,    97,    -1,
      96,    36,    67,    97,    -1,    -1,    -1,    53,    98,    71,
      -1,    -1,    97,    34,    53,    99,    71,    -1,     6,    81,
      36,    -1,    -1,    20,   102,    -1,    -1,    53,    35,   103,
     105,    36,    -1,    -1,   102,    53,    35,   104,   105,    36,
      -1,    53,    -1,   105,    34,    53,    -1,    -1,    -1,    12,
     107,   109,    36,   108,   113,    -1,    -1,   110,    -1,   109,
      34,   110,    -1,    -1,    53,   111,   112,    -1,    35,    38,
      -1,    -1,    -1,    13,   114,   115,    36,    -1,   116,    -1,
     109,    -1,   115,    36,   109,    -1,    14,    -1,    15,    36,
      -1,    15,   118,    36,    -1,    15,    36,    -1,    15,   121,
      36,    -1,    40,   118,    41,    -1,    30,    -1,    31,    -1,
      22,    -1,    42,   118,    -1,   118,    44,   118,    -1,   118,
      43,   118,    -1,   118,    43,    24,    -1,   118,    43,    25,
      -1,   118,    43,    26,    -1,    -1,    53,   119,   120,    -1,
      50,    38,    -1,    49,    38,    -1,    47,    38,    -1,    48,
      38,    -1,    46,    38,    -1,    45,    38,    -1,    40,   121,
      41,    -1,    30,    -1,    31,    -1,    42,   121,    -1,   121,
      44,   121,    -1,   121,    43,   121,    -1,    -1,    53,   122,
     120,    -1,    -1,   123,   124,    -1,    -1,    -1,    -1,    -1,
      16,    53,   125,   129,   126,   130,    17,   127,   132,    36,
      18,   128,   132,    36,   136,   138,    -1,    -1,    23,    38,
      36,    -1,    -1,    21,   131,    36,    -1,    53,    -1,   131,
      34,    53,    -1,    -1,   133,    -1,   132,    34,   133,    -1,
      -1,    53,   134,   135,    -1,    35,    38,    -1,    -1,    -1,
       9,   137,    36,    -1,    -1,    53,    -1,   137,    34,    53,
      -1,    -1,    11,   139,    36,    -1,    53,    -1,   139,    34,
      53,    -1
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
     196,   199,   201,   203,   205,   207,   209,   210,   215,   216,
     221,   222,   227,   230,   235,   236,   237,   241,   242,   248,
     252,   253,   256,   257,   263,   264,   271,   273,   277,   278,
     279,   286,   287,   289,   293,   294,   298,   301,   302,   303,
     308,   310,   312,   316,   318,   321,   325,   328,   332,   336,
     338,   340,   342,   345,   349,   353,   357,   361,   365,   366,
     370,   373,   376,   379,   382,   385,   388,   392,   394,   396,
     399,   403,   407,   408,   412,   413,   416,   417,   418,   419,
     420,   437,   438,   442,   443,   447,   449,   453,   454,   456,
     460,   461,   465,   468,   469,   470,   474,   475,   477,   481,
     482,   486,   488
  };

  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
  const unsigned short int
  BisonParser::yyrline_[] =
  {
         0,    96,    96,   100,   104,   113,   130,   131,   135,   139,
     140,   143,   145,   146,   149,   151,   155,   156,   157,   158,
     159,   163,   163,   167,   167,   171,   171,   176,   177,   180,
     182,   183,   186,   189,   188,   212,   211,   236,   238,   241,
     243,   244,   245,   247,   246,   255,   254,   266,   265,   278,
     277,   291,   293,   296,   298,   301,   303,   307,   308,   311,
     314,   313,   330,   329,   349,   353,   355,   361,   362,   365,
     367,   371,   372,   373,   374,   375,   379,   379,   383,   383,
     387,   387,   392,   393,   396,   399,   398,   408,   407,   419,
     424,   438,   443,   442,   455,   454,   469,   486,   512,   513,
     512,   516,   518,   519,   524,   523,   532,   544,   558,   557,
     564,   577,   583,   592,   593,   594,   598,   599,   607,   608,
     609,   610,   615,   620,   626,   632,   637,   641,   646,   645,
     657,   661,   665,   669,   673,   677,   686,   687,   688,   689,
     694,   700,   707,   706,   728,   730,   735,   748,   753,   760,
     734,   770,   771,   774,   776,   780,   785,   792,   794,   795,
     800,   799,   812,   826,   840,   842,   845,   847,   854,   863,
     865,   869,   871
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
  const int BisonParser::yylast_ = 269;
  const int BisonParser::yynnts_ = 89;
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
#line 2103 "parser-owfn.cc"


