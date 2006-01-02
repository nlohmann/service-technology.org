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
 *          - last changed: \$Date: 2006/01/02 20:14:48 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universit&auml;t zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel
 *          for details.
 *
 */

#include "check-symbols.h"

#include <stack>

extern std::string intToString(int); // little helper function (helpers.cc)
extern int yylineno;		     // line number from flex/bison
extern bool inWhile;		     // flag if in while activity (bpel-syntax.y)

/// variable to format scope tree output
int SymbolScope::indent = 0;

/// constructor for class SymbolManager
SymbolManager::SymbolManager()
{
  processScope = NULL;
  currentScope = NULL;
}

/// destructor for class SymbolManager
SymbolManager::~SymbolManager()
{
  // if != NULL we have scopes (should always be so)
  if (processScope != NULL)
  {
    // delete all scopes; they are all stored in #mapping
    for ( map<kc::integer, SymbolScope*>::iterator iter = mapping.begin();
		    iter != mapping.end();
		    iter++)
    {
      delete((*iter).second);
    }
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
  trace(TRACE_DEBUG, "[CS] - Leaving scope " 
		   + intToString(currentScope->id->value));

  if ( currentScope->parent != NULL )
  {
    currentScope = currentScope->parent;
    trace(TRACE_DEBUG, " -> new scope is " 
		      + intToString(currentScope->id->value) 
		      + " again\n");
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
  trace(TRACE_VERY_DEBUG, "[CS] Adding (" 
		         + intToString(currentScope->id->value) 
		         + ") PartnerLink " 
			 + pl->name + ", " + pl->partnerLinkType + ", "
		         + pl->myRole + ", " + pl->partnerRole + "\n");
  // since we want to add a PartnerLink, 
  // we assume currentScope is a ProcessScope
  try
  {
    // check, if PartnerLink name is unique, otherwise throw Exception
    for (list<csPartnerLink*>::iterator iter = 
	   (dynamic_cast <ProcessScope *> (currentScope))->partnerLinks.begin();
	  iter != 
	   (dynamic_cast <ProcessScope *> (currentScope))->partnerLinks.end(); 
	  iter++)
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
    throw Exception(CHECK_SYMBOLS_CAST_ERROR,
		    "Dynamic cast error while building scope tree\n",
		    "Seems node "
		   + intToString(currentScope->id->value) + " is no Process\n"); 
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
    trace(TRACE_VERY_DEBUG, 
	  "[CS] Checking PartnerLink, but no name is given; returning.\n");
    return;
  }

  trace(TRACE_DEBUG, "[CS] Checking PartnerLink " 
		    + pl->name + ", " + pl->partnerLinkType + ", "
		    + pl->myRole + ", " + pl->partnerRole + "\n");
  // since we want to add a PartnerLink, 
  // we assume currentScope is a ProcessScope
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
 	     
        trace(TRACE_VERY_DEBUG, "[CS]     ... leaving scope " 
			       + intToString(scope->id->value) + "\n");
        scope = scope->parent;
      }

      if (scope != NULL)
      {
        trace(TRACE_VERY_DEBUG, 
	      "[CS]   Looking for defined PartnerLinks ...\n");
        for (list<csPartnerLink*>::iterator iter = 
	       (dynamic_cast <ProcessScope *> (scope))->partnerLinks.begin();
 	      iter != (dynamic_cast <ProcessScope *> (scope))->partnerLinks.end(); 
	      iter++)
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
      yyerror(string("Name of undefined PartnerLink is \"" 
	             + pl->name + "\"\n").c_str());
    }
  }
  catch(bad_cast)
  {
    throw Exception(CHECK_SYMBOLS_CAST_ERROR,
		    "Dynamic cast error while checking PartnerLink\n",
		    "Seems node " + intToString(currentScope->id->value) 
		    + " is no Process\n"); 
  }
}

/**
 * Checks if PartnerLink is defined in scope.
 *
 * \param name Name of the PartnerLink to be checked
 *
 */
void SymbolManager::checkPartnerLink(std::string name)
{
  checkPartnerLink(new csPartnerLink(name, "", "", ""));
}


