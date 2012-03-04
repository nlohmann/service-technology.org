#include <string>

#include <pnapi/pnapi.h>
#include <cuddObj.hh>
#include <json.h>
#include <b64/encode.h>
#include <b64/decode.h>

#include "Socket.h"
#include "Reporter.h"

pnapi::PetriNet* net = NULL;
Socket* inputSocket = NULL;
Socket* outputSocket = NULL;
Reporter* rep = new ReporterStream();


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

void sendError() {
    /* create an error message for the GUI */
    outputSocket->send("your net has a problem");
}


void sendInformation() {
    /* send further information to the GUI */
    outputSocket->send("here is more information for you");
}


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

void receiveNet() {
}


void receiveReassignment() {
    /* update local assignment */
}


void calculateCharacterization() {
    /* calculate BDD */

    /* in case of error, call sendError() */
}


void evaluateReassignment() {
    /* do stuff with the BDD */
}


int main() {
    int inputPort = 5556;
    int outputPort = 5555;


//    base64::encoder E;
//    E.encode(std::cin, std::cout);


    // initialize ports
    rep->status("listening to socket %d", inputPort);
    inputSocket = new Socket(inputPort);
    rep->status("talking to socket %d", outputPort);
    outputSocket = new Socket(outputPort, "localhost");

    // state machine
    while (true) {
        rep->status("waiting for messages");

        json_value* json = receiveJson();
        assert(json);
        print_json(json);
    }

    rep->status("done");

    // tidy up
    delete inputSocket;
    delete outputSocket;
    delete rep;
    return 0;
}
