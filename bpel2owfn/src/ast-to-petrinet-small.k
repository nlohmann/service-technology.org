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
 *          - last changed: \$Date: 2006/06/07 12:44:18 $
 *
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \note    This file was created using Kimwitu++ reading file
 *          bpel-unparse-petri.k.
 *          See http://site.informatik.hu-berlin.de/kimwitu++ for details.
 *
 * \version \$Revision: 1.15 $
 *
 */
%}


%{
/*!
 * \file bpel-unparse-petri-small.h
 * \brief Petri net unparser (interface)
 *
 * See \ref bpel-unparse-petri-small.cc for more information.
 */
%}




/******************************************************************************/

// All the includes and variables can be used during the unparsing.
%{ KC_UNPARSE
#include <iostream>
#include "petrinet.h"
#include "check-symbols.h"
#include "symbol-table.h"
#include "options.h"
#include "bpel-unparse-tools.h"
#include "ast-printers.h"
#include <cmath>

using namespace std;

// introduced in main.c
extern PetriNet *TheNet;

// introduced in bpel-syntax.y
extern SymbolTable symTab;
//CG extern SymbolManager symMan;
extern STProcess * stProcess;

%}




/******************************************************************************
  PROCESS
******************************************************************************/

/*
 * Pattern of BPEL's <process> as it is depicted in Fig. 28. Note that it
 * contains only one event handler since alarm-event handler and message-event
 * handler are merged.
 */

