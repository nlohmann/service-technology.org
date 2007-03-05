/*****************************************************************************\
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
 * \file    petrinet-patterns-scopes.cc
 *
 * \brief   Petri net patterns for BPEL's <scope> and <process>
 *
 * \author  Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          last changes of: \$Author: nielslohmann $ 
 * 
 * \since   2006-11-29
 *
 * \date    \$Date: 2007/03/05 15:15:33 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \note    This file was created using Kimwitu++ reading file petrinet-patterns-scopes.k.
 *          See http://site.informatik.hu-berlin.de/kimwitu++ for details.
 *
 * \version \$Revision: 1.27 $
 */
%}


%{ HEADER
/*!
 * \file petrinet-patterns-scopes.h
 * \brief Petri net patterns for BPEL's <scope> and <process>
 *
 * See \ref petrinet-patterns-scopes.cc for more information.
 */
%}

%{
#include "ast-config.h"
#include "helpers.h"
#include "ast-tools.h"
#include "ast-details.h"
#include "options.h"
#include "globals.h"

using std::cerr;
using std::endl;

extern PetriNet PN;
extern map<unsigned int, ASTE*> ASTEmap;
%}





/******************************************************************************
  PROCESS
******************************************************************************/

void PProcess(impl_tProcess* abstract_phylum, uview current_view)
{
  impl_tProcess_Process* phylum = static_cast<impl_tProcess_Process*>(abstract_phylum);
  assert(phylum != NULL);

  header(phylum->id, true);
  string prefix = toString(phylum->id) + ".internal.";


  switch(current_view)
  {
    case(petrinetnew_enum):
      {
	globals::currentScope = prefix;

	// Create the interface places.
	// The used lists are filled during parsing mapping each combination of
	// partnerLink, portType and operation to one input (resp. output) place.
	for (set<string>::iterator in = globals::ASTE_inputChannels.begin(); in != globals::ASTE_inputChannels.end(); in++)
	  PN.newPlace("in." + *in, IN);
	for (set<string>::iterator out = globals::ASTE_outputChannels.begin(); out != globals::ASTE_outputChannels.end(); out++)
	  PN.newPlace("out." + *out, OUT);


	// Create the places for the links.
	// The used list is filled during parsing and contains all links of the
	// whole process -- also links defined in a deeper hierarchy.
	for (set<string>::iterator link = globals::ASTE_linkNames.begin(); link != globals::ASTE_linkNames.end(); link++)
	{
	  PN.newPlace("link." + *link);
	  PN.newPlace("!link." + *link);
	}


	Place *p1  = PN.newPlace(prefix + "initial"); p1->mark();
	Place *p2  = PN.newPlace(prefix + "final");
	Place *p3  = PN.newPlace(prefix + "fault_in");
	Place *p4  = PN.newPlace(prefix + "exit");
	Place *p5  = PN.newPlace(prefix + "Active");
	Place *p6  = PN.newPlace(prefix + "!Active"); p6->mark();
	Place *p7  = PN.newPlace(prefix + "Exiting");
	Place *p8  = PN.newPlace(prefix + "!Exiting"); p8->mark();
	Place *p9  = PN.newPlace(prefix + "inner_stop");
	Place *p10 = PN.newPlace(prefix + "inner_stopped");
	Place *p11 = PN.newPlace(prefix + "activity_initial");
	Place *p12 = PN.newPlace(prefix + "activity_final");
	Place *p13 = PN.newPlace(prefix + "fh_fault");
	Place *p14 = PN.newPlace(prefix + "ch_fault_in");

        p2->isFinal = true;

	Transition *t1 = PN.newTransition(prefix + "initialize");
	PN.newArc(p1, t1);
	PN.newArc(t1, p5);
	PN.newArc(p6, t1);
	PN.newArc(t1, p11);

	Transition *t2 = PN.newTransition(prefix + "finalize");
	PN.newArc(t2, p2);
	PN.newArc(p5, t2);
	PN.newArc(t2, p6);
	if (!ASTEmap[phylum->id]->hasEH)
	  PN.newArc(p12, t2);

	Transition *t3 = PN.newTransition(prefix + "ft");
	PN.newArc(p3, t3);
	PN.newArc(p5, t3);
	PN.newArc(t3, p6);
	PN.newArc(t3, p9);
	PN.newArc(t3, p13);

	Transition *t4 = PN.newTransition(prefix + "ignore_fault");
	PN.newArc(p3, t4);
	PN.newArc(p6, t4, READ);

	Transition *t5 = PN.newTransition(prefix + "exit_process");
	PN.newArc(p4, t5);
	PN.newArc(p5, t5);
	PN.newArc(t5, p6);
	PN.newArc(t5, p7);
	PN.newArc(p8, t5);
	PN.newArc(t5, p9);

	Transition *t6 = PN.newTransition(prefix + "exit_fh");
	PN.newArc(p4, t6);
	PN.newArc(p6, t6, READ);
	PN.newArc(t6, p7);
	PN.newArc(p8, t6);
	PN.newArc(t6, p9);

	Transition *t7 = PN.newTransition(prefix + "ignore_exit");
	PN.newArc(p4, t7);
	PN.newArc(p7, t7, READ);

	Transition *t8 = PN.newTransition(prefix + "process_exited");
	PN.newArc(t8, p2);
	PN.newArc(p7, t8, READ);
	PN.newArc(p10, t8);

	Transition *t9 = PN.newTransition(prefix + "forced_exit");
	PN.newArc(t9, p4);
	PN.newArc(p6, t9, READ);
	PN.newArc(p14, t9);

	Transition *t10 = PN.newTransition(prefix + "handle_ch_fault");
	PN.newArc(t10, p3);
	PN.newArc(p5, t10, READ);
	PN.newArc(p14, t10);


	// unparse the embedded activity
	phylum->activity_1->unparse(pseudoPrinter, current_view);

	// unparse the fault handlers
	phylum->tFaultHandlers_1->unparse(pseudoPrinter, current_view);

	// unparse the fault handlers
	phylum->tEventHandlers_1->unparse(pseudoPrinter, current_view);


	if (ASTEmap[phylum->id]->hasEH)
	{
	  Transition *t11 = PN.newTransition(prefix + "finish_eh");
	  PN.newArc(p12, t11);
	  PN.newArc(PN.findPlace(toString(phylum->tEventHandlers_1->id) + ".internal.running"), t11);
	  PN.newArc(t11, PN.findPlace(toString(phylum->tEventHandlers_1->id) + ".internal.finishing"));

	  PN.newArc(t1, PN.findPlace(toString(phylum->tEventHandlers_1->id) + ".internal.initial"));
	  PN.newArc(PN.findPlace(toString(phylum->tEventHandlers_1->id) + ".internal.final"), t2);
	}


  	// embed fault handler
    	PN.mergePlaces(prefix + "fh_fault",      prefix + "faultHandler.fault");
      	PN.mergePlaces(prefix + "inner_stopped", prefix + "faultHandler.initial");
	PN.mergePlaces(prefix + "final",         prefix + "faultHandler.final");
	PN.mergePlaces(prefix + "inner_stop",    prefix + "faultHandler.stop");
	PN.mergePlaces(prefix + "final",         prefix + "faultHandler.stopped");

	// embed inner activity
	PN.mergePlaces(prefix + "activity_initial", toString(phylum->activity_1->id) + ".initial");
	PN.mergePlaces(prefix + "activity_final",   toString(phylum->activity_1->id) + ".final");
	PN.mergePlaces(prefix + "inner_stop",       toString(phylum->activity_1->id) + ".stop");
	PN.mergePlaces(prefix + "inner_stopped",    toString(phylum->activity_1->id) + ".stopped");

        break;
      }

    case(petrinetsmall_enum):
      {
	// Create the interface places.
	// The used lists are filled during parsing mapping each combination of
	// partnerLink, portType and operation to one input (resp. output) place.
	for (set<string>::iterator in = globals::ASTE_inputChannels.begin(); in != globals::ASTE_inputChannels.end(); in++)
	  PN.newPlace("in." + *in, IN);
	for (set<string>::iterator out = globals::ASTE_outputChannels.begin(); out != globals::ASTE_outputChannels.end(); out++)
	  PN.newPlace("out." + *out, OUT);

	// Create the places for the links.
	// The used list is filled during parsing and contains all links of the
	// whole process -- also links defined in a deeper hierarchy.
	for (set<string>::iterator link = globals::ASTE_linkNames.begin(); link != globals::ASTE_linkNames.end(); link++)
	{
	  PN.newPlace("link." + *link);
	  PN.newPlace("!link." + *link);
	}

	// Create the initial place of the process pattern and mark it.
	Place *p1 = PN.newPlace(prefix + "initial");
	p1->mark();
      
	// Create the internal places of the process pattern.
	Place *p2 = PN.newPlace(prefix + "final");

        p2->isFinal = true;

	// unparse the embedded activity
	phylum->activity_1->unparse(pseudoPrinter, current_view);


	PN.mergePlaces("1.internal.initial", toString(phylum->activity_1->id) + ".initial");
	PN.mergePlaces("1.internal.final",   toString(phylum->activity_1->id) + ".final");

	break;
      }

    default: { /* no pattern in this case */ }
  }


  // this global mapping is not used any more
  globals::ASTE_linkNames.clear();

  footer(phylum->id, true);
}




