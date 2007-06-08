/*****************************************************************************\
 * Copyright 2007       Niels Lohmann                                        *
 * Copyright 2005, 2006 Niels Lohmann, Christian Gierds                      *
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
 * \file    ast-postprocess.cc
 *
 * \brief   AST postprocessor (implementation)
 *
 * \author  Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          Christian Gierds <gierds@informatik.hu-berlin.de>,
 *          last changes of: \$Author: gierds $
 * 
 * \since   2006-07-02
 *
 * \date    \$Date: 2007/06/08 10:39:15 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \note    This file was created using Kimwitu++ reading file bpel-unparse.k.
 *          See http://site.informatik.hu-berlin.de/kimwitu++ for details.
 *
 * \version \$Revision: 1.186 $
 */
%}





/******************************************************************************
  PROCESS
******************************************************************************/

Process(a,b,messageExchange,c,d,e,g,i,_id) ->
  [postprocessing:
    {
      assert(_id != NULL);
      unsigned int myid = _id->value;
      ASTE *myself = globals::ASTEmap[myid];
      assert(myself != NULL);

      

      myself->parentScopeId = myid;
      myself->parentActivityId = myid;
      globals::PPcurrentScope = myid;

      // annotate AST and check information
      myself->checkAttributes();
      myself->hasEH = (string(g->op_name()) == "EventHandlers");

      // distribute the process' id to the enclosed FH/TH/EH/activity
      assert(globals::ASTEmap[e->id] != NULL);
      assert(globals::ASTEmap[g->id] != NULL);
      assert(globals::ASTEmap[i->id] != NULL);
      globals::ASTEmap[e->id]->parentActivityId = myid;
      globals::ASTEmap[g->id]->parentActivityId = myid;
      globals::ASTEmap[i->id]->parentActivityId = myid;

      myself->enclosedFH = e->id;
    }
    a b messageExchange c d
    i // inner activity
    e g
    {

      bool callableFH = false;
      for ( set< string >::iterator fault = globals::ASTEmap[ e->id ]->catches.begin(); fault != globals::ASTEmap[ e->id ]->catches.end(); fault++)
      {
        string faultName = fault->substr( fault->find_first_of( "|" ) + 1 );
        faultName = faultName.substr( 0, faultName.find_first_of( "|" ) );
        if (faultName.find_first_of(":") < faultName.npos)
        {
          faultName = faultName.substr( faultName.find_first_of(":") + 1 );
        }
        if ( myself->possiblyTriggeredFaults.find( faultName ) != myself->possiblyTriggeredFaults.end() )
        {
          // cerr << "callable !" << endl;
          callableFH = true;
        }
      }
      if ( globals::ASTEmap[ e->id ]->hasCatchAll && ! myself->possiblyTriggeredFaults.empty() )
      {
        callableFH = true;
      }
      
      globals::ASTEmap[ e->id ]->callable = callableFH;

      // collect enclosed source and target links
      assert(globals::ASTEmap[i->id] != NULL);
      myself->enclosedSourceLinks = globals::ASTEmap[i->id]->enclosedSourceLinks;
      myself->enclosedSourceLinks = setUnion( myself->enclosedSourceLinks, globals::ASTEmap[e->id]->enclosedSourceLinks );
      myself->enclosedSourceLinks = setUnion( myself->enclosedSourceLinks, globals::ASTEmap[g->id]->enclosedSourceLinks );

      myself->enclosedTargetLinks = globals::ASTEmap[i->id]->enclosedTargetLinks;
      myself->enclosedTargetLinks = setUnion( myself->enclosedTargetLinks, globals::ASTEmap[e->id]->enclosedTargetLinks );
      myself->enclosedTargetLinks = setUnion( myself->enclosedTargetLinks, globals::ASTEmap[g->id]->enclosedTargetLinks );

      set< unsigned int > innerIds = globals::ASTEmap[ i->id ]->enclosedActivities;
      innerIds.insert( i->id );
      set< unsigned int > faultIds = globals::ASTEmap[ e->id ]->enclosedActivities;
      faultIds.insert( e->id );

      enterConflictingActivities( innerIds, faultIds );
//      enterConflictingActivities( faultIds, compIds );


      // trigger [SA00015]
      if (globals::ASTE_startActivities.empty())
        SAerror(15, "", myself->attributes["referenceLine"]);


      // trigger [SA00066]
      // should be actually calculated at a <flow> activity, but "linksOK" is
      // used below to avoid an assertion failure
      bool linksOK = true;
      for(map<string, unsigned int>::iterator it = globals::ASTE_linkIdMap.begin(); it != globals::ASTE_linkIdMap.end(); it++)
      {
        if ( it-> first != "" )
        {
          assert(globals::ASTEmap[it->second] != NULL);
  
          if ((globals::ASTEmap[it->second]->targetActivity == 0) || (globals::ASTEmap[it->second]->sourceActivity == 0))
          {
            SAerror(66, it->first, globals::ASTEmap[it->second]->attributes["referenceLine"]);
            linksOK = false;
          }

          // trigger [SA00067]
          if (linksOK)
          {
            assert(globals::ASTEmap[globals::ASTEmap[it->second]->sourceActivity] != NULL);
            assert(globals::ASTEmap[globals::ASTEmap[it->second]->targetActivity] != NULL);
            set<unsigned int> otherSources = globals::ASTEmap[globals::ASTEmap[it->second]->sourceActivity]->sourceLinks;
            set<unsigned int> otherTargets = globals::ASTEmap[globals::ASTEmap[it->second]->targetActivity]->targetLinks;
            set<unsigned int> jointLinks = setIntersection(otherSources, otherTargets);
            jointLinks.erase(it->second);

            if (jointLinks.size() > 0)
              SAerror(67, "`" + it->first + "' and <link> `" + globals::ASTEmap[*(jointLinks.begin())]->attributes["name"] + "'", globals::ASTEmap[it->second]->attributes["referenceLine"]);
          }
        }
      }


      // trigger [SA00057]
      set<string> reallySharedCorrelationSets;
      if (globals::ASTE_startActivities.size() > 1)
      {
  	for (set<unsigned int>::iterator it = globals::ASTE_startActivities.begin(); it != globals::ASTE_startActivities.end(); it++)
  	{
  	  if (it == globals::ASTE_startActivities.begin())
    	    reallySharedCorrelationSets = globals::ASTEmap[*it]->sharedCorrelationSets;
  	  else
	    reallySharedCorrelationSets = setIntersection(reallySharedCorrelationSets, globals::ASTEmap[*it]->sharedCorrelationSets);

  	  if (reallySharedCorrelationSets.empty())
  	  {
  	    SAerror(57, "", globals::ASTEmap[*it]->attributes["referenceLine"]);
      	    break;
    	  }
  	}
      }


      // this assertion guarantees that we did not forget a link and encounter
      // problems generating the Petri net
      if (linksOK)
      {
        assert(myself->enclosedSourceLinks.size() == globals::ASTE_linkNames.size());
        assert(myself->enclosedTargetLinks.size() == globals::ASTE_linkNames.size());
      }


      // clear global maps
      globals::ASTE_correlationSetNames.clear(); // correlation sets are only tested but not used for output
      globals::ASTE_partnerLinkNames.clear();    // the same with partner links
      globals::ASTE_startActivities.clear();	// the same with start activities
    }
  ]
;


/******************************************************************************
  PARTNER LINKS
******************************************************************************/

PartnerLink(_id) ->
  [postprocessing:
    {
      assert(_id != NULL);
      unsigned int myid = _id->value;
      ASTE *myself = globals::ASTEmap[myid];
      assert(myself != NULL);

      myself->parentScopeId = globals::PPcurrentScope;
      myself->checkAttributes();
      
      //Correct ordering of roles in the WSDLInfo
      if(globals::WSDLInfo.partnerLinkTypes[myself->attributes["partnerLinkType"]] != NULL)
        globals::WSDLInfo.partnerLinkTypes[myself->attributes["partnerLinkType"]]->correctOrder(myself);

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

FaultHandlers(a,b,_id) ->
  [postprocessing:
    {
      assert(_id != NULL);
      unsigned int myid = _id->value;
      ASTE *myself = globals::ASTEmap[myid];
      assert(myself != NULL);

      

      myself->parentScopeId = globals::PPcurrentScope;
      myself->hasCatchAll = (string(b->op_name()) == "CatchAll");
      
//      myself->inProcess = (myself->parentScopeId == 1);

      // distribute the fault handlers's id to the enclosed catch/catchAll branches
      // also mark the activities as "inside fault handlers"
      foreach(branch=Catch(innerActivity); tCatch_list a)
      {
        assert(globals::ASTEmap[innerActivity->id] != NULL);
        globals::ASTEmap[innerActivity->id]->parentActivityId = myid; // this is not intended: the activity should be child of the catch
        globals::ASTEmap[branch->id]->parentActivityId = myid;
        globals::ASTEmap[branch->id]->parentScopeId = myself->parentScopeId;
        globals::ASTEmap[innerActivity->id]->controlFlow = FHCF;
        globals::ASTEmap[innerActivity->id]->max_occurrences = myself->max_occurrences;
      }

      // check whether a <catchAll>-branch with an enclosed activity is present
      // and if yes, annotate it
      with (b)
      {
        CatchAll(c,_id) :
        {
          assert(globals::ASTEmap[c->id] != NULL);
//NL//          globals::ASTEmap[c->id]->parentActivityId = myid;
          globals::ASTEmap[b->id]->parentActivityId = myid;
          globals::ASTEmap[c->id]->controlFlow = FHCF;
          globals::ASTEmap[c->id]->max_occurrences = myself->max_occurrences;
        }
        default :
        {
          // triggers SA00080
          if (a->length() == 0)
            SAerror(80, "", toInt(globals::ASTEmap[myself->parentScopeId]->attributes["referenceLine"]));
        }
      }
    }
    a b
    {
      globals::ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myid );
      globals::ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myself->enclosedActivities.begin(), myself->enclosedActivities.end() );
      enterEnclosedActivities( myid, globals::ASTEmap[ myid ]->enclosedActivities );

      foreach(branch=Catch(innerActivity,_id2); tCatch_list a)
      {
        foreach(otherbranch=Catch(secondActivity,_id3); tCatch_list a)
        {
          assert(_id2 != NULL);
          assert(_id3 != NULL);
          if ( _id2->value < _id3->value )
          {
            enterConflictingActivities( globals::ASTEmap[ _id2->value ]->enclosedActivities, globals::ASTEmap[ _id3->value ]->enclosedActivities );
          }
        }
      }

      with (b)
      {
        CatchAll(c,_id2) :
        {
          foreach(otherbranch=Catch(secondActivity,_id3); tCatch_list a)
          {
            assert(_id2 != NULL);
            assert(_id3 != NULL);
            enterConflictingActivities( globals::ASTEmap[ _id2->value ]->enclosedActivities, globals::ASTEmap[ _id3->value ]->enclosedActivities );
          }
        }
        default :
        {
        }
      }

      // count this fault handlers
      if (! myself->isUserDefined)
        globals::process_information.implicit_handlers++;
    }
  ]
