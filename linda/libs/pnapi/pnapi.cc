#include "pnapi.h"

namespace pnapi
{


/*!
 \brief return a version string for customer tools

 \note We cannot use the macros of the config.h file, because this might be
       unreachable due to the customer tool

 \note src/pnapi.cc.  Generated from pnapi.cc.in by configure.
*/
std::string version() {
  return "Petri Net API 4.01-unreleased\nhttp://service-technology.org/pnapi";
}


} /* namespace pnapi */

