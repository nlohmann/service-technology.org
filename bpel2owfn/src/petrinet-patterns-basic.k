/*****************************************************************************\
 * Copyright 2007 Niels Lohmann, Christian Gierds                            *
 * Copyright 2006 Niels Lohmann                                              *
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
 *            - <exit> / <terminate>
 *            - <compensate>
 *            - <compensateScope>
 *
 * \author  Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          last changes of: \$Author: gierds $ 
 * 
 * \since   2006-11-26
 *
 * \date    \$Date: 2007/03/06 13:07:42 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \note    This file was created using Kimwitu++ reading file
 *          petrinet-patterns-basic.k.
 *          See http://site.informatik.hu-berlin.de/kimwitu++ for details.
 *
 * \version \$Revision: 1.33 $
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
#include "options.h"
#include "globals.h"
#include "petrinet-patterns-shortcuts.h"

using std::cerr;
using std::endl;

extern PetriNet PN;
extern map<unsigned int, ASTE*> ASTEmap;

%}




/******************************************************************************
  EMPTY
******************************************************************************/

%{
/*!
 * \fn void kc::PEmpty(impl_tEmpty *abstract_phylum, uview current_view)
 * \brief Pattern for the <empty> activity.
 * \image html "empty.png" "pattern"
 */
%}
void PEmpty(impl_tEmpty* abstract_phylum, uview current_view)
{
  impl_tEmpty_Empty* phylum = static_cast<impl_tEmpty_Empty*>(abstract_phylum);
  assert(phylum != NULL);

  header(phylum->id);
  string prefix = toString(phylum->id) + ".internal.";  


  // places
  Place *p1 = PN.newPlace(prefix + "initial");
  Place *p2 = PN.newPlace(prefix + "final");


  // transitions
  Transition *t1 = PN.newTransition(prefix + "empty");
  PN.newArc(p1, t1);
  PN.newArc(t1, p2);


  // places and transitions for the negative control flow
  if (current_view != petrinetsmall_enum)
  {
    Place *p3 = PN.newPlace(prefix + "stop");
    Place *p4 = PN.newPlace(prefix + "stopped");
    Transition *t2 = stop(p1, "initial", prefix);
    Transition *t3 = stop(p2, "final", prefix);
  }


  // unparse the standard elements
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
 * \fn void kc::PAsynchronousInvoke(impl_tInvoke* abstract_phylum, uview current_view)
 * \brief Pattern for asynchronous <invoke> activity.
 * \image html "invoke_a.png" "pattern"
 */
%}
void PAsynchronousInvoke(impl_tInvoke* abstract_phylum, uview current_view)
{
  impl_tInvoke_AsynchronousInvoke* phylum = static_cast<impl_tInvoke_AsynchronousInvoke*>(abstract_phylum);
  assert(phylum != NULL);

  header(phylum->id);
  string prefix = toString(phylum->id) + ".internal.";

  switch(current_view)
  {
    case(petrinetnew_enum):
      {
	Place *p1 = PN.newPlace(prefix + "initial");
	Place *p2 = PN.newPlace(prefix + "final");
	Place *p3 = PN.newPlace(prefix + "stop");
	Place *p4 = PN.newPlace(prefix + "stopped");

	Transition *t1 = PN.newTransition(prefix + "invoke");
	PN.newArc(p1, t1); 
	PN.newArc(t1, p2);
	PN.newArc(t1, PN.findPlace("out." + ASTEmap[phylum->id]->channelName));

	Transition *t2 = stop(p1, "initial", prefix);
	Transition *t3 = stop(p2, "final", prefix);

	break;
      }

    case(petrinetsmall_enum):
      {
	Place *p1 = PN.newPlace(prefix + "initial");
	Place *p2 = PN.newPlace(prefix + "final");

	Transition *t1 = PN.newTransition(prefix + "invoke");
	PN.newArc(p1, t1);
	PN.newArc(t1, p2);
	PN.newArc(t1, PN.findPlace("out." + ASTEmap[phylum->id]->channelName));
	
	break;
      }

    default: { /* no pattern in this case */ }      
  }


  // unparse the standard elements
  phylum->standardElements_1->unparse(pseudoPrinter, current_view);


  footer(phylum->id);
}


