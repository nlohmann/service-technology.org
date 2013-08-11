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

typedef struct _BSDNode {
	MarkingList list;
	struct _BSDNode** pointer;
	uint8_t lambda;
} BSDNode;


// stores all nodes of the BSD automaton
typedef std::list<BSDNode *> BSDNodeList;

typedef struct _BSDgraph {
	BSDNodeList* graph;
	Label_ID first_receive;
	Label_ID last_receive;
	Label_ID first_send;
	Label_ID last_send;
	Label_ID send_events;
	Label_ID receive_events;
	Label_ID events;
	BSDNode* U;
	BSDNode* emptyset;
	std::map<Label_ID, std::string> id2name;
} BSDgraph;
