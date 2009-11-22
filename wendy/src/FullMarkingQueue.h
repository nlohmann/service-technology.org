#ifndef _FULLMARKINGQUEUE_H
#define _FULLMARKINGQUEUE_H

#include <vector>
#include "FullMarking.h"

class FullMarkingQueue {
    public:
        /// the maximal queue length
        static size_t maximal_length;

        /// the initially reserved queue length
        static size_t initial_length;

    private:
        /// a vector to implement the queue
        std::vector<FullMarking> storage;

        /// index of the top element
        size_t first;

        /// index of the bottom element
        size_t last;

    public:
        /// constructor
        FullMarkingQueue();

        /// denstructor
        ~FullMarkingQueue();

        /// returns whether the queue is empty
        bool empty() const;

        /// returns reference to top element
        FullMarking& front();

        /// adds en element to the end of the queue
        void push(const FullMarking&);

        /// removes an element from the top of the queue
        void pop();
};

#endif
