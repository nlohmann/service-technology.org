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
 * \version \$Revision: 
 *
 */

#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <map>
#include <list>

#include "bpel-attributes.h"

// forward declaration of classes
class Activity;
class Attribute;
class CompensationHandler;
class CorrelationSet;
class CorrelationSets;
class Element;
class Envelope;
class EventHandlers;
class FaultHandlers;
class PartnerLink;
class PartnerLinks;
class Process;
class Scope;
class SymbolTable;
class SymbolTableEntry;
class Variable;
class Variables;

class SymbolTableEntry
{
  public:

    ///     
    unsigned int entryId;

    /// BPEL-element Id
    unsigned int elementId;
    
    /// cast information
    std::string elementType;
    
    /// contructor
    SymbolTableEntry();
    
    /// destructor
    ~SymbolTableEntry();
    
    std::string getElementId();
    
    std::string getElementType();    
  
};

class Element
{
  public:
    ///
    std::list<Attribute*> listOfAttributes;
    
    /// line position of element within bpel file
    unsigned int line;
    
    ///
    unsigned int negativeControlFlow; 
};

class Attribute
{
  public:
	/// name of attribute
    kc::casestring name;
    
    /// attribute value
    kc::casestring value;
    
    /// type of attribute value
    std::string type;
    
    /// line position of attribute within bpel file
    unsigned int line;
};

class Variables
{
  public:
    ///
    std::list<Variable*> listOfVariables;
};

class Variable: public Element
{
  public:
    /// 
    bool used;  
};

class PartnerLinks
{
  public:
    ///
    std::list<PartnerLink*> listOfPartnerLinks;    	
};

class PartnerLink: public Element
{
  public:
    ///
};

class CorrelationSets
{
  public:
    ///
    std::list<CorrelationSet*> listOfCorrelationSets;    	
};

class CorrelationSet: public Element
{
  public:
    ///
};

class FaultHandlers
{
  public:
    ///
    
};

class CompensationHandler
{
  public:
    ///
};

class EventHandlers
{
  public:
    ///
};

class Envelope
{
  public:
    ///
    Variables* var;
    
    ///
    CorrelationSets cor;
    
    ///
    FaultHandlers* fHandler;
    
    ///
    CompensationHandler* cHandler;  
    
    ///
    EventHandlers* eHandler;
};

class Process: public Element, Envelope, SymbolTableEntry
{
  public:
    ///
    PartnerLinks* links;
};

class Scope: public Element, Envelope, SymbolTableEntry
{
  public:
    /// additional attribute used for inter-scope communication (push-places)
    unsigned int parentScopeId;
    
    /// has this scope an event handler?
    bool hasEH;
};

class Activity: public Element, SymbolTableEntry
{
  public:
    ///
};

class SymbolTable
{
  private:
    /// a container to store ...
    std::map<unsigned int, SymbolTableEntry*> symTab;
    
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

