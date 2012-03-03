
#include <iostream>
#include "pruner-statespace.h"

std::map< unsigned int, shared_ptr<State> > State::stateSpace;


State::State(long number, ListOfPairs::List_ptr marking, ListOfPairs::List_ptr transitions, unsigned int lowlink, List<unsigned int>::List_ptr sccmember) {
  this->number = number;
  this->marking = marking;
  this->transitions = transitions;
  this->lowlink = lowlink;
  this->sccmember = sccmember;
}

State::~State() {
  this->marking = ListOfPairs::List_ptr();
  this->transitions = ListOfPairs::List_ptr();
  this->sccmember = List<unsigned int>::List_ptr();
}


void State::output() {

  using namespace std;

  for (map< unsigned int, shared_ptr<State> >::const_iterator state = stateSpace.begin(); state != stateSpace.end(); ++state) {
    cout << "STATE " << state->second->number << " Lowlink: " << state->second->lowlink;
    if (state->second->sccmember.get() != 0) {
      cout << " SCC:";
      for (List<unsigned int>::List_ptr next = state->second->sccmember; next.get() != 0; cout << " " << next->getValue(), next = next->getNext());
    }
    if (state->second->marking.get() != 0) {
      bool first = true;
      for (ListOfPairs::List_ptr marking = state->second->marking; marking.get() != 0;
           cout << (first?"":",") << endl << marking->getValue()->getFirst() << " : " << marking->getValue()->getSecond(), marking = marking->getNext(), first = false);

    }
    cout << endl;
    if (state->second->transitions.get() != 0) {
      for (ListOfPairs::List_ptr transition = state->second->transitions; transition.get() != 0;
           cout << endl << transition->getValue()->getFirst() << " -> " << transition->getValue()->getSecond(), transition = transition->getNext());

    }
    cout << endl;
    cout << endl;
  }
}