%{
/*!
 * \fn void kc::PSynchronousInvoke(impl_tInvoke* abstract_phylum, uview current_view)
 * \brief Pattern for the synchronous <invoke> activity.
 * \image html "invoke_s.png" "pattern"
 */
%}
void PSynchronousInvoke(impl_tInvoke* abstract_phylum, uview current_view)
{
  impl_tInvoke_SynchronousInvoke* phylum = static_cast<impl_tInvoke_SynchronousInvoke*>(abstract_phylum);
  assert(phylum != NULL);

  header(phylum->id);
  string prefix = toString(phylum->id) + ".internal.";


  switch(current_view)
  {
    case(petrinetnew_enum):
      {
	Place *p1 = PN.newPlace(prefix + "initial");
        Place *p2 = PN.newPlace(prefix + "running");
	Place *p3 = PN.newPlace(prefix + "final");
	Place *p4 = PN.newPlace(prefix + "stop");
	Place *p5 = PN.newPlace(prefix + "stopped");

	Transition *t1 = PN.newTransition(prefix + "invoke_out");
	PN.newArc(p1, t1);
	PN.newArc(t1, p2);
	PN.newArc(t1, PN.findPlace("out." + ASTEmap[phylum->id]->channelName));

        Transition *t2 = PN.newTransition(prefix + "invoke_in");
	PN.newArc(t2, PN.findPlace("in." + ASTEmap[phylum->id]->channelName2));
	PN.newArc(p5, t2);
	PN.newArc(t2, p3);

	
	Transition *t3 = stop(p1, "initial", prefix);
	Transition *t4 = stop(p2, "running", prefix);
	Transition *t5 = stop(p3, "final", prefix);
	
	break;
      }

    case(petrinetsmall_enum):
      {
	Place *p1 = PN.newPlace(prefix + "initial");
	Place *p2 = PN.newPlace(prefix + "running");
	Place *p3 = PN.newPlace(prefix + "final");

	Transition *t1 = PN.newTransition(prefix + "invokeO");
	PN.newArc(t1, PN.findPlace("out." + ASTEmap[phylum->id]->channelName));
	PN.newArc(p1, t1);
	PN.newArc(t1, p2);

	Transition *t2 = PN.newTransition(prefix + "invokeI");
	PN.newArc(PN.findPlace("in." + ASTEmap[phylum->id]->channelName), t2);
	PN.newArc(p2, t2);
	PN.newArc(t2, p3);

	break;
      }

    default: { /* no pattern in this case */ }
  }


  // unparse the standard elements
  phylum->standardElements_1->unparse(pseudoPrinter, current_view);


  footer(phylum->id);
}





/******************************************************************************
  RECEIVE
******************************************************************************/

%{
/*!
 * \fn void kc::PReceive(impl_tReceive* abstract_phylum, uview current_view)
 * \brief Pattern for the <receive> activity.
 * \image html "receive.png" "pattern"
 */
%}
void PReceive(impl_tReceive* abstract_phylum, uview current_view)
{
  impl_tReceive_Receive* phylum = static_cast<impl_tReceive_Receive*>(abstract_phylum);
  assert(phylum != NULL);

  header(phylum->id);
  string prefix = toString(phylum->id) + ".internal.";  


  switch(current_view)
  {
     case(petrinetnew_enum):
      {
	Place *p1 = PN.newPlace(prefix + "initial");
	Place *p2 = PN.newPlace(prefix + "final");
	Place *p3 = PN.newPlace(prefix + "stop");
	Place *p4 = PN.newPlace(prefix + "stopped");

	Transition *t1 = PN.newTransition(prefix + "receive");
	PN.newArc(PN.findPlace("in." + ASTEmap[phylum->id]->channelName), t1);
	PN.newArc(p1, t1);
	PN.newArc(t1, p2);

	Transition *t2 = stop(p1, "initial", prefix);
	Transition *t3 = stop(p2, "final", prefix);

	break;
      }

    case(petrinetsmall_enum):
      {
	Place *p1 = PN.newPlace(prefix + "initial");
	Place *p2 = PN.newPlace(prefix + "final");

	Transition *t1 = PN.newTransition(prefix + "receive");
	PN.newArc(PN.findPlace("in." + ASTEmap[phylum->id]->channelName), t1);
	PN.newArc(p1, t1);
	PN.newArc(t1, p2);

	break;
      }

    default: { /* no pattern in this case */ }      
  }


  // unparse the standard elements
  phylum->standardElements_1->unparse(pseudoPrinter, current_view);


  footer(phylum->id);      
}