This=Process(a,b,c,d,e,f,g,h,i) ->
  [petrinetsmall:
    {
      trace(TRACE_INFORMATION, "Generating Petri net...\n");
      header(This->id, true);
      STProcess* myself = dynamic_cast<STProcess*> (symTab.lookup(This->id));

      string prefix = intToString(This->id->value) + ".internal.";
      currentScope = prefix;


      // Create the interface places.
      // The used lists are filled during parsing mapping each combination of
      // partnerLink, portType and operation to one input (resp. output) place.
      for (set<string>::iterator in = (myself->inChannels).begin(); in != (myself->inChannels).end(); in++)
	TheNet->newPlace("in." + *in, IN);
      for (set<string>::iterator out = (myself->outChannels).begin(); out != (myself->outChannels).end(); out++)
	TheNet->newPlace("out." + *out, OUT);


      // Create the places for the links.
      // The used list is filled during parsing and contains all links of the
      // whole process -- also links defined in a deeper hierarchy.
      for (list<STLink*>::iterator link = myself->enclosedLinks.begin(); link != myself->enclosedLinks.end(); link++)
      {
	TheNet->newPlace("link." + (*link)->name);
	TheNet->newPlace("!link." + (*link)->name);
      }


      // Create the initial place of the process pattern and mark it.
      Place *p1 = TheNet->newPlace(prefix + "initial");
      p1->mark();
      
      // Create the internal places of the process pattern.
      Place *p39 = TheNet->newPlace(prefix + "final");
    }
//      g // Event Handler
      i // innerActivity
    {
      // Embed the inner activity of the process.
      TheNet->mergePlaces(TheNet->findPlace("1.internal.initial"), TheNet->findPlace(i, ".initial"));
      TheNet->mergePlaces(TheNet->findPlace("1.internal.final"), TheNet->findPlace(i, ".final"));

      footer(This->id, true);
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

This=userDefinedEventHandler(a,b) ->
  [petrinetsmall:
    {
      trace(TRACE_DEBUG, "[PNU]" + inString() + "<eventHandler scope=\"" + currentScope + "\">\n");

      string prefix = intToString(This->parentScopeId->value) + ".internal.eventHandler.";
      Place *p6  = TheNet->newPlace(prefix + "initial");
      Place *p8  = TheNet->newPlace(prefix + "final");
      Place *p13 = TheNet->newPlace(prefix + "finish");
      Place *p17 = TheNet->newPlace(prefix + "running");
      Place *p18 = TheNet->newPlace(prefix + "finishing");
      Place *p22 = TheNet->newPlace(prefix + "stop");
      Place *p23 = TheNet->newPlace(prefix + "stopped");

      Transition *t5 = TheNet->newTransition(prefix + "t5"); // t6 in MEH
      TheNet->newArc(p6, t5);
      TheNet->newArc(t5, p17);
      TheNet->newArc(TheNet->findPlace("1.internal.clock"), t5, READ);

      Transition *t7 = TheNet->newTransition(prefix + "allFinished");
      TheNet->newArc(t7, p8);
      TheNet->newArc(p18, t7);

      Transition *t11 = TheNet->newTransition(prefix + "t11"); // t13 in MEH
      TheNet->newArc(p13, t11); // finish
      TheNet->newArc(p17, t11); // running
      TheNet->newArc(t11, p18); // finishing

      Transition *t16 = TheNet->newTransition(prefix + "allStopped"); // t24 in MEH (allStopped)
      TheNet->newArc(t16, p23);

      Transition *t17 = TheNet->newTransition(prefix + "stop+finish"); // t18 in MEH (stop+finish)
      TheNet->newArc(p18, t17);
      TheNet->newArc(p22, t17);

      Transition *t18 = TheNet->newTransition(prefix + "t18");
      TheNet->newArc(p6, t18);
      TheNet->newArc(p22, t18);
      TheNet->newArc(t18, p23);

      Transition *t19 = TheNet->newTransition(prefix + "normalStop"); // t27 in MEH (normalStop)
      TheNet->newArc(p17, t19);
      TheNet->newArc(p22, t19);

      Transition *t20 = TheNet->newTransition(prefix + "t20");
      TheNet->newArc(p8, t20);
      TheNet->newArc(p22, t20);
      TheNet->newArc(t20, p23);
    }
    a // onMessage branches
    b // onAlarm branches
    {
      int onMessageCount = 1;
      foreach (ThisBranch = OnMessage(innerActivity); tOnMessage_list a)
      {
	string message = intToString(onMessageCount++);
	string newPrefix = prefix + "onMessage" + message + ".";

	Place *onMessage_p1  = TheNet->newPlace(newPrefix + "p1");
	Place *onMessage_p2  = TheNet->newPlace(newPrefix + "p2");
	Place *onMessage_p3  = TheNet->newPlace(newPrefix + "p3");
	Place *onMessage_p4  = TheNet->newPlace(newPrefix + "p4");
	Place *onMessage_p5  = TheNet->newPlace(newPrefix + "wait" + message );
	Place *onMessage_p6  = TheNet->newPlace(newPrefix + "p6");
	Place *onMessage_p7  = TheNet->newPlace(newPrefix + "finish" + message );
	Place *onMessage_p9  = TheNet->newPlace(newPrefix + "finish+stop" + message );
	Place *onMessage_p11 = TheNet->newPlace(newPrefix + "stop" + message );
	Place *onMessage_p12 = TheNet->newPlace(newPrefix + "p12");

	Transition *onMessage_t2 = TheNet->newTransition(newPrefix + "t2");
	TheNet->newArc(TheNet->findPlace("in." + string((dynamic_cast<STCommunicationActivity*>(symTab.lookup(ThisBranch->id)))->channelId)), onMessage_t2);
	TheNet->newArc(onMessage_t2, onMessage_p2);
	TheNet->newArc(onMessage_p5, onMessage_t2);
	onMessage_t2->type = IN;

	Transition *onMessage_t3 = TheNet->newTransition(newPrefix + "t3");
	TheNet->newArc(TheNet->findPlace("variable." + string((dynamic_cast<STCommunicationActivity*>(symTab.lookup(ThisBranch->id)))->variable->name)), onMessage_t3);
  	TheNet->newArc(onMessage_t3, TheNet->findPlace("variable." + string((dynamic_cast<STCommunicationActivity*>(symTab.lookup(ThisBranch->id)))->variable->name)));
	TheNet->newArc(onMessage_p2, onMessage_t3);
	TheNet->newArc(onMessage_t3, onMessage_p3);

	
  	// When the parameter "-p cycliceh" is set, the pattern is created as
    	// it is depicted in Fig. 29/30. Setting this option leads to cyclic
      	// Petri nets.
	Transition *onMessage_t4 = TheNet->newTransition(newPrefix + "t4");
	TheNet->newArc(onMessage_p4, onMessage_t4);
	Transition *onMessage_t12 = TheNet->newTransition(newPrefix + "t12");
	TheNet->newArc(onMessage_p11, onMessage_t12);
	TheNet->newArc(onMessage_t12, onMessage_p12);
	
	if (parameters[P_CYCLICEH] == true)
	{
	  TheNet->newArc(onMessage_t4, onMessage_p5); // loop
	  TheNet->newArc(onMessage_p5, onMessage_t12);
	}
	else
	{
	  TheNet->newArc(onMessage_t4, onMessage_p6); // abort
	  TheNet->newArc(onMessage_p6, onMessage_t12);
	}

	
	Transition *onMessage_t5 = TheNet->newTransition(newPrefix + "t5");
	TheNet->newArc(onMessage_p5, onMessage_t5);
	TheNet->newArc(onMessage_t5, onMessage_p6);
	TheNet->newArc(onMessage_p7, onMessage_t5);

	Transition *onMessage_t7 = TheNet->newTransition(newPrefix + "t7");
	TheNet->newArc(onMessage_p7, onMessage_t7);
	TheNet->newArc(onMessage_p9, onMessage_t7);
	TheNet->newArc(onMessage_t7, onMessage_p11);

	Transition *onMessage_t8 = TheNet->newTransition(newPrefix + "t8");
	TheNet->newArc(onMessage_p6, onMessage_t8);
	TheNet->newArc(onMessage_p9, onMessage_t8);
	TheNet->newArc(onMessage_t8, onMessage_p12);

	Transition *onMessage_t10 = TheNet->newTransition(newPrefix + "t10");
	TheNet->newArc(onMessage_p1, onMessage_t10);
	TheNet->newArc(onMessage_p11, onMessage_t10);
	TheNet->newArc(onMessage_t10, onMessage_p12);

	Transition *onMessage_t11 = TheNet->newTransition(newPrefix + "t11");
	TheNet->newArc(onMessage_p2, onMessage_t11);
	TheNet->newArc(onMessage_p11, onMessage_t11);
	TheNet->newArc(onMessage_t11, onMessage_p12);


	if (parameters[P_NOSTANDARDFAULTS] == false)
	  Transition *t1 = throwFault(onMessage_p2, onMessage_p1, "p2", prefix, mkinteger(0));

	// arcs from the EH
	TheNet->newArc(t11, onMessage_p7); // t11 in AEH is t13 in MEH
	TheNet->newArc(t5,  onMessage_p5); // t5  in AEH is t6  in MEH
	TheNet->newArc(t17, onMessage_p9); // t17 in AEH is t18 in MEH (stop+finish)
	TheNet->newArc(t19, onMessage_p11); // t19 in AEH is t27 in MEH (normalStop)

	// arcs to the EH
	TheNet->newArc(onMessage_p6, t7); // "allFinished" in AEH is "finishAll" in MEH
	TheNet->newArc(onMessage_p12, t16);

	// embed the innerActivity
	TheNet->mergePlaces(onMessage_p3,  TheNet->findPlace(innerActivity, ".initial"));
	TheNet->mergePlaces(onMessage_p11, TheNet->findPlace(innerActivity, ".stop"));
	TheNet->mergePlaces(onMessage_p12, TheNet->findPlace(innerActivity, ".stopped"));
	TheNet->mergePlaces(onMessage_p4,  TheNet->findPlace(innerActivity, ".final"));
      }


      int onAlarmCount = 1;
      foreach (onAlarmBranch=OnAlarm(innerActivity); tOnAlarm_list b)
      {
	string onAlarmGuard;
	string newPrefix = prefix + "onAlarm" + intToString(onAlarmCount) + ".";

	if (onAlarmBranch->For != mkcasestring(""))
	  onAlarmGuard = "x + " + string(onAlarmBranch->For->name) + " \\<= y"; // "<=" is escaped
	else
	  onAlarmGuard = string(onAlarmBranch->until->name) + " \\<= y"; // "<=" is escaped

	Place *onAlarm_p1  = TheNet->newPlace(newPrefix + "p1");
	Place *onAlarm_p2  = TheNet->newPlace(newPrefix + "p2");
	Place *onAlarm_p3  = TheNet->newPlace(newPrefix + "wait" + intToString(onAlarmCount));
	Place *onAlarm_p4  = TheNet->newPlace(newPrefix + "p4");
	Place *onAlarm_p5  = TheNet->newPlace(newPrefix + "finish" + intToString(onAlarmCount));
	Place *onAlarm_p7  = TheNet->newPlace(newPrefix + "stop+finish" + intToString(onAlarmCount));
	Place *onAlarm_p9  = TheNet->newPlace(newPrefix + "stop" + intToString(onAlarmCount));
	Place *onAlarm_p10 = TheNet->newPlace(newPrefix + "p10");

	Transition *onAlarm_t1 = TheNet->newTransition(newPrefix + "t1");
	TheNet->newArc(onAlarm_p3, onAlarm_t1);
	TheNet->newArc(onAlarm_t1, onAlarm_p1);
	TheNet->newArc(TheNet->findPlace("1.internal.clock"), onAlarm_t1, READ);

	Transition *onAlarm_t2 = TheNet->newTransition(newPrefix + "t2");
	TheNet->newArc(onAlarm_p2, onAlarm_t2);
	TheNet->newArc(onAlarm_t2, onAlarm_p4);
	TheNet->newArc(onAlarm_p5, onAlarm_t2);

	Transition *onAlarm_t3 = TheNet->newTransition(newPrefix + "t3");
	TheNet->newArc(onAlarm_p3, onAlarm_t3);
	TheNet->newArc(onAlarm_t3, onAlarm_p4);
	TheNet->newArc(onAlarm_p5, onAlarm_t3);

	Transition *onAlarm_t4 = TheNet->newTransition(newPrefix + "t4");
	TheNet->newArc(onAlarm_p5, onAlarm_t4);
	TheNet->newArc(onAlarm_p7, onAlarm_t4);
	TheNet->newArc(onAlarm_t4, onAlarm_p9);

	Transition *onAlarm_t6 = TheNet->newTransition(newPrefix + "t6");
	TheNet->newArc(onAlarm_p4, onAlarm_t6);
	TheNet->newArc(onAlarm_p7, onAlarm_t6);
	TheNet->newArc(onAlarm_t6, onAlarm_p10);

	Transition *onAlarm_t8 = TheNet->newTransition(newPrefix + "t8");
	TheNet->newArc(onAlarm_p3, onAlarm_t8);
	TheNet->newArc(onAlarm_p9, onAlarm_t8);
	TheNet->newArc(onAlarm_t8, onAlarm_p10);


	// Arcs from the EH.
	TheNet->newArc(t5,  onAlarm_p3);
	TheNet->newArc(t11, onAlarm_p5);
	TheNet->newArc(t17, onAlarm_p7);
	TheNet->newArc(t19, onAlarm_p9);


	// Arcs to the EH.
	TheNet->newArc(onAlarm_p4,  t7);
	TheNet->newArc(onAlarm_p10, t16);


	// Embed the innerActivity.
	TheNet->mergePlaces(onAlarm_p1,  TheNet->findPlace(innerActivity, ".initial"));
	TheNet->mergePlaces(onAlarm_p9,  TheNet->findPlace(innerActivity, ".stop"));
	TheNet->mergePlaces(onAlarm_p10, TheNet->findPlace(innerActivity, ".stopped"));
	TheNet->mergePlaces(onAlarm_p2,  TheNet->findPlace(innerActivity, ".final"));

	onAlarmCount++;
      }

      trace(TRACE_DEBUG, "[PNU]" + inString() + "</eventHandler scope=\"" + currentScope + "\">\n");
    }
  ]
;









/******************************************************************************
  EMPTY
******************************************************************************/

/*
 * The <empty> activity as depicted in Fig. 3.
 */

This=Empty(a) ->
  [petrinetsmall:
    {
      string prefix = intToString(This->id->value) + ".internal.";
      header(This->id);

      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "final");

      Transition *t1 = TheNet->newTransition(prefix + "empty");
      TheNet->newArc(p1, t1);
      TheNet->newArc(t1, p2);
    }
    a // standardElements
    {
      footer(This->id);
    }
  ]
;





/******************************************************************************
  INVOKE
******************************************************************************/

/* The (asynchronous) <invoke> activity */

This=Invoke(a,b) provided ((dynamic_cast<STCommunicationActivity*> (symTab.lookup(This->id)))->isAsynchronousInvoke) ->
  [petrinetsmall:
    {
      string prefix = intToString(This->id->value) + ".internal.";
      STCommunicationActivity* myself = dynamic_cast<STCommunicationActivity*> (symTab.lookup(This->id));
      header(This->id);

      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "final");

      Transition *t1 = TheNet->newTransition(prefix + "invoke");
      TheNet->newArc(p1, t1);
      TheNet->newArc(t1, p2);
      TheNet->newArc(t1, TheNet->findPlace("out." + string((dynamic_cast<STCommunicationActivity*>(symTab.lookup(This->id)))->channelId)));
      t1->type = OUT;
    }
    a // standardElements
    {
      footer(This->id);
    }
  ]
