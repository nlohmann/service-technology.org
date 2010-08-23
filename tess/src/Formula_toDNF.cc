/*****************************************************************************\
 Tess -- Selecting Test Cases for Services

 Copyright (c) 2010 Kathrin Kaschner

 Tess is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Tess is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Tess.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

#include <config.h>
#include <queue>


#include "Formula.h"
#include "helpers.h"

extern map<int, string> id2label;
extern map<string, int> label2id;

/****************************************************************************
 * merge
 * Directly after parsing each AND node and each OR node
 * has only two children. By executing the Merge method
 * we can assure that connected AND nodes are aggregated
 * into one AND node. Merging also aggregates connected
 * OR nodes into one single OR node.
 ***************************************************************************/
void Formula::merge() {

}

void FormulaAND::merge() {

	if (!merged){
		formulas.size();
		formulas.push_back(left);
		formulas.push_back(right); //list formulas contains two elements
		//left = NULL;
		//right = NULL;
		merged = true;
	}

	list <Formula*> newChildren;
	for (list<Formula*>::const_iterator iter = formulas.begin(); iter!= formulas.end(); ++iter){
		Formula* n = *iter;
		n->merge();
		if (n->formulaType == AND) {
			newChildren.splice(newChildren.end(), n->formulas);
		}
		else {
			if (n->formulaType != TRUE){
				newChildren.push_back(n);
			}
		}
	}
	formulas.clear();
	formulas.splice(formulas.end(),newChildren);


}

void FormulaOR::merge() {

	if (!merged){
		formulas.size();
		formulas.push_back(left);
		formulas.push_back(right); //list formulas contains two elements
		//left = NULL;
		//right = NULL;
		merged = true;
	}

	list <Formula*> newChildren;
	for (list<Formula*>::const_iterator iter = formulas.begin(); iter!= formulas.end(); ++iter){
		Formula* n = *iter;
		n->merge();


		if (n->formulaType == OR) {
			newChildren.splice(newChildren.end(), n->formulas);
		}
		else {
				newChildren.push_back(n);
		}
	}
	formulas.clear();
	formulas.splice(formulas.end(),newChildren);
}

/***************************************************************
 * toMinimalDnf
 **************************************************************/

list< set<int> > FormulaAND::toMinimalDnf() {
	return minimizeDnf(toDnf());
}

list< set<int> > FormulaOR::toMinimalDnf() {
	return minimizeDnf(toDnf());
}

list< set<int> > FormulaLit::toMinimalDnf() {
	return toDnf();
}


list< set<int> > FormulaNUM::toMinimalDnf() {
	assert(false);
}

list< set<int> > FormulaTrue::toMinimalDnf() {
	return toDnf();
}

list< set<int> > FormulaFinal::toMinimalDnf() {
	return toDnf();
}


/***************************************************************
 * toDnf
 * Returns the disjunctive normal form of this formula
 * as a list of sets of literal ids.
 * Each inner set represents a conjuction term,
 * the outer list represents a disjunction term.
 *
 **************************************************************/


list< set<int> > FormulaAND::toDnf() {

	merge();

	queue< list <set<int> > > childResultQueue;

	for (list<Formula*>::const_iterator iter = formulas.begin(); iter!= formulas.end(); ++iter){
		Formula* f = *iter;
		list <set<int> > childResult = f->toDnf();
		childResultQueue.push(childResult);
	}

//	while (childResultQueue.size() != 0){
//		list <set<int> > l = childResultQueue.front();
//		set<int> s = *(l.begin());
//		int i = *(s.begin());// *((childResultQueue.front()).begin());
//		cout << id2label[i] << endl;
//		childResultQueue.pop();
//	}
//	cout << "size: " << childResultQueue.size() << endl;

	assert(childResultQueue.size() >= 1);


	if (childResultQueue.size() == 1){
		return childResultQueue.front();
	}


	while (childResultQueue.size() > 1){

		int size = childResultQueue.size();

		list <set<int> > first = childResultQueue.front();
		childResultQueue.pop();

		list <set<int> > second = childResultQueue.front();
		childResultQueue.pop();

		list <set<int> > combined = combine(first, second);

		childResultQueue.push(combined);

		assert (size > childResultQueue.size()); //eventually the while-loop will terminate!!!
	}

	assert(childResultQueue.size() == 1);

	return childResultQueue.front();
	//	list <set<int> > l = putIdIntoList(3);
	//	return l;

}


list< set<int> > FormulaAND::combine(list< set<int> > firstDnf, list< set<int> > secondDnf){
	list< set<int> > result;

	for (list< set<int> >::iterator first_iter = firstDnf.begin(); first_iter != firstDnf.end(); ++first_iter){
		set<int> firstSet = *first_iter;

		for (list <set<int> >::const_iterator snd_iter = secondDnf.begin(); snd_iter != secondDnf.end(); ++snd_iter){
			set<int> sndSet = *snd_iter;

			set<int> combination (firstSet.begin(), firstSet.end());
			combination.insert(sndSet.begin(), sndSet.end());

			result.push_back(combination);
		}
	}
	return result;
}


list< set<int> > FormulaOR::toDnf() {

	merge();
	list< set<int> > result;

	for (list<Formula*>::const_iterator iter = formulas.begin(); iter!= formulas.end(); ++iter){
		Formula* f = *iter;
		list< set<int> > subresult = f->toDnf();
		result.splice(result.end(), subresult);
	}

	return result;
}


list< set<int> > FormulaLit::toDnf() {
	return putIdIntoList(id);
}


list< set<int> > FormulaNUM::toDnf() {
	assert(false);
}

list< set<int> > FormulaTrue::toDnf() {
	return putIdIntoList(id);
}

list< set<int> > FormulaFinal::toDnf() {
	return putIdIntoList(id);
}
