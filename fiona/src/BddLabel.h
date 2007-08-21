/*****************************************************************************
 * Copyright 2005, 2006 Kathrin Kaschner                                     *
 *                                                                           *
 * This file is part of Fiona.                                               *
 *                                                                           *
 * Fiona is free software; you can redistribute it and/or modify it          *
 * under the terms of the GNU General Public License as published by the     *
 * Free Software Foundation; either version 2 of the License, or (at your    *
 * option) any later version.                                                *
 *                                                                           *
 * Fiona is distributed in the hope that it will be useful, but WITHOUT      *
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or     *
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for  *
 * more details.                                                             *
 *                                                                           *
 * You should have received a copy of the GNU General Public License along   *
 * with Fiona; if not, write to the Free Software Foundation, Inc., 51       *
 * Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.                      *
 *****************************************************************************/

/*!
 * \file    BddLabel.h
 *
 * \brief   functions for representing an OG as a BDD
 *
 * \author  responsible: Kathrin Kaschner <kathrin.kaschner@informatik.uni-rostock.de>
 *
 * \note    This file is part of the tool Fiona and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 */

#ifndef BDDLABEL_H_
#define BDDLABEL_H_

#include "mynew.h" 
#include <string>

class BddLabelTab;

class BddLabel {
    public:
        std::string name; //name of Label
        unsigned int nbr; //number of Label
        BddLabel * next;
        BddLabel(const std::string& c, int n, BddLabelTab * table);
        ~BddLabel();

        // Provides user defined operator new. Needed to trace all new
        // operations on this class.
#undef new
        NEW_OPERATOR(BddLabel)
#define new NEW_NEW
};


class BddLabelTab {
    public:
        BddLabel ** table;
        BddLabel * lookup(const std::string&) const;
        void add(BddLabel *);
        BddLabelTab(unsigned int size = 65536);
        ~BddLabelTab();
        unsigned int card;
        unsigned int size;

        // Provides user defined operator new. Needed to trace all new
        // operations on this class.
#undef new
        NEW_OPERATOR(BddLabelTab)
#define new NEW_NEW
};

#endif /*BDDLABEL_H_*/
