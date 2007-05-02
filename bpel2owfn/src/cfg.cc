/*****************************************************************************\
 * Copyright 2007 Christian Gierds, Niels Lohmann                            *
 * Copyright 2006 Christian Gierds                                           *
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

/*!
 * \file    cfg.cc
 *
 * \brief   control flow graph
 *
 *          This file implements the class defined in cfg.h
 * 
 * \author  Christian Gierds <gierds@informatik.hu-berlin.de>,
 *          Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          last changes of: \$Author: nielslohmann $
 * 
 * \since   2006-01-19
 *
 * \date    \$Date: 2007/05/02 06:29:19 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \version \$Revision: 1.57 $
 *
 * \todo    
 *          - commandline option to control drawing of clusters 
 *          - do not use "temporaryAttributeMap", because it should be deleted
 *            during postprocessing
 */





#include <cassert>
#include <iostream>

#include "bpel2owfn.h"
#include "cfg.h"
#include "options.h"
#include "debug.h"
#include "ast-details.h"
#include "ast-config.h"		// all you need from Kimwitu++
#include "globals.h"

using std::cerr;
using std::endl;





/******************************************************************************
 * GLOBAL VARIABLES
 *****************************************************************************/

/// The CFG
CFGBlock * CFG = NULL;

/// mapping of Link names to Source blocks
map<string, CFGBlock*> sources; 

/// mapping of Link names to Target blocks
map<string, CFGBlock*> targets; 





/******************************************************************************
 * CONSTRUCTORS AND DESTRUCTORS
 *****************************************************************************/


/*!
 * Constructor for a CFG block.
 */
CFGBlock::CFGBlock()
{
  firstBlock = this;
  lastBlock  = this;
  dotted = false;
  processed = false;
}





/*!
 * Constructor for a CFG block.
 *
 * \param pType	  the type of the block (like CFGEmpty, CFGInvoke etc.)
 * \param pId	  the ID of the block in the SymbolTable
 * \param pLabel  a label for identifing the block (e.g. start vs. end block of a flow)
 *
 */
CFGBlock::CFGBlock(CFGBlockType pType, int pId = 0, string pLabel = "")
{
  firstBlock = this;
  lastBlock  = this;
  type = pType;
  id = pId;
  label = pLabel;
  dotted = false;
  processed = false;

  globals::cfgMap[toString(pId) + "." + pLabel] = this;
}

/*!
 * Destructor
 */
CFGBlock::~CFGBlock()
{
}





/******************************************************************************
 * DOT OUTPUT FUNCTIONS
 *****************************************************************************/

/*!
 * Prints dot-style information about the block and creates arcs to its successors.
 */
void CFGBlock::print_dot()
{
  if (!dotted)
  {
    dotted = true;

    (*output) << "  // " << dot_name() << endl;
    (*output) << "  \"" << dot_name() << "\" [ label=\"" << label << " (" << id <<")";
    if (channel_name != "")
    {
      (*output) << "\\nchannel: " << channel_name;
    }
    if (! initializedVariables.empty())
    {
      (*output) << "\\ninitializedVariables: \\n ";
      for (set<string>::iterator iter = initializedVariables.begin(); iter != initializedVariables.end(); iter++)
      {
	(*output) << (*iter) << "\\n";
      }
    }
    if (! receives.empty())
    {
      (*output) << "\\nreceives: \\n ";
      for (set< pair< string, long > >::iterator iter = receives.begin(); iter != receives.end(); iter++)
      {
	(*output) << iter->first << "," << iter->second << "\\n";
      }
    }
    if (! globals::ASTEmap[id]->peerScopes.empty())
    {
      (*output) << "\\npeer scopes: \\n ";
      for (set< unsigned int  >::iterator iter = globals::ASTEmap[id]->peerScopes.begin(); iter != globals::ASTEmap[id]->peerScopes.end(); iter++)
      {
	(*output) << (*iter) << "\\n";
      }
    }
    if (! controllingPeers.empty())
    {
      (*output) << "\\ncontrolling peers: \\n ";
      for (set< unsigned int  >::iterator iter = controllingPeers.begin(); iter != controllingPeers.end(); iter++)
      {
	(*output) << (*iter) << "\\n";
      }
    }
    (*output) << "\"";
    if (type == CFGSource)
    {
      (*output) << " color=red style=filled"; // style=filled
    }
    if (type == CFGTarget)
    {
      (*output) << " color=green style=filled"; //
    }
    (*output) << " ]; " << endl;
    (*output) << "  // all outgoing edges" << endl;
    for(list<CFGBlock *>::iterator iter = nextBlocks.begin(); iter != nextBlocks.end(); iter++)
    {
      (*output) << "  \"" << dot_name() << "\" -> \"" << (*iter)->dot_name() << "\";" << endl;
    }
    (*output) << endl;
    // draw link
    if (type == CFGSource)
    {
      string targ = "dummy";
      if (targets[dot_name()] != NULL)
	  targ = (targets[dot_name()])->dot_name();
      (*output) << "  \"" << dot_name() << "\" -> \"" << targ << "\" [ style=dotted ];" << endl;
    }

    for(list<CFGBlock *>::iterator iter = nextBlocks.begin(); iter != nextBlocks.end(); iter++)
    {
      (*iter)->print_dot();
    }
  }
}





