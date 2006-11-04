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
 * \file    ast-postprocess.cc
 *
 * \author  responsible: Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          last changes of: \$Author: nlohmann $
 * 
 * \since   2006-07-02
 *
 * \date    \$Date: 2006/11/04 16:58:05 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \note    This file was created using Kimwitu++ reading file bpel-unparse.k.
 *          See http://site.informatik.hu-berlin.de/kimwitu++ for details.
 *
 * \version \$Revision: 1.78 $
 */
%}





/******************************************************************************
  PROCESS
******************************************************************************/

Process(a,b,c,d,e,f,g,h,i,_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      assert(ASTEmap[myid] != NULL);

      ASTEmap[myid]->parentScopeId = myid;
      ASTEmap[myid]->parentActivityId = myid;
      PPcurrentScope = myid;

      // annotate AST and check information
      ASTEmap[myid]->checkAttributes();
      ASTEmap[myid]->hasEH = (string(g->op_name()) == "userDefinedEventHandler");
      ASTEmap[f->id]->inProcess = true;

      // distribute the process' id to the enclosed FH/CH/EH/activity
      assert(ASTEmap[e->id] != NULL);
      assert(ASTEmap[f->id] != NULL);
      assert(ASTEmap[g->id] != NULL);
      assert(ASTEmap[i->id] != NULL);
      ASTEmap[e->id]->parentActivityId = myid;
      ASTEmap[f->id]->parentActivityId = myid;
      ASTEmap[g->id]->parentActivityId = myid;
      ASTEmap[i->id]->parentActivityId = myid;
    }
    i // inner activity
    a b c d e f g h
    {
      // trigger [SA00015]
      if (startActivities.empty())
        SAerror(15, "", ASTEmap[myid]->attributes["referenceLine"]);

      // collect enclosed source links
      assert(ASTEmap[i->id] != NULL);
      ASTEmap[myid]->enclosedSourceLinks = ASTEmap[i->id]->enclosedSourceLinks;

      // add an attribute to the compensation handler
      if(PPhasCompensateMap[myid] == 1)
        f->hasCompensateWithoutScope = true;
      if(PPhasCompensateMap[myid] == 2)
        f->hasCompensateWithScope = true;

      // pass list of enclosed scopes to the compensation handler
      ASTEmap[f->id]->enclosedScopes = ASTEmap[myid]->enclosedScopes;


      // this assertion guarantees that we did not forget a link and encounter
      // problems generating the Petri net
      assert(ASTEmap[myid]->enclosedSourceLinks.size() == ASTE_links.size());

//      cerr << myid << " has " << ASTEmap[myid]->enclosedSourceLinks.size() << " indirectly enclosed source links" << endl;
    }
  ]
;


/******************************************************************************
  PARTNER LINKS
******************************************************************************/

PartnerLink(_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      assert(ASTEmap[myid] != NULL);

      ASTEmap[myid]->parentScopeId = PPcurrentScope;
      ASTEmap[myid]->checkAttributes();
    }
  ]
;


/******************************************************************************
  PARTNERS
******************************************************************************/

Partner(a,_id) ->
  [postprocessing:
    {
      // annotate all enclosed partner links
      foreach(partner; tPartnerLink_list a)
      {
//        partner->inPartners = true;
      }
    }
    a
  ]
;


/******************************************************************************
  FAULT HANDLERS
******************************************************************************/

userDefinedFaultHandler(a,b,_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      assert(ASTEmap[myid] != NULL);

      ASTEmap[myid]->parentScopeId = PPcurrentScope;
      ASTEmap[myid]->hasCatchAll = (b->op_name() == "CatchAll");
      ASTEmap[myid]->inProcess = (ASTEmap[myid]->parentScopeId == 1);

      // distribute the fault handlers's id to the enclosed catch/catchAll branches
      // also mark the activities as "inside fault handlers"
      foreach(branch=Catch(innerActivity); tCatch_list a)
      {
        assert(ASTEmap[innerActivity->id] != NULL);
        ASTEmap[innerActivity->id]->parentActivityId = myid;
        ASTEmap[innerActivity->id]->controlFlow = FHCF;
      }

      // check whether a <catchAll>-branch with an enclosed activity is present
      // and if yes, annotate it
      with (b)
      {
        CatchAll(c,_id) :
        {
          assert(ASTEmap[c->id] != NULL);
          ASTEmap[c->id]->parentActivityId = myid;
          ASTEmap[c->id]->controlFlow = FHCF;
        }
        default :
        {
          // triggers SA00080
          if (a->length() == 0)
            SAerror(80, "", toInt(ASTEmap[ASTEmap[myid]->parentScopeId]->attributes["referenceLine"]));
        }
      }
    }
    a b
  ]
