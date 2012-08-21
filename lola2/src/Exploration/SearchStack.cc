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

template<class T>
Chunk::Chunk()
{
    prev = NULL;
    content = calloc(sizeof(T),SIZEOF_STACKCHUNK);
}

template<class T>
Chunk::~Chunk(){
delete content;
}


template<class T>
void Chunk::delete_all_prev_chunks(){
	if (!prev)
		return;
	prev->delete_all_prev_chunks();
	// if we want to delete all the prev chunks, we have to delete all its lists,
	// as no one will pop them in the future
	for(unsigned int i = 0; i < SIZEOF_STACKCHUNK;i++)
	{
		prev -> content[i].~T();
	}
	delete prev;

}

//template<class T>
//Chunk &Chunk::operator=(const Chunk &chunk)
//{
//    // copy the current values
//    memcpy(current, chunk.current, SIZEOF_STACKCHUNK * SIZEOF_INDEX_T);
//    for (int i = 0; i < SIZEOF_STACKCHUNK; i++)
//    {
//    	if (list[i])
//    		delete[] list[i];
//
//    	if (chunk.list[i])
//        {
//            list[i] = new index_t[current[i] + 1];
//            for (int j = 0; j <= current[i]; j++)
//                list[i][j] = chunk.list[i][j];
//        } else
//        	list[i] = 0;
//    }
//
//    if (chunk.prev != NULL)
//    {
//    	// if there is not next element to copy onto create one
//    	if (!prev)
//    		prev = new Chunk();
//        *prev = *(chunk.prev);
//    }
//    else{
//    	if (prev)
//    		delete_all_prev_chunks();
//        prev = NULL;
//    }
//}

template<class T>
SearchStack::SearchStack() :
    StackPointer(0), currentchunk(NULL)
{
}

template<class T>
SearchStack::~SearchStack()
{
	if (!currentchunk)
		return;
   currentchunk->delete_all_prev_chunks();
   do
   {
	content[StackPointer % SIZEOF_STACKCHUNK].~T();
   }
   while((--StackPointer % SIZEOF_STACKCHUNK) != 0)
   delete currentchunk;
}

template<class t>
T* SearchStack::push()
{
    if ((StackPointer % SIZEOF_STACKCHUNK) == 0)
    {
        // need new chunk
        Chunk* newchunk = new Chunk();
        newchunk->prev = currentchunk;
        currentchunk = newchunk;
    }
    return(currentchunk->content + (StackPointer++ % SIZEOF_STACKCHUNK));
}

template<class T>
void SearchStack::pop()
{
    content[--StackPointer % SIZEOF_STACKCHUNK].~T();
    if ((StackPointer % SIZEOF_STACKCHUNK) == 0)
    {
        // need to jump to previous chunk
        Chunk* tempchunk = currentchunk;
        currentchunk = currentchunk->prev;
        delete tempchunk;
    }
}

template<class T>
T& SearchStack::top() const
{
    return currentchunk->content[(StackPointer - 1)% SIZEOF_STACKCHUNK];
}


template<class T>
SearchStack<T> &SearchStack::operator=(const SearchStack<T> &stack)
{
	// 1. copy top chunk
	if(currentchunk)
	{
		do
		{
			currentchunk->content[(--StackPointer)%SIZEOF_STACKCHUNK].~T();
		}
		while((StackPointer % SIZEOF_STACKCHUNK) != 0);
	}
	if(stack.currentchunk)
	{
		if(!currentchunk) currentchunk = new Chunk<T>();
		for(unsigned int i = 0; i <(StackPointer % SIZEOF_STACKCHUNK); i++)
		{
			new (currentchunk.content+i) T(stack.currentchunk.content[i]);
		}
	}
	else
	{
		if(currentchunk) delete currentchunk;
	}
	StackPointer = stack.StackPointer;
	
	// 2. copy previous chunks

	// 2a. copy as long as both source and target stacks have chunks

	Chunk<T> ** target = &prev;
	Chunk<T> ** source = &stack.prev;
	while(*target && *source)

	{
		for(unsigned int i = 0; i < SIZEOF_STACKCHUNK; i++)
		{
			(*target) -> content[i].~T();
			new ((*target)->content + i) T((*source)->content[i]);
		}
		target = &((*target) -> prev);
		source = &((*source) -> prev);
	}

	// 2b. create new chunks at target if source has spare chunks
	while(*source)
	{
		*target = new Chunk<T>();
		(*target) -> prev = NULL;
		for(unsigned int i = 0; i < SIZEOF_STACKCHUNK; i++)
		{
			new ((*target)->content + i) T((*source)->content[i]);
		}
		target = &((*target) -> prev);
		source = &((*source) -> prev);
	}
	// 2c. release spare chunks at target
	Chunk<T> * targ = * target;
	while(*target)
	{
		for(unsigned int i = 0; i < SIZEOF_STACKCHUNK; i++)
		{
			(*target) -> content[i].~T();
		}
		target = &((*target) -> prev);
	}
	while(targ)
	{
		Chunk<T> tmp = targ;
		targ = targ -> prev;
		delete tmp;
	}
	
    return *this;
}