/**
 * Adds a new Variable to current scope
 *
 * \param var the variable to add
 * \return    the unique name of the variable
 *
 */
kc::casestring SymbolManager::addVariable(csVariable* var)
{
  std::string uniqueID;
	
  trace(TRACE_VERY_DEBUG, "[CS] Adding Variable " 
		         + var->name + ", " + var->messageType + ", "
		         + var->type + ", " + var->element + "\n");
  for (list<csVariable*>::iterator iter = currentScope->variables.begin();
	    iter != currentScope->variables.end(); 
	    iter++)
  {
    if (*(*iter) == *var)
    {
      throw Exception(DOUBLE_VARIABLE, "Two Variables with same name\n",
		      "Name of double Variable is \"" + var->name + "\"\n");
    }
  }
  // add to currentScope for later checking
  currentScope->variables.push_back(var);

  uniqueID = std::string(intToString(currentScope->id->value) 
		         + "." + var->name);
  // add unique name to global variable list
  variables.push_back(uniqueID);

  trace(TRACE_VERY_DEBUG, "[CS] Unique ID of Variable is " 
		          + uniqueID + "\n");
  return kc::mkcasestring(uniqueID.c_str());
}

/**
 * Checks, if Variable is defined in scope.
 *
 * \param var             the Variable to be checked
 * \param isFaultVariable true iff variable is FaultVariable
 *                        (which can be defined implicitly)
 * \return                the unique name of the variable
 *
 */
kc::casestring SymbolManager::checkVariable(csVariable* var, 
		                            bool isFaultVariable)
{
  int id = 0;
  std::string uniqueID;
  
  // if no name is set, there was probably no Variable given
  if (var->name == "")
  {
    trace(TRACE_VERY_DEBUG, 
          "[CS] Checking Variable, but no name is given; returning.\n");
    return kc::mkcasestring(string().c_str());
  }

  trace(TRACE_DEBUG, "[CS] Checking Variable " 
		    + var->name + ", " + var->messageType + ", "
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
      while ((scope != NULL) && 
	     (typeid(*scope) != typeid(ProcessScope)) && 
	     (typeid(*scope) != typeid(ScopeScope)))
      {
        trace(TRACE_VERY_DEBUG, "[CS]     typeids are ");
        trace(TRACE_VERY_DEBUG, typeid(*scope).name());
        trace(TRACE_VERY_DEBUG, "\n");
 	     
        trace(TRACE_VERY_DEBUG, "[CS]     ... leaving scope " 
			       + intToString(scope->id->value) + "\n");
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
      yyerror(string("Name of undefined Variable is \"" 
		     + var->name + "\"\n").c_str());
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
    throw Exception(CHECK_SYMBOLS_CAST_ERROR,
		    "Dynamic cast error while checking Variables\n"); 
  }

  trace(TRACE_VERY_DEBUG, "[CS] Unique ID of Variable is " 
		         + std::string(intToString(id) + "." 
			 + var->name) + "\n");
  return kc::mkcasestring(uniqueID.c_str());
  
}

/**
 * Checks if Variable is defined in scope.
 *
 * \param name            name of the variable to be checked
 * \param isFaultVariable true iff variable is FaultVariable
 *                        (which can be defined implicitly)
 * \return                the unique name of the variable
 *
 */
kc::casestring SymbolManager::checkVariable(std::string name,
	       				    bool isFaultVariable)
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
    trace(TRACE_VERY_DEBUG,
	  "[CS] Checking Link, but no name is given; returning.\n");
    return kc::mkcasestring(string().c_str());
  }

  trace(TRACE_DEBUG, "[CS] Checking Link " + link->name + "\n");
  try
  {
    // check, if Link name is present, otherwise throw Exception
    bool found = false;
    SymbolScope * scope = currentScope;
    while ((! found) && (scope != NULL))
    {
      // ascent to next scope with link definitions
      trace(TRACE_VERY_DEBUG, "[CS]   Ascending to next suitable scope ...\n");
      while ((scope != NULL) && (typeid(*scope) != typeid(FlowScope)))
      {
        trace(TRACE_VERY_DEBUG, "[CS]     typeids are ");
        trace(TRACE_VERY_DEBUG, typeid(*scope).name());
        trace(TRACE_VERY_DEBUG, " and ");
        trace(TRACE_VERY_DEBUG, typeid(FlowScope).name());
        trace(TRACE_VERY_DEBUG, "\n");
 	     
        trace(TRACE_VERY_DEBUG, 
	      "[CS]     ... leaving scope " 
	      + intToString(scope->id->value) + "\n");
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
                yyerror(string("Link \"" + link->name 
			       + "\" was already used as source\n").c_str());
	      }
	      (*iter)->isSource = true;
	    }
	    else
	    {
              if ((*iter)->isTarget)
	      {
                yyerror(string("Link \"" + link->name 
			       + "\" was already used as target\n").c_str());
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
      yyerror(string("Name of undefined Link is \"" 
		     + link->name + "\"\n").c_str());
    }
    // if there is a surrounding scope, tell it that the link exists
    if (asSource)
    {
      scope = currentScope;
      while( (scope != NULL) && (typeid(*scope) != typeid(ScopeScope)) )
      {
        scope = scope->parent;
      }
      if ( (scope != NULL) && typeid(*scope) == typeid(ScopeScope))
      {
        trace(TRACE_VERY_DEBUG, "[CS]   Adding " + uniqueID 
	  	              + " to list of inner links for Scope " 
			      + intToString(scope->id->value) + "\n");
        (dynamic_cast <ScopeScope *> (scope))->innerLinks.push_back(uniqueID);
      }
    }
  }
  catch(bad_cast)
  {
    throw Exception(CHECK_SYMBOLS_CAST_ERROR,
		    "Dynamic cast error while checking Links\n"); 
  }

  trace(TRACE_VERY_DEBUG, "[CS] Unique ID of Link is " 
		         + std::string(intToString(id) + "." + link->name) 
			 + "\n");
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
      for (list<csLink*>::iterator iter = 
	      (dynamic_cast <FlowScope*> (currentScope))->links.begin();
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
		      + " has problems with the used links! (see above)\n"
		     ).c_str());
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
    throw Exception(CHECK_SYMBOLS_CAST_ERROR,
		    "Dynamic cast error while checking Links\n"); 
  }

}

