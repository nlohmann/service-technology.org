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
