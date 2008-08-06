/*****************************************************************************\
  GNU BPEL2oWFN -- Translating BPEL Processes into Petri Net Models

  Copyright (C) 2006, 2007  Niels Lohmann,
                            Christian Gierds, and
                            Martin Znamirowski
  Copyright (C) 2005        Niels Lohmann and
			    Christian Gierds

  GNU BPEL2oWFN is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the Free
  Software Foundation; either version 3 of the License, or (at your option) any
  later version.

  GNU BPEL2oWFN is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
  details.

  You should have received a copy of the GNU General Public License along with
  GNU BPEL2oWFN (see file COPYING); if not, see http://www.gnu.org/licenses
  or write to the Free Software Foundation,Inc., 51 Franklin Street, Fifth
  Floor, Boston, MA 02110-1301  USA.
\*****************************************************************************/

/*!
 * \file    petrinet.h
 *
 * \brief   Petri Net API
 *
 * \author  Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          Christian Gierds <gierds@informatik.hu-berlin.de>,
 *          Martin Znamirowski <znamirow@informatik.hu-berlin.de>,
 *          last changes of: \$Author: nielslohmann $
 *
 * \since   2005/10/18
 *
 * \date    \$Date: 2007/06/28 07:38:17 $
 *
 * \note    This file is part of the tool GNU BPEL2oWFN and was created during
 *          the project Tools4BPEL at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \version \$Revision: 1.153 $
 *
 * \ingroup petrinet
 */





#ifndef BLOCK_H
#define BLOCK_H





/******************************************************************************
 * Headers
 *****************************************************************************/

#include <string>
#include <ostream>
#include <vector>
#include <set>
#include <map>
#include <list>
#include <deque>
#include "pnapi.h"
#include "bom-process.h"

using std::string;
using std::vector;
using std::deque;
using std::set;
using std::map;
using std::list;
using std::pair;
using std::ostream;





/******************************************************************************
 * Data structures
 *****************************************************************************/
typedef enum
{
  PROCESS,
  SERVICE,			///< input of an open workflow net (oWFN)
  TASK,			///< output of an open workflow net (oWFN
  STARTNODE,			///< input of an open workflow net (oWFN)
  STOPNODE,			///< input of an open workflow net (oWFN)
  ENDNODE,			///< input of an open workflow net (oWFN)
  DECISION,			///< input of an open workflow net (oWFN)
  MERGE,			///< input of an open workflow net (oWFN)
  FORK,			///< input of an open workflow net (oWFN)
  JOIN,			///< input of an open workflow net (oWFN)
  NOTIFICATIONBROADCASTER,			///< input of an open workflow net (oWFN)
  NOTIFICATIONRECEIVER,			///< input of an open workflow net (oWFN)
  OBSERVER,			///< input of an open workflow net (oWFN)
  MAP,			///< input of an open workflow net (oWFN)
  TIMER,			///< input of an open workflow net (oWFN)
  LOOP,			///< input of an open workflow net (oWFN)
  FORLOOP,			///< input of an open workflow net (oWFN)
  CALLTOPROCESS,			///< input of an open workflow net (oWFN)
  CALLTOSERVICE,			///< input of an open workflow net (oWFN)
  CALLTOTASK			///< input of an open workflow net (oWFN)
} blockType;



/******************************************************************************
 * Classes
 *****************************************************************************/
class BlockConnection;
class Block;

class BlockConnection
{
    /// class PetriNet is allowed to access the privates of class Transition
    friend class Block;

    private:
    
    /// target and source blocks of this connection
    Block* tgt;
    Block* src;
    
    /// the connection is valid only if its source and target blocks exist
    /// and if the right pins could be found in those blocks
    bool valid;
    
    /// DECOMPOSITION a connection is rolecrossing if its target and 
    /// source block differ in their roles depending on the chosen 
    /// mode for this "difference"
    bool roleCrossing;
    
    /// DECOMPOSITION: A connection is only enabled if it is connected 
    /// to the current roleset
    bool enabled;  
    
