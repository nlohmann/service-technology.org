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
 * \file    ast-details.cc
 *
 * \brief   annotations of the AST
 *
 * \author  responsible: Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          last changes of: \$Author: nlohmann $
 * 
 * \since   2005/07/02
 *
 * \date    \$Date: 2006/11/08 10:16:44 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel
 *          for details.
 *
 * \version \$Revision: 1.61 $
 */





/******************************************************************************
 * Headers
 *****************************************************************************/

#include <cassert>
#include <iostream>

#include "ast-details.h"
#include "debug.h"
#include "helpers.h"
#include "parser.h"

using namespace std;





/******************************************************************************
 * External variables
 *****************************************************************************/

extern set<string> catches;





/******************************************************************************
 * Member functions
 *****************************************************************************/

/*!
 * \brief constructor
 *
 * \param myid an id of an AST node
 * \param mytype value of the type of the node using the token values defined
 *               by flex and bison
 */
ASTE::ASTE(unsigned int myid, unsigned int mytype)
{
  extern map<unsigned int, map<string, string> > temporaryAttributeMap;

  assert(myid != 0);

  id = myid;
  type = mytype;
  attributes = temporaryAttributeMap[id];

  inWhile = false;		// required initialization!
  inProcess = false;
  isStartActivity = false;
  targetActivity = 0; // required initialization!
  sourceActivity = 0; // required initialization!

  controlFlow = POSITIVECF;
}





/******************************************************************************
 * check attributes
 *****************************************************************************/

/*!
 * \brief checks whether required attributes are set
 *
 * \param required array of attribute names
 * \param length   length of the array
 */
void ASTE::checkRequiredAttributes(string required[], unsigned int length)
{
  extern string filename;

  for (unsigned int i = 0; i < length; i++)
    if (attributes[required[i]] == "")
    {
      cerr << filename << ":" << attributes["referenceLine"];
      cerr << " - attribute `" << required[i] << "' is required for <";
      cerr << activityTypeName() << ">" << endl;
    }
}





/*!
 * \brief set unset attributes to their standard values
 *
 * \param names  array of attribute names
 * \param values array of standard values
 * \param length length of the arrays
 */
void ASTE::setStandardAttributes(string names[], string values[], unsigned int length)
{
  for (unsigned int i = 0; i < length; i++)
    if (attributes[names[i]] == "")
      attributes[names[i]] = values[i];
}





/*!
 * \brief checks and returns attributes.
 */
