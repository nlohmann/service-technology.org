/*****************************************************************************\
 UML2oWFN -- Translating UML2 Activity Diagrams to Petri nets

 Copyright (C) 2007, 2008, 2009  Dirk Fahland and Martin Znamirowski

 UML2oWFN is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 UML2oWFN is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with UML2oWFN.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

#include <list>
#include <string>
#include <map>
#include <iostream>
#include "AST.h"
#include "debug.h"
#include "helpers.h"  // for toInt
#include "options.h"

using std::string;
using std::map;
using std::list;
using std::cerr;
using std::endl;


// Global Variables needed to translate the xml-tree
// into the internal representation
Process* currProcess = NULL;
FlowContentNode* currNode = NULL;
Pin* additionalOutput = NULL;
Pin* additionalInput = NULL;
map<string, FlowContentNode*> currNodeNameMap;
InputCriterion* currentInputCriterion = NULL;
OutputCriterion* currentOutputCriterion = NULL;
InputCriterion* currentProcessInputCriterion = NULL;
OutputCriterion* currentProcessOutputCriterion = NULL;
PinCombination* currentBranch = NULL;
FlowContentConnection* currConnection = NULL;
bool prozessSchachtelung = false;

/*!
 */
ASTElement::ASTElement() {
  parent = NULL;
}

/*!
 */
ASTNode* ASTElement::getParent() {
    return parent;
}

// constructor
ASTLiteral::ASTLiteral(string literalContent, ASTNode* parentNode) {
    content = literalContent;
    parent = parentNode;
}

// generic destructor
ASTLiteral::~ASTLiteral() {}

// constructor
ASTNode::ASTNode(string nodeTag, ASTNode* parentNode) {
    tag = nodeTag;
    parent = parentNode;
}

// generic destructor
ASTNode::~ASTNode() {}

/// adds an Attribute to the attribute map
void ASTNode::addAttribute(string attribute, string value) {
    attributes[attribute] = value;
}

/// adds a child to this node
void ASTNode::addChild(ASTElement* child) {
    children.push_back(child);
}

// generates a debug output on the console for this node
void ASTNode::debugOutput(string delay) {
    std::cout << delay << tag << std::endl;

    // print all attributes
    for (map<string, string>::iterator attribute = attributes.begin(); attribute != attributes.end(); attribute ++) {
        std::cout << delay << " \"" << attribute->first << " = " << attribute->second << "\"" << std::endl;
    }

    // proceed all child elements
    for (list<ASTElement*>::iterator child = children.begin(); child != children.end(); child++) {
        (*child)->debugOutput(delay + "  ");
    }
}

// First dive into the XML-tree to collect information about all global tasks, services and processes, so
// that callTo-tasks can instantiate them correctly
void ASTNode::prepareInstances(list<Process*>& processes, list<SimpleTask*>& tasks, list<SimpleTask*>& services) {

    // Since we only want to gather the data for the global tasks, services and processes
    // there is a series of tags, which we dont want to ascend below. Aborting at this
    // place is primarily a matter of optimization.
    if (tag == "flowContent" || tag == "catalogs") {
        return;
    }

    // Now Proceed depending on the tag of this AST node, this is done in several Blocks:

    // task, service, process:
    if (tag == "task") {
        currNode = new SimpleTask(attributes["name"], NTASK, this, NULL);
        tasks.push_back(static_cast<SimpleTask*>(currNode));
    } else if (tag == "humanTask") {

    // translate humanTasks as normal tasks
        currNode = new Process(attributes["name"], NTASK, this, NULL);
        tasks.push_back(static_cast<SimpleTask*>(currNode));
    } else if (tag == "service") {
        currNode = new SimpleTask(attributes["name"], NSERVICE, this, NULL);
        services.push_back(static_cast<SimpleTask*>(currNode));
    } else if (tag == "process") {
        currNode = new Process(attributes["name"], NPROCESS, this, NULL);
        processes.push_back(static_cast<Process*>(currNode));


    // input criteria and output criteria:
    } else if (tag == "inputCriterion") {
        if (currentOutputCriterion == NULL) {
            InputCriterion* newCriterion = new InputCriterion(attributes["name"], currNode);
            static_cast<Task*>(currNode)->inputCriteria.push_back(newCriterion);
            currentInputCriterion = static_cast<Task*>(currNode)->inputCriteria.back();
        } else {
            currentOutputCriterion->relatedInputCriteria.push_back(static_cast<Task*>(currNode)->getInputCriterionByName(attributes["name"]));
            return;
        }
    } else if (tag == "outputCriterion") {
        OutputCriterion* newCriterion = new OutputCriterion(attributes["name"], currNode);
        static_cast<Task*>(currNode)->outputCriteria.push_back(newCriterion);
        currentOutputCriterion = static_cast<Task*>(currNode)->outputCriteria.back();


    // input and output
    } else if (tag == "input") {
        if (currentInputCriterion == NULL) {
            Pin* newPin = new Pin(attributes["name"],currNode);
            currNode->inputPins.push_back(newPin);
            // set minimum and maximum range of pin multiplicities
            if (attributes.find("minimum") != attributes.end())
              newPin->min = toInt(attributes["minimum"]);
            if (attributes.find("maximum") != attributes.end())
              newPin->max = toInt(attributes["maximum"]);
        } else {
            Pin* existingPin = currNode->getPinByName(attributes["name"]);
            if (!existingPin->free()) {
                currNode->processCharacteristics |= UML_PC(PC_OVERLAPPING);
            }
            currentInputCriterion->pins.push_back(existingPin);
            existingPin->setCriterion(currentInputCriterion);
        }
    } else if (tag == "output") {
        if (currentOutputCriterion == NULL) {
            Pin* newPin = new Pin(attributes["name"],currNode);
            currNode->outputPins.push_back(newPin);
            // set minimum and maximum range of pin multiplicities
            if (attributes.find("minimum") != attributes.end())
              newPin->min = toInt(attributes["minimum"]);
            if (attributes.find("maximum") != attributes.end())
              newPin->max = toInt(attributes["maximum"]);

        } else {
            Pin* existingPin = currNode->getPinByName(attributes["name"]);
            if (!existingPin->free()) {
                currNode->processCharacteristics |= UML_PC(PC_OVERLAPPING);
            }
            currentOutputCriterion->pins.push_back(existingPin);
            existingPin->setCriterion(currentOutputCriterion);
        }


    // role requirement
    } else if (tag == "roleRequirement") {
        currNode->roles.insert(attributes["role"]);
    }


    // always proceed all child-elements of this node
    for (list<ASTElement*>::iterator child = children.begin(); child != children.end(); child++) {
        (*child)->prepareInstances(processes,tasks,services);
    }


    // if a Criterion has finished all it's children we reset it
    if (tag == "inputCriterion") {
        currentInputCriterion = NULL;
    } else if (tag == "outputCriterion") {
        currentOutputCriterion = NULL;
    }

}

