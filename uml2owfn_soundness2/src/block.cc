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
 * \file    petrinet.cc
 *
 * \brief   Petri Net API: base functions
 *
 * \author  Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          Christian Gierds <gierds@informatik.hu-berlin.de>,
 *          Martin Znamirowski <znamirow@informatik.hu-berlin.de>,
 *          last changes of: \$Author: nielslohmann $
 *
 * \since   2005-10-18
 *
 * \date    \$Date: 2007/06/28 07:38:17 $
 *
 * \note    This file is part of the tool GNU BPEL2oWFN and was created during
 *          the project Tools4BPEL at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \version \$Revision: 1.222 $
 *
 * \ingroup petrinet
 */
/******************************************************************************
 * Headers
 *****************************************************************************/
#include <cassert>
#include <iostream>
#include <utility>
#include "block.h"
#include "globals.h"
#include "helpers.h"	// helper functions (setUnion, setDifference, max, toString)
#include "pnapi.h"		// helper functions (setUnion, setDifference, max, toString)
#include "debug.h"		// debug and error message support

using std::pair;
using std::cerr;
using std::endl;
using std::string;

/******************************************************************************
 * Implementation of class functions
 *****************************************************************************/

//! \brief basic deconstructor of BlockConnection
BlockConnection::~BlockConnection()
{
}


//! \brief basic constructor of BlockConnection
BlockConnection::BlockConnection()
{
    targetAdditional = false;
    sourceAdditional = false;
    valid = false;
    enabled = true;
    roleCrossing = false;
    input = false;
    
    tgt = NULL;
    src = NULL;
}


//! \brief returns the name of a connection
string BlockConnection::connectionName()
{
    return name;
}


//! \brief Adds an input to the block
//! \param inputString input to add
void Block::addInput(string inputString)
{
    inputs.insert(inputString);
}


//! \brief Adds an output to the block
//! \param outputString output to add
void Block::addOutput(string outputString)
{
    outputs.insert(outputString);
}


//! \brief Adds an inputCriterion to the block
//! \param inputString inputCriterion to add
void Block::addInputCriterion(string inputString)
{
    inputCriteria.insert(inputString);
}


//! \brief Adds an outputCriterion to the block
//! \param outputString outputCriterion to add
void Block::addOutputCriterion(string outputString)
{
    outputCriteria.insert(outputString);
}


//! \brief Adds an inputBranch to the block
//! \param inputString inputBranch to add
void Block::addInputBranch(string inputString)
{
    inputBranches.insert(inputString);
}


//! \brief Adds an outputBranch to the block
//! \param outputString outputBranch to add
void Block::addOutputBranch(string outputString)
{
    outputBranches.insert(outputString);
}


//! \brief Adds an additionalInput to the block
//! \param inputString additionalInput to add
void Block::addAdditionalInput(string inputString)
{
    additionalInputs.insert(inputString);
}


//! \brief Adds an additionalOutput to the block
//! \param outputString additionalOutput to add
void Block::addAdditionalOutput(string outputString)
{
    additionalOutputs.insert(outputString);
}

//! \brief Adds a connection to the set of connections of this block
//! \param con connection to add
void Block::addConnection(BlockConnection* con)
{
    connections.insert(con);
}


//! \brief Adds a role to the set of roles of this block
//! \param Role role to add
void Block::addRole(string Role)
{
    roleRequirements.insert(Role);
}

/*!
 * \brief check whether a complex block has no contents
 */
bool Block::isComplexEmpty() {
	return (children.empty());
}

//! \brief Checks whether overlapping inputsets and/or outputsets exist
//!        and filters this block out if so
void Block::filter()
{
	if (filtered)	// is already marked for filtering
		return;
	if (parent != NULL && parent->filtered)
		return;
	
    // check if the inputSets are overlapping
    set<string> alreadyUsedInputs;
    for (set<string>::iterator currentInputCriterion = inputCriteria.begin(); currentInputCriterion != inputCriteria.end(); currentInputCriterion++)
    {
    	/*
    	// empty input criterion (has no inputs)
    	if (inputCriterionSet[*currentInputCriterion].begin() == inputCriterionSet[*currentInputCriterion].end()) {
    		filtered = true;
    		trace(TRACE_INFORMATION, returnType() + " \"" + attributes["name"] + "\" has an empty inputCriterion and is candidate for filtering.\n");
    		return;
    	}
    	*/
    	// non empty input critertion, check for overlaps
        for (set<string>::iterator currentInput = inputCriterionSet[*currentInputCriterion].begin(); currentInput != inputCriterionSet[*currentInputCriterion].end(); currentInput++)
        {
            if (!alreadyUsedInputs.insert(*currentInput).second)
            {
                filtered = true;
                if (complex || parent == NULL)
                {
                	trace(TRACE_INFORMATION, returnType() + " \"" + attributes["name"] + "\" has overlapping inputCriteria and is candidate for filtering.\n");
                    //cerr << returnType() << " \"" << attributes["name"] << "\" has overlapping inputCriteria and is candidate for filtering." << endl;
                    return;
                }
                else
                {
                	trace(TRACE_INFORMATION, returnType() + " \"" + attributes["name"] + "\" has overlapping inputCriteria, its containing process \"" + parent->attributes["name"] + "\" is candidate for filtering.\n");
                    //cerr << returnType() << " \"" << attributes["name"] << "\" has overlapping inputCriteria, its containing process \"" << parent->attributes["name"] << "\" is candidate for filtering."  << endl;
                    return;
                }
            }
        }
    }

    // check if the outputSets are overlapping
    set<string> alreadyUsedOutputs;
    for (set<string>::iterator currentOutputCriterion = outputCriteria.begin(); currentOutputCriterion != outputCriteria.end(); currentOutputCriterion++)
    {
    	/*
    	// empty output criterion (has no outputs)
    	if (outputCriterionSet[*currentOutputCriterion].begin() == outputCriterionSet[*currentOutputCriterion].end()) {
    		filtered = true;
    		trace(TRACE_INFORMATION, returnType() + " \"" + attributes["name"] + "\" has an empty outputCriterion and is candidate for filtering.\n");
    		return;
    	}
    	*/
    	// non empty output critertion, check for overlaps
        for (set<string>::iterator currentOutput = outputCriterionSet[*currentOutputCriterion].begin(); currentOutput != outputCriterionSet[*currentOutputCriterion].end(); currentOutput++)
        {
            if (!alreadyUsedOutputs.insert(*currentOutput).second)
            {
                filtered = true;
                if (complex || parent == NULL)
                {
                	trace(TRACE_INFORMATION, returnType() + " \"" + attributes["name"] + "\" has overlapping outputCriteria and is candidate for filtering.\n");
                    //cerr << returnType() << " \"" << attributes["name"] << "\" has overlapping outputCriteria and is candidate for filtering." << endl;
                    return;
                }
                else
                {
                	trace(TRACE_INFORMATION, returnType() + " \"" + attributes["name"] + "\" has overlapping outputCriteria, its containing process \"" + parent->attributes["name"] + "\" is candidate for filtering.\n");
                    //cerr << returnType() << " \"" << attributes["name"] << "\" has overlapping outputCriteria, its containing process \"" << parent->attributes["name"] << "\" is candidate for filtering." << endl;
                    return;
                }
            }
        }
    }
}


//! \brief returns the type of this block as a string
//! \return the type of this block as a string
string Block::returnType() const
{
    if ( type == PROCESS) return "process";
    if ( type == SERVICE) return "service";
    if ( type == TASK) return "task";
    if ( type == STARTNODE) return "startnode";
    if ( type == STOPNODE) return "stopnode";
    if ( type == ENDNODE) return "endnode";
    if ( type == DECISION) return "decision";
    if ( type == MERGE) return "merge";
    if ( type == FORK) return "fork";
    if ( type == JOIN) return "join";
    if ( type == NOTIFICATIONBROADCASTER) return "notificationBroadcaster";
    if ( type == NOTIFICATIONRECEIVER) return "notificationReceiver";
    if ( type == OBSERVER) return "observer";
    if ( type == MAP) return "map";
    if ( type == TIMER) return "timer";
    if ( type == LOOP) return "loop";
    if ( type == FORLOOP) return "forloop";
    if ( type == CALLTOPROCESS) return "callToProcess";
    if ( type == CALLTOSERVICE) return "callToService";
    if ( type == CALLTOTASK) return "callToTask";
return "no type";
}


//! \brief creates a simple cerr output of the whole structure of this
//!        block for debugging purposes. Parts ar outcommented dependant
//!        on the information that is needed (if none are commented out
//!        the output becomes enormous!)
//! \param delay a number of freespaces as string, so the output is more readable
void Block::simpleOutput (string delay)
{
	cerr << delay << returnType() << ": \""<< attributes["name"] << "\"";
    if (inputs.size() != 0)
    {
        cerr << endl << delay << "  Der Block hat folgende outputs!" << endl;
        for (set<string>::iterator input = inputs.begin(); input != inputs.end(); input++)
        {
            cerr << delay << "    " << (*input) << endl;
        }
    }
    if (outputs.size() != 0)
    {
        cerr << endl << delay << "  Der Block hat folgende Outputs!" << endl;
        for (set<string>::iterator output = outputs.begin(); output != outputs.end(); output++)
        {
            cerr << delay << "    " << (*output) << endl;
        }
    }
    if (roleRequirements.size() != 0)
    {
        cerr << endl << delay << "  Der Block hat folgende RoleRequirements!" << endl;
        for (set<string>::iterator role = roleRequirements.begin(); role != roleRequirements.end(); role++)
        {
            cerr << delay << "    " << (*role) << endl;
        }
    }
   /*
    if (inputCriteria.size() != 0)
    {
        cerr << endl << delay << "  Der Block hat folgende InputCriteria!" << endl;
        for (set<string>::iterator inputCriterion = inputCriteria.begin(); inputCriterion != inputCriteria.end(); inputCriterion++)
        {
            cerr << delay << "    " << (*inputCriterion) << endl;
            for (set<string>::iterator input = inputCriterionSet[(*inputCriterion)].begin(); input != inputCriterionSet[(*inputCriterion)].end(); input++)
            {
                cerr << delay << "      " << (*input) << endl;
            }
        }
    }
    if (outputCriteria.size() != 0)
    {
        cerr << endl << delay << "  Der Block hat folgende OutputCriteria!" << endl;
        for (set<string>::iterator outputCriterion = outputCriteria.begin(); outputCriterion != outputCriteria.end(); outputCriterion++)
        {
            cerr << delay << "    " << (*outputCriterion) << endl;
            for (set<string>::iterator output = outputCriterionSet[(*outputCriterion)].begin(); output != outputCriterionSet[(*outputCriterion)].end(); output++)
            {
                cerr << delay << "      " << (*output) << endl;
            }
        }
    }
    if (inputBranches.size() != 0)
    {
        cerr << endl << delay << "  Der Block hat folgende InputBranches!" << endl;
        for (set<string>::iterator inputBranch = inputBranches.begin(); inputBranch != inputBranches.end(); inputBranch++)
        {
            cerr << delay << "    " << (*inputBranch) << endl;
            for (set<string>::iterator input = inputBranchSet[(*inputBranch)].begin(); input != inputBranchSet[(*inputBranch)].end(); input++)
            {
                cerr << delay << "      " << (*input) << endl;
            }
        }
    }
    if (outputBranches.size() != 0)
    {
        cerr << endl << delay << "  Der Block hat folgende OutputBranches!" << endl;
        for (set<string>::iterator outputBranch = outputBranches.begin(); outputBranch != outputBranches.end(); outputBranch++)
        {
            cerr << delay << "    " << (*outputBranch) << endl;
            for (set<string>::iterator output = outputBranchSet[(*outputBranch)].begin(); output != outputBranchSet[(*outputBranch)].end(); output++)
            {
                cerr << delay << "      " << (*output) << endl;
            }
        }
    }
    if (additionalInputs.size() != 0)
    {
        cerr << endl << delay << "  Der Block hat folgende additionalInputs!" << endl;
        for (set<string>::iterator additionalInput = additionalInputs.begin(); additionalInput != additionalInputs.end(); additionalInput++)
        {
            cerr << delay << "    " << (*additionalInput) << endl;
            for (set<string>::iterator inputCriterion = additionalInputSet[(*additionalInput)].begin(); inputCriterion != additionalInputSet[(*additionalInput)].end(); inputCriterion++)
            {
                cerr << delay << "      " << (*inputCriterion) << endl;
            }
        }
    }
    if (additionalOutputs.size() != 0)
    {
        cerr << endl << delay << "  Der Block hat folgende additionalOutputs!" << endl;
        for (set<string>::iterator additionalOutput = additionalOutputs.begin(); additionalOutput != additionalOutputs.end(); additionalOutput++)
        {
            cerr << delay << "    " << (*additionalOutput) << endl;
            for (set<string>::iterator outputCriterion = additionalOutputSet[(*additionalOutput)].begin(); outputCriterion != additionalOutputSet[(*additionalOutput)].end(); outputCriterion++)
            {
                cerr << delay << "      " << (*outputCriterion) << endl;
            }
        }
    }
*/
    
    if (children.size() != 0)
    {
        cerr << endl << delay << "  Der Prozess hat folgende Kinder!" << endl;
        for (set<Block*>::iterator child = children.begin(); child != children.end(); child++)
        {
            (*child)->simpleOutput((delay+ "    "));
        }
        
        /*
        cerr << endl << delay << "  Der Prozess hat folgende Connections!" << endl;
        for (set<BlockConnection*>::iterator con = connections.begin(); con != connections.end(); con++)
        {
            cerr << delay << "    " << (*con)->name << ":" << endl;
            cerr << delay << "      ";
            if ((*con)->source != "")
            {
                cerr << "sourceNode: \"" << (*con)->source << "\" " << endl << delay << "      " ; 
            }
            if ((*con)->sourceContact != "")
            {
                cerr << "sourceContactPoint: \"" << (*con)->sourceContact << "\" " ; 
            }
            cerr << endl << delay << "      ";
            if ((*con)->target != "")
            {
                cerr << "targetNode: \"" << (*con)->target << "\" " << endl << delay << "      " ;
            }
            if ((*con)->targetContact != "")
            {
                cerr << "targetContactPoint: \"" << (*con)->targetContact << "\" " ; 
            }
            cerr << endl;
        }
        */
    }
    cerr << endl;
}


