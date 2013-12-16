/*!
\author Karsten
\file
\status approved 25.01.2012
\ingroup g_symboltable
\brief implementation of class Symbol
*/

#include <cstdlib>
#include <Frontend/SymbolTable/Symbol.h>

index_t Symbol::getIndex() const
{
    return index;
}

const char * const Symbol::getKey() const
{
    return key;
}

Symbol *Symbol::getNext() const
{
    return next;
}

void Symbol::setNext(Symbol *sym)
{
    next = sym;
}

void Symbol::setIndex(index_t i)
{
    index = i;
}

/*!
\param[in] k  the name of the symbol

\note The pointer next is lated set by SymbolTable::insert.
\note The index is later changed by ParserPTNet::symboltable2net.
*/
Symbol::Symbol(const char * const k) :
    key(k),
    next(NULL),  // intermediate initialization
    index(0)     // intermediate initialization
{
}
