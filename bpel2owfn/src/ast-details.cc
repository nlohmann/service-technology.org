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
 * \date    \$Date: 2006/10/27 12:15:27 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel
 *          for details.
 *
 * \version \$Revision: 1.33 $
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

extern map<unsigned int, map<string, string> > temporaryAttributeMap;
extern set<string> ASTE_inputChannels;
extern set<string> ASTE_outputChannels;
extern set<string> ASTE_variables;





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
ASTE::ASTE(int myid, int mytype)
{
  assert(myid != 0);

  id = myid;
  type = mytype;
  attributes = temporaryAttributeMap[id];

  inWhile = false;		// required initialization!
  inProcess = false;

  controlFlow = POSITIVECF;
}





/*!
 * \brief checks whether required attributes are set
 *
 * \todo a real error message
 *
 * \param array of strings holding attribute names
 */
void ASTE::checkRequiredAttributes(string required[], unsigned int length)
{
  for (unsigned int i = 0; i < length; i++)
    if (attributes[required[i]] == "")
      cerr << "error: attribute `" << required[i] << "' is not set!" << endl;
}




/*!
 * \brief set unset attributes to their standard values
 *
 * \param array of pairs holding attribute names and their standard value
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

  // pass 1: set the values of suppressJoinFailure and exitOnStandardFault
  switch (type)
  {
    case(K_PROCESS):
      {
	string names[] = {"suppressJoinFailure", "exitOnStandardFault"};
	string values[] = {"no", "no"};
	setStandardAttributes(names, values, 2);
	assert(attributes["suppressJoinFailure"] != "");
	assert(attributes["exitOnStandardFault"] != "");

	break;
      }

    case(K_SCOPE):
      {
	/* organize the exitOnStandardFault attribute */
	assert(ASTEmap[parentScopeId] != NULL);
	if (attributes["exitOnStandardFault"] == "")
	  attributes["exitOnStandardFault"] = ASTEmap[parentScopeId]->attributes["exitOnStandardFault"];
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


  // pass 2: check the required attributes
  switch (type)
  {
    case(K_CORRELATION):
      {
      	string required[] = {"set"};
        checkRequiredAttributes(required, 1);
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
	break;
      }

    case(K_FROMPART):
      {
      	string required[] = {"part", "toVariable"};
        checkRequiredAttributes(required, 2);
	break;
      }

    case(K_INVOKE):
    case(K_RECEIVE):
    case(K_REPLY):
    case(K_ONMESSAGE):
      {
      	string required[] = {"partnerLink", "operation"};
        checkRequiredAttributes(required, 2);
	break;
      }

    case(K_PROCESS):
      {
      	string required[] = {"name", "targetNamespace"};
        checkRequiredAttributes(required, 2);
	break;
      }

    case(K_SOURCE):
    case(K_TARGET):
      {
      	string required[] = {"linkName"};
        checkRequiredAttributes(required, 1);
	break;
      }

    case(K_THROW):
      {
      	string required[] = {"faultName"};
        checkRequiredAttributes(required, 1);
	break;
      }

    case(K_TOPART):
      {
      	string required[] = {"part", "fromVariable"};
        checkRequiredAttributes(required, 2);
	break;
      }

    case(K_VALIDATE):
      {
      	string required[] = {"variables"};
        checkRequiredAttributes(required, 1);
	break;
      }

    case(K_VARIABLE):
      {
      	string required[] = {"name"};
        checkRequiredAttributes(required, 1);
	break;
      }

    default: { /* do nothing */ }
  }
}





/*!
 * \brief creates a channel for communicating activities
 *
 * \return name of the channel
 */
