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
 * \file    ast-details.cc
 *
 * \brief   annotations of the AST
 *
 * \author  Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          Christian Gierds <gierds@informatik.hu-berlin.de>,
 *          last changes of: \$Author: nielslohmann $
 *
 * \since   2005/07/02
 *
 * \date    \$Date: 2007/07/31 07:38:10 $
 *
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \version \$Revision: 1.134 $
 */





/******************************************************************************
 * Headers
 *****************************************************************************/

#include <config.h>
#include <iostream>
#include <climits>

#include "ast-details.h"
#include "debug.h"
#include "helpers.h"
#include "frontend-parser.h"
#include "globals.h"
#include "extension-wsdl.h"
#include "extension-chor.h"

using std::cerr;
using std::cout;
using std::endl;





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
ASTE::ASTE(unsigned int myid, unsigned int mytype) :
    id(myid),
    type(mytype),
    sourceActivity(0),
    targetActivity(0),
    visConnection("none"),
    secVisConnection("none"),
    drawn(false),
    highlighted(false),
    enclosedFH(0),
    enclosedCH(0),
    callable(false),
    hasCompensate(false),
    isUserDefined(true),
    controlFlow(POSITIVECF),
    plRoleDetails(NULL),
    isStartActivity(false),
    cyclic(false),
    max_occurrences(1),
    max_loops(UINT_MAX),
    channel_instances(1), // was initialized to 0
    partnerLinkType(NULL),
    messageLink(NULL) {
    assert(myid != 0);

    attributes = globals::temporaryAttributeMap[id];

    if (globals::parsing) {
        // we are parsing: these activities are explicit
        switch (mytype) {
            case(K_LINK):
                globals::process_information.links++;
                break;
            case(K_SCOPE):
                globals::process_information.scopes++;
                break;
            case(K_VARIABLE):
                globals::process_information.variables++;
                break;

                // basic activity
            case(K_EMPTY):
            case(K_INVOKE):
            case(K_RECEIVE):
            case(K_REPLY):
            case(K_ASSIGN):
            case(K_VALIDATE):
            case(K_WAIT):
            case(K_THROW):
            case(K_RETHROW):
            case(K_EXIT):
            case(K_COMPENSATE):
            case(K_COMPENSATESCOPE):
            case(K_OPAQUEACTIVITY):
                globals::process_information.basic_activities++;
                break;

                // structured activities
            case(K_WHILE):
            case(K_REPEATUNTIL):
            case(K_SEQUENCE):
            case(K_FLOW):
            case(K_PICK):
            case(K_IF):
            case(K_FOREACH):
                globals::process_information.structured_activities++;
                break;

            case(K_PARTNERLINK):
                globals::process_information.partner_links++;
                break;
        }
    } else {
        // we are not parsing any more: these activities are implicit
        switch (mytype) {
            case(K_SCOPE):
            case(K_EMPTY):
            case(K_INVOKE):
            case(K_RECEIVE):
            case(K_REPLY):
            case(K_ASSIGN):
            case(K_VALIDATE):
            case(K_WAIT):
            case(K_THROW):
            case(K_RETHROW):
            case(K_EXIT):
            case(K_COMPENSATE):
            case(K_COMPENSATESCOPE):
            case(K_OPAQUEACTIVITY):
            case(K_WHILE):
            case(K_REPEATUNTIL):
            case(K_SEQUENCE):
            case(K_FLOW):
            case(K_PICK):
            case(K_IF):
            case(K_FOREACH):
                globals::process_information.implicit_activities++;
                break;
        }
    }
}





/*!
 * \brief destructor
 */
