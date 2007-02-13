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
 * \author  Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          last changes of: \$Author: gierds $
 *
 * \since   2006-10-30
 *
 * \date    \$Date: 2007/02/13 14:41:08 $
 *
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \note    This file was created using Kimwitu++ reading file
 *          bpel-unparse-petri.k.
 *          See http://site.informatik.hu-berlin.de/kimwitu++ for details.
 *
 * \version \$Revision: 1.26 $
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
      assert(ASTE_scopeNames[ASTEmap[_id->value]->attributes["target"]] != 0);

      unsigned int scopeId = ASTE_scopeNames[ASTEmap[_id->value]->attributes["target"]];

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


/******************************************************************************
  STANDARD ELEMENTS
******************************************************************************/

This=StandardElements(NiltTarget_list(), NiltSource_list(), jc) ->
  [petrinetnew:
    {
      string prefix = toString(This->parentId) + ".";
      string newPrefix = toString(This->parentId) + ".internal";

      Place *p2 = PN.newPlace(prefix + "initial");
      Place *p3 = PN.newPlace(prefix + "final");
      Place *p4 = PN.newPlace(prefix + "stop");
      Place *p5 = PN.newPlace(prefix + "stopped");

      // Embed the inner activity.
      PN.mergePlaces(p2, PN.findPlace(newPrefix + ".initial"));
      PN.mergePlaces(p3, PN.findPlace(newPrefix + ".final"));
      PN.mergePlaces(p4, PN.findPlace(newPrefix + ".stop"));
      PN.mergePlaces(p5, PN.findPlace(newPrefix + ".stopped"));
    }
  ]
;





/******************************************************************************
 * Figure 22: Pattern for an activity that is source of links.
 *****************************************************************************/

This=StandardElements(NiltTarget_list(), a, jc) ->
  [petrinetnew:
    {
      trace(TRACE_DEBUG, "[PNU]" + inString() + "Fig. 22 (new)\n");
      string prefix = toString(This->parentId) + ".";
      string newPrefix = toString(This->parentId) + ".internal";

      Place *p1 = PN.newPlace(prefix + "initial");
      Place *p2 = PN.newPlace(prefix + "done");
      Place *p3 = PN.newPlace(prefix + "final");
      Place *p6 = PN.newPlace(prefix + "stop");
      Place *p7 = PN.newPlace(prefix + "stopped");
      PN.mergePlaces(p6, PN.findPlace(newPrefix + ".stop"));
      PN.mergePlaces(p7, PN.findPlace(newPrefix + ".stopped"));

      Transition *tf = stop(p3, "final", prefix);

      unsigned int linkCounter = 1;
      if (parameters[P_XOR] && (a->length() > 1))
      {
	// Set the outgoing links (XOR given).
	foreach (source; tSource_list a)
	{
  	  Transition *t1 = PN.newTransition(prefix + "setLinks" + toString(linkCounter++));
    	  PN.newArc(p2, t1);
      	  PN.newArc(t1, p3);

          Place *p4 = PN.findPlace("link." + ASTEmap[source->id]->linkName);
	  PN.newArc(t1, p4);

          foreach (source2; tSource_list a)
          {
            if (ASTEmap[source2->id]->linkName != ASTEmap[source->id]->linkName)
            {
              Place *p5 = PN.findPlace("!link." + ASTEmap[source2->id]->linkName);
	      PN.newArc(t1, p5);
            }
          }

          string givenTransitionCondition = ASTEmap[source->id]->attributes["transitionCondition"];
          if (givenTransitionCondition != "" && givenTransitionCondition != "true")
          {
            cerr << "Ignoring user-defined transition condition \"" << givenTransitionCondition << "\"" << endl;
            cerr << "Using XOR instead." << endl << endl;
          }
        }
      }
      else
      {
  	Transition *t1 = PN.newTransition(prefix + "setLinks");
    	PN.newArc(p2, t1);
      	PN.newArc(t1, p3);

	// Set the outgoing links (standard transition condition expected).
	foreach (source; tSource_list a)
	{
          Place *p4 = PN.findPlace("link." + ASTEmap[source->id]->linkName);
	  PN.newArc(t1, p4);

          string givenTransitionCondition = ASTEmap[source->id]->attributes["transitionCondition"];
          if (givenTransitionCondition != "" && givenTransitionCondition != "true")
          {
            cerr << "Ignoring user-defined transition condition \"" << givenTransitionCondition << "\"" << endl;
            cerr << "Using standard transition \"true\" instead." << endl << endl;
          }
        }
      }




      // Embed the inner activity.
      PN.mergePlaces(p1, PN.findPlace(newPrefix + ".initial"));
      PN.mergePlaces(p2, PN.findPlace(newPrefix + ".final"));
    }
  ]
