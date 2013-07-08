// -*- C++ -*-

/*!
 * \file    rules.cc
 *
 * \brief   Class Rules
 *
 * \author  Harro Wimmel <harro.wimmel@uni-rostock.de>
 *
 * \since   2013-05-21
 *
 * \date    $Date: 2013-05-21 12:00:00 +0200 (Di, 21. May 2013) $
 *
 * \version $Revision: 1.00 $
 */

#ifndef RULES_H
#define RULES_H

#include <set>
#include <map>
#include "imatrix.h"
#include "facts.h"
#include <pthread.h>

using std::set;
using std::map;
using std::vector;
using std::cerr;
using std::endl;

/*! \brief Reduction Rules
	
	Class containing the reduction rules for Petri nets as well as a general call by id. 
*/
class Rules {

public:
	/// Constructor
	Rules(IMatrix& im, Facts& facts);

	/// Call of a reduction rule by its mode number
	void apply(unsigned int mode);

	/// Applicability check of Rule mode for Node node
	bool checkAppl(Vertex node, unsigned int mode);

	enum rulenames { // changing these values will break autotests (see --debug option)
		INITDEADPLACE = 0x0L, // deleted
		INITDEADPL2 = 0x1L,
		PARPLACE = 0x2L,
		PARTRANSITION = 0x4L,
		EQUIVPLACE = 0x8L,
		MELDTRANS1 = 0x10L,
		MELDTRANS2 = 0x20L,
		MELDTRANS3 = 0x40L,
		MELDTRANS4 = 0x80L,
		LOOPPLACE = 0x100L,
		LOOPTRANS = 0x200L,
		MELDPLACE = 0x400L,
		LIVETRANS = 0x800L,
		MELDTRANS5 = 0x1000L,
		ISOLATEDTRANS = 0x2000L,
		SERIESPLACE = 0x4000L,
		FINALPLACE = 0x8000L,
		FINALTRANS = 0x10000L,

		ALLMODES = 0x1FFFFL,
		MAXMODE = 17
	};

private:
	/// Starke rule 1
	void liveTransitions(Mode mode);

	/// Starke rule 2
	void initiallyDeadPlace(Mode mode);

	/// extended Starke rule 2
	void initiallyDeadPlace2(Mode mode);

	/// Starke rule 3(a)
	void parallelPlaces(Mode mode);

	/// Starke rule 3(b)
	void parallelTransitions(Mode mode);

	/// Starke rule 3(c)
	void isolatedTransitions(Mode mode);

	/// Starke rule 4
	void equivalentPlaces(Mode mode);

	/// Starke rule 5(a)
	void meldTransitions1(Mode mode);

	/// Starke rule 5(b)
	void meldTransitions2(Mode mode);

	/// Starke rule 5(c)
	void meldTransitions3(Mode mode);

	/// Starke rule 6
	void meldTransitions4(Mode mode);

	/// Starke rule 5(d)
	void meldTransitions5(Mode mode);

	/// Starke rule 7
	void loopPlace(Mode mode);

	/// Starke rule 8
	void loopTransition(Mode mode);

	/// Starke rule 9
	void meldPlaces(Mode mode);

	/// Murata's series place rule
	void seriesPlace(Mode mode);

	/// remove a place with empty postset
	void finalPlace(Mode mode);

	/// remove a transition with empty postset
	void finalTransition(Mode mode);

	/// Mark surroundings of nodes to be rechecked
	void propagateChange(Map& stamps);

	// Move the preset from one node to another (adding)
	void movePre(Vertex id, Vertex id2);

	// Check if two nodes have the same pre- and postsets
	bool checkParallel(Vertex id1, Vertex id2);

	// Check if two nodes have almost the same pre-/postsets
	Vertex checkParallel(Vertex id1, Vertex id2, Vertex prediff);

	// Get all nodes surrounding a given node
	void collectNeighbors(Vertex id, Map& stamps);

	// Debug info on rule application
	void printApply(uint32_t mode, Vertex id, const Map& nodestamp);

	// Make sure two edge maps have no common nodes
	bool disjoint(const Map& map1, const Map& map2);

	// Check if an edge map contains another (i.e. with same or less edge weights)
	bool contains(const Map& map1, const Map& map2);


	// Incidence matrix related stuff
	IMatrix& im;

	// Property related stuff
	Facts& facts;
};

#endif


