/*!
 * \file check_symbols.cc
 *
 * \brief Provides classes and functions in order to check the correct use of
 * 	  symbols within a BPEL process.
 *
 * \author  
 *          - responsible: Christian Gierds <gierds@informatik.hu-berlin.de>
 *          - last changes of: \$Author: nlohmann $
 *          
 * \date
 *          - created: 2005/11/22
 *          - last changed: \$Date: 2005/12/04 12:51:06 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel
 *          for details.
 *
 * \version \$Revision: 1.9 $
 *          - 2005-11-22 (gierds) Initial version.
 *	    - 2005-11-30 (gierds) Checking for PartnerLinks completed.
 *
 */

#include "check-symbols.h"

#include <stack>

extern std::string intToString(int);	// little helper function (helpers.cc)
extern int yylineno;			// line number from flex/bison

/// variable to format scope tree output
int SymbolScope::indent = 0;

SymbolManager::SymbolManager()
{
  processScope = NULL;
  currentScope = NULL;
}

SymbolManager::~SymbolManager()
{
  if (processScope != NULL)
  {
    delete(processScope);
    processScope = NULL;
    currentScope = NULL;
  }
}  

/**
 * Initialise the Process Scope.
 *
 * \param id ID of the AST node
 *
 */
void SymbolManager::initialiseProcessScope(kc::integer id)
{
  processScope = new ProcessScope(id);
  currentScope = processScope;

  mapping[id] = currentScope;
}

/**
 * Adds a new Scope scope to scope hierachy.
 *
 * \param id ID of the AST node
 *
 */
void SymbolManager::newScopeScope(kc::integer id)
{
  currentScope = new ScopeScope(id, currentScope);

  mapping[id] = currentScope;
}

/**
 * Adds a new Flow scope to scope hierachy.
 *
 * \param id ID of the AST node
 *
 */
void SymbolManager::newFlowScope(kc::integer id)
{
  currentScope = new FlowScope(id, currentScope);

  mapping[id] = currentScope;
}

/**
 * Leaves the current scope to the parent scope (if any).
 *
 */
void SymbolManager::quitScope()
{
  trace(TRACE_DEBUG, "[CS] - Leaving scope " + intToString(currentScope->id->value));

  /// \todo just testing #mapping, remove the following lines:
  if (currentScope != getScope(currentScope->id))
  {
    throw Exception(CHECK_SYMBOLS_CAST_ERROR,"Scope mapping sucks!\n");
  }
  
  if ( currentScope->parent != NULL )
  {
    currentScope = currentScope->parent;
    trace(TRACE_DEBUG, " -> new scope is " + intToString(currentScope->id->value) + " again\n");
  }  
  else
  {
    currentScope = NULL;
    trace(TRACE_DEBUG, " -> no further scope above\n");
  }
}

/**
 * Returns a pointer to the scope that corresponds to a
 * given id.
 *
 * \param id ID of the AST node
 * \returns a pointer to the appropriate scope
 *
 */
SymbolScope * SymbolManager::getScope(kc::integer id)
{
  return mapping[id];
}


/**
 * Adds a PartnerLink to the current scope (should be Process).
 *
 * \param id ID of the AST node
 * \param pl Pointer to the PartnerLink that shall be added 
 *
 */
void SymbolManager::addPartnerLink(kc::integer id, csPartnerLink* pl)
{
  trace(TRACE_VERY_DEBUG, "[CS] Adding (" + intToString(currentScope->id->value) 
		        + ") PartnerLink " + pl->name + ", " + pl->partnerLinkType + ", "
		      + pl->myRole + ", " + pl->partnerRole + "\n");
  // since we want to add a PartnerLink, we assume currentScope is a ProcessScope
  try
  {
    // check, if PartnerLink name is unique, otherwise throw Exception
    for (list<csPartnerLink*>::iterator iter = (dynamic_cast <ProcessScope *> (currentScope))->partnerLinks.begin();
		    iter != (dynamic_cast <ProcessScope *> (currentScope))->partnerLinks.end(); iter++)
    {
      if (*(*iter) == *pl)
      {
        throw Exception(DOUBLE_PARTNERLINK, "Two PartnerLinks with same name\n",
		       	"Name of double PartnerLink is \"" + pl->name + "\"\n");
      }
    }
    ((dynamic_cast <ProcessScope *> (currentScope))->partnerLinks).push_back(pl);
  }
  catch(bad_cast)
  {
    throw Exception(CHECK_SYMBOLS_CAST_ERROR,"Dynamic cast error while building scope tree\n",
		    "Seems node " + intToString(id->value) + " is no Process\n"); 
  }
}

