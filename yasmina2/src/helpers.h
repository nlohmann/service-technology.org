#ifndef YASMINA_HELPERS_H
#define YASMINA_HELPERS_H

#include <sstream>
#include <iostream>
#include <fstream>
#include <set>
#include <vector>
#include <typeinfo>

#include "config.h"
#include "verbose.h"
#include <pnapi/pnapi.h>

#include <lp_solve/lp_lib.h>

/*
Classes and functions that are commonly used throughout Yasmina.
 */

// Given an unsigned integer representing a bit field, returns the bit on the given position. 
unsigned int getBit(unsigned int field, unsigned int position);
// Transforms an int to a string
std::string intToStr(int i);

// Provides static members to store and find identifiers.
// An identifier is a transition or channel name.
// Each identifier has a unique numerical id.
// All constraints use the numerical ids to reference on identifiers.
// Numerical ids start at 1 for more convenient lp_solve-integration. 
// Therefore, the highest numerical id coincides with the number of identifiers.
class Universe {
  // stores the identifiers together with their numIDs. Adding and looking up the numIDs is fast. 
  // Finding an identifier for a given numID is slow.
  static std::map<std::string, int> identifiers;
public:
  // Adds an identifier to the universe and returns its numID.
  // If this identifier already exists, it is not added again and keeps its numID.
  static int addIdentifier (std::string* identifier);
  // Looks up if an identifier with this numID can be found and, if yes, returns it. If not, it returns the empty string.
  // Rather slow.
  static std::string getIdentifier (int numID);  
  // Looks up the num ID for an identifier.
  static int getNumID(std::string* identifier);
  
  // Returns the size of the universe.
  static int getUniverseSize();  
  
};

// Abstract constraint over some domain. 
// A constraint is a term, possibly together with bounds.
// The domain is an array of integers which are numerical ids of identifiers.
// The term is specified by an array of REAL. values[i] = x means that domain[i] has value x in this term.
// A constraint has a virtual member function responsible for adding the constraint to a given lp system.
class Constraint {
protected:
  int* domain;
  REAL* values;
  int domainsize;
public:
  // Adds this constraint to an lp-system and returns the number of lp constraints added.
  virtual int addToLP(lprec* lp) = 0;
};

// Custom constraint, having a specific operator that compares the term with a rhs value
// Results in one lp constraint.
class CustomConstraint : public Constraint {
  REAL rhs;
  int op;
public:
  int addToLP(lprec* lp);
  CustomConstraint(std::vector<int>* aDomain, std::vector<int>* someValues, int op, int rhs);
};

// A left and right bounded constraint.
// Results in two lp constraints.
class BoundedConstraint : public Constraint {
  REAL lBound, rBound;
public:
  int addToLP(lprec* lp);
  BoundedConstraint(std::vector<int>* identifiers, std::vector<int>* values, int aLeftBound, int aRightBound);
};

// A left bounded constraint.
// Results in one lp constraint.
class LeftBoundedConstraint : public Constraint {
  REAL bound;
public:
  int addToLP(lprec* lp);
  LeftBoundedConstraint(std::vector<int>* identifiers, std::vector<int>* values, int aBound);
};

// A right bounded constraint.
// Results in one lp constraint.
class RightBoundedConstraint : public Constraint {
  REAL bound;
public:
  int addToLP(lprec* lp);
  RightBoundedConstraint(std::vector<int>* identifiers, std::vector<int>* values, int aBound);
};

// A unbounded constraint which is equal to the empty constraint, or: true.
// Results in no lp constraints.
class UnboundedConstraint : public Constraint {
public:
  int addToLP(lprec* lp) {;}
  UnboundedConstraint(std::vector<int>* aDomain, std::vector<int>* someValues);
};

// A service model is a propositional formula having constraitns as propositions.
// It is given in disjunctive normal form without negations.
// Its domain, i.e. the identifiers occurring in all its constraitns, is split in two: inputs and outputs.
class ServiceModel {
public:
  // inputs domain
  std::vector<int> inputs;
  // outputs domain
  std::vector<int> outputs;
  // a name for this service model
  std::string name;
  // an integer alias for this service model for easier referencing in output (files)
  int alias;
  // The formula, given in dnf: constraints[x][y] is the y-th constraint in the x-th conjunctive clause
  std::vector<std::vector<Constraint*> > constraints;
};

class StateEquation : public ServiceModel {
private: 
  std::vector<std::map<pnapi::Place*, std::pair<int,int> > > translateFinalCondition(pnapi::PetriNet* net);
public:
  StateEquation(pnapi::PetriNet* net);

};

// A result of the compatibility check of a series of models.
// Stores the models and the result.
// Can be converted to a string.
class Result {
public:
  // Models analyzed - their id corresponds to that in the YasminaLog.
  std::vector<int>* models;
  // The actual result.
  int result; // 0 = inconclusive, 1 = incompatible, -1 = syntactically incompatible
  // A constructor, simply setting the members.
  Result(std::vector<int>* composite, int res);  
  // Convert this result to a string. 
  // Has the general form M1 (+) ... (+) Mn: maybe|no|undefined
  // undefined indicates that the composition of the models is not well-defined
  std::string* toString();      
};

// A log storing the analysis results.
class YasminaLog {
public:
  // All models occuring.
  static std::vector<ServiceModel*> models;
  // All results.
  static std::vector<Result> results;
  // Composes a string of this log. 
  // By default does not include lines for syntactically incompatible composites.
  // Contains of a declaration of all models and the string representations of the results.
  static std::string* toString(bool reportSyntacticallyIncompatible = false);
};





#endif /* YASMINA_HELPERS_H */
