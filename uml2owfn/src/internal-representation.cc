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

#include <config.h>
#include <iostream>
#include <utility>
#include "debug.h"
#include "globals.h"
#include "helpers.h"        // helper functions (setUnion, setDifference, max, toString)

// include PN Api headers
#include "pnapi/pnapi.h"
#include "petrinet-workflow.h"

#include "internal-representation.h"
using std::pair;
using std::cerr;
using std::endl;
using std::string;

/**
 * return true iff type denotes a control-flow element of the process
 */
bool isControlFlowType(uml_elementType type) {
  switch (type) {
    case NSTARTNODE: return true;
    case NSTOPNODE: return true;
    case NENDNODE: return true;
    case NDECISION: return true;
    case NMERGE: return true;
    case NFORK: return true;
    case NJOIN: return true;
    case NLOOP: return true;
    case NFORLOOP: return true;
  }
  return false;
}



/******************************************************************************
 * Implementation of class functions
 *****************************************************************************/

// returns the name
string Pin::getName() {
    return name;
}

// returns if this pin is already part of a criterion
bool Pin::free() {
    return (owningCriterion==NULL);
}

// returns if this pin is already part of a criterion
void Pin::setCriterion(PinCombination* combination) {
    //cerr << this->getName() << ".setCriterion("<< combination->getName() << ")" << endl;
    owningCriterion = combination;
}

/// set the data type associated to this pin
void Pin::setAssociatedData(string associatedData) {
  this->associatedData = associatedData;
}

/// get the data type associated to this pin
string Pin::getAssociatedData() {
  return associatedData;
}

/// returns the petri net place of this pin
pnapi::Place* Pin::getPlace() {
    return pinPlace;
}

/// sets the petri net place of this pin
void Pin::setPlace(pnapi::Place* placeToSet) {
    pinPlace = placeToSet;
}

/// generates a debug output on the console for this Pin
void Pin::debugOutput(string delay) {
    std::cout << delay << "Pin: " << name << " | Owner: " << owningNode->getName() << "\n";
}

/// returns this pins connection
FlowContentConnection* Pin::getConnection() {
    return attachedConnection;
}

/// sets this pins connections
void Pin::setConnection(FlowContentConnection* attach) {
    attachedConnection = attach;
}

/// returns this pins owning node
FlowContentNode* Pin::getOwner() {
    return owningNode;
}

int pinIDs = 0;

/// translates this pin to its petri net pattern in the given petri net
void Pin::translateToNet(pnapi::ExtendedWorkflowNet* PN) {

  string name;

  if (options[O_DATAINTERFACES]) {

    // data interface naming: Data_DataTypeName_id
    if (this->isDataPin) {
      name = "Data_"+this->getAssociatedData() + "_" + toString(pinIDs);
    } else {
      if (isInput)
        name = "Control_"+getOwner()->getName() + "_" + toString(pinIDs);
      else
        name = "Control_"+getOwner()->getName() + "_" + toString(pinIDs);
    }
    pinIDs++;

  } else {
    // standard pin names: nodeType.name.direction.pinName
    if (isInput)
      name = getOwner()->typeString() + "." + getOwner()->getName()
              + ".input."
              + this->getName();
    else
      name = getOwner()->typeString() + "." + getOwner()->getName()
              + ".output."
              + this->getName();
  }

  pnapi::Place &p = PN->createPlace(name, pnapi::Node::INTERNAL);
  //p.roles().insert(roles.begin(),roles.end());
  this->setPlace(&p);
  PN->pinPlaces.insert(&p);

  switch (getOwner()->getType()) {
    case NENDNODE:
      PN->process_endNodes.insert(&p);
      break;
    case NSTOPNODE:
      PN->process_stopNodes.insert(&p);
      break;
    case NSTARTNODE:
      PN->process_startNodes.insert(&p);
      break;
    default:
      // store as internal place
      PN->process_internalPlaces.insert(&p);
      break;
  }
}


// constructor
Pin::Pin(string inputName, FlowContentNode* owner, bool isInput) {
    this->name = inputName;
    this->owningNode = owner;
    this->isInput = isInput;
    attachedConnection = NULL;
    owningCriterion = NULL;
    min = 1;  // standard production/consumption ranges
    max = 1;
    isDataPin = false;
}

// generic destructor
Pin::~Pin() {
}



// constructor
PinCombination::PinCombination(string inputName, FlowContentNode* owner) {
    name = inputName;
    owningNode = owner;
}

// returns the name
string PinCombination::getName() {
    return name;
}

// returns this pins owning node
FlowContentNode* PinCombination::getOwner() {
    return owningNode;
}

// constructor
void PinCombination::debugOutput(string delay) {
    std::cout << delay << name << ":\n";
    for(list<Pin*>::iterator pin = pins.begin(); pin != pins.end(); pin++) {
        (*pin)->debugOutput(delay + "  ");
    }
}

// constructor
FlowContentElement::FlowContentElement(ASTElement* link, FlowContentElement* parent) {
    ASTLink = link;
    parentFCE = parent;
}

// returns the xml element representing this FCE
ASTElement* FlowContentElement::getASTElement() {
    return ASTLink;
}

// returns the parent element for this FCE. If this FCE is global, the parent is NULL
FlowContentElement* FlowContentElement::getParent() {
    return parentFCE;
}

/*!
 * \result  returns the type of the flow content element according to the FCE enum in internal-representation.h
 */
uml_elementType FlowContentElement::getType() {
    return type;
}

/*!
 * \result  a string representing this FCE's enum type
 */
string FlowContentElement::typeString() {
    // TODO: placeholder
    switch(type) {
        case NSTARTNODE: return "startNode";
        case NENDNODE: return "endNode";
        case NSTOPNODE: return "stopNode";
        case NCALLTOSERVICE: return "callToService";
        case NCALLTOTASK: return "callToTask";
        case NCALLTOPROCESS: return "callToProcess";
        case NMERGE: return "merge";
        case NFORK: return "fork";
        case NJOIN: return "join";
        case NDECISION: return "decision";
        case NPROCESS: return "process";
        case NSERVICE: return "service";
        case NTASK: return "task";
        case NMAP: return "map";
        case NOBSERVER: return "observer";
        case NTIMER: return "timer";
        case NNOTIFICATIONBROADCASTER: return "notificationBroadcaster";
        case NNOTIFICATIONRECEIVER: return "notificationReceiver";
        case NLOOP: return "loop";
        case NFORLOOP: return "forLoop";
        //case ECONNECTION: return "edge";
    }
    return "";
}

