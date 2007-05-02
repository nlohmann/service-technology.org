/*****************************************************************************\
 * Copyright 2005, 2006, 2007 Niels Lohmann, Christian Gierds                *
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
 * \file    ast-details.h
 *
 * \brief   annotations of the AST
 *
 * \author  Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          Christian Gierds <gierds@informatik.hu-berlin.de>,
 *          last changes of: \$Author: znamirow $
 *          
 * \since   2005/07/02
 *
 * \date    \$Date: 2007/05/02 10:51:28 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \version \$Revision: 1.77 $
 */





#ifndef AST_DETAILS_H
#define AST_DETAILS_H





/******************************************************************************
 * Headers
 *****************************************************************************/

#include <map>
#include <string>
#include <set>
#include <vector>

#include "ast-config.h"

using std::map;
using std::string;
using std::set;
using std::vector;

class ASTE;
class pPartnerLink;
class WSDL_PartnerLinkType;





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





/******************************************************************************
 * Class definitions
 *****************************************************************************/

/*!
 * \brief AST extensions
 *
 * \todo Comment this class properly.
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
    string channelName2;		///< name of a channel (for synchronous invoke)
    string variableName;		///< (unique) name of a variable
    string inputVariableName;		///< (unique) name of an input variable
    string outputVariableName;		///< (unique) name of an output variable
    string linkName;			///< (unique) name of a link
    string visConnection;		///< (unique) name of a connectionnode for the activity diagram
    string secVisConnection;		///< (unique) name of a connectionnode for the activity diagram

    pPartnerLink * plRoleDetails;        ///< additional information (myRole, partnerRole) for role-based matching of PartnerLinks

    bool hasEH;				///< true if <process> or <scope> have <eventHandlers>
    bool hasCatchAll;			///< true if <faultHandlers> have a <catchAll> branch
    bool isStartActivity;		///< true if "createInstance" attribute is set to "yes"
    bool cyclic;			///< true if OnAlarm has RepeatEvery
    bool highlighted;			///< true if the activtity is to be highlighted in visualization
    bool isUserDefined;			///< true if construct is user-defined

    set<unsigned int> sourceLinks;	///< outgoing links
    set<unsigned int> targetLinks;	///< ingoing links

    unsigned int sourceActivity;	///< id of the source of a <link>
    unsigned int targetActivity;	///< id of the target of a <link>


    unsigned int max_occurrences;	///< number of maximum occurrences of the activity
    unsigned int max_loops;		///< number of maximum loops of the repeatable construct

    set<unsigned int> peerScopes;	///< as defined on page 132 of the spec
    set<unsigned int> enclosedScopes;	///< as defined on page 122 of the spec

    set<unsigned int> enclosedSourceLinks;	///< the identifiers of all (recursively) enclosed source links
    set<unsigned int> enclosedTargetLinks;	///< the identifiers of all (recursively) enclosed target links

    unsigned int enclosedFH;		///< the id of the enclosed fault handlers
    unsigned int enclosedCH;		///< the id of the enclosed compensation handler
   
    set<string> sharedCorrelationSets;	///< the identifiers of the joined correlation sets in case activity is a start activity
    set<string> catches;		///< the <catch> branches of <faultHandlers> to detect duplicates

    set<unsigned int> enclosedActivities;       ///< set of all the enclosed activities

    string createChannel(bool synchronousCommunication = false);

    string checkVariable(string attributename);
    string checkLink();
    string checkPartnerLink();
    string checkCorrelationSet();

    vector<unsigned int> ancestorActivities() const;
    vector<unsigned int> ancestorScopes() const;
    vector<unsigned int> ancestorLoops() const;
    
    bool checkAncestors();
    bool findIsolatedAncestor();

    void checkAttributes();

    string defineCorrelationSet();
    string defineVariable();
    string defineLink();
    string definePartnerLink();

    string activityTypeName() const;

    bool structured();

    void output();
    bool drawn;

    WSDL_PartnerLinkType* partnerLinkType; ///< a link to a partnerLinkType object holding more information; used for partnerLinks

    ASTE(unsigned int myid, unsigned int mytype);
    virtual ~ASTE();
};





/*!
 * \brief   Data structure to save information about the different roles of a
 *          partner link
 *
 *          In order to do a role based matching (which means an according
 *          naming) of channels some information of a partner link have to be
 *          stored.
 *
 * \todo    Comment me!
 */
class pPartnerLink {
  public:
    
    string name;              ///< name of the partner link
    string partnerLinkType;   ///< the partner link type
    string myRole;            ///< role of the process when using this partner link
    string partnerRole;       ///< role of the partner when using this partner link

    /// the constructor
    pPartnerLink(string n, string type, string me, string you);
    
    /// self defined equals-operator
    bool operator==(pPartnerLink & pl);
};





#endif