/*!
 * Returns a unique name for dot
 *
 * \return the dot name
 */
string CFGBlock::dot_name()
{
  if (type == CFGSource || type == CFGTarget)
  {
    return label;
  }
  else
  {
    return label + "_" + toString(id);
  }
}





/*!
 * Initializes the dot output by printing the graph information.
 */
void cfgDot(CFGBlock * block)
{
  (*output) << "digraph{" << endl;
  (*output) << "  graph ["
            << " label=\"CFG generated from '" << globals::filename <<"'\" ]; " << endl;
  (*output) << "  node [ fontsize=10 shape=box fontname=Helvetica ]; " << endl;
  (*output) << "  edge [ fontsize=10 ]; " << endl << endl;

  // print all the nodes and edges
  block->print_dot();
  
  (*output) << "}" << endl;

}





/// resets the processed flag to false
void CFGBlock::resetProcessedFlag()
{

  for (map< string, CFGBlock * >::iterator iter = globals::cfgMap.begin(); iter != globals::cfgMap.end(); iter++)
  {
    if ( iter->second != NULL )
    {
      iter->second->processed = false;
    }
  }
}





/*!
 * Connects two blocks.
 *
 * \param from	the first block
 * \param to	the second block
 */
void connectBlocks(CFGBlock * from, CFGBlock * to)
{
  from->nextBlocks.push_back(to);
  to->prevBlocks.push_back(from);
}





/******************************************************************************
 * CONTROL FLOW GRAPH ANALYSIS
 *****************************************************************************/

/*!
 * Checks for uninitialized variables.
 * This is a forward-must analysis of the data flow.
 */