//! \brief fills the "name" variable of this block and of all of its children.
void Block::transferName ()
{
	//trace(TRACE_DEBUG, "setting name to: ");
    name=attributes["name"];
    //trace(TRACE_DEBUG, name+"\n");
    
    if (complex) {
    	for (set<Block*>::iterator child = children.begin(); child != children.end(); child++) {
    		
    		if (*child == this) {
    			trace(TRACE_ERROR, " [ERROR] node "+name+" has itself as child\n");
    			syntaxError = true;
    			return;
    		}
    		
    		// resolve names of children
    		(*child)->transferName();
    		
    		if ((*child)->syntaxError)	// inherit error value from child
    			syntaxError = true;
        }
    }
}

/*!
 * \brief extend the interface description of this block with
 *        interface the description of the source block, the
 *        method is called to instantiate callToTask, callToProcess,
 *        and callToService tasks
 */
void Block::instantiateInterface(Block* sourceBlock) {
	
	for (set<string>::iterator input = sourceBlock->inputs.begin(); input != sourceBlock->inputs.end(); input++) {
		//assert((*input) != st);
		this->inputs.insert((*input));
	}

	for (set<string>::iterator output = sourceBlock->outputs.begin(); output != sourceBlock->outputs.end(); output++) {
		//assert((*output) != NULL);
		this->outputs.insert((*output));
	}
	for (set<string>::iterator inputCriterion = sourceBlock->inputCriteria.begin(); inputCriterion != sourceBlock->inputCriteria.end(); inputCriterion++) {
		//assert((*inputCriterion) != NULL);
		this->inputCriteria.insert((*inputCriterion));
	}
	for (set<string>::iterator outputCriterion = sourceBlock->outputCriteria.begin(); outputCriterion != sourceBlock->outputCriteria.end(); outputCriterion++) {
		//assert((*outputCriterion) != NULL);
		this->outputCriteria.insert((*outputCriterion));
	}
	for (set<string>::iterator role = sourceBlock->roleRequirements.begin(); role != sourceBlock->roleRequirements.end(); role++) {
		//assert((*role) != NULL);
		this->roleRequirements.insert((*role));
	}

	this->inputCriterionSet = sourceBlock->inputCriterionSet;
	this->outputCriterionSet = sourceBlock->outputCriterionSet;
	this->criterionMap = sourceBlock->criterionMap;
	
	// the wrong multiplicities from the childs additional pins and 
	// the tasks original pins need to be merged
	this->wrongMultiplicity.insert(sourceBlock->wrongMultiplicity.begin(),sourceBlock->wrongMultiplicity.end());
    
    for (set<string>::iterator additionalInput = this->additionalInputs.begin(); additionalInput != this->additionalInputs.end(); additionalInput++)
    {
    	//assert((*additionalInput) != NULL);
        for (set<string>::iterator additionalInputCriterion = this->additionalInputSet[*additionalInput].begin(); additionalInputCriterion != this->additionalInputSet[*additionalInput].end(); additionalInputCriterion++) 
        {
        	//assert((*additionalInputCriterion) != NULL);
        	this->inputCriterionSet[*additionalInputCriterion].insert(*additionalInput);
            this->inputs.insert(*additionalInput);
        }
    }

    for (set<string>::iterator additionalOutput = this->additionalOutputs.begin(); additionalOutput != this->additionalOutputs.end(); additionalOutput++)
    {
    	//assert((*additionalOutput) != NULL);
        for (set<string>::iterator additionalOutputCriterion = this->additionalOutputSet[*additionalOutput].begin(); additionalOutputCriterion != this->additionalOutputSet[*additionalOutput].end(); additionalOutputCriterion++) 
        {
        	//assert((*additionalOutputCriterion) != NULL);
        	this->outputCriterionSet[*additionalOutputCriterion].insert(*additionalOutput);
            this->outputs.insert(*additionalOutput);
        }
    }
}


//! \brief searches this blocks children for all "callto" Blocks in order to
//!        gather needed information from the referenced tasks/processes/services.
//!        The complexity so far is awful and needs to be improved by redesigning.
void Block::linkInserts()
{
	filter();	// check filtering criteria for this block
	
    // Only do this, if this is a process
	if (complex)
    {
		// search all children
		for (set<Block*>::iterator childIt = children.begin(); childIt != children.end(); childIt++) {
			Block* child = (*childIt);
			assert(child != NULL);
			
			// if the childblock is a call to task
			if (child->type == CALLTOTASK)
			{
				// search the referenced task in all global tasks
				Block* task = NULL;
				for ( set<Block*>::iterator searchTask = globals::tasks.begin(); searchTask != globals::tasks.end(); searchTask++) {
					assert((*searchTask) != NULL);
					if ((*searchTask)->attributes["name"] == child->attributes["task"]) {
						task = *searchTask;
						break;
					}                    
				}
				//assert(task != NULL);
				if (task == NULL) {
					trace(TRACE_ERROR, "[ERROR] Task "+child->name+" references unknown global task "+child->attributes["task"]+". Skipping process in the translation.\n");
					child->filtered = true;
				} else {
					child->instantiateInterface(task);
				}
	        }
			// if the childblock is a call to process
			else if (child->type == CALLTOPROCESS)
			{
				// search the referenced process in all global processes
				Block* process = NULL;
				for ( set<Block*>::iterator searchProcess = globals::processes.begin(); searchProcess != globals::processes.end(); searchProcess++)
				{
	            	assert((*searchProcess) != NULL);
					if ((*searchProcess)->attributes["name"] == child->attributes["process"]) {
						process = *searchProcess;
						break;
					}
				}
				//assert(process != NULL);
				if (process == NULL) {
					trace(TRACE_ERROR, "[ERROR] Task "+child->name+" references unknown global process "+child->attributes["process"]+". Skipping process in the translation.\n");
					child->filtered = true;
				} else {
					child->instantiateInterface(process);
				}
			}
			// if the childblock is a call to service
			else if (child->type == CALLTOSERVICE)
			{
	
				// search the referenced service in all global services
				Block* service = NULL;
				for ( set<Block*>::iterator searchService = globals::services.begin(); searchService != globals::services.end(); searchService++)
				{
					if ((*searchService)->attributes["name"] == child->attributes["service"]) {
						service = *searchService;
						break;
					}                    
				}
				//assert(service != NULL);
				if (service == NULL) {
					trace(TRACE_ERROR, "[ERROR] Task "+child->name+" references unknown global service "+child->attributes["service"]+". Skipping process in the translation.\n");
					child->filtered = true;
				} else {
					child->instantiateInterface(service);
				}
			}
	
			// Now that all needed information are gathered, test
		    // whether this child has overlapping input- and/or outputsets
		    child->filter();
	
			// Copy all of the referenced tasks values to the callToTask- Block
		    if(child->filtered == true) {
		        filtered = true;
			}
	    }
    } // end if(complex)
}


//! \brief tests every connection of this process to determine whether it is valid.
//!        A connection is valid if its two pins can be found
void Block::linkNodes()
{
	// TODO re-factor this method, it is too long!
	
    // only link nodes if this is a process
	if (complex)
    {
	// iterate over all connections
	for (set<BlockConnection*>::iterator con = connections.begin(); con != connections.end(); con++)
	{
		// assume that the source as well as the target pins are invalid
		bool sourceValid = false;
		bool targetValid = false;
		
		// If the sourcestring is empty this is either an input connection for
		// the process, or invalid
		if ((*con)->source == "")
		{
			// This connection has no contant pin and thus is invalid
			if((*con)->sourceContact == "")
			{
				cerr << "Ungültige Connection. Kein Ziel spezifiziert !" << endl;
			} 
			else
			{
				// Search the inputs of the process whether they match the sourceContact 
				for (set<string>::iterator input = inputs.begin(); input != inputs.end(); input++)
				{
					// if a matching input could be found, this source is valid
					if ((*input) == (*con)->sourceContact)
					{
						sourceValid = true;
						(*con)->src = this;
						break;
					}
				}                    
			}
		}
		else
		// if the source string is not empty
		{
			if((*con)->sourceContact == "")
			{
				// In case that the source is not an empty string but the
				// sourceContact is, this source is either invalid or atomic,
				// thus being a pin in itself
				for (set<Block*>::iterator child = children.begin(); child != children.end(); child++)
				{
					// if the source is atomic, this connection is valid
					if ( (*child)->atomic && (*child)->name == (*con)->source)
					{
						sourceValid = true;
						(*con)->src = (*child);
						break;
					}
				}
			} 
			else
			// if a source contact is given as well
			{
				// search all other blocks
				for (set<Block*>::iterator child = children.begin(); child != children.end(); child++)
				{
					// if they are not atomic and the sourcename matches the blocks name
					if ( !(*child)->atomic && (*con)->source == (*child)->name)
					{
						// iterate over all outputs to find the pin matching source contact
						for (set<string>::iterator output = (*child)->outputs.begin(); output != (*child)->outputs.end(); output++)
						{
							// if a matching output could be found, this source is valid
							if ( (*output) == (*con)->sourceContact)
							{
                                if ((*child)->wrongMultiplicity[(*output)])
                                {
                                    //std::cerr << "Process \"" << attributes["name"] <<"\" filtered out. Violated the rule of not having optional connections!\n\n";
                                    filtered = true;
                                    //return; // do not cancel translation
                                }
								sourceValid = true;
								(*con)->src = (*child);
								break;
							}
						}

						// iterate over all additional outputs to find the pin matching source contact
						for (set<string>::iterator output = (*child)->additionalOutputs.begin(); output != (*child)->additionalOutputs.end(); output++)
						{
							// if a matching output could be found, this source is valid
							if ( (*output) == (*con)->sourceContact)
							{
                                if ((*child)->wrongMultiplicity[(*output)])
                                {
                                    //std::cerr << "Process \"" << attributes["name"] <<"\" filtered out. Violated the rule of not having optional connections!\n\n";
                                    filtered = true;
                                    //return; // do not cancel translation
                                }
								sourceValid = true;
								(*con)->sourceAdditional = true;
								(*con)->src = (*child);
								break;
							}
						}
						break;
					}
				}
			}
		}
		
		// Now do the same tests for the target of this connection

		// If the targetstring is empty this is either an output connection for
		// the process, or invalid
		if ((*con)->target == "")
		{
			// This connection has no contant pin and thus is invalid
			if((*con)->targetContact == "")
			{
				cerr << "Ungültige Connection. Kein Ziel spezifiziert !" << endl;
			} 
			else
			{
				// Search the outputs of the process whether they match the targetContact 
				for (set<string>::iterator output = outputs.begin(); output != outputs.end(); output++)
				{
					// if a matching output could be found, this target is valid
					if ((*output) == (*con)->targetContact)
					{
						targetValid = true;
						(*con)->tgt = this;
						break;
					}
				}                    
			}
		}
		else
		// if the source string is not empty
		{
			if((*con)->targetContact == "")
			{
				// In case that the target is not an empty string but the
				// targetContact is, this target is either invalid or atomic,
				// thus being a pin in itself
				for (set<Block*>::iterator child = children.begin(); child != children.end(); child++)
				{
					// if the target is atomic, this connection is valid
					if ( (*child)->atomic && (*child)->name == (*con)->target)
					{
						targetValid = true;
						(*con)->tgt = (*child);
						break;                        
					}
				}
			} 
			else
			// if a source contact is given as well
			{
				// search all other blocks
				for (set<Block*>::iterator child = children.begin(); child != children.end(); child++)
				{
					// if they are not atomic and the targetname matches the blocks name
					if ( !(*child)->atomic && (*con)->target == (*child)->name)
					{
						// iterate over all outputs to find the pin matching target contact
						for (set<string>::iterator input = (*child)->inputs.begin(); input != (*child)->inputs.end(); input++)
						{
							// if a matching input could be found, this target is valid
							if ( (*input) == (*con)->targetContact)
							{
                                if ((*child)->wrongMultiplicity[(*input)])
                                {
                                    //std::cerr << "Process \"" << attributes["name"] <<"\" filtered out. Violated the rule of not having optional connections!\n\n";
                                    filtered = true;
                                    //return; // do not cancel translation
                                }
								targetValid = true;
								(*con)->tgt = (*child);
								break;
							}
						}

						// iterate over all additional outputs to find the pin matching source contact
						for (set<string>::iterator input = (*child)->additionalInputs.begin(); input != (*child)->additionalInputs.end(); input++)
						{
							// if a matching input could be found, this target is valid
							if ( (*input) == (*con)->targetContact)
							{
                            	if ((*child)->wrongMultiplicity[(*input)])
                                {
                                    //std::cerr << "Process \"" << attributes["name"] <<"\" filtered out. Violated the rule of not having optional connections!\n\n";
                                    filtered = true;
                                    //return; // do not cancel translation
                                }
								targetValid = true;
								(*con)->targetAdditional = true;
								(*con)->tgt = (*child);
								break;
							}
						}
						break;
					}
				}
			}
		}      

		// if the target and the source both are valid, this connection is valid
		if (targetValid && sourceValid)
		{
			(*con)->valid = true;
		}
		else
		{
            cerr<< "Connection konnte nicht gueltig gemacht werden." << endl;
            cerr<< "Process:\"" << name << "\"" << endl;                
            cerr<< "Connectionname:\"" << (*con)->name << "\"" << endl;
// <Dirk.F start> extended debug info
            if (!sourceValid)
            	cerr << "[invalid source]" << endl;
// <Dirk.F end>
            cerr<< "SourceContact:\"" << (*con)->sourceContact << "\"" << endl;                
            cerr<< "SourceNode:\"" << (*con)->source << "\"" << endl;
            
// <Dirk.F start> extended debug info
            if (!targetValid)
            	cerr << "[invalid target]" << endl;
// <Dirk.F end>
            cerr<< "TargetContact:\"" << (*con)->targetContact << "\"" << endl;                
            cerr<< "TargetNode:\"" << (*con)->target << "\"" << endl;                       
		}
	}
	
    } // end if(complex)
}

