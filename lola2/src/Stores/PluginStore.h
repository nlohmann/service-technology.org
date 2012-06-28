/*!
\file PluginStore.h
\author Christian Koch
\status new
\brief general-purpose base store consisting of two components: a NetStateEncoder, that converts the current state into a input vector, and a VectorStore, that stores the input vectors and checks whether they are already known. Implementations for both components can be combined arbitrarily. Specific components may have some functional or compatibility limitations, though.
*/

#pragma once
#include <Core/Dimensions.h>
#include <Stores/Store.h>

class PluginStore : public Store
{
    public:
        class NetStateEncoder
        {
            protected:
                int numThreads;
            public:
                NetStateEncoder(int _numThreads);
                virtual ~NetStateEncoder() {}
                virtual vectordata_t* encodeNetState(NetState &ns, bitindex_t &bitlen, index_t threadIndex) = 0;
        };

        class VectorStore
        {
            public:
                virtual ~VectorStore() {}
                virtual bool searchAndInsert(const vectordata_t* in, bitindex_t bitlen, hash_t hash, index_t threadIndex) = 0;
        };

        /// creates new Store using the specified components. The given components are assumed to be used exclusively and are freed once the PluggableStore gets destructed.
        PluginStore(NetStateEncoder* _netStateEncoder, VectorStore* _vectorStore, uint32_t _number_of_threads);

        /// frees both components
        ~PluginStore();

        /// check whether current marking is stored
        bool searchAndInsert(NetState &ns, uint32_t threadIndex);

        /// check whether current marking is stored
        bool searchAndInsert(NetState &ns, void** s = NULL);

    private:
        // a mutex for incrementing
        pthread_mutex_t inc_mutex;
        NetStateEncoder* netStateEncoder;
        VectorStore* vectorStore;
};