/**
 * Adds a channel to the list of #channels with type of channel
 *
 * \param channel	the channel to add
 * \param isInChannel 	true iff channel shall be an incoming channel,
 *                      otherwise it shall be an outgoing channel
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

/* --- Dead Path Elimination --- */
/// add a possible start for DPE
void SymbolManager::addDPEstart()
{
  trace(TRACE_VERY_DEBUG, "[CS] DPE: Adding possible start\n");
  dpePossibleStarts++;
}

/// add a possible end for DPE
void SymbolManager::addDPEend()
{
  trace(TRACE_VERY_DEBUG, "[CS] DPE: Adding possible end\n");
  dpePossibleEnds++;
}

/// remove a possible start for DPE
void SymbolManager::remDPEstart()
{
  trace(TRACE_VERY_DEBUG, "[CS] DPE: Removing possible start\n");
  dpePossibleStarts--;
}

/// remove a possible end for DPE
void SymbolManager::remDPEend()
{
  trace(TRACE_VERY_DEBUG, "[CS] DPE: Removing possible end(s)\n");
  dpePossibleEnds--;
}

/// set number of possible DPE ends to 0
void SymbolManager::resetDPEend()
{
  trace(TRACE_VERY_DEBUG, "[CS] DPE: Resetting number of possible ends\n");
  dpePossibleEnds = 0;
}

/// links are not allowed to cross the borders of whiles, so special treatment
void SymbolManager::startDPEinWhile()
{
  trace(TRACE_VERY_DEBUG, "[CS] DPE: Entering while\n");
  dpeStartStack.push(dpePossibleStarts);
  dpePossibleStarts = 0;
  dpePossibleEnds = 0;
}