;





/* The (synchronous) <invoke> activity. */

This=Invoke(a,b) provided (!(dynamic_cast<STCommunicationActivity*> (symTab.lookup(This->id)))->isAsynchronousInvoke) ->
  [petrinetsmall:
    {
      header(This->id);
      STCommunicationActivity* myself = dynamic_cast<STCommunicationActivity*> (symTab.lookup(This->id));
      string prefix = intToString(This->id->value) + ".internal.";

      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "running");
      Place *p3 = TheNet->newPlace(prefix + "final");

      Transition *t1 = TheNet->newTransition(prefix + "invokeSend");
      TheNet->newArc(t1, TheNet->findPlace("out." + string((dynamic_cast<STCommunicationActivity*>(symTab.lookup(This->id)))->channelId)));
      TheNet->newArc(p1, t1);
      TheNet->newArc(t1, p2);
      t1->type = OUT;

      Transition *t2 = TheNet->newTransition(prefix + "invokeReceive");
      TheNet->newArc(TheNet->findPlace("in." + string((dynamic_cast<STCommunicationActivity*>(symTab.lookup(This->id)))->channelId)), t2);
      TheNet->newArc(p2, t2);
      TheNet->newArc(t2, p3);
      t2->type = IN;
    }
    a // standardElements
    {
      footer(This->id);
    }
  ]
;





/******************************************************************************
  RECEIVE
******************************************************************************/

This=Receive(a,b) ->
  [petrinetsmall:
    {
      header(This->id);
      STCommunicationActivity* myself = dynamic_cast<STCommunicationActivity*> (symTab.lookup(This->id));
      string prefix = intToString(This->id->value) + ".internal.";

      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "final");

      Transition *t1 = TheNet->newTransition(prefix + "receive");
      TheNet->newArc(TheNet->findPlace("in." + string((dynamic_cast<STCommunicationActivity*>(symTab.lookup(This->id)))->channelId)), t1);
      TheNet->newArc(p1, t1);
      TheNet->newArc(t1, p2);
      t1->type = IN;
    }
    a // standardElements
    {
      footer(This->id);
    }
  ]
;





/******************************************************************************
  REPLY
******************************************************************************/

This=Reply(a,b) ->
  [petrinetsmall:
    {
      header(This->id);
      STCommunicationActivity* myself = dynamic_cast<STCommunicationActivity*> (symTab.lookup(This->id));
      string prefix = intToString(This->id->value) + ".internal.";

      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "final");

      Transition *t1 = TheNet->newTransition(prefix + "reply");
      TheNet->newArc(t1, TheNet->findPlace("out." + string((dynamic_cast<STCommunicationActivity*>(symTab.lookup(This->id)))->channelId)));
      TheNet->newArc(p1, t1);
      TheNet->newArc(t1, p2);
      t1->type = OUT;
    }
    a // standardElements
    {
      footer(This->id);
    }
  ]
;





/******************************************************************************
  ASSIGN
******************************************************************************/

This=Assign(a,b) ->
  [petrinetsmall:
    {
      header(This->id);
      string prefix = intToString(This->id->value) + ".internal.";

      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "final");

      Transition *t1 = TheNet->newTransition(prefix + "assign"); // dummy transition

      TheNet->newArc(p1, t1);
      TheNet->newArc(t1, p2);
    }
    a // standardElements
    {
      footer(This->id);
    }
  ]
;






/******************************************************************************
  WAIT
******************************************************************************/

