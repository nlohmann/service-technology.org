/*!
\author Sascha Lamp
\file Invariants.cc
\status new
*/

#include <Invariants/Invariants.h>

#include <Net/Net.h>
#include <Net/LinearAlgebra.h>


using namespace std;

void TransitionInv::solve() {
	// initiate matrix for equations
	Matrix equations(Net::Card[TR]);
	u = new bool[Net::Card[TR]];
	
	// some variables
	index_t* vars = NULL;
	int64_t* coefs = NULL;
	
	// iterate over each place of the petrinet Net
	for(index_t i = 0; i < Net::Card[PL]; i++) {		
		//TODO too much space will be reserved when a transition does not change a marking of a place
		vars = new index_t[Net::CardArcs[PL][PRE][i] + Net::CardArcs[PL][POST][i]];
		coefs = new int64_t[Net::CardArcs[PL][PRE][i] + Net::CardArcs[PL][POST][i]];
		
		index_t j1 = 0, j2 = 0;
		index_t k = 0;
		
		// iterate over transitions in pre and post set of place i to merge both sets
		while(j1 < Net::CardArcs[PL][PRE][i] || j2 < Net::CardArcs[PL][POST][i]) {
			if(j1 >= Net::CardArcs[PL][PRE][i] || (j2 < Net::CardArcs[PL][PRE][i] && Net::Arc[PL][PRE][i][j1] > Net::Arc[PL][POST][i][j2])) {
				vars[k] = Net::Arc[PL][POST][i][j2];
				coefs[k] = -(int32_t) Net::Mult[PL][POST][i][j2];
				j2++;
			}
			else if(j2 >= Net::CardArcs[PL][PRE][i] || Net::Arc[PL][PRE][i][j1] < Net::Arc[PL][POST][i][j2]) {
				vars[k] = Net::Arc[PL][PRE][i][j1];
				coefs[k] = (int32_t) Net::Mult[PL][PRE][i][j1];
				j1++;
			}
			else {
				if((int32_t) Net::Mult[PL][PRE][i][j1] - (int32_t) Net::Mult[PL][POST][i][j2] != 0) {
					vars[k] = Net::Arc[PL][PRE][i][j1];
					coefs[k] = (int32_t) Net::Mult[PL][PRE][i][j1] - (int32_t) Net::Mult[PL][POST][i][j2];
				}
				j1++;
				j2++;
			}
			k++;
		}
		
		equations.addRow(k, vars, coefs, i);
	}
	
	// generate upper triangular matrix
	equations.reduce();
	
	//TODO equations.DEBUG__printMatrix();
	
	for(index_t i = 0; i < Net::Card[TR]; i++)
		u[i] = !equations.isSignificant(i);
		
	delete vars;
	delete coefs;
}