/// links are not allowed to cross the borders of whiles, so special treatment
void SymbolManager::endDPEinWhile()
{
  trace(TRACE_VERY_DEBUG, "[CS] DPE: Leaving while\n");
  dpePossibleStarts = dpeStartStack.top();
  dpeStartStack.pop();
  dpePossibleEnds = 0;
}

/// checks if weed need a negLink under current conditions
kc::integer SymbolManager::needsDPE()
{
  // if we have possible starts and ends, we need a negLink and return 1
  if ( (dpePossibleEnds > 0) && (dpePossibleStarts > 0))
  {
    trace(TRACE_DEBUG, "[CS] DPE: negLink needed\n");
    trace(TRACE_VERY_DEBUG, "[CS] DPE: possible starts: " 
		           + intToString(dpePossibleStarts) + "\n"
		           + "          possible ends: " 
			   + intToString(dpePossibleEnds)
			   + "\n");
    return kc::mkinteger(1);
  }
  // no need for a negLink, so return 0
  trace(TRACE_DEBUG, "[CS] DPE: no negLink needed\n");
  trace(TRACE_VERY_DEBUG, "[CS] DPE: possible starts: " 
		         + intToString(dpePossibleStarts) + "\n"
		         + "          possible ends: " 
			 + intToString(dpePossibleEnds) + "\n");
  return kc::mkinteger(0);
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

/**
 * Constructor for a general scope (without a given parent scope)
 *
 * \param myid the associated AST id
 *
 */
SymbolScope::SymbolScope(kc::integer myid)
{
  trace(TRACE_DEBUG, "[CS] Creating new scope without parent for action "
		    + intToString((int) myid->value) + "\n");
 
  id = myid;
  parent = NULL;

}

/**
 * Constructor for a general scope (with a given parent scope)
 *
 * \param myid     the associated AST id
 * \param myparent pointer to the parent scope
 *
 */
SymbolScope::SymbolScope(kc::integer myid, SymbolScope * myparent)
{
  trace(TRACE_DEBUG, "[CS] Creating new scope with parent " 
		    + intToString((int) (myparent->id)->value) + " for action " 
		    + intToString((int) myid->value) + "\n");

  id = myid;
  parent = myparent;
  parent->children.push_back(this);

}

/**
 * Destructor for a scope, cleans up the variables
 *
 */
SymbolScope::~SymbolScope()
{
  trace(TRACE_DEBUG, "[CS]   ~ Destructing scope starting in " 
		    + intToString( id->value ) + "\n");
  if ( variables.empty() ) 
  {
    trace(TRACE_VERY_DEBUG, "[CS]      No variables.\n");     
  }
  for ( list<csVariable*>::iterator elem = variables.begin(); 
        elem != variables.end(); 
	elem++)
  {
    trace(TRACE_VERY_DEBUG, "[CS]      Deleting variable " 
		           + (*elem)->name + "\n");
    delete(*elem);
  }
}

/**
 * Prints a hierarchical picture of the scopes
 *
 */
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
  for (list<SymbolScope*>::iterator iter = children.begin(); 
       iter != children.end(); 
       iter++)
  {
    (*iter)->print();
  }
  indent -= 4;
}


/**
 * Constructor for a Process scope (without a given parent scope)
 *
 * \param myid     the associated AST id
 *
 */
ProcessScope::ProcessScope(kc::integer myid) : SymbolScope(myid)
{
  // empty	
}

/**
 * Constructor for a Process scope (with a given parent scope)
 *
 * \param myid     the associated AST id
 * \param myparent pointer to the parent scope
 *
 */
ProcessScope::ProcessScope(kc::integer myid, SymbolScope* myparent) : 
	SymbolScope(myid, myparent)
{
  // empty
}

/**
 * Constructor for a Scope scope (without a given parent scope)
 *
 * \param myid     the associated AST id
 *
 */
ScopeScope::ScopeScope(kc::integer myid) : SymbolScope(myid)
{
  // empty	
}

/**
 * Constructor for a Scope scope (with a given parent scope)
 *
 * \param myid     the associated AST id
 * \param myparent pointer to the parent scope
 *
 */
