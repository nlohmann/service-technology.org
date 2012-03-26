#include <config.h>

#include <cstring>
#include <sstream>
#include <cstdio>
#include <map>
#include <string>

#include <pnapi/pnapi.h>
#include <cuddObj.hh>
#include <json.h>
#include <b64/decode.h>

#include "Socket.h"
#include "Reporter.h"

#include "AnicaLib.h"

pnapi::PetriNet net;
Reporter* rep = new ReporterStream();
Socket* inputSocket;
Socket* outputSocket;

anica::AnicaLib* alib;
char** cuddVariableNames;
BDD* cuddOutput;
Cudd* myBDD;
std::map<std::string, int> cuddVariables;

const char* levels[] = {"", "LOW", "HIGH"};

/// the current assignment
std::map<std::string, anica::confidence_e> assignment;

/////////////////////////////////////////////////////////////////////////////

#define IDENT(n) for (int i = 0; i < n; ++i) printf("    ")

void print_json(json_value* value, int ident = 0) {
    IDENT(ident);
    if (value->name) {
        printf("\"%s\" = ", value->name);
    }
    switch (value->type) {
        case JSON_NULL:
            printf("null\n");
            break;
        case JSON_OBJECT:
        case JSON_ARRAY:
            printf(value->type == JSON_OBJECT ? "{\n" : "[\n");
            for (json_value* it = value->first_child; it; it = it->next_sibling) {
                print_json(it, ident + 1);
            }
            IDENT(ident);
            printf(value->type == JSON_OBJECT ? "}\n" : "]\n");
            break;
        case JSON_STRING:
            printf("\"%s\"\n", value->string_value);
            break;
        case JSON_INT:
            printf("%d\n", value->int_value);
            break;
        case JSON_FLOAT:
            printf("%f\n", value->float_value);
            break;
        case JSON_BOOL:
            printf(value->int_value ? "true\n" : "false\n");
            break;
    }
}

/////////////////////////////////////////////////////////////////////////////

/// send an assignment as JSON to the output socket
void sendAssignment() {
    assert(assignment.size() == net.getTransitions().size());

    std::string assignment_string = "{\n  \"assignment\": {";

    for (std::map<std::string, anica::confidence_e>::iterator it = assignment.begin(); it != assignment.end(); ++it) {
        if (it != assignment.begin()) {
            assignment_string += ",";
        }
        assignment_string += "\n    \"" + it->first + "\": \"" + levels[it->second] + "\"";
    }

    assignment_string += "\n  }\n}\n";

    outputSocket->send(assignment_string.c_str());
    rep->status("sent %d bytes", assignment_string.length());
}


/// send an error message as JSON to the output socket
void sendError(const char* message) {
    // create message
    char* msg;
    asprintf(&msg, "{\n  \"error\": \"%s\"\n}", message);
    assert(msg);

    // send message
    outputSocket->send(msg);
    rep->status("sent %d bytes", strlen(msg));

    // tidy up
    free(msg);
}


/// receive a JSON object from the given input socket
json_value* receiveJson() {
    char* msg = inputSocket->receiveMessage();

    rep->status("received %d bytes", strlen(msg));

    char* errorPos = 0;
    char* errorDesc = 0;
    int errorLine = 0;
    block_allocator allocator(1 << 10); // 1 KB per block

    json_value* root = json_parse(msg, &errorPos, &errorDesc, &errorLine, &allocator);

    if (root == NULL) {
        rep->status("error at line %d - %s", errorLine, errorDesc);
        rep->abort(ERROR_SYNTAX);
    }

    return root;
}


/// get the command from JSON object
const char* getCommand(json_value* json) {
    assert(json->type == JSON_OBJECT);

    for (json_value* it = json->first_child; it; it = it->next_sibling) {
        if (it->name && !strcmp(it->name, "command")) {
            rep->status("command: %s", it->string_value);
            return it->string_value;
        }
    }

    rep->status("no command found");
    rep->abort(ERROR_SYNTAX);
    return "";
}