;

implicitFaultHandler(_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      assert(ASTEmap[myid] != NULL);

      ASTEmap[myid]->parentScopeId = PPcurrentScope;
      ASTEmap[myid]->inProcess = (ASTEmap[myid]->parentScopeId == 1);
    }
  ]
;


Catch(a,_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      assert(ASTEmap[myid] != NULL);

      ASTEmap[myid]->checkAttributes();
    }
    a
  ]
;

CatchAll(a,_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      assert(ASTEmap[myid] != NULL);
    }
    a
  ]
;


/******************************************************************************
  EVENT HANDLERS
******************************************************************************/

implicitEventHandler(_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      assert(ASTEmap[myid] != NULL);

      ASTEmap[myid]->parentScopeId = PPcurrentScope;
    }  
  ]
;

userDefinedEventHandler(a,b,_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      assert(ASTEmap[myid] != NULL);

      ASTEmap[myid]->parentScopeId = PPcurrentScope;
      ASTEmap[myid]->inWhile = true;

      // triggers SA00083
      if (a->length() == 0 && b->length() == 0)
        SAerror(83, "", toInt(ASTEmap[ASTEmap[myid]->parentScopeId]->attributes["referenceLine"]));

      // distribute the event handlers's id to the enclosed activities
      foreach(branch=OnMessage(innerActivity); tOnMessage_list a)
      {
        assert(ASTEmap[innerActivity->id] != NULL);
        ASTEmap[innerActivity->id]->parentActivityId = myid;
        ASTEmap[branch->id]->parentActivityId = myid;
        ASTEmap[innerActivity->id]->controlFlow = ASTEmap[myid]->controlFlow;
      }
      foreach(branch=OnAlarm(innerActivity); tOnAlarm_list b)
      {
        assert(ASTEmap[innerActivity->id] != NULL);
        ASTEmap[innerActivity->id]->parentActivityId = myid;
        ASTEmap[branch->id]->parentActivityId = myid;
        ASTEmap[innerActivity->id]->controlFlow = ASTEmap[myid]->controlFlow;
      }
    }
    a b
  ]
;

OnMessage(a,b,c,_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      assert(ASTEmap[myid] != NULL);
      ASTE *myself = ASTEmap[myid];

      assert(ASTEmap[myself->parentActivityId] != NULL);
      myself->inWhile = ASTEmap[myself->parentActivityId]->inWhile;
      myself->checkAttributes();

      // trigger [SA00063]
      if (b->length() != 0 && myself->attributes["variable"] != "")
        SAerror(63, "", myself->attributes["referenceLine"]);

      myself->channelName = myself->createChannel();
      myself->variableName = myself->checkVariable();
    }
    c
  ]
;

OnAlarm(a,b) ->
  [postprocessing: a]
;


/******************************************************************************
  COMPENSATION HANDLERS
******************************************************************************/

implicitCompensationHandler(_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      assert(ASTEmap[myid] != NULL);

      ASTEmap[myid]->parentScopeId = PPcurrentScope;
    }
  ]
;

userDefinedCompensationHandler(a,_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      assert(ASTEmap[myid] != NULL);

      ASTEmap[myid]->parentScopeId = PPcurrentScope;

      // distribute the compensation handlers's id to the enclosed activity
      // also mark the activities as "inside compensation handlers"
      assert(ASTEmap[a->id] != NULL);
      ASTEmap[a->id]->parentActivityId = myid;
      ASTEmap[a->id]->controlFlow = CHCF;
    }
    a
  ]
;


/******************************************************************************
  TERMINATION HANDLER                                            (WS-BPEL 2.0)
******************************************************************************/

TerminationHandler(a,_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      assert(ASTEmap[myid] != NULL);

      ASTEmap[myid]->parentScopeId = PPcurrentScope;

      // distribute the compensation handlers's id to the enclosed activity
      // also mark the activities as "inside compensation handlers"
      assert(ASTEmap[a->id] != NULL);
      ASTEmap[a->id]->parentActivityId = myid;
      ASTEmap[a->id]->controlFlow = THCF;
    }
    a
  ]
;





/******************************************************************************
  VARIABLES
******************************************************************************/

Variable(_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      assert(ASTEmap[myid] != NULL);

      ASTEmap[myid]->parentScopeId = PPcurrentScope;
      ASTEmap[myid]->checkAttributes();
      ASTEmap[myid]->defineVariable();
    }
  ]
;


/******************************************************************************
  CORRELATION SETS
******************************************************************************/

CorrelationSet(_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      assert(ASTEmap[myid] != NULL);

      ASTEmap[myid]->checkAttributes();
     }
  ]
;


/******************************************************************************
  CORRELATIONS
******************************************************************************/