;

Catch(a,_id) ->
  [postprocessing:
    {
      assert(_id != NULL);
      unsigned int myid = _id->value;
      ASTE *myself = globals::ASTEmap[myid];
      assert(myself != NULL);

      myself->checkAttributes();
      
      //Check for SAerror 00003
      if (globals::ASTEmap[globals::PPcurrentScope]->attributes["exitOnStandardFault"] == "yes")
      {
        if ( myself->attributes["faultName"] == "ambiguousReceive" ||
             myself->attributes["faultName"] == "completionConditionFailure" ||
             myself->attributes["faultName"] == "conflictingReceive" ||
             myself->attributes["faultName"] == "conflictingRequest" ||
             myself->attributes["faultName"] == "correlationViolation" ||
             myself->attributes["faultName"] == "invalidBranchCondition" ||
             myself->attributes["faultName"] == "invalidExpressionValue" ||
             myself->attributes["faultName"] == "invalidVariables" ||
             myself->attributes["faultName"] == "joinFailurea" ||
             myself->attributes["faultName"] == "mismatchedAssignmentFailurei" ||
             myself->attributes["faultName"] == "missingReply" ||
             myself->attributes["faultName"] == "missingRequest" ||
             myself->attributes["faultName"] == "scopeInitializationFailure" ||
             myself->attributes["faultName"] == "selectionFailure" ||
             myself->attributes["faultName"] == "subLanguageExecutionFaultx" ||
             myself->attributes["faultName"] == "uninitializedPartnerRole" ||
             myself->attributes["faultName"] == "uninitializedVariablez" ||
             myself->attributes["faultName"] == "unsupportedReference" ||
             myself->attributes["faultName"] == "xsltInvalidSourcel" ||
             myself->attributes["faultName"] == "xsltStylesheetNotFound" )
             
             SAerror( 3, myself->attributes["faultName"], myself->attributes["referenceLine"]);
      }
      
      // make fault variable visible
      if (myself->attributes["faultVariable"] != "")
      {
        globals::ASTE_variableNames.insert(toString(globals::PPcurrentScope) + "." + myself->attributes["faultVariable"]);
        if (myself->attributes["faultMessageType"] != "")
          globals::ASTE_variableMap[(toString(globals::PPcurrentScope) + "." + myself->attributes["faultVariable"])] = myself->attributes["faultMessageType"];
      }
    }
    a
    {
      // collect enclosed source and target links
      assert(globals::ASTEmap[a->id] != NULL);
      globals::ASTEmap[ myself->parentActivityId ]->enclosedSourceLinks = setUnion(globals::ASTEmap[ myself->parentActivityId ]->enclosedSourceLinks, globals::ASTEmap[a->id]->enclosedSourceLinks);
      globals::ASTEmap[ myself->parentActivityId ]->enclosedTargetLinks = setUnion(globals::ASTEmap[ myself->parentActivityId ]->enclosedTargetLinks, globals::ASTEmap[a->id]->enclosedTargetLinks);

      myself->enclosedActivities = globals::ASTEmap[ a->id ]->enclosedActivities;
      myself->enclosedActivities.insert( a->id );
      enterEnclosedActivities( myid, globals::ASTEmap[ myid ]->enclosedActivities );

      globals::ASTEmap[a->id]->parentActivityId = myid; // necessary/safe?

      // necessary?
      globals::ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myid );      

      check_SA00071( myid );
    }
  ]
;

CatchAll(a,_id) ->
  [postprocessing:
    {
      assert(_id != NULL);
      unsigned int myid = _id->value;
      ASTE *myself = globals::ASTEmap[myid];
      assert(myself != NULL);
      globals::ASTEmap[a->id]->parentActivityId = myid;
    }
    a
    {
      // collect enclosed source and target links
      assert(globals::ASTEmap[a->id] != NULL);
      globals::ASTEmap[ myself->parentActivityId ]->enclosedSourceLinks = setUnion(globals::ASTEmap[ myself->parentActivityId ]->enclosedSourceLinks, globals::ASTEmap[a->id]->enclosedSourceLinks);
      globals::ASTEmap[ myself->parentActivityId ]->enclosedTargetLinks = setUnion(globals::ASTEmap[ myself->parentActivityId ]->enclosedTargetLinks, globals::ASTEmap[a->id]->enclosedTargetLinks);

      myself->enclosedActivities = globals::ASTEmap[ a->id ]->enclosedActivities;
      myself->enclosedActivities.insert( a->id );
      enterEnclosedActivities( myid, globals::ASTEmap[ myid ]->enclosedActivities );

      globals::ASTEmap[a->id]->parentActivityId = myid; // necessary/safe?

      // necessary?
      globals::ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myid );      

      check_SA00071( myid );
    }
  ]
;


/******************************************************************************
  EVENT HANDLERS
******************************************************************************/

emptyEventHandlers(_id) ->
  [postprocessing:
    {
      assert(_id != NULL);
      unsigned int myid = _id->value;
      ASTE *myself = globals::ASTEmap[myid];
      assert(myself != NULL);

      

      myself->parentScopeId = globals::PPcurrentScope;

      globals::ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myid );
    }
  ]
;

EventHandlers(a,b,_id) ->
  [postprocessing:
    {
      assert(_id != NULL);
      unsigned int myid = _id->value;
      ASTE *myself = globals::ASTEmap[myid];
      assert(myself != NULL);

      

      myself->parentScopeId = globals::PPcurrentScope;
      //BAM myself->max_occurrences = UINT_MAX; 
      
      // triggers SA00083
      if (a->length() == 0 && b->length() == 0)
        SAerror(83, "", toInt(globals::ASTEmap[myself->parentScopeId]->attributes["referenceLine"]));

      // distribute the event handlers's id to the enclosed activities
      foreach(branch=OnMessage(_cor,_from,_act,_id2); tOnMessage_list a)
      {
        assert(_id2 != NULL);
        assert(globals::ASTEmap[_id2->value] != NULL);
        globals::ASTEmap[_id2->value]->parentActivityId = myid;
        globals::ASTEmap[_id2->value]->controlFlow = myself->controlFlow;
        globals::ASTEmap[_id2->value]->max_occurrences = myself->max_occurrences;
        
      }
      foreach(branch=OnAlarm(*,innerActivity, _id3); tOnAlarm_list b)
      {
        assert(globals::ASTEmap[innerActivity->id] != NULL);
        globals::ASTEmap[innerActivity->id]->parentActivityId = myid;
        globals::ASTEmap[_id3->value]->parentActivityId = myid;
        globals::ASTEmap[innerActivity->id]->controlFlow = myself->controlFlow;
        globals::ASTEmap[_id3->value]->max_occurrences = myself->max_occurrences;
      }
    }
    a b
    {
      globals::ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myid );
      globals::ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myself->enclosedActivities.begin(), myself->enclosedActivities.end() );
      enterEnclosedActivities( myid, globals::ASTEmap[ myid ]->enclosedActivities );

      check_SA00070( myid );
    }
  ]
;

OnMessage(a,b,c,_id) ->
  [postprocessing:
    {
      assert(_id != NULL);
      unsigned int myid = _id->value;
      ASTE *myself = globals::ASTEmap[myid];
      assert(myself != NULL);

      

      myself->parentScopeId = globals::PPcurrentScope;
      assert(globals::ASTEmap[myself->parentActivityId] != NULL);
      myself->checkAttributes();

      myself->checkPartnerLink();

      myself->checkMessageExchange();

      globals::ASTE_IMAs.insert(myid);

      if ( globals::ASTE_partnerLinks[ myself->attributes["partnerLink"] ] != 0 )
      {
        globals::ASTEmap[ myself->parentScopeId]->enterFault( globals::ASTEmap[ globals::ASTE_partnerLinks[ myself->attributes["partnerLink"] ] ]->partnerLinkType );
      }
      if ( globals::parameters[P_STANDARDFAULTS] )
      {
        globals::ASTEmap[ myself->parentScopeId]->enterFault( ";" );
      }

      // trigger [SA00063]
      if (b->length() != 0 && myself->attributes["variable"] != "")
        SAerror(63, "", myself->attributes["referenceLine"]);

      // trigger [SA00005]
      if (globals::wsdl_filename != "")
      {
        if (myself->attributes["portType"] != "")
          if (!( globals::WSDLInfo.checkPortType( myself->attributes["partnerLink"],
                                                  globals::ASTEmap[globals::ASTE_partnerLinks[ myself->attributes["partnerLink"] ]]->attributes["myRole"],
                                                  myself->attributes["portType"]) ) )
            SAerror(5, ("portType `" + myself->attributes["portType"] +"' referenced in <onMessage>"),myself->attributes["referenceLine"]);
        // trigger [SA00058]
        globals::WSDLInfo.checkVariable(myself);
      }

      myself->channelName = myself->createChannel();
      // checkVariable does not work correctly for onEvent since onEvent-variable does not have to be defined previously
      //myself->variableName = myself->checkVariable("variable");

      assert(globals::ASTEmap[c->id] != NULL);
      globals::ASTEmap[c->id]->parentActivityId = myid;

      //Processing max occurrences
      unsigned int max_loops = toUInt(myself->attributes["hu:maxloops"]);
      if(globals::ASTEmap[myself->parentActivityId]->activityTypeName() == "eventHandlers")
      {
        if(max_loops != UINT_MAX && myself->max_occurrences != UINT_MAX)
        {
            myself->max_occurrences = max_loops * myself-> max_occurrences;
        } 
        else
        {
            myself->max_occurrences = UINT_MAX;
        }
      }

        globals::ASTEmap[c->id]->max_occurrences = myself->max_occurrences;

//      occurrences output for debugging  
//      cerr << myself->attributes["operation"] << " hat an seinem Kind " << globals::ASTEmap[c->id]->activityTypeName() << " den Wert "  << globals::ASTEmap[c->id]->max_occurrences << " uebergeben\n";


      foreach(correlation=Correlation(_id2); tCorrelation_list a)
      {
        assert(_id2 != NULL);
        assert(globals::ASTEmap[_id2->value] != NULL);
        globals::ASTEmap[_id2->value]->parentActivityId = myid;
      }
    }
    a b c
    {
      // collect enclosed source and target links
      assert(globals::ASTEmap[c->id] != NULL);
      globals::ASTEmap[ myself->parentActivityId ]->enclosedSourceLinks = setUnion(globals::ASTEmap[ myself->parentActivityId ]->enclosedSourceLinks, globals::ASTEmap[c->id]->enclosedSourceLinks);
      globals::ASTEmap[ myself->parentActivityId ]->enclosedTargetLinks = setUnion(globals::ASTEmap[ myself->parentActivityId ]->enclosedTargetLinks, globals::ASTEmap[c->id]->enclosedTargetLinks);

      globals::ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myid );
      globals::ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myself->enclosedActivities.begin(), myself->enclosedActivities.end() );
      enterEnclosedActivities( myid, globals::ASTEmap[ myid ]->enclosedActivities );
    }
  ]
