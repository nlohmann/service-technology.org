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
 * \file bpel-unparse-petri-small.cc
 *
 * \brief Petri net unparser (implementation)
 *
 * This file defines and implements the conversion of each BPEL activity to its
 * associated Petri net pattern followed by neccessary merge operations.
 *
 * \author
 *          - responsible: Niels Lohmann <nlohmann@informatik.hu-berlin.de>
 *          - last changes of: \$Author: nlohmann $
 *
 * \date
 *          - created 2005-11-01
 *          - last changed: \$Date: 2006/11/04 13:08:12 $
 *
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \note    This file was created using Kimwitu++ reading file
 *          bpel-unparse-petri.k.
 *          See http://site.informatik.hu-berlin.de/kimwitu++ for details.
 *
 * \version \$Revision: 1.64 $
 *
 */
%}





/******************************************************************************
  PROCESS
******************************************************************************/

/*
 * Pattern of BPEL's <process> as it is depicted in Fig. 28. Note that it
 * contains only one event handler since alarm-event handler and message-event
 * handler are merged.
 */

Process(a,b,c,d,e,f,g,h,i,_id) ->
  [petrinetsmall:
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


      // Create the initial place of the process pattern and mark it.
      Place *p1 = TheNet->newPlace(prefix + "initial");
      p1->mark();
      
      // Create the internal places of the process pattern.
      Place *p2 = TheNet->newPlace(prefix + "final");


      // Create subnet to support event handlers.
      if (ASTEmap[_id->value]->hasEH)
      {
	Place *p3 = TheNet->newPlace(prefix + "EHactive");
	Place *p4 = TheNet->newPlace(prefix + "begin");
	Place *p5 = TheNet->newPlace(prefix + "end");

	Transition *t1 = TheNet->newTransition(prefix + "activate");
	TheNet->newArc(p1, t1);
	TheNet->newArc(t1, p3);
	TheNet->newArc(t1, p4);

	Transition *t2 = TheNet->newTransition(prefix + "finalize1");
	TheNet->newArc(p3, t2);
	TheNet->newArc(p5, t2);
	TheNet->newArc(t2, p2);

	Transition *t3 = TheNet->newTransition(prefix + "finalize2");
	TheNet->newArc(p3, t3);
	TheNet->newArc(p5, t3);
	TheNet->newArc(t3, p2);
      }
    }
      g // Event Handler
      i // innerActivity
    {
      // Embed the inner activity of the process.
      if (ASTEmap[_id->value]->hasEH)
      {
	TheNet->mergePlaces(TheNet->findPlace("1.internal.begin"), TheNet->findPlace(i, ".initial"));
	TheNet->mergePlaces(TheNet->findPlace("1.internal.end"), TheNet->findPlace(i, ".final"));
      }
      else
      {
	TheNet->mergePlaces(TheNet->findPlace("1.internal.initial"), TheNet->findPlace(i, ".initial"));
	TheNet->mergePlaces(TheNet->findPlace("1.internal.final"), TheNet->findPlace(i, ".final"));
      }

      footer(_id, true);
      trace(TRACE_INFORMATION, "Generating Petri net complete.\n");
    }
  ]
;





/******************************************************************************
  EVENT HANDLERS
******************************************************************************/

/*
 * Instead of creating patterns for a alarm event handler and a message event
 * handler we united both patterns to a single event handler. In order to do
 * so, we used the place and transition numberings as they are introduced in
 * Fig. 29. This situation leads to different numberings when embedding the
 * onAlarm-activities into the event handler. In this case, both names (the
 * name in Fig. 29 and in Fig. 30) are mentioned in a comment.
 */

userDefinedEventHandler(a,b,_id) ->
  [petrinetsmall:
    {
      trace(TRACE_DEBUG, "[PNU]" + inString() + "<eventHandler scope=\"" + currentScope + "\">\n");
      string prefix = toString(ASTEmap[_id->value]->parentScopeId) + ".internal.eventHandler.";
    }
    a // onMessage branches
    b // onAlarm branches
    {
      int onMessageCount = 1;
      foreach (ThisBranch = OnMessage(innerActivity); tOnMessage_list a)
      {
        Transition *t1 = TheNet->newTransition(prefix + "ehM" + toString(onMessageCount));
        onMessageCount++;
      }


      int onAlarmCount = 1;
      foreach (onAlarmBranch=OnAlarm(innerActivity); tOnAlarm_list b)
      {
        Transition *t1 = TheNet->newTransition(prefix + "ehA" + toString(onAlarmCount));
        onAlarmCount++;
      }

      trace(TRACE_DEBUG, "[PNU]" + inString() + "</eventHandler scope=\"" + currentScope + "\">\n");
    }
  ]
