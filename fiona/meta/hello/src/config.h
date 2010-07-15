/* src/config.h.  Generated from config.h.in by configure.  */
/* src/config.h.in.  Generated from configure.ac by autoheader.  */

/* The platform. */
#define CONFIG_BUILDSYSTEM "x86_64-unknown-linux-gnu"

/* Define to 1 if you have the `pnapi' library (-lpnapi). */
#define HAVE_LIBPNAPI 1

/* Define to 1 if you have the `mktemp' function. */
#define HAVE_MKTEMP 1

/* Define to 1 if you have the `pclose' function. */
#define HAVE_PCLOSE 1

/* Define to 1 if you have the `popen' function. */
#define HAVE_POPEN 1

/* Define to 1 if you have the `vfprintf' function. */
#define HAVE_VFPRINTF 1

/* Define to 1 if assertions should be disabled. */
/* #undef NDEBUG */

/* Define to 1 if your C compiler doesn't accept -c and -o together. */
/* #undef NO_MINUS_C_MINUS_O */

/* Name of package */
#define PACKAGE "hello"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "hello@service-technology.org"

/* Define to the full name of this package. */
#define PACKAGE_NAME "Hello"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "Hello 1.0-unreleased"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "hello"

/* Define to the home page for this package. */
#define PACKAGE_URL "http://service-technology.org/hello"

/* Define to the version of this package. */
#define PACKAGE_VERSION "1.0-unreleased"

/* awk */
#define TOOL_AWK "\"gawk\""

/* grep */
#define TOOL_GREP "\"/bin/grep\""

/* whether to use syslog */
/* #undef USE_SYSLOG */

/* Version number of package */
#define VERSION "1.0-unreleased"

/* The SVN revision. */
#define VERSION_SVN "5750M"

/* Define to 1 if `lex' declares `yytext' as a `char *' by default, not a
   `char[]'. */
#define YYTEXT_POINTER 1

#ifdef __cplusplus
#include <cassert>
#else
#include <assert.h>
#endif
