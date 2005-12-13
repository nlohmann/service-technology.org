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
 *          - last changed: \$Date: 2005/12/13 14:02:12 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel
 *          for details.
 *
 * \version \$Revision: 1.16 $
 *          - 2005-11-22 (gierds) Initial version.
 *          - 2005-11-24 (gierds) Put all funcionality into a class #SymbolManager
 *          - 2005-11-29 (gierds) Added checking of variables.
 *          - 2005-12-01 (gierds) Added checking of links.
 *          - 2005-12-08 (gierds) Added channels in SymbolManager
 *
 */

#ifndef CHECK_SYMBOLS_H
#define CHECK_SYMBOLS_H

#include <list>
#include <algorithm>
#include <typeinfo>
#include <map>

#include "bpel-kc-k.h" // phylum definitions
#include "bpel-kc-yystype.h" // data types for tokens and non-terminals

#include "debug.h"

using namespace std;

// forward declaration of classes
class SymbolManager;
class SymbolScope;
class ScopeScope;
class FlowScope;
class csPartnerLink;
class csVariable;
class csLink;
class csChannel;

class SymbolManager
{
  private:
    /// most outer scope - the Process
    SymbolScope * processScope;
    /// the current scope
    SymbolScope * currentScope;
    /// mapping of AST IDs to pointer of SymbolScope
    map<kc::integer, SymbolScope*> mapping;
    
  public:
    /// list of all Links in a Process
    list<std::string> links; 
    /// list of all Variables in a Process
    list<std::string> variables;
    /// list of all outgoing channels
    set<std::string> outChannels;
    /// list of all incoming channels
    set<std::string> inChannels;
    /// mapping of channel names to either "in" or "out"
    // map<std::string, std::string> channels;    

    /// constructor
    SymbolManager();

    /// destructor
    ~SymbolManager();

    /// initialise the Process scope
    void initialiseProcessScope(kc::integer id);
    /// add a new Scope scope
    void newScopeScope(kc::integer id);
    /// add a new Flow scope
    void newFlowScope(kc::integer id);
    /// quit a scope
    void quitScope();

    /// returns SymbolScope pointer to the appropriate scope with ID
    SymbolScope * getScope(kc::integer id);
    
    /// add a PartnerLink to the current scope
    void addPartnerLink(csPartnerLink* pl);
    /// check, if a PartnerLink exists in the current scope
    void checkPartnerLink(csPartnerLink* pl);
    /// check, if a PartnerLink with name exists in the current scope
    void checkPartnerLink(std::string);

    /// add a Variable to the current scope
    kc::casestring addVariable(csVariable* var);
    /// check, if a Variable exists in the current scope
    kc::casestring checkVariable(csVariable* var, bool isFaultVariable = false);
    /// check, if a Variable with name exists in the current scope
    kc::casestring checkVariable(std::string, bool isFaultVariable = false);

    /// add a Variable to the current scope
    kc::casestring addLink(csLink* link);
    /// check, if a Variable exists in the current scope
    kc::casestring checkLink(csLink* link, bool asSource);
    /// check, if a Variable with name exists in the current scope
    kc::casestring checkLink(std::string, bool asSource);
    /// run some simple checks on the Links defined in the current scope
    void checkLinks();

    /// add a channel to the inChannel list
    // kc::casestring addInChannel(csChannel *);
    /// add a channel to the outChannel list
    // kc::casestring addOutChannel(csChannel *);
    /// adds a channel with an appropriate type
    kc::casestring addChannel(csChannel * channel, bool isInChannel);
 
    /// prints the scope tree
    void printScope();
};

/**
 * \class	SymbolScope
 *
 * \author	Christian Gierds <gierds@informatik.hu-berlin.de>
 *
 * \brief	Represents a scope in genral.
 *
 * This class shall be used to represent a scope (not only a BPEL Scope)
 * in a process with all necessary elements like parent scope and children.
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
    list<SymbolScope*> children;
    /// ids of all children (necessary?)
    list<kc::integer> ids;
    /// list of variables (not needed for Flows)	
    list<csVariable*> variables;

    /// number of incoming links
    int inLinks;
    /// number of outgoing links
    int outLinks;
    
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
    list<csLink *> links;

    /// Constructor for scope without parent
    FlowScope(kc::integer myid);
    /// Constructor for scope with parent scope
    FlowScope(kc::integer myid, SymbolScope* myparent);

    ~FlowScope();
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

    /// Constructor that demands a value (maybe "") for all possible attributes
    csVariable ( string myname, string mymessageType, string mytype, string myelement);

    /// our own equality
    bool operator==(csVariable& other);
};

class csLink
{
  public:
    /// attribute name
    string name;
    /// line number of the link's definition
    int line;

    /// flag indicating the link's usage as a source
    bool isSource;
    /// flag indicating the link's usage as a target
    bool isTarget;

    // csLink * Target;

    /// Constructor
    csLink( string myname );

    /// our own equality
    bool operator==(csLink& other);
};

class csChannel
{
  public:
    /// attribute portType
    string portType;
    /// attribute operation
    string operation;
    /// attribute partnerLink
    string partnerLink;

    /// Constructor
    csChannel(string myportType, string myoperation, string mypartnerLink);

    /// our own equality
    bool operator==(csChannel& other);

    kc::casestring name();
};

#endif

