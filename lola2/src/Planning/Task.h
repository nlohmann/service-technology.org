#pragma once

#include <cmdline.h>

class StatePredicate;
class SimpleProperty;
class Store;
class Firelist;
class ChooseTransition;

class Task
{
    private:
        StatePredicate* sp;
        ChooseTransition* choose;
        enum_search search;
        Firelist* fl;
        SimpleProperty* p;

    public:
        Store* s;

        Task();
        ~Task();

        bool getResult();

        void setFormula();
        void setStore();
        void setProperty();

        void printWitness();
        void printMarking();
};
