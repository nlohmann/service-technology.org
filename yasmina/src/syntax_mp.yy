%{
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
extern char* mp_yytext;
extern int mp_yylineno;
extern int mp_yylex();

#include "eventTerm.h"
#include <iostream>
#include <string>
#include <stdio.h>
#include "lp_lib.h"
#include <vector>
#include "limits.h"
using namespace std;
extern std::vector<EventTerm*>* term_vec;


/// the set of inputPlaces and outputPlaces
extern std::set<std::string> inputPlaces,outputPlaces,synchrT;
/// TYPE OF THE PLACE
extern unsigned short pType;
/// the current place token as string
extern std::string IDENT_token;
/// final marking ids
 unsigned int nfm=0;
/// constraint ids
unsigned int nc=0;
typedef struct constraintf{
	unsigned int fmi; //final marking index
	unsigned int termi; //term index
	unsigned int lb;
	unsigned int ub;
} ;
extern std::set<const constraintf *> fingerprint;

//gets the bounds for a particular final marking
//extern set<constraintf *> getconstrforfm(set<constraintf *> fingerprint, unsigned int fmn);
	


//extern std::set<std::string> fm_set;fm_set.insert(*$1);
typedef std::pair<int,int>  BoundsMP;
//extern std::vector<std::vector<BoundsMP> > bMP;
//extern std::vector<std::pair<int,int> > bMP;
extern std::vector<std::map<std::string, int> > fmar; 
std::map<std::string, int> imar;//current marking

int mp_yyerror(const char* msg)
{
  std::cerr << msg << std::endl;
  std::cerr << "error in line " << mp_yylineno << ": token last read: `" << mp_yytext << "'" << std::endl;
  exit(1);
}


%}


// Bison options
%name-prefix="mp_yy"
%defines 

%token LPAR RPAR NUMBER SEMICOLON ADD MULT IDENT FIDENT CIDENT MINUS MEQ GT LT COMMA COLON MUNBOUNDED FM 
%token KEY_FINALMARKING KEY_PLACE KEY_INTERNAL KEY_INPUT KEY_OUTPUT KEY_TERMS KEY_BOUNDS KEY_SYNCHRONOUS
%token_table

%union 
{
  int yt_int;
  std::string* yt_string;
  EventTerm* yt_term;
}


%type <yt_int> NUMBER lbound ubound MUNBOUNDED
%type <yt_int> sign
%type <yt_term> sum
%type <yt_term> produkt
%type <yt_string> IDENT
%type <yt_string> CIDENT
%type <yt_string> FIDENT



%%

message_profile:
	interface fmarkings terms ulbounds
;

interface:
	KEY_PLACE intplaces SEMICOLON iplaces SEMICOLON oplaces SEMICOLON synchr 
;

iplaces:
	KEY_INPUT { pType = 0; } places

oplaces:
	KEY_OUTPUT { pType = 1; } places

intplaces:
	KEY_INTERNAL { pType = 7; } places

synchr:
|KEY_SYNCHRONOUS { pType = 2;} places SEMICOLON

places:
| place
;

place:
  IDENT
  {
    switch(pType)
    {//IDENT_token
    case 0: inputPlaces.insert(*$1); break;
    case 1: outputPlaces.insert(*$1); break;
    case 2: synchrT.insert(*$1); break;
    default: /* ignore */ ;
    }
  }
| IDENT COMMA place
  {
    switch(pType)
    {
    case 0: inputPlaces.insert(*$1); break;
    case 1: outputPlaces.insert(*$1); break;
    case 2: synchrT.insert(*$1); break;
	default: /* ignore */ ;
    }
  }
;


fmarkings:
	KEY_FINALMARKING { nfm = 0;fmar.clear();} markings 

markings:
  marking
| marking markings
;

marking:
FIDENT COLON  SEMICOLON  { nfm++ ;imar.clear();
fmar.push_back(imar);
}
| FIDENT COLON  mplaces SEMICOLON  { nfm++ ;
fmar.push_back(imar);
}
;

mplaces:
mplace {imar.clear();}
| mplace COMMA mplaces
;

mplace:
IDENT MEQ NUMBER{

if((inputPlaces.find(*$1)!=inputPlaces.end())||(outputPlaces.find(*$1)!=outputPlaces.end())){ //cout<<"bo help "<<*$1<<endl;
imar.insert(std::pair<std::string,int>(*$1,$3));	
}

}
;
  
terms: KEY_TERMS {nc=0;} term2 
;

term2: 
CIDENT COLON sum SEMICOLON {term_vec->push_back($3);++nc;}
|CIDENT COLON sum SEMICOLON {term_vec->push_back($3);++nc;} term2

sum: produkt {$$ = $1;}
       | produkt sum {$$ = new AddTerm($1,$2);} 
;

produkt: IDENT {$$ = new BasicTerm($1);}
         | NUMBER MULT LPAR sum RPAR {$$ = new MultiplyTerm($4,$1);}
         | sign NUMBER LPAR sum RPAR {$$ = new MultiplyTerm($4,$1*$2);}
         | NUMBER MULT IDENT {$$ = new MultiplyTerm(new BasicTerm($3),$1);}
         | sign NUMBER MULT IDENT {$$ = new MultiplyTerm(new BasicTerm($4),$1*$2);}
         | sign IDENT {$$ = new MultiplyTerm(new BasicTerm($2),$1);}
;

sign: MINUS {$$ = -1;}
            | ADD {$$ = 1;}
;

ulbounds: KEY_BOUNDS {fingerprint.clear();} bounds { /*bMP.resize(nfm);BoundsMP dummy(0,0);
for(int i=0;i<nfm;++i)  { vector<BoundsMP> hh;
for(int i=0;i<nc;++i)  {hh.push_back(dummy);}; 
bMP.push_back(hh);}*/
}
;
bounds: 
|bound bounds
;
bound: FIDENT COMMA CIDENT COLON lbound COMMA ubound SEMICOLON
 {
	unsigned int fmx=atoi($1->substr(1,$1->size()).c_str());
	unsigned int cx=atoi($3->substr(1,$3->size()).c_str());
	//cout << nc*(fm-1)+c-1;
	//std::pair<unsigned int,unsigned int> mat(fm,c); 
	 int lb=$5;
	int ub=$7; 
	const constraintf cs={fmx,cx,lb,ub};const constraintf *cp(new  constraintf(cs)); //
	fingerprint.insert(cp);
	//std::string ub(*$7);
	//BoundsMP bds(lb,ub);bMP.push_back(bds);
	//std::vector<BoundsMP> stu(bMP.at(fm));
	//std::pair<int,int>  boundMP;
	//boundMP.first=mat;boundMP.second=bds;boundsMP.push_back(boundMP);
	//cout<<stu.size()<<nc;//bMP.at(fm-1).at(c-1)=bds;
 }

lbound: NUMBER{$$=$1;}| MINUS NUMBER{$$=-$2;}|MUNBOUNDED{$$=-USHRT_MAX;}
;
ubound: NUMBER{$$=$1;}| MINUS NUMBER{$$=-$2;}|MUNBOUNDED{$$=USHRT_MAX;}
;