Correlation(_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      assert(ASTEmap[myid] != NULL);

      ASTEmap[myid]->checkAttributes();
     }
  ]
;


/******************************************************************************
  TO & FROM PARTS                                                (WS-BPEL 2.0)
******************************************************************************/

ToPart(_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      assert(ASTEmap[myid] != NULL);

      ASTEmap[myid]->checkAttributes();
     }
  ]
;

FromPart(_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      assert(ASTEmap[myid] != NULL);

      ASTEmap[myid]->checkAttributes();
     }
  ]
;


/******************************************************************************
  EMPTY
******************************************************************************/

Empty(a,_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      assert(ASTEmap[myid] != NULL);

      a->parentId = myid;

      ASTEmap[myid]->parentScopeId = PPcurrentScope;
      ASTEmap[myid]->checkAttributes();
    }
    a
  ]
;


/******************************************************************************
  INVOKE
******************************************************************************/

Invoke(a,b,c,d,_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      assert(ASTEmap[myid] != NULL);
      ASTE *myself = ASTEmap[myid];

      a->parentId = myid;

      myself->parentScopeId = PPcurrentScope;
      myself->checkAttributes();

      /* above is generic */

      // trigger [SA00051]
      if (c->length() != 0 && myself->attributes["inputVariable"] != "")
        SAerror(51, "", myself->attributes["referenceLine"]);

      // trigger [SA00052]
      if (d->length() != 0 && myself->attributes["outputVariable"] != "")
        SAerror(52, "", myself->attributes["referenceLine"]);

      myself->channelName = myself->createChannel((myself->attributes["outputVariable"] != ""));
      myself->inputVariableName = myself->checkInputVariable();
      myself->outputVariableName = myself->checkOutputVariable();
      myself->inWhile = ASTEmap[myself->parentActivityId]->inWhile;
    }
    a b
  ]
;


/******************************************************************************
  RECEIVE
******************************************************************************/

Receive(a,b,c,_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      assert(ASTEmap[myid] != NULL);
      ASTE *myself = ASTEmap[myid];

      a->parentId = myid;

      myself->parentScopeId = PPcurrentScope;
      myself->checkAttributes();

      /* above is generic */

      // trigger [SA00055]
      if (c->length() != 0 && myself->attributes["variable"] != "")
        SAerror(55, "", myself->attributes["referenceLine"]);

      myself->channelName = myself->createChannel();
      myself->variableName = myself->checkVariable();
      myself->inWhile = ASTEmap[myself->parentActivityId]->inWhile;
    }
    a b
    {
      if (myself->isStartActivity)
      {
        startActivities.insert(myid);

        // trigger [SA00056]
        if (!ASTEmap[myself->parentActivityId]->checkAncestors() || myself->hasTargetLink)
          SAerror(56, "<receive>", myself->attributes["referenceLine"]);
      }
    }
  ]
;


/******************************************************************************
  REPLY
******************************************************************************/

Reply(a,b,_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      assert(ASTEmap[myid] != NULL);

      a->parentId = myid;

      ASTEmap[myid]->parentScopeId = PPcurrentScope;
      ASTEmap[myid]->checkAttributes();

      /* above is generic */

      ASTEmap[myid]->channelName = ASTEmap[myid]->createChannel();
      ASTEmap[myid]->variableName = ASTEmap[myid]->checkVariable();
      ASTEmap[myid]->inWhile = ASTEmap[ASTEmap[myid]->parentActivityId]->inWhile;
    }
    a b
  ]
;


/******************************************************************************
  ASSIGN
******************************************************************************/

Assign(a,b,_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      assert(ASTEmap[myid] != NULL);

      a->parentId = myid;

      ASTEmap[myid]->parentScopeId = PPcurrentScope;
      ASTEmap[myid]->checkAttributes();
    }
    a b
  ]
;

Copy(a,b,c) ->
  [postprocessing: a b]
;

From(_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      assert(ASTEmap[myid] != NULL);

      ASTEmap[myid]->checkAttributes();
      ASTEmap[myid]->variableName = ASTEmap[myid]->checkVariable();
    }
  ]
;

To(_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      assert(ASTEmap[myid] != NULL);

      ASTEmap[myid]->checkAttributes();
      ASTEmap[myid]->variableName = ASTEmap[myid]->checkVariable();
    }
  ]
;


/******************************************************************************
  VALIDATE                                                       (WS-BPEL 2.0)
******************************************************************************/

Validate(a,_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      assert(ASTEmap[myid] != NULL);

      a->parentId = myid;

      ASTEmap[myid]->parentScopeId = PPcurrentScope;
      ASTEmap[myid]->checkAttributes();
    }
    a
  ]
