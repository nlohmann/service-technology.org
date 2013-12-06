/*!
\author Karsten
\file
\status approved 25.01.2012
\ingroup g_frontend g_symboltable

\brief definition of class ParserPTNet
*/

#pragma once

#include <Core/Dimensions.h>
#include <Parser/SymbolTable.h>

/*!
symbol tables for a low-level net

This data structure should be independent from the input file format (LoLA,
PNML, ...).

\ingroup g_frontend g_symboltable
*/
class ParserPTNet
{
public:
    /// a symbol table for places
    SymbolTable *PlaceTable;
    /// a symbol table for transitions
    SymbolTable *TransitionTable;

    /// translation into main net structures Node, Place, Transition, and Marking
    void symboltable2net();

    /// constructor
    ParserPTNet();

    /// destructor
    ~ParserPTNet();
};
