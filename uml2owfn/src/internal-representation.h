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

#ifndef INTERNALREPRESENTATION_H
#define INTERNALREPRESENTATION_H


/******************************************************************************
 * Headers
 *****************************************************************************/

#include <string>
#include <ostream>
#include <sstream>  // (std::ostringstream, std::istringstream)
#include <vector>
#include <set>
#include <map>
#include <list>
#include <deque>

// include PN Api headers
#include "pnapi/pnapi.h"
#include "petrinet-workflow.h"

#include "AST.h"
#include "UML-public.h"

#include "options.h"

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

typedef enum uml_elementType_e
{
    NPROCESS,
    NSERVICE,			///< input of an open workflow net (oWFN)
    NTASK,			///< output of an open workflow net (oWFN
    NSTARTNODE,			///< input of an open workflow net (oWFN)
    NSTOPNODE,			///< input of an open workflow net (oWFN)
    NENDNODE,			///< input of an open workflow net (oWFN)
    NDECISION,			///< input of an open workflow net (oWFN)
    NMERGE,			///< input of an open workflow net (oWFN)
    NFORK,			///< input of an open workflow net (oWFN)
    NJOIN,			///< input of an open workflow net (oWFN)
    NNOTIFICATIONBROADCASTER,			///< input of an open workflow net (oWFN)
    NNOTIFICATIONRECEIVER,			///< input of an open workflow net (oWFN)
    NOBSERVER,			///< input of an open workflow net (oWFN)
    NMAP,			///< input of an open workflow net (oWFN)
    NTIMER,			///< input of an open workflow net (oWFN)
    NLOOP,			///< input of an open workflow net (oWFN)
    NFORLOOP,			///< input of an open workflow net (oWFN)
    NCALLTOPROCESS,			///< input of an open workflow net (oWFN)
    NCALLTOSERVICE,			///< input of an open workflow net (oWFN)
    NCALLTOTASK,		///< input of an open workflow net (oWFN)
    ECONNECTION     ///< a connection
} uml_elementType;


/*!
 * \brief   helper class to gather and print statistics of a UML process
 */
class UmlProcessStatistics {
  friend class Process;

  private:
    bool    correct_syntax;
    int     processCharacteristics;
    int     numNodes;
    int     numEdges;
    float   avgInDegree;
    float   avgOutDegree;
    int     maxInDegree;
    int   	maxOutDegree;


  public:
    /*!
     * \brief   constructor, initialize with standard values
     */
    UmlProcessStatistics () {
      correct_syntax = true;
      processCharacteristics = UML_PC(PC_NORMAL);
      numNodes = 0;
      numEdges = 0;
      avgInDegree = 0;
      avgOutDegree = 0;
      maxInDegree = 0;
      maxOutDegree = 0;
    }

    /*!
     * \brief   print contents of this statistics object into a string
     * \param   delim   delimiter character for separating the different fields
     * \result  string representation of process statistics
     */
    string toString (char delim) {
      std::ostringstream result;

      // correct syntax
      if (correct_syntax)
        result << delim << "true";
      else
        result << delim << "false";

      // additional syntactical characteristics of the process
      if (processCharacteristics == UML_PC(PC_NORMAL))
        result << delim << "---";
      else {
        result << delim;
        if (processCharacteristics & UML_PC(PC_EMPTY))
          result << "empty process/";
        if (processCharacteristics & UML_PC(PC_OVERLAPPING))
          result << "overlapping pins/";
        if (processCharacteristics & UML_PC(PC_PIN_MULTI))
          result << "pin multiplicities/";
        if (processCharacteristics & UML_PC(PC_PIN_MULTI_NONMATCH))
          result << "non-matching pin multiplicities/";
      }

      // structural properties of the process
      result << delim << numNodes;
      result << delim << numEdges;
      result << delim << avgInDegree;
      result << delim << avgOutDegree;
      result << delim << maxInDegree;
      result << delim << maxOutDegree;

      return result.str();
    }
};

/******************************************************************************
 * Classes
 *****************************************************************************/

// the pin represents in- and outputs of tasks in the BOM model
class Pin {
    private:
        string name;

        string associatedData;

        /// whether this pin is an input pin of a process or task, if false, then it is an output pin
        bool isInput;

        /// pointer to the one connection attached to this pin
        FlowContentConnection* attachedConnection;

        /// pointer to the node owning this pin
        FlowContentNode* owningNode;

        /// pointer to the petrinet place representing this pin
        pnapi::Place* pinPlace;

