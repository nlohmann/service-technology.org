/*!
\file SearchStack.h
\author Karsten
\status new

\brief Organizes the search stack for dfs or bfs.
*/

#pragma once

#include<new>

/// serves as a chunk of `SIZEOF_STACKCHUNK` elements in a search stack
template <class T>
class Chunk
{
public:
    /// previous chunk
    Chunk * prev;

    /// the content-array of the chunk
    T * content;

    /// stores loop variable for firelists
    // index_t current[SIZEOF_STACKCHUNK];
    //
    /// stores firelists
    // index_t* list[SIZEOF_STACKCHUNK];

    Chunk();
    ~Chunk();
    Chunk &operator=(const Chunk &chunk);
    /// function needed in copy operation for chunks
    void delete_all_prev_chunks();
};

/*!
\brief fast stack class, containing objects of type T
\author Karsten

Simple stack based on chunks of size `SIZEOF_STACKCHUNK.`
If the current top object is accessed and the pop method is called the destructor will _not_ be called, thus the user of the top object has to delete itself.
In case the copy-operator `=` will be used the destructor will be called on all elements currently contained in the chunk.
Be aware that a `pop`-call might result in a free of the memory chunk the pop-result resides in.
So all values stored the stack _should_ be moved before a call to pop.
 */
template<class T>
class SearchStack
{
public:
	/*!
	\brief returns a new stack frame; result is a default-constructed object that takes the payload

	This function will return the pointer to the place for the new top element of the stack.
	The entry itself has to be copied manually to the location of the pointer.

	The  following code fragment gives an example how to place a new `StackElement` in general.

		StackEntry * entry = stack.push();
		entry = new (entry) StackEntry(...);
	It might be noticed that this function will not always allocate new memory. As the implementation uses chunks, the memory may already be allocated.
	*/
    T* push();
    /*!
     \brief release a stack frame, destructor to payload object might be called

     Removes the current top element of the stack.
     This might result in freeing the memory in which the currently resides.
     For reasons of efficiency the call of the destructor and the free-operation may be done later.
     */
    void pop();
    /*!
     \brief access to payload of top stack frame

     Returns a reference to the current top element of the stack frame.
     */
    T& top() const;

    /*!
     \brief current stack pointer
     This contains the current stack pointer. The only reason for beeing public is efficiency in testing whether the stack is empty or not.
     This variable __must not__ be modified form the outside.
     */
    unsigned int StackPointer;

    SearchStack();
    ~SearchStack();

    /*!
     \brief copy operator, this will delete the current content of the stack

     This is the copy operator of a SearchStack.
     The content of the current stack will completely erased and the destructor will be called on every element of the stack.
     The type T is required to have a copy constructor with the signature:

     	 T(T& source)

     For all elements in the old stack this constructor will be called to create the elements in the new stack.
    */
    SearchStack &operator=(const SearchStack &);

private:
    Chunk<T>* currentchunk;
};

#include <cstdlib>
#include <cstring>
#include <Core/Dimensions.h>
#include <Exploration/SearchStack.h>

template<class T>
Chunk<T>::Chunk()
{
    prev = NULL;
    content = (T*) calloc(sizeof(T),SIZEOF_STACKCHUNK);
}

template<class T>
Chunk<T>::~Chunk() {
    free(content);
}


template<class T>
void Chunk<T>::delete_all_prev_chunks() {
    if (!prev)
        return;
    prev->delete_all_prev_chunks();
    // if we want to delete all the prev chunks, we have to delete all its lists,
    // as no one will pop them in the future
    for(unsigned int i = 0; i < SIZEOF_STACKCHUNK; i++)
    {
        prev -> content[i].~T();
    }
    delete prev;

}

//template<class T>
//Chunk<T> &Chunk<T>::operator=(const Chunk<T> &chunk)
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
SearchStack<T>::SearchStack() :
    StackPointer(0), currentchunk(NULL)
{
}

template<class T>
SearchStack<T>::~SearchStack()
{
    if (!currentchunk)
        return;
    currentchunk->delete_all_prev_chunks();
    do
    {
        currentchunk->content[--StackPointer % SIZEOF_STACKCHUNK].~T();
    }
    while((StackPointer % SIZEOF_STACKCHUNK) != 0);
    delete currentchunk;
}

template<class T>
T* SearchStack<T>::push()
{
    if ((StackPointer % SIZEOF_STACKCHUNK) == 0)
    {
        // need new chunk
        Chunk<T>* newchunk = new Chunk<T>;
        newchunk->prev = currentchunk;
        currentchunk = newchunk;
    }
    return(currentchunk->content + (StackPointer++ % SIZEOF_STACKCHUNK));
}

template<class T>
void SearchStack<T>::pop()
{
    StackPointer--;
    if ((StackPointer % SIZEOF_STACKCHUNK) == 0)
    {
        // need to jump to previous chunk
        Chunk<T>* tempchunk = currentchunk;
        currentchunk = currentchunk->prev;
        delete tempchunk;
    }
}

template<class T>
T& SearchStack<T>::top() const
{
    return currentchunk->content[(StackPointer - 1)% SIZEOF_STACKCHUNK];
}

template<class T>
SearchStack<T> &SearchStack<T>::operator=(const SearchStack<T> &stack)
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
        for(unsigned int i = 0; i <(stack.StackPointer % SIZEOF_STACKCHUNK); i++)
        {
            new (currentchunk->content+i) T(stack.currentchunk->content[i]);
        }
    }
    else
    {
        if(currentchunk) {
            delete currentchunk;
            currentchunk = 0;
        }
    }
    StackPointer = stack.StackPointer;

    // for safety
    if (!currentchunk)
        return *this;

    // 2. copy previous chunks

    // 2a. copy as long as both source and target stacks have chunks

    Chunk<T> ** target = &(currentchunk->prev);
    Chunk<T> ** source = &(stack.currentchunk->prev);
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
    if (*target) {
        (*target) -> delete_all_prev_chunks();
        for(unsigned int i = 0; i < SIZEOF_STACKCHUNK; i++)
        {
            (*target) -> content[i].~T();
        }
        delete *target;
        *target = NULL;
    }

    return *this;
}
