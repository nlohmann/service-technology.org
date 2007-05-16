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
 * \file    extension-wsdl.h
 *
 * \brief   WSDL extension
 *
 * \author  Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          last changes of: \$Author: znamirow $
 *
 * \since   2007/04/30
 *
 * \date    \$Date: 2007/05/16 11:52:39 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \version \$Revision: 1.11 $
 */





#ifndef EXTENSION_WSDL_H
#define EXTENSION_WSDL_H





#include <map>
#include <string>
#include <utility>

using std::map;
using std::string;
using std::pair;

class ASTE; // forward declaration




class WSDL_Message
{
  public:
    /// name of the message
    string name;

    /// name of a single part with the element attribute
    pair<string, string> element;

    /// parts of the message
    map<string, string> parts;

    /// constructor
    WSDL_Message(string message_name);
};





class WSDL_Operation
{
  public:
    /// name of the operation
    string name;

    /// opetional faultName
    string faultName;

    /// input message
    WSDL_Message *input;

    /// output message
    WSDL_Message *output;

    /// fault message
    WSDL_Message *fault;

    /// constructor
    WSDL_Operation(string operation_name);
};





class WSDL_PortType
{
  public:
    /// name of the portType
    string name;

    /// map of all operations
    map<string, WSDL_Operation*> Operations;

    /// pointer to the last (i.e., current) operation
    WSDL_Operation *last;

    /// constructor
    WSDL_PortType(string portType_name);

    /// add an operation to this portType
    void addOperation(string operation_name);

    /// add details of the current operation
    void addOperationDetails(string type_name, string message_name, string fault_name = "");
};





class WSDL_PartnerLinkType
{
  public:
    /// name of the partnerLinkType
    string name;

    /// myRole (used for receiving communication)
    pair<string, WSDL_PortType*> myRole;

    /// partnerRole (used for invoking communication)
    pair<string, WSDL_PortType*> partnerRole;

    /// first temporary placeholder for a role
    pair<string, WSDL_PortType*> role1;

    /// second temporary placeholder for a role
    pair<string, WSDL_PortType*> role2;

    /// constructor
    WSDL_PartnerLinkType(string my_name);

    /// add a role to this partnerLinkType
    void addRole(string role, string portType_name);
    
    /// corrects the order of roles when called by the bpel postprocess
    void correctOrder(ASTE *plink);
};





class WSDL
{
  public:
    /// the portTypes of the parsed WSDL file
    map<string, WSDL_PortType*> portTypes;

    /// the partnerLinkTypes of the parsed WSDL file
    map<string, WSDL_PartnerLinkType*> partnerLinkTypes; 

    /// the messages of the parsed WSDL file
    map<string, WSDL_Message*> messages;

    /// check an operation
    bool checkOperation(WSDL_PartnerLinkType *partnerLinkType, string operation_name) const;

    /// check a partnerLinkType
    bool checkPartnerLinkType(WSDL_PartnerLinkType *partnerLinkType, string partnerRole_name) const;

    /// check a combination of partnerLink, role and portType
    bool checkPortType(string partnerLink, string role, string portType) const;

    /// check if a given variable has the correct message type/element
    void checkVariable(ASTE *activity) const;
};

#endif