;

OnAlarm(b,a,_id) ->
  [postprocessing: 
    {
      assert(_id != NULL);
      unsigned int myid = _id->value;
      ASTE *myself = globals::ASTEmap[myid];
      assert(myself != NULL);

      

      myself->parentScopeId = globals::PPcurrentScope;
      assert(globals::ASTEmap[myself->parentActivityId] != NULL);

      if(string(b->op_name())=="RepeatEvery")
      {
        myself->cyclic=true;
        globals::ASTEmap[a->id]->max_occurrences = UINT_MAX;
      } 
      else
      {
      //Processing max occurrences
      unsigned int max_loops = toUInt(myself->attributes["hu:maxloops"]);
      if (max_loops != UINT_MAX && myself->max_occurrences != UINT_MAX)
        globals::ASTEmap[a->id]->max_occurrences = myself->max_occurrences * max_loops;
      else
        globals::ASTEmap[a->id]->max_occurrences = myself->max_occurrences;
      }


      assert(globals::ASTEmap[a->id] != NULL);
      globals::ASTEmap[a->id]->parentActivityId = myid;
    }
    a
    {
      // collect enclosed source and target links
      assert(globals::ASTEmap[a->id] != NULL);
      globals::ASTEmap[ myself->parentActivityId ]->enclosedSourceLinks = setUnion(globals::ASTEmap[ myself->parentActivityId ]->enclosedSourceLinks, globals::ASTEmap[a->id]->enclosedSourceLinks);
      globals::ASTEmap[ myself->parentActivityId ]->enclosedTargetLinks = setUnion(globals::ASTEmap[ myself->parentActivityId ]->enclosedTargetLinks, globals::ASTEmap[a->id]->enclosedTargetLinks);

      globals::ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myid );
      globals::ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myself->enclosedActivities.begin(), myself->enclosedActivities.end() );
      enterEnclosedActivities( myid, globals::ASTEmap[ myid ]->enclosedActivities );
    }
  ]
;


/******************************************************************************
  COMPENSATION HANDLERS
******************************************************************************/

CompensationHandler(a,_id) ->
  [postprocessing:
    {
      assert(_id != NULL);
      unsigned int myid = _id->value;
      ASTE *myself = globals::ASTEmap[myid];
      assert(myself != NULL);

      

      myself->parentScopeId = globals::PPcurrentScope;

      // distribute the compensation handlers's id to the enclosed activity
      // also mark the activities as "inside compensation handlers"
      assert(globals::ASTEmap[a->id] != NULL);
      globals::ASTEmap[a->id]->parentActivityId = myid;
      globals::ASTEmap[a->id]->controlFlow = CHCF;
      globals::ASTEmap[a->id]->max_occurrences = myself->max_occurrences;
    }
    a
    {
      // collect enclosed source and target links
      assert(globals::ASTEmap[a->id] != NULL);
      myself->enclosedSourceLinks = setUnion(myself->enclosedSourceLinks, globals::ASTEmap[a->id]->enclosedSourceLinks);
      myself->enclosedTargetLinks = setUnion(myself->enclosedTargetLinks, globals::ASTEmap[a->id]->enclosedTargetLinks);

      globals::ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myid );
      globals::ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myself->enclosedActivities.begin(), myself->enclosedActivities.end() );
      enterEnclosedActivities( myid, globals::ASTEmap[ myid ]->enclosedActivities );

      // necessary?
      globals::ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myid );      

      check_SA00070( myid );

      /* Debugging hilfe
      if (myself->isUserDefined)
        cerr<<"Ist userdefined\n";

      if (!myself->isUserDefined)
        cerr<<"ist nicht userdefined\n";
      */

      // trigger SA00079
      vector<unsigned int> ancestors = myself->ancestorActivities();
      if (myself->isUserDefined && ancestors.size() > 1)
      {
      	unsigned int i = 1;
       	while(i < ancestors.size())
      	{
      	  string ancestor = globals::ASTEmap[ancestors[i]]->activityTypeName();
    	  if (ancestor == "catch" ||
  	      ancestor == "catchAll" ||
  	      ancestor == "terminationHandler" ||
  	      ancestor == "compensationHandler")
          {
  	    SAerror(79, ancestor);
  	    break;
          }
          i++;
        }
      }

      // count this compensation handler
      if (! myself->isUserDefined)
        globals::process_information.implicit_handlers++;
    }
  ]
;


/******************************************************************************
  TERMINATION HANDLER                                            (WS-BPEL 2.0)
******************************************************************************/

TerminationHandler(a,_id) ->
  [postprocessing:
    {
      assert(_id != NULL);
      unsigned int myid = _id->value;
      ASTE *myself = globals::ASTEmap[myid];
      assert(myself != NULL);

      

      myself->parentScopeId = globals::PPcurrentScope;

      // distribute the termination handlers's id to the enclosed activity
      // also mark the activities as "inside termination handlers"
      assert(globals::ASTEmap[a->id] != NULL);
      globals::ASTEmap[a->id]->parentActivityId = myid;
      globals::ASTEmap[a->id]->controlFlow = THCF;
      globals::ASTEmap[a->id]->max_occurrences = myself->max_occurrences;
    }
    a
    {
      // collect enclosed source and target links
      assert(globals::ASTEmap[a->id] != NULL);
      myself->enclosedSourceLinks = setUnion(myself->enclosedSourceLinks, globals::ASTEmap[a->id]->enclosedSourceLinks);
      myself->enclosedTargetLinks = setUnion(myself->enclosedTargetLinks, globals::ASTEmap[a->id]->enclosedTargetLinks);

      globals::ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myid );
      globals::ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myself->enclosedActivities.begin(), myself->enclosedActivities.end() );
      enterEnclosedActivities( myid, globals::ASTEmap[ myid ]->enclosedActivities );

      check_SA00071( myid );

      // count this termination handler
      if (! myself->isUserDefined)
        globals::process_information.implicit_handlers++;
    }
  ]
;





/******************************************************************************
  VARIABLES
******************************************************************************/

Variable(_id) ->
  [postprocessing:
    {
      assert(_id != NULL);
      unsigned int myid = _id->value;
      ASTE *myself = globals::ASTEmap[myid];
      assert(myself != NULL);

      myself->parentScopeId = globals::PPcurrentScope;
      myself->checkAttributes();
      myself->defineVariable();
    }
  ]
;


/******************************************************************************
  MESSAGE EXCHANGES
******************************************************************************/

MessageExchange(_id) ->
  [postprocessing:
    {
      assert(_id != NULL);
      unsigned int myid = _id->value;
      ASTE *myself = globals::ASTEmap[myid];
      assert(myself != NULL);

      myself->parentScopeId = globals::PPcurrentScope;
      myself->checkAttributes();
      myself->defineMessageExchange();
    }
  ]
;


/******************************************************************************
  CORRELATION SETS
******************************************************************************/

CorrelationSet(_id) ->
  [postprocessing:
    {
      assert(_id != NULL);
      unsigned int myid = _id->value;
      ASTE *myself = globals::ASTEmap[myid];
      assert(myself != NULL);

      myself->parentScopeId = globals::PPcurrentScope;
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
      assert(_id != NULL);
      unsigned int myid = _id->value;
      ASTE *myself = globals::ASTEmap[myid];
      assert(myself != NULL);

      myself->parentScopeId = globals::PPcurrentScope;
      myself->checkAttributes();
      string correlationSetName = myself->checkCorrelationSet();

      // needed to trigger [SA00057] later
      assert(globals::ASTEmap[myself->parentActivityId] != NULL);
      if (globals::ASTEmap[myself->parentActivityId]->isStartActivity &&
          myself->attributes["initiate"] == "join")
      {
        globals::ASTEmap[myself->parentActivityId]->sharedCorrelationSets.insert(correlationSetName);
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
      assert(_id != NULL);
      unsigned int myid = _id->value;
      ASTE *myself = globals::ASTEmap[myid];
      assert(myself != NULL);

      myself->checkAttributes();
     }
  ]
;

FromPart(_id) ->
  [postprocessing:
    {
      assert(_id != NULL);
      unsigned int myid = _id->value;
      ASTE *myself = globals::ASTEmap[myid];
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
      assert(_id != NULL);
      unsigned int myid = _id->value;
      ASTE *myself = globals::ASTEmap[myid];
      assert(myself != NULL);

      a->parentId = myid;

      myself->parentScopeId = globals::PPcurrentScope;
      myself->checkAttributes();
    }
    a
    {
      globals::ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myid );
    }
  ]
;


/******************************************************************************
  EMPTY
******************************************************************************/

OpaqueActivity(a,_id) ->
  [postprocessing:
    {
      assert(_id != NULL);
      unsigned int myid = _id->value;
      ASTE *myself = globals::ASTEmap[myid];
      assert(myself != NULL);

      a->parentId = myid;

      myself->parentScopeId = globals::PPcurrentScope;
      myself->checkAttributes();
    }
    a
    {
      globals::ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myid );
    }
  ]
;


/******************************************************************************
  INVOKE
******************************************************************************/

volatile_Invoke(a,b,c,d,_id) ->
  [postprocessing:
    {
      assert(_id != NULL);
      unsigned int myid = _id->value;
      ASTE *myself = globals::ASTEmap[myid];
      assert(myself != NULL);

      a->parentId = myid;

      myself->parentScopeId = globals::PPcurrentScope;
      myself->checkAttributes();

      /* above is generic */

      // trigger [SA00051]
      if (c->length() != 0 && myself->attributes["inputVariable"] != "")
        SAerror(51, "", myself->attributes["referenceLine"]);

      // trigger [SA00052]
      if (d->length() != 0 && myself->attributes["outputVariable"] != "")
        SAerror(52, "", myself->attributes["referenceLine"]);

      if (globals::wsdl_filename != "")
      {
        // trigger [SA00005]
        if (myself->attributes["portType"] != "")
          if (!( globals::WSDLInfo.checkPortType( myself->attributes["partnerLink"],
                                                  globals::ASTEmap[globals::ASTE_partnerLinks[ myself->attributes["partnerLink"] ]]->attributes["partnerRole"],
                                                  myself->attributes["portType"]) ) )
            SAerror(5, ("portType `" + myself->attributes["portType"] +"' referenced in <invoke>"),myself->attributes["referenceLine"]);
        // trigger [SA00058]
        globals::WSDLInfo.checkVariable(myself);
      }

      myself->checkPartnerLink();
     
      if ( globals::ASTE_partnerLinks[ myself->attributes["partnerLink"] ] != 0 )
      {
        globals::ASTEmap[ myself->parentScopeId]->enterFault( globals::ASTEmap[ globals::ASTE_partnerLinks[ myself->attributes["partnerLink"] ] ]->partnerLinkType );
      }
      if ( globals::parameters[P_STANDARDFAULTS] )
      {
        globals::ASTEmap[ myself->parentScopeId]->enterFault( ";" );
      }

      myself->channelName = myself->createChannel((myself->attributes["outputVariable"] != ""));
      myself->inputVariableName = myself->checkVariable("inputVariable");
      myself->outputVariableName = myself->checkVariable("outputVariable");

      foreach(correlation=Correlation(_id2); tCorrelation_list b)
      {
        assert(_id2 != NULL);
        assert(globals::ASTEmap[_id2->value] != NULL);
        globals::ASTEmap[_id2->value]->parentActivityId = myid;
      }
    }
    a b
    {
      globals::ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myid );
    }
  ]
