/*****************************************************************************\
 LoLA -- a Low Level Petri Net Analyzer

 Copyright (C)  1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007,
                2008, 2009  Karsten Wolf <lola@service-technology.org>

 LoLA is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 LoLA is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with LoLA.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/


/// This file organises the internal representation of a Petri net, i.e.
/// Nodes, arcs, Petri net containers, and basic access to a net (enabling, firing,...)
/// as well as output of a net.
/// This file does also contain the main routine and related topics such as
/// command line options.

#pragma once


#include "Node.h"
#include "Place.h"
#include "Transition.h"
#include "Arc.h"

//#include "userconfig.h" // should fix https://gna.org/bugs/?15223


///// 4 LINES ADDED BY NIELS
//extern void statistics(unsigned int s, unsigned int e, unsigned int h);
//extern unsigned int NrOfStates;
//extern unsigned int Edges;
//extern unsigned int NonEmptyHash;


#ifdef MAXIMALSTATES
/// abort LoLA if more than MAXIMALSTATES states are processed
void checkMaximalStates(unsigned int states);
#endif


//class formula; ///< Can hold a state predicate or a temporal logic formula
class overflow {}; ///< a dummy class for handling overflow exceptions


/// contains dfsnum of currently processed transition, used for
/// stubborn.ignorance management which requires depth first search
/// through transitions
//extern unsigned int currentdfsnum;



// TODO: shift to check.h ?
//#ifdef MODELCHECKING
//extern unsigned int formulaindex; ///< in modelchecking,
///< the different subformulas must be treated spearately
//#endif

//extern unsigned int Edges; ///< number of transition occurrences during state space exploration
//extern Transition* LastAttractor;  ///< Last transition in list of
///< static attractor set. Attractor sets form always a prefix in the
///< list of stubborn transitions. Static attractor transitions are never
///< removed from that list.


void readnet(); ///< parse a net(+verification task parameters)
