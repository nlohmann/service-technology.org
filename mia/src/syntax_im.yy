%token KW_INTERFACE COMMA RBRACKET LBRACKET NUMBER NAME

%defines
%name-prefix="im_"

%{
#include <cstdio>
#include <string>
#include <vector>
#include <map>

/// the tuples of the source (innermarking, mppstate, interfacemarking)
std::map<unsigned, std::vector<std::vector<unsigned int> > > tuples_source;

std::vector<unsigned int> currentTuple;

unsigned int stat_tupleCount = 0;

// mappings for the port name/id management
std::map<unsigned int, std::string> id2name;
std::map<std::string, unsigned int> name2id;

unsigned int currentPos;

/// the number of interface places
unsigned int interfaceLength = 1;

/// the current NAME token as string
extern std::string NAME_token;

extern int im_lex();
extern int im_error(const char *);
%}

%union {
  unsigned int val;
}

%type <val> NUMBER

%%

im:
  KW_INTERFACE LBRACKET interface RBRACKET
    { currentTuple = std::vector<unsigned int>(interfaceLength, 0); }
  tuples
;

interface:
  NAME
    { id2name[interfaceLength] = NAME_token;
      name2id[NAME_token] = interfaceLength;
      ++interfaceLength; }
| interface COMMA NAME
    { id2name[interfaceLength] = NAME_token;
      name2id[NAME_token] = interfaceLength;
      ++interfaceLength; }
;

tuples:
  tuple
| tuples tuple;

tuple:
  NUMBER NUMBER 
    { currentTuple[0] = $2; currentPos = 1; }
  LBRACKET vector RBRACKET
    { tuples_source[$1].push_back(currentTuple); ++stat_tupleCount; }
;

vector:
  NUMBER
    { currentTuple[currentPos++] = $1; }
| vector COMMA NUMBER
    { currentTuple[currentPos++] = $3; }
;