void CFGBlock::checkForUninitializedVariables()
{
  if (processed)
    return;

  processed = true;
  
  bool allPrerequisites = true;
  // check if all entries are allready processed
  if (!prevBlocks.empty())
  {
    /// for a while, we assume, that the body is never executed, so we drop that set
    list<CFGBlock *>::iterator blockBegin = prevBlocks.begin();
    if (type == CFGWhile || type == CFGForEach || type == CFGRepeatUntil && label == "Repeat" )
    {
      blockBegin++;
    }
    for (list<CFGBlock *>::iterator iter = blockBegin; iter != prevBlocks.end(); iter++)
    {
      allPrerequisites = allPrerequisites && (*iter)->processed;
    }
  }

  if (type == CFGTarget)
  {
    allPrerequisites = allPrerequisites && sources[dot_name()]->processed;
  }

  if (allPrerequisites)
  {
    if (!prevBlocks.empty())
    {
      list<CFGBlock *>::iterator blockBegin = prevBlocks.begin();

      if (type == CFGWhile || type == CFGForEach || type == CFGRepeatUntil && label == "Repeat" )
      {
	blockBegin++;
      }

      initializedVariables = (*blockBegin)->initializedVariables;

      for (list<CFGBlock *>::iterator iter = blockBegin; iter != prevBlocks.end(); iter++)
      {
	if (type == CFGFlow && label == "Flow_end")
	{
	  initializedVariables = setUnion(initializedVariables, (*iter)->initializedVariables);
	}
	else
	{
	  initializedVariables = setIntersection(initializedVariables, (*iter)->initializedVariables);
	}
      }
    }

    if (type == CFGTarget)
    {
      initializedVariables = setUnion(initializedVariables, sources[dot_name()]->initializedVariables);
    }
  }
  else
  {
    processed = false;
    return;
  }

  // checking Variables for this Block
  string var = "";
  string attributeName = "variable";

  switch(type)
  {
    case CFGReply     : var = globals::ASTEmap[id]->variableName; break;
    case CFGFrom      : var = globals::ASTEmap[id]->variableName; break;
    case CFGInvoke    : var = globals::ASTEmap[id]->inputVariableName; 
			attributeName = "inputVariable"; break;

    default: { /* A switch needs a default branch! */ }
  }
  
  if ((type == CFGReply || type == CFGFrom || type == CFGInvoke) && var != "")
  {
    if (initializedVariables.find(var) == initializedVariables.end())
    {
      // uninitialized variable found: display error message
      assert(globals::ASTEmap[id] != NULL);
      string errormessage = "variable `" + globals::temporaryAttributeMap[id][attributeName] + "' used as `" + attributeName + "' in <" + globals::ASTEmap[id]->activityTypeName() + "> might be uninitialized";
      genericError(114, errormessage, globals::ASTEmap[id]->attributes["referenceLine"], ERRORLEVER_WARNING);
    }
  }
  
  // adding Variables for this Block
  var = "";
  attributeName = "variable";
    
  switch(type)
  {
    case CFGReceive   : var = globals::ASTEmap[id]->variableName; break;
    case CFGCatch     : var = globals::ASTEmap[id]->variableName; 
			attributeName = "faultVariable"; 
			break;
    case CFGTo        : var = globals::ASTEmap[id]->variableName; break;
    case CFGInvoke    : var = globals::ASTEmap[id]->outputVariableName;
			attributeName = "outputVariable";
			break;
    case CFGOnMessage : var = globals::ASTEmap[id]->variableName; break;

    default: { /* A switch needs a default branch! */ }
  }
  
  if ((type == CFGReceive || type == CFGCatch || type == CFGTo || type==CFGInvoke || type == CFGOnMessage) && var != "")
  {
    initializedVariables.insert(var); 
  }
  
  if (! nextBlocks.empty())
  {
    for (list<CFGBlock *>::iterator iter = nextBlocks.begin(); iter != nextBlocks.end(); iter++)
    {
      (*iter)->checkForUninitializedVariables();
    }
  }  
}





/// checks for cyclic links
void CFGBlock::checkForCyclicLinks()
{
  if (!processed)
  {
    processed = true;
    string linkname = "";

    if (type == CFGTarget)
    {
      trace( TRACE_VERY_DEBUG, "[CFG] Get Name of the link when target\n");
      linkname = globals::ASTEmap[id]->linkName;
    }
    else if (type == CFGSource)
    {
      trace( TRACE_VERY_DEBUG, "[CFG] Get Name of the link when source\n");
      linkname = globals::ASTEmap[id]->linkName;
    }

    
    if (type == CFGSource)
    {
      trace( TRACE_VERY_DEBUG, "[CFG] Have we seen the target?\n");
      if (!targetsSeen.empty() && targetsSeen.find(linkname) != targetsSeen.end())
      {
        // triggers SA00072
	SAerror(72, globals::ASTEmap[ id ]->attributes["linkName"], toInt(globals::ASTEmap[id]->attributes["referenceLine"]));
	return;
      }
      trace( TRACE_VERY_DEBUG, "[CFG] So get the target to the source\n");
      CFGBlock * targ = targets[dot_name()];
      trace( TRACE_VERY_DEBUG, "[CFG] Unify the targets we have seen, now really\n");
      targ->targetsSeen = setUnion(targ->targetsSeen, targetsSeen);
      targ->checkForCyclicLinks();
    }
    else if (type == CFGTarget)
    {
      trace( TRACE_VERY_DEBUG, "[CFG] Oh, we have a new target seen.\n" );
      targetsSeen.insert(linkname);
    }

    if (!nextBlocks.empty())
    {
      for (list<CFGBlock*>::iterator iter = nextBlocks.begin(); iter != nextBlocks.end(); iter++)
      {
        trace( TRACE_VERY_DEBUG, "[CFG] Unify the targets we have seen 2\n");
        (*iter)->targetsSeen = setUnion((*iter)->targetsSeen, targetsSeen);
	(*iter)->checkForCyclicLinks();
      }
    }
  }
  LEAVE("checkForCyclicLinks");
}





