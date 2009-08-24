#ifndef IO_H
#define IO_H

#include <ostream>
#include <set>
#include <string>
#include "peerautomaton.h"


std::ostream & operator <<(std::ostream &, const PeerAutomaton &);

std::ostream & operator <<(std::ostream &, const std::set<std::string> &);
std::ostream & operator <<(std::ostream &, const std::set<int> &);

#endif /* IO_H */
