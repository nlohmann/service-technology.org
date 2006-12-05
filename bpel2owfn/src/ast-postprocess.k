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
 * \brief   AST postprocessor (implementation)
 *
 * \author  responsible: Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          last changes of: \$Author: gierds $
 * 
 * \since   2006-07-02
 *
 * \date    \$Date: 2006/12/05 11:51:08 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \note    This file was created using Kimwitu++ reading file bpel-unparse.k.
 *          See http://site.informatik.hu-berlin.de/kimwitu++ for details.
 *
 * \version \$Revision: 1.112 $
 */
%}





/******************************************************************************
  PROCESS
******************************************************************************/

Process(a,b,c,d,e,f,g,h,i,_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      ASTE *myself = ASTEmap[myid];
      assert(myself != NULL);

      myself->parentScopeId = myid;
      myself->parentActivityId = myid;
      PPcurrentScope = myid;

      // annotate AST and check information
      myself->checkAttributes();
      myself->hasEH = (string(g->op_name()) == "userDefinedEventHandler");
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
    a b c d
    i // inner activity
    e f g h
    {
      // collect enclosed source links
      assert(ASTEmap[i->id] != NULL);
      myself->enclosedSourceLinks = ASTEmap[i->id]->enclosedSourceLinks;

      set< unsigned int > innerIds = ASTEmap[ i->id ]->enclosedActivities;
      innerIds.insert( i->id );
      set< unsigned int > faultIds = ASTEmap[ e->id ]->enclosedActivities;
      faultIds.insert( e->id );
      set< unsigned int > compIds = ASTEmap[ f->id ]->enclosedActivities;
      compIds.insert( f->id );

      enterConflictingActivities( innerIds, faultIds );
      enterConflictingActivities( faultIds, compIds );

      // add an attribute to the compensation handler
      if(PPhasCompensateMap[myid] == 1)
        f->hasCompensateWithoutScope = true;
      if(PPhasCompensateMap[myid] == 2)
        f->hasCompensateWithScope = true;

      // pass list of enclosed scopes to the compensation handler
      ASTEmap[f->id]->enclosedScopes = myself->enclosedScopes;

      // trigger [SA00015]
      if (startActivities.empty())
        SAerror(15, "", myself->attributes["referenceLine"]);

      // trigger [SA00066]
      // should be actually calculated at a <flow> activity, but "linksOK" is
      // used below to avoid an asserion failure
      bool linksOK = true;
      for(map<string, unsigned int>::iterator it = linkIdMap.begin(); it != linkIdMap.end(); it++)
      {
        assert(ASTEmap[it->second] != NULL);

        if ((ASTEmap[it->second]->targetActivity == 0) || (ASTEmap[it->second]->sourceActivity == 0))
        {
          SAerror(66, it->first, ASTEmap[it->second]->attributes["referenceLine"]);
          linksOK = false;
        }

        // trigger [SA00067]
        if (linksOK)
        {
          assert(ASTEmap[ASTEmap[it->second]->sourceActivity] != NULL);
          assert(ASTEmap[ASTEmap[it->second]->targetActivity] != NULL);
          set<unsigned int> otherSources = ASTEmap[ASTEmap[it->second]->sourceActivity]->sourceLinks;
          set<unsigned int> otherTargets = ASTEmap[ASTEmap[it->second]->targetActivity]->targetLinks;
          set<unsigned int> jointLinks = setIntersection(otherSources, otherTargets);
          jointLinks.erase(it->second);

          if (jointLinks.size() > 0)
            SAerror(67, "`" + it->first + "' and <link> `" + ASTEmap[*(jointLinks.begin())]->attributes["name"] + "'", ASTEmap[it->second]->attributes["referenceLine"]);
        }
      }

      // trigger [SA00057]
      set<string> reallySharedCorrelationSets;
      if (startActivities.size() > 1)
      {
  	for (set<unsigned int>::iterator it = startActivities.begin(); it != startActivities.end(); it++)
  	{
  	  if (it == startActivities.begin())
    	    reallySharedCorrelationSets = ASTEmap[*it]->sharedCorrelationSets;
  	  else
	    reallySharedCorrelationSets = setIntersection(reallySharedCorrelationSets, ASTEmap[*it]->sharedCorrelationSets);

  	  if (reallySharedCorrelationSets.empty())
  	  {
  	    SAerror(57, "", ASTEmap[*it]->attributes["referenceLine"]);
      	    break;
    	  }
  	}
      }

      // this assertion guarantees that we did not forget a link and encounter
      // problems generating the Petri net
      if (linksOK)
        assert(myself->enclosedSourceLinks.size() == linkNames.size());
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
      ASTE *myself = ASTEmap[myid];
      assert(myself != NULL);

      myself->parentScopeId = PPcurrentScope;
      myself->checkAttributes();

      myself->definePartnerLink();
    }
  ]
;


/******************************************************************************
  PARTNERS
******************************************************************************/

Partner(a,_id) ->
  [postprocessing: a]
;


/******************************************************************************
  FAULT HANDLERS
******************************************************************************/

userDefinedFaultHandler(a,b,_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      ASTE *myself = ASTEmap[myid];
      assert(myself != NULL);

      myself->parentScopeId = PPcurrentScope;
      myself->hasCatchAll = (b->op_name() == "CatchAll");
      myself->inProcess = (myself->parentScopeId == 1);

      // distribute the fault handlers's id to the enclosed catch/catchAll branches
      // also mark the activities as "inside fault handlers"
      foreach(branch=Catch(innerActivity); tCatch_list a)
      {
        assert(ASTEmap[innerActivity->id] != NULL);
        ASTEmap[innerActivity->id]->parentActivityId = myid;
        ASTEmap[branch->id]->parentActivityId = myid;
        ASTEmap[innerActivity->id]->controlFlow = FHCF;
        ASTEmap[innerActivity->id]->max_occurrences = myself->max_occurrences;
      }

      // check whether a <catchAll>-branch with an enclosed activity is present
      // and if yes, annotate it
      with (b)
      {
        CatchAll(c,_id) :
        {
          assert(ASTEmap[c->id] != NULL);
//NL//          ASTEmap[c->id]->parentActivityId = myid;
          ASTEmap[b->id]->parentActivityId = myid;
          ASTEmap[c->id]->controlFlow = FHCF;
          ASTEmap[c->id]->max_occurrences = myself->max_occurrences;
        }
        default :
        {
          // triggers SA00080
          if (a->length() == 0)
            SAerror(80, "", toInt(ASTEmap[myself->parentScopeId]->attributes["referenceLine"]));
        }
      }
    }
    a b
    {
      ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myid );
      ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myself->enclosedActivities.begin(), myself->enclosedActivities.end() );
      enterEnclosedActivities( myid, ASTEmap[ myid ]->enclosedActivities );

      foreach(branch=Catch(innerActivity,_id2); tCatch_list a)
      {
        foreach(otherbranch=Catch(secondActivity,_id3); tCatch_list a)
        {
          if ( _id2->value < _id3->value )
          {
            enterConflictingActivities( ASTEmap[ _id2->value ]->enclosedActivities, ASTEmap[ _id3->value ]->enclosedActivities );
          }
        }
      }

      with (b)
      {
        CatchAll(c,_id2) :
        {
          foreach(otherbranch=Catch(secondActivity,_id3); tCatch_list a)
          {
            enterConflictingActivities( ASTEmap[ _id2->value ]->enclosedActivities, ASTEmap[ _id3->value ]->enclosedActivities );
          }
        }
        default :
        {
        }
      }
    }
  ]
