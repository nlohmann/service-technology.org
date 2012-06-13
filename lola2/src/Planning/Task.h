#pragma once

#include <cmdline.h>

class StatePredicate;
class SimpleProperty;
class Store;
class FireListCreator;
class ChooseTransition;
class DFSExploration;


class Task
{
    private:
        StatePredicate* sp;
        ChooseTransition* choose;
        enum_search search;
        FireListCreator* flc;
        SimpleProperty* p;
        DFSExploration* exploration;
        int number_of_threads;
        formula_t formulaType;
        void setFormula();

    public:
        Store* s;

        Task();
        ~Task();

        bool getResult();
        void interpreteResult(bool);

        void setStore();
        void setProperty();

        void printWitness();
        void printMarking();
};
