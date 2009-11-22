/*****************************************************************************\
 Wendy -- Synthesizing Partners for Services

 Copyright (c) 2009 Niels Lohmann, Christian Sura, and Daniela Weinberg

 Wendy is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Wendy is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Wendy.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/


#include <config.h>
#include <cstdlib>
#include "Queue.h"


size_t Queue::maximal_length = 0;
size_t Queue::initial_length = 128;
size_t Queue::current_objects = 0;
size_t Queue::maximal_objects = 0;


Queue::Queue()
  : interface(static_cast<InterfaceMarking**>(malloc(initial_length * SIZEOF_VOIDP))),
    inner(static_cast<InnerMarking_ID*>(malloc(initial_length * sizeof(InnerMarking_ID)))),
    first(0), last(0) {
    assert(interface);
    assert(inner);
    maximal_objects = (++current_objects > maximal_objects) ? current_objects : maximal_objects;
}

Queue::~Queue() {
    maximal_length = (last > maximal_length) ? last : maximal_length;
    --current_objects;

    free(interface);
    free(inner);
}

void Queue::push(InnerMarking_ID _inner, InterfaceMarking* _interface) {
    // if storage is full
    if (last++ == initial_length) {
        // double its size and use this size from now on
        initial_length *= 2;
        interface = static_cast<InterfaceMarking**>(realloc(interface, initial_length * SIZEOF_VOIDP));
        inner = static_cast<InnerMarking_ID*>(realloc(inner, initial_length * sizeof(InnerMarking_ID)));
        assert(interface);
        assert(inner);
    }

    inner[last-1] = _inner;
    interface[last-1] = _interface;
}

InterfaceMarking* Queue::popInterface() {
    if (first == last) {
        return NULL;
    }

    return interface[first++];
}

InnerMarking_ID& Queue::popInner() {
    assert(first-1 != last);

    return inner[first-1];
}