/******************************************************************************
  REPLY
******************************************************************************/

%{
/*!
 * \fn void kc::PReply(impl_tReply* abstract_phylum, uview current_view)
 * \brief Pattern for the <reply> activity.
 * \image html "reply.png" "pattern"
 */
%}
void PReply(impl_tReply* abstract_phylum, uview current_view)
{
  impl_tReply_Reply* phylum = static_cast<impl_tReply_Reply*>(abstract_phylum);
  assert(phylum != NULL);

  header(phylum->id);
  string prefix = toString(phylum->id) + ".internal.";  


  switch(current_view)
  {
    case(petrinetnew_enum):
      {
	Place *p1 = PN.newPlace(prefix + "initial");
	Place *p2 = PN.newPlace(prefix + "final");
	Place *p3 = PN.newPlace(prefix + "stop");
	Place *p4 = PN.newPlace(prefix + "stopped");

	Transition *t1 = PN.newTransition(prefix + "read_var");
	PN.newArc(p1, t1);
	PN.newArc(t1, p2);
	PN.newArc(t1, PN.findPlace("out." + ASTEmap[phylum->id]->channelName));

        Transition *t2 = stop(p1, "initial", prefix);
	Transition *t3 = stop(p2, "final", prefix);

	break;
      }

    case(petrinetsmall_enum):
      {
	Place *p1 = PN.newPlace(prefix + "initial");
	Place *p2 = PN.newPlace(prefix + "final");

	Transition *t1 = PN.newTransition(prefix + "reply");
	PN.newArc(t1, PN.findPlace("out." + ASTEmap[phylum->id]->channelName));
	PN.newArc(p1, t1);
	PN.newArc(t1, p2);

	break;
      }

    default: { /* no pattern in this case */ }      
  }


  // unparse the standard elements
  phylum->standardElements_1->unparse(pseudoPrinter, current_view);


  footer(phylum->id);
}





/******************************************************************************
  ASSIGN
******************************************************************************/

