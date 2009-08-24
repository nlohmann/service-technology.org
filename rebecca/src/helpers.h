#ifndef HELPERS_H
#define HELPERS_H

#include <vector>

using std::vector;


// forward declaration
class Peer;


/// checks whether each input has an output
bool sanityCheck(const vector<Peer *> &);


#endif /* HELPERS_H */
