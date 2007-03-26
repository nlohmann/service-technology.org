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

using namespace std;

OGFromFileNode::OGFromFileNode(const std::string& name_, CommGraphFormula* annotation_) :
    name(name_),
    firstClause(annotation_),
    depthFirstSearchParent(NULL) {

}

OGFromFileNode::~OGFromFileNode()
{
    for (transitions_t::const_iterator trans_iter = transitions.begin();
         trans_iter != transitions.end(); ++trans_iter)
    {
        delete *trans_iter;
    }

    delete firstClause;
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
    return name;
}

void OGFromFileNode::addTransition(OGFromFileTransition* transition)
{
    transitions.insert(transition);
}

bool OGFromFileNode::hasTransitionWithLabel(const std::string& transitionLabel)
    const
{
	return getTransitionWithLabel(transitionLabel) != NULL;
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
    assert(firstClause != NULL);
    return firstClause->satisfies(assignment);
}

std::string OGFromFileNode::getAnnotationAsString() const
{
    assert(firstClause != NULL);
    return firstClause->asString();
}

CommGraphFormula* OGFromFileNode::getAnnotation() const {

    return firstClause;

}


// return the assignment that is imposed by present or absent arcs leaving the node
CommGraphFormulaAssignment* OGFromFileNode::getAssignment() {
	
	trace(TRACE_5, "computing annotation of node " + getName() + "\n");

	CommGraphFormulaAssignment* myassignment = new CommGraphFormulaAssignment();
	
	// traverse outgoing edges and set the corresponding literals
	// to true if the respective node is BLUE

    for (OGFromFileNode::transitions_t::iterator trans_iter = transitions.begin();
         trans_iter != transitions.end(); ++trans_iter) {

		myassignment->setToTrue((*trans_iter)->getLabel());
	}
	
	// we assume that literal final is always true
	myassignment->setToTrue(CommGraphFormulaAssignment::FINAL);
	
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
    CommGraphFormula* annotation)
{
    OGFromFileNode* node = new OGFromFileNode(nodeName, annotation);
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


//! \fn OGFromFile* OGFromFile::removeFalseNodes()
void OGFromFile::removeFalseNodes() {

	// the set representing the nodes that have to be analysed
	nodes_t* toAnalyse = new nodes_t();
	for (OGFromFile::nodes_t::iterator node_iter = nodes.begin();
	     node_iter != nodes.end(); ++node_iter) {
		
		cout << "adding node " << (*node_iter)->getName() << " to 'toAnalyse'" << endl;
		toAnalyse->insert(*node_iter);
	}
	
	cout << toAnalyse->size() << " nodes have to be analysed" << endl;
	
	removeFalseNodes(toAnalyse);
	
	cout << "back..." << endl;
}
	
//! \fn OGFromFile* OGFromFile::removeFalseNodes(nodes_t& toAnalyse)
void OGFromFile::removeFalseNodes(nodes_t* toAnalyse) {

	trace(TRACE_5, "OGFromFile::removeFalseNodes(nodes_t& toAnalyse): start\n");
	cout << "entering with size " << toAnalyse->size() << endl;

	// take the first node and analyse it
	for (OGFromFile::nodes_t::iterator node_iter = toAnalyse->begin();
	     node_iter == toAnalyse->begin(); ) {

		cout << "analysing node " << (*node_iter)->getName() << endl;
	
		CommGraphFormulaAssignment* myAssignment = (*node_iter)->getAssignment();
		if ((*node_iter)->getAnnotation()->value(*myAssignment) == true) {
			cout << "  node " << (*node_iter)->getName() << " fulfills its annotation" << endl;
			
			// since the node fulfills its annotation, remove it from the set
			// of nodes that have to be analysed and continue with the next one
			toAnalyse->erase(node_iter++);
			
			removeFalseNodes(toAnalyse);
		} else {
			cout << "  node " << (*node_iter)->getName() << " does NOT fulfill its annotation" << endl;
			toAnalyse->erase(node_iter++);
		}
	}

	cout << "backtracking" << endl;
	trace(TRACE_5, "OGFromFile::removeFalseNodes(nodes_t& toAnalyse): end\n");
}


//! \fn OGFromFile* OGFromFile::enforce(OGFromFile* constraint)
//! \brief enforces the current OG to respect the given constraint
//! \return the OG respecting the constraint
//! \param constraint the constraint to be enforced
OGFromFile* OGFromFile::enforce(OGFromFile* constraint) {
	trace(TRACE_5, "OGFromFile::enforce(OGFromFile* constraint): start\n");

	OGFromFile* newOG = new OGFromFile();

	// we now fill the new OG with pairs of nodes
	// therefore, we perform a coordinated dfs through OG and constraint
	
	// building a new node that has name and annotation constructed
	// from current nodes of OG and constraint
	OGFromFileNode* currentOGNode = this->getRoot();
	OGFromFileNode* currentConstraintNode = constraint->getRoot();

	std::string currentName;
	currentName = currentOGNode->getName() + "x" + currentConstraintNode->getName();

	CommGraphFormulaMultiaryAnd* currentFormula;
	currentFormula = new CommGraphFormulaMultiaryAnd(currentOGNode->getAnnotation(), currentConstraintNode->getAnnotation());

	// building the new node
	OGFromFileNode* newNode = new OGFromFileNode(currentName, currentFormula);
//	trace(TRACE_0, "adding node " + newNode->getName() + " with annotation " + newNode->getAnnotation()->asString() + "\n");
	newOG->addNode(newNode);
	newOG->setRoot(newNode);

	buildConstraintOG(currentOGNode, currentConstraintNode, newOG);	    

	trace(TRACE_5, "OGFromFile::enforce(OGFromFile* constraint): end\n");

	return newOG;
}


//! \fn OGFromFile* OGFromFile::enforce(OGFromFile* constraint)
//! \brief enforces the current OG to respect the given constraint
//! \return the OG respecting the constraint
//! \param constraint the constraint to be enforced
void OGFromFile::buildConstraintOG(OGFromFileNode* currentOGNode,
                                   OGFromFileNode* currentConstraintNode,
                                   OGFromFile* newOG) {

	trace(TRACE_5, "OGFromFile::buildConstraintOG(OGFromFileNode* currentOGNode, OGFromFileNode* currentConstraintNode, OGFromFile* newOG): start\n");

	// at this time, the node constructed from currentOGNode and
	// currentConstraintNode is already inserted

	assert(newOG->getRoot() != NULL);
	// iterate over all outgoing edges from current node of OG
    std::string currentLabel;
    for (OGFromFileNode::transitions_t::iterator trans_iter = currentOGNode->transitions.begin();
         trans_iter != currentOGNode->transitions.end(); ++trans_iter) {

		// remember the label of the egde
		currentLabel = (*trans_iter)->getLabel();

		// if the constraint automaton allows this edge
		if (currentConstraintNode->hasTransitionWithLabel(currentLabel)) {

			// remember the name of the old node of the constrained OG
			std::string currentName;
			currentName = currentOGNode->getName() + "x" + currentConstraintNode->getName();
			assert(newOG->hasNodeWithName(currentName));
	
			// compute both successors and recursively call buildConstraintOG again
			OGFromFileNode* newOGNode;
			newOGNode = currentOGNode->fireTransitionWithLabel(currentLabel);
			
			OGFromFileNode* newConstraintNode;
			newConstraintNode = currentConstraintNode->fireTransitionWithLabel(currentLabel);

			// build the new node of the constrained OG 
			// that has name and annotation constructed from current nodes of OG and constraint
			std::string newName;
			newName = newOGNode->getName() + "x" + newConstraintNode->getName();
		
			CommGraphFormulaMultiaryAnd* newFormula;
			newFormula = new CommGraphFormulaMultiaryAnd(newOGNode->getAnnotation(), newConstraintNode->getAnnotation());

			OGFromFileNode* newNode = new OGFromFileNode(newName, newFormula);

			// if the node is new, add that node to the OG
			OGFromFileNode* found = newOG->getNodeWithName(newName);

			if (found != NULL) {
				// the node was known before

//				trace(TRACE_0, "the node " + newNode->getName() + " with annotation " + newNode->getAnnotation()->asString() + " was already known\n");

//				trace(TRACE_0, "\tadding a transition from " + currentName + " to " + newName + " via label " + currentLabel + "\n");
				newOG->addTransition(currentName, newName, currentLabel);

				trace(TRACE_5, "OGFromFile::buildConstraintOG(OGFromFileNode* currentOGNode, OGFromFileNode* currentConstraintNode, OGFromFile* newOG): end\n");
			} else {
				// we computed a new node
//				trace(TRACE_0, "adding node " + newNode->getName() + " with annotation " + newNode->getAnnotation()->asString() + "\n");
				newOG->addNode(newNode);

//				trace(TRACE_0, "\tadding a transition from " + currentName + " to " + newName + " via label " + currentLabel + "\n");
				newOG->addTransition(currentName, newName, currentLabel);

				buildConstraintOG(newOGNode, newConstraintNode, newOG);
			}
		}
	}
	trace(TRACE_5, "OGFromFile::buildConstraintOG(OGFromFileNode* currentOGNode, OGFromFileNode* currentConstraintNode, OGFromFile* newOG): end\n");
}


//! \fn void OGFromFile::printDotFile()
//! \brief creates a dot file of the graph
void OGFromFile::printDotFile() {
    
	trace(TRACE_0, "creating the dot file of the OG...\n");	
    OGFromFileNode* tmp = root;

	char buffer[256];
	sprintf(buffer, "%s.constrained.out", netfile);

    fstream dotFile(buffer, ios_base::out | ios_base::trunc);
    dotFile << "digraph g1 {\n";
    dotFile << "graph [fontname=\"Helvetica\", label=\"";
    dotFile << "constrained OG of ";
    dotFile << netfile << "a.og and " << ogfile;
    dotFile << "\"];\n";
    dotFile << "node [fontname=\"Helvetica\" fontsize=10];\n";
    dotFile << "edge [fontname=\"Helvetica\" fontsize=10];\n";

    printGraphToDot(tmp, dotFile, visitedNodes);
    
    dotFile << "}";
    dotFile.close();
    	
    // prepare dot command line for printing
	sprintf(buffer, "dot -Tpng %s.constrained.out -o %s.constrained.png", netfile, netfile);

	// print commandline and execute system command
	trace(TRACE_0, string(buffer) + "\n\n");
    system(buffer);
}


//! \fn void OGFromFile::printGraphToDot(vertex * v, fstream& os, bool visitedNodes[])
//! \param v current node in the iteration process
//! \param os output stream
//! \param visitedNodes maps nodes to Bools remembering already visited nodes
//! \brief dfs through the graph printing each node and edge to the output stream
void OGFromFile::printGraphToDot(OGFromFileNode* v, fstream& os, OGFromFileNode_map& visitedNodes) {

	assert(v != NULL);

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

			os << "p" << v->getName() << "->" << "p" << successor->getName() << " [label=\"" << currentLabel << "\", fontcolor=black, color= blue];\n";
			printGraphToDot(successor, os, visitedNodes);
		}
	}
}

