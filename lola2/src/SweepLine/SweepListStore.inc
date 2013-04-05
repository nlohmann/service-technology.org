/*
\file SweepListStore.inc
\author Harro
\status new

\brief List of Stores for persistent markings in the SweepLine method

This class realizes one store in a list of stores, sorted by progress
measure and containing known persistent markings, either old or new ones. 
*/

#include <cstdlib>
#include <cmdline.h>
#include <SweepLine/SweepListStore.h>
#include <Planning/Task.h>

extern gengetopt_args_info args_info;
extern Reporter* rep;

/*! 
 * \brief Constructor for one store in a list of stores
 */
template <class T>
SweepListStore<T>::SweepListStore(index_t _nr_of_threads) 
: next(NULL), nr_of_threads(_nr_of_threads)
{
	store = createSweepStore();
}

/*! 
 * \brief Constructor for a list of stores, returns pointer to the first store
 * \param size The number of stores in the list
 */
template <class T>
SweepListStore<T>::SweepListStore(index_t _nr_of_threads, index_t size) 
: next(size>0 ? new SweepListStore(_nr_of_threads,size-1) : NULL), nr_of_threads(_nr_of_threads)
{
	store = createSweepStore();
}

/*!
 * \brief Destructor for the list starting at this store
 */
template <class T>
SweepListStore<T>::~SweepListStore()
{
	delete store;
	delete next;
}

/*!
 * \brief Check if there exists a store after this one in the list
 * \return The next store if it exists, NULL otherwise
 */
template <class T>
SweepListStore<T>* SweepListStore<T>::checkNext()
{
	return next;
}

/*!
 * \brief Check if there exists a store after this one in the list
 *        and create one if it does not exist
 * \return The next store in the list, possibly just created
 */
template <class T>
SweepListStore<T>* SweepListStore<T>::getNext()
{
	if (!next) next = new SweepListStore<T>(nr_of_threads);
	return next;
}

/*!
 * \brief Manually set the next store in the list
 * \param sls The store that should be the next after this one
 */
template <class T>
void SweepListStore<T>::setNext(SweepListStore<T>* sls)
{
	next = sls;
}

template <class T>
VectorStore<T>* SweepListStore<T>::createSweepStore()
{
    switch (args_info.store_arg)
    {
        case store_arg_prefix:
            if(args_info.bucketing_given)
				return new HashingWrapperStore<T>(new UnaryVectorStoreCreator<T,PrefixTreeStore<T>,index_t>(nr_of_threads),16,nr_of_threads);
            else
				return new PrefixTreeStore<T>(nr_of_threads);
        case store_arg_stl:
/*
            if(args_info.bucketing_given)
				return new PluginStore<T>(enc, new HashingWrapperStore<T>(new UnaryVectorStoreCreator<T,VSTLStore<T>,index_t>(number_of_threads)), number_of_threads);
            else
*/
				return new VSTLStore<T>(nr_of_threads);
        default:
			rep->message(rep->markup(MARKUP_BAD,"specified store does not fit the given task").str());
			rep->abort(ERROR_COMMANDLINE);
			return NULL;
    }
}

