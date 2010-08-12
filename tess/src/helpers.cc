/*****************************************************************************\
 Safira -- Implementing a Set Algebra for Service Behavior

 Copyright (c) 2010 Kathrin Kaschner

 Safira is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Safira is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Safira.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

#include <config.h>
#include <cassert>
#include "helpers.h"
#include "types.h"
#include <cstdio>
#include <iostream>
#include <sstream>
#include <map>
#include <algorithm>
#include "cmdline.h"

/// the command line parameters
extern gengetopt_args_info args_info;

extern map<string, int> label2id;
extern map<int, string> id2label;
extern map<int, char> inout;
int firstLabelId; //all labels including tau
int firstInputId;
int lastInputId;
int firstOutputId;
int lastOutputId;


void initGlobalVariables(){
    label2id.clear();
    id2label.clear();
    inout.clear();

    addLabel(Final, ' ');   //Pos. 0
    addLabel(True, ' ');    //Pos. 1
    addLabel(Tau, ' ');     //Pos. 2

	firstLabelId = 2;
}

void addLabel(string label, char io){

	int lastLabelId = label2id.size() - 1;
	if (label2id.find(label) == label2id.end()){
		++lastLabelId;
		assert(id2label.find(lastLabelId) == id2label.end());
		assert(inout.find(lastLabelId) == inout.end());
		id2label[lastLabelId] = label;
		label2id[label] = lastLabelId;
		inout[lastLabelId] = io;

		//cout << "label: " << label << "  id: " << label2id[label] << endl;
	}
}


string intToString(const int i){

	stringstream ssout;
	ssout <<  i;
       // string s (ssout.str());
	   string s = ssout.str();
	return s;
}

string setToString(set<int> myset){
	string s = "";
	for (set<int>::const_iterator iter = myset.begin(); iter != myset.end(); ++iter ){
		s = s + intToString(*iter) + " ";
	}
	return s;
}

list< set<int> > minimizeDnf(list< set<int> > dnf){

	/* remove duplicates */
	list< set<int> > modifiedDnf;
	bool hasDuplicate;

	//cout << "in minimizeDNF (begin): " << dnfToString(dnf) << endl;

	for (list< set<int> >::const_iterator list_iter1 = dnf.begin(); list_iter1 != dnf.end(); ++list_iter1){
			set<int> currentSet = *list_iter1;
			hasDuplicate = false;

			list< set<int> >::const_iterator currentPos = list_iter1;
			for (list< set<int> >::const_iterator list_iter2 = ++currentPos; list_iter2 != dnf.end(); ++list_iter2){
				set<int> setToCompare = *list_iter2;

				 if(equal(currentSet.begin(),currentSet.end(), setToCompare.begin())){
					hasDuplicate = true;
					break;
				 }
			}

			if(hasDuplicate == false){
				modifiedDnf.push_back(currentSet);
			}
	}

	//cout << "in minimizeDNF (middle): " << dnfToString(modifiedDnf) << endl;

	/* delete not essential sets */
	list< set<int> > minimalDnf;
	bool isEssential;

	for (list< set<int> >::const_iterator list_iter1 = modifiedDnf.begin(); list_iter1 != modifiedDnf.end(); ++list_iter1){
		set<int> currentSet = *list_iter1;
		isEssential = true;

		for (list< set<int> >::const_iterator list_iter2 = modifiedDnf.begin(); list_iter2 != modifiedDnf.end(); ++list_iter2){
			set<int> setToCompare = *list_iter2;

			if (list_iter1 != list_iter2){

				set<int> difference;
				set_intersection(currentSet.begin(), currentSet.end(),
						setToCompare.begin(), setToCompare.end(),
						inserter(difference, difference.begin()));

				/* In our case the two sets difference and setToCompare are equal iff they have same size.
				 * Then we can conclude that currentSet is an upperset to setToCompare (duplicates are deleted before).
				 * Thus, currentSet is not essential and must not be a part of the minimal DNF.
				 */
				if (difference.size() == setToCompare.size()){
					isEssential = false;
					break;
				}
			}
		}

		if (isEssential){
			minimalDnf.push_back(currentSet);
		}
	}

	//cout << "in minimizeDNF (end): " << dnfToString(minimalDnf) << endl;

	return minimalDnf;

}


string dnfToString(list< set<int> > dnf){
	string s = "";
	bool first = true;
	assert (dnf.size() > 0);
	for (list< set<int> >::const_iterator list_iter = dnf.begin(); list_iter != dnf.end(); ++list_iter){

		set<int> currentSet = *list_iter;
		assert(currentSet.size()> 0);

		if (first){
			first = false;
		}
		else {
			s = s + " + ";
		}

        s = s + "(";
		bool firstelement = true;
		for (set<int>::const_iterator set_iter = currentSet.begin(); set_iter != currentSet.end(); ++set_iter){
			int currentId = *set_iter;
			if (firstelement){
				firstelement = false;
			}
			else {
				s = s + " * ";
			}
			//cout << "currentId" << currentId << endl;
			//cout << "label: " <<  id2label[currentId] << endl;
			assert(id2label.find(currentId) != id2label.end());
			s = s + id2label[currentId];
		}

		s = s + ")";



	}
	return s;

}


list< set<int> > putIdIntoList(int id){
	assert(id < (int)label2id.size());
	assert(id >= 0);
	set<int> int_set;
	int_set.insert(id);
	list< set<int> > list;
	list.push_back(int_set);
	return list;
}

void printAllPartialTestCases(map<Node*, vector<TNode*> >* allPartialTestCases){
	for (int i = 0; i < label2id.size(); ++i){
		map<Node*, vector<TNode*> > currentMap = allPartialTestCases[i];

		for(map<Node*, vector<TNode*> >::const_iterator iter1 = currentMap.begin(); iter1 != currentMap.end(); ++iter1){
			vector<TNode*> currentVector = iter1->second;
			cout << "number of partial test cases for edge: --" << id2label[i] << "->" << iter1->first->id << ": " << currentVector.size() << endl;
//			if (currentVector.size() == 0){
//				cout << "empty List" << endl;
//			}
//			else {
//				for (vector<TNode*>::const_iterator iter2 = currentVector.begin(); iter2 != currentVector.end(); ++iter2){
//
//				}
//			}
		}
	}

}