;









/******************************************************************************
  EMPTY
******************************************************************************/

Empty(a,_id) ->
  [petrinetsmall:
    {
      header(_id);
      string prefix = toString(_id) + ".internal.";

      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "final");

      Transition *t1 = TheNet->newTransition(prefix + "empty");
      TheNet->newArc(p1, t1);
      TheNet->newArc(t1, p2);
    }
    a // standardElements
    {
      footer(_id);
    }
  ]
;





/******************************************************************************
  INVOKE
******************************************************************************/

AsynchronousInvoke(a,b,c,d,_id) ->
  [petrinetsmall:
    {
      header(_id);
      string prefix = toString(_id) + ".internal.";

      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "final");

      Transition *t1 = TheNet->newTransition(prefix + "invoke");
      TheNet->newArc(p1, t1);
      TheNet->newArc(t1, p2);
      TheNet->newArc(t1, TheNet->findPlace("out." + ASTEmap[_id->value]->channelName));
      t1->type = OUT;

      if (ASTEmap[_id->value]->inWhile)
        TheNet->findPlace("out." + ASTEmap[_id->value]->channelName)->inWhile = true;
    }
    a // standardElements
    {
      footer(_id);
    }
  ]
;


SynchronousInvoke(a,b,c,d,_id) ->
  [petrinetsmall:
    {
      header(_id);
      string prefix = toString(_id) + ".internal.";

      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "running");
      Place *p3 = TheNet->newPlace(prefix + "final");

      Transition *t1 = TheNet->newTransition(prefix + "invokeO");
      TheNet->newArc(t1, TheNet->findPlace("out." + ASTEmap[_id->value]->channelName));
      TheNet->newArc(p1, t1);
      TheNet->newArc(t1, p2);
      t1->type = OUT;

      Transition *t2 = TheNet->newTransition(prefix + "invokeI");
      TheNet->newArc(TheNet->findPlace("in." + ASTEmap[_id->value]->channelName), t2);
      TheNet->newArc(p2, t2);
      TheNet->newArc(t2, p3);
      t2->type = IN;

      if (ASTEmap[_id->value]->inWhile)
      {
        TheNet->findPlace("in." + ASTEmap[_id->value]->channelName)->inWhile = true;
        TheNet->findPlace("out." + ASTEmap[_id->value]->channelName)->inWhile = true;
      }
    }
    a // standardElements
    {
      footer(_id);
    }
  ]
;





/******************************************************************************
  RECEIVE
******************************************************************************/

Receive(a,b,c,_id) ->
  [petrinetsmall:
    {
      header(_id);
      string prefix = toString(_id) + ".internal.";

      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "final");

      Transition *t1 = TheNet->newTransition(prefix + "receive");
      TheNet->newArc(TheNet->findPlace("in." + ASTEmap[_id->value]->channelName), t1);
      TheNet->newArc(p1, t1);
      TheNet->newArc(t1, p2);
      t1->type = IN;

      if (ASTEmap[_id->value]->inWhile)
        TheNet->findPlace("in." + ASTEmap[_id->value]->channelName)->inWhile = true;
    }
    a // standardElements
    {
      footer(_id);
    }
  ]
;





/******************************************************************************
  REPLY
******************************************************************************/

Reply(a,b,_id) ->
  [petrinetsmall:
    {
      header(_id);
      string prefix = toString(_id) + ".internal.";

      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "final");

      Transition *t1 = TheNet->newTransition(prefix + "reply");
      TheNet->newArc(t1, TheNet->findPlace("out." + ASTEmap[_id->value]->channelName));
      TheNet->newArc(p1, t1);
      TheNet->newArc(t1, p2);
      t1->type = OUT;

      if (ASTEmap[_id->value]->inWhile)
        TheNet->findPlace("out." + ASTEmap[_id->value]->channelName)->inWhile = true;
    }
    a // standardElements
    {
      footer(_id);
    }
  ]
;





/******************************************************************************
  ASSIGN
******************************************************************************/

