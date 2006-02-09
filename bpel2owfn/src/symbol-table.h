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
 *          - last changes of: \$Author: nlohmann $
 *          
 * \date
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universit�t zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel
 *          for details.
 *
 * \version \$Revision: 1.11 $: 
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
class SymbolTable;
class SymbolTableEntry;
class STActivity;
class STAttribute;
class STCompensationHandler;
class STCorrelationSet;
class STElement;
class STEnvelope;
class STEventHandlers;
class STFaultHandlers;
class STLink;
class STPartner;
class STPartnerLink;
class STProcess;
class STScope;
class STVariable;


/**
 * \class	Element
 *
 * \brief
 * 
 */
class STElement
{
  public:
    /// a map of the element's attributes
    map<string, STAttribute*> mapOfAttributes;
    
    /// line position of attribute within BPEL file
    unsigned int line;
    
    /// location of the activity (Scope/Process, FH, CH) to distribute error
    /// tokens correctly
    ActivityLocationId activityLocation;
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
    /// mapping from AST id to symTab entry  
    map<unsigned int, unsigned int> id2key;

    /// a container to store ...
    map<unsigned int, SymbolTableEntry*> symTab;
    
  public:
    /// constructor
    SymbolTable();
    
    /// destructor
    ~SymbolTable();
    
    /// current entry key
    unsigned int entryKey;
    
    /// increase the map key
    unsigned int nextKey();

	/// ST traces
	void traceST(string traceMsg);
    
    /// return the key of the last insert element
    unsigned int getCurrentEntryKey();
    
    /// create a new entry in the symbol table and return symbol table entry key
    unsigned int insert(unsigned int elementId);
    
    /// create a new attribute
    STAttribute* newAttribute(kc::casestring name, kc::casestring value);
    
    /// to add an attribute to the special symbol table entry
    void addAttribute(unsigned int entryKey, STAttribute* attribute);
    
    /// return value from desired attribute
    STAttribute* readAttribute(unsigned int entryKey, string name);
    
    /// return entry of symbol table
    SymbolTableEntry* lookup(unsigned int entryKey);
    
    /// mapping between AST Id and symbol table entries
    void setMapping(unsigned int entryKey, kc::integer astId);
    
    /// return retranslation from elementId to BPEL-element name, e.g. K_ASSIGN->"assign"
    string translateToElementName(unsigned int elementId);
    
    /// return
    string getInformation(kc::integer astId, bool closeTag = false);
};


/**
 * \class	SymbolTableEntry
 *
 * \brief the grandmother
 * 
 */
class SymbolTableEntry
{
  public:
    /// entry key within symbol table
    unsigned int entryKey;

    /// BPEL-element Id, used for cast decision
    unsigned int elementId;
    
    /// contructor
    SymbolTableEntry();
    SymbolTableEntry(unsigned int elementId);
    SymbolTableEntry(unsigned int elementId, unsigned int entryKey);
    
    /// destructor
    virtual ~SymbolTableEntry();
    
    ///
    void setEntryKey(unsigned int val);
    void setElementId(unsigned int val);
    
    ///
    unsigned int getEntryKey();
    unsigned int getElementId();
};




/**
 * \class	Activity
 *
 * \brief
 * 
 * All other activities.
 * 
 */
class STActivity: public STElement, public SymbolTableEntry
{
  public:
    /// constructor
    STActivity();
    STActivity(unsigned int elementId, unsigned int entryKey);
    
    /// destructor
    ~STActivity();

    /// true if activity is source of a link
    bool isSourceOfLink;

    /// true if activity is target of a link
    bool isTargetOfLink;
    
    // only used for <invoke> activity
    /// true if <invoke> has both input and output variable
    bool isSynchronousInvoke;

    // only used for communication activities
    string channelId;
    STVariable *inputVariable;
    STVariable *outputVariable;

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
 * \class	Attribute
 *
 * \brief
 * 
 */
class STAttribute
{
  public:
    /// constructor
    STAttribute(kc::casestring name, kc::casestring value);
    
    ///
    ~STAttribute();
  
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
 * \class	CompensationHandler
 *
 * \brief
 * 
 * Everything about <compensationHandler>.
 * 
 */
class STCompensationHandler
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
 * \class	CorrelationSet
 *
 * \brief
 * 
 */
class STCorrelationSet: public STElement
{
  public:
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
class STEnvelope
{
  public:
    /// a list of variables declared in this scope/process
    list<STVariable*> variables;
    
    /// a list of correlation sets declared in this scope/process
    list<STCorrelationSet*> correlationSets;
    
    /// the fault handler of the scope/process
    STFaultHandlers* faultHandler;
    
    /// the compensation handler of the scope/process
    STCompensationHandler* compensationHandler;
    
    /// the event handler of the scope/process
    STEventHandlers* eventHandler;

    /// true if scope had an event handler?
    bool hasEventHandler;    
};


/**
 * \class	EventHandlers
 *
 * \brief
 * 
 * Everything about <eventHandlers>.
 * 
 */
class STEventHandlers
{
  public:
};


/**
 * \class	FaultHandlers
 *
 * \brief
 * 
 * Everything about <faultHandlers>.
 * 
 */
class STFaultHandlers
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
 * \class	Link
 *
 * \brief
 * 
 */
class STLink: public STElement, public SymbolTableEntry
{
  public:
    /// constructor
    STLink();
    STLink(unsigned int elementId, unsigned int entryKey);
        
    /// destructor
    ~STLink();

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
 * \class	Partner
 *
 * \brief
 * 
 */
class STPartner: public STElement, public SymbolTableEntry
{
  public:
    /// constructor
    STPartner();
    STPartner(unsigned int elementId, unsigned int entryKey);
    
    /// destructor
    ~STPartner();
};


/**
 * \class	PartnerLink
 *
 * \brief
 * 
 */
class STPartnerLink: public STElement, public SymbolTableEntry
{
  public:
    /// constructor
    STPartnerLink();
    STPartnerLink(unsigned int elementId, unsigned int entryKey);
    
    /// destructor
    ~STPartnerLink();
};


/**
 * \class	STProcess
 *
 * \brief
 * 
 * The <process> activity.
 * 
 */
class STProcess: public STElement, public STEnvelope, public SymbolTableEntry
{
  public:
    /// constructor
    STProcess();
    STProcess(unsigned int elementId, unsigned int entryKey);
    
    /// destructor
    ~STProcess();
    
    /// a list of partner links declared in the process
    list<STPartnerLink*> partnerLinks;

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
class STScope: public STElement, public STEnvelope, public SymbolTableEntry
{
  public:
    /// constructor
    STScope();
    STScope(unsigned int elementId, unsigned int entryKey);
    
    /// destructor
    ~STScope();

    /// additional attribute used for inter-scope communication (push-places)
    unsigned int parentScopeId;

    /// list of all enclosed links (recursively)
    list<STLink*> enclosedLinks;
};


/**
 * \class	Variable
 *
 * \brief
 * 
 */
class STVariable : public STElement, public SymbolTableEntry
{
  public:
    /// constructor
    STVariable();
    STVariable(unsigned int elementId, unsigned int entryKey);
        
    /// destructor
    ~STVariable();
    
    /// true if variable is used in an activity
    bool used;
};

#endif

