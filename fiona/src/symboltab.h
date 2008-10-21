/*****************************************************************************\
 Copyright (c) 2008  Robert Danitz, Christian Gierds, Niels Lohmann,
                     Peter Massuthe, Richard Mu"ller, Janine Ott,
                     Jan Su"rmeli, Daniela Weinberg, Karsten Wolf,
                     Martin Znamirowski

 Copyright (c) 2007  Jan Bretschneider, Robert Danitz, Christian Gierds,
                     Kathrin Kaschner, Leonard Kern, Niels Lohmann,
                     Peter Massuthe, Daniela Weinberg, Karsten Wolf

 Copyright (c) 2006  Kathrin Kaschner, Peter Massuthe, Daniela Weinberg,
                     Karsten Wolf

 This file is part of Fiona, see <http://service-technology.org/fiona>.

 Fiona is free software; you can redistribute it and/or modify it under the
 terms of the GNU General Public License as published by the Free Software
 Foundation; either version 3 of the License, or (at your option) any later
 version.
 
 Fiona is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License along with
 Fiona (see file COPYING). If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

/*!
 * \file    symboltab.h
 *
 * \brief   functions for Petri net elements
 *
 * \author  responsible: Daniela Weinberg <weinberg@informatik.hu-berlin.de>
 */

#ifndef SYMBOLTAB_H
#define SYMBOLTAB_H

#include "mynew.h"
#include <map>
#include <string>

//-------------------------- forward declarations -----------------------------
class Symbol;
class PlSymbol;

template<typename SymbolType> class SymbolTab;

class owfnPlace;
class owfnTransition;

//---------------------------- global variables -------------------------------
extern SymbolTab<PlSymbol>* PlaceTable;

//----------------------------- class SymbolTab -------------------------------
template<typename SymbolType> class SymbolTab {
    private:
        typedef std::map<std::string, SymbolType*> symbols_t;
        typedef typename symbols_t::const_iterator const_iterator;
        typedef typename symbols_t::size_type size_type;
        symbols_t symbols;
        const_iterator currentSymbolIter;
        const_iterator begin() const;
        const_iterator end() const;
    public:
        ~SymbolTab();
        void add(SymbolType*);
        SymbolType* lookup(const std::string& name) const;
        size_type getSize() const;
        void initGetNextSymbol();
        SymbolType* getNextSymbol();

#undef new
        // Provides user defined operator new. Needed to trace all new operations
        // on this class.
        NEW_OPERATOR(SymbolTab)
#define new NEW_NEW
};


//------------------------------ class Symbol ---------------------------------
class Symbol {
    private:
        std::string name;
    public:
        Symbol(const std::string&);
        std::string getName() const;

        // Provides user defined operator new. Needed to trace all new operations
        // on this class.
#undef new
        NEW_OPERATOR(Symbol)
#define new NEW_NEW
};


//----------------------------- class PlSymbol --------------------------------
class PlSymbol : public Symbol {
    private:
        owfnPlace* place;
    public:
        PlSymbol(owfnPlace*);
        owfnPlace* getPlace() const;

#undef new
        // Provides user defined operator new. Needed to trace all new operations
        // on this class.
        NEW_OPERATOR(PlSymbl)
#define new NEW_NEW
};


//--------- implementation of template functions of class SymbolTab -----------
template<typename SymbolType> SymbolTab<SymbolType>::~SymbolTab() {
    for (typename symbols_t::const_iterator symbolIter = symbols.begin(); symbolIter
            != symbols.end(); ++symbolIter) {
        delete symbolIter->second;
    }
}


template<typename SymbolType> void SymbolTab<SymbolType>::add(SymbolType* s) {
    symbols[s->getName()] = s;
}


template<typename SymbolType> SymbolType* SymbolTab<SymbolType>::lookup(const std::string& name) const {
    typename symbols_t::const_iterator foundSymbolIter = symbols.find(name);
    if (foundSymbolIter == symbols.end()) {
        return NULL;
    }

    return foundSymbolIter->second;
}


template<typename SymbolType> typename SymbolTab<SymbolType>::size_type SymbolTab<
        SymbolType>::getSize() const {
    return symbols.size();
}


template<typename SymbolType> typename SymbolTab<SymbolType>::const_iterator SymbolTab<
        SymbolType>::begin() const {
    return symbols.begin();
}


template<typename SymbolType> typename SymbolTab<SymbolType>::const_iterator SymbolTab<
        SymbolType>::end() const {
    return symbols.end();
}


template<typename SymbolType> void SymbolTab<SymbolType>::initGetNextSymbol() {
    currentSymbolIter = begin();
}


template<typename SymbolType> SymbolType* SymbolTab<SymbolType>::getNextSymbol() {
    if (currentSymbolIter == end()) {
        return NULL;
    }

    SymbolType* result = currentSymbolIter->second;
    ++currentSymbolIter;
    return result;
}
#endif
