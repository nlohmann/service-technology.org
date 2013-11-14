/*!
\file PrefixTreeStore.cc
\author Christian Koch
\status new
\brief VectorStore implementation using binary suffix trees. Based on BinStore. Relies on the assumption that different input vectors (possibly of different length) are not prefix of another.
*/

#include <Stores/VectorStores/PrefixTreeStore.h>

template<>
size_t PrefixTreeStore<void>::getPayloadSize()
{
    return 0;
}

