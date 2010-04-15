#include <typeinfo>
#include <iostream>
#include "setsOfFinalMarkings.h"

using std::string;

bool PartialMarking::isEquivalentTo(PartialMarking* other) {

	// If the two partial markings are equivalent, they define the same number of places.
	if (values.size != other->values.size) {
		return false;
	}

	for (BinaryTreeIterator<const pnapi::Place*, int>* it = values.begin(); it->valid(); it->next()) {
		// If the two partial markings are equivalent, each place that is defined by this, is also defined by the other.
		if (!(other->defines(it->getKey()))) {
			return false;
		}

		// If the two partial markings are equivalent, both markings define the same value
		if (other->values.find(it->getKey())->value != it->getValue()) {
			return false;
		}

	}

	/*	for (BinaryTreeIterator<const pnapi::Place*,int>* it = other->values.begin(); it->valid(); it->next()) {

	 if (!(defines(it->getKey()))) {
	 return false;
	 }

	 if (values.find(it->getKey())->value != it->getValue()) {
	 return false;
	 }

	 }
	 */

	return true;
}

PartialMarking* PartialMarking::merge(PartialMarking* m1, PartialMarking* m2) {

	PartialMarking* result = new PartialMarking();

	// Collect each element of the first marking.
	for (BinaryTreeIterator<const pnapi::Place*, int>* it = m1->values.begin(); it->valid(); it->next()) {

		// If we find a contradiction, we delete the result and return the null pointer.
		if (m2->defines(it->getKey()) && m2->values.find(it->getKey())->value
				!= it->getValue()) {
			delete result;
			return 0;
		}
		result->values.insert(it->getKey(), it->getValue(), true);
	}

	// Collect each element of the second marking.
	for (BinaryTreeIterator<const pnapi::Place*, int>* it = m2->values.begin(); it->valid(); it->next()) {
		// Only add those places that are not defined yet.
		if (!result->defines(it->getKey())) {
			result->values.insert(it->getKey(), it->getValue(), true);
		}
	}

	return result;

}

void PartialMarking::output(std::ostream& stream) {

	for (BinaryTreeIterator<const pnapi::Place*, int>* it = values.begin(); it->valid(); it->next()) {
		stream << it->getKey()->getName() << "=" << it->getValue() << "; ";
	}
	stream << std::endl;

}

string PartialMarking::toString() {

	string result = "";
	
	bool firsttime = true;

	for (BinaryTreeIterator<const pnapi::Place*, int>* it = values.begin(); it->valid(); it->next()) {
	if (!firsttime) {
		result += ", ";
		
		} else {
		firsttime = false;
		}
	result += it->getKey()->getName() + "=" + LindaHelpers::intToStr(
				it->getValue());
				
	}

	
	return result + ";";
}

void SetOfPartialMarkings::output() {

	for (std::vector<PartialMarking*>::iterator it = partialMarkings.begin(); it
			!= partialMarkings.end(); ++it) {
		(*it)->output(std::cerr);
	}

}

void SetOfPartialMarkings::add(SetOfPartialMarkings* aSet) {
	for (std::vector<PartialMarking*>::iterator mIt =
			aSet->partialMarkings.begin(); mIt != aSet->partialMarkings.end(); ++mIt) {
		protectedInsert(*mIt);
	}

}

SetOfPartialMarkings* SetOfPartialMarkings::intersect(
		SetOfPartialMarkings* set1, SetOfPartialMarkings* set2) {

	SetOfPartialMarkings* result = new SetOfPartialMarkings();

	for (std::vector<PartialMarking*>::iterator mIt =
			set1->partialMarkings.begin(); mIt != set1->partialMarkings.end(); ++mIt) {
		for (std::vector<PartialMarking*>::iterator mIt2 =
				set2->partialMarkings.begin(); mIt2
				!= set2->partialMarkings.end(); ++mIt2) {

			PartialMarking* temp = PartialMarking::merge(*mIt, *mIt2);
			if (temp != 0) {
				result->partialMarkings.push_back(temp);
			}
		}
	}

	return result;
}

bool SetOfPartialMarkings::contains(PartialMarking* partialMarking) {

	// Iterate over the vector to find out if the partial marking is contained.
	for (std::vector<PartialMarking*>::iterator mIt = partialMarkings.begin(); mIt
			!= partialMarkings.end(); ++mIt) {
		if ((*mIt)->isEquivalentTo(partialMarking)) {
			return true;
		}
	}
	return false;
}

bool SetOfPartialMarkings::protectedInsert(PartialMarking* partialMarking) {

	// Check if the partial marking is already contained.
	if (contains(partialMarking)) {
		return false;
	}

	// Partial marking is not an element --> insert it.
	partialMarkings.push_back(partialMarking);
	return true;
}