        /// pointer to the owning criterion. Mainly used to get sure that pinsets are not overlapping
        PinCombination* owningCriterion;

    public:

        /// minimum consumption/production of tokens at this pin
        int min;

        /// maximum consumption/production of tokens at this pin
        int max;

        /// whether this pin takes data-flow tokens, plays a role when filtering pin multiplicities on forks and joins
        bool isDataPin;


        /// constructor
        Pin(string inputName, FlowContentNode* owner, bool isInput);

        /// returns the name
        string getName();

        /// returns the owning node
        FlowContentNode*  getOwner();

        /// set the data type associated to this pin
        void setAssociatedData(string associatedData);

        /// get the data type associated to this pin
        string getAssociatedData();

        /// set the Petri net place representing this pin
        void setPlace(pnapi::Place* placeToSet);

        /// get the Petri net place representing this pin
        pnapi::Place* getPlace();

        /// sets the criterion owning this pin
        void setCriterion(PinCombination* combination);

        /// returns true if there is no owning criterion
        bool free();

        /// attaches the connection
        void setConnection(FlowContentConnection* attach);

        /// returns the attached connection
        FlowContentConnection* getConnection();

        /// translates this pin to its petri net pattern in the given petri net
        void translateToNet(pnapi::ExtendedWorkflowNet* PN);

        /// generates a debug output on the console for this pin
        void debugOutput(string delay);

        /// destructor
        ~Pin();

};


// a combination of pins can either be a branch of a CFN or a criterion of a task
class PinCombination {
    private:
        string name;

        // pointer to the node owning this pin
        FlowContentNode* owningNode;

    public:

        // the pins of this combination
        list<Pin*> pins;

        // returns the name
        string getName();

        // returns the owning node
        FlowContentNode* getOwner();

        // constructor
        PinCombination(string givenName, FlowContentNode* owner);

        // generates a debug output on the console for this pin combination
        void debugOutput(string delay);

        // destructor
        virtual ~PinCombination() {}

};


// every input criterion has a status place
class InputCriterion : public PinCombination {
    public:
        // place indicating, that this inputcriterion has been used
        pnapi::Place* statusPlace;

        // constructor
        InputCriterion(string givenName, FlowContentNode* owner) : PinCombination(givenName, owner) {}

        // destructor
        ~InputCriterion() {}

};


// output criteria are always related to input criteria
class OutputCriterion : public PinCombination {
    public:
        // place indicating, that this inputcriterion has been used
        list<InputCriterion*> relatedInputCriteria;

        // constructor
        OutputCriterion(string givenName, FlowContentNode* owner) : PinCombination(givenName, owner) {}

        // destructor
        ~OutputCriterion() {}

};


// superclass for everything in the BOM model which is not a pin
class FlowContentElement {
    protected:

        /// pointer to the corresponding AST Element
        ASTElement* ASTLink;

        /// pointer to the parent FCE, NULL if this is a global element
        FlowContentElement* parentFCE;

        /// type of the node as from the enum
        uml_elementType type;

    public:
        /// returns the pointer to the parent-node
        ASTElement* getASTElement();

        /// returns the type
        uml_elementType getType();

        /// pointer to the corresponding AST Element
        FlowContentElement* getParent();

        /// pointer to the next process that contains this element
        Process* getEnclosingProcess();

        /// constructor
        FlowContentElement(ASTElement* link, FlowContentElement* parent);

        /// the type of the node as string
        string typeString();

        /// creates a debug output on the console for this element
        virtual void debugOutput(string delay) {}

        /// translates this flow content element to its petri net pattern
        /// in the given petri net
        virtual void translateToNet(pnapi::ExtendedWorkflowNet* PN) {}

        /// get number of incoming edges of this control-flow element
        virtual int statistics_getInDegree () { return 0; }

        /// get number of outgoing edges of this control-flow element
        virtual int statistics_getOutDegree () {return 0; }

        /// generic destructor
        virtual ~FlowContentElement() {}
};

class FlowContentConnection : public FlowContentElement {
    private:

        /// name of the connection in the process
        string name;

        /// source pin of the connection
        Pin* inputPin;

        /// target pin of the connection
        Pin* outputPin;

        /// source node of the connection
        FlowContentNode* sourceNode;

        /// target node of the connection
        FlowContentNode* targetNode;

    public:

        /// returns the name
        string getName();

        // translates this flow content element to its petri net pattern
        // in the given petri net
        void translateToNet(pnapi::ExtendedWorkflowNet* PN);

