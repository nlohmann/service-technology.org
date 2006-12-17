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
 * \file    ast-to-petrinet-new1.cc
 *
 * \brief   Petri net unparser (implementation)
 *
 * \author  Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          last changes of: \$Author: nielslohmann $
 *
 * \since   2006-10-29
 *
 * \date    \$Date: 2006/12/17 11:42:13 $
 *
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \note    This file was created using Kimwitu++ reading file
 *          bpel-unparse-petri.k.
 *          See http://site.informatik.hu-berlin.de/kimwitu++ for details.
 *
 * \version \$Revision: 1.31 $
 *
 */
%}





This=userDefinedFaultHandler(a,b, _id) ->
  [petrinetnew:
    {
      string prefix = toString(ASTEmap[_id->value]->parentScopeId) + ".internal.faultHandler.";
      header(_id, true);

      Place *p1 =  PN.newPlace(prefix + "initial");
      Place *p2 =  PN.newPlace(prefix + "final");
      Place *p3 =  PN.newPlace(prefix + "fault");
      Place *p4 =  PN.newPlace(prefix + "stop");
      Place *p5 =  PN.newPlace(prefix + "stopped");
      Place *p7 =  PN.newPlace(prefix + "running");
      Place *p8 =  PN.newPlace(prefix + "inner_stop");

      Transition *t4 = PN.newTransition(prefix + "stopped.initial");
      PN.newArc(p1, t4);
      PN.newArc(p3, t4);
      PN.newArc(p4, t4);
      PN.newArc(t4, p5);

      Transition *t5 = PN.newTransition(prefix + "stop_fh");
      PN.newArc(p4, t5);
      PN.newArc(p7, t5);
      PN.newArc(t5, p8);

      Transition *t9 = stop(p2, "final", prefix);
    }
    a // catch-branches
    b // catchAll-branch
    {
      int catchBranches = 1;
      foreach (catchBranch=Catch(innerActivity); tCatch_list a)
      {
        Transition *t1 = PN.newTransition(prefix + "catch" + toString(catchBranches));
        PN.newArc(p1, t1);
        PN.newArc(p3, t1);
        PN.newArc(t1, p7);
	PN.newArc(t1, PN.findPlace(innerActivity->id, ".initial"));

        Transition *t6 = PN.newTransition(prefix + "finish" + toString(catchBranches++));
	PN.newArc(p7, t6);
        PN.newArc(t6, p2);
	PN.newArc(PN.findPlace(innerActivity->id, ".final"), t6);

	PN.mergePlaces(PN.findPlace(prefix + "inner_stop"), PN.findPlace(innerActivity->id, ".stop"));
	PN.mergePlaces(PN.findPlace(prefix + "stopped"), PN.findPlace(innerActivity->id, ".stopped"));
      }

      with(b)
      {
	CatchAll(innerActivity, _id2):
	{
          Transition *t2 = PN.newTransition(prefix + "catchAll");
          PN.newArc(p1, t2);
          PN.newArc(p3, t2);
          PN.newArc(t2, p7);
          PN.newArc(t2, PN.findPlace(innerActivity->id, ".initial"));

          Transition *t7 = PN.newTransition(prefix + "finishCatchAll");
	  PN.newArc(p7, t7);
          PN.newArc(t7, p2);
	  PN.newArc(PN.findPlace(innerActivity->id, ".final"), t7);

	  PN.mergePlaces(PN.findPlace(prefix + "inner_stop"), PN.findPlace(innerActivity->id, ".stop"));
	  PN.mergePlaces(PN.findPlace(prefix + "stopped"), PN.findPlace(innerActivity->id, ".stopped"));
	}

        NoCatchAll():
        {
          Place *p6 = PN.newPlace(prefix + "faulted");
          Transition *t1 = throwFault(p1, p6, "initial", prefix, _id, 4);
          Transition *t2 = stop(p6, "faulted", prefix);
        }

	default:
        {
          /* this should never happen */
        }
      }

      footer(_id, true);
    }
  ]
;



Catch(a, _id) ->
  [petrinetnew:
    { header(_id, true); }
    a
    { footer(_id, true); }
  ]
;



CatchAll(a, _id) ->
  [petrinetnew:
    { header(_id, true); }
    a
    { footer(_id, true); }
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
    {
      string prefix = toString(ASTEmap[_id->value]->parentScopeId) + ".internal.terminationHandler.";
      header(_id->value, true);

      Place *p1 = PN.newPlace(prefix + "initial");
      Place *p2 = PN.newPlace(prefix + "final");
      Place *p3 = PN.newPlace(prefix + "stop");
      Place *p4 = PN.newPlace(prefix + "running");
      Place *p5 = PN.newPlace(prefix + "stopping");
      Place *p6 = PN.newPlace(prefix + "inner_initial");
      Place *p7 = PN.newPlace(prefix + "inner_final");
      Place *p8 = PN.newPlace(prefix + "inner_fault");

      Transition *t1 = PN.newTransition(prefix + "begin");
      PN.newArc(p1, t1);
      PN.newArc(t1, p4);
      PN.newArc(t1, p6);

      Transition *t2 = PN.newTransition(prefix + "end");
      PN.newArc(t2, p2);
      PN.newArc(p4, t2);
      PN.newArc(p7, t2);

      Transition *t3 = PN.newTransition(prefix + "abort");
      PN.newArc(t3, p3);
      PN.newArc(p4, t3);
      PN.newArc(t3, p5);
      PN.newArc(p8, t3);

      Transition *t4 = PN.newTransition(prefix + "ignore_fault");
      PN.newArc(p5, t4, READ);
      PN.newArc(p8, t4);

      Transition *t5 = PN.newTransition(prefix + "aborted");
      PN.newArc(t5, p2);
      PN.newArc(p5, t5);
      PN.newArc(p7, t5);
    }
    a
    {
      // embed inner activity
      PN.mergePlaces(PN.findPlace(prefix + "inner_initial"), PN.findPlace(a->id, ".initial"));
      PN.mergePlaces(PN.findPlace(prefix + "inner_final"),   PN.findPlace(a->id, ".final"));
      PN.mergePlaces(PN.findPlace(prefix + "stop"),          PN.findPlace(a->id, ".stop"));
      PN.mergePlaces(PN.findPlace(prefix + "inner_final"),   PN.findPlace(a->id, ".stopped"));
      
      footer(_id->value, true);
    }
  ]
;
