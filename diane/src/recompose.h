/*
 *  recompose.h
 *
 *
 *  Created by Olivia Oanea on 8/14/10.
 *  Copyright 2010 University of Rostock. All rights reserved.
 *
 */

#ifndef RECOMPOSE_H
#define RECOMPOSE_H

#include <sstream>
#include <string>
#include <set>
#include "verbose.h"
#include <vector>
#include "pnapi/petrinet.h"
#include <sstream>
#include <fstream>
#include <iomanip>
#include <iostream>
#include "cmdline.h"
//#include "verbose.h"
#include "config.h"
#include "config-log.h"
#include "algorithm"


using std::cerr;
using std::cin;
using std::cout;
using std::endl;
using std::ifstream;
using std::map;
using std::ofstream;
using std::setfill;
using std::setw;
using std::string;
using std::stringstream;
using std::vector;
using std::set;
using std::deque;
using pnapi::PetriNet;
using pnapi::Place;
using pnapi::Label;
using pnapi::Node;
using pnapi::Transition;

extern string  filename;

typedef struct component {
    PetriNet process; // the initial process
    string cname; //component name used to derive the component file name
    set<string> inputl;//set of input labels
    set<string> outputl;//set of output labels
    unsigned int sizep;// number of places of the component which determines recomposition
} Component;

// builds a component
Component makeComponent(std::pair<vector<bool>, PetriNet> c, PetriNet process);
// checks whether two components are composable based on their interfaces (ports are liberal)
bool areAsyncComposable(Component c1, Component c2);
//composes two components into a new one of the original process (without computing the structure)
Component asyncCompose(Component c1, Component c2);
// creates a component net, its component and the composition

// builds a net from a Component structure
PetriNet makeComponentNet(Component c);

//given a set of componentss  of a process recomposes them to medium sized components
vector<Component> recompose(vector<PetriNet*> nets, PetriNet net);


//namespace recompose {


PetriNet addComplementaryPlaces(PetriNet c);

PetriNet deletePattern(PetriNet nnn);

PetriNet addPattern(PetriNet nnn);

PetriNet addinterfcompl(PetriNet nnn, set<string> inputplaces1, set<string> outputplaces1);

PetriNet complementnet(PetriNet diffc, PetriNet part);

// check if components are composable (without composing them)

bool areComposable(PetriNet net1, PetriNet net2);


bool noInterfaceLabel(set<string> sset, string sub);

vector<string> setPlaceOrder(PetriNet net);


template <class T>
std::set<T> setUnion(const std::set<T> & a, const std::set<T> & b) {
    std::set<T> result;
    std::insert_iterator<std::set<T, std::less<T> > > res_ins(result, result.begin());
    set_union(a.begin(), a.end(), b.begin(), b.end(), res_ins);

    return result;
}


//}

#endif