string ASTE::createChannel(bool synchronousCommunication)
{
  string channelName = attributes["partnerLink"] + "." + attributes["operation"];
  if (channelName == ".")
  {
    cerr << "no operation or partnerLink given" << endl;
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
 * \brief checks a variable and returns its name
 *
 * Checks whether a given variable was defined before and returns the name of
 * the variable.
 *
 * \return name of the variable
 */

string ASTE::checkVariable()
{
  string variableName = attributes["variable"];

  if (variableName != "")
    if (ASTE_variables.find(variableName) == ASTE_variables.end())
      cerr << "variable " << variableName << " was not defined before" << endl;

  return variableName;
}





/*!
 * \brief checks a variable and returns its name
 *
 * Checks whether a given input variable was defined before and returns the
 * name of the variable.
 *
 * \return name of the variable
 */
string ASTE::checkInputVariable()
{
  string variableName = attributes["inputVariable"];

  if (variableName != "")
    if (ASTE_variables.find(variableName) == ASTE_variables.end())
      cerr << "variable " << variableName << " was not defined before" << endl;

  return variableName;
}





/*!
 * \brief checks a variable and returns its name
 *
 * Checks whether a given output variable was defined before and returns the
 * name of the variable.
 *
 * \return name of the variable
 */
string ASTE::checkOutputVariable()
{
  string variableName = attributes["outputVariable"];

  if (variableName != "")
    if (ASTE_variables.find(variableName) == ASTE_variables.end())
      cerr << "variable " << variableName << " was not defined before" << endl;

  return variableName;
}





/*!
 * \brief defines a variable
 */
void ASTE::defineVariable()
{
  string variableName = attributes["name"];

  // triggers SA00023
  if (ASTE_variables.find(variableName) != ASTE_variables.end())
    SAerror(23, variableName, toInt(attributes["referenceLine"]));

  ASTE_variables.insert(variableName);
}





/*!
 * \brief returns the name of an activity type
 *
 * Returns the name of an acitivity given using the token value passed upon
 * construction.
 *
 * \returns name of the activity type
 *
 * \todo complete the list
 */
string ASTE::activityTypeName()
{
  switch (type)
  {
    case(K_ASSIGN):		return "assign";
    case(K_CASE):		return "case";
    case(K_CATCH):		return "catch";
    case(K_CATCHALL):		return "catchAll";
    case(K_COMPENSATE):		return "compensate";
    case(K_COMPENSATIONHANDLER):return "compensationHandler";
    case(K_CORRELATION):	return "correlation";
    case(K_CORRELATIONSET):	return "correlationSet";
    case(K_ELSE):		return "else";
    case(K_ELSEIF):		return "elseIf";
    case(K_EMPTY):		return "empty";
    case(K_EXIT):		return "exit";
    case(K_EVENTHANDLERS):	return "eventHandlers";
    case(K_FAULTHANDLERS):	return "faultHandlers";
    case(K_FROM):		return "from";
    case(K_FLOW):		return "flow";
    case(K_IF):			return "if";
    case(K_INVOKE):		return "invoke";
    case(K_LINK):		return "link";
    case(K_ONALARM):		return "onAlarm";
    case(K_ONMESSAGE):		return "onMessage";
    case(K_OTHERWISE):		return "otherwise";
    case(K_PICK):		return "pick";
    case(K_PARTNERLINK):	return "partnerLink";
    case(K_PROCESS):		return "process";
    case(K_RECEIVE):		return "receive";
    case(K_REPEATUNTIL):	return "repeatUntil";
    case(K_REPLY):		return "reply";
    case(K_SCOPE):		return "scope";
    case(K_SOURCE):		return "source";
    case(K_SEQUENCE):		return "sequence";
    case(K_SWITCH):		return "switch";
    case(K_TERMINATE):		return "terminate";
    case(K_TARGET):		return "target";
    case(K_THROW):		return "throw";
    case(K_TO):			return "to";
    case(K_VALIDATE):		return "validate";
    case(K_VARIABLE):		return "variable";
    case(K_WAIT):		return "wait";
    case(K_WHILE):		return "while";
    default:			return "unknown";
  }
}
