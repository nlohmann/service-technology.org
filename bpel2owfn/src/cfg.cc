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

/**
 * \file cfg.cc
 *
 * \brief Functions for the Control Flow Graph (implementation)
 *
 * This file implements the class defined in cfg.h
 * 
 * \author  
 *          - responsible: Christian Gierds <gierds@informatik.hu-berlin.de>
 *          - last changes of: \$Author: gierds $
 *          
 * \date
 *          - created: 2006-01-19
 *          - last changed: \$Date: 2006/11/16 11:44:07 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel
 *          for details.
 *
 * \version \$Revision: 1.38 $
 *
 * \todo    - commandline option to control drawing of clusters 
 */



#include <cassert>
#include "cfg.h"
#include "options.h"
#include "debug.h"
#include "ast-details.h"
#include "ast-config.h"		// all you need from Kimwitu++

extern map<unsigned int, map<string, string> > temporaryAttributeMap;
extern map<unsigned int, ASTE*> ASTEmap;

/// The CFG
CFGBlock * TheCFG = NULL;

/// mapping of Link names to Source blocks
map<std::string, CFGBlock*> sources; 

/// mapping of Link names to Target blocks
map<std::string, CFGBlock*> targets; 



/**
 * Constructor for a CFG block.
 *
 */
CFGBlock::CFGBlock()
{
  firstBlock = this;
  lastBlock  = this;
  dotted = false;
  processed = false;
  dpe = false;
  
}

/**
 * Constructor for a CFG block.
 *
 * \param pType	  the type of the block (like CFGEmpty, CFGInvoke etc.)
 * \param pId	  the ID of the block in the SymbolTable
 * \param pLabel  a label for identifing the block (e.g. start vs. end block of a flow)
 *
 */
CFGBlock::CFGBlock(CFGBlockType pType, int pId = 0, std::string pLabel = "")
{
  firstBlock = this;
  lastBlock  = this;
  type = pType;
  id = pId;
  label = pLabel;
  dotted = false;
  processed = false;
  dpe = false;
}

/**
 * Destructor
 *
 */
CFGBlock::~CFGBlock()
{

}

/**
 * Prints dot-style information about the block and creates arcs to its successors.
 *
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
      for (set<std::string>::iterator iter = initializedVariables.begin(); iter != initializedVariables.end(); iter++)
      {
	(*output) << (*iter) << "\\n";
      }
    }
    if (! receives.empty())
    {
      (*output) << "\\nreceives: \\n ";
      for (set< pair< std::string, long > >::iterator iter = receives.begin(); iter != receives.end(); iter++)
      {
	(*output) << iter->first << "," << iter->second << "\\n";
      }
    }
    if (! ASTEmap[id]->peerScopes.empty())
    {
      (*output) << "\\npeer scopes: \\n ";
      for (set< unsigned int  >::iterator iter = ASTEmap[id]->peerScopes.begin(); iter != ASTEmap[id]->peerScopes.end(); iter++)
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
    if (dpe)
    {
      (*output) << " fontcolor=blue";
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
      std::string targ = "dummy";
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

/**
 * Returns a unique name for dot
 *
 * \return the dot name
 *
 */
std::string CFGBlock::dot_name()
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

/**
 * Initializes the dot output by printing the graph information.
 */
void cfgDot(CFGBlock * block)
{
  (*output) << "digraph{" << endl;
  (*output) << "  graph ["
            << " label=\"CFG generated from '" << filename <<"'\" ]; " << endl;
  (*output) << "  node [ fontsize=10 shape=box fontname=Helvetica ]; " << endl;
  (*output) << "  edge [ fontsize=10 ]; " << endl << endl;

  // print all the nodes and edges
  block->print_dot();
  
  (*output) << "}" << endl;

}

/**
 * Checks if a block needs DPE.
 *
 * \param hasStartingBlock  true iff a Switch or Pick was seen.
 * \param lastTargets	    a list of all seen Targets
 * \return		    true iff DPE is needed
 *
 */
