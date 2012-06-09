/*!
 \file StatePredicateProperty.cc
 \author Karsten
 \status new

 \brief Evaluates state predicate
 Actual property is virtual, default (base class) is full exploration
 */

#include <cstdlib>
#include <Exploration/StatePredicateProperty.h>
#include <Formula/StatePredicate.h>
#include <Formula/AtomicStatePredicate.h>
#include <Net/Net.h>
#include <Net/Transition.h>
#include <stdlib.h>
#include <Net/Marking.h>

StatePredicateProperty::StatePredicateProperty(StatePredicate* f) {
	// set formula
	predicate = f;
	f->top = f;

	index_t cardAtomic = f->countAtomic();
	AtomicStatePredicate** atomic = (AtomicStatePredicate**) malloc(
			cardAtomic * SIZEOF_VOIDP);
	f->collectAtomic(atomic);

	// initialize up sets
	for (index_t i = 0; i < cardAtomic; i++) {
		atomic[i]->setUpSet();
	}

	cardChanged = (index_t*) calloc(Net::Card[TR], SIZEOF_INDEX_T);
	changedPredicate = (AtomicStatePredicate***) malloc(
			SIZEOF_VOIDP * Net::Card[TR]);
	changedSum = (int**) malloc(SIZEOF_VOIDP * Net::Card[TR]);
	for (index_t t = 0; t < Net::Card[TR]; t++) {
		changedPredicate[t] = (AtomicStatePredicate**) malloc(
				SIZEOF_VOIDP * cardAtomic);
		changedSum[t] = (int*) malloc(SIZEOF_INDEX_T * cardAtomic);
		for (index_t i = 0; i < cardAtomic; i++) {
			int s = 0;
			// evaluate delta that t causes on this predicate
			for (index_t j = 0; j < Transition::CardDeltaT[PRE][t]; j++) {
				const index_t p = Transition::DeltaT[PRE][t][j];
				for (index_t k = 0; k < atomic[i]->cardPos; k++) {
					if (atomic[i]->posPlaces[k] == p) {
						s -= Transition::MultDeltaT[PRE][t][j]
								* atomic[i]->posMult[k];
						break;

					}
				}
				for (index_t k = 0; k < atomic[i]->cardNeg; k++) {
					if (atomic[i]->negPlaces[k] == p) {
						s += Transition::MultDeltaT[PRE][t][j]
								* atomic[i]->negMult[k];
						break;

					}
				}
			}
			for (index_t j = 0; j < Transition::CardDeltaT[POST][t]; j++) {
				const index_t p = Transition::DeltaT[POST][t][j];
				for (index_t k = 0; k < atomic[i]->cardPos; k++) {
					if (atomic[i]->posPlaces[k] == p) {
						s += Transition::MultDeltaT[POST][t][j]
								* atomic[i]->posMult[k];
						break;

					}
				}
				for (index_t k = 0; k < atomic[i]->cardNeg; k++) {
					if (atomic[i]->negPlaces[k] == p) {
						s -= Transition::MultDeltaT[POST][t][j]
								* atomic[i]->negMult[k];
						break;

					}
				}
			}
			if (s) {
				changedPredicate[t][cardChanged[t]] = atomic[i];
				changedSum[t][cardChanged[t]++] = s;
			}
		}
		changedPredicate[t] = (AtomicStatePredicate**) realloc(
				changedPredicate[t], SIZEOF_VOIDP * cardChanged[t]);
		changedSum[t] = (int*) realloc(changedSum[t],
				SIZEOF_INDEX_T * cardChanged[t]);
	}
	free(atomic);
}

StatePredicateProperty::~StatePredicateProperty() {

	for (index_t t = 0; t < Net::Card[TR]; ++t) {
		free(changedPredicate[t]);
		free(changedSum[t]);
	}
	free(cardChanged);
	free(changedPredicate);
	free(changedSum);
}


bool StatePredicateProperty::initProperty(NetState &ns) {
	predicate->evaluate(ns);
	predicate->consistency(ns);
	return predicate->value;
}

bool StatePredicateProperty::checkProperty(NetState &ns, index_t t) {
	for (index_t i = 0; i < cardChanged[t]; i++) {
		changedPredicate[t][i]->update(ns, changedSum[t][i]);
	}
	predicate->consistency(ns);
	return predicate->value;
}

bool StatePredicateProperty::updateProperty(NetState &ns, index_t t) {
	for (index_t i = 0; i < cardChanged[t]; i++) {
		changedPredicate[t][i]->update(ns, -changedSum[t][i]);
	}
	predicate->consistency(ns);
	return predicate->value;
}


SimpleProperty* StatePredicateProperty::copy() {
	StatePredicateProperty* spp = new StatePredicateProperty(predicate->copy());
	spp->stack = stack;
	spp->value = value;
	return spp;
}