Assign(a,b,_id) ->
  [petrinetsmall:
    {
      header(_id);
      string prefix = toString(_id) + ".internal.";

      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "final");

      Transition *t1 = TheNet->newTransition(prefix + "assign"); // dummy transition

      TheNet->newArc(p1, t1);
      TheNet->newArc(t1, p2);
    }
    a // standardElements
    {
      footer(_id);
    }
  ]
;





/******************************************************************************
  VALIDATE                                                       (WS-BPEL 2.0)
******************************************************************************/

Validate(a,_id) ->
  [petrinetsmall:
    {
      header(_id);
      string prefix = toString(_id) + ".internal.";

      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "final");

      Transition *t1 = TheNet->newTransition(prefix + "validate"); // dummy transition

      TheNet->newArc(p1, t1);
      TheNet->newArc(t1, p2);
    }
    a // standardElements
    {
      footer(_id);
    }
  ]
;





/******************************************************************************
  WAIT
******************************************************************************/

WaitUntil(a,_id) ->
  [petrinetsmall:
    {
      header(_id);
      string prefix = toString(_id) + ".internal.";

      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "final");

      Transition *t1 = TheNet->newTransition(prefix + "waitUntil");
      TheNet->newArc(p1, t1);
      TheNet->newArc(t1, p2);
    }
    a // standardElements
    {
      footer(_id);
    }
  ]
;





WaitFor(a,_id) ->
  [petrinetsmall:
    {
      header(_id);
      string prefix = toString(_id) + ".internal.";

      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "final");

      Transition *t1 = TheNet->newTransition(prefix + "waitFor");
      TheNet->newArc(p1, t1);
      TheNet->newArc(t1, p2);
    }
    a // standardElements
    {
      footer(_id);
    }
  ]
;





/******************************************************************************
  THROW
******************************************************************************/

Throw(a,_id) ->
  [petrinetsmall:
    {
      header(_id);
      string prefix = toString(_id) + ".internal.";

      cerr << "the <throw> activity cannot be modelled!" << endl;

      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "final");

      Transition *t1 = TheNet->newTransition(prefix + "throw");
      TheNet->newArc(p1, t1);
      TheNet->newArc(t1, p2);
    }
    a // standardElements
    {
      footer(_id);
    }
  ]
;





/******************************************************************************
  COMPENSATE
******************************************************************************/

Compensate(a,_id) ->
  [petrinetsmall:
    {
      header(_id);
      string prefix = toString(_id) + ".internal.";

      cerr << "the <compensate> activity cannot be modelled!" << endl;

      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "final");

      Transition *t1 = TheNet->newTransition(prefix + "empty");
      TheNet->newArc(p1, t1);
      TheNet->newArc(t1, p2);
    }
    a // standardElements
    {
      footer(_id);
    }
  ]
;





/******************************************************************************
  TERMINATE                                                      (BPEL4WS 1.1)
******************************************************************************/

Terminate(a,_id) ->
  [petrinetsmall:
    {
      header(_id);
      string prefix = toString(_id) + ".internal.";

      cerr << "the <terminate> activity cannot be modelled!" << endl;

      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "final");

      Transition *t1 = TheNet->newTransition(prefix + "empty");
      TheNet->newArc(p1, t1);
      TheNet->newArc(t1, p2);
    }
    a // standardElements
    {
      footer(_id);
    }
  ]
;





/******************************************************************************
  EXIT                                                           (WS-BPEL 2.0)
******************************************************************************/

Exit(a,_id) ->
  [petrinetsmall:
    {
      header(_id);
      string prefix = toString(_id) + ".internal.";

      cerr << "the <exit> activity cannot be modelled!" << endl;

      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "final");

      Transition *t1 = TheNet->newTransition(prefix + "empty");
      TheNet->newArc(p1, t1);
      TheNet->newArc(t1, p2);
    }
    a // standardElements
    {
      footer(_id);
    }
  ]
;





/******************************************************************************
  FLOW
******************************************************************************/

Flow(a,b,c,_id) ->
  [petrinetsmall:
    {
      header(_id, true);
      string prefix = toString(_id) + ".internal.";

      Place *p1  = TheNet->newPlace(prefix + "initial");
      Place *p2  = TheNet->newPlace(prefix + "final");

      Transition *t1 = TheNet->newTransition(prefix + "split");
      TheNet->newArc(p1, t1);

      Transition *t2 = TheNet->newTransition(prefix + "join");
      TheNet->newArc(t2, p2);
    }
    a // standardElements
    c // inner activities
    {
      foreach (innerActivity; activity_list c)
      {
	TheNet->newArc(t1, TheNet->findPlace(innerActivity, ".initial"));
	TheNet->newArc(TheNet->findPlace(innerActivity, ".final"), t2);
      }
      footer(_id, true);
    }
  ]