;

implicitFaultHandler(_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      ASTE *myself = ASTEmap[myid];
      assert(myself != NULL);

      myself->parentScopeId = PPcurrentScope;
      myself->inProcess = (myself->parentScopeId == 1);

      ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myid );
    }
  ]
;


Catch(a,_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      ASTE *myself = ASTEmap[myid];
      assert(myself != NULL);

      myself->checkAttributes();
    }
    a
    {
      myself->enclosedActivities = ASTEmap[ a->id ]->enclosedActivities;
      myself->enclosedActivities.insert( a->id );
      enterEnclosedActivities( myid, ASTEmap[ myid ]->enclosedActivities );
    }
  ]
;

CatchAll(a,_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      ASTE *myself = ASTEmap[myid];
      assert(myself != NULL);
      ASTEmap[a->id]->parentActivityId = myid;
    }
    a
    {
      myself->enclosedActivities = ASTEmap[ a->id ]->enclosedActivities;
      myself->enclosedActivities.insert( a->id );
      enterEnclosedActivities( myid, ASTEmap[ myid ]->enclosedActivities );
    }
  ]
;


/******************************************************************************
  EVENT HANDLERS
******************************************************************************/

implicitEventHandler(_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      ASTE *myself = ASTEmap[myid];
      assert(myself != NULL);

      myself->parentScopeId = PPcurrentScope;

      ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myid );
    }
  ]
;

userDefinedEventHandler(a,b,_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      ASTE *myself = ASTEmap[myid];
      assert(myself != NULL);

      myself->parentScopeId = PPcurrentScope;

      // triggers SA00083
      if (a->length() == 0 && b->length() == 0)
        SAerror(83, "", toInt(ASTEmap[myself->parentScopeId]->attributes["referenceLine"]));

      // distribute the event handlers's id to the enclosed activities
      foreach(branch=OnMessage(_cor,_from,_act,_id2); tOnMessage_list a)
      {
        assert(ASTEmap[_id2->value] != NULL);
        ASTEmap[_id2->value]->parentActivityId = myid;
        ASTEmap[branch->id]->parentActivityId = myid;
        ASTEmap[_id2->value]->controlFlow = myself->controlFlow;
      }
      foreach(branch=OnAlarm(innerActivity); tOnAlarm_list b)
      {
        assert(ASTEmap[innerActivity->id] != NULL);
        ASTEmap[innerActivity->id]->parentActivityId = myid;
        ASTEmap[branch->id]->parentActivityId = myid;
        ASTEmap[innerActivity->id]->controlFlow = myself->controlFlow;
      }
    }
    a b
    {
      ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myid );
      ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myself->enclosedActivities.begin(), myself->enclosedActivities.end() );
      enterEnclosedActivities( myid, ASTEmap[ myid ]->enclosedActivities );
    }
  ]
;

OnMessage(a,b,c,_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      ASTE *myself = ASTEmap[myid];
      assert(myself != NULL);

      myself->parentScopeId = PPcurrentScope;
      assert(ASTEmap[myself->parentActivityId] != NULL);
      myself->checkAttributes();

      myself->checkPartnerLink();

      // trigger [SA00063]
      if (b->length() != 0 && myself->attributes["variable"] != "")
        SAerror(63, "", myself->attributes["referenceLine"]);

      myself->channelName = myself->createChannel();
      myself->variableName = myself->checkVariable("variable");

      assert(ASTEmap[c->id] != NULL);
      ASTEmap[c->id]->parentActivityId = myid;

      foreach(correlation=Correlation(_id2); tCorrelation_list a)
      {
        assert(ASTEmap[_id2->value] != NULL);
        ASTEmap[_id2->value]->parentActivityId = myid;
      }
    }
    a b c
    {
      ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myid );
      ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myself->enclosedActivities.begin(), myself->enclosedActivities.end() );
      enterEnclosedActivities( myid, ASTEmap[ myid ]->enclosedActivities );
    }
  ]
;

OnAlarm(a,_id) ->
  [postprocessing: a
    {
      int myid = _id->value;
      ASTE *myself = ASTEmap[myid];
      assert(myself != NULL);

      myself->parentScopeId = PPcurrentScope;
      assert(ASTEmap[myself->parentActivityId] != NULL);
      //todo: cyclic-attribute if "repeatEvery" is set

      assert(ASTEmap[a->id] != NULL);
      ASTEmap[a->id]->parentActivityId = myid;
    }
    a
    {
      ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myid );
      ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myself->enclosedActivities.begin(), myself->enclosedActivities.end() );
      enterEnclosedActivities( myid, ASTEmap[ myid ]->enclosedActivities );
    }
  ]
;


/******************************************************************************
  COMPENSATION HANDLERS
******************************************************************************/

implicitCompensationHandler(_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      ASTE *myself = ASTEmap[myid];
      assert(myself != NULL);

      myself->parentScopeId = PPcurrentScope;

      ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myid );
    }
  ]
;