void ASTE::checkAttributes()
{
  extern map<unsigned int, ASTE*> ASTEmap;

  // pass 1: set the default values
  switch (type)
  {
    case(K_PROCESS):
      {
	string names[] = {"suppressJoinFailure", "exitOnStandardFault", "abstractProcess", "enableInstanceCompensation"};
	string values[] = {"no", "no", "no", "no"};
	setStandardAttributes(names, values, 3);

	break;
      }

    case(K_PICK):
      {
	string names[] = {"createInstance"};
	string values[] = {"no"};
	setStandardAttributes(names, values, 1);

	break;
      }

    case(K_RECEIVE):
      {
	string names[] = {"createInstance"};
	string values[] = {"no"};
	setStandardAttributes(names, values, 1);

	break;
      }

    case(K_PARTNERLINK):
      {
	string names[] = {"initializePartnerRole"};
	string values[] = {"no"};
	setStandardAttributes(names, values, 1);

	break;
      }

    case(K_CORRELATION):
      {
	string names[] = {"initiate"};
	string values[] = {"no"};
	setStandardAttributes(names, values, 1);

	break;
      }

    case(K_SCOPE):
      {
	string names[] = {"isolated", "variableAccessSerializable"};
	string values[] = {"no", "no"};
	setStandardAttributes(names, values, 2);

	break;
      }

    case(K_COPY):
      {
	string names[] = {"keepSrcElementName"};
	string values[] = {"no"};
	setStandardAttributes(names, values, 1);

	break;
      }

    case(K_BRANCHES):
      {
	string names[] = {"successfulBranchesOnly"};
	string values[] = {"no"};
	setStandardAttributes(names, values, 1);

	break;
      }

    case(K_ASSIGN):
      {
	string names[] = {"validate"};
	string values[] = {"no"};
	setStandardAttributes(names, values, 1);

	break;
      }      

    default: { /* do nothing */ }
  }


  // pass 2: set the values of suppressJoinFailure and exitOnStandardFault
  switch (type)
  {
    case(K_SCOPE):
      {
	/* organize the exitOnStandardFault attribute */
	assert(ASTEmap[parentScopeId] != NULL);
	if (attributes["exitOnStandardFault"] == "")
	  attributes["exitOnStandardFault"] = ASTEmap[parentScopeId]->attributes["exitOnStandardFault"];
	else
	  checkAttributeType("exitOnStandardFault", T_BOOLEAN);

	assert(attributes["exitOnStandardFault"] != "");

	/* no break here */
      }

    case(K_EMPTY):
    case(K_INVOKE):
    case(K_RECEIVE):
    case(K_REPLY):
    case(K_ASSIGN):
    case(K_VALIDATE):
    case(K_WAIT):
    case(K_THROW):
    case(K_COMPENSATE):
    case(K_COMPENSATESCOPE):
    case(K_TERMINATE):
    case(K_EXIT):
    case(K_FLOW):
    case(K_SWITCH):
    case(K_IF):
    case(K_WHILE):
    case(K_REPEATUNTIL):
    case(K_SEQUENCE):
    case(K_PICK):
      {
	/* organize the suppressJoinFailure attribute */
	assert(ASTEmap[parentActivityId] != NULL);
	assert(ASTEmap[parentScopeId] != NULL);
	if (attributes["suppressJoinFailure"] == "")
	  attributes["suppressJoinFailure"] = ASTEmap[parentActivityId]->attributes["suppressJoinFailure"];
	else
	  checkAttributeType("suppressJoinFailure", T_BOOLEAN);

	// if the attribute is not set now, the activity is directly enclosed
	// to a fault, compensation, event or termination handler, thus the
	// value of the surrounding scope should be taken.
	if (attributes["suppressJoinFailure"] == "")
	  attributes["suppressJoinFailure"] = ASTEmap[parentScopeId]->attributes["suppressJoinFailure"];

	assert(attributes["suppressJoinFailure"] != "");
	
	break;
      }
    default: { /* do nothing */ }
  }


  // pass 3: check the required attributes and perform other tests
  switch (type)
  {
    case(K_BRANCHES):
      {
	checkAttributeType("successfulBranchesOnly", T_BOOLEAN);	
	break;
      }

    case(K_CATCH):
      {
	checkAttributeType("faultVariable", T_BPELVARIABLENAME);
	
	// trigger [SA00081]
	if (attributes["faultVariable"] != "" && 
	    attributes["faultMessageType"] != "" &&
	    attributes["faultElement"] != "")
	  SAerror(81, "`faultVariable' must not be used with `faultMessageType' AND `faultElement'", attributes["referenceLine"]);

	if (attributes["faultMessageType"] != "" &&
	    attributes["faultVariable"] == "")
	  SAerror(81, "`faultMessageType' must be used with `faultVariable'", attributes["referenceLine"]);

	if (attributes["faultElement"] != "" &&
	    attributes["faultVariable"] == "")
	  SAerror(81, "`faultElement' must be used with `faultVariable'", attributes["referenceLine"]);

	string catchString = toString(parentScopeId)
	  + "|" + attributes["faultName"]
	  + "|" + attributes["faultElement"]
	  + "|" + attributes["faultMessageType"];

	// trigger [SA00093]
	assert(ASTEmap[parentActivityId] != NULL);
	if (ASTEmap[parentActivityId]->catches.find(catchString) != ASTEmap[parentActivityId]->catches.end())
	  SAerror(93, "", attributes["referenceLine"]);
	else
	  ASTEmap[parentActivityId]->catches.insert(catchString);

	break;
      }

    case(K_COMPENSATESCOPE):
      {
      	string required[] = {"target"};
        checkRequiredAttributes(required, 1);
	break;
      }

    case(K_CORRELATION):
      {
      	string required[] = {"set"};
        checkRequiredAttributes(required, 1);

	checkAttributeType("initiate", T_INITIATE);	
	checkAttributeType("pattern", T_PATTERN);	
	break;
      }

    case(K_CORRELATIONSET):
      {
      	string required[] = {"name", "properties"};
        checkRequiredAttributes(required, 2);
	break;
      }
      
    case(K_FOREACH):
      {
      	string required[] = {"counterName", "parallel"};
        checkRequiredAttributes(required, 2);

	checkAttributeType("parallel", T_BOOLEAN);
	checkAttributeType("counterName", T_BPELVARIABLENAME);
	break;
      }

    case(K_FROM):
      {
	checkAttributeType("variable", T_BPELVARIABLENAME);
	break;
      }

    case(K_FROMPART):
      {
      	string required[] = {"part", "toVariable"};
        checkRequiredAttributes(required, 2);
	checkAttributeType("toVariable", T_BPELVARIABLENAME);
	break;
      }

    case(K_INVOKE):
      {
      	string required[] = {"partnerLink", "operation"};
        checkRequiredAttributes(required, 2);
	checkAttributeType("inputVariable", T_BPELVARIABLENAME);
	checkAttributeType("outputVariable", T_BPELVARIABLENAME);
	break;
      }

    case(K_ONMESSAGE):
      {
      	string required[] = {"partnerLink", "operation"};
        checkRequiredAttributes(required, 2);
	checkAttributeType("variable", T_BPELVARIABLENAME);
	break;
      }
    
    case(K_PARTNERLINK):
      {
	string required[] = {"name", "partnerLinkType"};
        checkRequiredAttributes(required, 2);
	checkAttributeType("initializePartnerRole", T_BOOLEAN);

	// trigger [SA00016]
	if (attributes["myRole"] == "" && 
	    attributes["partnerRole"] == "")
	  SAerror(16, attributes["name"], attributes["referenceLine"]);

	// trigger [SA00017]
	if (attributes["partnerRole"] == "" &&
	    attributes["initializePartnerRole"] == "yes")
	  SAerror(17, attributes["name"], attributes["referenceLine"]);

	break;
      }

    case(K_PICK):
      {
	checkAttributeType("createInstance", T_BOOLEAN);

	if (attributes["createInstance"] == "yes")
	  isStartActivity = true;

	break;
      }

    case(K_PROCESS):
      {
      	string required[] = {"name", "targetNamespace"};
        checkRequiredAttributes(required, 2);
	checkAttributeType("suppressJoinFailure", T_BOOLEAN);
	checkAttributeType("exitOnStandardFault", T_BOOLEAN);
	break;
      }

    case(K_RECEIVE):
      {
      	string required[] = {"partnerLink", "operation"};
        checkRequiredAttributes(required, 2);
	checkAttributeType("createInstance", T_BOOLEAN);
	checkAttributeType("variable", T_BPELVARIABLENAME);
	
	if (attributes["createInstance"] == "yes")
	  isStartActivity = true;

	break;
      }

    case(K_REPLY):
      {
      	string required[] = {"partnerLink", "operation"};
        checkRequiredAttributes(required, 2);
	checkAttributeType("variable", T_BPELVARIABLENAME);
	break;
      }

    case(K_SOURCE):
    case(K_TARGET):
      {
      	string required[] = {"linkName"};
        checkRequiredAttributes(required, 1);
	break;
      }

    case(K_SCOPE):
      {
	checkAttributeType("isolated", T_BOOLEAN);
        break;	
      }

    case(K_THROW):
      {
      	string required[] = {"faultName"};
        checkRequiredAttributes(required, 1);
	checkAttributeType("faultVariable", T_BPELVARIABLENAME);
	break;
      }

    case(K_TO):
      {
	checkAttributeType("variable", T_BPELVARIABLENAME);
	break;
      }

    case(K_TOPART):
      {
      	string required[] = {"part", "fromVariable"};
        checkRequiredAttributes(required, 2);
	checkAttributeType("fromVariable", T_BPELVARIABLENAME);
	break;
      }

    case(K_VALIDATE):
      {
      	string required[] = {"variables"};
	checkAttributeType("variables", T_BPELVARIABLENAME);	// in fact BPELVariableNameS
        checkRequiredAttributes(required, 1);
	break;
      }

    case(K_VARIABLE):
      {
      	string required[] = {"name"};
        checkRequiredAttributes(required, 1);
	checkAttributeType("name", T_BPELVARIABLENAME);

	// trigger [SA00025]
	if (attributes["messageType"] != "" && 
	    attributes["type"] != "")
	  SAerror(25, attributes["name"], attributes["referenceLine"]);
	if (attributes["messageType"] != "" && 
	    attributes["element"] != "")
	  SAerror(25, attributes["name"], attributes["referenceLine"]);
	if (attributes["element"] != "" && 
	    attributes["type"] != "")
	  SAerror(25, attributes["name"], attributes["referenceLine"]);

	break;
      }

    default: { /* do nothing */ }
  }
}





