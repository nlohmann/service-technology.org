/*****************************************************************************\
  GNU BPEL2oWFN -- Translating BPEL Processes into Petri Net Models

  Copyright (C) 2006, 2007  Niels Lohmann,
                            Christian Gierds, and
                            Martin Znamirowski
  Copyright (C) 2005        Niels Lohmann and
			    Christian Gierds

  GNU BPEL2oWFN is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the Free
  Software Foundation; either version 3 of the License, or (at your option) any
  later version.

  GNU BPEL2oWFN is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
  details.

  You should have received a copy of the GNU General Public License along with
  GNU BPEL2oWFN (see file COPYING); if not, see http://www.gnu.org/licenses
  or write to the Free Software Foundation,Inc., 51 Franklin Street, Fifth
  Floor, Boston, MA 02110-1301  USA.
\*****************************************************************************/

%{
/*!
 * \file    petrinet-patterns-basic.cc
 *
 * \brief   Petri net patterns for BPEL's basic activities
 *
 *          This file implements the patterns for BPEL's basic activities,
 *          namely:
 *            - <empty>
 *            - <invoke> (asynchronous and synchronous)
 *            - <receive>
 *            - <reply>
 *            - <assign>
 *            - <validate>
 *            - <wait> (deadline and duration)
 *            - <throw>
 *            - <rethrow>
 *            - <exit>
 *            - <compensate>
 *            - <compensateScope>
 *
 * \author  Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          Christian Gierds <gierds@informatik.hu-berlin.de>,
 *          last changes of: \$Author: nielslohmann $ 
 * 
 * \since   2006-11-26
 *
 * \date    \$Date: 2007/06/28 07:38:17 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \note    This file was created using Kimwitu++ reading file
 *          petrinet-patterns-basic.k.
 *          See http://site.informatik.hu-berlin.de/kimwitu++ for details.
 *
 * \version \$Revision: 1.56 $
 */
%}


%{ HEADER
/*!
 * \file petrinet-patterns-basic.h
 * \brief Petri net patterns for BPEL's basic activities
 *
 * See \ref petrinet-patterns-basic.cc for more information.
 */
%}

%{
#include "ast-config.h"
#include "helpers.h"
#include "ast-tools.h"
#include "ast-details.h"
#include "globals.h"
#include "petrinet-patterns-shortcuts.h"
#include "extension-chor.h"

extern PetriNet PN;
%}




/******************************************************************************
  EMPTY
******************************************************************************/

%{
/*!
 * \fn      void kc::PEmpty(impl_tEmpty *abstract_phylum, uview current_view)
 *
 * \brief   Pattern for the <empty> activity.
 *
 *          \image html "empty.png" "pattern"
 *
 * \param   abstract_phylum  pointer to the phylum of the abstract syntax tree
 * \param   current_view     the current unparse view
 *
 * \ingroup patterns
 */
%}
void PEmpty(impl_tEmpty* abstract_phylum, uview current_view)
{
  impl_tEmpty_Empty* phylum = static_cast<impl_tEmpty_Empty*>(abstract_phylum);
  assert(phylum != NULL);

  header(phylum->id);
  string prefix = toString(phylum->id) + ".internal.";  

  // Places.
  Place *p1 = PN.newPlace(prefix + "initial");
  Place *p2 = PN.newPlace(prefix + "final");

  // A transition modelling the <empty> activity.
  Transition *t1 = PN.newTransition(prefix + "empty");
  PN.newArc(p1, t1);
  PN.newArc(t1, p2);

  // Stop places and stopping transitions.
  if (current_view != petrinetsmall_enum)
  {
    Place *p3 = PN.newPlace(prefix + "stop");
    Place *p4 = PN.newPlace(prefix + "stopped");
    Transition *t2 = stop(p1, "initial", prefix);
    Transition *t3 = stop(p2, "final", prefix);
  }

  // Unparse the standard elements.
  phylum->standardElements_1->unparse(pseudoPrinter, current_view);

  footer(phylum->id);
}





/******************************************************************************
  OPAQUE ACTIVITY
******************************************************************************/

%{
/*!
 * \fn      void kc::POpaqueActivity(impl_tOpaqueActivity *abstract_phylum, uview current_view)
 *
 * \brief   Pattern for the <opaqueActivity> activity.
 *
 * \param   abstract_phylum  pointer to the phylum of the abstract syntax tree
 * \param   current_view     the current unparse view
 *
 * \note    This is the same pattern as used for the <empty> activity.
 *
 * \ingroup patterns
 */
%}
void POpaqueActivity(impl_tOpaqueActivity* abstract_phylum, uview current_view)
{
  impl_tOpaqueActivity_OpaqueActivity* phylum = static_cast<impl_tOpaqueActivity_OpaqueActivity*>(abstract_phylum);
  assert(phylum != NULL);

  header(phylum->id);
  string prefix = toString(phylum->id) + ".internal.";  

  // Places.
  Place *p1 = PN.newPlace(prefix + "initial");
  Place *p2 = PN.newPlace(prefix + "final");

  // A transition modelling the <opaqueActivity> activity.
  Transition *t1 = PN.newTransition(prefix + "opaqueActivity");
  PN.newArc(p1, t1);
  PN.newArc(t1, p2);

  // Stop places and stopping transitions.
  if (current_view != petrinetsmall_enum)
  {
    Place *p3 = PN.newPlace(prefix + "stop");
    Place *p4 = PN.newPlace(prefix + "stopped");
    Transition *t2 = stop(p1, "initial", prefix);
    Transition *t3 = stop(p2, "final", prefix);
  }

  // Unparse the standard elements.
  phylum->standardElements_1->unparse(pseudoPrinter, current_view);

  footer(phylum->id);
}





