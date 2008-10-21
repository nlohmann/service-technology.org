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
 * \file    formula.h
 *
 * \brief   functions for expressing and evaluating the formula that represents
 *          the Petri net's final condition
 *
 * \author  responsible: Daniela Weinberg <weinberg@informatik.hu-berlin.de>
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
        
        /// value of formula w.r.t. CurrentMarking
        bool value; 
        
        /// remove negation in formulae without temporal
        virtual formula * posate() = 0; 
        
        /// negates the formula
        virtual formula * negate() = 0; 

        /// checks whether this unary boolean formula is true with a given current marking
        virtual bool init(unsigned int *) = 0;

        /// returns a deep copy of this formula
        virtual formula * deep_copy() = 0;

        /// returns a flat copy of this formula
        virtual formula * flat_copy() = 0;

        /// Compress chains of AND or OR to single n-ary AND/OR and returns a new
        /// formula.
        ///
        /// BEWARE: After a call to merge() the old formula, that merge() was called
        /// on, MUST be deleted by the caller. The only exceptions are the other
        /// derived formula classes. They should not delete any old formula in their
        /// implementations of merge(), because they know that their old formulas are
        /// recursively deleted as soon as the outside caller deletes his old formula.
        ///
        /// FURTHERMORE: the caller MUST set the parent and parentindex variables
        /// of the returned newly created formula to appropriate values. merge()
        /// cannot do this for you, because the parent is known to the
        /// caller only. This requirement also applies to all derived formula classes
        /// that call merge(). You can, however, leave parent and parentindex of the
        /// returned formula alone if it has the same parent as the formula you called
        /// merge() on, because their values are simply copied (flat copy).
        virtual formula * merge() = 0;
        
        /// set links to parents and from/to mentioned places
        virtual void setstatic() = 0; 
        
        /// incremental re-calculation of partial formula
        virtual void update(unsigned int) = 0; 
        
        /// returns the size of longest chain of subformulas connected by a specified operator
        virtual unsigned int counttype(FType) = 0;

        /// returns an array of subformulas consisting of longest chain of subformulas connected by 
        /// a specified operator
        virtual unsigned int collectsubs(FType, formula **, unsigned int) = 0;
         
        /// brief collects all places that are used in the formula
        virtual void collectplaces(std::set<owfnPlace*>& places) = 0; //collect all mentioned places and add them to /places/

        formula * parent;
        
        /// position of this in parent's array of subformulas
        unsigned int parentindex; 
        
        /// constructor
        formula() : parent(NULL), parentindex(0) {}
        
        /// destructor
        virtual ~formula() {};

#undef new
        /// Provides user defined operator new. Needed to trace all new operations
        /// on this class.
        NEW_OPERATOR(formula)
#define new NEW_NEW
};


class atomicformula : public formula {
    public:
        
        /// Stelle
        owfnPlace * p; 
        
        /// Vergleichszahl
        unsigned int k;
        
        /// constructor
        atomicformula(FType, owfnPlace *, unsigned int);
        
        /// destructor
        virtual ~atomicformula() {};

        ///checks whether this unary boolean formula is true with a given current marking
        virtual bool init(unsigned int *);

        /// incremental re-calculation of partial formula
        virtual void update(unsigned int);

        /// remove negation in formulae without temporal
        virtual formula * posate();

        /// negates the formula
        virtual formula * negate();

        /// set links to parents and from/to mentioned places
        virtual void setstatic();

        /// returns a deep copy of this formula
        virtual atomicformula * deep_copy();

        /// returns a flat copy of this formula
        virtual atomicformula * flat_copy();

        /// Compress chains of AND or OR to single n-ary AND/OR and returns a new
        /// formula.
        virtual formula * merge();

        /// returns an array of subformulas consisting of longest chain of subformulas connected by 
        /// a specified operator
        virtual unsigned int collectsubs(FType, formula **, unsigned int);

        /// brief collects all places that are used in the formula
        virtual void collectplaces(std::set<owfnPlace*>& places);

        /// returns the longest chain of subformulas connected by a specified operator
        virtual unsigned int counttype(FType);

#undef new
        /// Provides user defined operator new. Needed to trace all new operations
        /// on this class.
        NEW_OPERATOR(atomicformula)
#define new NEW_NEW
};


class unarybooleanformula : public formula {
    public:
        // Teilformeln zum Parsen, AND/OR-Ketten werden
        // nachher zu **sub komprimiert
        formula * sub; 

        /// constructor
        unarybooleanformula(FType, formula *);