/*
 * \result   pointer to the next process that contains this element
 */
Process* FlowContentElement::getEnclosingProcess() {
  FlowContentElement* parent = getParent();
  if (parent == NULL)
    return NULL;

  if (parent->getType() == NPROCESS)
    return static_cast<Process*>(parent);
  else
    return parent->getEnclosingProcess();
}

// constructor
FlowContentNode::FlowContentNode(string givenName, uml_elementType givenType, ASTElement* link, FlowContentElement* parent) : FlowContentElement(link,parent) {
    name = givenName;
    type = givenType;
    processCharacteristics = UML_PC(PC_NORMAL);
    parentFCE = parent;
}


// returns the name of the connection
string FlowContentConnection::getName() {
    return name;
}

// translates this Flow content element to its corresponding
// petri net pattern in the given petri net
void FlowContentConnection::translateToNet(pnapi::ExtendedWorkflowNet* PN) {
#ifdef DEBUG
    trace(TRACE_VERY_DEBUG, "  translating connection " + getName() + "\n");
#endif
    pnapi::Place *in = inputPin->getPlace();
    pnapi::Place *out = outputPin->getPlace();

    // merge places connected by this connection, preserve the name
    // of the non-control-flow place (merging preserves the name of the
    // first operand)
    pnapi::Place* result;
    if (!isControlFlowType(inputPin->getOwner()->getType())) {
      result = PN->mergePlaces(in, out);
    } else {
      result = PN->mergePlaces(out, in);
    }

    // update relation between model and net
    inputPin->setPlace(result);
    outputPin->setPlace(result);
}

// generates a debug output on the console for this Pin
void FlowContentConnection::debugOutput(string delay) {
    std::cout << delay << "Connection: " << name << "\n";
    std::cout << delay << " Source\n";
    inputPin->debugOutput(delay + "  ");
    std::cout << delay << " Target\n";
    outputPin->debugOutput(delay + "  ");
}

// sets the input pin of this connection
void FlowContentConnection::setInput(Pin* givenInput) {
    inputPin = givenInput;
}

// sets the output pin of this connection
void FlowContentConnection::setOutput(Pin* givenOutput) {
    outputPin = givenOutput;
}

// sets the source node of this connection
void FlowContentConnection::setSource(FlowContentNode* givenSource) {
    sourceNode = givenSource;
}

// sets the target node of this connection
void FlowContentConnection::setTarget(FlowContentNode* givenTarget) {
    targetNode = givenTarget;
}

// constructor
FlowContentConnection::FlowContentConnection(string givenName, ASTElement* link, FlowContentElement* parent) : FlowContentElement(link,parent) {
    name = givenName;
    type = ECONNECTION;   // every connection has type connection
    targetNode = NULL;
    sourceNode = NULL;
    outputPin = NULL;
    inputPin = NULL;
}

// returns the input pin of this connection
Pin* FlowContentConnection::getInput() {
    return inputPin;
}

// returns the output pin of this connection
Pin* FlowContentConnection::getOutput() {
    return outputPin;
}

// returns the source node of this connection
FlowContentNode* FlowContentConnection::getSource() {
    return sourceNode;
}

// returns the target node of this connection
FlowContentNode* FlowContentConnection::getTarget() {
    return targetNode;
}

// generic destructor
FlowContentConnection::~FlowContentConnection() {
}

// inherits the roles of the given node
void FlowContentNode::inherit(FlowContentNode* givenNode) {
  if (globals::deriveRolesToCut)
    roles = givenNode->roles; // inherit by overwriting
  else
    roles = setUnion(roles, givenNode->roles);
}

// returns the name of a flow content node
string FlowContentNode::getName() {
    return name;
}


// returns a pin of this FCE given its name
Pin* FlowContentNode::getPinByName(string find) {
    //
    // search in all input pins
    for(list<Pin*>::iterator currPin = inputPins.begin(); currPin != inputPins.end(); currPin++) {
        if ((*currPin)->getName() == find) {
            return *currPin;
        }
    }

    // search in all output pins
    for(list<Pin*>::iterator currPin = outputPins.begin(); currPin != outputPins.end(); currPin++) {
        if ((*currPin)->getName() == find) {
            return *currPin;
        }
    }

    // this should never be reached
    cerr << "Pin not found by Name, this should never happen.\n";
    cerr << "FCN = " << name << " | FindString = " << find << ".\n";
    assert(false);
    return new Pin("",NULL,true);
}

/*!
 * \result    number of incoming edges of this node
 */
int FlowContentNode::statistics_getInDegree () {
  Process* proc = getEnclosingProcess();
  if (proc == NULL)
    return 0;

  int connectedPins = 0;

  for (list<Pin*>::iterator pin = inputPins.begin(); pin != inputPins.end(); pin++) {
    FlowContentConnection* c = proc->getAdjacentConnection(*pin);
    if (c == NULL)
      continue;

    connectedPins++;
  }

  return connectedPins;
}

/*!
 * \result    number of incoming edges of this node
 */
int FlowContentNode::statistics_getOutDegree () {
  Process* proc = getEnclosingProcess();
  if (proc == NULL)
    return 0;

  int connectedPins = 0;

  for (list<Pin*>::iterator pin = outputPins.begin(); pin != outputPins.end(); pin++) {
    FlowContentConnection* c = proc->getAdjacentConnection(*pin);
    if (c == NULL)
      continue;

    connectedPins++;
  }

  return connectedPins;
}


// constructor
Process::~Process() {
}

// returns true if this process has an empty flow content
bool Process::empty() {
    return flowContentNodes.empty();
}