/**
 * Checks, if PartnerLink is defined in scope.
 *
 * \param pl The PartnerLink to be checked
 *
 */
void SymbolManager::checkPartnerLink(csPartnerLink* pl)
{
  // if no name is set, there was probably no PartnerLink given
  if (pl->name == "")
  {
    trace(TRACE_VERY_DEBUG, "[CS] Checking PartnerLink, but no name is given; returning.\n");
    return;
  }

  trace(TRACE_DEBUG, "[CS] Checking PartnerLink " + pl->name + ", " + pl->partnerLinkType + ", "
		      + pl->myRole + ", " + pl->partnerRole + "\n");
  // since we want to add a PartnerLink, we assume currentScope is a ProcessScope
  try
  {
    // check, if PartnerLink name is present, otherwise throw Exception
    bool found = false;
    SymbolScope * scope = currentScope;
    while ((! found) && (scope != NULL))
    {
      // ascent to next scope with variable definitions
      trace(TRACE_VERY_DEBUG, "[CS]   Ascending to Process scope ...\n");
      while ((scope != NULL) && (typeid(*scope) != typeid(ProcessScope)))
      {
        trace(TRACE_VERY_DEBUG, "[CS]     typeids are ");
        trace(TRACE_VERY_DEBUG, typeid(*scope).name());
        trace(TRACE_VERY_DEBUG, " and ");
        trace(TRACE_VERY_DEBUG, typeid(ProcessScope).name());
        trace(TRACE_VERY_DEBUG, "\n");
 	     
        trace(TRACE_VERY_DEBUG, "[CS]     ... leaving scope " + intToString(scope->id->value) + "\n");
        scope = scope->parent;
      }

      trace(TRACE_VERY_DEBUG, "[CS]   Looking for defined PartnerLinks ...\n");
      for (list<csPartnerLink*>::iterator iter = (dynamic_cast <ProcessScope *> (scope))->partnerLinks.begin();
 		    iter != (dynamic_cast <ProcessScope *> (scope))->partnerLinks.end(); iter++)
      {
        if (*(*iter) == *pl)
        {
  	  found = true;
        }
      }
      scope = scope->parent;
    }
    if (!found)
    {
      yyerror(string("Name of undefined PartnerLink is \"" + pl->name + "\"\n").c_str());
    }
  }
  catch(bad_cast)
  {
    throw Exception(CHECK_SYMBOLS_CAST_ERROR,"Dynamic cast error while checking PartnerLink\n",
		    "Seems node " + intToString(currentScope->id->value) + " is no Process\n"); 
  }
  
}

/**
 * Checks, if PartnerLink is defined in scope.
 *
 * \param name Name of the PartnerLink to be checked
 *
 */
void SymbolManager::checkPartnerLink(std::string name)
{
  checkPartnerLink(new csPartnerLink(name, "", "", ""));
}


