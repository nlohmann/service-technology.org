/*!
\author Niels and Max Görner
\file EmptyStore.h
\status new
*/

#pragma once
#include <Stores/Store.h>

template <typename T>
class EmptyStore : public Store<T>
{
public:
    int tries;

    EmptyStore();
    bool searchAndInsert(NetState &ns, T** payload, index_t thread);
};


//////////////////////////////////////////////
/// implementation of EmptyStore.h follows ///
/// for now, just copied + pasted cc-File  ///
//////////////////////////////////////////////

#include <cmath>
//#include <Stores/EmptyStore.h>


template <typename T>
EmptyStore<T>::EmptyStore() : Store<T>(1), tries(0)
{
}

template <typename T>
inline bool EmptyStore<T>::searchAndInsert(NetState &ns, T** payload, index_t thread){
	if(payload) *payload = NULL;
	++this->calls[0]; //We're using "this" since access to protected attributes is harder when using templates than when not.
	return true;
}
