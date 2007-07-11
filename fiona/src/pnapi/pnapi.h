#ifndef PETRINET_API_H
#define PETRINET_API_H

#include <string>
#include <map>
#include "options.h"
#include "petrinet.h"

#ifndef PACKAGE_STRING
#define PACKAGE_STRING "Petri Net API"
#endif

using std::string;

class ASTE; 			// forward declaration of class ASTE

namespace globals {
  extern string filename;
  extern string output_filename;
  extern string invocation;
  extern map<possibleParameters, bool> parameters;
}

#endif