This=WaitUntil(a) ->
  [petrinetsmall:
    {
      header(This->id);
      string prefix = intToString(This->id->value) + ".internal.";

      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "final");

      Transition *t1 = TheNet->newTransition(prefix + "wait");
      TheNet->newArc(p1, t1);
      TheNet->newArc(t1, p2);
    }
    a // standardElements
    {
      footer(This->id);
    }
  ]
;





This=WaitFor(a) ->
  [petrinetsmall:
    {
      header(This->id);
      string prefix = intToString(This->id->value) + ".internal.";

      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "final");

      Transition *t1 = TheNet->newTransition(prefix + "wait");
      TheNet->newArc(p1, t1);
      TheNet->newArc(t1, p2);
    }
    a // standardElements
    {
      footer(This->id);
    }
  ]
;





/******************************************************************************
  THROW
******************************************************************************/

This=Throw(a) ->
  [petrinetsmall:
    {
      header(This->id);
      cerr << "the <throw> activity cannot be modelled!" << endl;
      footer(This->id);
    }
  ]
;





/******************************************************************************
  COMPENSATE
******************************************************************************/

This=Compensate(a) ->
  [petrinetsmall:
    {
      header(This->id);
      cerr << "the <compensate> activity cannot be modelled!" << endl;
      footer(This->id);
    }
  ]
;





/******************************************************************************
  TERMINATE
******************************************************************************/

This=Terminate(a) ->
  [petrinetsmall:
    {
      header(This->id);
      cerr << "the <terminate> activity cannot be modelled!" << endl;
      footer(This->id);
    }
  ]
;






/******************************************************************************
  FLOW
******************************************************************************/

/*
 * The <flow> activity as it is depicted in Fig. 17.
 */

This=Flow(a,b,c) ->
  [petrinetsmall:
    {
      header(This->id, true);
      string prefix = intToString(This->id->value) + ".internal.";

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
      footer(This->id, true);
    }
  ]
;





/******************************************************************************
  SWITCH
******************************************************************************/

/*
 * The <switch> activity as it is depicted in Fig. 19. Please note that during
 * parsing an empty otherwise branch was added if it was not specified.
 */

This=activitySwitch(Switch(a,b,otherwiseBranch=Otherwise(c))) ->
  [petrinetsmall:
    {
      header(This->id, true);
      string prefix = intToString(This->id->value) + ".internal.";

      Place *p1 =  TheNet->newPlace(prefix + "final");
      Place *p2 =  TheNet->newPlace(prefix + "initial");
    }
    a // standardElements
    b // case braches
    c // otherwise branch
    {
      int currentCase = 1;
      foreach (caseBranch=Case(innerActivity); tCase_list b)
      {
	string caseNumber = intToString(currentCase++);

	trace(TRACE_VERY_DEBUG, "[PNU]\tEmbedding case branch " + caseNumber + "...\n");

	Transition* t1 = TheNet->newTransition(prefix + "case" + caseNumber);
	TheNet->newArc(TheNet->findPlace(This, ".initial"), t1);


	// Embed the inner activity.
	TheNet->newArc(t1, TheNet->findPlace(innerActivity, ".initial"));
	TheNet->mergePlaces(This, ".internal.final", innerActivity, ".final");
      }


      trace(TRACE_VERY_DEBUG, "[PNU]\tEmbedding otherwise branch...\n");
      // The <otherwise> branch.
      Transition *t1 = TheNet->newTransition(prefix + "caseo");
      TheNet->newArc(TheNet->findPlace(This, ".initial"), t1);


      // embed inner activity
      TheNet->newArc(t1, TheNet->findPlace(c, ".initial"));
      TheNet->mergePlaces(This, ".internal.final",   c, ".final");


      // Create the arcs for the Dead Path elimination: each chosen activity
      // (i.e. firing of the respective t2-transition) has set the links of all
      // other branches to false.
      currentCase = 1;
      foreach (thisCaseBranch=Case(innerActivity1); tCase_list b)
      {
        foreach (otherCaseBranch=Case(innerActivity2); tCase_list b)
	{
	  if (innerActivity1 != innerActivity2)
            dpeLinks(TheNet->findTransition(prefix + "case" + intToString(currentCase)), otherCaseBranch->id); // case -> case'
    	}
        dpeLinks(TheNet->findTransition(prefix + "case" + intToString(currentCase)), otherwiseBranch->id); // case -> otherwise
        dpeLinks(TheNet->findTransition(prefix + "caseo"), thisCaseBranch->id); // otherwise -> case
  	currentCase++;
      }

      footer(This->id, true);
    }
  ]
;





/******************************************************************************
  WHILE
******************************************************************************/

/*
 * The <while> activity as it is depicted in Fig. 18, but with two possible
 * scenarios:
 *
 *   - cyclic <while>: the standard case as it is depicted in Fig. 18.
 *   - acyclic <while>: a different case leading to an acyclic pattern.
 *
 * The scenario is selected by setting the parameter "-p cyclicwhile". For more
 * information refer to the comments below or the User's Manual.
 */

This=activityWhile(TheWhile=While(a,b)) ->
  [petrinetsmall:
    {
      header(This->id, true);
      string prefix = intToString(TheWhile->id->value) + ".internal.";
      string condition = string(TheWhile->condition->name);

      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "running");
      Place *p3 = TheNet->newPlace(prefix + "final");
      Place *p4 = TheNet->newPlace(prefix + "done");

      Transition *t1 = TheNet->newTransition(prefix + "choose");
      TheNet->newArc(p1, t1);
      TheNet->newArc(t1, p2);

      Transition *t2 = TheNet->newTransition(prefix + "skip");
      TheNet->newArc(p1, t2);
      TheNet->newArc(t2, p3);

      // When the parameter "-p cyclicwhile" is set, the pattern is created as
      // it is depicted in Fig. 18. Setting this option might lead to not
      // 1-safe Petri nets as links embedded in the inner activity might be
      // initialized more than once.
      if (parameters[P_CYCLICWHILE] == true)
      {
	Transition *t3 = TheNet->newTransition(prefix + "loop");
	TheNet->newArc(p4, t3);
	TheNet->newArc(t3, p1);
      }
      else
      {
	Transition *t3 = TheNet->newTransition(prefix + "finish");
	TheNet->newArc(p4, t3);
	TheNet->newArc(t3, p3);
      }
    }
    a // standardElements
    b // innerActivity
    {
      // Embed the inner activity.
      TheNet->mergePlaces(This, ".internal.running",      b, ".initial");
      TheNet->mergePlaces(This, ".internal.done",      b, ".final");

      footer(This->id, true);
    }
  ]
;





/******************************************************************************
  SEQUENCE
******************************************************************************/

/*
 * The <sequence> activity as it is depicted in Fig. 16.
 */