/// \todo (gierds) comment me
std::string SymbolManager::addVariable(kc::integer id, csVariable* var)
{
  trace(TRACE_VERY_DEBUG, "[CS] Adding Variable " + var->name + ", " + var->messageType + ", "
		      + var->type + ", " + var->element + "\n");
  for (list<csVariable*>::iterator iter = currentScope->variables.begin();
	    iter != currentScope->variables.end(); iter++)
  {
    if (*(*iter) == *var)
    {
      throw Exception(DOUBLE_VARIABLE, "Two Variables with same name\n",
		      "Name of double Variable is \"" + var->name + "\"\n");
    }
  }
  currentScope->variables.push_back(var);

  trace(TRACE_VERY_DEBUG, "[CS] Unique ID of Variable is " + std::string(intToString(currentScope->id->value) + "." + var->name) + "\n");
  return (intToString(currentScope->id->value) + "." + var->name);
}

/**
 * Checks, if Variable is defined in scope.
 *
 * \param pl The Variable to be checked
 *
 */
std::string SymbolManager::checkVariable(csVariable* var, bool isFaultVariable)
{
  int id = 0;
  std::string uniqueID;
  
  // if no name is set, there was probably no Variable given
  if (var->name == "")
  {
    trace(TRACE_VERY_DEBUG, "[CS] Checking Variable, but no name is given; returning.\n");
    return "";
  }

  trace(TRACE_DEBUG, "[CS] Checking Variable " + var->name + ", " + var->messageType + ", "
		      + var->type + ", " + var->element + "\n");
  try
  {
    // check, if Variable name is present, otherwise throw Exception
    bool found = false;
    SymbolScope * scope = currentScope;
    while ((! found) && (scope != NULL))
    {
      // ascent to next scope with variable definitions
      trace(TRACE_VERY_DEBUG, "[CS]   Ascending to next suitable scope ...\n");
      while ((scope != NULL) && (typeid(*scope) != typeid(ProcessScope)) && (typeid(*scope) != typeid(ScopeScope)))
      {
        trace(TRACE_VERY_DEBUG, "[CS]     typeids are ");
        trace(TRACE_VERY_DEBUG, typeid(*scope).name());
        trace(TRACE_VERY_DEBUG, "\n");
 	     
        trace(TRACE_VERY_DEBUG, "[CS]     ... leaving scope " + intToString(scope->id->value) + "\n");
        scope = scope->parent;
      }

      trace(TRACE_VERY_DEBUG, "[CS]   Looking for defined variables ...\n");
      for (list<csVariable*>::iterator iter = scope->variables.begin();
 		    iter != scope->variables.end(); iter++)
      {
        if (*(*iter) == *var)
        {
  	  found = true;
	  id = scope->id->value;
        }
      }
      scope = scope->parent;

    }
    uniqueID = std::string(intToString(id) + "." + var->name);
    if ((!found) && (!isFaultVariable))
    {
      yyerror(string("Name of undefined Variable is \"" + var->name + "\"\n").c_str());
    }
    else
    if ((!found) && (isFaultVariable))
    {
      // Fault Variables might be undefined, but that works for us
      uniqueID = addVariable(currentScope->id, var);
    }
  }
  catch(bad_cast)
  {
    throw Exception(CHECK_SYMBOLS_CAST_ERROR,"Dynamic cast error while checking Variables\n"); 
  }

  trace(TRACE_VERY_DEBUG, "[CS] Unique ID of Variable is " + std::string(intToString(id) + "." + var->name) + "\n");
  return uniqueID;
  
}

/**
 * Checks, if Variable is defined in scope.
 *
 * \param name Name of the Variable to be checked
 *
 */
std::string SymbolManager::checkVariable(std::string name, bool isFaultVariable)
{
  if (isFaultVariable)
  {
    trace(TRACE_VERY_DEBUG, "[CS] Checking Fault Variable");
  }
  return checkVariable(new csVariable(name, "", "", ""), isFaultVariable);
}


/**
 *
 *  \todo (gierds) comment me
 *
 */
void SymbolManager::printScope()
{
  if (processScope != NULL)
  {
    trace(TRACE_DEBUG, "\nPrinting scope tree:\n\n");
    processScope->print();
    trace(TRACE_DEBUG, "\n");
  }
}

