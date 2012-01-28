
/*!
\author Karsten
\file ParserPTNet.h
\status new

Class definition for the result of the parsing of a low level net. This result
should be independent from the format (LoLA / PNML / ...)
*/


#include "SymbolTable.h"
#include "ParserPTNet.h"

ParserPTNet::ParserPTNet()
{
    PlaceTable = new SymbolTable();
    TransitionTable = new SymbolTable();
}

ParserPTNet::~ParserPTNet()
{
    delete PlaceTable;
    delete TransitionTable;
}