/// checks for cycles in control dependency
void CFGBlock::checkForCyclicControlDependency()
{
  ENTER("checkForCyclicControlDependency");

  if ( processed )
    return;

  processed = true;
 
  bool allPrerequisites = true;
  // check if all entries are allready processed
  if ( !prevBlocks.empty() )
  {
    /// for a while, we assume, that the body is never executed, so we drop that set
    list<CFGBlock *>::iterator blockBegin = prevBlocks.begin();
    if ( type == CFGWhile || type == CFGForEach || type == CFGRepeatUntil && label == "Repeat"  )
    {
      blockBegin++;
    }

    for ( list<CFGBlock *>::iterator iter = blockBegin; iter != prevBlocks.end(); iter++ )
    {
      allPrerequisites = allPrerequisites && ( *iter )->processed;
    }
  }
  /*
  if ( type == CFGTarget )
  {
    allPrerequisites = allPrerequisites && sources[ dot_name() ]->processed;
  }
  */

  if ( allPrerequisites )
  {
    if ( type == CFGTarget )
    {
      unsigned int possiblePeer = globals::ASTEmap[ globals::ASTEmap[ sources[ dot_name() ]->id ]->parentActivityId ]->parentScopeId;
      unsigned int parentId = id;
      parentId = globals::ASTEmap[ parentId ]->parentActivityId;

      while ( parentId != 1 && parentId != possiblePeer )
      {
	parentId = globals::ASTEmap[parentId]->parentScopeId;
      }
      if ( parentId == 1 && parentId != possiblePeer )
      {
	controllingPeers.insert( possiblePeer );
      }
    }
    else if ( type == CFGFlow && label == "Flow_end" || type == CFGSequence && label == "Sequence_end" ) //
//    else if ( type == CFGScope && label == "Scope_end" ) //
    {
      // find direct child scopes
      unsigned int child = 0;
      for ( set< unsigned int >::iterator enclosedScope = globals::ASTEmap[ globals::ASTEmap[ id ]->parentScopeId ]->enclosedScopes.begin(); 
					  enclosedScope != globals::ASTEmap[ globals::ASTEmap[ id ]->parentScopeId ]->enclosedScopes.end(); 
					  enclosedScope++)
      {
	if ( globals::ASTEmap[ *enclosedScope ]->parentScopeId == globals::ASTEmap[ id ]->parentScopeId )
	{
	  child = *enclosedScope;
	}
      }

      if ( child != 0 )
      {
	set< unsigned int > subscopes ( globals::ASTEmap[ child ]->peerScopes );
	subscopes.insert( child );
	map< unsigned int, bool > seen;

	for ( set< unsigned int >::iterator scope = subscopes.begin(); scope != subscopes.end(); scope++ )
	{
	  if ( ! seen[ *scope ] )
	  {
	    // depth first search for cycles
	    list< unsigned int > queue;
	    queue.push_back( *scope );
	    while ( queue.size() > 0 )
	    {
	      unsigned int current = *( queue.begin() );
//              cerr << "Looking at scope " << current << endl;
	      if ( ! seen[ current ] )
	      {
		seen[ current ] = true;
//		for ( set< unsigned int >::iterator peer = globals::ASTEmap[ current ]->peerScopes.begin(); peer != globals::ASTEmap[ current ]->peerScopes.end(); peer++ )
		for ( set< unsigned int >::iterator peer = globals::cfgMap[ toString(current) ]->lastBlock->controllingPeers.begin(); peer != globals::cfgMap[ toString(current) ]->lastBlock->controllingPeers.end(); peer++ )
		{
		  // is there a cycle?
//                  cerr << "    peer " << *peer << endl;
		  if ( seen [ *peer ] )
		  {
		    SAerror( 82, "", toInt( globals::ASTEmap[ *peer ]->attributes[ "referenceLine" ] ) );
                    return;
		  }
		  else
		  {
		    queue.push_back( *peer );
		  }
		}
		queue.pop_front();
	      }
	    }
	  }
	}
      }
    }

    if ( !nextBlocks.empty() )
    {
      for (list<CFGBlock*>::iterator iter = nextBlocks.begin(); iter != nextBlocks.end(); iter++)
      {
	(*iter)->controllingPeers = setUnion( (*iter)->controllingPeers, controllingPeers );
	(*iter)->checkForCyclicControlDependency();
      }
    }

    if (type == CFGSource)
    {
      targets[dot_name()]->checkForCyclicControlDependency();
    }
  }
  else
  {
    processed = false;
    LEAVE("checkForCyclicControlDependency");
    return;
  }

  LEAVE("checkForCyclicControlDependency");
}