/*!
 * \brief check whether an attribute is of a certain type
 *
 * Checks whether the value of an attribute matches a given attribute type as
 * defined in the BPEL specification.
 */
void ASTE::checkAttributeType(string attribute, attributeType type)
{
  extern string filename;

  switch (type)
  {
    case(T_BOOLEAN):
      {
	if (attributes[attribute] == "yes" ||
	    attributes[attribute] == "no")
	  return;

	cerr << filename << ":" << attributes["referenceLine"];
	cerr << " - attribute `" << attribute << "' in <";
	cerr << activityTypeName() << "> must be of type tBoolean" << endl;
	
	break;
      }

    case(T_INITIATE):
      {
	if (attributes[attribute] == "yes" ||
	    attributes[attribute] == "join" ||
	    attributes[attribute] == "no")
	  return;

	cerr << filename << ":" << attributes["referenceLine"];
	cerr << " - attribute `" << attribute << "' in <";
	cerr << activityTypeName() << "> must be of type tInitiate" << endl;
	
	break;
      }

    case(T_ROLES):
      {
	if (attributes[attribute] == "myRole" ||
	    attributes[attribute] == "partnerRole")
	  return;

	cerr << filename << ":" << attributes["referenceLine"];
	cerr << " - attribute `" << attribute << "' in <";
	cerr << activityTypeName() << "> must be of type tRoles" << endl;
	
	break;
      }

    case(T_PATTERN):
      {
	if (attributes[attribute] == "" ||
	    attributes[attribute] == "request" ||
	    attributes[attribute] == "response" ||
	    attributes[attribute] == "request-response")
	  return;

	cerr << filename << ":" << attributes["referenceLine"];
	cerr << " - attribute `" << attribute << "' in <";
	cerr << activityTypeName() << "> must be of type tPattern" << endl;
	
	break;
      }

    case(T_BPELVARIABLENAME):
      {
	// trigger [SA00024]
	if (attributes[attribute].find('.', 0) != string::npos)
	  SAerror(24, attribute, attributes["referenceLine"]);

	break;
      }
  }

  return;
}





