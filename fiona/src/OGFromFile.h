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
 * \file    OGFromFile.h
 *
 * \brief   Holds an Operating Guidelines (OG) read from a OG file. We do not
 *          use the class OG for storing OGs read from a file because the class
 *          OG is tailored for computing an OG and therefore heavily depends
 *          on the existence of an underlying oWFN. If we used the class OG for
 *          storing an already computed OG, we would have to reimplement most
 *          methods anyway and the possibility of reuse would be minimal.
 *
 * \author  responsible: Jan Bretschneider <bretschn@informatik.hu-berlin.de>
 *
 * \note    This file is part of the tool Fiona and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 */

#ifndef OGFROMFILE_H_
#define OGFROMFILE_H_

#include <string>
#include <set>
#include <map>

class OGFromFileFormulaAssignment
{
private:
    typedef std::map<std::string, bool> label2bool_t;
    label2bool_t label2bool;
public:
    // Reserverd labels:
    static const std::string TAU;
    static const std::string FINAL;

    void set(const std::string& label, bool value);
    void setToTrue(const std::string& label);
    bool get(const std::string& label) const;
};

class OGFromFileFormula
{
public:
    virtual ~OGFromFileFormula() {};
    virtual bool value(const OGFromFileFormulaAssignment& assignment) const = 0;
    virtual bool satisfies(const OGFromFileFormulaAssignment& assignment) const;
    virtual std::string asString() const = 0;
};

class OGFromFileFormulaBinary : public OGFromFileFormula
{
private:
    OGFromFileFormula* lhs;
    OGFromFileFormula* rhs;
public:
    OGFromFileFormulaBinary(OGFromFileFormula* lhs, OGFromFileFormula* rhs);
    virtual ~OGFromFileFormulaBinary();
    virtual bool value(const OGFromFileFormulaAssignment& assignment) const = 0;
    bool lhsValue(const OGFromFileFormulaAssignment& assignment) const;
    bool rhsValue(const OGFromFileFormulaAssignment& assignment) const;
    virtual std::string asString() const;
    virtual std::string getOperator() const = 0;
};

class OGFromFileFormulaBinaryAnd : public OGFromFileFormulaBinary
{
public:    
    OGFromFileFormulaBinaryAnd(OGFromFileFormula* lhs, OGFromFileFormula* rhs);
    virtual ~OGFromFileFormulaBinaryAnd() {};
    virtual bool value(const OGFromFileFormulaAssignment& assignment) const;
    virtual std::string getOperator() const;
};

class OGFromFileFormulaBinaryOr : public OGFromFileFormulaBinary
{
public:    
    OGFromFileFormulaBinaryOr(OGFromFileFormula* lhs, OGFromFileFormula* rhs);
    virtual ~OGFromFileFormulaBinaryOr() {};
    virtual bool value(const OGFromFileFormulaAssignment& assignment) const;
    virtual std::string getOperator() const;
};

class OGFromFileFormulaTrue : public OGFromFileFormula
{
public:    
    virtual ~OGFromFileFormulaTrue() {};
    virtual bool value(const OGFromFileFormulaAssignment& assignment) const;
    virtual std::string asString() const;
};

class OGFromFileFormulaFalse : public OGFromFileFormula
{
public:    
    virtual ~OGFromFileFormulaFalse() {};
    virtual bool value(const OGFromFileFormulaAssignment& assignment) const;
    virtual std::string asString() const;
};

class OGFromFileFormulaProposition : public OGFromFileFormula
{
private:
    std::string proposition;
public:    
    OGFromFileFormulaProposition(const std::string& proposition_);
    virtual ~OGFromFileFormulaProposition() {};
    virtual bool value(const OGFromFileFormulaAssignment& assignment) const;
    virtual std::string asString() const;
};

class OGFromFileTransition;

class OGFromFileNode
{
private:
    std::string name;
    typedef std::set<OGFromFileTransition*> transitions_t;
    transitions_t transitions;
    typedef std::map<std::string, OGFromFileNode*> transitionLabel2parentNode_t;
    transitionLabel2parentNode_t transitionLabel2parentNode;
    void addParentNodeForTransitionLabel(const std::string& transitionLabel,
        OGFromFileNode* parentNode);
    OGFromFileNode* getParentNodeForTransitionLabel(
        const std::string& transitionLabel) const;
    OGFromFileFormula* firstClause;
    OGFromFileNode* depthFirstSearchParent;
public:
    OGFromFileNode(const std::string& name_, OGFromFileFormula* annotation_);
    ~OGFromFileNode();
    std::string getName() const;
    void addTransition(OGFromFileTransition* transition);
    bool hasTransitionWithLabel(const std::string& transitionLabel) const;
    OGFromFileTransition* getTransitionWithLabel(
        const std::string& transitionLabel) const;
    OGFromFileNode* fireTransitionWithLabel(const std::string& transitionLabel);
    OGFromFileNode* backfireTransitionWithLabel(
        const std::string& transitionLabel) const;
    bool assignmentSatisfiesAnnotation(
        const OGFromFileFormulaAssignment& assignment) const;
    std::string getAnnotationAsString() const;
    void setDepthFirstSearchParent(OGFromFileNode* depthFirstSearchParent);
    OGFromFileNode* getDepthFirstSearchParent() const;
};

class OGFromFileTransition
{
private:
    OGFromFileNode* src;
    OGFromFileNode* dst;
    std::string label;
public:
    OGFromFileTransition(OGFromFileNode* src, OGFromFileNode* dst,
        const std::string& label);
    bool hasLabel(const std::string& label_) const;
    OGFromFileNode* getDst() const;
};

class oWFN;

class OGFromFile
{
private:
    OGFromFileNode* root;
    typedef std::set<OGFromFileNode*> nodes_t;
    nodes_t nodes; // needed for proper deletion of OG.
    typedef nodes_t::const_iterator nodes_const_iterator;
    typedef nodes_t::iterator nodes_iterator;
public:
    OGFromFile();
    ~OGFromFile();
    void addNode(OGFromFileNode* node);
    OGFromFileNode* addNode(const std::string& nodeName,
        OGFromFileFormula* annotation);
    void addTransition(const std::string& srcName, const std::string& dstName,
        const std::string& label);
    bool hasNodeWithName(const std::string& nodeName) const;
    OGFromFileNode* getRoot() const;
    void setRoot(OGFromFileNode* newRoot);
    void setRootToNodeWithName(const std::string& nodeName);
    OGFromFileNode* getNodeWithName(const std::string& nodeName) const;
    bool hasNoRoot() const;
};


#endif
