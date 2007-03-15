/*****************************************************************************
 * Copyright 2005, 2006 Jan Bretschneider                                    *
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
#include <cassert>

const std::string OGFromFileFormulaAssignment::TAU = std::string("tau");
const std::string OGFromFileFormulaAssignment::FINAL = std::string("final");

void OGFromFileFormulaAssignment::set(const std::string& label, bool value)
{
    label2bool[label] = value;
}

void OGFromFileFormulaAssignment::setToTrue(const std::string& label)
{
    set(label, true);
}

bool OGFromFileFormulaAssignment::get(const std::string& label) const
{
    label2bool_t::const_iterator label2bool_iter = label2bool.find(label);

    if (label2bool_iter == label2bool.end())
        return false;

    return label2bool_iter->second;
}

bool OGFromFileFormula::satisfies(const OGFromFileFormulaAssignment& assignment)
    const
{
    return value(assignment) == true;
}

OGFromFileFormulaBinary::OGFromFileFormulaBinary(OGFromFileFormula* lhs_,
    OGFromFileFormula* rhs_) : lhs(lhs_), rhs(rhs_)
{
}

OGFromFileFormulaBinary::~OGFromFileFormulaBinary()
{
    delete lhs;
    delete rhs;
}

bool OGFromFileFormulaBinary::lhsValue(
    const OGFromFileFormulaAssignment& assignment) const
{
    return lhs->value(assignment);
}

bool OGFromFileFormulaBinary::rhsValue(
    const OGFromFileFormulaAssignment& assignment) const
{
    return rhs->value(assignment);
}

std::string OGFromFileFormulaBinary::asString() const
{
    return "(" + lhs->asString() + " " + getOperator() + " " +
        rhs->asString() + ")";
}

OGFromFileFormulaBinaryAnd::OGFromFileFormulaBinaryAnd(OGFromFileFormula* lhs_,
    OGFromFileFormula* rhs_) : OGFromFileFormulaBinary(lhs_, rhs_)
{
}

bool OGFromFileFormulaBinaryAnd::value(
    const OGFromFileFormulaAssignment& assignment) const
{
    if (!lhsValue(assignment))
        return false;

    return rhsValue(assignment);
}

std::string OGFromFileFormulaBinaryAnd::getOperator() const
{
    return "*";
}

OGFromFileFormulaBinaryOr::OGFromFileFormulaBinaryOr(OGFromFileFormula* lhs_,
    OGFromFileFormula* rhs_) : OGFromFileFormulaBinary(lhs_, rhs_)
{
}

bool OGFromFileFormulaBinaryOr::value(
    const OGFromFileFormulaAssignment& assignment) const
{
    if (lhsValue(assignment))
        return true;

    return rhsValue(assignment);
}

std::string OGFromFileFormulaBinaryOr::getOperator() const
{
    return "+";
}

bool OGFromFileFormulaTrue::value(const OGFromFileFormulaAssignment&) const
{
    return true;
}

std::string OGFromFileFormulaTrue::asString() const
{
    return "true";
}

bool OGFromFileFormulaFalse::value(const OGFromFileFormulaAssignment&) const
{
    return false;
}

std::string OGFromFileFormulaFalse::asString() const
{
    return "false";
}

OGFromFileFormulaProposition::OGFromFileFormulaProposition(
    const std::string& proposition_) : proposition(proposition_)
{
}

bool OGFromFileFormulaProposition::value(
    const OGFromFileFormulaAssignment& assignment) const
{
    return assignment.get(proposition);
}

std::string OGFromFileFormulaProposition::asString() const
{
    return proposition;
}

OGFromFileNode::OGFromFileNode(const std::string& name_, OGFromFileFormula* annotation_) :
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
    for (transitions_t::const_iterator trans_iter = transitions.begin();
        trans_iter != transitions.end(); ++trans_iter)
    {
        if ((*trans_iter)->hasLabel(transitionLabel)) {
            return *trans_iter;
        }
    }

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
    const OGFromFileFormulaAssignment& assignment) const
{
    assert(firstClause != NULL);
    return firstClause->satisfies(assignment);
}

std::string OGFromFileNode::getAnnotationAsString() const
{
    assert(firstClause != NULL);
    return firstClause->asString();
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
    OGFromFileFormula* annotation)
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