%{
/*!
 * \fn void kc::PAssign(impl_tAssign* abstract_phylum, uview current_view)
 * \brief Pattern for the <assign> activity.
 * \image html "assign.png" "pattern"
 */
%}
void PAssign(impl_tAssign* abstract_phylum, uview current_view)
{
  impl_tAssign_Assign* phylum = static_cast<impl_tAssign_Assign*>(abstract_phylum);
  assert(phylum != NULL);

  header(phylum->id);
  string prefix = toString(phylum->id) + ".internal.";


  switch (current_view)
  {
    case(petrinetnew_enum):
      {
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

	  if (ASTEmap[from->id]->variableName != "")
	  {
	    Place *p2 = PN.newPlace(newPrefix + "running");
	    Place *p4 = PN.newPlace(newPrefix + "p4");
	    Place *p8 = PN.newPlace(newPrefix + "final");
      
	    Transition *t1 = PN.newTransition(newPrefix + "t1");
	    PN.newArc(lastPlace, t1);
	    PN.newArc(t1, p2);
            if (parameters[P_VARIABLES])
	      PN.newArc(PN.findPlace("variable." + ASTEmap[from->id]->variableName), t1, READ);

	    Transition *t2 = PN.newTransition(newPrefix + "t2");
	    PN.newArc(p2, t2);
	    PN.newArc(t2, p8);
	    if (parameters[P_VARIABLES])
	    {
	      PN.newArc(PN.findPlace("variable." + ASTEmap[to->id]->variableName), t2);
	      PN.newArc(t2, PN.findPlace("variable." + ASTEmap[to->id]->variableName));
	    }

	    // Transitions for the negative control flow.
	    if (parameters[P_STANDARDFAULTS])
	      Transition *t3 = throwFault(p2, p4, "running", newPrefix,
                                 mkinteger(phylum->id), ASTEmap[phylum->id]->controlFlow);
	    
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
	    if (parameters[P_VARIABLES])
	    {
	      PN.newArc(PN.findPlace("variable." + ASTEmap[to->id]->variableName), t2);
	      PN.newArc(t2, PN.findPlace("variable." + ASTEmap[to->id]->variableName));
	    }

	    // Transitions for the negative control flow.
	    if (parameters[P_STANDARDFAULTS])
	      Transition *t3 = throwFault(lastPlace, p4, "initial", newPrefix,
                                 mkinteger(phylum->id), ASTEmap[phylum->id]->controlFlow);

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
 * \fn void kc::PValidate(impl_tValidate* abstract_phylum, uview current_view)
 * \brief Pattern for the <validate> activity.
 * \image html "validate.png" "pattern"
 */
%}
void PValidate(impl_tValidate* abstract_phylum, uview current_view)
{
  impl_tValidate_Validate* phylum = static_cast<impl_tValidate_Validate*>(abstract_phylum);
  assert(phylum != NULL);

  header(phylum->id);
  string prefix = toString(phylum->id) + ".internal.";


  
  switch (current_view)
  {
/*    case(petrinetnew_enum):
      {
	Place *p1 = PN.newPlace(prefix + "initial");
	Place *p2 = PN.newPlace(prefix + "running");
	Place *p3 = PN.newPlace(prefix + "final");
	Place *p4 = PN.newPlace(prefix + "p4");
	Place *p5 = PN.newPlace(prefix + "stop");
	Place *p6 = PN.newPlace(prefix + "stopped");

	Transition *t1 = PN.newTransition(prefix + "t1");
	PN.newArc(p1, t1);
	PN.newArc(t1, p2);

	Transition *t2 = PN.newTransition(prefix + "t2");
	PN.newArc(p2, t2);
	PN.newArc(t2, p3);

	// Transitions for the negative control flow.
	if (parameters[P_STANDARDFAULTS])
	  Transition *t3 = throwFault(p2, p4, "running", prefix,
                             mkinteger(phylum->id), ASTEmap[phylum->id]->controlFlow);

	Transition *t4 = stop(p1, "initial", prefix);
	Transition *t5 = stop(p2, "running", prefix);
	Transition *t6 = stop(p4, "p4", prefix);
	Transition *t7 = stop(p3, "final", prefix);

	break;
      }
*/
    case(petrinetnew_enum):
      {
	Place *p1 = PN.newPlace(prefix + "initial");
	Place *p2 = PN.newPlace(prefix + "running");
	Place *p3 = PN.newPlace(prefix + "final");
	Place *p4 = PN.newPlace(prefix + "stop");
	Place *p5 = PN.newPlace(prefix + "stopped");
        Place *p6 = PN.newPlace(prefix + "fault");

	Transition *t1 = PN.newTransition(prefix + "t1");
	PN.newArc(p1, t1);
	PN.newArc(t1, p2);

	Transition *t2 = PN.newTransition(prefix + "t2");
	PN.newArc(p2, t2);
	PN.newArc(t2, p3);

	// Transitions for the negative control flow.
	if (parameters[P_STANDARDFAULTS])
	  Transition *t3 = throwFault(p2, p6, "running", prefix, mkinteger(phylum->id), ASTEmap[phylum->id]->controlFlow);

	Transition *t4 = stop(p1, "initial", prefix);
	Transition *t5 = stop(p2, "running", prefix);
	Transition *t6 = stop(p3, "final", prefix);

	break;
      }

    case(petrinetsmall_enum):
      {
	Place *p1 = PN.newPlace(prefix + "initial");
	Place *p2 = PN.newPlace(prefix + "final");

	Transition *t1 = PN.newTransition(prefix + "validate");
	PN.newArc(p1, t1);
	PN.newArc(t1, p2);

	break;
      }
    
    default: { /* no pattern in this case */ }
  }


  // unparse the standard elements
  phylum->standardElements_1->unparse(pseudoPrinter, current_view);


  footer(phylum->id);
}





/******************************************************************************
  WAIT

  The <wait> activity has two patterns: one for the "wait until" (deadline)
  version and one for the "wait for" (duration) version.
******************************************************************************/

void PWait(impl_tWait* abstract_phylum, uview current_view)
{
  impl_tWait_Wait* phylum = static_cast<impl_tWait_Wait*>(abstract_phylum);
  assert(phylum != NULL);

  header(phylum->id);
  string prefix = toString(phylum->id) + ".internal.";


  switch (current_view)
  {
    case(petrinetnew_enum):
      {
	Place *p1 = PN.newPlace(prefix + "initial");
	Place *p2 = PN.newPlace(prefix + "final");
	Place *p3 = PN.newPlace(prefix + "stop");
	Place *p4 = PN.newPlace(prefix + "stopped");

	Transition *t1 = PN.newTransition(prefix + "wait");
	PN.newArc(p1, t1);
	PN.newArc(t1, p2);

        // Transitions for the negative control flow.
	Transition *t2 = stop(p1, "initial", prefix);
	Transition *t3 = stop(p2, "final", prefix);

	break;
      }

    case (petrinetsmall_enum):
      {
	Place *p1 = PN.newPlace(prefix + "initial");
	Place *p2 = PN.newPlace(prefix + "final");

	Transition *t1 = PN.newTransition(prefix + "wait");
	PN.newArc(p1, t1);
	PN.newArc(t1, p2);

	break;
      }

    default: { /* no pattern in this case */ }
  }


  // unparse the standard elements
  phylum->standardElements_1->unparse(pseudoPrinter, current_view);


  footer(phylum->id);      
}

void PWaitUntil(impl_tWait* abstract_phylum, uview current_view)
{
  impl_tWait_WaitUntil* phylum = static_cast<impl_tWait_WaitUntil*>(abstract_phylum);
  assert(phylum != NULL);

  header(phylum->id);
  string prefix = toString(phylum->id) + ".internal.";


  switch (current_view)
  {
    case(petrinetnew_enum):
      {
	Place *p1 = PN.newPlace(prefix + "initial");
	Place *p2 = PN.newPlace(prefix + "final");
	Place *p3 = PN.newPlace(prefix + "stop");
	Place *p4 = PN.newPlace(prefix + "stopped");

	Transition *t1 = PN.newTransition(prefix + "waitUntil");
	PN.newArc(p1, t1);
	PN.newArc(t1, p2);
        if (parameters[P_VARIABLES])
          PN.newArc(PN.findPlace("1.internal.clock"), t1, READ);

	// Transitions for the negative control flow.
	Transition *t2 = stop(p1, "initial", prefix);
	Transition *t3 = stop(p2, "final", prefix);

	break;
      }

    case (petrinetsmall_enum):
      {
	Place *p1 = PN.newPlace(prefix + "initial");
	Place *p2 = PN.newPlace(prefix + "final");

	Transition *t1 = PN.newTransition(prefix + "waitUntil");
	PN.newArc(p1, t1);
	PN.newArc(t1, p2);

	break;
      }

    default: { /* no pattern in this case */ }
  }


  // unparse the standard elements
  phylum->standardElements_1->unparse(pseudoPrinter, current_view);


  footer(phylum->id);      
}


void PWaitFor(impl_tWait* abstract_phylum, uview current_view)
{
  impl_tWait_WaitFor* phylum = static_cast<impl_tWait_WaitFor*>(abstract_phylum);
  assert(phylum != NULL);

  header(phylum->id);
  string prefix = toString(phylum->id) + ".internal.";


  switch (current_view)
  {
    case(petrinetnew_enum):
      {
	Place *p1 = PN.newPlace(prefix + "initial");
	Place *p2 = PN.newPlace(prefix + "TimeStamp");
	Place *p3 = PN.newPlace(prefix + "final");
	Place *p4 = PN.newPlace(prefix + "stop");
	Place *p5 = PN.newPlace(prefix + "stopped");

	Transition *t1 = PN.newTransition(prefix + "readTime");
	PN.newArc(p1, t1);
	PN.newArc(t1, p2);
        if (parameters[P_VARIABLES])
	  PN.newArc(PN.findPlace("1.internal.clock"), t1, READ);

	Transition *t2 = PN.newTransition(prefix + "waitFor");
	PN.newArc(p2, t2);
	PN.newArc(t2, p3);
	PN.newArc(PN.findPlace("1.internal.clock"), t2, READ);

	// Transitions for the negative control flow.
	Transition *t3 = stop(p1, "initial", prefix);
	Transition *t4 = stop(p2, "TimeStamp", prefix);
	Transition *t5 = stop(p3, "final", prefix);
      }

    case (petrinetsmall_enum):
      {
	Place *p1 = PN.newPlace(prefix + "initial");
	Place *p2 = PN.newPlace(prefix + "final");

	Transition *t1 = PN.newTransition(prefix + "waitFor");
	PN.newArc(p1, t1);
	PN.newArc(t1, p2);

	break;
      }

    default: { /* no pattern in this case */ }
  }


  // unparse the standard elements
  phylum->standardElements_1->unparse(pseudoPrinter, current_view);


  footer(phylum->id);      
}





/******************************************************************************
  THROW
******************************************************************************/

%{
/*!
 * \fn void kc::PThrow(impl_tThrow* abstract_phylum, uview current_view)
 * \brief Pattern for the <throw> activity.
 * \image html "throw.png" "pattern"
 */
%}
void PThrow(impl_tThrow* abstract_phylum, uview current_view)
{
  impl_tThrow_Throw* phylum = static_cast<impl_tThrow_Throw*>(abstract_phylum);
  assert(phylum != NULL);

  header(phylum->id);
  string prefix = toString(phylum->id) + ".internal.";


  switch(current_view)
  {
/* 
   case(petrinet_enum):
      {
	Place *p1 = PN.newPlace(prefix + "initial");
	Place *p2 = PN.newPlace(prefix + "faulted");
	Place *p3 = PN.newPlace(prefix + "stop");
	Place *p4 = PN.newPlace(prefix + "stopped");
	Place *p5 = PN.newPlace(prefix + "final"); // it's OK that this place is not connected

	// Transitions for the negative control flow.
	Transition *t1 = throwFault(p1, p2, "initial", prefix,
                           mkinteger(phylum->id), ASTEmap[phylum->id]->controlFlow);
	Transition *t2 = stop(p1, "initial", prefix);
	Transition *t3 = stop(p2, "faulted", prefix);

	break;
      }
*/
    case(petrinetnew_enum):
      {
	Place *p1 = PN.newPlace(prefix + "initial");
	Place *p2 = PN.newPlace(prefix + "faulted");
        Place *p3 = PN.newPlace(prefix + "fault");
	Place *p4 = PN.newPlace(prefix + "stop");
	Place *p5 = PN.newPlace(prefix + "stopped");
        Place *p6 = PN.newPlace(prefix + "final");

	// Transitions for the negative control flow.
	Transition *t1 = PN.newTransition(prefix + "throw");
        PN.newArc(p1, t1);
        PN.newArc(t1, p2);
        PN.newArc(t1, p3);

	Transition *t2 = stop(p1, "initial", prefix);
	Transition *t3 = stop(p2, "faulted", prefix);

	break;
      }

    case(petrinetsmall_enum):
      {
	cerr << "the <throw> activity cannot be modelled!" << endl;

	Place *p1 = PN.newPlace(prefix + "initial");
	Place *p2 = PN.newPlace(prefix + "final");
	
	Transition *t1 = PN.newTransition(prefix + "empty");
	PN.newArc(p1, t1);
	PN.newArc(t1, p2);

	break;
      }

    default: { /* no pattern in this case */ }
  }


  // unparse the standard elements
  phylum->standardElements_1->unparse(pseudoPrinter, current_view);


  footer(phylum->id);      
}





/******************************************************************************
  RETHROW
******************************************************************************/

%{
/*!
 * \fn void kc::PRethrow(impl_tRethrow* abstract_phylum, uview current_view)
 * \brief Pattern for the <rethrow> activity.
 * \image html "rethrow.png" "pattern"
 * The gray place is from the <faultHandlers> pattern.
 */
%}
void PRethrow(impl_tRethrow* abstract_phylum, uview current_view)
{
  impl_tRethrow_Rethrow* phylum = static_cast<impl_tRethrow_Rethrow*>(abstract_phylum);
  assert(phylum != NULL);

  header(phylum->id);
  string prefix = toString(phylum->id) + ".internal.";

  Place *p1 = PN.newPlace(prefix + "initial");
//  Place *p5 = PN.newPlace(prefix + "final"); // it's OK that this place is not connected
  
  // assert(ASTEmap[phylum->id] != NULL);
  Place *p2 = PN.newPlace(prefix + "faulted");
  Place *p3 = PN.newPlace(prefix + "fault");
  Place *p4 = PN.newPlace(prefix + "stop");
  Place *p5 = PN.newPlace(prefix + "stopped");
  Place *p6 = PN.newPlace(prefix + "final");

  // Transitions for the negative control flow.
  Transition *t1 = PN.newTransition(prefix + "rethrow");
  PN.newArc(PN.findPlace(toString(ASTEmap[phylum->id]->parentScopeId) + ".internal.faultHandler.fault"), t1, READ);
  PN.newArc(p1, t1);
  PN.newArc(t1, p2);
  PN.newArc(t1, p3);

  Transition *t2 = stop(p1, "initial", prefix);
  Transition *t3 = stop(p2, "faulted", prefix);


  // unparse the standard elements
  phylum->standardElements_1->unparse(pseudoPrinter, current_view);


  footer(phylum->id);
}





/******************************************************************************
  EXIT / TERMINATE

  The patterns for <exit> and <terminate> are the same.
******************************************************************************/

%{
/*!
 * \fn void kc::PExit(impl_tExit* abstract_phylum, uview current_view)
 * \brief Pattern for the <exit> activity.
 * \image html "exit.png" "pattern"
 * The gray places are from the <process> pattern.
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
	Place *p1 = PN.newPlace(prefix + "initial");
	Place *p2 = PN.newPlace(prefix + "done");
	Place *p3 = PN.newPlace(prefix + "stop");
	Place *p4 = PN.newPlace(prefix + "stopped");
        Place *p5 = PN.newPlace(prefix + "final");

        Transition *t1 = PN.newTransition(prefix + "exit");
	PN.newArc(p1, t1);
	PN.newArc(t1, p2);
	PN.newArc(PN.findPlace("1.internal.!Exiting"), t1);
	PN.newArc(t1, PN.findPlace("1.internal.Exiting"));
	PN.newArc(t1, PN.findPlace("1.internal.exit"));

	// Transitions for the negative control flow.
	Transition *t2 = stop(p1, "initial", prefix);
	Transition *t3 = stop(p2, "done", prefix);

	break;
      }

    case(petrinetsmall_enum):
      {
	cerr << "the <exit> activity cannot be modelled!" << endl;

	Place *p1 = PN.newPlace(prefix + "initial");
	Place *p2 = PN.newPlace(prefix + "final");
	
	Transition *t1 = PN.newTransition(prefix + "empty");
	PN.newArc(p1, t1);
	PN.newArc(t1, p2);

	break;
      }

    default: { /* no pattern in this case */ }
  }


  // unparse the standard elements
  phylum->standardElements_1->unparse(pseudoPrinter, current_view);


  footer(phylum->id);
}





/******************************************************************************
  COMPENSATE
******************************************************************************/

%{
/*!
 * \fn void kc::PCompensate(impl_tCompensate* abstract_phylum, uview current_view)
 * \brief Pattern for the <compensate> activity.
 * \image html "compensate.png" "pattern"
 * The gray places are from the child <scope> patterns.
 */
%}
void PCompensate(impl_tCompensate* abstract_phylum, uview current_view)
{
  impl_tCompensate_Compensate* phylum = static_cast<impl_tCompensate_Compensate*>(abstract_phylum);
  assert(phylum != NULL);

  header(phylum->id);
  string prefix = toString(phylum->id) + ".internal.";

  
  switch(current_view)
  {
    case(petrinetnew_enum):
      {
	Place *p1 = PN.newPlace(prefix + "initial");
	Place *p2 = PN.newPlace(prefix + "final");
        Place *p3 = PN.newPlace(prefix + "stop");
        Place *p4 = PN.newPlace(prefix + "stopped");
        Place *p5 = PN.newPlace(prefix + "compensating");
	
	Transition *t1 = PN.newTransition(prefix + "call_ch");
	PN.newArc(p1, t1);
	PN.newArc(t1, p5);

	Transition *t2 = PN.newTransition(prefix + "finish");
	PN.newArc(p5, t2);
	PN.newArc(t2, p1);

        Transition *t3 = PN.newTransition(prefix + "stop_ch");
        PN.newArc(p3, t3);
        PN.newArc(p5, t3, READ);

        Transition *t4 = PN.newTransition(prefix + "ch_stopped");
        PN.newArc(p5, t4);
        PN.newArc(t4, p4);


        // connect with places of enclosed scopes of the parent activity
        unsigned int parentId = ASTEmap[ phylum->id ]->parentScopeId;
        if( ! ASTEmap[ parentId ]->enclosedScopes.empty() )
        {
          for( set< unsigned int >::iterator scope = ASTEmap[ parentId ]->enclosedScopes.begin();
                                             scope != ASTEmap[ parentId ]->enclosedScopes.end();
                                             scope++ )
          {
            string newPrefix = toString(*scope) + ".internal.";
            PN.newArc(t1, PN.findPlace(newPrefix + "compensate"));
            PN.newArc(t3, PN.findPlace(newPrefix + "ch_stop"));
            PN.newArc(PN.findPlace(newPrefix + "ch_stopped"),  t4);
            PN.newArc(PN.findPlace(newPrefix + "compensated"), t2);
          }
          Transition *t5 = stop(p1, "initial", prefix);
          Transition *t6 = stop(p2, "final",   prefix);
        }
        else
        {
          PN.mergePlaces(p1, p2);
          PN.mergePlaces(p3, p4);
        }

        break;
      }

    case(petrinetsmall_enum):
      {
	cerr << "the <compensate> activity cannot be modelled!" << endl;

	Place *p1 = PN.newPlace(prefix + "initial");
	Place *p2 = PN.newPlace(prefix + "final");
	
	Transition *t1 = PN.newTransition(prefix + "empty");
	PN.newArc(p1, t1);
	PN.newArc(t1, p2);

	break;
      }

    default: { /* no pattern in this case */ }
  }


  // unparse the standard elements
  phylum->standardElements_1->unparse(pseudoPrinter, current_view);


  footer(phylum->id);
}





/******************************************************************************
  COMPENSATE SCOPE

  The <compensate scope="a"> activity of BPEL4WS 1.1 has been replaced by a
  <compensateScope target="a"> activity.
******************************************************************************/

%{
/*!
 * \fn void kc::PCompensateScope(impl_tCompensate* abstract_phylum, uview current_view)
 * \brief Pattern for the <compensateScope> activity.
 * \image html "compensatescope.png" "pattern"
 * The gray places are from the target child <scope> pattern.
 */
%}
void PCompensateScope(impl_tCompensate* abstract_phylum, uview current_view)
{
  impl_tCompensate_CompensateScope* phylum = static_cast<impl_tCompensate_CompensateScope*>(abstract_phylum);
  assert(phylum != NULL);

  header(phylum->id);
  string prefix = toString(phylum->id) + ".internal.";

  switch(current_view)
  {
    case(petrinetnew_enum):
      {

	Place *p1 = PN.newPlace(prefix + "initial");
	Place *p2 = PN.newPlace(prefix + "final");
        Place *p3 = PN.newPlace(prefix + "stop");
        Place *p4 = PN.newPlace(prefix + "stopped");
        Place *p5 = PN.newPlace(prefix + "compensating");
	
	Transition *t1 = PN.newTransition(prefix + "call_ch");
	PN.newArc(p1, t1);
	PN.newArc(t1, p5);

	Transition *t2 = PN.newTransition(prefix + "finish");
	PN.newArc(p5, t2);
	PN.newArc(t2, p1);

        Transition *t3 = PN.newTransition(prefix + "stop_ch");
        PN.newArc(p3, t3);
        PN.newArc(p5, t3, READ);

        Transition *t4 = PN.newTransition(prefix + "ch_stopped");
        PN.newArc(p5, t4);
        PN.newArc(t4, p4);


        string newPrefix = toString(globals::ASTE_scopeNames[ASTEmap[phylum->id]->attributes["target"]]) + ".internal.";
        PN.newArc(t1, PN.findPlace(newPrefix + "compensate"));
        PN.newArc(t3, PN.findPlace(newPrefix + "ch_stop"));
        PN.newArc(PN.findPlace(newPrefix + "ch_stopped"),  t4);
        PN.newArc(PN.findPlace(newPrefix + "compensated"), t2);
        
        Transition *t5 = stop(p1, "initial", prefix);
        Transition *t6 = stop(p2, "final",   prefix);

        break;
      }

    case(petrinetsmall_enum):
      {
	cerr << "the <compensateScope> activity cannot be modelled!" << endl;

	Place *p1 = PN.newPlace(prefix + "initial");
	Place *p2 = PN.newPlace(prefix + "final");
	
	Transition *t1 = PN.newTransition(prefix + "empty");
	PN.newArc(p1, t1);
	PN.newArc(t1, p2);

	break;
      }

    default: { /* no pattern in this case */ }
  }


  // unparse the standard elements
  phylum->standardElements_1->unparse(pseudoPrinter, current_view);


  footer(phylum->id);
}
