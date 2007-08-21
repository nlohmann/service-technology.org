/*****************************************************************************
 * Copyright 2005, 2006 Peter Massuthe, Daniela Weinberg, Karsten Wolf       *
 *                      Jan Bretschneider, Christian Gierds                  *
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
 * \file    formula.h
 *
 * \brief   functions for expressing and evaluating the formula that represents
 *          the Petri net's final condition
 *
 * \author  responsible: Daniela Weinberg <weinberg@informatik.hu-berlin.de>
 *
 * \note    This file is part of the tool Fiona and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 */

#ifndef FORMULA_H
#define FORMULA_H
#include "mynew.h"
#include"owfnPlace.h"
#include"symboltab.h"
#include<fstream>
#include<set>

#include"dimensions.h"

using namespace std;

typedef enum {eq, neq, geq, leq, lt, gt, conj, disj, neg} FType;

class formula {
    public:
        FType type;
        bool value; // value of formula w.r.t. CurrentMarking
        virtual formula * posate() = 0; // remove negation in formulae without temporal
        virtual formula * negate() = 0; // operators
        virtual bool init(unsigned int *) = 0; // compute value of whole formula 
        virtual formula * deep_copy() = 0;
        virtual formula * flat_copy() = 0;

        // Compress chains of AND or OR to single n-ary AND/OR and returns a new
        // formula.
        //
        // BEWARE: After a call to merge() the old formula, that merge() was called
        // on, MUST be deleted by the caller. The only exceptions are the other
        // derived formula classes. They should not delete any old formula in their
        // implementations of merge(), because they know that their old formulas are
        // recursively deleted as soon as the outside caller deletes his old formula.
        //
        // FURTHERMORE: the caller MUST set the parent and parentindex variables
        // of the returned newly created formula to appropriate values. merge()
        // cannot do this for you, because the parent is known to the
        // caller only. This requirement also applies to all derived formula classes
        // that call merge(). You can, however, leave parent and parentindex of the
        // returned formula alone if it has the same parent as the formula you called
        // merge() on, because their values are simply copied (flat copy).
        virtual formula * merge() = 0;
        virtual void setstatic() = 0; // set links to parents and from/to mentioned places
        virtual void update(unsigned int) = 0; // incremental re-calculation of partial formula
        virtual unsigned int counttype(FType) = 0; // explore size of AND or OR chain
        virtual unsigned int collectsubs(FType, formula **, unsigned int) = 0; //collect elements of AND or OR chain
        virtual void collectplaces(std::set<owfnPlace*>& places) = 0; //collect all mentioned places and add them to /places/
        formula * parent;
        unsigned int parentindex; //position of this in parent's array of subformulas
        formula() : parent(NULL), parentindex(0) {
        }
        virtual ~formula() {
        };
};


class atomicformula : public formula {
    public:
        owfnPlace * p; // Stelle
        unsigned int k;// Vergleichszahl
        atomicformula(FType, owfnPlace *, unsigned int);
        virtual ~atomicformula() {
        };
        virtual bool init(unsigned int *);
        virtual void update(unsigned int);
        virtual formula * posate();
        virtual formula * negate();
        virtual void setstatic();
        virtual atomicformula * deep_copy();
        virtual atomicformula * flat_copy();
        virtual formula * merge();
        virtual unsigned int collectsubs(FType, formula **, unsigned int);
        virtual void collectplaces(std::set<owfnPlace*>& places);
        virtual unsigned int counttype(FType);
};


class unarybooleanformula : public formula {
    public:
        formula * sub; // Teilformeln zum Parsen, AND/OR-Ketten werden
        // nachher zu **sub komprimiert
        unarybooleanformula(FType, formula *);
        virtual ~unarybooleanformula();
        virtual void update(unsigned int);
        virtual formula * merge();
        virtual formula * posate();
        virtual formula * negate();
        virtual bool init(unsigned int *);
        virtual void setstatic();
        virtual unsigned int collectsubs(FType, formula **, unsigned int);
        virtual void collectplaces(std::set<owfnPlace*>& places);
        virtual unsigned int counttype(FType);
        virtual unarybooleanformula * deep_copy();
        virtual unarybooleanformula * flat_copy();
};


class binarybooleanformula : public formula {
    public:
        formula * left, * right; // Teilformeln zum Parsen, AND/OR-Ketten werden
        // nachher zu **sub komprimiert
        binarybooleanformula(FType, formula *, formula *);
        virtual ~binarybooleanformula();
        virtual formula * merge();
        virtual formula * posate() {
            return (formula *) 0;
        }
        virtual formula * negate() {
            return (formula *) 0;
        }
        virtual void setstatic();
        virtual void update(unsigned int) {
        }
        virtual unsigned int collectsubs(FType, formula **, unsigned int);
        virtual void collectplaces(std::set<owfnPlace*>& places);
        virtual unsigned int counttype(FType);
        virtual binarybooleanformula * deep_copy();
        virtual binarybooleanformula * flat_copy();
        virtual bool init(unsigned int *);
};


class booleanformula : public formula {
    public:
        formula ** sub; // Teilformeln 
        virtual formula * merge();
        virtual formula * posate();
        virtual formula * negate();
        virtual unsigned int collectsubs(FType, formula **, unsigned int);
        virtual void collectplaces(std::set<owfnPlace*>& places);
        virtual void update(unsigned int);
        virtual unsigned int counttype(FType);
        virtual void setstatic();
        virtual booleanformula * deep_copy();
        virtual booleanformula * flat_copy();
        unsigned int cardsub;
        virtual bool init(unsigned int *);
        unsigned int firstvalid; // Teilformeln werden manchmal nach Gueltigkeit sortiert
        // (links die ungueltigen, rechts die gueltigen)
        booleanformula() {
        }
        virtual ~booleanformula();
};

#endif
