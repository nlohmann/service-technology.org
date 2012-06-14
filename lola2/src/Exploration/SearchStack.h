/*!
\file SearchStack.h
\author Karsten
\status new

\brief Organizes the search stack for dfs or bfs.
*/

#pragma once


/// serves as a chunk of SIZEOF_STACKCHUNK elements in a search stack
class Chunk
{
    public:
        /// previous chunk
        Chunk* prev;

        /// stores loop variable for firelists
        index_t current[SIZEOF_STACKCHUNK];

        /// stores firelists
        index_t* list[SIZEOF_STACKCHUNK];

        Chunk();
        Chunk &operator=(const Chunk &chunk);
};

class SearchStack
{
    public:
        void push(index_t, index_t*);
        void pop(index_t*, index_t**);
        unsigned int StackPointer;
        index_t topTransition() const;

        SearchStack();
        ~SearchStack();

        SearchStack &operator=(const SearchStack &);

    private:
        Chunk* currentchunk;
};
