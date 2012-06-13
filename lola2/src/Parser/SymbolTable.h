/*!
\author Karsten
\file SymbolTable.h
\status approved 25.01.2012
\ingroup g_frontend g_symboltable
\todo Eventuell ein FOREACH Makro (basierend auf first() und next()) bauen

\brief class definition for a symbol table
*/

#pragma once

class Symbol;


/*!
\brief Symbol table

The table has strings as keys. Payload is not explicitly represented. It can
be attached by deriving from class symbol. Collissions are handled as linked
lists.
*/
class SymbolTable
{
    public:
        /// overall hash table collisions
        static unsigned int collisions;

    public:
        /// If symbol with same key is in table: return false
        /// If symbol with same key is not in table: return true and insert it
        bool insert(Symbol*);

        /// If key is in table: return corresponding symbol
        /// If key is not in table: return NULL
        Symbol* lookup(const char*) const;

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

        /// The number of entries in table
        unsigned int card;

    private:
        /// The actual symbol table. It gets pointers as we use lists for collisions.
        Symbol** table;

        /// The hash function to be used
        unsigned int hash(const char*) const;

        /// The index of the current element in iteration
        unsigned int currentIndex;

        /// Points to the current element in iteration
        Symbol* currentSymbol;
};
