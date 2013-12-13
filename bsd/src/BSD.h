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

#include <pnapi/pnapi.h>
#include <utility>
#include <vector>
#include <map>
#include <set>
#include "types.h"


/*!
 \brief BSD automata generation

 Contains all functions needed to generate BSD automata from given OWFNs.
*/
class BSD {

    public:
		static std::list<BSDNode *>* graph;

		static BSDNode* U;
		static BSDNode* emptyset;


		static void initialize();

        static void finalize();

		/*========================================================
		 *-------------------- BSD computation -------------------
		 *========================================================*/

        static void computeBSD(int og_flag);

        static void computeSuccessor(BSDNode &node, Label_ID label, int og_flag);

        static BSDNode* setup(std::list<MarkingList> &SCCs, int og_flag);

        static std::list<MarkingList>* computeClosureTarjan(InnerMarking_ID id);

        static std::list<MarkingList>* tarjanClosure(InnerMarking_ID markingID);

        static void mergeSCCsWithoutDuplicates(std::list<MarkingList> &result, std::list<MarkingList> &temp);

        static bool checkEquality(MarkingList &list1, MarkingList &list2);

        static void assignLambda(BSDNode *node, std::list<MarkingList> &SCCs);

    private:

        // helpers for tarjan algorithm (lambda value computation)
        static MarkingList* templist;
        static std::map<InnerMarking_ID, int>* dfs;
        static std::map<InnerMarking_ID, int>* lowlink;
        static int maxdfs;
        static std::stack<InnerMarking_ID>* S;
        static std::map<InnerMarking_ID, bool>* inStack;

};

#define TAU 0
