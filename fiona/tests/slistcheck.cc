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
 * \file    slistcheck.cc
 *
 * \brief   Checks the implemenation of SList.
 *
 * \author  responsible: Daniela Weinberg <weinberg@informatik.hu-berlin.de>
 */
 
#include <iostream>
#include <vector>
#include "SinglyLinkedList.h"

using namespace std;

/**
 * Adds the sequence held by 'v' to the end of 'slist'.
 */
template<typename T>
void copy(SList<T>& slist, const vector<T>& v) {
    for (typename vector<T>::const_iterator i = v.begin(); i != v.end(); ++i) {
        slist.add(*i);
    }
}

/**
 * Returns true iff 'slist' and 'v' hold the same sequence of elements.
 */
template<typename T>
bool equal(SList<T>& slist, const vector<T>& v) {
    if (slist.size() != v.size()) {
        return false;
    }

    typename vector<T>::size_type index = 0;
    typename SList<T>::Iterator iter = slist.getIterator();
    while (iter->hasNext()) {
        int element = iter->getNext();
        if (v[index] != element) {
            return false;
        }

        ++index;
    }
    delete iter;
    return true;
}

/**
 * Tests adding integers to a new SList.
 */
int test1() {
    vector<int> input;
    input.push_back(11);
    input.push_back(13);
    input.push_back(17);
    input.push_back(19);

    SList<int> list;
    copy(list, input);

    return equal(list, input) ? 0 : 1;
}

/**
 * Tests removing through iterator.
 */
int test2() {
    vector<int> input;
    input.push_back(11);
    input.push_back(13);
    input.push_back(17);

    vector<int> expectedOutput;
    expectedOutput.push_back(13);
    expectedOutput.push_back(17);

    SList<int> list;
    copy(list, input);

    SList<int>::Iterator iter = list.getIterator();
    iter->getNext();
    iter->remove();
    delete iter;

    return equal(list, expectedOutput) ? 0 : 1;
}

/**
 * Tests removing through SList-Interface.
 */
int test3() {
    vector<int> input;
    input.push_back(11);
    input.push_back(13);
    input.push_back(11);
    input.push_back(17);
    input.push_back(11);
    input.push_back(19);
    input.push_back(11);

    vector<int> expectedOutput;
    expectedOutput.push_back(13);
    expectedOutput.push_back(17);
    expectedOutput.push_back(19);

    SList<int> list;
    copy(list, input);

    list.remove(11);

    return equal(list, expectedOutput) ? 0 : 1;
}

/**
 * Tests removing from and adding to a SList.
 */
int test4() {
    vector<int> input;
    input.push_back(11);
    input.push_back(13);
    input.push_back(11);
    input.push_back(17);
    input.push_back(11);
    input.push_back(19);
    input.push_back(11);

    vector<int> expectedOutput;
    expectedOutput.push_back(13);
    expectedOutput.push_back(17);
    expectedOutput.push_back(19);
    expectedOutput.push_back(23);

    SList<int> list;
    copy(list, input);

    list.remove(11);
    list.add(23);

    return equal(list, expectedOutput) ? 0 : 1;
}

/**
 * Tests clearing a SList.
 */
int test5() {
    vector<int> input1;
    input1.push_back(11);
    input1.push_back(13);
    input1.push_back(11);
    input1.push_back(17);
    input1.push_back(11);
    input1.push_back(19);
    input1.push_back(11);

    vector<int> input2;
    input2.push_back(23);
    input2.push_back(29);
    input2.push_back(31);
    input2.push_back(37);

    vector<int> expectedOutput;
    expectedOutput.push_back(29);
    expectedOutput.push_back(31);
    expectedOutput.push_back(41);

    SList<int> list;
    copy(list, input1);

    list.remove(11);
    list.clear();
    if (!equal(list, vector<int>())) {
        return 1;
    }
    copy(list, input2);
    list.remove(23);
    list.remove(37);
    list.add(41);

    return equal(list, expectedOutput) ? 0 : 1;
}

/**
 * Tests isEmpty().
 */
int test6() {
    SList<int> list;

    if (!list.isEmpty()) {
        return 1;
    }

    list.add(5);
    
    if (list.isEmpty()) {
        return 1;
    }

    list.remove(5);
    
    if (!list.isEmpty()) {
        return 1;
    }

    return 0;
}

/**
 * Tests get().
 */
int test7() {
    SList<int> list;

    list.add(2);
    list.add(3);
    list.add(5);

    if (list.get(0) != 2) {
        return 1;
    }

    if (list.get(1) != 3) {
        return 1;
    }

    if (list.get(2) != 5) {
        return 1;
    }

    return 0;
}

typedef int (*testFunction)();

int main() {
    int result = 0;
    int testResult = 0;

    testFunction tests[] = {
        test1,
        test2,
        test3,
        test4,
        test5,
        test6,
        test7,
        NULL   // sentinel to mark end of array
    };

    unsigned int iTest = 0;
    while (tests[iTest] != NULL) {
        cout << "running test " << (iTest + 1) << ": ";

        testResult = tests[iTest]();

        if (testResult == 0) {
            cout << "PASS" << endl;
        } else {
            cout << "FAIL" << endl;
            result += testResult;
        }

        ++iTest;
    }

    return result;
}
