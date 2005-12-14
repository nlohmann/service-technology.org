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
 * \file check_symbols.cc
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
 *          - last changed: \$Date: 2005/12/14 10:16:28 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universit&auml;t zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel
 *          for details.
 *
 */

#include "check-symbols.h"

#include <stack>

extern std::string intToString(int);	// little helper function (helpers.cc)
extern int yylineno;			// line number from flex/bison
extern bool inWhile;			// flag if in while activity (bpel-syntax.y)

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
  
  SymbolScope * higherScope = currentScope;
  currentScope = new ScopeScope(id, currentScope);
  
  // we want to inform a higher scope, that is no Flow
  if (typeid(*higherScope) == typeid(FlowScope))
  {
    while ( typeid(*higherScope) == typeid(FlowScope) )
    {
      higherScope = higherScope->parent;
    }

    higherScope->children.push_back(currentScope);
  }
  
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
  // no need for Flows to be in the children list
  // currentScope = new FlowScope(id, currentScope);

  SymbolScope * parent = currentScope;
  currentScope = new FlowScope(id);
  currentScope->parent = parent;

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
void SymbolManager::addPartnerLink(csPartnerLink* pl)
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
		    "Seems node " + intToString(currentScope->id->value) + " is no Process\n"); 
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

      if (scope != NULL)
      {
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
kc::casestring SymbolManager::addVariable(csVariable* var)
{
  std::string uniqueID;
	
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
  // add to currentScope for later checking
  currentScope->variables.push_back(var);

  uniqueID = std::string(intToString(currentScope->id->value) + "." + var->name);
  // add unique name to global variable list
  variables.push_back(uniqueID);

  trace(TRACE_VERY_DEBUG, "[CS] Unique ID of Variable is " 
		          + uniqueID + "\n");
  return kc::mkcasestring(uniqueID.c_str());
}

/**
 * Checks, if Variable is defined in scope.
 *
 * \param pl The Variable to be checked
 *
 */
kc::casestring SymbolManager::checkVariable(csVariable* var, bool isFaultVariable)
{
  int id = 0;
  std::string uniqueID;
  
  // if no name is set, there was probably no Variable given
  if (var->name == "")
  {
    trace(TRACE_VERY_DEBUG, "[CS] Checking Variable, but no name is given; returning.\n");
    return kc::mkcasestring(string().c_str());
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

      if (scope != NULL)
      {
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
      uniqueID = string(addVariable(var)->name);
    }
  }
  catch(bad_cast)
  {
    throw Exception(CHECK_SYMBOLS_CAST_ERROR,"Dynamic cast error while checking Variables\n"); 
  }

  trace(TRACE_VERY_DEBUG, "[CS] Unique ID of Variable is " + std::string(intToString(id) + "." + var->name) + "\n");
  return kc::mkcasestring(uniqueID.c_str());
  
}

/**
 * Checks, if Variable is defined in scope.
 *
 * \param name Name of the Variable to be checked
 *
 */
kc::casestring SymbolManager::checkVariable(std::string name, bool isFaultVariable)
{
  if (isFaultVariable)
  {
    trace(TRACE_VERY_DEBUG, "[CS] Checking Fault Variable");
  }
  return checkVariable(new csVariable(name, "", "", ""), isFaultVariable);
}

/**
 * Adds a Link to the current scope (should be a <flow>).
 *
 * \param id   ID of the AST node
 * \param link the Link to be added
 * \return     the unique Link ID
 * 
 */
kc::casestring SymbolManager::addLink(csLink* link)
{
  std::string linkID;
	
  trace(TRACE_VERY_DEBUG, "[CS] Adding Link " + link->name + "\n");
  

  // since we want to add a Link, we assume currentScope is a FlowScope
  try
  {
    // check, if Link name is unique, otherwise call yyerror
    for (list<csLink*>::iterator 
	    iter = (dynamic_cast <FlowScope *> (currentScope))->links.begin();
	    iter != (dynamic_cast <FlowScope *> (currentScope))->links.end(); 
	    iter++)
    {
      if (*(*iter) == *link)
      {
        yyerror(string("Two Links with same name\nName of double Link is \"" + 
			link->name + "\"\n").c_str());
      }
    }
    ((dynamic_cast <FlowScope *> (currentScope))->links).push_back(link);
    linkID = (intToString(currentScope->id->value) + "." + link->name);
    links.push_back(linkID);
  }
  catch(bad_cast)
  {
    throw Exception(CHECK_SYMBOLS_CAST_ERROR,
		    "Dynamic cast error while building scope tree\n",
		    "Seems node " + intToString(currentScope->id->value) + 
		    " is no Flow\n"); 
  }

  trace(TRACE_VERY_DEBUG, "[CS] Unique ID of Link is " 
		  + linkID + "\n");
  return kc::mkcasestring(linkID.c_str());
}

/**
 * Checks, if Link is defined in scope.
 *
 * \param pl       The Link to be checked
 * \param asSource false, iff used as Source of a Link (otherwise as a Target)
 * \return         the unique ID of the Link
 *
 */
kc::casestring SymbolManager::checkLink(csLink* link, bool asSource)
{
  int id = 0;
  std::string uniqueID;
  
  // if no name is set, there was probably no Link given
  if (link->name == "")
  {
    trace(TRACE_VERY_DEBUG, "[CS] Checking Link, but no name is given; returning.\n");
    return kc::mkcasestring(string().c_str());
  }

  // exit because in while activities no links are allowed
  if (inWhile)
  {
    yyerror(string("Usage of Links is not allowed within Whiles\n").c_str());
  }

  trace(TRACE_DEBUG, "[CS] Checking Link " + link->name + "\n");
  try
  {
    // check, if Link name is present, otherwise throw Exception
    bool found = false;
    SymbolScope * scope = currentScope;
    while ((! found) && (scope != NULL))
    {
      // ascent to next scope with variable definitions
      trace(TRACE_VERY_DEBUG, "[CS]   Ascending to next suitable scope ...\n");
      while ((scope != NULL) && (typeid(*scope) != typeid(FlowScope)))
      {
        trace(TRACE_VERY_DEBUG, "[CS]     typeids are ");
        trace(TRACE_VERY_DEBUG, typeid(*scope).name());
        trace(TRACE_VERY_DEBUG, " and ");
        trace(TRACE_VERY_DEBUG, typeid(FlowScope).name());
        trace(TRACE_VERY_DEBUG, "\n");
 	     
        trace(TRACE_VERY_DEBUG, "[CS]     ... leaving scope " + intToString(scope->id->value) + "\n");
        scope = scope->parent;
      }

      if (scope != NULL)
      {
        trace(TRACE_VERY_DEBUG, "[CS]   Looking for defined Links ...\n");
        for (list<csLink*>::iterator 
	        iter = (dynamic_cast <FlowScope *> (scope))->links.begin();
 	        iter != (dynamic_cast <FlowScope *> (scope))->links.end(); 
	        iter++)
        {
          if (*(*iter) == *link)
          {
    	    found = true;
	    if (asSource)
	    {
              if ((*iter)->isSource)
	      {
                yyerror(string("Link \"" + link->name + "\" was already used as source\n").c_str());
	      }
	      (*iter)->isSource = true;
	    }
	    else
	    {
              if ((*iter)->isTarget)
	      {
                yyerror(string("Link \"" + link->name + "\" was already used as target\n").c_str());
	      }
	      (*iter)->isTarget = true;
	    }
	    id = scope->id->value;
          }
        }
        scope = scope->parent;
      }
    }
    uniqueID = std::string(intToString(id) + "." + link->name);
    if ((!found))
    {
      yyerror(string("Name of undefined Link is \"" + link->name + "\"\n").c_str());
    }
  }
  catch(bad_cast)
  {
    throw Exception(CHECK_SYMBOLS_CAST_ERROR,"Dynamic cast error while checking Links\n"); 
  }

  trace(TRACE_VERY_DEBUG, "[CS] Unique ID of Link is " + std::string(intToString(id) + "." + link->name) + "\n");
  return kc::mkcasestring(uniqueID.c_str());
  
}

/**
 * Checks, if Link is defined in scope.
 *
 * \param name     Name of the Link to be checked
 * \param asSource false, iff used as Source of a Link (otherwise as a Target)
 * \return         the unique ID of the Link
 *
 */
kc::casestring SymbolManager::checkLink(std::string name, bool asSource)
{
  return checkLink(new csLink(name), asSource);
}

/**
 * Performes some simple checks on the links defined in the current scope
 * (should be a FlowScope, otherwise an #Exception is thrown).
 * 
 * Checks:
 *  - link is used as a source as well as a target
 * 
 */
void SymbolManager::checkLinks()
{
  bool problems = false;
  try
  {
    trace(TRACE_DEBUG,"[CS] Checking correct usage of links\n");
    if (typeid(*currentScope) == typeid(FlowScope))
    {
//        for (list<csLink*>::iterator 
//	        iter = (dynamic_cast <FlowScope *> (scope))->links.begin();
// 	        iter != (dynamic_cast <FlowScope *> (scope))->links.end(); 
//	        iter++)
      for (list<csLink*>::iterator 
		      iter = (dynamic_cast <FlowScope*> (currentScope))->links.begin();
		      iter != (dynamic_cast <FlowScope*> (currentScope))->links.end();
		      iter++)
      {
        if (!((*iter)->isSource && (*iter)->isTarget))
	{
	  problems = true;
          if ( (*iter)->isSource )
	  {
            trace("The Link " + (*iter)->name + " defined in line " 
		  + intToString((*iter)->line)+ " was used as source,"
		  + " but never as a target!\n");
	  }
	  else if ( (*iter)->isTarget )
	  {
            trace("The Link " + (*iter)->name + " defined in line " 
		  + intToString((*iter)->line)+ " was used as target,"
		  + " but never as a source!\n");
	  }
	  else
	  {
            trace("The Link " + (*iter)->name + " defined in line " 
		  + intToString((*iter)->line)+ " has never been used!\n");
	  }
	}
      }
      if (problems)
      {
      yyerror (string("The Flow finished in line " + intToString(yylineno)
		      + " has problems with the used links! (see above)\n").c_str());
      }
    }
    else
    {
      // you should only use this function in a FlowScope!
      //throw (bad_cast;
    }
  }
  catch (bad_cast)
  {
    throw Exception(CHECK_SYMBOLS_CAST_ERROR,"Dynamic cast error while checking Links\n"); 
  }

}

/**
 * Adds a channel to the list of #channels with type, under the condition that it is not in
 * the l list.
 *
 * \param channel	the channel to add
 * \param isInChannel 	true iff channel shall be an incoming channel, otherwise it shall be an outgoing channel
 * \return        	the unique channel name
 *
 */
kc::casestring SymbolManager::addChannel(csChannel * channel, bool isInChannel)
{

  std::string key = string(channel->name()->name);
  trace(TRACE_DEBUG, "[CS] Adding channel " + key + "\n"); 
  if (isInChannel)
  {
    trace(TRACE_VERY_DEBUG, "[CS]  --> incoming channel\n");
    inChannels.insert(key);
  }
  else
  {
    trace(TRACE_VERY_DEBUG, "[CS]  --> outgoinig channel\n");
    outChannels.insert(key);
  }
  // return unique name
  return channel->name();
}

/**
 * Prints recursively a very simple tree of the scopes.
 *
 */
void SymbolManager::printScope()
{
	
  if (processScope != NULL)
  {
    trace(TRACE_DEBUG, "\nPrinting scope tree (without Flows):\n\n");
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

FlowScope::~FlowScope()
{
  for( list<csLink*>::iterator iter = links.begin(); iter != links.end(); iter++)
  {
    trace(TRACE_VERY_DEBUG, "[CS]      Deleting link " + (*iter)->name + "\n");
    delete(*iter);
  }
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

/// \todo (gierds) comment me
csLink::csLink( string myname )
{
  name = myname;
  line = yylineno;
}

/// \todo (gierds) comment me
bool csLink::operator==(csLink& other)
{
  return (name == other.name);
}

/// \todo (gierds) comment me
csChannel::csChannel(string myportType, string myoperation, string mypartnerLink)
{
  portType    = myportType;
  operation   = myoperation;
  partnerLink = mypartnerLink;
}

/// our own equality
bool csChannel::operator==(csChannel& other)
{
  return (portType == other.portType && operation == other.operation && partnerLink == other.partnerLink);
}

kc::casestring csChannel::name()
{
  return kc::mkcasestring(string(partnerLink + "." + portType + "." + operation).c_str());
}

