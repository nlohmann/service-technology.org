/*****************************************************************************\
 Copyright (c) 2008  Robert Danitz, Christian Gierds, Niels Lohmann,
                     Peter Massuthe, Richard Mu"ller, Janine Ott,
                     Jan Su"rmeli, Daniela Weinberg, Karsten Wolf,
                     Martin Znamirowski

 Copyright (c) 2007  Jan Bretschneider, Robert Danitz, Christian Gierds,
                     Kathrin Kaschner, Leonard Kern, Niels Lohmann,
                     Peter Massuthe, Daniela Weinberg, Karsten Wolf

 Copyright (c) 2006  Kathrin Kaschner, Peter Massuthe, Daniela Weinberg,
                     Karsten Wolf

 This file is part of Fiona, see <http://service-technology.org/fiona>.

 Fiona is free software; you can redistribute it and/or modify it under the
 terms of the GNU General Public License as published by the Free Software
 Foundation; either version 3 of the License, or (at your option) any later
 version.

 Fiona is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 A PARTICULAR PURPOSE. See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with
 Fiona (see file COPYING). If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/


/*!
 * \file    ConstraintOG.cc
 *
 * \brief   Implementation of class Graph. See ConstraintOG.h for further
 *          information.
 *
 * \author  responsible: Robert Danitz <danitz@informatik.hu-berlin.de>
 *
 */

#include "ConstraintOG.h"
#include "owfnTransition.h"
#include "debug.h"
#include "set_helpers.h"

extern int cov_yylineno;
extern int cov_yydebug;
extern int cov_yy_flex_debug;
extern FILE* cov_yyin;
extern int cov_yyerror();
extern int cov_yyparse();

extern set<string>* PlacesToCover;
extern set<string>* TransitionsToCover;

using namespace std;


//! \brief constructor
//! \param _PN underlying petri net
ConstraintOG::ConstraintOG(oWFN *_PN) : OG(_PN) {
}


//! \brief destructor
ConstraintOG::~ConstraintOG() {
    if (covConstraint) {
        delete covConstraint;
    }
}


//! \brief  computes the coverability formula in CNF and stores it
void ConstraintOG::computeCovConstraint() {
    GraphFormulaCNF *cnf= new GraphFormulaCNF;

    // every place to be covered constitutes a disjunctive clause
    for (set<string>::iterator iPlace = placesToCover.begin();
          iPlace != placesToCover.end(); ++iPlace) {
        GraphFormulaMultiaryOr *clause= new GraphFormulaMultiaryOr;

        // all OG nodes are present in the clause whose knowledge
        // contain this place
        for (nodes_t::iterator iNode = setOfNodes.begin();
             iNode != setOfNodes.end(); ++iNode) {
            if (knownPlaces[*iNode].find(*iPlace) !=
                knownPlaces[*iNode].end()) {
                GraphFormulaLiteral *literal =
                    new GraphFormulaLiteral((*iNode)->getName());
                clause->addSubFormula(literal);
            }
        }

        cnf->addClause(clause);
    }

    // every transition to be covered constitutes a disjunctive clause
    for (set<string>::iterator iTrans = transitionsToCover.begin();
         iTrans != transitionsToCover.end(); ++iTrans) {
        GraphFormulaMultiaryOr *clause= new GraphFormulaMultiaryOr;

        // all OG nodes are present in the clause whose knowledge
        // contain this transition
        for (nodes_t::iterator iNode = setOfNodes.begin();
             iNode != setOfNodes.end(); ++iNode) {
            if (knownTransitions[*iNode].find(*iTrans) !=
                knownTransitions[*iNode].end()) {
                GraphFormulaLiteral *literal =
                    new GraphFormulaLiteral((*iNode)->getName());
                clause->addSubFormula(literal);
            }
        }

        cnf->addClause(clause);
    }

    cnf->simplify();

    // if the formula is still "empty" we have to add a false clause
    // to the set of clauses, thus it becomes unsatisfiable
    if (cnf->equals() == TRUE) {
        GraphFormulaLiteral* literal = new GraphFormulaFalse;

        GraphFormulaMultiaryOr* clause= new GraphFormulaMultiaryOr;
        clause->addSubFormula(literal);

        cnf->addClause(clause);
    }

    // store the constraint
    covConstraint = cnf;
}


