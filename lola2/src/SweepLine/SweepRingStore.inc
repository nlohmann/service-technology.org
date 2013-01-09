/*
\file SweepRingStore.inc
\author Harro
\status new

\brief Stores for transient markings (analogue to PluginStore)

	This store needs additional Lists of stores for new and old
	persistent markings. The lists must have an initial size of
	at least the size of the store itself
*/

#include <cstdio>
#include <cstdlib>
#include <iostream>
//#include <SweepLine/SweepRingStore.h>
#include <Stores/NetStateEncoder/FullCopyEncoder.h>
#include <Stores/NetStateEncoder/CopyEncoder.h>
#include <SweepLine/SweepListStore.h>
#include <SweepLine/SweepStore.h>
#include <Net/NetState.h>

using std::cout;
using std::endl;

/*!
 * \brief Constructor for a ring store for the SweepLine method
 * \param _size Store size, i.e. maximal progress difference between transitions (plus one)
 * \param _front_offset Maximal progress of a single transition
 */
template <class T>
SweepRingStore<T>::SweepRingStore(index_t _size, index_t _front_offset, index_t _transient_offset)
: size(_size), front_offset(_front_offset), active(size-front_offset-1), offset(0), transient_offset(_transient_offset)
{
	assert(size>front_offset);
	// get encoders
	fullencoder = new FullCopyEncoder(1);
	sigpencoder = new CopyEncoder(1);
	// obtain space for counters
	count = (int64_t*) calloc(SIZEOF_INT64_T,size);
	// obtain store ring for transient states
	store = (SweepStore<T>**) malloc(sizeof(SweepStore<T>**)*size);
	for(index_t i=0; i<size; ++i) store[i] = new SweepStore<T>();
	// obtain swap space for current progress measure
	samevalue = new SweepStore<T>();
	// obtain space for pointers to old and new persistent states
	newpersistent = (SweepListStore<T>**) calloc(sizeof(SweepListStore<T>**),size);
	oldpersistent = (SweepListStore<T>**) calloc(sizeof(SweepListStore<T>**),size);
	deleted_store_size = 0;
}

/*!
 * \brief Destructor. Will not free new and old persistent states
 */
template <class T>
SweepRingStore<T>::~SweepRingStore()
{
	delete fullencoder;
	delete sigpencoder;
	for(index_t i=0; i<size; ++i) delete store[i]; 
	free(store);
	free(count);
	delete samevalue;
	free(newpersistent);
	free(oldpersistent);
}

/*!
 * \brief Set the current progress offset for a subsequent call to searchAndInsert
 * \param _offset The offset with respect to the current progress measure
 */
template <class T>
void SweepRingStore<T>::setOffset(int32_t _offset)
{
	offset = _offset;
}

/*!
 * \brief Search for a given state with a given progress measure, insert it if not stored yet
 * \param ns The state to search for
 * \param offset The progress offset of the state ns in relation to the current progress measure in this store
 * \param payload Dummy at this time; additional information for this state could be stores
 * \param thread Dummy; SweepRingStore is single-threaded
 * \return If the state was found in the store (and thus not inserted)
 */
template <class T>
bool SweepRingStore<T>::searchAndInsert(NetState &ns, T** payload, index_t thread)
{
	// calculate the index on our store ring where the state could be
//cout << "size=" << size << " active=" << active << endl;
	index_t pos((active+size+offset)%size);
	// check old persistent markings first, use CopyEncoder
	bitindex_t bitlen;
	vectordata_t* data(sigpencoder->encodeNetState(ns, bitlen, 0));
//cout << "inserting at offset " << offset << ", pos " << pos << ": ";
//printState(ns);

	if (oldpersistent[pos]->store.searchAndInsert(data,bitlen,payload,false)) return true;
	// now check other stores with FullCopyEncoder
	data = fullencoder->encodeNetState(ns, bitlen, 0);
	bool ret(true);
	if (offset>0)
	{
		// future states are made transient, we will see them shortly
		// so check first if they are persistent already
		if (newpersistent[pos]->store.searchAndInsert(data,bitlen,payload,false)) return true;
		// then possibly insert them into a transient store
		if (!(ret = store[pos]->searchAndInsert(data,bitlen,payload)))
		{
			inserted_persistent = false;
			++(count[pos]);
		}
	}
	else if (offset<0)
	{
		// past states must be made persistent, the next front will visit them
		// but check first if we have already seen them ourselves
		if (store[pos]->searchAndInsert(data,bitlen,payload,false)) return true;
		// and only if not, make the new persistent
		if (!(ret = newpersistent[pos]->store.searchAndInsert(data,bitlen,payload)))
		{
			new_persistent_empty = false;
			inserted_persistent = true;
		}
	}
	else
	{
		// successor states with the same progress value must additionally check
		// the swap space before being inserted as transient states like
		// in the future case
		if (newpersistent[pos]->store.searchAndInsert(data,bitlen,payload,false)) return true;
		if (samevalue->searchAndInsert(data,bitlen,payload,false)) return true;
		if (!(ret = store[pos]->searchAndInsert(data,bitlen,payload)))
		{
			inserted_persistent = false;
			++(count[pos]);
		}
	}
	return ret;
}

/*!
 * \brief Get a state with the current progress value.
 *	The state will get relocated from transient storage to the swap space or from
 *	new to old persistent storage during this operation.
 * \param ns Reserved memory for the state to be put in
 * \return If a state with current progress value has been found at all
 */
