
#ifndef CCSEARCH_H
#define CCSEARCH_H

#include <stdio.h>
#include "Tara.h"
#include <stdlib.h>
#include <pthread.h>
#include "tinythread.h"
#include <vector>
#include "iModification.h"

/** 
 * Concurrent Search
 *
 * Tries to use several threads for wendy checks
 * for better runtime on multi-core archtitectures
 * 
 * The idea of the algorithm is to advance the binary search to
 * k-ary search...
 *
 * */
class CCSearch {
    private:
        /// this function is run by each thread
        static void threadFunction(void* args);
        /** compare to the value, that is search,
         */
        static bool isLessEq(unsigned int x);
    public:
        // constructor
        static void setBounds(unsigned int lowerBound, unsigned int upperBound);
        // search the highes value, which satisfies the predicate "isLessEq(.)"
        static unsigned int search();
        /// the upper bound
        static volatile unsigned int upper;
        /// lower bound
        static volatile unsigned int lower;

};

#endif // include guard
