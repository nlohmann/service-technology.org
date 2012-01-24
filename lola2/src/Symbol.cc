/*!
\author Karsten
\file Symbol.cc
\status new
\ingroup g_frontend g_symboltable

\brief class implementation for a symbol

A symbol is an entry in a symbol table (class SymbolTable). It has a string
as key. For dealing with collissions in the symbol table, symbols can be
lnked as lists.
Payload can be added by deriving subclasses.

*/

#include "Symbol.h"

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

/// Generate and initialize a symbol
/// Argument is the key.

Symbol::Symbol(char* k)
    :
    key(k),
    next(NULL)
{
}

Symbol::~Symbol()
{
	
}
