/*****************************************************************************\
 * Copyright 2005, 2006 Niels Lohmann, Christian Gierds, Dennis Reinert      *
 *                                                                           *
 * This file is part of BPEL2oWFN.                                           *
 *                                                                           *
 * BPEL2oWFN is free software; you can redistribute it and/or modify it      *
 * under the terms of the GNU General Public License as published by the     *
 * Free Software Foundation; either version 2 of the License, or (at your    *
 * option) any later version.                                                *
 *                                                                           *
 * BPEL2oWFN is distributed in the hope that it will be useful, but WITHOUT  *
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or     *
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for  *
 * more details.                                                             *
 *                                                                           *
 * You should have received a copy of the GNU General Public License along   *
 * with BPEL2oWFN; if not, write to the Free Software Foundation, Inc., 51   *
 * Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.                      *
\*****************************************************************************/

%{
/*!
 * \file bpel-unparse-petri-new.cc
 *
 * \brief Petri net unparser (implementation)
 *
 * \author
 *          - responsible: Niels Lohmann <nlohmann@informatik.hu-berlin.de>
 *          - last changes of: \$Author: nlohmann $
 *
 * \date
 *          - created 2006-10-29
 *          - last changed: \$Date: 2006/10/30 12:43:40 $
 *
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \note    This file was created using Kimwitu++ reading file
 *          bpel-unparse-petri.k.
 *          See http://site.informatik.hu-berlin.de/kimwitu++ for details.
 *
 * \version \$Revision: 1.4 $
 *
 */
%}





/******************************************************************************
  PROCESS
******************************************************************************/

Process(a,b,c,d,e,f,g,h,i,_id) ->
  [petrinetnew:
    {
      trace(TRACE_INFORMATION, "Generating Petri net...\n");
      header(_id, true);

      string prefix = toString(_id) + ".internal.";
      currentScope = prefix;


      // Create the interface places.
      // The used lists are filled during parsing mapping each combination of
      // partnerLink, portType and operation to one input (resp. output) place.
      for (set<string>::iterator in = ASTE_inputChannels.begin(); in != ASTE_inputChannels.end(); in++)
	TheNet->newPlace("in." + *in, IN);
      for (set<string>::iterator out = ASTE_outputChannels.begin(); out != ASTE_outputChannels.end(); out++)
	TheNet->newPlace("out." + *out, OUT);


      // Create the places for the links.
      // The used list is filled during parsing and contains all links of the
      // whole process -- also links defined in a deeper hierarchy.
      for (set<string>::iterator link = ASTE_links.begin(); link != ASTE_links.end(); link++)
      {
	TheNet->newPlace("link." + *link);
	TheNet->newPlace("!link." + *link);
      }


      Place *p1  = TheNet->newPlace(prefix + "initial"); p1->mark();
      Place *p2  = TheNet->newPlace(prefix + "final");
      Place *p3  = TheNet->newPlace(prefix + "activity_stop");
      Place *p4  = TheNet->newPlace(prefix + "activity_stopped");
      Place *p5  = TheNet->newPlace(prefix + "fault_in");
      Place *p6  = TheNet->newPlace(prefix + "stopping");
      Place *p7  = TheNet->newPlace(prefix + "fh_fault_in");
      Place *p8  = TheNet->newPlace(prefix + "fh_stop");
      Place *p9  = TheNet->newPlace(prefix + "!FH_Faulted"); p9->mark();
      Place *p10 = TheNet->newPlace(prefix + "FH_Faulted");
      Place *p11 = TheNet->newPlace(prefix + "!Faulted"); p11->mark();
      Place *p12 = TheNet->newPlace(prefix + "Faulted");
      Place *p13 = TheNet->newPlace(prefix + "!Terminated"); p13->mark();
      Place *p14 = TheNet->newPlace(prefix + "Terminated");

      Transition *t1 = TheNet->newTransition(prefix + "t1");
      TheNet->newArc(p5, t1);
      TheNet->newArc(t1, p3);
      TheNet->newArc(t1, p6);
      TheNet->newArc(p11, t1);
      TheNet->newArc(t1, p12);

      Transition *t2 = TheNet->newTransition(prefix + "t2");
      TheNet->newArc(p4, t2);
      TheNet->newArc(p6, t2);
      TheNet->newArc(t2, p7);

      Transition *t3 = TheNet->newTransition(prefix + "t3");
      TheNet->newArc(p5, t3);
      TheNet->newArc(p12, t3, READ);
    }
    e // faultHandler
    i // innerActivity
    {
      // embed fault handler
      TheNet->mergePlaces(TheNet->findPlace(prefix + "fh_fault_in"), TheNet->findPlace(prefix + "faultHandler.fault"));
      TheNet->mergePlaces(TheNet->findPlace(prefix + "final"), TheNet->findPlace(prefix + "faultHandler.handled"));
      TheNet->mergePlaces(TheNet->findPlace(prefix + "final"), TheNet->findPlace(prefix + "faultHandler.!handled"));
      TheNet->mergePlaces(TheNet->findPlace(prefix + "fh_stop"), TheNet->findPlace(prefix + "faultHandler.stop"));
      TheNet->mergePlaces(TheNet->findPlace(prefix + "final"), TheNet->findPlace(prefix + "faultHandler.stopped"));

      // embed inner activity
      TheNet->mergePlaces(TheNet->findPlace(prefix + "initial"), TheNet->findPlace(i->id, ".initial"));
      TheNet->mergePlaces(TheNet->findPlace(prefix + "final"), TheNet->findPlace(i->id, ".final"));
      TheNet->mergePlaces(TheNet->findPlace(prefix + "activity_stop"), TheNet->findPlace(i->id, ".stop"));
      TheNet->mergePlaces(TheNet->findPlace(prefix + "activity_stopped"), TheNet->findPlace(i->id, ".stopped"));

      footer(_id, true);
      trace(TRACE_INFORMATION, "Generating Petri net complete.\n");
    }
  ]
