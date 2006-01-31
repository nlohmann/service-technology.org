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
 * \file symbol-table.cc
 *
 * \brief
 *
 * \author  
 *          
 * \date
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universit&auml;t zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel
 *          for details.
 *
 */ 

#include "symbol-table.h"
#include "bpel-attributes.h"
#include "bpel-syntax.h"
#include "debug.h"

/********************************************
 * implementation of SymbolTable CLASS
 ********************************************/

/*!
 * constructor
 */
SymbolTable::SymbolTable()
{
  this->symTab.clear();
  this->entryKey = 0;
}

/*!
 * destructor
 */
SymbolTable::~SymbolTable() {}

/*!
 * increase the entryId of the SymbolTable 
 */
unsigned int SymbolTable::nextKey()
{
  this->entryKey++;
  return this->entryKey;
}

/*!
 * 
 */
unsigned int SymbolTable::insert(unsigned int elementId)
{
  switch(elementId)
  {
    case K_PROCESS:
    {
      traceST("PROCESS\n");
      symTab[this->nextKey()] = new STProcess(elementId, this->entryKey);
//      traceST("--> " + intToString(this->entryKey) + "\n");
    }
    break;
    
    case K_SCOPE:
    {
      traceST("Scope\n");
//      symTab[this->nextKey()] = new STScope(elementId);      
    }
    break;

    default : /* activities */
    {
      traceST("activities\n");
//      symTab[this->nextKey()] = new STActivity(elementId);      
    }
    break;
  }
  
  return this->entryKey;

}

/*!
 * 
 */
SymbolTableEntry* SymbolTable::lookup(unsigned int entryKey)
{
//  traceST("lookup -> " + intToString((dynamic_cast <SymbolTableEntry*> (symTab[entryKey]))->entryKey) + "\n");
  return (dynamic_cast <SymbolTableEntry*> (symTab[entryKey])); 
}

/*!
 * 
 */
unsigned int SymbolTable::getCurrentEntryKey()
{
  return this->entryKey;
}

/*!
 * 
 */
STAttribute* SymbolTable::newAttribute(kc::casestring name, kc::casestring value)
{
  return (new STAttribute(name, value));
}

/*!
 * 
 */
void SymbolTable::addAttribute(unsigned int entryKey, STAttribute* attribute)
{
  ///
  switch((dynamic_cast <SymbolTableEntry*> (symTab[entryKey]))->elementId)
  {
    case K_PROCESS:
    {
//      traceST("cast to STProcess\n");
      (dynamic_cast <STProcess*> (symTab[entryKey]))->mapOfAttributes[attribute->name->name] = attribute;
      break;
    }
    case K_SCOPE:
    {
//      traceST("cast to STScope\n");
      (dynamic_cast <STScope*> (symTab[entryKey]))->mapOfAttributes[attribute->name->name] = attribute;
      break;
    }    
    default:
    { /* cast to Activity */
//      traceST("cast to STActivity\n");    	
      (dynamic_cast <STActivity*> (symTab[entryKey]))->mapOfAttributes[attribute->name->name] = attribute;
      break;
    }
  }
}

/*!
 * 
 */
STAttribute* SymbolTable::readAttribute(unsigned int entryKey, string name)
{
  ///
  switch((dynamic_cast <SymbolTableEntry*> (symTab[entryKey]))->elementId)
  {
    case K_PROCESS:
    {
//      traceST("cast to STProcess\n");
      return (dynamic_cast <STProcess*> (symTab[entryKey]))->mapOfAttributes[name];
      break;
    }
    case K_SCOPE:
    {
//      traceST("cast to STScope\n");
      return (dynamic_cast <STScope*> (symTab[entryKey]))->mapOfAttributes[name];
      break;
    }    
    default:
    { /* cast to Activity */
//      traceST("cast to STActivity\n");    	
      return (dynamic_cast <STActivity*> (symTab[entryKey]))->mapOfAttributes[name];
      break;
    }
  }
} 

/*!
 * trace method for the ST class
 * \param traceMsg
 */
void SymbolTable::traceST(string traceMsg)
{
  trace(TRACE_DEBUG, "[ST] " + traceMsg);
}


/********************************************
 * implementation of SymbolTableEntry CLASS
 ********************************************/
 
/*!
 * constructor
 */
SymbolTableEntry::SymbolTableEntry(unsigned int elementId)
{
  setElementId(elementId);
}


/*!
 * constructor
 */
SymbolTableEntry::SymbolTableEntry(unsigned int elementId, unsigned int entryKey)
{
  setElementId(elementId);
  setEntryKey(entryKey);
}


/*!
 * destructor
 */
SymbolTableEntry::~SymbolTableEntry() {}

/*!
 * 
 */
void SymbolTableEntry::setElementId(unsigned int id)
{
  this->elementId = id;
}

/*!
 * 
 */
void SymbolTableEntry::setEntryKey(unsigned int key)
{
  this->entryKey = key;
}
 
/*!
 * 
 */
unsigned int SymbolTableEntry::getElementId()
{
  return this->elementId;
}

/*!
 * 
 */
unsigned int SymbolTableEntry::getEntryKey()
{
  return this->entryKey;
}

/********************************************
 * implementation of Activity CLASS
 ********************************************/

/*!
 * constructor
 */
STAttribute::STAttribute(kc::casestring name, kc::casestring value)
{
  this->name = name;
  this->value = value;
  this->line = 0;
}

/*!
 * destructor
 */
STAttribute::~STAttribute() {}
 
/********************************************
 * implementation of Attribute CLASS
 ********************************************/
 
/********************************************
 * implementation of CompensationHandler CLASS
 ********************************************/
 
/********************************************
 * implementation of CorrelationSet CLASS
 ********************************************/
 
/********************************************
 * implementation of Element CLASS
 ********************************************/
 
/********************************************
 * implementation of Envelope CLASS
 ********************************************/
 
/********************************************
 * implementation of EventHandlers CLASS
 ********************************************/
 
/********************************************
 * implementation of FaultHandlers CLASS
 ********************************************/
 
/********************************************
 * implementation of Link CLASS
 ********************************************/
 
/********************************************
 * implementation of PartnerLink CLASS
 ********************************************/
 
/********************************************
 * implementation of Process CLASS
 ********************************************/

/*!
 * constructor
 */
STProcess::STProcess(unsigned int elementId) :SymbolTableEntry(elementId) {}

STProcess::STProcess(unsigned int elementId, unsigned int entryKey)
 :SymbolTableEntry(elementId, entryKey) {}
 
/********************************************
 * implementation of Scope CLASS
 ********************************************/
 
/********************************************
 * implementation of Variable CLASS
 ********************************************/

/*!
 * constructor
 */
STVariable::STVariable() {}

/*!
 * destructor
 */
STVariable::~STVariable() {}