/******************************************************************************
  INVOKE

  The <invoke> activity has two patterns: one for the asynchronous version
  and one for the synchronous version.
******************************************************************************/

%{
/*!
 * \fn      void kc::PAsynchronousInvoke(impl_tInvoke* abstract_phylum, uview current_view)
 *
 * \brief   Pattern for asynchronous <invoke> activity.
 *
 *          \image html "invoke_a.png" "pattern"
 *
 * \param   abstract_phylum  pointer to the phylum of the abstract syntax tree
 * \param   current_view     the current unparse view
 *
 * \ingroup patterns
 */
%}
void PAsynchronousInvoke(impl_tInvoke* abstract_phylum, uview current_view)
{
  impl_tInvoke_AsynchronousInvoke* phylum =
  static_cast<impl_tInvoke_AsynchronousInvoke*>(abstract_phylum);
  assert(phylum != NULL);
  
  header(phylum->id);
  string prefix = toString(phylum->id) + ".internal.";
  
  ASTE *myself = globals::ASTEmap[phylum->id];
  assert(myself != NULL);
  
  switch(current_view)
  {
    case(petrinetnew_enum):
    {
      // Places.
      Place *p1 = PN.newPlace(prefix + "initial");
      Place *p2 = PN.newPlace(prefix + "final");
      Place *p3 = PN.newPlace(prefix + "stop");
      Place *p4 = PN.newPlace(prefix + "stopped");
      Place *p5 = PN.newPlace(prefix + "faulted");
      
      // A subnet sending a message (replacement of t1).
      sendingTransition("invoke", p1, p2,
                        PN.findPlace("out." + myself->channelName),
                        p5, prefix, mkinteger(phylum->id),
                        myself->controlFlow);
      
      // Stopping transitions.
      Transition *t2 = stop(p1, "initial", prefix);
      Transition *t3 = stop(p2, "final", prefix);
      Transition *t4 = stop(p5, "faulted", prefix);
      
      break;
    }
      
    case(petrinetsmall_enum):
    {
      // Places.
      Place *p1 = PN.newPlace(prefix + "initial");
      Place *p2 = PN.newPlace(prefix + "final");
      
      // Set the name of the interface place.
      string interface_place_name = "out." + myself->channelName;
      
      // Add an instance suffix when necessary.            
      if (globals::instances_of_current_process == 0 &&
          myself->messageLink != NULL &&
          (myself->messageLink->receiver_is_iterator ||
           myself->messageLink->sender_is_iterator ||
           myself->messageLink->sender_is_unique))
      {
        interface_place_name += ".instance_" + toString(myself->channel_instances++);
      }
      
      // Receive message from all unique senders.
      if (myself->messageLink != NULL &&
          myself->messageLink->receiver_is_unique)
      {
        for (unsigned int i = 1; i <= myself->messageLink->participantSet->count; i++)
        {
          Transition *t1 = PN.newTransition(prefix + "invoke" + toString(i));
          PN.newArc(t1, PN.findPlace(interface_place_name + ".instance_" + toString(i)));
          PN.newArc(p1, t1);
          PN.newArc(t1, p2);          
        }
      }
      else
      {
        // Create a "normal" message sending transition.
        Transition *t1 = PN.newTransition(prefix + "invoke");
        PN.newArc(t1, PN.findPlace(interface_place_name));
        PN.newArc(p1, t1);
        PN.newArc(t1, p2);
      }
      
      break;
    }
      
    default: { /* no pattern in this case */ }      
  }
  
  // Unparse the standard elements.
  phylum->standardElements_1->unparse(pseudoPrinter, current_view);
  
  footer(phylum->id);
}


