/*****************************************************************************
 * Copyright 2005, 2006, 2007 Jan Bretschneider, Peter Massuthe              *
 *                                                                           *
 * This file is part of Fiona.                                               *
 *                                                                           *
 * Fiona is free software; you can redistribute it and/or modify it          *
 * under the terms of the GNU General Public License as published by the     *
 * Free Software Foundation; either version 2 of the License, or (at your    *
 * option) any later version.                                                *
 *                                                                           *
 * Fiona is distributed in the hope that it will be useful, but WITHOUT      *
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or     *
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for  *
 * more details.                                                             *
 *                                                                           *
 * You should have received a copy of the GNU General Public License along   *
 * with Fiona; if not, write to the Free Software Foundation, Inc., 51       *
 * Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.                      *
 *****************************************************************************/

/*!
 * \file    OGFromFile.cc
 *
 * \brief   Implementation of class OGFromFile. See OGFromFile.h for further
 *          information.
 *
 * \author  responsible: Jan Bretschneider <bretschn@informatik.hu-berlin.de>
 *
 * \note    This file is part of the tool Fiona and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 */

#include "OGFromFile.h"
#include "debug.h"
#include "options.h"
#include <cassert>
#include "main.h"

using namespace std;

OGFromFileNode::OGFromFileNode(const std::string& name,
    CommGraphFormula* annotation, vertexColor color) :
    name_(name),
    color_(color),
    annotation_(annotation),
    depthFirstSearchParent(NULL) {

}

OGFromFileNode::~OGFromFileNode()
{
    for (transitions_t::const_iterator trans_iter = transitions.begin();
         trans_iter != transitions.end(); ++trans_iter)
    {
        delete *trans_iter;
    }

    delete annotation_;
}

void OGFromFileNode::addParentNodeForTransitionLabel(
    const std::string& transitionLabel, OGFromFileNode* parentNode)
{
    transitionLabel2parentNode[transitionLabel] = parentNode;
}

OGFromFileNode* OGFromFileNode::getParentNodeForTransitionLabel(
    const std::string& transitionLabel) const
{
    transitionLabel2parentNode_t::const_iterator trans2parent_iter =
        transitionLabel2parentNode.find(transitionLabel);

    if (trans2parent_iter == transitionLabel2parentNode.end())
        return NULL;

    return trans2parent_iter->second;
}

std::string OGFromFileNode::getName() const
{
    return name_;
}

vertexColor OGFromFileNode::getColor() const
{
    return color_;
}

bool OGFromFileNode::isBlue() const
{
    return getColor() == BLUE;
}

bool OGFromFileNode::isRed() const
{
    return getColor() == RED;
}

void OGFromFileNode::addTransition(OGFromFileTransition* transition)
{
    transitions.insert(transition);
}

void OGFromFileNode::removeTransitionsToNode(const OGFromFileNode* nodeToDelete)
{
	transitions_t::iterator iTransition = transitions.begin();
	while (iTransition != transitions.end()) {
		if ((*iTransition)->getDst() == nodeToDelete) {
			assert((*iTransition)->getSrc() == this);
			delete *iTransition;
			transitions.erase(iTransition++);
		} else {
			++iTransition;
		}
	}
}

bool OGFromFileNode::hasTransitionWithLabel(const std::string& transitionLabel)
    const
{
	return getTransitionWithLabel(transitionLabel) != NULL;
}

bool OGFromFileNode::hasBlueTransitionWithLabel(
    const std::string& transitionLabel) const
{
    OGFromFileTransition* transition = getTransitionWithLabel(transitionLabel);
    if (transition == NULL)
        return false;

    return transition->getDst()->isBlue();
}

