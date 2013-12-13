/*****************************************************************************\
 BSD -- generating BSD automata

 Copyright (c) 2013 Simon Heiden

 BSD is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 BSD is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with BSD.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

#pragma once

#include <list>
#include <config.h>
#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif

/// the range of hash values (max. 65535)
typedef uint16_t hash_t;

/// the range of labels (max. 255)
typedef uint8_t Label_ID;

/// the range of markings (all markings, deadlock markings) stored within a knowledge (max. 65535)
typedef uint32_t innermarkingcount_t;

/// the range of inner markings (max. 4294967296)
typedef uint32_t InnerMarking_ID;

// represents a node of the BSD automaton
typedef std::list<InnerMarking_ID> MarkingList;

// a node of a BSD (or CSD) automaton
typedef struct _BSDNode {
	// is this node actually the U node? (for computation of CSD and MP automata)
	bool isU;
	// list of markings in the closure
	MarkingList list;
	// list of SCCs in the closure (may be empty?!)
	std::list<MarkingList> SCCs;
	// pointers to successing BSD nodes (ordered and accessible by label id)
	struct _BSDNode** successors;
	// pointers to predecessing BSD nodes
	std::list<std::pair<Label_ID, struct _BSDNode *> >* predecessors;
	// number of successors that are the U node
	uint8_t Ucount;
	// lambda value of the node (see paper for more information)
	uint8_t lambda;
	// formula (if existing): conjunction of disjunctions
	std::list<std::list<Label_ID> > formula;
} BSDNode;

// representation of a BSD (or/and a CSD) automaton with needed values for computations
typedef struct _BSDgraph {
	// the BSD (or/and CSD) automaton (list of nodes)
	std::list<BSDNode *>* graph;
	// first id of receiving labels
	Label_ID first_receive;
	// last id of receiving labels
	Label_ID last_receive;
	// first id of sending labels
	Label_ID first_send;
	// last id of sending labels
	Label_ID last_send;
	// number of sending events
	Label_ID send_events;
	// number of receiving events
	Label_ID receive_events;
	// number of total events
	Label_ID events;
	// pointer to U node
	BSDNode* U;
	// pointer to empty node
	BSDNode* emptyset;
	// mapping of label ids to label names (strings)
	std::map<Label_ID, std::string> id2name;

	// computation time
	double comp_time;

	// memory consumptions
	unsigned int memory_consumption;
} BSDgraph;

// a parsed node of a BSD or CSD automaton
typedef struct _parsedNode {
	// the name of the parsed node
	std::string name;
	// pointers to other nodes
	std::map<Label_ID, struct _parsedNode* > * pointer;
	// lambda value of the node (see paper for more information)
	uint8_t lambda;
} parsedNode;

// representation of a parsed BSD or CSD automaton with needed values for computations
typedef struct _parsedGraph {
	// array of node names (names[node] = node.name)
	std::string * names;
	// array of array of pointers to successor nodes (pointer[node][labelID] = node.succ)
	unsigned int ** pointer;
	// array of lamba values (lambdas[node] = node.lambda)
	int * lambdas;
	// assignment of labels to sending if true (receiving if false)
	std::map<Label_ID, bool>* is_sending_label;
	// total count of nodes in the graph
	unsigned int nodes;
	// number of total events
	Label_ID events;
	// the bound of the graph
	int bound;
	// id of the U node
	unsigned int U;
	// id of the empty node
	unsigned int emptyset;
	// mapping of label ids to label names (strings)
	std::map<Label_ID, std::string>* id2name;
	// the reverse mapping
	std::map<std::string, Label_ID>* name2id;
} parsedGraph;
