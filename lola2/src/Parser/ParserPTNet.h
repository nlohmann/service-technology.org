/*!
\author Karsten
\file ParserPTNet.h
\status approved 25.01.2012

Class definition for the result of the parsing of a low level net. This result
should be independent from the format (LoLA / PNML / ...)
*/

#pragma once
#include "Core/Dimensions.h"
#include "Parser/SymbolTable.h"

class ParserPTNet
{
    private:

    public:
        static void sort_arcs(index_t* arcs, mult_t* mults, const index_t from, const index_t to);
        SymbolTable* PlaceTable;
        SymbolTable* TransitionTable;

        /// translation into main net structures Node, Place, Transition, and Marking
        void symboltable2net();

        ParserPTNet();
        ~ParserPTNet();
};