/// \todo (gierds) comment me
SymbolScope::SymbolScope(kc::integer myid)
{
  trace(TRACE_DEBUG, "[CS] Creating new scope without parent for action " + intToString((int) myid->value) + "\n");
 
  id = myid;
  parent = NULL;

}

/// \todo (gierds) comment me
SymbolScope::SymbolScope(kc::integer myid, SymbolScope * myparent)
{
  trace(TRACE_DEBUG, "[CS] Creating new scope with parent " + intToString((int) (myparent->id)->value) + " for action " 
		  + intToString((int) myid->value) + "\n");

  id = myid;
  parent = myparent;
  parent->children.push_back(this);

}

/// \todo (gierds) comment me
SymbolScope::~SymbolScope()
{
  trace(TRACE_DEBUG, "[CS]   ~ Destructing scope starting in " + intToString( id->value ) + "\n");
  if ( variables.empty() ) 
  {
    trace(TRACE_VERY_DEBUG, "[CS]      No variables.\n");     
  }
  for ( list<csVariable*>::iterator elem = variables.begin(); elem != variables.end(); elem++)
  {
    trace(TRACE_VERY_DEBUG, "[CS]      Deleting variable " + (*elem)->name + "\n");
    delete(*elem);
  }

  if ( children.empty() ) 
  {
    trace(TRACE_VERY_DEBUG, "[CS]      No children.\n");     
  }
  for ( list<SymbolScope*>::iterator elem = children.begin(); elem != children.end(); elem++)
  {
    trace(TRACE_VERY_DEBUG, "[CS]      Deleting child element " + intToString(( (*elem)->id )->value) + "\n");
    delete(*elem);
  }
  
}

/// \todo (gierds) comment me
void SymbolScope::print()
{
  for (int i = 0; i < indent; i++)
  {
    trace(TRACE_DEBUG," ");
  }
  trace(TRACE_DEBUG, "scope " + intToString(id->value) + " of type ");
  trace(TRACE_DEBUG, typeid(*this).name());
  trace(TRACE_DEBUG, "\n");
  indent += 4;
  for (list<SymbolScope*>::iterator iter = children.begin(); iter != children.end(); iter++)
  {
    (*iter)->print();
  }
  indent -= 4;
}


/// \todo (gierds) comment me
ProcessScope::ProcessScope(kc::integer myid) : SymbolScope(myid)
{
  // empty	
}

/// \todo (gierds) comment me
ProcessScope::ProcessScope(kc::integer myid, SymbolScope* myparent) : SymbolScope(myid, myparent)
{
  // empty
}

/// \todo (gierds) comment me
ScopeScope::ScopeScope(kc::integer myid) : SymbolScope(myid)
{
  // empty	
}

/// \todo (gierds) comment me
ScopeScope::ScopeScope(kc::integer myid, SymbolScope* myparent) : SymbolScope(myid, myparent)
{
  // empty
}

/// \todo (gierds) comment me
FlowScope::FlowScope(kc::integer myid) : SymbolScope(myid)
{
  // empty	
}

/// \todo (gierds) comment me
FlowScope::FlowScope(kc::integer myid, SymbolScope* myparent) : SymbolScope(myid, myparent)
{
  // empty
}


/// \todo (gierds) comment me
csPartnerLink::csPartnerLink ( string myname, string mytype, string mymyrole, string mypartnerRole)
{
  // trace(TRACE_VERY_DEBUG, "Creating new PartnerLink\n");
  name            = myname;
  partnerLinkType = mytype;
  myRole          = mymyrole;
  partnerRole     = mypartnerRole;

}

/// \todo (gierds) comment me
bool csPartnerLink::operator==(csPartnerLink& other)
{
  return (name == other.name); // not needed&& (myRole == other.myRole) && 
}

csVariable::csVariable ( string myname, string mymessageType, string mytype, string myelement)
{
  name        = myname;
  messageType = mymessageType;
  type        = mytype;
  element     = myelement;

}

/// \todo (gierds) comment me
bool csVariable::operator==(csVariable& other)
{
  return (name == other.name);
}