/*
//! \brief translates this block and all of its children into a petrinet
//!        by following the pattern for each blocktype and then translating
//!        all connections in the end
//! \param PN petrinet to be filled with transitions, places and arcs
//! \return returns zero in case that this block is not a process, thus cannot
//!         be translated into a petrinet
PetriNet* Block::returnSimpleNet(PetriNet* PN)
{
	// TODO re-factor this method, it is too long!
	
	// Only do this, if this block is a process
    if(type == PROCESS) 
    {
    
    //Create pointer to the current place and the current transition
	Place* p;
	Transition* t;
	
	// DECOMPOSITION: Important variable for decomposition
	PN->realStartNode = NULL;

	// create all input places of the process
	for (set<string>::iterator input = inputs.begin(); input != inputs.end(); input++)
	{
		PN->newPlace(("process."+ name + ".input." +(*input)), IN);
	}

	// create all output places of the process
	for (set<string>::iterator output = outputs.begin(); output != outputs.end(); output++)
	{
		PN->newPlace(("process."+ name + ".output." +(*output)), OUT);
	}
	
	// iterate over all children of the process, translating every child
	// by its types pattern
	for (set<Block*>::iterator child = children.begin(); child != children.end(); child++)
	{

		// DECOMPOSITION: if not decomposed, all children are enabled
		if(!(*child)->enabled)
		{
			continue;
		}

		// if the child is a startnode, use the startnode pattern
		if ((*child)->type == STARTNODE)
		{
			p = PN->newPlace((name + ".startNode." + (*child)->name), INTERNAL);
			p->mark();
			(*child)->centralNode = p;
			PN->realStartNode=p;
		}

		// if the child is a endnode, use the endnode pattern
		if ((*child)->type == ENDNODE)
		{
			p = PN->newPlace((name + ".endNode." + (*child)->name), INTERNAL);
// <Dirk.F start>, corrected end node semantics
//			p->isFinal = true;
            // a token on an end-node is consumed
            t = PN->newTransition((name + ".t_endNode." + (*child)->name));
            PN->newArc(p, t);
// <Dirk.F end>             
		}

		// if the child is a stopnode, use the stopnode pattern
		if ((*child)->type == STOPNODE)
		{
			p = PN->newPlace((name + ".stopNode." + (*child)->name), INTERNAL);
// <Dirk.F start>, corrected stop node semantics
//			p->isFinal = true;
            // a token on a stop-node describes that the process has
            // terminated - we need to detect this state, but it must not
            // be a deadlock, make it a life-lock instead
            t = PN->newTransition((name + ".t_stopNode." + (*child)->name));
            PN->newArc(p, t);  // add a loop
            PN->newArc(t, p);
// <Dirk.F end>             
		}

		// now come a lot of blocktypes that are translated equally in their
		// their patterns but need to be named correctly to their type
		
		// if the child is a callToProcess, use the task pattern
		if ((*child)->type == CALLTOPROCESS)
		{

			t = PN->newTransition(("callToProcess." + (*child)->name + ".fire"));
			for (set<string>::iterator input = (*child)->inputs.begin(); input != (*child)->inputs.end(); input++)
			{
				p = PN->newPlace(("callToProcess." + (*child)->name + ".input." + (*input)), INTERNAL);
				PN->newArc(p,t);
			}

			for (set<string>::iterator output = (*child)->outputs.begin(); output != (*child)->outputs.end(); output++)
			{
				p = PN->newPlace(("callToProcess." + (*child)->name + ".output." + (*output)), INTERNAL);
				PN->newArc(t,p);
			}
			for (set<string>::iterator additionalInput = (*child)->additionalInputs.begin(); additionalInput != (*child)->additionalInputs.end(); additionalInput++)
			{
				p = PN->newPlace(("callToProcess." + (*child)->name + ".additionalInput." + (*additionalInput)), INTERNAL);
				PN->newArc(p,t);
			}
			for (set<string>::iterator additionalOutput = (*child)->additionalOutputs.begin(); additionalOutput != (*child)->additionalOutputs.end(); additionalOutput++)
			{
				p = PN->newPlace(("callToProcess." + (*child)->name + ".additionalOutput." + (*additionalOutput)), INTERNAL);
				PN->newArc(t,p);
			}
		}

		// if the child is a callToTask, use the task pattern
		if ((*child)->type == CALLTOTASK)
		{

			t = PN->newTransition(("callToTask." + (*child)->name + ".fire"));
			for (set<string>::iterator input = (*child)->inputs.begin(); input != (*child)->inputs.end(); input++)
			{
				p = PN->newPlace(("callToTask." + (*child)->name + ".input." + (*input)), INTERNAL);
				PN->newArc(p,t);
			}

			for (set<string>::iterator output = (*child)->outputs.begin(); output != (*child)->outputs.end(); output++)
			{
				p = PN->newPlace(("callToTask." + (*child)->name + ".output." + (*output)), INTERNAL);
				PN->newArc(t,p);
			}
			for (set<string>::iterator additionalInput = (*child)->additionalInputs.begin(); additionalInput != (*child)->additionalInputs.end(); additionalInput++)
			{
				p = PN->newPlace(("callToTask." + (*child)->name + ".additionalInput." + (*additionalInput)), INTERNAL);
				PN->newArc(p,t);
			}
			for (set<string>::iterator additionalOutput = (*child)->additionalOutputs.begin(); additionalOutput != (*child)->additionalOutputs.end(); additionalOutput++)
			{
				p = PN->newPlace(("callToTask." + (*child)->name + ".additionalOutput." + (*additionalOutput)), INTERNAL);
				PN->newArc(t,p);
			}
		}

		// if the child is a callToService, use the task pattern
		if ((*child)->type == CALLTOSERVICE)
		{

			t = PN->newTransition(("callToService." + (*child)->name + ".fire"));
			for (set<string>::iterator input = (*child)->inputs.begin(); input != (*child)->inputs.end(); input++)
			{
				p = PN->newPlace(("callToService." + (*child)->name + ".input." + (*input)), INTERNAL);
				PN->newArc(p,t);
			}

			for (set<string>::iterator output = (*child)->outputs.begin(); output != (*child)->outputs.end(); output++)
			{
				p = PN->newPlace(("callToService." + (*child)->name + ".output." + (*output)), INTERNAL);
				PN->newArc(t,p);
			}
			for (set<string>::iterator additionalInput = (*child)->additionalInputs.begin(); additionalInput != (*child)->additionalInputs.end(); additionalInput++)
			{
				p = PN->newPlace(("callToService." + (*child)->name + ".additionalInput." + (*additionalInput)), INTERNAL);
				PN->newArc(p,t);
			}
			for (set<string>::iterator additionalOutput = (*child)->additionalOutputs.begin(); additionalOutput != (*child)->additionalOutputs.end(); additionalOutput++)
			{
				p = PN->newPlace(("callToService." + (*child)->name + ".additionalOutput." + (*additionalOutput)), INTERNAL);
				PN->newArc(t,p);
			}
		}
		
		// if the child is an observer, use the task pattern
		if ((*child)->type == OBSERVER)
		{

			t = PN->newTransition(("observer." + (*child)->name + ".fire"));
			for (set<string>::iterator input = (*child)->inputs.begin(); input != (*child)->inputs.end(); input++)
			{
				p = PN->newPlace(("observer." + (*child)->name + ".input." + (*input)), INTERNAL);
				PN->newArc(p,t);
			}

			for (set<string>::iterator output = (*child)->outputs.begin(); output != (*child)->outputs.end(); output++)
			{
				p = PN->newPlace(("observer." + (*child)->name + ".output." + (*output)), INTERNAL);
				PN->newArc(t,p);
			}
		}

		// if the child is map, use the task pattern
		if ((*child)->type == MAP)
		{

			t = PN->newTransition(("map." + (*child)->name + ".fire"));
			for (set<string>::iterator input = (*child)->inputs.begin(); input != (*child)->inputs.end(); input++)
			{
				p = PN->newPlace(("map." + (*child)->name + ".input." + (*input)), INTERNAL);
				PN->newArc(p,t);
			}

			for (set<string>::iterator output = (*child)->outputs.begin(); output != (*child)->outputs.end(); output++)
			{
				p = PN->newPlace(("map." + (*child)->name + ".output." + (*output)), INTERNAL);
				PN->newArc(t,p);
			}
		}

		// if the child is an timer, use the task pattern
		if ((*child)->type == TIMER)
		{

			t = PN->newTransition(("timer." + (*child)->name + ".fire"));
			for (set<string>::iterator input = (*child)->inputs.begin(); input != (*child)->inputs.end(); input++)
			{
				p = PN->newPlace(("timer." + (*child)->name + ".input." + (*input)), INTERNAL);
				PN->newArc(p,t);
			}

			for (set<string>::iterator output = (*child)->outputs.begin(); output != (*child)->outputs.end(); output++)
			{
				p = PN->newPlace(("timer." + (*child)->name + ".output." + (*output)), INTERNAL);
				PN->newArc(t,p);
			}
		}

		// if the child is an notificationReceiver, use the task pattern
		if ((*child)->type == NOTIFICATIONRECEIVER)
		{

			t = PN->newTransition(("notificationReceiver." + (*child)->name + ".fire"));
			for (set<string>::iterator input = (*child)->inputs.begin(); input != (*child)->inputs.end(); input++)
			{
				p = PN->newPlace(("notificationReceiver." + (*child)->name + ".input." + (*input)), INTERNAL);
				PN->newArc(p,t);
			}

			for (set<string>::iterator output = (*child)->outputs.begin(); output != (*child)->outputs.end(); output++)
			{
				p = PN->newPlace(("notificationReceiver." + (*child)->name + ".output." + (*output)), INTERNAL);
				PN->newArc(t,p);
			}
		}

		// if the child is an notificationBroadcaster, use the task pattern
		if ((*child)->type == NOTIFICATIONBROADCASTER)
		{

			t = PN->newTransition(("notificationBroadcaster." + (*child)->name + ".fire"));
			for (set<string>::iterator input = (*child)->inputs.begin(); input != (*child)->inputs.end(); input++)
			{
				p = PN->newPlace(("notificationBroadcaster." + (*child)->name + ".input." + (*input)), INTERNAL);
				PN->newArc(p,t);
			}

			for (set<string>::iterator output = (*child)->outputs.begin(); output != (*child)->outputs.end(); output++)
			{
				p = PN->newPlace(("notificationBroadcaster." + (*child)->name + ".output." + (*output)), INTERNAL);
				PN->newArc(t,p);
			}
		}

		// if the child is a task, use the task pattern
		if ((*child)->type == TASK)
		{
			t = PN->newTransition(("task." + (*child)->name + ".fire"));
			for (set<string>::iterator input = (*child)->inputs.begin(); input != (*child)->inputs.end(); input++)
			{
				p = PN->newPlace(("task." + (*child)->name + ".input." + (*input)), INTERNAL);
				PN->newArc(p,t);
			}

			for (set<string>::iterator output = (*child)->outputs.begin(); output != (*child)->outputs.end(); output++)
			{
				p = PN->newPlace(("task." + (*child)->name + ".output." + (*output)), INTERNAL);
				PN->newArc(t,p);
			}
		}

		// Now come the controlflow types, these are the most interesting
		
		// if the child is a decision, use the decision pattern
		if ((*child)->type == DECISION)
		{
			map<string, bool> generated;
			Place* central;
			central = PN->newPlace(("decision." + (*child)->name + ".activated"), INTERNAL);
			for (set<string>::iterator inputBranch = (*child)->inputBranches.begin(); inputBranch != (*child)->inputBranches.end(); inputBranch++)
			{                   
				t = PN->newTransition(("decision." + (*child)->name + ".activate." + (*inputBranch)));
				for (set<string>::iterator input = (*child)->inputBranchSet[(*inputBranch)].begin(); input != (*child)->inputBranchSet[(*inputBranch)].end(); input++)
				{
					if (!generated[(*input)])
					{
						p = PN->newPlace(("decision." + (*child)->name + ".input." + (*input)), INTERNAL);
						generated[(*input)] = true;
					}
					else
					{
						p =PN->findPlace(("decision." + (*child)->name + ".input." + (*input)));
					}
					PN->newArc(p,t);                        
				}
				PN->newArc(t,central);
			}
			for (set<string>::iterator outputBranch = (*child)->outputBranches.begin(); outputBranch != (*child)->outputBranches.end(); outputBranch++)
			{                   
				t = PN->newTransition(("decision." + (*child)->name + ".fire." + (*outputBranch)));
				for (set<string>::iterator output = (*child)->outputBranchSet[(*outputBranch)].begin(); output != (*child)->outputBranchSet[(*outputBranch)].end(); output++)
				{
					if (!generated[(*output)])
					{
						p = PN->newPlace(("decision." + (*child)->name + ".output." + (*output)), INTERNAL);
						generated[(*output)] = true;
					}
					else
					{
						p =PN->findPlace(("decision." + (*child)->name + ".output." + (*output)));
					}
					PN->newArc(t,p);                        
				}
				PN->newArc(central, t);
			}
		}

		// if the child is a fork, use the fork pattern
		if ((*child)->type == FORK)
		{
			map<string, bool> generated;
			set<Place*> centrals;
			for (set<string>::iterator outputBranch = (*child)->outputBranches.begin(); outputBranch != (*child)->outputBranches.end(); outputBranch++)
			{
				Place* central = PN->newPlace(("fork." + (*child)->name + ".activated." + (*outputBranch)), INTERNAL);
				centrals.insert(central);                
				t = PN->newTransition(("fork." + (*child)->name + ".fire." + (*outputBranch)));
				for (set<string>::iterator output = (*child)->outputBranchSet[(*outputBranch)].begin(); output != (*child)->outputBranchSet[(*outputBranch)].end(); output++)
				{
					if (!generated[(*output)])
					{
						p = PN->newPlace(("fork." + (*child)->name + ".output." + (*output)), INTERNAL);
						generated[(*output)] = true;
					}
					else
					{
						p =PN->findPlace(("fork." + (*child)->name + ".output." + (*output)));
					}
					PN->newArc(t,p);                        
				}
				PN->newArc(central, t);
			}
			for (set<string>::iterator inputBranch = (*child)->inputBranches.begin(); inputBranch != (*child)->inputBranches.end(); inputBranch++)
			{                   
				t = PN->newTransition(("fork." + (*child)->name + ".activate." + (*inputBranch)));
				for (set<string>::iterator input = (*child)->inputBranchSet[(*inputBranch)].begin(); input != (*child)->inputBranchSet[(*inputBranch)].end(); input++)
				{
					if (!generated[(*input)])
					{
						p = PN->newPlace(("fork." + (*child)->name + ".input." + (*input)), INTERNAL);
						generated[(*input)] = true;
					}
					else
					{
						p =PN->findPlace(("fork." + (*child)->name + ".input." + (*input)));
					}
					PN->newArc(p,t);                        
				}

				for (set<Place*>::iterator central = centrals.begin(); central != centrals.end(); central++)
				{
					PN->newArc(t,(*central));
				}
			}
		}

		// if the child is a merge, use the merge pattern
		if ((*child)->type == MERGE)
		{
			map<string, bool> generated;
			Place* central;
			central = PN->newPlace(("merge." + (*child)->name + ".activated"), INTERNAL);
			for (set<string>::iterator inputBranch = (*child)->inputBranches.begin(); inputBranch != (*child)->inputBranches.end(); inputBranch++)
			{                   
				t = PN->newTransition(("merge." + (*child)->name + ".activate." + (*inputBranch)));
				for (set<string>::iterator input = (*child)->inputBranchSet[(*inputBranch)].begin(); input != (*child)->inputBranchSet[(*inputBranch)].end(); input++)
				{
					if (!generated[(*input)])
					{
						p = PN->newPlace(("merge." + (*child)->name + ".input." + (*input)), INTERNAL);
						generated[(*input)] = true;
					}
					else
					{
						p =PN->findPlace(("merge." + (*child)->name + ".input." + (*input)));
					}
					PN->newArc(p,t);                        
				}
				PN->newArc(t,central);
			}
			for (set<string>::iterator outputBranch = (*child)->outputBranches.begin(); outputBranch != (*child)->outputBranches.end(); outputBranch++)
			{                   
				t = PN->newTransition(("merge." + (*child)->name + ".fire." + (*outputBranch)));
				for (set<string>::iterator output = (*child)->outputBranchSet[(*outputBranch)].begin(); output != (*child)->outputBranchSet[(*outputBranch)].end(); output++)
				{
					if (!generated[(*output)])
					{
						p = PN->newPlace(("merge." + (*child)->name + ".output." + (*output)), INTERNAL);
						generated[(*output)] = true;
					}
					else
					{
						p =PN->findPlace(("merge." + (*child)->name + ".output." + (*output)));
					}
					PN->newArc(t,p);                        
				}
				PN->newArc(central, t);
			}
		}

		// if the child is a join, use the join pattern
		if ((*child)->type == JOIN)
		{
			map<string, bool> generated;
			set<Place*> centrals;
			for (set<string>::iterator inputBranch = (*child)->inputBranches.begin(); inputBranch != (*child)->inputBranches.end(); inputBranch++)
			{                   
				t = PN->newTransition(("join." + (*child)->name + ".activate." + (*inputBranch)));
				for (set<string>::iterator input = (*child)->inputBranchSet[(*inputBranch)].begin(); input != (*child)->inputBranchSet[(*inputBranch)].end(); input++)
				{
					if (!generated[(*input)])
					{
						p = PN->newPlace(("join." + (*child)->name + ".input." + (*input)), INTERNAL);
						generated[(*input)] = true;
					}
					else
					{
						p =PN->findPlace(("join." + (*child)->name + ".input." + (*input)));
					}
					PN->newArc(p,t);                        
				}

				Place* central = PN->newPlace(("join." + (*child)->name + ".activated.", (*inputBranch)), INTERNAL);
				centrals.insert(central);                
				PN->newArc(t,(central));
			}
			for (set<string>::iterator outputBranch = (*child)->outputBranches.begin(); outputBranch != (*child)->outputBranches.end(); outputBranch++)
			{
				t = PN->newTransition(("join." + (*child)->name + ".fire." + (*outputBranch)));
				for (set<string>::iterator output = (*child)->outputBranchSet[(*outputBranch)].begin(); output != (*child)->outputBranchSet[(*outputBranch)].end(); output++)
				{
					if (!generated[(*output)])
					{
						p = PN->newPlace(("join." + (*child)->name + ".output." + (*output)), INTERNAL);
						generated[(*output)] = true;
					}
					else
					{
						p =PN->findPlace(("join." + (*child)->name + ".output." + (*output)));
					}
					PN->newArc(t,p);                        
				}
				for (set<Place*>::iterator central = centrals.begin(); central != centrals.end(); central++)
				{
					PN->newArc((*central), t);
				}
			}
		}
	}
	
	// now that all children of the process have been translated, they need to be
	// connected
	translateConnections(PN);
	
	// returns the ready petrinet.
	return PN;    
    
    } // end if (this.type == PROCESS)
    return NULL;
}

*/

