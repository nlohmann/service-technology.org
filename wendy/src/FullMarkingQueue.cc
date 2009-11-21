#include "FullMarkingQueue.h"

size_t FullMarkingQueue::maximal_length = 0;

FullMarkingQueue::~FullMarkingQueue() {
    maximal_length = (last > maximal_length) ? last : maximal_length;
}

FullMarkingQueue::FullMarkingQueue() : first(0), last(0) {
    storage.reserve(10000);
}

bool FullMarkingQueue::empty() const {
    return (first == last);
}

FullMarking& FullMarkingQueue::front() {
    return storage[first];
}

void FullMarkingQueue::push(const FullMarking& m) {
    if (last == 10000) {
        storage.reserve(10000);
    }
    storage.push_back(m);
    ++last;
}

void FullMarkingQueue::pop() {
    ++first;
}
