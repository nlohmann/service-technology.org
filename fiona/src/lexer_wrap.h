#include "lexer.h"

// #defines YY_FLEX_HAS_YYLEX_DESTROY for newer versions of flex. If
// YY_FLEX_HAS_YYLEX_DESTROY is defined, you should call yylex_destroy() after
// parsing is complete.

#if (YY_FLEX_MAJOR_VERSION > 2) || \
    ((YY_FLEX_MAJOR_VERSION == 2) && (YY_FLEX_MINOR_VERSION > 5)) || \
    ((YY_FLEX_MAJOR_VERSION == 2) && (YY_FLEX_MINOR_VERSION == 5) && \
     (YY_FLEX_SUBMINOR_VERSION >= 31))
#define YY_FLEX_HAS_YYLEX_DESTROY
#endif