OGFromFileTransition* OGFromFileNode::getTransitionWithLabel(
    const std::string& transitionLabel) const
{

    //cout << "\tgetTransitionWithLabel : start " << endl;

    for (transitions_t::const_iterator trans_iter = transitions.begin();
        trans_iter != transitions.end(); ++trans_iter)
    {
        if ((*trans_iter)->hasLabel(transitionLabel)) {
		    //cout << "\tgetTransitionWithLabel : end1 " << endl;
            return *trans_iter;
        }
    }

    //cout << "\tgetTransitionWithLabel : end2 " << endl;
    return NULL;
}

OGFromFileNode* OGFromFileNode::fireTransitionWithLabel(
    const std::string& transitionLabel)
{
    OGFromFileTransition* transition = getTransitionWithLabel(transitionLabel);
    if (transition == NULL) {
        return NULL;
    }

    OGFromFileNode* dst = transition->getDst();
    dst->addParentNodeForTransitionLabel(transitionLabel, this);
    return dst;
}

OGFromFileNode* OGFromFileNode::backfireTransitionWithLabel(
    const std::string& transitionLabel) const
{
    return getParentNodeForTransitionLabel(transitionLabel);
}

bool OGFromFileNode::assignmentSatisfiesAnnotation(
    const CommGraphFormulaAssignment& assignment) const
{
    assert(annotation_ != NULL);
    return annotation_->satisfies(assignment);
}

std::string OGFromFileNode::getAnnotationAsString() const
{
    assert(annotation_ != NULL);
    return annotation_->asString();
}

CommGraphFormula* OGFromFileNode::getAnnotation() const {

    return annotation_;

}


// return the assignment that is imposed by present or absent arcs leaving the node
CommGraphFormulaAssignment* OGFromFileNode::getAssignment() const {
	
	trace(TRACE_5, "computing annotation of node " + getName() + "\n");

	CommGraphFormulaAssignment* myassignment = new CommGraphFormulaAssignment();
	
	// traverse outgoing edges and set the corresponding literals
	// to true if the respective node is BLUE

    for (OGFromFileNode::transitions_t::iterator trans_iter = transitions.begin();
         trans_iter != transitions.end(); ++trans_iter) {

		myassignment->setToTrue((*trans_iter)->getLabel());
	}
	
	// we assume that literal final is always true
	myassignment->setToTrue(CommGraphFormulaLiteral::FINAL);
	
	return myassignment;
}


void OGFromFileNode::setDepthFirstSearchParent(
    OGFromFileNode* depthFirstSearchParent_)
{
    depthFirstSearchParent = depthFirstSearchParent_;
}

OGFromFileNode* OGFromFileNode::getDepthFirstSearchParent() const
{
    return depthFirstSearchParent;
}

OGFromFileTransition::OGFromFileTransition(OGFromFileNode* src_,
    OGFromFileNode* dst_, const std::string& label_) :
    src(src_), dst(dst_), label(label_)
{
}

bool OGFromFileTransition::hasLabel(const std::string& label_) const
{
    return label == label_;
}

OGFromFileNode* OGFromFileTransition::getDst() const
{
    return dst;
}

OGFromFileNode* OGFromFileTransition::getSrc() const
{
    return src;
}


const std::string OGFromFileTransition::getLabel() {
	return label;
}


OGFromFile::OGFromFile() : root(NULL)
{
}

OGFromFile::~OGFromFile()
{
    for (nodes_iterator node_iter = nodes.begin(); node_iter != nodes.end();
         ++node_iter)
    {
        delete *node_iter;
    }
}

void OGFromFile::addNode(OGFromFileNode* node)
{
    nodes.insert(node);
}

OGFromFileNode* OGFromFile::addNode(const std::string& nodeName,
    CommGraphFormula* annotation, vertexColor color)
{
    OGFromFileNode* node = new OGFromFileNode(nodeName, annotation, color);
    addNode(node);
    return node;
}

void OGFromFile::addTransition(const std::string& srcName,
    const std::string& dstName, const std::string& label)
{
    OGFromFileNode* src = getNodeWithName(srcName);
    OGFromFileNode* dst = getNodeWithName(dstName);
    assert(src != NULL);
    assert(dst != NULL);
    OGFromFileTransition* transition = new OGFromFileTransition(src,dst,label);
    src->addTransition(transition);
}