;


/******************************************************************************
  WAIT
******************************************************************************/

Wait(a,_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      assert(ASTEmap[myid] != NULL);

      a->parentId = myid;

      ASTEmap[myid]->parentScopeId = PPcurrentScope;
      ASTEmap[myid]->checkAttributes();
    }
    a
  ]
;


/******************************************************************************
  THROW
******************************************************************************/

Throw(a,_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      assert(ASTEmap[myid] != NULL);

      a->parentId = myid;

      ASTEmap[myid]->parentScopeId = PPcurrentScope;
      ASTEmap[myid]->checkAttributes();
    }
    a
  ]
;


/******************************************************************************
  RETHROW                                                        (WS-BPEL 2.0)
******************************************************************************/

Rethrow(a,_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      assert(ASTEmap[myid] != NULL);

      a->parentId = myid;

      ASTEmap[myid]->parentScopeId = PPcurrentScope;
      ASTEmap[myid]->checkAttributes();
    }
    a
  ]
;


/******************************************************************************
  COMPENSATE
******************************************************************************/

Compensate(a,_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      assert(ASTEmap[myid] != NULL);

      a->parentId = myid;

      ASTEmap[myid]->parentScopeId = PPcurrentScope;
      ASTEmap[myid]->checkAttributes();

      /* above is generic */

      // register this compensate at the current scope
      /* here we can check whether it is the only compensate activity */
      PPhasCompensateMap[PPcurrentScope] = 1;
    }
    a
  ]
;


/******************************************************************************
  COMPENSATE SCOPE                                               (WS-BPEL 2.0)
******************************************************************************/

CompensateScope(a,b,_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      assert(ASTEmap[myid] != NULL);
      ASTE *myself = ASTEmap[myid];

      a->parentId = myid;

      myself->parentScopeId = PPcurrentScope;
      myself->checkAttributes();

      /* above is generic */

      // triggers SA00078 and SA00077
      if (scopeNames[myself->attributes["target"]] == 0)
        SAerror(78, myself->attributes["target"], myself->attributes["referenceLine"]);
      else if (ASTEmap[myself->parentScopeId]->enclosedScopes.find(scopeNames[myself->attributes["target"]]) == ASTEmap[myself->parentScopeId]->enclosedScopes.end())
        SAerror(77, myself->attributes["target"], myself->attributes["referenceLine"]);

      // register this compensate at the current scope
      /* here we can check whether it is the only compensate activity */
      PPhasCompensateMap[PPcurrentScope] = 2;
    }
    a
  ]
;


/******************************************************************************
  TERMINATE                                                      (BPEL4WS 1.1)
******************************************************************************/

Terminate(a,_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      assert(ASTEmap[myid] != NULL);

      a->parentId = myid;

      ASTEmap[myid]->parentScopeId = PPcurrentScope;
      ASTEmap[myid]->checkAttributes();
    }
    a
  ]
;


/******************************************************************************
  EXIT                                                           (WS-BPEL 2.0)
******************************************************************************/

Exit(a,_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      assert(ASTEmap[myid] != NULL);

      a->parentId = myid;

      ASTEmap[myid]->parentScopeId = PPcurrentScope;
      ASTEmap[myid]->checkAttributes();
    }
    a
  ]
;


/******************************************************************************
  FLOW
******************************************************************************/

Flow(a,b,c,_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      assert(ASTEmap[myid] != NULL);

      a->parentId = myid;

      ASTEmap[myid]->parentScopeId = PPcurrentScope;
      ASTEmap[myid]->checkAttributes();

      /* above is generic */

      assert(ASTEmap[ASTEmap[myid]->parentActivityId] != NULL);
      ASTEmap[myid]->inWhile = ASTEmap[ASTEmap[myid]->parentActivityId]->inWhile;

      // distribute the flow's id to the enclosed activities
      // also distribute the control flow type
      foreach(innerActivity; activity_list c)
      {
        assert(ASTEmap[innerActivity->id] != NULL);
        ASTEmap[innerActivity->id]->parentActivityId = myid;
        ASTEmap[innerActivity->id]->controlFlow = ASTEmap[myid]->controlFlow;
      }
      foreach(link; tLink_list b)
      {
        assert(ASTEmap[link->id] != NULL);
        ASTEmap[link->id]->parentActivityId = myid;
      }
    }
    a b c
    {
      // id of an enclosed start activity (for [SA00056])
      unsigned int startActivityId = 0;

      // collect enclosed source links
      foreach(innerActivity; activity_list c)
      {
        assert(ASTEmap[innerActivity->id] != NULL);
        ASTEmap[myid]->enclosedSourceLinks = setUnion(ASTEmap[myid]->enclosedSourceLinks, ASTEmap[innerActivity->id]->enclosedSourceLinks);

        // collect start activities (for [SA00056])
        if (ASTEmap[innerActivity->id]->isStartActivity)
          startActivityId = innerActivity->id;
      }

      // check whether activities precede a start actvitiy (for [SA00056])
      if (startActivityId != 0)
      {
	foreach(innerActivity; activity_list c)
	{
          assert(ASTEmap[innerActivity->id] != NULL);
	
	  if (!ASTEmap[innerActivity->id]->isStartActivity &&
	      !ASTEmap[innerActivity->id]->hasTargetLink)
          {
	    SAerror(56, "<" + ASTEmap[startActivityId]->activityTypeName() + ">", ASTEmap[startActivityId]->attributes["referenceLine"]);
            break;
          }
	}
      }
      
      /* here we can check whether each link was used as source and target */
    }
  ]