userDefinedCompensationHandler(a,_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      ASTE *myself = ASTEmap[myid];
      assert(myself != NULL);

      myself->parentScopeId = PPcurrentScope;

      // distribute the compensation handlers's id to the enclosed activity
      // also mark the activities as "inside compensation handlers"
      assert(ASTEmap[a->id] != NULL);
      ASTEmap[a->id]->parentActivityId = myid;
      ASTEmap[a->id]->controlFlow = CHCF;
      ASTEmap[a->id]->max_occurrences = myself->max_occurrences;
    }
    a
    {
      ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myid );
      ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myself->enclosedActivities.begin(), myself->enclosedActivities.end() );
      enterEnclosedActivities( myid, ASTEmap[ myid ]->enclosedActivities );

      // trigger SA00079
      vector<unsigned int> ancestors = myself->ancestorActivities();
      assert(ancestors.size() > 1);
      string ancestor = ASTEmap[ancestors[1]]->activityTypeName();
      if (ancestor == "catch" ||
	  ancestor == "catchAll" ||
	  ancestor == "terminationHandler" ||
	  ancestor == "compensationHandler")
	SAerror(79, ancestor);
    }
  ]
;


/******************************************************************************
  TERMINATION HANDLER                                            (WS-BPEL 2.0)
******************************************************************************/

TerminationHandler(a,_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      ASTE *myself = ASTEmap[myid];
      assert(myself != NULL);

      myself->parentScopeId = PPcurrentScope;

      // distribute the termination handlers's id to the enclosed activity
      // also mark the activities as "inside termination handlers"
      assert(ASTEmap[a->id] != NULL);
      ASTEmap[a->id]->parentActivityId = myid;
      ASTEmap[a->id]->controlFlow = THCF;
      ASTEmap[a->id]->max_occurrences = myself->max_occurrences;
    }
    a
    {
      ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myid );
      ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myself->enclosedActivities.begin(), myself->enclosedActivities.end() );
      enterEnclosedActivities( myid, ASTEmap[ myid ]->enclosedActivities );
    }
  ]
;





/******************************************************************************
  VARIABLES
******************************************************************************/

Variable(_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      ASTE *myself = ASTEmap[myid];
      assert(myself != NULL);

      myself->parentScopeId = PPcurrentScope;
      myself->checkAttributes();
      myself->defineVariable();
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
      ASTE *myself = ASTEmap[myid];
      assert(myself != NULL);

      myself->parentScopeId = PPcurrentScope;
      myself->checkAttributes();
      myself->defineCorrelationSet();
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
      ASTE *myself = ASTEmap[myid];
      assert(myself != NULL);

      myself->parentScopeId = PPcurrentScope;
      myself->checkAttributes();
      string correlationSetName = myself->checkCorrelationSet();

      // needed to trigger [SA00057] later
      assert(ASTEmap[myself->parentActivityId] != NULL);
      if (ASTEmap[myself->parentActivityId]->isStartActivity &&
          myself->attributes["initiate"] == "join")
      {
        ASTEmap[myself->parentActivityId]->sharedCorrelationSets.insert(correlationSetName);
      }
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
      ASTE *myself = ASTEmap[myid];
      assert(myself != NULL);

      myself->checkAttributes();
     }
  ]
;

FromPart(_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      ASTE *myself = ASTEmap[myid];
      assert(myself != NULL);

      myself->checkAttributes();
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
      ASTE *myself = ASTEmap[myid];
      assert(myself != NULL);

      a->parentId = myid;

      myself->parentScopeId = PPcurrentScope;
      myself->checkAttributes();
    }
    a
    {
      ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myid );
    }
  ]
;


/******************************************************************************
  INVOKE
******************************************************************************/

Invoke(a,b,c,d,_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      ASTE *myself = ASTEmap[myid];
      assert(myself != NULL);

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

      myself->checkPartnerLink();

      myself->channelName = myself->createChannel((myself->attributes["outputVariable"] != ""));
      myself->inputVariableName = myself->checkVariable("inputVariable");
      myself->outputVariableName = myself->checkVariable("outputVariable");

      foreach(correlation=Correlation(_id2); tCorrelation_list b)
      {
        assert(ASTEmap[_id2->value] != NULL);
        ASTEmap[_id2->value]->parentActivityId = myid;
      }
    }
    a b
    {
      ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myid );
    }
  ]
;


/******************************************************************************
  RECEIVE
******************************************************************************/

Receive(a,b,c,_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      ASTE *myself = ASTEmap[myid];
      assert(myself != NULL);

      a->parentId = myid;

      myself->parentScopeId = PPcurrentScope;
      myself->checkAttributes();

      /* above is generic */

      // trigger [SA00055]
      if (c->length() != 0 && myself->attributes["variable"] != "")
        SAerror(55, "", myself->attributes["referenceLine"]);

      myself->checkPartnerLink();

      myself->channelName = myself->createChannel();
      myself->variableName = myself->checkVariable("variable");

      foreach(correlation=Correlation(_id2); tCorrelation_list b)
      {
        assert(ASTEmap[_id2->value] != NULL);
        ASTEmap[_id2->value]->parentActivityId = myid;
      }
    }
    a b
    {
      if (myself->isStartActivity)
      {
        startActivities.insert(myid);

        // trigger [SA00056]
        if (!ASTEmap[myself->parentActivityId]->checkAncestors() || !myself->targetLinks.empty())
          SAerror(56, "<receive>", myself->attributes["referenceLine"]);
      }

      ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myid );
    }
  ]
;


/******************************************************************************
  REPLY
******************************************************************************/

Reply(a,b,c,_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      ASTE *myself = ASTEmap[myid];
      assert(myself != NULL);

      a->parentId = myid;

      myself->parentScopeId = PPcurrentScope;
      myself->checkAttributes();

      /* above is generic */

      myself->checkPartnerLink();

      // trigger [SA00059]
      if (c->length() != 0 && myself->attributes["variable"] != "")
        SAerror(59, "", myself->attributes["referenceLine"]);

      myself->channelName = myself->createChannel();
      myself->variableName = myself->checkVariable("variable");

      foreach(correlation=Correlation(_id2); tCorrelation_list b)
      {
        assert(ASTEmap[_id2->value] != NULL);
        ASTEmap[_id2->value]->parentActivityId = myid;
      }
    }
    a b
    {
      ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myid );
    }
  ]
;


/******************************************************************************
  ASSIGN
******************************************************************************/

