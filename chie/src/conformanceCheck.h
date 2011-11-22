#ifndef CONFORMANCECHECK_H_
#define CONFORMANCECHECK_H_

#include "automata.h"
#include <vector>

typedef std::map<unsigned int, std::set<unsigned int> > searchCache;

bool isConformancePartner(ServiceAutomaton &, ServiceAutomaton &);
bool checkStrongReceivability(ServiceAutomaton &, ServiceAutomaton &, ProductAutomaton &);
bool checkWeakReceivability(ServiceAutomaton &, ServiceAutomaton &, ProductAutomaton &);
void checkWeakReceivabilityDFS(unsigned int, ProductAutomaton &, ServiceAutomaton &, std::vector<std::string> &, searchCache &, searchCache &, searchCache &);

#endif /* CONFORMANCECHECK_H_ */