bool OGFromFile::hasNodeWithName(const std::string& nodeName) const
{
    return getNodeWithName(nodeName) != NULL;
}

OGFromFileNode* OGFromFile::getNodeWithName(const std::string& nodeName) const
{
    for (nodes_const_iterator node_iter = nodes.begin();
         node_iter != nodes.end(); ++node_iter)
    {
        if ((*node_iter)->getName() == nodeName)
            return *node_iter;
    }

    return NULL;
}

OGFromFileNode* OGFromFile::getRoot() const
{
    return root;
}

void OGFromFile::setRoot(OGFromFileNode* newRoot)
{
    root = newRoot;
}

void OGFromFile::setRootToNodeWithName(const std::string& nodeName)
{
    setRoot(getNodeWithName(nodeName));
}

bool OGFromFile::hasNoRoot() const
{
    return getRoot() == NULL;
}


void OGFromFile::removeFalseNodes() {

	trace(TRACE_5, "OGFromFile::removeFalseNodes(): start\n");

	bool nodesHaveChanged = true;
	while (nodesHaveChanged) {
		nodesHaveChanged = false;
		nodes_iterator iNode = nodes.begin();
		while (iNode != nodes.end())
		{
			CommGraphFormulaAssignment* iNodeAssignment = (*iNode)->getAssignment();
			if (!(*iNode)->assignmentSatisfiesAnnotation(*iNodeAssignment)) {
				removeTransitionsToNodeFromAllOtherNodes(*iNode);
				if (*iNode == getRoot()) {
					setRoot(NULL);
				}
				delete *iNode;
				nodes.erase(iNode++);
				nodesHaveChanged = true;
			} else {
				++iNode;
			}

			delete iNodeAssignment;
		}
	}

	trace(TRACE_5, "OGFromFile::removeFalseNodes(): end\n");
}


void OGFromFile::removeTransitionsToNodeFromAllOtherNodes(
    const OGFromFileNode* nodeToDelete) {

	for (nodes_iterator iNode = nodes.begin(); iNode != nodes.end(); ++iNode) {
		if (*iNode != nodeToDelete) {
			(*iNode)->removeTransitionsToNode(nodeToDelete);
		}
	}
}

//! \fn bool OGFromFile::simulates(OGFromFile *simulant)
//! \brief checks, whether this OGFromFile simulates the given simulant
//! \return true on positive check, otherwise: false
//! \param simulant the simulant that should be simulated
bool OGFromFile::simulates ( OGFromFile *simulant ) {
	trace(TRACE_5, "OGFromFile::simulates(OGFromFile *simulant): start\n" );
	//Simulation is impossible without a simulant.
	if ( simulant == NULL )
		return false;
	
	//We need to remember the nodes we already visited.
	set<OGFromFileNode*> *myVisitedNodes, *simVisitedNodes;
	myVisitedNodes = new set<OGFromFileNode*>;
	simVisitedNodes = new set<OGFromFileNode*>;
	
	//Get things moving...
	bool result = false;
	if ( simulatesRecursive(root, myVisitedNodes, simulant->getRoot(), simVisitedNodes) )
		result = true;
	
	trace(TRACE_5, "OGFromFile::simulates(OGFromFile *simulant): end\n" );
	return result;
}