;


/******************************************************************************
  RECEIVE
******************************************************************************/

Receive(a,b,c,_id) ->
  [postprocessing:
    {
      assert(_id != NULL);
      unsigned int myid = _id->value;
      ASTE *myself = globals::ASTEmap[myid];
      assert(myself != NULL);

      a->parentId = myid;

      myself->parentScopeId = globals::PPcurrentScope;
      myself->checkAttributes();
      myself->checkMessageExchange();
      globals::ASTE_IMAs.insert(myid);
      
      /* above is generic */

      // trigger [SA00055]
      if (c->length() != 0 && myself->attributes["variable"] != "")
        SAerror(55, "", myself->attributes["referenceLine"]);

      // trigger WSDL dependent SAs
      if (globals::wsdl_filename != "")
      {
        // trigger [SA00005]
        if (myself->attributes["portType"] != "")
          if (!( globals::WSDLInfo.checkPortType( myself->attributes["partnerLink"],
                                                  globals::ASTEmap[globals::ASTE_partnerLinks[ myself->attributes["partnerLink"] ]]->attributes["myRole"],
                                                  myself->attributes["portType"]) ) )
            SAerror(5, ("portType `" + myself->attributes["portType"] +"' referenced in <receive>"), myself->attributes["referenceLine"]);
        // trigger [SA00058]
        globals::WSDLInfo.checkVariable(myself);
      }

      myself->checkPartnerLink();

      if ( globals::ASTE_partnerLinks[ myself->attributes["partnerLink"] ] != 0 )
      {
        globals::ASTEmap[ myself->parentScopeId]->enterFault( globals::ASTEmap[ globals::ASTE_partnerLinks[ myself->attributes["partnerLink"] ] ]->partnerLinkType );
      }
      if ( globals::parameters[P_STANDARDFAULTS] )
      {
        globals::ASTEmap[ myself->parentScopeId]->enterFault( ";" );
      }

      myself->channelName = myself->createChannel();
      myself->variableName = myself->checkVariable("variable");

      foreach(correlation=Correlation(_id2); tCorrelation_list b)
      {
        assert(_id2 != NULL);
        assert(globals::ASTEmap[_id2->value] != NULL);
        globals::ASTEmap[_id2->value]->parentActivityId = myid;
      }
    }
    a b
    {
      if (myself->isStartActivity)
      {
        globals::ASTE_startActivities.insert(myid);

        // trigger [SA00056]
        if (!globals::ASTEmap[myself->parentActivityId]->checkAncestors() || !myself->targetLinks.empty())
          SAerror(56, "<receive>", myself->attributes["referenceLine"]);
      }

      globals::ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myid );
    }
  ]
;


/******************************************************************************
  REPLY
******************************************************************************/

Reply(a,b,c,_id) ->
  [postprocessing:
    {
      assert(_id != NULL);
      unsigned int myid = _id->value;
      ASTE *myself = globals::ASTEmap[myid];
      assert(myself != NULL);

      a->parentId = myid;

      myself->parentScopeId = globals::PPcurrentScope;
      myself->checkAttributes();
      myself->checkMessageExchange();

      /* above is generic */

      // trigger [SA00005]
      if (globals::wsdl_filename != "")
      {
        if (myself->attributes["portType"] != "")
          if (!( globals::WSDLInfo.checkPortType( myself->attributes["partnerLink"],
                                                  globals::ASTEmap[globals::ASTE_partnerLinks[ myself->attributes["partnerLink"] ]]->attributes["myRole"],
                                                  myself->attributes["portType"]) ) )
            SAerror(5, ("portType `" + myself->attributes["portType"] +"' referenced in <reply>"), myself->attributes["referenceLine"]);
        // trigger [SA00058]
        globals::WSDLInfo.checkVariable(myself);
      }

      myself->checkPartnerLink();

      if ( globals::ASTE_partnerLinks[ myself->attributes["partnerLink"] ] != 0 )
      {
        globals::ASTEmap[ myself->parentScopeId]->enterFault( globals::ASTEmap[ globals::ASTE_partnerLinks[ myself->attributes["partnerLink"] ] ]->partnerLinkType );
      }
      if ( globals::parameters[P_STANDARDFAULTS] )
      {
        globals::ASTEmap[ myself->parentScopeId]->enterFault( ";" );
      }

      // trigger [SA00059]
      if (c->length() != 0 && myself->attributes["variable"] != "")
        SAerror(59, "", myself->attributes["referenceLine"]);

      myself->channelName = myself->createChannel();
      myself->variableName = myself->checkVariable("variable");

      foreach(correlation=Correlation(_id2); tCorrelation_list b)
      {
        assert(_id2 != NULL);
        assert(globals::ASTEmap[_id2->value] != NULL);
        globals::ASTEmap[_id2->value]->parentActivityId = myid;
      }
    }
    a b
    {
      globals::ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myid );
    }
  ]
;


/******************************************************************************
  ASSIGN
******************************************************************************/

Assign(a,b,_id) ->
  [postprocessing:
    {
      assert(_id != NULL);
      unsigned int myid = _id->value;
      ASTE *myself = globals::ASTEmap[myid];
      assert(myself != NULL);

      a->parentId = myid;

      myself->parentScopeId = globals::PPcurrentScope;
      myself->checkAttributes();
    }
    a b
    {
      globals::ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myid );
    }
  ]
;

Copy(a,b,c) ->
  [postprocessing: a b]
;

From(_id) ->
  [postprocessing:
    {
      assert(_id != NULL);
      unsigned int myid = _id->value;
      ASTE *myself = globals::ASTEmap[myid];
      assert(myself != NULL);

      // trigger [SA00032]
      if (myself->attributes["has_literal"] == "")
      {
        if(myself->attributes["has_expression"] == "")
        {
          if (myself->attributes["variable"] == "")
          {
            if (myself->attributes["endpointReference"] == "")
            {
              if (! (myself->attributes["property"] == "" && myself->attributes["partnerLink"] == ""
                  && myself->attributes["expressionLanguage"] == "" && myself->attributes["has_query"] == ""
                  && myself->attributes["part"] == ""))
                SAerror(32, "from", myself->attributes["referenceLine"]);
            }
            else
            {
              if (! (myself->attributes["property"] == "" && myself->attributes["partnerLink"] != ""
                  && myself->attributes["expressionLanguage"] == "" && myself->attributes["has_query"] == ""
                  && myself->attributes["part"] == ""))
                SAerror(32, "from", myself->attributes["referenceLine"]);
            }
          }
          else
          {
            if (! (myself->attributes["endpointReference"] == "" && myself->attributes["property"] != "" 
                && myself->attributes["partnerLink"] == "" && myself->attributes["expressionLanguage"] == ""
                && myself->attributes["has_query"] == "" && myself->attributes["part"] == "")
                &&
                ! (myself->attributes["endpointReference"] == "" && myself->attributes["property"] == "" 
                && myself->attributes["partnerLink"] == "" && myself->attributes["expressionLanguage"] == "")
                )
              SAerror(32, "from", myself->attributes["referenceLine"]);           
          }
        }
        else
        {
          if (! (myself->attributes["has_query"] == "" && myself->attributes["variable"] == "" 
              && myself->attributes["endpointReference"] == "" && myself->attributes["property"] == "" 
              && myself->attributes["partnerLink"] == ""&& myself->attributes["part"] == ""))
            SAerror(32, "from", myself->attributes["referenceLine"]);
        }
      }
      else
      {
        if (! (myself->attributes["has_query"] == "" && myself->attributes["has_expression"] == ""
            && myself->attributes["variable"] == "" && myself->attributes["endpointReference"] == ""
            && myself->attributes["property"] == "" && myself->attributes["partnerLink"] == ""
            && myself->attributes["expressionLanguage"] == "" && myself->attributes["part"] == ""))
          SAerror(32, "from", myself->attributes["referenceLine"]);
      }

      // trigger [SA00035]
      if (myself->attributes["endpointReference"] == "myRole")
        if(globals::ASTEmap[globals::ASTE_partnerLinks[myself->attributes["partnerLink"]]]->attributes["myRole"] == "")
          SAerror(35, myself->attributes["partnerLink"], myself->attributes["referenceLine"]);
          
      // trigger [SA00036]
      if (myself->attributes["endpointReference"] == "partnerRole")
        if(globals::ASTEmap[globals::ASTE_partnerLinks[myself->attributes["partnerLink"]]]->attributes["partnerRole"] == "")
          SAerror(36, myself->attributes["partnerLink"], myself->attributes["referenceLine"]);

      myself->parentScopeId = globals::PPcurrentScope;
      myself->checkAttributes();

      myself->checkPartnerLink();
      myself->variableName = myself->checkVariable("variable");
    }
  ]
;

To(_id) ->
  [postprocessing:
    {
      assert(_id != NULL);
      unsigned int myid = _id->value;
      ASTE *myself = globals::ASTEmap[myid];
      assert(myself != NULL);

      if(myself->attributes["has_expression"] == "")
      {
        if (myself->attributes["variable"] == "")
        {
          if (! (myself->attributes["property"] == "" && myself->attributes["expressionLanguage"] == "" 
              && myself->attributes["has_query"] == "" && myself->attributes["part"] == ""))
            SAerror(32, "to", myself->attributes["referenceLine"]);
        }
        else
        {
          if (! (myself->attributes["property"] != "" && myself->attributes["part"] == ""
              && myself->attributes["partnerLink"] == "" && myself->attributes["expressionLanguage"] == ""
              && myself->attributes["has_query"] == "")
              &&
              ! (myself->attributes["expressionLanguage"] == "" && myself->attributes["property"] == "" 
              && myself->attributes["partnerLink"] == "")
              )
            SAerror(32, "to", myself->attributes["referenceLine"]);           
        }
      }
      else
      {
        if (! (myself->attributes["has_query"] == "" && myself->attributes["variable"] == "" 
            && myself->attributes["part"] == "" && myself->attributes["property"] == "" 
            && myself->attributes["partnerLink"] == ""))
          SAerror(32, "to", myself->attributes["referenceLine"]);
      }

      // trigger [SA00037]
      if (myself->attributes["partnerLink"] != "")
        if(globals::ASTEmap[globals::ASTE_partnerLinks[myself->attributes["partnerLink"]]]->attributes["partnerRole"] == "")
          SAerror(37, myself->attributes["partnerLink"], myself->attributes["referenceLine"]);

      myself->parentScopeId = globals::PPcurrentScope;
      myself->checkAttributes();

      myself->checkPartnerLink();
      myself->variableName = myself->checkVariable("variable");
    }
  ]
