/*!
\file SweepStore.cc
\author Christian Koch
\status new
\brief VectorStore implementation using binary suffix trees. Based on BinStore. Relies on the assumption that different input vectors (possibly of different length) are not prefix of another.
*/

#include <SweepLine/SweepStore.h>

template<>
size_t SweepStore<void>::getPayloadSize()
{
    return 0;
}

