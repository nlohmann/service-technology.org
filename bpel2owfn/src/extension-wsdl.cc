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
 * \file    extension-wsdl.cc
 *
 * \brief   WSDL extension
 *
 * \author  Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          last changes of: \$Author: znamirow $
 *
 * \since   2007/04/30
 *
 * \date    \$Date: 2007/07/11 11:35:51 $
 *
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \version \$Revision: 1.16 $
 */






/******************************************************************************
 * Headers
 *****************************************************************************/

#include <iostream>

#include "extension-wsdl.h"
#include "globals.h"
#include "debug.h"
#include "helpers.h"
#include "ast-details.h"


extern int frontend_lineno;





/******************************************************************************
 * Member functions for class WSDL_Message
 *****************************************************************************/

/*!
 * \param message_name  the name of a message
 */
WSDL_Message::WSDL_Message(string message_name):
    name(message_name) {
    globals::wsdl_information.messages++;
}





/******************************************************************************
 * Member functions for class WSDL_Operation
 *****************************************************************************/

/*!
 * \param operation_name  the name of an operation
 */
WSDL_Operation::WSDL_Operation(string operation_name):
    name(operation_name), faultName(""), input(NULL), output(NULL), fault(NULL) {
    globals::wsdl_information.operations++;
}





/******************************************************************************
 * Member functions for class WSDL_PortType
 *****************************************************************************/

/*!
 * \param portType_name  the name of a portType
 */
WSDL_PortType::WSDL_PortType(string portType_name):
    name(portType_name) {
    globals::wsdl_information.portTypes++;
}





/*!
 * \param operation_name  the name of an operation
 */
void WSDL_PortType::addOperation(string operation_name) {
    if (Operations[operation_name] == NULL) {
        Operations[operation_name] = last = new WSDL_Operation(operation_name);
    } else {
        SAerror(2, operation_name, 0);
        Operations[operation_name] = last = new WSDL_Operation(operation_name);
    }
}





/*!
 * \param type_name     the type of the operation ("input", "output", or "fault")
 * \param message_name  the name of the message
 * \param fault_name    the name of a fault (optional)
 */
void WSDL_PortType::addOperationDetails(string type_name, string message_name, string fault_name) {
    WSDL_Message* message = globals::WSDLInfo.messages[message_name];

    if (message == NULL) {
        genericError(126, message_name, toString(frontend_lineno), ERRORLEVER_WARNING);
    }

    if (type_name == "input") {
        last->input = message;
    } else if (type_name == "output") {
        last->output = message;
    } else if (type_name == "fault") {
        last->fault = message;
        last->faultName = fault_name;
    }
}





/******************************************************************************
 * Member functions for class WSDL_PartnerLinkType
 *****************************************************************************/

/*!
 * \param partnerLinkType_name  the name of the partnerLinkType
 */
WSDL_PartnerLinkType::WSDL_PartnerLinkType(string partnerLinkType_name):
    name(partnerLinkType_name) {
    myRole = pair<string, WSDL_PortType*>("", NULL);
    partnerRole = pair<string, WSDL_PortType*>("", NULL);

    globals::wsdl_information.partnerLinkTypes++;
}





/*!
 * \param role           the name of a role
 * \param portType_name  the name of a portType
 */

void WSDL_PartnerLinkType::addRole(string role_name, string portType_name) {
    WSDL_PortType* portType = globals::WSDLInfo.portTypes[portType_name];

    if (portType == NULL) {
        genericError(127, portType_name, toString(frontend_lineno), ERRORLEVER_WARNING);
    }

    if (role1.first == "" && role2.second == NULL) {
        role1.first = role_name;
        role1.second = portType;
    } else {
        role2.first = role_name;
        role2.second = portType;
    }
}




/*!
 * \param plink          Pointer to the partnerLink in the ASTEmap
 */

