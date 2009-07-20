#ifndef MACROS_H
#define MACROS_H

#include <iostream>

#include "config.h"

extern bool veryverbose;
extern unsigned int veryverboseindent;

#define INDENT for (unsigned int i = 0; i < veryverboseindent; i++) { std::cerr << " "; }
#define INDENTP veryverboseindent++;
#define INDENTM if (veryverboseindent > 0) { veryverboseindent--; }
#define CHECK INDENT std::cerr << PACKAGE << ": check " << __FILE__ << "@" << __LINE__ << std::endl;

#if !defined(NDEBUG) || NDEBUG == 0
#ifdef __GNUG__
#define FUNC std::cerr << " ... in function " << __PRETTY_FUNCTION__ << " in " << __FILE__ << "@" << __LINE__ << std::endl;
#define FUNCIN  if (veryverbose) { std::cerr << PACKAGE << ": "; INDENT INDENTP std::cerr << " enter function " << __PRETTY_FUNCTION__ << " in " << __FILE__ << "@" << __LINE__ << std::endl; }
#define FUNCOUT if (veryverbose) { std::cerr << PACKAGE << ": "; INDENTM INDENT std::cerr << " leave function " << __PRETTY_FUNCTION__ << " in " << __FILE__ << "@" << __LINE__ << std::endl; }
#else
#define FUNC std::cerr << " ... in function " << __FUNCTION__ << " in " << __FILE__ << "@" << __LINE__ << std::endl;
#define FUNCIN  if (veryverbose) { INDENT INDENTP std::cerr << " enter function " << __FUNCTION__ << " in " << __FILE__ << "@" << __LINE__ << std::endl; }
#define FUNCOUT if (veryverbose) { INDENTM INDENT std::cerr << " leave function " << __FUNCTION__ << " in " << __FILE__ << "@" << __LINE__ << std::endl; }
#endif
#else 
#define FUNC 
#define FUNCIN 
#define FUNCOUT 
#endif


#endif