;





/******************************************************************************
 * Figure 23: Pattern for an activity that is target of links in case of
 * suppressJoinFailure="no".
 *****************************************************************************/

This=StandardElements(a, NiltSource_list(), jc) provided (!This->suppressJF)
 ->
  [petrinetnew:
    {
      trace(TRACE_DEBUG, "[PNU]" + inString() + "Fig. 23 (new)\n");
      string prefix = toString(This->parentId) + ".";
      string newPrefix = toString(This->parentId) + ".internal";

      Place *p1 = PN.newPlace(prefix + "initial");
      Place *p2 = PN.newPlace(prefix + "final");
      Place *p3 = PN.newPlace(prefix + "joinTrue");
      Place *p4 = PN.newPlace(prefix + "joinFalse");
      Place *p5 = PN.newPlace(prefix + "begin");
      Place *p6 = PN.newPlace(prefix + "stop");
      Place *p7 = PN.newPlace(prefix + "stopped");
      Place *p8 = PN.newPlace(prefix + "faulted");

      PN.mergePlaces(p6, PN.findPlace(newPrefix + ".stop"));
      PN.mergePlaces(p7, PN.findPlace(newPrefix + ".stopped"));

      Transition *t1 = PN.newTransition(prefix + "start");
      PN.newArc(p1, t1);
      PN.newArc(p3, t1);
      PN.newArc(t1, p5);

      Transition *t2 = PN.newTransition(prefix + "skip");
      PN.newArc(p1, t2);
      PN.newArc(p4, t2);
      PN.newArc(t2, p2);
      PN.newArc(t2, p8);

      Transition *t3 = stop(p1, "initial", prefix);
      Transition *t4 = stop(p3, "joinTrue", prefix);
      Transition *t5 = stop(p4, "joinFalse", prefix);
      Transition *t6 = stop(p8, "faulted", prefix);

      // sets enclosed source links to false
      dpeLinks(t2, parentId);



      /************************************************************************
       * pattern for the join condition
       ***********************************************************************/

      // containers for the join transitions and the incoming links
      map<unsigned int, Transition*> joinTransitions;
      map<unsigned int, Place*> trueLinks;
      map<unsigned int, Place*> falseLinks;
      unsigned int transitionNumber = (unsigned int)pow(2.0, a->length());

      // fill link containers
      unsigned int linkCount = 0;
      foreach (target; tTarget_list a)
      {
        Place *linkT = PN.findPlace("link." + ASTEmap[target->id]->linkName);
	Place *linkF = PN.findPlace("!link." + ASTEmap[target->id]->linkName);

        trueLinks[linkCount] = linkT;
        falseLinks[linkCount] = linkF;

        linkCount++;
      }

      // add transitions and connect them with the links
      for (unsigned int i = 0; i < transitionNumber; i++)
      {
        joinTransitions[i] = PN.newTransition(prefix + "join" + toString(i));

        for (unsigned int j = 0; j < linkCount; j++)
        {
          if (((i >> j) % 2) == 0) // don't ask me why this is working :-|
            PN.newArc(trueLinks[j], joinTransitions[i]);
          else
            PN.newArc(falseLinks[j], joinTransitions[i]);
        }
      }

      // case 1: standard join condition
      if (string(jc->op_name()) == "standardJoinCondition")
      {
        // create OR-join
        for (unsigned int i = 0; i < transitionNumber; i++)
        {
          // only one transition makes the join fail
          if (i == transitionNumber-1)
            PN.newArc(joinTransitions[i], p4); // p4=joinFalse
          else
            PN.newArc(joinTransitions[i], p3); // p3=joinTrue
        }
      }
      else // case 2: user-defined join condition
      {
        // create pattern for join condition
        for (unsigned int i = 0; i < transitionNumber; i++)
        {
          if (joinCondition_result[i])
            PN.newArc(joinTransitions[i], p3);
          else
            PN.newArc(joinTransitions[i], p4);
        }
      }



      // Embed the inner activity.
      PN.mergePlaces(p5, PN.findPlace(newPrefix + ".initial"));
      PN.mergePlaces(p2, PN.findPlace(newPrefix + ".final"));
    }
  ]