ScopeScope::ScopeScope(kc::integer myid, SymbolScope* myparent) : 
	SymbolScope(myid, myparent)
{
  // empty
}

/**
 * Constructor for a Flow scope (without a given parent scope)
 *
 * \param myid     the associated AST id
 *
 */
FlowScope::FlowScope(kc::integer myid) : SymbolScope(myid)
{
  // empty	
}

/**
 * Constructor for a Flow scope (with a given parent scope)
 *
 * \param myid     the associated AST id
 * \param myparent pointer to the parent scope
 *
 */
FlowScope::FlowScope(kc::integer myid, SymbolScope* myparent) : SymbolScope(myid, myparent)
{
  // empty
}

/**
 * Special destructor for Flow scopes in order to delete possible links.
 *
 */
FlowScope::~FlowScope()
{
  for( list<csLink*>::iterator iter = links.begin(); 
       iter != links.end(); 
       iter++)
  {
    trace(TRACE_VERY_DEBUG, "[CS]      Deleting link " + (*iter)->name + "\n");
    delete(*iter);
  }
}

/**
 * Constructor for a new PartnerLink
 *
 * \param myname        parameter for attribute name
 * \param mytype        parameter for attribute type
 * \param mymyrole      parameter for attribute myRole
 * \param mypartnerRole parameter for attribute partnerRole
 *
 */
csPartnerLink::csPartnerLink ( string myname, string mytype, 
		               string mymyrole, string mypartnerRole)
{
  name            = myname;
  partnerLinkType = mytype;
  myRole          = mymyrole;
  partnerRole     = mypartnerRole;

}

/**
 * Operator == for checking equality where *this == other 
 * iff attribute name is equal
 *
 * \param other the PartnerLink to check for equality
 * \return      true iff equal
 *
 */
bool csPartnerLink::operator==(csPartnerLink& other)
{
  return (name == other.name); 
}

/**
 * Constructor for a new Variable
 *
 * \param myname        parameter for attribute name
 * \param mymessageType parameter for attribute messageType
 * \param mytype        parameter for attribute type
 * \param myelement     parameter for attribute element
 * 
 */
csVariable::csVariable ( string myname, string mymessageType, string mytype, string myelement)
{
  name        = myname;
  messageType = mymessageType;
  type        = mytype;
  element     = myelement;

}

/**
 * Operator == for checking equality where *this == other 
 * iff attribute name is equal
 *
 * \param other the Variable to check for equality
 * \return      true iff equal
 *
 */
bool csVariable::operator==(csVariable& other)
{
  return (name == other.name);
}

/**
 *  Constructor for a new Link
 *
 *  \param myname parameter for the attribute name
 *
 */
csLink::csLink( string myname )
{
  name = myname;
  line = yylineno;
}

/**
 * Operator == for checking equality where *this == other 
 * iff attribute name is equal
 *
 * \param other the Link to check for equality
 * \return      true iff equal
 *
 */
bool csLink::operator==(csLink& other)
{
  return (name == other.name);
}

/**
 * Constructor for a new Channel
 * 
 * \param myportType    parameter for the attribute portType
 * \param myoperation   parameter for the attribute operation
 * \param mypartnerLink parameter for the attribute partnerLink
 *
 */
csChannel::csChannel(string myportType, string myoperation, string mypartnerLink)
{
  portType    = myportType;
  operation   = myoperation;
  partnerLink = mypartnerLink;
}

/**
 * Operator == for checking equality where *this == other 
 * iff attributes portType, operation and partnerLink are equal
 *
 * \param other the channel to check for equality
 * \return      true iff equal
 *
 */
bool csChannel::operator==(csChannel& other)
{
  return (portType == other.portType && operation == other.operation && partnerLink == other.partnerLink);
}

/**
 * Handles the naming of channels (not necessarily unique)
 *
 * \return the name of the channel
 *
 */
kc::casestring csChannel::name()
{
  return kc::mkcasestring(string(partnerLink + "." + portType + "." + operation).c_str());
}

