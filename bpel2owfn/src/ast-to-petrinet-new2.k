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
 * \file    ast-to-petrinet-new2.cc
 *
 * \brief   Petri net unparser (implementation)
 *
 * \author  responsible: Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          last changes of: \$Author: nlohmann $
 *
 * \since   2006-10-30
 *
 * \date    \$Date: 2006/11/26 17:15:44 $
 *
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \note    This file was created using Kimwitu++ reading file
 *          bpel-unparse-petri.k.
 *          See http://site.informatik.hu-berlin.de/kimwitu++ for details.
 *
 * \version \$Revision: 1.20 $
 */
%}




/* edited */

/******************************************************************************
  COMPENSATE

  The <compensate> activity concurrently calls all compensation handlers of
  the enclosed scopes. If there are no enclosed scopes, the Petri net pattern
  can be simplified and consists of a single place both labelled with
  "initial" and "final".
******************************************************************************/

This=Compensate(a,_id) ->
  [petrinetnew:
    {
      header(This->id);
      string prefix = toString(This->id) + ".internal.";
      assert(This->id == _id->value);
      assert(ASTEmap[_id->value] != NULL);

      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "final");

      assert(ASTEmap[ASTEmap[_id->value]->parentScopeId] != NULL);
      if (!ASTEmap[ASTEmap[_id->value]->parentScopeId]->enclosedScopes.empty())
      {
        Place *p5 = TheNet->newPlace(prefix + "compensating");
        Place *p3 = TheNet->newPlace(prefix + "stop");
        Place *p4 = TheNet->newPlace(prefix + "stopped");

        Transition *t1 = TheNet->newTransition(prefix + "begin_compensation");
        TheNet->newArc(p1, t1);
        TheNet->newArc(t1, p5);

        Transition *t2 = TheNet->newTransition(prefix + "complete_compensation");
        TheNet->newArc(p5, t2);
        TheNet->newArc(t2, p2);

        Transition *t3 = TheNet->newTransition(prefix + "stop_compensation");
        TheNet->newArc(p3, t3);
        TheNet->newArc(p5, t3, READ);

        Transition *t4 = TheNet->newTransition(prefix + "compensation_stopped");
        TheNet->newArc(t4, p4);
        TheNet->newArc(p5, t4);

        // connect the transitions with all enclosed scopes
        assert(ASTEmap[ASTEmap[_id->value]->parentScopeId] != NULL);
        for (set<unsigned int>::iterator it = ASTEmap[ASTEmap[_id->value]->parentScopeId]->enclosedScopes.begin();
             it != ASTEmap[ASTEmap[_id->value]->parentScopeId]->enclosedScopes.end(); it++)
        {
          TheNet->newArc(t1, TheNet->findPlace(toString(*it) + ".internal.compensate"));
          TheNet->newArc(TheNet->findPlace(toString(*it) + ".internal.compensated"), t2);
          TheNet->newArc(t3, TheNet->findPlace(toString(*it) + ".internal.ch_stop"));
          TheNet->newArc(TheNet->findPlace(toString(*it) + ".internal.ch_stopped"), t4);
        }

        // Transitions for the negative control flow.
        Transition *t5 = stop(p1, "initial", prefix);
        Transition *t6 = stop(p2, "final", prefix);
      }
      else // no enclosed scopes
      {
        TheNet->mergePlaces(p1, p2);
      }
    }
    a // standardElements
    {
      footer(This->id);
    }
  ]
;




/* edited */

/******************************************************************************
  COMPENSATE SCOPE                                               (WS-BPEL 2.0)
******************************************************************************/

This=CompensateScope(a,b,_id) ->
  [petrinetnew:
    {
      header(This->id);
      string prefix = toString(This->id) + ".internal.";
      assert(This->id == _id->value);

      assert(ASTEmap[_id->value]->attributes["target"] != "");
      assert(scopeNames[ASTEmap[_id->value]->attributes["target"]] != 0);

      unsigned int scopeId = scopeNames[ASTEmap[_id->value]->attributes["target"]];

      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "final");
      Place *p3 = TheNet->newPlace(prefix + "stop");
      Place *p4 = TheNet->newPlace(prefix + "stopped");
      Place *p5 = TheNet->newPlace(prefix + "compensating");

      Transition *t1 = TheNet->newTransition(prefix + "begin_compensation");
      TheNet->newArc(p1, t1);
      TheNet->newArc(t1, p5);
      TheNet->newArc(t1, TheNet->findPlace(toString(scopeId) + ".internal.compensate"));

      Transition *t2 = TheNet->newTransition(prefix + "complete_compensation");
      TheNet->newArc(p5, t2);
      TheNet->newArc(t2, p2);
      TheNet->newArc(TheNet->findPlace(toString(scopeId) + ".internal.compensated"), t2);

      Transition *t3 = TheNet->newTransition(prefix + "stop_compensation");
      TheNet->newArc(p3, t3);
      TheNet->newArc(p5, t3, READ);
      TheNet->newArc(t3, TheNet->findPlace(toString(scopeId) + ".internal.ch_stop"));

      Transition *t4 = TheNet->newTransition(prefix + "compensation_stopped");
      TheNet->newArc(t4, p4);
      TheNet->newArc(p5, t4);
      TheNet->newArc(TheNet->findPlace(toString(scopeId) + ".internal.ch_stopped"), t4);

      // Transitions for the negative control flow.
      Transition *t5 = stop(p1, "initial", prefix);
      Transition *t6 = stop(p2, "final", prefix);
    }
    a // standardElements
    {
      footer(This->id);
    }
  ]
;