%{
/*!
 * \fn      void kc::PSynchronousInvoke(impl_tInvoke* abstract_phylum, uview current_view)
 *
 * \brief   Pattern for the synchronous <invoke> activity.
 *
 *          \image html "invoke_s.png" "pattern"
 *
 * \param   abstract_phylum  pointer to the phylum of the abstract syntax tree
 * \param   current_view     the current unparse view
 *
 * \ingroup patterns
 */
%}
void PSynchronousInvoke(impl_tInvoke* abstract_phylum, uview current_view)
{
  impl_tInvoke_SynchronousInvoke* phylum =
    static_cast<impl_tInvoke_SynchronousInvoke*>(abstract_phylum);
  assert(phylum != NULL);

  header(phylum->id);
  string prefix = toString(phylum->id) + ".internal.";

  ASTE *myself = globals::ASTEmap[phylum->id];
  assert(myself != NULL);

  switch(current_view)
  {
    case(petrinetnew_enum):
    {
      // Places.
      Place *p1 = PN.newPlace(prefix + "initial");
      Place *p2 = PN.newPlace(prefix + "running");
      Place *p3 = PN.newPlace(prefix + "final");
      Place *p4 = PN.newPlace(prefix + "stop");
      Place *p5 = PN.newPlace(prefix + "stopped");
      Place *p6 = PN.newPlace(prefix + "faulted");

      // Set the name of the (output) interface place.
      string interface_place_name = "out." + myself->channelName;

      // Set the name of the (input) interface place.
      string interface_place_name2 = "in." + myself->channelName2;

      // A subnet sending a message (replacement of t1).
      sendingTransition("invoke_out",  p1, p2,
                        PN.findPlace(interface_place_name),
                        p6, prefix, mkinteger(phylum->id),
                        myself->controlFlow);

      // A subnet receiving a message (replacement of t2).
      receivingTransition("invoke_in", p2, p3,
                          PN.findPlace(interface_place_name2),
                          p6, prefix, mkinteger(phylum->id),
                          myself->controlFlow);

      // Stopping transitions.
      Transition *t3 = stop(p1, "initial", prefix);
      Transition *t4 = stop(p2, "running", prefix);
      Transition *t5 = stop(p3, "final", prefix);
      Transition *t6 = stop(p6, "faulted", prefix);

      break;
    }

    case(petrinetsmall_enum):
    {
      // Places.
      Place *p1 = PN.newPlace(prefix + "initial");
      Place *p2 = PN.newPlace(prefix + "running");
      Place *p3 = PN.newPlace(prefix + "final");

      // A transition for the outgoing message.
      Transition *t1 = PN.newTransition(prefix + "invokeO");
      PN.newArc(t1, PN.findPlace("out." + myself->channelName));
      PN.newArc(p1, t1);
      PN.newArc(t1, p2);

      // A transition for the incoming message.
      Transition *t2 = PN.newTransition(prefix + "invokeI");
      PN.newArc(PN.findPlace("in." + myself->channelName2), t2);
      PN.newArc(p2, t2);
      PN.newArc(t2, p3);

      break;
    }

    default: { /* no pattern in this case */ }
  }

  // Unparse the standard elements.
  phylum->standardElements_1->unparse(pseudoPrinter, current_view);

  footer(phylum->id);
}





/******************************************************************************
  RECEIVE
******************************************************************************/

%{
/*!
 * \fn      void kc::PReceive(impl_tReceive* abstract_phylum, uview current_view)
 *
 * \brief   Pattern for the <receive> activity.
 * 
 *          \image html "receive.png" "pattern"
 *
 * \param   abstract_phylum  pointer to the phylum of the abstract syntax tree
 * \param   current_view     the current unparse view
 *
 * \ingroup patterns
 */
%}
void PReceive(impl_tReceive* abstract_phylum, uview current_view)
{
  impl_tReceive_Receive* phylum =
    static_cast<impl_tReceive_Receive*>(abstract_phylum);
  assert(phylum != NULL);

  header(phylum->id);
  string prefix = toString(phylum->id) + ".internal.";
  
  ASTE *myself = globals::ASTEmap[phylum->id];
  assert(myself != NULL);
  
  switch(current_view)
  {
    case(petrinetnew_enum):
    {
      // Places.
      Place *p1 = PN.newPlace(prefix + "initial");
      Place *p2 = PN.newPlace(prefix + "final");
      Place *p3 = PN.newPlace(prefix + "stop");
      Place *p4 = PN.newPlace(prefix + "stopped");
      Place *p5 = PN.newPlace(prefix + "faulted");

      // A subnet receiving a message (replacement of t1).
      receivingTransition("receive", p1, p2,
                          PN.findPlace("in." + myself->channelName),
                          p5, prefix, mkinteger(phylum->id),
                          myself->controlFlow);

      // Stopping transitions.
      Transition *t2 = stop(p1, "initial", prefix);
      Transition *t3 = stop(p2, "final", prefix);
      Transition *t4 = stop(p5, "faulted", prefix);
      
      break;
    }

    case(petrinetsmall_enum):
    {
      // Places.
      Place *p1 = PN.newPlace(prefix + "initial");
      Place *p2 = PN.newPlace(prefix + "final");

      // Set the name of the interface place.
      string interface_place_name = "in." + myself->channelName;

      // Add an instance suffix when necessary.            
      if (globals::instances_of_current_process == 0 &&
           myself->messageLink != NULL &&
          (myself->messageLink->receiver_is_iterator ||
           myself->messageLink->sender_is_iterator ||
           myself->messageLink->receiver_is_unique))
      {
        interface_place_name += ".instance_" + toString(myself->channel_instances++);
      }
      
      // Receive message from all unique senders.
      if (myself->messageLink != NULL &&
          myself->messageLink->sender_is_unique)
      {
        for (unsigned int i = 1; i <= myself->messageLink->participantSet->count; i++)
        {
          Transition *t1 = PN.newTransition(prefix + "receive" + toString(i));
          PN.newArc(PN.findPlace(interface_place_name + ".instance_" + toString(i)), t1);
          PN.newArc(p1, t1);
          PN.newArc(t1, p2);          
        }
      }
      else
      {
        // Create a "normal" message receiving transition.
        Transition *t1 = PN.newTransition(prefix + "receive");
        PN.newArc(PN.findPlace(interface_place_name), t1);
        PN.newArc(p1, t1);
        PN.newArc(t1, p2);
      }

      break;
    }

    default: { /* no pattern in this case */ }      
  }

  // Unparse the standard elements.
  phylum->standardElements_1->unparse(pseudoPrinter, current_view);

  footer(phylum->id);      
}




/******************************************************************************
  REPLY
******************************************************************************/