;

Link(_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      assert(ASTEmap[myid] != NULL);

      ASTEmap[myid]->parentScopeId = PPcurrentScope;
      ASTEmap[myid]->checkAttributes();

      /* here we can check whether this link was not defined before */
      linkIdMap[ASTEmap[myid]->attributes["name"]] = myid;

      // cerr << "defined a link " << $0->A_name << " (" << myid << ")" << endl;

      ASTE_links.insert(ASTEmap[myid]->attributes["name"]);
    }
  ]
;


/******************************************************************************
  SWITCH                                                         (BPEL4WS 1.1)
******************************************************************************/

Switch(a,b,c,_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      assert(ASTEmap[myid] != NULL);

      a->parentId = myid;

      ASTEmap[myid]->parentScopeId = PPcurrentScope;
      ASTEmap[myid]->checkAttributes();

      /* above is generic */

      assert(ASTEmap[ASTEmap[myid]->parentActivityId] != NULL);
      ASTEmap[myid]->inWhile = ASTEmap[ASTEmap[myid]->parentActivityId]->inWhile;

      // distribute the switch's id to the enclosed activities
      // also distribute the control flow
      foreach(branch=Case(innerActivity); tCase_list b)
      {
        assert(ASTEmap[innerActivity->id] != NULL);
        ASTEmap[innerActivity->id]->parentActivityId = myid;
        ASTEmap[innerActivity->id]->controlFlow = ASTEmap[myid]->controlFlow;
      }

      // process Otherwise-branch
      with(c)
      {
        c=Otherwise(d,e):
        { 
          assert(ASTEmap[d->id] != NULL);
          ASTEmap[d->id]->parentActivityId = myid;
          ASTEmap[d->id]->controlFlow = ASTEmap[myid]->controlFlow;
        }
        default: { /* do nothing */ }
      }
    }
    a b c
    {
      // collect enclosed source links
      foreach(branch=Case(innerActivity); tCase_list b)
      {
        assert(ASTEmap[innerActivity->id] != NULL);
        ASTEmap[myid]->enclosedSourceLinks = setUnion(ASTEmap[myid]->enclosedSourceLinks, ASTEmap[innerActivity->id]->enclosedSourceLinks);
      }

      with(c)
      {
        c=Otherwise(d,e):
        {
          assert(ASTEmap[d->id] != NULL);
          ASTEmap[myid]->enclosedSourceLinks = setUnion(ASTEmap[myid]->enclosedSourceLinks, ASTEmap[d->id]->enclosedSourceLinks);
        }
        default: { /* do nothing */ }
      }
    }
  ]
;

Case(a,_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      assert(ASTEmap[myid] != NULL);

      ASTEmap[myid]->parentScopeId = PPcurrentScope;
      ASTEmap[myid]->checkAttributes();
    }
    a
  ]
;

Otherwise(a,_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      assert(ASTEmap[myid] != NULL);

      ASTEmap[myid]->parentScopeId = PPcurrentScope;
    }
    a
  ]
;


/******************************************************************************
  IF                                                             (WS-BPEL 2.0)
******************************************************************************/