;


/******************************************************************************
  VALIDATE
******************************************************************************/

Validate(a,_id) ->
  [postprocessing:
    {
      assert(_id != NULL);
      unsigned int myid = _id->value;
      ASTE *myself = globals::ASTEmap[myid];
      assert(myself != NULL);

      a->parentId = myid;

      myself->parentScopeId = globals::PPcurrentScope;
      myself->checkAttributes();
    }
    a
    {
      globals::ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myid );
    }
  ]
;


/******************************************************************************
  WAIT
******************************************************************************/

Wait(a,_id) ->
  [postprocessing:
    {
      assert(_id != NULL);
      unsigned int myid = _id->value;
      ASTE *myself = globals::ASTEmap[myid];
      assert(myself != NULL);

      a->parentId = myid;

      myself->parentScopeId = globals::PPcurrentScope;
      myself->checkAttributes();
    }
    a
    {
      globals::ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myid );
    }
  ]
;


/******************************************************************************
  THROW
******************************************************************************/

Throw(a,_id) ->
  [postprocessing:
    {
      assert(_id != NULL);
      unsigned int myid = _id->value;
      ASTE *myself = globals::ASTEmap[myid];
      assert(myself != NULL);

      a->parentId = myid;

      myself->parentScopeId = globals::PPcurrentScope;
      myself->checkAttributes();
      globals::ASTEmap[ myself->parentScopeId ]->enterFault( myself->attributes[ "faultName" ] );
    }
    a
    {
      globals::ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myid );

      // warn for non-existing Petri net patterns
      if ( (modus == M_PETRINET || modus == M_CHOREOGRAPHY)
         && globals::parameters[P_COMMUNICATIONONLY] )
	genericError(121, myself->activityTypeName(), myself->attributes["referenceLine"], ERRORLEVEL_NOTICE);
    }
  ]
;


/******************************************************************************
  RETHROW
******************************************************************************/

Rethrow(a,_id) ->
  [postprocessing:
    {
      assert(_id != NULL);
      unsigned int myid = _id->value;
      ASTE *myself = globals::ASTEmap[myid];
      assert(myself != NULL);

      a->parentId = myid;

      myself->parentScopeId = globals::PPcurrentScope;
      myself->checkAttributes();

      /* above is generic */

      // triggers [SA00006]
      bool correctUse = false;
      vector<unsigned int> ancestorActivities = myself->ancestorActivities();
      for (vector<unsigned int>::iterator it = ancestorActivities.begin(); it != ancestorActivities.end(); it++)
      {
        assert(globals::ASTEmap[*it] != NULL);
        if (globals::ASTEmap[*it]->activityTypeName() == "faultHandlers")
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
      globals::ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myid );
    }
  ]
;


/******************************************************************************
  COMPENSATE
******************************************************************************/

Compensate(a,_id) ->
  [postprocessing:
    {
      assert(_id != NULL);
      unsigned int myid = _id->value;
      ASTE *myself = globals::ASTEmap[myid];
      assert(myself != NULL);

      a->parentId = myid;

      myself->parentScopeId = globals::PPcurrentScope;
      myself->checkAttributes();

      /* above is generic */

      // triggers [SA00008]
      bool correctUse = false;
      vector<unsigned int> ancestorActivities = myself->ancestorActivities();

      for (vector<unsigned int>::iterator it = ancestorActivities.begin(); it != ancestorActivities.end(); it++)
      {
        if (globals::ASTEmap[*it]->activityTypeName() == "faultHandlers" ||
            globals::ASTEmap[*it]->activityTypeName() == "compensationHandler" ||
            globals::ASTEmap[*it]->activityTypeName() == "terminationHandler")
        {
          correctUse = true;
          break;
        }
      }
      if (!correctUse)
        SAerror(8, "", myself->attributes["referenceLine"]);

      // the parent scope has to deal with a compensate
      globals::ASTEmap[myself->parentActivityId]->hasCompensate = true;
      for (set< unsigned int >::iterator scope =  globals::ASTEmap[myself->parentActivityId]->enclosedScopes.begin();
                                         scope != globals::ASTEmap[myself->parentActivityId]->enclosedScopes.begin(); 
                                         scope++)
      {
        globals::ASTEmap[ globals::ASTEmap[ *scope ]->enclosedCH ]->callable = true;
      }
    }
    a
    {
      globals::ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myid );

      // warn for non-existing Petri net patterns
      if ( (modus == M_PETRINET || modus == M_CHOREOGRAPHY)
         && globals::parameters[P_COMMUNICATIONONLY] )
	genericError(121, myself->activityTypeName(), myself->attributes["referenceLine"], ERRORLEVEL_NOTICE);
    }
  ]
;


/******************************************************************************
  COMPENSATE SCOPE
******************************************************************************/

CompensateScope(a,b,_id) ->
  [postprocessing:
    {
      assert(_id != NULL);
      unsigned int myid = _id->value;
      ASTE *myself = globals::ASTEmap[myid];
      assert(myself != NULL);

      a->parentId = myid;

      // copy attribute from AST to attribute map
      // (necessary to be able to parse BPEL4WS 1.1)
      if (myself->attributes["target"] == "")
        myself->attributes["target"] = string(b->name);

      myself->parentScopeId = globals::PPcurrentScope;
      myself->checkAttributes();

      /* above is generic */


      // triggers [SA00007]
      bool correctUse = false;
      vector<unsigned int> ancestorActivities = myself->ancestorActivities();
      for (vector<unsigned int>::iterator it = ancestorActivities.begin(); it != ancestorActivities.end(); it++)
      {
        if (globals::ASTEmap[*it]->activityTypeName() == "faultHandlers" ||
            globals::ASTEmap[*it]->activityTypeName() == "compensationHandler" ||
            globals::ASTEmap[*it]->activityTypeName() == "terminationHandler")
        {
          correctUse = true;
          break;
        }
      }
      if (!correctUse)
        SAerror(7, "", myself->attributes["referenceLine"]);


      // triggers SA00078 and SA00077
      if (globals::ASTE_scopeNames[myself->attributes["target"]] == 0)
        SAerror(78, myself->attributes["target"], myself->attributes["referenceLine"]);
      else if (globals::ASTEmap[myself->parentScopeId]->enclosedScopes.find(globals::ASTE_scopeNames[myself->attributes["target"]]) == globals::ASTEmap[myself->parentScopeId]->enclosedScopes.end())
        SAerror(77, myself->attributes["target"], myself->attributes["referenceLine"]);

      globals::ASTEmap[ 
        globals::ASTEmap[ 
          *(globals::ASTEmap[myself->parentScopeId]->enclosedScopes.find(globals::ASTE_scopeNames[myself->attributes["target"]])) 
        ]->enclosedCH 
      ]->callable = true;
    }
    a
    {
      globals::ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myid );

      // warn for non-existing Petri net patterns
      if ( (modus == M_PETRINET || modus == M_CHOREOGRAPHY)
         && globals::parameters[P_COMMUNICATIONONLY] )
	genericError(121, myself->activityTypeName(), myself->attributes["referenceLine"], ERRORLEVEL_NOTICE);
    }
  ]
;


/******************************************************************************
  EXIT
******************************************************************************/

Exit(a,_id) ->
  [postprocessing:
    {
      assert(_id != NULL);
      unsigned int myid = _id->value;
      ASTE *myself = globals::ASTEmap[myid];
      assert(myself != NULL);

      a->parentId = myid;

      myself->parentScopeId = globals::PPcurrentScope;
      myself->checkAttributes();
    }
    a
    {
      globals::ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myid );

      // warn for non-existing Petri net patterns
      if ( (modus == M_PETRINET || modus == M_CHOREOGRAPHY)
         && globals::parameters[P_COMMUNICATIONONLY] )
	genericError(121, myself->activityTypeName(), myself->attributes["referenceLine"], ERRORLEVEL_NOTICE);
    }
  ]
;


/******************************************************************************
  FLOW
******************************************************************************/

Flow(a,b,c,_id) ->
  [postprocessing:
    {
      assert(_id != NULL);
      unsigned int myid = _id->value;
      ASTE *myself = globals::ASTEmap[myid];
      assert(myself != NULL);

      

      a->parentId = myid;

      myself->parentScopeId = globals::PPcurrentScope;
      myself->checkAttributes();

      /* above is generic */

      // distribute the flow's id to the enclosed activities
      // also distribute the control flow type
      foreach(innerActivity; activity_list c)
      {
        assert(globals::ASTEmap[innerActivity->id] != NULL);
        globals::ASTEmap[innerActivity->id]->parentActivityId = myid;
        globals::ASTEmap[innerActivity->id]->controlFlow = myself->controlFlow;
        globals::ASTEmap[innerActivity->id]->max_occurrences = myself->max_occurrences;
      }
      foreach(link; tLink_list b)
      {
        assert(globals::ASTEmap[link->id] != NULL);
        globals::ASTEmap[ link->id ]->parentActivityId = myid;
        globals::ASTEmap[ myid ]->enclosedActivities.insert( link->id );
      }
    }
    a b c
    {
      // id of an enclosed start activity (for [SA00056])
      unsigned int startActivityId = 0;

      foreach(innerActivity; activity_list c)
      {
        // collect enclosed source and target links
        assert(globals::ASTEmap[innerActivity->id] != NULL);
        myself->enclosedSourceLinks = setUnion(myself->enclosedSourceLinks, globals::ASTEmap[innerActivity->id]->enclosedSourceLinks);
        myself->enclosedTargetLinks = setUnion(myself->enclosedTargetLinks, globals::ASTEmap[innerActivity->id]->enclosedTargetLinks);

        // collect start activities (for [SA00056])
        if (globals::ASTEmap[innerActivity->id]->isStartActivity)
          startActivityId = innerActivity->id;
      }

      // check whether activities precede a start actvitiy (for [SA00056])
      if (startActivityId != 0)
      {
	foreach(innerActivity; activity_list c)
	{
          assert(globals::ASTEmap[innerActivity->id] != NULL);
	
	  if (!globals::ASTEmap[innerActivity->id]->isStartActivity &&
	      globals::ASTEmap[innerActivity->id]->targetLinks.empty())
          {
	    SAerror(56, "<" + globals::ASTEmap[startActivityId]->activityTypeName() + ">", globals::ASTEmap[startActivityId]->attributes["referenceLine"]);
            break;
          }
	}
      }
      
      /* here we can check whether each link was used as source and target */


      globals::ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myid );
      globals::ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myself->enclosedActivities.begin(), myself->enclosedActivities.end() );
      enterEnclosedActivities( myid, globals::ASTEmap[ myid ]->enclosedActivities );
    }
  ]
;

