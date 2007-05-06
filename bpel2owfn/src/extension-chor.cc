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
 * \author  Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          last changes of: \$Author: nielslohmann $
 *
 * \since   2007/04/30
 *
 * \date    \$Date: 2007/05/06 11:08:53 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \version \$Revision: 1.7 $
 *
 * \todo    Comment me!
 */





#include <iostream>
#include "extension-chor.h"


void Choreography::addMessageLink(map<string, string> &attribute_map)
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

  std::cerr << "<messageLink> `" << messageLink_name << "' between `" << messageLink_sender << "' and `" << messageLink_receiver << "'" << std::endl;

  messageLinks[messageLink_name] = pair<string, string>(messageLink_sender, messageLink_receiver);
}





string Choreography::channelName(string activity_name) const
{
  for (map<string, pair<string, string> >::const_iterator messageLink = messageLinks.begin(); messageLink != messageLinks.end(); messageLink++)
  {
    if (messageLink->second.first == activity_name)
      return messageLink->first;

    if (messageLink->second.second == activity_name)
      return messageLink->first;
  }

  return "";
}
