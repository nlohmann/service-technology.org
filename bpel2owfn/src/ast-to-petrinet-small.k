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
 * \file    ast-to-petrinet-small.cc
 *
 * \brief   Petri net unparser (implementation)
 *
 * \author  responsible: Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          last changes of: \$Author: nlohmann $
 *
 * \since   2005-11-01
 *
 * \date    \$Date: 2006/11/29 13:06:30 $
 *
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \note    This file was created using Kimwitu++ reading file
 *          bpel-unparse-petri.k.
 *          See http://site.informatik.hu-berlin.de/kimwitu++ for details.
 *
 * \version \$Revision: 1.81 $
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

      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "final");

      // Embed the inner activity.
      TheNet->mergePlaces(p1, TheNet->findPlace(newPrefix + ".initial"));
      TheNet->mergePlaces(p2, TheNet->findPlace(newPrefix + ".final"));
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

      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "done");
      Place *p3 = TheNet->newPlace(prefix + "final");


      unsigned int linkCounter = 1;
      if (parameters[P_XOR] && (a->length() > 1))
      {
	// Set the outgoing links (XOR given).
	foreach (source; tSource_list a)
	{
  	  Transition *t1 = TheNet->newTransition(prefix + "setLinks" + toString(linkCounter++));
    	  TheNet->newArc(p2, t1);
      	  TheNet->newArc(t1, p3);

          Place *p4 = TheNet->findPlace("link." + ASTEmap[source->id]->linkName);
	  TheNet->newArc(t1, p4);

          foreach (source2; tSource_list a)
          {
            if (ASTEmap[source2->id]->linkName != ASTEmap[source->id]->linkName)
            {
              Place *p5 = TheNet->findPlace("!link." + ASTEmap[source2->id]->linkName);
	      TheNet->newArc(t1, p5);
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
  	Transition *t1 = TheNet->newTransition(prefix + "setLinks");
    	TheNet->newArc(p2, t1);
      	TheNet->newArc(t1, p3);

	// Set the outgoing links (standard transition condition expected).
	foreach (source; tSource_list a)
	{
          Place *p4 = TheNet->findPlace("link." + ASTEmap[source->id]->linkName);
	  TheNet->newArc(t1, p4);

          string givenTransitionCondition = ASTEmap[source->id]->attributes["transitionCondition"];
          if (givenTransitionCondition != "" && givenTransitionCondition != "true")
          {
            cerr << "Ignoring user-defined transition condition \"" << givenTransitionCondition << "\"" << endl;
            cerr << "Using standard transition \"true\" instead." << endl << endl;
          }
        }
      }




      // Embed the inner activity.
      TheNet->mergePlaces(p1, TheNet->findPlace(newPrefix + ".initial"));
      TheNet->mergePlaces(p2, TheNet->findPlace(newPrefix + ".final"));
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

      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "final");
      Place *p3 = TheNet->newPlace(prefix + "joinTrue");
      Place *p4 = TheNet->newPlace(prefix + "joinFalse");
      Place *p5 = TheNet->newPlace(prefix + "begin");

      Transition *t1 = TheNet->newTransition(prefix + "start");
      TheNet->newArc(p1, t1);
      TheNet->newArc(p3, t1);
      TheNet->newArc(t1, p5);

      Transition *t2 = TheNet->newTransition(prefix + "skip");
      TheNet->newArc(p1, t2);
      TheNet->newArc(p4, t2);
      TheNet->newArc(t2, p2);

      // sets enclosed source links to false
      dpeLinks(t2, parentId);


      // evaluate the join condition
      if (string(jc->op_name()) == "standardJoinCondition")
      {
        // containers for the join transitions and the incoming links
        map<unsigned int, Transition*> joinTransitions;
        map<unsigned int, Place*> trueLinks;
        map<unsigned int, Place*> falseLinks;


        // fill link containers
        int linkCount = 0;
	foreach (target; tTarget_list a)
	{
	  Place *linkT = TheNet->findPlace("link." + ASTEmap[target->id]->linkName);
	  Place *linkF = TheNet->findPlace("!link." + ASTEmap[target->id]->linkName);

          trueLinks[linkCount] = linkT;
          falseLinks[linkCount] = linkF;

          linkCount++;
	}


        // fill transition container and create OR-join
        unsigned int transitionNumber = (unsigned int)pow(2.0, a->length());
        for (unsigned int i = 0; i < transitionNumber; i++)
        {
          joinTransitions[i] = TheNet->newTransition(prefix + "join" + toString(i));

          for (unsigned int j = 0; j < linkCount; j++)
          {
            if (((i >> j) % 2) == 0) // this rather cryptic expression correctly connects the links
              TheNet->newArc(trueLinks[j], joinTransitions[i]);
            else
              TheNet->newArc(falseLinks[j], joinTransitions[i]);
          }

          // only one transition makes the join fail
          if (i == transitionNumber-1)
            TheNet->newArc(joinTransitions[i], p4);
          else
            TheNet->newArc(joinTransitions[i], p3);
        }
      }
      else
        cerr << "user-defined join conditions are not yet supported!" << endl;


      // Embed the inner activity.
      TheNet->mergePlaces(p5, TheNet->findPlace(newPrefix + ".initial"));
      TheNet->mergePlaces(p2, TheNet->findPlace(newPrefix + ".final"));
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

      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "begin");
      Place *p3 = TheNet->newPlace(prefix + "done");
      Place *p4 = TheNet->newPlace(prefix + "final");
      Place *p5 = TheNet->newPlace(prefix + "joinFalse");
      Place *p6 = TheNet->newPlace(prefix + "joinTrue");

      Transition *t1 = TheNet->newTransition(prefix + "start");
      TheNet->newArc(p1, t1);
      TheNet->newArc(p6, t1);
      TheNet->newArc(t1, p2);

      Transition *t2 = TheNet->newTransition(prefix + "skip");
      TheNet->newArc(p1, t2);
      TheNet->newArc(p5, t2);
      TheNet->newArc(t2, p4);

      // sets enclosed source links to false
      dpeLinks(t2, This->parentId);

      unsigned int linkCounter = 1;
      if (parameters[P_XOR] && (b->length() > 1))
      {
	// Set the outgoing links (XOR given).
	foreach (source; tSource_list b)
	{
  	  Transition *t3 = TheNet->newTransition(prefix + "setLinks" + toString(linkCounter++));
    	  TheNet->newArc(p3, t3);
      	  TheNet->newArc(t3, p4);

          Place *p7 = TheNet->findPlace("link." + ASTEmap[source->id]->linkName);
	  TheNet->newArc(t3, p7);

          foreach (source2; tSource_list b)
          {
            if (source2 != source)
            {
              Place *p8 = TheNet->findPlace("!link." + ASTEmap[source2->id]->linkName);
	      TheNet->newArc(t3, p8);
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
  	Transition *t3 = TheNet->newTransition(prefix + "setLinks");
    	TheNet->newArc(p3, t3);
      	TheNet->newArc(t3, p4);
      
	// Set the outgoing links (standard transition condition expected).
	foreach (source; tSource_list b)
        {
	  Place *p7 = TheNet->findPlace("link." + ASTEmap[source->id]->linkName);
          TheNet->newArc(t3, p7);

          string givenTransitionCondition = ASTEmap[source->id]->attributes["transitionCondition"];
          if (givenTransitionCondition != "" && givenTransitionCondition != "true")
          {
            cerr << "Ignoring user-defined transition condition \"" << givenTransitionCondition << "\"" << endl;
            cerr << "Using standard transition \"true\" instead." << endl << endl;
          }
        }
      }





      // evaluate the join condition
      if (string(jc->op_name()) == "standardJoinCondition")
      {
        // containers for the join transitions and the incoming links
        map<unsigned int, Transition*> joinTransitions;
        map<unsigned int, Place*> trueLinks;
        map<unsigned int, Place*> falseLinks;


        // fill link containers
        int linkCount = 0;
	foreach (target; tTarget_list a)
	{
	  Place *linkT = TheNet->findPlace("link." + ASTEmap[target->id]->linkName);
	  Place *linkF = TheNet->findPlace("!link." + ASTEmap[target->id]->linkName);

          trueLinks[linkCount] = linkT;
          falseLinks[linkCount] = linkF;

          linkCount++;
	}


        // fill transition container and create OR-join
        unsigned int transitionNumber = (unsigned int)pow(2.0, a->length());
        for (unsigned int i = 0; i < transitionNumber; i++)
        {
          joinTransitions[i] = TheNet->newTransition(prefix + "join" + toString(i));

          for (unsigned int j = 0; j < linkCount; j++)
          {
            if (((i >> j) % 2) == 0) // this rather cryptic expression correctly connects the links
              TheNet->newArc(trueLinks[j], joinTransitions[i]);
            else
              TheNet->newArc(falseLinks[j], joinTransitions[i]);
          }

          // only one transition makes the join fail
          if (i == transitionNumber-1)
            TheNet->newArc(joinTransitions[i], p5);
          else
            TheNet->newArc(joinTransitions[i], p6);
        }
      }
      else
        cerr << "user-defined join conditions are not yet supported!" << endl;



      // Embed the inner activity.
      TheNet->mergePlaces(p2, TheNet->findPlace(newPrefix + ".initial"));
      TheNet->mergePlaces(p3, TheNet->findPlace(newPrefix + ".final"));
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