/*!
 * checks for conflicting receives
 */
void CFGBlock::checkForConflictingReceive()
{
  ENTER("checkForConflictingReceive");

  if (processed)
  {
    LEAVE("checkForConflictingReceive");
    return;
  }

  processed = true;
  if (type == CFGWhile)
    (*(prevBlocks.begin()))->checkForConflictingReceive();

  bool allPrerequisites = true;

  // check if all entries are allready processed
  if (!nextBlocks.empty())
  {
    /// for a while, we assume, that the body is never executed, so we drop that set
    list<CFGBlock *>::iterator blockBegin = nextBlocks.begin();
    if (type == CFGWhile || type == CFGForEach || type == CFGRepeatUntil && label == "Until" )
    {
      // blockBegin++;
    }
    for (list<CFGBlock *>::iterator iter = blockBegin; iter != nextBlocks.end(); iter++)
    {
      allPrerequisites = allPrerequisites && (*iter)->processed;
    }
  }

  if (type == CFGSource)
  {
    allPrerequisites = allPrerequisites && targets[dot_name()]->processed;
  }

  if (allPrerequisites)
  {
    if (!nextBlocks.empty())
    {
      list<CFGBlock *>::iterator blockBegin = nextBlocks.begin();
      if (type == CFGWhile || type == CFGForEach || type == CFGRepeatUntil && label == "Until" )
      {
	// blockBegin++;
      }
      receives = (*blockBegin)->receives;
      for (list<CFGBlock *>::iterator iter = blockBegin; iter != nextBlocks.end(); iter++)
      {
	// the actual check for duplicate receives but only for flows
	if (type == CFGFlow && label == "Flow_begin" || type == CFGProcess && label == "Process_begin" || type == CFGScope && label == "Scope_begin" || type == CFGPick && label == "Pick_begin")
	{
	  for (set< pair< string, long> >::iterator elemA = (*iter)->receives.begin(); elemA != (*iter)->receives.end(); elemA++)
	  {
	    for (set< pair< string, long> >::iterator elemB = receives.begin(); elemB != receives.end(); elemB++)
	    {
	      if(elemA->first == elemB->first && elemA->second != elemB->second && receives.find(*elemA) == receives.end())
	      {
		// conflicting receive found: display error message
		assert(globals::ASTEmap[elemA->second] != NULL);
		assert(globals::ASTEmap[elemB->second] != NULL);

		string errormessage = "<" + globals::ASTEmap[elemA->second]->activityTypeName() + "> is in conflict with " +
		  "<" + globals::ASTEmap[elemB->second]->activityTypeName() + "> (line " +
		  globals::ASTEmap[elemB->second]->attributes["referenceLine"] + "): " +
		  "both activities receive from <partnerLink> `" +
		  globals::ASTEmap[elemB->second]->attributes["partnerLink"] + "' (operation `" +
		  globals::ASTEmap[elemB->second]->attributes["operation"] + "')";
		genericError(106, errormessage, globals::ASTEmap[elemA->second]->attributes["referenceLine"]);
	      }
	    }
	  }
	}
	if ((*iter)->type == CFGOnMessage)
	{
	  list<CFGBlock *>::iterator iter2 = iter;
	  ++iter2;
          for (list<CFGBlock *>::iterator otherBlock = iter2; otherBlock != nextBlocks.end(); otherBlock++)
	  {
	    if ((*otherBlock)->type == CFGOnMessage && (*iter)->channel_name == (*otherBlock)->channel_name) 
	    {
	      // conflicting receive found: display error message
	      assert(globals::ASTEmap[(*iter)->id] != NULL);
	      assert(globals::ASTEmap[(*otherBlock)->id] != NULL);

	      string errormessage = "<" + globals::ASTEmap[(*iter)->id]->activityTypeName() + "> is in conflict with " +
		"<" + globals::ASTEmap[(*otherBlock)->id]->activityTypeName() + "> (line " +
		globals::ASTEmap[(*otherBlock)->id]->attributes["referenceLine"] + "): " +
		"both activities receive from <partnerLink> `" +
		globals::ASTEmap[(*otherBlock)->id]->attributes["partnerLink"] + "' (operation `" +
		globals::ASTEmap[(*otherBlock)->id]->attributes["operation"] + "')";
	      genericError(106, errormessage, globals::ASTEmap[(*iter)->id]->attributes["referenceLine"]);
	    }
	  }
	  receives.insert(pair<string, long>( (*iter)->channel_name, (*iter)->id));
	}
	// 
	receives = setUnion(receives, (*iter)->receives);
      }
    }

    if (type == CFGSource)
    {
      for (set< pair< string, long> >::iterator elemA = targets[dot_name()]->receives.begin(); elemA != targets[dot_name()]->receives.end(); elemA++)
      {
	for (set< pair< string, long> >::iterator elemB = receives.begin(); elemB != receives.end(); elemB++)
	{
	  if(elemA->first == elemB->first && elemA->second != elemB->second && receives.find(*elemA) == receives.end())
	  {
	    // conflicting receive found: display error message
	    assert(globals::ASTEmap[elemA->second] != NULL);
	    assert(globals::ASTEmap[elemB->second] != NULL);

	    string errormessage = "<" + globals::ASTEmap[elemA->second]->activityTypeName() + "> is in conflict with " +
	      "<" + globals::ASTEmap[elemB->second]->activityTypeName() + "> (line " +
	      globals::ASTEmap[elemB->second]->attributes["referenceLine"] + "): " +
	      "both activities receive from <partnerLink> `" +
	      globals::ASTEmap[elemB->second]->attributes["partnerLink"] + "' (operation `" +
	      globals::ASTEmap[elemB->second]->attributes["operation"] + "')";
	    genericError(106, errormessage, globals::ASTEmap[elemA->second]->attributes["referenceLine"]);
	  }
	}
      }
      receives = setUnion(receives, targets[dot_name()]->receives);
    }

    if (type == CFGReceive)
    {
      receives.insert(pair<string, long>( channel_name, id));
    }

    if (type == CFGInvoke)
    {
      if( globals::temporaryAttributeMap[id]["outputVariable"] != "") {
	receives.insert(pair<string, long>( channel_name, id));
      }
    }

    if (!prevBlocks.empty())
    {
      /// for a while, we assume, that the body is never executed, so we drop that set
      for (list<CFGBlock *>::iterator iter = prevBlocks.begin(); iter != prevBlocks.end(); iter++)
      {
        (*iter)->checkForConflictingReceive();
      }
    }

    if (type == CFGTarget)
    {
      sources[dot_name()]->checkForConflictingReceive();
    }
  }
  else
  {
    processed = false;
    LEAVE("checkForConflictingReceive");
    return;
  }

  LEAVE("checkForConflictingReceive");
}





