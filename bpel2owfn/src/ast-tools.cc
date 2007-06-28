/*****************************************************************************\
  GNU BPEL2oWFN -- Translating BPEL Processes into Petri Net Models

  Copyright (C) 2006, 2007  Niels Lohmann,
                            Christian Gierds, and
                            Martin Znamirowski
  Copyright (C) 2005        Niels Lohmann and
			    Christian Gierds

  GNU BPEL2oWFN is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the Free
  Software Foundation; either version 3 of the License, or (at your option) any
  later version.

  GNU BPEL2oWFN is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
  details.

  You should have received a copy of the GNU General Public License along with
  GNU BPEL2oWFN (see file COPYING); if not, see http://www.gnu.org/licenses
  or write to the Free Software Foundation,Inc., 51 Franklin Street, Fifth
  Floor, Boston, MA 02110-1301  USA.
\*****************************************************************************/

/*!
 * \file    ast-tools.cc
 *
 * \brief   unparse helper tools
 *
 * \author  Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          Christian Gierds <gierds@informatik.hu-berlin.de>,
 *          last changes of: \$Author: nielslohmann $
 *
 * \since   2006/02/08
 *
 * \date    \$Date: 2007/06/28 07:38:15 $
 *
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \version \$Revision: 1.72 $
 *
 * \ingroup debug
 * \ingroup creation
 */





/******************************************************************************
 * Headers
 *****************************************************************************/

#include <cmath>
#include <cassert>
#include <utility>

#include "options.h"
#include "helpers.h"
#include "ast-details.h"
#include "ast-tools.h"
#include "globals.h"
#include "debug.h"
#include "petrinet.h"

using std::pair;





/******************************************************************************
 * Global variables
 *****************************************************************************/

/// number of spaces to be added as indention
unsigned int indent = 0;

/// number of spaces to be added when indention is increased
unsigned int indentStep = 2;










/*!
 * \brief   returns a string used for identation.
 * \ingroup debug
 */
string inString()
{
  string result = "";

  for(unsigned int i=0; i<indent; i++)
    result += " ";

  return result;
}





/*!
 * \brief prints name of activity that is unparsed
 *
 * Prints a debug trace message containing the (opening) tag name of the
 * activity of the given id. If myindent is set to true, the enclosed
 * elements are indented.
 *
 * \see #footer
 *
 * \ingroup debug
*/
void header(int id, bool myindent)
{
  trace(TRACE_DEBUG, "[PNU]" + inString() + "<" + globals::ASTEmap[id]->activityTypeName() + " id=" + toString(id) + ">\n");

  if (myindent)
    indent += indentStep;
}





/*!
 * \brief prints name of activity that is unparsed
 * \overload
 * \see header(int id, bool myindent)
 * \ingroup debug
*/
void header(kc::integer id, bool myindent)
{
  assert(id!=NULL);
  header(id->value, myindent);
}





/*!
 * \brief prints name of activity that is unparsed
 *
 * Prints a debug trace message containing the (closing) tag name of the
 * activity of the given id. If myindent is set to true, the indentation is
 * reduced.
 *
 * \see #header

 * \ingroup debug
 */
void footer(int id, bool myindent)
{
  if (myindent)
    indent -= indentStep;

  trace(TRACE_DEBUG, "[PNU]" + inString() + "</" + globals::ASTEmap[id]->activityTypeName() + " id=" + toString(id) + ">\n");
}





/*!
 * \brief prints name of activity that is unparsed
 * \overload
 * \see footer(int id, bool myindent)
 * \ingroup debug
 */
void footer(kc::integer id, bool myindent)
{
  assert(id != NULL);
  footer(id->value, myindent);
}





/*!
 * \param loop_bounds	    a vector holding the maximal indices of the ancestor loops
 * \param loop_identifiers  a vector holding the identifiers of the ancestor loops
 * \param prefix            the prefix of the calling <scope>
 * \param my_max	    the maximal index of the direct parent loop
 */
