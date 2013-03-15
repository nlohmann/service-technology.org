#include <iostream>
#include <cassert>
#include <set>
#include <string>

#include "Graph.h"
#include "Formula.h"
#include "types.h"
#include "helpers.h"


/****************************************************************************
 * constructors
 ***************************************************************************/

FormulaBinary::FormulaBinary(const Formula *_left, const Formula *_right) :
    left(_left), right(_right)
{
    assert(_left);
    assert(_right);
}

FormulaAND::FormulaAND(const Formula *_left, const Formula *_right) :
    FormulaBinary(_left, _right) {}

FormulaOR::FormulaOR(const Formula *_left, const Formula *_right) :
    FormulaBinary(_left, _right) {}

FormulaNeg::FormulaNeg(const Formula *_f, Graph *_graph) :
    f(_f), graph(_graph)
{
    assert(_f);
}

FormulaLit::FormulaLit(const std::string& _literal, Graph *_graph, Node *_node) :
    literal(_literal), graph(_graph), node(_node) {}

FormulaFinal::FormulaFinal(Graph *_graph, Node *_node) :
    graph(_graph), node(_node) {}


/****************************************************************************
 * satisfaction test
 ***************************************************************************/

bool FormulaAND::sat(const Labels *l) const {
    return (left->sat(l) && right->sat(l));
}

bool FormulaOR::sat(const Labels *l) const {
    return (left->sat(l) || right->sat(l));
}

bool FormulaNeg::sat(const Labels *l) const {
    return !(f->sat(l));
}

bool FormulaLit::sat(const Labels *l) const {
    assert(l);
    for (size_t i = 0; i < l->size(); ++i) {
        if ((*l)[i] == literal) {
            return true;
        }
    }
    return false;
}

bool FormulaTrue::sat(const Labels *l) const {
    return true;
}

bool FormulaFalse::sat(const Labels *l) const {
    return false;
}


/*!
 * When calculating the correcting matching between a service automaton and an
 * OG, we require the service automaton's final states to be sink states.e.
 * have no successors (see BPM2008 paper). Therefore, evaulation of formulas of
 * nodes in which "final" occurred were not necessary, because in that case, a
 * further correction of the automaton would be suboptimal (because adding more
 * nodes beyond that point would increase the edit distance and will never be
 * necessary due to the requirement that final states are sink states). 
 * 
 * When counting the services characterizes by an OG, however, the evaluation
 * for formulas in which "final" occurs is important. Then, we can set the
 * predicate "final" to true, because it does not change the number of
 * characterized services, but only whether the characterized services have a
 * final state or not.
 *
 * This bug was found by Martin Znamirowski. See <http://gna.org/bugs/?11944>.
 */
bool FormulaFinal::sat(const Labels *l) const {
    return true; // true by default
}


/****************************************************************************
 * test if formula contains final
 ***************************************************************************/

bool FormulaAND::hasFinal() const {
    return (left->hasFinal() || right->hasFinal()); // sic!
}

bool FormulaOR::hasFinal() const {
    return (left->hasFinal() || right->hasFinal());
}

bool FormulaNeg::hasFinal() const {
    return (f->hasFinal());
}

bool FormulaLit::hasFinal() const {
    return false;
}

bool FormulaTrue::hasFinal() const { // sic!
    return false;
}

bool FormulaFalse::hasFinal() const {
    return false;
}

bool FormulaFinal::hasFinal() const {
    return true;
}


/****************************************************************************
 * disjunctive normal form
 ***************************************************************************/

/*!
 * both children are disjunctions
 * AND(OR(a,b), OR(c,d)) == OR( OR(AND(a,c),AND(a,d)), OR(AND(b,c),AND(b,d)) )
 *
 * one child is a disjunction, the other a conjunction
 * AND(AND(a,b), OR(c,d)) == OR( AND(a,b,c), AND(a,b,d) )
 * AND(OR(c,d), AND(a,b)) == OR( AND(a,b,c), AND(a,b,d) )
 *
 * one child is a disjction, the other a uniary formula
 * AND(a,OR(b,c)) == OR( AND(a,b), AND(a,c) )
 * AND(OR(b,c),a) == OR( AND(a,b), AND(a,c) )
 */
