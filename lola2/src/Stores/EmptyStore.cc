/* Since the separation of interface and definition is quite hard when one uses templates, the "cc"-part is included in the .h-file.
#include <cmath>
#include <Stores/EmptyStore.h>


template <typename T>
EmptyStore<T>::EmptyStore() : Store<T>(1), tries(0)
{
}

template <typename T>
inline bool EmptyStore<T>::searchAndInsert(NetState &ns, void**)
{
    ++calls[0];
    return true;
}

template <typename T>
inline bool EmptyStore<T>::searchAndInsert(NetState &ns, T payload, T** payload_pointer,  void**){
	++calls[0];
	*payload_pointer = NULL;
}
*/