Link(_id) ->
  [postprocessing:
    {
      assert(_id != NULL);
      unsigned int myid = _id->value;
      ASTE *myself = globals::ASTEmap[myid];
      assert(myself != NULL);

      myself->parentScopeId = globals::PPcurrentScope;
      myself->checkAttributes();

      myself->linkName = myself->defineLink();

      globals::ASTE_linkIdMap[ myself->linkName ] = myid;

      globals::ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myid );
    }
  ]
;


/******************************************************************************
  IF
******************************************************************************/

If(a,b,c,_id) ->
  [postprocessing:
    {
      assert(_id != NULL);
      unsigned int myid = _id->value;
      ASTE *myself = globals::ASTEmap[myid];
      assert(myself != NULL);

      
      a->parentId = myid;

      myself->parentScopeId = globals::PPcurrentScope;
      myself->checkAttributes();

      /* above is generic */
      
      
      // count the <if>'s branches for Thomas Heidinger's Diplomarbeit
      globals::if_branches[myid] = 1;
      

      // distribute the switch's id to the enclosed activities
      // also distribute the control flow
      foreach(branch=ElseIf(innerActivity,_id2); tElseIf_list b)
      {
        assert(globals::ASTEmap[innerActivity->id] != NULL);
        globals::ASTEmap[innerActivity->id]->parentActivityId = myid;
        globals::ASTEmap[innerActivity->id]->controlFlow = myself->controlFlow;
        globals::ASTEmap[innerActivity->id]->max_occurrences = myself->max_occurrences;
	
	globals::if_branches[myid]++;
      }

      // process Else-branch
      with(c)
      {
        c=Else(d,_id2):
        {
          assert(globals::ASTEmap[d->id] != NULL);
          globals::ASTEmap[d->id]->parentActivityId = myid;
          globals::ASTEmap[d->id]->controlFlow = myself->controlFlow;
          globals::ASTEmap[d->id]->max_occurrences = myself->max_occurrences;
        }
        default: { /* do nothing */ }
      }
    }
    a b c
    {
      // collect enclosed source and target links
      foreach(branch=ElseIf(innerActivity,_id2); tElseIf_list b)
      {
        assert(globals::ASTEmap[innerActivity->id] != NULL);
        myself->enclosedSourceLinks = setUnion(myself->enclosedSourceLinks, globals::ASTEmap[innerActivity->id]->enclosedSourceLinks);
        myself->enclosedTargetLinks = setUnion(myself->enclosedTargetLinks, globals::ASTEmap[innerActivity->id]->enclosedTargetLinks);
      }

      with(c)
      {
        c=Else(d,_id2):
        {
          assert(globals::ASTEmap[d->id] != NULL);
          myself->enclosedSourceLinks = setUnion(myself->enclosedSourceLinks, globals::ASTEmap[d->id]->enclosedSourceLinks);
          myself->enclosedTargetLinks = setUnion(myself->enclosedTargetLinks, globals::ASTEmap[d->id]->enclosedTargetLinks);
        }
        default: { /* do nothing */ }
      }

      globals::ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myid );
      globals::ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myself->enclosedActivities.begin(), myself->enclosedActivities.end() );
      enterEnclosedActivities( myid, globals::ASTEmap[ myid ]->enclosedActivities );
    }
  ]
;

ElseIf(a,_id) ->
  [postprocessing:
    {
      assert(_id != NULL);
      unsigned int myid = _id->value;
      ASTE *myself = globals::ASTEmap[myid];
      assert(myself != NULL);

      myself->parentScopeId = globals::PPcurrentScope;
      myself->checkAttributes();
    }
    a
    {
      myself->enclosedActivities = globals::ASTEmap[ a->id ]->enclosedActivities;
      myself->enclosedActivities.insert( a->id );
      enterEnclosedActivities( myid, globals::ASTEmap[ myid ]->enclosedActivities );
    }
  ]
;

Else(a,_id) ->
  [postprocessing:
    {
      assert(_id != NULL);
      unsigned int myid = _id->value;
      ASTE *myself = globals::ASTEmap[myid];
      assert(myself != NULL);

      myself->parentScopeId = globals::PPcurrentScope;
    }
    a
    {
      myself->enclosedActivities = globals::ASTEmap[ a->id ]->enclosedActivities;
      myself->enclosedActivities.insert( a->id );
      enterEnclosedActivities( myid, globals::ASTEmap[ myid ]->enclosedActivities );
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
      assert(_id != NULL);
      unsigned int myid = _id->value;
      ASTE *myself = globals::ASTEmap[myid];
      assert(myself != NULL);

      

      a->parentId = myid;

      myself->parentScopeId = globals::PPcurrentScope;
      myself->checkAttributes();

      /* above is generic */

      // distribute the while's id to the enclosed activity
      assert(globals::ASTEmap[b->id] != NULL);
      globals::ASTEmap[b->id]->parentActivityId = myid;
      globals::ASTEmap[b->id]->controlFlow = myself->controlFlow;

      // calculate the maximal occurrences of the enclosed activity
      unsigned int max_loops = toUInt(myself->attributes["hu:maxloops"]);
      if (max_loops != UINT_MAX && myself->max_occurrences != UINT_MAX)
        globals::ASTEmap[b->id]->max_occurrences = myself->max_occurrences * max_loops;
      else
        globals::ASTEmap[b->id]->max_occurrences = UINT_MAX;

      myself->max_loops = max_loops;
    }
    a b
    {
      // collect enclosed source and target links
      assert(globals::ASTEmap[b->id] != NULL);
      myself->enclosedSourceLinks = setUnion(myself->enclosedSourceLinks, globals::ASTEmap[b->id]->enclosedSourceLinks);
      myself->enclosedTargetLinks = setUnion(myself->enclosedTargetLinks, globals::ASTEmap[b->id]->enclosedTargetLinks);

      globals::ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myid );
      globals::ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myself->enclosedActivities.begin(), myself->enclosedActivities.end() );
      enterEnclosedActivities( myid, globals::ASTEmap[ myid ]->enclosedActivities );

      check_SA00070( myid );
    }
  ]
;





/******************************************************************************
  REPEAT UNTIL
******************************************************************************/

RepeatUntil(a,b,_id) ->
  [postprocessing:
    {
      assert(_id != NULL);
      unsigned int myid = _id->value;
      ASTE *myself = globals::ASTEmap[myid];
      assert(myself != NULL);

      

      a->parentId = myid;

      myself->parentScopeId = globals::PPcurrentScope;
      myself->checkAttributes();

      /* above is generic */

      // distribute the repeatUntil's id to the enclosed activity
      assert(globals::ASTEmap[b->id] != NULL);
      globals::ASTEmap[b->id]->parentActivityId = myid;
      globals::ASTEmap[b->id]->controlFlow = myself->controlFlow;

      // calculate the maximal occurrences of the enclosed activity
      unsigned int max_loops = toUInt(myself->attributes["hu:maxloops"]);
      if (max_loops != UINT_MAX && myself->max_occurrences != UINT_MAX)
        globals::ASTEmap[b->id]->max_occurrences = myself->max_occurrences * max_loops;
      else
        globals::ASTEmap[b->id]->max_occurrences = UINT_MAX;
      
      myself->max_loops = max_loops;
    }
    a b
    {
      // collect enclosed source and target links
      assert(globals::ASTEmap[b->id] != NULL);
      myself->enclosedSourceLinks = setUnion(myself->enclosedSourceLinks, globals::ASTEmap[b->id]->enclosedSourceLinks);
      myself->enclosedTargetLinks = setUnion(myself->enclosedTargetLinks, globals::ASTEmap[b->id]->enclosedTargetLinks);

      globals::ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myid );
      globals::ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myself->enclosedActivities.begin(), myself->enclosedActivities.end() );
      enterEnclosedActivities( myid, globals::ASTEmap[ myid ]->enclosedActivities );
      
      check_SA00070( myid );
    }
  ]
;





/******************************************************************************
  SEQUENCE
******************************************************************************/

Sequence(a,b,_id) ->
  [postprocessing:
    {
      assert(_id != NULL);
      unsigned int myid = _id->value;
      ASTE *myself = globals::ASTEmap[myid];
      assert(myself != NULL);

      

      a->parentId = myid;

      myself->parentScopeId = globals::PPcurrentScope;
      myself->checkAttributes();

      /* above is generic */

      // distribute the sequence's id to the enclosed activities
      foreach(innerActivity; activity_list b)
      {
        assert(globals::ASTEmap[innerActivity->id] != NULL);
        globals::ASTEmap[innerActivity->id]->parentActivityId = myid;
        globals::ASTEmap[innerActivity->id]->controlFlow = myself->controlFlow;
        globals::ASTEmap[innerActivity->id]->max_occurrences = myself->max_occurrences;
      }
    }
    a b
    {
      // collect enclosed source links and check whether the sequence contains
      // a start activity (for [SA00056])
      int activityCount = 1;
      foreach(innerActivity; activity_list b)
      {
        assert(globals::ASTEmap[innerActivity->id] != NULL);

        bool before = true;
        foreach(innerActivity2; activity_list b)
        {
          if ( innerActivity2->id == innerActivity->id)
          {
            before == false;
          }
          else if (before == true)
          {
            enterConsecutiveActivities( innerActivity2->id, innerActivity->id );
          }
        }

        // collect enclosed source and target links
        myself->enclosedSourceLinks = setUnion(myself->enclosedSourceLinks, globals::ASTEmap[innerActivity->id]->enclosedSourceLinks);
        myself->enclosedTargetLinks = setUnion(myself->enclosedTargetLinks, globals::ASTEmap[innerActivity->id]->enclosedTargetLinks);

        // trigger [SA00056]
        if (globals::ASTEmap[innerActivity->id]->isStartActivity && activityCount++ > 1)
          SAerror(56, "<" + globals::ASTEmap[innerActivity->id]->activityTypeName() + ">", globals::ASTEmap[innerActivity->id]->attributes["referenceLine"]);
      }

      globals::ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myid );
      globals::ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myself->enclosedActivities.begin(), myself->enclosedActivities.end() );
      enterEnclosedActivities( myid, globals::ASTEmap[ myid ]->enclosedActivities );
    }
  ]
;


/******************************************************************************
  PICK
******************************************************************************/