    /// Only relevant for connections from/to in-/outputs of the process
    /// itself. Determines whether this connection represents and input
    /// or an output (in case of false)
    bool input;
    
    /// pointer to the transition leading from the source pin to the connection place
    Transition* firstTransition;

    /// pointer to the transition from the connectionplace to the destination pin
    Transition* secondTransition;
    
    /// pointer to the connection place 
    Place* connectionPlace;
    
    /// a pair of Arcs leading to and from the first transition 
    pair<Arc*, Arc*> firstArcs;

    /// a pair of Arcs leading to and from the first transition
    pair<Arc*, Arc*> secondArcs;
    

    public:

    /// name of the connection in the process
    string name;

    /// name of the target block
    string target;

    /// name of the source block
    string source;

    /// name of the pin in the target block
    string targetContact;

    /// name of the pin in the source block
    string sourceContact;
    
    /// true if the pin is not in the input set of the target block, but in the
    /// additional input set of the target block
    bool targetAdditional;

    /// true if the pin is not in the output set of the source block, but in the
    /// additional output set of the source block
    bool sourceAdditional;

    /// returns the name of the connection
    string connectionName();    
    
    /// basic constructor
    BlockConnection();
    
    /// destructor
    virtual ~BlockConnection();
    
    /// returns the target block of the connection
    Block* getTarget();
  
};

class Block
{
    friend class BlockConnection;
    friend class BomProcess;

    private:
  
	/// is true if the block is atomic and can be represented by a place
    /// for example Startnode and Endnode
    bool atomic;
    
    /// is true if the block has an own flow content as for example processes
    bool complex;
    
    /// DECOMPOSITION: Is true, if this block is part of the current role set
    bool enabled;
    
    /// Set of all inputs of the block
    set<string> inputs;

    /// Set of all outputs of the block
    set<string> outputs;

    /// Set of all inputCriteria of the block
    set<string> inputCriteria;

    /// Set of all outputCriteria of the block
    set<string> outputCriteria;

    /// Set of all inputBranches of the block
    set<string> inputBranches;

    /// Set of all outputBranches of the block
    set<string> outputBranches;
    
    /// Set of additional Inputs of the block
    set<string> additionalInputs;

    /// Set of additional Outputs of the block
    set<string> additionalOutputs;
    
    /// All connections of the current block. Only valid for complex Blocks.
    set<BlockConnection *> connections;
    
    // DECOMPOSTION: set of connections that have either an input or a startnode
    //               as source
    set<BlockConnection *> startingConnections;

    // DECOMPOSTION: set of connections that have either an output or a endnode
    //               as target    
    set<BlockConnection *> endingConnections;
    
    /// the set of rolerequirements for this block
    set<string> roleRequirements;
    
    /// Actually translates all connections of this block into transitions
    /// and places for the petrinet and connects them
    void translateConnections(PetriNet* PN, BomProcess *bom);
    
    void inclusiveDecisionPatternRecursively(Block* decision, Place* centralNode, set<string>& currentOutputBranches, string number, PetriNet* PN, BomProcess* bom, map<string,bool>& generated, set<set<string> >& powerSet);
    
    /// returns a prefix-string for node names at the global level 
    const string globalPrefixString() const;
    
    /// copy interface description from a declaring block to this node
    void instantiateInterface(Block *sourceBlock);
  
  public:

    // saves for every input and output, if it has a wrong multiplicity
    map<string, bool> wrongMultiplicity;
    
    /// contains the set of inputs for every inputCriterion
	map<string, set<string> > inputCriterionSet;

    /// contains the set of outputs for every outputCriterion
	map<string, set<string> > outputCriterionSet;

    /// contains the set of inputs for every inputBranch
	map<string, set<string> > inputBranchSet;

    /// contains the set of outputs for every outputBranch
    map<string, set<string> > outputBranchSet;

    /// contains the set of inputCriteria for every AdditionalInput
    map<string, set<string> > additionalInputSet;

