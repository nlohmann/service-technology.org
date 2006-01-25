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
\****************************************************************************/ 

/*!
 * \file symbol-table.h
 *
 * \brief
 *
 * \author  
 *          - last changes of: \$Author: reinert $
 *          
 * \date
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universit�t zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel
 *          for details.
 *
 * \version \$Revision: 1.5 $: 
 *
 */

#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <map>
#include <list>

#include "bpel-attributes.h"

using namespace std;



/// enumeration of possible locations of an activity used to send failures to
/// the correct place in the Petri net
typedef enum
{
  SCOPE,		///< activity enclosed in scope or process
  FAULTHANDLER,		///< activity enclosed in fault handler
  COMPENSATIONHANDLER	///< activity enclosed in compensation handler
} ActivityLocationId;


// forward declaration of classes
class Activity;
class Attribute;
class CompensationHandler;
class CorrelationSet;
class Element;
class Envelope;
class EventHandlers;
class FaultHandlers;
class PartnerLink;
class Process;
class Scope;
class SymbolTable;
class SymbolTableEntry;
class Variable;
class Link;




/**
 * \class	SymbolTableEntry
 *
 * \brief
 * 
 */
class SymbolTableEntry
{
  public:
    ///   
    unsigned int entryId;

    /// BPEL-element Id
    unsigned int elementId;
    
    /// cast information
    string elementType;
    
    /// contructor
    SymbolTableEntry();
    
    /// destructor
    ~SymbolTableEntry();
    
    string getElementId();
    
    string getElementType();
};




/**
 * \class	Element
 *
 * \brief
 * 
 */
class Element
{
  public:
    /// a list of the element's attributes
    list<Attribute*> listOfAttributes;
    
    /// line position of attribute within BPEL file
    unsigned int line;
    
    /// location of the activity (Scope/Process, FH, CH) to distribute error
    /// tokens correctly
    ActivityLocationId activityLocation;
};




/**
 * \class	Attribute
 *
 * \brief
 * 
 */
class Attribute
{
  public:
    /// name of attribute
    kc::casestring name;
    
    /// attribute value
    kc::casestring value;
    
    /// type of attribute value
    string type;
    
    /// line position of attribute within BPEL file
    unsigned int line;
};




/**
 * \class	Variable
 *
 * \brief
 * 
 */
class Variable: public Element
{
  public:
    /// true if variable is used in an activity
    bool used;
};



/**
 * \class	Link
 *
 * \brief
 * 
 */
class Link: public Element
{
  public:
    /// the name of the link
    string name;
    
    /// the identifier of the source activity
    unsigned int sourceId;
    
    /// the identifier of the target activity
    unsigned int targetId;

    /// the identifier of the flow which defined the link
    unsigned int parentId;
};




/**
 * \class	PartnerLink
 *
 * \brief
 * 
 */
class PartnerLink: public Element
{
};



/**
 * \class	CorrelationSet
 *
 * \brief
 * 
 */
class CorrelationSet: public Element
{
};




/**
 * \class	FaultHandlers
 *
 * \brief
 * 
 * Everything about <faultHandlers>.
 * 
 */
class FaultHandlers
{
  public:
    /// true if fault handler has an <catchAll> branch
    bool hasCatchAll;

    /// true if fault handler is user-defined, false if implicit
    bool isUserDefined;

    /// true if fault handler is enclosed in process, false if enclosed in a
    /// scope
    bool isInProcess;

    /// the identifier of the scope or process that encloses this fault handler
    int parentScopeId;
};


/**
 * \class	CompensationHandler
 *
 * \brief
 * 
 * Everything about <compensationHandler>.
 * 
 */
class CompensationHandler
{
  public:
    /// true if compensation handler encloses an <compensate/> activity
    bool hasCompensateWithoutScope;

    /// true if compensation handler encloses an <compensate scope="A"/>
    /// activity
    bool hasCompensateWithScope;

    /// true if compensation handler is user-defined, false if implicit
    bool isUserDefined;
};





/**
 * \class	EventHandlers
 *
 * \brief
 * 
 * Everything about <eventHandlers>.
 * 
 */
class EventHandlers
{
};



/**
 * \class	Envelope
 *
 * \brief
 * 
 * An envelope for the process and scopes, i.e. those activities that may
 * enclose variables, correlation sets, fault handlers, compensation handlers
 * or event handlers.
 * 
 */
class Envelope
{
  public:
    /// a list of variables declared in this scope/process
    list<Variable*> variables;
    
    /// a list of correlation sets declared in this scope/process
    list<CorrelationSet*> correlationSets;
    
    /// the fault handler of the scope/process
    FaultHandlers* faultHandler;
    
    /// the compensation handler of the scope/process
    CompensationHandler* compensationHandler;
    
    /// the event handler of the scope/process
    EventHandlers* eventHandler;

    /// true if scope had an event handler?
    bool hasEventHandler;    
};





/**
 * \class	Process
 *
 * \brief
 * 
 * The <process> activity.
 * 
 */
class Process: public Element, Envelope, SymbolTableEntry
{
  public:
    /// a list of partner links declared in the process
    list<PartnerLink*> partnerLinks;

    /// true if process is abstract (i.e. a business protocol)
    bool abstractProcess;
};




/**
 * \class	Scope
 *
 * \brief
 * 
 * The <scope> activity.
 * 
 */
class Scope: public Element, Envelope, SymbolTableEntry
{
  public:
    /// additional attribute used for inter-scope communication (push-places)
    unsigned int parentScopeId;

    /// list of all enclosed links (recursively)
    list<Link*> enclosedLinks;
};




/**
 * \class	Activity
 *
 * \brief
 * 
 * All other activities.
 * 
 */
class Activity: public Element, SymbolTableEntry
{
  public:
    /// true if activity is source of a link
    bool isSourceOfLink;

    /// true if activity is target of a link
    bool isTargetOfLink;
    
    // only used for <invoke> activity
    /// true if <invoke> has both input and output variable
    bool isSynchronousInvoke;

    // only used for communication activities
    string channelId;
    Variable *inputVariable;
    Variable *outputVariable;

    // only used for <wait> activity
    /// true if <wait> has a "until" attribute
    bool isWaitUntil;

    // only used for <compensate> activity
    bool isInFaultHandler;
    bool isInCompensationHandler;
    bool hasScopeName;

    // only used for <terminate> activity
    bool isFirstTerminate;
};




/**
 * \class	SymbolTable
 *
 * \brief
 * 
 */
class SymbolTable
{
  private:
    /// a container to store ...
    map<unsigned int, SymbolTableEntry*> symTab;
    
  public:
    /// constructor
    SymbolTable();
    
    /// destructor
    ~SymbolTable();
    
    /// 
    unsigned int entryId;
    
    /// increase the id
    kc::integer nextId();

	/// ST traces
	void traceST(string traceMsg);
    
    ///
    void insert(unsigned int elementType);
    
    ///
    SymbolTableEntry lookup(kc::integer entryId);
};

#endif