Assign(a,b,_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      ASTE *myself = ASTEmap[myid];
      assert(myself != NULL);

      a->parentId = myid;

      myself->parentScopeId = PPcurrentScope;
      myself->checkAttributes();
    }
    a b
    {
      ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myid );
    }
  ]
;

Copy(a,b,c) ->
  [postprocessing: a b]
;

From(_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      ASTE *myself = ASTEmap[myid];
      assert(myself != NULL);

      myself->parentScopeId = PPcurrentScope;
      myself->checkAttributes();

      myself->checkPartnerLink();
      myself->variableName = myself->checkVariable("variable");
    }
  ]
;

To(_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      ASTE *myself = ASTEmap[myid];
      assert(myself != NULL);

      myself->parentScopeId = PPcurrentScope;
      myself->checkAttributes();

      myself->checkPartnerLink();
      myself->variableName = myself->checkVariable("variable");
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
      ASTE *myself = ASTEmap[myid];
      assert(myself != NULL);

      a->parentId = myid;

      myself->parentScopeId = PPcurrentScope;
      myself->checkAttributes();
    }
    a
    {
      ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myid );
    }
  ]
;


/******************************************************************************
  WAIT
******************************************************************************/

Wait(a,_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      ASTE *myself = ASTEmap[myid];
      assert(myself != NULL);

      a->parentId = myid;

      myself->parentScopeId = PPcurrentScope;
      myself->checkAttributes();
    }
    a
    {
      ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myid );
    }
  ]
;


/******************************************************************************
  THROW
******************************************************************************/

Throw(a,_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      ASTE *myself = ASTEmap[myid];
      assert(myself != NULL);

      a->parentId = myid;

      myself->parentScopeId = PPcurrentScope;
      myself->checkAttributes();
    }
    a
    {
      ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myid );
    }
  ]
;


/******************************************************************************
  RETHROW                                                        (WS-BPEL 2.0)
******************************************************************************/

Rethrow(a,_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      ASTE *myself = ASTEmap[myid];
      assert(myself != NULL);

      a->parentId = myid;

      myself->parentScopeId = PPcurrentScope;
      myself->checkAttributes();

      /* above is generic */

      // triggers [SA00006]
      bool correctUse = false;
      vector<unsigned int> ancestorActivities = myself->ancestorActivities();
      for (vector<unsigned int>::iterator it = ancestorActivities.begin(); it != ancestorActivities.end(); it++)
      {
        assert(ASTEmap[*it] != NULL);
        if (ASTEmap[*it]->activityTypeName() == "faultHandlers")
        {
          correctUse = true;
          break;
        }
      }
      if (!correctUse)
        SAerror(6, "", myself->attributes["referenceLine"]);
    }
    a
    {
      ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myid );
    }
  ]
;


/******************************************************************************
  COMPENSATE
******************************************************************************/

Compensate(a,_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      ASTE *myself = ASTEmap[myid];
      assert(myself != NULL);

      a->parentId = myid;

      myself->parentScopeId = PPcurrentScope;
      myself->checkAttributes();

      /* above is generic */

      // triggers [SA00008]
      bool correctUse = false;
      vector<unsigned int> ancestorActivities = myself->ancestorActivities();

      for (vector<unsigned int>::iterator it = ancestorActivities.begin(); it != ancestorActivities.end(); it++)
      {
        if (ASTEmap[*it]->activityTypeName() == "faultHandlers" ||
            ASTEmap[*it]->activityTypeName() == "compensationHandler" ||
            ASTEmap[*it]->activityTypeName() == "terminationHandler")
        {
          correctUse = true;
          break;
        }
      }
      if (!correctUse)
        SAerror(8, "", myself->attributes["referenceLine"]);


      // register this compensate at the current scope
      /* here we can check whether it is the only compensate activity */
      PPhasCompensateMap[PPcurrentScope] = 1;
    }
    a
    {
      ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myid );
    }
  ]
;


/******************************************************************************
  COMPENSATE SCOPE                                               (WS-BPEL 2.0)
******************************************************************************/

CompensateScope(a,b,_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      ASTE *myself = ASTEmap[myid];
      assert(myself != NULL);

      a->parentId = myid;

      myself->parentScopeId = PPcurrentScope;
      myself->checkAttributes();

      /* above is generic */


      // triggers [SA00007]
      bool correctUse = false;
      vector<unsigned int> ancestorActivities = myself->ancestorActivities();
      for (vector<unsigned int>::iterator it = ancestorActivities.begin(); it != ancestorActivities.end(); it++)
      {
        if (ASTEmap[*it]->activityTypeName() == "faultHandlers" ||
            ASTEmap[*it]->activityTypeName() == "compensationHandler" ||
            ASTEmap[*it]->activityTypeName() == "terminationHandler")
        {
          correctUse = true;
          break;
        }
      }
      if (!correctUse)
        SAerror(7, "", myself->attributes["referenceLine"]);


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
    {
      ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myid );
    }
  ]
;


/******************************************************************************
  TERMINATE                                                      (BPEL4WS 1.1)
******************************************************************************/

Terminate(a,_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      ASTE *myself = ASTEmap[myid];
      assert(myself != NULL);

      a->parentId = myid;

      myself->parentScopeId = PPcurrentScope;
      myself->checkAttributes();
    }
    a
    {
      ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myid );
    }
  ]
;


/******************************************************************************
  EXIT                                                           (WS-BPEL 2.0)
******************************************************************************/

Exit(a,_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      ASTE *myself = ASTEmap[myid];
      assert(myself != NULL);

      a->parentId = myid;

      myself->parentScopeId = PPcurrentScope;
      myself->checkAttributes();
    }
    a
    {
      ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myid );
    }
  ]
;


/******************************************************************************
  FLOW
******************************************************************************/

