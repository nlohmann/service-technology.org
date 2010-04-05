/*
   oWFN2BPEL - translate open workflow net models into BPEL processes

   Copyright (C) 2008       Niels Lohmann <niels.lohmann@uni-rostock.de>
   Copyright (C) 2006, 2007 Jens Kleine <jkleine@informatik.hu-berlin.de>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/


#include <fstream>
#include <iostream>
#include <string>
#include "Transition.h"
#include "BPEL.h"
#include "classes.h"

using std::string;
using std::cout;
using std::endl;


//constructor
Transition::Transition(string str, Transition* ptr) :
    name(str),
    consumes(NULL),
    produces(NULL),
    bpel_code(NULL),
    next(ptr),
    dfs(-1),
    lowlink(-1) {
}


void Transition::add_consume(string str) {
    consumes = new Place(str, consumes);
}

void Transition::add_produce(string str) {
    produces = new Place(str, produces);
}


void Transition::del_produces() {
    Place* ptr;
    Place* help;

    ptr = produces;
    while (ptr != NULL) {
        help = ptr;
        ptr = ptr->next;
        delete(help);
    }

    produces = NULL;
}

void Transition::del_consumes() {
    Place* ptr;
    Place* help;

    ptr = consumes;
    while (ptr != NULL) {
        help = ptr;
        ptr = ptr->next;
        delete(help);
    }

    consumes = NULL;
}

void Transition::del_lists() {
    Place* ptr;
    Place* help;

    ptr = produces;
    while (ptr != NULL) {
        help = ptr;
        ptr = ptr->next;
        delete(help);
    }
    ptr = consumes;
    while (ptr != NULL) {
        help = ptr;
        ptr = ptr->next;
        delete(help);
    }

    produces = NULL;
    consumes = NULL;
}

void Transition::add_bpel(int bpel_id) {
    bpel_code = new BPEL(bpel_id, bpel_code);
}

void Transition::add_bpel(int bpel_id, string name) {
    bpel_code = new BPEL(bpel_id, bpel_code, name);
}

void Transition::add_last_bpel(int bpel_id, string name) {
    BPEL* tmp;

    tmp = bpel_code;

    if (tmp == NULL) {
        bpel_code = new BPEL(bpel_id, bpel_code, name);
    } else {
        while (tmp->next != NULL) {
            tmp = tmp->next;
        }
        tmp->next = new BPEL(bpel_id, NULL, name);
    }
}


void Transition::append_bpel(BPEL* list) {
    BPEL* help;

    if (list != NULL) {
        help = list;
        while (list->next != NULL) {
            list = list->next;
        }
        list->next = bpel_code;
        bpel_code = help;
    }   //else nothing to do
}

void Transition::out() {
    Place* tmpplaces;
    cout << endl << "Transition: '" << name << "'" << " (dfs: " << dfs << ", lowlink: " << lowlink << ")" << endl;
    tmpplaces = consumes;
    cout << "   Consumes: ";
    while (tmpplaces != NULL) {
        cout << "'" << tmpplaces->name << "' ";
        tmpplaces = tmpplaces->next;
    }
    cout << endl << "   Produces: ";
    tmpplaces = produces;
    while (tmpplaces != NULL) {
        cout << "'" << tmpplaces->name << "' ";
        tmpplaces = tmpplaces->next;
    }
    cout << endl;
    if (bpel_code != NULL) {
        bpel_code->out();
    }
    if (next != NULL) {
        next->out();
    }
}

void Transition::pure_out() {
    cout << "Transition: '" << name << "'" << endl;;
    if (next != NULL) {
        next->pure_out();
    }
}

void Transition::owfn_out() {
    Place* tmpplaces;
    cout << endl << "TRANSITION " << name << endl << "CONSUME" << endl;
    tmpplaces = consumes;
    if (tmpplaces == NULL) {
        cout << ";" << endl << "PRODUCE" << endl;
    }
    while (tmpplaces != NULL) {
        cout << tmpplaces->name << " : 1";

        if (tmpplaces->next != NULL) {
            cout << "," << endl;
        } else {
            cout << ";" << endl << "PRODUCE" << endl;
        }

        tmpplaces = tmpplaces->next;
    }

    tmpplaces = produces;
    if (tmpplaces == NULL) {
        cout << ";" << endl;
    }
    while (tmpplaces != NULL) {
        cout << tmpplaces->name << " : 1";

        if (tmpplaces->next != NULL) {
            cout << "," << endl;
        } else {
            cout << ";" << endl;
        }

        tmpplaces = tmpplaces->next;
    }

    if (next != NULL) {
        next->owfn_out();
    }
}

void Transition::list_out() {
    Place* tmpplaces;
    cout << endl << "Transition: '" << name << "'" << endl;
    tmpplaces = consumes;
    cout << "   Consumes: ";
    while (tmpplaces != NULL) {
        cout << "'" << tmpplaces->name << "' ";
        tmpplaces = tmpplaces->next;
    }
    cout << endl << "   Produces: ";
    tmpplaces = produces;
    while (tmpplaces != NULL) {
        cout << "'" << tmpplaces->name << "' ";
        tmpplaces = tmpplaces->next;
    }
    cout << endl;
}



void TransitionList::out() {
    cout << "Transition: " << transitionptr->name << endl;
    if (next != NULL) {
        next->out();
    }
}

void TransitionList::list_out() {
    transitionptr->list_out();
    if (next != NULL) {
        next->list_out();
    }
}
