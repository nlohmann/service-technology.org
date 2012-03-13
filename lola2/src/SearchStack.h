/*!
\file SearchStack.h
\author Karsten
\status new
\brief organizes the search stack for dfs or bfs
*/

/// serves as a chunk of SIZEOF_STACKCHUNK elements in a search stack
class Chunk
{
    public:
        Chunk() {}
        /// previous chunk
        Chunk* prev;
        /// stores loop variable for firelists
        index_t current[SIZEOF_STACKCHUNK];
        /// stores firelists
        index_t* list[SIZEOF_STACKCHUNK];
};

class SearchStack
{
    public:
        SearchStack();
        void push(index_t, index_t*);
        void  pop(index_t*, index_t**);
        unsigned int StackPointer;
    private:
        Chunk* currentchunk;
};
