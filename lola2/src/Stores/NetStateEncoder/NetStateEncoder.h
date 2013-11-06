/*!
\file NetStateEncoder.h
\author Christian Koch
\status new
\brief NetStateEncoder provides an interface for converting NetState markings into input vectors.
*/

#pragma once
#include <Core/Dimensions.h>
#include <Net/NetState.h>

class NetStateEncoder
{
protected:
    /// the maximum number of threads this NetStateEncoder has to work with.
    /// The value is used to create thread-local auxiliary data structures if needed.
    int numThreads;

public:
    /// constructor; initialize auxiliary data structures if needed.
    /// @param _numThreads maximum number of threads that may work with this NetStateEncoder concurrently.
    NetStateEncoder(int _numThreads) : numThreads(_numThreads) {}

    /// destructor; frees all memory used for auxiliary data structures
    virtual ~NetStateEncoder() {}

    /// converts the given NetState marking into an input vector.
    /// @param ns input: NetState that needs to be converted
    /// @param bitlen output: reference to length of input vector (in bits). Will be filled by the method.
    /// @param threadIndex input: the index of the thread that requests this call. Values will range from 0 to (numThreads - 1). Used to allow using thread-local auxiliary data structures without locking any variables.
    /// @return the final input vector.
    virtual vectordata_t *encodeNetState(NetState &ns, bitindex_t &bitlen, index_t threadIndex) = 0;

    /// decodes a given encoded state and sets the netstate appropriately
    /// @param ns NetState the decoded state will be written to
    /// @param data vector to be decoded
    virtual void decodeNetState(NetState &ns, vectordata_t *data);
};