/******************************************************************************
 *
 *****************************************************************************/

/*!
 * \brief creates a channel for communicating activities
 *
 * \return name of the channel
 */
string ASTE::createChannel(bool synchronousCommunication)
{
  extern set<string> ASTE_inputChannels;
  extern set<string> ASTE_outputChannels;

  string channelName = attributes["partnerLink"] + "." + attributes["operation"];
  if (channelName == ".")
  {
//    cerr << "no operation or partnerLink given" << endl;
    return "";
  }

  switch (type)
  {
    case(K_RECEIVE):
    case(K_ONMESSAGE):
      {
	ASTE_inputChannels.insert(channelName);
	break;
      }

    case(K_INVOKE):
    case(K_REPLY):
      {
	ASTE_outputChannels.insert(channelName);

	if (synchronousCommunication)
	  ASTE_inputChannels.insert(channelName);

	break;
      }
    
    default:
      assert(false);
  }

  return channelName;
}





/*!
 * \brief for [SA00056]
 *
 * \returns true  if only <scope>, <flow>, <sequence> or <process> activities
 *                are found in the ancestors
 * \returns false otherwise
 */
bool ASTE::checkAncestors()
{
  extern map<unsigned int, ASTE*> ASTEmap;

  if (activityTypeName() != "scope" &&
      activityTypeName() != "flow" &&
      activityTypeName() != "sequence" &&      
      activityTypeName() != "process")
    return false;

  if (id == 1)
    return true;
  
  return ASTEmap[parentActivityId]->checkAncestors();
}





