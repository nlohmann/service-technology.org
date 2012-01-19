/*!
\author Karsten
\file Symbol.h
\status new

\brief class definition for a symbol

A symbol is an entry in a symbol table (class SymbolTable). It has a string
as key. For dealing with collissions in the symbol table, symbols can be
lnked as lists.
Payload can be added by deriving subclasses.

*/

#pragma once

#include <string>

using std::string;

class Symbol
{
    public:
        /// Getter for key
        string getKey();
        /// Getter for next
        Symbol* getNext();
        /// Setter for next
        void setNext(Symbol*);

        /// Generate and initialize a symbol
        Symbol(string);

    private:
        ///The key. Used for insertion in symbol table
        string key;
        /// Symbols with same hash value are organized as lists.
        Symbol* next;
};
