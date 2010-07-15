%token NAME COMMA COLON SEMICOLON KEY_PLACE KEY_MARKING KEY_TRANSITIONS
%token LBRACK RBRACK KEY_CONSUME KEY_PRODUCE

%defines
%name-prefix="llowfn_initial_"

%{
#include <cstdlib>
#include <cstdio>
#include <string>
#include <map>
#include <vector>
#include "types.h"

extern int llowfn_initial_lex();
extern int llowfn_initial_error(const char *);

extern FILE* llowfn_initial_out;
extern std::map<std::string, pType> placeTypes;

std::vector<std::string> internalPlaces;
std::vector<std::string> inputPlaces;
std::vector<std::string> outputPlaces; 
%}

%union {
  char* str;
}

%type <str> NAME

%%


placelist:
  places SEMICOLON
  {
    /* Write interface */
    size_t i;

    //Internal places
    if(internalPlaces.empty() == false){
	fprintf(llowfn_initial_out, "%s\n", "INTERNAL");
    	for(i=0;i<internalPlaces.size()-1;++i){
		fprintf(llowfn_initial_out, "\t%s,\n", internalPlaces[i].c_str()); 
    	}	    
    	fprintf(llowfn_initial_out, "\t%s\n;\n", internalPlaces[i].c_str());
    }
    internalPlaces.clear();

    //Input places

    if(inputPlaces.empty() == false){
    	    fprintf(llowfn_initial_out, "%s\n", "INPUT");
	    for(i=0;i<inputPlaces.size()-1;++i){
		fprintf(llowfn_initial_out, "\t%s,\n", inputPlaces[i].c_str());
            } 
    		fprintf(llowfn_initial_out, "\t%s\n;\n", inputPlaces[i].c_str());
    }
    inputPlaces.clear();

    //Output places

    if(outputPlaces.empty() == false){
    	fprintf(llowfn_initial_out, "%s\n", "OUTPUT");
        for(i=0;i<outputPlaces.size()-1;++i){
      		fprintf(llowfn_initial_out, "\t%s,\n", outputPlaces[i].c_str()); 
    	}    
        	fprintf(llowfn_initial_out, "\t%s\n;\n", outputPlaces[i].c_str());
    }
    outputPlaces.clear();
  }
;

places:
  place
| places COMMA place
;

place:
  NAME
  {
    std::string id = std::string($1);
    int pos = id.rfind(".");
    id = id.substr(0, id.length() - (id.length() - pos));
    switch ( placeTypes[id] ) {
	 case INTERNAL:
                 internalPlaces.push_back($1);	
		 break;
         case INPUT:
                 inputPlaces.push_back($1);
	         break;
	 case OUTPUT:	
		  outputPlaces.push_back($1);			  
    }
  }			
;