/******************************************************************************
 * PROCESS THE CFG
 *****************************************************************************/

void processCFG()
{
  assert(modus == M_CFG);

  CFG = NULL;
  trace(TRACE_INFORMATION, "-> Unparsing AST to CFG ...\n");
  globals::AST->unparse(kc::pseudoPrinter, kc::cfg);
  
  trace(TRACE_DEBUG, "[CFG] checking for cyclic links\n");
  /// \todo (gierds) check for cyclic links, otherwise we will fail
  CFG->checkForCyclicLinks();
  CFG->resetProcessedFlag();

  trace(TRACE_DEBUG, "[CFG] checking for cyclic control dependency\n");
  CFG->checkForCyclicControlDependency();
  CFG->resetProcessedFlag();

  trace(TRACE_DEBUG, "[CFG] checking for uninitialized variables\n");
  // test
  CFG->checkForUninitializedVariables();
  CFG->resetProcessedFlag();
  // end test

  trace(TRACE_DEBUG, "[CFG] checking for conflicting receive actions\n");
  CFG->lastBlock->checkForConflictingReceive();
  CFG->resetProcessedFlag();

  if (formats[F_DOT])
  {
    if (globals::output_filename != "")
      output = openOutput(globals::output_filename + ".cfg." + suffixes[F_DOT]);
    
    trace(TRACE_INFORMATION, "-> Printing CFG in dot ...\n");
    
    // output CFG;
    cfgDot(CFG);
    
    if (globals::output_filename != "")
    {
      closeOutput(output);
      output = NULL;
#ifdef HAVE_DOT
      string systemcall = "dot -q -Tpng -o" + globals::output_filename + ".cfg.png " + globals::output_filename + ".cfg." + suffixes[F_DOT];
      trace(TRACE_INFORMATION, "Invoking dot with the following options:\n");
      trace(TRACE_INFORMATION, systemcall + "\n\n");
      system(systemcall.c_str());
#endif

    }
  }

  delete(CFG);
}