If(a,b,c,_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      assert(ASTEmap[myid] != NULL);

      a->parentId = myid;

      ASTEmap[myid]->parentScopeId = PPcurrentScope;
      ASTEmap[myid]->checkAttributes();

      /* above is generic */

      assert(ASTEmap[ASTEmap[myid]->parentActivityId] != NULL);
      ASTEmap[myid]->inWhile = ASTEmap[ASTEmap[myid]->parentActivityId]->inWhile;

      // distribute the switch's id to the enclosed activities
      // also distribute the control flow
      foreach(branch=ElseIf(innerActivity,_id2); tElseIf_list b)
      {
        assert(ASTEmap[innerActivity->id] != NULL);
        ASTEmap[innerActivity->id]->parentActivityId = myid;
        ASTEmap[innerActivity->id]->controlFlow = ASTEmap[myid]->controlFlow;
      }

      // process Else-branch
      with(c)
      {
        c=Else(d,_id2):
        { 
          assert(ASTEmap[d->id] != NULL);
          ASTEmap[d->id]->parentActivityId = myid;
          ASTEmap[d->id]->controlFlow = ASTEmap[myid]->controlFlow;
        }
        default: { /* do nothing */ }
      }
    }
    a b c
    {
      // collect enclosed source links
      foreach(branch=ElseIf(innerActivity,_id2); tElseIf_list b)
      {
        assert(ASTEmap[innerActivity->id] != NULL);
        ASTEmap[myid]->enclosedSourceLinks = setUnion(ASTEmap[myid]->enclosedSourceLinks, ASTEmap[innerActivity->id]->enclosedSourceLinks);
      }

      with(c)
      {
        c=Else(d,_id2):
        {
          assert(ASTEmap[d->id] != NULL);
          ASTEmap[myid]->enclosedSourceLinks = setUnion(ASTEmap[myid]->enclosedSourceLinks, ASTEmap[d->id]->enclosedSourceLinks);
        }
        default: { /* do nothing */ }
      }
    }
  ]
;

ElseIf(a,_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      assert(ASTEmap[myid] != NULL);

      ASTEmap[myid]->parentScopeId = PPcurrentScope;
      ASTEmap[myid]->checkAttributes();
    }
    a
  ]
;

Else(a,_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      assert(ASTEmap[myid] != NULL);

      ASTEmap[myid]->parentScopeId = PPcurrentScope;
    }
    a
  ]
;

NoElse(_id) -> [postprocessing: ];


/******************************************************************************
  WHILE
******************************************************************************/

While(a,b,_id) ->
  [postprocessing:
    {
      // manage identifiers
      int myid = _id->value;
      assert(ASTEmap[myid] != NULL);

      a->parentId = myid;

      ASTEmap[myid]->parentScopeId = PPcurrentScope;
      ASTEmap[myid]->checkAttributes();

      /* above is generic */

      ASTEmap[myid]->inWhile = true;

      // distribute the while's id to the enclosed activity
      assert(ASTEmap[b->id] != NULL);
      ASTEmap[b->id]->parentActivityId = myid;
      ASTEmap[b->id]->controlFlow = ASTEmap[myid]->controlFlow;
    }
    a b
    {
      // collect enclosed source links
      assert(ASTEmap[b->id] != NULL);
      ASTEmap[myid]->enclosedSourceLinks = setUnion(ASTEmap[myid]->enclosedSourceLinks, ASTEmap[b->id]->enclosedSourceLinks);
    }
  ]
;





/******************************************************************************
  REPEAT UNTIL                                                   (WS-BPEL 2.0)
******************************************************************************/

RepeatUntil(a,b,_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      assert(ASTEmap[myid] != NULL);

      a->parentId = myid;

      ASTEmap[myid]->parentScopeId = PPcurrentScope;
      ASTEmap[myid]->checkAttributes();

      /* above is generic */

      ASTEmap[myid]->inWhile = true;

      // distribute the repeatUntil's id to the enclosed activity
      assert(ASTEmap[b->id] != NULL);
      ASTEmap[b->id]->parentActivityId = myid;
      ASTEmap[b->id]->controlFlow = ASTEmap[myid]->controlFlow;
    }
    a b
    {
      // collect enclosed source links
      assert(ASTEmap[b->id] != NULL);
      ASTEmap[myid]->enclosedSourceLinks = setUnion(ASTEmap[myid]->enclosedSourceLinks, ASTEmap[b->id]->enclosedSourceLinks);
    }
  ]
;





/******************************************************************************
  SEQUENCE
******************************************************************************/

Sequence(a,b,_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      assert(ASTEmap[myid] != NULL);

      a->parentId = myid;

      ASTEmap[myid]->parentScopeId = PPcurrentScope;
      ASTEmap[myid]->checkAttributes();

      /* above is generic */

      assert(ASTEmap[ASTEmap[myid]->parentActivityId] != NULL);
      ASTEmap[myid]->inWhile = ASTEmap[ASTEmap[myid]->parentActivityId]->inWhile;

      // distribute the sequence's id to the enclosed activities
      foreach(innerActivity; activity_list b)
      {
        assert(ASTEmap[innerActivity->id] != NULL);
        ASTEmap[innerActivity->id]->parentActivityId = myid;
        ASTEmap[innerActivity->id]->controlFlow = ASTEmap[myid]->controlFlow;
      }
    }
    a b
    {
      // collect enclosed source links and check whether the sequence contains
      // a start activity (for [SA00056])
      int activityCount = 1;
      foreach(innerActivity; activity_list b)
      {
        assert(ASTEmap[innerActivity->id] != NULL);

        ASTEmap[myid]->enclosedSourceLinks = setUnion(ASTEmap[myid]->enclosedSourceLinks, ASTEmap[innerActivity->id]->enclosedSourceLinks);

        // trigger [SA00056]
        if (ASTEmap[innerActivity->id]->isStartActivity && activityCount++ > 1)
          SAerror(56, "<" + ASTEmap[innerActivity->id]->activityTypeName() + ">", ASTEmap[innerActivity->id]->attributes["referenceLine"]);
      }
    }
  ]
