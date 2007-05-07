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
 * \date    \$Date: 2007/05/07 16:00:49 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \version \$Revision: 1.11 $
 */





#include <iostream>

#include "extension-chor.h"
#include "debug.h"
#include "helpers.h"
#include "globals.h"
#include "ast-details.h"


extern int frontend_lineno;





/*!
 * \param  attribute_map  an attribute mapping provided by the BPEL4Chor parser
 */
void Choreography::add_participantType(map<string, string> &attribute_map)
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
void Choreography::add_participant(map<string, string> &attribute_map)
{
  string participant_name = attribute_map["name"];
  string participantType_name = attribute_map["type"];
  string forEach_name = attribute_map["forEach"];

  // if participant type was not defined before, display an error
  if (participantTypes[participantType_name] == "")
    genericError(135, "BPEL4Chor <participantType> `" + participantType_name +
	"' referenced by <participant> `" + participant_name + "' not defined before",
	toString(frontend_lineno), ERRORLEVER_WARNING);

  if (forEach_name != "")
    forEach_participants[forEach_name] = participant_name;

  // reset name and forEach attribute -- the type attribute is not touched
  attribute_map["name"] = "";
  attribute_map["forEach"] = "";
}





/*!
 * \param  attribute_map  an attribute mapping provided by the BPEL4Chor parser
 */
void Choreography::add_messageLink(map<string, string> &attribute_map)
{
  string messageLink_name = (attribute_map["name"] != "") ?
    attribute_map["name"] :
    attribute_map["messageName"];

  string messageLink_sender = (attribute_map["sendActivity"] != "") ?
    attribute_map["sendActivity"] :
    attribute_map["sendActivities"];

  string messageLink_receiver = (attribute_map["receiveActivity"] != "") ?
    attribute_map["receiveActivity"] :
    attribute_map["receiveActivities"];

  // std::cerr << "<messageLink> `" << messageLink_name << "' between `" << messageLink_sender << "' and `" << messageLink_receiver << "'" << std::endl;

  messageLinks[messageLink_name] = pair<string, string>(messageLink_sender, messageLink_receiver);
}





/*!
 * \param  ASTE_id  the AST id of an activity
 *
 * \return the name of the message link that can be used to create a channel
 *         name
 */
string Choreography::find_channel(unsigned int ASTE_id) const
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
  for (map<string, pair<string, string> >::const_iterator messageLink = messageLinks.begin();
      messageLink != messageLinks.end(); messageLink++)
  {
    if (messageLink->second.first == activity_name)
      return messageLink->first;

    if (messageLink->second.second == activity_name)
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
string Choreography::find_forEach(unsigned int ASTE_id)
{
  assert(globals::ASTEmap[ASTE_id] != NULL);

  string forEach_name = (globals::ASTEmap[ASTE_id]->attributes["id"] != "") ?
    globals::ASTEmap[ASTE_id]->attributes["id"] :
    globals::ASTEmap[ASTE_id]->attributes["name"];

  if (forEach_participants[forEach_name] == "")
    genericError(136, forEach_name, globals::ASTEmap[ASTE_id]->attributes["referenceLine"], ERRORLEVER_WARNING);

  return forEach_participants[forEach_name];
}





void Choreography::print_information() const
{
  std::cerr << messageLinks.size() << " message links, " <<
    participantTypes.size() << " participant types, " <<
    forEach_participants.size() << " forEach participants" << std::endl;
}
