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


#include "buchi.H"

int buchistate::nr = 0;

SymbolTab* BuchiTable;
buchistate* initialbuchistate;
buchistate** buchiautomaton;

buchitransition** buchistate::getenabled() {
    int i, j;
    buchitransition** enabled;

    enabled = new buchitransition * [nrdelta+1];
    for (i = j = 0; i < nrdelta; i++) {
        if (delta[i]->guard->value) {
            enabled[j++] = delta[i];
        }
    }
    enabled[j] = NULL;
    return enabled;
}

buchistate::buchistate(char* n) {
    name = n;
    code = nr++;
    final = 0;
    nrdelta = 0;
    delta = NULL;
    transitionlist = NULL;
    next = NULL;
}

void init_buchi() {

    int i;
    // 0. translate list of states into array of states
    buchiautomaton = new buchistate * [buchistate::nr + 1];
    buchistate* b;
    for (i = 0, b = initialbuchistate; b; b = b -> next, i++) {
        buchiautomaton[i] = b;
    }
    buchiautomaton[i] = NULL;

    // 1. translate list of transitions into array of transitions


    // 2. process guard formulas
}

void print_buchi() {
    int i, j;

    cout << "Initial state: " << initialbuchistate -> name << endl;
    for (i = 0; i < buchistate::nr; i++) {
        cout << "State " << buchiautomaton[i]->name << buchiautomaton[i]-> code;
        if (buchiautomaton[i]->final) {
            cout << " (final)";
        }
        cout << endl << "transitions:" << endl;
        for (j = 0; j < buchiautomaton[i]->nrdelta; j++) {
            cout << "to " << buchiautomaton[i]->delta[j]->delta -> name << endl;
        }
    }
}

