/*!
 \author Niels
 \file STLStore.cc
 \status new
 */

#include <algorithm>
#include "Net/Net.h"
#include "Net/Place.h"
#include "Net/Marking.h"
#include "Stores/SISTLStore.h"


SISTLStore::SISTLStore(int numberOfThreads) : SIStore(numberOfThreads) {
	insertInfo = (std::set<std::vector<capacity_t> >::iterator*) calloc(
			numberOfThreads,
			sizeof(std::set<std::vector<capacity_t> >::iterator));

	m = new std::vector<capacity_t>[numberOfThreads];
	for (int i = 0; i < numberOfThreads; i++){
		m[i].resize(Place::CardSignificant);
	}
}

bool SISTLStore::search(NetState* ns, int threadNumber) {
	// organize vector as intermediate data structure: set size on first call


	// copy current marking to vector
	std::copy(ns->Current, ns->Current + Place::CardSignificant, m[threadNumber].begin());

	// add vector to marking store
	insertInfo[threadNumber] = store.find(m[threadNumber]);

	return insertInfo[threadNumber] == store.end();
}

bool SISTLStore::insert(NetState* ns, int threadNumber) {
	store.insert(insertInfo[threadNumber], m[threadNumber]);
	return 1;
}
