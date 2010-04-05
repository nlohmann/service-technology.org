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


#ifndef _CLASSES_H_
#define _CLASSES_H_


#include <cstring>
#include <cstdlib>
#include <string>

#include "Transition.h"
#include "BPEL.h"
#include "Place.h"


extern int* stat_array;
extern int number_of_rounds;
extern char* lola_ex;
enum activities {EMPTY, REPLY, RECEIVE, FLOW, SWITCH, PICK, PROCESS, WHILE, SEQUENCE, EXIT, INVOKE, FOREACH, SCOPE, OPAQUE};





class plists {
    public:
        std::string name;
        plists* next;

        plists(plists* ptr = NULL, std::string str = "") :
            name(str),
            next(ptr) {
        }
};





//usage of class link: increment() once, create new instance in source and in target list with same id

class Links {
    public:
        int link_id;
        Links* next;
        static int link_counter;

        Links(Links* ptr = NULL) :
            link_id(link_counter),
            next(ptr) {
        }

        static void increment() {
            link_counter++;
        }
};





class Branch {
    public:
        BPEL* bpel_code;
        Branch* next;
        std::string pick_receive;

        Branch(Branch* ptr = NULL) :
            bpel_code(NULL),
            next(ptr),
            pick_receive("") {
        }
};





class PlaceList {
    public:
        Place* placeptr;
        PlaceList* next;
        int ausgang;

        PlaceList(Place* plptr, PlaceList* listptr) :
            placeptr(plptr),
            next(listptr),
            ausgang(0) {
        }

        void out();
};




class TransitionList {
    public:
        Transition* transitionptr;
        TransitionList* next;

        TransitionList(Transition* transptr, TransitionList* listptr) :
            transitionptr(transptr),
            next(listptr) {
        }

        void out();
        void list_out();
};





class Stack {
    public:
        Place* placeptr;
        Transition* transptr;
        Stack* next;

        Stack(Place* plptr, Transition* trptr, Stack* listptr) :
            placeptr(plptr),
            transptr(trptr),
            next(listptr) {
        }
};


#endif