    /// contains the set of outputCriteria for every AdditionalOutput
    map<string, set<string> > additionalOutputSet;

    /// maps every outputCriterion to its related inputCriteria
    map<string, set<string> > criterionMap;
    
    /// This variable is only used for processes. If it is true, the process
    /// will not be translated due to violating the filtering rules
    bool filtered;
    
    /// used to flag a block as erroneous (debugging purpose)
    bool syntaxError;
    
    /// basic constructor of Block
    Block (blockType mytype, bool ifcomplex, bool ifatomic);
    
    /// the name of the block
    std::string name;   

    /// all blocks mentioned in the flow content of this block. Only given
    /// if this block is complex
    set<Block*> children;

    /// Adds an Input to the Block
    void addInput(string inputString);

    /// Adds an Output to the Block
    void addOutput(string outputString);

    /// Adds an InputCriterion to the Block
    void addInputCriterion(string inputString);

    /// Adds an OutputCriterion to the Block
    void addOutputCriterion(string outputString);

    /// Adds an InputBranch to the Block
    void addInputBranch(string inputString);

    /// Adds an OutputBranch to the Block
    void addOutputBranch(string outputString);

    /// Adds an AdditionalInput to the Block
    void addAdditionalInput(string inputString);

    /// Adds an AdditionalOutput to the Block
    void addAdditionalOutput(string outputString);

    /// adds a connectin to the set of this blocks connections (only if complex)
    void addConnection(BlockConnection* con);

    /// adds a role to the set of this blocks roles
    void addRole(string Role);
    
    /// in case the block is a complex process with no contents
    bool isComplexEmpty();
    
    /// checks whether this block have to be filtered
    /// out due to having overlapping inputsets and/or outputsets
    void filter();
    
    /// creates a simple output of this blocks structure for debugging
    void simpleOutput(string delay);

    /// Checks every connecton of this block (if complex) if the information
    /// read in the parser are valid, such that source and target of the connections
    /// exist
    void linkNodes();
    
    /// For every CallTo-Block, the information is not in the flow content
    /// but rather needs to be searched for in other parts of the xml in order
    /// to get information about inputs, outputs, roles etc.
    void linkInserts();
    
    /// Actually copies the names of the blocks stored by the parser into the
    /// blocks' string variable "name"
    void transferName();

    /// DECOMPOSITION: Changes the roles of roleless control flow components
    ///                so that they become part of the roleview
    void adjustRoles();

    /// DECOMPOSITION: Disables the original starting connections of the process
    void disableStart();

    /// DECOMPOSITION: Disables the original ending connections of the process
    void disableEnd();

    /// DECOMPOSITION: Merges the start for a cut net, not correct in theory
    void mergeStart(PetriNet* PN);

    /// DECOMPOSITION: Merges the ends for a cut net, not correct in theory
    void mergeEnd(PetriNet* PN);

    /// DECOMPOSITION: Tries to creat a correct final condition for a cut net
    //                 So far not working at all.
    void createEndState(PetriNet* PN);

    /// the type of the block as string
    string returnType() const;
    
    /// the attributes of the block as stored by the parser
    map<string, string> attributes;

    /// pointer to a parent block, if this block is part of a flow content
    Block* parent;

    /// type of block in #communication_type
    blockType type;
    
    /// destructor
    virtual ~Block();
    
    /// translates all blocks in this complex block to a petri net and connects them
    PetriNet* returnNet(PetriNet* PN, BomProcess *bom);

    /// DECOMPOSITION: Sets the enabled and disabled values for a decompostion
    ///                of the net
    void cutNet(set<string> roleSet, int mode);

};





/*****************************************************************************/
#endif





/*!
 * \defgroup petrinet Petri Net API
 *
 * \author Niels Lohmann <nlohmann@informatik.hu-berlin.de>
 *
 * All functions needed to organize a Petri net representation that can be
 * written to several output file formats and that supports structural
 * reduction rules.
 */
 
 
