/*!
\author Karsten
\file ParserPTNet.h
\status approved 25.01.2012

Class definition for the result of the parsing of a low level net. This result
should be independent from the format (LoLA / PNML / ...)
*/

#pragma once

#include "SymbolTable.h"

class ParserPTNet
{
    public:
        SymbolTable * PlaceTable;
        SymbolTable * TransitionTable;
	ParserPTNet();
	~ParserPTNet();
};
