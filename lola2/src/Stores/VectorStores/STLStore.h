/*!
\author Niels, Christian Koch
\file STLStore.cc
\status new
*/

#pragma once
#include <set>
#include <vector>
#include <Core/Dimensions.h>
#include <Stores/PluginStore.h>


/// prepended "V" to avoid name clash
class VSTLStore : public PluginStore::VectorStore
{
    private:
        std::set<std::vector<vectordata_t> > store;
        std::vector<vectordata_t> intermediate;

    public:
        virtual bool searchAndInsert(const vectordata_t* in, bitindex_t bitlen, hash_t hash);
};
