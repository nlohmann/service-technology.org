#include "RBTreeStack.h"  

RBTreeStack::RBTreeStack() {
	rbts_node_t* tmpNode;
	
	tmpNode = this->mNil = (rbts_node_t*) malloc(sizeof(rbts_node_t));
	assert(this->mNil != NULL);
	tmpNode->mTreeParent = tmpNode->mTreeLeft = tmpNode->mTreeRight = tmpNode;
	tmpNode->mRed = 0;
	tmpNode->mKey.mIndexOne = tmpNode->mKey.mIndexTwo = RBTS_NODE_KEY_UNUSED;

	tmpNode = this->mRoot = (rbts_node_t*) malloc(sizeof(rbts_node_t));
	assert(this->mRoot != NULL);	
	tmpNode->mTreeParent = tmpNode->mTreeLeft = tmpNode->mTreeRight = this->mNil;
	tmpNode->mRed = 0;
	tmpNode->mKey.mIndexOne = tmpNode->mKey.mIndexTwo = RBTS_NODE_KEY_UNUSED;

	this->mRoot->mStackParent = this->mNil->mStackParent = this->mTop = NULL;
	
	this->mSize = 0;

}

RBTreeStack::~RBTreeStack() {
	this->deleteHelper(this->mRoot->mTreeLeft);
	free(this->mRoot);
	free(this->mNil);
}

bool RBTreeStack::add(og_service_index_t indexOne, og_service_index_t indexTwo) {
	assert(indexOne != RBTS_NODE_KEY_UNUSED);
	assert(indexTwo != RBTS_NODE_KEY_UNUSED);

	rbts_node_t* x;
	rbts_node_t* y;
	rbts_node_t* newNode;
	x = (rbts_node_t*) malloc(sizeof(rbts_node_t));
	x->mKey.mIndexOne = indexOne;
	x->mKey.mIndexTwo = indexTwo;
	x->mStackParent = this->mTop;

	if (!this->addHelper(x)) {
		free(x);
		return false;		
	}

	this->mSize++;

	newNode = this->mTop = x;
	x->mRed = 1;	
	while (x->mTreeParent->mRed) {
		if (x->mTreeParent == x->mTreeParent->mTreeParent->mTreeLeft) {
			y = x->mTreeParent->mTreeParent->mTreeRight;
			if (y->mRed) {
				x->mTreeParent->mRed = y->mRed = 0;
				x->mTreeParent->mTreeParent->mRed = 1;
				x = x->mTreeParent->mTreeParent;
			} // y->mRed
			else {
				if (x == x->mTreeParent->mTreeRight) {
					x = x->mTreeParent;
					this->leftRotate(x);
				} // x == x->mTreeParent->mTreeRight
				x->mTreeParent->mRed = 0;
				x->mTreeParent->mTreeParent->mRed = 1;
				this->rightRotate(x->mTreeParent->mTreeParent);			
			} // !y->mRed
		} // x->mTreeParent == x->mTreeParent->mTreeParent->mTreeLeft
		else {
			y = x->mTreeParent->mTreeParent->mTreeLeft;
			if (y->mRed) {
				x->mTreeParent->mRed = y->mRed = 0;
				x->mTreeParent->mTreeParent->mRed = 1;
				x = x->mTreeParent->mTreeParent;
			} // y->mRed
			else {
				if (x == x->mTreeParent->mTreeLeft) {
					x = x->mTreeParent;
					this->rightRotate(x);
				} // x == x->mTreeParent->mTreeLeft
				x->mTreeParent->mRed = 0;
				x->mTreeParent->mTreeParent->mRed = 1;
				this->leftRotate(x->mTreeParent->mTreeParent);
			} // !y->mRed
		} // x->mTreeParent != x->mTreeParent->mTreeParent->mTreeLeft
	} // x->mTreeParent->mRed

	this->mRoot->mTreeLeft->mRed = 0;
	return true;

}

bool RBTreeStack::pop(og_service_index_t& indexOne, og_service_index_t& indexTwo) {
	if (this->mTop == NULL) {
		return false;
	}
	indexOne = this->mTop->mKey.mIndexOne;
	indexTwo = this->mTop->mKey.mIndexTwo;
	this->mTop = this->mTop->mStackParent;
	return true;
}

void RBTreeStack::deleteHelper(rbts_node_t* node) {
	assert(node != NULL);
	if (node != this->mNil) {
		this->deleteHelper(node->mTreeLeft);
		this->deleteHelper(node->mTreeRight);
		free(node);
	}
}

inline void RBTreeStack::rightRotate(rbts_node_t* node) {
	assert(this->mNil != NULL);
	assert(node != NULL);

	rbts_node_t* x;

	x = node->mTreeLeft;
	node->mTreeLeft = x->mTreeRight;

	if (this->mNil != x->mTreeRight) {
		x->mTreeRight->mTreeParent = node;
	}

	x->mTreeParent = node->mTreeParent;
	if (node == node->mTreeParent->mTreeLeft) {
		node->mTreeParent->mTreeLeft = x;
	}
	else {
		node->mTreeParent->mTreeRight = x;
	}
	x->mTreeRight = node;
	node->mTreeParent = x;

}

inline void RBTreeStack::leftRotate(rbts_node_t* node) {
	assert(this->mNil != NULL);
	assert(node != NULL);

	rbts_node_t* x;

	x = node->mTreeRight;
	node->mTreeRight = x->mTreeLeft;

	if (this->mNil != x->mTreeLeft) {
		x->mTreeLeft->mTreeParent = node;
	}

	x->mTreeParent = node->mTreeParent;
	if (node == node->mTreeParent->mTreeLeft) {
		node->mTreeParent->mTreeLeft = x;
	}
	else {
		node->mTreeParent->mTreeRight = x;
	}
	x->mTreeLeft = node;
	node->mTreeParent = x;

}

inline bool RBTreeStack::addHelper(rbts_node_t* node) {
	assert(node != NULL);

	rbts_node_t* x;
	rbts_node_t* y;

	node->mTreeLeft = node->mTreeRight = this->mNil;

	y = this->mRoot;
	x = this->mRoot->mTreeLeft;
	while (x != this->mNil) {
		y = x;
		if ((x->mKey.mIndexOne == node->mKey.mIndexOne) && (x->mKey.mIndexTwo == node->mKey.mIndexTwo)) {
			return false;
		} 
		if (x->mKey.mIndexOne < node->mKey.mIndexOne) {
			x = x->mTreeRight;
		}
		else if (x->mKey.mIndexOne > node->mKey.mIndexOne) {
			x = x->mTreeLeft;
		}
		else {
			// IndexOne is equal
			if (x->mKey.mIndexTwo < node->mKey.mIndexTwo) {
				x = x->mTreeRight;
			}
			else if (x->mKey.mIndexTwo > node->mKey.mIndexTwo) {
				x = x->mTreeLeft;
			}
		}
	}

	node->mTreeParent = y;
	if ( (y == this->mRoot) || (y->mKey.mIndexOne > node->mKey.mIndexOne)) {
		y->mTreeLeft = node;
	}
	else {
		y->mTreeRight = node;
	}

	return true;

}
