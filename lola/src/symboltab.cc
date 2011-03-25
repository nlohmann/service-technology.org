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
#include "net.H"

SymbolTab* PlaceTable, * TransitionTable;
extern SymbolTab* GlobalTable, * LocalTable;

SymbolTab::SymbolTab(unsigned int s = 65536) : card(0), size(s) {
    table = new Symbol*[s];
    for (unsigned int i = 0; i < s; ++i) {
        table[i] = NULL;
    }
}

SymbolTab::~SymbolTab() {
    for (unsigned int i = 0; i < size; i++) {
        while (table[i]) {
            Symbol* temp = table[i];
            table[i] = table[i] -> next;
            delete temp;
        }
    }
    delete [] table;
}


Symbol* SymbolTab::lookup(char* name) {
    /* 1. Hashvalue bestimmen */
    unsigned int h = 0;
    for (unsigned int i = 0; i < strlen(name); i++) {
        h += (int) name[i];
        h %= size;
    }
    /* 2. suchen */
    Symbol* lst;
    for (lst = table[h]; lst; lst = lst -> next) {
        if (!strcmp(lst->name, name)) {
            break;
        }
    }
    return lst;
}

Symbol::Symbol(char* n, SymbolTab* table, kinds _kind) : name(n), kind(_kind) {
    table->add(this);
}

void SymbolTab::add(Symbol* s) {
    /* 1. Hashvalue bestimmen */
    unsigned int h = 0;
    for (unsigned int i = 0; i < strlen(s->name); i++) {
        h += (int) s->name[i];
        h %= size;
    }

    /* 2. eintragen */
    s->next = table[h];
    table[h] = s;
    card++;
}

Symbol::~Symbol() {}

PlSymbol::PlSymbol(char* txt, UType* _sort): Symbol(txt, PlaceTable, pl), place(NULL), sort(_sort) {}

PlSymbol::PlSymbol(Place* p, UType* _sort): Symbol(p->name, PlaceTable, pl), place(p), sort(_sort) {}

TrSymbol::TrSymbol(char* c): Symbol(c, TransitionTable, tr), transition(NULL), vars(NULL), guard(NULL) {}

SoSymbol::SoSymbol(char* name, UType* ty): Symbol(name, GlobalTable, so), type(ty) {}

StSymbol::StSymbol(char* name): Symbol(name, BuchiTable, st) {
    state = new buchistate(name);
}

RcSymbol::RcSymbol(char* name, UType* ty): Symbol(name, GlobalTable, rc), type(ty) {}

VaSymbol::VaSymbol(char* name, UVar* t): Symbol(name, LocalTable, va), var(t) {}

FcSymbol::FcSymbol(char* name, UFunction* f): Symbol(name, GlobalTable, fc), function(f) {}

EnSymbol::EnSymbol(char* name): Symbol(name, GlobalTable, en) {}

IdList::IdList(char* _name, IdList *_next) : name(_name), next(_next) {}