// spreads the roles of this process's tasks to this process's control flow nodes
void Process::spreadRoles() {
    bool changed = false;

    do {
        changed = false;
        FlowContentNode* inheritanceNode;

        // iterate over all flow content nodes of this process
        // as long as something changes
        for (list<FlowContentNode*>::iterator fce = flowContentNodes.begin(); fce != flowContentNodes.end(); fce++) {

            // if this fce already has roles ..
            if (!(*fce)->roles.empty()) {
                continue;

            // else if this fce is an element that first should start to follow its outputs
            } else if ((*fce)->getType() == NSTARTNODE || (*fce)->getType() == NJOIN || (*fce)->getType() == NMERGE) {

                // try to inherit roles by following the outputs
                FlowContentConnection* inheritanceConnection;
                for (list<Pin*>::iterator pin = (*fce)->outputPins.begin(); pin != (*fce)->outputPins.end(); pin++) {
                    inheritanceConnection = (*pin)->getConnection();
                    if (inheritanceConnection != NULL) {
                        inheritanceNode = inheritanceConnection->getTarget();
                        if (!inheritanceNode->roles.empty()) {
                            (*fce)->inherit(inheritanceNode);
                            changed = true;
                            break;
                        }
                    }
                }

                // if no role was inherited after trying all outputs, try the inputs
                if ((*fce)->roles.empty()) {
                    for (list<Pin*>::iterator pin = (*fce)->inputPins.begin(); pin != (*fce)->inputPins.end(); pin++) {
                        inheritanceConnection = (*pin)->getConnection();
                        if (inheritanceConnection != NULL) {
                            inheritanceNode = inheritanceConnection->getSource();
                            if (!inheritanceNode->roles.empty()) {
                                (*fce)->inherit(inheritanceNode);
                                changed = true;
                                break;
                            }
                        }
                    }
                }


            // else if this fce is an element that first should start to follow its inputs
            } else {
                // try to inherit roles by following the inputs
                FlowContentConnection* inheritanceConnection;
                for (list<Pin*>::iterator pin = (*fce)->inputPins.begin(); pin != (*fce)->inputPins.end(); pin++) {
                    inheritanceConnection = (*pin)->getConnection();
                    if (inheritanceConnection != NULL) {
                        inheritanceNode = inheritanceConnection->getSource();
                        if (!inheritanceNode->roles.empty()) {
                            (*fce)->inherit(inheritanceNode);
                            changed = true;
                            break;
                        }
                    }
                }

                // if no role was inherited after trying all inputs, try the outputs
                if ((*fce)->roles.empty()) {
                    for (list<Pin*>::iterator pin = (*fce)->outputPins.begin(); pin != (*fce)->outputPins.end(); pin++) {
                        inheritanceConnection = (*pin)->getConnection();
                        if (inheritanceConnection != NULL) {
                            inheritanceNode = inheritanceConnection->getTarget();
                            if (!inheritanceNode->roles.empty()) {
                                (*fce)->inherit(inheritanceNode);
                                changed = true;
                                break;
                            }
                        }
                    }
                }
            }
        }
    } while (changed);

    for (list<FlowContentNode*>::iterator fce = flowContentNodes.begin(); fce != flowContentNodes.end(); fce++) {
        if ((*fce)->getType() == NSTARTNODE) {
            if (globals::deriveRolesToCut)
              globals::keepRolesExact.insert((*fce)->roles);
            else
              // TODO: check whether union of roles make sense
              globals::keepRoles = setUnion(globals::keepRoles,(*fce)->roles);

            // global processes mostly do not posess any roles, which can result
            // in the processes transitions and places being deletet in a role
            // dependant cut. To prevent this some non deletable roles are added to
            // the processes original ones
            if (!globals::deriveRolesToCut)
              roles = setUnion(globals::keepRoles,roles);
        }
    }
}

