#ifndef __TARJAN_H
#define __TARJAN_H

#include <map>
#include <set>
#include <stack>
#include <string>

#include <pnapi/pnapi.h>
#include "types.h"
#include "verbose.h"

using std::map;
using std::multimap;
using std::set;
using std::stack;
using std::string;

typedef set<node_t> nodes_t;
typedef map<node_t, int> node_value_t;

class Tarjan {

	private:
		bool mSCCCalculated;
		bool mHasNonTrivialSCC;
		int mMaxDFS;
		nodes_t mUnprocessedNodes;
		nodes_t mStackContent;
		stack<node_t> mStack;
		node_value_t mNodeDFS;
		node_value_t mNodeLowlink;
		node_value_t mNode2SCC;
		multimap<int, node_t> mSCC2Node;	
		pnapi::PetriNet *mNet;
		
		void init();
		void tarjan(node_t);

		void push(node_t &);
		node_t pop();

		void setDFS(node_t &, int);
		void setLowlink(node_t &, int);
		int getDFS(node_t &);
		int getLowlink(node_t &);

	public:
		Tarjan(pnapi::PetriNet &);
		~Tarjan();

		void calculateSCC();

		node_value_t getNode2SCC() const;
		multimap<int, node_t> getSCC2Node() const;
		
		int getNodeSCC(const node_t &);

		bool hasNonTrivialSCC();
		bool isNonTrivialSCC(int);

		
};

#endif //__TARJAN_H