%{
/*!
 * \fn      void kc::PReply(impl_tReply* abstract_phylum, uview current_view)
 *
 * \brief   Pattern for the <reply> activity.
 *
 *          \image html "reply.png" "pattern"
 *
 * \param   abstract_phylum  pointer to the phylum of the abstract syntax tree
 * \param   current_view     the current unparse view
 *
 * \ingroup patterns
 */
%}
void PReply(impl_tReply* abstract_phylum, uview current_view)
{
  impl_tReply_Reply* phylum = static_cast<impl_tReply_Reply*>(abstract_phylum);
  assert(phylum != NULL);
  
  header(phylum->id);
  string prefix = toString(phylum->id) + ".internal.";  
  
  ASTE *myself = globals::ASTEmap[phylum->id];
  assert(myself != NULL);
  
  switch(current_view)
  {
    case(petrinetnew_enum):
    {
      // Places.
      Place *p1 = PN.newPlace(prefix + "initial");
      Place *p2 = PN.newPlace(prefix + "final");
      Place *p3 = PN.newPlace(prefix + "stop");
      Place *p4 = PN.newPlace(prefix + "stopped");
      Place *p5 = PN.newPlace(prefix + "faulted");
      
      // A subnet sending a message (replacement of t1).
      sendingTransition("reply", p1, p2,
                        PN.findPlace("out." + myself->channelName),
                        p5, prefix, mkinteger(phylum->id),
                        myself->controlFlow);
      
      // Stopping transitions.
      Transition *t2 = stop(p1, "initial", prefix);
      Transition *t3 = stop(p2, "final", prefix);
      Transition *t4 = stop(p5, "faulted", prefix);
      
      break;
    }
      
    case(petrinetsmall_enum):
    {
      // Places.
      Place *p1 = PN.newPlace(prefix + "initial");
      Place *p2 = PN.newPlace(prefix + "final");
      
      // Set the name of the interface place.
      string interface_place_name = "out." + myself->channelName;
      if (myself->messageLink != NULL &&
          myself->messageLink->receiver_is_iterator)
        interface_place_name += ".instance_" + toString(myself->channel_instances++);
      
      // send a message from one unique instance
      if (myself->messageLink != NULL &&
          myself->messageLink->receiver_is_unique)
      {
        for (unsigned int i = 1; i <= myself->messageLink->participantSet->count; i++)
        {
          Transition *t1 = PN.newTransition(prefix + "reply" + toString(i));
          PN.newArc(t1, PN.findPlace(interface_place_name + ".instance_" + toString(i)));
          PN.newArc(p1, t1);
          PN.newArc(t1, p2);
        }
        break;
      }
      
      // A transition sending a message.
      Transition *t1 = PN.newTransition(prefix + "reply");
      PN.newArc(t1, PN.findPlace(interface_place_name));
      PN.newArc(p1, t1);
      PN.newArc(t1, p2);
      
      break;
    }
      
    default: { /* no pattern in this case */ }      
  }
  
  // Unparse the standard elements.
  phylum->standardElements_1->unparse(pseudoPrinter, current_view);
  
  footer(phylum->id);
}





/******************************************************************************
  ASSIGN
******************************************************************************/

%{
/*!
 * \fn      void kc::PAssign(impl_tAssign* abstract_phylum, uview current_view)
 *
 * \brief   Pattern for the <assign> activity.
 *
 *          \image html "assign.png" "pattern"
 *
 * \param   abstract_phylum  pointer to the phylum of the abstract syntax tree
 * \param   current_view     the current unparse view
 *
 * \ingroup patterns
 * 
 * \todo    Overwork this pattern and adjust it to the new semantics.
 */
%}
void PAssign(impl_tAssign* abstract_phylum, uview current_view)
{
  impl_tAssign_Assign* phylum =
  static_cast<impl_tAssign_Assign*>(abstract_phylum);
  assert(phylum != NULL);
  
  header(phylum->id);
  string prefix = toString(phylum->id) + ".internal.";
  
  
  switch (current_view)
  {
    case(petrinetnew_enum):
    {
      // Places.
      Place *p1 = PN.newPlace(prefix + "initial");
      Place *p3 = PN.newPlace(prefix + "final");
      Place *p5 = PN.newPlace(prefix + "stop");
      Place *p6 = PN.newPlace(prefix + "stopped");
      
      Transition *t4 = stop(p1, "initial", prefix);
      Transition *t7 = stop(p3, "final", prefix);
      
      // unparse the standard elements
      phylum->standardElements_1->unparse(pseudoPrinter, current_view);
      
      int copyCount = 1;
      Place *lastPlace = PN.findPlace(prefix + "initial");
      
      foreach (copyBlock=Copy(from,to); tCopy_list phylum->tCopy_list_1)
      {
        string newPrefix = prefix + "copy." + toString(copyCount) + ".";
        
        if (globals::ASTEmap[from->id]->variableName != "")
        {
          Place *p2 = PN.newPlace(newPrefix + "running");
          Place *p4 = PN.newPlace(newPrefix + "p4");
          Place *p8 = PN.newPlace(newPrefix + "final");
          
          Transition *t1 = PN.newTransition(newPrefix + "t1");
          PN.newArc(lastPlace, t1);
          PN.newArc(t1, p2);
          if (globals::parameters[P_VARIABLES])
            PN.newArc(PN.findPlace("variable." + globals::ASTEmap[from->id]->variableName), t1, READ);
          
          Transition *t2 = PN.newTransition(newPrefix + "t2");
          PN.newArc(p2, t2);
          PN.newArc(t2, p8);
          if (globals::parameters[P_VARIABLES])
          {
            PN.newArc(PN.findPlace("variable." + globals::ASTEmap[to->id]->variableName), t2);
            PN.newArc(t2, PN.findPlace("variable." + globals::ASTEmap[to->id]->variableName));
          }
          
          // Transitions for the negative control flow.
          if (globals::parameters[P_STANDARDFAULTS])
            Transition *t3 = throwFault(p2, p4, "running", newPrefix,
                                        mkinteger(phylum->id), globals::ASTEmap[phylum->id]->controlFlow);
          
          Transition *t6 = stop(p4, "p4at" + toString(copyCount), prefix);
          
          lastPlace = p8;
        }
        else
        {
          Place *p4 = PN.newPlace(newPrefix + "p4");
          Place *p8 = PN.newPlace(newPrefix + "final");
          
          Transition *t2 = PN.newTransition(newPrefix + "t2");
          PN.newArc(lastPlace, t2);
          PN.newArc(t2, p8);
          if (globals::parameters[P_VARIABLES])
          {
            PN.newArc(PN.findPlace("variable." + globals::ASTEmap[to->id]->variableName), t2);
            PN.newArc(t2, PN.findPlace("variable." + globals::ASTEmap[to->id]->variableName));
          }
          
          // Transitions for the negative control flow.
          if (globals::parameters[P_STANDARDFAULTS])
            Transition *t3 = throwFault(lastPlace, p4, "initial", newPrefix,
                                        mkinteger(phylum->id), globals::ASTEmap[phylum->id]->controlFlow);
          
          Transition *t6 = stop(p4, "p4at" + toString(copyCount), prefix);
          
          lastPlace = p8;
        }
        
        copyCount++;
      }
      
      PN.mergePlaces(prefix + "final", lastPlace);
      break;
    }
      
    case (petrinetsmall_enum):
    {
      Place *p1 = PN.newPlace(prefix + "initial");
      Place *p2 = PN.newPlace(prefix + "final");
      
      Transition *t1 = PN.newTransition(prefix + "assign");
      
      PN.newArc(p1, t1);
      PN.newArc(t1, p2);
      
      // unparse the standard elements
      phylum->standardElements_1->unparse(pseudoPrinter, current_view);
      
      break;
    }
      
    default: { /* no pattern in this case */ }
  }
  
  footer(phylum->id);
}