//! \fn bool OGFromFile::simulatesRecursive(OGFromFileNode *myNode, OGFromFileNode *simNode)
//! \brief checks, whether the part of an OGFromFile below myNode simulates
//         the part of an OGFromFile below simNode
//! \return true on positive check, otherwise: false
//! \param myNode a node in this OGFromFile
//! \param simNode a node in the simulant
bool OGFromFile::simulatesRecursive ( OGFromFileNode *myNode, 
									  set<OGFromFileNode*> *myVisitedNodes, 
									  OGFromFileNode *simNode,
									  set<OGFromFileNode*> *simVisitedNodes) {
	//If the simulant has no further nodes then myNode simulates simNode.
	if ( simNode == NULL )
		return true;
	//If simNode has a subgraph but myNode does not 
	//then myNode cannot simulate simNode.
	if ( myNode == NULL ) 
		return false;
	//The above two checks shouldn't matter anyway because there should be no
	//edges pointing to NULL, or should they? Let's just keep those checks
	//there for now.
	
	//If we already visited this node in the simulant, then we're done.
	if ( simVisitedNodes->find(simNode) != simVisitedNodes->end() )
		return true;
	else
		simVisitedNodes->insert(simNode);
	//If we have visited this node in the simulator, but not in the simulant,
	//then we screwed up badly (I think). Simulation isn't possible, for sure.
	if ( myVisitedNodes->find(myNode) != myVisitedNodes->end() )
		return false;
	
	trace(TRACE_5, "OGFromFile::simulateRecursive: checking annotations\n");
	if ( simNode->getAnnotation()->getCNF()->implies(myNode->getAnnotation()->getCNF()) )
		trace(TRACE_5, "OGFromFile::simulatesRecursive: annotations ok\n" );
	else {
		trace(TRACE_5, "OGFromFile::simulatesRecursive: annotations incompatible\n");
		return false;
	}
	
	//Now, we have to check whether the two graphs are compatible.
	OGFromFileNode::transitions_t::iterator myTransIter, simTransIter;
	trace ( TRACE_5, "Iterating over the transitions of the simulant's node.\n" );
	for ( simTransIter = simNode->transitions.begin();
		  simTransIter!= simNode->transitions.end();
		  simTransIter++ ) {
		trace(TRACE_5, "Trying to find the transition in the simulator.\n" );
		myTransIter = myNode->transitions.begin();
		while ( ( myTransIter != myNode->transitions.end() ) &&
				( (*myTransIter)->getLabel().compare((*simTransIter)->getLabel()) ) ) {
			myTransIter++;
		}
		
		if ( myTransIter == myNode->transitions.end() )
			return false;
		else {
			trace(TRACE_5, "These two nodes seem compatible.\n" );
			if (!simulatesRecursive ( (*myTransIter)->getDst(), 
									  myVisitedNodes,
									  (*simTransIter)->getDst(),
									  simVisitedNodes))
				return false;
		}
	}
	
	//All checks were successful.
	return true;
}

OGFromFile* OGFromFile::enforce(const OGFromFile* rhs) {
    return product(rhs);
}

OGFromFile* OGFromFile::product(const OGFromFile* rhs) {
	trace(TRACE_5, "OGFromFile::product(const OGFromFile* rhs): start\n");

	// this will be the product OG
	OGFromFile* productOG = new OGFromFile();

	// first we build a new root node that has name and annotation constructed
	// from the root nodes of OG and the rhs OG.
	OGFromFileNode* currentOGNode = this->getRoot();
	OGFromFileNode* currentRhsNode = rhs->getRoot();

	std::string currentName;
	currentName = currentOGNode->getName() + "x" + currentRhsNode->getName();

	CommGraphFormulaMultiaryAnd* currentFormula;
	currentFormula = new CommGraphFormulaMultiaryAnd(
	    currentOGNode->getAnnotation()->getDeepCopy(),
	    currentRhsNode->getAnnotation()->getDeepCopy()
	);

	// building the new root node of the product OG
	OGFromFileNode* productNode = new OGFromFileNode(currentName,
	    currentFormula);
	productOG->addNode(productNode);
	productOG->setRoot(productNode);

	// builds the successor nodes of the root nodes of OG and rhs OG
	// therefore, we perform a coordinated dfs through OG and the rhs OG
	buildProductOG(currentOGNode, currentRhsNode, productOG);	    

	trace(TRACE_5, "OGFromFile::product(const OGFromFile* rhs): end\n");

	return productOG;
}


