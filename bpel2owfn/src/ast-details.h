/*****************************************************************************\
 * Copyright 2005, 2006 Niels Lohmann, Christian Gierds, Dennis Reinert      *
 *                                                                           *
 * This file is part of BPEL2oWFN.                                           *
 *                                                                           *
 * BPEL2oWFN is free software; you can redistribute it and/or modify it      *
 * under the terms of the GNU General Public License as published by the     *
 * Free Software Foundation; either version 2 of the License, or (at your    *
 * option) any later version.                                                *
 *                                                                           *
 * BPEL2oWFN is distributed in the hope that it will be useful, but WITHOUT  *
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or     *
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for  *
 * more details.                                                             *
 *                                                                           *
 * You should have received a copy of the GNU General Public License along   *
 * with BPEL2oWFN; if not, write to the Free Software Foundation, Inc., 51   *
 * Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.                      *
\****************************************************************************/

/*!
 * \file    ast-details.h
 *
 * \brief   annotations of the AST
 *
 * \author  responsible: Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          last changes of: \$Author: nlohmann $
 *          
 * \since   2005/07/02
 *
 * \date    \$Date: 2006/11/24 09:52:23 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universitšt zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel
 *          for details.
 *
 * \version \$Revision: 1.53 $
 */





#ifndef AST_DETAILS_H
#define AST_DETAILS_H





/******************************************************************************
 * Headers
 *****************************************************************************/

#include <map>
#include <string>
#include <set>
#include <list>

#include "ast-config.h"

using namespace std;





/******************************************************************************
 * Data structures
 *****************************************************************************/

/*!
 * \brief enumeration of possible types of control flow
 */
typedef enum
{
  POSITIVECF = 0,	///< positive control flow
  FHCF = 1,		///< control flow in fault handler
  CHCF = 2,		///< control flow in compensation handler
  THCF = 3		///< control flow in termination handler
} controlFlowType;


/*!
 * \brief enumeration of possible attribute types
 */
typedef enum
{
  T_BOOLEAN,		///< Boolean value: "yes" or "no"
  T_INITIATE,		///< for correlation: "yes", "join" or "no"
  T_ROLES,		///< for query: "myRole" or "partnerRole"
  T_PATTERN,		///< for correlation: "request", "response" or "request-response"
  T_BPELVARIABLENAME,	///< a variable name (NCName without ".")
  T_UINT		///< unsigned integers
} attributeType;


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
} activityRelation;



/******************************************************************************
 * Class definitions
 *****************************************************************************/

/*!
 * \brief AST extensions
 */
class ASTE
{
  private:
    unsigned int id;			///< the id of the AST phylum
    unsigned int type;			///< the type of the node, identified by the keywords as parsed by Flex

    void checkRequiredAttributes(string required[], unsigned int length);
    void setStandardAttributes(string names[], string values[], unsigned int length);
    void checkAttributeType(string attribute, attributeType type);

  public:
    kc::impl_abstract_phylum* phylum;	///< the AST phylum

    map<string, string> attributes;	///< the parsed attributes

    unsigned int parentScopeId;		///< the identifier of the parent scope
    unsigned int parentActivityId;	///< the identifier of the parent activity
    
    controlFlowType controlFlow;	///< the kind of control flow the activity is embedded in

    string channelName;			///< name of a channel
    string variableName;		///< (unique) name of a variable
    string inputVariableName;		///< (unique) name of an input variable
    string outputVariableName;		///< (unique) name of an output variable
    string linkName;			///< (unique) name of a link

    bool hasEH;				///< true if <process> or <scope> have <eventHandlers>
    bool hasCatchAll;			///< true if <faultHandlers> have a <catchAll> branch
    bool inProcess;			///< true if <compensationHandler> is embedded to a <process>
    bool isStartActivity;		///< true if "createInstance" attribute is set to "yes"

    set<unsigned int> sourceLinks;	///< outgoing links
    set<unsigned int> targetLinks;	///< ingoing links

    unsigned int targetActivity;	///< id of the target of a <link>
    unsigned int sourceActivity;	///< id of the source of a <link>

    unsigned int max_occurrences;	///< number of maximum occurrences of the activity
    unsigned int max_loops;		///< number of maximum loops of the repeatable construct

    set<unsigned int> peerScopes;	///< as defined on page 132 of the spec
    set<unsigned int> enclosedScopes;	///< as defined on page 122 of the spec
    set<int> enclosedSourceLinks;	///< the identifiers of all (recursively) enclosed source links
   
    set<string> sharedCorrelationSets;	///< the identifiers of the joined correlation sets in case activity is a start activity
    set<string> catches;		///< the <catch> branches of <faultHandlers> to detect duplicates

    set<unsigned int> enclosedActivities;       ///< set of all the enclosed activities
    set<unsigned int> conflictingActivities;    ///< set of all conflicting activities

    string createChannel(bool synchronousCommunication = false);

    string checkVariable(string attributename);
    string checkLink();
    void checkPartnerLink();
    string checkCorrelationSet();

    list<unsigned int> ancestorActivities();
    list<unsigned int> ancestorScopes();
    list<unsigned int> ancestorLoops();
    
    bool checkAncestors();
    bool findIsolatedAncestor();

    void checkAttributes();

    void defineCorrelationSet();
    void defineVariable();
    void defineLink();
    void definePartnerLink();

    string activityTypeName();

    ASTE(unsigned int myid, unsigned int mytype);
};





#endif