/******************************************************************************
  VALIDATE
******************************************************************************/

%{
/*!
 * \fn      void kc::PValidate(impl_tValidate* abstract_phylum, uview current_view)
 *
 * \brief   Pattern for the <validate> activity.
 *
 *          \image html "validate.png" "pattern"
 *
 * \param   abstract_phylum  pointer to the phylum of the abstract syntax tree
 * \param   current_view     the current unparse view
 *
 * \ingroup patterns
 *
 * \todo    Overwork this pattern and connect it with the respective variable.
 */
%}
void PValidate(impl_tValidate* abstract_phylum, uview current_view)
{
  impl_tValidate_Validate* phylum =
  static_cast<impl_tValidate_Validate*>(abstract_phylum);
  assert(phylum != NULL);
  
  header(phylum->id);
  string prefix = toString(phylum->id) + ".internal.";
  
  switch (current_view)
  {
    case(petrinetnew_enum):
    {
      // Places
      Place *p1 = PN.newPlace(prefix + "initial");
      Place *p2 = PN.newPlace(prefix + "running");
      Place *p3 = PN.newPlace(prefix + "final");
      Place *p4 = PN.newPlace(prefix + "stop");
      Place *p5 = PN.newPlace(prefix + "stopped");
      Place *p6 = PN.newPlace(prefix + "faulted");
      
      Transition *t1 = PN.newTransition(prefix + "read_variable");
      PN.newArc(p1, t1);
      PN.newArc(t1, p2);
      
      Transition *t2 = PN.newTransition(prefix + "continue");
      PN.newArc(p2, t2);
      PN.newArc(t2, p3);
      
      // Transitions for the negative control flow.
      if (globals::parameters[P_STANDARDFAULTS])
      {
        Transition *t3 = throwFault(p2, p6, "running", prefix,
                                    mkinteger(phylum->id),
                                    globals::ASTEmap[phylum->id]->controlFlow);
      }
      
      // Stopping transitions.
      Transition *t4 = stop(p1, "initial", prefix);
      Transition *t5 = stop(p2, "running", prefix);
      Transition *t6 = stop(p3, "final", prefix);
      
      break;
    }
      
    case(petrinetsmall_enum):
    {
      // Places.
      Place *p1 = PN.newPlace(prefix + "initial");
      Place *p2 = PN.newPlace(prefix + "final");
      
      // Bogus validation.
      Transition *t1 = PN.newTransition(prefix + "validate");
      PN.newArc(p1, t1);
      PN.newArc(t1, p2);
      
      break;
    }
      
    default: { /* no pattern in this case */ }
  }
  
  // Unparse the standard elements.
  phylum->standardElements_1->unparse(pseudoPrinter, current_view);
  
  footer(phylum->id);
}





/******************************************************************************
  WAIT
******************************************************************************/

