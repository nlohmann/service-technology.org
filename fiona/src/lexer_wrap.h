#include "lexer.h"

#if (YY_FLEX_MAJOR_VERSION > 2) || \
    ((YY_FLEX_MAJOR_VERSION == 2) && (YY_FLEX_MINOR_VERSION > 5)) || \
    ((YY_FLEX_MAJOR_VERSION == 2) && (YY_FLEX_MINOR_VERSION == 5) && \
     (YY_FLEX_SUBMINOR_VERSION >= 31))
#define YY_FLEX_HAS_YYLEX_DESTROY
#endif