/*!
 * \brief for [SA00091]
 *
 * \returns true  if a parent scope with "isolated=yes" is found
 * \returns false otherwise
 */
bool ASTE::findIsolatedAncestor()
{
  extern map<unsigned int, ASTE*> ASTEmap;

  if (id == 1)
    return false;

  if (attributes["isolated"] == "yes")
    return true;
  else
    return ASTEmap[parentScopeId]->findIsolatedAncestor();
}





/******************************************************************************
 * functions to define entities like variables, links, etc.
 *****************************************************************************/

/*!
 * \brief defines a correlation set
 */
void ASTE::defineCorrelationSet()
{
  extern set<string> correlationSetNames;

  string name = toString(parentScopeId) + "." + attributes["name"];

  // triggers [SA00044]
  if (correlationSetNames.find(name) != correlationSetNames.end())
    SAerror(44, attributes["name"], attributes["referenceLine"]);
  else
    correlationSetNames.insert(name);
}





/*!
 * \brief defines a variable
 */
void ASTE::defineVariable()
{
  extern set<string> variableNames;

  string name = toString(parentScopeId) + "." + attributes["name"];

  // triggers [SA00023]
  if (variableNames.find(name) != variableNames.end())
    SAerror(23, attributes["name"], attributes["referenceLine"]);
  else
    variableNames.insert(name);
}





/*!
 * \brief defines a link
 */
void ASTE::defineLink()
{
  extern set<string> linkNames;

  string name = toString(parentActivityId) + "." + attributes["name"];

  // triggers [SA00064]
  if (linkNames.find(name) != linkNames.end())
    SAerror(64, attributes["name"], attributes["referenceLine"]);
  else
    linkNames.insert(name);
}





/*!
 * \brief defines a partner link
 */
void ASTE::definePartnerLink()
{
  extern set<string> partnerLinkNames;

  string name = toString(parentScopeId) + "." + attributes["name"];

  // triggers [SA00018]
  if (partnerLinkNames.find(name) != partnerLinkNames.end())
    SAerror(18, attributes["name"], attributes["referenceLine"]);
  else
    partnerLinkNames.insert(name);
}





/******************************************************************************
 * functions to check entities like variables, links, etc.
 *****************************************************************************/


/*!
 * \brief checks a variable and returns its name
 *
 * Checks whether a given variable was defined in an ancestor scope and returns
 * the name of the variable preceeded by the first scope that defined the
 * variable, e.g. "1.purchase".
 *
 * \param attributename name of the attribute ("variable", "inputVariable",
 *                      "outputVariable")
 * \return name of the variable
 */

string ASTE::checkVariable(string attributename)
{
  extern set<string> variableNames;
  extern string filename;

  string variableName = attributes[attributename];
  if (variableName == "")
    return variableName;

  list<unsigned int> ancestorScopes = this->ancestorScopes();

  // travers the ancestor scopes
  for (list<unsigned int>::iterator scope = ancestorScopes.begin(); scope != ancestorScopes.end(); scope++)
    if (variableNames.find(toString(*scope) + "." + variableName) != variableNames.end())
      return (toString(*scope) + "." + variableName);

  // display an error message
  cerr << filename << ":" << attributes["referenceLine"];
  cerr << " - <variable> `" << variableName << "' referenced as `" << attributename << "' in <";
  cerr << activityTypeName() << "> was not defined before" << endl;

  return "";
}





/*!
 * \brief checks a link and returns its name
 *
 * Checks whether a given link was defined in an ancestor flow and returns
 * the name of the link preceeded by the first flow that defined the
 * link, e.g. "1.ship-to-invoice".
 *
 * \return name of the link
 */

string ASTE::checkLink()
{
  extern map<unsigned int, ASTE*> ASTEmap;
  extern set<string> linkNames;
  extern string filename;

  string linkName = attributes["linkName"];
  if (linkName == "")
    return linkName;

  list<unsigned int> ancestorActivities = this->ancestorActivities();

  // travers the ancestor flows
  for (list<unsigned int>::iterator flow = ancestorActivities.begin(); flow != ancestorActivities.end(); flow++)
  {
    if (ASTEmap[*flow]->activityTypeName() == "flow")
      if (linkNames.find(toString(*flow) + "." + linkName) != linkNames.end())
	return (toString(*flow) + "." + linkName);
  }

  // trigger [SA00065]
  SAerror(65, linkName, attributes["referenceLine"]);

  return "";
}





