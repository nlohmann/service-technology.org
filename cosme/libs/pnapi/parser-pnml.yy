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

/// a mapping to store the attributes, accessible by the parse depth
std::map<unsigned int, std::map<std::string, std::string> > pnml_attributes;

/// a mapping to store created Petri net nodes, accessible by the "id"
std::map<std::string, Node*> pnml_id2node;
std::map<std::string, std::string> pnml_id2name;

// we only care about these elements
const char* whitelist[] = {"pnml", "net", "arc", "place", "transition", "inscription", "initialMarking", "annotation", "text", "module", "ports", "port", "input", "output", "synchronous", "receive", "send", "synchronize", "finalmarkings", "marking", "page", "name", NULL };
enum elementTypes { T_PNML, T_NET, T_ARC, T_PLACE, T_TRANSITION, T_INSCRIPTION, T_INITIALMARKING, T_ANNOTATION, T_TEXT, T_MODULE, T_PORTS, T_PORT, T_INPUT, T_OUTPUT, T_SYNCHRONOUS, T_RECEIVE, T_SEND, T_SYNCHRONIZE, T_FINALMARKINGS, T_MARKING, T_PAGE, T_NAME, T_NONE };

// a marking we build during parsing
pnapi::Marking* currentMarking;

unsigned int current_depth = 0;
unsigned int last_interesting_depth = 0;
bool ignoring = false;
unsigned int file_part = T_NONE;
std::vector<unsigned int> elementStack;

/// whether we care about a given element
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
    pnml_attributes[current_depth].clear();
}

std::string sanitize(char* s) {
    std::string str(s);
    char const* delims = " \t\r\n";

    // trim leading whitespace
    std::string::size_type  notwhite = str.find_first_not_of(delims);
    str.erase(0,notwhite);

    // trim trailing whitespace
    notwhite = str.find_last_not_of(delims); 
    str.erase(notwhite+1);

    return str;
}

