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
 *          - last changed: \$Date: 2006/11/05 13:16:47 $
 *
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \note    This file was created using Kimwitu++ reading file
 *          bpel-unparse-petri.k.
 *          See http://site.informatik.hu-berlin.de/kimwitu++ for details.
 *
 * \version \$Revision: 1.12 $
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
      for (set<string>::iterator link = linkNames.begin(); link != linkNames.end(); link++)
      {
	TheNet->newPlace("link." + *link);
	TheNet->newPlace("!link." + *link);
      }


      Place *p1  = TheNet->newPlace(prefix + "initial"); p1->mark();
      Place *p2  = TheNet->newPlace(prefix + "final");
      Place *p3  = TheNet->newPlace(prefix + "Active");
      Place *p4  = TheNet->newPlace(prefix + "!Active"); p4->mark();
      Place *p5  = TheNet->newPlace(prefix + "Faulted");
      Place *p6  = TheNet->newPlace(prefix + "!Faulted"); p6->mark();
      Place *p7  = TheNet->newPlace(prefix + "Exiting");
      Place *p8  = TheNet->newPlace(prefix + "!Exiting"); p8->mark();
      Place *p9  = TheNet->newPlace(prefix + "activity_initial");
      Place *p10 = TheNet->newPlace(prefix + "stop");
      Place *p11 = TheNet->newPlace(prefix + "activity_stopped");
      Place *p12 = TheNet->newPlace(prefix + "activity_final");
      Place *p13 = TheNet->newPlace(prefix + "fh_initial");

      Transition *t1 = TheNet->newTransition(prefix + "initialize");
      TheNet->newArc(p1, t1);
      TheNet->newArc(t1, p3);
      TheNet->newArc(p4, t1);
      TheNet->newArc(t1, p9);

      Transition *t2 = TheNet->newTransition(prefix + "finalize");
      TheNet->newArc(t2, p2);
      TheNet->newArc(p3, t2);
      TheNet->newArc(t2, p4);
      TheNet->newArc(p12, t2);

      Transition *t3 = TheNet->newTransition(prefix + "start_fh");
      TheNet->newArc(p4, t3, READ);
      TheNet->newArc(p5, t3);
      TheNet->newArc(t3, p6);
      TheNet->newArc(p11, t3);
      TheNet->newArc(t3, p13);

      Transition *t4 = TheNet->newTransition(prefix + "exited");
      TheNet->newArc(t4, p2);
      TheNet->newArc(p6, t4, READ);
      TheNet->newArc(p11, t4);
    }
    i // innerActivity
    e // faultHandler
    {
      // embed fault handler
      TheNet->mergePlaces(TheNet->findPlace(prefix + "fh_initial"), TheNet->findPlace(prefix + "faultHandler.initial"));
      TheNet->mergePlaces(TheNet->findPlace(prefix + "final"), TheNet->findPlace(prefix + "faultHandler.final"));
      TheNet->mergePlaces(TheNet->findPlace(prefix + "stop"), TheNet->findPlace(prefix + "faultHandler.stop"));
      TheNet->mergePlaces(TheNet->findPlace(prefix + "final"), TheNet->findPlace(prefix + "faultHandler.stopped"));

      // embed inner activity
      TheNet->mergePlaces(TheNet->findPlace(prefix + "activity_initial"), TheNet->findPlace(i->id, ".initial"));
      TheNet->mergePlaces(TheNet->findPlace(prefix + "activity_final"), TheNet->findPlace(i->id, ".final"));
      TheNet->mergePlaces(TheNet->findPlace(prefix + "stop"), TheNet->findPlace(i->id, ".stop"));
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

      Place *p1 =  TheNet->newPlace(prefix + "initial");
      Place *p2 =  TheNet->newPlace(prefix + "final");
      Place *p3 =  TheNet->newPlace(prefix + "stop");
      Place *p4 =  TheNet->newPlace(prefix + "stopped");
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
	TheNet->mergePlaces(TheNet->findPlace(prefix + "final"), TheNet->findPlace(innerActivity, ".final"));
	TheNet->mergePlaces(TheNet->findPlace(prefix + "stop"), TheNet->findPlace(innerActivity, ".stop"));
	TheNet->mergePlaces(TheNet->findPlace(prefix + "stopped"), TheNet->findPlace(innerActivity, ".stopped"));
      }

      with(b) {
	CatchAll(innerActivity, _id):
	{
	  Transition *t2 = TheNet->newTransition(prefix + "catchAll");
	  TheNet->newArc(p1, t2);
	  TheNet->newArc(t2, TheNet->findPlace(innerActivity, ".initial"));

	  TheNet->mergePlaces(TheNet->findPlace(prefix + "final"), TheNet->findPlace(innerActivity, ".final"));
	  TheNet->mergePlaces(TheNet->findPlace(prefix + "stop"), TheNet->findPlace(innerActivity, ".stop"));
	  TheNet->mergePlaces(TheNet->findPlace(prefix + "stopped"), TheNet->findPlace(innerActivity, ".stopped"));
	}
        NoCatchAll():
        {
          Place *p5 = TheNet->newPlace(prefix + "faulted");
          Transition *t1 = throwFault(p1, p5, "initial", prefix, _id, 4);
          Transition *t2 = stop(p5, "faulted", prefix);
        }
	default: { /* this should never happen */ }
      }

      trace(TRACE_DEBUG, "[PNU]" + inString() + "</userDefinedFaultHandler scope=\"" + currentScope + "\">\n");
    }
  ]