ASTE::~ASTE() {
    if (plRoleDetails != NULL) {
        delete(plRoleDetails);
        plRoleDetails = NULL;
    }
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
void ASTE::checkRequiredAttributes(string required[], unsigned int length) {
    for (unsigned int i = 0; i < length; i++)
        if (attributes[required[i]] == "") {
            string errormessage = "`" + required[i] + "' is missing in <" + activityTypeName() + ">";
            genericError(107, errormessage, attributes["referenceLine"]);
        }
}





/*!
 * \brief set unset attributes to their standard values
 *
 * \param names  array of attribute names
 * \param values array of standard values
 * \param length length of the arrays
 */
void ASTE::setStandardAttributes(string names[], string values[], unsigned int length) {
    for (unsigned int i = 0; i < length; i++)
        if (attributes[names[i]] == "") {
            attributes[names[i]] = values[i];
        }
}





/*!
 * \brief checks and returns attributes.
 */
void ASTE::checkAttributes() {
    // pass 1: set the default values
    switch (type) {
        case(K_PROCESS): {
            string names[] = {"suppressJoinFailure", "exitOnStandardFault", "abstractProcess", "enableInstanceCompensation"};
            string values[] = {"no", "no", "no", "no"};
            setStandardAttributes(names, values, 3);

            // read the optional BPEL4Chor file to find the number of instances of this process
            if (globals::choreography_filename != "") {
                globals::instances_of_current_process = globals::BPEL4ChorInfo.instances(attributes["name"]);
                if (globals::instances_of_current_process == -1) {
                    genericError(137, "process `" + attributes["name"] + "' not found in BPEL4Chor topology file", attributes["referenceLine"], ERRORLEVEL_ERROR);
                }
            }

            // if the process is abstract, print a notice
            if (attributes["abstractProcess"] == "yes" || attributes["abstractProcessProfile"] != "") {
                globals::abstract_process = true;
                genericError(115, "", attributes["referenceLine"], ERRORLEVEL_NOTICE);
            }

            break;
        }

        case(K_PICK): {
            string names[] = {"createInstance"};
            string values[] = {"no"};
            setStandardAttributes(names, values, 1);

            break;
        }

        case(K_RECEIVE): {
            string names[] = {"createInstance"};
            string values[] = {"no"};
            setStandardAttributes(names, values, 1);

            break;
        }

        case(K_PARTNERLINK): {
            string names[] = {"initializePartnerRole"};
            string values[] = {"no"};
            setStandardAttributes(names, values, 1);

            break;
        }

        case(K_CORRELATION): {
            string names[] = {"initiate"};
            string values[] = {"no"};
            setStandardAttributes(names, values, 1);

            break;
        }

        case(K_SCOPE): {
            string names[] = {"isolated", "variableAccessSerializable"};
            string values[] = {"no", "no"};
            setStandardAttributes(names, values, 2);

            break;
        }

        case(K_COPY): {
            string names[] = {"keepSrcElementName"};
            string values[] = {"no"};
            setStandardAttributes(names, values, 1);

            break;
        }

        case(K_BRANCHES): {
            string names[] = {"successfulBranchesOnly"};
            string values[] = {"no"};
            setStandardAttributes(names, values, 1);

            break;
        }

        case(K_ASSIGN): {
            string names[] = {"validate"};
            string values[] = {"no"};
            setStandardAttributes(names, values, 1);

            break;
        }
    }


    // pass 2: set the values of suppressJoinFailure and exitOnStandardFault
    switch (type) {
        case(K_SCOPE): {
            /* organize the exitOnStandardFault attribute */
            assert(globals::ASTEmap[parentScopeId] != NULL);
            if (attributes["exitOnStandardFault"] == "") {
                attributes["exitOnStandardFault"] = globals::ASTEmap[parentScopeId]->attributes["exitOnStandardFault"];
            } else {
                checkAttributeType("exitOnStandardFault", T_BOOLEAN);
            }

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
        case(K_PICK): {
            /* organize the suppressJoinFailure attribute */
            assert(globals::ASTEmap[parentActivityId] != NULL);
            assert(globals::ASTEmap[parentScopeId] != NULL);
            if (attributes["suppressJoinFailure"] == "") {
                attributes["suppressJoinFailure"] = globals::ASTEmap[parentActivityId]->attributes["suppressJoinFailure"];
            } else {
                checkAttributeType("suppressJoinFailure", T_BOOLEAN);
            }

            // if the attribute is not set now, the activity is directly enclosed
            // to a fault, compensation, event or termination handler, thus the
            // value of the surrounding scope should be taken.
            if (attributes["suppressJoinFailure"] == "") {
                attributes["suppressJoinFailure"] = globals::ASTEmap[parentScopeId]->attributes["suppressJoinFailure"];
            }

            assert(attributes["suppressJoinFailure"] != "");

            break;
        }
    }


    // pass 3: check the required attributes and perform other tests
    switch (type) {
        case(K_BRANCHES): {
            checkAttributeType("successfulBranchesOnly", T_BOOLEAN);
            break;
        }

        case(K_CATCH): {
            checkAttributeType("faultVariable", T_BPELVARIABLENAME);

            // trigger [SA00081]
            if (attributes["faultVariable"] != "" &&
                    attributes["faultMessageType"] != "" &&
                    attributes["faultElement"] != "") {
                SAerror(81, "`faultVariable' must not be used with `faultMessageType' AND `faultElement'", attributes["referenceLine"]);
            }

            if (attributes["faultMessageType"] != "" &&
                    attributes["faultVariable"] == "") {
                SAerror(81, "`faultMessageType' must be used with `faultVariable'", attributes["referenceLine"]);
            }

            if (attributes["faultElement"] != "" &&
                    attributes["faultVariable"] == "") {
                SAerror(81, "`faultElement' must be used with `faultVariable'", attributes["referenceLine"]);
            }

            string catchString = toString(parentScopeId)
                                 + "|" + attributes["faultName"]
                                 + "|" + attributes["faultVariable"]
                                 + "|" + attributes["faultElement"]
                                 + "|" + attributes["faultMessageType"];

            // trigger [SA00093]
            assert(globals::ASTEmap[parentActivityId] != NULL);
            if (globals::ASTEmap[parentActivityId]->catches.find(catchString) != globals::ASTEmap[parentActivityId]->catches.end()) {
                SAerror(93, "", attributes["referenceLine"]);
            } else {
                globals::ASTEmap[parentActivityId]->catches.insert(catchString);
            }

            break;
        }

        case(K_COMPENSATESCOPE): {
            string required[] = {"target"};
            checkRequiredAttributes(required, 1);
            break;
        }

        case(K_CORRELATION): {
            string required[] = {"set"};
            checkRequiredAttributes(required, 1);

            checkAttributeType("initiate", T_INITIATE);
            checkAttributeType("pattern", T_PATTERN);
            break;
        }

        case(K_CORRELATIONSET): {
            string required[] = {"name", "properties"};
            checkRequiredAttributes(required, 2);
            break;
        }

        case(K_FOREACH): {
            string required[] = {"counterName", "parallel"};
            checkRequiredAttributes(required, 2);

            checkAttributeType("hu:maxloops", T_UINT);
            checkAttributeType("parallel", T_BOOLEAN);
            checkAttributeType("counterName", T_BPELVARIABLENAME);
            break;
        }

        case(K_FROM): {
            checkAttributeType("variable", T_BPELVARIABLENAME);
            break;
        }

        case(K_FROMPART): {
            string required[] = {"part", "toVariable"};
            checkRequiredAttributes(required, 2);
            checkAttributeType("toVariable", T_BPELVARIABLENAME);
            break;
        }

        case(K_INVOKE): {
            if (globals::choreography_filename == "") {
                string required[] = {"partnerLink", "operation"};
                checkRequiredAttributes(required, 2);
            }

            checkAttributeType("inputVariable", T_BPELVARIABLENAME);
            checkAttributeType("outputVariable", T_BPELVARIABLENAME);

            break;
        }

        case(K_ONMESSAGE): {
            if (globals::choreography_filename == "") {
                string required[] = {"partnerLink", "operation"};
                checkRequiredAttributes(required, 2);
            }

            checkAttributeType("variable", T_BPELVARIABLENAME);

            break;
        }

        case(K_PARTNERLINK): {
            string required[] = {"name", "partnerLinkType"};
            checkRequiredAttributes(required, 2);
            checkAttributeType("initializePartnerRole", T_BOOLEAN);

            // trigger [SA00016]
            if (attributes["myRole"] == "" &&
                    attributes["partnerRole"] == "") {
                SAerror(16, attributes["name"], attributes["referenceLine"]);
            }

            // trigger [SA00017]
            if (attributes["partnerRole"] == "" &&
                    attributes["initializePartnerRole"] == "yes") {
                SAerror(17, attributes["name"], attributes["referenceLine"]);
            }

            break;
        }

        case(K_PICK): {
            checkAttributeType("createInstance", T_BOOLEAN);

            if (attributes["createInstance"] == "yes") {
                isStartActivity = true;
            }

            break;
        }

        case(K_PROCESS): {
            string required[] = {"name", "targetNamespace"};
            checkRequiredAttributes(required, 2);
            checkAttributeType("suppressJoinFailure", T_BOOLEAN);
            checkAttributeType("exitOnStandardFault", T_BOOLEAN);
            break;
        }

        case(K_RECEIVE): {
            if (globals::choreography_filename == "") {
                string required[] = {"partnerLink", "operation"};
                checkRequiredAttributes(required, 2);
            }

            checkAttributeType("createInstance", T_BOOLEAN);
            checkAttributeType("variable", T_BPELVARIABLENAME);

            if (attributes["createInstance"] == "yes") {
                isStartActivity = true;
            }

            break;
        }

        case(K_REPEATUNTIL): {
            checkAttributeType("hu:maxloops", T_UINT);
            break;
        }

        case(K_REPLY): {
            if (globals::choreography_filename == "") {
                string required[] = {"partnerLink", "operation"};
                checkRequiredAttributes(required, 2);
            }

            checkAttributeType("variable", T_BPELVARIABLENAME);

            break;
        }

        case(K_SOURCE):
        case(K_TARGET): {
            string required[] = {"linkName"};
            checkRequiredAttributes(required, 1);
            break;
        }

        case(K_SCOPE): {
            checkAttributeType("isolated", T_BOOLEAN);
            break;
        }

        case(K_THROW): {
            string required[] = {"faultName"};
            checkRequiredAttributes(required, 1);
            checkAttributeType("faultVariable", T_BPELVARIABLENAME);
            break;
        }

        case(K_TO): {
            checkAttributeType("variable", T_BPELVARIABLENAME);
            break;
        }

        case(K_TOPART): {
            string required[] = {"part", "fromVariable"};
            checkRequiredAttributes(required, 2);
            checkAttributeType("fromVariable", T_BPELVARIABLENAME);
            break;
        }

        case(K_VALIDATE): {
            string required[] = {"variables"};
            checkAttributeType("variables", T_BPELVARIABLENAME);  // in fact BPELVariableNameS
            checkRequiredAttributes(required, 1);
            break;
        }

        case(K_VARIABLE): {
            string required[] = {"name"};
            checkRequiredAttributes(required, 1);
            checkAttributeType("name", T_BPELVARIABLENAME);

            // trigger [SA00025]
            if (attributes["messageType"] != "" &&
                    attributes["type"] != "") {
                SAerror(25, attributes["name"], attributes["referenceLine"]);
            }
            if (attributes["messageType"] != "" &&
                    attributes["element"] != "") {
                SAerror(25, attributes["name"], attributes["referenceLine"]);
            }
            if (attributes["element"] != "" &&
                    attributes["type"] != "") {
                SAerror(25, attributes["name"], attributes["referenceLine"]);
            }

            break;
        }

        case(K_WHILE): {
            checkAttributeType("hu:maxloops", T_UINT);
            break;
        }
    }
}





/*!
 * \brief check whether an attribute is of a certain type
 *
 * Checks whether the value of an attribute matches a given attribute type as
 * defined in the BPEL specification.
 */
void ASTE::checkAttributeType(string attribute, ENUM_attribute_type type) {
    switch (type) {
        case(T_BOOLEAN): {
            if (attributes[attribute] == "yes" ||
                    attributes[attribute] == "no") {
                return;
            }

            string errormessage = "attribute `" + attribute + "' in <" + activityTypeName() + "> must be of type tBoolean";
            genericError(108, errormessage, attributes["referenceLine"], ERRORLEVEL_SYNTAX);

            break;
        }

        case(T_INITIATE): {
            if (attributes[attribute] == "yes" ||
                    attributes[attribute] == "join" ||
                    attributes[attribute] == "no") {
                return;
            }

            string errormessage = "attribute `" + attribute + "' in <" + activityTypeName() + "> must be of type tInitiate";
            genericError(108, errormessage, attributes["referenceLine"], ERRORLEVEL_SYNTAX);

            break;
        }

        case(T_ROLES): {
            if (attributes[attribute] == "myRole" ||
                    attributes[attribute] == "partnerRole") {
                return;
            }

            string errormessage = "attribute `" + attribute + "' in <" + activityTypeName() + "> must be of type tRoles";
            genericError(108, errormessage, attributes["referenceLine"], ERRORLEVEL_SYNTAX);

            break;
        }

        case(T_PATTERN): {
            if (attributes[attribute] == "" ||
                    attributes[attribute] == "request" ||
                    attributes[attribute] == "response" ||
                    attributes[attribute] == "request-response") {
                return;
            }

            string errormessage = "attribute `" + attribute + "' in <" + activityTypeName() + "> must be of type tPattern";
            genericError(108, errormessage, attributes["referenceLine"], ERRORLEVEL_SYNTAX);

            break;
        }

        case(T_BPELVARIABLENAME): {
            // trigger [SA00024]
            if (attributes[attribute].find('.', 0) != string::npos) {
                SAerror(24, attribute, attributes["referenceLine"]);
            }

            break;
        }

        case(T_UINT): {
            if (attributes[attribute] != "" && toUInt(attributes[attribute]) == UINT_MAX) {
                string errormessage = "attribute `" + attribute + "' in <" + activityTypeName() + "> must be of type unsigned integer";
                genericError(108, errormessage, attributes["referenceLine"], ERRORLEVEL_SYNTAX);
            }

            break;
        }
    }
}





/******************************************************************************
 * functions to trigger some SA errors
 *****************************************************************************/

/*!
 * \brief creates a channel for communicating activities
 *
 * \bug The <invoke> activity has one field "channelName" used for the input
 *      and output channel. The "plRoleDetails"-stuff creates different strings
 *      for input and output channels. Thus, the Petri net unparser cannot find
 *      the channel place.
 *
 * \todo Synchronous invoke in BPEL4Chor?
 *
 * \return name of the channel
 */
string ASTE::createChannel(bool synchronousCommunication) {
    // in case of BPEL4Chor, we can read the channel name from the choreography
    if (globals::choreography_filename != "") {
        messageLink = globals::BPEL4ChorInfo.get_messageLink(id);
        assert(messageLink != NULL);

        channelName = messageLink->name;

        switch (type) {
                // receiving activity
            case(K_RECEIVE):
            case(K_ONMESSAGE): {
                // depending on the channel count, create one or more input channel(s)
                globals::ASTE_inputChannels[channelName] = (messageLink->participantSet != NULL) ?
                                                           messageLink->participantSet->count :
                                                           0;

                break;
            }

            // sending activity
            case(K_INVOKE):
            case(K_REPLY): {
                // depending on the channel count, create one or more output channel(s)
                globals::ASTE_outputChannels[channelName] = (messageLink->participantSet != NULL) ?
                                                            messageLink->participantSet->count :
                                                            0;

                break;
            }
        }

        return channelName;
    }


    // Find the name of the channel: it is usually a combination of partnerLink
    // and operation. If no partnerLink and operation are given, the channel name
    // is derived from an optional "id" or "name" attribute which is usually
    // defined for BPEL4Chor processes.
    string channelName = attributes["partnerLink"] + "." + attributes["operation"];

    // check if a channel name could be derived from partnerLink and operation
    if (channelName == ".") {
        // check if an "id" or "name" attribute is found
        if (attributes["id"] != "" || attributes["name"] != "") {
            // set the channel name and warn
            channelName = (attributes["id"] != "") ? attributes["id"] : attributes["name"];
            genericError(138, channelName, attributes["referenceLine"], ERRORLEVEL_NOTICE);
        } else {
            // no "operation", "partnerLink", "id", or "name" attribute is found
            return "";
        }
    }


    switch (type) {
        case(K_RECEIVE):
        case(K_ONMESSAGE): {
            if (plRoleDetails != NULL) {
                channelName = plRoleDetails->partnerRole + "." + plRoleDetails->myRole + "." + attributes["operation"];
            }

            // no instances needed
            globals::ASTE_inputChannels[channelName] = 0;
            break;
        }

        case(K_INVOKE):
        case(K_REPLY): {
            if (plRoleDetails != NULL) {
                channelName = plRoleDetails->myRole + "." + plRoleDetails->partnerRole + "." + attributes["operation"];
            }

            // no instances needed
            globals::ASTE_outputChannels[channelName] = 0;

            if (synchronousCommunication) {
                string channelName2 = attributes["partnerLink"] + "." + attributes["operation"];
                if (plRoleDetails != NULL) {
                    channelName2 = plRoleDetails->partnerRole + "." + plRoleDetails->myRole + "." + attributes["operation"];
                }
                this->channelName2 = channelName2;

                // no instances needed
                globals::ASTE_inputChannels[channelName2] = 0;
            }

            break;
        }

        default:
            assert(false);
    }


    // map the channel to the name of the partner link to obtain ports
    if (attributes["partnerLink"] != "") {
        globals::ASTE_channel_to_port[channelName] = attributes["partnerLink"];
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
bool ASTE::checkAncestors() {
    if (activityTypeName() != "scope" &&
            activityTypeName() != "flow" &&
            activityTypeName() != "sequence" &&
            activityTypeName() != "process") {
        return false;
    }

    if (id == 1) {
        return true;
    }

    return globals::ASTEmap[parentActivityId]->checkAncestors();
}





/*!
 * \brief for [SA00091]
 *
 * \returns true  if a parent scope with "isolated=yes" is found
 * \returns false otherwise
 */
bool ASTE::findIsolatedAncestor() {
    if (id == 1) {
        return false;
    }

    if (attributes["isolated"] == "yes") {
        return true;
    } else {
        return globals::ASTEmap[parentScopeId]->findIsolatedAncestor();
    }
}





/******************************************************************************
 * functions to define entities like variables, links, etc.
 *****************************************************************************/

/*!
 * \brief defines a correlation set
 *
 * \todo comment me
 */
string ASTE::defineCorrelationSet() {
    string name = toString(parentScopeId) + "." + attributes["name"];

    // triggers [SA00044]
    if (globals::ASTE_correlationSetNames.find(name) != globals::ASTE_correlationSetNames.end()) {
        SAerror(44, attributes["name"], attributes["referenceLine"]);
    } else {
        globals::ASTE_correlationSetNames.insert(name);
    }

    return name;
}





/*!
 * \brief defines a variable
 */
string ASTE::defineVariable() {
    string name = toString(parentScopeId) + "." + attributes["name"];

    // triggers [SA00023]
    if (globals::ASTE_variableNames.find(name) != globals::ASTE_variableNames.end()) {
        SAerror(23, attributes["name"], attributes["referenceLine"]);
    } else {
        globals::ASTE_variableNames.insert(name);
    }

    if (attributes["messageType"] != "") {
        globals::ASTE_variableMap[name] = attributes["messageType"];
    }


    return name;
}





/*!
 * \brief defines a messageExchange
 */
string ASTE::defineMessageExchange() {
    string name = toString(parentScopeId) + "." + attributes["name"];

    if (!(globals::ASTE_messageExchangeNames.find(name) != globals::ASTE_messageExchangeNames.end())) {
        globals::ASTE_messageExchangeNames.insert(name);
    }

    return name;
}





/*!
 * \brief defines a link
 *
 * \return the unique name of the link
 *
 * \todo simplify the test using the result of the insertion
 */
string ASTE::defineLink() {
    string name = toString(parentActivityId) + "." + attributes["name"];

    // triggers [SA00064]
    if (globals::ASTE_linkNames.find(name) != globals::ASTE_linkNames.end()) {
        SAerror(64, attributes["name"], attributes["referenceLine"]);
    } else {
        globals::ASTE_linkNames.insert(name);
    }

    return name;
}





/*!
 * \brief defines a partner link
 *
 * \todo comment me
 */
string ASTE::definePartnerLink() {
    string name = toString(parentScopeId) + "." + attributes["name"];

    // triggers [SA00018]
    if (globals::ASTE_partnerLinkNames.find(name) != globals::ASTE_partnerLinkNames.end()) {
        SAerror(18, attributes["name"], attributes["referenceLine"]);
    } else {
        globals::ASTE_partnerLinkNames.insert(name);
        globals::ASTE_partnerLinks[ attributes["name"] ] = id;


        // find partnerLinkType_name
        partnerLinkType = globals::WSDLInfo.partnerLinkTypes[attributes["partnerLinkType"]];

        if (globals::wsdl_filename != "") {
            if (partnerLinkType == NULL) {
                genericError(130, "partnerLinkType `" + attributes["partnerLinkType"] + "' was not defined in WSDL file", attributes["referenceLine"], ERRORLEVER_WARNING);
            } else if (!globals::WSDLInfo.checkPartnerLinkType(partnerLinkType, attributes["partnerRole"])) {
                genericError(129, "role `" + attributes["partnerRole"] + "' was not defined for partnerLinkType `" + attributes["partnerLinkType"] + "' in WSDL file", attributes["referenceLine"], ERRORLEVER_WARNING);
            }
        }

    }

    return name;
}





/*!
 * \brief enters a possibly triggered fault to the appropriate set
 *
 * \todo comment me
 */
void ASTE::enterFault(string fault) {
    if (fault == "") {
        return;
    }

    if (fault.find_first_of(":") < fault.npos) {
        fault = fault.substr(fault.find_first_of(":") + 1);
    }
    possiblyTriggeredFaults.insert(fault);
    if (id > 1) {
        globals::ASTEmap[parentScopeId]->enterFault(fault);
    }
}





/*!
 * \brief enters a possibly triggered fault to the appropriate set
 *
 * \todo comment me
 */
void ASTE::enterFault(WSDL_PartnerLinkType* plt) {
    if (plt == NULL) {
        return;
    }
    if (plt->myRole.second != NULL) {
        for (map<string, WSDL_Operation*>::iterator it = plt->myRole.second->Operations.begin(); it != plt->myRole.second->Operations.end(); it++)
            if (it->second != NULL) {
                enterFault(it->second->faultName);
            }
    }
    if (plt->partnerRole.second != NULL) {
        for (map<string, WSDL_Operation*>::iterator it2 = plt->partnerRole.second->Operations.begin(); it2 != plt->partnerRole.second->Operations.end(); it2++)
            if (it2->second != NULL) {
                enterFault(it2->second->faultName);
            }
    }

}





/*!
 * \brief removes a possibly triggered fault from the appropriate set because it was caught
 *
 * \todo comment me
 */
void ASTE::removeFault(string fault) {
    if (fault == "") {
        return;
    }

    if (fault.find_first_of(":") < fault.npos) {
        fault = fault.substr(fault.find_first_of(":") + 1);
    }
    possiblyTriggeredFaults.erase(fault);
    if (id > 1) {
        globals::ASTEmap[parentScopeId]->removeFault(fault);
    }
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

string ASTE::checkVariable(string attributename) {
    string variableName = attributes[attributename];
    if (variableName == "") {
        return variableName;
    }

    vector<unsigned int> ancestorScopes = this->ancestorScopes();

    // traverse the ancestor scopes
    for (vector<unsigned int>::iterator scope = ancestorScopes.begin(); scope != ancestorScopes.end(); scope++)
        if (globals::ASTE_variableNames.find(toString(*scope) + "." + variableName) != globals::ASTE_variableNames.end()) {
            return (toString(*scope) + "." + variableName);
        }

    // display an error message
    string errormessage;
    if (attributename != "variable") {
        errormessage = "variable `" + variableName + "' referenced as `" + attributename + "' in <" + activityTypeName() + "> was not defined before";
    } else {
        errormessage = "variable `" + variableName + "' referenced in <" + activityTypeName() + "> was not defined before";
    }
    genericError(109, errormessage, attributes["referenceLine"]);

    return "";
}


/*!
 * \brief checks a messageExchange and returns its name
 *
 * Checks whether a given messageExchange was defined in an ancestor scope and returns
 * the name of the messageExchange preceeded by the first scope that defined the
 * messageExchange, e.g. "1.purchase".
 *
 * \param name name of the messageExchange
 *
 * \return name of the variable
 */

string ASTE::checkMessageExchange() {
    // SA00060
    if (this->type == K_REPLY) {
        bool valid = false;
        for (set<unsigned int>:: iterator ima = globals::ASTE_IMAs.begin(); ima != globals::ASTE_IMAs.end(); ima++) {
            if (globals::ASTEmap[(*ima)]->attributes["operation"] == attributes["operation"] &&
                    globals::ASTEmap[(*ima)]->attributes["partnerLink"] == attributes["partnerLink"] &&
                    ((globals::ASTEmap[(*ima)]->attributes["messageExchange"] == "" && attributes["messageExchange"] == "") ||
                     (globals::ASTEmap[(*ima)]->attributes["messageExchange"] == attributes["messageExchange"] &&
                      globals::ASTEmap[(*ima)]->attributes["messageExchange"] != "" && attributes["messageExchange"] != ""))) {
                // If two legal messageexchanges are found, it is no more valid
                if (valid) {
                    valid = false;
                    break;
                }
                valid = true;
            }
        }
        if (!valid) {
            SAerror(60, "", attributes["referenceLine"]);
        }
    }

    if (attributes["messageExchange"] == "") {
        return "";
    }

    vector<unsigned int> ancestorScopes = this->ancestorScopes();

    // traverse the ancestor scopes
    for (vector<unsigned int>::iterator scope = ancestorScopes.begin(); scope != ancestorScopes.end(); scope++)
        if (globals::ASTE_messageExchangeNames.find(toString(*scope) + "." + attributes["messageExchange"]) != globals::ASTE_messageExchangeNames.end()) {
            return (toString(*scope) + "." + attributes["messageExchange"]);
        }

    // SA00061
    SAerror(61, attributes["messageExchange"], attributes["referenceLine"]);

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

string ASTE::checkLink() {
    string linkName = attributes["linkName"];
    if (linkName == "") {
        return linkName;
    }

    vector<unsigned int> ancestorActivities = this->ancestorActivities();

    // travers the ancestor flows
    for (vector<unsigned int>::iterator flow = ancestorActivities.begin(); flow != ancestorActivities.end(); flow++) {
        if (globals::ASTEmap[*flow]->activityTypeName() == "flow")
            if (globals::ASTE_linkNames.find(toString(*flow) + "." + linkName) != globals::ASTE_linkNames.end()) {
                return (toString(*flow) + "." + linkName);
            }
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
string ASTE::checkPartnerLink() {
    string partnerLinkName = attributes["partnerLink"];

    if (partnerLinkName == "") {
        return partnerLinkName;
    }

    vector<unsigned int> ancestorScopes = this->ancestorScopes();

    // traverse the ancestor scopes
    for (vector<unsigned int>::iterator scope = ancestorScopes.begin(); scope != ancestorScopes.end(); scope++) {
        if (globals::ASTE_partnerLinkNames.find(toString(*scope) + "." + partnerLinkName) != globals::ASTE_partnerLinkNames.end()) {
            string partnerLinkType_name = globals::ASTEmap[ globals::ASTE_partnerLinks[partnerLinkName] ]->attributes["partnerLinkType"];
            string myRole_name          = globals::ASTEmap[ globals::ASTE_partnerLinks[partnerLinkName] ]->attributes["myRole"];
            string partnerRole_name     = globals::ASTEmap[ globals::ASTE_partnerLinks[partnerLinkName] ]->attributes["partnerRole"];

            if (partnerLinkType_name != "" && myRole_name != "" && partnerRole_name != "") {
                plRoleDetails = new pPartnerLink(partnerLinkName, partnerLinkType_name, myRole_name, partnerRole_name);
            }

            // check if operation was defined in WSDL file
            if (globals::wsdl_filename != "")
                if (globals::ASTEmap[ globals::ASTE_partnerLinks[partnerLinkName] ]->partnerLinkType != NULL && attributes["operation"] != "")
                    if (globals::WSDLInfo.checkOperation(globals::ASTEmap[ globals::ASTE_partnerLinks[partnerLinkName] ]->partnerLinkType, attributes["operation"]) == false) {
                        genericError(128, "operation `" + attributes["operation"] + "' referenced in <" + activityTypeName() + "> not defined in WSDL file", attributes["referenceLine"], ERRORLEVER_WARNING);
                    }

            return (toString(*scope) + "." + partnerLinkName);
        }
    }

    // trigger [SA00084]
    assert(globals::ASTEmap[parentActivityId] != NULL);
    if (globals::ASTEmap[parentActivityId]->activityTypeName() == "eventHandlers") {
        SAerror(84, partnerLinkName, attributes["referenceLine"]);
        return partnerLinkName;
    }

    // display an error message
    string errormessage = "partner link `" + partnerLinkName + "' referenced in <" + activityTypeName() + "> was not defined before";
    genericError(110, errormessage, attributes["referenceLine"]);

    return "";
}





/*!
 * \brief checks a correlation set
 *
 * Checks whether a given correlation set was defined in an ancestor scope.
 */
string ASTE::checkCorrelationSet() {
    string correlationSetName = attributes["set"];
    if (correlationSetName == "") {
        return correlationSetName;
    }

    vector<unsigned int> ancestorScopes = this->ancestorScopes();

    // travers the ancestor scopes
    for (vector<unsigned int>::iterator scope = ancestorScopes.begin(); scope != ancestorScopes.end(); scope++)
        if (globals::ASTE_correlationSetNames.find(toString(*scope) + "." + correlationSetName) != globals::ASTE_correlationSetNames.end()) {
            return (toString(*scope) + "." + correlationSetName);
        }

    // trigger [SA00088]
    assert(globals::ASTEmap[parentActivityId] != NULL);
    assert(globals::ASTEmap[globals::ASTEmap[parentActivityId]->parentActivityId] != NULL);
    if (globals::ASTEmap[globals::ASTEmap[parentActivityId]->parentActivityId]->activityTypeName() == "eventHandlers") {
        SAerror(88, correlationSetName, attributes["referenceLine"]);
        return correlationSetName;
    }

    // display an error message
    string errormessage = "correlation set `" + correlationSetName + "' referenced in <" + globals::ASTEmap[parentActivityId]->activityTypeName() + "> was not defined before";
    genericError(111, errormessage, attributes["referenceLine"]);

    return "";
}





/******************************************************************************
 * helper functions
 *****************************************************************************/

/*!
 * \returns list of identifiers of all acestor activities
 *
 * \todo Add a check to prevent crash caused by too many elements (cf. task
 *       #6393)
 */
vector<unsigned int> ASTE::ancestorActivities() const {
    vector<unsigned int> result;

    if (id != 1) {
        result.push_back(parentActivityId);
        assert(globals::ASTEmap[parentActivityId] != NULL);
        vector<unsigned int> result2 = globals::ASTEmap[parentActivityId]->ancestorActivities();
        result.insert(result.end(), result2.begin(), result2.end());

        return result;
    } else {
        return result;
    }
}





/*!
 * \returns list of identifiers of all ancestor scopes
 */
vector<unsigned int> ASTE::ancestorScopes() const {
    vector<unsigned int> result;

    if (id != 1) {
        result.push_back(parentScopeId);
        assert(globals::ASTEmap[parentScopeId] != NULL);
        vector<unsigned int> result2 = globals::ASTEmap[parentScopeId]->ancestorScopes();
        result.insert(result.end(), result2.begin(), result2.end());

        return result;
    } else {
        return result;
    }
}





/*!
 * \returns list of identifiers of all ancestor loops (<while>, <repeatUntil>,
 * <forEach>)
 */
vector<unsigned int> ASTE::ancestorLoops() const {
    vector<unsigned int> result;

    if (id != 1) {
        assert(globals::ASTEmap[parentActivityId] != NULL);

        if (globals::ASTEmap[parentActivityId]->activityTypeName() == "while" ||
                globals::ASTEmap[parentActivityId]->activityTypeName() == "repeatUntil" ||
                globals::ASTEmap[parentActivityId]->activityTypeName() == "forEach") {
            result.push_back(parentActivityId);
        }
        vector<unsigned int> result2 = globals::ASTEmap[parentActivityId]->ancestorLoops();
        result.insert(result.end(), result2.begin(), result2.end());

        return result;
    } else {
        return result;
    }
}





/*!
 * \brief returns the name of an activity type
 *
 * Returns the name of an acitivity given using the token value passed upon
 * construction.
 *
 * \returns name of the activity type
 */
string ASTE::activityTypeName() const {
    switch (type) {
        case(K_ASSIGN):
            return "assign";
        case(K_BRANCHES):
            return "branches";
        case(K_CASE):
            return "case";
        case(K_CATCH):
            return "catch";
        case(K_CATCHALL):
            return "catchAll";
        case(K_COMPENSATE):
            return "compensate";
        case(K_COMPENSATESCOPE):
            return "compensateScope";
        case(K_COMPENSATIONHANDLER):
            return "compensationHandler";
        case(K_COMPLETIONCONDITION):
            return "completionCondition";
        case(K_COPY):
            return "copy";
        case(K_CONDITION):
            return "condition";
        case(K_CORRELATION):
            return "correlation";
        case(K_CORRELATIONS):
            return "correlations";
        case(K_CORRELATIONSET):
            return "correlationSet";
        case(K_CORRELATIONSETS):
            return "correlationSets";
        case(K_ELSE):
            return "else";
        case(K_ELSEIF):
            return "elseIf";
        case(K_EMPTY):
            return "empty";
        case(K_EVENTHANDLERS):
            return "eventHandlers";
        case(K_EXIT):
            return "exit";
        case(K_EXTENSION):
            return "extension";
        case(K_EXTENSIONACTIVITY):
            return "extensionActivity";
        case(K_EXTENSIONASSIGNOPERATION):
            return "extensionAssignOperation";
        case(K_EXTENSIONS):
            return "extensions";
        case(K_FAULTHANDLERS):
            return "faultHandlers";
        case(K_FINALCOUNTERVALUE):
            return "finalCounterValue";
        case(K_FLOW):
            return "flow";
        case(K_FOR):
            return "for";
        case(K_FOREACH):
            return "forEach";
        case(K_FROM):
            return "from";
        case(K_FROMPART):
            return "fromPart";
        case(K_FROMPARTS):
            return "fromParts";
        case(K_IF):
            return "if";
        case(K_IMPORT):
            return "import";
        case(K_INVOKE):
            return "invoke";
        case(K_LINK):
            return "link";
        case(K_LINKS):
            return "links";
        case(K_LITERAL):
            return "literal";
        case(K_MESSAGEEXCHANGE):
            return "messageExchange";
        case(K_MESSAGEEXCHANGES):
            return "messageExchanges";
        case(K_ONALARM):
            return "onAlarm";
        case(K_ONEVENT):
            return "onEvent";
        case(K_ONMESSAGE):
            return "onMessage";
        case(K_OPAQUEACTIVITY):
            return "opaqueActivity";
        case(K_OTHERWISE):
            return "otherwise";
        case(K_PARTNER):
            return "partner";
        case(K_PARTNERLINK):
            return "partnerLink";
        case(K_PARTNERLINKS):
            return "partnerLinks";
        case(K_PARTNERS):
            return "partners";
        case(K_PICK):
            return "pick";
        case(K_PROCESS):
            return "process";
        case(K_QUERY):
            return "query";
        case(K_RECEIVE):
            return "receive";
        case(K_REPLY):
            return "reply";
        case(K_REPEATEVERY):
            return "repeatEvery";
        case(K_REPEATUNTIL):
            return "repeatUntil";
        case(K_RETHROW):
            return "rethrow";
        case(K_SCOPE):
            return "scope";
        case(K_SEQUENCE):
            return "sequence";
        case(K_SOURCE):
            return "source";
        case(K_SOURCES):
            return "sources";
        case(K_STARTCOUNTERVALUE):
            return "startCounterValue";
        case(K_SWITCH):
            return "switch";
        case(K_TARGET):
            return "target";
        case(K_TARGETS):
            return "targets";
        case(K_TERMINATE):
            return "terminate";
        case(K_TERMINATIONHANDLER):
            return "terminationHandler";
        case(K_THROW):
            return "throw";
        case(K_TO):
            return "to";
        case(K_TOPART):
            return "toPart";
        case(K_TOPARTS):
            return "toParts";
        case(K_TRANSITIONCONDITION):
            return "transitionCondition";
        case(K_UNTIL):
            return "until";
        case(K_VALIDATE):
            return "validate";
        case(K_VARIABLE):
            return "variable";
        case(K_VARIABLES):
            return "variables";
        case(K_WAIT):
            return "wait";
        case(K_WHILE):
            return "while";

        default:
            return "unknown"; /* should not happen */
    }
}





/*!
 * \return  true iff the activity is structured
 */
bool ASTE::structured() {
    switch (type) {
        case(K_FLOW):
        case(K_FOREACH):
        case(K_IF):
        case(K_PICK):
        case(K_PROCESS):
        case(K_REPEATUNTIL):
        case(K_SCOPE):
        case(K_SEQUENCE):
        case(K_WHILE):
            return true;
    }

    return false;
}





/*!
 * \brief   experimental dot output of the ASTE
 *
 * \todo    comment me
 */
void ASTE::output() {
    if (drawn) {
        return;
    }

    drawn = true;

    // arcs to parents
    if (activityTypeName() != "link") {
        cout << id << " [label=\"" << id << " " << activityTypeName() << "\"]" << endl;

        if (id != 1) {
            cout << parentActivityId << " -> " << id << endl;
        }
    }

    // arcs for links
    if (activityTypeName() == "link") {
        cout << sourceActivity << " -> " << targetActivity << "[label = \"" << id << " link\" color=blue]" << endl;
    }


    // arcs for compensation
    if (activityTypeName() == "compensateScope") {
        cout << id << " -> " << globals::ASTEmap[globals::ASTE_scopeNames[attributes["target"]]]->enclosedCH << "[color=green]" << endl;
    }

    if (activityTypeName() == "compensate") {
        for (set<unsigned int>::iterator it = globals::ASTEmap[parentScopeId]->enclosedScopes.begin(); it != globals::ASTEmap[parentScopeId]->enclosedScopes.end(); it++) {
            cout << id << " -> " << globals::ASTEmap[*it]->enclosedCH << "[color=green]" << endl;
        }
    }


    // arcs for faults
    if (activityTypeName() == "throw") {
        cout << id << " -> " << globals::ASTEmap[parentScopeId]->enclosedFH << "[color=red]" << endl;
    }
    if (activityTypeName() == "rethrow" && parentScopeId != 1) {
        cout << id << " -> " << globals::ASTEmap[globals::ASTEmap[globals::ASTEmap[parentScopeId]->id]->parentScopeId]->enclosedFH << "[color=red]" << endl;
    }


    // process children
    for (set<unsigned int>::iterator it = enclosedActivities.begin(); it != enclosedActivities.end(); it++) {
        globals::ASTEmap[*it]->output();
    }
}





/*****************************************************************************
 * Class pPartnerLink                                                        *
 *****************************************************************************/

/*!
 *  \brief constructor
 *
 *  Initializes the PartnerLink information.
 *
 *  \param n the name of the PartnerLink
 *  \param type the partnerLinkType
 *  \param me my role
 *  \param you the role of the partner
 *
 */
pPartnerLink::pPartnerLink(string n, string type, string me, string you) :
    name(n), partnerLinkType(type), myRole(me), partnerRole(you) {
    /* nothing more to be done */
}





/*!
 *  Implements the semantical matching of PartnerLinks
 *
 *  \param pl the partnerLink which should be checked for matching
 *
 *  \return true iff partnerLinkType matches and myRole und partnerRole correspond accordingly
 *
 */
bool pPartnerLink::operator==(pPartnerLink& pl) {
    return (pl.partnerLinkType == partnerLinkType && pl.myRole == partnerRole && pl.partnerRole == myRole);
}
