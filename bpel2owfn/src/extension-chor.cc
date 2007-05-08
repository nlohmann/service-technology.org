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
 * \file    extension-chor.cc
 *
 * \brief   BPEL4Chor extension
 *  
 * \author  Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          last changes of: \$Author: nielslohmann $
 *
 * \since   2007/04/30
 *
 * \date    \$Date: 2007/05/08 14:18:56 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \version \$Revision: 1.12 $
 */





#include <iostream>

#include "extension-chor.h"
#include "debug.h"
#include "helpers.h"
#include "globals.h"
#include "ast-details.h"


extern int frontend_lineno;





BPEL4Chor_forEach::BPEL4Chor_forEach(map<string, string> &attribute_map)
{
  name = attribute_map["forEach"];
  count = toUInt(attribute_map["count"]);
  iterator_participant_name = attribute_map["name"];

  if (count == UINT_MAX)
    std::cerr << "no count given" << std::endl;
}





BPEL4Chor_MessageLink::BPEL4Chor_MessageLink(map<string, string> &attribute_map)
{
  name = attribute_map["name"];
  sender = attribute_map["sender"];
  receiver = attribute_map["receiver"];
  messageName = attribute_map["messageName"];

  id = (attribute_map["name"] != "") ?
    attribute_map["name"] :
    attribute_map["messageName"];

  sendActivity = (attribute_map["sendActivity"] != "") ?
    attribute_map["sendActivity"] :
    attribute_map["sendActivities"];

  receiveActivity = (attribute_map["receiveActivity"] != "") ?
    attribute_map["receiveActivity"] :
    attribute_map["receiveActivities"];

  forEach = NULL;
  for (map<string, BPEL4Chor_forEach*>::const_iterator forEach_it = globals::BPEL4ChorInfo.forEachs.begin();
      forEach_it != globals::BPEL4ChorInfo.forEachs.end(); forEach_it++)
  {
    if (forEach_it->second->iterator_participant_name == receiver ||
	forEach_it->second->iterator_participant_name == sender)
      forEach = forEach_it->second;
  }
}





/*!
 * \param  attribute_map  an attribute mapping provided by the BPEL4Chor parser
 */
void BPEL4Chor::add_participantType(map<string, string> &attribute_map)
{
  string participantType_name = attribute_map["name"];
  string participantBehaviorDescription_name = attribute_map["participantBehaviorDescription"];

  // if participant type was already defined before, display a warning
  if (participantTypes[participantType_name] != "")
    genericError(134, participantType_name, toString(frontend_lineno), ERRORLEVER_WARNING);

  participantTypes[participantType_name] = participantBehaviorDescription_name;
}





/*!
 * \param  attribute_map  an attribute mapping provided by the BPEL4Chor parser
 */
void BPEL4Chor::add_participant(map<string, string> &attribute_map)
{
  string participant_name = attribute_map["name"];
  string participantType_name = attribute_map["type"];
  string forEach_name = attribute_map["forEach"];

  // if participant type was not defined before, display an error
  if (participantTypes[participantType_name] == "")
    genericError(135, "BPEL4Chor <participantType> `" + participantType_name +
	"' referenced by <participant> `" + participant_name + "' not defined before",
	toString(frontend_lineno), ERRORLEVER_WARNING);

  // store the forEach
  if (forEach_name != "")
    forEachs[forEach_name] = new BPEL4Chor_forEach(attribute_map);

  // reset name, count, and forEach attribute -- the type attribute is not touched
  attribute_map["name"] = "";
  attribute_map["forEach"] = "";
  attribute_map["count"] = "";
}





/*!
 * \param  attribute_map  an attribute mapping provided by the BPEL4Chor parser
 */
void BPEL4Chor::add_messageLink(map<string, string> &attribute_map)
{
  BPEL4Chor_MessageLink *temp = new BPEL4Chor_MessageLink(attribute_map);
  messageLinks[temp->id] = temp;
}





/*!
 * \param  ASTE_id  the AST id of an activity
 *
 * \return the name of the message link that can be used to create a channel
 *         name
 */
string BPEL4Chor::channel_name(unsigned int ASTE_id) const
{
  assert(globals::ASTEmap[ASTE_id] != NULL);

  string activity_name = (globals::ASTEmap[ASTE_id]->attributes["id"] != "") ?
    globals::ASTEmap[ASTE_id]->attributes["id"] :
    globals::ASTEmap[ASTE_id]->attributes["name"];

  // if no activity name is given, display an error
  if (activity_name == "")
  {
    genericError(132, globals::ASTEmap[ASTE_id]->activityTypeName(),
	globals::ASTEmap[ASTE_id]->attributes["referenceLine"], ERRORLEVEL_ERROR);
    return "";
  }

  // search for the message link
  for (map<string, BPEL4Chor_MessageLink*>::const_iterator messageLink = messageLinks.begin();
      messageLink != messageLinks.end(); messageLink++)
  {
    if (messageLink->second->sendActivity == activity_name)
      return messageLink->first;

    if (messageLink->second->receiveActivity == activity_name)
      return messageLink->first;
  }

  // if no message link was found, display an error
  genericError(131, "activity id or name `" + activity_name + "' of <" +
      globals::ASTEmap[ASTE_id]->activityTypeName() + "> does not reference a BPEL4Chor <messageLink>",
      globals::ASTEmap[ASTE_id]->attributes["referenceLine"], ERRORLEVEL_ERROR);
  return "";
}





/*!
 * \param  ASTE_id  the AST id of an activity
 *
 * \todo   make me const
 */
unsigned int BPEL4Chor::forEach_count(unsigned int ASTE_id)
{
  assert(globals::ASTEmap[ASTE_id] != NULL);

  string forEach_name = (globals::ASTEmap[ASTE_id]->attributes["id"] != "") ?
    globals::ASTEmap[ASTE_id]->attributes["id"] :
    globals::ASTEmap[ASTE_id]->attributes["name"];

  if (forEachs[forEach_name] == NULL)
  {
    return UINT_MAX;
    genericError(136, forEach_name, globals::ASTEmap[ASTE_id]->attributes["referenceLine"], ERRORLEVER_WARNING);
  }
  else
    return forEachs[forEach_name]->count;
}





unsigned int BPEL4Chor::channel_count(unsigned int ASTE_id, bool sending)
{
  BPEL4Chor_MessageLink* messageLink = messageLinks[channel_name(ASTE_id)];
  
  // if messageLink was not found, display an error
  if (messageLink == NULL)
  {
    std::cerr << "messageLink not found" << std::endl;
    return 0;
  }

  if (messageLink->forEach == NULL)
    return 0;

  // activity is sending messages to a receiver nested in a forEach
  if (sending && (messageLink->forEach->iterator_participant_name == messageLink->receiver))
    return messageLink->forEach->count;

  // activity is receiving messages from a sender nested in a forEach
  if (!sending && (messageLink->forEach->iterator_participant_name == messageLink->sender))
    return messageLink->forEach->count;

  return 0;
}





void BPEL4Chor::print_information() const
{
  std::cerr << messageLinks.size() << " message links, " <<
    participantTypes.size() << " participant types, " <<
    forEachs.size() << " forEach participants" << std::endl;
}