/*!
 * \brief	returns a string that can be used as a prefix for node-names
 * 			at the global level, e.g. inputCriteria, startNodes, connections
 */
const string Block::globalPrefixString() const {
	//return "process."+ name + ".";
	return "process.";
	//return "";
}

//! \brief translates this block and all of its children into a petrinet
//!        by following the pattern for each blocktype and then translating
//!        all connections in the end.
//! \param PN petrinet to be filled with transitions, places and arcs
//! \return returns zero in case that this block is not a process, thus cannot
//!         be translated into a petrinet
ExtendedWorkflowNet* Block::returnNet(ExtendedWorkflowNet* PN, BomProcess *bom)
{
    
    // Only do this, if this block is a process
    if(type == PROCESS) 
    {
    
    //Create pointer to the current place and the current transition
    Place* p;
    Transition* t;
    
    bom->bom_nodes.insert(this);	// is a BOM-node of the process

    set<Transition*> startingTransitions;
    map<string, Node*> processCentralNodes;
    
    // prefix for node names at the global level, e.g. inputCriteria etc.
    string globalPrefix = globalPrefixString();

    // create all input places of the process
    for (set<string>::iterator inputCriterion = inputCriteria.begin(); inputCriterion != inputCriteria.end(); inputCriterion++)
    {
        t = PN->newTransition(globalPrefix + "inputCriterion." +(*inputCriterion) + ".fire");
        startingTransitions.insert(t);
        bom->process_inputPinSets.insert(t);	// insert input pinset
        
        p = PN->newPlace((globalPrefix + "inputCriterion." +(*inputCriterion) + ".used"), INTERNAL);
        processCentralNodes[*inputCriterion] = p;
        PN->newArc(t,p);
        for (set<string>::iterator inputOfThisCriterion = inputCriterionSet[*inputCriterion].begin(); inputOfThisCriterion != inputCriterionSet[*inputCriterion].end(); inputOfThisCriterion++)
        {
        	string placeName = globalPrefix + "input." +(*inputOfThisCriterion);
        	p = PN->findPlace(placeName);		// see if input place was already created
        	// TODO: solution of input/output pins of blocks avoid this problem
        	if (p == NULL) p = PN->newPlace(placeName, INTERNAL);
            PN->newArc(t,p);			// add arc from the inputCriterion-transition
            bom->pinPlaces.insert(p);	// process-side place is a pin to the process
            
            placeName = globalPrefix + "trueInput." +(*inputOfThisCriterion);
            p = PN->findPlace(placeName);		// see if input place was already created
        	if (p == NULL) p = PN->newPlace(placeName, IN);
            PN->newArc(p,t);			// add arc to the inputCriterion-transition
        }
    }
    
    // create all output places of the process
    for (set<string>::iterator outputCriterion = outputCriteria.begin(); outputCriterion != outputCriteria.end(); outputCriterion++)
    {
        t = PN->newTransition(globalPrefix + "outputCriterion." +(*outputCriterion) + ".fire");
        bom->process_outputPinSets.insert(t);	// insert output pinset
        
        /*
        if (criterionMap[*outputCriterion].begin() == criterionMap[*outputCriterion].begin())
        {
            PN->newArc(processCentralNodes[(*(inputCriteria.begin()))],t);
        }
        else*/
        {
            for (set<string>::iterator relatedInputCriterion = criterionMap[*outputCriterion].begin(); relatedInputCriterion != criterionMap[*outputCriterion].end(); relatedInputCriterion++)
            {
                PN->newArc(processCentralNodes[*relatedInputCriterion],t);
            }
        }
        for (set<string>::iterator outputOfThisCriterion = outputCriterionSet[*outputCriterion].begin(); outputOfThisCriterion != outputCriterionSet[*outputCriterion].end(); outputOfThisCriterion++)
        {
        	string placeName = globalPrefix + "output." +(*outputOfThisCriterion);
        	p = PN->findPlace(placeName);  	// see if output place was already created
        	if (p == NULL) p = PN->newPlace(placeName, INTERNAL);
            PN->newArc(p,t);
            bom->pinPlaces.insert(p);		// process-side place is a pin to the process
            
            placeName = globalPrefix + "trueOutput." +(*outputOfThisCriterion);
            p = PN->findPlace(placeName);  	// see if output place was already created
            if (p == NULL) p = PN->newPlace(placeName, OUT);
            PN->newArc(t,p);
        }
    }
    
    // iterate over all children of the process, translating every child
    // by its types pattern
    for (set<Block*>::iterator child = children.begin(); child != children.end(); child++)
    {

        // DECOMPOSITION: if not decomposed, all children are enabled
        if(!(*child)->enabled)
        {
            continue;
        }

        // if the child is a startnode, use the startnode pattern
        if ((*child)->type == STARTNODE)
        {
            p = PN->newPlace((globalPrefix + "startNode." + (*child)->name), INTERNAL);
            bom->process_internalPlaces.insert(p);	// is an internal place
            for (set<Transition*>::iterator startingTransition = startingTransitions.begin(); startingTransition != startingTransitions.end(); startingTransition++)
            {
                PN->newArc((*startingTransition),p);
            }
        }

        // if the child is a endnode, use the endnode pattern
        if ((*child)->type == ENDNODE)
        {
            p = PN->newPlace((globalPrefix + "endNode." + (*child)->name), INTERNAL);
            bom->process_endNodes.insert(p);		// is an endNode
            t = PN->newTransition((globalPrefix + "endNode." + (*child)->name + ".eat"));
            PN->newArc(p,t);
        }

        // if the child is a stopnode, use the stopnode pattern
        if ((*child)->type == STOPNODE)
        {
            p = PN->newPlace((globalPrefix + "stopNode." + (*child)->name), INTERNAL);
            bom->process_stopNodes.insert(p);		// is a stopNode
// <Dirk.F start> distinguish net and verification property
// TODO stop nodes are final if we check the net as an oWFN --> develop corresponding function 
// DONE stop nodes are looping if the net is used for soundness checking

            t = PN->newTransition((globalPrefix + "stopNode." + (*child)->name + ".eat"));
//            t = PN->newTransition((globalPrefix + "stopNode." + (*child)->name + ".loop"));
//            PN->newArc(t,p);
            PN->newArc(p,t);
//            p->isFinal = true;

// <Dirk.F end>
        }

        // now come a lot of blocktypes that are translated equally in their
        // their patterns but need to be named correctly to their type
        
        // if the child is a callToProcess, use the task pattern
        if ((*child)->type == CALLTOPROCESS ||
            (*child)->type == CALLTOTASK ||
            (*child)->type == CALLTOSERVICE ||
            (*child)->type == OBSERVER ||
            (*child)->type == MAP ||
            (*child)->type == TIMER ||
            (*child)->type == NOTIFICATIONRECEIVER ||
            (*child)->type == NOTIFICATIONBROADCASTER ||
            (*child)->type == TASK)
        {
            map<string, Node*> inputMap;
            map<string, Node*> outputMap;
            map<string, Node*> centralMap;
        	
            for (set<string>::iterator input = (*child)->inputs.begin(); input != (*child)->inputs.end(); input++)
            {
                p = PN->newPlace(((*child)->returnType() + "." + (*child)->name + ".input." + (*input)), INTERNAL);
                inputMap[*input] = p;
                bom->pinPlaces.insert(p);				// this place represents a pin
                bom->process_internalPlaces.insert(p);	// is an internal place
            }

            for (set<string>::iterator output = (*child)->outputs.begin(); output != (*child)->outputs.end(); output++)
            {
                p = PN->newPlace(((*child)->returnType() + "." + (*child)->name + ".output." + (*output)), INTERNAL);
                outputMap[*output] = p;
                bom->pinPlaces.insert(p);				// this place represents a pin
                bom->process_internalPlaces.insert(p);	// is an internal place
            }
            

            Transition* tNodeFire = PN->newTransition(((*child)->returnType() + "." + (*child)->name + ".fire"));
            
            int num_inputCriteria = (*child)->inputCriteria.size();
            int num_outputCriteria = (*child)->outputCriteria.size();
            
            if (num_inputCriteria > 1) {
                Place* pNodeActive = PN->newPlace(((*child)->returnType() + "." + (*child)->name + ".activated"), INTERNAL);
                PN->newArc(pNodeActive, tNodeFire);	// node can fire only if it is activated
                bom->process_internalPlaces.insert(pNodeActive);	// internal place
                
	            for (set<string>::iterator inputCriterion = (*child)->inputCriteria.begin(); inputCriterion != (*child)->inputCriteria.end(); inputCriterion++)
	            {
	                Transition* tCriterionFire = PN->newTransition(((*child)->returnType() + "." + (*child)->name + ".inputCriterion." + (*inputCriterion) + ".fire"));
	            	// create a complex mapping between input and output criteria only if
	            	// there are multiple criteria
	            	if (num_outputCriteria > 1) {
	            		p = PN->newPlace(((*child)->returnType() + "." + (*child)->name + ".inputCriterion." + (*inputCriterion) + ".used"), INTERNAL);
	            		centralMap[*inputCriterion] = p;
	            		PN->newArc(tCriterionFire,p);			// input criterion remembers it was chosen
	            		bom->process_internalPlaces.insert(p);	// is an internal place
	            	}
	            	PN->newArc(tCriterionFire,pNodeActive);	// input criterion activates this node

	            	// criterion gets activated by its input pins only
	                for (set<string>::iterator inputOfThisCriterion = (*child)->inputCriterionSet[*inputCriterion].begin(); inputOfThisCriterion != (*child)->inputCriterionSet[*inputCriterion].end(); inputOfThisCriterion++)
	                {
	                    PN->newArc(inputMap[*inputOfThisCriterion],tCriterionFire);
	                }
	            }
	        // if there is only one input criterion, then merge it with the node.fire-transition:
	        // i.e. the node.fire-transition consumes directly from the input
            } else {
            	for (set<string>::iterator input = (*child)->inputs.begin(); input != (*child)->inputs.end(); input++) {
            		PN->newArc(inputMap[*input], tNodeFire);
            	}
            }
            
            if (num_outputCriteria > 1) {
                Place* pNodeCompleted = PN->newPlace(((*child)->returnType() + "." + (*child)->name + ".completed"), INTERNAL);
                PN->newArc(tNodeFire, pNodeCompleted);	// node completes only after it fired
                bom->process_internalPlaces.insert(pNodeCompleted);	// internal place
                
	            for (set<string>::iterator outputCriterion = (*child)->outputCriteria.begin(); outputCriterion != (*child)->outputCriteria.end(); outputCriterion++)
	            {
	            	Transition* tCriterionFire = PN->newTransition(((*child)->returnType() + "." + (*child)->name + ".outputCriterion." + (*outputCriterion) + ".fire"));
	            	// create a complex mapping between input and output criteria only if
	            	// there are multiple criteria
	            	if (num_inputCriteria > 1) {
		                for (set<string>::iterator relatedInputCriterion = (*child)->criterionMap[*outputCriterion].begin(); relatedInputCriterion != (*child)->criterionMap[*outputCriterion].end(); relatedInputCriterion++)
		                {
		                    //t = PN->newTransition((*child)->returnType() + "." + (*child)->name + "." + (*relatedInputCriterion) + "." + (*outputCriterion));
		                	// output criterion requires that its related input criterion was fired
		                    PN->newArc(centralMap[*relatedInputCriterion],tCriterionFire);
		                }
	            	}
	            	
	            	PN->newArc(pNodeCompleted, tCriterionFire);	// completion of task activates this criterion

	            	// criterion produces on its output pins only
                    for (set<string>::iterator outputOfThisCriterion = (*child)->outputCriterionSet[*outputCriterion].begin(); outputOfThisCriterion != (*child)->outputCriterionSet[*outputCriterion].end(); outputOfThisCriterion++)
                    {
                        PN->newArc(tCriterionFire, outputMap[*outputOfThisCriterion]);
                    }
	            }
            } else {
                for (set<string>::iterator output = (*child)->outputs.begin(); output != (*child)->outputs.end(); output++) {
                	PN->newArc(tNodeFire, outputMap[*output]);
                }
            }
        }

        // Now come the controlflow types, these are the most interesting
        
        // if the child is a decision, use the decision pattern
        if ((*child)->type == DECISION)
        {
            map<string, bool> generated;
            Place* central;
            central = PN->newPlace(("decision." + (*child)->name + ".activated"), INTERNAL);
            bom->process_internalPlaces.insert(central);	// is an internal place
            
            for (set<string>::iterator inputBranch = (*child)->inputBranches.begin(); inputBranch != (*child)->inputBranches.end(); inputBranch++)
            {                   
                t = PN->newTransition(("decision." + (*child)->name + ".activate." + (*inputBranch)));
                for (set<string>::iterator input = (*child)->inputBranchSet[(*inputBranch)].begin(); input != (*child)->inputBranchSet[(*inputBranch)].end(); input++)
                {
                    if (!generated[(*input)])
                    {
                        p = PN->newPlace(("decision." + (*child)->name + ".input." + (*input)), INTERNAL);
                        generated[(*input)] = true;
                        bom->pinPlaces.insert(p);				// this place represents a pin
                        bom->process_internalPlaces.insert(p);	// is an internal place
                    }
                    else
                    {
                        p = PN->findPlace(("decision." + (*child)->name + ".input." + (*input)));
                    }
                    PN->newArc(p,t);                        
                }
                PN->newArc(t,central);
            }
            
            if ((*child)->attributes["isInclusive"] == "true")
            {
                set<set<string> > powerSet;
            	inclusiveDecisionPatternRecursively((*child), central, (*child)->outputBranches, "0", PN, bom, generated, powerSet);
            }
            else
            {
            	for (set<string>::iterator outputBranch = (*child)->outputBranches.begin(); outputBranch != (*child)->outputBranches.end(); outputBranch++)
            	{                   
            		t = PN->newTransition(("decision." + (*child)->name + ".fire." + (*outputBranch)));
            		for (set<string>::iterator output = (*child)->outputBranchSet[(*outputBranch)].begin(); output != (*child)->outputBranchSet[(*outputBranch)].end(); output++)
            		{
            			if (!generated[(*output)])
            			{
            				p = PN->newPlace(("decision." + (*child)->name + ".output." + (*output)), INTERNAL);
            				generated[(*output)] = true;
                            bom->pinPlaces.insert(p);				// this place represents a pin
                            bom->process_internalPlaces.insert(p);	// is an internal place
            			}
            			else
            			{
            				p =PN->findPlace(("decision." + (*child)->name + ".output." + (*output)));
            			}
            			PN->newArc(t,p);                        
            		}
            		PN->newArc(central, t);
            	}
            }
        }

        // if the child is a fork, use the fork pattern
        if ((*child)->type == FORK)
        {
            map<string, bool> generated;
            set<Place*> centrals;
            for (set<string>::iterator outputBranch = (*child)->outputBranches.begin(); outputBranch != (*child)->outputBranches.end(); outputBranch++)
            {
                Place* central = PN->newPlace(("fork." + (*child)->name + ".activated." + (*outputBranch)), INTERNAL);
                centrals.insert(central);    
                bom->process_internalPlaces.insert(central);	// is an internal place
                
                t = PN->newTransition(("fork." + (*child)->name + ".fire." + (*outputBranch)));
                for (set<string>::iterator output = (*child)->outputBranchSet[(*outputBranch)].begin(); output != (*child)->outputBranchSet[(*outputBranch)].end(); output++)
                {
                    if (!generated[(*output)])
                    {
                        p = PN->newPlace(("fork." + (*child)->name + ".output." + (*output)), INTERNAL);
                        generated[(*output)] = true;
                        bom->pinPlaces.insert(p);				// this place represents a pin
                        bom->process_internalPlaces.insert(p);	// is an internal place
                    }
                    else
                    {
                        p =PN->findPlace(("fork." + (*child)->name + ".output." + (*output)));
                    }
                    PN->newArc(t,p);                        
                }
                PN->newArc(central, t);
            }
            for (set<string>::iterator inputBranch = (*child)->inputBranches.begin(); inputBranch != (*child)->inputBranches.end(); inputBranch++)
            {                   
                t = PN->newTransition(("fork." + (*child)->name + ".activate." + (*inputBranch)));
                for (set<string>::iterator input = (*child)->inputBranchSet[(*inputBranch)].begin(); input != (*child)->inputBranchSet[(*inputBranch)].end(); input++)
                {
                    if (!generated[(*input)])
                    {
                        p = PN->newPlace(("fork." + (*child)->name + ".input." + (*input)), INTERNAL);
                        generated[(*input)] = true;
                        bom->pinPlaces.insert(p);				// this place represents a pin
                        bom->process_internalPlaces.insert(p);	// is an internal place
                    }
                    else
                    {
                        p =PN->findPlace(("fork." + (*child)->name + ".input." + (*input)));
                    }
                    PN->newArc(p,t);                        
                }

                for (set<Place*>::iterator central = centrals.begin(); central != centrals.end(); central++)
                {
                    PN->newArc(t,(*central));
                }
            }
        }

        // if the child is a merge, use the merge pattern
        if ((*child)->type == MERGE)
        {
            map<string, bool> generated;
            Place* central;
            central = PN->newPlace(("merge." + (*child)->name + ".activated"), INTERNAL);
            bom->process_internalPlaces.insert(central);	// is an internal place
            
            for (set<string>::iterator inputBranch = (*child)->inputBranches.begin(); inputBranch != (*child)->inputBranches.end(); inputBranch++)
            {                   
                t = PN->newTransition(("merge." + (*child)->name + ".activate." + (*inputBranch)));
                for (set<string>::iterator input = (*child)->inputBranchSet[(*inputBranch)].begin(); input != (*child)->inputBranchSet[(*inputBranch)].end(); input++)
                {
                    if (!generated[(*input)])
                    {
                        p = PN->newPlace(("merge." + (*child)->name + ".input." + (*input)), INTERNAL);
                        generated[(*input)] = true;
                        bom->pinPlaces.insert(p);				// this place represents a pin
                        bom->process_internalPlaces.insert(p);	// is an internal place
                    }
                    else
                    {
                        p =PN->findPlace(("merge." + (*child)->name + ".input." + (*input)));
                    }
                    PN->newArc(p,t);                        
                }
                PN->newArc(t,central);
            }
            for (set<string>::iterator outputBranch = (*child)->outputBranches.begin(); outputBranch != (*child)->outputBranches.end(); outputBranch++)
            {                   
                t = PN->newTransition(("merge." + (*child)->name + ".fire." + (*outputBranch)));
                for (set<string>::iterator output = (*child)->outputBranchSet[(*outputBranch)].begin(); output != (*child)->outputBranchSet[(*outputBranch)].end(); output++)
                {
                    if (!generated[(*output)])
                    {
                        p = PN->newPlace(("merge." + (*child)->name + ".output." + (*output)), INTERNAL);
                        generated[(*output)] = true;
                        bom->pinPlaces.insert(p);				// this place represents a pin
                        bom->process_internalPlaces.insert(p);	// is an internal place
                    }
                    else
                    {
                        p = PN->findPlace(("merge." + (*child)->name + ".output." + (*output)));
                    }
                    PN->newArc(t,p);                        
                }
                PN->newArc(central, t);
            }
        }

        // if the child is a join, use the join pattern
        if ((*child)->type == JOIN)
        {
            map<string, bool> generated;
            set<Place*> centrals;
            for (set<string>::iterator inputBranch = (*child)->inputBranches.begin(); inputBranch != (*child)->inputBranches.end(); inputBranch++)
            {                   
                t = PN->newTransition(("join." + (*child)->name + ".activate." + (*inputBranch)));
                for (set<string>::iterator input = (*child)->inputBranchSet[(*inputBranch)].begin(); input != (*child)->inputBranchSet[(*inputBranch)].end(); input++)
                {
                    if (!generated[(*input)])
                    {
                        p = PN->newPlace(("join." + (*child)->name + ".input." + (*input)), INTERNAL);
                        generated[(*input)] = true;
                        bom->pinPlaces.insert(p);				// this place represents a pin
                        bom->process_internalPlaces.insert(p);	// is an internal place
                    }
                    else
                    {
                        p =PN->findPlace(("join." + (*child)->name + ".input." + (*input)));
                    }
                    PN->newArc(p,t);                        
                }

                Place* central = PN->newPlace(("join." + (*child)->name + ".activated." + (*inputBranch)), INTERNAL);
                centrals.insert(central);                
                PN->newArc(t,(central));
                bom->process_internalPlaces.insert(central);	// is an internal place
            }
            for (set<string>::iterator outputBranch = (*child)->outputBranches.begin(); outputBranch != (*child)->outputBranches.end(); outputBranch++)
            {
                t = PN->newTransition(("join." + (*child)->name + ".fire." + (*outputBranch)));
                for (set<string>::iterator output = (*child)->outputBranchSet[(*outputBranch)].begin(); output != (*child)->outputBranchSet[(*outputBranch)].end(); output++)
                {
                    if (!generated[(*output)])
                    {
                        p = PN->newPlace(("join." + (*child)->name + ".output." + (*output)), INTERNAL);
                        generated[(*output)] = true;
                        bom->pinPlaces.insert(p);				// this place represents a pin
                        bom->process_internalPlaces.insert(p);	// is an internal place
                    }
                    else
                    {
                        p =PN->findPlace(("join." + (*child)->name + ".output." + (*output)));
                    }
                    PN->newArc(t,p);                        
                }
                for (set<Place*>::iterator central = centrals.begin(); central != centrals.end(); central++)
                {
                    PN->newArc((*central), t);
                }
            }
        }
    }
    
    // now that all children of the process have been translated, they need to be
    // connected
    translateConnections(PN, bom);
    
    // returns the ready petrinet.
    return PN;    
    
    } // end if (this.type == PROCESS)
    return NULL;
}