;


/******************************************************************************
  PICK
******************************************************************************/

Pick(a,b,c,_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      assert(ASTEmap[myid] != NULL);
      ASTE *myself = ASTEmap[myid];

      a->parentId = myid;

      myself->parentScopeId = PPcurrentScope;
      myself->checkAttributes();

      /* above is generic */

      assert(ASTEmap[myself->parentActivityId] != NULL);
      myself->inWhile = ASTEmap[myself->parentActivityId]->inWhile;

      // distribute the pick's id to the enclosed activities
      foreach(branch=OnMessage(_corr,_from,innerActivity,_id2); tOnMessage_list b)
      {
        assert(ASTEmap[innerActivity->id] != NULL);
        ASTEmap[innerActivity->id]->parentActivityId = myid;
        ASTEmap[branch->id]->parentActivityId = myid;
        ASTEmap[innerActivity->id]->controlFlow = myself->controlFlow;
      }
      foreach(branch=OnAlarm(innerActivity); tOnAlarm_list c)
      {
        assert(ASTEmap[innerActivity->id] != NULL);
        ASTEmap[innerActivity->id]->parentActivityId = myid;
        ASTEmap[branch->id]->parentActivityId = myid;
        ASTEmap[innerActivity->id]->controlFlow = myself->controlFlow;
      }
    }
    a b c
    {
      if (myself->isStartActivity)
      {
        startActivities.insert(myid);

        // trigger [SA00062]
        if (c->length() != 0)
          SAerror(62, "", myself->attributes["referenceLine"]);

        // trigger [SA00056]
        if (!ASTEmap[myself->parentActivityId]->checkAncestors() || myself->hasTargetLink)
          SAerror(56, "<pick>", myself->attributes["referenceLine"]);
      }

      // collect enclosed source links
      foreach(branch=OnMessage(_corr,_from,innerActivity,_id2); tOnMessage_list b)
      {
        assert(ASTEmap[innerActivity->id] != NULL);
        myself->enclosedSourceLinks = setUnion(myself->enclosedSourceLinks, ASTEmap[innerActivity->id]->enclosedSourceLinks);
      }
      foreach(branch=OnAlarm(innerActivity); tOnAlarm_list c)
      {
        assert(ASTEmap[innerActivity->id] != NULL);
        myself->enclosedSourceLinks = setUnion(myself->enclosedSourceLinks, ASTEmap[innerActivity->id]->enclosedSourceLinks);
      }
    }
  ]
;


/******************************************************************************
  FOREACH                                                        (WS-BPEL 2.0)
******************************************************************************/

ForEach(a,b,_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      assert(ASTEmap[myid] != NULL);

      a->parentId = myid;

      ASTEmap[myid]->parentScopeId = PPcurrentScope;
      ASTEmap[myid]->checkAttributes();

      /* above is generic */

      // distribute the forEach's id to the enclosed activity
      assert(ASTEmap[b->id] != NULL);
      ASTEmap[b->id]->parentActivityId = myid;
      ASTEmap[b->id]->controlFlow = ASTEmap[myid]->controlFlow;
    }
    a b
    {
      // collect enclosed source links
      assert(ASTEmap[b->id] != NULL);
      ASTEmap[myid]->enclosedSourceLinks = setUnion(ASTEmap[myid]->enclosedSourceLinks, ASTEmap[b->id]->enclosedSourceLinks);
    }
  ]
;


/******************************************************************************
  SCOPE
******************************************************************************/