        /// sets the input pin
        void setInput(Pin* givenInput);

        /// sets the output pin
        void setOutput(Pin* givenOutput);

        /// sets the source node
        void setSource(FlowContentNode* givenSource);

        /// sets the target node
        void setTarget(FlowContentNode* givenTarget);

        /// returns the input pin
        Pin* getInput();

        /// returns the output pin
        Pin* getOutput();

        /// returns the source node
        FlowContentNode* getSource();

        /// returns the target node
        FlowContentNode* getTarget();

        // creates a debug output on the console for this element
        virtual void debugOutput(string delay);

        // constructor
        FlowContentConnection(string givenName, ASTElement* link, FlowContentElement* parent);

        // generic destructor
        ~FlowContentConnection();
};

// Any BOM Model element that is not a connection
class FlowContentNode : public FlowContentElement {
    protected:
        /// name of this node in the process
        string name;


    public:
        // is true if node is not to be translated due to some constraints
        int processCharacteristics;

        /// returns the name
        string getName();

        /// inherits all roles of the given FCN
        void inherit(FlowContentNode* givenNode);

        // translates this flow content element to its petri net pattern
        // in the given petri net
        virtual void translateToNet(pnapi::ExtendedWorkflowNet* PN) {}

        /// returns a pin if its name is given
        Pin* getPinByName(string find);

        /// set describing the role requirements of this FCN
        set<string> roles;

        // list of all input pins
        list<Pin*> inputPins;

        // list of all output pins
        list<Pin*> outputPins;

        /// get number of incoming edges of this control-flow node
        int statistics_getInDegree ();

        /// get number of outgoing edges of this control-flow node
        int statistics_getOutDegree ();

        // creates a debug output on the console for this element
        virtual void debugOutput(string delay) {}

        // constructor
        FlowContentNode(string givenName, uml_elementType givenType, ASTElement* link, FlowContentElement* parent);

        // destructor
        virtual ~FlowContentNode() {}
};

// tasks have roles and input/output criteria
class Task : public FlowContentNode {
    public:
        // constructor
        Task(string givenName, uml_elementType givenType, ASTElement* link, FlowContentElement* parent) : FlowContentNode(givenName,givenType,link,parent) {}

        // translates this flow content element to its petri net pattern
        // in the given petri net
        virtual void translateToNet(pnapi::ExtendedWorkflowNet* PN) {}

        // list of this tasks input criteria
        list<InputCriterion*> inputCriteria;

        // list of this tasks output criteria
        list<OutputCriterion*> outputCriteria;

        // returns an input Criterion given its name
        InputCriterion* getInputCriterionByName(string givenName);

        // returns an output Criterion given its name
        OutputCriterion* getOutputCriterionByName(string givenName);

        // generic destructor
        virtual ~Task() {}

        // creates a debug output on the console for this element
        virtual void debugOutput(string delay) {}
};

// A process is a special task containing a flow content
class Process : public Task {
    private:
        // pointer to this process petrinet representation
        pnapi::ExtendedWorkflowNet* processNet;

        // all nodes from the flow content
        list<FlowContentNode*> flowContentNodes;

        // all connections from the flow content
        list<FlowContentConnection*> flowContentConnections;


    public:
        // constructor
        Process(string givenName, uml_elementType givenType, ASTElement* link, FlowContentElement* parent) : Task(givenName,givenType,link,parent) {nodeNameMap = map<string,FlowContentNode*>();}

        // list of starting transition of this process
        list<pnapi::Node*> startingTransitions;

        // current map for names->nodes in this process
        map<string,FlowContentNode*> nodeNameMap;

        // spreads the roles of tasks to control flow nodes in this process
        void spreadRoles();

        // returns if this process has no flowcontent
        bool empty();

        /// the connection that enters/leaves this pin (if any)
        FlowContentConnection* getAdjacentConnection(Pin *pin) const;

        /// translates this flow content element to its petri net pattern
        /// in the given petri net
        void translateToNet(pnapi::ExtendedWorkflowNet* PN);

        /// adds a flow content node to this process
        void addFCN(FlowContentNode* toAdd);

        /// adds a connection to this process
        void addConnection(FlowContentConnection* toAdd);

        /// creates a debug output on the console for this element
        void debugOutput(string delay);

        /// traverse the model to update the process characteristics that
        /// could not be set earlier
        void updateCharacteristics();

        /// generate process statistics
        UmlProcessStatistics getStatistics() const;

