/*****************************************************************************\
 Wendy -- Synthesizing Partners for Services

 Copyright (c) 2009 Niels Lohmann, Christian Sura, and Daniela Weinberg

 Wendy is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Wendy is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Wendy.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

#include <config.h>
#include <set>
#include <string>
#include <vector>
#include <algorithm>

#include "StoredKnowledge.h"
#include "AnnotationLivelockOG.h"
#include "LivelockOperatingGuideline.h"
#include "CompositeMarking.h"
#include "Clause.h"
#include "verbose.h"
#include "cmdline.h"
#include "util.h"

LivelockOperatingGuideline::_stats LivelockOperatingGuideline::stats;
AnnotationLivelockOG LivelockOperatingGuideline::annotationLivelockOG = AnnotationLivelockOG();
unsigned int LivelockOperatingGuideline::maxDFS = 0;

extern gengetopt_args_info args_info;


/********************
 * STATIC FUNCTIONS *
 ********************/

/*!
  initialize statistics
 */
LivelockOperatingGuideline::_stats::_stats()
    : numberOfSCSs(0), numberOfTSCCInSCSs(0), numberAllElementsAnnotations(0) {}


void LivelockOperatingGuideline::initialize() {
    Clause::initialize();
}



/*!
  Tarjan's algorithm to find SCCs within all nodes (consisting of SCCs themselves) contained in the TS
  \param toBeVisited the set of knowledges to be traversed
  \param setOfSCCs a set of strongly connected components
 */
bool LivelockOperatingGuideline::getSCCs(const std::set<StoredKnowledge* > & toBeVisited,
                                         SetOfSCCs& setOfSCCs) {

    // reset tarjan stack (used in StoredKnowledge before)
    StoredKnowledge::tarjanStack.clear();
    StoredKnowledge::tarjanMapping.clear();

    std::set<StoredKnowledge* > visited;

    // go through the set of nodes that are to be visited
    FOREACH(iter, toBeVisited) {
        // we have not yet seen the node, find the SCCs of its reachability graph
        getSCCsRecursively(*iter, visited, toBeVisited, setOfSCCs);
    }

    return true;
}


/*!
  check if given knowledge is on the Tarjan stack (is a std::vector, so a find() does not help)
  \param currentNode the knowledge that is to be checked
  \return true, if knowledge is on the Tarjan stack; false, otherwise
  \todo make an abstract data type (push, pop, find)
 */
bool LivelockOperatingGuideline::findNodeInStack(const StoredKnowledge* currentNode) {

    FOREACH(it, StoredKnowledge::tarjanStack) {
        if (currentNode == *it) {
            return true;
        }
    }
    return false;
}


/*!
   recursive function to find all strongly connected components within a given set of knowledges
   \param currentNode currently considered knowledge
   \param visited set of knowledges that have been visited already
   \param toBeVisited set of knowledges that still need to be considered
   \param setOfSCCs set of SCCs that have been found already

   \todo check if we can get rid of the mapping (idea: inherit class LLStoredKnowledge from StoredKnowledge)
   \todo implement a nice set<void*> with the _needed_ operators
   \todo alternatively: store an "i'm in the set" bit at each object
 */