// translates this Flow content element to its corresponding
// petri net pattern in the given petri net
void Process::translateToNet(pnapi::ExtendedWorkflowNet* PN) {

#ifdef DEBUG
    trace(TRACE_INFORMATION, "translating process: " + name +"\n");
#endif

    startingTransitions.clear();
    map<InputCriterion*, pnapi::Node*> processCentralNodes;
    map<Pin*, pnapi::Node*> trueInterfacePlaces;

    // create the overall starting place for this process
    pnapi::Place& alpha = PN->createPlace("process.alpha", pnapi::Node::INTERNAL, 1);
    //alpha.roles().insert(roles.begin(),roles.end());

    // create all input places of the process
    for (list<Pin*>::iterator input = inputPins.begin(); input != inputPins.end(); input++) {
        pnapi::Place& p = PN->createPlace(("process."+ name + ".input." + (*input)->getName()), pnapi::Node::INTERNAL);
        //p.roles().insert(roles.begin(),roles.end());
        (*input)->setPlace(&p);
        PN->pinPlaces.insert(&p);

        pnapi::Place& p2 = PN->createPlace(("process."+ name + ".trueInput." + (*input)->getName()), pnapi::Node::INPUT);
        //p2.roles().insert(roles.begin(),roles.end());
        trueInterfacePlaces [(*input)] = &p2;
    }

    // translate all of the input criteria
    for (list<InputCriterion*>::iterator inputCriterion = inputCriteria.begin(); inputCriterion != inputCriteria.end(); inputCriterion++)
    {
        pnapi::Transition& t = PN->createTransition("process."+ name + ".inputCriterion." + (*inputCriterion)->getName());
        //t.roles().insert(roles.begin(),roles.end());
	t.addRoles(roles.begin(),roles.end());
        startingTransitions.push_back(&t);
        PN->createArc(alpha,t);
        PN->process_inputPinSets.insert(&t);

        pnapi::Place& p = PN->createPlace(("process."+ name + ".inputCriterion." + (*inputCriterion)->getName() + ".used"), pnapi::Node::INTERNAL);
        //p.roles().insert(roles.begin(),roles.end());
        processCentralNodes[(*inputCriterion)] = &p;
        PN->createArc(t,p);
        PN->process_inputCriterion_used.insert(&p);

        for (list<Pin*>::iterator input = (*inputCriterion)->pins.begin(); input != (*inputCriterion)->pins.end(); input++) {
            PN->createArc(t,*(*input)->getPlace());
            PN->createArc(*trueInterfacePlaces[(*input)],t);
        }
    }

    // create all output places of the process
    for (list<Pin*>::iterator output = outputPins.begin(); output != outputPins.end(); output++) {
        pnapi::Place& p = PN->createPlace(("process."+ name + ".output." + (*output)->getName()), pnapi::Node::INTERNAL);
        //p.roles().insert(roles.begin(),roles.end());
        (*output)->setPlace(&p);
        PN->pinPlaces.insert(&p);

        pnapi::Place& p2 = PN->createPlace(("process."+ name + ".trueOutput." + (*output)->getName()), pnapi::Node::OUTPUT);
        //p2.roles().insert(roles.begin(),roles.end());
        trueInterfacePlaces [(*output)] = &p2;
    }

    // translate all of the output criteria
    for (list<OutputCriterion*>::iterator outputCriterion = outputCriteria.begin(); outputCriterion != outputCriteria.end(); outputCriterion++) {
        pnapi::Transition& t = PN->createTransition("process."+ name + ".outputCriterion." + (*outputCriterion)->getName());
        //t.roles().insert(roles.begin(),roles.end());
	t.addRoles(roles.begin(),roles.end());
        PN->process_outputPinSets.insert(&t);

        if ((*outputCriterion)->relatedInputCriteria.begin() == (*outputCriterion)->relatedInputCriteria.end()) {
            PN->createArc(*processCentralNodes[(*(inputCriteria.begin()))],t);
        } else {
            for (list<InputCriterion*>::iterator relatedInputCriterion = (*outputCriterion)->relatedInputCriteria.begin(); relatedInputCriterion != (*outputCriterion)->relatedInputCriteria.end(); relatedInputCriterion++) {
                PN->createArc(*processCentralNodes[*relatedInputCriterion],t);
            }
        }

        for (list<Pin*>::iterator output = (*outputCriterion)->pins.begin(); output != (*outputCriterion)->pins.end(); output++) {
            PN->createArc(*(*output)->getPlace(),t);
            PN->createArc(t,*trueInterfacePlaces[(*output)]);
        }
    }

    // iterate over the flow content of the process, translating every FCN
    // by its types pattern
    for(list<FlowContentNode*>::iterator flowContentNode = flowContentNodes.begin(); flowContentNode != flowContentNodes.end(); flowContentNode++) {
        if ((*flowContentNode)->getType() == NSTARTNODE || (*flowContentNode)->getType() == NENDNODE || (*flowContentNode)->getType() == NSTOPNODE) {
            (static_cast<AtomicCFN*>(*flowContentNode))->translateToNet(PN);
        } else if ((*flowContentNode)->getType() == NJOIN) {
            (static_cast<Join*>(*flowContentNode))->translateToNet(PN);
        } else if ((*flowContentNode)->getType() == NMERGE) {
            (static_cast<Merge*>(*flowContentNode))->translateToNet(PN);
        } else if ((*flowContentNode)->getType() == NDECISION) {
            (static_cast<Decision*>(*flowContentNode))->translateToNet(PN);
        } else if ((*flowContentNode)->getType() == NFORK) {
            (static_cast<Fork*>(*flowContentNode))->translateToNet(PN);
        } else {
            (static_cast<SimpleTask*>(*flowContentNode))->translateToNet(PN);
        }
    }

    // now that all FCN of the process have been translated, they need to be
    // connected
    for(list<FlowContentConnection*>::iterator flowContentConnection = flowContentConnections.begin(); flowContentConnection != flowContentConnections.end(); flowContentConnection++) {
        (*flowContentConnection)->translateToNet(PN);
    }
}


// adds a node to this process's flow content
void Process::addFCN(FlowContentNode* toAdd) {
    flowContentNodes.push_back(toAdd);
}

// adds a connection to this process's flow content
void Process::addConnection(FlowContentConnection* toAdd) {
    flowContentConnections.push_back(toAdd);
}

/*
 * \result  the connection that is adjacent to the given pin (if any)
 */
FlowContentConnection* Process::getAdjacentConnection(Pin *pin) const {
  for(list<FlowContentConnection*>::const_iterator c = flowContentConnections.begin();
      c != flowContentConnections.end(); c++) {
    // this connection either begins or ends at the given pin
    if ((*c)->getInput() == pin || (*c)->getOutput() == pin)
      return *c;
  }
  // no connection found
  return NULL;
}

// generates a debug output on the console for this Pin
void Process::debugOutput(string delay) {

    std::cout << delay << "Process: " << name << "\n";

    std::cout << delay << " Roles\n";
    for(set<string>::iterator role = roles.begin(); role != roles.end(); role++) {
        std::cout << delay + "  Role: " + (*role);
    }

    std::cout << delay << " Inputs\n";
    for(list<Pin*>::iterator inputPin = inputPins.begin(); inputPin != inputPins.end(); inputPin++) {
        (*inputPin)->debugOutput(delay + "  ");
    }

    std::cout << delay << " Outputs\n";
    for(list<Pin*>::iterator outputPin = outputPins.begin(); outputPin != outputPins.end(); outputPin++) {
        (*outputPin)->debugOutput(delay + "  ");
    }

    std::cout << delay << " InputCriteria\n";
    for(list<InputCriterion*>::iterator inputCriterion = inputCriteria.begin(); inputCriterion != inputCriteria.end(); inputCriterion++) {
        (*inputCriterion)->debugOutput(delay + "  ");
    }

    std::cout << delay << " OutputCriteria\n";
    for(list<OutputCriterion*>::iterator outputCriterion = outputCriteria.begin(); outputCriterion != outputCriteria.end(); outputCriterion++) {
        (*outputCriterion)->debugOutput(delay + "  ");
    }

    for(list<FlowContentNode*>::iterator child = flowContentNodes.begin(); child != flowContentNodes.end(); child++) {
        if ((*child)->getType() == NSTARTNODE || (*child)->getType() == NENDNODE || (*child)->getType() == NSTOPNODE) {
            (static_cast<AtomicCFN*>(*child))->debugOutput(delay+"    ");
        } else if ((*child)->getType() == NJOIN ||(*child)->getType() == NFORK ||(*child)->getType() == NDECISION ||(*child)->getType() == NMERGE) {
            (static_cast<ControlFlowNode*>(*child))->debugOutput(delay+"    ");
        } else {
            (static_cast<SimpleTask*>(*child))->debugOutput(delay+"    ");
        }
    }
}


