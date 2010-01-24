 // -*- C++ -*-

 /*****************************************************************************
  * bison options 
  ****************************************************************************/

/* plain c parser: the prefix is our "namespace" */
%name-prefix="pnapi_pnml_yy"

/* write tokens to parser-pnml.h for use by scanner */
%defines

/* more verbose error reporting */
%error-verbose


 /*****************************************************************************
  * C declarations
  ****************************************************************************/
%{
#include "parser.h"
#include <string>
#include <cstring>
#include <sstream>
#include <map>
#include <vector>

#undef yylex
#undef yyparse
#undef yyerror

#define yyerror pnapi::parser::error
#define yylex pnapi::parser::pnml::lex
#define yylex_destory pnapi::parser::pnml::lex_destroy
#define yyparse pnapi::parser::pnml::parse

using namespace pnapi;
using namespace pnapi::parser::pnml;

std::map<unsigned int, std::map<std::string, std::string> > attributes;

 /* we only care about these elements */
const char* whitelist[] = {"pnml", "net", "arc", "place", "transition", "inscription", "initialMarking", "annotation", "text", "module", "ports", "port", "input", "output", "synchronous", "receive", "send", "synchronize", "finalmarkings", "marking", "page", NULL };
enum elementTypes {
    T_PNML,
    T_NET, T_ARC, T_PLACE, T_TRANSITION, T_INSCRIPTION,
    T_INITIALMARKING, T_ANNOTATION, T_TEXT, T_MODULE, T_PORTS,
    T_PORT, T_INPUT, T_OUTPUT, T_SYNCHRONOUS, T_RECEIVE,
    T_SEND, T_SYNCHRONIZE, T_FINALMARKINGS, T_MARKING, T_PAGE,
    T_NONE };

pnapi::Marking* currentMarking;

unsigned int current_depth = 0;
unsigned int last_interesting_depth = 0;
bool ignoring = false;
unsigned int file_part = T_NONE;
std::vector<unsigned int> elementStack;

bool is_whitelisted(char *s) {
    unsigned int i = 0;
    while (whitelist[i]) {
        if (!strcmp(s, whitelist[i++])) {

            // remember which part of the net we are parsing
            switch(i-1) {
                case T_PORTS:
                case T_NET:
                case T_FINALMARKINGS:
                    file_part = i-1;
                    break;
                case T_MARKING:
                    currentMarking = new pnapi::Marking(pnapi_pnml_yynet);
                    break;
            }
            
            elementStack.push_back(i-1);
            return true;
        }
    }
    return false;
}

void open_element(char *s) {    
    // first check if this element is whitelisted
    if (!ignoring && !is_whitelisted(s)) {
        // element is blacklistet -- remember this!
        last_interesting_depth = current_depth;
        ignoring = true;
    }

    // we traverse a new element
    ++current_depth;
    attributes[current_depth].clear();
}

void close_element() {
    unsigned int myType = T_NONE;
    
    if (!ignoring) {
        myType = elementStack[elementStack.size()-1];
        elementStack.pop_back();
    }

    if (file_part == T_PORTS) {
        switch(myType) {
            case(T_INPUT): {
                pnapi_pnml_yynet.createPlace(attributes[current_depth]["id"], Node::INPUT);
                break;
            }
            case(T_OUTPUT): {
                pnapi_pnml_yynet.createPlace(attributes[current_depth]["id"], Node::OUTPUT);
                break;
            }
            case(T_SYNCHRONOUS): {
                pnapi_pnml_yynet.addSynchronousLabel(attributes[current_depth]["id"]);
                break;
            }
        }
    }

    if (file_part == T_NET) {
        switch(myType) {
            case(T_PLACE): {
                //printf("creating place %s\n", attributes[current_depth]["id"].c_str());
                pnapi_pnml_yynet.createPlace(attributes[current_depth]["id"], Node::INTERNAL, atoi(attributes[current_depth]["__initialMarking__"].c_str()));
                break;
            }
            case(T_TRANSITION): {
                //printf("creating transition %s\n", attributes[current_depth]["id"].c_str());
                pnapi::Transition *currentTransition = &pnapi_pnml_yynet.createTransition(attributes[current_depth]["id"]);

                if (!attributes[current_depth]["__send__"].empty()) {
                    Node *target = pnapi_pnml_yynet.findNode(attributes[current_depth]["__send__"]);
                    if (!target) {
                        yyerror(std::string("output place '" + attributes[current_depth]["__send__"] + "' not found").c_str());
                    }
                    pnapi_pnml_yynet.createArc(*currentTransition, *target);
                }
                if (!attributes[current_depth]["__receive__"].empty()) {
                    Node *target = pnapi_pnml_yynet.findNode(attributes[current_depth]["__receive__"]);
                    if (!target) {
                        yyerror(std::string("input place '" + attributes[current_depth]["__receive__    "] + "' not found").c_str());
                    }
                    pnapi_pnml_yynet.createArc(*target, *currentTransition);
                }
                if (!attributes[current_depth]["__synchronize__"].empty()) {
                    std::set<std::string> labels = currentTransition->getSynchronizeLabels();
                    labels.insert(attributes[current_depth]["__synchronize__"]);
                    currentTransition->setSynchronizeLabels(labels);
                }
                break;
            }
            case(T_ARC): {
                //printf("creating arc %s\n", attributes[current_depth]["id"].c_str());
                Node *source = pnapi_pnml_yynet.findNode(attributes[current_depth]["source"]);
                if (!source) {
                    yyerror(std::string("node '" + attributes[current_depth]["source"] + "' not found").c_str());
                }
                Node *target = pnapi_pnml_yynet.findNode(attributes[current_depth]["target"]);
                if (!target) {
                    yyerror(std::string("node '" + attributes[current_depth]["target"] + "' not found").c_str());
                }
                unsigned int weight = (attributes[current_depth]["__inscription__"].empty()) ? 1 : atoi(attributes[current_depth]["__inscription__"].c_str());
                pnapi_pnml_yynet.createArc(*source, *target, weight);
                break;
            }
            case(T_TEXT): {
                // check the parent
                if (elementStack[elementStack.size()-1] == T_INITIALMARKING) {
                    // store data as parent's attribute
                    attributes[current_depth-2]["__initialMarking__"] = attributes[current_depth]["__data__"];
                }
                if (elementStack[elementStack.size()-1] == T_INSCRIPTION) {
                    // store data as parent's attribute
                    attributes[current_depth-2]["__inscription__"] = attributes[current_depth]["__data__"];
                }
                break;
            }
            case(T_RECEIVE): {
                if (elementStack[elementStack.size()-1] == T_TRANSITION) {
                    attributes[current_depth-1]["__receive__"] = attributes[current_depth]["id"];
                }
                break;
            }
            case(T_SEND): {
                if (elementStack[elementStack.size()-1] == T_TRANSITION) {
                    attributes[current_depth-1]["__send__"] = attributes[current_depth]["id"];
                }
                break;
            }
            case(T_SYNCHRONIZE): {
                if (elementStack[elementStack.size()-1] == T_TRANSITION) {
                    attributes[current_depth-1]["__synchronize__"] = attributes[current_depth]["id"];
                }
                break;
            }
        }
    }

    if (file_part == T_FINALMARKINGS) {
        switch(myType) {
            case(T_MARKING): {
                pnapi_pnml_yynet.finalCondition().addMarking(*currentMarking);
                delete currentMarking;
                break;
            }
            
            case(T_PLACE): {
                unsigned int tokens = atoi(attributes[current_depth]["__finalMarking__"].c_str());
                Node *place = pnapi_pnml_yynet.findNode(attributes[current_depth]["id"]);
                if (!place) {
                    yyerror(std::string("place '" + attributes[current_depth]["id"] + "' not found").c_str());
                }
                (*currentMarking)[(const pnapi::Place&)*place] = tokens;
            }
            
            case(T_TEXT): {
                // check the parent
                if (elementStack[elementStack.size()-1] == T_PLACE) {
                    // store data as parent's attribute
                    attributes[current_depth-1]["__finalMarking__"] = attributes[current_depth]["__data__"];
                }
            }
            break;
        }
    }

    --current_depth;

    // return form ignoring elements
    if (current_depth == last_interesting_depth) {
        last_interesting_depth = 0;
        ignoring = false;
    }
}

void store_attributes(char *a1, char *a2) {
    if (!ignoring) {
        // store attribute (strip quotes from a2)
        attributes[current_depth][a1] = std::string(a2).substr(1, strlen(a2)-2);
    }
}

void store_data(char *s) {
    if (!ignoring) {
        // store current data as attribute of surrouding element
        attributes[current_depth]["__data__"] = s;
    }
}

%}


 /*****************************************************************************
  * types, tokens, start symbol
  ****************************************************************************/

%union {char *s;}

%token XML_ENDDEF XML_EQ XML_SLASH XML_CLOSE XML_END
%token <s> XML_NAME XML_VALUE XML_DATA XML_COMMENT XML_START
%type <s> name_opt


 /*****************************************************************************
  * grammar rules 
  ****************************************************************************/
%%

document:
  misc_seq_opt element misc_seq_opt
;
misc_seq_opt:
  misc_seq_opt misc
| /*empty*/
;
misc:
  XML_COMMENT
;
element:
  XML_START                          {open_element($1); free($1);}
  attribute_seq_opt empty_or_content
;
empty_or_content:
  XML_SLASH XML_CLOSE                {close_element();}
| XML_CLOSE content XML_END name_opt XML_CLOSE   {close_element();}
;
content:
  content XML_DATA                   {store_data($2);}
| content misc
| content element
| /*empty*/
;
name_opt:
  XML_NAME                           {$$ = $1;}
| /*empty*/                          {$$ = strdup("");}
;
attribute_seq_opt:
  attribute_seq_opt attribute
| /*empty*/
;
attribute:
  XML_NAME                           { free($1);}
| XML_NAME XML_EQ XML_VALUE          { store_attributes($1, $3); free($1); free($3);}
;
