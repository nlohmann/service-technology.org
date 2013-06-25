#ifndef USECASE_H
#define USECASE_H

#include <map>
#include <pnapi/pnapi.h>
#include "Modification.h"

class Usecase : public Modification {

    public:
        Usecase(
            pnapi::PetriNet* origin,
            pnapi::PetriNet* usecase,
            std::map<pnapi::Transition*,unsigned int>* costfunction,
            unsigned int i
        );

        virtual unsigned int getI();
        virtual void setToValue(unsigned int i);

    private:
        pnapi::PetriNet* net;
        unsigned int i;
};

#endif // USECASE_H
