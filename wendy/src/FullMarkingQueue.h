#ifndef _FULLMARKINGQUEUE_H
#define _FULLMARKINGQUEUE_H

#include "FullMarking.h"

class FullMarkingQueue {
    public:
        /// the maximal queue length
        static size_t maximal_length;

        /// the initially reserved queue length
        static size_t initial_length;

        /// the current number of FullMarkingQueue objects
        static size_t current_objects;

        /// the maximal number of simultaneously stored FullMarkingQueue objects
        static size_t maximal_objects;

    private:
        /// a vector to implement the queue
        FullMarking** storage;

        /// index of the top element
        size_t first;

        /// index of the bottom element
        size_t last;

    public:
        /// constructor
        FullMarkingQueue();

        /// denstructor
        ~FullMarkingQueue();

        /// adds en element to the end of the queue
        void push(FullMarking*);

        /// removes an element from the top of the queue and returns pointer to it
        FullMarking* pop();
};

#endif
