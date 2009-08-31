#ifndef REBECCAIO_H
#define REBECCAIO_H

#include <ostream>
#include <set>
#include <string>
#include "peerautomaton.h"


std::ostream & operator <<(std::ostream &, const PeerAutomaton &);

std::ostream & operator <<(std::ostream &, const std::set<std::string> &);
std::ostream & operator <<(std::ostream &, const std::set<int> &);

#endif /* REBECCAIO_H */
