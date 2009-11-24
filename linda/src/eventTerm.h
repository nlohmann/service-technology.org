#ifndef EVENTTERM_H_
#define EVENTTERM_H_

#include <pnapi/pnapi.h>
#include "helpers.h"
#include <cstdlib>
#include <vector>
#include <set>
#include <ctime>
#include <string>


/*!
 * \brief Parent class for all types of terms.
 * Declares a number of pure virtual and static functions.
 */
class EventTerm {
public:

	/// Returns a string representation of the term.
	virtual std::string toString() = 0;

	/// Multiplies the term with a given integer. Uses distributive law instead of creating a new MultiplyTerm if possible.
	virtual EventTerm* multiplyWith(int) = 0;

	/*!
	 * \brief Transforms a given term to a mapping.
	 * The term is first flattened and then summarized to an integer array,
	 * where the n-th element in the array equals the factor of the n-th event.
	 */
	static int* termToMap(EventTerm*);

	/// First creates a mapping (s. EventTerm::termToMap) and transforms it to a string.
	static std::string toPrettyString(EventTerm*);

	/// Destructs the term, depending on its type.
	virtual ~EventTerm() {}

	/// Flattens a term. Flattening means transforming a term to a normal form.
	virtual EventTerm* flatten() = 0;

	/// Summarizes the factors of terms to an integer array. Used by EventTerm::termToMap(EventTerm*) -- expects a flattened term.
	virtual void collectR(int*) = 0;

};

/// A term of the form (T1 + T2) where T1 and T2 are terms.
class AddTerm : public EventTerm {
public:
	EventTerm* term1; //!< the first addend.
	EventTerm* term2; //!< the second addend.

	/// Creates a new AddTerm from to given addends.
	AddTerm(EventTerm* t1, EventTerm* t2) : term1(t1),term2(t2) {};

	/// Multiplies by multiplying both addends (distributive law) and returns the result. Changes the term itself.
	virtual EventTerm* multiplyWith(int);

	/// Returns a string representation.
	virtual std::string toString();

	/// Flattens the AddTerm by flattening its addends. Changes the term structure.
	virtual EventTerm* flatten();

	/// s. EventTerm::collectR(int*);
	virtual void collectR(int*);

	/// Deletes the addends.
	virtual ~AddTerm();
};

/// A term of the form (k * T) where T is a term.
class MultiplyTerm : public EventTerm {
public:
	EventTerm* term; //!< The term T
	int factor; //!< the integer factor k

	/// Constructs a MultiplyTerm by assigning the given parameters to the fields.
	MultiplyTerm(EventTerm* t, int f) : term(t),factor(f) {};

	/// Multiplies by multiplying the factor (associative law) and returns the result. Changes the term itself.
	virtual EventTerm* multiplyWith(int);

	/// Returns a string representation.
	virtual std::string toString();

	/// Flattens the MultiplyTerm by flattening its underlying term and multiplying the result with the factor. Changes the term structure.
	virtual EventTerm* flatten();

	/// s. EventTerm::collectR(int*);
	virtual void collectR(int*);

	/// Deletes the underlying term.
	virtual ~MultiplyTerm();
};

/// A term of the form (E) where E is an event.
class BasicTerm : public EventTerm {
public:
	int event; //!< The event

	/// Creates a BasicTerm by assigning the event ID that is associated with the given string. Used by the parser.
	BasicTerm(std::string* s) : event(LindaHelpers::getEventID(s)) {
		if (event == -1) {
			std::cerr << PACKAGE << ": An unspecified event label has been used: \"" << *s <<  "\" ! Cancel." << std::endl;
			exit(1);
		}
	}

	/// Creates a BasicTerm from a known event ID.
	BasicTerm(int i) : event(i) {	}

	/// Returns a string representation.
	virtual std::string toString();

	/// Multiplies the BasicTerm by creating a new MultiplyTerm with the given factor.
	virtual EventTerm* multiplyWith(int);

	/// Flattens the BasicTerm by creating a new MultiplyTerm with a factor of 1.
	virtual EventTerm* flatten();

	/// s. EventTerm::collectR(int*);
	virtual void collectR(int*)  {};

	/// Does nothing since nothing is to be deleted.
	~BasicTerm();
};
/*
class RawTerm {
public:
	RawTerm() {
		data = new BinaryTree<int,int>();
	}
	void setValue(int id, int newValue);
	void addValue(int id, int addend);
	int getValue(int id);
private:
	BinaryTree<int,int>* data;
};
*/

/// A record containing a lower and an upper bound, including unboundedness in one or both directions.
class EventTermBound {
public:

	/// Merges several event bounds by finding the minimum lower bound and the maximum upper bound.
	static EventTermBound* merge(EventTermBound* termArray, unsigned int size);

	bool lowerBounded; //!< True, if there exists a lower bound.
	bool upperBounded; //!< True, if there exists an upper bound.
	int lowerBound; //!< An integer lower bound.
	int upperBound; //!< An integer upper bound.

	/// Creates a record, assigning lower and upper bounds if they exist.
	EventTermBound(bool lBounded,bool uBounded,int lBound,int uBound) : lowerBounded(lBounded), lowerBound(lBound), upperBounded(uBounded), upperBound(uBound) {}

	/// Creates a record that represents the interval with neither a lower nor an upper bound.
	EventTermBound() {lowerBounded = false; upperBounded = false;}

	/// Compares these bounds with others, checking if the intervals overlap.
	bool intersectionEmpty (EventTermBound* other);

	/// Compares the bounds with others, checking if the others are included in these.
	bool contains(EventTermBound* other);

	/// Returns true, if the lower bound and the upper bound both exist and are identical.
	bool isDecided ();

	/// Outputs the bounds, together with a given term, to std::cerr.
	void output (EventTerm* term, bool as_given);

	/// Returns a string representation for the lower bound: Either "unbounded" or the integer.
	std::string getLowerBoundString();

	/// Returns a string representation for the upper bound: Either "unbounded" or the integer.
	std::string getUpperBoundString();

};

/// A constraint, consisting of lower and upper bounds for an event term.
class EventTermConstraint {
public:
	EventTermBound* vals; //!< The specified lower and upper bound
	EventTerm* e; //!< The event term that is subject of this constraint

	/// Basic constructor, assigning the fields.
	EventTermConstraint(EventTerm* e, EventTermBound* vals);

	/// Getter function for the event term.
	EventTerm* getEventTerm();

	/// Compares this constraint with a lower and an upper bound, checking for a contradiction (= their intervals do not overlap).
	bool contradicts(EventTermBound* toCheck);

	static const int is_true = 1; //!< The constraint could be proven.
	static const int is_false = 0; //!< The negation of the constraint could be proven.
	static const int is_maybe = -1; //!< Neither the constraint nor its negation could be proven.


	/// Tries to prove that the constraint holds by comparing the bounds with given ones.
	unsigned int holds(EventTermBound* toCheck);

	/// Returns a string representation.
	std::string toString();
};


#endif /* EVENTTERM_H_ */