bool LivelockOperatingGuideline::getSCCsRecursively(StoredKnowledge* currentNode,
                                                    std::set<StoredKnowledge* > & visited,
                                                    const std::set<StoredKnowledge* > & toBeVisited,
                                                    SetOfSCCs& setOfSCCs) {

    // remember, that we visited the current node
    if (not visited.insert(currentNode).second) {
        return true;
    }

    // adjust Tarjan values of the current node (first == dfs; second == lowlink)
    StoredKnowledge::tarjanMapping[currentNode].first = StoredKnowledge::tarjanMapping[currentNode].second = maxDFS;

    // increment depth number
    ++maxDFS;

    // put current node on Tarjan stack
    StoredKnowledge::tarjanStack.push_back(currentNode);

    // iterate over the successors of current node
    for (Label_ID l = Label::first_receive; l <= Label::last_sync; ++l) {
        if (currentNode->successors[l - 1] != NULL and currentNode->successors[l - 1] != StoredKnowledge::empty and
                currentNode->successors[l - 1]->is_sane) {

            // we have not yet touched the successor node and successor node is to be visited
            // and if we should consider the edge
            if (visited.find(currentNode->successors[l - 1]) == visited.end() and
                    toBeVisited.find(currentNode->successors[l - 1]) != toBeVisited.end()) {

                // do Tarjan algorithm for the successor state
                getSCCsRecursively(currentNode->successors[l - 1], visited, toBeVisited, setOfSCCs);

                // adjust lowlink value
                StoredKnowledge::tarjanMapping[currentNode].second = MINIMUM(StoredKnowledge::tarjanMapping[currentNode].second,
                                                                             StoredKnowledge::tarjanMapping[currentNode->successors[l - 1]].second);
            } else {

                // current node has been visited before, now check if it is still on the Tarjan stack
                if (findNodeInStack(currentNode->successors[l - 1])) {

                    // adjust lowlink value
                    StoredKnowledge::tarjanMapping[currentNode].second = MINIMUM(StoredKnowledge::tarjanMapping[currentNode].second,
                                                                                 StoredKnowledge::tarjanMapping[currentNode->successors[l - 1]].first);
                }
            }
        }
    }

    // we have traversed through the reachability graph of the current node
    // check, if the current node is a representative of a SCC
    // if so, get all nodes within the SCC
    if (StoredKnowledge::tarjanMapping[currentNode].first == StoredKnowledge::tarjanMapping[currentNode].second) {

        // node which has just been popped from Tarjan stack
        StoredKnowledge* poppedNode;
        // current SCC
        std::set<StoredKnowledge* > scc;
        do {
            if (StoredKnowledge::tarjanStack.empty()) {
                continue;
            }
            // get last element
            poppedNode = StoredKnowledge::tarjanStack.back();
            // delete last element from stack
            StoredKnowledge::tarjanStack.pop_back();
            // put popped node into current SCC
            scc.insert(poppedNode);

        } while (currentNode != poppedNode);

        // put newly found SCC into the set storing all SCCs
        setOfSCCs.push_back(scc);
    }

    return true;
}


/*!
  given a set of stored knowledges generate the whole reachability tree of the markings stored within
  the knowledges and use Tarjan's algorithm to detect TSCCs
  \param knowledgeSCS set of knowledges that is to be checked

  \todo Statistik: wie viele unterschiedliche CompositeMarkingsHandler::conjunctionOfDisjunctions gibt es
*/
void LivelockOperatingGuideline::calculateTSCCInKnowledgeSet(const std::set<StoredKnowledge* > & knowledgeSCS) {

    CompositeMarkingsHandler::initialize(knowledgeSCS);

    // traverse the set of knowledges
    FOREACH(iterSCS, knowledgeSCS) {
        // traverse the markings of the current knowledge
        for (innermarkingcount_t i = 0; i < (*iterSCS)->sizeAllMarkings; ++i) {

            // create new interface
            InterfaceMarking* newInterface = new InterfaceMarking(*(*iterSCS)->interface[i]);

            // check if we have seen the new marking already
            bool foundMarking;
            CompositeMarking* currentMarking = CompositeMarkingsHandler::isVisited(*iterSCS, (*iterSCS)->inner[i], newInterface, foundMarking);

            // if not, go on
            if (not foundMarking) {
                calculateTSCCInKnowledgeSetRecursively(currentMarking, knowledgeSCS);
            }
        }
    }

    // all TSCCs have been calculated within the current set of knowledges

    // remember annotation
    annotationLivelockOG.push(knowledgeSCS, CompositeMarkingsHandler::conjunctionOfDisjunctionsBoolean);

    // clean up
    CompositeMarkingsHandler::finalize();
}


/*!
   checks whether the composite marking given by the parameters is real, that is if the given knowledge
   really contains an inner marking and an associated interface given by the parameters; if this is the
   the case indeed, it is checked whether this composite marking has been visited before
   \param storedKnowledge a knowledge
   \param innerMarking an inner marking
   \param interface a pointer to an interface
   \param foundSuccessorMarking successor marking has been found or not
   \return if the composite markings exists already, a pointer to that markings is returned; NULL if the
           composite marking specified by the parameters does not exist at all; a pointer to a new
           composite marking in case the marking has not been visited yet
 */
