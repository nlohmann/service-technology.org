#pragma once
#include <Stores/Store.h>
/// a binary decision node
class DecisionNode4BinStore
{
public:
	DecisionNode4BinStore(bitindex_t);
	bitindex_t bit;
	unsigned char* vector;
	DecisionNode4BinStore* nextold;
	DecisionNode4BinStore* nextnew;
	~DecisionNode4BinStore();
	virtual void getPayload(void**) {};
};