Pick(a,b,c,_id) ->
  [postprocessing:
    {
      assert(_id != NULL);
      unsigned int myid = _id->value;
      ASTE *myself = globals::ASTEmap[myid];
      assert(myself != NULL);

      

      a->parentId = myid;

      myself->parentScopeId = globals::PPcurrentScope;
      myself->checkAttributes();

      /* above is generic */

      // distribute the pick's id to the enclosed activities
      foreach(branch=OnMessage(_corr,_from,innerActivity,_id2); tOnMessage_list b)
      {
        assert(globals::ASTEmap[innerActivity->id] != NULL);
        globals::ASTEmap[innerActivity->id]->parentActivityId = myid;
        globals::ASTEmap[_id2->value]->parentActivityId = myid;
        globals::ASTEmap[innerActivity->id]->controlFlow = myself->controlFlow;
        globals::ASTEmap[_id2->value]->max_occurrences = myself->max_occurrences;
      }
      foreach(branch=OnAlarm(*,innerActivity,_id3); tOnAlarm_list c)
      {
        assert(globals::ASTEmap[innerActivity->id] != NULL);
        globals::ASTEmap[innerActivity->id]->parentActivityId = myid;
        globals::ASTEmap[_id3->value]->parentActivityId = myid;
        globals::ASTEmap[innerActivity->id]->controlFlow = myself->controlFlow;
        globals::ASTEmap[_id3->value]->max_occurrences = myself->max_occurrences;
      }
    }
    a b c
    {
      if (myself->isStartActivity)
      {
        globals::ASTE_startActivities.insert(myid);

        // trigger [SA00062]
        if (c->length() != 0)
          SAerror(62, "", myself->attributes["referenceLine"]);

        // trigger [SA00056]
        if (!globals::ASTEmap[myself->parentActivityId]->checkAncestors() || !myself->targetLinks.empty())
          SAerror(56, "<pick>", myself->attributes["referenceLine"]);
      }

      // collect enclosed source and target links
      foreach(branch=OnMessage(_corr,_from,innerActivity,_id2); tOnMessage_list b)
      {
        assert(globals::ASTEmap[innerActivity->id] != NULL);
        myself->enclosedSourceLinks = setUnion(myself->enclosedSourceLinks, globals::ASTEmap[innerActivity->id]->enclosedSourceLinks);
        myself->enclosedTargetLinks = setUnion(myself->enclosedTargetLinks, globals::ASTEmap[innerActivity->id]->enclosedTargetLinks);

        foreach(otherbranch=OnMessage(*,*,secondActivity,_id3); tOnMessage_list b)
        {
          assert(_id2 != NULL);
          assert(_id3 != NULL);
          if (_id2->value < _id3->value)
          {
            enterConflictingActivities( globals::ASTEmap[ _id2->value ]->enclosedActivities, globals::ASTEmap[ _id3->value ]->enclosedActivities );
          }
        }
      }
      foreach(branch=OnAlarm(*,innerActivity); tOnAlarm_list c)
      {
        assert(globals::ASTEmap[innerActivity->id] != NULL);
        myself->enclosedSourceLinks = setUnion(myself->enclosedSourceLinks, globals::ASTEmap[innerActivity->id]->enclosedSourceLinks);
        myself->enclosedTargetLinks = setUnion(myself->enclosedTargetLinks, globals::ASTEmap[innerActivity->id]->enclosedTargetLinks);

        foreach(otherbranch=OnMessage(*,*,secondActivity,*); tOnMessage_list b)
        {
          enterConflictingActivities( globals::ASTEmap[ innerActivity->id ]->enclosedActivities, globals::ASTEmap[ secondActivity->id ]->enclosedActivities );
        }

        foreach(otherbranch=OnAlarm(*,secondActivity); tOnAlarm_list c)
        {
          if (innerActivity->id < secondActivity->id)
          {
            enterConflictingActivities( globals::ASTEmap[ innerActivity->id ]->enclosedActivities, globals::ASTEmap[ secondActivity->id ]->enclosedActivities );
          }
        }
      }


      globals::ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myid );
      globals::ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myself->enclosedActivities.begin(), myself->enclosedActivities.end() );
      enterEnclosedActivities( myid, globals::ASTEmap[ myid ]->enclosedActivities );
    }
  ]
;


/******************************************************************************
  FOREACH
******************************************************************************/

ForEach(a,b,c,d,e,_id) ->
  [postprocessing:
    {
      assert(_id != NULL);
      unsigned int myid = _id->value;
      ASTE *myself = globals::ASTEmap[myid];
      assert(myself != NULL);    

      a->parentId = myid;

      myself->parentScopeId = globals::PPcurrentScope;
      myself->checkAttributes();

      /* above is generic */

      myself->checkVariable("counterName");

      // distribute the forEach's id to the enclosed activity
      assert(globals::ASTEmap[e->id] != NULL);
      globals::ASTEmap[e->id]->parentActivityId = myid;
      globals::ASTEmap[e->id]->controlFlow = myself->controlFlow;

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
          globals::ASTEmap[e->id]->max_occurrences = myself->max_occurrences * (finalCounter - startCounter);
          myself->max_loops = (finalCounter - startCounter);
        }

        if ((completion != UINT_MAX) && ((completion > (finalCounter-startCounter))))
          SAerror(75, "", myself->attributes["referenceLine"]);
      }
      else
      {
        // the forEach is annotated by an id that references a BPEL4Chor participantSet
        if ((globals::choreography_filename != "") && (string(b->name) == "opaque") && (string(c->name) == "opaque"))
        {
          // get the number of maximal instances from the BPEL4Chor file
          myself->max_loops = globals::BPEL4ChorInfo.forEach_count(myid);
        }
        else
        {
	  // calculate the maximal occurrences of the enclosed activity
	  unsigned int max_loops = toUInt(myself->attributes["hu:maxloops"]);
	  if (max_loops != UINT_MAX && myself->max_occurrences != UINT_MAX)
	    globals::ASTEmap[e->id]->max_occurrences = myself->max_occurrences * max_loops;
	  else
	    globals::ASTEmap[e->id]->max_occurrences = UINT_MAX;

	  myself->max_loops = max_loops;
        }
      }
    }
    a e
    {
      // trigger [SA00076]
      with (e)
      {
        e=Scope(*,_variables,messageExchange,*,*,*,*,*,*,*,*):
          {
            foreach (variable=Variable(_id2); tVariable_list _variables)
            {
              assert(_id2 != NULL);
              assert(globals::ASTEmap[_id2->value] != NULL);
              if (globals::ASTEmap[_id2->value]->attributes["name"] == globals::ASTEmap[_id->value]->attributes["counterName"])
                SAerror(76, globals::ASTEmap[_id2->value]->attributes["name"], globals::ASTEmap[_id2->value]->attributes["referenceLine"]);
            }            
          }
      }

      // collect enclosed source and target links
      assert(globals::ASTEmap[e->id] != NULL);
      myself->enclosedSourceLinks = setUnion(myself->enclosedSourceLinks, globals::ASTEmap[e->id]->enclosedSourceLinks);
      myself->enclosedTargetLinks = setUnion(myself->enclosedTargetLinks, globals::ASTEmap[e->id]->enclosedTargetLinks);

      globals::ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myid );
      globals::ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myself->enclosedActivities.begin(), myself->enclosedActivities.end() );
      enterEnclosedActivities( myid, globals::ASTEmap[ myid ]->enclosedActivities );

      check_SA00070( myid );
    }
  ]
;


/******************************************************************************
  SCOPE
******************************************************************************/

Scope(a,b,messageExchange,c,d,e,f,g,i,j,_id) ->
  [postprocessing:
    {
      assert(_id != NULL);
      unsigned int myid = _id->value;
      ASTE *myself = globals::ASTEmap[myid];
      assert(myself != NULL);
      a->parentId = myid;

      

      // my parent is the current scope
      myself->parentScopeId = globals::PPcurrentScope;

      // now I am the current scope
      globals::PPcurrentScope = myid;

      assert(globals::ASTEmap[myself->parentActivityId] != NULL);
      assert(globals::ASTEmap[myself->parentScopeId] != NULL);
      myself->checkAttributes();

      for ( std::set<unsigned int>::iterator peerScope = globals::ASTEmap[myself->parentScopeId]->enclosedScopes.begin(); 
	    peerScope != globals::ASTEmap[myself->parentScopeId]->enclosedScopes.end(); 
	    peerScope++)
      {
	globals::ASTEmap[*peerScope]->peerScopes.insert(myid);
	globals::ASTEmap[myid]->peerScopes.insert(*peerScope);
      }

      // register this scope to be enclosed in the parent scope only if this
      // scope is embedded to positive control flow (only then in can be
      // compensated)
      if (myself->controlFlow == POSITIVECF)
	globals::ASTEmap[myself->parentScopeId]->enclosedScopes.insert(myid);

      myself->hasEH = (string(f->op_name()) == "EventHandlers");


      // distribute the scopes' id to the enclosed FH/CH/EH/TH/activity
      assert(globals::ASTEmap[c->id] != NULL);
      assert(globals::ASTEmap[d->id] != NULL);
      assert(globals::ASTEmap[e->id] != NULL);
      assert(globals::ASTEmap[f->id] != NULL);
      assert(globals::ASTEmap[i->id] != NULL);
      globals::ASTEmap[c->id]->parentActivityId = myid;
      globals::ASTEmap[d->id]->parentActivityId = myid;
      globals::ASTEmap[e->id]->parentActivityId = myid;
      globals::ASTEmap[f->id]->parentActivityId = myid;
      globals::ASTEmap[i->id]->parentActivityId = myid;

      myself->enclosedFH = c->id;
      myself->enclosedCH = d->id;

      globals::ASTEmap[c->id]->max_occurrences = myself->max_occurrences;
      globals::ASTEmap[d->id]->max_occurrences = myself->max_occurrences;
      globals::ASTEmap[e->id]->max_occurrences = myself->max_occurrences;
      globals::ASTEmap[f->id]->max_occurrences = myself->max_occurrences;
      globals::ASTEmap[i->id]->max_occurrences = myself->max_occurrences;

      // remember the scope's name for compensation
      if (myself->attributes["name"] != "")
      {
        // trigger [SA00092]
        if (globals::ASTE_scopeNames[myself->attributes["name"]] != 0)
          SAerror(92, myself->attributes["name"], myself->attributes["referenceLine"]);
        
        globals::ASTE_scopeNames[myself->attributes["name"]] = myid;
      }

      // trigger [SA00091]
      if (myself->attributes["isolated"] == "yes" &&
          globals::ASTEmap[myself->parentScopeId]->findIsolatedAncestor())
        SAerror(91, "", myself->attributes["referenceLine"]);
    }
    a b messageExchange j g
    i // inner activity
    c d e f
    {

      bool callableFH = false;
      for ( set< string >::iterator fault = globals::ASTEmap[ c->id ]->catches.begin(); fault != globals::ASTEmap[ c->id ]->catches.end(); fault++)
      {
        string faultName = fault->substr( fault->find_first_of( "|" ) + 1 );
        faultName = faultName.substr( 0, faultName.find_first_of( "|" ) );
        if (faultName.find_first_of(":") < faultName.npos)
        {
          faultName = faultName.substr( faultName.find_first_of(":") + 1 );
        }

        if ( myself->possiblyTriggeredFaults.find( faultName ) != myself->possiblyTriggeredFaults.end() )
        {
          callableFH = true;
          // might remove to much ... faults occuring in handlers are caught in higher scopes, especially if a <rethrow> occurs ...
          // myself->removeFault( faultName );
        }
      }
      if ( globals::ASTEmap[ c->id ]->hasCatchAll && ! myself->possiblyTriggeredFaults.empty() )
      {
        callableFH = true;
      }
      
      globals::ASTEmap[ c->id ]->callable = callableFH;

      set< unsigned int > innerIds = globals::ASTEmap[ i->id ]->enclosedActivities;
      innerIds.insert( i->id );
      set< unsigned int > faultIds = globals::ASTEmap[ c->id ]->enclosedActivities;
      faultIds.insert( c->id );
      set< unsigned int > compIds = globals::ASTEmap[ d->id ]->enclosedActivities;
      compIds.insert( d->id );

      enterConflictingActivities( innerIds, faultIds );
      enterConflictingActivities( faultIds, compIds );

      // pass list of enclosed scopes to the compensation handler
      globals::ASTEmap[d->id]->enclosedScopes = myself->enclosedScopes;

      // collect enclosed source and target links
      assert(globals::ASTEmap[i->id] != NULL);
      myself->enclosedSourceLinks = setUnion(myself->enclosedSourceLinks, globals::ASTEmap[i->id]->enclosedSourceLinks);
      myself->enclosedSourceLinks = setUnion(myself->enclosedSourceLinks, globals::ASTEmap[c->id]->enclosedSourceLinks);
      myself->enclosedSourceLinks = setUnion(myself->enclosedSourceLinks, globals::ASTEmap[d->id]->enclosedSourceLinks);
      myself->enclosedSourceLinks = setUnion(myself->enclosedSourceLinks, globals::ASTEmap[e->id]->enclosedSourceLinks);
      myself->enclosedSourceLinks = setUnion(myself->enclosedSourceLinks, globals::ASTEmap[f->id]->enclosedSourceLinks);

      myself->enclosedTargetLinks = setUnion(myself->enclosedTargetLinks, globals::ASTEmap[i->id]->enclosedTargetLinks);
      myself->enclosedTargetLinks = setUnion(myself->enclosedTargetLinks, globals::ASTEmap[c->id]->enclosedTargetLinks);
      myself->enclosedTargetLinks = setUnion(myself->enclosedTargetLinks, globals::ASTEmap[d->id]->enclosedTargetLinks);
      myself->enclosedTargetLinks = setUnion(myself->enclosedTargetLinks, globals::ASTEmap[e->id]->enclosedTargetLinks);
      myself->enclosedTargetLinks = setUnion(myself->enclosedTargetLinks, globals::ASTEmap[f->id]->enclosedTargetLinks);


      // everybody after me has the same parent scope as me
      globals::PPcurrentScope = myself->parentScopeId;

      globals::ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myid );
      globals::ASTEmap[ myself->parentActivityId ]->enclosedActivities.insert( myself->enclosedActivities.begin(), myself->enclosedActivities.end() );
      enterEnclosedActivities( myid, globals::ASTEmap[ myid ]->enclosedActivities );
      
      
      // warn for non-existing Petri net patterns
      if ( (modus == M_PETRINET || modus == M_CHOREOGRAPHY)
         && globals::parameters[P_COMMUNICATIONONLY] )
	genericError(120, "", myself->attributes["referenceLine"], ERRORLEVEL_NOTICE);
    }
  ]
