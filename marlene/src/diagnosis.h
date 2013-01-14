/*****************************************************************************\
 Marlene -- synthesizing behavioral adapters

 Copyright (C) 2009, 2010  Christian Gierds <gierds@informatik.hu-berlin.de>

 Marlene is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Marlene is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Marlene.  If not, see <http://www.gnu.org/licenses/>.
 \*****************************************************************************/

#pragma once

#include <string>
#include <vector>
#include <set>
#include "config.h"
#include "markings.h"
#include "adapter.h"

// forward declarations
class DGraph;
class DNode;
class Output;

class DNode {

    public:
        struct Initializer {
                unsigned int id;
                bool has_deadlock;
                bool has_livelock;
                std::vector<std::pair<unsigned int, unsigned int> >
                        successors;

                Initializer();
        };

    private:
        unsigned int id;

//        bool has_deadlock;
//        bool has_livelock;
//        bool has_waitstate;
//        bool has_capacity_violation;

        std::vector<std::vector<int> > mappingToMPP;
        std::vector<int> markings;

    public:
#ifdef USE_SHARED_PTR
        typedef std::tr1::shared_ptr<DNode> DNode_ptr;
#else
        typedef DNode * DNode_ptr;
#endif

        DNode(Initializer & init);

        unsigned int getID() const {
            return id;
        }

        std::vector<std::pair<unsigned int, unsigned int> > successors;
        std::vector<unsigned int> deadlockMarkings;
        std::vector<unsigned int> livelockMarkings;
        std::vector<unsigned int> waitstateMarkings;
        std::set<std::string> missedAlternatives;
        std::set<std::string> rulesApplied;

};

class DGraph {

    private:
        unsigned int nodeId;
        std::map<int, unsigned int> name2id;
        std::map<unsigned int, int> id2name;
        unsigned int labelId;
        std::map<std::string, unsigned int> label2id;
        std::map<unsigned int, std::string> id2label;

    public:
#ifdef USE_SHARED_PTR
        typedef std::tr1::shared_ptr<DGraph> DGraph_ptr;
#else
        typedef DGraph * DGraph_ptr;
#endif

        DGraph();

        ~DGraph();

        std::vector<DNode::DNode_ptr> nodes;
        std::map<unsigned int, DNode::DNode_ptr> nodeMap;
        unsigned int initialNode;
        std::vector<DNode::DNode_ptr> deadlockNodes;
        std::vector<DNode::DNode_ptr> livelockNodes;
        std::set<DNode::DNode_ptr> alternativeNodes;
        std::map<unsigned int, unsigned int> noOfPredecessors;

        unsigned int getIDForName(int nameId);
        int getNameForID(unsigned int id);
        unsigned int getIDForLabel(std::string & labelName);
        std::string & getLabelForID(unsigned int id);

        void collectRules();
};

class Diagnosis {

    private:
        struct DiagnosisInformation {
#ifdef USE_SHARED_PTR
                typedef std::tr1::shared_ptr<DiagnosisInformation>
                        DiagnosisInformation_ptr;
#else
                typedef DiagnosisInformation * DiagnosisInformation_ptr;
#endif
                std::string type;
                std::vector<std::string> pendingMessages;
                std::vector<std::string> requiredMessages;
                std::set<std::string> previouslyAppliedRules;
                std::set<unsigned int> netsInFinalState;

                std::string getLive() const;

                bool operator==(const DiagnosisInformation & d1) const;
                bool operator<=(const DiagnosisInformation & d2) const;
                bool operator()(const DiagnosisInformation & d1,
                        const DiagnosisInformation & d2) const;
        };

        DGraph::DGraph_ptr dgraph;
        MarkingInformation & mi;
        Output live;
        std::set<DiagnosisInformation, DiagnosisInformation>
                diagnosisInformation;

        unsigned int messageBound;

    public:
        bool superfluous;

        Diagnosis(std::string filename, MarkingInformation & pmi,
                unsigned int messageBound = 1);

        ~Diagnosis();

        void readMPPs(std::vector<std::string> & resultfiles);
        void evaluateDeadlocks(std::vector<PetriNet_ptr> & nets,
                pnapi::PetriNet & engine);
        void evaluateLivelocks(std::vector<PetriNet_ptr> & nets,
                pnapi::PetriNet & engine);
        void evaluateAlternatives(std::vector<PetriNet_ptr> & nets,
                pnapi::PetriNet & engine);
        void outputLive() const;

        bool isSuitableForRepair(const RuleSet::AdapterRule::AdapterRule_ptr, RuleSet &) const;

};

