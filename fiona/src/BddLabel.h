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
 * \file    BddLabel.h
 *
 * \brief   functions for representing an OG as a BDD
 *
 * \author  responsible: Kathrin Kaschner <kathrin.kaschner@informatik.uni-rostock.de>
 */

#ifndef BDDLABEL_H_
#define BDDLABEL_H_

#include "mynew.h" 
#include <string>

class BddLabelTab;

class BddLabel {
    public:
        /// name of Label
        std::string name;
        
        ///number of Label
        unsigned int nbr;
        
        BddLabel * next;
        
        /// constructor (3 parameters)
        BddLabel(const std::string& c, int n, BddLabelTab * table);
        
        /// destructor
        ~BddLabel();

        /// Provides user defined operator new. Needed to trace all new
        /// operations on this class.
#undef new
        NEW_OPERATOR(BddLabel)
#define new NEW_NEW
};


class BddLabelTab {
    public:
        BddLabel ** table;
        
        /// checks whether a BddLabel with the given string is part of the 
        /// table and returns it
        BddLabel * lookup(const std::string&) const;
        
        /// Adds a BddLabel to the table
        void add(BddLabel *);
        
        /// constructor no parameters)
        BddLabelTab(unsigned int size = 65536);
        
        /// destructor
        ~BddLabelTab();
        unsigned int card;
        unsigned int size;

        /// Provides user defined operator new. Needed to trace all new
        /// operations on this class.
#undef new
        NEW_OPERATOR(BddLabelTab)
#define new NEW_NEW
};

#endif /*BDDLABEL_H_*/
