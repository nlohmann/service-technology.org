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
 *          
 * \date
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universit�t zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel
 *          for details.
 *
 * \version \$Revision: 1.3 $: 
 *
 */

#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <map>
#include <list>

#include "bpel-attributes.h"

using namespace std;



typedef enum
{
  SCOPE,
  FAULTHANDLER,
  COMPENSATIONHANDLER
} ActivityPositionId;


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





class Element
{
  public:
    /// a list of the element's attributes
    list<Attribute*> listOfAttributes;
    
    /// line position of attribute within BPEL file
    unsigned int line;
    
    /// position of the activity (Scope/Process, FH, CH) to distribute error
    /// tokens correctly
    ActivityPositionId activityPosition;
};





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





class Variable: public Element
{
  public:
    /// true if variable is used in an activity
    bool used;
};





class PartnerLink: public Element
{
};





class CorrelationSet: public Element
{
};





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





class EventHandlers
{
  public:
    ///
};





/**
 * An envelope for the process and scopes, i.e. those activities that may
 * enclose variables, correlation sets, fault handlers, compensation handlers
 * or event handlers.
 */
class Envelope
{
  public:
    /// a list of variables declared in this scope/process
    list<Variable*> variables;
    
    /// a list of correlation sets declared in this scope/process
    list<CorrelationSet> correlationSets;
    
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
 * The <process> activity.
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
 * The <scope> activity.
 */
class Scope: public Element, Envelope, SymbolTableEntry
{
  public:
    /// additional attribute used for inter-scope communication (push-places)
    unsigned int parentScopeId;
};





/**
 * All other activities.
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
    
    ///
    void insert();
    
    ///
    SymbolTableEntry lookup(kc::integer entryId);
    
};

#endif

