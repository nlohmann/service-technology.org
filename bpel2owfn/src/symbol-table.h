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
 * \brief   Central storage for all parsed BPEL activities and whose datas.
 *
 * \author  
 *          - responsible: Dennis Reinert <reinert@informatik.hu-berlin.de>
 *          - last changes of: \$Author: reinert $
 *          
 * \date
 *          - created:
 *          - last changed:
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universit�t zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel
 *          for details.
 *
 * \version \$Revision: 1.16 $: 
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
class STCommunicationActivity;
class STCompensate;
class STCompensationHandler;
class STCorrelationSet;
class STElement;
class STEnvelope;
class STEventHandlers;
class STFaultHandlers;
class STInvoke;
class STLink;
class STPartner;
class STPartnerLink;
class STProcess;
class STReceive;
class STReply;
class STScope;
class STTerminate;
class STVariable;
class STWait;


/**
 * \class	STElement
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
 * \brief   - administrated the central storage for all parsed BPEL activities and whose datas 
 * 
 */
class SymbolTable
{
  private:
    /// mapping from AST id to symTab entry  
    map<unsigned int, unsigned int> id2key;

    /// a container to store ...
    map<unsigned int, SymbolTableEntry*> symTab;

    /// print formatted symbol table error message
	void printErrorMsg(string errorMsg);
    
    /// return the key of the last insert element
    unsigned int getCurrentEntryKey();

    /// return retranslation from elementId to BPEL-element name, e.g. K_ASSIGN->"assign"
    string translateToElementName(unsigned int elementId);
	
	/// domain check
	void checkAttributeValueYesNo(string attributeName, string attributeValue);	
	
	/// returns valid or unvalid depending on attribute value
	bool isValidAttributeValue(string attributeName, string attributeValue);
	
	/// returns valid or unvalid depending on attribute already exists or not
	bool isDuplicate(unsigned int entryKey, STAttribute* attribute);
    
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
    
    /// create a new entry in the symbol table and return symbol table entry key
    unsigned int insert(unsigned int elementId);

    /// return entry of symbol table
    SymbolTableEntry* lookup(unsigned int entryKey);

    /// to add an attribute to the special symbol table entry
    void addAttribute(unsigned int entryKey, STAttribute* attribute);

    /// wrapper for <readAttribute(unsigned int, string)> and return value from desired attribute
    STAttribute* readAttribute(kc::integer entryKey, string attributeName);

    /// return value from desired attribute
    STAttribute* readAttribute(unsigned int entryKey, string attributeName);
    
    /// create a new attribute
    STAttribute* newAttribute(kc::casestring attributeName, kc::casestring attributeValue);
    
    /// wrapper for checkAttributes(unsigned int entryKey, kc::casestring bpelElementValue)
	void checkAttributes(kc::integer entryKey, kc::casestring bpelElementValue);

    /// wrapper for checkAttributes(unsigned int entryKey)
    void checkAttributes(kc::integer entryKey);
	
	/// checked the attributes and the value of BPEL-elements
	void checkAttributes(unsigned int entryKey, kc::casestring bpelElementValue);

    /// checked the attributes and the value of BPEL-elements
    void checkAttributes(unsigned int entryKey);
    
    /// return 
    unsigned int idToKey(kc::integer astId);
    
    /// mapping between AST Id and symbol table entries
    void setMapping(unsigned int entryKey, kc::integer astId);
  
    /// return symbol table information string
    string getInformation(kc::integer astId, bool closeTag = false);
};


/**
 * \class	SymbolTableEntry
 *
 * \brief
 * 
 */
class SymbolTableEntry
{
  public:
    /// entry key within symbol table
    unsigned int entryKey;

    /// BPEL-element Id, e.g. used for cast decision
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
 * \class	STActivity
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
};

/**
 * \class	STTerminate
 *
 * \brief
 * 
 */
class STTerminate: public STActivity
{
  public:
    /// constructor
    STTerminate();
    STTerminate(unsigned int elementId, unsigned int entryKey);
    
    /// destructor
    ~STTerminate();

    // only used for <terminate> activity
    bool isFirstTerminate;
};


/**
 * \class	STCompensate
 *
 * \brief
 * 
 */
class STCompensate: public STActivity
{
  public:
    /// constructor
    STCompensate();
    STCompensate(unsigned int elementId, unsigned int entryKey);
    
    /// destructor
    ~STCompensate();

    // only used for <compensate> activity
    bool isInFaultHandler;
    bool isInCompensationHandler;
    bool hasScopeName;
};


/**
 * \class	STWait
 *
 * \brief
 * 
 */
class STWait: public STActivity
{
  public:
    /// constructor
    STWait();
    STWait(unsigned int elementId, unsigned int entryKey);
    
    /// destructor
    ~STWait();

    // only used for <wait> activity
    /// true if <wait> has a "until" attribute
    bool isWaitUntil;
};


/**
 * \class	STAttribute
 *
 * \brief
 * 
 */
class STAttribute
{
  public:
    /// constructor
    STAttribute(string name, string value);
    
    /// destructor
    ~STAttribute();
  
    /// name of attribute
    string name;
    
    /// attribute value
    string value;
    
    /// type of attribute value
    string type;
    
    /// line position of attribute within BPEL file
    unsigned int line;
};


/**
 * \class	STCompensationHandler
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
 * \class	STCorrelationSet
 *
 * \brief
 * 
 */
class STCorrelationSet: public STElement
{
  public:
};


/**
 * \class	STEnvelope
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
 * \class	STEventHandlers
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
 * \class	STFaultHandlers
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
 * \class	STCommunicationActivity
 *
 * \brief   for <onMessage>, <invoke>, <receive> and <reply>
 * 
 */
class STCommunicationActivity: public STActivity
{
  public:
    /// constructor
    STCommunicationActivity();
    STCommunicationActivity(unsigned int elementId, unsigned int entryKey);
    
    /// destructor
    ~STCommunicationActivity();

    // only used for communication activities
    string channelId;
    STVariable *inputVariable;
    STVariable *outputVariable;
};


/**
 * \class	STInvoke
 *
 * \brief
 * 
 */
class STInvoke: public STCommunicationActivity
{
  public:
    /// constructor
    STInvoke();
    STInvoke(unsigned int elementId, unsigned int entryKey);
    
    /// destructor
    ~STInvoke();

    // only used for <invoke> activity
    /// true if <invoke> has both input and output variable
    bool isSynchronousInvoke;
    
};


/**
 * \class	STReceive
 *
 * \brief
 * 
 */
class STReceive: public STCommunicationActivity
{
  public:
    /// constructor
    STReceive();
    STReceive(unsigned int elementId, unsigned int entryKey);
    
    /// destructor
    ~STReceive();
};

/**
 * \class	STReply
 *
 * \brief
 * 
 */
class STReply: public STCommunicationActivity
{
  public:
    /// constructor
    STReply();
    STReply(unsigned int elementId, unsigned int entryKey);
    
    /// destructor
    ~STReply();
};



/**
 * \class	STLink
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
 * \class	STPartner
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
 * \class	STPartnerLink
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
 * \class	STScope
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
 * \class	STVariable
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