Scope(a,b,c,d,e,f,g,h,_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      assert(ASTEmap[myid] != NULL);
      ASTE *myself = ASTEmap[myid];
      a->parentId = myid;

      // my parent is the current scope
      myself->parentScopeId = PPcurrentScope;
      // now I am the current scope
      PPcurrentScope = myid;

      assert(ASTEmap[myself->parentActivityId] != NULL);
      assert(ASTEmap[myself->parentScopeId] != NULL);
      myself->inWhile = ASTEmap[myself->parentActivityId]->inWhile;
      myself->checkAttributes();
      ASTEmap[myself->parentScopeId]->enclosedScopes.insert(myid);
      myself->hasEH = (string(e->op_name()) == "userDefinedEventHandler");

      // remember the scope's name for compensation
      if (myself->attributes["name"] != "")
      {
        if (scopeNames[myself->attributes["name"]] != 0)
          SAerror(92, myself->attributes["name"], myself->attributes["referenceLine"]);
        
        scopeNames[myself->attributes["name"]] = myid;
      }

      // distribute the scope's id to the enclosed activity
      assert(ASTEmap[h->id] != NULL);
      ASTEmap[h->id]->parentActivityId = myid;
    }
    h // inner activity
    a b c d e f g
    {
      // add an attribute to the compensation handler
      if(PPhasCompensateMap[myid] == 1)
        d->hasCompensateWithoutScope = true;
      if(PPhasCompensateMap[myid] == 2)
        d->hasCompensateWithScope = true;


      // pass list of enclosed scopes to the compensation handler
      ASTEmap[d->id]->enclosedScopes = myself->enclosedScopes;

      // collect enclosed source links
      assert(ASTEmap[h->id] != NULL);
      myself->enclosedSourceLinks = setUnion(myself->enclosedSourceLinks, ASTEmap[h->id]->enclosedSourceLinks);

      // everybody after me has the same parent scope as me
      PPcurrentScope = myself->parentScopeId;
    }
  ]
;


/******************************************************************************
  STANDARD ELEMENTS
******************************************************************************/

StandardElements(a,b,c) ->
  [postprocessing:
    {
      assert(ASTEmap[$0->parentId] != NULL);
      $0->suppressJF = (ASTEmap[$0->parentId]->attributes["suppressJoinFailure"] == "yes");

      foreach(targetLink; tTarget_list a)
      {
        assert(ASTEmap[targetLink->id] != NULL);
        ASTEmap[targetLink->id]->controlFlow = ASTEmap[$0->parentId]->controlFlow;
        ASTEmap[targetLink->id]->parentActivityId = $0->parentId;
      }

      foreach(sourceLink; tSource_list b)
      {
        assert(ASTEmap[sourceLink->id] != NULL);
        ASTEmap[sourceLink->id]->controlFlow = ASTEmap[$0->parentId]->controlFlow;
        ASTEmap[sourceLink->id]->parentActivityId = $0->parentId;
      }
    }
    a b c
    {
      /* here we can check the links */
    }
  ]
;

Target(a) ->
  [postprocessing:
    {
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);

      // find the id of the link used as target here
      int linkId = linkIdMap[ASTEmap[myid]->attributes["linkName"]];
      /* here we can check whether this link was defined before */


      // annotate AST and check information
      ASTEmap[myid]->checkAttributes();

      assert(ASTEmap[ASTEmap[myid]->parentActivityId] != NULL);
      ASTEmap[ASTEmap[myid]->parentActivityId]->hasTargetLink = true;

/*TODO
      // remember that that link was used as a target
      assert(ASTEmap[linkId] != NULL);
      kc::impl_tLink *theLink = (kc::impl_tLink*)(ASTEmap[linkId]->node);
      assert(theLink != NULL);
      assert(theLink->id == linkId);
      // here we can check whether this link was not used as target before
      theLink->usedAsTarget = true;
*/

      ASTEmap[myid]->linkName = ASTEmap[myid]->attributes["linkName"];
    }
  ]
;

Source(a) ->
  [postprocessing:
    {
      int myid = $0->id;
      assert(ASTEmap[myid] != NULL);

      // find the id of the link used as source here
      int linkId = linkIdMap[ASTEmap[myid]->attributes["linkName"]];
      /* here we can check whether this link was defined before */


      // annotate AST and check information
      ASTEmap[myid]->checkAttributes();

      assert(ASTEmap[ASTEmap[myid]->parentActivityId] != NULL);
      ASTEmap[ASTEmap[myid]->parentActivityId]->hasSourceLink = true;

/*TODO
      // remember that that link was used as a source
      assert(ASTEmap[linkId] != NULL);
      kc::impl_tLink *theLink = (kc::impl_tLink*)(ASTEmap[linkId]->node);
      assert(theLink != NULL);
      assert(theLink->id == linkId);
      // here we can check whether this link was not used as target before
      theLink->usedAsSource = true;
*/

      // add that link to the parent activity
      assert(ASTEmap[ASTEmap[myid]->parentActivityId] != NULL);
      ASTEmap[ASTEmap[myid]->parentActivityId]->enclosedSourceLinks.insert(linkId);

      ASTEmap[myid]->linkName = ASTEmap[myid]->attributes["linkName"];
    }
  ]
;