;





/******************************************************************************
 * Figure 24: Pattern for an activity that is target of links in case of
 * suppressJoinFailure="yes".
 *****************************************************************************/

This=StandardElements(a, NiltSource_list(), jc) provided (This->suppressJF)
 ->
  [petrinetnew:
    {
      trace(TRACE_DEBUG, "[PNU]" + inString() + "Fig. 24 (new)\n");
      string prefix = toString(This->parentId) + ".";
      string newPrefix = toString(This->parentId) + ".internal";

      Place *p1 = PN.newPlace(prefix + "initial");
      Place *p2 = PN.newPlace(prefix + "final");
      Place *p3 = PN.newPlace(prefix + "joinTrue");
      Place *p4 = PN.newPlace(prefix + "joinFalse");
      Place *p5 = PN.newPlace(prefix + "begin");
      Place *p6 = PN.newPlace(prefix + "stop");
      Place *p7 = PN.newPlace(prefix + "stopped");
      PN.mergePlaces(p6, PN.findPlace(newPrefix + ".stop"));
      PN.mergePlaces(p7, PN.findPlace(newPrefix + ".stopped"));

      Transition *t1 = PN.newTransition(prefix + "start");
      PN.newArc(p1, t1);
      PN.newArc(p3, t1);
      PN.newArc(t1, p5);

      Transition *t2 = PN.newTransition(prefix + "skip");
      PN.newArc(p1, t2);
      PN.newArc(p4, t2);
      PN.newArc(t2, p2);

      // sets enclosed source links to false
      dpeLinks(t2, parentId);



      /************************************************************************
       * pattern for the join condition
       ***********************************************************************/

      // containers for the join transitions and the incoming links
      map<unsigned int, Transition*> joinTransitions;
      map<unsigned int, Place*> trueLinks;
      map<unsigned int, Place*> falseLinks;
      unsigned int transitionNumber = (unsigned int)pow(2.0, a->length());

      // fill link containers
      unsigned int linkCount = 0;
      foreach (target; tTarget_list a)
      {
        Place *linkT = PN.findPlace("link." + ASTEmap[target->id]->linkName);
	Place *linkF = PN.findPlace("!link." + ASTEmap[target->id]->linkName);

        trueLinks[linkCount] = linkT;
        falseLinks[linkCount] = linkF;

        linkCount++;
      }

      // add transitions and connect them with the links
      for (unsigned int i = 0; i < transitionNumber; i++)
      {
        joinTransitions[i] = PN.newTransition(prefix + "join" + toString(i));

        for (unsigned int j = 0; j < linkCount; j++)
        {
          if (((i >> j) % 2) == 0) // don't ask me why this is working :-|
            PN.newArc(trueLinks[j], joinTransitions[i]);
          else
            PN.newArc(falseLinks[j], joinTransitions[i]);
        }
      }

      // case 1: standard join condition
      if (string(jc->op_name()) == "standardJoinCondition")
      {
        // create OR-join
        for (unsigned int i = 0; i < transitionNumber; i++)
        {
          // only one transition makes the join fail
          if (i == transitionNumber-1)
            PN.newArc(joinTransitions[i], p4); // p4=joinFalse
          else
            PN.newArc(joinTransitions[i], p3); // p3=joinTrue
        }
      }
      else // case 2: user-defined join condition
      {
        // create pattern for join condition
        for (unsigned int i = 0; i < transitionNumber; i++)
        {
          if (joinCondition_result[i])
            PN.newArc(joinTransitions[i], p3);
          else
            PN.newArc(joinTransitions[i], p4);
        }
      }



      // Embed the inner activity.
      PN.mergePlaces(p5, PN.findPlace(newPrefix + ".initial"));
      PN.mergePlaces(p2, PN.findPlace(newPrefix + ".final"));
    }
  ]
