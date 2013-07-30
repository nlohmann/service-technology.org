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
 * \date    $Date: 2013-07-19 12:00:00 +0200 (Fr, 19. Jul 2013) $
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
	void apply(unsigned int mode, unsigned int tid);

	/// Applicability check of Rule mode for Node node
	bool checkAppl(Vertex node, unsigned int mode);

	enum rulenames { // changing these values will break autotests (see --debug option)
		INITDEADPL2 = 0x1L,
		PARPLACE = 0x2L,
		PARTRANSITION = 0x4L,
		EQUIVPLACE = 0x8L,
		MELDTRANS1 = 0x10L,
		MELDTRANS2 = 0x20L,
		MELDTRANS3 = 0x40L, // deleted
		MELDTRANS4 = 0x80L,
		LOOPPLACE = 0x100L,
		LOOPTRANS = 0x200L,
		ISOLATEDTRANS = 0x400L,
		LIVETRANS = 0x800L,
		MELDTRANS5 = 0x1000L,
		SERIESPLACE = 0x2000L,
		FINALPLACE = 0x4000L,
		FINALTRANS = 0x8000L,

		ALLMODES = 0xFFFFL,
		MAXMODE = 16
	};

private:
	/// Starke rule 1
	void liveTransitions(unsigned int tid);

	/// extended Starke rule 2
	void initiallyDeadPlace2(unsigned int tid);

	/// Starke rule 3(a)
	void parallelPlaces(unsigned int tid);

	/// Starke rule 3(b)
	void parallelTransitions(unsigned int tid);

	/// Starke rule 3(c)
	void isolatedTransitions(unsigned int tid);

	/// Starke rule 4
	void equivalentPlaces(unsigned int tid);

	/// Starke rule 5(a)
	void meldTransitions1(unsigned int tid);

	/// Starke rule 5(b)
	void meldTransitions2(unsigned int tid);

	/// Starke rule 5(c)
//	void meldTransitions3(unsigned int tid);

	/// Starke rule 6
	void meldTransitions4(unsigned int tid);

	/// Starke rule 5(c)
	void meldTransitions5(unsigned int tid);

	/// Starke rule 7
	void loopPlace(unsigned int tid);

	/// Starke rule 7 extended
	void loopPlace2(unsigned int tid);

	/// Starke rule 8
	void loopTransition(unsigned int tid);

	/// Murata's series place rule
	void seriesPlace(unsigned int tid);

	/// remove a place with empty postset
	void finalPlace(unsigned int tid);

	/// remove a transition with empty postset
	void finalTransition(unsigned int tid);

	// Move the preset from one node to another (adding)
	void movePre(Vertex id, Vertex id2);

	// Check if two nodes have the same pre- and postsets
	bool checkParallel(Vertex id1, Vertex id2);

	// Check if two nodes have almost the same pre-/postsets
	Vertex checkParallel(Vertex id1, Vertex id2, Vertex prediff);

	// Get all nodes surrounding a given node
	void collectNeighbors(Vertex id, Map& stamps);

	// Debug info on rule application
	void printApply(Mode mode, Vertex id, const Map& nodestamp);

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


