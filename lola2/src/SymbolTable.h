/*!
 * \author Karsten
 * \file class definition for a symbol table
 *
 * The table has strings as keys. Payload  is not explicitly represented.
 * It can be attached by deriving from class symbol.
 * Collissions are handled as linked lists
 */

#pragma once

#include<string>
#include"Dimensions"

class Symbol;

class SymbolTable
{
public:
	/// If symbol with same key is in table: return false
	/// If symbol with same key is not in table: return true and insert it
	bool insert(Symbol *);
	/// If key is in table: return corresponding symbol
	/// If key is not in table: return NULL
	Symbol * lookup(string);
/// Generate and initialize a symbol table
SymbolTable();
private:
	///The actual symbol table. It gets pointers as we use lists for collisions.
	Symbol * table[SIZEOF_SYMBOLTABLE];
	/// The hash function to be used
	unsigned int hash(string);
}