void close_element() {
    unsigned int myType = T_NONE;
    
    if (!ignoring) {
        myType = elementStack[elementStack.size()-1];
        elementStack.pop_back();
    }

    switch(file_part) {
        case(T_PORTS): {
            std::string id = pnml_attributes[current_depth]["id"];
            std::string name = pnml_attributes[current_depth]["__name__"];
            //sanitize(name);
            std::string usedName = name.empty() ? id : name; 
            switch(myType) {
                case(T_INPUT): {
                    Place &p = pnapi_pnml_yynet.createPlace(usedName, Node::INPUT);
                    pnml_id2node[id] = &p;
                    break;
                }
                case(T_OUTPUT): {
                    Place &p = pnapi_pnml_yynet.createPlace(usedName, Node::OUTPUT);
                    pnml_id2node[id] = &p;
                    break;
                }
                case(T_SYNCHRONOUS): {
                    pnapi_pnml_yynet.addSynchronousLabel(usedName);
                    pnml_id2name[id] = usedName;
                    break;
                }
                case(T_TEXT): {
                    // check the parent
                    if (elementStack[elementStack.size()-1] == T_NAME) {
                        // store data as parent's attribute
                        pnml_attributes[current_depth-2]["__name__"] = pnml_attributes[current_depth]["__data__"];
                    }
                    break;
                }
            }
            break;
        }

        case(T_NET): {
            switch(myType) {
                case(T_PLACE): {
                    std::string id = pnml_attributes[current_depth]["id"];
                    std::string name = pnml_attributes[current_depth]["__name__"];
                    //sanitize(name);
                    std::string usedName = name.empty() ? id : name; 
                    Place &p = pnapi_pnml_yynet.createPlace(usedName, Node::INTERNAL, atoi(pnml_attributes[current_depth]["__initialMarking__"].c_str()));
                    pnml_id2node[id] = &p;
                    break;
                }
                case(T_TRANSITION): {
                    std::string id = pnml_attributes[current_depth]["id"];
                    std::string name = pnml_attributes[current_depth]["__name__"];
                    //sanitize(name);
                    std::string usedName = name.empty() ? id : name; 
                    pnapi::Transition *currentTransition = &pnapi_pnml_yynet.createTransition(usedName);
                    pnml_id2node[id] = currentTransition;

                    if (!pnml_attributes[current_depth]["__send__"].empty()) {
                        Node *outputPlace = pnml_id2node[pnml_attributes[current_depth]["__send__"]];
                        if (!outputPlace) {
                            yyerror(std::string("output place with id '" + pnml_attributes[current_depth]["__send__"] + "' not found").c_str());
                        }
                        pnapi_pnml_yynet.createArc(*currentTransition, *outputPlace);
                    }
                    if (!pnml_attributes[current_depth]["__receive__"].empty()) {
                        Node *inputPlace = pnml_id2node[pnml_attributes[current_depth]["__receive__"]];
                        if (!inputPlace) {
                            yyerror(std::string("input place with id '" + pnml_attributes[current_depth]["__receive__    "] + "' not found").c_str());
                        }
                        pnapi_pnml_yynet.createArc(*inputPlace, *currentTransition);
                    }
                    if (!pnml_attributes[current_depth]["__synchronize__"].empty()) {
                        std::string sync_name = pnml_id2name[pnml_attributes[current_depth]["__synchronize__"]];
                        std::set<std::string> labels = currentTransition->getSynchronizeLabels();
                        labels.insert(sync_name);
                        currentTransition->setSynchronizeLabels(labels);
                    }
                    break;
                }
                case(T_ARC): {
                    Node *source = pnml_id2node[pnml_attributes[current_depth]["source"]];
                    if (!source) {
                        yyerror(std::string("source node '" + pnml_attributes[current_depth]["source"] + "' not found").c_str());
                    }
                    Node *target = pnml_id2node[pnml_attributes[current_depth]["target"]];
                    if (!target) {
                        yyerror(std::string("target node '" + pnml_attributes[current_depth]["target"] + "' not found").c_str());
                    }
                    unsigned int weight = (pnml_attributes[current_depth]["__inscription__"].empty()) ? 1 : atoi(pnml_attributes[current_depth]["__inscription__"].c_str());
                    pnapi_pnml_yynet.createArc(*source, *target, weight);
                    break;
                }
                case(T_TEXT): {
                    // check the parent
                    if (elementStack[elementStack.size()-1] == T_INITIALMARKING) {
                    // store data as parent's attribute
                        pnml_attributes[current_depth-2]["__initialMarking__"] = pnml_attributes[current_depth]["__data__"];
                    }
                    if (elementStack[elementStack.size()-1] == T_INSCRIPTION) {
                        // store data as parent's attribute
                        pnml_attributes[current_depth-2]["__inscription__"] = pnml_attributes[current_depth]["__data__"];
                    }
                    // check the parent
                    if (elementStack[elementStack.size()-1] == T_NAME) {
                    // store data as parent's attribute
                        pnml_attributes[current_depth-2]["__name__"] = pnml_attributes[current_depth]["__data__"];
                    }
                    break;
                }
                case(T_RECEIVE): {
                    if (elementStack[elementStack.size()-1] == T_TRANSITION) {
                        pnml_attributes[current_depth-1]["__receive__"] = pnml_attributes[current_depth]["idref"];
                    }
                    break;
                }
                case(T_SEND): {
                    if (elementStack[elementStack.size()-1] == T_TRANSITION) {
                        pnml_attributes[current_depth-1]["__send__"] = pnml_attributes[current_depth]["idref"];
                    }
                    break;
                }
                case(T_SYNCHRONIZE): {
                    if (elementStack[elementStack.size()-1] == T_TRANSITION) {
                        pnml_attributes[current_depth-1]["__synchronize__"] = pnml_attributes[current_depth]["idref"];
                    }
                    break;
                }
            }
            break;
        }

        case(T_FINALMARKINGS): {
            switch(myType) {
                case(T_MARKING): {
                    pnapi_pnml_yynet.finalCondition().addMarking(*currentMarking);
                    delete currentMarking;
                    break;
                }

                case(T_PLACE): {
                    unsigned int tokens = atoi(pnml_attributes[current_depth]["__finalMarking__"].c_str());
                    Node *place = pnml_id2node[pnml_attributes[current_depth]["idref"]];
                    if (!place) {
                        yyerror(std::string("place with id '" + pnml_attributes[current_depth]["idref"] + "' not found").c_str());
                    }
                    (*currentMarking)[(const pnapi::Place&)*place] = tokens;
                }

                case(T_TEXT): {
                    // check the parent
                    if (elementStack[elementStack.size()-1] == T_PLACE) {
                    // store data as parent's attribute
                        pnml_attributes[current_depth-1]["__finalMarking__"] = pnml_attributes[current_depth]["__data__"];
                    }
                }
                break;
            }
            break;
        }

        default: {}
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
        pnml_attributes[current_depth][a1] = std::string(a2).substr(1, strlen(a2)-2);
    }
}

void store_data(char *s) {
    if (!ignoring) {
        // store current data as attribute of surrouding element
        pnml_attributes[current_depth]["__data__"] = sanitize(s);
    }
}

%}


 /*****************************************************************************
  * types, tokens, start symbol
  ****************************************************************************/

%union {char *s;}

%token XML_ENDDEF XML_EQ XML_SLASH XML_CLOSE XML_END
%token <s> XML_NAME XML_VALUE XML_DATA XML_START
%type <s> name_opt


 /*****************************************************************************
  * grammar rules 
  ****************************************************************************/
%%

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