;










This=userDefinedFaultHandler(a,b, _id) ->
  [petrinetnew:
    {
      string prefix = toString(ASTEmap[_id->value]->parentScopeId) + ".internal.faultHandler.";
      trace(TRACE_DEBUG, "[PNU]" + inString() + "<userDefinedFaultHandler scope=\"" + currentScope + "\">\n");

      Place *p1 =  TheNet->newPlace(prefix + "fault");
      Place *p2 =  TheNet->newPlace(prefix + "handled");
      Place *p3 =  TheNet->newPlace(prefix + "!handled");
      Place *p4 =  TheNet->newPlace(prefix + "stop");
      Place *p5 =  TheNet->newPlace(prefix + "stopped");
    }
    a // catch-branches
    b // catchAll-branch
    {
      int catchBranches = 1;
      foreach (catchBranch=Catch(innerActivity); tCatch_list a)
      {
        Transition *t1 = TheNet->newTransition(prefix + "catch" + toString(catchBranches++));
        TheNet->newArc(p1, t1);
	TheNet->newArc(t1, TheNet->findPlace(innerActivity, ".initial"));
	TheNet->mergePlaces(TheNet->findPlace(prefix + "handled"), TheNet->findPlace(innerActivity, ".final"));
	TheNet->mergePlaces(TheNet->findPlace(prefix + "stop"), TheNet->findPlace(innerActivity, ".stop"));
	TheNet->mergePlaces(TheNet->findPlace(prefix + "stopped"), TheNet->findPlace(innerActivity, ".stopped"));
      }

      with(b) {
	CatchAll(innerActivity, _id):
	{
	  Transition *t2 = TheNet->newTransition(prefix + "catchAll");
	  TheNet->newArc(p1, t2);
	  TheNet->newArc(t2, TheNet->findPlace(innerActivity, ".initial"));

	  TheNet->mergePlaces(TheNet->findPlace(prefix + "handled"), TheNet->findPlace(innerActivity, ".final"));
	  TheNet->mergePlaces(TheNet->findPlace(prefix + "stop"), TheNet->findPlace(innerActivity, ".stop"));
	  TheNet->mergePlaces(TheNet->findPlace(prefix + "stopped"), TheNet->findPlace(innerActivity, ".stopped"));
	}
        NoCatchAll():
        {
          Transition *t2 = TheNet->newTransition(prefix + "rethrow");
          TheNet->newArc(p1, t2);
          TheNet->newArc(t2, p3);
        }
	default: { /* this should never happen */ }
      }

      trace(TRACE_DEBUG, "[PNU]" + inString() + "</userDefinedFaultHandler scope=\"" + currentScope + "\">\n");
    }
  ]
;