/*!
 * \brief checks a partner Link
 *
 * Checks whether a given partnerLink was defined in an ancestor scope.
 */
void ASTE::checkPartnerLink()
{
  extern map<unsigned int, ASTE*> ASTEmap;	
  extern set<string> partnerLinkNames;
  extern string filename;

  string partnerLinkName = attributes["partnerLink"];
  if (partnerLinkName == "")
    return;

  list<unsigned int> ancestorScopes = this->ancestorScopes();

  // travers the ancestor scopes
  for (list<unsigned int>::iterator scope = ancestorScopes.begin(); scope != ancestorScopes.end(); scope++)
    if (partnerLinkNames.find(toString(*scope) + "." + partnerLinkName) != partnerLinkNames.end())
      return;

  // trigger [SA00084]
  assert(ASTEmap[parentActivityId] != NULL);
  if (ASTEmap[parentActivityId]->activityTypeName() == "eventHandlers")
  {
    SAerror(84, partnerLinkName, attributes["referenceLine"]);
    return;
  }

  // display an error message
  cerr << filename << ":" << attributes["referenceLine"];
  cerr << " - <partnerLink> `" << partnerLinkName << "' referenced in <";
  cerr << activityTypeName() << "> was not defined before" << endl;

  return;
}




/*!
 * \brief checks a correlation set
 *
 * Checks whether a given correlation set was defined in an ancestor scope.
 */
string ASTE::checkCorrelationSet()
{
  extern map<unsigned int, ASTE*> ASTEmap;	
  extern set<string> correlationSetNames;
  extern string filename;

  string correlationSetName = attributes["set"];
  if (correlationSetName == "")
    return correlationSetName;

  list<unsigned int> ancestorScopes = this->ancestorScopes();

  // travers the ancestor scopes
  for (list<unsigned int>::iterator scope = ancestorScopes.begin(); scope != ancestorScopes.end(); scope++)
    if (correlationSetNames.find(toString(*scope) + "." + correlationSetName) != correlationSetNames.end())
      return (toString(*scope) + "." + correlationSetName);

  // trigger [SA00088]
  assert(ASTEmap[parentActivityId] != NULL);
  assert(ASTEmap[ASTEmap[parentActivityId]->parentActivityId] != NULL);
  if (ASTEmap[ASTEmap[parentActivityId]->parentActivityId]->activityTypeName() == "eventHandlers")
  {
    SAerror(88, correlationSetName, attributes["referenceLine"]);
    return correlationSetName;
  }

  // display an error message
  cerr << filename << ":" << attributes["referenceLine"];
  cerr << " - <correlationSet> `" << correlationSetName << "' referenced in <";
  cerr << ASTEmap[parentActivityId]->activityTypeName() << "> was not defined before" << endl;

  return "";
}





/******************************************************************************
 * helper functions
 *****************************************************************************/

/*!
 * \returns list of identifiers of all acestor activities
 */
list<unsigned int> ASTE::ancestorActivities()
{
  extern map<unsigned int, ASTE*> ASTEmap;
  list<unsigned int> result;
  
  if (id != 1)
  {
    result.push_back(parentActivityId);
    assert(ASTEmap[parentActivityId] != NULL);
    list<unsigned int> result2 = ASTEmap[parentActivityId]->ancestorActivities();
    result.splice(result.end(), result2);
    
    return result;
  }
  else
    return result;
}





/*!
 * \returns list of identifiers of all ancestor scopes
 */
list<unsigned int> ASTE::ancestorScopes()
{
  extern map<unsigned int, ASTE*> ASTEmap;
  list<unsigned int> result;

  if (id != 1)
  {
    result.push_back(parentScopeId);
    assert(ASTEmap[parentScopeId] != NULL);
    list<unsigned int> result2 = ASTEmap[parentScopeId]->ancestorScopes();
    result.splice(result.end(), result2);
    
    return result;
  }
  else
    return result;
}