void process_loop_bounds(const vector<unsigned int> &loop_bounds, const vector<unsigned int> &loop_identifiers, string prefix, unsigned int my_max)
{
  extern PetriNet PN;	// introduced in main.c

  vector<unsigned int> current_index(loop_bounds.size(), 1);
  unsigned int number = 1;

  // count the possible permutations of indices
  for (unsigned int i = 0; i < loop_bounds.size(); i++)
    number *= loop_bounds[i];

  // create transitions, places and arcs
  for (unsigned int i = 1; i <= number; i++)
  {
    Place *p1 = PN.newPlace(prefix + "c" + toString(current_index));
    Place *p2 = PN.newPlace(prefix + "!c" + toString(current_index));
    p2->mark(my_max);

    Transition *t14 = PN.newTransition(prefix + "t14." + toString(i));
    PN.newArc(PN.findPlace(prefix + "final1"), t14);
    PN.newArc(PN.findPlace(prefix + "!Successful"), t14, READ);
    PN.newArc(t14, PN.findPlace(prefix + "final"));

    Transition *t15 = PN.newTransition(prefix + "t15." + toString(i));
    PN.newArc(t15, p1);
    PN.newArc(p2, t15);
    PN.newArc(PN.findPlace(prefix + "final1"), t15);
    PN.newArc(PN.findPlace(prefix + "Successful"), t15);
    PN.newArc(t15, PN.findPlace(prefix + "!Successful"));
    PN.newArc(t15, PN.findPlace(prefix + "final"));

    Transition *t16 = PN.newTransition(prefix + "t16." + toString(i));
    PN.newArc(p2, t16, READ, my_max);
    PN.newArc(PN.findPlace(prefix + "compensated1"), t16);
    PN.newArc(t16, PN.findPlace(prefix + "compensated"));

    Transition *t17 = PN.newTransition(prefix + "t17." + toString(i));
    PN.newArc(p1, t17);
    PN.newArc(t17, p2);
    PN.newArc(PN.findPlace(prefix + "compensate"), t17);
    PN.newArc(t17, PN.findPlace(prefix + "ch_initial"));

    PN.mergePlaces(prefix + "compensated1", prefix + "compensate");

    // connect transitions with counters of ancestor loops
    for (unsigned i = 0; i < loop_identifiers.size(); i++)
    {
      Place *p3 = PN.findPlace(toString(loop_identifiers[i]) + ".internal.count." + toString(current_index[i]));
      assert(p3 != NULL);

      PN.newArc(p3, t15, READ);
      PN.newArc(p3, t16, READ);
      PN.newArc(p3, t17, READ);
    }

    // generate next index
    next_index(current_index, loop_bounds);
  }
}





/******************************************************************************
 * ACTIVITY RELATIONS
 *****************************************************************************/

namespace {
    map< pair< unsigned int, unsigned int >, activityRelationType > activityRelationMap;
}


/*!
 * \brief Returns the relationship between to activities.
 *
 * Two activities are always in some kind of relationship. Either conflicting, concurrent, or consecutively.
 * This method returns this relationship as analyzed in postprocessing.
 *
 * \param a	the id of the first activity
 * \param b	the id of the second activity
 * \returns     the relationship of a and b
 *
 * \ingroup creation
 */
activityRelationType activityRelation(unsigned int a, unsigned int b)
{
    ENTER("activityRelation");
    return activityRelationMap[pair<unsigned int,unsigned int>(a,b)];
    LEAVE("activityRelation");
}

/*!
 * \brief Defines two activities as conflicting.
 *
 * \param a	the id of the first activity
 * \param b	the id of the second activity
 *
 * \ingroup creation
 */
void conflictingActivities(unsigned int a, unsigned int b)
{
    ENTER("conflictingActivities");
    activityRelationMap[pair<unsigned int, unsigned int>(a,b)] = AR_CONFLICT;
    activityRelationMap[pair<unsigned int, unsigned int>(b,a)] = AR_CONFLICT;
    LEAVE("conflictingActivities");
}

/*!
 * \brief For two sets of activities define each pair of an a and a b activity as conflicting.
 *
 * \param a	the first set activity ids
 * \param b	the second set activity ids
 *
 * \ingroup creation
 */
void enterConflictingActivities( set< unsigned int > a, set< unsigned int > b )
{
  ENTER("enterConflictingActivities");
  for ( set< unsigned int >::iterator id1 = a.begin();
        id1 != a.end();
        id1++ )
  {
    for ( set< unsigned int >::iterator id2 = b.begin();
          id2 != b.end();
          id2++ )
    {
      conflictingActivities( *id1, *id2 );
    }
  }
  LEAVE("enterConflictingActivities");
}

