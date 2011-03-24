#include "Transition.h"

unsigned int Transition::cnt = 0;

Transition* LastAttractor;  // Last transition in static attractor sets
unsigned int Transition::NrEnabled = 0;
Transition* Transition::StartOfEnabledList = NULL;

#ifdef STUBBORN
unsigned int Transition::NrStubborn = 0;
Transition* Transition::TarjanStack = NULL;
Transition* Transition::CallStack = NULL;
#endif

#ifdef EXTENDED
Transition* Transition::StartOfIgnoredList = NULL;
#endif

#ifdef STUBBORN
Transition* Transition::StartOfStubbornList = NULL;
Transition* Transition::EndOfStubbornList = NULL;
#endif
