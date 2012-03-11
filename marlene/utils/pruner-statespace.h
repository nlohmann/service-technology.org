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

    static unsigned int messageBound;

    static void
    output();

    static bool
    checkFinalReachable();

    static void
    calculateSCC();

    static void
    tauFolding();

    static void
    prune();

private:
    long number;
    ListOfPairs::List_ptr marking;
    ListOfPairs::List_ptr transitions;
    bool isFinal;
    bool finalReachable;
    bool finalEnforceable;
    unsigned int index;
    unsigned int lowlink;
    unsigned int lastMessagesReset;
    List<unsigned int>::List_ptr sccmember;
    std::map<std::string, unsigned int> messages;

    bool checkFinalReachable(std::map<unsigned int, bool>& seen
                        , unsigned int & count);

    void calculateSCC(unsigned int& index, unsigned int& lowlink,
                 std::map<unsigned int, bool>& seen, std::list<unsigned int>& stack);

    void output(std::map<unsigned int, bool>& seen);

    ListOfPairs::List_ptr taufolding(std::map<unsigned int, bool>& seen, std::map<unsigned int, bool>& statesFolded);

    void prune(std::map<unsigned int, bool>& seen
          , std::map<unsigned int, bool>& active, unsigned int & count);

};