/*!
 * \brief   traverse the model and update the model characteristics that
 *          could not be set earler
 */
void Process::updateCharacteristics() {

  for (list<FlowContentNode*>::const_iterator child = flowContentNodes.begin(); child != flowContentNodes.end(); child++) {
    FlowContentNode* n = (*child);

    // joins have pin multiplicities if 'join' tokens with data
    if (n->getType() == NJOIN) {
      for (list<Pin*>::const_iterator pin = n->inputPins.begin(); pin != n->inputPins.end(); pin++) {
        if ((*pin)->isDataPin) {
          n->processCharacteristics |= UML_PC(PC_PIN_MULTI);
        }
      }
    }

    // the process inherits all node characteristics
    processCharacteristics |= n->processCharacteristics;
  }

  for (list<FlowContentConnection*>::const_iterator edge = flowContentConnections.begin(); edge != flowContentConnections.end(); edge++) {
    FlowContentConnection* c = (*edge);

    // process has non-normal pin-multiplicities
    if (   c->getInput()->min != 1 || c->getOutput()->min != 1
        || c->getInput()->max != 1 || c->getOutput()->max != 1)
    {
      // extend characteristics for filtering
      processCharacteristics |= UML_PC(PC_PIN_MULTI);
    }

    // pin multiplicities at source and target do not match
    if (   c->getInput()->min != c->getOutput()->min
        || c->getInput()->max != c->getOutput()->max)
    {
      // extend characteristics for filtering
      processCharacteristics |= UML_PC(PC_PIN_MULTI_NONMATCH);
    }
  }
}

/*!
 * \brief   calculate and return a statistics of static properties of this process
*/
UmlProcessStatistics Process::getStatistics() const {
  UmlProcessStatistics stat = UmlProcessStatistics();

  stat.processCharacteristics = processCharacteristics;
  stat.numNodes = flowContentNodes.size();
  stat.numEdges = flowContentConnections.size();

  int countedNodes = 0;

  for (list<FlowContentNode*>::const_iterator child = flowContentNodes.begin(); child != flowContentNodes.end(); child++) {

    /*
    if ((*child)->getType() == NSTARTNODE ||
        (*child)->getType() == NENDNODE ||
        (*child)->getType() == NSTOPNODE) {
      continue;
    }
     */

    countedNodes++;

    stat.avgInDegree += (*child)->statistics_getInDegree();
    stat.avgOutDegree += (*child)->statistics_getOutDegree();

    if (stat.maxInDegree < (*child)->statistics_getInDegree())
    	stat.maxInDegree = (*child)->statistics_getInDegree();
    if (stat.maxOutDegree < (*child)->statistics_getOutDegree())
    	stat.maxOutDegree = (*child)->statistics_getOutDegree();
  }

  stat.avgInDegree = stat.avgInDegree / (float)countedNodes;
  stat.avgOutDegree = stat.avgOutDegree / (float)countedNodes;

  return stat;
}

set<string> Process::getNonStartNodeRoles() const {

  for (list<FlowContentNode*>::const_iterator fce = flowContentNodes.begin(); fce != flowContentNodes.end(); fce++) {
      if ((*fce)->getType() == NSTARTNODE) {
          //globals::keepRoles = setUnion(globals::keepRoles,(*fce)->roles);
      }
  }
}

/*!
 * \brief   overwrite name of the process, may only be called after the
 *          no more reference resolving by name will happen
 */
void Process::setName(string newName) {
  name = newName;
}


// returns an input criterion of this Task given the name
InputCriterion* Task::getInputCriterionByName(string find) {

    // search in all input criteria
    for(list<InputCriterion*>::iterator inputCriterion = inputCriteria.begin(); inputCriterion != inputCriteria.end(); inputCriterion++) {
        if ((*inputCriterion)->getName() == find) {
            return *inputCriterion;
        }
    }

    // this should never be reached
    cerr << "Input criterion not found by Name, this should never happen.\n";
    cerr << "FCN = " << name << " | FindString = " << find << ".\n";
    assert(false);
    return new InputCriterion("",NULL);
}

// returns an output criterion of this Task given the name
OutputCriterion* Task::getOutputCriterionByName(string find) {

    // search in all output criteria
    for(list<OutputCriterion*>::iterator outputCriterion = outputCriteria.begin(); outputCriterion != outputCriteria.end(); outputCriterion++) {
        if ((*outputCriterion)->getName() == find) {
            return *outputCriterion;
        }
    }

    // this should never be reached
    cerr << "Output criterion not found by Name, this should never happen.\n";
    cerr << "FCN = " << name << " | FindString = " << find << ".\n";
    assert(false);
    return new OutputCriterion("",NULL);
}



// generic destructor
SimpleTask::~SimpleTask() {
}

