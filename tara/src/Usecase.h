#ifndef USECASE_H
#define USECASE_H

#include <map>
#include <pnapi/pnapi.h>

void applyUsecase(
        pnapi::PetriNet* origin,
        pnapi::PetriNet* usecase,
        std::map<pnapi::Transition*,unsigned int>* costfunction
        );

#endif // USECASE_H