;


/******************************************************************************
  STANDARD ELEMENTS
******************************************************************************/

StandardElements(a,b, _joinCondition) ->
  [postprocessing:
    {
      assert(globals::ASTEmap[$0->parentId] != NULL);
      $0->suppressJF = (globals::ASTEmap[$0->parentId]->attributes["suppressJoinFailure"] == "yes");

      // pass id to join condition
      _joinCondition->parentId = $0->parentId;
    }
    _joinCondition
    {
      unsigned int linkCount = 0;
      foreach(targetLink; tTarget_list a)
      {
        assert(globals::ASTEmap[targetLink->id] != NULL);
        globals::ASTEmap[targetLink->id]->controlFlow = globals::ASTEmap[$0->parentId]->controlFlow;
        globals::ASTEmap[targetLink->id]->parentActivityId = $0->parentId;
        
        // links are numbered and stored in globals::joinCondition_links map
        globals::joinCondition_links[globals::ASTEmap[targetLink->id]->attributes["linkName"]] = ++linkCount; // start enumerating with '1'
      }

      foreach(sourceLink; tSource_list b)
      {
        assert(globals::ASTEmap[sourceLink->id] != NULL);
        globals::ASTEmap[sourceLink->id]->controlFlow = globals::ASTEmap[$0->parentId]->controlFlow;
        globals::ASTEmap[sourceLink->id]->parentActivityId = $0->parentId;
      }

      /*
       * Calculate the join condition for all possible link statuses:
       *
       * There are 2^n possible assignments for n ingoing links (first
       * for-loop). For each assignment (second for-loop) set the links
       * respectively. Then call an uparser "evaluation" to actually
       * calculate the join condition for the current assignment. The result
       * is stored in mapping "globals::joinCondition_result", indexed by the number of
       * the assignment (0 .. 2^n -1).
       */
      unsigned int assignments = (unsigned int)pow(2.0, a->length());
      for (unsigned int assignment = 0; assignment < assignments; assignment++)
      {
        // set the links according to the current assignment
        for (unsigned int link = 0; link < linkCount; link++)
        {
          if (((assignment >> link) % 2) == 0) // don't ask why this is working :-|
            globals::joinCondition_linkStatus[link] = true;
          else
            globals::joinCondition_linkStatus[link] = false;
        }

        // evaluate the join condition in the current assignment
        _joinCondition->unparse(pseudoPrinter, evaluation);

        // save the result
        with(_joinCondition)
        {
          _joinCondition = userDefinedJoinCondition(_exp):
	    { globals::joinCondition_result[_joinCondition->parentId][assignment] = _exp->value; }
          default: {}
        }
      }
      
      // clear mappings that are not used any more
      globals::joinCondition_links.clear();
      globals::joinCondition_linkStatus.clear();

    }
    a b
    {
    
      set<string> targetLinks;
      foreach(targetLink; tTarget_list a)
      {
        // trigger [SA00069]
        string linkName = globals::ASTEmap[targetLink->id]->linkName;
        if (targetLinks.find(linkName) != targetLinks.end())
          SAerror(69, "<" + globals::ASTEmap[$0->parentId]->activityTypeName() + "> must not use <link> `" +
                  linkName + "'", globals::ASTEmap[globals::ASTE_linkIdMap[linkName]]->attributes["referenceLine"]);
        else
          targetLinks.insert(linkName);
      }

      set<string> sourceLinks;
      foreach(sourceLink; tSource_list b)
      {
        // trigger [SA00068]
        string linkName = globals::ASTEmap[sourceLink->id]->linkName;
        if (sourceLinks.find(linkName) != sourceLinks.end())
          SAerror(68, "<" + globals::ASTEmap[$0->parentId]->activityTypeName() + "> must not use <link> `" +
                  linkName + "'", globals::ASTEmap[globals::ASTE_linkIdMap[linkName]]->attributes["referenceLine"]);
        else
          sourceLinks.insert(linkName);
      }
    
    }
  ]
;

Target(a) ->
  [postprocessing:
    {
      ASTE *myself = globals::ASTEmap[$0->id];
      assert(myself != NULL);

      myself->checkAttributes();
      myself->linkName = myself->checkLink();

      // find the id of the link used as target here and add that link to the
      // parent activity
      assert(globals::ASTEmap[myself->parentActivityId] != NULL);
      unsigned int linkId = globals::ASTE_linkIdMap[ myself->linkName ];
      globals::ASTEmap[myself->parentActivityId]->enclosedTargetLinks.insert(linkId);
      globals::ASTEmap[myself->parentActivityId]->targetLinks.insert(linkId);

      // trigger [SA00066]
      assert(globals::ASTEmap[linkId] != NULL);
      if (globals::ASTEmap[linkId]->targetActivity != 0) 
      {
        SAerror(66, myself->attributes["linkName"], myself->attributes["referenceLine"]);
      }
      else
      {
        globals::ASTEmap[linkId]->targetActivity = myself->parentActivityId;
      }

      if ( myself->attributes["suppressJoinFailure"]  == "no" )
      {
        globals::ASTEmap[ myself->parentScopeId]->enterFault( "joinFailure" );
      }
    }
  ]
;

Source(a) ->
  [postprocessing:
    {
      ASTE *myself = globals::ASTEmap[$0->id];
      assert(myself != NULL);

      myself->checkAttributes();
      myself->linkName = myself->checkLink();


      // find the id of the link used as source here and add that link to the
      // parent activity
      assert(globals::ASTEmap[myself->parentActivityId] != NULL);
      unsigned int linkId = globals::ASTE_linkIdMap[ myself->linkName ];
      globals::ASTEmap[myself->parentActivityId]->enclosedSourceLinks.insert(linkId);
      globals::ASTEmap[myself->parentActivityId]->sourceLinks.insert(linkId);

      // trigger [SA00066]
      assert(globals::ASTEmap[linkId] != NULL);
      if (globals::ASTEmap[linkId]->sourceActivity != 0)
      {
        SAerror(66, myself->attributes["linkName"], myself->attributes["referenceLine"]);
      }
      else
      {
        globals::ASTEmap[linkId]->sourceActivity = myself->parentActivityId;
      }
    }
  ]
;

userDefinedJoinCondition(_exp) ->
  [postprocessing:
    { _exp->parentId = $0->parentId; }
    _exp
  ]
;

Conjunction(_exp1, _exp2) -> 
  [postprocessing:
    { _exp1->parentId = _exp2->parentId = $0->parentId; }
    _exp1 _exp2
  ]
;

Disjunction(_exp1, _exp2) -> 
  [postprocessing:
    { _exp1->parentId = _exp2->parentId = $0->parentId; }
    _exp1 _exp2
  ]
;

Term(_linkname) -> [postprocessing: ];





/***************************
 * EVALUATE JOIN CONDITION *
 ***************************/
Term(_linkname) ->
  [evaluation:
    {
      // If there was a `$' preceeding the link name, it is removed here.
      string linkname_string = string(_linkname->name);
      if (linkname_string[0] == '$')
        linkname_string = linkname_string.substr(1, linkname_string.length() -1);

      // if no value was set in this join condition, the link is not inside the
      // target link list of the current activity --> triggeer SA00073
      if (globals::joinCondition_links[linkname_string] == 0)
      {
        assert(globals::ASTEmap[$0->parentId] != NULL);
        SAerror(73, linkname_string, globals::ASTEmap[$0->parentId]->attributes["referenceLine"]);
      }
      else
      {
        // numbering in globals::joinCondition_linkStatus should start with 0, thus subtract 1
        $0->value = globals::joinCondition_linkStatus[globals::joinCondition_links[linkname_string] -1];
      }
    }
  ]
;

Conjunction(_exp1, _exp2) ->
  [evaluation:
    _exp1
    _exp2
    { $0->value = (_exp1->value && _exp2->value); }
  ]
;

Disjunction(_exp1, _exp2) ->
  [evaluation:
    _exp1
    _exp2
    { $0->value = (_exp1->value || _exp2->value); }
  ]
;
