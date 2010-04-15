#ifndef PNAPI_VERBOSE_H
#define PNAPI_VERBOSE_H

/*!
 * \file verbose.h
 */

namespace pnapi
{

/*!
 * \brief Handle for verbose output
 */
namespace verbose
{

/// print a verbose output if desired
extern void (*status)(const char *, ...);
/// target for function pointer "status"
void quiet(const char *, ...);
/// default output function printing to standard error
void defaultStatus(const char *, ...);

} /* namespace verbose */

} /* namespace pnapi */

#endif /* PNAPI_VERBOSE_H */
