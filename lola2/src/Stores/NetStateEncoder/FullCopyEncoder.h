/*!
\file CopyEncoder.h
\author Christian Koch
\status new
\brief NetStateEncoder implementation that copies the marking, while ignoring capacity limitations. The copy operation isn't done at all if possible (just passing the marking pointer), otherwise memcpy is used
*/

#pragma once
#include <Core/Dimensions.h>
#include <Stores/PluginStore.h>

class FullCopyEncoder : public NetStateEncoder
{
public:
    /// constructor
    /// @param numThreads maximum number of threads that may work with this NetStateEncoder concurrently.
    FullCopyEncoder(int numThreads);
    /// destructor
    ~FullCopyEncoder();

    /// converts the given NetState marking into an input vector.
    /// @param ns input: NetState that needs to be converted
    /// @param bitlen output: reference to length of input vector (in bits). Will be filled by the method.
    /// @param threadIndex input: the index of the thread that requests this call. Values will range from 0 to (numThreads - 1). Used to allow using thread-local auxiliary data structures without locking any variables.
    /// @return the final input vector.
    vectordata_t *encodeNetState(NetState &ns, bitindex_t &bitlen, index_t threadIndex);

    /// decodes a given encoded state and sets the netstate appropriately
    /// @param ns NetState the decoded state will be written to
    /// @param data vector to be decoded
    virtual void decodeNetState(NetState &ns, vectordata_t *data);
private:

    /// number of words in input vector
    index_t insize;

    /// instead of copying the data, the marking pointer can be casted directly into the input vector pointer if:
    /// a) the marking bit width is divisble by the input vector data width
    ///    or
    /// b) the marking vector has at least as many bytes as the needed input vector
    /// a) will be checked at compile time whereas b) will be checked at runtime. Only if both tests fail memcpy is actually used.
#if SIZEOF_CAPACITY_T % SIZEOF_VECTORDATA_T != 0
    /// states whether condition (b) is met. Will be determined once in the constructor.
    bool nocopy;

    /// vector of input vectors that are returned from encodeNetState. Each thread has its own vector to avoid conflicts. Only needed if memcpy is actually used.
    vectordata_t **inputs;
#endif
};
