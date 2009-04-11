#include "Label.h"
#include "InnerMarking.h"

using std::set;
using std::string;
using pnapi::Node;
using pnapi::Place;
using pnapi::Transition;


/******************
 * STATIC MEMBERS *
 ******************/

Label_ID Label::first_receive = 0;
Label_ID Label::last_receive = 0;
Label_ID Label::first_send = 0;
Label_ID Label::last_send = 0;
Label_ID Label::first_sync = 0;
Label_ID Label::last_sync = 0;
Label_ID Label::async_events = 0;
Label_ID Label::sync_events = 0;
Label_ID Label::events = 0;

std::map<Label_ID, string> Label::id2name;
std::map<string, Label_ID> Label::name2id;


/******************
 * STATIC METHODS *
 ******************/

void Label::initialize() {
    // ASYNCHRONOUS RECEIVE EVENTS (?-step for us)
    first_receive = 1;
    
    const set<Place*> outputPlaces = InnerMarking::net->getOutputPlaces();
    for (set<Place*>::const_iterator p = outputPlaces.begin(); p != outputPlaces.end(); ++p) {
        ++events;
        id2name[events] = "?" + (*p)->getName();
        
        const set<Node*> preset = (*p)->getPreset();
        for (set<Node*>::const_iterator t = preset.begin(); t != preset.end(); ++t) {
            name2id[(*t)->getName()] = events;
        }
    }
    
    last_receive = events;


    // ASYNCHRONOUS RECEIVE SEND (!-step for us)
    first_send = events+1;

    const set<Place*> inputPlaces= InnerMarking::net->getInputPlaces();

    for (set<Place*>::const_iterator p = inputPlaces.begin(); p != inputPlaces.end(); ++p) {
        ++events;
        id2name[events] = "!" + (*p)->getName();

        const set<Node*> postset = (*p)->getPostset();
        for (set<Node*>::const_iterator t = postset.begin(); t != postset.end(); ++t) {
            name2id[(*t)->getName()] = events;
        }
    }
    
    last_send = events;
    
    
    // SYNCHRONOUS EVENTS (!?-step for us)
    first_sync = events+1;
    std::map<string, Label_ID> sync_labels;
    
    const set<Transition*> trans = InnerMarking::net->getTransitions();
    
    for (set<Transition*>::const_iterator t = trans.begin(); t != trans.end(); ++t) {
        if ((*t)->isSynchronized()) {
            if ((*t)->getSynchronizeLabels().size() > 1) {
                fprintf(stderr, "%s: net is not normal!\n", PACKAGE);
                exit (EXIT_FAILURE);
            }
            
            if (sync_labels[*(*t)->getSynchronizeLabels().begin()] == 0) {
                sync_labels[*(*t)->getSynchronizeLabels().begin()] = ++events;
            }

            id2name[events] = "#" + *(*t)->getSynchronizeLabels().begin();            
            name2id[(*t)->getName()] = sync_labels[*(*t)->getSynchronizeLabels().begin()];
        }
    }

    /// \todo: events eins abziehen?
    
    last_sync = events;

    
    async_events = last_send;
    sync_events = last_sync - async_events;
    
//    fprintf(stderr, "%s: initialized labels for %d events (%d async, %d sync)\n",
//        PACKAGE, events, async_events, sync_events);
}