OGFromFile* OGFromFile::product(const ogs_t& ogs)
{
    assert(ogs.size() != 0);

    ogs_t::const_iterator iOG = ogs.begin();
    OGFromFile* productOG = *iOG;
    for (++iOG; iOG != ogs.end(); ++iOG)
    {
        OGFromFile* oldProductOG = productOG;
        productOG = productOG->product(*iOG);
        delete oldProductOG;
    }

    return productOG;
}

void OGFromFile::buildProductOG(OGFromFileNode* currentOGNode,
                                OGFromFileNode* currentRhsNode,
                                OGFromFile* productOG) {

	trace(TRACE_5, "OGFromFile::buildProductOG(const OGFromFileNode* currentOGNode, const OGFromFileNode* currentRhsNode, OGFromFile* productOG): start\n");

	// at this time, the node constructed from currentOGNode and
	// currentRhsNode is already inserted

	assert(productOG->getRoot() != NULL);

	// iterate over all outgoing edges from current node of OG
    std::string currentLabel;
    for (OGFromFileNode::transitions_t::iterator trans_iter = currentOGNode->transitions.begin();
         trans_iter != currentOGNode->transitions.end(); ++trans_iter) {

		// remember the label of the egde
		currentLabel = (*trans_iter)->getLabel();

		// if the rhs automaton allows this edge
		if (currentRhsNode->hasTransitionWithLabel(currentLabel)) {

			// remember the name of the old node of the product OG
			std::string currentName;
			currentName = currentOGNode->getName() + "x" + currentRhsNode->getName();
			assert(productOG->hasNodeWithName(currentName));
	
			// compute both successors and recursively call buildProductOG again
			OGFromFileNode* newOGNode;
			newOGNode = currentOGNode->fireTransitionWithLabel(currentLabel);
			
			OGFromFileNode* newRhsNode;
			newRhsNode = currentRhsNode->fireTransitionWithLabel(currentLabel);

			// build the new node of the product OG 
			// that has name and annotation constructed from current nodes of OG and rhs OG
			std::string newProductName;
			newProductName = newOGNode->getName() + "x" + newRhsNode->getName();
			// if the node is new, add that node to the OG
			OGFromFileNode* found = productOG->getNodeWithName(newProductName);

			if (found != NULL) {
				// the node was known before, so we just have to add a new edge
				productOG->addTransition(currentName, newProductName, currentLabel);

				trace(TRACE_5, "OGFromFile::buildProductOG(const OGFromFileNode* currentOGNode, const OGFromFileNode* currentRhsNode, OGFromFile* productOG): end\n");
			} else {
				// we computed a new node, so we add a node and an edge
//				trace(TRACE_0, "adding node " + newNode->getName() + " with annotation " + newNode->getAnnotation()->asString() + "\n");

				CommGraphFormulaMultiaryAnd* newProductFormula;
				newProductFormula = new CommGraphFormulaMultiaryAnd(
					newOGNode->getAnnotation()->getDeepCopy(),
					newRhsNode->getAnnotation()->getDeepCopy()
				);

				OGFromFileNode* newProductNode = new OGFromFileNode(newProductName, newProductFormula);

				productOG->addNode(newProductNode);

				// going down recursively
				productOG->addTransition(currentName, newProductName, currentLabel);

				buildProductOG(newOGNode, newRhsNode, productOG);
			}
		}
	}
	trace(TRACE_5, "OGFromFile::buildProductOG(const OGFromFileNode* currentOGNode, const OGFromFileNode* currentRhsNode, OGFromFile* productOG): end\n");
}

std::string OGFromFile::getProductOGFilePrefix(const ogfiles_t& ogfiles)
{
    assert(ogfiles.size() != 0);

    ogfiles_t::const_iterator iOgFile = ogfiles.begin();
    string productFilePrefix = stripOGFileSuffix(*iOgFile);

    for (++iOgFile; iOgFile != ogfiles.end(); ++iOgFile)
    {
        productFilePrefix += "_X_" +
            stripOGFileSuffix(platform_basename(*iOgFile));
    }

    return productFilePrefix + ".og";
}

