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
 * \file    petrinet-patterns-scopes.cc
 *
 * \brief   Petri net patterns for BPEL's <scope> and <process>
 *
 * \author  Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          Christian Gierds <gierds@informatik.hu-berlin.de>,
 *          last changes of: \$Author: nielslohmann $
 * 
 * \since   2006-11-29
 *
 * \date    \$Date: 2007/06/05 15:04:00 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \note    This file was created using Kimwitu++ reading file petrinet-patterns-scopes.k.
 *          See http://site.informatik.hu-berlin.de/kimwitu++ for details.
 *
 * \version \$Revision: 1.47 $
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
#include "petrinet-patterns-shortcuts.h"

extern PetriNet PN;
%}





/******************************************************************************
  PROCESS
******************************************************************************/

%{
/*!
 * \fn      void kc::PProcess(impl_tProcess* abstract_phylum, uview current_view)
 *
 * \brief   Pattern for the <process>.
 *
 * \param   abstract_phylum  pointer to the phylum of the abstract syntax tree
 * \param   current_view     the current unparse view
 *
 * \ingroup patterns
 */
%}
void PProcess(impl_tProcess* abstract_phylum, uview current_view)
{
  impl_tProcess_Process* phylum = static_cast<impl_tProcess_Process*>(abstract_phylum);
  assert(phylum != NULL);
  
  header(phylum->id, true);
  string prefix = toString(phylum->id) + ".internal.";
  
  // Create the interface places. The used lists are filled during parsing
  // mapping each combination of partnerLink, portType and operation to one
  // input (resp. output) place.
  // If an interface is associated with many instances, several copies of
  // it are created.
  for (map<string, unsigned int>::const_iterator in = globals::ASTE_inputChannels.begin();
       in != globals::ASTE_inputChannels.end(); in++)
  {
    // case 1: only one instance of each channel is needed
    if (in->second == 1 || globals::instances_of_current_process != 0)
    {
      trace(TRACE_DEBUG, "[PNU]\tcreating channel `in." + in->first + "'\n");
      PN.newPlace("in." + in->first, IN);
    }
    else // case 2: several instances of each channel are needed
    {
      for (unsigned int i = 1; i <= in->second; i++)
      {
        trace(TRACE_DEBUG, "[PNU]\tcreating channel `in." + in->first + ".instance_" + toString(i) + "'\n");
      	PN.newPlace("in." + in->first + ".instance_" + toString(i), IN);
      }
    }
  }
  
  for (map<string, unsigned int>::const_iterator out = globals::ASTE_outputChannels.begin();
       out != globals::ASTE_outputChannels.end(); out++)
  {
    // case 1: only one instance of each channel is needed
    if (out->second == 1 || globals::instances_of_current_process != 0)
    {
      trace(TRACE_DEBUG, "[PNU]\tcreating channel `out." + out->first + "'\n");
      PN.newPlace("out." + out->first, OUT);
    }
    else // case 2: several instances of each channel are needed
      for (unsigned int i = 1; i <= out->second; i++)
      {
        trace(TRACE_DEBUG, "[PNU]\tcreating channel `out." + out->first + ".instance_" + toString(i) + "'\n");
      	PN.newPlace("out." + out->first + ".instance_" + toString(i), OUT);
      }
  }
    
    
    // Create the places for the links. The used list is filled during parsing
    // and contains all links of the whole process -- also links defined in a
    // deeper hierarchy.
    for (set<string>::iterator link = globals::ASTE_linkNames.begin(); link != globals::ASTE_linkNames.end(); link++)
    {
      PN.newPlace("link." + *link);
      PN.newPlace("!link." + *link);
    }
    
    
    switch(current_view)
    {
      case(petrinetnew_enum):
      {
        // Places.
        Place *p1  = PN.newPlace(prefix + "initial");
        Place *p2  = PN.newPlace(prefix + "final");
        Place *p3  = PN.newPlace(prefix + "fault_in");
        Place *p4  = PN.newPlace(prefix + "exit");
        Place *p5  = PN.newPlace(prefix + "Active");
        Place *p6  = PN.newPlace(prefix + "!Active");
        Place *p7  = PN.newPlace(prefix + "Exiting");
        Place *p8  = PN.newPlace(prefix + "!Exiting");
        Place *p9  = PN.newPlace(prefix + "inner_stop");
        Place *p10 = PN.newPlace(prefix + "inner_stopped");
        Place *p11 = PN.newPlace(prefix + "eha_initial");
        Place *p12 = PN.newPlace(prefix + "eha_final");
        Place *p13 = PN.newPlace(prefix + "fh_fault");
        Place *p14 = PN.newPlace(prefix + "ch_fault_in");
        
        // Process is initially in states "!Active" and "!Exiting".
        p6->mark();
        p8->mark();
        
        // Indicate the process's initial and final marking.
        p1->mark();
        p2->isFinal = true;
        
        Transition *t1 = PN.newTransition(prefix + "initialize");
        PN.newArc(p1, t1);
        PN.newArc(t1, p5);
        PN.newArc(p6, t1);
        PN.newArc(t1, p11);
        
        // Create places for the variables and connect them with the
        // initializing transition.
        if ( globals::parameters[P_VARIABLES] )
        {
          for ( set<string>::iterator variable = globals::ASTE_variableNames.begin(); variable != globals::ASTE_variableNames.end(); variable++ )
          {
            Place * pVar = PN.newPlace("variable." + (*variable));
            PN.newArc(t1, pVar);
          }
        }
        
        Transition *t2 = PN.newTransition(prefix + "finalize");
        PN.newArc(t2, p2);
        PN.newArc(p5, t2);
        PN.newArc(t2, p6);
        if (!globals::ASTEmap[phylum->id]->hasEH)
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
        
        
        // Create subpattern for the event handlers and the inner activity.
        if (globals::ASTEmap[phylum->id]->hasEH)
        {
          Place *p15 = PN.newPlace(prefix + "eh_start");
          Place *p16 = PN.newPlace(prefix + "eh_active");
          Place *p17 = PN.newPlace(prefix + "eh_disabling");
          Place *p20 = PN.newPlace(prefix + "eh_disabled");
          Place *p18 = PN.newPlace(prefix + "eh_stopped");
          Place *p19 = PN.newPlace(prefix + "ia_final");
          
          Transition *t11 = PN.newTransition(prefix + "disable_eh");
          PN.newArc(p19, t11);
          PN.newArc(p16, t11);
          PN.newArc(t11, p17);
          
          Transition *t12 = PN.newTransition(prefix + "eh_finalize");
          PN.newArc(p19, t12);
          PN.newArc(p20, t12);
          PN.newArc(t12, p12);
          
          // Unparse and embed the event handlers.
          phylum->tEventHandlers_1->unparse(pseudoPrinter, current_view);
          PN.mergePlaces((prefix + "eha_initial"),  (toString(phylum->tEventHandlers_1->id) + ".initial"));
          PN.mergePlaces((prefix + "inner_stop"),   (toString(phylum->tEventHandlers_1->id) + ".stop"));
          PN.mergePlaces((prefix + "eha_final"),    (toString(phylum->tEventHandlers_1->id) + ".disabled"));
          PN.mergePlaces((prefix + "eh_start"),     (toString(phylum->tEventHandlers_1->id) + ".start"));
          PN.mergePlaces((prefix + "eh_active"),    (toString(phylum->tEventHandlers_1->id) + ".active"));
          PN.mergePlaces((prefix + "eh_disabling"), (toString(phylum->tEventHandlers_1->id) + ".disabling"));
          PN.mergePlaces((prefix + "eh_stopped"),   (toString(phylum->tEventHandlers_1->id) + ".stopped"));
          
          // Unparse and embed the inner activity.
          phylum->activity_1->unparse(pseudoPrinter, current_view);
          PN.mergePlaces((prefix + "eh_start"),      (toString(phylum->activity_1->id) + ".initial"));
          PN.mergePlaces((prefix + "ia_final"),      (toString(phylum->activity_1->id) + ".final"));
          PN.mergePlaces((prefix + "eh_stopped"),    (toString(phylum->activity_1->id) + ".stop"));
          PN.mergePlaces((prefix + "inner_stopped"), (toString(phylum->activity_1->id) + ".stopped"));
        }
        else
        {
          // Unparse and embed the inner activity.
          phylum->activity_1->unparse(pseudoPrinter, current_view);
          PN.mergePlaces((prefix + "eha_initial"),   (toString(phylum->activity_1->id) + ".initial"));
          PN.mergePlaces((prefix + "eha_final"),     (toString(phylum->activity_1->id) + ".final"));
          PN.mergePlaces((prefix + "inner_stop"),    (toString(phylum->activity_1->id) + ".stop"));
          PN.mergePlaces((prefix + "inner_stopped"), (toString(phylum->activity_1->id) + ".stopped"));
        }
        
        // Unparse the fault handlers (the handlers embed themselves to the
        // process).
        phylum->tFaultHandlers_1->unparse(pseudoPrinter, current_view);
        
        break;
      }
        
        
      case(petrinetsmall_enum):
      {
        // Places.
        Place *p1 = PN.newPlace(prefix + "initial");
        Place *p2 = PN.newPlace(prefix + "final");
        
        // Indicate the process's initial and final marking.
        p1->mark();
        p2->isFinal = true;
        
        // Unparse the embedded activity.
        phylum->activity_1->unparse(pseudoPrinter, current_view);
        
        // No event handlers: embed activity.
        if (globals::ASTEmap[phylum->id]->hasEH == false)
        {
          PN.mergePlaces(phylum->id, ".internal.initial", phylum->activity_1->id, ".initial");
          PN.mergePlaces(phylum->id, ".internal.final", phylum->activity_1->id, ".final");
        }
        
        // Event handlers present.
        if (globals::ASTEmap[phylum->id]->hasEH == true)
        {
          Place *p3 = PN.newPlace(prefix + "activity_begin");
          Place *p4 = PN.newPlace(prefix + "activity_done");
          Place *p5 = PN.newPlace(prefix + "disabling_eh");
          Place *p6 = PN.newPlace(prefix + "active_eh");
          
          Transition *t1 = PN.newTransition(prefix + "disable_eh");
          PN.newArc(p4, t1);
          PN.newArc(t1, p5);
          PN.newArc(p6, t1);
          
          // Unparse the event handlers.
          phylum->tEventHandlers_1->unparse(pseudoPrinter, current_view);
          
          // Embed the inner activity.
          PN.mergePlaces(phylum->id, ".internal.activity_begin", phylum->activity_1->id, ".initial");
          PN.mergePlaces(phylum->id, ".internal.activity_done", phylum->activity_1->id, ".final");
          
          // Embed the event handlers.
          PN.mergePlaces(phylum->id, ".internal.initial",        phylum->tEventHandlers_1->id, ".initial");
          PN.mergePlaces(phylum->id, ".internal.final",          phylum->tEventHandlers_1->id, ".disabled");
          PN.mergePlaces(phylum->id, ".internal.disabling_eh",   phylum->tEventHandlers_1->id, ".disabling");
          PN.mergePlaces(phylum->id, ".internal.active_eh",      phylum->tEventHandlers_1->id, ".active");
          PN.mergePlaces(phylum->id, ".internal.activity_begin", phylum->tEventHandlers_1->id, ".start");
        }
        
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

%{
/*!
 * \fn      void kc::PScope(impl_tScope* abstract_phylum, uview current_view)
 *
 * \brief   Pattern for the <scope> activity.
 *
 *          \image html "scope.png" "pattern"
 *
 * \param   abstract_phylum  pointer to the phylum of the abstract syntax tree
 * \param   current_view     the current unparse view
 *
 * \ingroup patterns
 *
 * \todo    Remove or implement code snippet for loop count parameter.
 */
%}
void PScope(impl_tScope* abstract_phylum, uview current_view)
{
  impl_tScope_Scope* phylum = static_cast<impl_tScope_Scope*>(abstract_phylum);
  assert(phylum != NULL);
  assert(globals::ASTEmap[phylum->id] != NULL);
  
  header(phylum->id, true);
  string prefix = toString(phylum->id) + ".internal.";
  
  
  switch(current_view)
  {
    case(petrinetnew_enum):
    {
      // Places.
      Place *p1  = PN.newPlace(prefix + "initial");
      Place *p2  = PN.newPlace(prefix + "final");
      Place *p3  = PN.newPlace(prefix + "fault_in");
      Place *p4  = PN.newPlace(prefix + "compensate");
      Place *p5  = PN.newPlace(prefix + "compensated");
      Place *p6  = PN.newPlace(prefix + "stop");
      Place *p7  = PN.newPlace(prefix + "stopped");
      Place *p8  = PN.newPlace(prefix + "fault_up");
      Place *p9  = PN.newPlace(prefix + "Active");
      Place *p10 = PN.newPlace(prefix + "!Active");
      Place *p11 = PN.newPlace(prefix + "Successful");
      Place *p12 = PN.newPlace(prefix + "!Successful");
      Place *p13 = PN.newPlace(prefix + "inner_stopped");
      Place *p14 = PN.newPlace(prefix + "eha_initial");
      Place *p15 = PN.newPlace(prefix + "eha_final");
      Place *p16 = PN.newPlace(prefix + "fh_fault");
      Place *p17 = PN.newPlace(prefix + "ch_initial");
      Place *p18 = PN.newPlace(prefix + "th_initial");
      Place *p19 = PN.newPlace(prefix + "ch_fault_in");
      Place *p20 = PN.newPlace(prefix + "ch_fault_up");
      Place *p21 = PN.newPlace(prefix + "!ThrownCHFault");
      Place *p22 = PN.newPlace(prefix + "ThrownCHFault");
      Place *p23 = PN.newPlace(prefix + "ch_stop");
      Place *p24 = PN.newPlace(prefix + "ch_stopped");
      
      // Scope is initially in states "!Active", "!Successful" and "!ThrownCHFault".
      p10->mark();
      p12->mark();
      p21->mark();
      
      // Initialize scope: start inner activity, set state to "Active".
      Transition *t1 = PN.newTransition(prefix + "initialize");
      PN.newArc(p1, t1);
      PN.newArc(t1, p9);
      PN.newArc(p10, t1);
      PN.newArc(t1, p14);
      
      // Finalize scope: end inner activity, set state to "Successful".
      Transition *t2 = PN.newTransition(prefix + "finalize");
      PN.newArc(t2, p2);
      PN.newArc(p9, t2);
      PN.newArc(t2, p10);
      PN.newArc(t2, p11);
      PN.newArc(p12, t2);
      PN.newArc(p15, t2);
      
      // Forced termination: stop inner activity.
      Transition *t3 = PN.newTransition(prefix + "forced_termination");
      PN.newArc(p3, t3);
      PN.newArc(t3, p6);
      PN.newArc(p9, t3);
      PN.newArc(t3, p10);
      PN.newArc(t3, p16);
      
      // Ignore further faults.
      Transition *t4 = PN.newTransition(prefix + "ignore_fault");
      PN.newArc(p3, t4);
      PN.newArc(p10, t4, READ);
      
      // Start compensation handler.
      Transition *t5 = PN.newTransition(prefix + "start_ch");
      PN.newArc(p4, t5);
      PN.newArc(p11, t5);
      PN.newArc(t5, p12);
      PN.newArc(t5, p17);
      
      // Skip compensation handler (in case that it was already called or the
      // scope did not complete successfully).
      Transition *t6 = PN.newTransition(prefix + "skip_ch");
      PN.newArc(p4, t6);
      PN.newArc(t6, p5);
      PN.newArc(p12, t6, READ);
      
      // Start termination handler (in case the process is not exiting).
      Transition *t7 = PN.newTransition(prefix + "start_th");
      PN.newArc(p9, t7);
      PN.newArc(t7, p10);
      PN.newArc(p13, t7);
      PN.newArc(t7, p18);
      PN.newArc(PN.findPlace("1.internal.!Exiting"), t7, READ);
      
      // Skip termination handler (in case the process is exiting).
      Transition *t8 = PN.newTransition(prefix + "skip_th");
      PN.newArc(t8, p7);
      PN.newArc(p13, t8);
      PN.newArc(PN.findPlace("1.internal.Exiting"), t8, READ);
      
      // Rethrow compensation handler's fault (in case this scope's fault
      // handlers are uninstalled and it is the first compensation handler's
      // fault to be thrown up).
      Transition *t9 = PN.newTransition(prefix + "rethrow_ch_fault");
      PN.newArc(p10, t9, READ);
      PN.newArc(p19, t9);
      PN.newArc(t9, p20);
      PN.newArc(p21, t9);
      PN.newArc(t9, p22);
      
      // Handle compensation handler's fault (in case this scope's fault
      // handlers are installed and it is the first compensation handler's
      // fault thrown to this scope.
      Transition *t10 = PN.newTransition(prefix + "handle_ch_fault");
      PN.newArc(t10, p3);
      PN.newArc(p9, t10, READ);
      PN.newArc(p19, t10);
      PN.newArc(p21, t10);
      PN.newArc(t10, p22);
      
      // Ignore compensation handler's fault (in case it is not the first one
      // thrown to this scope).
      Transition *t11 = PN.newTransition(prefix + "ignore_ch_fault");
      PN.newArc(p19, t11);
      PN.newArc(p22, t11, READ);
      
      // Stopping transitions.
      Transition *t12 = stop(p1, "initial", prefix);
      Transition *t13 = stop(p2, "final", prefix);
      
      
      // Unparse the standard elements.
      phylum->standardElements_1->unparse(pseudoPrinter, current_view);
      
      
      // Create subpattern for the event handlers and the inner activity.
      if (globals::ASTEmap[phylum->id]->hasEH)
      {
        Place *p25 = PN.newPlace(prefix + "eh_start");
        Place *p26 = PN.newPlace(prefix + "eh_active");
        Place *p27 = PN.newPlace(prefix + "eh_disabling");
        Place *p30 = PN.newPlace(prefix + "eh_disabled");
        Place *p28 = PN.newPlace(prefix + "eh_stopped");
        Place *p29 = PN.newPlace(prefix + "ia_final");
        
        Transition *t14 = PN.newTransition(prefix + "disable_eh");
        PN.newArc(p29, t14, READ);
        PN.newArc(p26, t14);
        PN.newArc(t14, p27);
        
        Transition *t15 = PN.newTransition(prefix + "eh_finalize");
        PN.newArc(p29, t15);
        PN.newArc(p30, t15);
        PN.newArc(t15, p15);
        
        // Unparse and embed the event handlers.
        phylum->tEventHandlers_1->unparse(pseudoPrinter, current_view);
        PN.mergePlaces((prefix + "eha_initial"),  (toString(phylum->tEventHandlers_1->id) + ".initial"));
        PN.mergePlaces((prefix + "stop"),         (toString(phylum->tEventHandlers_1->id) + ".stop"));
        PN.mergePlaces((prefix + "eh_disabled"),    (toString(phylum->tEventHandlers_1->id) + ".disabled"));
        PN.mergePlaces((prefix + "eh_start"),     (toString(phylum->tEventHandlers_1->id) + ".start"));
        PN.mergePlaces((prefix + "eh_active"),    (toString(phylum->tEventHandlers_1->id) + ".active"));
        PN.mergePlaces((prefix + "eh_disabling"), (toString(phylum->tEventHandlers_1->id) + ".disabling"));
        PN.mergePlaces((prefix + "eh_stopped"),   (toString(phylum->tEventHandlers_1->id) + ".stopped"));
        
        // Unparse and embed the inner activity.
        phylum->activity_1->unparse(pseudoPrinter, current_view);
        PN.mergePlaces((prefix + "eh_start"),      (toString(phylum->activity_1->id) + ".initial"));
        PN.mergePlaces((prefix + "ia_final"),      (toString(phylum->activity_1->id) + ".final"));
        PN.mergePlaces((prefix + "eh_stopped"),    (toString(phylum->activity_1->id) + ".stop"));
        PN.mergePlaces((prefix + "inner_stopped"), (toString(phylum->activity_1->id) + ".stopped"));
      }
      else
      {
        // Unparse and embed the inner activity.
        phylum->activity_1->unparse(pseudoPrinter, current_view);
        PN.mergePlaces((prefix + "eha_initial"),   (toString(phylum->activity_1->id) + ".initial"));
        PN.mergePlaces((prefix + "eha_final"),     (toString(phylum->activity_1->id) + ".final"));
        PN.mergePlaces((prefix + "stop"),          (toString(phylum->activity_1->id) + ".stop"));
        PN.mergePlaces((prefix + "inner_stopped"), (toString(phylum->activity_1->id) + ".stopped"));
      }
      
      
      // Unparse the FTC handlers (the handlers embed themselves to this
      // scope).
      phylum->tFaultHandlers_1->unparse(pseudoPrinter, current_view);
      phylum->tTerminationHandler_1->unparse(pseudoPrinter, current_view);
      phylum->tCompensationHandler_1->unparse(pseudoPrinter, current_view); 
      
      
      // Embed scope to parent scope: Merge `ch_fault_up' place with
      // `ch_fault_in' place of the parent scope.
    	PN.mergePlaces((prefix + "ch_fault_up"), (toString(globals::ASTEmap[phylum->id]->parentScopeId) + ".internal.ch_fault_in"));
      
      break;
      }
      
      
      
    case(petrinetsmall_enum):
    {
      genericError(120, "", globals::ASTEmap[phylum->id]->attributes["referenceLine"], ERRORLEVEL_NOTICE);
      
      // Places.
      Place *p1 = PN.newPlace(prefix + "initial");
      Place *p2 = PN.newPlace(prefix + "final");
      
      // Unparse the standard elements.
      phylum->standardElements_1->unparse(pseudoPrinter, current_view);
      
      // Unparse the embedded activity.
      phylum->activity_1->unparse(pseudoPrinter, current_view);
      
      // No event handlers: embed activity.
      if (globals::ASTEmap[phylum->id]->hasEH == false)
      {
        PN.mergePlaces(phylum->id, ".internal.initial", phylum->activity_1->id, ".initial");
        PN.mergePlaces(phylum->id, ".internal.final", phylum->activity_1->id, ".final");
      }
      
      // Event handlers present.
      if (globals::ASTEmap[phylum->id]->hasEH == true)
      {
        Place *p3 = PN.newPlace(prefix + "activity_begin");
        Place *p4 = PN.newPlace(prefix + "activity_done");
        Place *p5 = PN.newPlace(prefix + "disabling_eh");
        Place *p6 = PN.newPlace(prefix + "active_eh");
        
        Transition *t1 = PN.newTransition(prefix + "disable_eh");
        PN.newArc(p4, t1);
        PN.newArc(t1, p5);
        PN.newArc(p6, t1);
        
        // Unparse the event handlers.
        phylum->tEventHandlers_1->unparse(pseudoPrinter, current_view);
        
        // Embed the inner activity.
        PN.mergePlaces(phylum->id, ".internal.activity_begin", phylum->activity_1->id, ".initial");
        PN.mergePlaces(phylum->id, ".internal.activity_done", phylum->activity_1->id, ".final");
        
        // Embed the event handlers.
        PN.mergePlaces(phylum->id, ".internal.initial",        phylum->tEventHandlers_1->id, ".initial");
        PN.mergePlaces(phylum->id, ".internal.final",          phylum->tEventHandlers_1->id, ".disabled");
        PN.mergePlaces(phylum->id, ".internal.disabling_eh",   phylum->tEventHandlers_1->id, ".disabling");
        PN.mergePlaces(phylum->id, ".internal.active_eh",      phylum->tEventHandlers_1->id, ".active");
        PN.mergePlaces(phylum->id, ".internal.activity_begin", phylum->tEventHandlers_1->id, ".start");
      }
      
      break;
    }
      
    default: { /* no pattern in this case */ }
  }
  
  
  footer(phylum->id, true);
}







/*
	// now comes everything we need for compensation in loops
	assert(globals::ASTEmap[phylum->id] != NULL);
	if (globals::ASTEmap[phylum->id]->ancestorLoops().size() != 0 && globals::parameters[P_LOOPCOUNT])
	{
	  // rename existing places
	  PN.renamePlace(prefix + "final", prefix + "final1");
	  PN.renamePlace(prefix + "compensate", prefix + "compensate1");
	  PN.renamePlace(prefix + "compensated", prefix + "compensated1");

	  // add new places (the interface places of the wrapper)
	  Place *p21 = PN.newPlace(prefix + "final");
	  Place *p22 = PN.newPlace(prefix + "compensate");
	  Place *p23 = PN.newPlace(prefix + "compensated");

	  vector<unsigned int> ancestor_loops = globals::ASTEmap[phylum->id]->ancestorLoops();
	  cerr << ancestor_loops.size() << " ancestor_loops" << endl;
	  if (ancestor_loops.size() > 1)
	  {
	    vector<unsigned int> loop_bounds;
	    vector<unsigned int> loop_identifiers;
	    unsigned int i = 0;
	    for (vector<unsigned int>::iterator loop = ancestor_loops.begin(); loop != ancestor_loops.end(); loop++)
	    {
	      assert(globals::ASTEmap[*loop] != NULL);

	      if (loop == ancestor_loops.begin())
		continue;
  
	      loop_bounds.push_back(globals::ASTEmap[*loop]->max_loops);
	      loop_identifiers.push_back(*loop);
	    }

	    process_loop_bounds(loop_bounds, loop_identifiers, prefix, globals::ASTEmap[*ancestor_loops.begin()]->max_loops); 
	  }
	  else
	  {
	    Place *c1 = PN.newPlace(prefix + "c");
	    Place *c2 = PN.newPlace(prefix + "!c");
	    c2->mark(globals::ASTEmap[*ancestor_loops.begin()]->max_loops);
	    
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
	    PN.newArc(c2, t16, READ, globals::ASTEmap[*ancestor_loops.begin()]->max_loops);
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

        // that line is moved up as I commented this out...
*/