        /// destructor
        virtual ~unarybooleanformula();

        /// incremental re-calculation of partial formula
        virtual void update(unsigned int);

        /// Compress chains of AND or OR to single n-ary AND/OR and returns a new
        /// formula.
        virtual formula * merge();

        /// remove negation in formulae without temporal
        virtual formula * posate();

        /// negates the formula
        virtual formula * negate();
        
        ///checks whether this unary boolean formula is true with a given current marking
        virtual bool init(unsigned int *);

        /// set links to parents and from/to mentioned places
        virtual void setstatic();

        /// returns an array of subformulas consisting of longest chain of subformulas connected by 
        /// a specified operator
        virtual unsigned int collectsubs(FType, formula **, unsigned int);

        /// brief collects all places that are used in the formula
        virtual void collectplaces(std::set<owfnPlace*>& places);

        /// returns the longest chain of subformulas connected by a specified operator
        virtual unsigned int counttype(FType);

        /// returns a deep copy of this formula
        virtual unarybooleanformula * deep_copy();

        /// returns a flat copy of this formula
        virtual unarybooleanformula * flat_copy();

#undef new
        /// Provides user defined operator new. Needed to trace all new operations
        /// on this class.
        NEW_OPERATOR(unarybooleanformula)
#define new NEW_NEW
};


class binarybooleanformula : public formula {
    public:
        // Teilformeln zum Parsen, AND/OR-Ketten werden
        // nachher zu **sub komprimiert
        formula * left, * right; 

        /// constructor
        binarybooleanformula(FType, formula *, formula *);

        /// destructor
        virtual ~binarybooleanformula();

        /// Compress chains of AND or OR to single n-ary AND/OR and returns a new
        /// formula.
        virtual formula * merge();

        /// remove negation in formulae without temporal
        virtual formula * posate() {
            return (formula *) 0;
        }

        /// negates the formula
        virtual formula * negate() {
            return (formula *) 0;
        }

        /// set links to parents and from/to mentioned places
        virtual void setstatic();

        /// incremental re-calculation of partial formula
        virtual void update(unsigned int) {}

        /// returns an array of subformulas consisting of longest chain of subformulas connected by 
        /// a specified operator
        virtual unsigned int collectsubs(FType, formula **, unsigned int);

        /// brief collects all places that are used in the formula
        virtual void collectplaces(std::set<owfnPlace*>& places);

        /// returns the longest chain of subformulas connected by a specified operator
        virtual unsigned int counttype(FType);

        /// returns a deep copy of this formula
        virtual binarybooleanformula * deep_copy();

        /// returns a flat copy of this formula
        virtual binarybooleanformula * flat_copy();

        ///checks whether this unary boolean formula is true with a given current marking
        virtual bool init(unsigned int *);

#undef new
        /// Provides user defined operator new. Needed to trace all new operations
        /// on this class.
        NEW_OPERATOR(binarybooleanformula)
#define new NEW_NEW
};


class booleanformula : public formula {
    public:
        /// Teilformeln
        formula ** sub; 

        /// Compress chains of AND or OR to single n-ary AND/OR and returns a new
        /// formula.
        virtual formula * merge();

        /// remove negation in formulae without temporal
        virtual formula * posate();

        /// negates the formula
        virtual formula * negate();

        /// returns an array of subformulas consisting of longest chain of subformulas connected by 
        /// a specified operator
        virtual unsigned int collectsubs(FType, formula **, unsigned int);

        /// brief collects all places that are used in the formula
        virtual void collectplaces(std::set<owfnPlace*>& places);

        /// incremental re-calculation of partial formula
        virtual void update(unsigned int);

        /// returns the longest chain of subformulas connected by a specified operator
        virtual unsigned int counttype(FType);

        /// set links to parents and from/to mentioned places
        virtual void setstatic();

        /// returns a deep copy of this formula
        virtual booleanformula * deep_copy();

        /// returns a flat copy of this formula
        virtual booleanformula * flat_copy();
        
        unsigned int cardsub;

        ///checks whether this unary boolean formula is true with a given current marking
        virtual bool init(unsigned int *);

        // Teilformeln werden manchmal nach Gueltigkeit sortiert
        // (links die ungueltigen, rechts die gueltigen)
        unsigned int firstvalid; 
        
        /// constructor
        booleanformula() {}
        
        /// destructor
        virtual ~booleanformula();

#undef new
        /// Provides user defined operator new. Needed to trace all new operations
        /// on this class.
        NEW_OPERATOR(booleanformula)
#define new NEW_NEW
};

#endif
