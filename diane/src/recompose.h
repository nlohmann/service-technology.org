/*
 *  recompose.h
 *  
 *
 *  Created by Olivia Oanea on 8/14/10.
 *  Copyright 2010 University of Rostock. All rights reserved.
 *
 */

#include <sstream>
#include <string>
#include <set>
#include "verbose.h"
#include <vector>
#include "pnapi/petrinet.h"
#include <sstream>

using pnapi::Place;
using pnapi::Transition;
using pnapi::Label;
using pnapi::PetriNet;
using pnapi::Port;
using std::string;
using std::iostream;
using std::stringstream;
using std::set;
using std::map;
using std::cerr;
using std::cin;
using std::cout;
using std::endl;

PetriNet deletePattern(PetriNet nnn);

PetriNet addPattern(PetriNet nnn);

PetriNet addinterfcompl(PetriNet nnn,set<string> inputplaces1, set<string> outputplaces1);

PetriNet complementnet(PetriNet diffc, PetriNet part);

// check if components are composable (without composing them)

bool areComposable(PetriNet net1, PetriNet net2);

PetriNet deleteEmptyPorts(PetriNet net1);

bool noInterfaceLabel(set<string> sset, string sub);

//my compose function?

//given a Petri net, separate its labels in different ports

PetriNet separatePorts(PetriNet net);