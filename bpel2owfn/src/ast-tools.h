/*****************************************************************************\
 * Copyright 2005, 2006 Niels Lohmann, Christian Gierds                      *
 *                                                                           *
 * This file is part of GNU BPEL2oWFN.                                       *
 *                                                                           *
 * GNU BPEL2oWFN is free software; you can redistribute it and/or modify it  *
 * under the terms of the GNU General Public License as published by the     *
 * Free Software Foundation; either version 2 of the License, or (at your    *
 * option) any later version.                                                *
 *                                                                           *
 * GNU BPEL2oWFN is distributed in the hope that it will be useful, but      *
 * WITHOUT ANY WARRANTY; without even the implied warranty of                *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General  *
 * Public License for more details.                                          *
 *                                                                           *
 * You should have received a copy of the GNU General Public License along   *
 * with GNU BPEL2oWFN; see file COPYING. if not, write to the Free Software  *
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA. *
\*****************************************************************************/

/*!
 * \file    ast-tools.h
 *
 * \brief   unparse helper tools
 *
 * \author  Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          Christian Gierds <gierds@informatik.hu-berlin.de>,
 *          last changes of: \$Author: gierds $
 *
 * \since   2006/02/08
 *
 * \date    \$Date: 2007/01/17 10:17:23 $
 *
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \version \$Revision: 1.26 $
 *
 * \ingroup debug
 * \ingroup creation
 */





#ifndef AST_TOOLS_H
#define AST_TOOLS_H





/******************************************************************************
 * Headers
 *****************************************************************************/

#include <string>
#include <map>
#include <set>
#include <vector>

#include "petrinet.h"
#include "debug.h"
#include "ast-details.h"

using std::string;
using std::vector;
using std::set;





/*!
 * \brief enumaration of possible relationship between activities
 *
 * Possible answers to the question: What is the relation of A to B?
 */
typedef enum
{
  AR_CONCURRENT,        ///< to activities are concurrent
  AR_CONFLICT,          ///< to activities are conflict (like in switch)
  AR_ENCLOSES,          ///< activity B is enclosed in activity A
  AR_DESCENDS,          ///< activity B is a descendant of activity A
  AR_BEFORE,            ///< activity A comes before activity B
  AR_AFTER
} activityRelationType;





/******************************************************************************
 * Functions for the Petri net unparser defined in bpel-unparse-petri.k
 *****************************************************************************/

Transition *throwFault(Place *p1, Place *p2,
    string p1name, string prefix, kc::integer id,
    int negativeControlFlow, bool preventFurtherFaults = true);
Transition *stop(Place *p, string p_name, string prefix);
void dpeLinks(Transition* t, int id);
string inString();
void header(kc::integer id, bool indent = false);
void header(int id, bool indent = false);
void footer(kc::integer id, bool indent = false);
void footer(int id, bool indent = false);


activityRelationType activityRelation( unsigned int a, unsigned int b );
void conflictingActivities( unsigned int a, unsigned int b );
void enterConflictingActivities( set< unsigned int > a, set< unsigned int > b );
void enclosedActivities( unsigned int a, unsigned int b );
void enterEnclosedActivities( unsigned int a, set< unsigned int > b );


/******************************************************************************
 * Functions for checking SA00070 and SA00071
 *****************************************************************************/

void check_SA00070( unsigned int id );



/******************************************************************************
 * Functions for the XML (pretty) unparser defined in bpel-unparse-xml.k
 *****************************************************************************/

void in();
void inup();
void indown();
void listAttributes ( unsigned int id );

/*!
 * \todo this function should not be inline -- this is the compiler's work!
 */
inline void listAttributes ( kc::integer id )
{
    listAttributes ( id->value );
}



/// comment me!
void process_loop_bounds(const vector<unsigned int> &loop_bounds, const vector<unsigned int> &loop_identifiers, string prefix, unsigned int my_max);





#endif


/*!
 * \defgroup creation Petri Net Creation Module
 */
