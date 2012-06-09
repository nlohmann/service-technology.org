/*!
 \file SearchStack.cc
 \author Karsten
 \status new

 \brief Organizes the search stack for dfs or bfs.
 */

#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <Core/Dimensions.h>
#include <Exploration/SearchStack.h>


Chunk::Chunk() {
    prev = NULL;
    for (int i = 0; i < SIZEOF_STACKCHUNK; i++)
        list[i] = NULL;
}

Chunk& Chunk::operator=(const Chunk& chunk) {
    // copy the current values
    memcpy(current, chunk.current, SIZEOF_STACKCHUNK * SIZEOF_INDEX_T);
    for (int i = 0; i < SIZEOF_STACKCHUNK; i++) {
        if (chunk.list[i]) {
            list[i] = (index_t*) calloc(current[i] + 1, SIZEOF_INDEX_T);
            for (int j = 0; j <= current[i]; j++)
                list[i][j] = chunk.list[i][j];
        }
    }

    if (prev != NULL)
        delete prev;

    if (chunk.prev != NULL) {
        prev = new Chunk();
        *prev = *(chunk.prev);
    } else
        prev = NULL;
}

SearchStack::SearchStack() :
    StackPointer(0), currentchunk(NULL) {
}

void SearchStack::push(index_t c, index_t* f) {
    if ((StackPointer % SIZEOF_STACKCHUNK) == 0) {
        // need new chunk
        Chunk* newchunk = new Chunk();
        newchunk->prev = currentchunk;
        currentchunk = newchunk;
    }
    currentchunk->current[StackPointer % SIZEOF_STACKCHUNK] = c;
    currentchunk->list[StackPointer++ % SIZEOF_STACKCHUNK] = f;
}

/*!
 \param[in,out] c  index within firelist
 \param[in,out] f  firelist
 */
void SearchStack::pop(index_t* c, index_t** f) {
    *c = currentchunk->current[(--StackPointer) % SIZEOF_STACKCHUNK];
    currentchunk->current[(StackPointer) % SIZEOF_STACKCHUNK] = 0;
    *f = currentchunk->list[StackPointer % SIZEOF_STACKCHUNK];
    currentchunk->list[StackPointer % SIZEOF_STACKCHUNK] = 0;
    if ((StackPointer % SIZEOF_STACKCHUNK) == 0) {
        // need to jump to previous chunk
        Chunk* tempchunk = currentchunk;
        currentchunk = currentchunk->prev;
        delete tempchunk;
    }
}

index_t SearchStack::topTransition() const {
    return currentchunk->list[(StackPointer - 1) % SIZEOF_STACKCHUNK][currentchunk->current[(StackPointer
            - 1) % SIZEOF_STACKCHUNK]];
}


SearchStack& SearchStack::operator=(const SearchStack& stack) {
    StackPointer = stack.StackPointer;
    //printf("COPY STACK %d\n", StackPointer);

    if (currentchunk != NULL)
        delete currentchunk;
    if (stack.currentchunk != NULL) {
        currentchunk = new Chunk();
        *currentchunk = *(stack.currentchunk);
    } else
        currentchunk = NULL;
    return *this;
}
