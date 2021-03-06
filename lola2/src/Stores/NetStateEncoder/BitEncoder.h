/*!
\file BitEncoder.h
\author Christian Koch
\status new
\brief NetStateEncoder implementation that copies the marking bit-perfect with no compression (analogous to BitStore).
*/

#pragma once
#include <Core/Dimensions.h>
#include <Stores/PluginStore.h>

class BitEncoder : public NetStateEncoder
{
public:
    /// constructor; initialize auxiliary data structure (namely "inputs").
    /// @param numThreads maximum number of threads that may work with this NetStateEncoder concurrently.
    BitEncoder(int numThreads);

    /// destructor; frees all memory used for auxiliary data structure (namely "inputs").
    ~BitEncoder();

    vectordata_t *encodeNetState(NetState &ns, bitindex_t &bitlen, index_t threadIndex);
private:
    /// vector of input vectors that are returned from encodeNetState. Each thread has its own vector to avoid conflicts.
    vectordata_t **inputs;

    /// number of words in input vector (fixed for all threads)
    index_t insize;

    /// memcpy can be used only if
    /// - input and suffix tree vectors use the same data type
    /// - the alignment is right (offset is 0 bits)
    /// - all bits are significant for the used places
    ///
    /// to simplify, memcpy is used only for the first couple of places where all conditions are met. memcpylen states the number of such leading places
    index_t memcpylen;
};

