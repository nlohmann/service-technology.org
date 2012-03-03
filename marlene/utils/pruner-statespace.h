#pragma once

#include "config.h"

#include <map>
#include "pruner-helper.h"

typedef List< shared_ptr< Pair<std::string, unsigned int> > > ListOfPairs;

class State {

public:
  State(long number, ListOfPairs::List_ptr marking, ListOfPairs::List_ptr transitions, unsigned int lowlink, List<unsigned int>::List_ptr sccmember);
  ~State();

  static std::map< unsigned int, shared_ptr<State> > stateSpace;

  static void output();


private:
  long number;
  ListOfPairs::List_ptr marking;
  ListOfPairs::List_ptr transitions;
  unsigned int lowlink;
  List<unsigned int>::List_ptr sccmember;
};