/*!
 * \brief returns the name of an activity type
 *
 * Returns the name of an acitivity given using the token value passed upon
 * construction.
 *
 * \returns name of the activity type
 */
string ASTE::activityTypeName()
{
  switch (type)
  {
    case(K_ASSIGN):		return "assign";
    case(K_BRANCHES):		return "branches";
    case(K_CASE):		return "case";
    case(K_CATCH):		return "catch";
    case(K_CATCHALL):		return "catchAll";
    case(K_COMPENSATE):		return "compensate";
    case(K_COMPENSATESCOPE):	return "compensateScope";
    case(K_COMPENSATIONHANDLER):return "compensationHandler";
    case(K_COMPLETIONCONDITION):return "completionCondition";
    case(K_COPY):		return "copy";
    case(K_CONDITION):		return "condition";
    case(K_CORRELATION):	return "correlation";
    case(K_CORRELATIONS):	return "correlations";
    case(K_CORRELATIONSET):	return "correlationSet";
    case(K_CORRELATIONSETS):	return "correlationSets";
    case(K_ELSE):		return "else";
    case(K_ELSEIF):		return "elseIf";
    case(K_EMPTY):		return "empty";
    case(K_EVENTHANDLERS):	return "eventHandlers";
    case(K_EXIT):		return "exit";
    case(K_EXTENSION):		return "extension";
    case(K_EXTENSIONACTIVITY):	return "extensionActivity";
    case(K_EXTENSIONASSIGNOPERATION): return "extensionAssignOperation";
    case(K_EXTENSIONS):		return "extensions";
    case(K_FAULTHANDLERS):	return "faultHandlers";
    case(K_FINALCOUNTERVALUE):	return "finalCounterValue";
    case(K_FLOW):		return "flow";
    case(K_FOR):		return "for";
    case(K_FOREACH):		return "forEach";
    case(K_FROM):		return "from";
    case(K_FROMPART):		return "fromPart";
    case(K_FROMPARTS):		return "fromParts";
    case(K_IF):			return "if";
    case(K_IMPORT):		return "import";
    case(K_INVOKE):		return "invoke";
    case(K_LINK):		return "link";
    case(K_LINKS):		return "links";
    case(K_LITERAL):		return "literal";
    case(K_MESSAGEEXCHANGE):	return "messageExchange";
    case(K_MESSAGEEXCHANGES):	return "messageExchanges";
    case(K_ONALARM):		return "onAlarm";
    case(K_ONEVENT):		return "onEvent";
    case(K_ONMESSAGE):		return "onMessage";
    case(K_OTHERWISE):		return "otherwise";
    case(K_PARTNER):		return "partner";
    case(K_PARTNERLINK):	return "partnerLink";
    case(K_PARTNERLINKS):	return "partnerLinks";
    case(K_PARTNERS):		return "partners";
    case(K_PICK):		return "pick";
    case(K_PROCESS):		return "process";
    case(K_QUERY):		return "query";
    case(K_RECEIVE):		return "receive";
    case(K_REPLY):		return "reply";
    case(K_REPEATEVERY):	return "repeatEvery";
    case(K_REPEATUNTIL):	return "repeatUntil";
    case(K_RETHROW):		return "rethrow";
    case(K_SCOPE):		return "scope";
    case(K_SEQUENCE):		return "sequence";
    case(K_SOURCE):		return "source";
    case(K_SOURCES):		return "sources";
    case(K_STARTCOUNTERVALUE):	return "startCounterValue";
    case(K_SWITCH):		return "switch";
    case(K_TARGET):		return "target";
    case(K_TARGETS):		return "targets";
    case(K_TERMINATE):		return "terminate";
    case(K_TERMINATIONHANDLER):	return "terminationHandler";
    case(K_THROW):		return "throw";
    case(K_TO):			return "to";
    case(K_TOPART):		return "toPart";
    case(K_TOPARTS):		return "toParts";
    case(K_TRANSITIONCONDITION):return "transitionCondition";
    case(K_UNTIL):		return "until";
    case(K_VALIDATE):		return "validate";
    case(K_VARIABLE):		return "variable";
    case(K_VARIABLES):		return "variables";
    case(K_WAIT):		return "wait";
    case(K_WHILE):		return "while";

    default:			return "unknown"; /* should not happen */
  }
}