const Formula *FormulaAND::dnf() const {
    const Formula *l_dnf = left->dnf();
    const Formula *r_dnf = right->dnf();
    const Formula *result = this;

    // both children are disjunctions
    if (l_dnf->getType() == FORMULA_OR && r_dnf->getType() == FORMULA_OR) {
        const FormulaBinary *templ = static_cast<const FormulaBinary*>(l_dnf);
        const FormulaBinary *tempr = static_cast<const FormulaBinary*>(r_dnf);
        result = new FormulaOR(new FormulaOR(new FormulaAND(templ->left,
                                                            tempr->left),
                                             new FormulaAND(templ->left,
                                                            tempr->right)),
                               new FormulaOR(new FormulaAND(templ->right,
                                                            tempr->left),
                                             new FormulaAND(templ->right,
                                                            tempr->right)));
    }
    
    // left child is disjunction, right child conjunction
    if (l_dnf->getType() == FORMULA_OR && r_dnf->getType() == FORMULA_AND) {
        const FormulaBinary *templ = static_cast<const FormulaBinary*>(l_dnf);
        result = new FormulaOR(new FormulaAND(templ->left, r_dnf),
                               new FormulaAND(templ->right, r_dnf));
    }

    // right child is disjunction, left child conjunction
    if (l_dnf->getType() == FORMULA_AND && r_dnf->getType() == FORMULA_OR) {
        const FormulaBinary *tempr = static_cast<const FormulaBinary*>(r_dnf);
        result = new FormulaOR(new FormulaAND(tempr->left, l_dnf),
                               new FormulaAND(tempr->right, l_dnf));
    }

    // left child is disjunction, right child unary
    if (l_dnf->getType() == FORMULA_OR && r_dnf->getType() == FORMULA_GEN) {
        const FormulaBinary *templ = static_cast<const FormulaBinary*>(l_dnf);
        result = new FormulaOR(new FormulaAND(templ->left, r_dnf),
                               new FormulaAND(templ->right, r_dnf));
    }

    // right child is disjunction, left child unary
    if (l_dnf->getType() == FORMULA_GEN && r_dnf->getType() == FORMULA_OR) {
        const FormulaBinary *tempr = static_cast<const FormulaBinary*>(r_dnf);
        result = new FormulaOR(new FormulaAND(tempr->left, l_dnf),
                               new FormulaAND(tempr->right, l_dnf));
    }
    
    assert(result);
    return result;
}

/*!
 */
const Formula *FormulaNeg::dnf() const { return this; }

const Formula *FormulaOR::dnf() const { return this; }
const Formula *FormulaLit::dnf() const { return this; }
const Formula *FormulaTrue::dnf() const { return this; }
const Formula *FormulaFalse::dnf() const { return this; }
const Formula *FormulaFinal::dnf() const { return this; }




/****************************************************************************
 * conjunctive normal form
 ***************************************************************************/

/*!
 * both children are conjunctions
 * OR(AND(a,b), AND(c,d)) == AND( AND(OR(a,c),OR(a,d)), AND(OR(b,c),OR(b,d)) )
 *
 * one child is a disjunction, the other a conjunction
 * OR(AND(a,b), OR(c,d)) == AND( OR(a,c,d), OR(b,c,d) )
 * OR(OR(c,d), AND(a,b)) == AND( OR(a,c,d), OR(b,c,d) )
 *
 * one child is a conjunction, the other a uniary formula
 * OR(a,AND(b,c)) == AND( OR(a,b), OR(a,c) )
 * OR(AND(b,c),a) == AND( OR(a,b), OR(a,c) )
 */