        /// get all roles that do not contain a start node
        set<string> getNonStartNodeRoles() const;

        /// overwrite process name for the purpose of anonymization
        void setName(string newName);

        // generic destructor
        ~Process();
};

// every task that is not a process is a simple task
class SimpleTask : public Task {

    public:
        // constructor
        SimpleTask(string givenName, uml_elementType givenType, ASTElement* link, FlowContentElement* parent) : Task(givenName,givenType,link,parent) {}

        // translates this flow content element to its petri net pattern
        // in the given petri net
        void translateToNet(pnapi::ExtendedWorkflowNet* PN);

        // creates a debug output on the console for this element
        void debugOutput(string delay);

        // generic destructor
        ~SimpleTask();
};

// control flow nodes have branches instead of criteria and posess no roles
class ControlFlowNode: public FlowContentNode {
    public:

        // translates this flow content element to its petri net pattern
        // in the given petri net
        virtual void translateToNet(pnapi::ExtendedWorkflowNet* PN) {}

        // list of output branches
        list<PinCombination*> outputBranches;

        // list of input branches
        list<PinCombination*> inputBranches;

        // constructor
        ControlFlowNode(string givenName, uml_elementType givenType, ASTElement* link, FlowContentElement* parent) : FlowContentNode(givenName,givenType,link,parent) {}

        // creates a debug output on the console for this element
        void debugOutput(string delay);

        // generic destructor
        ~ControlFlowNode();
};

// special type of controlflownode, distinguished by its translation pattern
class Decision: public ControlFlowNode {
    public:

        // translates this flow content element to its petri net pattern
        // in the given petri net
        void translateToNet(pnapi::ExtendedWorkflowNet* PN);

        // decides whether this is an inclusive decision
        bool inclusive;

        // generates the pattern for an inclusive decision recursively
        void inclusiveDecisionPatternRecursively(pnapi::Place & centralNode, set<PinCombination*>& currentOutputBranches, string number, pnapi::ExtendedWorkflowNet* PN, set<set<PinCombination*> >& powerSet);

        /// number of outgoing edges of this node
        int statistics_getOutDegree();

        // constructor
        Decision(string givenName, uml_elementType givenType, ASTElement* link, FlowContentElement* parent) : ControlFlowNode(givenName,givenType,link,parent) { inclusive = false; }

        // generic destructor
        ~Decision();
};

// special type of controlflownode, distinguished by its translation pattern
class Fork: public ControlFlowNode {
    public:

        // translates this flow content element to its petri net pattern
        // in the given petri net
        void translateToNet(pnapi::ExtendedWorkflowNet* PN);

        // constructor
        Fork(string givenName, uml_elementType givenType, ASTElement* link, FlowContentElement* parent) : ControlFlowNode(givenName,givenType,link,parent) {}

        // generic destructor
        ~Fork();
};

// special type of controlflownode, distinguished by its translation pattern
class Join: public ControlFlowNode {
    public:

        // translates this flow content element to its petri net pattern
        // in the given petri net
        void translateToNet(pnapi::ExtendedWorkflowNet* PN);

        // constructor
        Join(string givenName, uml_elementType givenType, ASTElement* link, FlowContentElement* parent) : ControlFlowNode(givenName,givenType,link,parent) {}

        // generic destructor
        ~Join();
};

// special type of controlflownode, distinguished by its translation pattern
class Merge: public ControlFlowNode {
    public:

        // translates this flow content element to its petri net pattern
        // in the given petri net
        void translateToNet(pnapi::ExtendedWorkflowNet* PN);

        /// number of incoming edges of this node
        int statistics_getInDegree();

        //constructor
        Merge(string givenName, uml_elementType givenType, ASTElement* link, FlowContentElement* parent) : ControlFlowNode(givenName,givenType,link,parent) {}

        // generic destructor
        ~Merge();
};

// atomic nodes are those that can be represented by a single petrinet place
class AtomicCFN: public FlowContentNode {
    public:

        // translates this flow content element to its petri net pattern
        // in the given petri net
        void translateToNet(pnapi::ExtendedWorkflowNet* PN);

        // constructor
        AtomicCFN(string givenName, uml_elementType givenType, ASTElement* link, FlowContentElement* parent) : FlowContentNode(givenName,givenType,link,parent) {}

        // generic destructor
        ~AtomicCFN();

        // creates a debug output on the console for this element
        void debugOutput(string delay);
};

#endif // #define INTERNALREPRESENTATION