std::string OGFromFile::stripOGFileSuffix(const std::string& filename)
{
    static const string ogFileSuffix = ".og";
    if (filename.substr(filename.size() - ogFileSuffix.size()) == ogFileSuffix)
    {
        return filename.substr(0, filename.size() - ogFileSuffix.size());
    }

    return filename;
}

void OGFromFile::printDotFile(const std::string& filenamePrefix,
    const std::string& dotGraphTitle) const
{
    trace(TRACE_0, "creating the dot file of the OG...\n");

    string dotFile = filenamePrefix + ".out";
    string pngFile = filenamePrefix + ".png";
    fstream dotFileHandle(dotFile.c_str(), ios_base::out | ios_base::trunc);
    dotFileHandle << "digraph g1 {\n";
    dotFileHandle << "graph [fontname=\"Helvetica\", label=\"";
    dotFileHandle << dotGraphTitle;
    dotFileHandle << "\"];\n";
    dotFileHandle << "node [fontname=\"Helvetica\" fontsize=10];\n";
    dotFileHandle << "edge [fontname=\"Helvetica\" fontsize=10];\n";

    std::map<OGFromFileNode*, bool> visitedNodes;
    printGraphToDot(getRoot(), dotFileHandle, visitedNodes);

    dotFileHandle << "}";
    dotFileHandle.close();

    // prepare dot command line for printing
    string cmd = "dot -Tpng " + dotFile + " -o " + pngFile;

    // print commandline and execute system command
    trace(TRACE_0, cmd + "\n\n");
    system(cmd.c_str());
}

void OGFromFile::printDotFile(const std::string& filenamePrefix) const
{
    printDotFile(filenamePrefix, filenamePrefix);
}

//! \fn void OGFromFile::printDotFile()
//! \brief creates a dot file of the graph
void OGFromFile::printDotFile() const {

    string netogfile = string(netfile) + ".a.og";
    string outfilesPrefix = netogfile + ".under." +
        platform_basename(constraintfile);
    printDotFile(outfilesPrefix, "constrained OG of " + netogfile + " and " +
        constraintfile);
}


//! \fn void OGFromFile::printGraphToDot(vertex * v, fstream& os, bool visitedNodes[])
//! \param v current node in the iteration process
//! \param os output stream
//! \param visitedNodes maps nodes to Bools remembering already visited nodes
//! \brief dfs through the graph printing each node and edge to the output stream
void OGFromFile::printGraphToDot(OGFromFileNode* v, fstream& os, std::map<OGFromFileNode*, bool>& visitedNodes) const {

	if (v == NULL) {
		// print the empty OG...	
		os << "p0" << " [label=\"#0\", fontcolor=black, color=red, style=dashed];\n";
		return;		
	}

	if (visitedNodes[v] != true) {
	
		os << "p" << v->getName() << " [label=\"# " << v->getName() << "\\n";
	
		string CNFString = v->getAnnotation()->asString();
	
		os << CNFString;
	
	    os << "\", fontcolor=black, color=blue];\n";
	
	    visitedNodes[v] = true;
	
	    std::string currentLabel;
	    
	    for (OGFromFileNode::transitions_t::iterator trans_iter = v->transitions.begin();
	         trans_iter != v->transitions.end(); ++trans_iter) {
	
			// remember the label of the egde
			currentLabel = (*trans_iter)->getLabel();
			OGFromFileNode* successor = v->fireTransitionWithLabel(currentLabel);
			assert(successor != NULL);

			os << "p" << v->getName() << "->" << "p" << successor->getName() << " [label=\"" << currentLabel << "\", fontcolor=black, color= blue];\n";
			printGraphToDot(successor, os, visitedNodes);
		}
	}
}