CompositeMarking* LivelockOperatingGuideline::getSuccessorMarking(const StoredKnowledge* storedKnowledge,
                                                                  const InnerMarking_ID innerMarking,
                                                                  InterfaceMarking* interface,
                                                                  bool& foundSuccessorMarking) {


    assert(foundSuccessorMarking == false);
    assert(storedKnowledge != NULL);
    assert(interface != NULL);

    for (innermarkingcount_t i = 0; i < storedKnowledge->sizeAllMarkings; ++i) {

        if (storedKnowledge->inner[i] == innerMarking and(*storedKnowledge->interface[i] == *interface)) {

            CompositeMarking* foundMarking = CompositeMarkingsHandler::isVisited(storedKnowledge, innerMarking, interface, foundSuccessorMarking);

            return foundMarking;
        }
    }

    return NULL;
}


/*!
   recursive function to calculate all terminal strongly connected components within a given set of knowledges

   \note here the whole closure operation is done once again, i.e. given the current composite marking all of its
         successors within the current knowledge are calculated again; besides that also its successors in a successor
         knowledge are calculated
   \param currentMarking composite marking that is considered now
   \param knowledgeSCS a set of knowledges
 */
void LivelockOperatingGuideline::calculateTSCCInKnowledgeSetRecursively(CompositeMarking* currentMarking,
                                                                        const std::set<StoredKnowledge* > & knowledgeSCS) {


    assert(currentMarking != NULL);

    // remember, that we visited the current node and set Tarjan values
    CompositeMarkingsHandler::visitMarking(currentMarking);

    // put current node on Tarjan stack
    CompositeMarkingsHandler::tarjanStack.push_back(currentMarking);

    // get inner marking of current marking
    const InnerMarking* currentInner = InnerMarking::inner_markings[currentMarking->innerMarking_ID];

    assert(currentInner != NULL);

    // iterate over the successors of current inner marking --> steps internal to current knowledge
    for (uint8_t j = 0; j < currentInner->out_degree; ++j) {

        // successor marking
        CompositeMarking* successorMarking = NULL;

        // remember if the successor marking has been visited before
        bool foundSuccessorMarking = false;

        if (SYNC(currentInner->labels[j])) {
            continue;
        }

        // First step: calculate successor marking

        // we need to calculate the interface of the successor marking
        InterfaceMarking* candidate_interface = NULL;

        // if a sending or receiving transition leaves current inner marking
        if (RECEIVING(currentInner->labels[j]) or SENDING(currentInner->labels[j])) {

            bool success = true;

            // it is a receiving event, but a sending transition leaves the inner marking ;-)
            // or if it is a sending event, a receiving transition leaves the inner marking
            bool increase = RECEIVING(currentInner->labels[j]);

            candidate_interface = new InterfaceMarking(*(currentMarking->interface), currentInner->labels[j], increase, success);

            // there was either a message bound violation or the interface has not been marked appropriately
            if (not success) {
                delete candidate_interface;

                continue;
            }

            // everything is fine, so get the successor marking
            successorMarking = getSuccessorMarking(currentMarking->storedKnowledge, currentInner->successors[j], candidate_interface, foundSuccessorMarking);

        } else if (SILENT(currentInner->labels[j])) { // tau step, interface is not touched

            candidate_interface = new InterfaceMarking(*(currentMarking->interface));

            successorMarking = getSuccessorMarking(currentMarking->storedKnowledge, currentInner->successors[j], candidate_interface, foundSuccessorMarking);
        }

        if (successorMarking == NULL) {
            delete candidate_interface;

            continue;
        }

        // Second step: do Tarjan steps

        if (not foundSuccessorMarking) {
            // do Tarjan algorithm for the successor state
            calculateTSCCInKnowledgeSetRecursively(successorMarking, knowledgeSCS);

            // adjust lowlink value
            currentMarking->lowlink = MINIMUM(currentMarking->lowlink, successorMarking->lowlink);
        } else {
            currentMarking->lowlink = MINIMUM(currentMarking->lowlink, successorMarking->dfs);
        }
    }

    if (knowledgeSCS.size() > 1) {
        // iterate over the successors of current inner marking --> steps external to current knowledge
        for (Label_ID j = Label::first_receive; j <= Label::last_sync; ++j) {

            // do we actually have to consider the successor knowledge
            if (currentMarking->storedKnowledge->successors[j - 1] == NULL or
                    knowledgeSCS.find(currentMarking->storedKnowledge->successors[j - 1]) == knowledgeSCS.end()) {

                continue;
            }

            // successor marking
            CompositeMarking* successorMarking = NULL;

            // remember if the successor marking has been visited before
            bool foundSuccessorMarking = false;

            // First step: calculate successor marking

            // we need to calculate the interface of the successor marking
            InterfaceMarking* candidate_interface = NULL;

            // receive message
            if (RECEIVING(j) or SENDING(j)) {

                bool success = true;
                bool increase = SENDING(j);

                candidate_interface = new InterfaceMarking(*currentMarking->interface, j, increase, success);

                // there was either a message bound violation or the interface has not been marked appropriately
                if (not success) {
                    delete candidate_interface;

                    continue;
                }

                // everything is fine, so get the successor marking
                successorMarking = getSuccessorMarking(currentMarking->storedKnowledge->successors[j - 1], currentMarking->innerMarking_ID, candidate_interface, foundSuccessorMarking);

            } else if (SYNC(j)) { // synch step

                candidate_interface = new InterfaceMarking(*currentMarking->interface);

                for (Label_ID l = 0; l < currentInner->out_degree; ++l) {
                    if (currentInner->labels[l] == j) {
                        successorMarking = getSuccessorMarking(currentMarking->storedKnowledge->successors[j - 1], currentInner->successors[l], candidate_interface, foundSuccessorMarking);
                    }
                }
            }

            if (successorMarking == NULL) {
                delete candidate_interface;

                continue;
            }

            // Second step: do Tarjan steps

            // successor marking may be NULL in case a message bound violation is reached or there are not enough token on the
            // interface in case of an receiving event

            if (not foundSuccessorMarking) {
                // do Tarjan algorithm for the successor state
                calculateTSCCInKnowledgeSetRecursively(successorMarking, knowledgeSCS);

                // adjust lowlink value
                currentMarking->lowlink = MINIMUM(currentMarking->lowlink, successorMarking->lowlink);
            } else {

                currentMarking->lowlink = MINIMUM(currentMarking->lowlink, successorMarking->dfs);
            }
        }
    }

    // check, if the current marking is a representative of a TSCC
    // if so, get all markings within the TSCC
    if (currentMarking->lowlink == currentMarking->dfs and
            currentMarking->dfs > CompositeMarkingsHandler::minBookmark) {

        // remember this TSCC
        CompositeMarkingsHandler::minBookmark = currentMarking->dfs;

        // node which has just been popped from Tarjan stack
        CompositeMarking* poppedMarking;

        // current TSCC
        std::set<CompositeMarking* > scc;
        std::set<const StoredKnowledge* > mappingSCC;

        // at first we suppose that the clause of the current TSCC is empty
        bool emptyClause = true;
        Clause* booleanClause = new Clause();

        do {
            if (CompositeMarkingsHandler::tarjanStack.empty()) {
                continue;
            }
            // get last element
            poppedMarking = CompositeMarkingsHandler::tarjanStack.back();
            // delete last element from stack
            CompositeMarkingsHandler::tarjanStack.pop_back();

            // put popped marking into current SCC
            scc.insert(poppedMarking);
            mappingSCC.insert(poppedMarking->storedKnowledge);

            // create new clause to store the clause corresponding to the current composite marking
            Clause* booleanClauseTemp = new Clause();

            poppedMarking->getMyFormula(booleanClauseTemp, emptyClause);

            // use boolean OR to let the overall clause know about the current (part) clause
            *booleanClause |= *booleanClauseTemp;

            delete booleanClauseTemp;

        } while (currentMarking != poppedMarking);

        // check whether the TSCC projects exactly onto the given SCS of knowledges
        if (mappingSCC.size() == knowledgeSCS.size()) {
            FOREACH(knowledge, knowledgeSCS) {
                if (mappingSCC.find(*knowledge) == mappingSCC.end()) {
                    // delete the clause
                    delete booleanClause;

                    booleanClause = Clause::trueClause;
                }
            }
        } else {
            // delete the clause
            delete booleanClause;

            booleanClause = Clause::trueClause;
        }

        // the clause of the current TSCC is empty, then it false
        if (emptyClause) {

            // delete the clause
            delete booleanClause;

            booleanClause = Clause::falseClause;
        }

        CompositeMarkingsHandler::addClause(booleanClause);

        ++stats.numberOfTSCCInSCSs;

        // statistics output
        if (args_info.reportFrequency_arg and stats.numberOfTSCCInSCSs % args_info.reportFrequency_arg == 0) {
            message("%8d TSCCs within SCSs", stats.numberOfTSCCInSCSs);
        }
    }
}


