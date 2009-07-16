#ifndef _FORMULA_H
#define _FORMULA_H

#include <string>
#include <vector>
#include <set>
#include "types.h"


/// a formula to be attached to an OG node
class Formula {
    public:
        /// returns type of the formula
        virtual formulaType getType() const = 0;
        
        /// returns whether given label set satisfies the formula
        virtual bool sat(const Labels *l) const = 0;
        
        /// returns whether formula contains a final predicate
        virtual bool hasFinal() const = 0;
        
        /// destructor
        virtual ~Formula() {};
        
        /// return a disjunctive normal form of the formula
        virtual const Formula *dnf() const = 0;

        /// return a conjunctive normal form of the formula
        virtual const Formula *cnf() const = 0;
        
        /// returns a string representation of the formula
        virtual std::string toString() const = 0;
        
        virtual void rep(std::set<std::set<std::string> > &a, std::set<std::string> &b) const = 0;
};

/// a base class for binary formulae
class FormulaBinary : public Formula {
    public:
        const Formula *left;  ///< left sub-formula
        const Formula *right; ///< right sub-formula

    public:        
        FormulaBinary(const Formula *left, const Formula *right);
};

/// a formula to express a conjunction of two other formulae
class FormulaAND : public FormulaBinary {
    public:
        FormulaAND(const Formula *left, const Formula *right);
        bool sat(const Labels *l) const;
        bool hasFinal() const;
        const Formula *dnf() const;
        const Formula *cnf() const;
        std::string toString() const;
        formulaType getType() const;
        void rep(std::set<std::set<std::string> > &a, std::set<std::string> &b) const;
};

/// a formula to express a disjunction of two other formulae
class FormulaOR : public FormulaBinary {        
    public:
        FormulaOR(const Formula *left, const Formula *right);
        bool sat(const Labels *l) const;
        bool hasFinal() const;
        const Formula *dnf() const;
        const Formula *cnf() const;
        std::string toString() const;
        formulaType getType() const;
        void rep(std::set<std::set<std::string> > &a, std::set<std::string> &b) const;
};

/// a formula to express a negation of another formula
class FormulaNeg : public Formula {
    private:
        const Formula *f; ///< sub-formula
    
    public:
        FormulaNeg(const Formula *f);
        bool sat(const Labels *l) const;
        bool hasFinal() const;
        const Formula *dnf() const;
        const Formula *cnf() const;
        std::string toString() const;
        formulaType getType() const;
        void rep(std::set<std::set<std::string> > &a, std::set<std::string> &b) const;
};

/// a formula to express a literal
class FormulaLit : public Formula {
    private:
        std::string literal; ///< the lable the litaral consists

    public:    
        FormulaLit(const std::string literal);
        bool sat(const Labels *l) const;
        bool hasFinal() const;
        const Formula *dnf() const;
        const Formula *cnf() const;
        std::string toString() const;
        formulaType getType() const;
        void rep(std::set<std::set<std::string> > &a, std::set<std::string> &b) const;
};

/// a formula to express truth
class FormulaTrue : public Formula {
    public:
        bool sat(const Labels *l) const;
        bool hasFinal() const;
        const Formula *dnf() const;
        const Formula *cnf() const;
        std::string toString() const;
        formulaType getType() const;
        void rep(std::set<std::set<std::string> > &a, std::set<std::string> &b) const;
};

/// a formula to express falsity
class FormulaFalse : public Formula {
    public:
        bool sat(const Labels *l) const;
        bool hasFinal() const;
        const Formula *dnf() const;
        const Formula *cnf() const;
        std::string toString() const;
        formulaType getType() const;
        void rep(std::set<std::set<std::string> > &a, std::set<std::string> &b) const;
};

/// a formula to express the final predicate
class FormulaFinal : public Formula {
    public:
        bool sat(const Labels *l) const;
        bool hasFinal() const;
        const Formula *dnf() const;
        const Formula *cnf() const;
        std::string toString() const;
        formulaType getType() const;
        void rep(std::set<std::set<std::string> > &a, std::set<std::string> &b) const;
};


#endif