void Block::inclusiveDecisionPatternRecursively(Block* decision, Place* centralNode, set<string>& currentOutputBranches, string number, PetriNet* PN, BomProcess* bom, map<string, bool>& generated, set<set<string> >& powerSet)
{
	if (!powerSet.insert(currentOutputBranches).second)
	{
	    return;
	}
	Transition* t;
	Place* p;
	t = PN->newTransition(("decision." + decision->name + ".fire." + number));
	PN->newArc(centralNode, t);
	for (set<string>::iterator outputBranch = currentOutputBranches.begin(); outputBranch != currentOutputBranches.end(); outputBranch++)
	{                   
		for (set<string>::iterator output = decision->outputBranchSet[(*outputBranch)].begin(); output != decision->outputBranchSet[(*outputBranch)].end(); output++)
		{
			if (!generated[(*output)])
			{
				p = PN->newPlace(("decision." + decision->name + ".output." + (*output)), INTERNAL);
				generated[(*output)] = true;
                bom->pinPlaces.insert(p);				// this place represents a pin
                bom->process_internalPlaces.insert(p);	// is an internal place
			}
			else
			{
				p = PN->findPlace(("decision." + decision->name + ".output." + (*output)));
			}
			PN->newArc(t,p);
		}
	}
	int i = 0;
	
	if (currentOutputBranches.size() == 1)
	{
		return;
	}
	
	for (set<string>::iterator outputBranch = currentOutputBranches.begin(); outputBranch != currentOutputBranches.end(); outputBranch++)
	{
		set<string> newSet = set<string>(currentOutputBranches);
		newSet.erase(*outputBranch);
		inclusiveDecisionPatternRecursively(decision, centralNode, newSet, (number+"."+toString(i)), PN, bom, generated, powerSet);
		i++;
	}
}


