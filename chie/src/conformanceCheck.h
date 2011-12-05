#ifndef CONFORMANCECHECK_H_
#define CONFORMANCECHECK_H_

#include "automata.h"
#include <vector>

// cache type for weak receivability searches
typedef std::map<unsigned int, std::set<unsigned int> > searchCache;

// whether two service automata are conformance partners
bool isConformancePartner(ServiceAutomaton &, ServiceAutomaton &);

// whether messages from the first automaton are strong receivable in the second automaton
bool checkStrongReceivability(ServiceAutomaton &, ServiceAutomaton &, ProductAutomaton &);

// whether messages from the second automaton are weak receivable in the first automaton
bool checkWeakReceivability(ServiceAutomaton &, ServiceAutomaton &, ProductAutomaton &);

#endif /* CONFORMANCECHECK_H_ */