Flow(a,b,c,_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      ASTE *myself = ASTEmap[myid];
      assert(myself != NULL);

      a->parentId = myid;

      myself->parentScopeId = PPcurrentScope;
      myself->checkAttributes();

      /* above is generic */

      // distribute the flow's id to the enclosed activities
      // also distribute the control flow type
      foreach(innerActivity; activity_list c)
      {
        assert(ASTEmap[innerActivity->id] != NULL);
        ASTEmap[innerActivity->id]->parentActivityId = myid;
        ASTEmap[innerActivity->id]->controlFlow = myself->controlFlow;
        ASTEmap[innerActivity->id]->max_occurrences = myself->max_occurrences;
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

      foreach(innerActivity; activity_list c)
      {
        // collect enclosed source links
        assert(ASTEmap[innerActivity->id] != NULL);
        myself->enclosedSourceLinks = setUnion(myself->enclosedSourceLinks, ASTEmap[innerActivity->id]->enclosedSourceLinks);

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
	      ASTEmap[innerActivity->id]->targetLinks.empty())
          {
	    SAerror(56, "<" + ASTEmap[startActivityId]->activityTypeName() + ">", ASTEmap[startActivityId]->attributes["referenceLine"]);
            break;
          }
	}
      }
      
      /* here we can check whether each link was used as source and target */


      ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myid );
      ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myself->enclosedActivities.begin(), myself->enclosedActivities.end() );
      enterEnclosedActivities( myid, ASTEmap[ myid ]->enclosedActivities );
    }
  ]
;

Link(_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      ASTE *myself = ASTEmap[myid];
      assert(myself != NULL);

      myself->parentScopeId = PPcurrentScope;
      myself->checkAttributes();

      myself->linkName = myself->defineLink();

      linkIdMap[myself->attributes["name"]] = myid;
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
      ASTE *myself = ASTEmap[myid];
      assert(myself != NULL);

      a->parentId = myid;

      myself->parentScopeId = PPcurrentScope;
      myself->checkAttributes();

      /* above is generic */

      // distribute the switch's id to the enclosed activities
      // also distribute the control flow
      foreach(branch=Case(innerActivity,_id2); tCase_list b)
      {
        assert(ASTEmap[innerActivity->id] != NULL);
        ASTEmap[innerActivity->id]->parentActivityId = myid;
        ASTEmap[_id->value]->parentActivityId = myid;
        ASTEmap[innerActivity->id]->controlFlow = myself->controlFlow;
        ASTEmap[innerActivity->id]->max_occurrences = myself->max_occurrences;
      }

      // process Otherwise-branch
      with(c)
      {
        c=Otherwise(d,e):
        { 
          assert(ASTEmap[d->id] != NULL);
          ASTEmap[d->id]->parentActivityId = myid;
          ASTEmap[d->id]->controlFlow = myself->controlFlow;
          ASTEmap[d->id]->max_occurrences = myself->max_occurrences;
          
        }
        default: { /* do nothing */ }
      }
    }
    a b c
    {
      // collect enclosed source links
      foreach(branch=Case(innerActivity,_id2); tCase_list b)
      {
        assert(ASTEmap[innerActivity->id] != NULL);
        myself->enclosedSourceLinks = setUnion(myself->enclosedSourceLinks, ASTEmap[innerActivity->id]->enclosedSourceLinks);

        foreach(otherbranch=Case(secondActivity,_id3); tCase_list b)
        {
          if ( _id2->value < _id3->value )
          {
            enterConflictingActivities( ASTEmap[ _id2->value ]->enclosedActivities, ASTEmap[ _id3->value ]->enclosedActivities );
          }
        }
      }

      with(c)
      {
        c=Otherwise(d,e):
        {
          assert(ASTEmap[d->id] != NULL);
          myself->enclosedSourceLinks = setUnion(myself->enclosedSourceLinks, ASTEmap[d->id]->enclosedSourceLinks);

          foreach(otherbranch=Case(secondActivity,_id3); tCase_list b)
          {
            enterConflictingActivities( ASTEmap[ e->value ]->enclosedActivities, ASTEmap[ _id3->value ]->enclosedActivities );
          }
        }
        default: { /* do nothing */ }
      }

      ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myid );
      ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myself->enclosedActivities.begin(), myself->enclosedActivities.end() );
      enterEnclosedActivities( myid, ASTEmap[ myid ]->enclosedActivities );
    }
  ]
;

Case(a,_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      ASTE *myself = ASTEmap[myid];
      assert(myself != NULL);

      myself->parentScopeId = PPcurrentScope;
      myself->checkAttributes();
    }
    a
    {
      myself->enclosedActivities = ASTEmap[ a->id ]->enclosedActivities;
      myself->enclosedActivities.insert( a->id );
      enterEnclosedActivities( myid, ASTEmap[ myid ]->enclosedActivities );
    }
  ]
;

Otherwise(a,_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      ASTE *myself = ASTEmap[myid];
      assert(myself != NULL);

      myself->parentScopeId = PPcurrentScope;
    }
    a
    {
      myself->enclosedActivities = ASTEmap[ a->id ]->enclosedActivities;
      myself->enclosedActivities.insert( a->id );
      enterEnclosedActivities( myid, ASTEmap[ myid ]->enclosedActivities );
    }
  ]
;


/******************************************************************************
  IF                                                             (WS-BPEL 2.0)
******************************************************************************/

If(a,b,c,_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      ASTE *myself = ASTEmap[myid];
      assert(myself != NULL);

      a->parentId = myid;

      myself->parentScopeId = PPcurrentScope;
      myself->checkAttributes();

      /* above is generic */

      // distribute the switch's id to the enclosed activities
      // also distribute the control flow
      foreach(branch=ElseIf(innerActivity,_id2); tElseIf_list b)
      {
        assert(ASTEmap[innerActivity->id] != NULL);
        ASTEmap[innerActivity->id]->parentActivityId = myid;
        ASTEmap[innerActivity->id]->controlFlow = myself->controlFlow;
        ASTEmap[innerActivity->id]->max_occurrences = myself->max_occurrences;
      }

      // process Else-branch
      with(c)
      {
        c=Else(d,_id2):
        { 
          assert(ASTEmap[d->id] != NULL);
          ASTEmap[d->id]->parentActivityId = myid;
          ASTEmap[d->id]->controlFlow = myself->controlFlow;
          ASTEmap[d->id]->max_occurrences = myself->max_occurrences;
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
        myself->enclosedSourceLinks = setUnion(myself->enclosedSourceLinks, ASTEmap[innerActivity->id]->enclosedSourceLinks);
      }

      with(c)
      {
        c=Else(d,_id2):
        {
          assert(ASTEmap[d->id] != NULL);
          myself->enclosedSourceLinks = setUnion(myself->enclosedSourceLinks, ASTEmap[d->id]->enclosedSourceLinks);
        }
        default: { /* do nothing */ }
      }

      ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myid );
      ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myself->enclosedActivities.begin(), myself->enclosedActivities.end() );
      enterEnclosedActivities( myid, ASTEmap[ myid ]->enclosedActivities );
    }
  ]