//! \brief this function is pretty simple as for its means, but very uncomfortable
//!        to read. The functionality is simple if there is no decomposition in it.
//!        just create a place and two transations for every connection and connect
//!        them, so they gather one token from the source pin and produce one token
//!        on the target pin.
//!        In the case of decomposition only roleinternal connections are translated
//!        like this. All rolecrossing connections become either inputs, or outputs.
//!        and all connections, that do not have their source or target in the
//!        current roleset are not enabled, thus, not translated at all
//! \param PN Petrinet to be extended by the connections
void Block::translateConnections(PetriNet* PN, BomProcess *bom)
{
    // prefix for node names at the global level, e.g. inputCriteria etc.
    string globalPrefix = globalPrefixString();

	// iterate over all connections
    for ( set<BlockConnection*>::iterator con = connections.begin(); con != connections.end(); con++)
    {
    	// DECOMPOSITION: If the connection is not enabled, dont do anything
    	if((*con)->enabled == false)
        {
            continue;
        }
        
        // create at least one of the transitions and the central place of the connection
    	Transition* t;
        Place* p;

        // if the connection is not rolecrossing, create the second transition to
        // for this is a simple roleinternal connection.
        if((*con)->roleCrossing == false)
        {
            t = PN->newTransition(globalPrefix + "connection." + (*con)->name);
            (*con)->firstTransition = t;
        }
        else
        // If the connection is rolecrossing, the central place of the connection
        // will become a part of the interface depending on, if this connection
        // is an input or an output connection. The transition will become the
        // corresponding input/output transition
        {
            if((*con)->input == false)
            {
                t = PN->newTransition(globalPrefix + "connection." + (*con)->name + ".output");
                p = PN->newPlace((globalPrefix + "connection." + (*con)->name), OUT);
                (*con)->firstArcs.second = PN->newArc(t,p);
            }
            else
            {
                t = PN->newTransition(globalPrefix + "connection." + (*con)->name + ".input");
                p = PN->newPlace((globalPrefix + "connection." + (*con)->name), IN);
                (*con)->firstArcs.first = PN->newArc(p,t);
            }
            (*con)->firstTransition = t;            
        }
        
        
        // From this point on the code becomes rather obfuscated, because we always
        // need to differentiate whether a block is atomic or not, if the source
        // or target are rolecrossing, if one of the common or one of the addiotional
        // inputs/outputs are used, or if some of the original (not decomposed
        // processes inputs and outputs are used. The if brackets distinguish
        // every of the above mentioned cases by their conditions.
        
        if ((*con)->src->atomic == true && (!(*con)->roleCrossing || !(*con)->input))
        {
            if ((*con)->src->type == STARTNODE)
            {
                (*con)->firstArcs.first = PN->newArc(PN->findPlace((globalPrefix + "startNode." + (*con)->src->name)),t);
            }
        
            if ((*con)->src->type == ENDNODE)
            {
                (*con)->firstArcs.first = PN->newArc(PN->findPlace((globalPrefix + "endNode." + (*con)->src->name)),t);             
            }
        
            if ((*con)->src->type == STOPNODE)
            {
                (*con)->firstArcs.first = PN->newArc(PN->findPlace((globalPrefix + "stopNode." + (*con)->src->name)),t);             
            }
        }
        else if((*con)->src == this && (!(*con)->roleCrossing))
        {
            for (set<string>::iterator input = inputs.begin(); input != inputs.end(); input++)
            {
                if( (*input) == (*con)->sourceContact )
                {
                    (*con)->firstArcs.first = PN->newArc(PN->findPlace(globalPrefix + "input." + (*input)),t);
                    break;             
                }
            }
            for (set<string>::iterator output = outputs.begin(); output != outputs.end(); output++)
            {
                if( (*output) == (*con)->sourceContact )
                {
                    (*con)->firstArcs.first = PN->newArc(PN->findPlace(globalPrefix + "output." + (*output)),t);
                    break;             
                }
            }
        }
        else
        {
            if(!(*con)->roleCrossing || !(*con)->input)
            {
                bool found = false;
                for (set<string>::iterator output = (*con)->src->outputs.begin(); output != (*con)->src->outputs.end(); output++)
                {
                    if( (*output) == (*con)->sourceContact )
                    {
                        (*con)->firstArcs.first = PN->newArc(PN->findPlace((*con)->src->returnType() + "." + (*con)->src->name + ".output." + (*output)),t);
                        found = true;
                        break;             
                    }
                }
                
             }
        }
        if ((*con)->tgt->atomic == true && (!(*con)->roleCrossing || (*con)->input))
        {
            if ((*con)->tgt->type == STARTNODE)
            {
                (*con)->firstArcs.second = PN->newArc(t,PN->findPlace((globalPrefix + "startNode." + (*con)->tgt->name)));             
            }
        
            if ((*con)->tgt->type == ENDNODE)
            {
                (*con)->firstArcs.second = PN->newArc(t,PN->findPlace((globalPrefix + "endNode." + (*con)->tgt->name)));             
            }
        
            if ((*con)->tgt->type == STOPNODE)
            {
                (*con)->firstArcs.second = PN->newArc(t,PN->findPlace((globalPrefix + "stopNode." + (*con)->tgt->name)));             
            }
        }
        else if((*con)->tgt == this && (!(*con)->roleCrossing))
        {
            for (set<string>::iterator input = inputs.begin(); input != inputs.end(); input++)
            {
                if( (*input) == (*con)->targetContact )
                {
                    (*con)->firstArcs.second = PN->newArc(t,PN->findPlace(globalPrefix + "input." + (*input)));
                    break;             
                }
            }
            for (set<string>::iterator output = outputs.begin(); output != outputs.end(); output++)
            {
                if( (*output) == (*con)->targetContact )
                {
                    (*con)->firstArcs.second = PN->newArc(t,PN->findPlace(globalPrefix + "output." + (*output)));
                    break;             
                }
            }
        }
        else
        {
            if(!(*con)->roleCrossing || (*con)->input)            
            {
                bool found = false;
                for (set<string>::iterator input = (*con)->tgt->inputs.begin(); input != (*con)->tgt->inputs.end(); input++)
                {
                    if( (*input) == (*con)->targetContact )
                    {
                        (*con)->firstArcs.second = PN->newArc(t,PN->findPlace((*con)->tgt->returnType() + "." + (*con)->tgt->name + ".input." + (*input)));
                        found = true;
                        break;             
                    }
                }
            }
        }
    }
}

