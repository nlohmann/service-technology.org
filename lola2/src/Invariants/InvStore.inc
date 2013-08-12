/*!
\author Sascha Lamp
\file InvStore.inc
\status new
*/

#include <Invariants/Invariants.h>
#include <Core/Dimensions.h>
#include <Net/Net.h>

#include <iostream>

template <typename T>
TInvStore<T>::TInvStore(TransitionInv* tinv, NetStateEncoder* _netStateEncoder, VectorStore<T>* _vectorStore, uint32_t _number_of_threads)
		: PluginStore<T>(_netStateEncoder, _vectorStore, _number_of_threads), tinv(tinv), depth(0) {
	this->tinv->solve();
}

template <typename T>
bool TInvStore<T>::searchAndInsert(NetState &ns, T** payload, uint32_t threadIndex) {
	// count calls
    ++(this->calls[threadIndex]);

    // fetch input vector
    bitindex_t bitlen;
    vectordata_t* input = this->netStateEncoder->encodeNetState(ns, bitlen, threadIndex);

	// check whether state should be saved
	bool noinsert = true;
	for(int i = 0; i < Net::Card[TR]; i++) {
		//TODO more efficiency
		if(ns.Enabled[i] && this->tinv->u[i]) {
			noinsert = false;
			break;
		}
	}

    // check input vector in vector store
    bool ret = this->vectorStore->searchAndInsert(input, bitlen, ns.HashCurrent, payload, threadIndex, noinsert);
    if (!ret && !noinsert) {
        ++(this->markings[threadIndex]);
    }
    return ret;
}