std::vector<std::vector<std::string> > *FormulaOR::cnf() const {
	std::vector<std::vector<std::string> > *l_cnf = left->cnf();
	std::vector<std::vector<std::string> > *r_cnf = right->cnf();
	std::vector<std::vector<std::string> > *result = new std::vector<std::vector<std::string> >();

	for (std::vector<std::vector<std::string> >::const_iterator it_left = l_cnf->begin(); it_left != l_cnf->end(); ++it_left) {
		for (std::vector<std::vector<std::string> >::const_iterator it_right = r_cnf->begin(); it_right != r_cnf->end(); ++it_right) {
			std::vector<std::string> temp;
			temp.insert(temp.end(), it_left->begin(), it_left->end());
			temp.insert(temp.end(), it_right->begin(), it_right->end());
			result->push_back(temp);
		}
	}

	delete l_cnf;
	delete r_cnf;
//
//	// both children are conjunctions
//	if (l_cnf->getType() == FORMULA_AND && r_cnf->getType() == FORMULA_AND) {
//		const FormulaBinary *templ = static_cast<const FormulaBinary*>(l_cnf);
//		const FormulaBinary *tempr = static_cast<const FormulaBinary*>(r_cnf);
//		result = new FormulaAND(
//					new FormulaAND(
//						new FormulaOR(templ->left, tempr->left),
//						new FormulaOR(templ->left, tempr->right)),
//					new FormulaAND(
//						new FormulaOR(templ->right,tempr->left),
//						new FormulaOR(templ->right,tempr->right)));
//	}
//
//	// left child is disjunction, right child conjunction
//	if (l_cnf->getType() == FORMULA_OR && r_cnf->getType() == FORMULA_AND) {
//		const FormulaBinary *templ = static_cast<const FormulaBinary*>(r_cnf);
//		result = new FormulaAND(new FormulaOR(templ->left, l_cnf),
//							    new FormulaOR(templ->right, l_cnf));
//	}
//
//	// right child is disjunction, left child conjunction
//	if (l_cnf->getType() == FORMULA_AND && r_cnf->getType() == FORMULA_OR) {
//		const FormulaBinary *tempr = static_cast<const FormulaBinary*>(l_cnf);
//		result = new FormulaAND(new FormulaOR(tempr->left, r_cnf),
//							    new FormulaOR(tempr->right, r_cnf));
//	}
//
//	// left child is disjunction, right child unary
//	if (l_cnf->getType() == FORMULA_OR && r_cnf->getType() == FORMULA_GEN) {
//		const FormulaBinary *templ = static_cast<const FormulaBinary*>(l_cnf);
//		result = new FormulaOR(new FormulaAND(templ->left, r_cnf),
//				new FormulaAND(templ->right, r_cnf));
//	}
//
//	// right child is disjunction, left child unary
//	if (l_cnf->getType() == FORMULA_GEN && r_cnf->getType() == FORMULA_OR) {
//		const FormulaBinary *tempr = static_cast<const FormulaBinary*>(r_cnf);
//		result = new FormulaAND(
//					new FormulaOR(tempr->left, l_cnf),
//					new FormulaOR(tempr->right, l_cnf));
//	}
//
//	assert(result);
	return result;
}


/// we don't support negations at this point
std::vector<std::vector<std::string> > *FormulaNeg::cnf() const {
    assert(false);
    std::vector<std::vector<std::string> > *result = new std::vector<std::vector<std::string> >();
    return result; // \todo:
}

std::vector<std::vector<std::string> > *FormulaAND::cnf() const {
	std::vector<std::vector<std::string> > *l_cnf = left->cnf();
	std::vector<std::vector<std::string> > *r_cnf = right->cnf();
	std::vector<std::vector<std::string> > *result = new std::vector<std::vector<std::string> >();

	result->insert(result->end(), l_cnf->begin(), l_cnf->end());
	result->insert(result->end(), r_cnf->begin(), r_cnf->end());

	delete l_cnf;
	delete r_cnf;

	return result;
}