// translates this Flow content element to its corresponding
// petri net pattern in the given petri net
void SimpleTask::translateToNet(pnapi::ExtendedWorkflowNet* PN) {
    //
    //Create pointer to the current place and the current transition

    map<InputCriterion*, pnapi::Node*> processCentralNodes;

    // create all input places of the task
    for (list<Pin*>::iterator input = inputPins.begin(); input != inputPins.end(); input++) {
      (*input)->translateToNet(PN);
    }

    // translate all of the input criteria
    for (list<InputCriterion*>::iterator inputCriterion = inputCriteria.begin(); inputCriterion != inputCriteria.end(); inputCriterion++)
    {
        pnapi::Transition &t = PN->createTransition(typeString() + "." + name + ".inputCriterion." + (*inputCriterion)->getName());
        //t.roles().insert(roles.begin(),roles.end());
        t.addRoles(roles.begin(),roles.end());
        pnapi::Place &p = PN->createPlace((typeString() + "." + name + ".inputCriterion." + (*inputCriterion)->getName() + ".used"), pnapi::Node::INTERNAL);
        //p.roles().insert(roles.begin(),roles.end());
        processCentralNodes[(*inputCriterion)] = &p;
        PN->createArc(t,p);
        PN->process_internalPlaces.insert(&p);

        for (list<Pin*>::iterator input = (*inputCriterion)->pins.begin(); input != (*inputCriterion)->pins.end(); input++) {
            PN->createArc(*(*input)->getPlace(),t);
        }
    }

    // create all output places of the task
    for (list<Pin*>::iterator output = outputPins.begin(); output != outputPins.end(); output++) {
      (*output)->translateToNet(PN);
    }

    // translate all of the output criteria
    for (list<OutputCriterion*>::iterator outputCriterion = outputCriteria.begin(); outputCriterion != outputCriteria.end(); outputCriterion++) {
        pnapi::Transition &t = PN->createTransition(typeString() + "." + name + ".outputCriterion." + (*outputCriterion)->getName());
        //t.roles().insert(roles.begin(),roles.end());
        t.addRoles(roles.begin(),roles.end());

        if ((*outputCriterion)->relatedInputCriteria.begin() == (*outputCriterion)->relatedInputCriteria.end()) {
            PN->createArc(*processCentralNodes[(*(inputCriteria.begin()))],t);
        } else {
            for (list<InputCriterion*>::iterator relatedInputCriterion = (*outputCriterion)->relatedInputCriteria.begin(); relatedInputCriterion != (*outputCriterion)->relatedInputCriteria.end(); relatedInputCriterion++) {
                PN->createArc(*processCentralNodes[*relatedInputCriterion],t);
            }
        }

        for (list<Pin*>::iterator output = (*outputCriterion)->pins.begin(); output != (*outputCriterion)->pins.end(); output++) {
            PN->createArc(t,*(*output)->getPlace());
        }
    }
}

// generates a debug output on the console for this Pin
void SimpleTask::debugOutput(string delay) {
    std::cout << delay << typeString() << " | " << name << "\n";

    std::cout << delay << " Roles\n";
    for(set<string>::iterator role = roles.begin(); role != roles.end(); role++) {
        std::cout << delay + "  Role: " + (*role) + "\n";
    }

    std::cout << delay << " Inputs\n";
    for(list<Pin*>::iterator inputPin = inputPins.begin(); inputPin != inputPins.end(); inputPin++) {
        (*inputPin)->debugOutput(delay + "  ");
    }

    std::cout << delay << " Outputs\n";
    for(list<Pin*>::iterator outputPin = outputPins.begin(); outputPin != outputPins.end(); outputPin++) {
        (*outputPin)->debugOutput(delay + "  ");
    }

    std::cout << delay << " InputCriteria\n";
    for(list<InputCriterion*>::iterator inputCriterion = inputCriteria.begin(); inputCriterion != inputCriteria.end(); inputCriterion++) {
        (*inputCriterion)->debugOutput(delay + "  ");
    }

    std::cout << delay << " OutputCriteria\n";
    for(list<OutputCriterion*>::iterator outputCriterion = outputCriteria.begin(); outputCriterion != outputCriteria.end(); outputCriterion++) {
        (*outputCriterion)->debugOutput(delay + "  ");
    }
}



// generic destructor
ControlFlowNode::~ControlFlowNode() {
}

// generates a debug output on the console for this Pin
void ControlFlowNode::debugOutput(string delay) {

    std::cout << delay << typeString() <<  " | " << name << "\n";

    std::cout << delay << " Roles\n";
    for(set<string>::iterator role = roles.begin(); role != roles.end(); role++) {
        std::cout << delay + "  Role: " + (*role) + "\n";
    }

    std::cout << delay << " Inputs\n";
    for(list<Pin*>::iterator inputPin = inputPins.begin(); inputPin != inputPins.end(); inputPin++) {
        (*inputPin)->debugOutput(delay + "  ");
    }

    std::cout << delay << " Outputs\n";
    for(list<Pin*>::iterator outputPin = outputPins.begin(); outputPin != outputPins.end(); outputPin++) {
        (*outputPin)->debugOutput(delay + "  ");
    }

    std::cout << delay << " InputBranches\n";
    for(list<PinCombination*>::iterator inputBranch = inputBranches.begin(); inputBranch != inputBranches.end(); inputBranch++) {
        (*inputBranch)->debugOutput(delay + "  ");
    }

    std::cout << delay << " OutputBranches\n";
    for(list<PinCombination*>::iterator outputBranch = outputBranches.begin(); outputBranch != outputBranches.end(); outputBranch++) {
        (*outputBranch)->debugOutput(delay + "  ");
    }
}


// generic destructor
Decision::~Decision() {
}

// translates this Flow content element to its corresponding
// petri net pattern in the given petri net
void Decision::translateToNet(pnapi::ExtendedWorkflowNet* PN) {

    //Create pointer to the current place and the current transition
    pnapi::Place& centralNode = PN->createPlace((typeString() + "." + name + ".activated"), pnapi::Node::INTERNAL);
    //centralNode.roles().insert(roles.begin(),roles.end());
    PN->process_internalPlaces.insert(&centralNode);

    // create all input places of the process
    for (list<Pin*>::iterator input = inputPins.begin(); input != inputPins.end(); input++) {
        (*input)->translateToNet(PN);
    }

    // create all output places of the process
    for (list<Pin*>::iterator output = outputPins.begin(); output != outputPins.end(); output++) {
        (*output)->translateToNet(PN);
    }

    // translate all of the input branch
    for (list<PinCombination*>::iterator inputBranch = inputBranches.begin(); inputBranch != inputBranches.end(); inputBranch++)
    {
        pnapi::Transition &t = PN->createTransition(typeString() + "." + name + ".activate." + (*inputBranch)->getName());
        //t.roles().insert(roles.begin(),roles.end());
	t.addRoles(roles.begin(),roles.end());
        PN->createArc(t,centralNode);

        for (list<Pin*>::iterator input = (*inputBranch)->pins.begin(); input != (*inputBranch)->pins.end(); input++) {
            PN->createArc(*(*input)->getPlace(),t);
        }
    }

    // use different pattern depending on if this is an inclusive decision
    if(inclusive) {

        set<PinCombination*> turnToSet;

        // translate all of the output branch
        for (list<PinCombination*>::iterator outputBranch = outputBranches.begin(); outputBranch != outputBranches.end(); outputBranch++)
        {
            turnToSet.insert(*outputBranch);
        }

        set<set<PinCombination*> > powerSet;
        inclusiveDecisionPatternRecursively(centralNode, turnToSet, "0", PN, powerSet);

    } else {
        // translate all of the output branch
        for (list<PinCombination*>::iterator outputBranch = outputBranches.begin(); outputBranch != outputBranches.end(); outputBranch++)
        {
            pnapi::Transition &t = PN->createTransition(typeString() + "." + name + ".fire." + (*outputBranch)->getName());
            //t.roles().insert(roles.begin(),roles.end());
	    t.addRoles(roles.begin(),roles.end());
            PN->createArc(centralNode,t);

            for (list<Pin*>::iterator output = (*outputBranch)->pins.begin(); output != (*outputBranch)->pins.end(); output++) {
                PN->createArc(t,*(*output)->getPlace());
            }
        }
    }
}

