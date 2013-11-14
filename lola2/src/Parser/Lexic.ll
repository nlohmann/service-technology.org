/*!
\author Karsten
\status approved 25.01.2012

Joint lexic for all inputs to LoLA
Mainly copied from LoLA1

\todo Herausfinden, ob es Probleme bei zu langen Kommentaren/Bezeichnern gibt. Idee: Maximallänge angeben.
\todo Präfix hinzufügen?
*/

/* we want line numbering */
%option yylineno  
/* we don't neet yyunput() */
%option nounput

%option outfile="lex.yy.c"
%option prefix="ptnetlola_"


%{
#include <cmdline.h>
#include <Parser/ArcList.h>
#include <Parser/FairnessAssumptions.h>
#include <Parser/ParserPTNetLoLA.hh>
#include <InputOutput/Reporter.h>
#include <InputOutput/InputOutput.h>

extern int currentFile;
extern gengetopt_args_info args_info;
extern Input *netFile;

void setlval();
void setcol();
extern void ptnetlola_error(char const* mess);
int ptnetlola_colno = 1;
%}

%s IN_COMMENT

%%

 /* from http://flex.sourceforge.net/manual/How-can-I-match-C_002dstyle-comments_003f.html */
"/*"                                     { setcol(); BEGIN(IN_COMMENT); }
<IN_COMMENT>"*/"                         { setcol(); BEGIN(INITIAL); }
<IN_COMMENT>[^*\n\r]+                    { setcol(); /* comments */ }
<IN_COMMENT>"*"                          { setcol(); /* comments */ }
<IN_COMMENT>[\n\r]                       { setcol(); /* comments */ }

CONSUME                                  { setcol(); return _CONSUME_; }
FAIR                                     { setcol(); return _FAIR_; }
MARKING                                  { setcol(); return _MARKING_; }
PLACE                                    { setcol(); return _PLACE_; }
PRODUCE                                  { setcol(); return _PRODUCE_; }
SAFE                                     { setcol(); return _SAFE_; }
STRONG                                   { setcol(); return _STRONG_; }
TRANSITION                               { setcol(); return _TRANSITION_; }
WEAK                                     { setcol(); return _WEAK_; }

\:                                       { setcol(); return _colon_; }
,                                        { setcol(); return _comma_; }
\;                                       { setcol(); return _semicolon_; }

[\n\r]                                   { ptnetlola_colno = 1; /* whitespace */ }
[\t ]                                    { setcol();  /* whitespace */ }

[0-9]+                                   { setcol(); setlval(); return NUMBER; }

"{"[^\n\r]*"}"                           { setcol(); /* comments */ }


[^,;:()\t \n\r\{\}]+                     { setcol(); setlval(); return IDENTIFIER; }

.                                        { setcol(); ptnetlola_error("lexical error"); }

%%

/*! pass token string as attribute to bison */
inline void setlval()
{
    ptnetlola_lval.attributeString = strdup(ptnetlola_text);
}

inline void setcol()
{
    ptnetlola_colno += ptnetlola_leng;
}

int ptnetlola_wrap()
{
    if (currentFile == (int)args_info.inputs_num-1 or currentFile == -1)
    {
        // last or only file parsed - done parsing
        return 1;
    }
    else
    {
        // open next file
        delete netFile;
        netFile = new Input("net", args_info.inputs[++currentFile]);

        ptnetlola_colno = 1;
        ptnetlola_lineno = 1;
        return 0;
    }
}