;






/******************************************************************************
 * Figure 25: Pattern for an activity that is source and target of links in
 * case of suppressJoinFailure="yes".
 *****************************************************************************/

This=StandardElements(a,b, jc) provided (This->suppressJF) ->
  [petrinetnew:
    {
      trace(TRACE_DEBUG, "[PNU]" + inString() + "Fig. 25 (new)\n");
      string prefix = toString(This->parentId) + ".";
      string newPrefix = toString(This->parentId) + ".internal";

      Place *p1 = PN.newPlace(prefix + "initial");
      Place *p2 = PN.newPlace(prefix + "begin");
      Place *p3 = PN.newPlace(prefix + "done");
      Place *p4 = PN.newPlace(prefix + "final");
      Place *p5 = PN.newPlace(prefix + "joinFalse");
      Place *p6 = PN.newPlace(prefix + "joinTrue");
      Place *p7 = PN.newPlace(prefix + "stop");
      Place *p8 = PN.newPlace(prefix + "stopped");
      PN.mergePlaces(p7, PN.findPlace(newPrefix + ".stop"));
      PN.mergePlaces(p8, PN.findPlace(newPrefix + ".stopped"));

      Transition *t1 = PN.newTransition(prefix + "start");
      PN.newArc(p1, t1);
      PN.newArc(p6, t1);
      PN.newArc(t1, p2);

      Transition *t2 = PN.newTransition(prefix + "skip");
      PN.newArc(p1, t2);
      PN.newArc(p5, t2);
      PN.newArc(t2, p4);

      // sets enclosed source links to false
      dpeLinks(t2, This->parentId);

      unsigned int linkCounter = 1;
      if (parameters[P_XOR] && (b->length() > 1))
      {
	// Set the outgoing links (XOR given).
	foreach (source; tSource_list b)
	{
  	  Transition *t3 = PN.newTransition(prefix + "setLinks" + toString(linkCounter++));
    	  PN.newArc(p3, t3);
      	  PN.newArc(t3, p4);

          Place *p7 = PN.findPlace("link." + ASTEmap[source->id]->linkName);
	  PN.newArc(t3, p7);

          foreach (source2; tSource_list b)
          {
            if (source2 != source)
            {
              Place *p8 = PN.findPlace("!link." + ASTEmap[source2->id]->linkName);
	      PN.newArc(t3, p8);
            }
          }

          string givenTransitionCondition = ASTEmap[source->id]->attributes["transitionCondition"];
          if (givenTransitionCondition != "" && givenTransitionCondition != "true")
          {
            cerr << "Ignoring user-defined transition condition \"" << givenTransitionCondition << "\"" << endl;
            cerr << "Using XOR instead." << endl << endl;
          }
        }
      }
      else
      {
  	Transition *t3 = PN.newTransition(prefix + "setLinks");
    	PN.newArc(p3, t3);
      	PN.newArc(t3, p4);
      
	// Set the outgoing links (standard transition condition expected).
	foreach (source; tSource_list b)
        {
	  Place *p7 = PN.findPlace("link." + ASTEmap[source->id]->linkName);
          PN.newArc(t3, p7);

          string givenTransitionCondition = ASTEmap[source->id]->attributes["transitionCondition"];
          if (givenTransitionCondition != "" && givenTransitionCondition != "true")
          {
            cerr << "Ignoring user-defined transition condition \"" << givenTransitionCondition << "\"" << endl;
            cerr << "Using standard transition \"true\" instead." << endl << endl;
          }
        }
      }





      /************************************************************************
       * pattern for the join condition
       ***********************************************************************/

      // containers for the join transitions and the incoming links
      map<unsigned int, Transition*> joinTransitions;
      map<unsigned int, Place*> trueLinks;
      map<unsigned int, Place*> falseLinks;
      unsigned int transitionNumber = (unsigned int)pow(2.0, a->length());

      // fill link containers
      unsigned int linkCount = 0;
      foreach (target; tTarget_list a)
      {
        Place *linkT = PN.findPlace("link." + ASTEmap[target->id]->linkName);
	Place *linkF = PN.findPlace("!link." + ASTEmap[target->id]->linkName);

        trueLinks[linkCount] = linkT;
        falseLinks[linkCount] = linkF;

        linkCount++;
      }

      // add transitions and connect them with the links
      for (unsigned int i = 0; i < transitionNumber; i++)
      {
        joinTransitions[i] = PN.newTransition(prefix + "join" + toString(i));

        for (unsigned int j = 0; j < linkCount; j++)
        {
          if (((i >> j) % 2) == 0) // don't ask me why this is working :-|
            PN.newArc(trueLinks[j], joinTransitions[i]);
          else
            PN.newArc(falseLinks[j], joinTransitions[i]);
        }
      }

      // case 1: standard join condition
      if (string(jc->op_name()) == "standardJoinCondition")
      {
        // create OR-join
        for (unsigned int i = 0; i < transitionNumber; i++)
        {
          // only one transition makes the join fail
          if (i == transitionNumber-1)
            PN.newArc(joinTransitions[i], p5); // p5=joinFalse
          else
            PN.newArc(joinTransitions[i], p6); // p6=joinTrue
        }
      }
      else // case 2: user-defined join condition
      {
        // create pattern for join condition
        for (unsigned int i = 0; i < transitionNumber; i++)
        {
          if (joinCondition_result[i])
            PN.newArc(joinTransitions[i], p5);
          else
            PN.newArc(joinTransitions[i], p6);
        }
      }




      // Embed the inner activity.
      PN.mergePlaces(p2, PN.findPlace(newPrefix + ".initial"));
      PN.mergePlaces(p3, PN.findPlace(newPrefix + ".final"));
    }
  ]