std::vector<std::vector<std::string> > *FormulaLit::cnf() const {
	std::vector<std::string> temp;
	temp.push_back(literal);
	std::vector<std::vector<std::string> > *result = new std::vector<std::vector<std::string> >();
	result->push_back(temp);

	return result;
}

std::vector<std::vector<std::string> > *FormulaTrue::cnf() const {
	std::vector<std::string> temp;
	temp.push_back("true");
	std::vector<std::vector<std::string> > *result = new std::vector<std::vector<std::string> >();
	result->push_back(temp);

	return result;
}

std::vector<std::vector<std::string> > *FormulaFalse::cnf() const {
	std::vector<std::string> temp;
	temp.push_back("false");
	std::vector<std::vector<std::string> > *result = new std::vector<std::vector<std::string> >();
	result->push_back(temp);

	return result;
}

std::vector<std::vector<std::string> > *FormulaFinal::cnf() const {
	std::vector<std::string> temp;
	temp.push_back("final");
	std::vector<std::vector<std::string> > *result = new std::vector<std::vector<std::string> >();
	result->push_back(temp);

	return result;
}


/****************************************************************************
 * conjunctive normal form (ClauseList)
 ***************************************************************************/

/*!
 * both children are conjunctions
 * OR(AND(a,b), AND(c,d)) == AND( AND(OR(a,c),OR(a,d)), AND(OR(b,c),OR(b,d)) )
 *
 * one child is a disjunction, the other a conjunction
 * OR(AND(a,b), OR(c,d)) == AND( OR(a,c,d), OR(b,c,d) )
 * OR(OR(c,d), AND(a,b)) == AND( OR(a,c,d), OR(b,c,d) )
 *
 * one child is a conjunction, the other a uniary formula
 * OR(a,AND(b,c)) == AND( OR(a,b), OR(a,c) )
 * OR(AND(b,c),a) == AND( OR(a,b), OR(a,c) )
 */
ClauseList *FormulaOR::toClauseList() const {
	ClauseList *l_cnf = left->toClauseList();
	ClauseList *r_cnf = right->toClauseList();
	ClauseList *result = new ClauseList();

	for (ClauseList::const_iterator it_left = l_cnf->begin(); it_left != l_cnf->end(); ++it_left) {
		for (ClauseList::const_iterator it_right = r_cnf->begin(); it_right != r_cnf->end(); ++it_right) {
			Clause temp;
			temp.insert(temp.end(), it_left->begin(), it_left->end());
			temp.insert(temp.end(), it_right->begin(), it_right->end());
			result->push_back(temp);
		}
	}

	delete l_cnf;
	delete r_cnf;

	return result;
}


/// we don't support negations at this point
ClauseList *FormulaNeg::toClauseList() const {
	// sub-formula in cnf: subf = (a v b v ...) & (x v y v ...) & ...
	// => clause list: { {a,b,...}, {x,y,...}, ... }
	ClauseList *subf = f->toClauseList();

    ClauseList *result = new ClauseList();

    if(subf->empty()) { // sub formula is true!
    	Clause empty;
    	result->push_back(empty);
    	return result;
    } else if (subf->begin()->empty()) { // sub formula is false!
    	return result;
    }

    Clause empty;
    result->push_back(empty);

    // sub-formula negated: (-a & -b & ...) v (-x & -y & ...) v ...
    // => convert to clause lists: {{-a},{-b},...}, {{-x},{-y},...}, ...
    for (ClauseList::const_iterator it_clause = subf->begin(); it_clause != subf->end(); ++it_clause) {
    	ClauseList *clauses = new ClauseList();
    	for (Clause::const_iterator it_id = it_clause->begin(); it_id != it_clause->end(); ++it_id) {
    		Clause tempclause;
    		tempclause.push_back(-(*it_id));
    		clauses->push_back(tempclause);
    	}

    	// => combine all clause lists
    	result = graph->combineClauses(clauses, result);
    }

    return result;
}

