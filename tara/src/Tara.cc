#include "Tara.h"


unsigned int Tara::highestTransitionCosts = 0;
unsigned int Tara::sumOfLocalMaxCosts = 0;
unsigned int Tara::initialState = 0;

std::map<pnapi::Transition* ,unsigned int> Tara::partialCostFunction;

pnapi::PetriNet* Tara::net = 0; 

std::deque<innerState *> Tara::graph;

unsigned int Tara::cost(pnapi::Transition* t) {
   std::map<pnapi::Transition*,unsigned int>::iterator cost = Tara::partialCostFunction.find(t);
   if(cost==partialCostFunction.end()) 
      return 0;

   return cost->second;
}

