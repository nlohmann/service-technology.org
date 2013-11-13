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
#include <Stores/NetStateEncoder/FullCopyEncoder.h>
#include <Stores/NetStateEncoder/CopyEncoder.h>
#include <Stores/VectorStores/PrefixTreeStore.h>
#include <SweepLine/SweepListStore.h>
#include <Net/NetState.h>

extern gengetopt_args_info args_info;
extern Reporter *rep;

using std::cout;
using std::endl;

/*!
 * \brief Constructor for a ring store for the SweepLine method
 * \param _size Store size, i.e. maximal progress difference between transitions (plus one)
 * \param _front_offset Maximal progress of a single transition
 */
template <class T>
SweepRingStore<T>::SweepRingStore(index_t _size, index_t _front_offset, index_t _transient_offset,
                                  FullCopyEncoder *fce, NetStateEncoder *nse, index_t _nr_of_threads, hash_t _nr_of_buckets)
    : size(_size), front_offset(_front_offset), active(size - front_offset - 1),
      transient_offset(_transient_offset), nr_of_threads(_nr_of_threads), nr_of_buckets(_nr_of_buckets)
{
    assert(size > front_offset);
    // get encoders
    fullencoder = fce;
    sigpencoder = nse;
    // obtain space for counters
    count = new int64_t[SIZEOF_INT64_T]();
    // obtain store ring for transient states
    store = new VectorStore<T> *[size];
    for (index_t i = 0; i < size; ++i)
    {
        store[i] = createSweepStore();
    }
    // obtain swap space for current progress measure
    samevalue = createSweepStore();
    // obtain space for pointers to old and new persistent states
    newpersistent = new SweepListStore<T> *[sizeof(SweepListStore<T> **)]();
    oldpersistent = new SweepListStore<T> *[sizeof(SweepListStore<T> **)]();
    inserted_persistent = new bool[SIZEOF_BOOL];
    deleted_store_size = 0;
}

/*!
 * \brief Destructor. Will not free new and old persistent states
 */
template <class T>
SweepRingStore<T>::~SweepRingStore()
{
    for (index_t i = 0; i < size; ++i)
    {
        delete store[i];
    }
    delete[] store;
    delete[] count;
    delete samevalue;
    delete[] newpersistent;
    delete[] oldpersistent;
    delete[] inserted_persistent;
}

/*!
 * \brief Search for a given state with a given progress measure, insert it if not stored yet
 * \param ns The state to search for
 * \param offset The progress offset of the state ns in relation to the current progress measure in this store
 * \param payload Dummy at this time; additional information for this state could be stored
 * \param thread The active thread number
 * \return If the state was found in the store (and thus not inserted)
 */
