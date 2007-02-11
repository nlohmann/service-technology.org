/*****************************************************************************\
 * Copyright 2005, 2006, 2007 Niels Lohmann                                  *
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
 * \file    ast-to-petrinet-small.cc
 *
 * \brief   Petri net unparser (implementation)
 *
 * \author  Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          last changes of: \$Author: nielslohmann $
 *
 * \since   2005-11-01
 *
 * \date    \$Date: 2007/02/11 20:53:02 $
 *
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \note    This file was created using Kimwitu++ reading file
 *          bpel-unparse-petri.k.
 *          See http://site.informatik.hu-berlin.de/kimwitu++ for details.
 *
 * \version \$Revision: 1.88 $
 *
 */
%}





/******************************************************************************
  STANDARD ELEMENTS
******************************************************************************/

This=StandardElements(NiltTarget_list(), NiltSource_list(), jc) ->
  [petrinetsmall:
    {
      string prefix = toString(This->parentId) + ".";
      string newPrefix = toString(This->parentId) + ".internal";

      Place *p1 = PN.newPlace(prefix + "initial");
      Place *p2 = PN.newPlace(prefix + "final");

      // Embed the inner activity.
      PN.mergePlaces(p1, PN.findPlace(newPrefix + ".initial"));
      PN.mergePlaces(p2, PN.findPlace(newPrefix + ".final"));
    }
  ]
;





/******************************************************************************
 * Figure 22: Pattern for an activity that is source of links.
 *****************************************************************************/

This=StandardElements(NiltTarget_list(), a, jc) ->
  [petrinetsmall:
    {
      trace(TRACE_DEBUG, "[PNU]" + inString() + "Fig. 22 (new)\n");
      string prefix = toString(This->parentId) + ".";
      string newPrefix = toString(This->parentId) + ".internal";

      Place *p1 = PN.newPlace(prefix + "initial");
      Place *p2 = PN.newPlace(prefix + "done");
      Place *p3 = PN.newPlace(prefix + "final");


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
  [petrinetsmall:
    {
      trace(TRACE_DEBUG, "[PNU]" + inString() + "Fig. 23\n");
      cerr << "no patterns allowed with suppressJoinFailure=\"no\"" << endl;

      // we should do a litlle bit more here, but I don't like to go on (will fail anyway)
      assert(false);
    }
  ]
;





/******************************************************************************
 * Figure 24: Pattern for an activity that is target of links in case of
 * suppressJoinFailure="yes".
 *****************************************************************************/

This=StandardElements(a, NiltSource_list(), jc) provided (This->suppressJF)
 ->
  [petrinetsmall:
    {
      trace(TRACE_DEBUG, "[PNU]" + inString() + "Fig. 24 (new)\n");
      string prefix = toString(This->parentId) + ".";
      string newPrefix = toString(This->parentId) + ".internal";

      Place *p1 = PN.newPlace(prefix + "initial");
      Place *p2 = PN.newPlace(prefix + "final");
      Place *p3 = PN.newPlace(prefix + "joinTrue");
      Place *p4 = PN.newPlace(prefix + "joinFalse");
      Place *p5 = PN.newPlace(prefix + "begin");

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
          cerr << i << " " << joinCondition_result[i] << endl;

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
  [petrinetsmall:
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
          cerr << i << " " << joinCondition_result[i] << endl;

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
  [petrinetsmall:
    {
      trace(TRACE_DEBUG, "[PNU]" + inString() + "Fig. 26\n");
      cerr << "no patterns allowed with suppressJoinFailure=\"no\"" << endl;
    }
  ]
;
