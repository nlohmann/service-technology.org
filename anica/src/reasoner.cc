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

pnapi::PetriNet net;
Reporter* rep = new ReporterStream();
Socket* inputSocket;
Socket* outputSocket;

typedef enum {
    CONFIDENCE_NONE = 0,
    CONFIDENCE_HIGH = 1,
    CONFIDENCE_LOW  = 2
} confidence_levels;

const char* levels[] = {"", "HIGH", "LOW"};

/// the current assignment
std::map<std::string, confidence_levels> assignment;

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

void sendAssignment() {
    std::string assignment_string = "{\n  \"assignment\": {";

    for (std::map<std::string, confidence_levels>::iterator it = assignment.begin(); it != assignment.end(); ++it) {
        if (it != assignment.begin()) {
            assignment_string += ",";
        }
        assignment_string += "\n    \"" + it->first + "\": \"" + levels[it->second] + "\"";
    }

    assignment_string += "\n  }\n}\n";

    outputSocket->send(assignment_string.c_str());
    rep->status("sent %d bytes", assignment_string.length());
}

void sendError(char* message) {
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

// receive a JSON object from the given input socket
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
    for (json_value* it = ass->first_child; it; it = it->next_sibling) {
        assert(it->name);
        assert(it->string_value);
        if (!strcmp(it->string_value, "HIGH")) {
            assignment[it->name] = CONFIDENCE_HIGH;
        } else {
            if (!strcmp(it->string_value, "LOW")) {
                assignment[it->name] = CONFIDENCE_LOW;
            } else {
                if (!strcmp(it->string_value, "")) {
                    assignment[it->name] = CONFIDENCE_NONE;
                } else {
                    rep->status("wrong confidence level %s", it->string_value);
                    rep->abort(ERROR_SYNTAX);
                }
            }
        }
    }

    rep->status("updated assignment");
}


int main() {
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
            updateNet(json);
            continue;
        }

        if (!strcmp(command, "assignment")) {
            updateAssignment(json);
            continue;
        }

        if (!strcmp(command, "terminate")) {
            rep->status("done");
            return EXIT_SUCCESS;
        }
    }
}
