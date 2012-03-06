#pragma once

#include "config.h"

#include <map>
#include <list>
#include "pruner-helper.h"

typedef List<shared_ptr<Pair<std::string, unsigned int> > > ListOfPairs;

class State
{

public:
  State(long number, ListOfPairs::List_ptr marking,
      ListOfPairs::List_ptr transitions, bool isFinal, unsigned int lowlink,
      List<unsigned int>::List_ptr sccmember);
  ~State();

  static std::map<unsigned int, shared_ptr<State> > stateSpace;

  static std::map< std::string, std::string > abstractedIn;
  static std::map< std::string, std::string > abstractedOut;
  static std::map< std::string, std::string > abstractedSync;

  static std::map< std::string, std::string > remainingIn;
  static std::map< std::string, std::string > remainingOut;
  static std::map< std::string, std::string > remainingSync;

  static void
  output();

  static bool
  checkFinalReachable();

  static void
  calculateSCC();

  static void
  prune();

private:
  long number;
  ListOfPairs::List_ptr marking;
  ListOfPairs::List_ptr transitions;
  bool isFinal;
  bool finalReachable;
  unsigned int index;
  unsigned int lowlink;
  List<unsigned int>::List_ptr sccmember;

  static bool
  checkFinalReachable(unsigned int stateNo, std::map<unsigned int, bool>& seen
      , unsigned int & count);

  static void
  calculateSCC(unsigned int stateNo, unsigned int& index, unsigned int& lowlink,
      std::map<unsigned int, bool>& seen, std::list<unsigned int>& stack);

  static void
  output(unsigned int stateNo, std::map<unsigned int, bool>& seen);

  static void
  prune(unsigned int stateNo, std::map<unsigned int, bool>& seen
      , unsigned int & count);

};