/******************************************************************************
  SCOPE
******************************************************************************/

void PScope(impl_tScope* abstract_phylum, uview current_view)
{
  impl_tScope_Scope* phylum = static_cast<impl_tScope_Scope*>(abstract_phylum);
  assert(phylum != NULL);

  header(phylum->id, true);
  string prefix = toString(phylum->id) + ".internal.";


  switch(current_view)
  {
    case(petrinetnew_enum):
      {
	globals::currentScope = prefix;

	Place *p1  = PN.newPlace(prefix + "initial");
	Place *p2  = PN.newPlace(prefix + "final");
	Place *p3  = PN.newPlace(prefix + "fault_in");
	Place *p4  = PN.newPlace(prefix + "compensate");
	Place *p5  = PN.newPlace(prefix + "compensated");
	Place *p6  = PN.newPlace(prefix + "stop");
	Place *p7  = PN.newPlace(prefix + "stopped");
	Place *p8  = PN.newPlace(prefix + "fault_up");
	Place *p9  = PN.newPlace(prefix + "Active");
	Place *p10 = PN.newPlace(prefix + "!Active"); p10->mark();
	Place *p11 = PN.newPlace(prefix + "Successful");
	Place *p12 = PN.newPlace(prefix + "!Successful"); p12->mark();
	Place *p13 = PN.newPlace(prefix + "inner_stopped");
	Place *p14 = PN.newPlace(prefix + "activity_initial");
	Place *p15 = PN.newPlace(prefix + "activity_final");
	Place *p16 = PN.newPlace(prefix + "fh_fault");
	Place *p17 = PN.newPlace(prefix + "ch_initial");
	Place *p18 = PN.newPlace(prefix + "th_initial");
	Place *p19 = PN.newPlace(prefix + "ch_fault_in");
	Place *p20 = PN.newPlace(prefix + "ch_fault_up");
	Place *p21 = PN.newPlace(prefix + "p21"); p21->mark();
	Place *p22 = PN.newPlace(prefix + "p22");
	Place *p23 = PN.newPlace(prefix + "p23");
	Place *p24 = PN.newPlace(prefix + "ch_stop");
	Place *p25 = PN.newPlace(prefix + "ch_stopped");

	Transition *t1 = PN.newTransition(prefix + "initialize");
	PN.newArc(p1, t1);
	PN.newArc(t1, p9);
	PN.newArc(p10, t1);
	PN.newArc(t1, p14);

	Transition *t2 = PN.newTransition(prefix + "finalize");
	PN.newArc(t2, p2);
	PN.newArc(p9, t2);
	PN.newArc(t2, p10);
	PN.newArc(t2, p11);
	PN.newArc(p12, t2);
	PN.newArc(p15, t2);

	Transition *t3 = PN.newTransition(prefix + "ft");
	PN.newArc(p3, t3);
	PN.newArc(t3, p6);
	PN.newArc(p9, t3);
	PN.newArc(t3, p10);
	PN.newArc(t3, p16);

	Transition *t4 = PN.newTransition(prefix + "ignore_fault");
	PN.newArc(p3, t4);
	PN.newArc(p10, t4, READ);

	Transition *t5 = PN.newTransition(prefix + "start_ch");
	PN.newArc(p4, t5);
	PN.newArc(p11, t5);
	PN.newArc(t5, p12);
	PN.newArc(t5, p17);

	Transition *t6 = PN.newTransition(prefix + "skip_ch");
	PN.newArc(p4, t6);
	PN.newArc(t6, p5);
	PN.newArc(p12, t6, READ);

	Transition *t7 = PN.newTransition(prefix + "start_th");
	PN.newArc(p9, t7);
	PN.newArc(t7, p10);
	PN.newArc(p13, t7);
	PN.newArc(t7, p18);
	PN.newArc(PN.findPlace("1.internal.!Exiting"), t7, READ);

	Transition *t8 = PN.newTransition(prefix + "skip_th");
	PN.newArc(t8, p7);
	PN.newArc(p13, t8);
	PN.newArc(PN.findPlace("1.internal.Exiting"), t8, READ);

	Transition *t9 = PN.newTransition(prefix + "rethrow_ch_fault");
	PN.newArc(p10, t9, READ);
	PN.newArc(p19, t9);
	PN.newArc(t9, p20);
	PN.newArc(p21, t9);
	PN.newArc(t9, p22);

	Transition *t10 = PN.newTransition(prefix + "handle_ch_fault");
	PN.newArc(t10, p3);
	PN.newArc(p9, t10, READ);
	PN.newArc(p19, t10);
	PN.newArc(p21, t10);
	PN.newArc(t10, p22);

	Transition *t11 = PN.newTransition(prefix + "ignore_ch_fault");
	PN.newArc(p19, t11);
	PN.newArc(p22, t11, READ);

	Transition *t12 = stop(p1, "initial", prefix);

	// now comes everything we need for compensation in loops
	assert(ASTEmap[phylum->id] != NULL);
	if (ASTEmap[phylum->id]->ancestorLoops().size() != 0 && parameters[P_LOOPCOUNT])
	{
	  // rename existing places
	  PN.renamePlace(prefix + "final", prefix + "final1");
	  PN.renamePlace(prefix + "compensate", prefix + "compensate1");
	  PN.renamePlace(prefix + "compensated", prefix + "compensated1");

	  // add new places (the interface places of the wrapper)
	  Place *p21 = PN.newPlace(prefix + "final");
	  Place *p22 = PN.newPlace(prefix + "compensate");
	  Place *p23 = PN.newPlace(prefix + "compensated");

	  vector<unsigned int> ancestor_loops = ASTEmap[phylum->id]->ancestorLoops();
	  cerr << ancestor_loops.size() << " ancestor_loops" << endl;
	  if (ancestor_loops.size() > 1)
	  {
	    vector<unsigned int> loop_bounds;
	    vector<unsigned int> loop_identifiers;
	    unsigned int i = 0;
	    for (vector<unsigned int>::iterator loop = ancestor_loops.begin(); loop != ancestor_loops.end(); loop++)
	    {
	      assert(ASTEmap[*loop] != NULL);

	      if (loop == ancestor_loops.begin())
		continue;
  
	      loop_bounds.push_back(ASTEmap[*loop]->max_loops);
	      loop_identifiers.push_back(*loop);
	    }

	    process_loop_bounds(loop_bounds, loop_identifiers, prefix, ASTEmap[*ancestor_loops.begin()]->max_loops); 
	  }
	  else
	  {
	    Place *c1 = PN.newPlace(prefix + "c");
	    Place *c2 = PN.newPlace(prefix + "!c");
	    c2->mark(ASTEmap[*ancestor_loops.begin()]->max_loops);
	    
	    Transition *t14 = PN.newTransition(prefix + "t14");
	    PN.newArc(PN.findPlace(prefix + "final1"), t14);
	    PN.newArc(PN.findPlace(prefix + "!Successful"), t14, READ);
	    PN.newArc(t14, PN.findPlace(prefix + "final"));

	    Transition *t15 = PN.newTransition(prefix + "t15");
	    PN.newArc(t15, c1);
	    PN.newArc(c2, t15);
	    PN.newArc(PN.findPlace(prefix + "final1"), t15);
	    PN.newArc(PN.findPlace(prefix + "Successful"), t15);
	    PN.newArc(t15, PN.findPlace(prefix + "!Successful"));
	    PN.newArc(t15, PN.findPlace(prefix + "final"));

	    Transition *t16 = PN.newTransition(prefix + "t16");
	    PN.newArc(c2, t16, READ, ASTEmap[*ancestor_loops.begin()]->max_loops);
	    PN.newArc(PN.findPlace(prefix + "compensated1"), t16);
	    PN.newArc(t16, PN.findPlace(prefix + "compensated"));

	    Transition *t17 = PN.newTransition(prefix + "t17");
	    PN.newArc(c1, t17);
	    PN.newArc(t17, c2);
	    PN.newArc(PN.findPlace(prefix + "compensate"), t17);
	    PN.newArc(t17, PN.findPlace(prefix + "ch_initial"));

	    PN.mergePlaces(prefix + "compensated1", prefix + "compensate");
	  }
	}

	// don't stop from "final1" -- that's why this line is down here
	Transition *t13 = stop(PN.findPlace(prefix + "final"), "final", prefix);


	// unparse the standard elements
	phylum->standardElements_1->unparse(pseudoPrinter, current_view);

	// unparse the embedded activity
	phylum->activity_1->unparse(pseudoPrinter, current_view);

	// unparse the fault handlers
	phylum->tFaultHandlers_1->unparse(pseudoPrinter, current_view);

	// unparse the compensation handler
	phylum->tCompensationHandler_1->unparse(pseudoPrinter, current_view);

	// unparse the termination handler
	phylum->tTerminationHandler_1->unparse(pseudoPrinter, current_view);

	// unparse the event handlers
	phylum->tEventHandlers_1->unparse(pseudoPrinter, current_view);


        if (ASTEmap[phylum->id]->hasEH)
	{
          Place *p26 = PN.newPlace(prefix + "p26");
          Place *p27 = PN.newPlace(prefix + "p27");
          Place *p28 = PN.newPlace(prefix + "p28");

          PN.newArc(t1, p23);
          PN.newArc(p26, t2);

	  Transition *t13 = PN.newTransition(prefix + "t13");
	  PN.newArc(p27, t13);
	  PN.newArc(p15, t13);
          PN.newArc(t13, p28);
          
          Transition *t14 = PN.newTransition(prefix + "t14");
	  PN.newArc(p21, t14);
          PN.newArc(p27, t14);
          PN.newArc(t14, p28);
          PN.newArc(t14, p22);
	  
          
          PN.mergePlaces((toString(phylum->tEventHandlers_1->id) + ".internal.initial"),   (prefix + "p23"));
          PN.mergePlaces((toString(phylum->tEventHandlers_1->id) + ".internal.running"),   (prefix + "p27"));
          PN.mergePlaces((toString(phylum->tEventHandlers_1->id) + ".internal.finishing"), (prefix + "p28"));
          PN.mergePlaces((toString(phylum->tEventHandlers_1->id) + ".internal.stop"),      (prefix + "p22"));
          PN.mergePlaces((toString(phylum->tEventHandlers_1->id) + ".internal.stopped"),   (prefix + "inner_stopped")); // was: "p13"
          PN.mergePlaces((toString(phylum->tEventHandlers_1->id) + ".internal.fault"),     (prefix + "fault_in")); // obsolete?
          PN.mergePlaces((toString(phylum->tEventHandlers_1->id) + ".internal.final"),     (prefix + "p26"));
	}


	// embed fault handler
	PN.mergePlaces((prefix + "inner_stopped"), (prefix + "faultHandler.initial"));
	PN.mergePlaces((prefix + "fh_fault"),      (prefix + "faultHandler.fault"));
	PN.mergePlaces((prefix + "final"),         (prefix + "faultHandler.final"));
	PN.mergePlaces((prefix + "stop"),          (prefix + "faultHandler.stop"));
	PN.mergePlaces((prefix + "stopped"),       (prefix + "faultHandler.stopped"));

	impl_tCompensationHandler *d = phylum->tCompensationHandler_1;

	// embed compensation handler
	with (d)
	{
	  d = CompensationHandler(ch_activity, *):
	  {
	    PN.mergePlaces((prefix + "ch_initial"), (toString(ch_activity->id) + ".initial"));
	    PN.mergePlaces((prefix + "ch_stop"),    (toString(ch_activity->id) + ".stop"));
	    PN.mergePlaces((prefix + "ch_stopped"), (toString(ch_activity->id) + ".stopped"));

	    if (ASTEmap[phylum->id]->ancestorLoops().size() != 0 && parameters[P_LOOPCOUNT])
	      PN.mergePlaces((prefix + "compensated1"), (toString(ch_activity->id) + ".final"));
	    else
	      PN.mergePlaces((prefix + "compensated"),  (toString(ch_activity->id) + ".final"));
	  }
	  default: { assert(false); }
	}

	// embed termination handler
	PN.mergePlaces((prefix + "th_initial"), (prefix + "terminationHandler.initial"));
	PN.mergePlaces((prefix + "stopped"),    (prefix + "terminationHandler.final"));
	PN.mergePlaces((prefix + "stop"),       (prefix + "terminationHandler.stop"));
	PN.mergePlaces((prefix + "stopped"),    (prefix + "terminationHandler.final"));

	// embed inner activity
	PN.mergePlaces((prefix + "activity_initial"), (toString(phylum->activity_1->id) + ".initial"));
	PN.mergePlaces((prefix + "activity_final"),   (toString(phylum->activity_1->id) + ".final"));
	PN.mergePlaces((prefix + "stop"),             (toString(phylum->activity_1->id) + ".stop"));
	PN.mergePlaces((prefix + "inner_stopped"),    (toString(phylum->activity_1->id) + ".stopped"));

      	// merge `ch_fault_up' place with `ch_fault_in' place of the parent scope
    	PN.mergePlaces((prefix + "ch_fault_up"), (toString(ASTEmap[phylum->id]->parentScopeId) + ".internal.ch_fault_in"));
  
	break;
      }

    case(petrinetsmall_enum):
      {
        cerr << "the handlers of the <scope>-activity will not be modelled!" << endl;

	Place *p1 = PN.newPlace(prefix + "initial");
	Place *p2 = PN.newPlace(prefix + "final");


	// unparse the standard elements
	phylum->standardElements_1->unparse(pseudoPrinter, current_view);

	// unparse the embedded activity
	phylum->activity_1->unparse(pseudoPrinter, current_view);

  
	// Embed the inner activity.
	PN.mergePlaces(phylum->id, ".internal.initial", phylum->activity_1->id, ".initial");
	PN.mergePlaces(phylum->id, ".internal.final", phylum->activity_1->id, ".final");

	break;
      }

    default: { /* no pattern in this case */ }
  }


  footer(phylum->id, true);
}