/// update net from JSON object
void updateNet(json_value* json) {
    // 1. strip net string from JSON
    char* netstring = NULL;

    assert(json->type == JSON_OBJECT);
    for (json_value* it = json->first_child; it; it = it->next_sibling) {
        if (it->name && !strcmp(it->name, "net")) {
            netstring = it->string_value;
        }
    }

    if (netstring == NULL) {
        rep->status("no net found");
        rep->abort(ERROR_SYNTAX);
    }


    // 2. decode net string from Base64
    std::stringstream n1;
    std::stringstream n2;
    base64::decoder D;
    n1 << netstring;
    D.decode(n1, n2);


    // 3. parse the net
    try {
        n2 >> pnapi::io::lola >> net;
    } catch (const pnapi::exception::InputError& error) {
        std::stringstream ss;
        ss << error;
        rep->status("parse error: %s", ss.str().c_str());
        rep->abort(ERROR_SYNTAX);
    }

    std::stringstream ss;
    ss << pnapi::io::stat << net;
    rep->status("received net: %s", ss.str().c_str());


    // 4. set assignment
    assignment.clear();
    PNAPI_FOREACH(t, net.getTransitions()) {
        assignment[(*t)->getName()] = static_cast<anica::confidence_e>((*t)->getConfidence());
    }
}


/// update assignment from JSON object
void updateAssignment(json_value* json) {
    // 1. get assignment
    assert(json->type == JSON_OBJECT);

    json_value* ass = NULL;
    for (json_value* it = json->first_child; it; it = it->next_sibling) {
        if (it->name && !strcmp(it->name, "assignment")) {
            ass = it;
        }
    }

    if (ass == NULL) {
        rep->status("no assignment found");
        rep->abort(ERROR_SYNTAX);
    }

    // 2. update assignment
    assignment.clear();
    for (json_value* it = ass->first_child; it; it = it->next_sibling) {
        assert(it->name);
        assert(it->string_value);
        if (!strcmp(it->string_value, "HIGH")) {
            assignment[it->name] = anica::CONFIDENCE_HIGH;
        } else {
            if (!strcmp(it->string_value, "LOW")) {
                assignment[it->name] = anica::CONFIDENCE_LOW;
            } else {
                if (!strcmp(it->string_value, "")) {
                    assignment[it->name] = anica::CONFIDENCE_NONE;
                } else {
                    rep->status("wrong confidence level %s", it->string_value);
                    rep->abort(ERROR_SYNTAX);
                }
            }
        }
    }

    assert(assignment.size() == net.getTransitions().size());

    rep->status("updated assignment");
}


