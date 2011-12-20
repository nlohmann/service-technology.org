#ifndef DOT_H_
#define DOT_H_

#include "automata.h"
#include <string>

// write automaton do dot file with given filename
void writeToDotFile(ProductAutomaton &, std::string &, bool = true);

#endif /* DOT_H_ */
