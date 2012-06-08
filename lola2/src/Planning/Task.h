#pragma once

#include <cmdline.h>

class StatePredicate;
class SimpleProperty;
class Store;
class FireListCreator;
class ChooseTransition;


class Task
{
    private:
        StatePredicate* sp;
        ChooseTransition* choose;
        enum_search search;
        FireListCreator* flc;
        SimpleProperty* p;
        int number_of_threads;

    public:
        Store* s;

        Task();
        ~Task();

        bool getResult();

        void setThreads();
        void setFormula();
        void setStore();
        void setProperty();

        void printWitness();
        void printMarking();
};
