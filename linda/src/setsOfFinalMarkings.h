#ifndef SETSOFFINALMARKINGS_H_
#define SETSOFFINALMARKINGS_H_

#include <cstdlib>
#include <map>
#include <vector>
#include <pnapi/pnapi.h>
#include "helpers.h"

/*!
 * A marking of a subset of all places.
 */
class PartialMarking {
public:

	BinaryTree<const pnapi::Place*,int> values; //!< The mapping from places to integers.

	/// Returns true iff the marking specifies a value for this place.
	bool defines(const pnapi::Place* place) {return values.find(place) != 0;}


	/// Returns true if this and the given PartialMarking represent the same markings.
	bool isEquivalentTo(PartialMarking* other);

	/// Merges to PartialMarkings.
	static PartialMarking* merge(PartialMarking* m1,PartialMarking* m2);

	/// Puts a string representation of the PartialMarking to a given stream.
	void output(std::ostream&);

	/// Returns a string representation.
	std::string toString();

};

/// A container for partial markings supporting some set operations.
class SetOfPartialMarkings {

public:

	std::vector<PartialMarking*> partialMarkings; //!< The PartialMarkings in the container.

	int size() {
		return partialMarkings.size();
	}

	/// Returns a PartialMarking into the container. Returns true if this PartialMarking was not in the container before.
	bool protectedInsert(PartialMarking* partialMarking);

	/// Returns true, iff the given PartialMarking is in the container.
	bool contains(PartialMarking* partialMarking);

	/// Adds all members of a given SetOfPartialMarkings to this one.
	void add(SetOfPartialMarkings* aSet);

	/// Creates the intersection of two given containers.
	static SetOfPartialMarkings* intersect(SetOfPartialMarkings* set1, SetOfPartialMarkings* set2);

	/// Creates a set of partial markings from the final condition of an open net. Expects a maximum bound for place markings, covering cases like "P1 > 4".
	static SetOfPartialMarkings* create(pnapi::PetriNet* net, const pnapi::formula::Formula* const src, unsigned int bound);

	/// Puts a string representation of the container to cerr.
	void output();

};

#endif

