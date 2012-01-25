/*!
\author Karsten
\file Symbol.cc
\status approved 25.01.2012
\ingroup g_frontend g_symboltable

\brief class implementation for a symbol
*/

#include <cstdlib>
#include "Symbol.h"

/// Getter for key
const char* Symbol::getKey() const
{
    return key;
}

/// Getter for next
Symbol* Symbol::getNext() const
{
    return next;
}

/// Setter for next
void Symbol::setNext(Symbol* sym)
{
    next = sym;
}

/// Generate and initialize a symbol
/// Argument is the key.
Symbol::Symbol(const char* k)
    :
    key(k),
    next(NULL)
{
}

Symbol::~Symbol()
{
    free((char*)key);
}
