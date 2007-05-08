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
 * \file    extension-chor.h
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
 * \version \$Revision: 1.10 $
 */





#ifndef EXTENSION_CHOR_H
#define EXTENSION_CHOR_H


#include <map>
#include <string>
#include <utility>

using std::map;
using std::string;
using std::pair;





class BPEL4Chor_forEach
{
  friend class BPEL4Chor;
  friend class BPEL4Chor_MessageLink;

  private:
    /// name of the forEach (attribute "forEach")
    string name;

    /// maximal number of iterations (attribute "count")
    unsigned int count;

    /// iterator partcipant name
    string iterator_participant_name;

    /// constructor
    BPEL4Chor_forEach(map<string, string> &attribute_map);
};





/*!
 * \class BPEL4Chor_MessageLink
 *
 * A BPEL4Chor messageLink.
 */
class BPEL4Chor_MessageLink
{
  friend class BPEL4Chor;

  private:
    /// the identifier of the messageLink (either attribute "id" or "name")
    string id;

    /// attribute "name"
    string name;

    /// attribute "sender" or "senders"
    string sender;

    /// attribute "sendActivity" or "sendActivties"
    string sendActivity;

    /// attribute "receiver" or "receivers"
    string receiver;

    /// attribute "receiveActivity" or "receiveActivities"
    string receiveActivity;

    /// attribute "messageName"
    string messageName;

    /// a pointer to a forEach if an iterator participant is involved
    BPEL4Chor_forEach *forEach;

    /// constructor
    BPEL4Chor_MessageLink(map<string, string> &attribute_map);
};





/*!
 * \class   BPEL4Chor
 *
 * This class organizes informations read from a BPEL4Chor topology file.
 */
class BPEL4Chor
{
  friend class BPEL4Chor_forEach;
  friend class BPEL4Chor_MessageLink;

  private:
    /// the messageLinks, indexed by their name
    map<string, BPEL4Chor_MessageLink*> messageLinks;

    /// the participantTypes, indexed by their name
    map<string, string> participantTypes;

    /// the forEachs, indexed by their name
    map<string, BPEL4Chor_forEach*> forEachs;

  public:
    /// add a <participantType>
    void add_participantType(map<string, string> &attribute_map);

    /// add a <participant>
    void add_participant(map<string, string> &attribute_map);

    /// add a <messageLink>
    void add_messageLink(map<string, string> &attribute_map);

    /// find a channel name given an activity id or name
    string channel_name(unsigned int ASTE_id) const;

    /// find a forEach name given an activity id or name
    unsigned int forEach_count(unsigned int ASTE_id);

    /// returns the number of times a channel is used
    unsigned int channel_count(unsigned int ASTE_id, bool sending);

    /// prints information about the BPEL4Chor topology
    void print_information() const;
};


#endif