//! \brief DECOMPOSITION: This is the actual decomposition function, that is
//!        executed before the petrinet is created. It computes for every block
//!        and connection whether it is enabled and whether the connection is
//!        rolecrossing
//! \param roleSet a set of roles given as strings
//! \param mode a mode for how to treat the roleset
void Block::cutNet(set<string> roleSet, int mode)
{
    // print all given roles of the roleset
	cerr << "The given roles are:\n";
    for (set<string>::iterator role = roleSet.begin(); role != roleSet.end(); role++)
    {
        cerr << "    " + (*role) + "\n";        
    }
    cerr << "Der Modus ist: \"";
    
    // print an explanation for the used mode
    switch(mode)
    {
        case 1: cerr << "Use all blocks that have exactly those roles\"\n"; break;
        case 2: cerr << "Use all blocks that have at least one of those roles\"\n"; break;
        case 3: cerr << "Use all blocks that have at least all of those roles\"\n"; break;
        case 4: cerr << "Use all blocks that have none of the given roles\"\n"; break;
    }
    
    // This is the set where all enabled blocks will be gathered. A block is enabled
    // if it fits the roles in the given mode
    set<Block*> cutBlocks;
    
    // iterate over all blocks of the process
    for (set<Block*>::iterator child = children.begin(); child != children.end(); child++)
    {
        
    	// "Use all blocks that have exactly those roles" mode
    	if(mode == 1)
        {
            bool isEqual = true;
            
            // if the set of roles differ this is already false
            if(roleSet.size() != (*child)->roleRequirements.size())
            {
                continue;
            }
            
            // if the number of roles match, check for every role if it is
            // contained in the role requirements
            for (set<string>::iterator role = roleSet.begin(); role != roleSet.end(); role++)
            {
                if((*child)->roleRequirements.find((*role)) == (*child)->roleRequirements.end())
                {
                    isEqual = false;
                    break;
                }
            }
            
            // If the sets are equal add this block to the cutBlocks
            if (isEqual)
            {
                cutBlocks.insert((*child));
            }
        }
    	// "Use all blocks that have at least one of those roles\" mode
    	if(mode == 2)
        {
            // test if at least one of the roles matches. If so, add this block to cutBlocks
    		for (set<string>::iterator role = roleSet.begin(); role != roleSet.end(); role++)
            {
                if((*child)->roleRequirements.find((*role)) != (*child)->roleRequirements.end())
                {
                    cutBlocks.insert((*child));
                    break;
                }
            }        
        }
    	// "Use all blocks that have at least all of those roles" mode
        if(mode == 3)
        {
            // assume this block fits
        	bool isEqual = true;
            
            // check for every role in the set, if it is also in the blocks roles
            for (set<string>::iterator role = roleSet.begin(); role != roleSet.end(); role++)
            {
                if((*child)->roleRequirements.find((*role)) == (*child)->roleRequirements.end())
                {
                    isEqual = false;
                    break;
                }
            }
            
            // if so, add the block to the cut Blocks
            if (isEqual)
            {
                cutBlocks.insert((*child));
            }
        }
        if(mode == 4)
        // "Use all blocks that have none of the given roles" mode
        {
            // assume this block fits
        	bool isEqual = true;
        	
        	// if one of the roles if found, this block doesnt fit
            for (set<string>::iterator role = roleSet.begin(); role != roleSet.end(); role++)
            {
                if((*child)->roleRequirements.find((*role)) != (*child)->roleRequirements.end())
                {
                    isEqual = false;
                }
            }
            
            // none fitting role could be found, add the block to the cut blocks
            if (isEqual)
            {
                cutBlocks.insert((*child));
            }
        }
    }
    
    // print all blocks that match the roles 
    cerr << "The following blocks match the roles:\n";
    for (set<Block*>::iterator block = cutBlocks.begin(); block != cutBlocks.end(); block++)
    {
        cerr << "    " + (*block)->name + "\n";        
    }
    
    // Horrible complexity
    // Iterate over all blocks, if it is not part of the cut Blocks, it is disabled
    // (enabled is default)
    for (set<Block*>::iterator child = children.begin(); child != children.end(); child++)
    {
        if (cutBlocks.find(*child) == cutBlocks.end())
        {
            (*child)->enabled = false;
            cerr << "    Dieser Block ist jetzt false: " + (*child)->name + "\n";        
        }
    }
    
    // iterate over all connections
    for ( set<BlockConnection*>::iterator con = connections.begin(); con != connections.end(); con++)
    {
        // assume its neither enabled nor rolecrossing
    	(*con)->enabled = false;
        (*con)->roleCrossing = false;
        
        
        // if the source of the connection is enabled, this connection is too.
        // assume it is rolecrossing and not an input
        if (cutBlocks.find((*con)->src) != cutBlocks.end())
        {
            (*con)->enabled = true;
            (*con)->roleCrossing = true;
            (*con)->input = false;            
        }

        // if the target of the connection is enabled, this connection is too.
        // it can only be an output or neither and if it was rolecrossing 
        // (because the source was also enabled) it cant be rolecrossing anymore.
        if (cutBlocks.find((*con)->tgt) != cutBlocks.end())
        {
            (*con)->enabled = true;
            (*con)->input = true;
            if ((*con)->roleCrossing)
            {
                (*con)->roleCrossing = false;
            }
            else
            {
                (*con)->roleCrossing = true;
            }
        }
    }
    
    // Check all enabled connections again:
    // if it was assumed rolecrossing, but the nonenabled source/target is the
    // process itself, it is not rolecrossing, but an original input/output
    for ( set<BlockConnection*>::iterator con = connections.begin(); con != connections.end(); con++)
    {
        if ((*con)->enabled)
        {
        
            if((*con)->roleCrossing)
            {
                if ((*con)->src == this)
                {
                    (*con)->roleCrossing = false;
                }
                if ((*con)->tgt == this)
                {
                    (*con)->roleCrossing = false;
                }
            }
        }
    }    
}

//! \brief DECOMPOSTION: Determine all connections that have an input or a startnode
//!        as source. If so, disable it and save the connection as a startingconnection
void Block::disableStart()
{
    // Sets to gather the connections
	set<BlockConnection*> relevantConnections;
    Block* relevantTarget;
    
    // iterate over all connections
    for ( set<BlockConnection*>::iterator con = connections.begin(); con != connections.end(); con++)
    {
        // if it is enabled
    	if ((*con)->enabled)
        {
            // if the source is an input add the connection to the relevant ones
    		if ((*con)->src == this)
            {
                relevantConnections.insert((*con));
                relevantTarget = (*con)->tgt;
            }
    		// or if the source is a startnode add the connection to the relevant ones
            else if ((*con)->src->type == STARTNODE)
            {
                relevantConnections.insert((*con));
            }
        }
    }
    
    // disable all relevant connections and add them to the starting connections
    for ( set<BlockConnection*>::iterator con = relevantConnections.begin(); con != relevantConnections.end(); con++)
    {
        if ((*con)->tgt == relevantTarget)
        {
            (*con)->enabled = false;
            startingConnections.insert(*con);
        }
    }
}


//! \brief DECOMPOSTION: Determine all connections that have an output or a endnode
//!        as target. If so, disable it and save the connection as an endingconnection
void Block::disableEnd()
{
    // Sets to gather the connections
    set<BlockConnection*> relevantConnections;
    Block* relevantSource;

    // iterate over all connections
    for ( set<BlockConnection*>::iterator con = connections.begin(); con != connections.end(); con++)
    {
        // if it is enabled
        if ((*con)->enabled)
        {
            // if the target is an output add the connection to the relevant ones
            if ((*con)->tgt == this)
            {
                relevantConnections.insert((*con));
                relevantSource = (*con)->src;
            }
            // if the target is an endnode add the connection to the relevant ones
// <Dirk.F start> included stop node treatment
//            else if ((*con)->tgt->type == ENDNODE)
            else if ((*con)->tgt->type == ENDNODE || (*con)->tgt->type == STOPNODE)
// <Dirk.F end>
            {
                relevantConnections.insert((*con));
            }
        }
    }
    
    // disable all relevant connections and add them to the ending connections
    for ( set<BlockConnection*>::iterator con = relevantConnections.begin(); con != relevantConnections.end(); con++)
    {
        if ((*con)->src == relevantSource)
        {
            (*con)->enabled = false;
            endingConnections.insert(*con);
        }
    }
          
}