;

ElseIf(a,_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      ASTE *myself = ASTEmap[myid];
      assert(myself != NULL);

      myself->parentScopeId = PPcurrentScope;
      myself->checkAttributes();
    }
    a
    {
      myself->enclosedActivities = ASTEmap[ a->id ]->enclosedActivities;
      myself->enclosedActivities.insert( a->id );
      enterEnclosedActivities( myid, ASTEmap[ myid ]->enclosedActivities );
    }
  ]
;

Else(a,_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      ASTE *myself = ASTEmap[myid];
      assert(myself != NULL);

      myself->parentScopeId = PPcurrentScope;
    }
    a
    {
      myself->enclosedActivities = ASTEmap[ a->id ]->enclosedActivities;
      myself->enclosedActivities.insert( a->id );
      enterEnclosedActivities( myid, ASTEmap[ myid ]->enclosedActivities );
    }
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
      ASTE *myself = ASTEmap[myid];
      assert(myself != NULL);

      a->parentId = myid;

      myself->parentScopeId = PPcurrentScope;
      myself->checkAttributes();

      /* above is generic */

      // distribute the while's id to the enclosed activity
      assert(ASTEmap[b->id] != NULL);
      ASTEmap[b->id]->parentActivityId = myid;
      ASTEmap[b->id]->controlFlow = myself->controlFlow;

      // calculate the maximal occurrences of the enclosed activity
      unsigned int max_loops = toUInt(myself->attributes["hu:maxloops"]);
      if (max_loops != UINT_MAX && myself->max_occurrences != UINT_MAX)
        ASTEmap[b->id]->max_occurrences = myself->max_occurrences * max_loops;
      else
        ASTEmap[b->id]->max_occurrences = UINT_MAX;

      myself->max_loops = max_loops;
    }
    a b
    {
      // collect enclosed source links
      assert(ASTEmap[b->id] != NULL);
      myself->enclosedSourceLinks = setUnion(myself->enclosedSourceLinks, ASTEmap[b->id]->enclosedSourceLinks);

      ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myid );
      ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myself->enclosedActivities.begin(), myself->enclosedActivities.end() );
      enterEnclosedActivities( myid, ASTEmap[ myid ]->enclosedActivities );
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
      ASTE *myself = ASTEmap[myid];
      assert(myself != NULL);

      a->parentId = myid;

      myself->parentScopeId = PPcurrentScope;
      myself->checkAttributes();

      /* above is generic */

      // distribute the repeatUntil's id to the enclosed activity
      assert(ASTEmap[b->id] != NULL);
      ASTEmap[b->id]->parentActivityId = myid;
      ASTEmap[b->id]->controlFlow = myself->controlFlow;

      // calculate the maximal occurrences of the enclosed activity
      unsigned int max_loops = toUInt(myself->attributes["hu:maxloops"]);
      if (max_loops != UINT_MAX && myself->max_occurrences != UINT_MAX)
        ASTEmap[b->id]->max_occurrences = myself->max_occurrences * max_loops;
      else
        ASTEmap[b->id]->max_occurrences = UINT_MAX;
      
      myself->max_loops = max_loops;
    }
    a b
    {
      // collect enclosed source links
      assert(ASTEmap[b->id] != NULL);
      myself->enclosedSourceLinks = setUnion(myself->enclosedSourceLinks, ASTEmap[b->id]->enclosedSourceLinks);

      ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myid );
      ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myself->enclosedActivities.begin(), myself->enclosedActivities.end() );
      enterEnclosedActivities( myid, ASTEmap[ myid ]->enclosedActivities );
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
      ASTE *myself = ASTEmap[myid];
      assert(myself != NULL);

      a->parentId = myid;

      myself->parentScopeId = PPcurrentScope;
      myself->checkAttributes();

      /* above is generic */

      // distribute the sequence's id to the enclosed activities
      foreach(innerActivity; activity_list b)
      {
        assert(ASTEmap[innerActivity->id] != NULL);
        ASTEmap[innerActivity->id]->parentActivityId = myid;
        ASTEmap[innerActivity->id]->controlFlow = myself->controlFlow;
        ASTEmap[innerActivity->id]->max_occurrences = myself->max_occurrences;
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

        myself->enclosedSourceLinks = setUnion(myself->enclosedSourceLinks, ASTEmap[innerActivity->id]->enclosedSourceLinks);

        // trigger [SA00056]
        if (ASTEmap[innerActivity->id]->isStartActivity && activityCount++ > 1)
          SAerror(56, "<" + ASTEmap[innerActivity->id]->activityTypeName() + ">", ASTEmap[innerActivity->id]->attributes["referenceLine"]);
      }

      ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myid );
      ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myself->enclosedActivities.begin(), myself->enclosedActivities.end() );
      enterEnclosedActivities( myid, ASTEmap[ myid ]->enclosedActivities );
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
      ASTE *myself = ASTEmap[myid];
      assert(myself != NULL);

      a->parentId = myid;

      myself->parentScopeId = PPcurrentScope;
      myself->checkAttributes();

      /* above is generic */

      // distribute the pick's id to the enclosed activities
      foreach(branch=OnMessage(_corr,_from,innerActivity,_id2); tOnMessage_list b)
      {
        assert(ASTEmap[innerActivity->id] != NULL);
        ASTEmap[innerActivity->id]->parentActivityId = myid;
        ASTEmap[branch->id]->parentActivityId = myid;
        ASTEmap[innerActivity->id]->controlFlow = myself->controlFlow;
        ASTEmap[innerActivity->id]->max_occurrences = myself->max_occurrences;
      }
      foreach(branch=OnAlarm(innerActivity); tOnAlarm_list c)
      {
        assert(ASTEmap[innerActivity->id] != NULL);
        ASTEmap[innerActivity->id]->parentActivityId = myid;
        ASTEmap[branch->id]->parentActivityId = myid;
        ASTEmap[innerActivity->id]->controlFlow = myself->controlFlow;
        ASTEmap[innerActivity->id]->max_occurrences = myself->max_occurrences;
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
        if (!ASTEmap[myself->parentActivityId]->checkAncestors() || !myself->targetLinks.empty())
          SAerror(56, "<pick>", myself->attributes["referenceLine"]);
      }

      // collect enclosed source links
      foreach(branch=OnMessage(_corr,_from,innerActivity,_id2); tOnMessage_list b)
      {
        assert(ASTEmap[innerActivity->id] != NULL);
        myself->enclosedSourceLinks = setUnion(myself->enclosedSourceLinks, ASTEmap[innerActivity->id]->enclosedSourceLinks);

        foreach(otherbranch=OnMessage(*,*,secondActivity,_id3); tOnMessage_list b)
        {
          if (_id2->value < _id3->value)
          {
            enterConflictingActivities( ASTEmap[ _id2->value ]->enclosedActivities, ASTEmap[ _id3->value ]->enclosedActivities );
          }
        }
      }
      foreach(branch=OnAlarm(innerActivity); tOnAlarm_list c)
      {
        assert(ASTEmap[innerActivity->id] != NULL);
        myself->enclosedSourceLinks = setUnion(myself->enclosedSourceLinks, ASTEmap[innerActivity->id]->enclosedSourceLinks);

        foreach(otherbranch=OnMessage(*,*,secondActivity,*); tOnMessage_list b)
        {
          enterConflictingActivities( ASTEmap[ innerActivity->id ]->enclosedActivities, ASTEmap[ secondActivity->id ]->enclosedActivities );
        }

        foreach(otherbranch=OnAlarm(secondActivity); tOnAlarm_list c)
        {
          if (innerActivity->id < secondActivity->id)
          {
            enterConflictingActivities( ASTEmap[ innerActivity->id ]->enclosedActivities, ASTEmap[ secondActivity->id ]->enclosedActivities );
          }
        }
      }


      ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myid );
      ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myself->enclosedActivities.begin(), myself->enclosedActivities.end() );
      enterEnclosedActivities( myid, ASTEmap[ myid ]->enclosedActivities );
    }
  ]
