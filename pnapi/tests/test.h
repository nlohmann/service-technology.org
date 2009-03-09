// -*- C++ -*-

#include <cassert>
#include <string>
#include <iostream>

#include "pnapi.h"

using std::cout;
using std::endl;

using pnapi::PetriNet;
using pnapi::Place;
using pnapi::Transition;
using pnapi::Arc;
using pnapi::Node;

using pnapi::Automaton;


void begin_test(const std::string &);

void skip_test();

void end_test();

PetriNet createExamplePetriNet();
