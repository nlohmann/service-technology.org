/*****************************************************************************\
 * Copyright 2007  Niels Lohmann                                             *
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
 * \file    extension-wsdl.cc
 *
 * \author  Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          last changes of: \$Author: nielslohmann $
 *
 * \since   2007/04/30
 *
 * \date    \$Date: 2007/05/02 10:22:15 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \version \$Revision: 1.3 $
 *
 * \todo    Comment me!
 */





#include <iostream>

#include "extension-wsdl.h"
#include "globals.h"
#include "debug.h"
#include "helpers.h"


extern int frontend_lineno;





/******************************************************************************
 * Functions for class WSDL_Message
 *****************************************************************************/

WSDL_Message::WSDL_Message(string my_name):
  name(my_name)
{
  globals::wsdl_information.messages++;
}





/******************************************************************************
 * Functions for class WSDL_Operation
 *****************************************************************************/

WSDL_Operation::WSDL_Operation(string my_name):
  name(my_name), faultName(""), input(NULL), output(NULL), fault(NULL)
{
  globals::wsdl_information.operations++;
}





/******************************************************************************
 * Functions for class WSDL_PortType
 *****************************************************************************/

WSDL_PortType::WSDL_PortType(string my_name):
  name(my_name)
{
  globals::wsdl_information.portTypes++;
}





void WSDL_PortType::addOperation(string operation_name)
{
  if (last == NULL)
    operation1 = last = new WSDL_Operation(operation_name);
  else
  {
    operation1 = last;
    operation2 = last = new WSDL_Operation(operation_name);
  }
}





void WSDL_PortType::addOperationDetails(string type_name, string message_name, string fault_name)
{
  // strip XML namespace prefix
  if (message_name.find_first_of(":") != string::npos)
    message_name = message_name.substr(message_name.find_first_of(":")+1, message_name.length());

  WSDL_Message *message = globals::WSDLInfo.messages[message_name];

  if (message == NULL)
    genericError(126, message_name, toString(frontend_lineno), ERRORLEVER_WARNING);

  if (type_name == "input")
    last->input = message;
  else if (type_name == "output")
    last->output = message;
  else if (type_name == "fault")
  {
    last->fault = message;
    last->faultName = fault_name;
  }
}





/******************************************************************************
 * Functions for class WSDL_PartnerLinkType
 *****************************************************************************/

WSDL_PartnerLinkType::WSDL_PartnerLinkType(string my_name):
  name(my_name)
{
  role1 = pair<string, WSDL_PortType*>("", NULL);
  role2 = pair<string, WSDL_PortType*>("", NULL);

  globals::wsdl_information.partnerLinkTypes++;
}





void WSDL_PartnerLinkType::addRole(string role, string portType_name)
{
  // strip XML namespace prefix
  if (portType_name.find_first_of(":") != string::npos)
    portType_name = portType_name.substr(portType_name.find_first_of(":")+1, portType_name.length());

  WSDL_PortType *portType = globals::WSDLInfo.portTypes[portType_name];

  if (portType == NULL)
    genericError(127, portType_name, toString(frontend_lineno), ERRORLEVER_WARNING);

  if (role1.first == "" && role2.second == NULL)
  {
    role1.first = role;
    role1.second = portType;
  }
  else
  {
    role2.first = role;
    role2.second = portType;
  }
}





/******************************************************************************
 * Functions for class WSDL
 *****************************************************************************/

bool WSDL::checkOperation(string operation_name)
{
  for (map<string, WSDL_PortType*>::iterator portType = portTypes.begin(); portType != portTypes.end(); portType++)
  {
    if (portType->second == NULL)
      continue;

    if ((portType->second->operation1 != NULL) && (portType->second->operation1->name == operation_name))
      return true;

    if ((portType->second->operation2 != NULL) && (portType->second->operation2->name == operation_name))
      return true;
  }

  return false;
}





bool WSDL::checkPartnerLinkType(WSDL_PartnerLinkType *partnerLinkType, string partnerRole_name)
{
  assert(partnerLinkType != NULL);

  if (partnerLinkType->role1.first == partnerRole_name)
    return true;
  else if (partnerLinkType->role2.first == partnerRole_name)
    return true;

  return false;
}