%{
/*!
 * \fn      void kc::PWait(impl_tWait* abstract_phylum, uview current_view)
 *
 * \brief   Pattern for the <wait> activity.
 *
 *          \image html "wait.png" "pattern"
 *
 *          As we do not model time, the <wait> pattern is modeled by the
 *          pattern of the <empty> activity.
 *
 * \param   abstract_phylum  pointer to the phylum of the abstract syntax tree
 * \param   current_view     the current unparse view
 *
 * \ingroup patterns
 */
%}
void PWait(impl_tWait* abstract_phylum, uview current_view)
{
  impl_tWait_Wait* phylum = static_cast<impl_tWait_Wait*>(abstract_phylum);
  assert(phylum != NULL);

  header(phylum->id);
  string prefix = toString(phylum->id) + ".internal.";

  // Places.
  Place *p1 = PN.newPlace(prefix + "initial");
  Place *p2 = PN.newPlace(prefix + "final");

  // A transition modelling the <empty> activity.
  Transition *t1 = PN.newTransition(prefix + "wait");
  PN.newArc(p1, t1);
  PN.newArc(t1, p2);

  // Stop places and stopping transitions.
  if (current_view != petrinetsmall_enum)
  {
    Place *p3 = PN.newPlace(prefix + "stop");
    Place *p4 = PN.newPlace(prefix + "stopped");
    Transition *t2 = stop(p1, "initial", prefix);
    Transition *t3 = stop(p2, "final", prefix);
  }

  // Unparse the standard elements.
  phylum->standardElements_1->unparse(pseudoPrinter, current_view);

  footer(phylum->id);      
}





/******************************************************************************
  THROW
******************************************************************************/

%{
/*!
 * \fn      void kc::PThrow(impl_tThrow* abstract_phylum, uview current_view)
 *
 * \brief   Pattern for the <throw> activity.
 *
 *          \image html "throw.png" "pattern"
 *
 * \param   abstract_phylum  pointer to the phylum of the abstract syntax tree
 * \param   current_view     the current unparse view
 *
 * \note    This pattern is replaced by an <empty> activity in the
 *          "communicationonly" patterns.
 *
 * \ingroup patterns
 *
 * \todo    Handle the outgoing links of the throw activity. Right now, they
 *          are not set at all.
 */
%}
void PThrow(impl_tThrow* abstract_phylum, uview current_view)
{
  impl_tThrow_Throw* phylum = static_cast<impl_tThrow_Throw*>(abstract_phylum);
  assert(phylum != NULL);
  
  header(phylum->id);
  string prefix = toString(phylum->id) + ".internal.";
  assert(globals::ASTEmap[phylum->id] != NULL);
  
  switch(current_view)
  {
    case(petrinetnew_enum):
    {
      // Places. Place "final" is not connected, as the <throw> activity
      // stops the positive control flow. The "fault" place is not modeled as
      // faults are organized by the "throwFault" function.
      Place *p1 = PN.newPlace(prefix + "initial");
      Place *p2 = PN.newPlace(prefix + "faulted");
      Place *p4 = PN.newPlace(prefix + "stop");
      Place *p5 = PN.newPlace(prefix + "stopped");
      Place *p6 = PN.newPlace(prefix + "final");
      
      // Throw a fault.
      Transition *t1 = throwFault(p1, p2, "throw", prefix,
                                  mkinteger(phylum->id),
                                  globals::ASTEmap[phylum->id]->controlFlow);
      
      // Stopping transitions.
      Transition *t2 = stop(p1, "initial", prefix);
      Transition *t3 = stop(p2, "faulted", prefix);
      
      break;
    }
      
    case(petrinetsmall_enum):
    {
      // Model an <empty> activity instead.
      Place *p1 = PN.newPlace(prefix + "initial");
      Place *p2 = PN.newPlace(prefix + "final");
      Transition *t1 = PN.newTransition(prefix + "empty");
      PN.newArc(p1, t1);
      PN.newArc(t1, p2);
      
      break;
    }
      
    default: { /* no pattern in this case */ }
  }
  
  // Unparse the standard elements.
  phylum->standardElements_1->unparse(pseudoPrinter, current_view);
  
  footer(phylum->id);      
}





/******************************************************************************
  RETHROW
******************************************************************************/

%{
/*!
 * \fn      void kc::PRethrow(impl_tRethrow* abstract_phylum, uview current_view)
 *
 * \brief   Pattern for the <rethrow> activity.
 *
 *          \image html "rethrow.png" "pattern"
 * 
 *          The gray place is from the <faultHandlers> pattern.
 *
 * \param   abstract_phylum  pointer to the phylum of the abstract syntax tree
 * \param   current_view     the current unparse view
 *
 * \ingroup patterns
 *
 * \todo    
 *          - Check whether this pattern should also use the "throwFault"
 *            function.
 *          - Handle the outgoing links of the throw activity. Right now, they
 *            are not set at all.
 */
%}
void PRethrow(impl_tRethrow* abstract_phylum, uview current_view)
{
  impl_tRethrow_Rethrow* phylum =
  static_cast<impl_tRethrow_Rethrow*>(abstract_phylum);
  assert(phylum != NULL);
  
  header(phylum->id);
  string prefix = toString(phylum->id) + ".internal.";
  
  // Places.
  Place *p1 = PN.newPlace(prefix + "initial");
  Place *p2 = PN.newPlace(prefix + "faulted");
  Place *p4 = PN.newPlace(prefix + "stop");
  Place *p5 = PN.newPlace(prefix + "stopped");
  Place *p6 = PN.newPlace(prefix + "final");
  
  // Transitions for the negative control flow.
  Transition *t1 = throwFault(p1, p2, "rethrow", prefix,
                              mkinteger(phylum->id),
                              globals::ASTEmap[phylum->id]->controlFlow);
  
  PN.newArc(PN.findPlace(toString(globals::ASTEmap[phylum->id]->parentScopeId) +
                         ".faultHandlers.fault"), t1, READ);
  
  // Stopping transitions.
  Transition *t2 = stop(p1, "initial", prefix);
  Transition *t3 = stop(p2, "faulted", prefix);
  
  // Unparse the standard elements.
  phylum->standardElements_1->unparse(pseudoPrinter, current_view);
  
  footer(phylum->id);
}





