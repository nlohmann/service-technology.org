/*!
\author Karsten
\file Symbol.cc
\status approved 25.01.2012
\ingroup g_frontend g_symboltable

\brief class implementation for a symbol
*/

#include <cstdlib>
#include "Symbol.h"

/// Getter for index
index_t Symbol::getIndex() const
{
    return index;
}

/// Getter for key
char* Symbol::getKey() const
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

/// Setter for index
void Symbol::setIndex(index_t i)
{
    index = i;
}

/// Generate and initialize a symbol
/// Argument is the key.
Symbol::Symbol(const char* k)
    :
    key(const_cast<char*>(k)),
    next(NULL)
{
}

Symbol::~Symbol()
{
    // do not free key here, as it is passed to Node::Name
    // free(const_cast<char*>(key));
}