//! \brief delivers the coverability constraint in CNF
GraphFormulaCNF* ConstraintOG::getCovConstraint() const {
    return covConstraint;
}

//! \brief reads the set of oWFN nodes to be covered from a file
//! \param filename file name of the constraint (default: owfn-file+'.cov')
void ConstraintOG::readCovConstraint(const string& filename) {

    cov_yylineno = 1;
    cov_yydebug = 0;
    cov_yy_flex_debug = 0;
    assert(filename != "");
    TRACE(TRACE_5, "reading from file " + filename + "\n");
    cov_yyin = fopen(filename.c_str(), "r");

    if (!cov_yyin) {
    	cerr << "cannot open COV file '" << filename
    	                 << "' for reading'\n" << endl;
        setExitCode(EC_NO_COV_FILE);
    }

    PlacesToCover = new set<string>;
    TransitionsToCover = new set<string>;
    covfileToParse = filename;
    cov_yyparse();
    fclose(cov_yyin);

    for (set<string>::iterator i = PlacesToCover->begin();
         i != PlacesToCover->end(); ++i) {
        placesToCover.insert(*i);
    }

    for (set<string>::iterator i = TransitionsToCover->begin();
         i != TransitionsToCover->end(); ++i) {
        transitionsToCover.insert(*i);
    }
}

//! \brief Computes the ConstraintOG of the associated oWFN
//! NOTE: this method is derived from AnnotatedGraph::buildGraph()
void ConstraintOG::buildGraph() {

    OG::buildGraph();

    // for every blue OG node, compute its knowledge, and derive from the knowledge
    // the set of oWFN places and transitions that are seen and thus covered
    for(nodes_t::iterator iNode = setOfNodes.begin(); iNode != setOfNodes.end(); ++iNode) {

        if ((*iNode)->isBlue()) {
            for (StateSet::iterator iState=(*iNode)->reachGraphStateSet.begin();
                iState != (*iNode)->reachGraphStateSet.end(); ++iState) {

                (*iState)->decode(PN);

                // store all places with markings in the knowledge set
                unsigned int* TempCurrentMarking = PN->copyCurrentMarking();
                for(unsigned int i = 0; i<PN->getPlaceCount(); ++i) {
                    if (PN->CurrentMarking[i] > 0) {
                        knownPlaces[*iNode].insert(PN->getPlace(i)->name);
                    }
                }

                // store all transitions within the reachability graph in
                // the knowledge set
                for(unsigned int i = 0; i < (*iState)->cardFireList; ++i) {
                    owfnTransition* transition = (*iState)->firelist[i];
                    knownTransitions[*iNode].insert(transition->name);
                    for(unsigned int iArc = 0;
                        iArc != transition->getLeavingArcsCount(); ++iArc) {
                        knownPlaces[*iNode].insert(transition->getLeavingArc(iArc)->Destination->name);
                    }
                }
            }
        }
    }

    computeCovConstraint();
}


