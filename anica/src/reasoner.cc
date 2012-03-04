#include <string>

#include <pnapi/pnapi.h>
#include <cuddObj.hh>
#include <json.h>

#include "Socket.h"
#include "Reporter.h"

pnapi::PetriNet *net = NULL;
Socket *inputSocket = NULL;
Socket *outputSocket = NULL;
Reporter *rep = new ReporterStream();


void sendError() {
    /* create an error message for the GUI */
    outputSocket->send("your net has a problem");
}


void sendInformation() {
    /* send further information to the GUI */
    outputSocket->send("here is more information for you");
}


void receiveNet() {
    char *msg = inputSocket->receiveMessage();
    std::string netString(msg);
    free(msg);

    rep->status("received %d bytes", netString.length());

    /* create net from string netString */
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

std::string receiveCommand() {
    char *msg = inputSocket->receiveMessage();
    std::string result(msg);
    free(msg);

    rep->status("received command '%s'", result.c_str());

    return result;
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
        rep->status("waiting for command");
        std::string command = receiveCommand();

        if (command == "TERMINATE") {
            break;
        }

        if (command == "NET") {
            receiveNet();
            calculateCharacterization();
            continue;
        }

        if (command == "REASSIGNMENT") {
            receiveReassignment();
            evaluateReassignment();
            continue;
        }

        rep->status("wrong command '%s'", command.c_str());
        rep->abort(ERROR_SYNTAX);
    }

    rep->status("done");

    // tidy up
    delete inputSocket;
    delete outputSocket;
    delete rep;
    return 0;
}
