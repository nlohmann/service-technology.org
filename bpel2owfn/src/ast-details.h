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
 * \date    \$Date: 2006/11/05 12:42:27 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel
 *          for details.
 *
 * \version \$Revision: 1.38 $
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
 * \brief structure to store standard values of attributes
 */
typedef struct attribute {
  string name;
  string value;
};


/*!
 * \brief enumeration of possible attribute types
 */
typedef enum
{
  T_BOOLEAN,		///< Boolean value: "yes" or "no"
  T_INITIATE,		///< for correlation: "yes", "join" or "no"
  T_ROLES,		///< for query: "myRole" or "partnerRole"
  T_PATTERN,		///< for correlation: "request", "response" or "request-response"
  T_BPELVARIABLENAME	///< a variable name (NCName without ".")
} attributeType;




/******************************************************************************
 * Class definitions
 *****************************************************************************/

/*!
 * \brief AST extensions
 */
class ASTE
{
  private:
    unsigned int id;	///< the id of the AST phylum
    unsigned int type;	///< the type of the node, identified by the keywords as parsed by Flex

    void checkRequiredAttributes(string required[], unsigned int length);
    void setStandardAttributes(string names[], string values[], unsigned int length);
    void checkAttributeType(string attribute, attributeType type);

  public:
    kc::impl_abstract_phylum* phylum;	///< the AST phylum

    map<string, string> attributes;	///< the parsed attributes
    set<int> enclosedSourceLinks;	///< the identifiers of all (recursively) enclosed source links

    unsigned int parentScopeId;		///< the identifier of the parent scope
    unsigned int parentActivityId;	///< the identifier of the parent activity
    
    controlFlowType controlFlow;	///< the kind of control flow the activity is embedded in

    string channelName;		///< name of a channel
    string variableName;	///< name of a variable
    string inputVariableName;	///< name of an input variable
    string outputVariableName;	///< name of an output variable
    string linkName;		///< name of a link

    bool hasEH;			///< true if process or scope has an event handler
    bool hasCatchAll;		///< true if fault handler has a catchAll branch
    bool inProcess;		///< true if compensation handler is embedded to a process
    bool inWhile;		///< true if activity is embedded in a while, repeatUntil activity or in onEvent
    bool isStartActivity;	///< true if "createInstance" attribute is set to "yes"
    bool hasSourceLink;		///< true if activity is source of at least one link
    bool hasTargetLink;		///< true if activity is target of at least one link

    set<unsigned int> peerScopes;			///< as defined on page 132 of the spec
    set<unsigned int> enclosedScopes;			///< as defined on page 122 of the spec
   

    ASTE(int myid, int mytype);

    string createChannel(bool synchronousCommunication = false);

    string checkVariable(string attributename);

    list<unsigned int> ancestorActivities();
    list<unsigned int> ancestorScopes();
    
    bool checkAncestors();
    bool findIsolatedAncestor();
    void checkAttributes();
    void defineVariable();
    void defineCorrelationSet();
    string activityTypeName();
};





#endif