//! \brief Print ConstraintOG
//! NOTE: this method is derived from AnnotatedGraph::printOGFile()
//! \param filenamePrefix prefix of the output file
//! \param hasOWFN gives true if this Annotated Graph is also a Communication
//! Graph that contains it's oWFN. Important to determine whether a true
//! annotated node is the empty node or not.
//! \return name of the created .covog file
string ConstraintOG::createOGFile(const std::string& filenamePrefix,
                                  bool hasOWFN) const {
    ofstream ogFile(addOGFileSuffix(filenamePrefix).c_str(), ios_base::out);

    TRACE(TRACE_5, "ConstraintOG::createOGFile(): start\n");
    TRACE(TRACE_1, "creating .covog file for " + filenamePrefix + "\n");

    if (hasNoRoot()) {
        // print file for empty OG
        ogFile << "NODES" << endl
               << "  0 : " << GraphFormulaLiteral::FALSE << " : "
               << GraphNodeColor(RED).toString() << ';' << endl << endl
               << "INITIALNODE" << endl
               << "  0;" << endl << endl
               << "TRANSITIONS" << endl
               << "  ;" << endl;

        ogFile  << "COVER" << endl
                << "  ;" << endl << endl;

        ogFile.close();
        return addOGFileSuffix(filenamePrefix);
    }

    ogFile << "NODES" << endl;
    bool printedFirstNode = false;
    for (nodes_t::const_iterator iNode = setOfNodes.begin();
         iNode != setOfNodes.end(); ++iNode) {

        AnnotatedGraphNode* node = *iNode;
        if (!node->isToShow(root, hasOWFN)) {
            continue;
        }

        if (printedFirstNode) {
            ogFile << ',' << endl;
        }

        ogFile << "  " << node->getName() << " : "
               << node->getAnnotationAsString() << " : " << node->getColor().toString();

        printedFirstNode = true;
    }
    ogFile << ';'<< endl << endl;

    ogFile << "INITIALNODE"<< endl;
    assert(getRoot() != NULL);
    ogFile << "  " << getRoot()->getName() << ';' << endl << endl;

    ogFile << "TRANSITIONS" << endl;
    bool printedFirstEdge = false;
    for (nodes_t::const_iterator iNode = setOfNodes.begin();
         iNode != setOfNodes.end(); ++iNode) {

        AnnotatedGraphNode* node = *iNode;
        if (!node->isToShow(root, hasOWFN)) {
            continue;
        }

        AnnotatedGraphNode::LeavingEdges::ConstIterator iEdge =
            node->getLeavingEdgesConstIterator();

        while (iEdge->hasNext()) {
            AnnotatedGraphEdge* edge = iEdge->getNext();
            if (!edge->getDstNode()->isToShow(root, hasOWFN)) {
                continue;
            }

            if (printedFirstEdge) {
                ogFile << ',' << endl;
            }

            ogFile << "  " << node->getName() << " -> "
                   << edge->getDstNode()->getName() << " : "
                   << edge->getLabel();

            printedFirstEdge = true;
        }
        delete iEdge;
    }
    ogFile << ';' << endl << endl;


    ogFile << "COVER" << endl;

    // add places to be covered
    ogFile << "  Places to cover:";

    if (placesToCover.empty()) {
        ogFile << " none," << endl;
    } else {
        for (set<string>::iterator iSet = placesToCover.begin();
                            iSet != placesToCover.end(); ++iSet) {

            ogFile << " " << *iSet;
        }
        ogFile << "," << endl;
    }

    // add transitions to be covered
    ogFile << "  Transitions to cover:";

    if (transitionsToCover.empty()) {
        ogFile << " none," << endl;
    } else {
        for (set<string>::iterator iSet = transitionsToCover.begin();
                            iSet != transitionsToCover.end(); ++iSet) {

            ogFile << " " << *iSet;
        }
        ogFile << "," << endl;
    }

    // add global constraint
    ogFile  << "  Global Constraint: " << getCovConstraint()->asString()
            << "," << endl << endl;

    set<string> coveredNodes;
    bool first = true;
    bool empty = true;

    // add covered places/transitions per node
    for(nodes_t::const_iterator iNode = setOfNodes.begin();
        iNode != setOfNodes.end(); ++iNode) {

        if (!first) {
            ogFile << "," << endl;
        } else {
            first = false;
        }

        ogFile << "  " << (*iNode)->getName() << " :";

        if (parameters[P_COVERALL]) {
            // add all covered places of this node
            coveredNodes = knownPlaces.find(*iNode)->second;
        } else {
            // add covered places of this node:
            //      intersection of all covered places of this node
            //      and places to be covered
            coveredNodes = setIntersection(knownPlaces.find(*iNode)->second,
                                           placesToCover);
        }

        for (set<string>::iterator iSet = coveredNodes.begin();
                        iSet != coveredNodes.end(); ++iSet) {

            ogFile << " " << *iSet;
        }

        empty &= coveredNodes.empty();

        if (parameters[P_COVERALL]) {
            // add all covered transitions of this node
            coveredNodes = knownTransitions.find(*iNode)->second;
        } else {
            // add covered transitions of this node:
            //      intersection of all covered transitions in this node
            //      and transitions to be covered
            coveredNodes = setIntersection(knownTransitions.find(*iNode)->second,
                                           transitionsToCover);
        }

        for (set<string>::iterator iSet = coveredNodes.begin();
                        iSet != coveredNodes.end(); ++iSet) {

            ogFile << " " << *iSet;
        }

        empty &= coveredNodes.empty();

        if (empty) {
            ogFile << " none";
        }

        empty = true;
    }
    ogFile << ";" << endl;
    ogFile.close();

    TRACE(TRACE_5, "Constraint::createOGFile(): end\n");

    return addOGFileSuffix(filenamePrefix);
}


