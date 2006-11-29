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
 * \file    ast-to-petrinet-new1.cc
 *
 * \brief   Petri net unparser (implementation)
 *
 * \author  responsible: Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          last changes of: \$Author: nlohmann $
 *
 * \since   2006-10-29
 *
 * \date    \$Date: 2006/11/29 14:18:37 $
 *
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \note    This file was created using Kimwitu++ reading file
 *          bpel-unparse-petri.k.
 *          See http://site.informatik.hu-berlin.de/kimwitu++ for details.
 *
 * \version \$Revision: 1.28 $
 *
 */
%}





This=userDefinedFaultHandler(a,b, _id) ->
  [petrinetnew:
    {
      string prefix = toString(ASTEmap[_id->value]->parentScopeId) + ".internal.faultHandler.";
      header(_id, true);

      Place *p1 =  TheNet->newPlace(prefix + "initial");
      Place *p2 =  TheNet->newPlace(prefix + "final");
      Place *p3 =  TheNet->newPlace(prefix + "fault");
      Place *p4 =  TheNet->newPlace(prefix + "stop");
      Place *p5 =  TheNet->newPlace(prefix + "stopped");
      Place *p7 =  TheNet->newPlace(prefix + "running");
      Place *p8 =  TheNet->newPlace(prefix + "inner_stop");

      Transition *t4 = TheNet->newTransition(prefix + "stopped.initial");
      TheNet->newArc(p1, t4);
      TheNet->newArc(p3, t4);
      TheNet->newArc(p4, t4);
      TheNet->newArc(t4, p5);

      Transition *t5 = TheNet->newTransition(prefix + "stop_fh");
      TheNet->newArc(p4, t5);
      TheNet->newArc(p7, t5);
      TheNet->newArc(t5, p8);

      Transition *t9 = stop(p2, "final", prefix);
    }
    a // catch-branches
    b // catchAll-branch
    {
      int catchBranches = 1;
      foreach (catchBranch=Catch(innerActivity); tCatch_list a)
      {
        Transition *t1 = TheNet->newTransition(prefix + "catch" + toString(catchBranches));
        TheNet->newArc(p1, t1);
        TheNet->newArc(p3, t1);
        TheNet->newArc(t1, p7);
	TheNet->newArc(t1, TheNet->findPlace(innerActivity->id, ".initial"));

        Transition *t6 = TheNet->newTransition(prefix + "finish" + toString(catchBranches++));
	TheNet->newArc(p7, t6);
        TheNet->newArc(t6, p2);
	TheNet->newArc(TheNet->findPlace(innerActivity->id, ".final"), t6);

	TheNet->mergePlaces(TheNet->findPlace(prefix + "inner_stop"), TheNet->findPlace(innerActivity->id, ".stop"));
	TheNet->mergePlaces(TheNet->findPlace(prefix + "stopped"), TheNet->findPlace(innerActivity->id, ".stopped"));
      }

      with(b)
      {
	CatchAll(innerActivity, _id2):
	{
          Transition *t2 = TheNet->newTransition(prefix + "catchAll");
          TheNet->newArc(p1, t2);
          TheNet->newArc(p3, t2);
          TheNet->newArc(t2, p7);
          TheNet->newArc(t2, TheNet->findPlace(innerActivity->id, ".initial"));

          Transition *t7 = TheNet->newTransition(prefix + "finishCatchAll");
	  TheNet->newArc(p7, t7);
          TheNet->newArc(t7, p2);
	  TheNet->newArc(TheNet->findPlace(innerActivity->id, ".final"), t7);

	  TheNet->mergePlaces(TheNet->findPlace(prefix + "inner_stop"), TheNet->findPlace(innerActivity->id, ".stop"));
	  TheNet->mergePlaces(TheNet->findPlace(prefix + "stopped"), TheNet->findPlace(innerActivity->id, ".stopped"));
	}

        NoCatchAll():
        {
          Place *p6 = TheNet->newPlace(prefix + "faulted");
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

      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "final");
      Place *p3 = TheNet->newPlace(prefix + "stop");
      Place *p4 = TheNet->newPlace(prefix + "running");
      Place *p5 = TheNet->newPlace(prefix + "stopping");
      Place *p6 = TheNet->newPlace(prefix + "inner_initial");
      Place *p7 = TheNet->newPlace(prefix + "inner_final");
      Place *p8 = TheNet->newPlace(prefix + "inner_fault");

      Transition *t1 = TheNet->newTransition(prefix + "begin");
      TheNet->newArc(p1, t1);
      TheNet->newArc(t1, p4);
      TheNet->newArc(t1, p6);

      Transition *t2 = TheNet->newTransition(prefix + "end");
      TheNet->newArc(t2, p2);
      TheNet->newArc(p4, t2);
      TheNet->newArc(p7, t2);

      Transition *t3 = TheNet->newTransition(prefix + "abort");
      TheNet->newArc(t3, p3);
      TheNet->newArc(p4, t3);
      TheNet->newArc(t3, p5);
      TheNet->newArc(p8, t3);

      Transition *t4 = TheNet->newTransition(prefix + "ignore_fault");
      TheNet->newArc(p5, t4, READ);
      TheNet->newArc(p8, t4);

      Transition *t5 = TheNet->newTransition(prefix + "aborted");
      TheNet->newArc(t5, p2);
      TheNet->newArc(p5, t5);
      TheNet->newArc(p7, t5);
    }
    a
    {
      // embed inner activity
      TheNet->mergePlaces(TheNet->findPlace(prefix + "inner_initial"), TheNet->findPlace(a->id, ".initial"));
      TheNet->mergePlaces(TheNet->findPlace(prefix + "inner_final"),   TheNet->findPlace(a->id, ".final"));
      TheNet->mergePlaces(TheNet->findPlace(prefix + "stop"),          TheNet->findPlace(a->id, ".stop"));
      TheNet->mergePlaces(TheNet->findPlace(prefix + "inner_final"),   TheNet->findPlace(a->id, ".stopped"));
      
      footer(_id->value, true);
    }
  ]
;
