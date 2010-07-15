/* src/config.h.  Generated from config.h.in by configure.  */
/* src/config.h.in.  Generated from configure.ac by autoheader.  */

/* The platform. */
#define CONFIG_BUILDSYSTEM "x86_64-unknown-linux-gnu"

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the `pnapi' library (-lpnapi). */
#define HAVE_LIBPNAPI 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the `mktemp' function. */
#define HAVE_MKTEMP 1

/* Define to 1 if you have the `pclose' function. */
#define HAVE_PCLOSE 1

/* Define to 1 if you have the `popen' function. */
#define HAVE_POPEN 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to 1 if you have the `vfprintf' function. */
#define HAVE_VFPRINTF 1

/* Define to 1 if assertions should be disabled. */
/* #undef NDEBUG */

/* Define to 1 if your C compiler doesn't accept -c and -o together. */
/* #undef NO_MINUS_C_MINUS_O */

/* Name of package */
#define PACKAGE "cosme"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "cosme@service-technology.org"

/* Define to the full name of this package. */
#define PACKAGE_NAME "Cosme"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "Cosme 0.7"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "cosme"

/* Define to the home page for this package. */
#define PACKAGE_URL "http://service-technology.org/cosme"

/* Define to the version of this package. */
#define PACKAGE_VERSION "0.7"

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* awk */
#define TOOL_AWK "\"gawk\""

/* grep */
#define TOOL_GREP "\"/bin/grep\""

/* whether to use syslog */
/* #undef USE_SYSLOG */

/* Version number of package */
#define VERSION "0.7"

/* The SVN revision. */
#define VERSION_SVN "5755"

/* Define to 1 if `lex' declares `yytext' as a `char *' by default, not a
   `char[]'. */
#define YYTEXT_POINTER 1

/* Define for Solaris 2.5.1 so the uint32_t typedef from <sys/synch.h>,
   <pthread.h>, or <semaphore.h> is not used. If the typedef were allowed, the
   #define below would cause a syntax error. */
/* #undef _UINT32_T */

/* Define for Solaris 2.5.1 so the uint8_t typedef from <sys/synch.h>,
   <pthread.h>, or <semaphore.h> is not used. If the typedef were allowed, the
   #define below would cause a syntax error. */
/* #undef _UINT8_T */

/* Define to the type of an unsigned integer type of width exactly 16 bits if
   such a type exists and the standard includes do not define it. */
/* #undef uint16_t */

/* Define to the type of an unsigned integer type of width exactly 32 bits if
   such a type exists and the standard includes do not define it. */
/* #undef uint32_t */

/* Define to the type of an unsigned integer type of width exactly 8 bits if
   such a type exists and the standard includes do not define it. */
/* #undef uint8_t */

#ifdef __cplusplus
#include <cassert>
#else
#include <assert.h>
#endif