// Completly translates the XML-tree into the internal representation of the BOM model
void ASTNode::finishInternal(list<Process*>& processes, list<SimpleTask*>& tasks, list<SimpleTask*>& services, list<Process*>& realProcesses) {
    //
    // At this point we can optimize by returning upwards either when we discover catalogs or if we
    // would again explore global tasks and services (this has already been done in prepareInstances())
    if (tag == "catalogs" || tag == "tasks") {
        return;
    } else if (( tag == "task" || tag == "service" || tag == "humanTask")
            && currProcess == NULL) {
        return;
    } else if (tag == "flowContent" && prozessSchachtelung) {
        prozessSchachtelung = false;
    }

#ifdef DEBUG
    if (currProcess == NULL) {
      trace(TRACE_VERY_DEBUG, "ERROR: process is NULL\n");
      trace(TRACE_VERY_DEBUG, "was trying to translate '" + tag + "'\n");
    } else {
      if (currNode == NULL)
        trace(TRACE_VERY_DEBUG, "creating '" + tag + "' of process " + currProcess->getName() + "\n");
      else
        trace(TRACE_VERY_DEBUG, "creating '" + tag + "' of " + currProcess->getName() + "/" + currNode->getName() + "\n");
    }
#endif

    // Now Proceed depending on the tag of this AST node, this is done in several Blocks:

    // startnode, stopnode, endnode:
    if (tag == "startNode") {
        currNode = new AtomicCFN(attributes["name"], NSTARTNODE, this, currProcess);
        currNode->outputPins.push_back(new Pin("default", currNode));
        currProcess->addFCN(static_cast<AtomicCFN*>(currNode));
        currNodeNameMap[attributes["name"]] = currNode;
    } else if (tag == "stopNode") {
        currNode = new AtomicCFN(attributes["name"], NSTOPNODE, this, currProcess);
        currNode->inputPins.push_back(new Pin("default", currNode));
        currProcess->addFCN(static_cast<AtomicCFN*>(currNode));
        currNodeNameMap[attributes["name"]] = currNode;
    } else if (tag == "endNode") {
        currNode = new AtomicCFN(attributes["name"], NENDNODE, this, currProcess);
        currNode->inputPins.push_back(new Pin("default", currNode));
        currProcess->addFCN(static_cast<AtomicCFN*>(currNode));
        currNodeNameMap[attributes["name"]] = currNode;


    // All nodes that can be treated as simple tasks:
    } else if (tag == "task") {
        currNode = new SimpleTask(attributes["name"], NTASK, this, currProcess);
        currProcess->addFCN(static_cast<SimpleTask*>(currNode));
        currNodeNameMap[attributes["name"]] = currNode;
    } else if (tag == "humanTask") {
        // translate humanTask as normal task
        currNode = new SimpleTask(attributes["name"], NTASK, this, currProcess);
        currProcess->addFCN(static_cast<SimpleTask*>(currNode));
        currNodeNameMap[attributes["name"]] = currNode;
    } else if (tag == "service") {
        currNode = new SimpleTask(attributes["name"], NSERVICE, this, currProcess);
        currProcess->addFCN(static_cast<SimpleTask*>(currNode));
        currNodeNameMap[attributes["name"]] = currNode;
    } else if (tag == "notificationBroadcaster") {
        currNode = new SimpleTask(attributes["name"], NNOTIFICATIONBROADCASTER, this, currProcess);
        currProcess->addFCN(static_cast<SimpleTask*>(currNode));
        currNodeNameMap[attributes["name"]] = currNode;
    } else if (tag == "notificationReceiver") {
        currNode = new SimpleTask(attributes["name"], NNOTIFICATIONRECEIVER, this, currProcess);
        currProcess->addFCN(static_cast<SimpleTask*>(currNode));
        currNodeNameMap[attributes["name"]] = currNode;
    } else if (tag == "observer") {
        currNode = new SimpleTask(attributes["name"], NOBSERVER, this, currProcess);
        currProcess->addFCN(static_cast<SimpleTask*>(currNode));
        currNodeNameMap[attributes["name"]] = currNode;
    } else if (tag == "map") {
        currNode = new SimpleTask(attributes["name"], NMAP, this, currProcess);
        currProcess->addFCN(static_cast<SimpleTask*>(currNode));
        currNodeNameMap[attributes["name"]] = currNode;
    } else if (tag == "timer") {
        currNode = new SimpleTask(attributes["name"], NTIMER, this, currProcess);
        currProcess->addFCN(static_cast<SimpleTask*>(currNode));
        currNodeNameMap[attributes["name"]] = currNode;


    // all control flow nodes:
    } else if (tag == "decision") {
        currNode = new Decision(attributes["name"], NDECISION, this, currProcess);
        currProcess->addFCN(static_cast<Decision*>(currNode));
        currNodeNameMap[attributes["name"]] = currNode;
        if (attributes["isInclusive"] == "true") {
            (static_cast<Decision*>(currNode))->inclusive = true;
        }
    } else if (tag == "merge") {
        currNode = new Merge(attributes["name"], NMERGE, this, currProcess);
        currProcess->addFCN(static_cast<Merge*>(currNode));
        currNodeNameMap[attributes["name"]] = currNode;
    } else if (tag == "fork") {
        currNode = new Fork(attributes["name"], NFORK, this, currProcess);
        currProcess->addFCN(static_cast<Fork*>(currNode));
        currNodeNameMap[attributes["name"]] = currNode;
    } else if (tag == "join") {
        currNode = new Join(attributes["name"], NJOIN, this, currProcess);
        currProcess->addFCN(static_cast<Join*>(currNode));
        currNodeNameMap[attributes["name"]] = currNode;


    // All callTo-types:
    // Warning: Most of the code in the callTo-translation bothers with
    // deep-copying values from the global tasks, services, processes and
    // thus is neither well readable nor small
    // TODO: Since most of the copying is similar it should be made into a seperate function
    } else if (tag == "callToTask") {
        currNode = new SimpleTask(attributes["name"], NCALLTOTASK, this, currProcess);

        // Find the correct task
        for(list<SimpleTask*>::iterator globalTask = tasks.begin(); globalTask != tasks.end(); globalTask++) {

            // if it is the correct task
            if((*globalTask)->getName() == attributes["task"]) {

                // copy the role requirements
                currNode->inherit(*globalTask);

                // inherit process characteristics
                currNode->processCharacteristics |= (*globalTask)->processCharacteristics;

                // copy all input pins
                for(list<Pin*>::iterator copyInput = (*globalTask)->inputPins.begin(); copyInput != (*globalTask)->inputPins.end(); copyInput++) {
                    Pin* newPin = new Pin((*copyInput)->getName(), currNode);
                    currNode->inputPins.push_back(newPin);
                    newPin->min = (*copyInput)->min;
                    newPin->max = (*copyInput)->max;
                }

                // copy all output pins
                for(list<Pin*>::iterator copyOutput = (*globalTask)->outputPins.begin(); copyOutput != (*globalTask)->outputPins.end(); copyOutput++) {
                    Pin* newPin = new Pin((*copyOutput)->getName(), currNode);
                    currNode->inputPins.push_back(newPin);
                    newPin->min = (*copyOutput)->min;
                    newPin->max = (*copyOutput)->max;
                }

                // copy all input criteria and translate the pin pointers of the original to pin pointers of the instance
                for(list<InputCriterion*>::iterator copyInputCriterion = (*globalTask)->inputCriteria.begin(); copyInputCriterion != (*globalTask)->inputCriteria.end(); copyInputCriterion++) {
                    InputCriterion* copiedCriterion = new InputCriterion((*copyInputCriterion)->getName(), (*copyInputCriterion)->getOwner());
                    for(list<Pin*>::iterator copyInput = (*copyInputCriterion)->pins.begin(); copyInput != (*copyInputCriterion)->pins.end(); copyInput++) {
                        copiedCriterion->pins.push_back(currNode->getPinByName((*copyInput)->getName()));
                    }
                    (static_cast<Task*>(currNode))->inputCriteria.push_back(copiedCriterion);
                }

                // copy all output criteria and translate the pin pointers of the original to pin pointers of the instance
                for(list<OutputCriterion*>::iterator copyOutputCriterion = (*globalTask)->outputCriteria.begin(); copyOutputCriterion != (*globalTask)->outputCriteria.end(); copyOutputCriterion++) {
                    OutputCriterion* copiedCriterion = new OutputCriterion((*copyOutputCriterion)->getName(), (*copyOutputCriterion)->getOwner());
                    for(list<Pin*>::iterator copyOutput = (*copyOutputCriterion)->pins.begin(); copyOutput != (*copyOutputCriterion)->pins.end(); copyOutput++) {
                        copiedCriterion->pins.push_back(currNode->getPinByName((*copyOutput)->getName()));
                    }
                    for(list<InputCriterion*>::iterator relatedCriterion = (*copyOutputCriterion)->relatedInputCriteria.begin(); relatedCriterion!= (*copyOutputCriterion)->relatedInputCriteria.end(); relatedCriterion++) {
                        copiedCriterion->relatedInputCriteria.push_back((static_cast<Task*>(currNode))->getInputCriterionByName((*relatedCriterion)->getName()));
                    }
                    (static_cast<Task*>(currNode))->outputCriteria.push_back(copiedCriterion);
                }
            }
        }
        currProcess->addFCN(static_cast<SimpleTask*>(currNode));
        currNodeNameMap[attributes["name"]] = currNode;
    } else if (tag == "callToService") {
        currNode = new SimpleTask(attributes["name"], NCALLTOSERVICE, this, currProcess);

        // Find the correct service
        for(list<SimpleTask*>::iterator globalService = services.begin(); globalService != services.end(); globalService++) {

            // if it is the correct service
            if((*globalService)->getName() == attributes["service"]) {

                // copy the role requirements
                currNode->inherit(*globalService);

                // inherit process characteristics
                currNode->processCharacteristics |= (*globalService)->processCharacteristics;

                // copy all input pins
                for(list<Pin*>::iterator copyInput = (*globalService)->inputPins.begin(); copyInput != (*globalService)->inputPins.end(); copyInput++) {
                    Pin* newPin = new Pin((*copyInput)->getName(), currNode);
                    currNode->inputPins.push_back(newPin);
                    newPin->min = (*copyInput)->min;
                    newPin->max = (*copyInput)->max;
                }

                // copy all output pins
                for(list<Pin*>::iterator copyOutput = (*globalService)->outputPins.begin(); copyOutput != (*globalService)->outputPins.end(); copyOutput++) {
                    Pin* newPin = new Pin((*copyOutput)->getName(), currNode);
                    currNode->inputPins.push_back(newPin);
                    newPin->min = (*copyOutput)->min;
                    newPin->max = (*copyOutput)->max;
                }

                // copy all input criteria and translate the pin pointers of the original to pin pointers of the instance
                for(list<InputCriterion*>::iterator copyInputCriterion = (*globalService)->inputCriteria.begin(); copyInputCriterion != (*globalService)->inputCriteria.end(); copyInputCriterion++) {
                    InputCriterion* copiedCriterion = new InputCriterion((*copyInputCriterion)->getName(), (*copyInputCriterion)->getOwner());
                    for(list<Pin*>::iterator copyInput = (*copyInputCriterion)->pins.begin(); copyInput != (*copyInputCriterion)->pins.end(); copyInput++) {
                        copiedCriterion->pins.push_back(currNode->getPinByName((*copyInput)->getName()));
                    }
                    (static_cast<Task*>(currNode))->inputCriteria.push_back(copiedCriterion);
                }

                // copy all output criteria and translate the pin pointers of the original to pin pointers of the instance
                for(list<OutputCriterion*>::iterator copyOutputCriterion = (*globalService)->outputCriteria.begin(); copyOutputCriterion != (*globalService)->outputCriteria.end(); copyOutputCriterion++) {
                    OutputCriterion* copiedCriterion = new OutputCriterion((*copyOutputCriterion)->getName(), (*copyOutputCriterion)->getOwner());
                    for(list<Pin*>::iterator copyOutput = (*copyOutputCriterion)->pins.begin(); copyOutput != (*copyOutputCriterion)->pins.end(); copyOutput++) {
                        copiedCriterion->pins.push_back(currNode->getPinByName((*copyOutput)->getName()));
                    }
                    for(list<InputCriterion*>::iterator relatedCriterion = (*copyOutputCriterion)->relatedInputCriteria.begin(); relatedCriterion!= (*copyOutputCriterion)->relatedInputCriteria.end(); relatedCriterion++) {
                        copiedCriterion->relatedInputCriteria.push_back((static_cast<Task*>(currNode))->getInputCriterionByName((*relatedCriterion)->getName()));
                    }
                    (static_cast<Task*>(currNode))->outputCriteria.push_back(copiedCriterion);
                }
            }
        }
        currProcess->addFCN(static_cast<SimpleTask*>(currNode));
        currNodeNameMap[attributes["name"]] = currNode;
    } else if (tag == "callToProcess") {
        currNode = new SimpleTask(attributes["name"], NCALLTOPROCESS, this, currProcess);

        // Find the correct process
        for(list<Process*>::iterator globalProcess = processes.begin(); globalProcess != processes.end(); globalProcess++) {

            // if it is the correct process
            if((*globalProcess)->getName() == attributes["process"]) {

                // copy the role requirements
                currNode->inherit(*globalProcess);

                // inherit process characteristics
                currNode->processCharacteristics |= (*globalProcess)->processCharacteristics;

                // copy all input pins
                for(list<Pin*>::iterator copyInput = (*globalProcess)->inputPins.begin(); copyInput != (*globalProcess)->inputPins.end(); copyInput++) {
                    Pin* newPin = new Pin((*copyInput)->getName(), currNode);
                    currNode->inputPins.push_back(newPin);
                    newPin->min = (*copyInput)->min;
                    newPin->max = (*copyInput)->max;
                }

                // copy all output pins
                for(list<Pin*>::iterator copyOutput = (*globalProcess)->outputPins.begin(); copyOutput != (*globalProcess)->outputPins.end(); copyOutput++) {
                    Pin* newPin = new Pin((*copyOutput)->getName(), currNode);
                    currNode->inputPins.push_back(newPin);
                    newPin->min = (*copyOutput)->min;
                    newPin->max = (*copyOutput)->max;
                }

                // copy all input criteria and translate the pin pointers of the original to pin pointers of the instance
                for(list<InputCriterion*>::iterator copyInputCriterion = (*globalProcess)->inputCriteria.begin(); copyInputCriterion != (*globalProcess)->inputCriteria.end(); copyInputCriterion++) {
                    InputCriterion* copiedCriterion = new InputCriterion((*copyInputCriterion)->getName(), (*copyInputCriterion)->getOwner());
                    for(list<Pin*>::iterator copyInput = (*copyInputCriterion)->pins.begin(); copyInput != (*copyInputCriterion)->pins.end(); copyInput++) {
                        copiedCriterion->pins.push_back(currNode->getPinByName((*copyInput)->getName()));
                    }
                    (static_cast<Task*>(currNode))->inputCriteria.push_back(copiedCriterion);
                }

                // copy all output criteria and translate the pin pointers of the original to pin pointers of the instance
                for(list<OutputCriterion*>::iterator copyOutputCriterion = (*globalProcess)->outputCriteria.begin(); copyOutputCriterion != (*globalProcess)->outputCriteria.end(); copyOutputCriterion++) {
                    OutputCriterion* copiedCriterion = new OutputCriterion((*copyOutputCriterion)->getName(), (*copyOutputCriterion)->getOwner());
                    for(list<Pin*>::iterator copyOutput = (*copyOutputCriterion)->pins.begin(); copyOutput != (*copyOutputCriterion)->pins.end(); copyOutput++) {
                        copiedCriterion->pins.push_back(currNode->getPinByName((*copyOutput)->getName()));
                    }
                    for(list<InputCriterion*>::iterator relatedCriterion = (*copyOutputCriterion)->relatedInputCriteria.begin(); relatedCriterion!= (*copyOutputCriterion)->relatedInputCriteria.end(); relatedCriterion++) {
                        copiedCriterion->relatedInputCriteria.push_back((static_cast<Task*>(currNode))->getInputCriterionByName((*relatedCriterion)->getName()));
                    }
                    (static_cast<Task*>(currNode))->outputCriteria.push_back(copiedCriterion);
                }
            }
        }
        currProcess->addFCN(static_cast<SimpleTask*>(currNode));
        currNodeNameMap[attributes["name"]] = currNode;


    // process, loop and forloop are a specialty because they have an own flowcontent. Therefore
    // they will be treated as simple tasks in the parent process but added as processes to
    // the list of real processes
    } else if (tag == "process") {
        bool subprocess = false;
        if (currProcess != NULL) {
            currNode = new SimpleTask(attributes["name"], NPROCESS, this, currProcess);
            currProcess->addFCN(static_cast<Task*>(currNode));
            currNodeNameMap[attributes["name"]] = currNode;
            prozessSchachtelung = true;
            subprocess = true;
            currProcess->nodeNameMap = currNodeNameMap;
        }
        currProcess = new Process(attributes["name"], NPROCESS, this, currProcess);
        realProcesses.push_back(static_cast<Process*>(currProcess));
        currNodeNameMap = currProcess->nodeNameMap;
        currNodeNameMap[attributes["name"]] = currProcess;
        if (!subprocess) {
            currNode = currProcess;
        }
    } else if (tag == "loop") {
        currNode = new SimpleTask(attributes["name"], NLOOP, this, currProcess);
        currProcess->addFCN(static_cast<Task*>(currNode));
        currNodeNameMap[attributes["name"]] = currNode;
        prozessSchachtelung = true;
        currProcess->nodeNameMap = currNodeNameMap;
        currProcess = new Process(attributes["name"], NLOOP, this, currProcess);
        realProcesses.push_back(static_cast<Process*>(currProcess));
        currNodeNameMap = currProcess->nodeNameMap;
        currNodeNameMap[attributes["name"]] = currProcess;
    } else if (tag == "forLoop") {
        currNode = new SimpleTask(attributes["name"], NFORLOOP, this, currProcess);
        currProcess->addFCN(static_cast<Task*>(currNode));
        currNodeNameMap[attributes["name"]] = currNode;
        prozessSchachtelung = true;
        currProcess->nodeNameMap = currNodeNameMap;
        currProcess = new Process(attributes["name"], NFORLOOP, this, currProcess);
        realProcesses.push_back(static_cast<Process*>(currProcess));
        currNodeNameMap = currProcess->nodeNameMap;
        currNodeNameMap[attributes["name"]] = currProcess;

    // additional input and additional output:
    } else if (tag == "additionalInput") {
        Pin* newPin = new Pin(attributes["name"],currNode);
        currNode->inputPins.push_back(newPin);
        additionalInput = newPin;
    } else if (tag == "additionalOutput") {
        Pin* newPin = new Pin(attributes["name"],currNode);
        currNode->outputPins.push_back(newPin);
        additionalOutput = newPin;


    // input criteria and output criterion:
    } else if (tag == "inputCriterion") {
        Task* currTask = static_cast<Task*>(currNode);

        if (prozessSchachtelung) {
            if (currentOutputCriterion == NULL) {
                InputCriterion* newCriterion = new InputCriterion(attributes["name"], currNode);
                currTask->inputCriteria.push_back(newCriterion);
                currentInputCriterion = static_cast<Task*>(currNode)->inputCriteria.back();
                newCriterion = new InputCriterion(attributes["name"], currProcess);
                static_cast<Process*>(currProcess)->inputCriteria.push_back(newCriterion);
                currentProcessInputCriterion = static_cast<Process*>(currProcess)->inputCriteria.back();
            } else {
                currentOutputCriterion->relatedInputCriteria.push_back(currTask->getInputCriterionByName(attributes["name"]));
                currentProcessOutputCriterion->relatedInputCriteria.push_back(static_cast<Process*>(currProcess)->getInputCriterionByName(attributes["name"]));
                return;
            }
        } else {
            if (additionalInput != NULL) {
              // we are parsing an assignment: this <inputCriterion ... /> maps
              // a surrounding <additionalInput ... /> to an existing inputCriterion

              if (!additionalInput->free()) {
                // the current <additionalInput ...> already has a criterion which contains it
                currNode->processCharacteristics |= UML_PC(PC_OVERLAPPING);
              }

              // remember that the current <additionalInput... /> has this criterion for
              // checking of overlapping criteria
              InputCriterion* in = static_cast<InputCriterion*>(currTask->getInputCriterionByName(attributes["name"]));
              additionalInput->setCriterion(static_cast<PinCombination*>(in));

              // add this input to the input criterion
              in->pins.push_back(additionalInput);
            } else if (currentOutputCriterion == NULL) {
              InputCriterion* newCriterion = new InputCriterion(attributes["name"], currNode);
              currTask->inputCriteria.push_back(newCriterion);
              currentInputCriterion = static_cast<Task*>(currNode)->inputCriteria.back();
            } else {
              currentOutputCriterion->relatedInputCriteria.push_back(currTask->getInputCriterionByName(attributes["name"]));
              return;
            }
        }
    } else if (tag == "outputCriterion") {
        Task* currTask = static_cast<Task*>(currNode);

        if (prozessSchachtelung) {
            OutputCriterion* newCriterion = new OutputCriterion(attributes["name"], currNode);
            currTask->outputCriteria.push_back(newCriterion);
            currentOutputCriterion = currTask->outputCriteria.back();
            newCriterion = new OutputCriterion(attributes["name"], currProcess);
            static_cast<Process*>(currProcess)->outputCriteria.push_back(newCriterion);
            currentProcessOutputCriterion = static_cast<Process*>(currProcess)->outputCriteria.back();
        } else {
            if (additionalOutput != NULL) {
              // we are parsing an assignment: this <outputCriterion ... /> maps
              // a surrounding <additionalOutput ... /> to an existing outputCriterion
              if (!additionalOutput->free()) {
                currNode->processCharacteristics |= UML_PC(PC_OVERLAPPING);
              }

              // remember that the current <additionalOutput... /> has this criterion for
              // checking of overlapping criteria
              OutputCriterion* out = static_cast<OutputCriterion*>(currTask->getOutputCriterionByName(attributes["name"]));
              additionalOutput->setCriterion(static_cast<PinCombination*>(out));

              out->pins.push_back(additionalOutput);
            } else {
              OutputCriterion* newCriterion = new OutputCriterion(attributes["name"], currNode);
              currTask->outputCriteria.push_back(newCriterion);
              currentOutputCriterion = currTask->outputCriteria.back();
            }
        }


    // input branch and output branch:
    } else if (tag == "inputBranch") {
        PinCombination* newBranch = new PinCombination(attributes["name"],currNode);
        static_cast<ControlFlowNode*>(currNode)->inputBranches.push_back(newBranch);
        currentBranch = static_cast<ControlFlowNode*>(currNode)->inputBranches.back();
    } else if (tag == "outputBranch") {
        PinCombination* newBranch = new PinCombination(attributes["name"],currNode);
        static_cast<ControlFlowNode*>(currNode)->outputBranches.push_back(newBranch);
        currentBranch = static_cast<ControlFlowNode*>(currNode)->outputBranches.back();


    // input and output
    } else if (tag == "input") {
        if (prozessSchachtelung) {
            if (currentInputCriterion == NULL) {
                // instantiate a new pin
                Pin* newPin = new Pin(attributes["name"],currNode);
                currNode->inputPins.push_back(newPin);
                // set minimum and maximum range of pin multiplicities
                if (attributes.find("minimum") != attributes.end())
                  newPin->min = toInt(attributes["minimum"]);
                if (attributes.find("maximum") != attributes.end())
                  newPin->max = toInt(attributes["maximum"]);

                // if data is associated to this pin, then it is a data pin
                if (attributes.find("associatedData") != attributes.end())
                  newPin->isDataPin = true;

                newPin = new Pin(attributes["name"],currProcess);
                currProcess->inputPins.push_back(newPin);
                // set minimum and maximum range of pin multiplicities
                if (attributes.find("minimum") != attributes.end())
                  newPin->min = toInt(attributes["minimum"]);
                if (attributes.find("maximum") != attributes.end())
                  newPin->max = toInt(attributes["maximum"]);

                // if data is associated to this pin, then it is a data pin
                if (attributes.find("associatedData") != attributes.end())
                  newPin->isDataPin = true;
            } else {
                // assign a pin to the current input pinset
                Pin* existingPin = currNode->getPinByName(attributes["name"]);
                Pin* existingProcessPin = currProcess->getPinByName(attributes["name"]);
                if (!existingPin->free()) {
                  currNode->processCharacteristics |= UML_PC(PC_OVERLAPPING);
                }
                currentInputCriterion->pins.push_back(existingPin);
                currentProcessInputCriterion->pins.push_back(existingProcessPin);
                existingPin->setCriterion(currentInputCriterion);
                existingProcessPin->setCriterion(currentProcessInputCriterion);
            }
        } else {
            if (currentInputCriterion == NULL) {
                // instantiate a new pin
                Pin* newPin = new Pin(attributes["name"],currNode);
                currNode->inputPins.push_back(newPin);
                if (currentBranch != NULL) {
                    currentBranch->pins.push_back(newPin);
                }
                // set minimum and maximum range of pin multiplicities
                if (attributes.find("minimum") != attributes.end())
                  newPin->min = toInt(attributes["minimum"]);
                if (attributes.find("maximum") != attributes.end())
                  newPin->max = toInt(attributes["maximum"]);

                // if data is associated to this pin, then it is a data pin
                if (attributes.find("associatedData") != attributes.end())
                  newPin->isDataPin = true;
            } else {
                // assign a pin to the current input pinset
                Pin* existingPin = currNode->getPinByName(attributes["name"]);
                if (!existingPin->free()) {
                  currNode->processCharacteristics |= UML_PC(PC_OVERLAPPING);
                }
                currentInputCriterion->pins.push_back(existingPin);
                existingPin->setCriterion(currentInputCriterion);
            }
        }
    } else if (tag == "output") {
        if (prozessSchachtelung) {
            if (currentOutputCriterion == NULL) {
                Pin* newPin = new Pin(attributes["name"],currNode);
                currNode->outputPins.push_back(newPin);
                // set minimum and maximum range of pin multiplicities
                if (attributes.find("minimum") != attributes.end())
                  newPin->min = toInt(attributes["minimum"]);
                if (attributes.find("maximum") != attributes.end())
                  newPin->max = toInt(attributes["maximum"]);


                newPin = new Pin(attributes["name"],currProcess);
                currProcess->outputPins.push_back(newPin);
                // set minimum and maximum range of pin multiplicities
                if (attributes.find("minimum") != attributes.end())
                  newPin->min = toInt(attributes["minimum"]);
                if (attributes.find("maximum") != attributes.end())
                  newPin->max = toInt(attributes["maximum"]);

            } else {
                Pin* existingPin = currNode->getPinByName(attributes["name"]);
                Pin* existingProcessPin = currProcess->getPinByName(attributes["name"]);
                if (!existingPin->free()) {
                  currNode->processCharacteristics |= UML_PC(PC_OVERLAPPING);
                }
                currentOutputCriterion->pins.push_back(existingPin);
                currentProcessOutputCriterion->pins.push_back(existingProcessPin);
                existingPin->setCriterion(currentOutputCriterion);
                existingProcessPin->setCriterion(currentProcessOutputCriterion);
            }
        } else {
            if (currentOutputCriterion == NULL) {
                Pin* newPin = new Pin(attributes["name"],currNode);
                currNode->outputPins.push_back(newPin);
                if (currentBranch != NULL) {
                    currentBranch->pins.push_back(newPin);
                }
                // set minimum and maximum range of pin multiplicities
                if (attributes.find("minimum") != attributes.end())
                  newPin->min = toInt(attributes["minimum"]);
                if (attributes.find("maximum") != attributes.end())
                  newPin->max = toInt(attributes["maximum"]);

            } else {
                Pin* existingPin = currNode->getPinByName(attributes["name"]);
                if (!existingPin->free()) {
                  currNode->processCharacteristics |= UML_PC(PC_OVERLAPPING);
                }
                currentOutputCriterion->pins.push_back(existingPin);
                existingPin->setCriterion(currentOutputCriterion);
            }
        }

    // connection, source, target:
    } else if (tag == "connection") {
        currConnection = new FlowContentConnection(attributes["name"], this, currProcess);
        currProcess->addConnection(currConnection);
    } else if (tag == "source") {
        string sourceName;
        string sourceContact;

        if (attributes["node"] == "") {
            sourceName = currProcess->getName();
        } else {
            sourceName = attributes["node"];
        }

        if (attributes["contactPoint"] == "") {
            sourceContact = "default";
        } else {
            sourceContact = attributes["contactPoint"];
        }

        FlowContentNode* sourceNode = currNodeNameMap[sourceName];

        if (sourceNode == NULL) {
            // error handling to detect untranslated nodes
            ast_error("Failed to resolve '"+sourceName+"' in process "+currProcess->getName());
        } else {
            currConnection->setSource(sourceNode);
            currConnection->setInput(sourceNode->getPinByName(sourceContact));
        }
        Pin* connectedPin = sourceNode->getPinByName(sourceContact);
        connectedPin->setConnection(currConnection);

    } else if (tag == "target") {
        string targetName;
        string targetContact;
        if (attributes["node"] == "") {
            targetName = currProcess->getName();
        } else {
            targetName = attributes["node"];
        }

        if (attributes["contactPoint"] == "") {
            targetContact = "default";
        } else {
            targetContact = attributes["contactPoint"];
        }
        FlowContentNode* targetNode = currNodeNameMap[targetName];
        if (targetNode == NULL) {
            // error handling to detect untranslated nodes
            ast_error("Failed to resolve '"+targetName+"' in process "+currProcess->getName());
        } else {
            currConnection->setTarget(targetNode);
            currConnection->setOutput(targetNode->getPinByName(targetContact));
            Pin* connectedPin = targetNode->getPinByName(targetContact);
            connectedPin->setConnection(currConnection);
        }


    // role requirement
    } else if (tag == "roleRequirement") {
        if (prozessSchachtelung) {
            currNode->roles.insert(attributes["role"]);
            currProcess->roles.insert(attributes["role"]);
        } else {
            currNode->roles.insert(attributes["role"]);
        }

    } else {
        //ast_error("Unknown tag '"+tag+"'");
    }

#ifdef DEBUG
//    std::cerr << "translated node, now descend" << std::endl;
#endif

    // now proceed all children of this element
    for (list<ASTElement*>::iterator child = children.begin(); child != children.end(); child++) {
        (*child)->finishInternal(processes,tasks,services,realProcesses);

        // always inherit all characteristic flags of the current node
        if (currProcess != NULL && currNode != NULL)
          currProcess->processCharacteristics |= currNode->processCharacteristics;
    }

#ifdef DEBUG
//    std::cerr << "translated children, now wrap up '" << tag << "'" << std::endl;
#endif

    // Post operations adjusting global values
    if (tag == "process" || tag == "loop" || tag == "forloop") {
        currProcess->nodeNameMap = currNodeNameMap;
        Process* parentProcess = static_cast<Process*>(currProcess->getParent());
        if (parentProcess == NULL) {
            currProcess = NULL;
        } else {
            // inherit all process characteristics when ascending the process tree
            parentProcess->processCharacteristics |= currProcess->processCharacteristics;

            currNodeNameMap = parentProcess->nodeNameMap;
            currProcess = parentProcess;
        }
    } else if (tag == "inputBranch" || tag == "outputBranch") {
        currentBranch = NULL;
    } else if (tag == "inputCriterion") {
        if (prozessSchachtelung) {
            currentProcessInputCriterion = NULL;
            currentInputCriterion = NULL;
        } else {
            currentInputCriterion = NULL;
        }
    } else if (tag == "outputCriterion") {
        if (prozessSchachtelung) {
            currentProcessOutputCriterion = NULL;
            currentOutputCriterion = NULL;
        } else {
            currentOutputCriterion = NULL;
        }
    } else if (tag == "additionalInput") {
        additionalInput = NULL;
    } else if (tag == "additionalOutput") {
        additionalOutput = NULL;
    }
}

// generic function for literal
void ASTLiteral::prepareInstances(list<Process*>& processes, list<SimpleTask*>& tasks, list<SimpleTask*>& services) {
}

// generic function for literal
void ASTLiteral::finishInternal(list<Process*>& processes, list<SimpleTask*>& tasks, list<SimpleTask*>& services, list<Process*>& realProcesses) {
}

//  debug output for literal
void ASTLiteral::debugOutput(string delay) {
    std::cout << delay << content << std::endl;
}
