/*!
 \file SearchStack.cc
 \author Karsten
 \status new

 \brief Organizes the search stack for dfs or bfs.
 */

#include <cstdlib>
#include <cstring>
#include <Core/Dimensions.h>
#include <Exploration/SearchStack.h>


Chunk::Chunk()
{
	created_by_copy = false;
    prev = NULL;
    for (int i = 0; i < SIZEOF_STACKCHUNK; i++)
        list[i] = NULL;
}

Chunk::~Chunk(){
	if (created_by_copy)
	for (int i = 0; i < SIZEOF_STACKCHUNK; i++)
		if (list[i])
			delete[] list[i];
}

void Chunk::delete_all_prev_chunks(){
	if (!prev)
		return;
	prev->delete_all_prev_chunks();
	// if we want to delete all the prev chunks, we have to delete all its lists,
	// as no one will pop them in the future
	prev->created_by_copy = true;
	delete prev;
}

Chunk &Chunk::operator=(const Chunk &chunk)
{
    // copy the current values
    memcpy(current, chunk.current, SIZEOF_STACKCHUNK * SIZEOF_INDEX_T);
    created_by_copy = true;
    for (int i = 0; i < SIZEOF_STACKCHUNK; i++)
        if (chunk.list[i])
        {
        	if (list[i] && created_by_copy)
        		delete[] list[i];
            list[i] = new index_t[current[i] + 1];
            for (int j = 0; j <= current[i]; j++)
                list[i][j] = chunk.list[i][j];
        }

    if (chunk.prev != NULL)
    {
    	// if there is not next element to copy onto create one
    	if (!prev)
    		prev = new Chunk();
        *prev = *(chunk.prev);
    }
    else{
    	if (prev)
    		delete_all_prev_chunks();
        prev = NULL;
    }
}

SearchStack::SearchStack() :
    StackPointer(0), currentchunk(NULL)
{
}

SearchStack::~SearchStack()
{
	if (!currentchunk)
		return;
   currentchunk->delete_all_prev_chunks();
   currentchunk->created_by_copy = true;
   delete currentchunk;
}

void SearchStack::push(index_t c, index_t* f)
{
    if ((StackPointer % SIZEOF_STACKCHUNK) == 0)
    {
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
void SearchStack::pop(index_t* c, index_t** f)
{
    *c = currentchunk->current[(--StackPointer) % SIZEOF_STACKCHUNK];
    currentchunk->current[(StackPointer) % SIZEOF_STACKCHUNK] = 0;
    *f = currentchunk->list[StackPointer % SIZEOF_STACKCHUNK];
    currentchunk->list[StackPointer % SIZEOF_STACKCHUNK] = 0;
    if ((StackPointer % SIZEOF_STACKCHUNK) == 0)
    {
        // need to jump to previous chunk
        Chunk* tempchunk = currentchunk;
        currentchunk = currentchunk->prev;
        delete tempchunk;
    }
}

index_t SearchStack::topTransition() const
{
    return currentchunk->list[(StackPointer - 1) % SIZEOF_STACKCHUNK][currentchunk->current[(StackPointer
                                                                      - 1) % SIZEOF_STACKCHUNK]];
}


SearchStack &SearchStack::operator=(const SearchStack &stack)
{
	// first copy the stack pointer, they must be the same
    StackPointer = stack.StackPointer;

    if (stack.currentchunk != NULL)
    {
    	// should happen rarely
    	if (!currentchunk)
    		currentchunk = new Chunk();
    	// this almost in all cases
        *currentchunk = *(stack.currentchunk);
    }
    else
    {
    	if (currentchunk){
    		currentchunk->delete_all_prev_chunks();
    		// delete all the lists (see Chunk::delete_all_prev_chunks for explanation)
    		currentchunk->created_by_copy = true;
    		delete currentchunk;
    	}
        currentchunk = NULL;
    }
    return *this;
}