//! \brief DECOMPOSITION
void Block::mergeStart(PetriNet* PN)
{
    
	Transition* t;
    if (!startingConnections.empty())
    {
        t = PN->newTransition(("process."+ name + ".initialize"));
    }
    else
    {
        return;
    }
    
    for ( set<BlockConnection*>::iterator con = startingConnections.begin(); con != startingConnections.end(); con++)
    {
    	if ((*con)->src == this)
        {
    		for (set<string>::iterator input = inputs.begin(); input != inputs.end(); input++)
            {
    			if( (*input) == (*con)->sourceContact )
                {
                    (*con)->firstArcs.second = PN->newArc(PN->findPlace("process." + name + ".input." + (*input)), t);
                    break;             
                }
            }
        }
    	else if ((*con)->src->type == STARTNODE)
        {
            (*con)->firstArcs.second = PN->newArc(PN->findPlace((name + ".startNode." + (*con)->src->name)), t);             
        }
        
        
        if ((*con)->tgt->atomic == true && (!(*con)->roleCrossing || (*con)->input))
        {
            if ((*con)->tgt->type == STARTNODE)
            {
                (*con)->firstArcs.second = PN->newArc(t,PN->findPlace((name + ".startNode." + (*con)->tgt->name)));             
            }
        
            if ((*con)->tgt->type == ENDNODE)
            {
                (*con)->firstArcs.second = PN->newArc(t,PN->findPlace((name + ".endNode." + (*con)->tgt->name)));             
            }
        
            if ((*con)->tgt->type == STOPNODE)
            {
                (*con)->firstArcs.second = PN->newArc(t,PN->findPlace((name + ".stopNode." + (*con)->tgt->name)));             
            }
        }
        else if((*con)->tgt == this && (!(*con)->roleCrossing))
        {
            for (set<string>::iterator input = inputs.begin(); input != inputs.end(); input++)
            {
                if( (*input) == (*con)->targetContact )
                {
                    (*con)->firstArcs.second = PN->newArc(t,PN->findPlace("process." + name + ".input." + (*input)));
                    break;             
                }
            }
            for (set<string>::iterator output = outputs.begin(); output != outputs.end(); output++)
            {
                if( (*output) == (*con)->targetContact )
                {
                    (*con)->firstArcs.second = PN->newArc(t,PN->findPlace("process." + name + ".output." + (*output)));
                    break;             
                }
            }
        }
        else
        {
            if(!(*con)->roleCrossing || (*con)->input)            
            {
                bool found = false;
                for (set<string>::iterator input = (*con)->tgt->inputs.begin(); input != (*con)->tgt->inputs.end(); input++)
                {
                    if( (*input) == (*con)->targetContact )
                    {
                        (*con)->firstArcs.second = PN->newArc(t,PN->findPlace((*con)->tgt->returnType() + "." + (*con)->tgt->name + ".input." + (*input)));
                        found = true;
                        break;             
                    }
                }
                
                if( !found && (*con)->targetAdditional)
                {
                    for (set<string>::iterator input = (*con)->tgt->additionalInputs.begin(); input != (*con)->tgt->additionalInputs.end(); input++)
                    {
                        if( (*input) == (*con)->targetContact )
                        {
                            (*con)->firstArcs.second = PN->newArc(t,PN->findPlace((*con)->tgt->returnType() + "." + (*con)->tgt->name + ".additionalInput." + (*input)));
                            found = true;
                            break;             
                        }
                    }                
                }
            }
        }
    }
}


//! \brief DECOMPOSITION
void Block::mergeEnd(PetriNet* PN)
{
    Transition* t;
    if (!endingConnections.empty())
    {
        t = PN->newTransition(("process."+ name + ".finalize"));
    }
    else
    {
        return;
    }
    
    for ( set<BlockConnection*>::iterator con = endingConnections.begin(); con != endingConnections.end(); con++)
    {
        if ((*con)->tgt == this)
        {
            for (set<string>::iterator output = outputs.begin(); output != outputs.end(); output++)
            {
                if( (*output) == (*con)->targetContact )
                {
                    (*con)->firstArcs.second = PN->newArc(t, PN->findPlace("process." + name + ".output." + (*output)));
                    break;             
                }
            }
        }
        else if ((*con)->tgt->type == ENDNODE)
        {
            (*con)->firstArcs.second = PN->newArc(t, PN->findPlace((name + ".endNode." + (*con)->tgt->name)));             
        }
// <Dirk.F start> added stop node treatment
        else if ((*con)->tgt->type == STOPNODE)
        {
            (*con)->firstArcs.second = PN->newArc(t, PN->findPlace((name + ".stopNode." + (*con)->tgt->name)));             
        }
// <Dirk.F end>
        
        if ((*con)->src->atomic == true && (!(*con)->roleCrossing || !(*con)->input))
        {
            if ((*con)->src->type == STARTNODE)
            {
                (*con)->firstArcs.first = PN->newArc(PN->findPlace((name + ".startNode." + (*con)->src->name)),t);
            }
        
            if ((*con)->src->type == ENDNODE)
            {
                (*con)->firstArcs.first = PN->newArc(PN->findPlace((name + ".endNode." + (*con)->src->name)),t);             
            }
        
            if ((*con)->src->type == STOPNODE)
            {
                (*con)->firstArcs.first = PN->newArc(PN->findPlace((name + ".stopNode." + (*con)->src->name)),t);             
            }
        }
        else if((*con)->src == this && (!(*con)->roleCrossing))
        {
            for (set<string>::iterator input = inputs.begin(); input != inputs.end(); input++)
            {
                if( (*input) == (*con)->sourceContact )
                {
                    (*con)->firstArcs.first = PN->newArc(PN->findPlace("process." + name + ".input." + (*input)),t);
                    break;             
                }
            }
            for (set<string>::iterator output = outputs.begin(); output != outputs.end(); output++)
            {
                if( (*output) == (*con)->sourceContact )
                {
                    (*con)->firstArcs.first = PN->newArc(PN->findPlace("process." + name + ".output." + (*output)),t);
                    break;             
                }
            }
        }
        else
        {
            if(!(*con)->roleCrossing || !(*con)->input)
            {
                bool found = false;
                for (set<string>::iterator output = (*con)->src->outputs.begin(); output != (*con)->src->outputs.end(); output++)
                {
                    if( (*output) == (*con)->sourceContact )
                    {
                        (*con)->firstArcs.first = PN->newArc(PN->findPlace((*con)->src->returnType() + "." + (*con)->src->name + ".output." + (*output)),t);
                        found = true;
                        break;             
                    }
                }
                
                if( !found && (*con)->sourceAdditional)
                {
                    for (set<string>::iterator output = (*con)->src->additionalOutputs.begin(); output != (*con)->src->additionalOutputs.end(); output++)
                    {
                        if( (*output) == (*con)->sourceContact )
                        {
                            (*con)->firstArcs.first = PN->newArc(PN->findPlace((*con)->src->returnType() + "." + (*con)->src->name + ".additionalOutput." + (*output)),t);
                            found = true;
                            break;             
                        }
                    }                
                }
            }
        }
    }     
}


//! \brief DECOMPOSITION
void Block::createEndState(PetriNet* PN)
{
    set<Place*> mergePlaces1;
    set<Place*> mergePlaces2;
    set<string> histories;
    globals::finalCondition = "";
    for ( set<BlockConnection*>::iterator con = connections.begin(); con != connections.end(); con++)
    {
        if ((*con)->enabled && (*con)->roleCrossing && !(*con)->input)
        {
            Place* p = PN->newPlace("process." + name + ".connection." + (*con)->name + ".final");
            p->isFinal = true;
            PN->newArc((*con)->firstTransition, p);
            mergePlaces2.insert(p);
        }
    }
    
    for ( set<BlockConnection*>::iterator con = connections.begin(); con != connections.end(); con++)
    {
        if ((*con)->enabled && (*con)->roleCrossing && (*con)->input)
        {
            Place* p = PN->newPlace("process." + name + ".connection." + (*con)->name + ".initial");
            PN->newArc(p, (*con)->firstTransition);
            mergePlaces1.insert(p);
        }
    }
/*
    bool markAll = true;
    
    
    for ( set<Block*>::iterator block = children.begin(); block != children.end() ; block++)
    {
        if (((*block)->type == STARTNODE) && (*block)->enabled == true)
        {
            markAll = false;
        }
    }
  */  
    //set<Place*>::iterator merge2 = mergePlaces2.begin();
    for ( set<Place*>::iterator merge1 = mergePlaces1.begin(); merge1 != mergePlaces1.end(); merge1++)
    {
        
        (*merge1)->mark();
        PN->generatedInitials.insert(*merge1);
        
        //histories.insert((*merge1)->nodeFullName());
        //PN->mergePlaces((*merge1)->nodeFullName(),(*merge2)->nodeFullName());
        //merge2++;
    }
    
    // Code needs serious tightening here
    globals::finalCondition= "has to be determined by the output function itself";
}
void Block::adjustRoles()
{
    if(type == PROCESS)
    {
        Place* p;
        Transition* t;
        set<Block*> noRoles;
        for (set<Block*>::iterator child = children.begin(); child != children.end(); child++)
        {
            if ((*child)->roleRequirements.empty())
            {
                noRoles.insert(*child);
                cerr << (*child)->returnType() << " " << (*child)->name << "!\n";
            }
        }
        
        if (children.size() == noRoles.size())
        {
            return;
        }
        
        unsigned int lastSize = 0;
        while (noRoles.size() != lastSize)
        {
            lastSize = noRoles.size();
            for (set<Block*>::iterator child = children.begin(); child != children.end(); child++)
            {
                if ((*child)->type == STARTNODE)
                {
                    for ( set<BlockConnection*>::iterator con = connections.begin(); con != connections.end(); con++)
                    {
                        if ((*con)->src == (*child))
                        {
                            for(set<string>::iterator role = (*con)->tgt->roleRequirements.begin(); role != (*con)->tgt->roleRequirements.end(); role++)
                            {
                                if((*child)->roleRequirements.insert(*role).second)
                                {
                                    noRoles.erase(*child);
                                }
                            } 
                        }
                    }
                }
            
                if ((*child)->type == ENDNODE)
                {
                    for ( set<BlockConnection*>::iterator con = connections.begin(); con != connections.end(); con++)
                    {
                        if ((*con)->tgt == (*child))
                        {
                            for(set<string>::iterator role = (*con)->src->roleRequirements.begin(); role != (*con)->src->roleRequirements.end(); role++)
                            {
                                if((*child)->roleRequirements.insert(*role).second)
                                {
                                    noRoles.erase(*child);
                                }
                            } 
                        }
                    }
                }
            
                if ((*child)->type == STOPNODE)
                {
                    for ( set<BlockConnection*>::iterator con = connections.begin(); con != connections.end(); con++)
                    {
                        if ((*con)->tgt == (*child))
                        {
                            for(set<string>::iterator role = (*con)->src->roleRequirements.begin(); role != (*con)->src->roleRequirements.end(); role++)
                            {
                                if((*child)->roleRequirements.insert(*role).second)
                                {
                                    noRoles.erase(*child);
                                }
                            } 
                        }
                    }
                }
        
                if ((*child)->type == DECISION)
                {
                    for ( set<BlockConnection*>::iterator con = connections.begin(); con != connections.end(); con++)
                    {
                        if ((*con)->tgt == (*child))
                        {
                            for(set<string>::iterator role = (*con)->src->roleRequirements.begin(); role != (*con)->src->roleRequirements.end(); role++)
                            {
                                if((*child)->roleRequirements.insert(*role).second)
                                {
                                    noRoles.erase(*child);
                                }
                            } 
                        }
                    }
                }
        
                if ((*child)->type == FORK)
                {
                    for ( set<BlockConnection*>::iterator con = connections.begin(); con != connections.end(); con++)
                    {
                        if ((*con)->tgt == (*child))
                        {
                            for(set<string>::iterator role = (*con)->src->roleRequirements.begin(); role != (*con)->src->roleRequirements.end(); role++)
                            {
                                if((*child)->roleRequirements.insert(*role).second)
                                {
                                    noRoles.erase(*child);
                                }
                            } 
                        }
                    }
                }
        
                if ((*child)->type == MERGE)
                {
                    for ( set<BlockConnection*>::iterator con = connections.begin(); con != connections.end(); con++)
                    {
                        if ((*con)->tgt == (*child))
                        {
                            for(set<string>::iterator role = (*con)->src->roleRequirements.begin(); role != (*con)->src->roleRequirements.end(); role++)
                            {
                                if((*child)->roleRequirements.insert(*role).second)
                                {
                                    noRoles.erase(*child);
                                }
                            } 
                        }
                    }
                }
        
                if ((*child)->type == JOIN)
                {
                    for ( set<BlockConnection*>::iterator con = connections.begin(); con != connections.end(); con++)
                    {
                        if ((*con)->tgt == (*child))
                        {
                            for(set<string>::iterator role = (*con)->src->roleRequirements.begin(); role != (*con)->src->roleRequirements.end(); role++)
                            {
                                if((*child)->roleRequirements.insert(*role).second)
                                {
                                    noRoles.erase(*child);
                                }
                            } 
                        }
                    }
                }
            }
        }
    }
}
Block::Block( blockType mytype, bool ifcomplex, bool ifatomic)
{
    type = mytype;
    complex = ifcomplex;
    atomic = ifatomic;
    enabled = true;
    filtered = false;
    syntaxError = false;
}
Block::~Block()
{
}
Block* BlockConnection::getTarget()
{
    return tgt;
}
