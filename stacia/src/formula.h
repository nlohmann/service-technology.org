
/*!
 * \file    formula.h
 *
 * \brief   Class Formula
 *
 * \author  Harro Wimmel <harro.wimmel@uni-rostock.de>
 *
 * \since   2012/07/24
 *
 * \date    $Date: 2012-11-20 14:47:01 +0200 (Di, 20. Nov 2012) $
 *
 * \version $Revision: 1.01 $
 */

#ifndef FORMULA_H
#define FORMULA_H

// include header files
#include <config.h>
#include <libgen.h>
#include <string>

#include "types.h"
#include "subnet.h"
#include "subnetinfo.h"
#include "imatrix.h"
#include "matchings.h"
#include "config-log.h"
#include "verbose.h"

class Formula {

public:

	/// Constructor via matchings for incremental checks
	Formula(const Matchings& m);

	/// Constructor via net components for full checks
	Formula(const SubNet& sn);

	/// Print the formula
	void print() const;
	
	/// Print the outcome
	void printResult() const;

	/// Check the formula
	bool check(bool dummy);

	/// Get the corresponding siphon for a solution
	Siphon getSiphon(bool base);

private:
	// Create the formula for closed siphons
	void createSiphonFormula(const Matchings& m);

	/// Create the formula for checking for token traps inside siphons
	void createMatchingFormula(const Matchings& m);

	/// Create the formula for all siphons
	void createSiphonFormula(const SubNet& sn);

	/// Create the formula for non-empty sets (siphons)
	void createNonEmptyFormula();

	/// Create the formula for unmarked maximal traps in siphons
	void createMaxTrapFormula(const SubNet& sn);

	/// The formula in CNF
	vector<vector<Literal> > f;

	/// The number of variables occurring in the formula
	unsigned int maxvarnum;

	/// Names for the variables
	map<Literal,string> names;

	/// Names for siphons
	map<ExtSiphonID,string> siphonname;

	/// Mapping from siphons to variables
	map<ExtSiphonID,Literal> svarnum;

	/// Mappings from names to traps
	map<string,ExtTrapID> nametoiif, nametotif;

	/// Copy of MiniSats assignment after the formula has been solved
	vector<bool> mssolution;

	/// If the formula was satisfiable
	bool satisfied;
};

#endif

