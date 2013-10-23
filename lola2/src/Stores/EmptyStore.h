/*!
\author Niels
\author Max Görner
\author Christian Koch
\file EmptyStore.h
\status new

\brief Store implementation that is always empty. All calls to searchAndInsert() will return false.
*/

#pragma once
#include <Stores/Store.h>
#include <cmath>

template <typename T>
class EmptyStore : public Store<T>
{
public:
    int tries;

    EmptyStore(uint32_t number_of_threads);
    bool searchAndInsert(NetState &ns, T** payload, index_t thread, bool noinsert=false);
};

template <typename T>
EmptyStore<T>::EmptyStore(uint32_t number_of_threads) : Store<T>(number_of_threads), tries(0)
{
}

template <typename T>
inline bool EmptyStore<T>::searchAndInsert(NetState &ns, T** payload, index_t thread, bool noinsert){
	if(payload) *payload = NULL;
	++this->calls[thread]; //We're using "this" since access to protected attributes is harder when using templates than when not.
	return false;
}