void Decision::inclusiveDecisionPatternRecursively(pnapi::Place & centralNode, set<PinCombination*>& currentOutputBranches, string number, pnapi::ExtendedWorkflowNet* PN, set<set<PinCombination*> >& powerSet)
{
    if (!powerSet.insert(currentOutputBranches).second)
    {
        return;
    }

    pnapi::Transition& t = PN->createTransition(("decision." + name + ".fire." + number));
    //t.roles().insert(roles.begin(),roles.end());
    t.addRoles(roles.begin(),roles.end());
    PN->createArc(centralNode, t);

    for (set<PinCombination*>::iterator outputBranch = currentOutputBranches.begin(); outputBranch != currentOutputBranches.end(); outputBranch++)
    {
        for (list<Pin*>::iterator output = (*outputBranch)->pins.begin(); output != (*outputBranch)->pins.end(); output++)
        {
            PN->createArc(t,*(*output)->getPlace());
        }
    }
    int i = 0;

    if (currentOutputBranches.size() == 1)
    {
        return;
    }

    for (set<PinCombination*>::iterator outputBranch = currentOutputBranches.begin(); outputBranch != currentOutputBranches.end(); outputBranch++)
    {
        set<PinCombination*> newSet = set<PinCombination*>(currentOutputBranches);
        newSet.erase(*outputBranch);
        inclusiveDecisionPatternRecursively(centralNode, newSet, (number+"."+toString(i)), PN, powerSet);
        i++;
    }
}

/*!
 * \result    number of outgoing edges of this node
 */
int Decision::statistics_getOutDegree () {

  int connectedPins = FlowContentNode::statistics_getOutDegree();
  // an explicit decision only activates one of its output branches
  if (inclusive)
    return connectedPins;
  else
    return connectedPins/outputBranches.size();
}


// generic destructor
Fork::~Fork() {
}

// translates this Flow content element to its corresponding
// petri net pattern in the given petri net
void Fork::translateToNet(pnapi::ExtendedWorkflowNet* PN) {

    //Create pointer to the current place and the current transition
    list<pnapi::Place*> centralPlaces;

    // create all output places of the process
    for (list<Pin*>::iterator output = outputPins.begin(); output != outputPins.end(); output++) {
        (*output)->translateToNet(PN);
    }

    // translate all of the output branches
    for (list<PinCombination*>::iterator outputBranch = outputBranches.begin(); outputBranch != outputBranches.end(); outputBranch++)
    {
        pnapi::Place &p = PN->createPlace((typeString() + "." + name + ".activated." + (*outputBranch)->getName()),pnapi::Node::INTERNAL);
        //p.roles().insert(roles.begin(),roles.end());
        centralPlaces.push_back(&p);
        PN->process_internalPlaces.insert(&p);

        pnapi::Transition &t = PN->createTransition(typeString() + "." + name + ".fire." + (*outputBranch)->getName());
        //t.roles().insert(roles.begin(),roles.end());
	t.addRoles(roles.begin(),roles.end());
        PN->createArc(p,t);

        for (list<Pin*>::iterator output = (*outputBranch)->pins.begin(); output != (*outputBranch)->pins.end(); output++) {
            PN->createArc(t,*(*output)->getPlace());
        }
    }

    // create all input places of the process
    for (list<Pin*>::iterator input = inputPins.begin(); input != inputPins.end(); input++) {
        (*input)->translateToNet(PN);
    }

    // translate all of the input branches
    for (list<PinCombination*>::iterator inputBranch = inputBranches.begin(); inputBranch != inputBranches.end(); inputBranch++)
    {
        pnapi::Transition &t = PN->createTransition(typeString() + "." + name + ".activate." + (*inputBranch)->getName());
        //t.roles().insert(roles.begin(),roles.end());
	t.addRoles(roles.begin(),roles.end());

        for (list<Pin*>::iterator input = (*inputBranch)->pins.begin(); input != (*inputBranch)->pins.end(); input++) {
            PN->createArc(*(*input)->getPlace(),t);
        }

        for (list<pnapi::Place*>::iterator central = centralPlaces.begin(); central != centralPlaces.end(); central++) {
            PN->createArc(t, **central);
        }
    }
}

// generic destructor
Join::~Join() {
}