;


/******************************************************************************
  FOREACH                                                        (WS-BPEL 2.0)
******************************************************************************/

ForEach(a,b,c,d,e,_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      ASTE *myself = ASTEmap[myid];
      assert(myself != NULL);

      a->parentId = myid;

      myself->parentScopeId = PPcurrentScope;
      myself->checkAttributes();

      /* above is generic */

      myself->checkVariable("counterName");

      // distribute the forEach's id to the enclosed activity
      assert(ASTEmap[e->id] != NULL);
      ASTEmap[e->id]->parentActivityId = myid;
      ASTEmap[e->id]->controlFlow = myself->controlFlow;

      // trigger [SA00074] and [SA00075]
      unsigned int startCounter = toUInt(b->name);
      unsigned int finalCounter = toUInt(c->name);
      unsigned int completion = toUInt(d->name);
      if (startCounter != UINT_MAX && finalCounter != UINT_MAX)
      {
        if (startCounter >= finalCounter)
        {
          string error = "constant <finalCounterValue> `" + toString(finalCounter) +
             "' less or equal than constant <startCounterValue> `" + toString(startCounter) + "'";
          SAerror(74, error, myself->attributes["referenceLine"]);
        }
        else
        {
          if (myself->attributes["hu:maxloops"] != "")
            cerr << "ignoring attribute `hu:maxloops'" << endl; //TODO: make this a nice error message
          ASTEmap[e->id]->max_occurrences = myself->max_occurrences * (finalCounter - startCounter);
          myself->max_loops = (finalCounter - startCounter);
        }

        if ((completion != UINT_MAX) && ((completion > (finalCounter-startCounter))))
          SAerror(75, "", myself->attributes["referenceLine"]);
      }
      else
      {
  	// calculate the maximal occurrences of the enclosed activity
    	unsigned int max_loops = toUInt(myself->attributes["hu:maxloops"]);
      	if (max_loops != UINT_MAX && myself->max_occurrences != UINT_MAX)
	  ASTEmap[e->id]->max_occurrences = myself->max_occurrences * max_loops;
	else
	  ASTEmap[e->id]->max_occurrences = UINT_MAX;

        myself->max_loops = max_loops;
       }
    }
    a e
    {
      // trigger [SA00076]
      with (e)
      {
        e=Scope(*,_variables,*,*,*,*,*,*,*,*,*):
          {
            foreach (variable=Variable(_id2); tVariable_list _variables)
            {
              assert(ASTEmap[_id2->value] != NULL);
              if (ASTEmap[_id2->value]->attributes["name"] == ASTEmap[_id->value]->attributes["counterName"])
                SAerror(76, ASTEmap[_id2->value]->attributes["name"], ASTEmap[_id2->value]->attributes["referenceLine"]);
            }            
          }
      }

      // collect enclosed source links
      assert(ASTEmap[e->id] != NULL);
      myself->enclosedSourceLinks = setUnion(myself->enclosedSourceLinks, ASTEmap[e->id]->enclosedSourceLinks);

      ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myid );
      ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myself->enclosedActivities.begin(), myself->enclosedActivities.end() );
      enterEnclosedActivities( myid, ASTEmap[ myid ]->enclosedActivities );
    }
  ]
;


/******************************************************************************
  SCOPE
******************************************************************************/

