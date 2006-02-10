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
  this->id2key.clear();
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
void SymbolTable::setMapping(unsigned int entryKey, kc::integer astId)
{
  id2key[astId->value] = entryKey;	
}

/*!
 * 
 */
string SymbolTable::getInformation(kc::integer astId, bool closeTag)
{
  string result = "<";
  
  if (closeTag)
    result += "/";
  
  result += translateToElementName((lookup(id2key[astId->value]))->getElementId());
  result += " id=\"" + intToString(astId->value) + "\">";
  
  return result;
}

/*!
 * 
 */
string SymbolTable::translateToElementName(unsigned int elementId)
{
  //traceST("translateToElementName:\n");
  switch(elementId)
  {
    case K_ASSIGN:	return "assign";
    case K_CATCH:	return "catch";
    case K_COMPENSATE:	return "compensate";
    case K_CORRELATION:	return "correlation";
    case K_CORRELATIONSET: return "correlationSet";
    case K_EMPTY:	return "empty";
    case K_FLOW:	return "flow";
    case K_INVOKE:	return "invoke";
    case K_LINK:	return "link";
    case K_ONALARM:	return "onAlarm";
    case K_ONMESSAGE:	return "onMessage";
    case K_PARTNER:	return "partner";
    case K_PARTNERLINK:	return "partnerLink";
    case K_PICK:	return "pick";
    case K_PROCESS:	return "process";
    case K_RECEIVE:	return "receive";
    case K_REPLY:	return "reply";
    case K_SCOPE:	return "scope";
    case K_SEQUENCE:	return "sequence";
    case K_SOURCE:	return "source";
    case K_SWITCH:	return "switch";
    case K_TARGET:	return "target";
    case K_TERMINATE:	return "terminate";
    case K_VARIABLE:	return "variable";
    case K_WAIT:	return "wait";
    case K_WHILE:	return "while";
    default:		return "unknown";
  }
} 



/*!
 * 
 */
unsigned int SymbolTable::insert(unsigned int elementId)
{
  switch(elementId)
  {
    case K_LINK:
    {
      traceST("LINK\n");
      symTab[this->nextKey()] = new STLink(elementId, this->entryKey);
//      traceST("--> " + intToString(this->entryKey) + "\n");
    }

    break;

    case K_PARTNER:
    {
      traceST("PARTNER\n");
      symTab[this->nextKey()] = new STPartner(elementId, this->entryKey);
//      traceST("--> " + intToString(this->entryKey) + "\n");
    }
    break;

    case K_PARTNERLINK:
    {
      traceST("PARTNERLINK\n");
      symTab[this->nextKey()] = new STPartnerLink(elementId, this->entryKey);
//      traceST("--> " + intToString(this->entryKey) + "\n");
    }
    break;

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
      symTab[this->nextKey()] = new STScope(elementId, this->entryKey);
//      traceST("--> " + intToString(this->entryKey) + "\n");      
    }
    break;

    case K_VARIABLE:
    {
      traceST("VARIABLE\n");
      symTab[this->nextKey()] = new STVariable(elementId, this->entryKey);
//      traceST("--> " + intToString(this->entryKey) + "\n");
    }
    break;

    default : /* activities */
    {
      traceST("activities\n");
      symTab[this->nextKey()] = new STActivity(elementId, this->entryKey);
//      traceST("--> " + intToString(this->entryKey) + "\n");      
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
    case K_LINK:
    {
//      traceST("cast to STLink\n");
      (dynamic_cast <STLink*> (symTab[entryKey]))->mapOfAttributes[attribute->name->name] = attribute;
      break;
    }

    case K_PARTNER:
    {
//      traceST("cast to STPartner\n");
      (dynamic_cast <STPartner*> (symTab[entryKey]))->mapOfAttributes[attribute->name->name] = attribute;
      break;
    }

    case K_PARTNERLINK:
    {
//      traceST("cast to STPartnerLink\n");
      (dynamic_cast <STPartnerLink*> (symTab[entryKey]))->mapOfAttributes[attribute->name->name] = attribute;
      break;
    }

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

    case K_VARIABLE:
    {
//      traceST("cast to STProcess\n");
      (dynamic_cast <STVariable*> (symTab[entryKey]))->mapOfAttributes[attribute->name->name] = attribute;
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
    case K_LINK:
    {
//      traceST("cast to STLink\n");
      return (dynamic_cast <STLink*> (symTab[entryKey]))->mapOfAttributes[name];
      break;
    }

    case K_PARTNER:
    {
//      traceST("cast to STPartner\n");
      return (dynamic_cast <STPartner*> (symTab[entryKey]))->mapOfAttributes[name];
      break;
    }

    case K_PARTNERLINK:
    {
//      traceST("cast to STPartnerLink\n");
      return (dynamic_cast <STPartnerLink*> (symTab[entryKey]))->mapOfAttributes[name];
      break;
    }

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

    case K_VARIABLE:
    {
//      traceST("cast to STPartnerLink\n");
      return (dynamic_cast <STVariable*> (symTab[entryKey]))->mapOfAttributes[name];
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
STActivity::STActivity(unsigned int elementId, unsigned int entryKey)
 :SymbolTableEntry(elementId, entryKey) {}

/*!
 * destructor
 */
STActivity::~STActivity() {}
 
/********************************************
 * implementation of Attribute CLASS
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

/*!
 * constructor
 */
STLink::STLink(unsigned int elementId, unsigned int entryKey)
 :SymbolTableEntry(elementId, entryKey) {}

/*!
 * destructor
 */
STLink::~STLink() {}

/********************************************
 * implementation of Partner CLASS
 ********************************************/

/*!
 * constructor
 */
STPartner::STPartner(unsigned int elementId, unsigned int entryKey)
 :SymbolTableEntry(elementId, entryKey) {}

/*!
 * destructor
 */
STPartner::~STPartner() {}

 
/********************************************
 * implementation of PartnerLink CLASS
 ********************************************/

/*!
 * constructor
 */
STPartnerLink::STPartnerLink(unsigned int elementId, unsigned int entryKey)
 :SymbolTableEntry(elementId, entryKey) {}

/*!
 * destructor
 */
STPartnerLink::~STPartnerLink() {}

/********************************************
 * implementation of Process CLASS
 ********************************************/

/*!
 * constructor
 */
STProcess::STProcess(unsigned int elementId, unsigned int entryKey)
 :SymbolTableEntry(elementId, entryKey) {}

/*!
 * destructor
 */
STProcess::~STProcess() {}

/********************************************
 * implementation of Scope CLASS
 ********************************************/

/*!
 * constructor
 */
STScope::STScope(unsigned int elementId, unsigned int entryKey)
 :SymbolTableEntry(elementId, entryKey) {}

/*!
 * destructor
 */
STScope::~STScope() {}
 
/********************************************
 * implementation of Variable CLASS
 ********************************************/

/*!
 * constructor
 */
STVariable::STVariable(unsigned int elementId, unsigned int entryKey)
 :SymbolTableEntry(elementId, entryKey) {}

/*!
 * destructor
 */
STVariable::~STVariable() {}
