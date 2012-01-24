/*!
\author Karsten
\file Symbol.h
\status new
\ingroup g_frontend g_symboltable

\brief class definition for a symbol

A symbol is an entry in a symbol table (class SymbolTable). It has a string
as key. For dealing with collissions in the symbol table, symbols can be
lnked as lists.
Payload can be added by deriving subclasses.

*/

#pragma once

#include <string>


class Symbol
{
    public:
        /// Getter for key
        char* getKey() const;
        /// Getter for next
        Symbol* getNext() const;
        /// Setter for next
        void setNext(Symbol*);

        /// Generate and initialize a symbol
        Symbol(char*);
	/// Delete a symbol
	virtual ~Symbol();

    private:
        ///The key. Used for insertion in symbol table
        char* key;
        /// Symbols with same hash value are organized as lists.
        Symbol* next;
};