//! \brief adds the suffix ".covog" to a string
//! \param filePrefix a string to be modified
//! \return returns the string with the og suffix
std::string ConstraintOG::addOGFileSuffix(const std::string& filePrefix) {
    return filePrefix + ".covog";
}


//! \brief dfs through the graph printing each node and edge to the output stream
//! NOTE: this method is derived from
//! \param v current node in the iteration process
//! \param os output stream
//! \param visitedNodes maps nodes to Bools remembering already visited nodes
void ConstraintOG::createDotFileRecursively(
        AnnotatedGraphNode* v, fstream& os,
        std::map<AnnotatedGraphNode*, bool>& visitedNodes) const {

    if (v == NULL) {
        // print the empty OG...
        os << "p0"
           << " [label=\"#0\", fontcolor=black, color=red, style=dashed];\n";
        return;
    }

    if (!visitedNodes[v]) {

        os << "p"<< v->getName() << " [label=\"# "<< v->getName() << "\\n";

        // add coversets
        os << "covered:";

        set<string> covered = setUnion(knownPlaces.find(v)->second,
                                       knownTransitions.find(v)->second);
        static set<string> tocover =
            setUnion(placesToCover, transitionsToCover);

        if (!parameters[P_COVERALL]) {
            // add covered places of this node:
            // intersection of all covered places in v and places to be covered
            covered = setIntersection(covered, tocover);
        }

        for (set<string>::iterator i = covered.begin();
             i != covered.end(); ++i) {

            os << "\\n" << (*i);
        }

        if (covered.empty()) {
            os << " none\\n";
        } else {
            os << "\\n\\n";
        }

        os << v->getAnnotation()->asString();

        if (v->isFinal()) {
            os << "\", fontcolor=black, color=blue, peripheries=2];\n";
        } else {
            os << "\", fontcolor=black, color=blue];\n";
        }

        visitedNodes[v] = true;

        string currentLabel;

        AnnotatedGraphNode::LeavingEdges::ConstIterator edgeIter =
            v->getLeavingEdgesConstIterator();

        while (edgeIter->hasNext()) {
            AnnotatedGraphEdge* edge = edgeIter->getNext();

            // remember the label of the egde
            currentLabel = edge->getLabel();
            AnnotatedGraphNode* successor = edge->getDstNode();
            assert(successor != NULL);

            os << "p" << v->getName() << "->" << "p" << successor->getName()
               << " [label=\""<< currentLabel
               << "\", fontcolor=black, color= blue];\n";

            createDotFileRecursively(successor, os, visitedNodes);
        }
        delete edgeIter;
    }
}


//! \brief computes those oWFN places and transitions, which cannot be covered
//! from the set of oWFN places and transitions which shall be covered
set<string> ConstraintOG::computeUncoverables() {

    set<string> covered;
    set<string> tocover;
    set<string> uncovered;

    for (nodes_t::const_iterator iNode = setOfNodes.begin();
         iNode != setOfNodes.end(); ++iNode) {

        AnnotatedGraphNode* node = *iNode;

        covered = setUnion(covered, knownPlaces[node]);
        covered = setUnion(covered, knownTransitions[node]);
    }

    tocover = setUnion(placesToCover, transitionsToCover);

    uncovered = setDifference(tocover, covered);

    return uncovered;
}

