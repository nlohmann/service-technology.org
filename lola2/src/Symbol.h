/*!
\author Karsten
\file Symbol.h
\status approved 25.01.2012
\ingroup g_frontend g_symboltable

\brief class definition for a symbol
*/

#pragma once

#include "Dimensions.h"

/*!
A symbol is an entry in a symbol table (class SymbolTable). It has a string as
key. For dealing with collissions in the symbol table, symbols can be lnked as
lists. Payload can be added by deriving subclasses.
*/
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
        explicit Symbol(const char*);
        /// Delete a symbol
        virtual ~Symbol();

	/// get index of symbol in net data structures
	const index_type getIndex() const;

	/// set index of symbol in net date structures
	void setIndex(index_type);

    private:
        ///The key. Used for insertion in symbol table
        char* key;
        /// Symbols with same hash value are organized as lists.
        Symbol* next;

	/// Index in net data structure. Set during transformation symbols --> net
	index_type index;
};
