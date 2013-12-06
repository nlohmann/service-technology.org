/*!
\author Karsten
\file
\status approved 25.01.2012
\ingroup g_symboltable
\todo Eventuell ein FOREACH Makro (basierend auf first() und next()) bauen
\brief definition of class SymbolTable
*/

#pragma once

// forward declaration
class Symbol;

/*!
\brief A symbol table for places and transitions.

The table has strings as keys. Payload is not explicitly represented. It can
be attached by deriving from class symbol. Collissions are handled as linked
lists.

\ingroup g_symboltable
*/
class SymbolTable
{
public:
    /// overall hash table collisions
    static unsigned int collisions;

public:
    /// If symbol with same key is in table: return false
    /// If symbol with same key is not in table: return true and insert it
    bool insert(Symbol *);

    /// If key is in table: return corresponding symbol
    /// If key is not in table: return NULL
    Symbol *lookup(const char *) const;

    /// initialize iteration; return NULL if table empty
    Symbol *first();
    /// continue iteration; return NULL if there is none
    Symbol *next();

    /// generate and initialize a symbol table
    SymbolTable();
    /// close SymbolTable
    ~SymbolTable();

    /// get number of entries in table
    unsigned int getCard() const;

private:
    /// the number of entries in table
    unsigned int card;

    /// the actual symbol table. It gets pointers as we use lists for collisions.
    Symbol **table;

    /// the index of the current element in iteration
    unsigned int currentIndex;

    /// points to the current element in iteration
    Symbol *currentSymbol;

    /// the hash function to be used
    unsigned int hash(const char *) const;
};