// translates this Flow content element to its corresponding
// petri net pattern in the given petri net
void Join::translateToNet(pnapi::ExtendedWorkflowNet* PN) {

    //Create pointer to the current place and the current transition
    list<pnapi::Place*> centralPlaces;

    // create all input places of the process
    for (list<Pin*>::iterator input = inputPins.begin(); input != inputPins.end(); input++) {
        (*input)->translateToNet(PN);
    }

    // translate all of the input branches
    for (list<PinCombination*>::iterator inputBranch = inputBranches.begin(); inputBranch != inputBranches.end(); inputBranch++)
    {
        pnapi::Place &p = PN->createPlace((typeString() + "." + name + ".activated." + (*inputBranch)->getName()),pnapi::Node::INTERNAL);
        //p.roles().insert(roles.begin(),roles.end());
        centralPlaces.push_back(&p);
        PN->process_internalPlaces.insert(&p);

        pnapi::Transition &t = PN->createTransition(typeString() + "." + name + ".activate." + (*inputBranch)->getName());
        //t.roles().insert(roles.begin(),roles.end());
	t.addRoles(roles.begin(),roles.end());
        PN->createArc(t,p);

        for (list<Pin*>::iterator input = (*inputBranch)->pins.begin(); input != (*inputBranch)->pins.end(); input++) {
            PN->createArc(*(*input)->getPlace(),t);
        }

    }

    // create all output places of the process
    for (list<Pin*>::iterator output = outputPins.begin(); output != outputPins.end(); output++) {
        (*output)->translateToNet(PN);
    }

    // translate all of the output branches
    for (list<PinCombination*>::iterator outputBranch = outputBranches.begin(); outputBranch != outputBranches.end(); outputBranch++)
    {
        pnapi::Transition &t = PN->createTransition(typeString() + "." + name + ".fire." + (*outputBranch)->getName());
        //t.roles().insert(roles.begin(),roles.end());
	t.addRoles(roles.begin(),roles.end());

        for (list<Pin*>::iterator output = (*outputBranch)->pins.begin(); output != (*outputBranch)->pins.end(); output++) {
            PN->createArc(t,*(*output)->getPlace());
        }

        for (list<pnapi::Place*>::iterator central = centralPlaces.begin(); central != centralPlaces.end(); central++) {
            PN->createArc(**central,t);
        }
    }
}

// generic destructor
Merge::~Merge() {
}

/*!
 * \result    number of incoming edges of this node
 */
int Merge::statistics_getInDegree () {
  int connectedPins = ControlFlowNode::statistics_getInDegree();
  // a merge is activated by only one of its input branches
  return connectedPins/inputBranches.size();
}

// translates this Flow content element to its corresponding
// petri net pattern in the given petri net
void Merge::translateToNet(pnapi::ExtendedWorkflowNet* PN) {

    //Create pointer to the current place and the current transition
    pnapi::Place& centralNode = PN->createPlace((typeString() + "." + name + ".activated"), pnapi::Node::INTERNAL);
    PN->process_internalPlaces.insert(&centralNode);

    // create all input places of the process
    for (list<Pin*>::iterator input = inputPins.begin(); input != inputPins.end(); input++) {
        (*input)->translateToNet(PN);
    }

    // translate all of the input criteria
    for (list<PinCombination*>::iterator inputBranch = inputBranches.begin(); inputBranch != inputBranches.end(); inputBranch++)
    {
        pnapi::Transition &t = PN->createTransition(typeString() + "." + name + ".activate." + (*inputBranch)->getName());
        //t.roles().insert(roles.begin(),roles.end());
	t.addRoles(roles.begin(),roles.end());
        PN->createArc(t,centralNode);

        for (list<Pin*>::iterator input = (*inputBranch)->pins.begin(); input != (*inputBranch)->pins.end(); input++) {
            PN->createArc(*(*input)->getPlace(),t);
        }
    }

    // create all output places of the process
    for (list<Pin*>::iterator output = outputPins.begin(); output != outputPins.end(); output++) {
        (*output)->translateToNet(PN);
    }

    // translate all of the output criteria
    for (list<PinCombination*>::iterator outputBranch = outputBranches.begin(); outputBranch != outputBranches.end(); outputBranch++)
    {
        pnapi::Transition &t = PN->createTransition(typeString() + "." + name + ".fire." + (*outputBranch)->getName());
        //t.roles().insert(roles.begin(),roles.end());
	t.addRoles(roles.begin(),roles.end());
        PN->createArc(centralNode,t);

        for (list<Pin*>::iterator output = (*outputBranch)->pins.begin(); output != (*outputBranch)->pins.end(); output++) {
            PN->createArc(t,*(*output)->getPlace());
        }
    }
}

// generates a debug output on the console for this Pin
void AtomicCFN::debugOutput(string delay) {

    std::cout << delay << typeString() << " | " << name << "\n";

    std::cout << delay << " Roles\n";
    for(set<string>::iterator role = roles.begin(); role != roles.end(); role++) {
        std::cout << delay + "  Role: " + (*role) + "\n";
    }

    for(list<Pin*>::iterator inputPin = inputPins.begin(); inputPin != inputPins.end(); inputPin++) {
        (*inputPin)->debugOutput(delay + "  ");
    }

    for(list<Pin*>::iterator outputPin = outputPins.begin(); outputPin != outputPins.end(); outputPin++) {
        (*outputPin)->debugOutput(delay + "  ");
    }
}

// generic destructor
AtomicCFN::~AtomicCFN() {
}

// translates this Flow content element to its corresponding
// petri net pattern in the given petri net
void AtomicCFN::translateToNet(pnapi::ExtendedWorkflowNet* PN) {

    // create all input places of the process
    for (list<Pin*>::iterator input = inputPins.begin(); input != inputPins.end(); input++) {
        (*input)->translateToNet(PN);

        pnapi::Place *p = (*input)->getPlace();
        // now act depending on if this atomic cnf is a stopNode or not (meaning an endnode)
        if (type == NSTOPNODE) {
            pnapi::Transition &t = PN->createTransition((typeString() + "." + name + ".loop"));
            t.addRoles(roles.begin(),roles.end());
            PN->createArc(*p,t);
            PN->createArc(t,*p);
            PN->omegaPlaces.insert(p);
        } else if (type == NENDNODE) {
            pnapi::Transition &t = PN->createTransition((typeString() + "." + name + ".eat"));
            t.addRoles(roles.begin(),roles.end());
            PN->createArc(*p,t);
        }
    }

    // create all output places of the process, this only happens for a startNode
    for (list<Pin*>::iterator output = outputPins.begin(); output != outputPins.end(); output++) {
        (*output)->translateToNet(PN);

        pnapi::Place *p = (*output)->getPlace();

        if (type == NSTARTNODE) {
          for(list<pnapi::Node*>::iterator t = (static_cast<Process*>(parentFCE))->startingTransitions.begin(); t != (static_cast<Process*>(parentFCE))->startingTransitions.end(); t++) {
              PN->createArc((**t),*p);
          }
        }
    }

}