/*!
 * \brief Defines two activities as enclosed (a encloses b).
 *
 * \param a	the id of the first activity
 * \param b	the id of the second activity
 *
 * \ingroup creation
 */
void enclosedActivities( unsigned int a, unsigned int b )
{
    ENTER("enclosedActivities");
    activityRelationMap[pair<unsigned int, unsigned int>(a,b)] = AR_ENCLOSES;
    activityRelationMap[pair<unsigned int, unsigned int>(b,a)] = AR_DESCENDS;
    LEAVE("enclosedActivities");
}

/*!
 * \brief For two sets of activities define each pair of an a and a b activity as enclosed.
 *
 * \param a	the first set activity ids
 * \param b	the second set activity ids
 *
 * \ingroup creation
 */
void enterEnclosedActivities( unsigned int a, set< unsigned int > b )
{
  ENTER("enterEnclosedActivities");
  for ( set< unsigned int >::iterator id = b.begin();
        id != b.end();
        id++ )
  {
    enclosedActivities( a, *id );
  }
  LEAVE("enterEnclosedActivities");
}

/*!
 * \brief Defines two activities as consecutive (a before b).
 *
 * \param a	the id of the first activity
 * \param b	the id of the second activity
 *
 * \ingroup creation
 */
void consecutiveActivities( unsigned int a, unsigned int b )
{
    ENTER("consecutiveActivities");
    activityRelationMap[pair<unsigned int, unsigned int>(a,b)] = AR_BEFORE;
    activityRelationMap[pair<unsigned int, unsigned int>(b,a)] = AR_AFTER;
    LEAVE("consecutiveActivities");
}

/*!
 * \brief For two sets of activities define each pair of an a and a b activity as consecutive (a before b).
 *
 * \param a	the first set activity ids
 * \param b	the second set activity ids
 *
 * \ingroup creation
 */
void enterConsecutiveActivities( unsigned int a, unsigned int b )
{
  ENTER("enterConsecutiveActivities");
  consecutiveActivities( a, b );
  LEAVE("enterConsecutiveActivities");
}

/******************************************************************************
 * Functions for checking SA00070 and SA00071
 *****************************************************************************/

/*!
 * \brief Checks for Static Analysis item SA00070.
 *
 * \param id  the id of the item to be checked
 *
 */
void check_SA00070( unsigned int id )
{
  ENTER("check_SA00070");
  
  // go through all enclosed activities and look if there have sources or targets
  for ( set< unsigned int >::iterator activities = globals::ASTEmap[ id ]->enclosedActivities.begin();
        activities != globals::ASTEmap[ id ]->enclosedActivities.end();
        activities++ )
  {
    // whether it's source or target doesn't matter - the link must be defined inside the element that is checked
    set< unsigned int > links = setUnion( globals::ASTEmap[ *activities ]->sourceLinks, globals::ASTEmap[ *activities ]->targetLinks );
    
    // so check every source and target
    for ( set< unsigned int>::iterator link = links.begin(); link != links.end(); link++ )
    {
      // get the id of the <link> element
      unsigned int linkId = globals::ASTE_linkIdMap[ globals::ASTEmap[ *link ]->linkName ];
      
      // look if the linkId is inside the set of enclosed activities
      bool internal = false;
      for ( set< unsigned int >::iterator activity = globals::ASTEmap[ id ]->enclosedActivities.begin();
            activity != globals::ASTEmap[ id ]->enclosedActivities.end();
            activity++ )
      {
        // is this the right id?
        if ( *activity == linkId )
        {
          // yeah, everything is fine
          internal = true;
        }
      }
      // the linkId doesn't belong to the set of enclosed activities, so trigger the error
      if ( !internal )
      {
        SAerror( 70, globals::ASTEmap[ *link ]->linkName, globals::ASTEmap[ id ]->attributes[ "referenceLine" ] );
      }
    }
  }
  LEAVE("check_SA00070");
}





/*!
 * \brief Checks for Static Analysis item SA00071.
 *
 * \param id  the id of the item to be checked
 *
 */
