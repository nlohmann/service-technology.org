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

// a node of the BSD automaton
typedef struct _BSDNode {
	// is this node actually the U node? (for computation of uBSD automaton)
	bool isU;
	// list of markings in the closure
	MarkingList list;
	// pointers to other BSD nodes (ordered and accessible by label id)
	struct _BSDNode** pointer;
	// lambda value of the node (see paper for more information)
	uint8_t lambda;
} BSDNode;


// stores all nodes of the BSD automaton
typedef std::list<BSDNode *> BSDNodeList;

// representation of a BSD automaton with needed values for computations
typedef struct _BSDgraph {
	// the BSD automaton
	BSDNodeList* graph;
	// the CSD automaton
	BSDNodeList* csdgraph;
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

	double BSD_comp_time;
	double CSD_comp_time;
} BSDgraph;

// a parsed node of the BSD automaton
typedef struct _parsedBSDNode {
	// list of markings in the closure
	std::string name;
	// pointers to other BSD nodes (ordered and accessible by label id)
	std::map<Label_ID, struct _parsedBSDNode* > * pointer;
	// lambda value of the node (see paper for more information)
	uint8_t lambda;
} parsedBSDNode;

// representation of a parsed BSD automaton with needed values for computations
typedef struct _parsedBSDgraph {
	// the BSD automaton
	std::list<parsedBSDNode *>* graph;
	std::map<Label_ID, bool>* is_sending_label;
	// number of total events
	Label_ID events;
	// pointer to U node
	parsedBSDNode* U;
	// pointer to empty node
	parsedBSDNode* emptyset;
	// mapping of label ids to label names (strings)
	std::map<Label_ID, std::string>* id2name;
	// the reverse mapping
	std::map<std::string, Label_ID>* name2id;
} parsedBSDgraph;