ClauseList *FormulaAND::toClauseList() const {
	ClauseList *l_cnf = left->toClauseList();
	ClauseList *r_cnf = right->toClauseList();
	ClauseList *result = new ClauseList();

	if (!l_cnf->begin()->empty() && !r_cnf->end()->empty()) {
		result->insert(result->end(), l_cnf->begin(), l_cnf->end());
		result->insert(result->end(), r_cnf->begin(), r_cnf->end());
	}

	delete l_cnf;
	delete r_cnf;

	return result;
}

ClauseList *FormulaLit::toClauseList() const {
	Clause temp;
	int literalID = graph->graphformula.getLabelVarId(literal, node);
	temp.push_back(literalID);
	ClauseList *result = new ClauseList();
	result->push_back(temp);

	return result;
}

ClauseList *FormulaTrue::toClauseList() const {
	//Clause temp;
	//temp.push_back("true");
	ClauseList *result = new ClauseList();
	//result->push_back(temp);

	return result;
}

ClauseList *FormulaFalse::toClauseList() const {
	Clause temp;
	//temp.push_back("false");
	ClauseList *result = new ClauseList();
	result->push_back(temp);

	return result;
}

ClauseList *FormulaFinal::toClauseList() const {
	Clause temp;
	int literalID = graph->graphformula.getLabelVarId("final", node);
	temp.push_back(literalID);
	ClauseList *result = new ClauseList();
	result->push_back(temp);

	return result;
}




/****************************************************************************
 * string output
 ***************************************************************************/

std::string FormulaAND::toString() const {
    return ("(" + left->toString() + " * " + right->toString() + ")");
}

std::string FormulaOR::toString() const {
    return ("(" + left->toString() + " + " + right->toString() + ")");
}

std::string FormulaNeg::toString() const {
    return ("!(" + f->toString() + ")");
}

std::string FormulaLit::toString() const {
    return literal;
}

std::string FormulaTrue::toString() const {
    return ("true");
}

std::string FormulaFalse::toString() const {
    return ("false");
}

std::string FormulaFinal::toString() const {
    return ("final");
}


/****************************************************************************
 * get type of formula
 ***************************************************************************/

formulaType FormulaAND::getType() const { return FORMULA_AND; }
formulaType FormulaOR::getType() const { return FORMULA_OR; }
formulaType FormulaLit::getType() const { return FORMULA_GEN; }
formulaType FormulaNeg::getType() const { return FORMULA_GEN; }
formulaType FormulaTrue::getType() const { return FORMULA_GEN; }
formulaType FormulaFalse::getType() const { return FORMULA_GEN; }
formulaType FormulaFinal::getType() const { return FORMULA_GEN; }


/****************************************************************************
 * collect a DNF representation
 ***************************************************************************/

void FormulaAND::rep(std::set<std::set<std::string> > &a, std::set<std::string> &b) const {
    left->rep(a,b);
    right->rep(a,b);
}

void FormulaOR::rep(std::set<std::set<std::string> > &a, std::set<std::string> &b) const {
    left->rep(a,b);    
    a.insert(b);
    b.clear();
    right->rep(a,b);
}

void FormulaLit::rep(std::set<std::set<std::string> > &a, std::set<std::string> &b) const {
    b.insert(literal);
}

void FormulaNeg::rep(std::set<std::set<std::string> > &a, std::set<std::string> &b) const {
    assert(false);
}

void FormulaTrue::rep(std::set<std::set<std::string> > &a, std::set<std::string> &b) const {
    b.insert("true");
}

void FormulaFalse::rep(std::set<std::set<std::string> > &a, std::set<std::string> &b) const {
    b.insert("false");
}

void FormulaFinal::rep(std::set<std::set<std::string> > &a, std::set<std::string> &b) const {
    b.insert("final");
}
