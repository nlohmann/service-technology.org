/*!
\file SearchStack.h
\author Karsten
\status new

\brief Organizes the search stack for dfs or bfs.

\todo Replace calloc by new - currently, this does not work due to this error message: "error: no matching constructor for initialization of 'LTLFairnessStackEntry'"
*/

#pragma once

#include<new>
#include <algorithm>

/// serves as a chunk of `SIZEOF_STACKCHUNK` elements in a search stack
template <class T>
class Chunk
{
public:
    /// previous chunk
    Chunk *prev;

    /// the content-array of the chunk
    T *content;

    /// default constructor
    Chunk();

    /// custom copy constructor
    Chunk(const Chunk *chunk, unsigned int numElements);
    ~Chunk();

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
    T *push();
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
    T &top() const;

    SearchStack();
    SearchStack(const SearchStack &);
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
    void swap(SearchStack &);

    /*!
     \brief current stack pointer
     This contains the current stack pointer. The only reason for being public is efficiency in testing whether the stack is empty or not.
     This variable __must not__ be modified form the outside.
     */
    unsigned int StackPointer;

private:
    Chunk<T> *currentchunk;
};

#include <cstdlib>
#include <cstring>
#include <Core/Dimensions.h>
#include <Exploration/SearchStack.h>

template<class T>
Chunk<T>::Chunk(const Chunk<T> *chunk, unsigned int numElements)
{
    content = (T *) calloc(sizeof(T), SIZEOF_STACKCHUNK);
    for (int i = 0; i < numElements; i++)
    {
        new (content + i) T(chunk->content[i]);
    }
    if (chunk->prev)
    {
        prev = new Chunk<T>(chunk->prev, SIZEOF_STACKCHUNK);
    }
    else
    {
        prev = NULL;
    }
}

template<class T>
Chunk<T>::Chunk()
{
    prev = NULL;
    content = (T *) calloc(sizeof(T), SIZEOF_STACKCHUNK);
}

template<class T>
Chunk<T>::~Chunk()
{
    free(content);
}


template<class T>
void Chunk<T>::delete_all_prev_chunks()
{
    if (!prev)
    {
        return;
    }
    prev->delete_all_prev_chunks();
    // if we want to delete all the prev chunks, we have to delete all its lists,
    // as no one will pop them in the future
    for (unsigned int i = 0; i < SIZEOF_STACKCHUNK; i++)
    {
        prev -> content[i].~T();
    }
    delete prev;

    prev = NULL;

}

template<class T>
SearchStack<T>::SearchStack() :
    StackPointer(0), currentchunk(NULL)
{
}

template<class T>
SearchStack<T>::~SearchStack()
{
    if (!currentchunk)
    {
        return;
    }
    currentchunk->delete_all_prev_chunks();
    do
    {
        currentchunk->content[--StackPointer % SIZEOF_STACKCHUNK].~T();
    }
    while ((StackPointer % SIZEOF_STACKCHUNK) != 0);
    delete currentchunk;
}

template<class T>
T *SearchStack<T>::push()
{
    if ((StackPointer % SIZEOF_STACKCHUNK) == 0)
    {
        // need new chunk
        Chunk<T> *newchunk = new Chunk<T>;
        if (StackPointer == 0)
        {
            assert(!currentchunk);
        }
        newchunk->prev = currentchunk;
        currentchunk = newchunk;
    }
    return (currentchunk->content + (StackPointer++ % SIZEOF_STACKCHUNK));
}

template<class T>
void SearchStack<T>::pop()
{
    StackPointer--;
    if ((StackPointer % SIZEOF_STACKCHUNK) == 0)
    {
        // need to jump to previous chunk
        Chunk<T> *tempchunk = currentchunk;
        currentchunk = currentchunk->prev;
        delete tempchunk;
    }
}

template<class T>
T &SearchStack<T>::top() const
{
    return currentchunk->content[(StackPointer - 1) % SIZEOF_STACKCHUNK];
}

template<class T>
void SearchStack<T>::swap(SearchStack<T> &s)
{
    std::swap(currentchunk, s.currentchunk);
    std::swap(StackPointer, s.StackPointer);
}

template<class T>
SearchStack<T>::SearchStack(const SearchStack<T> &stack)
{
    StackPointer = stack.StackPointer;
    if (stack.currentchunk)
    {
        currentchunk = new Chunk<T>(stack.currentchunk, (stack.StackPointer - 1) % SIZEOF_STACKCHUNK + 1);
    }
    else
    {
        currentchunk = NULL;
    }
}

template<class T>
SearchStack<T> &SearchStack<T>::operator=(const SearchStack<T> &stack)
{
    SearchStack<T> tmp(stack); // copy and swap
    swap(tmp);
    return *this;
}