/******************************************************************************
  EXIT / TERMINATE
******************************************************************************/

%{
/*!
 * \fn      void kc::PExit(impl_tExit* abstract_phylum, uview current_view)
 *
 * \brief   Pattern for the <exit> activity.
 * 
 *          \image html "exit.png" "pattern"
 *          
 *          The gray places are from the <process> pattern.
 *
 * \param   abstract_phylum  pointer to the phylum of the abstract syntax tree
 * \param   current_view     the current unparse view
 *
 * \note    The patterns for <exit> activity (WS-BPEL 2.0) and the <terminate>
 *          activity (BPEL4WS 1.1) are the same.
 *
 * \note    This pattern is replaced by an <empty> activity in the
 *          "communicationonly" patterns.
 *
 * \ingroup patterns
 */
%}
void PExit(impl_tExit* abstract_phylum, uview current_view)
{
  impl_tExit_Exit* phylum = static_cast<impl_tExit_Exit*>(abstract_phylum);
  assert(phylum != NULL);
  
  header(phylum->id);
  string prefix = toString(phylum->id) + ".internal.";
  
  switch(current_view)
  {
    case(petrinetnew_enum):
    {
      // Places. Place "final" is not connected, as the <exit> activity
      // stops the control flow of the <process>.
      Place *p1 = PN.newPlace(prefix + "initial");
      Place *p2 = PN.newPlace(prefix + "done");
      Place *p3 = PN.newPlace(prefix + "stop");
      Place *p4 = PN.newPlace(prefix + "stopped");
      Place *p5 = PN.newPlace(prefix + "final");
      
      // Signal the <process> termination.
      Transition *t1 = PN.newTransition(prefix + "exit");
      PN.newArc(p1, t1);
      PN.newArc(t1, p2);
      PN.newArc(PN.findPlace("1.internal.!Exiting"), t1);
      PN.newArc(t1, PN.findPlace("1.internal.Exiting"));
      PN.newArc(t1, PN.findPlace("1.internal.inner_stop"));
      
      // Stopping transitions.
      Transition *t2 = stop(p1, "initial", prefix);
      Transition *t3 = stop(p2, "done", prefix);
      
      break;
    }
      
    case(petrinetsmall_enum):
    {
      // Model an <empty> activity instead.
      Place *p1 = PN.newPlace(prefix + "initial");
      Place *p2 = PN.newPlace(prefix + "final");
      Transition *t1 = PN.newTransition(prefix + "empty");
      PN.newArc(p1, t1);
      PN.newArc(t1, p2);
      
      break;
    }
      
    default: { /* no pattern in this case */ }
  }
  
  // Unparse the standard elements.
  phylum->standardElements_1->unparse(pseudoPrinter, current_view);
  
  footer(phylum->id);
}





/******************************************************************************
  COMPENSATE
******************************************************************************/

%{
/*!
 * \fn      void kc::PCompensate(impl_tCompensate* abstract_phylum, uview current_view)
 *
 * \brief   Pattern for the <compensate> activity.
 *          
 *          \image html "compensate.png" "pattern"
 *
 *          The gray places are from the child <scope> patterns.
 *
 * \param   abstract_phylum  pointer to the phylum of the abstract syntax tree
 * \param   current_view     the current unparse view
 *
 * \note    This pattern is replaced by an <empty> activity in the
 *          "communicationonly" patterns.
 *
 * \ingroup patterns
 */
%}
void PCompensate(impl_tCompensate* abstract_phylum, uview current_view)
{
  impl_tCompensate_Compensate* phylum =
  static_cast<impl_tCompensate_Compensate*>(abstract_phylum);
  assert(phylum != NULL);
  
  header(phylum->id);
  string prefix = toString(phylum->id) + ".internal.";
  
  switch(current_view)
  {
    case(petrinetnew_enum):
    {
      // Places.
      Place *p1 = PN.newPlace(prefix + "initial");
      Place *p2 = PN.newPlace(prefix + "final");
      Place *p3 = PN.newPlace(prefix + "stop");
      Place *p4 = PN.newPlace(prefix + "stopped");
      Place *p5 = PN.newPlace(prefix + "compensating");
      
      // Concurrently call all compensation handlers of the child scopes.
      Transition *t1 = PN.newTransition(prefix + "call_ch");
      PN.newArc(p1, t1);
      PN.newArc(t1, p5);
      
      // Finish activity after called compensation handlers have finished.
      Transition *t2 = PN.newTransition(prefix + "finish");
      PN.newArc(p5, t2);
      PN.newArc(t2, p2);
      
      Transition *t3 = NULL;
      Transition *t4 = NULL;
      
      unsigned int parentId = globals::ASTEmap[ phylum->id ]->parentScopeId;
      if ( !globals::ASTEmap[ parentId ]->enclosedScopes.empty() )
      {
        // Forward stop request to compensation handler.
        t3 = PN.newTransition(prefix + "stop_ch");
        PN.newArc(p3, t3);
        PN.newArc(p5, t3, READ);
        
        // Set state to "stopped" after compensation handler has stopped.
        t4 = PN.newTransition(prefix + "ch_stopped");
        PN.newArc(p5, t4);
        PN.newArc(t4, p4);
      }
      
      // Stopping transitions.
      Transition *t5 = stop(p1, "initial", prefix);
      Transition *t6 = stop(p2, "final",   prefix);
      
      // Connect with places of child scopes.
      for( set< unsigned int >::iterator scope = globals::ASTEmap[ parentId ]->enclosedScopes.begin();
           scope != globals::ASTEmap[ parentId ]->enclosedScopes.end();
           scope++ )
      {
        string newPrefix = toString(*scope) + ".internal.";
        PN.newArc(t1, PN.findPlace(newPrefix + "compensate"));
        PN.newArc(t3, PN.findPlace(newPrefix + "ch_stop"));
        PN.newArc(PN.findPlace(newPrefix + "ch_stopped"),  t4);
        PN.newArc(PN.findPlace(newPrefix + "compensated"), t2);
      }
      
      break;
    }
      
    case(petrinetsmall_enum):
    {
      // Model an <empty> activity instead.
      Place *p1 = PN.newPlace(prefix + "initial");
      Place *p2 = PN.newPlace(prefix + "final");
      Transition *t1 = PN.newTransition(prefix + "empty");
      PN.newArc(p1, t1);
      PN.newArc(t1, p2);
      
      break;
    }
      
    default: { /* no pattern in this case */ }
  }
  
  // Unparse the standard elements.
  phylum->standardElements_1->unparse(pseudoPrinter, current_view);
  
  footer(phylum->id);
}