/*!
  traverse all nodes that are reachable from the root and find all strongly connected sets and
  generate the annotation accordingly
  \note the operating guideline must have been traversed first,
        so set StoredKnowledge::seen is filled with all reachable knowledges
  \note fixedNodes.clear(); in die finalize-Methode packen und in der Regel nie rufen
*/
void LivelockOperatingGuideline::generateLLOG() {

    // operating guideline does not contain any cycles
    if (StoredKnowledge::stats.numberOfNonTrivialSCCs == 0) {
        // the reachability graph of the inner contains cycles, so consider each knowledge as a trivial SCS
        if (not InnerMarking::is_acyclic) {
            FOREACH(iter, StoredKnowledge::seen) {
                // create trivial SCS containing only the current knowledge
                std::set<StoredKnowledge* > SCS;
                SCS.insert(*iter);

                calculateTSCCInKnowledgeSet(SCS);
            }
        }
        // if the inner does not contain any cycles, we do not have to do anything

        LivelockOperatingGuideline::stats.numberOfSCSs = StoredKnowledge::seen.size();

        return ;
    }

    // set of nodes, fixed and non fixed ones used for calculating the set of connected components
    std::set<StoredKnowledge* > fixedNodes;

    getSCSRecursively(fixedNodes, StoredKnowledge::seen);

    //fixedNodes.clear();
    //seen.clear();
}

