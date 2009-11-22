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
#include "FullMarkingQueue.h"


size_t FullMarkingQueue::maximal_length = 0;
size_t FullMarkingQueue::initial_length = 128;
size_t FullMarkingQueue::current_objects = 0;
size_t FullMarkingQueue::maximal_objects = 0;


FullMarkingQueue::FullMarkingQueue()
  : storage(static_cast<FullMarking**>(malloc(initial_length * SIZEOF_VOIDP))),
  first(0), last(0) {
    assert(storage);
    maximal_objects = (++current_objects > maximal_objects) ? current_objects : maximal_objects;
}

FullMarkingQueue::~FullMarkingQueue() {
    maximal_length = (last > maximal_length) ? last : maximal_length;
    --current_objects;

    for (size_t i = 0; i < last; ++i) {
        delete storage[i];
    }
    free(storage);
}

void FullMarkingQueue::push(FullMarking* m) {
    // if storage is full
    if (last++ == initial_length) {
        // double its size and use this size from now on
        initial_length *= 2;
        storage = static_cast<FullMarking**>(realloc(storage, initial_length * SIZEOF_VOIDP));
        assert(storage);
    }
    storage[last-1] = m;
}

FullMarking* FullMarkingQueue::pop() {
    if (first == last) {
        return NULL;
    }

    return storage[first++];
}
