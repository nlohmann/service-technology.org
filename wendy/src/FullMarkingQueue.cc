#include "FullMarkingQueue.h"

size_t FullMarkingQueue::maximal_length = 0;
size_t FullMarkingQueue::initial_length = 128;

FullMarkingQueue::~FullMarkingQueue() {
    maximal_length = (last > maximal_length) ? last : maximal_length;
}

FullMarkingQueue::FullMarkingQueue() : first(0), last(0) {
    storage.reserve(initial_length);
}

bool FullMarkingQueue::empty() const {
    return (first == last);
}

FullMarking& FullMarkingQueue::front() {
    return storage[first];
}

void FullMarkingQueue::push(const FullMarking& m) {
    // if storage is full
    if (last++ == initial_length) {
        // double its size and use this size from now on
        storage.reserve(initial_length);
        initial_length *= 2;
    }
    storage.push_back(m);
}

void FullMarkingQueue::pop() {
    ++first;
}