void WSDL_PartnerLinkType::correctOrder(ASTE* plink) {
    if (plink->attributes["myRole"] != "") {
        if (this->role1.first == plink->attributes["myRole"]) {
            this->myRole.first = this->role1.first;
            this->myRole.second = this->role1.second;
        }
        if (this->role2.first == plink->attributes["myRole"]) {
            this->myRole.first = this->role2.first;
            this->myRole.second = this->role2.second;
        }
    }
    if (plink->attributes["partnerRole"] != "") {
        if (this->role1.first == plink->attributes["partnerRole"]) {
            this->partnerRole.first = this->role1.first;
            this->partnerRole.second = this->role1.second;
        }
        if (this->role2.first == plink->attributes["partnerRole"]) {
            this->partnerRole.first = this->role2.first;
            this->partnerRole.second = this->role2.second;
        }
    }
}





/******************************************************************************
 * Member functions for class WSDL
 *****************************************************************************/

/*!
 * \param partnerLinkType  a pointer to a partnerLinkType
 * \param operation_name   a name of an operation
 * \param receiving        a boolean which is true if the operation is included in a receiving activity
 *
 * \return true, if operation_name identifies an operation of the given
 *         partnerLinkType
 * \return false, if the given operation was not found
 */
bool WSDL::checkOperation(WSDL_PartnerLinkType* partnerLinkType, string operation_name) const {
    assert(partnerLinkType != NULL);

    if (partnerLinkType->myRole.second != NULL &&
            partnerLinkType->myRole.second->Operations[operation_name] != NULL) {
        return true;
    }

    if (partnerLinkType->partnerRole.second != NULL &&
            partnerLinkType->partnerRole.second->Operations[operation_name] != NULL) {
        return true;
    }

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
bool WSDL::checkPartnerLinkType(WSDL_PartnerLinkType* partnerLinkType, string partnerRole_name) const {
    assert(partnerLinkType != NULL);

    if (partnerLinkType->myRole.first == partnerRole_name) {
        return true;
    } else if (partnerLinkType->partnerRole.first == partnerRole_name) {
        return true;
    }

    return false;
}




/*!
 * \param partnerLink  a name of a partnerLink
 * \param role         a name of a role
 * \param portType     a name of a portType
 *
 * \return true, if the combination is validated in WDSL
 * \return false, if it is not
 */
bool WSDL::checkPortType(string partnerLink, string role, string portType) const {
    string partnerLinkTypeName;
    WSDL_PartnerLinkType* partnerLinkType;
    unsigned int partnerLinkId;

    partnerLinkId = globals::ASTE_partnerLinks[partnerLink];
    if (partnerLinkId == 0) {
        return false;
    }

    partnerLinkTypeName = globals::ASTEmap[partnerLinkId]->attributes["partnerLinkType"];
    if (partnerLinkTypeName == "") {
        return false;
    }

    partnerLinkType = globals::WSDLInfo.partnerLinkTypes[partnerLinkTypeName];
    if (partnerLinkType == NULL) {
        return false;
    }

    if (partnerLinkType->myRole.first == role) {
        if (partnerLinkType->myRole.second == globals::WSDLInfo.portTypes[portType] && globals::WSDLInfo.portTypes[portType] != NULL) {
            return true;
        }
    }

    if (partnerLinkType->partnerRole.first == role) {
        if (partnerLinkType->partnerRole.second == globals::WSDLInfo.portTypes[portType] && globals::WSDLInfo.portTypes[portType] != NULL) {
            return true;
        }
    }

    return false;
}






/*!
 * \param activity     pointer to the activity to be tested
 *
 * \return true, if the variable is correct
 * \return false, if it is not
 */
void WSDL::checkVariable(ASTE* activity) const {
    assert(activity != NULL);

    string inputName = "";
    string outputName = "";
    string inputType = "";
    string outputType = "";
    int SAID = 0;
    WSDL_PartnerLinkType* plink;
    bool error = true;

    string operation = activity->attributes["operation"];

    plink = globals::WSDLInfo.partnerLinkTypes[globals::ASTEmap[globals::ASTE_partnerLinks[activity->attributes["partnerLink"]]]->attributes["partnerLinkType"]];

    if (activity->activityTypeName() == "invoke") {
        SAID = 48;
        if (activity->attributes["inputVariable"] != "") {
            inputName = activity->attributes["inputVariable"];
        }
        if (activity->attributes["outputVariable"] != "") {
            outputName = activity->attributes["outputVariable"];
            error = false;
        }
        if ((activity->attributes["inputVariable"] == "") && (activity->attributes["outputVariable"] == "")) {
            error = false;
        }
    }

    if (activity->activityTypeName() == "receive" || activity->activityTypeName() == "reply") {
        SAID = 58;
        if (activity->attributes["variable"] != "") {
            inputName = activity->attributes["variable"];
        } else {
            error = false;
        }

    }

    if (activity->activityTypeName() == "onMessage" || activity->activityTypeName() == "onEvent") {
        SAID = 87;
        if (activity->attributes["variable"] != "") {
            inputName = activity->attributes["variable"];
        }

        if (activity->attributes["messageType"] != "") {
            globals::ASTE_variableMap[toString(globals::PPcurrentScope) + "." + inputName] = activity->attributes["messageType"];
        }

        if (activity->attributes["element"] != "") {
            globals::ASTE_variableMap[toString(globals::PPcurrentScope) + "." + inputName] = activity->attributes["element"];
        }
        if ((activity->attributes["element"] == "") && (activity->attributes["messageType"] == "") && (activity->attributes["variable"] != "")) {
            error = false;
        }
    }

    if (inputName != "" && activity->activityTypeName() != "invoke") {
        inputType = globals::ASTE_variableMap[toString(globals::PPcurrentScope) + "." + inputName];
        if (plink != NULL &&
                plink->myRole.second != NULL &&
                plink->myRole.second->Operations[operation] != NULL &&
                (plink->myRole.second->Operations[operation]->input != NULL ||
                 plink->myRole.second->Operations[operation]->output != NULL)) {
            if (plink->myRole.second->Operations[operation]->input->name == inputType ||
                    globals::WSDLInfo.messages[plink->myRole.second->Operations[operation]->input->name]->element.second == inputType) {
                error = false;
            } else {
                if (plink->myRole.second->Operations[operation]->fault != NULL &&
                        plink->myRole.second->Operations[operation]->fault->name == inputType) {
                    error = false;
                } else {
                    if (activity->activityTypeName() == "reply" &&
                            plink->myRole.second->Operations[operation]->output != NULL &&
                            (plink->myRole.second->Operations[operation]->output->name == inputType ||
                             globals::WSDLInfo.messages[plink->myRole.second->Operations[operation]->output->name]->element.second == inputType)) {
                        error = false;
                    }
                }
            }
        }
    }

    if (inputName != "" && activity->activityTypeName() == "invoke") {
        inputType = globals::ASTE_variableMap[toString(globals::PPcurrentScope) + "." + inputName];
        if (plink != NULL &&
                plink->partnerRole.second != NULL &&
                plink->partnerRole.second->Operations[operation] != NULL &&
                plink->partnerRole.second->Operations[operation]->input != NULL) {
            if (plink->partnerRole.second->Operations[operation]->input->name == inputType ||
                    globals::WSDLInfo.messages[plink->partnerRole.second->Operations[operation]->input->name]->element.second == inputType) {
                error = false;
            } else {
                if (plink->partnerRole.second->Operations[operation]->fault != NULL &&
                        plink->partnerRole.second->Operations[operation]->fault->name == inputType) {
                    error = false;
                }
            }
        }
    }

    if (error) {
        SAerror(SAID, activity->activityTypeName(), activity->attributes["referenceLine"]);
    }
    error = false;

    if (outputName != "") {
        error = true;
        outputType = globals::ASTE_variableMap[toString(globals::PPcurrentScope) + "." + outputName];
        if (plink != NULL &&
                plink->partnerRole.second != NULL &&
                plink->partnerRole.second->Operations[operation] != NULL &&
                plink->partnerRole.second->Operations[operation]->output != NULL) {
            if (plink->partnerRole.second->Operations[operation]->output->name == outputType ||
                    globals::WSDLInfo.messages[plink->partnerRole.second->Operations[operation]->output->name]->element.second == outputType) {
                error = false;
            } else {
                if (plink->partnerRole.second->Operations[operation]->fault != NULL &&
                        plink->partnerRole.second->Operations[operation]->fault->name == outputType) {
                    error = false;
                }
            }
        }
    }

    if (error) {
        SAerror(SAID, activity->activityTypeName(), activity->attributes["referenceLine"]);
    }

}