template <class T>
bool SweepRingStore<T>::popState(NetState& ns)
{
	// get either a transient or new persistent state
	// and transfer it to the samevalue resp. old persistent store
	bitindex_t bitlen;
	vectordata_t* data(NULL);
	// check for a transient state first
	if (store[active]->popVector(data))
	{
		// decode, reencode, and insert into swap space
//cout << "transient popped " << data << " " << &ns << endl;
		fullencoder->decodeNetState(ns, data);
//printState(ns);
		data = fullencoder->encodeNetState(ns, bitlen, 0);
		samevalue->searchAndInsert(data,bitlen,NULL); // need to get payload from store!!!
		return true;
	}
	// if no transient state is found, check for the persistent ones
	if (newpersistent[active]->store.popVector(data))
	{
		// decode, reencode, and insert into old persistent store
//cout << "persistent popped " << data << endl;
		fullencoder->decodeNetState(ns, data);
//printState(ns);
		data = sigpencoder->encodeNetState(ns, bitlen, 0);
		oldpersistent[active]->store.searchAndInsert(data,bitlen,NULL); // need to get payload from store!!!
		return true;
	}
	// no state with current progress measure found
	return false;
}

/*!
 * \brief Advance the current progress measure by one
 * \return If the progress measure could be advanced (i.e. false = this front can be terminated)
 */
template <class T>
bool SweepRingStore<T>::advanceProgress()
{
	// find our foremost and rearmost store
	index_t front(active+front_offset);
	if (front>=size) front -= size;

	// check if all transient stores in front of the active are empty
	index_t pos(active);
	bool store_empty(true);
	while (pos!=front)
	{
		if (++pos==size) pos=0;
//cout << "advance at front=" << front << " pos=" << pos << endl;
		if (!store[pos]->empty()) { store_empty=false; break; }	
	}
	// if additionally we are at the end of the list of persistent markings then we are through
	if (store_empty && !oldpersistent[front]->checkNext()) return false;

	// delete the transients at the back and swap in the transients at the active position
	index_t back(transient_offset>active ? active+size-transient_offset : active-transient_offset);
	if (transient_offset>0) {
		delete store[back];
		store[back] = store[active]; // empty store at this point
		store[active] = samevalue;
	} else delete samevalue;
	deleted_store_size = count[back];
	count[back] = 0;
	samevalue = new SweepStore<T>();

	// link in next persistent stores at the back (= the new front)
	index_t newfront(front+1<size ? front+1 : 0);
	newpersistent[newfront] = newpersistent[front]->getNext();
	oldpersistent[newfront] = oldpersistent[front]->getNext();

	// advance on the ring
	if (++active>=size) active=0;

	return true;
}

/*!
 * \brief Initialise the ring store with pointers to new and old persistent states and set the current progress measure
 * \param oldp Pointer to the minimum progress measure store of old persistent states
 * \param newp Pointer to the minimum progress measure store of new persistent states
 */
template <class T>
void SweepRingStore<T>::init(SweepListStore<T>* oldp, SweepListStore<T>* newp)
{
	// Link in the old and new persistent stores with the ring store
	for(index_t i=0; i<size; ++i)
	{
		if (i) {
			oldp = oldp->getNext();
			newp = newp->getNext();
		}
		oldpersistent[i] = oldp;
		newpersistent[i] = newp;
//cout << "old=" << oldp << " new=" << newp << " tr=" << store[i] << endl;
	}
	// set the current progress measure, i.e. select the correct store on the ring
	active = size-front_offset-1;
//cout << "swap=" << samevalue << endl;
	new_persistent_empty = true;
}

/*!
 * \brief Clear the ring store of transient states
 * \return If the list of new persistent states is also empty
 */
template <class T>
bool SweepRingStore<T>::clear()
{
	// clear all non-empty stores on the ring
	for(index_t i=0; i<size; ++i)
	{
		if (!store[i]->empty())
		{
			delete store[i];
			store[i] = new SweepStore<T>();
		}
//		oldpersistent[i] = NULL;
//		newpersistent[i] = NULL;
	}
	// clear the swap space (usually necessary)
	if (!samevalue->empty())
	{
		delete samevalue;
		samevalue = new SweepStore<T>();
	}
	return new_persistent_empty;
}

/*!
 * \brief check if there are new persistent states that have minimal progress measure with respect to the ring store
 *	Minimal progress on the ring means that this store of new persistent states will be unlinked
 *	from the ring store by the next advanceProgress(). 
 * \return If a new persistent state was found in the minimal progress bucket of the ring store
 */
template <class T>
bool SweepRingStore<T>::checkNewPersistent()
{
	return !(newpersistent[(active+front_offset+1)%size]->store.empty()); 
}

/*!
 * \brief check if the last inserted state has been new persistent. Only valid directly after a successfull insert by searchAndInsert()
 * \return True if it is new persistent state
 */
template <class T>
bool SweepRingStore<T>::insertedIsNewPersistent()
{
	return inserted_persistent;
}

/*!
 * \brief check if the last inserted state has been new persistent. Only valid directly after a successfull insert by searchAndInsert()
 * \return True if it is new persistent state
 */
template <class T>
int64_t SweepRingStore<T>::getNumberOfDeletedStates()
{
//cout << "deleted: " << deleted_store_size << endl;
	return deleted_store_size;
}

template <class T>
void SweepRingStore<T>::printState(NetState& ns)
{
    for (index_t p = 0; p < Net::Card[PL]; ++p)
    {
        if (ns.Current[p] > 0)
	    cout << Net::Name[PL][p] << ":" << ns.Current[p] << " ";
    }
    cout << endl;
}

