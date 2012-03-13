/*****************************************************************************\
 Marlene -- synthesizing behavioral adapters

 Copyright (C) 2009  Christian Gierds <gierds@informatik.hu-berlin.de>

 Marlene is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Marlene is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Marlene.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

#pragma once

#include <iostream>
#include <string>
#include "config.h"
#include "pruner-cmdline.h"
#include "pruner-verbose.h"

using std::tr1::shared_ptr;

// some initial values, which are the default values

extern gengetopt_args_info args_info;
extern void evaluate_command_line(int argc, char* argv[]);

extern bool fileExists(std::string filename);

extern std::string toString(int i);

template<typename Keytype>
class List {

public:
    typedef shared_ptr< List<Keytype> > List_ptr;

    List(Keytype value);
    ~List();

    void push_back(Keytype value);
    void push_back(List_ptr list);
    Keytype getValue();
    List_ptr getNext();

    unsigned int length();
    static bool contains(List_ptr list, Keytype value);

private:
    Keytype value;
    List_ptr next;
};

template<typename type1, typename type2>
class Pair {

public:

    Pair(type1 first, type2 second);

    type1 getFirst();
    type2 getSecond();

private:
    type1 first;
    type2 second;

};

/* Template code */


template <typename Keytype>
List<Keytype>::List(Keytype value) {
    this->value = value;
}

template <typename Keytype>
List<Keytype>::~List() {
    next = shared_ptr< List >();
}

template <typename Keytype>
void List<Keytype>::push_back(Keytype value) {

    if ( next.get() == 0) {
        next = List_ptr(new List(value));
    }
    else {
        next->push_back(value);
    }
}

template <typename Keytype>
void List<Keytype>::push_back(List_ptr list) {

    if ( next.get() == 0) {
        next = list;
    }
    else {
        next->push_back(list);
    }
}

template <typename Keytype>
Keytype List<Keytype>::getValue() {
    return value;
}

template <typename Keytype>
typename List<Keytype>::List_ptr List<Keytype>::getNext() {
    return next;
}

template <typename Keytype>
unsigned int List<Keytype>::length() {
    if (next.get() == 0)
        return 1;
    else return 1 + next->length();
}

template <typename Keytype>
bool List<Keytype>::contains(List_ptr list, Keytype value) {
    if (list.get() == 0) {
        return false;
    }
    List_ptr next = list;
    do {
        if (next->getValue == value) {
            return true;
        }
        list = list->getNext();
    } while (list.get() != 0);
    return false;
}

template <typename type1, typename type2>
Pair<type1, type2>::Pair(type1 first, type2 second) {
    this->first = first;
    this->second = second;
}

template <typename type1, typename type2>
type1 Pair<type1, type2>::getFirst() {
    return first;
}

template <typename type1, typename type2>
type2 Pair<type1, type2>::getSecond() {
    return second;
}
