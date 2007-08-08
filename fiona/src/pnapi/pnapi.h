#ifndef PETRINET_API_H
#define PETRINET_API_H





////////////////////
// BPEL2oWFN ONLY //
////////////////////

//#define USING_BPEL2OWFN

//#include "bpel2owfn.h"
//#include "ast-details.h"	// (class ASTE)
//#include "globals.h"





//////////////////
// GENERIC CODE //
//////////////////

#include "petrinet.h"		// to define member functions
#include "fiona.h"




/////////////////
// PN API ONLY //
/////////////////

// #warning "using Petri Net API outside BPEL2oWFN"
class ASTE; 			// forward declaration of class ASTE

//#ifndef PACKAGE_STRING
//#define PACKAGE_STRING "Petri Net API"
//#endif

#include <string>

namespace globals {
  extern std::string filename;
  extern std::string output_filename;
  extern std::string invocation;
  extern unsigned int reduction_level;
}

typedef enum
{
  TRACE_ERROR,		///< only trace error messages
  TRACE_ALWAYS,		///< trace level for errors
  TRACE_WARNINGS,       ///< trace level for warnings 
  TRACE_INFORMATION,	///< trace level some more (useful) information
  TRACE_DEBUG,          ///< trace level for detailed debug information
  TRACE_VERY_DEBUG	///< trace level for everything
} trace_level;

void trace(unsigned int a, std::string b);

#endif
