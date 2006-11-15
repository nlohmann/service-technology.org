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
 * \file cfg.h
 *
 * \brief Functions for the Control Flow Graph (interface)
 *
 * This file provides the necessary classes for building a Control Flow Graph
 * 
 * \author  
 *          - responsible: Christian Gierds <gierds@informatik.hu-berlin.de>
 *          - last changes of: \$Author: gierds $
 *          
 * \date
 *          - created: 2006-01-19
 *          - last changed: \$Date: 2006/11/15 14:32:41 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel
 *          for details.
 *
 * \version \$Revision: 1.23 $
 */

#ifndef CFG_H
#define CFG_H

#include <string>
#include <iostream>
#include <list>
#include <map>
#include "helpers.h"
//#include "ast-config.h"		// all you need from Kimwitu++



/******************************************************************************
 * External variables
 *****************************************************************************/

extern std::string filename;



/// enumeration for the possible block types, that can occur in the CFG
typedef enum
{
  CFGProcess = 1,
  CFGFaultHandler,
  CFGCatch,
  CFGCatchAll,
  CFGCompensationHandler,
  CFGEventHandler,
  CFGEmpty,
  CFGInvoke,
  CFGReceive,
  CFGReply,
  CFGAssign,
  CFGCopy,
  CFGFrom,
  CFGTo,
  CFGWait,
  CFGThrow,
  CFGCompensate,
  CFGTerminate,
  CFGFlow,
  CFGSwitch,
  CFGCase,
  CFGOtherwise,
  CFGWhile,
  CFGSequence,
  CFGPick,
  CFGOnAlarm,
  CFGOnMessage,
  CFGScope,
  CFGTarget,
  CFGSource,
  // WS-BPEL 2.0
  CFGValidate,
  CFGExit,
  CFGRethrow,
  CFGCompensateScope,
  CFGIf,
  CFGElseIf,
  CFGElse,
  CFGForEach,
  CFGRepeatUntil
} CFGBlockType;

// forward declarations
class CFGBlock;

/// map for assigning a target to its appropriate source object
extern map<std::string, CFGBlock *> sources;
/// map for assigning a source to its appropriate target object
extern map<std::string, CFGBlock *> targets;

/**
 * This class is a generic template for all blocks within
 * a CFG.
 * 
 */
class CFGBlock {
  public:
    /// list of pointers to the previous blocks	  
    list<CFGBlock *> prevBlocks;
    /// list of pointers to the next blocks 
    list<CFGBlock *> nextBlocks;

    /// pointer to the first block of an activity
    CFGBlock * firstBlock;
    /// pointer to the last block of an activity
    CFGBlock * lastBlock;

    
    /// type of the block
    CFGBlockType type;
    /// ID of the block
    int id;
    /// label of the block
    std::string label;

    /// constructor
    CFGBlock();
    /// constructor
    CFGBlock(CFGBlockType, int, std::string);
    /// generic constructor
    CFGBlock(std::string id);
    /// generic constructor
    CFGBlock(int id);
    /// generic destructor
    virtual ~CFGBlock();
    
    /// returns the concrete type
    CFGBlockType getType(); 
    /// dot_output
    virtual void print_dot();
    /// the name of the dot node
    virtual std::string dot_name();

    /// additional name of the channel
    std::string channel_name;

    /// checks which blocks need DPE
    bool needsDPE(int hasStartingBlock, list<int> lastTarget);

    /// resets the processed flag to false
    void resetProcessedFlag(bool withLinks = false, bool forward = true);
    

/***************************** Program Analysis *******************************/

    /// checks if variables might be uninitialized
    void checkForUninitializedVariables();

    /// checks for cyclic links
    void checkForCyclicLinks();

    /// checks for cycles in control dependency (SA00082)
    void checkForCyclicControlDependency();

    /// checks for conflicting receives
    void checkForConflictingReceive();

  private:
    /// flag, if block was dotted
    bool dotted;
    /// flag, if block was processed by an algorithm (has to be resetted afterwards)
    bool processed;

/***************************** Program Analysis *******************************/
    /// set of initialized variables
    set<std::string> initializedVariables;

    /// set of targets seen so far
    set<std::string> targetsSeen;

    /// set of depending receives
    set< pair< std::string, long > > receives;

    set< unsigned int > controllingPeers;
    
    /// \todo: move to symbol table
    bool dpe;
    list<CFGBlock * > dpeList;
	  
};

/// wrapper for dot printing (prints all the graph data)
void cfgDot(CFGBlock *);

/// connects two blocks
void connectBlocks(CFGBlock *, CFGBlock *);


void processCFG();

#endif