;





/******************************************************************************
 * Figure 26: Pattern for an activity that is source and target of links in
 * case of suppressJoinFailure="no".
 *****************************************************************************/

This=StandardElements(a,b, jc) provided (!This->suppressJF) 
 ->
  [petrinetnew:
    {
      trace(TRACE_DEBUG, "[PNU]" + inString() + "Fig. 26 (new)\n");
      string prefix = toString(This->parentId) + ".";
      string newPrefix = toString(This->parentId) + ".internal";

      Place *p1 = PN.newPlace(prefix + "initial");
      Place *p2 = PN.newPlace(prefix + "begin");
      Place *p3 = PN.newPlace(prefix + "done");
      Place *p4 = PN.newPlace(prefix + "final");
      Place *p5 = PN.newPlace(prefix + "joinFalse");
      Place *p6 = PN.newPlace(prefix + "joinTrue");
      Place *p7 = PN.newPlace(prefix + "stop");
      Place *p8 = PN.newPlace(prefix + "stopped");
      Place *p9 = PN.newPlace(prefix + "faulted");

      PN.mergePlaces(p7, PN.findPlace(newPrefix + ".stop"));
      PN.mergePlaces(p8, PN.findPlace(newPrefix + ".stopped"));

      Transition *t1 = PN.newTransition(prefix + "start");
      PN.newArc(p1, t1);
      PN.newArc(p6, t1);
      PN.newArc(t1, p2);

      Transition *t2 = PN.newTransition(prefix + "skip");
      PN.newArc(p1, t2);
      PN.newArc(p5, t2);
      PN.newArc(t2, p4);
      PN.newArc(t2, p9);

      Transition *t6 = stop(p1, "initial", prefix);
      Transition *t7 = stop(p4, "final", prefix);
      Transition *t8 = stop(p5, "joinTrue", prefix);
      Transition *t9 = stop(p6, "joinFalse", prefix);
      Transition *t10 = stop(p9, "faulted", prefix);


      // sets enclosed source links to false
      dpeLinks(t2, This->parentId);

      unsigned int linkCounter = 1;
      if (parameters[P_XOR] && (b->length() > 1))
      {
	// Set the outgoing links (XOR given).
	foreach (source; tSource_list b)
	{
  	  Transition *t3 = PN.newTransition(prefix + "setLinks" + toString(linkCounter++));
    	  PN.newArc(p3, t3);
      	  PN.newArc(t3, p4);

          Place *p7 = PN.findPlace("link." + ASTEmap[source->id]->linkName);
	  PN.newArc(t3, p7);

          foreach (source2; tSource_list b)
          {
            if (source2 != source)
            {
              Place *p8 = PN.findPlace("!link." + ASTEmap[source2->id]->linkName);
	      PN.newArc(t3, p8);
            }
          }

          string givenTransitionCondition = ASTEmap[source->id]->attributes["transitionCondition"];
          if (givenTransitionCondition != "" && givenTransitionCondition != "true")
          {
            cerr << "Ignoring user-defined transition condition \"" << givenTransitionCondition << "\"" << endl;
            cerr << "Using XOR instead." << endl << endl;
          }
        }
      }
      else
      {
  	Transition *t3 = PN.newTransition(prefix + "setLinks");
    	PN.newArc(p3, t3);
      	PN.newArc(t3, p4);
      
	// Set the outgoing links (standard transition condition expected).
	foreach (source; tSource_list b)
        {
	  Place *p7 = PN.findPlace("link." + ASTEmap[source->id]->linkName);
          PN.newArc(t3, p7);

          string givenTransitionCondition = ASTEmap[source->id]->attributes["transitionCondition"];
          if (givenTransitionCondition != "" && givenTransitionCondition != "true")
          {
            cerr << "Ignoring user-defined transition condition \"" << givenTransitionCondition << "\"" << endl;
            cerr << "Using standard transition \"true\" instead." << endl << endl;
          }
        }
      }





      /************************************************************************
       * pattern for the join condition
       ***********************************************************************/

      // containers for the join transitions and the incoming links
      map<unsigned int, Transition*> joinTransitions;
      map<unsigned int, Place*> trueLinks;
      map<unsigned int, Place*> falseLinks;
      unsigned int transitionNumber = (unsigned int)pow(2.0, a->length());

      // fill link containers
      unsigned int linkCount = 0;
      foreach (target; tTarget_list a)
      {
        Place *linkT = PN.findPlace("link." + ASTEmap[target->id]->linkName);
	Place *linkF = PN.findPlace("!link." + ASTEmap[target->id]->linkName);

        trueLinks[linkCount] = linkT;
        falseLinks[linkCount] = linkF;

        linkCount++;
      }

      // add transitions and connect them with the links
      for (unsigned int i = 0; i < transitionNumber; i++)
      {
        joinTransitions[i] = PN.newTransition(prefix + "join" + toString(i));

        for (unsigned int j = 0; j < linkCount; j++)
        {
          if (((i >> j) % 2) == 0) // don't ask me why this is working :-|
            PN.newArc(trueLinks[j], joinTransitions[i]);
          else
            PN.newArc(falseLinks[j], joinTransitions[i]);
        }
      }

      // case 1: standard join condition
      if (string(jc->op_name()) == "standardJoinCondition")
      {
        // create OR-join
        for (unsigned int i = 0; i < transitionNumber; i++)
        {
          // only one transition makes the join fail
          if (i == transitionNumber-1)
            PN.newArc(joinTransitions[i], p5); // p5=joinFalse
          else
            PN.newArc(joinTransitions[i], p6); // p6=joinTrue
        }
      }
      else // case 2: user-defined join condition
      {
        // create pattern for join condition
        for (unsigned int i = 0; i < transitionNumber; i++)
        {
          if (joinCondition_result[i])
            PN.newArc(joinTransitions[i], p5);
          else
            PN.newArc(joinTransitions[i], p6);
        }
      }




      // Embed the inner activity.
      PN.mergePlaces(p2, PN.findPlace(newPrefix + ".initial"));
      PN.mergePlaces(p3, PN.findPlace(newPrefix + ".final"));
    }
  ]
;

