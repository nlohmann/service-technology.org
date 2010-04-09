/*****************************************************************************\
 LoLA -- a Low Level Petri Net Analyzer

 Copyright (C)  1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007,
                2008, 2009  Karsten Wolf <lola@service-technology.org>

 LoLA is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 LoLA is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with LoLA.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/


#include "symboltab.H"
#include "formula.H"
#include "net.H"

#ifndef BUCHI
#define BUCHI

//
extern SymbolTab* BuchiTable;


class buchitransition;

class buchistate {
    public:
        bool final;
        char* name;
        buchitransition** delta;
        buchitransition** getenabled();
        int nrdelta;
        int code;
        static int nr;
        buchistate(char*);
        buchistate() {}
        buchitransition* transitionlist;   // only used for parsing
        buchistate* next;  // only used for parsing
};

class buchitransition {
    public:
        formula* guard;
        buchistate* delta;
        buchitransition* next;   // only used for parsing
};


extern buchistate* initialbuchistate;
extern buchistate** buchiautomaton;
void print_buchi();

#endif