;






This=Scope(a,b,c,d,e,f,g,h,i,_id) ->
  [petrinetnew:
    {
      header(This->id, true);
      string prefix = toString(This->id) + ".internal.";
      assert(This->id == _id->value);

      currentScope = prefix;

      Place *p1  = TheNet->newPlace(prefix + "initial");
      Place *p2  = TheNet->newPlace(prefix + "final");
      Place *p3  = TheNet->newPlace(prefix + "compensate");
      Place *p4  = TheNet->newPlace(prefix + "compensated");
      Place *p5  = TheNet->newPlace(prefix + "stop");
      Place *p6  = TheNet->newPlace(prefix + "stopped");
      Place *p7  = TheNet->newPlace(prefix + "Active");
      Place *p8  = TheNet->newPlace(prefix + "!Active"); p8->mark();
      Place *p9  = TheNet->newPlace(prefix + "Successful");
      Place *p10 = TheNet->newPlace(prefix + "!Successful"); p10->mark();
      Place *p11 = TheNet->newPlace(prefix + "Faulted");
      Place *p12 = TheNet->newPlace(prefix + "!Faulted"); p12->mark();
      Place *p13 = TheNet->newPlace(prefix + "inner_stopped");
      Place *p14 = TheNet->newPlace(prefix + "activity_initial");
      Place *p15 = TheNet->newPlace(prefix + "activity_final");
      Place *p16 = TheNet->newPlace(prefix + "fh_initial");
      Place *p17 = TheNet->newPlace(prefix + "ch_initial");
      Place *p18 = TheNet->newPlace(prefix + "th_initial");

      Transition *t1 = TheNet->newTransition(prefix + "initialize");
      TheNet->newArc(p1, t1);
      TheNet->newArc(t1, p7);
      TheNet->newArc(p8, t1);
      TheNet->newArc(t1, p14);

      Transition *t2 = TheNet->newTransition(prefix + "finalize");
      TheNet->newArc(t2, p2);
      TheNet->newArc(p7, t2);
      TheNet->newArc(t2, p8);
      TheNet->newArc(t2, p9);
      TheNet->newArc(p10, t2);
      TheNet->newArc(p15, t2);

      Transition *t3 = TheNet->newTransition(prefix + "start_fh");
      TheNet->newArc(p8, t3, READ);
      TheNet->newArc(p11, t3);
      TheNet->newArc(t3, p12);
      TheNet->newArc(p13, t3);
      TheNet->newArc(t3, p16);

      Transition *t4 = TheNet->newTransition(prefix + "start_ch");
      TheNet->newArc(p3, t4);
      TheNet->newArc(p9, t4);
      TheNet->newArc(t4, p10);
      TheNet->newArc(t4, p17);

      Transition *t5 = TheNet->newTransition(prefix + "skip_ch");
      TheNet->newArc(p3, t5);
      TheNet->newArc(t5, p4);
      TheNet->newArc(p10, t5, READ);

      Transition *t6 = TheNet->newTransition(prefix + "start_th");
      TheNet->newArc(p7, t6);
      TheNet->newArc(t6, p8);
      TheNet->newArc(p13, t6);
      TheNet->newArc(t6, p18);
      TheNet->newArc(TheNet->findPlace("1.internal.!Exiting"), t6, READ);

      Transition *t7 = TheNet->newTransition(prefix + "skip_th");
      TheNet->newArc(t7, p6);
      TheNet->newArc(p8, t7, READ);
      TheNet->newArc(p12, t7, READ);
      TheNet->newArc(p13, t7);

      Transition *t8 = TheNet->newTransition(prefix + "exit");
      TheNet->newArc(t8, p6);
      TheNet->newArc(p13, t8);
      TheNet->newArc(TheNet->findPlace("1.internal.Exiting"), t8, READ);

      Transition *t9 = stop(p1, "initial", prefix);
      Transition *t10 = stop(p2, "final", prefix);
    }
      a // standard elements
      i // inner activity
      c // fault handlers
      d // compesation handler
      e // termination handler
    {
      // embed fault handler
      TheNet->mergePlaces(TheNet->findPlace(prefix + "fh_initial"), TheNet->findPlace(prefix + "faultHandler.initial"));
      TheNet->mergePlaces(TheNet->findPlace(prefix + "final"), TheNet->findPlace(prefix + "faultHandler.final"));
      TheNet->mergePlaces(TheNet->findPlace(prefix + "stop"), TheNet->findPlace(prefix + "faultHandler.stop"));
      TheNet->mergePlaces(TheNet->findPlace(prefix + "stopped"), TheNet->findPlace(prefix + "faultHandler.stopped"));

      // embed compensation handler
      with (d)
      {
        d = userDefinedCompensationHandler(ch_activity, *):
        {
          TheNet->mergePlaces(TheNet->findPlace(prefix + "ch_initial"), TheNet->findPlace(ch_activity->id, ".initial"));
          TheNet->mergePlaces(TheNet->findPlace(prefix + "compensated"), TheNet->findPlace(ch_activity->id, ".final"));
          TheNet->mergePlaces(TheNet->findPlace(prefix + "stop"), TheNet->findPlace(ch_activity->id, ".stop"));
          TheNet->mergePlaces(TheNet->findPlace(prefix + "stopped"), TheNet->findPlace(ch_activity->id, ".stopped"));
        }
        default: { assert(false); }
      }

      // embed compensation handler
      with (e)
      {
        e = TerminationHandler(th_activity, *):
        {
          TheNet->mergePlaces(TheNet->findPlace(prefix + "th_initial"), TheNet->findPlace(th_activity->id, ".initial"));
          TheNet->mergePlaces(TheNet->findPlace(prefix + "stopped"), TheNet->findPlace(th_activity->id, ".final"));
          TheNet->mergePlaces(TheNet->findPlace(prefix + "stop"), TheNet->findPlace(th_activity->id, ".stop"));
          TheNet->mergePlaces(TheNet->findPlace(prefix + "stopped"), TheNet->findPlace(th_activity->id, ".stopped"));
        }
        default: { /* do nothing */ }
      }

      // embed inner activity
      TheNet->mergePlaces(TheNet->findPlace(prefix + "activity_initial"), TheNet->findPlace(h->id, ".initial"));
      TheNet->mergePlaces(TheNet->findPlace(prefix + "activity_final"), TheNet->findPlace(h->id, ".final"));
      TheNet->mergePlaces(TheNet->findPlace(prefix + "stop"), TheNet->findPlace(h->id, ".stop"));
      TheNet->mergePlaces(TheNet->findPlace(prefix + "inner_stopped"), TheNet->findPlace(h->id, ".stopped"));

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
