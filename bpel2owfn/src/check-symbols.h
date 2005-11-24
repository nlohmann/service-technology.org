/*!
 * \file check-symbols.h
 *
 * \brief Provides classes and functions in order to check the correct use of
 * 	  symbols within a BPEL process.
 *
 * \author  
 *          - responsible: Christian Gierds <gierds@informatik.hu-berlin.de>
 *          - last changes of: \$Author: gierds $
 *          
 * \date
 *          - created: 2005/11/22
 *          - last changed: \$Date: 2005/11/24 10:41:00 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel
 *          for details.
 *
 * \version \$Revision: 1.1 $
 *          - 2005-11-22 (gierds) Initial version.
 *
 */

#ifndef CHECK_SYMBOLS_H
#define CHECK_SYMBOLS_H

#include <list>
#include <algorithm>
#include <typeinfo>

#include "bpel-kc-k.h" // phylum definitions
#include "bpel-kc-yystype.h" // data types for tokens and non-terminals

#include "debug.h"

using namespace std;

// forward declaration of classes
class SymbolScope;
class ScopeScope;
class FlowScope;
class csPartnerLink;
class csVariable;

/**
 * \class	Scope
 *
 * \author	Christian Gierds <gierds@informatik.hu-berlin.de>
 *
 * \brief	Represents a scope in genral.
 *
 * This class shall be used to represent a scope (not only a BPEL Scope)
 * in a process with all necessary elements like parent scope and childs.
 *
 */
class SymbolScope
{
  public:
    /// the object's ID which created the scope
    kc::integer id;
    /// the scope we are nested in
    SymbolScope* parent;
    /// out child scopes, needed?
    list<SymbolScope*> childs;
    /// ids of all childs (necessary?)
    list<kc::integer> ids;
    /// list of variables (not needed for Flows)	
    list<csVariable*> variables;
    
    /// Constructor for scope without parent
    SymbolScope(kc::integer myid);
    /// Constructor for scope with parent scope
    SymbolScope(kc::integer myid, SymbolScope* myparent);
    
    // cleans up the object (deletion of all members etc.)
    virtual ~SymbolScope();   

    virtual void print();
    
  private:
    static int indent;
};

/**
 * \class	ProcessScope
 *
 * \author	Christian Gierds <gierds@informatik.hu-berlin.de>
 *
 * \brief	Represents the scope given through a Process
 *
 * 
 *
 */
class ProcessScope: public SymbolScope
{
  public:
    /// the PartnerLinks defined within the Process
    list<csPartnerLink*> partnerLinks;

    /// Constructor for scope without parent
    ProcessScope(kc::integer myid);
    /// Constructor for scope with parent scope
    ProcessScope(kc::integer myid, SymbolScope* myparent);
};


/**
 * \class	ScopeScope
 *
 * \author	Christian Gierds <gierds@informatik.hu-berlin.de>
 *
 * \brief	Represents the scope given through a Scope
 *
 * 
 *
 */

class ScopeScope: public SymbolScope
{

  public:
    /// Constructor for scope without parent
    ScopeScope(kc::integer myid);
    /// Constructor for scope with parent scope
    ScopeScope(kc::integer myid, SymbolScope* myparent);

};


/**
 * \class	FlowScope
 *
 * \author	Christian Gierds <gierds@informatik.hu-berlin.de>
 *
 * \brief	Represents the scope given through a Flow
 *
 * 
 *
 */
class FlowScope: public SymbolScope
{
  public:
    /// list of links defined within a Flow
    list<kc::integer> links;

    /// Constructor for scope without parent
    FlowScope(kc::integer myid);
    /// Constructor for scope with parent scope
    FlowScope(kc::integer myid, SymbolScope* myparent);
};


/**
 * \class	csPartnerLink
 *
 * \author	Christian Gierds <gierds@informatik.hu-berlin.de>
 *
 * \brief	Represents a PartnerLink
 *
 * In order to characterise a PartnerLink we need to store its attributes.
 *
 */
class csPartnerLink
{
  public:
    /// attribute name	  
    string name;
    /// attribute partnerLinkType
    string partnerLinkType;
    /// attribute myRole
    string myRole;
    /// attribute partnerRole
    string partnerRole;

    /// Constructor that demands a value (maybe NULL) for all possible attributes
    csPartnerLink ( string myname, string mytype, string mymyrole, string mypartnerRole);

    /// our own equality
    bool operator==(csPartnerLink& other);
};

/**
 * \class	csVariable
 *
 * \author	Christian Gierds <gierds@informatik.hu-berlin.de>
 *
 * \brief	Represents a Variable
 *
 * In order to characterise a Variable we need to store its attributes.
 *
 */
class csVariable
{
  public:
    /// attribute name
    string name;
    /// attribute messageType
    string messageType;
    /// attribute type
    string type;
    /// attribute element
    string element;

    /// Constructor that demands a value (maybe NULL) for all possible attributes
    csVariable ( string myname, string mymessageType, string mytype, string myelement);

    /// our own equality
    bool operator==(csVariable& other);
};


/// initialise the Process scope
void initialiseProcessScope(kc::integer id);
/// add a new Scope scope
void csNewScopeScope(kc::integer id);
/// add a new Flow scope
void csNewFlowScope(kc::integer id);
/// quit a scope
void csQuitScope();

/// add a PartnerLink to the current scope
void csAddPartnerLink(kc::integer id, csPartnerLink* pl);
/// check, if a PartnerLink exists in the current scope
void csCheckPartnerLink(csPartnerLink* pl);
/// add a Variable to the current scope
void csAddVariable(kc::integer id, csVariable* var);

#endif

