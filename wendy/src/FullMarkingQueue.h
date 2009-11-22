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