void check_SA00071( unsigned int id )
{
  ENTER("check_SA00071");

  for ( set< unsigned int >::iterator activities = globals::ASTEmap[ id ]->enclosedActivities.begin();
          activities != globals::ASTEmap[ id ]->enclosedActivities.end();
          activities++ )
  {
    // whether it's source or target doesn't matter - the link must be defined inside the element that is checked
    set< unsigned int > targets = globals::ASTEmap[ *activities ]->targetLinks;

    // so check every source and target
    for ( set< unsigned int>::iterator link = targets.begin(); link != targets.end(); link++ )
    {
      // get the id of the <link> element
      unsigned int linkId = globals::ASTE_linkIdMap[ globals::ASTEmap[ *link ]->linkName ];
      
      // look if the linkId is inside the set of enclosed activities
      bool internal = false;
      for ( set< unsigned int >::iterator activity = globals::ASTEmap[ id ]->enclosedActivities.begin();
            activity != globals::ASTEmap[ id ]->enclosedActivities.end();
            activity++ )
      {
        set< unsigned int > sources = globals::ASTEmap[ *activity ]->enclosedSourceLinks;

        // is this the right id?
        if ( sources.find( linkId ) != sources.end() )
        {
          // yeah, everything is fine
          internal = true;
        }
      }
      // the linkId doesn't belong to the set of enclosed activities, so trigger the error
      if ( !internal )
      {
        SAerror( 71, globals::ASTEmap[ linkId ]->linkName, globals::ASTEmap[ id ]->attributes[ "referenceLine" ] );
      }
    }
  }

  LEAVE("check_SA00071");
}





/******************************************************************************
 * Functions for the XML (pretty) unparser defined in bpel-unparse-xml.k
 *****************************************************************************/

/*!
 * \brief adds whitespace to indent output
 * \see #inup #indown
 * \ingroup debug
 */
void in()
{
  for(unsigned int i=0; i<indent; i++)
    *output << " ";
}





/*!
 * \brief increases indention#
 * \see #indown #in
 * \ingroup debug
 */
void inup()
{
  in();
  indent += indentStep;
}





/*!
 * \brief decreases indention
 * \see #inup #in
 * \ingroup debug
 */
void indown()
{
  indent -= indentStep;
  in();
}





/*!
 * \brief Lists attributes for use in pretty printer mode
 *
 * \param id  the id of the activity, whose attributes should be printed
 *
 */
void listAttributes ( unsigned int id )
{
  string result = "";
  for (map< string, string >::iterator attribute = globals::ASTEmap[ id ]->attributes.begin(); attribute != globals::ASTEmap[ id ]->attributes.end(); attribute++ )
  {
    if ( attribute->second != "" &&
         attribute->first != "referenceLine" &&
         !( (attribute->first == "exitOnStandardFaults" ||
             attribute->first == "createInstance" ||
             attribute->first == "initializePartnerRole" ||
             attribute->first == "initiate" ||
             attribute->first == "isolated" ||
             attribute->first == "surpressJoinFailure" ||
             attribute->first == "validate")
            && attribute->second == "no" )
         )
    {
      result += " " + attribute->first + "=\""+ attribute->second +"\"";
    }
  }
  *output << result;
}





/******************************************************************************
 * Other functions
 *****************************************************************************/

/*!
 * The function increases the indices in the vector and propagates resulting
 * carries. For example, if the index vector [3,2] is increased and the maximal
 * bounds are [5,2] the resulting vector is [4,1].
 *
 * \param current_index  vector holding the current indices
 * \param max_index      vector holding the upper bounds of the indices
 *
 * \post Index vector is increased according to the described rules.
 *
 * \invariant Each index lies between 1 and its maximal value, i.e., 1 and the
 *            maximal value can be reached.
 */
void next_index(vector<unsigned int> &current_index, const vector<unsigned int> &max_index)
{
  assert(current_index.size() == max_index.size());

  for (unsigned int i = 0; i < current_index.size(); i++)
  {
    if (current_index[i] < max_index[i])
    {
      current_index[i]++;
      break;
    }
    else
      current_index[i] = 1;
  }
}




/*!
 * \todo Move me to the right place.
 */
void listAttributes ( kc::integer id )
{
  assert(id!=NULL);
  listAttributes ( id->value );
}