bool CFGBlock::needsDPE(int hasStartingBlock, list<int> lastTargets)
{
 
  if (processed)
  {
    return dpe;
  }
  
  bool childrenDPE = false;
  int newStartingBlockNumber = hasStartingBlock;
  list<int> localTargetList;

  if (!lastTargets.empty())
  {
    for (list<int>::iterator iter = lastTargets.begin(); iter != lastTargets.end(); iter++)
    {
      localTargetList.push_back(*iter);
    }
  }

  processed = true;

  if (! nextBlocks.empty())
  {
    switch (type)
    {
      case CFGSwitch :  if (label == "Switch_begin")
			{
			  newStartingBlockNumber++;
			}
			else
			{
			  newStartingBlockNumber--;
			}
			break; 
      case CFGIf :      if (label == "If_begin")
			{
			  newStartingBlockNumber++;
			}
			else
			{
			  newStartingBlockNumber--;
			}
			break; 
      case CFGPick :	if (label == "Pick_begin")
			{
			  newStartingBlockNumber++;
			}
			else
			{
			  newStartingBlockNumber--;
			}
			break; 
      case CFGTarget :	localTargetList.push_back(id);
			break;
      default :		if (! localTargetList.empty())
			{
			  while ( ! localTargetList.empty() && (*(--localTargetList.end())) > id)
			  {
			    localTargetList.remove( *(--localTargetList.end()) );
			  }
			}
			break;
    }
    for (list<CFGBlock *>::iterator iter = nextBlocks.begin(); iter != nextBlocks.end(); iter++)
    {
      bool result = (*iter)->needsDPE(newStartingBlockNumber, localTargetList);
      childrenDPE = childrenDPE || result;
    }
    if (type == CFGSource)
    {
      childrenDPE = true;
    }
    
    return dpe = (childrenDPE && ((hasStartingBlock > 0) || !localTargetList.empty()) );
  }
  else
  {
    return childrenDPE;
  }
}

/// resets the processed flag to false
void CFGBlock::resetProcessedFlag(bool withLinks, bool forward)
{
  if (! processed)
  {
    return;
  }
  processed = false;
  
  if (forward && ! nextBlocks.empty())
  {
    for (list<CFGBlock *>::iterator iter = nextBlocks.begin(); iter != nextBlocks.end(); iter++)
    {
      (*iter)->resetProcessedFlag(withLinks, forward);
    }
    if (withLinks && type == CFGSource)
    {
      targets[dot_name()]->resetProcessedFlag(withLinks, forward);
    }
  }
  else if (! forward && ! prevBlocks.empty())
  {
    for (list<CFGBlock *>::iterator iter = prevBlocks.begin(); iter != prevBlocks.end(); iter++)
    {
      (*iter)->resetProcessedFlag(withLinks, forward);
    }
    if (withLinks && type == CFGTarget)
    {
      sources[dot_name()]->resetProcessedFlag(withLinks, forward);
    }
  }
  
}

/**
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

/***************************** Program Analysis *******************************/

/**
 * Checks for uninitialized variables.
 * This is a forward-must analysis of the data flow.
 *
 *
 */
void CFGBlock::checkForUninitializedVariables()
{

  if (processed)
  {
    return;
  }
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
  std::string var = "";
  std::string attributeName = "variable";

  switch(type)
  {
    case CFGReply     : var = ASTEmap[id]->variableName; break;
    case CFGFrom      : var = ASTEmap[id]->variableName; break;
    case CFGInvoke    : var = ASTEmap[id]->inputVariableName; 
			attributeName = "inputVariable";
			break;
  }
  
  if ((type == CFGReply || type == CFGFrom || type == CFGInvoke) && var != "")
  {
    if (initializedVariables.find(var) == initializedVariables.end())
    {
      trace("[CFG] WARNING: Variable \"" + temporaryAttributeMap[id][attributeName] 
//	      + "\" ("+ var +") (activity ID = " 
//	      + toString(id->value)  
	      + "\" in line " 
// TODO new line number	      + toString(symTab.readAttribute(id, attributeName)->line) 
	      + " might be undefined!\n\n");
    }
  }
  
  // adding Variables for this Block
  var = "";
  attributeName = "variable";
    
  switch(type)
  {
    case CFGReceive   : var = ASTEmap[id]->variableName; break;
    case CFGCatch     : var = ASTEmap[id]->variableName; 
			attributeName = "faultVariable"; 
			break;
    case CFGTo        : var = ASTEmap[id]->variableName; break;
    case CFGInvoke    : var = ASTEmap[id]->outputVariableName;
			attributeName = "outputVariable";
			break;
    case CFGOnMessage : var = ASTEmap[id]->variableName; break;
//      default: // should not happen (thinks Niels)
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
    std::string linkname = "";

    if (type == CFGTarget)
    {
      linkname = ASTEmap[id]->linkName; //(dynamic_cast<STSourceTarget*>(symTab.lookup(id)))->link->name;
    }
    else if (type == CFGSource)
    {
      linkname = ASTEmap[id]->linkName; //(dynamic_cast<STSourceTarget*>(symTab.lookup(id)))->link->name;
    }

    
    if (type == CFGSource)
    {
      if (!targetsSeen.empty() && targetsSeen.find(linkname) != targetsSeen.end())
      {
        // triggers SA00072
	SAerror(72, ASTEmap[ id ]->attributes["linkName"], toInt(ASTEmap[id]->attributes["referenceLine"]));
	return;
      }
      CFGBlock * targ = targets[dot_name()];
      targ->targetsSeen = setUnion(targ->targetsSeen, targetsSeen);
      targ->checkForCyclicLinks();
    }
    else if (type == CFGTarget)
    {
      targetsSeen.insert(linkname);
    }

    if (!nextBlocks.empty())
    {
      for (list<CFGBlock*>::iterator iter = nextBlocks.begin(); iter != nextBlocks.end(); iter++)
      {
        (*iter)->targetsSeen = setUnion((*iter)->targetsSeen, targetsSeen);
	(*iter)->checkForCyclicLinks();
      }
    }
  }
}

