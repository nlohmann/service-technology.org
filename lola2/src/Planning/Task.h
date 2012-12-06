#pragma once

#include <cmdline.h>
#include <Core/Dimensions.h>
#include <Stores/Store.h>
#include <Exploration/LTLExploration.h>


class StatePredicate;
class SimpleProperty;
class Firelist;
class ChooseTransition;
class DFSExploration;
class NetState;


class Task
{
private:
    StatePredicate* sp;
    NetState* ns;
    ChooseTransition* choose;
    enum_search search;
    Firelist* fl;
    SimpleProperty* p;
    BuechiAutomata* bauto;
    DFSExploration* exploration;
    LTLExploration* ltlexploration;
    int number_of_threads;
    formula_t formulaType;
    void setNet();
    void setFormula();
    void setBuechiAutomata();

public:
    Store<void>* s;
    Store<AutomataTree>* sltl;

    Task();
    ~Task();

    bool getResult();
    void interpreteResult(bool);

    void setStore();
    void setProperty();

    void printWitness();
    void printMarking();

    void printDot();

    // this function is needed to allow the random walk in the main function
    NetState* getNetState();

    // this method is only to test Store::popState() until its use case is implemented
    void testPopState();
};