/*!
  recursive function to calculate all strongly connected sets within the set of knowledges
  \param fixedNodes
  \param nonFixedNodes
*/
void LivelockOperatingGuideline::getSCSRecursively(std::set<StoredKnowledge* > & fixedNodes,
                                                   std::set<StoredKnowledge* > & nonFixedNodes) {

    // initialize Tarjan's value
    maxDFS = 0;

    // storage of the union of the fixed and non fixed nodes
    std::set<StoredKnowledge* > unionFixedNodesNonFixedNodes;

    // calculate the union of the fixed and non fixed nodes and store it
    std::set_union(fixedNodes.begin(), fixedNodes.end(),
                   nonFixedNodes.begin(), nonFixedNodes.end(),
                   std::insert_iterator<std::set<StoredKnowledge* > >(unionFixedNodesNonFixedNodes, unionFixedNodesNonFixedNodes.begin()));


    // calculate the SCCs using Tarjan of the given set of nodes
    SetOfSCCs setOfSCCs;

    getSCCs(unionFixedNodesNonFixedNodes, setOfSCCs);

    if (setOfSCCs.empty()) {
        return ;
    }

    // traverse through all SCCs which are the strongly connected sets we are looking for ;-)
    FOREACH(currentSCS, setOfSCCs) {
        // if the set of fixed nodes is a subset of the current SCC we have found a real SCS
        if (std::includes((*currentSCS).begin(), (*currentSCS).end(), fixedNodes.begin(), fixedNodes.end())) {

            // statistics output
            if (args_info.reportFrequency_arg and ++stats.numberOfSCSs % args_info.reportFrequency_arg == 0) {
                message("%8d SCSs", stats.numberOfSCSs);
            }

            // calculate the reachability graph being spanned by the knowledges of the current SCS
            // and search for the TSCC within
            calculateTSCCInKnowledgeSet(*currentSCS);

            std::set<StoredKnowledge* > f(fixedNodes);

            std::set<StoredKnowledge* > n;
            std::set_intersection(nonFixedNodes.begin(), nonFixedNodes.end(),
                                  (*currentSCS).begin(), (*currentSCS).end(),
                                  std::insert_iterator<std::set<StoredKnowledge* > >(n, n.begin()));

            std::set<StoredKnowledge* > intersection(n);

            // traverse through all elements of current SCC
            FOREACH(iterSCC, intersection) {
                n.erase(*iterSCC);

                getSCSRecursively(f, n);

                f.insert(*iterSCC);
            }

            n.clear();
            f.clear();
            intersection.clear();
        }
    }

    // fixedNodes.clear();
    // nonFixedNodes.clear();
    setOfSCCs.clear();
}


/*!
   write the annotation of the livelock operating guideline to the given output stream
  \param dot true, in case we generate dot output
  \param file pointer to the output stream
  \param nodeMapping in case a more human-readable dot output is to be generated, we map every knowledge to a number and print that one instead
 */
void LivelockOperatingGuideline::output(const bool& dot, std::ostream& file, std::map<const StoredKnowledge*, unsigned int>& nodeMapping) {
    if (StoredKnowledge::stats.numberOfNonTrivialSCCs == 0 and InnerMarking::is_acyclic) {
        output_acyclic(dot, file, nodeMapping);
    } else {
        output_cyclic(dot, file, nodeMapping);
    }
}

