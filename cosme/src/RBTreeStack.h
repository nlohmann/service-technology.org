#ifndef __RBTREESTACK_H
#define __RBTREESTACK_H

#include <config.h>
#include <limits>
#include <iostream>
#include <stdint.h>
#include <cstdlib>
#include <cstdio>
#include "types.h"

typedef struct rbts_node_key_t {
	og_service_index_t mIndexOne;
	og_service_index_t mIndexTwo;
} rbts_node_key_t;

typedef struct rbts_node_t {	
	rbts_node_key_t mKey;
	unsigned mRed;
	rbts_node_t* mTreeLeft;
	rbts_node_t* mTreeRight;
	rbts_node_t* mTreeParent;
	rbts_node_t* mStackParent;
} rbts_node_t;

class RBTreeStack {
	public:
		RBTreeStack();
		~RBTreeStack();

		bool add(og_service_index_t indexOne, og_service_index_t indexTwo);
		bool pop(og_service_index_t& indexOne, og_service_index_t& indexTwo);
		inline og_service_index_t size() const { return this->mSize; } 

	private:
		inline void rightRotate(rbts_node_t* node);
		inline void leftRotate(rbts_node_t* node);
		inline bool addHelper(rbts_node_t* node);

		void deleteHelper(rbts_node_t* node);

	private:
		rbts_node_t* mRoot;
		rbts_node_t* mNil;
		rbts_node_t* mTop;
		og_service_index_t mSize;

};

#endif //__RBTREESTACK_H
