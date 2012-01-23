/*!
\author Karsten
\file SymbolTable.h
\status new
\ingroup g_frontend g_symboltable

\brief class definition for a symbol table

The table has strings as keys. Payload  is not explicitly represented.
It can be attached by deriving from class symbol.
Collissions are handled as linked lists.
*/

#pragma once

#include <string>
#include "Dimensions.h"
#include "Symbol.h"

using std::string;

class SymbolTable
{
    public:
        /// If symbol with same key is in table: return false
        /// If symbol with same key is not in table: return true and insert it
        bool insert(Symbol*);
        /// If key is in table: return corresponding symbol
        /// If key is not in table: return NULL
        Symbol* lookup(string) const;

        /// Initialize iteration; return NULL if table empty
        Symbol* first();
        /// Continue iteration; return NULL if there is none
        Symbol* next();

        /// Generate and initialize a symbol table
        SymbolTable();
	/// Close SymbolTable
	~SymbolTable();

        /// Get number of entries in table
        unsigned int getCard() const;

    private:
        ///The actual symbol table. It gets pointers as we use lists for collisions.
        Symbol* table[SIZEOF_SYMBOLTABLE];

        /// The hash function to be used
        unsigned int hash(string) const;
        /// The number of entrie in table

        unsigned int card;

        /// The index of the current element in iteration
        unsigned int currentIndex;
        /// Points to the current element in iteration
        Symbol* currentSymbol;

};
