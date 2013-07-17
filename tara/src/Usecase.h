#ifndef USECASE_H
#define USECASE_H

#include <map>
#include <pnapi/pnapi.h>
#include <sstream>
#include "Modification.h"

class Usecase : public Modification {

    public:
        Usecase(
            pnapi::PetriNet* orig,
            pnapi::PetriNet* usecase,
            std::map<pnapi::Transition*,unsigned int>* costfunction,
            unsigned int i
        );
        virtual void init();

        virtual unsigned int getI();
        virtual void setToValue(unsigned int i);

    private:
        pnapi::PetriNet* net;
        unsigned int i;
        pnapi::Place* credit;
        pnapi::Place* invoice;
        pnapi::Place* finish;
        pnapi::Transition* pay_for_invoice;
        pnapi::formula::Formula* oldFormula;
};

#endif // USECASE_H