;





/******************************************************************************
  SWITCH                                                         (BPEL4WS 1.1)
******************************************************************************/

Switch(a,b,otherwiseBranch=Otherwise(c,d),_id) ->
  [petrinetsmall:
    {
      header(_id, true);
      string prefix = toString(_id) + ".internal.";

      Place *p1 =  TheNet->newPlace(prefix + "final");
      Place *p2 =  TheNet->newPlace(prefix + "initial");
    }
    a // standardElements
    b // case braches
    c // otherwise branch
    {
      // the <case> branches
      int currentCase = 1;
      foreach (caseBranch=Case(innerActivity,_id2); tCase_list b)
      {
	string caseNumber = toString(currentCase++);
	Transition* t1 = TheNet->newTransition(prefix + "case" + caseNumber);
	TheNet->newArc(TheNet->findPlace(_id, ".internal.initial"), t1); // added ".internal" here

	// Embed the inner activity.
	TheNet->newArc(t1, TheNet->findPlace(innerActivity, ".initial"));
	TheNet->mergePlaces(_id->value, ".internal.final", _id2->value, ".final");
      }


      // the <otherwise> branch
      Transition *t1 = TheNet->newTransition(prefix + "caseo");
      TheNet->newArc(TheNet->findPlace(_id, ".internal.initial"), t1); // added ".internal" here

      // embed inner activity
      TheNet->newArc(t1, TheNet->findPlace(c, ".initial"));
      TheNet->mergePlaces(_id->value, ".internal.final", d->value, ".final");


      // Create the arcs for the Dead Path elimination: each chosen activity
      // (i.e. firing of the respective t2-transition) has set the links of all
      // other branches to false.
      currentCase = 1;
      foreach (Case(innerActivity1); tCase_list b)
      {
        foreach (Case(innerActivity2); tCase_list b)
        {
	  if (innerActivity1 != innerActivity2)
            dpeLinks(TheNet->findTransition(prefix + "case" + toString(currentCase)), innerActivity2->id); // case -> case'
    	}
        dpeLinks(TheNet->findTransition(prefix + "case" + toString(currentCase)), c->id); // case -> otherwise
        dpeLinks(TheNet->findTransition(prefix + "caseo"), innerActivity1->id); // otherwise -> case
  	currentCase++;
      }

      footer(_id, true);
    }
  ]
;





/******************************************************************************
  IF                                                             (WS-BPEL 2.0)
******************************************************************************/

If(a,b,c,_id) ->
  [petrinetsmall:
    {
      header(_id, true);
      string prefix = toString(_id) + ".internal.";

      Place *p1 =  TheNet->newPlace(prefix + "initial");
      Place *p2 =  TheNet->newPlace(prefix + "final");
    }
    a // standardElements
    b // elseIf braches
    c // else branch
    {
      int currentCase = 1;
      foreach (caseBranch=ElseIf(innerActivity, _id2); tElseIf_list b)
      {
	string caseNumber = toString(currentCase++);
	Transition* t1 = TheNet->newTransition(prefix + "case" + caseNumber);
	TheNet->newArc(TheNet->findPlace(_id, ".internal.initial"), t1); // added ".internal" here

	// Embed the inner activity.
	TheNet->newArc(t1, TheNet->findPlace(innerActivity, ".initial"));
	TheNet->mergePlaces(_id->value, ".internal.final", _id2->value, ".final");
      }

      // The <else> branch.
      Transition *t1 = TheNet->newTransition(prefix + "caseo");
      TheNet->newArc(TheNet->findPlace(_id, ".internal.initial"), t1); // added ".internal" here

      with (c)
      {
        c=Else(innerActivity,_id2):
        {
          // embed inner activity
          TheNet->newArc(t1, TheNet->findPlace(innerActivity, ".initial"));
          TheNet->mergePlaces(_id->value, ".internal.final", _id2->value, ".final");
        }
        c=NoElse(_id2):
        {
          TheNet->newArc(t1, TheNet->findPlace(_id, ".internal.final"));
        }
        default: { /* do nothing */ }
      }


      // Create the arcs for the Dead Path elimination: each chosen activity
      // (i.e. firing of the respective t2-transition) has set the links of all
      // other branches to false.
      currentCase = 1;
      foreach (ElseIf(innerActivity1); tElseIf_list b)
      {
        foreach (ElseIf(innerActivity2); tElseIf_list b)
         {
	  if (innerActivity1 != innerActivity2)
          {
            dpeLinks(TheNet->findTransition(prefix + "case" + toString(currentCase)), innerActivity2->id); // case -> case'
          }
    	}
        with (c)
        {
          c=Else(innerActivity3,_id2):
          {
            dpeLinks(TheNet->findTransition(prefix + "case" + toString(currentCase)), innerActivity3->id); // case -> otherwise
            dpeLinks(TheNet->findTransition(prefix + "caseo"), innerActivity1->id); // otherwise -> case
          }
          default: { /* do nothing */ }
        }
  	currentCase++;
      }

      footer(_id, true);
    }
  ]
