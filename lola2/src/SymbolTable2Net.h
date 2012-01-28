/*!

\file SymbolTable2Net.h
\author Karsten
\status new
\brief Translation of the symbol tables (from parsers) into main net structures Node, Place, Transition, and Marking
*/


#pragma once
#include "ParserPTNet.h"

/// Translation of the symbol tables (from parsers) into main net structures Node, Place, Transition, and Marking
void symboltable2net(ParserPTNet*);

