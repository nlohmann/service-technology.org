/*!
\author Sascha Lamp
\file InvStore.h
\status new
*/

#pragma once

#include <Stores/PluginStore.h>
#include <Stores/VectorStores/VectorStore.h>
#include <Stores/NetStateEncoder/NetStateEncoder.h>
#include <Invariants/Invariants.h>

template <typename T>
class TInvStore : public PluginStore<T> {
public:
	TInvStore(TransitionInv*, NetStateEncoder*, VectorStore<T>*, uint32_t);
	
    bool searchAndInsert(NetState &ns, T** payload, uint32_t threadIndex);

private:
	TransitionInv* tinv;
};

#include <Invariants/InvStore.inc>