;





/******************************************************************************
  WHILE
******************************************************************************/

While(a,b,_id) ->
  [petrinetsmall:
    {
      header(_id, true);
      string prefix = toString(_id) + ".internal.";

      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "running");
      Place *p3 = TheNet->newPlace(prefix + "final");
      Place *p4 = TheNet->newPlace(prefix + "done");

      Transition *t1 = TheNet->newTransition(prefix + "enter");
      TheNet->newArc(p1, t1);
      TheNet->newArc(t1, p2);

      Transition *t2 = TheNet->newTransition(prefix + "leave");
      TheNet->newArc(p1, t2);
      TheNet->newArc(t2, p3);

      Transition *t3 = TheNet->newTransition(prefix + "loop");
      TheNet->newArc(p4, t3);
      TheNet->newArc(t3, p1);
    }
    a // standardElements
    b // innerActivity
    {
      // Embed the inner activity.
      TheNet->mergePlaces(_id->value, ".internal.running", b->id, ".initial");
      TheNet->mergePlaces(_id->value, ".internal.done", b->id, ".final");

      footer(_id, true);
    }
  ]
;





/******************************************************************************
  REPEAT UNTIL                                                   (WS-BPEL 2.0)
******************************************************************************/

RepeatUntil(a,b,_id) ->
  [petrinetsmall:
    {
      header(_id, true);
      string prefix = toString(_id) + ".internal.";

      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "p2");
      Place *p3 = TheNet->newPlace(prefix + "final");

      Transition *t1 = TheNet->newTransition(prefix + "end");
      TheNet->newArc(p2, t1);
      TheNet->newArc(t1, p3);

      Transition *t2 = TheNet->newTransition(prefix + "repeat");
      TheNet->newArc(p2, t2);
      TheNet->newArc(t2, p1);
    }
    a // standardElements
    b // innerActivity
    {
      // Embed the inner activity.
      TheNet->mergePlaces(_id->value, ".internal.initial", b->id, ".initial");
      TheNet->mergePlaces(_id->value, ".internal.p2", b->id, ".final");

      footer(_id, true);
    }
  ]
;





/******************************************************************************
  SEQUENCE
******************************************************************************/

This=Sequence(a,b,c) ->
  [petrinetsmall:
    {
      header(This->id, true);
      string prefix = toString(This->id) + ".internal.";

      Place *p1  = TheNet->newPlace(prefix + "initial");
      Place *p2  = TheNet->newPlace(prefix + "final");
    }
    a // standardElements for this activity
    b // inner activities
    {
      int activityCount = 1;
      kc::impl_activity* lastActivity;

      foreach (innerActivity; activity_list b)
      {
	if (activityCount == 1)
	  TheNet->mergePlaces(This->id, ".internal.initial", innerActivity->id, ".initial");
	else
	  TheNet->mergePlaces(lastActivity->id, ".final", innerActivity->id, ".initial");

	if (activityCount == b->length())
	  TheNet->mergePlaces(This->id, ".internal.final", innerActivity->id, ".final");

	lastActivity = innerActivity;
	activityCount++;
      }

      footer(This->id, true);
    }
  ]
;





/******************************************************************************
  PICK
******************************************************************************/

