/*!
\author Karsten
\file SymbolTable.cc
\status new
\ingroup g_frontend g_symboltable

\brief class implementation for a symbol table
*/

#include "SymbolTable.h"

/// Intialization amounts to setting all entries to NULL
SymbolTable::SymbolTable()
{
    for (int i = 0; i < SIZEOF_SYMBOLTABLE; i++)
    {
        table[i] = NULL;
    }
    card = 0;
}

/// We use sum of ASCII values as hash value
unsigned int SymbolTable::hash(string s) const
{
    unsigned long int result = 0;

    for (size_t i = 0; i < s.length(); i++)
    {
        result += s[i];
        result %= SIZEOF_SYMBOLTABLE;
    }
    return (unsigned int) result;
}

/// lookup an element from symbol table
/// if input is correct, lookup is used for existing symbols
/// NULL is returned if key is not present which typically
/// indicates a syntax error "double definition"

Symbol* SymbolTable::lookup(string str) const
{
    for (Symbol* sym = table[hash(str)]; sym; sym = sym -> getNext())
    {
        if (sym -> getKey() == str)
        {
            return sym;
        }
    }
    return NULL;
}

/// insert an element into symbol table
/// if input is correct, insert is used when key is not yet present
/// If input is present, false is returned.
/// This typically indicates a syntax error "used but not defined
bool SymbolTable::insert(Symbol* sym)
{
    int index = hash(sym -> getKey());
    for (Symbol* othersym = table[index]; othersym; othersym = othersym -> getNext())
    {
        if (othersym -> getKey() == sym -> getKey())
        {
            return false;
        }
    }
    sym -> setNext(table[index]);
    table[index] = sym;
    ++card;
    return true;
}

/// returns pointer to first element in symbol table
Symbol* SymbolTable::first()
{
    for (currentIndex = 0; currentIndex < SIZEOF_SYMBOLTABLE; currentIndex++)
    {
        if (table[currentIndex])
        {
            return(currentSymbol = table[currentIndex]);
        }
    }
    return NULL;
}

/// returns pointer to next element in symbol table
Symbol* SymbolTable::next()
{
    if (currentSymbol -> getNext())
    {
        // there is another element in the same hash bucket
        return(currentSymbol = currentSymbol -> getNext());
    }
    else
    {
        // there is no other element in the same bucket
        for (++currentIndex; currentIndex < SIZEOF_SYMBOLTABLE; currentIndex++)
        {
            if (table[currentIndex])
            {
                return(currentSymbol = table[currentIndex]);
            }
        }
        return NULL;
    }
}

/// returns number of entries in table
unsigned int SymbolTable::getCard() const
{
    return card;
}
