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
 * \file    commGraphFormula.h
 *
 * \brief   Holds a Boolean formula over edge labels.
 *          The formulae are used as annotation to a node of an IG or OG.
 *
 * \author  responsible: Jan Bretschneider <bretschn@informatik.hu-berlin.de>
 *
 * \note    This file is part of the tool Fiona and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 */

#ifndef commGraphFormula_H_
#define commGraphFormula_H_

#include <string>
#include <set>
#include <map>

class OGFromFileFormulaAssignment {
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

class OGFromFileFormula {
public:
    virtual ~OGFromFileFormula() {};
    virtual bool value(const OGFromFileFormulaAssignment& assignment) const = 0;
    virtual bool satisfies(const OGFromFileFormulaAssignment& assignment) const;
    virtual std::string asString() const = 0;
};

class OGFromFileFormulaBinary : public OGFromFileFormula {
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

class OGFromFileFormulaBinaryAnd : public OGFromFileFormulaBinary {
public:
    OGFromFileFormulaBinaryAnd(OGFromFileFormula* lhs, OGFromFileFormula* rhs);
    virtual ~OGFromFileFormulaBinaryAnd() {};
    virtual bool value(const OGFromFileFormulaAssignment& assignment) const;
    virtual std::string getOperator() const;
};

class OGFromFileFormulaBinaryOr : public OGFromFileFormulaBinary {
public:
    OGFromFileFormulaBinaryOr(OGFromFileFormula* lhs, OGFromFileFormula* rhs);
    virtual ~OGFromFileFormulaBinaryOr() {};
    virtual bool value(const OGFromFileFormulaAssignment& assignment) const;
    virtual std::string getOperator() const;
};

class OGFromFileFormulaTrue : public OGFromFileFormula {
public:
    virtual ~OGFromFileFormulaTrue() {};
    virtual bool value(const OGFromFileFormulaAssignment& assignment) const;
    virtual std::string asString() const;
};

class OGFromFileFormulaFalse : public OGFromFileFormula {
public:
    virtual ~OGFromFileFormulaFalse() {};
    virtual bool value(const OGFromFileFormulaAssignment& assignment) const;
    virtual std::string asString() const;
};

class OGFromFileFormulaProposition : public OGFromFileFormula {
private:
    std::string proposition;
public:
    OGFromFileFormulaProposition(const std::string& proposition_);
    virtual ~OGFromFileFormulaProposition() {};
    virtual bool value(const OGFromFileFormulaAssignment& assignment) const;
    virtual std::string asString() const;
};

#endif