This=activitySequence(Sequence(a,b)) ->
  [petrinetsmall:
    {
      header(This->id, true);
      string prefix = intToString(This->id->value) + ".internal.";

      Place *p2  = TheNet->newPlace(prefix + "initial");
      Place *p9  = TheNet->newPlace(prefix + "final");
    }
    a // standardElements for this activity
    b // inner activities
    {
      int activityCount = 1;
      kc::impl_activity* lastActivity;

      foreach (innerActivity; activity_list b)
      {
	if (activityCount == 1)
	  TheNet->mergePlaces(This, ".internal.initial", innerActivity, ".initial");
	else
	  TheNet->mergePlaces(lastActivity, ".final", innerActivity, ".initial");

	if (activityCount == b->length())
	  TheNet->mergePlaces(This, ".internal.final", innerActivity, ".final");

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

/*
 * The <pick> activity as it is depicted in Fig. 20.
 */

TheActivity=activityPick(This=Pick(a,b,c)) ->
  [petrinetsmall:
    {
      header(This->id, true);
      string prefix = intToString(This->id->value) + ".internal.";

      Place *p5 = TheNet->newPlace(prefix + "final");
      Place *p6 = TheNet->newPlace(prefix + "initial");
    }
    a // standardElements
    b // onMessage branches
    c // onAlarm branches
    {
      // Traverse the <onMessage> branches.
      int onMessageCount = 1;
      foreach (ThisBranch = OnMessage(innerActivity); tOnMessage_list b)
      {
	trace(TRACE_DEBUG, "[PNU]\t\t<onMessage>\n");
	string message = intToString(onMessageCount++);
	string newPrefix = prefix + "onMessage" + message + ".";

	Transition *t2 = TheNet->newTransition(newPrefix + "t2");
	TheNet->newArc(TheNet->findPlace("in." + string((dynamic_cast<STCommunicationActivity*>(symTab.lookup(ThisBranch->id)))->channelId)), t2);
	TheNet->newArc(TheNet->findPlace(TheActivity, ".initial"), t2);
	TheNet->newArc(t2, TheNet->findPlace(innerActivity, ".initial"));
	t2->type = IN;


	// Embed the inner activity.
	TheNet->mergePlaces(TheActivity, ".final",   innerActivity, ".final");

	trace(TRACE_DEBUG, "[PNU]\t\t</onMessage>\n");
      }


      // Traverse the <onAlarm> branches.
      int onAlarmCount = 1;
      foreach (onAlarmBranch=OnAlarm(innerActivity); tOnAlarm_list c)
      {
	trace(TRACE_DEBUG, "[PNU]\t\t<onAlarm>\n");
	string newPrefix = prefix + "onAlarm" + intToString(onAlarmCount++) + ".";
	string onAlarmGuard;

	Transition *t6 = TheNet->newTransition(newPrefix + "t6");
	TheNet->newArc(TheNet->findPlace(TheActivity, ".initial"), t6);
	TheNet->newArc(t6, TheNet->findPlace(innerActivity, ".initial"));


	// Embed the inner activity.
	TheNet->mergePlaces(TheActivity, ".final",   innerActivity, ".final");

	trace(TRACE_DEBUG, "[PNU]\t\t</onAlarm>\n");
      }


      // Create the arcs for the Dead Path elimination: each chosen activity
      // (i.e. firing of the respective t2/t6-transition) has set the links of
      // all other branches to false.
      int currentMessage = 1;
      foreach (thisonmessage=OnMessage(innerActivity1); tOnMessage_list b)
      {
	foreach (otheronmessage=OnMessage(innerActivity2); tOnMessage_list b)
	{
	  if ((innerActivity1 != innerActivity2))
            dpeLinks(TheNet->findTransition(prefix + "onMessage" + intToString(currentMessage) + ".t2"), otheronmessage->id); // onMessage -> onMessage'
	}

	foreach (thisonalarm=OnAlarm(innerActivity2); tOnAlarm_list c)
	{
          dpeLinks(TheNet->findTransition(prefix + "onMessage" + intToString(currentMessage) + ".t2"), thisonalarm->id); // onMessage -> onAlarm
	}

	currentMessage++;
      }

      int currentAlarm = 1;
      foreach (thisonalarm=OnAlarm(innerActivity1); tOnAlarm_list c)
      {
	foreach (otheronalarm=OnAlarm(innerActivity2); tOnAlarm_list c)
	{
	  if ((innerActivity1 != innerActivity2))
            dpeLinks(TheNet->findTransition(prefix + "onAlarm" + intToString(currentAlarm) + ".t6"), otheronalarm->id); // onAlarm -> onAlarm'
	}

	foreach (thisonmessage=OnMessage(innerActivity2); tOnMessage_list b)
	{
          dpeLinks(TheNet->findTransition(prefix + "onAlarm" + intToString(currentAlarm) + ".t6"), thisonmessage->id); // onAlarm -> onMessage
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

/*
 * The <scope> activity as it is depicted in Fig. 27. Note that it contains
 * only one event handler since alarm-event handler and message-event handler
 * are merged.
 */

This=Scope(a,b,c,d,e,f,g) ->
  [petrinetsmall:
    {
      header(This->id, true);
      string prefix = intToString(This->id->value) + ".internal.";
      currentScope = prefix;
      STScope* myself = dynamic_cast<STScope*> (symTab.lookup(This->id));

      // Create push-places for the children of the scope.
      for (list< STScope * >::iterator child = myself->childScopes.begin(); child != myself->childScopes.end(); child++)
      {
	Place *push_child    = TheNet->newPlace( intToString((*child)->entryKey) + ".push");
	Place *notPush_child = TheNet->newPlace( intToString((*child)->entryKey) + ".!push");
      }


      // Create the internal places of the scope pattern.
      Place *p2  = TheNet->newPlace(prefix + "initial");
      Place *p3  = TheNet->newPlace(prefix + "ch_in");
      Place *p4  = TheNet->newPlace(prefix + "scopeCompensated");
      Place *p6  = TheNet->newPlace(prefix + "p6");
      Place *p7  = TheNet->newPlace(prefix + "upperTerminate");
      Place *p8  = TheNet->newPlace(prefix + "!Terminated");
      Place *p9  = TheNet->newPlace(prefix + "Terminated");
      Place *p10 = TheNet->newPlace(prefix + "Active");
      Place *p11 = TheNet->newPlace(prefix + "!Completed");
      Place *p12 = TheNet->newPlace(prefix + "Completed");
      Place *p13 = TheNet->newPlace(prefix + "!Compensated");
      Place *p14 = TheNet->newPlace(prefix + "!Active");
      Place *p15 = TheNet->newPlace(prefix + "Compensated");
      Place *p16 = TheNet->newPlace(prefix + "!Ended");
      Place *p17 = TheNet->newPlace(prefix + "Ended");
      Place *p18 = TheNet->newPlace(prefix + "!Faulted");
      Place *p19 = TheNet->newPlace(prefix + "Faulted");
      Place *p20 = TheNet->newPlace(prefix + "stop");
      Place *p21 = TheNet->newPlace(prefix + "p21");
      Place *p22 = TheNet->newPlace(prefix + "p22");
      Place *p23 = TheNet->newPlace(prefix + "p23");
      Place *p24 = TheNet->newPlace(prefix + "p24");
      Place *p35 = TheNet->newPlace(prefix + "p35");
      Place *p36 = TheNet->newPlace(prefix + "p36");
      Place *p37 = TheNet->newPlace(prefix + "p37");
      Place *p38 = TheNet->newPlace(prefix + "p38");
      Place *p39 = TheNet->newPlace(prefix + "p39");
      Place *p40 = TheNet->newPlace(prefix + "p40");
      Place *p41 = TheNet->newPlace(prefix + "p41");
      Place *p42 = TheNet->newPlace(prefix + "p42");
      Place *p43 = TheNet->newPlace(prefix + "p43");
      Place *p44 = TheNet->newPlace(prefix + "p44");
      Place *p45 = TheNet->newPlace(prefix + "stopped");
      Place *p46 = TheNet->newPlace(prefix + "p46");
      Place *p47 = TheNet->newPlace(prefix + "p47");
      Place *p48 = TheNet->newPlace(prefix + "p48");
      Place *p49 = TheNet->newPlace(prefix + "p49");
      Place *p50 = TheNet->newPlace(prefix + "p50");
      Place *p51 = TheNet->newPlace(prefix + "final");
      Place *p52 = TheNet->newPlace(prefix + "p52");
      Place *p53 = TheNet->newPlace(prefix + "p53");
      Place *p54 = TheNet->newPlace(prefix + "p54");
      Place *p56 = TheNet->newPlace(prefix + "compensated");


      // State places of the scope modeling the new fault model: The places
      // guarantee that the occurrence of more than one fault inside the fault
      // or compensation handler is prevented. The places are used as guards
      // by the transitions signalling faults.
      Place *fh_faulted = TheNet->newPlace(prefix + "FHFaulted");
      Place *fh_not_faulted = TheNet->newPlace(prefix + "!FHFaulted");
      Place *ch_faulted = TheNet->newPlace(prefix + "CHFaulted");
      Place *ch_not_faulted = TheNet->newPlace(prefix + "!CHFaulted");


      // The transitions of the process pattern.
      Transition *t2 = TheNet->newTransition(prefix + "t2");
      TheNet->newArc(p2, t2);
      TheNet->newArc(t2, p10);
      TheNet->newArc(t2, p11);
      TheNet->newArc(t2, p13);
      TheNet->newArc(t2, p16);
      TheNet->newArc(t2, p18);
      TheNet->newArc(t2, p21);
      TheNet->newArc(t2, fh_not_faulted);

      Transition *t4 = TheNet->newTransition(prefix + "t4");
      TheNet->newArc(p10, t4);
      TheNet->newArc(p11, t4);
      TheNet->newArc(t4, p12);
      TheNet->newArc(t4, p14);
      TheNet->newArc(TheNet->findPlace(intToString(This->id->value) + ".!push"), t4);
      TheNet->newArc(t4, TheNet->findPlace(intToString(This->id->value) + ".push"));
      TheNet->newArc(p35, t4);
      TheNet->newArc(t4, p51);

      Transition *t7 = TheNet->newTransition(prefix + "t7");
      TheNet->newArc(p8, t7, READ);
      TheNet->newArc(p10, t7);
      TheNet->newArc(t7, p14);
      TheNet->newArc(p20, t7);
      TheNet->newArc(t7, p36);

      Transition *t8 = TheNet->newTransition(prefix + "t8");
      TheNet->newArc(p9, t8, READ);
      TheNet->newArc(p10, t8);
      TheNet->newArc(t8, p14);
      TheNet->newArc(p20, t8);
      TheNet->newArc(t8, p37);

      Transition *t16 = TheNet->newTransition(prefix + "t16");
      TheNet->newArc(p53, t16);


      // Transitions for the negative control flow.
      Transition *t9  = stop(p2, "initial", prefix);
      Transition *t10 = stop(p17, "Ended", prefix);
      TheNet->newArc(t10, p17); // p17 is only read
      Transition *t11 = stop(p51, "final", prefix);


      // If the process has an event handler several places and transitions
      // have to be added to embed it. If the process lacks an event handler
      // some places can be merged.
      if (myself->hasEventHandler)
      {
	Place *p25 = TheNet->newPlace(prefix + "p25");
	Place *p26 = TheNet->newPlace(prefix + "p26");
	Place *p27 = TheNet->newPlace(prefix + "p27");
	Place *p28 = TheNet->newPlace(prefix + "p28");
	Place *p29 = TheNet->newPlace(prefix + "p29");

  	TheNet->newArc(t2, p25);
	TheNet->newArc(p27, t4);

      	Transition *t1 = TheNet->newTransition(prefix + "t1");
    	TheNet->newArc(p29, t1);
  	TheNet->newArc(t1, p38);

	Transition *t3 = TheNet->newTransition(prefix + "t3");
      	TheNet->newArc(p22, t3);
    	TheNet->newArc(t3, p26);
  	TheNet->newArc(t3, p35);

	Transition *t5 = TheNet->newTransition(prefix + "t5");
	TheNet->newArc(p24, t5);
	TheNet->newArc(t5, p28);

	Transition *t6 = TheNet->newTransition(prefix + "t6");
	TheNet->newArc(p23, t6);
	TheNet->newArc(t6, p28);
	TheNet->newArc(p35, t6);
      }
      else
      {
	TheNet->mergePlaces(p22, p35);
	TheNet->mergePlaces(p24, p38);
      }

      // New transitions to pass fault to parent.
      Transition *t12 = TheNet->newTransition(prefix + "passFaultUp");
      TheNet->newArc(p48, t12);
      TheNet->newArc(TheNet->findPlace(intToString(This->parentScopeId->value) + ".internal.Active"), t12);
      TheNet->newArc(t12, TheNet->findPlace(intToString(This->parentScopeId->value) + ".internal.!Active")); //newer fault management
      TheNet->newArc(t12, TheNet->findPlace(intToString(This->parentScopeId->value) + ".internal.stop.fault_in"));

      Transition *t13 = TheNet->newTransition(prefix + "ignoreFault");
      TheNet->newArc(p48, t13);
      TheNet->newArc(TheNet->findPlace(intToString(This->parentScopeId->value) + ".internal.!Active"), t13, READ);


      // Merge places with parent scope.
      TheNet->mergePlaces(TheNet->findPlace(intToString(This->parentScopeId->value) +
	    ".internal.compensationHandler.compScope." + intToString(This->id->value)), p3); // was "....internal.compScope."
      TheNet->mergePlaces(TheNet->findPlace(intToString(This->parentScopeId->value) + ".internal.scopeCompensated"), p56);
      TheNet->mergePlaces(TheNet->findPlace(intToString(This->parentScopeId->value) + ".internal.upperTerminate"), p46);
    }
    f // Stop Pattern
    a // standardElements
    c // Fault Handler
    d // Compensation Handler
    e // Event Handler
    g // innerActivity
    {
      // After the patterns of the inner activity of the process are generated
      // the push places of the child scopes are available and have to be
      // connected with the transition t2 of the process.
      for (list<STScope*>::iterator child = myself->childScopes.begin(); child != myself->childScopes.end(); child++)
	TheNet->newArc(t2, TheNet->findPlace( intToString((*child)->entryKey) + ".!push" ));

      string prefix2 = intToString(This->id->value) + ".internal.";


      // Embed the fault handler of the scope.
      TheNet->mergePlaces(prefix + "p43",     prefix2 + "faultHandler.fault");
      TheNet->mergePlaces(prefix + "p44",     prefix2 + "faultHandler.faultSave");
      TheNet->mergePlaces(prefix + "p47",     prefix2 + "faultHandler.rethrow");
      TheNet->mergePlaces(prefix + "p41",     prefix2 + "faultHandler.ft");
      TheNet->mergePlaces(prefix + "p42",     prefix2 + "faultHandler.ftSave");
      TheNet->mergePlaces(prefix + "!Ended",  prefix2 + "faultHandler.!Ended");
      TheNet->mergePlaces(prefix + "Ended",   prefix2 + "faultHandler.Ended");
      TheNet->mergePlaces(prefix + "p49",     prefix2 + "faultHandler.ch_fh");
      TheNet->mergePlaces(prefix + "p50",     prefix2 + "faultHandler.ch_out");
      TheNet->mergePlaces(prefix + "final",   prefix2 + "faultHandler.out");
      TheNet->mergePlaces(prefix + "p52",     prefix2 + "faultHandler.trueOut");
      TheNet->mergePlaces(prefix + "p53",     prefix2 + "faultHandler.sourceFalse");
      TheNet->mergePlaces(prefix + "p54",     prefix2 + "faultHandler.falseOut");
      TheNet->mergePlaces(prefix + "p48",     prefix2 + "faultHandler.upperFH");
      TheNet->mergePlaces(prefix + "stopped", prefix2 + "faultHandler.stopped");


      // Embed the compensation handler of the scope.
      TheNet->mergePlaces(prefix + "ch_in",            prefix2 + "compensationHandler.ch_in");
      TheNet->mergePlaces(prefix + "Completed",        prefix2 + "compensationHandler.Completed");
      TheNet->mergePlaces(prefix + "!Completed",       prefix2 + "compensationHandler.!Completed");
      TheNet->mergePlaces(prefix + "Compensated",      prefix2 + "compensationHandler.Compensated");
      TheNet->mergePlaces(prefix + "!Compensated",     prefix2 + "compensationHandler.!Compensated");
      TheNet->mergePlaces(prefix + "p49",              prefix2 + "compensationHandler.fh");
      TheNet->mergePlaces(prefix + "scopeCompensated", prefix2 + "compensationHandler.scopeCompensated");
      TheNet->mergePlaces(prefix + "p39",              prefix2 + "compensationHandler.clean");
      TheNet->mergePlaces(prefix + "p40",              prefix2 + "compensationHandler.cleaned");
      TheNet->mergePlaces(prefix + "p50",              prefix2 + "compensationHandler.ch_out");
      TheNet->mergePlaces(prefix + "compensated",      prefix2 + "compensationHandler.compensated");


      // If present, embed the event handler of the scope.
      if (myself->hasEventHandler)
      {
	TheNet->mergePlaces(prefix + "p25", prefix2 + "eventHandler.initial");
	TheNet->mergePlaces(prefix + "p26", prefix2 + "eventHandler.finish");
	TheNet->mergePlaces(prefix + "p27", prefix2 + "eventHandler.final");
	TheNet->mergePlaces(prefix + "p28", prefix2 + "eventHandler.stop");
	TheNet->mergePlaces(prefix + "p29", prefix2 + "eventHandler.stopped");
      }


      // Embed the stop-pattern of the scope.
      TheNet->mergePlaces(prefix + "p36",            prefix + "stop.ft_in");
      TheNet->mergePlaces(prefix + "Faulted",        prefix + "stop.Faulted");
      TheNet->mergePlaces(prefix + "p6",             prefix + "stop.fault_in");
      TheNet->mergePlaces(prefix + "Active",         prefix + "stop.Active");
      TheNet->mergePlaces(prefix + "p37",            prefix + "stop.terminate_up");
      TheNet->mergePlaces(prefix + "!Active",        prefix + "stop.!Active");
      TheNet->mergePlaces(prefix + "upperTerminate", prefix + "stop.terminate");
      TheNet->mergePlaces(prefix + "!Faulted",       prefix + "stop.!Faulted");
      TheNet->mergePlaces(prefix + "Ended",          prefix + "stop.Ended");
      TheNet->mergePlaces(prefix + "Compensated",    prefix + "stop.Compensated");
      TheNet->mergePlaces(prefix + "!Ended",         prefix + "stop.!Ended");
      TheNet->mergePlaces(prefix + "p23",            prefix + "stop.stop");
      TheNet->mergePlaces(prefix + "p38",            prefix + "stop.stopped");
      TheNet->mergePlaces(prefix + "p39",            prefix + "stop.cleanCH");
      TheNet->mergePlaces(prefix + "p40",            prefix + "stop.ch_cleaned");
      TheNet->mergePlaces(prefix + "p41",            prefix + "stop.ft");
      TheNet->mergePlaces(prefix + "p42",            prefix + "stop.ftSave");
      TheNet->mergePlaces(prefix + "p43",            prefix + "stop.fault");
      TheNet->mergePlaces(prefix + "p44",            prefix + "stop.faultSave");
      TheNet->mergePlaces(prefix + "stopped",        prefix + "stop.stopped_s");
      TheNet->mergePlaces(prefix + "p46",            prefix + "stop.upperTerminate");
      TheNet->mergePlaces(prefix + "p47",            prefix + "stop.rethrow");
      TheNet->mergePlaces(prefix + "p48",            prefix + "stop.upperFH");

      TheNet->mergePlaces(prefix + "FHFaulted",      prefix + "stop.FHFaulted");
      TheNet->mergePlaces(prefix + "!FHFaulted",     prefix + "stop.!FHFaulted");
      TheNet->mergePlaces(prefix + "CHFaulted",      prefix + "stop.CHFaulted");
      TheNet->mergePlaces(prefix + "!CHFaulted",     prefix + "stop.!CHFaulted");



      // Embed the inner activity of the scope.
      TheNet->mergePlaces(TheNet->findPlace(prefix + "p21"), TheNet->findPlace(g, ".initial"));
      TheNet->mergePlaces(TheNet->findPlace(prefix + "p22"), TheNet->findPlace(g, ".final"));
      TheNet->mergePlaces(TheNet->findPlace(prefix + "p23"), TheNet->findPlace(g, ".stop"));
      TheNet->mergePlaces(TheNet->findPlace(prefix + "p24"), TheNet->findPlace(g, ".stopped"));


      // Merge (!)Terminated places
      TheNet->mergePlaces(TheNet->findPlace("1.internal.!Terminated"), p8);
      TheNet->mergePlaces(TheNet->findPlace("1.internal.Terminated"), p9);


      // Setting all enclosed links to false in case of an error.
      for (list<STLink *>::iterator link = myself->enclosedLinks.begin(); link != myself->enclosedLinks.end(); link++)
      {
	Place *p61 = TheNet->newPlace(prefix + "p61.link." + (*link)->name);
	Place *p62 = TheNet->findPlace("link." + (*link)->name);
	Place *p63 = TheNet->findPlace("!link." + (*link)->name);
	Transition *t14 = TheNet->newTransition(prefix + "t14.link." + (*link)->name);
	Transition *t15 = TheNet->newTransition(prefix + "t15.link." + (*link)->name);
	TheNet->newArc(p61, t14);
	TheNet->newArc(t14, p62);
	TheNet->newArc(p63, t14);
	TheNet->newArc(p61, t15);
	TheNet->newArc(t16, p61);
	TheNet->newArc(p62, t15, READ);
      }

      currentScope = intToString(This->parentScopeId->value) + ".internal.";
      footer(This->id, true);
    }
  ]
;





/******************************************************************************
  STANDARD ELEMENTS
******************************************************************************/

This=StandardElements(NiltTarget_list(), NiltSource_list(), jc) ->
  [petrinetsmall:
    {
      string prefix = intToString(This->parentId->value) + ".";
      string newPrefix = intToString(This->parentId->value) + ".internal";

      Place *p2 = TheNet->newPlace(prefix + "initial");
      Place *p3 = TheNet->newPlace(prefix + "final");

      // Embed the inner activity.
      TheNet->mergePlaces(p2, TheNet->findPlace(newPrefix + ".initial"));
      TheNet->mergePlaces(p3, TheNet->findPlace(newPrefix + ".final"));
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
      string prefix = intToString(This->parentId->value) + ".";
      string newPrefix = intToString(This->parentId->value) + ".internal";

      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "done");
      Place *p3 = TheNet->newPlace(prefix + "final");

      Transition *t1 = TheNet->newTransition(prefix + "setLinks");
      TheNet->newArc(p2, t1);
      TheNet->newArc(t1, p3);

      // Set the outgoing links (standard transition condition expected).
      foreach (link; tSource_list a)
      {
	STLink *linkST = (dynamic_cast<STSourceTarget*> (symTab.lookup(link->id)))->link;
	Place *p4 = TheNet->findPlace("link." + string(linkST->name));
        TheNet->newArc(t1, p4);
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

This=StandardElements(a, NiltSource_list(), jc) provided 
(symTab.readAttributeValue(This->parentId, "suppressJoinFailure") == "no") ->
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

This=StandardElements(a, NiltSource_list(), jc) provided
(symTab.readAttributeValue(This->parentId, "suppressJoinFailure") == "yes") ->
  [petrinetsmall:
    {
      trace(TRACE_DEBUG, "[PNU]" + inString() + "Fig. 24 (new)\n");
      string prefix = intToString(This->parentId->value) + ".";
      string newPrefix = intToString(This->parentId->value) + ".internal";

      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "final");
      Place *p3 = TheNet->newPlace(prefix + "joinTrue");
      Place *p4 = TheNet->newPlace(prefix + "joinFalse");
      Place *p5 = TheNet->newPlace(prefix + "begin");

      Transition *t1 = TheNet->newTransition(prefix + "t1");
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
	foreach (link; tTarget_list a)
	{
	  STLink *linkST = (dynamic_cast<STSourceTarget*> (symTab.lookup(link->id)))->link;
	  Place *linkT = TheNet->findPlace("link." + string(linkST->name));
	  Place *linkF = TheNet->findPlace("!link." + string(linkST->name));

          trueLinks[linkCount] = linkT;
          falseLinks[linkCount] = linkF;

          linkCount++;
	}


        // fill transition container and create OR-join
        unsigned int transitionNumber = (unsigned int)pow(2.0, a->length());
        for (unsigned int i = 0; i < transitionNumber; i++)
        {
          joinTransitions[i] = TheNet->newTransition(prefix + "join" + intToString(i));

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

This=StandardElements(a,b, jc) provided 
(symTab.readAttributeValue(This->parentId, "suppressJoinFailure") == "yes") ->
  [petrinetsmall:
    {
      trace(TRACE_DEBUG, "[PNU]" + inString() + "Fig. 25 (new)\n");
      string prefix = intToString(This->parentId->value) + ".";
      string newPrefix = intToString(This->parentId->value) + ".internal";

      Place *p1 = TheNet->newPlace(prefix + "initial");
      Place *p2 = TheNet->newPlace(prefix + "begin");
      Place *p3 = TheNet->newPlace(prefix + "done");
      Place *p4 = TheNet->newPlace(prefix + "final");
      Place *p5 = TheNet->newPlace(prefix + "joinFalse");
      Place *p6 = TheNet->newPlace(prefix + "joinTrue");

      Transition *t1 = TheNet->newTransition(prefix + "t1");
      TheNet->newArc(p1, t1);
      TheNet->newArc(p6, t1);
      TheNet->newArc(t1, p2);

      Transition *t2 = TheNet->newTransition(prefix + "skip");
      TheNet->newArc(p1, t2);
      TheNet->newArc(p5, t2);
      TheNet->newArc(t2, p4);

      // sets enclosed source links to false
      dpeLinks(t2, parentId);

      Transition *t3 = TheNet->newTransition(prefix + "setLinks");
      TheNet->newArc(p3, t3);
      TheNet->newArc(t3, p4);

      
      // Set the outgoing links (standard transition condition expected).
      foreach (link; tSource_list b)
      {
	STLink *linkST = (dynamic_cast<STSourceTarget*> (symTab.lookup(link->id)))->link;
	Place *p7 = TheNet->findPlace("link." + string(linkST->name));
        TheNet->newArc(t3, p7);
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
	foreach (link; tTarget_list a)
	{
	  STLink *linkST = (dynamic_cast<STSourceTarget*> (symTab.lookup(link->id)))->link;
	  Place *linkT = TheNet->findPlace("link." + string(linkST->name));
	  Place *linkF = TheNet->findPlace("!link." + string(linkST->name));

          trueLinks[linkCount] = linkT;
          falseLinks[linkCount] = linkF;

          linkCount++;
	}


        // fill transition container and create OR-join
        unsigned int transitionNumber = (unsigned int)pow(2.0, a->length());
        for (unsigned int i = 0; i < transitionNumber; i++)
        {
          joinTransitions[i] = TheNet->newTransition(prefix + "join" + intToString(i));

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

This=StandardElements(a,b, jc) provided 
(symTab.readAttributeValue(This->parentId, "suppressJoinFailure") == "no") ->
  [petrinetsmall:
    {
      trace(TRACE_DEBUG, "[PNU]" + inString() + "Fig. 26\n");
      cerr << "no patterns allowed with suppressJoinFailure=\"no\"" << endl;
    }
  ]
;