This=Scope(a,b,c,d,e,f,g,h,_id) ->
  [petrinetnew:
    {
      header(This->id, true);
      string prefix = toString(This->id) + ".internal.";
      assert(This->id == _id->value);

      currentScope = prefix;

      Place *p1  = TheNet->newPlace(prefix + "initial");
      Place *p2  = TheNet->newPlace(prefix + "activity_final");
      Place *p3  = TheNet->newPlace(prefix + "final");
      Place *p4  = TheNet->newPlace(prefix + "activity_stop");
      Place *p5  = TheNet->newPlace(prefix + "activity_stopped");
      Place *p6  = TheNet->newPlace(prefix + "!Successful"); p6->mark();
      Place *p7  = TheNet->newPlace(prefix + "Successful");
      Place *p8  = TheNet->newPlace(prefix + "compensateScope");
      Place *p9  = TheNet->newPlace(prefix + "ch_activity_initial");
      Place *p10 = TheNet->newPlace(prefix + "ch_activity_stop");
      Place *p11 = TheNet->newPlace(prefix + "ch_activity_stopped");
      Place *p12 = TheNet->newPlace(prefix + "scopeCompensated");
      Place *p13 = TheNet->newPlace(prefix + "fault_in");
      Place *p14 = TheNet->newPlace(prefix + "stopping_fault");
      Place *p15 = TheNet->newPlace(prefix + "fh_fault_in");
      Place *p16 = TheNet->newPlace(prefix + "rethrow");
      Place *p17 = TheNet->newPlace(prefix + "fh_stop");
      Place *p18 = TheNet->newPlace(prefix + "fh_stopped");
      Place *p19 = TheNet->newPlace(prefix + "!Faulted"); p19->mark();
      Place *p20 = TheNet->newPlace(prefix + "Faulted");
      Place *p21 = TheNet->newPlace(prefix + "stop");
      Place *p22 = TheNet->newPlace(prefix + "stopped");
      Place *p23 = TheNet->newPlace(prefix + "stopping_termination1");
      Place *p24 = TheNet->newPlace(prefix + "stopping_termination2");
      Place *p25 = TheNet->newPlace(prefix + "th_activity_initial");
      Place *p26 = TheNet->newPlace(prefix + "!CH_Faulted"); p26->mark();
      Place *p27 = TheNet->newPlace(prefix + "CH_Faulted");
      Place *p28 = TheNet->newPlace(prefix + "!FH_Faulted"); p28->mark();
      Place *p29 = TheNet->newPlace(prefix + "FH_Faulted");

      Transition *t1 = TheNet->newTransition(prefix + "t1");
      TheNet->newArc(t1, p3);
      TheNet->newArc(p6, t1);
      TheNet->newArc(t1, p7);

      Transition *t2 = TheNet->newTransition(prefix + "t2");
      TheNet->newArc(t2, p6);
      TheNet->newArc(p7, t2);
      TheNet->newArc(p8, t2);
      TheNet->newArc(t2, p9);

      Transition *t3 = TheNet->newTransition(prefix + "t3");
      TheNet->newArc(p6, t3, READ);
      TheNet->newArc(p8, t3);
      TheNet->newArc(t3, p12);

      Transition *t4 = TheNet->newTransition(prefix + "t4");
      TheNet->newArc(t4, p4);
      TheNet->newArc(p13, t4);
      TheNet->newArc(t4, p14);
      TheNet->newArc(p19, t4);
      TheNet->newArc(t4, p20);

      Transition *t5 = TheNet->newTransition(prefix + "t5");
      TheNet->newArc(p5, t5);
      TheNet->newArc(p14, t5);
      TheNet->newArc(t5, p15);

      Transition *t6 = TheNet->newTransition(prefix + "t6");
      TheNet->newArc(p13, t6);
      TheNet->newArc(t6, p16);
      TheNet->newArc(p20, t6, READ);

      Transition *t7 = TheNet->newTransition(prefix + "t7");
      TheNet->newArc(p11, t7);
      TheNet->newArc(t7, p16);

      Transition *t8 = TheNet->newTransition(prefix + "t8");
      TheNet->newArc(t8, p16);
      TheNet->newArc(p18, t8);

      Transition *t9 = TheNet->newTransition(prefix + "t9");
      TheNet->newArc(t9, p4);
      TheNet->newArc(p19, t9, READ);
      TheNet->newArc(p21, t9);
      TheNet->newArc(t9, p23);
      TheNet->newArc(TheNet->findPlace("1.internal.!Terminated"), t9, READ);

      Transition *t10 = TheNet->newTransition(prefix + "t10");
      TheNet->newArc(t10, p4);
      TheNet->newArc(p20, t10, READ);
      TheNet->newArc(p21, t10);
      TheNet->newArc(t10, p24);
      TheNet->newArc(TheNet->findPlace("1.internal.!Terminated"), t10, READ);

      Transition *t11 = TheNet->newTransition(prefix + "t11");
      TheNet->newArc(t11, p4);
      TheNet->newArc(p19, t11, READ);
      TheNet->newArc(p21, t11);
      TheNet->newArc(t11, p24);
      TheNet->newArc(TheNet->findPlace("1.internal.Terminated"), t11, READ);

      Transition *t12 = TheNet->newTransition(prefix + "t12");
      TheNet->newArc(p20, t12, READ);
      TheNet->newArc(p21, t11);
      TheNet->newArc(t12, p22);
      TheNet->newArc(TheNet->findPlace("1.internal.Terminated"), t12, READ);

      Transition *t13 = TheNet->newTransition(prefix + "t13");
      TheNet->newArc(p24, t13);
      TheNet->newArc(p5, t13);
      TheNet->newArc(t13, p22);

      Transition *t14 = TheNet->newTransition(prefix + "t14");
      TheNet->newArc(p5, t14);
      TheNet->newArc(p23, t14);
      TheNet->newArc(t14, p25);
    }
      a // standard elements
      c // fault handlers
      d // compesation handler
      e // termination handler
      h // inner activity
    {
      // embed fault handler
      TheNet->mergePlaces(TheNet->findPlace(prefix + "fh_fault_in"), TheNet->findPlace(prefix + "faultHandler.fault"));
      TheNet->mergePlaces(TheNet->findPlace(prefix + "final"), TheNet->findPlace(prefix + "faultHandler.handled"));
      TheNet->mergePlaces(TheNet->findPlace(prefix + "rethrow"), TheNet->findPlace(prefix + "faultHandler.!handled"));
      TheNet->mergePlaces(TheNet->findPlace(prefix + "fh_stop"), TheNet->findPlace(prefix + "faultHandler.stop"));
      TheNet->mergePlaces(TheNet->findPlace(prefix + "fh_stopped"), TheNet->findPlace(prefix + "faultHandler.stopped"));

      // embed compensation handler
      with (d)
      {
        d = userDefinedCompensationHandler(ch_activity, *):
        {
          TheNet->mergePlaces(TheNet->findPlace(prefix + "ch_activity_initial"), TheNet->findPlace(ch_activity->id, ".initial"));
          TheNet->mergePlaces(TheNet->findPlace(prefix + "scopeCompensated"), TheNet->findPlace(ch_activity->id, ".final"));
          TheNet->mergePlaces(TheNet->findPlace(prefix + "ch_activity_stop"), TheNet->findPlace(ch_activity->id, ".stop"));
          TheNet->mergePlaces(TheNet->findPlace(prefix + "ch_activity_stopped"), TheNet->findPlace(ch_activity->id, ".stopped"));
        }
        default: { /* do nothing */ }
      }

      // embed compensation handler
      with (e)
      {
        e = TerminationHandler(th_activity, *):
        {
          TheNet->mergePlaces(TheNet->findPlace(prefix + "th_activity_initial"), TheNet->findPlace(th_activity->id, ".initial"));
          TheNet->mergePlaces(TheNet->findPlace(prefix + "stopped"), TheNet->findPlace(th_activity->id, ".final"));
        }
        default: { /* do nothing */ }
      }

      // embed inner activity
      TheNet->mergePlaces(TheNet->findPlace(prefix + "initial"), TheNet->findPlace(h->id, ".initial"));
      TheNet->mergePlaces(TheNet->findPlace(prefix + "activity_final"), TheNet->findPlace(h->id, ".final"));
      TheNet->mergePlaces(TheNet->findPlace(prefix + "activity_stop"), TheNet->findPlace(h->id, ".stop"));
      TheNet->mergePlaces(TheNet->findPlace(prefix + "activity_stopped"), TheNet->findPlace(h->id, ".stopped"));

      currentScope = toString(ASTEmap[_id->value]->parentScopeId) + ".internal.";
      footer(This->id, true);
    }
  ]
;



userDefinedCompensationHandler(a, _id) ->
  [petrinetnew:
    { header(_id->value, true); }
    a
    { footer(_id->value, true); }
  ]
;


TerminationHandler(a, _id) ->
  [petrinetnew:
    { header(_id->value, true); }
    a
    { footer(_id->value, true); }
  ]
;
