/*!
\author Karsten
\file
\status approved 25.01.2012
\ingroup g_symboltable
\brief definition of class Symbol
*/

#pragma once
#include <Core/Dimensions.h>

/*!
\brief a symbol is an entry in a symbol table (class SymbolTable)

It has a string as key. For dealing with collissions in the symbol table,
symbols can be lnked as lists. Payload can be added by deriving subclasses.

\ingroup g_symboltable
*/
class Symbol
{
public:
    explicit Symbol(const char * const);
    virtual ~Symbol() {}

    /// getter for key
    const char * const getKey() const;

    /// getter for next
    Symbol *getNext() const;

    /// get index of symbol in net data structures
    index_t getIndex() const;

    /// setter for next
    void setNext(Symbol *);

    /// set index of symbol in net date structures
    void setIndex(index_t);

private:
    /// the name of the symbol; used for insertion in symbol table
    const char * const key;

    /// symbols with same hash value are organized as lists
    Symbol *next;

    /// index in net data structure; set during transformation symbols --> net
    index_t index;
};
