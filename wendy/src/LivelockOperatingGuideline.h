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


#ifndef LIVELOCKOPERATINGGUIDELINE_H_
#define LIVELOCKOPERATINGGUIDELINE_H_

#include <set>
#include <vector>
#include <string>


class StoredKnowledge;
class CompositeMarking;
class AnnotationLivelockOG;

/// set of strongly connected components over knowledges
typedef std::vector<std::set<StoredKnowledge*> > SetOfSCCs;


/*! \todo comment me
*/
class LivelockOperatingGuideline {
    public: /* static attributes */
        /// struct combining the statistics on the class StoredKnowledge
        static struct _stats {
            public:
                /// constructor
                _stats();

                /// number of strongly connected sets within all knowledges
                unsigned int numberOfSCSs;

                /// number of terminal strongly connected sets within all SCSs
                unsigned int numberOfTSCCInSCSs;

                /// the number of all annotation elements
                unsigned int numberAllElementsAnnotations;
        } stats;

        /// livelock operating guideline only: annotation of all strongly connected sets
        static AnnotationLivelockOG annotationLivelockOG;

    private: /* static attributes */
        /// maximal dfs value given so far when calculating livelock operating guideline (Tarjan)
        static unsigned int maxDFS;

    public: /* static function */

        /// calculates all strongly connected sets within all knowledges that are reachable from the root and generates the annotation accordingly
        static void generateLLOG();

        /// write the annotation of the livelock operating guideline to the given output stream
        static void output(const bool & dot, std::ostream& file);

    private: /* static function */

        /// get all strongly connected components of the given set of knowledges
        inline static void getSCCs(const std::set<StoredKnowledge* > & toBeVisited, SetOfSCCs & setOfSCCs);

        /// searches for the given knowledge in the Tarjan stack
        inline static bool findNodeInStack(const StoredKnowledge* currentNode);

        /// recursive function to get the strongly connected components
        static void getSCCsRecursively(StoredKnowledge* currentNode,
                                std::set<StoredKnowledge* > & visited,
                                const std::set<StoredKnowledge* > & toBeVisited,
                                SetOfSCCs & setOfSCCs);

        /// recursive function to get the strongly connected sets
        static void getStronglyConnectedSetsRecursively(std::set<StoredKnowledge* > & fixedNodes,
                                                        std::set<StoredKnowledge* > & nonFixedNodes);

        /// get all terminal strongly connected components within the set of knowledges
        inline static void calculateTSCCInKnowledgeSet(const std::set<StoredKnowledge* > & knowledgeSCS);

        /// recursive function to get all terminal strongly connected components
        static void calculateTSCCInKnowledgeSetRecursively(CompositeMarking * currentMarking,
                                                                     const std::set<StoredKnowledge* > & knowledgeSCS);

        /// returns the pointer to the marking that corresponds to the given parameters
        inline static CompositeMarking * getSuccessorMarking(const StoredKnowledge * storedKnowledge,
                                                      const InnerMarking_ID innerMarking,
                                                      InterfaceMarking * interface,
                                                      bool & foundSuccessorMarking);

        /// inner marking is really a waitstate in the context of the current knowledge
        inline static bool isWaitstateInCurrentKnowledge(const InnerMarking_ID& inner, const InterfaceMarking* interface);
};

#endif /* LIVELOCKOPERATINGGUIDELINE_H_ */
