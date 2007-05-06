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
 * \brief   WSDL extension
 *
 * \author  Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          last changes of: \$Author: nielslohmann $
 *
 * \since   2007/04/30
 *
 * \date    \$Date: 2007/05/06 17:22:09 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \version \$Revision: 1.7 $
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

/*!
 * \param message_name  the name of a message
 */
WSDL_Message::WSDL_Message(string message_name):
  name(message_name)
{
  globals::wsdl_information.messages++;
}





/******************************************************************************
 * Functions for class WSDL_Operation
 *****************************************************************************/

/*!
 * \param operation_name  the name of an operation
 */
WSDL_Operation::WSDL_Operation(string operation_name):
  name(operation_name), faultName(""), input(NULL), output(NULL), fault(NULL)
{
  globals::wsdl_information.operations++;
}





/******************************************************************************
 * Functions for class WSDL_PortType
 *****************************************************************************/

/*!
 * \param portType_name  the name of a portType
 */
WSDL_PortType::WSDL_PortType(string portType_name):
  name(portType_name)
{
  globals::wsdl_information.portTypes++;
}





/*!
 * \param operation_name  the name of an operation
 */
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





/*!
 * \param type_name     the type of the operation ("input", "output", or "fault")
 * \param message_name  the name of the message
 * \param fault_name    the name of a fault (optional)
 */
void WSDL_PortType::addOperationDetails(string type_name, string message_name, string fault_name)
{
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

/*!
 * \param partnerLinkType_name  the name of the partnerLinkType
 */
WSDL_PartnerLinkType::WSDL_PartnerLinkType(string partnerLinkType_name):
  name(partnerLinkType_name)
{
  role1 = pair<string, WSDL_PortType*>("", NULL);
  role2 = pair<string, WSDL_PortType*>("", NULL);

  globals::wsdl_information.partnerLinkTypes++;
}





/*!
 * \param role           the name of a role
 * \param portType_name  the name of a portType
 */
void WSDL_PartnerLinkType::addRole(string role_name, string portType_name)
{
  WSDL_PortType *portType = globals::WSDLInfo.portTypes[portType_name];

  if (portType == NULL)
    genericError(127, portType_name, toString(frontend_lineno), ERRORLEVER_WARNING);

  if (role1.first == "" && role2.second == NULL)
  {
    role1.first = role_name;
    role1.second = portType;
  }
  else
  {
    role2.first = role_name;
    role2.second = portType;
  }
}





/******************************************************************************
 * Functions for class WSDL
 *****************************************************************************/

/*!
 * \param partnerLinkType  a pointer to a partnerLinkType
 * \param operation_name   a name of an operation
 *
 * \return true, if operation_name identifies an operation of the given
 *         partnerLinkType
 * \return false, if the given operation was not found
 */
bool WSDL::checkOperation(WSDL_PartnerLinkType *partnerLinkType, string operation_name) const
{
  assert(partnerLinkType != NULL);

  if (partnerLinkType->role1.second != NULL &&
      partnerLinkType->role1.second->operation1 != NULL &&
      partnerLinkType->role1.second->operation1->name == operation_name)
    return true;

  if (partnerLinkType->role1.second != NULL &&
      partnerLinkType->role1.second->operation2 != NULL &&
      partnerLinkType->role1.second->operation2->name == operation_name)
    return true;

  if (partnerLinkType->role2.second != NULL &&
      partnerLinkType->role2.second->operation1 != NULL &&
      partnerLinkType->role2.second->operation1->name == operation_name)
    return true;

  if (partnerLinkType->role2.second != NULL &&
      partnerLinkType->role2.second->operation2 != NULL &&
      partnerLinkType->role2.second->operation2->name == operation_name)
    return true;

  return false;
}





/*!
 * \param partnerLinkType  a pointer to a partnerLinkType
 * \param operation_name   a name of a partnerRole
 *
 * \return true, if partnerRole_name identifies a role of the given
 *         partnerLinkType
 * \return false, if the given role was not found
 */
bool WSDL::checkPartnerLinkType(WSDL_PartnerLinkType *partnerLinkType, string partnerRole_name) const
{
  assert(partnerLinkType != NULL);

  if (partnerLinkType->role1.first == partnerRole_name)
    return true;
  else if (partnerLinkType->role2.first == partnerRole_name)
    return true;

  return false;
}
