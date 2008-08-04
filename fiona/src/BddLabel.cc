/*****************************************************************************
 * Copyright 2005, 2006, 2007 Kathrin Kaschner, Peter Massuthe               *
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
 * \file    BddLabel.cc
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

#include "BddLabel.h"
#include <string>


//! \brief constructor for the BddLabel
//! \param c name of the new label
//! \param n number of the new label
//! \param table table to be added to
BddLabel::BddLabel(const std::string& c, int n, BddLabelTab * table) :
    name(c) {
    nbr = n;
    next = (BddLabel *) 0;
    table->add(this);
}


//! \brief deconstructor
BddLabel::~BddLabel() {
}


//! \brief constructor for the BddLabelTab
//! \param s size of the table
BddLabelTab::BddLabelTab(unsigned int s) {
    size = s;
    table = new BddLabel * [s];
    card = 0;
    for (unsigned int i = 0; i < s; i++) {
        table[i] = (BddLabel *) 0;
    }
}


//! \brief deconstructor
BddLabelTab::~BddLabelTab() {
    BddLabel* temp;
    for (unsigned int i = 0; i < size; i++) {
        while (table[i]) {
            temp = table[i];
            //std::cout << table[i]->name << "  ";
            table[i] = table[i] -> next;
            delete temp;
        }
    }
    delete [] table;
}



//! \brief checks whether a label with the given name exists in the table and returns it
//! \param name name of the label
//! \return returns either the label, or NULL if the name matches no label in the table
BddLabel * BddLabelTab::lookup(const std::string& name) const {

    /* 1. Hashvalue bestimmen */
    unsigned int h, i;
    BddLabel * lst = (BddLabel *) 0;
    h = 0;
    for (i = 0; i < name.size(); i++) {
        h += (int) name[i];
        h %= size;
    }

    /* 2. suchen */
    for (lst = table[h]; lst; lst = lst -> next) {
        if (lst->name == name) {
            break;
        }
    }
    return lst;
}


//! \brief adds a label to the label table :)
//! \param s label to be added
void BddLabelTab::add(BddLabel * s) {

    /* 1. Hashvalue bestimmen */
    unsigned int h, i;
    h = 0;
    for (i = 0; i < s->name.size(); i++) {
        h += (int) s->name[i];
        h %= size;
    }

    /* 2. eintragen */
    s->next = table[h];
    table[h] = s;
    card++;
}
