/*!
 * \file verbose.cc
 */

#include "config.h"

#include "verbose.h"

#include <cstdarg>
#include <cstdlib>
#include <fstream>

namespace pnapi
{

namespace verbose
{

/*!
 * \brief print a verbose output if desired
 */
void (*status)(const char *, ...) = quiet;

/*!
 * \brief target for function pointer "status"
 */
void quiet(const char *, ...)
{
}

/*! 
 * \brief default output function printing to standard error
 * 
 * \param format  the status message formatted as printf string
 */
void defaultStatus(const char * format, ...)
{
      fprintf(stderr, "PNAPI: ");

      va_list args;
      va_start(args, format);
      vfprintf(stderr, format, args);
      va_end(args);

      fprintf(stderr, "\n");  
}

} /* namespace verbose */

} /* namespace pnapi */
