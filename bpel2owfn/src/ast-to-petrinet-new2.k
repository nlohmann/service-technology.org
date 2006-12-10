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
 * \file    ast-to-petrinet-new2.cc
 *
 * \brief   Petri net unparser (implementation)
 *
 * \author  responsible: Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          last changes of: \$Author: nlohmann $
 *
 * \since   2006-10-30
 *
 * \date    \$Date: 2006/12/10 17:31:15 $
 *
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \note    This file was created using Kimwitu++ reading file
 *          bpel-unparse-petri.k.
 *          See http://site.informatik.hu-berlin.de/kimwitu++ for details.
 *
 * \version \$Revision: 1.23 $
 */
%}





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

      Place *p1 = PN.newPlace(prefix + "initial");
      Place *p2 = PN.newPlace(prefix + "final");

      assert(ASTEmap[ASTEmap[_id->value]->parentScopeId] != NULL);
      if (!ASTEmap[ASTEmap[_id->value]->parentScopeId]->enclosedScopes.empty())
      {
        Place *p5 = PN.newPlace(prefix + "compensating");
        Place *p3 = PN.newPlace(prefix + "stop");
        Place *p4 = PN.newPlace(prefix + "stopped");

        Transition *t1 = PN.newTransition(prefix + "begin_compensation");
        PN.newArc(p1, t1);
        PN.newArc(t1, p5);

        Transition *t2 = PN.newTransition(prefix + "complete_compensation");
        PN.newArc(p5, t2);
        PN.newArc(t2, p2);

        Transition *t3 = PN.newTransition(prefix + "stop_compensation");
        PN.newArc(p3, t3);
        PN.newArc(p5, t3, READ);

        Transition *t4 = PN.newTransition(prefix + "compensation_stopped");
        PN.newArc(t4, p4);
        PN.newArc(p5, t4);

        // connect the transitions with all enclosed scopes
        assert(ASTEmap[ASTEmap[_id->value]->parentScopeId] != NULL);
        for (set<unsigned int>::iterator it = ASTEmap[ASTEmap[_id->value]->parentScopeId]->enclosedScopes.begin();
             it != ASTEmap[ASTEmap[_id->value]->parentScopeId]->enclosedScopes.end(); it++)
        {
          PN.newArc(t1, PN.findPlace(toString(*it) + ".internal.compensate"));
          PN.newArc(PN.findPlace(toString(*it) + ".internal.compensated"), t2);
          PN.newArc(t3, PN.findPlace(toString(*it) + ".internal.ch_stop"));
          PN.newArc(PN.findPlace(toString(*it) + ".internal.ch_stopped"), t4);
        }

        // Transitions for the negative control flow.
        Transition *t5 = stop(p1, "initial", prefix);
        Transition *t6 = stop(p2, "final", prefix);
      }
      else // no enclosed scopes
      {
        PN.mergePlaces(p1, p2);
      }
    }
    a // standardElements
    {
      footer(This->id);
    }
  ]
;





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

      Place *p1 = PN.newPlace(prefix + "initial");
      Place *p2 = PN.newPlace(prefix + "final");
      Place *p3 = PN.newPlace(prefix + "stop");
      Place *p4 = PN.newPlace(prefix + "stopped");
      Place *p5 = PN.newPlace(prefix + "compensating");

      Transition *t1 = PN.newTransition(prefix + "begin_compensation");
      PN.newArc(p1, t1);
      PN.newArc(t1, p5);
      PN.newArc(t1, PN.findPlace(toString(scopeId) + ".internal.compensate"));

      Transition *t2 = PN.newTransition(prefix + "complete_compensation");
      PN.newArc(p5, t2);
      PN.newArc(t2, p2);
      PN.newArc(PN.findPlace(toString(scopeId) + ".internal.compensated"), t2);

      Transition *t3 = PN.newTransition(prefix + "stop_compensation");
      PN.newArc(p3, t3);
      PN.newArc(p5, t3, READ);
      PN.newArc(t3, PN.findPlace(toString(scopeId) + ".internal.ch_stop"));

      Transition *t4 = PN.newTransition(prefix + "compensation_stopped");
      PN.newArc(t4, p4);
      PN.newArc(p5, t4);
      PN.newArc(PN.findPlace(toString(scopeId) + ".internal.ch_stopped"), t4);

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