/// collect more information on the assignment
void updateNonInterference() {
    assert(alib != NULL);
    
    // evaluate non-interference
    if (alib->getHighLabeledTransitionsCount() + alib->getLowLabeledTransitionsCount() == net.getTransitions().size()) {
        // net has only high and low labeled transitions
        if (alib->isSecure() == 0) {
            rep->status("assignment is secure");
            sendAssignment();
        }
        else {
            sendError("assignment is not secure");
            rep->status("assignment is NOT secure");
        }
    }
    else {
        if (alib->getDownLabeledTransitionsCount() == 0) {
            cuddOutput = new BDD();
            myBDD = alib->getCharacterization(cuddVariableNames, cuddOutput, cuddVariables);
            
            rep->status("characterization calculated");
            
            // check if valid
            if (cuddOutput->IsZero()) {
                sendError("assignment is not secure");
                rep->status("assignment is NOT secure");
                return;
            }
            else if (cuddOutput->IsOne()) {
                return;
            }
            
            // check implications for all transitions
            int* cube; // a pointer to an array of ints
            CUDD_VALUE_TYPE value; // useless required variable
            cuddOutput->FirstCube(&cube, &value); // get an assignment
            
            // report implied assignments
            /*
            PNAPI_FOREACH(t, net.getTransitions()) {
                if ((**t).getConfidence() == anica::CONFIDENCE_NONE) {
                    const std::string transitionName = (**t).getName();
                    const int bddIndex = cuddVariables[transitionName];
                    
                    switch (cube[bddIndex]) {
                        case 0:
                            // variable must be false
                            rep->status("%s -> LOW", transitionName.c_str());
                            assignment[transitionName] = anica::CONFIDENCE_LOW;
                            break;
                        case 1:
                            // variable must be true
                            rep->status("%s -> HIGH", transitionName.c_str());
                            assignment[transitionName] = anica::CONFIDENCE_HIGH;
                            break;
                        case 2:
                            // variable is "free"
                            break;
                        default:
                            // should not happen
                            assert(false);
                    }
                }
            }
            */
            
            PNAPI_FOREACH(t, net.getTransitions()) { 
                if ((**t).getConfidence() == anica::CONFIDENCE_NONE) { 
                    const std::string transitionName = (**t).getName(); 
                    const int bddIndex = cuddVariables[transitionName]; 
                    rep->status("..checking: %s", transitionName.c_str()); 
                    
                    // is high implied? 
                    if ((!*cuddOutput + myBDD->bddVar(bddIndex)) == myBDD->bddOne()) { 
                        // yes 
                        rep->status("....HIGH"); 
                        assignment[transitionName] = anica::CONFIDENCE_HIGH; 
                    } 
                    else { 
                        // no 
                        // is low implied? 
                        if ((!*cuddOutput + !myBDD->bddVar(bddIndex)) == myBDD->bddOne()) { 
                            rep->status("....LOW"); 
                            assignment[transitionName] = anica::CONFIDENCE_LOW; 
                        }
                    }
                }
            }
            
            // delete current BDD structures
            delete cuddOutput;
            cuddOutput = NULL;
            delete myBDD;
            myBDD = NULL;
            cuddVariables.clear();

            // send back all implications (-> new assignment)
            sendAssignment();
        }
        else {
            rep->status("ERROR: assignment contains DOWNGRADES");
            rep->abort(ERROR_SYNTAX);
        }
    }
}


int main(int argc, char** argv) {
    if (UNLIKELY(argc == 2 and !strcmp(argv[1], "--help")))
    {
        printf("No help\n");
        return EXIT_SUCCESS;
    }

    if (UNLIKELY(argc == 2 and !strcmp(argv[1], "--version")))
    {
        printf("No version\n");
        return EXIT_SUCCESS;
    }

    int inputPort = 5556;
    int outputPort = 5555;

    // initialize ports
    rep->status("listening to socket %d", inputPort);
    inputSocket = new Socket(inputPort);
    rep->status("talking to socket %d", outputPort);
    outputSocket = new Socket(outputPort, "localhost");

    // state machine
    while (true) {
        rep->status("waiting for messages");

        json_value* json = receiveJson();
        const char* command = getCommand(json);

        if (!strcmp(command, "net")) {
            delete alib;
            for (size_t i = 0; i < net.getTransitions().size(); ++i) {
                free(cuddVariableNames[i]);
            }
            delete[] cuddVariableNames;
            
            updateNet(json);
            
            cuddVariableNames = new char*[net.getTransitions().size()];
            
            alib = new anica::AnicaLib(net);
            alib->setLolaPath(LOLA_STATEPREDICATE1);
            
            updateNonInterference();
            
            continue;
        }

        if (!strcmp(command, "assignment")) {
            updateAssignment(json);
            
            // if a net is present
            if (alib != NULL) {
                // update all transitions with new confidence
                PNAPI_FOREACH(t, net.getTransitions()) {
                    const std::string transitionName = (**t).getName();
                    assert(assignment.find(transitionName) != assignment.end());
                    alib->assignTransition(transitionName, assignment[transitionName]);       
                }
                updateNonInterference();
            }
            
            continue;
        }

        if (!strcmp(command, "terminate")) {
            rep->status("done");
            return EXIT_SUCCESS;
        }
    }
}