template <class T>
bool SweepRingStore<T>::searchAndInsert(NetState &ns, int32_t offset, T **payload, index_t thread)
{
    // calculate the index on our store ring where the state could be
    //cout << "size=" << size << " active=" << active << endl;
    index_t pos((active + size + offset) % size);
    // check old persistent markings first, use encoder selected by command line
    bitindex_t bitlen;
    vectordata_t *data(sigpencoder->encodeNetState(ns, bitlen, thread));
    hash_t hash(ns.HashCurrent % nr_of_buckets);
    //cout << "inserting at offset " << offset << ", pos " << pos << ": ";
    //printState(ns);

    if (oldpersistent[pos]->store->searchAndInsert(data, bitlen, hash, payload, thread, true))
    {
        return true;
    }
    // now check other stores with FullCopyEncoder
    data = fullencoder->encodeNetState(ns, bitlen, thread);
    bool ret(true);
    if (offset > 0)
    {
        // future states are made transient, we will see them shortly
        // so check first if they are persistent already
        if (newpersistent[pos]->store->searchAndInsert(data, bitlen, hash, payload, thread, true))
        {
            return true;
        }
        // then possibly insert them into a transient store
        if (!(ret = store[pos]->searchAndInsert(data, bitlen, hash, payload, thread)))
        {
            inserted_persistent[thread] = false;
            ++(count[pos]);
        }
    }
    else if (offset < 0)
    {
        // past states must be made persistent, the next front will visit them
        // but check first if we have already seen them ourselves
        if (store[pos]->searchAndInsert(data, bitlen, hash, payload, thread, true))
        {
            return true;
        }
        // and only if not, make them new persistent
        if (!(ret = newpersistent[pos]->store->searchAndInsert(data, bitlen, hash, payload, thread)))
        {
            new_persistent_empty = false;
            inserted_persistent[thread] = true;
        }
    }
    else
    {
        // successor states with the same progress value must additionally check
        // the swap space before being inserted as transient states like
        // in the future case
        if (newpersistent[pos]->store->searchAndInsert(data, bitlen, hash, payload, thread, true))
        {
            return true;
        }
        if (samevalue->searchAndInsert(data, bitlen, hash, payload, thread, true))
        {
            return true;
        }
        if (!(ret = store[pos]->searchAndInsert(data, bitlen, hash, payload, thread)))
        {
            inserted_persistent[thread] = false;
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
bool SweepRingStore<T>::popState(NetState &ns, index_t thread)
{
    // get either a transient or new persistent state
    // and transfer it to the samevalue resp. old persistent store
    bitindex_t bitlen;
    vectordata_t *data(NULL);
    hash_t hash(0);
    // check for a transient state first
    if (store[active]->popVector(data, thread))
    {
        // decode, reencode, and insert into swap space
        //cout << "transient popped " << data << " " << &ns << endl;
        fullencoder->decodeNetState(ns, data);

        if (nr_of_buckets > 1)
        {
            hash = (ns.HashCurrent = ((HashingWrapperStore<T> *)(store[active]))->getLastHash(thread));
        }

        //printState(ns);
        data = fullencoder->encodeNetState(ns, bitlen, thread);
        samevalue->searchAndInsert(data, bitlen, hash, NULL, thread); // need to get payload from store!!!
        return true;
    }

    // if no transient state is found, check for the persistent ones
    if (newpersistent[active]->store->popVector(data, thread))
    {
        // decode, reencode, and insert into old persistent store
        //cout << "persistent popped " << data << endl;
        fullencoder->decodeNetState(ns, data);
        //printState(ns);

        if (nr_of_buckets > 1)
        {
            hash = (ns.HashCurrent = ((HashingWrapperStore<T> *)(newpersistent[active]->store))->getLastHash(
                                         thread));
        }

        data = sigpencoder->encodeNetState(ns, bitlen, thread);
        oldpersistent[active]->store->searchAndInsert(data, bitlen, hash, NULL,
                thread); // need to get payload from store!!!
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
    index_t front(active + front_offset);
    if (front >= size)
    {
        front -= size;
    }

    // check if all transient stores in front of the active are empty
    index_t pos(active);
    bool store_empty(true);
    while (pos != front)
    {
        if (++pos == size)
        {
            pos = 0;
        }
        //cout << "advance at front=" << front << " pos=" << pos << endl;
        if (!store[pos]->empty())
        {
            store_empty = false;
            break;
        }
    }
    // if additionally we are at the end of the list of persistent markings then we are through
    if (store_empty && !oldpersistent[front]->checkNext())
    {
        return false;
    }

    // delete the transients at the back and swap in the transients at the active position
    index_t back(transient_offset > active ? active + size - transient_offset : active -
                 transient_offset);
    if (transient_offset > 0)
    {
        delete store[back];
        store[back] = store[active]; // empty store at this point
        store[active] = samevalue;
    }
    else
    {
        delete samevalue;
    }
    deleted_store_size = count[back];
    count[back] = 0;
    samevalue = createSweepStore();

    // link in next persistent stores at the back (= the new front)
    index_t newfront(front + 1 < size ? front + 1 : 0);
    newpersistent[newfront] = newpersistent[front]->getNext();
    oldpersistent[newfront] = oldpersistent[front]->getNext();

    // advance on the ring
    if (++active >= size)
    {
        active = 0;
    }

    return true;
}

/*!
 * \brief Initialise the ring store with pointers to new and old persistent states and set the current progress measure
 * \param oldp Pointer to the minimum progress measure store of old persistent states
 * \param newp Pointer to the minimum progress measure store of new persistent states
 */
template <class T>
void SweepRingStore<T>::init(SweepListStore<T> *oldp, SweepListStore<T> *newp)
{
    // Link in the old and new persistent stores with the ring store
    for (index_t i = 0; i < size; ++i)
    {
        if (i)
        {
            oldp = oldp->getNext();
            newp = newp->getNext();
        }
        oldpersistent[i] = oldp;
        newpersistent[i] = newp;
    }
    // set the current progress measure, i.e. select the correct store on the ring
    active = size - front_offset - 1;
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
    for (index_t i = 0; i < size; ++i)
        if (!store[i]->empty())
        {
            delete store[i];
            store[i] = createSweepStore();
        }

    // clear the swap space (usually necessary)
    if (!samevalue->empty())
    {
        delete samevalue;
        samevalue = createSweepStore();
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
    return !(newpersistent[(active + front_offset + 1) % size]->store->empty());
}

/*!
 * \brief check if there are new persistent states in the bp oldest stores with respect to the ring store
 * \return The relative "age" of the store with a new persistent state (0 if none)
 */
template <class T>
int32_t SweepRingStore<T>::checkNewPersistent(int32_t bp)
{
    // the oldest store has already been checked, thus "i+2" in the following formula
    // test the second oldest one first, and then stores with increasing progress
    assert(bp >= 0);
    for (index_t i = 0; i < bp; ++i)
        if (!(newpersistent[(active + front_offset + i + 2) % size]->store->empty()))
        {
            return bp - i;
        }
    return 0;
}

/*!
 * \brief check if the last inserted state has been new persistent. Only valid directly after a successfull insert by searchAndInsert()
 * \return True if it is new persistent state
 */
template <class T>
bool SweepRingStore<T>::insertedIsNewPersistent(index_t thread)
{
    return inserted_persistent[thread];
}

/*!
 * \brief check if the last inserted state has been new persistent. Only valid directly after a successfull insert by searchAndInsert()
 * \return True if it is new persistent state
 */
template <class T>
int64_t SweepRingStore<T>::getNumberOfDeletedStates()
{
    return deleted_store_size;
}

template <class T>
VectorStore<T> *SweepRingStore<T>::createSweepStore()
{
    switch (args_info.store_arg)
    {
    case store_arg_prefix:
        if (nr_of_buckets > 1)
        {
            return new HashingWrapperStore<T>(new UnaryVectorStoreCreator<T, PrefixTreeStore<T>, index_t>
                                              (nr_of_threads), nr_of_buckets, nr_of_threads);
        }
        else
        {
            return new PrefixTreeStore<T>(nr_of_threads);
        }

    case store_arg_stl:
        return new VSTLStore<T>(nr_of_threads);

    default:
        rep->message(rep->markup(MARKUP_BAD, "specified store does not fit the given task").str());
        rep->abort(ERROR_COMMANDLINE);
        return NULL;
    }
}

template <class T>
void SweepRingStore<T>::printState(NetState &ns)
{
    for (index_t p = 0; p < Net::Card[PL]; ++p)
    {
        if (ns.Current[p] > 0)
        {
            cout << Net::Name[PL][p] << ":" << ns.Current[p] << " ";
        }
    }
    cout << endl;
}

