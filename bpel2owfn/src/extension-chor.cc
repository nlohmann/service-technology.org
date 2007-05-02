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
 * \date    \$Date: 2007/05/02 15:22:57 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \version \$Revision: 1.2 $
 *
 * \todo    Comment me!
 */





#include <iostream>
#include "extension-chor.h"

void Choreography::addMessageLink(string messageLink_name, string sendActivity_name, string receiveActivity_name)
{
  messageLinks[messageLink_name] = pair<string, string>(sendActivity_name, receiveActivity_name);
}





string Choreography::channelName(string activity_name, bool sending)
{
  for (map<string, pair<string, string> >::iterator messageLink = messageLinks.begin(); messageLink != messageLinks.end(); messageLink++)
  {
    if (messageLink->second.first == activity_name)
      if (sending)
	return messageLink->second.second;
      else
	/* error */;

    if (messageLink->second.second == activity_name)
      if (!sending)
	return messageLink->second.second;
      else
	/* error */;
  }

  /* error */
  return "";
}