/// checks for cycles in control dependency
void CFGBlock::checkForCyclicControlDependency()
{
  if ( processed )
  {
    return;
  }
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
      unsigned int possiblePeer = ASTEmap[ ASTEmap[ sources[ dot_name() ]->id ]->parentActivityId ]->parentScopeId;
      unsigned int parentId = id;
      parentId = ASTEmap[ parentId ]->parentActivityId;

      while ( parentId != 1 && parentId != possiblePeer )
      {
	parentId = ASTEmap[parentId]->parentScopeId;
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
      for ( set< unsigned int >::iterator enclosedScope = ASTEmap[ ASTEmap[ id ]->parentScopeId ]->enclosedScopes.begin(); 
					  enclosedScope != ASTEmap[ ASTEmap[ id ]->parentScopeId ]->enclosedScopes.end(); 
					  enclosedScope++)
      {
	if ( ASTEmap[ *enclosedScope ]->parentScopeId == ASTEmap[ id ]->parentScopeId )
	{
	  child = *enclosedScope;
	}
      }
      if ( child != 0 )
      {
	set< unsigned int > subscopes ( ASTEmap[ child ]->peerScopes );
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
                extern map< std::string, CFGBlock* > cfgMap;
		seen[ current ] = true;
//		for ( set< unsigned int >::iterator peer = ASTEmap[ current ]->peerScopes.begin(); peer != ASTEmap[ current ]->peerScopes.end(); peer++ )
		for ( set< unsigned int >::iterator peer = cfgMap[ toString(current) ]->lastBlock->controllingPeers.begin(); peer != cfgMap[ toString(current) ]->lastBlock->controllingPeers.end(); peer++ )
		{
		  // is there a cycle?
//                  cerr << "    peer " << *peer << endl;
		  if ( seen [ *peer ] )
		  {
		    SAerror( 82, "", toInt( ASTEmap[ *peer ]->attributes[ "referenceLine" ] ) );
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
    return;
  }
}

/// checks for conflicting receives
void CFGBlock::checkForConflictingReceive()
{
  if (processed)
  {
    return;
  }

  processed = true;
  
  bool allPrerequisites = true;
  // check if all entries are allready processed
  if (!nextBlocks.empty())
  {
    /// for a while, we assume, that the body is never executed, so we drop that set
    list<CFGBlock *>::iterator blockBegin = nextBlocks.begin();
    if (type == CFGWhile || type == CFGForEach || type == CFGRepeatUntil && label == "Until" )
    {
      blockBegin++;
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
	blockBegin++;
      }
      receives = (*blockBegin)->receives;
      for (list<CFGBlock *>::iterator iter = blockBegin; iter != nextBlocks.end(); iter++)
      {
	// the actual check for duplicate receives but only for flows
	if (type == CFGFlow && label == "Flow_begin" || type == CFGProcess && label == "Process_begin")
	{
	  for (set< pair< std::string, long> >::iterator elemA = (*iter)->receives.begin(); elemA != (*iter)->receives.end(); elemA++)
	  {
	    for (set< pair< std::string, long> >::iterator elemB = receives.begin(); elemB != receives.end(); elemB++)
	    {
	      if(elemA->first == elemB->first && elemA->second != elemB->second && receives.find(*elemA) == receives.end())
	      {
		trace("[CFG] WARNING: There are conflicting receives!\n");
//		trace("               Please check lines " + toString((dynamic_cast<STElement*>(symTab.lookup(elemA->second)))->line));
//		trace(                " and " + toString((dynamic_cast<STElement*>(symTab.lookup(elemB->second)))->line) + "\n");
		cerr << "               " << elemA->first << " (" << elemA->second << ") vs. " << elemB->first << " (" << elemB->second << ")" << endl;
		trace("\n");
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
	      trace("[CFG] WARNING: There are conflicting onMessage conditions!\n");
//	      trace("               Please check lines " + toString((dynamic_cast<STElement*>(symTab.lookup((*iter)->id)))->line));
//	      trace(                " and " + toString((dynamic_cast<STElement*>(symTab.lookup((*otherBlock)->id)))->line) + "\n");
	      cerr << "               " << (*iter)->channel_name << " (" << (*iter)->id << ") vs. " << (*otherBlock)->channel_name << " (" << (*otherBlock)->id << ")" << endl;
		trace("\n");
	    }
	  }
	  receives.insert(pair<std::string, long>( ASTEmap[id]->channelName, (*iter)->id));
	}
	// 
	receives = setUnion(receives, (*iter)->receives);
      }
    }
    if (type == CFGSource)
    {
      for (set< pair< std::string, long> >::iterator elemA = targets[dot_name()]->receives.begin(); elemA != targets[dot_name()]->receives.end(); elemA++)
      {
	for (set< pair< std::string, long> >::iterator elemB = receives.begin(); elemB != receives.end(); elemB++)
	{
	  if(elemA->first == elemB->first && elemA->second != elemB->second && receives.find(*elemA) == receives.end())
	  {
	      trace("[CFG] WARNING: There are conflicting receives!\n");
//	      trace("               Please check lines " + toString((dynamic_cast<STElement*>(symTab.lookup(elemA->second)))->line));
//	      trace(                " and " + toString((dynamic_cast<STElement*>(symTab.lookup(elemB->second)))->line) + "\n");
	      cerr << "               " << elemA->first << " (" << elemA->second << ") vs. " << elemB->first << " (" << elemB->second << ")" << endl;
		trace("\n");
	  }
	}
      }
      receives = setUnion(receives, targets[dot_name()]->receives);
    }
    if (type == CFGReceive)
    {
      receives.insert(pair<std::string, long>( ASTEmap[id]->channelName, id));
    }
    if (type == CFGInvoke)
    {
      if( temporaryAttributeMap[id]["outputVariable"] != "") {
	receives.insert(pair<std::string, long>( ASTEmap[id]->channelName, id));
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
    return;
  }

}




void processCFG()
{
  extern kc::tProcess TheProcess;

  assert(modus == M_CFG);

  TheCFG = NULL;
  trace(TRACE_INFORMATION, "-> Unparsing AST to CFG ...\n");
  TheProcess->unparse(kc::pseudoPrinter, kc::cfg);
  
  //trace(TRACE_DEBUG, "[CFG] checking for DPE\n");
  // do some business with CFG
  //list<int> kcl;
  //TheCFG->needsDPE(0, kcl);
  //TheCFG->resetProcessedFlag();

  trace(TRACE_DEBUG, "[CFG] checking for cyclic links\n");
  /// \todo (gierds) check for cyclic links, otherwise we will fail
  TheCFG->checkForCyclicLinks();
  TheCFG->resetProcessedFlag(true);

  trace(TRACE_DEBUG, "[CFG] checking for cyclic control dependency\n");
  TheCFG->checkForCyclicControlDependency();
  TheCFG->resetProcessedFlag(true, false);

  trace(TRACE_DEBUG, "[CFG] checking for uninitialized variables\n");
  // test
  TheCFG->checkForUninitializedVariables();
  TheCFG->resetProcessedFlag();
  // end test

  TheCFG->lastBlock->checkForConflictingReceive();
  TheCFG->resetProcessedFlag(true, false);

  if (formats[F_DOT])
  {
    if (output_filename != "")
      output = openOutput(output_filename + ".cfg." + suffixes[F_DOT]);
    
    trace(TRACE_INFORMATION, "-> Printing CFG in dot ...\n");
    
    // output CFG;
    cfgDot(TheCFG);
    
    if (output_filename != "")
    {
      closeOutput(output);
      output = NULL;
    }
  }

  delete(TheCFG);
}