/******************************************************************************
  COMPENSATE SCOPE
******************************************************************************/

%{
/*!
 * \fn      void kc::PCompensateScope(impl_tCompensate* abstract_phylum, uview current_view)
 *
 * \brief   Pattern for the <compensateScope> activity.
 *
 *          \image html "compensatescope.png" "pattern"
 *
 *          The gray places are from the target <scope> pattern.
 *
 * \param   abstract_phylum  pointer to the phylum of the abstract syntax tree
 * \param   current_view     the current unparse view
 *
 * \note    The <compensate scope="a"> activity of BPEL4WS 1.1 has been
 *          replaced by a <compensateScope target="a"> activity during parsing.
 *
 * \note    This pattern is replaced by an <empty> activity in the
 *          "communicationonly" patterns.
 *
 * \ingroup patterns
 */
%}
void PCompensateScope(impl_tCompensate* abstract_phylum, uview current_view)
{
  impl_tCompensate_CompensateScope* phylum =
  static_cast<impl_tCompensate_CompensateScope*>(abstract_phylum);
  assert(phylum != NULL);
  
  header(phylum->id);
  string prefix = toString(phylum->id) + ".internal.";
  
  switch(current_view)
  {
    case(petrinetnew_enum):
    {
      // Find the prefix of the target scope.
      string newPrefix =
      toString(globals::ASTE_scopeNames[globals::ASTEmap[phylum->id]->attributes["target"]]) + ".internal.";
      
      // Places.
      Place *p1 = PN.newPlace(prefix + "initial");
      Place *p2 = PN.newPlace(prefix + "final");
      Place *p3 = PN.newPlace(prefix + "stop");
      Place *p4 = PN.newPlace(prefix + "stopped");
      Place *p5 = PN.newPlace(prefix + "compensating");
      
      // Call the compensation handler of the target scope.
      Transition *t1 = PN.newTransition(prefix + "call_ch");
      PN.newArc(p1, t1);
      PN.newArc(t1, p5);
      PN.newArc(t1, PN.findPlace(newPrefix + "compensate"));
      
      // Finish activity after called compensation handler has finished.
      Transition *t2 = PN.newTransition(prefix + "finish");
      PN.newArc(p5, t2);
      PN.newArc(t2, p2);
      PN.newArc(PN.findPlace(newPrefix + "compensated"), t2);
      
      // Forward stop request to compensation handler.
      Transition *t3 = PN.newTransition(prefix + "stop_ch");
      PN.newArc(p3, t3);
      PN.newArc(p5, t3, READ);
      PN.newArc(t3, PN.findPlace(newPrefix + "ch_stop"));
      
      // Set state to "stopped" after compensation handler has stopped.
      Transition *t4 = PN.newTransition(prefix + "ch_stopped");
      PN.newArc(p5, t4);
      PN.newArc(t4, p4);
      PN.newArc(PN.findPlace(newPrefix + "ch_stopped"),  t4);
      
      // Stopping transitions.        
      Transition *t5 = stop(p1, "initial", prefix);
      Transition *t6 = stop(p2, "final",   prefix);
      
      break;
    }
      
    case(petrinetsmall_enum):
    {
      // Model an <empty> activity instead.
      Place *p1 = PN.newPlace(prefix + "initial");
      Place *p2 = PN.newPlace(prefix + "final");
      Transition *t1 = PN.newTransition(prefix + "empty");
      PN.newArc(p1, t1);
      PN.newArc(t1, p2);
      
      break;
    }
      
    default: { /* no pattern in this case */ }
  }
  
  // Unparse the standard elements.
  phylum->standardElements_1->unparse(pseudoPrinter, current_view);
  
  footer(phylum->id);
}