/*!
  write the annotation of the livelock operating guideline to the given output stream
  \param dot true, in case we generate dot output
  \param file pointer to the output stream
  \param nodeMapping in case a more human-readable dot output is to be generated, we map every knowledge to a number and print that one instead

 */
void LivelockOperatingGuideline::output_cyclic(const bool& dot, std::ostream& file, std::map<const StoredKnowledge*, unsigned int>& nodeMapping) {

    if (dot) {
        file << " graph[fontname=\"Helvetica\" fontsize=10 label=<<table border=\"0\"><tr><td> </td></tr><tr><td align=\"left\">Annotations:</td></tr>\n";

        AnnotationElement* temp;
        LivelockOperatingGuideline::annotationLivelockOG.initIterator();

        while ((temp = LivelockOperatingGuideline::annotationLivelockOG.pop()) != NULL) {

            file << "<tr><td align=\"left\">";

            unsigned int i = 0;

            while (temp->setOfKnowledges[i] != NULL) {

                bool lastNode = false;

                if (not args_info.showInternalNodeNames_flag) {
                    if (nodeMapping.find(temp->setOfKnowledges[i]) == nodeMapping.end()) {
                        nodeMapping[temp->setOfKnowledges[i]] = nodeMapping.size();
                    }
                    file << nodeMapping[temp->setOfKnowledges[i]];
                } else {
                    file << reinterpret_cast<size_t>(temp->setOfKnowledges[i]);
                }

                lastNode = temp->setOfKnowledges[++i] == NULL;
                if (not lastNode) {
                    file << ", ";
                }
            }

            file << ": ";
            temp->myAnnotationToStream(true, file, nodeMapping);
            file << "</td></tr>\n";
        }

        file << "</table>>]\n";
    } else {
        file << "\nANNOTATIONS\n";

        // make sure to start with the root element
        LivelockOperatingGuideline::annotationLivelockOG.initIterator();

        AnnotationElement* temp;

        while ((temp = LivelockOperatingGuideline::annotationLivelockOG.pop()) != NULL) {

            unsigned int i = 0;
            while (temp->setOfKnowledges[i] != NULL) {

                bool lastNode = false;

                file << reinterpret_cast<size_t>(temp->setOfKnowledges[i]);
                lastNode = temp->setOfKnowledges[++i] == NULL;
                if (not lastNode) {
                    file << ", ";
                }
            }

            file << ": ";

            temp->myAnnotationToStream(false, file, nodeMapping);
            file << ";\n";
        }
    }
}


/*!
  write the annotation of the livelock operating guideline to the given output stream
  \param dot true, in case we generate dot output
  \param file pointer to the output stream
  \param nodeMapping in case a more human-readable dot output is to be generated, we map every knowledge to a number and print that one instead
 */
void LivelockOperatingGuideline::output_acyclic(const bool& dot, std::ostream& file, std::map<const StoredKnowledge*, unsigned int>& nodeMapping) {
    if (dot) {
        file << " graph[fontname=\"Helvetica\" fontsize=10 label=<<table border=\"0\"><tr><td> </td></tr><tr><td align=\"left\">Annotations:</td></tr>\n";

        FOREACH(iter, StoredKnowledge::seen) {
            if (not args_info.showInternalNodeNames_flag) {
                if (nodeMapping.find(*iter) == nodeMapping.end()) {
                    nodeMapping[*iter] = nodeMapping.size();
                }

                file << "<tr><td align=\"left\">" << nodeMapping[*iter] << ": " << (*iter)->formula(dot) << "</td></tr>\n";
            } else {
                file << "<tr><td align=\"left\">" << reinterpret_cast<size_t>(*iter) << ": " << (*iter)->formula(dot) << "</td></tr>\n";
            }
        }

        file << "</table>>]\n";
    } else {
        file << "\nANNOTATIONS\n";

        FOREACH(iter, StoredKnowledge::seen) {

            if (not args_info.showInternalNodeNames_flag) {
                if (nodeMapping.find(*iter) == nodeMapping.end()) {
                    nodeMapping[*iter] = nodeMapping.size();
                }

                file << nodeMapping[*iter] << ": " << (*iter)->formula(dot) << "\n";
            } else {
                file << reinterpret_cast<size_t>(*iter) << ": " << (*iter)->formula(dot) << "\n";
            }
        }
    }
}
