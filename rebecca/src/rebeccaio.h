#ifndef REBECCAIO_H
#define REBECCAIO_H

#include "choreography.h"
#include "peerautomaton.h"
#include <ostream>
#include <set>
#include <string>


std::ostream & operator <<(std::ostream &, const PeerAutomaton &);
std::ostream & operator <<(std::ostream &, const Choreography &);

std::ostream & operator <<(std::ostream &, const std::set<std::string> &);
std::ostream & operator <<(std::ostream &, const std::set<int> &);

#endif /* REBECCAIO_H */