This=Pick(a,b,c,d) ->
  [petrinetsmall:
    {
      header(This->id, true);
      string prefix = toString(This->id) + ".internal.";

      Place *p1 = TheNet->newPlace(prefix + "final");
      Place *p2 = TheNet->newPlace(prefix + "initial");
    }
    a // standardElements
    b // onMessage branches
    c // onAlarm branches
    {
      // Traverse the <onMessage> branches.
      int onMessageCount = 1;
      foreach (ThisBranch = OnMessage(_corr,_from,innerActivity,_id2); tOnMessage_list b)
      {
	trace(TRACE_DEBUG, "[PNU]\t\t<onMessage>\n");
	string message = toString(onMessageCount++);

	Transition *t1 = TheNet->newTransition(prefix + "pickM" + message);
	TheNet->newArc(TheNet->findPlace("in." + ASTEmap[ThisBranch->id]->channelName), t1);
	TheNet->newArc(TheNet->findPlace(This->id, ".internal.initial"), t1);
	TheNet->newArc(t1, TheNet->findPlace(innerActivity, ".initial"));
	t1->type = IN;

        if (ASTEmap[ThisBranch->id]->inWhile)
          TheNet->findPlace("in." + ASTEmap[ThisBranch->id]->channelName)->inWhile = true;

	// Embed the inner activity.
	TheNet->mergePlaces(This->id, ".final", innerActivity->id, ".final");

	trace(TRACE_DEBUG, "[PNU]\t\t</onMessage>\n");
      }


      // Traverse the <onAlarm> branches.
      int onAlarmCount = 1;
      foreach (onAlarmBranch=OnAlarm(innerActivity); tOnAlarm_list c)
      {
	trace(TRACE_DEBUG, "[PNU]\t\t<onAlarm>\n");
        string alarm = toString(onAlarmCount++);

	Transition *t1 = TheNet->newTransition(prefix + "pickA" + alarm);
	TheNet->newArc(TheNet->findPlace(This->id, ".internal.initial"), t1);
	TheNet->newArc(t1, TheNet->findPlace(innerActivity->id, ".initial"));


	// Embed the inner activity.
	TheNet->mergePlaces(This->id, ".final", innerActivity->id, ".final");

	trace(TRACE_DEBUG, "[PNU]\t\t</onAlarm>\n");
      }


      // Create the arcs for the Dead Path elimination: each chosen activity
      // (i.e. firing of the respective t2/t6-transition) has set the links of
      // all other branches to false.
      int currentMessage = 1;
      foreach (OnMessage(innerActivity1); tOnMessage_list b) {
	foreach (OnMessage(innerActivity2); tOnMessage_list b) {
	  if ((innerActivity1 != innerActivity2))
            dpeLinks(TheNet->findTransition(prefix + "pickM" + toString(currentMessage)), innerActivity2->id); // onMessage -> onMessage'
	}

	foreach (OnAlarm(innerActivity2); tOnAlarm_list c) {
          dpeLinks(TheNet->findTransition(prefix + "pickM" + toString(currentMessage)), innerActivity2->id); // onMessage -> onAlarm
	}
	currentMessage++;
      }

      int currentAlarm = 1;
      foreach (OnAlarm(innerActivity1); tOnAlarm_list c) {
	foreach (otheronalarm=OnAlarm(innerActivity2); tOnAlarm_list c) {
	  if ((innerActivity1 != innerActivity2))
            dpeLinks(TheNet->findTransition(prefix + "pickA" + toString(currentAlarm)), innerActivity2->id); // onAlarm -> onAlarm'
	}

	foreach (OnMessage(innerActivity2); tOnMessage_list b) {
          dpeLinks(TheNet->findTransition(prefix + "pickA" + toString(currentAlarm)), innerActivity2->id); // onAlarm -> onMessage
	}
	currentAlarm++;
      }

      footer(This->id, true);
    }
  ]
;





/******************************************************************************
  SCOPE
******************************************************************************/

This=Scope(a,b,c,d,e,f,g,h,i) ->
  [petrinetsmall:
    {
      header(This->id);
      string prefix = toString(This->id) + ".internal.";
      
      cerr << "the handlers of the <scope>-activity will not be modelled!" << endl;

      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "final");
    }
    a // standardElements
    h // innerActivity
    {
      // Embed the inner activity.
      TheNet->mergePlaces(This->id, ".internal.initial", h->id, ".initial");
      TheNet->mergePlaces(This->id, ".internal.final", h->id, ".final");

      footer(This->id);
    }
  ]
;





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
