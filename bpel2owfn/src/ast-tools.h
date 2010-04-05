/*****************************************************************************\
  GNU BPEL2oWFN -- Translating BPEL Processes into Petri Net Models

  Copyright (C) 2009        Niels Lohmann
  Copyright (C) 2006, 2007  Niels Lohmann,
                            Christian Gierds, and
                            Martin Znamirowski
  Copyright (C) 2005        Niels Lohmann and
                            Christian Gierds

  GNU BPEL2oWFN is free software: you can redistribute it and/or modify it
  under the terms of the GNU Affero General Public License as published by the
  Free Software Foundation, either version 3 of the License, or (at your
  option) any later version.

  GNU BPEL2oWFN is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
  FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more
  details.

  You should have received a copy of the GNU Affero General Public License
  along with GNU BPEL2oWFN. If not, see <http://www.gnu.org/licenses/>.
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
 * \date    \$Date: 2007/09/04 13:47:12 $
 *
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \version \$Revision: 1.35 $
 *
 * \todo    Comment me!
 *
 * \ingroup debug
 */





#ifndef AST_TOOLS_H
#define AST_TOOLS_H





/******************************************************************************
 * Headers
 *****************************************************************************/

#include <config.h>
#include <string>
#include <map>
#include <set>
#include <vector>

using std::string;
using std::vector;
using std::set;

class PetriNet;





/*!
 * \brief enumaration of possible relationship between activities
 *
 * Possible answers to the question: What is the relation of A to B?
 */
typedef enum {
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

string inString();
void header(kc::integer id, bool indent = false);
void header(int id, bool indent = false);
void footer(kc::integer id, bool indent = false);
void footer(int id, bool indent = false);


activityRelationType activityRelation(unsigned int a, unsigned int b);
void conflictingActivities(unsigned int a, unsigned int b);
void enterConflictingActivities(set< unsigned int > a, set< unsigned int > b);
void enclosedActivities(unsigned int a, unsigned int b);
void enterEnclosedActivities(unsigned int a, set< unsigned int > b);
void consecutiveActivities(unsigned int a, unsigned int b);
void enterConsecutiveActivities(unsigned int a, unsigned int b);
void resetActivityRelations();


/// increases the index vector
void next_index(vector<unsigned int> &current_index, const vector<unsigned int> &max_index);



/******************************************************************************
 * Functions for checking SA00070 and SA00071
 *****************************************************************************/

void check_SA00070(unsigned int id);
void check_SA00071(unsigned int id);


/******************************************************************************
 * Functions for the XML (pretty) unparser defined in bpel-unparse-xml.k
 *****************************************************************************/

void in();
void inup();
void indown();
void listAttributes(unsigned int id);
void listAttributes(kc::integer id);

/// comment me!
void process_loop_bounds(const vector<unsigned int> &loop_bounds, const vector<unsigned int> &loop_identifiers, string prefix, unsigned int my_max);





#endif