SetOfPartialMarkings* SetOfPartialMarkings::create(pnapi::PetriNet* net,
		const pnapi::formula::Formula* const src, unsigned int bound) {

	// The resulting set.
	SetOfPartialMarkings* result = new SetOfPartialMarkings();

	std::cerr << "Type: " << typeid(*src).name() << std::endl;
	
	// Disjunction means union of the children
	if (typeid(*src) == typeid(pnapi::formula::Disjunction)) {

				const pnapi::formula::Disjunction
						* const castedSrc =
								dynamic_cast<const pnapi::formula::Disjunction* const > (src);

				for (std::set<const pnapi::formula::Formula *>::iterator cIt =
						castedSrc->getChildren().begin(); cIt
						!= castedSrc->getChildren().end(); ++cIt) {
					SetOfPartialMarkings* current = create(net, (*cIt), bound);
					result->add(current);
				}
			}

			// Conjunction means intersection of the partial markings
			else if (typeid(*src) == typeid(pnapi::formula::Conjunction)) {

						const pnapi::formula::Conjunction
								* const castedSrc =
										dynamic_cast<const pnapi::formula::Conjunction* const > (src);
						/*
						 for (std::set<const pnapi::formula::Formula *>::iterator cIt = castedSrc->getChildren().begin(); cIt != castedSrc->getChildren().end(); ++cIt) {
						 SetOfPartialMarkings* current = create((*cIt),bound);
						 result->add(current);
						 }
						 */

						SetOfPartialMarkings* last = create(net,
								*(castedSrc->getChildren().begin()), bound);

						for (std::set<const pnapi::formula::Formula *>::iterator
								cIt = (++castedSrc->getChildren().begin()); cIt
								!= castedSrc->getChildren().end(); ++cIt) {
							SetOfPartialMarkings* current = intersect(last,
									create(net, *cIt, bound));
							delete last;
							last = current;
						}

						result = last;

					}

					// p1 = j means a new partial marking M with M(p1) = j
					else if (typeid(*src) == typeid(pnapi::formula::FormulaEqual)) {

								const pnapi::formula::FormulaEqual
										* const castedSrc =
												dynamic_cast<const pnapi::formula::FormulaEqual* const > (src);

									PartialMarking* m = new PartialMarking();
									m->values.insert(&(castedSrc->getPlace()),
											castedSrc->getTokens(), false);
									result->partialMarkings.push_back(m);
									std::cerr << castedSrc->getPlace().getName() << ": " <<
											castedSrc->getTokens() << std::endl;
							}

							// p1 > j means a new partial marking Mi for each 1 <= i <= bound - j where Mi(p1) = i+j+1
							else if (typeid(*src)
											== typeid(pnapi::formula::FormulaGreater)) {

										const pnapi::formula::FormulaGreater
												* const castedSrc =
														dynamic_cast<const pnapi::formula::FormulaGreater* const > (src);

											for (unsigned int i =
													castedSrc->getTokens() + 1; i
													<= bound; ++i) {
												PartialMarking* m =
														new PartialMarking();
												m->values.insert(
														&(castedSrc->getPlace()),
														i, false);
												result->partialMarkings.push_back(
														m);
											}
									}

									// p1 >= j means a new partial marking Mi for each 0 <= i <= bound - j where Mi(p1) = i+j
									else if (typeid(*src)
													== typeid(pnapi::formula::FormulaGreaterEqual)) {

												const pnapi::formula::FormulaGreaterEqual
														* const castedSrc =
																dynamic_cast<const pnapi::formula::FormulaGreaterEqual* const > (src);
													for (unsigned int i =
															castedSrc->getTokens(); i
															<= bound; ++i) {
														PartialMarking
																* m =
																		new PartialMarking();
														m->values.insert(
																&(castedSrc->getPlace()),
																i, false);
														result->partialMarkings.push_back(
																m);
													}
											}

											// p1 < j means a new partial marking Mi for each 0 <= i <= j-1 where Mi(p1) = i
											else if (typeid(*src)
															== typeid(pnapi::formula::FormulaLess)) {
														const pnapi::formula::FormulaLess
																* const castedSrc =
																		dynamic_cast<const pnapi::formula::FormulaLess* const > (src);
															for (unsigned int
																	i = 0; i
																	< castedSrc->getTokens(); ++i) {
																PartialMarking
																		* m =
																				new PartialMarking();
																m->values.insert(
																		&(castedSrc->getPlace()),
																		i,
																		false);
																result->partialMarkings.push_back(
																		m);
															}
													}

													// p1 <= j means a new partial marking Mi for each 0 <= i <= j where Mi(p1) = i
													else if (typeid(*src)
																	== typeid(pnapi::formula::FormulaLessEqual)) {
																const pnapi::formula::FormulaLessEqual
																		* const castedSrc =
																				dynamic_cast<const pnapi::formula::FormulaLessEqual* const > (src);
																	for (unsigned int
																			i =
																					0; i
																			<= castedSrc->getTokens(); ++i) {
																		PartialMarking
																				* m =
																						new PartialMarking();
																		m->values.insert(
																				&(castedSrc->getPlace()),
																				i,
																				false);
																		result->partialMarkings.push_back(
																				m);
																	}
															}

															// p1 != j means a new partial marking Mi for each 0 <= i < j where Mi(p1) = i and Mk for each 1 <= i <= bound-k where Mi(p1) = i+k
															else if (typeid(*src)
																			== typeid(pnapi::formula::FormulaNotEqual)) {
																		const pnapi::formula::FormulaNotEqual
																				* const castedSrc =
																						dynamic_cast<const pnapi::formula::FormulaNotEqual* const > (src);

																			for (unsigned int
																					i =
																							castedSrc->getTokens(); i
																					>= 0; --i) {
																				PartialMarking
																						* m =
																								new PartialMarking();
																				m->values.insert(
																						&(castedSrc->getPlace()),
																						i,
																						false);
																				result->partialMarkings.push_back(
																						m);
																			}

																			for (unsigned int
																					i =
																							castedSrc->getTokens()
																									+ 1; i
																					<= bound; ++i) {
																				PartialMarking
																						* m =
																								new PartialMarking();
																				m->values.insert(
																						&(castedSrc->getPlace()),
																						i,
																						false);
																				result->partialMarkings.push_back(
																						m);
																			}
																	} else {
																	
																	std::cerr << "wtf?:"  << typeid(*src).name() << std::endl;
																	
																	}

																	return result;

																}
