/*!
\file CompareStore.inc
\author Gregor Behnke
\status new

\brief a meta-store to test, whether a new store works correctly
*/

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <Net/Marking.h>
#include <Net/Net.h>
#include <Net/Place.h>


template<typename T>
CompareStore<T>::CompareStore(Store<T>* correct, Store<T>* test,uint32_t _number_of_threads) : Store<T>(_number_of_threads)
{
    correctStore = correct;
    testStore = test;
}

template<typename T>
CompareStore<T>::~CompareStore()
{
    delete correctStore;
    delete testStore;
}

template<typename T>
bool CompareStore<T>::searchAndInsert(NetState &ns, T** payload, uint32_t threadIndex)
{
   bool correctResult = correctStore->searchAndInsert(ns, payload, threadIndex);
   
   bool testResult = testStore->searchAndInsert(ns,payload,threadIndex);
   
   if (correctResult == testResult)
   	return correctResult;
   
   assert(false);
}

/*!
 * \brief gets and removes a state from the store
 * \param ns NetState where the removed state will be written to
 * \return false, if store was already empty
 */  
template<typename T>
bool CompareStore<T>::popState(NetState &ns) {
  testStore->popState(ns);
  return correctStore->popState(ns);
}