Scope(a,b,c,d,e,f,g,h,i,j,_id) ->
  [postprocessing:
    {
      int myid = _id->value;
      ASTE *myself = ASTEmap[myid];
      assert(myself != NULL);
      a->parentId = myid;

      // my parent is the current scope
      myself->parentScopeId = PPcurrentScope;

      // now I am the current scope
      PPcurrentScope = myid;

      assert(ASTEmap[myself->parentActivityId] != NULL);
      assert(ASTEmap[myself->parentScopeId] != NULL);
      myself->checkAttributes();

      for ( std::set<unsigned int>::iterator peerScope = ASTEmap[myself->parentScopeId]->enclosedScopes.begin(); 
	    peerScope != ASTEmap[myself->parentScopeId]->enclosedScopes.end(); 
	    peerScope++)
      {
	ASTEmap[*peerScope]->peerScopes.insert(myid);
	ASTEmap[myid]->peerScopes.insert(*peerScope);
      }

      ASTEmap[myself->parentScopeId]->enclosedScopes.insert(myid);

      myself->hasEH = (string(e->op_name()) == "userDefinedEventHandler");


      // distribute the scopes' id to the enclosed FH/CH/EH/TH/activity
      assert(ASTEmap[c->id] != NULL);
      assert(ASTEmap[d->id] != NULL);
      assert(ASTEmap[e->id] != NULL);
      assert(ASTEmap[f->id] != NULL);
      assert(ASTEmap[i->id] != NULL);
      ASTEmap[c->id]->parentActivityId = myid;
      ASTEmap[d->id]->parentActivityId = myid;
      ASTEmap[e->id]->parentActivityId = myid;
      ASTEmap[f->id]->parentActivityId = myid;
      ASTEmap[i->id]->parentActivityId = myid;

      ASTEmap[c->id]->max_occurrences = myself->max_occurrences;
      ASTEmap[d->id]->max_occurrences = myself->max_occurrences;
      ASTEmap[e->id]->max_occurrences = myself->max_occurrences;
      ASTEmap[f->id]->max_occurrences = myself->max_occurrences;
      ASTEmap[i->id]->max_occurrences = myself->max_occurrences;

      // remember the scope's name for compensation
      if (myself->attributes["name"] != "")
      {
        // trigger [SA00092]
        if (scopeNames[myself->attributes["name"]] != 0)
          SAerror(92, myself->attributes["name"], myself->attributes["referenceLine"]);
        
        scopeNames[myself->attributes["name"]] = myid;
      }

      // trigger [SA00091]
      if (myself->attributes["isolated"] == "yes" &&
          ASTEmap[myself->parentScopeId]->findIsolatedAncestor())
        SAerror(91, "", myself->attributes["referenceLine"]);
    }
    a b j g
    i // inner activity
    c d e f h
    {
      // add an attribute to the compensation handler
      if(PPhasCompensateMap[myid] == 1)
        d->hasCompensateWithoutScope = true;
      if(PPhasCompensateMap[myid] == 2)
        d->hasCompensateWithScope = true;

      set< unsigned int > innerIds = ASTEmap[ i->id ]->enclosedActivities;
      innerIds.insert( i->id );
      set< unsigned int > faultIds = ASTEmap[ c->id ]->enclosedActivities;
      faultIds.insert( c->id );
      set< unsigned int > compIds = ASTEmap[ d->id ]->enclosedActivities;
      compIds.insert( d->id );

      enterConflictingActivities( innerIds, faultIds );
      enterConflictingActivities( faultIds, compIds );

      // pass list of enclosed scopes to the compensation handler
      ASTEmap[d->id]->enclosedScopes = myself->enclosedScopes;

      // collect enclosed source links
      assert(ASTEmap[i->id] != NULL);
      myself->enclosedSourceLinks = setUnion(myself->enclosedSourceLinks, ASTEmap[i->id]->enclosedSourceLinks);

      // everybody after me has the same parent scope as me
      PPcurrentScope = myself->parentScopeId;

      ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myid );
      ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myself->enclosedActivities.begin(), myself->enclosedActivities.end() );
      enterEnclosedActivities( myid, ASTEmap[ myid ]->enclosedActivities );
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

      set<string> targetLinks;
      foreach(targetLink; tTarget_list a)
      {
        assert(ASTEmap[targetLink->id] != NULL);
        ASTEmap[targetLink->id]->controlFlow = ASTEmap[$0->parentId]->controlFlow;
        ASTEmap[targetLink->id]->parentActivityId = $0->parentId;

        // trigger [SA00069]
        string linkName = ASTEmap[targetLink->id]->attributes["linkName"];
        if (targetLinks.find(linkName) != targetLinks.end())
          SAerror(69, "<" + ASTEmap[$0->parentId]->activityTypeName() + "> must not use <link> `" +
                  linkName + "'", ASTEmap[linkIdMap[linkName]]->attributes["referenceLine"]);
        else
          targetLinks.insert(linkName);
      }

      set<string> sourceLinks;
      foreach(sourceLink; tSource_list b)
      {
        assert(ASTEmap[sourceLink->id] != NULL);
        ASTEmap[sourceLink->id]->controlFlow = ASTEmap[$0->parentId]->controlFlow;
        ASTEmap[sourceLink->id]->parentActivityId = $0->parentId;
        
        // trigger [SA00068]
        string linkName = ASTEmap[sourceLink->id]->attributes["linkName"];
        if (sourceLinks.find(linkName) != sourceLinks.end())
          SAerror(68, "<" + ASTEmap[$0->parentId]->activityTypeName() + "> must not use <link> `" +
                  linkName + "'", ASTEmap[linkIdMap[linkName]]->attributes["referenceLine"]);
        else
          sourceLinks.insert(linkName);
      }
    }
    a b c
  ]
;

Target(a) ->
  [postprocessing:
    {
      ASTE *myself = ASTEmap[$0->id];
      assert(myself != NULL);

      myself->checkAttributes();
      myself->linkName = myself->checkLink();

      // find the id of the link used as target here and add that link to the
      // parent activity
      unsigned int linkId = linkIdMap[myself->attributes["linkName"]];
      assert(ASTEmap[myself->parentActivityId] != NULL);
      ASTEmap[myself->parentActivityId]->targetLinks.insert(linkId);

      // trigger [SA00066]
      assert(ASTEmap[linkId] != NULL);
      if (ASTEmap[linkId]->targetActivity != 0)
        SAerror(66, myself->attributes["linkName"], myself->attributes["referenceLine"]);
      else
        ASTEmap[linkId]->targetActivity = myself->parentActivityId;
    }
  ]
;

Source(a) ->
  [postprocessing:
    {
      ASTE *myself = ASTEmap[$0->id];
      assert(myself != NULL);

      myself->checkAttributes();
      myself->linkName = myself->checkLink();

      // find the id of the link used as source here and add that link to the
      // parent activity
      assert(ASTEmap[myself->parentActivityId] != NULL);
      unsigned int linkId = linkIdMap[myself->attributes["linkName"]];
      ASTEmap[myself->parentActivityId]->enclosedSourceLinks.insert(linkId);
      ASTEmap[myself->parentActivityId]->sourceLinks.insert(linkId);

      // trigger [SA00066]
      assert(ASTEmap[linkId] != NULL);
      if (ASTEmap[linkId]->sourceActivity != 0)
        SAerror(66, myself->attributes["linkName"], myself->attributes["referenceLine"]);
      else
        ASTEmap[linkId]->sourceActivity = myself->parentActivityId;
    }
  ]
;

