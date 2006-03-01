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
 * \file    symbol-table.cc
 *
 * \brief   This file implements the classes and functions defined
 *          in symbol-table.h.
 *
 * \author
 *          - responsible: Dennis Reinert <reinert@informatik.hu-berlin.de>
 *          - last changes of: \$Author: reinert $  
 *          
 * \date
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universit&auml;t zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel
 *          for details.
 *
 * \todo
 *          - (ready) check <checkAttributes(FROM)> (special case with literals)
 *          - (ready) attention ... checkAttributes(kc::integer astId)and
 *            checkAttributes(kc::integer entryKey) <- NEW
 *          - (3/4) attribute value check -> empty then default value
 *                  suppressJoinFailure is missing
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
 * set mapping between AST id and symbol table entry key
 * \param entryKey  symbol table entry key
 * \astId           id of an AST (astract syntax tree) element
 */
void SymbolTable::setMapping(unsigned int entryKey, kc::integer astId)
{
  id2key[astId->value] = entryKey;	
}

/*!
 * \param astId  id of an AST (astract syntax tree) element
 */
unsigned int SymbolTable::idToKey(kc::integer astId)
{
  return id2key[astId->value];
} 

/*!
 * \param astId
 * \param closeTag 
 */
string SymbolTable::getInformation(kc::integer astId, bool closeTag)
{
  string result = "<";
  
  if (closeTag)
    result += "/";
  
  result += translateToElementName((lookup(idToKey(astId)))->getElementId());
  result += " id=\"" + intToString(astId->value) + "\">";
  
  return result;
}

/*!
 * retranslate the internal parse id of BPEL element to the real BPEL string
 * \param elementId BPEL element-id
 */
string SymbolTable::translateToElementName(unsigned int elementId)
{
  //traceST("translateToElementName:\n");
  switch(elementId)
  {
    case K_ASSIGN:	return "assign";
    case K_CASE:	return "case"; //now running under activity
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
 * create a new special BPEL element-object and insert these into symbol table
 * \param elementId  BPEL element-id (e.g. K_PROCESS)
 */
unsigned int SymbolTable::insert(unsigned int elementId)
{
  traceST("insert(" + translateToElementName(elementId) + ")\n");
  switch(elementId)
  {
    case K_COMPENSATE:
    {
//      traceST("COMPENSATE\n");
      symTab[this->nextKey()] = new STCompensate(elementId, this->entryKey);
//      traceST("--> " + intToString(this->entryKey) + "\n");
    }
    break;

    case K_INVOKE:
    {
//      traceST("INVOKE\n");
      symTab[this->nextKey()] = new STInvoke(elementId, this->entryKey);
//      traceST("--> " + intToString(this->entryKey) + "\n");
    }
    break;

    case K_LINK:
    {
//      traceST("LINK\n");
      symTab[this->nextKey()] = new STLink(elementId, this->entryKey);
//      traceST("--> " + intToString(this->entryKey) + "\n");
    }
    break;

    case K_PARTNER:
    {
//      traceST("PARTNER\n");
      symTab[this->nextKey()] = new STPartner(elementId, this->entryKey);
//      traceST("--> " + intToString(this->entryKey) + "\n");
    }
    break;

    case K_PARTNERLINK:
    {
//      traceST("PARTNERLINK\n");
      symTab[this->nextKey()] = new STPartnerLink(elementId, this->entryKey);
//      traceST("--> " + intToString(this->entryKey) + "\n");
    }
    break;

    case K_PROCESS:
    {
//      traceST("PROCESS\n");
      symTab[this->nextKey()] = new STProcess(elementId, this->entryKey);
//      traceST("--> " + intToString(this->entryKey) + "\n");
    }
    break;

    case K_RECEIVE:
    {
//      traceST("RECEIVE\n");
      symTab[this->nextKey()] = new STReceive(elementId, this->entryKey);
//      traceST("--> " + intToString(this->entryKey) + "\n");
    }
    break;

    case K_REPLY:
    {
//      traceST("REPLY\n");
      symTab[this->nextKey()] = new STReply(elementId, this->entryKey);
//      traceST("--> " + intToString(this->entryKey) + "\n");
    }
    break;
  
    case K_SCOPE:
    {
//      traceST("Scope\n");
      symTab[this->nextKey()] = new STScope(elementId, this->entryKey);
//      traceST("--> " + intToString(this->entryKey) + "\n");      
    }
    break;

    case K_TERMINATE:
    {
//      traceST("TERMINATE\n");
      symTab[this->nextKey()] = new STTerminate(elementId, this->entryKey);
//      traceST("--> " + intToString(this->entryKey) + "\n");
    }
    break;
    
    case K_VARIABLE:
    {
//      traceST("VARIABLE\n");
      symTab[this->nextKey()] = new STVariable(elementId, this->entryKey);
//      traceST("--> " + intToString(this->entryKey) + "\n");
    }
    break;

    case K_WAIT:
    {
//      traceST("WAIT\n");
      symTab[this->nextKey()] = new STWait(elementId, this->entryKey);
//      traceST("--> " + intToString(this->entryKey) + "\n");
    }
    break;

    default : /* activities */
    {
//      traceST("activities\n");
      symTab[this->nextKey()] = new STActivity(elementId, this->entryKey);
//      traceST("--> " + intToString(this->entryKey) + "\n");      
    }
    break;
  }
  
  return this->entryKey;

}

/*!
 * return symbol table entry
 * \param entryKey  symbol table entry key
 */
SymbolTableEntry* SymbolTable::lookup(unsigned int entryKey)
{
  traceST("lookup(entryKey=" + intToString((dynamic_cast <SymbolTableEntry*> (symTab[entryKey]))->entryKey) + ")\n");
  return (dynamic_cast <SymbolTableEntry*> (symTab[entryKey])); 
}

/*!
 * return
 */
unsigned int SymbolTable::getCurrentEntryKey()
{
  return this->entryKey;
}

/*!
 * create a new attribute object and return a pointer of these
 * \param name  name of attribute
 * \param value value of attribute
 */
STAttribute* SymbolTable::newAttribute(kc::casestring name, kc::casestring value)
{
  return (new STAttribute(name->name, value->name));
}

/*!
 * add an attribute object to an existend symbol table entry
 * \param entryKey  symbol table entry Key
 * \param attribute attribute object
 */
void SymbolTable::addAttribute(unsigned int entryKey, STAttribute* attribute)
{
  traceST("addAttribute(entryKey=" + intToString(entryKey) + ", [name=" + attribute->name + ", value=" + attribute->value + "])\n");	
  ///
  switch((dynamic_cast <SymbolTableEntry*> (symTab[entryKey]))->elementId)
  {
    case K_COMPENSATE:
    {
//     traceST("cast to STCompensate\n");
      (dynamic_cast <STCompensate*> (symTab[entryKey]))->mapOfAttributes[attribute->name] = attribute;
      break;
    }

    case K_INVOKE:
    {
//      traceST("cast to STInvoke\n");
      (dynamic_cast <STInvoke*> (symTab[entryKey]))->mapOfAttributes[attribute->name] = attribute;
      break;
    }
    
    case K_LINK:
    {
//      traceST("cast to STLink\n");
      (dynamic_cast <STLink*> (symTab[entryKey]))->mapOfAttributes[attribute->name] = attribute;
      break;
    }

    case K_PARTNER:
    {
//      traceST("cast to STPartner\n");
      (dynamic_cast <STPartner*> (symTab[entryKey]))->mapOfAttributes[attribute->name] = attribute;
      break;
    }

    case K_PARTNERLINK:
    {
//      traceST("cast to STPartnerLink\n");
      (dynamic_cast <STPartnerLink*> (symTab[entryKey]))->mapOfAttributes[attribute->name] = attribute;
      break;
    }

    case K_PROCESS:
    {
//      traceST("cast to STProcess\n");
      (dynamic_cast <STProcess*> (symTab[entryKey]))->mapOfAttributes[attribute->name] = attribute;
      break;
    }

    case K_RECEIVE:
    {
//      traceST("cast to STReceive\n");
      (dynamic_cast <STReceive*> (symTab[entryKey]))->mapOfAttributes[attribute->name] = attribute;
      break;
    }

    case K_REPLY:
    {
//      traceST("cast to STReply\n");
      (dynamic_cast <STReply*> (symTab[entryKey]))->mapOfAttributes[attribute->name] = attribute;
      break;
    }
        
    case K_SCOPE:
    {
//      traceST("cast to STScope\n");
      (dynamic_cast <STScope*> (symTab[entryKey]))->mapOfAttributes[attribute->name] = attribute;
      break;
    }    

    case K_TERMINATE:
    {
//      traceST("cast to STTerminate\n");
      (dynamic_cast <STTerminate*> (symTab[entryKey]))->mapOfAttributes[attribute->name] = attribute;
      break;
    }
    
    case K_VARIABLE:
    {
//      traceST("cast to STProcess\n");
      (dynamic_cast <STVariable*> (symTab[entryKey]))->mapOfAttributes[attribute->name] = attribute;
      break;
    }

    case K_WAIT:
    {
//      traceST("cast to STWait\n");
      (dynamic_cast <STWait*> (symTab[entryKey]))->mapOfAttributes[attribute->name] = attribute;
      break;
    }

    default:
    { /* cast to Activity */
//      traceST("cast to STActivity\n");    	
      (dynamic_cast <STActivity*> (symTab[entryKey]))->mapOfAttributes[attribute->name] = attribute;
      break;
    }
  }
}

/*!
 * wrapper
 * \param entryKey
 * \param name
 */
STAttribute* SymbolTable::readAttribute(kc::integer entryKey, string name)
{
  return readAttribute(entryKey->value, name);
}

/*!
 * returns a pointer of an attribute object
 * \param entryKey  symbol table entry key
 * \param name      name of attribute 
 */
STAttribute* SymbolTable::readAttribute(unsigned int entryKey, string name)
{
  traceST("readAttribute(entryKey=" + intToString(entryKey) + ", name=" + name + ")\n");
  
  ///
  switch((dynamic_cast <SymbolTableEntry*> (symTab[entryKey]))->elementId)
  {
    case K_COMPENSATE:
    {
//      traceST("cast to STCompensate\n");
      return (dynamic_cast <STCompensate*> (symTab[entryKey]))->mapOfAttributes[name];
    }

    case K_CORRELATION:
    {
//      traceST("cast to STActivity Correlation\n");
      STAttribute* attribute = (dynamic_cast <STActivity*> (symTab[entryKey]))->mapOfAttributes[name];
	  
	  /// if attribute value empty then set default value
      if(attribute->value.empty())
      {
      	if(attribute->name == A__INITIATE)
      	{
      	  attribute->name = DV__INITIATE;
      	}
      }
      return attribute;
    }

    case K_INVOKE:
    {
//      traceST("cast to STInvoke\n");
      return (dynamic_cast <STInvoke*> (symTab[entryKey]))->mapOfAttributes[name];
    }

    case K_LINK:
    {
//      traceST("cast to STLink\n");
      return (dynamic_cast <STLink*> (symTab[entryKey]))->mapOfAttributes[name];
    }

    case K_PARTNER:
    {
//      traceST("cast to STPartner\n");
      return (dynamic_cast <STPartner*> (symTab[entryKey]))->mapOfAttributes[name];
    }

    case K_PARTNERLINK:
    {
//      traceST("cast to STPartnerLink\n");
      return (dynamic_cast <STPartnerLink*> (symTab[entryKey]))->mapOfAttributes[name];
    }

    case K_PICK:
    {
//      traceST("cast to STActivity Pick\n");
      STAttribute* attribute = (dynamic_cast <STActivity*> (symTab[entryKey]))->mapOfAttributes[name];
	  
	  /// if attribute value empty then set default value
      if(attribute->value.empty())
      {
      	if(attribute->name == A__CREATE_INSTANCE)
      	{
      	  attribute->name = DV__CREATE_INSTANCE;
      	}
      }
      return attribute;
    }

    case K_PROCESS:
    {
//      traceST("cast to STProcess\n");
      STAttribute* attribute = (dynamic_cast <STProcess*> (symTab[entryKey]))->mapOfAttributes[name];
	  
	  /// if attribute value empty then set default value
      if(attribute->value.empty())
      {
      	if(attribute->name == A__QUERY_LANGUAGE)
      	{
      	  attribute->name = DV__QUERY_LANGUAGE;
      	}
      	else if(attribute->name == A__EXPRESSION_LANGUAGE)
      	{
      	  attribute->name = DV__EXPRESSION_LANGUAGE;
      	}
      	else if(attribute->name == A__SUPPRESS_JOIN_FAILURE)
      	{
      	  attribute->name = DV__SUPPRESS_JOIN_FAILURE;
      	}
      	else if(attribute->name == A__ENABLE_INSTANCE_COMPENSATION)
      	{
          attribute->name = DV__ENABLE_INSTANCE_COMPENSATION;
      	}
      	else if(attribute->name == A__ABSTRACT_PROCESS)
      	{
      	  attribute->name = DV__ABSTRACT_PROCESS;
      	}
      	else if(attribute->name == A__XMLNS)
      	{ 
      	  attribute->name = DV__XMLNS;
      	}	      	
      }

      return attribute;
    }

    case K_RECEIVE:
    {
//      traceST("cast to STReceive\n");
      STAttribute* attribute = (dynamic_cast <STReceive*> (symTab[entryKey]))->mapOfAttributes[name];
	  
	  /// if attribute value empty then set default value
      if(attribute->value.empty())
      {
      	if(attribute->name == A__CREATE_INSTANCE)
      	{
      	  attribute->name = DV__CREATE_INSTANCE;
      	}
      }
      return attribute;
    }
    
    case K_REPLY:
    {
//      traceST("cast to STReply\n");
      return (dynamic_cast <STReply*> (symTab[entryKey]))->mapOfAttributes[name];
    }
   
    case K_SCOPE:
    {
//      traceST("cast to STScope\n");
      STAttribute* attribute = (dynamic_cast <STScope*> (symTab[entryKey]))->mapOfAttributes[name];
	  
	  /// if attribute value empty then set default value
      if(attribute->value.empty())
      {
      	if(attribute->name == A__VARIABLE_ACCESS_SERIALIZABLE)
      	{
      	  attribute->name = DV__VARIABLE_ACCESS_SERIALIZABLE;
      	}
      }
      return attribute;
    }    

    case K_SOURCE:
    {
//      traceST("cast to STActivity Source\n");
      STAttribute* attribute = (dynamic_cast <STActivity*> (symTab[entryKey]))->mapOfAttributes[name];
	  
	  /// if attribute value empty then set default value
      if(attribute->value.empty())
      {
      	if(attribute->name == A__TRANSITION_CONDITION)
      	{
      	  attribute->name = DV__TRANSITION_CONDITION;
      	}
      }
      return attribute;
    }

    case K_TERMINATE:
    {
//      traceST("cast to STTerminate\n");
      return (dynamic_cast <STTerminate*> (symTab[entryKey]))->mapOfAttributes[name];
    }

    case K_VARIABLE:
    {
//      traceST("cast to STPartnerLink\n");
      return (dynamic_cast <STVariable*> (symTab[entryKey]))->mapOfAttributes[name];
    }
    
    case K_WAIT:
    {
//      traceST("cast to STWait\n");
      return (dynamic_cast <STWait*> (symTab[entryKey]))->mapOfAttributes[name];
    }
    
    default:
    { /* cast to Activity */
//      traceST("cast to STActivity\n");    	
      return (dynamic_cast <STActivity*> (symTab[entryKey]))->mapOfAttributes[name];
    }
  }
} 

/*!
 * print formatted symbol table error message
 * \param errorMsg
 */
void SymbolTable::printErrorMsg(std::string errorMsg)
{
  yyerror(string("[SymbolTable]: " + errorMsg + "\n").c_str());
}

/*!
 * trace method for the ST class
 * \param traceMsg
 */
void SymbolTable::traceST(string traceMsg)
{
  trace(TRACE_DEBUG, "[ST] " + traceMsg);
}

/*!
 * wrapper for checkAttributes(unsigned int entryKey, kc::casestring bpelElementValue)
 * \param entryKey
 * \param bpelElementValue
 */
void SymbolTable::checkAttributes(kc::integer entryKey, kc::casestring bpelElementValue)
{
  checkAttributes(entryKey->value, bpelElementValue);
}


/*!
 * wrapper for checkAttributes(unsigned int entryKey)
 * \param entryKey
 */
void SymbolTable::checkAttributes(kc::integer entryKey)
{
  checkAttributes(entryKey->value);
}

/*!
 * checked the attributes and the value of BPEL-elements
 * \param entryKey
 * \param bpelElementValue
 */
void SymbolTable::checkAttributes(unsigned int entryKey, kc::casestring bpelElementValue)
{
  traceST("checkAttributes(" + intToString(entryKey) + ", " + string(bpelElementValue->name) + ")\n");
  /// pointer of attribute map
  std::map<string, STAttribute*>* mapOfAttributes;
  mapOfAttributes = &((dynamic_cast <STElement*> (symTab[entryKey]))->mapOfAttributes);	

  //special case: <from> ... literal value ... </from>
  
  if((dynamic_cast <SymbolTableEntry*> (symTab[entryKey]))->elementId == K_FROM)
  {
    std::string lit = bpelElementValue->name;

    /* without attributes */
    if(mapOfAttributes->size() == 0)
    { /* literal value is empty */
      if(lit.empty())
      {
        printErrorMsg("from clause is wrong");
      }
    }
    /* with attributes */
    else
    {  /* literal value is empty */
      if(lit.empty())
      {
        /* element with attributes and without literal value */
        checkAttributes(entryKey);
      }
      else
      {
        printErrorMsg("from clause is wrong");        
      }
    }
  }
  else
  {  
    checkAttributes(entryKey);      
  }
}

/*!
 * checked the attributes of BPEL-elements
 * \param entryKey
 */
void SymbolTable::checkAttributes(unsigned int entryKey)
{
  traceST("checkAttributes(" + intToString(entryKey) + ")" + "\n");
  /// pointer of attribute map
  std::map<string, STAttribute*>* mapOfAttributes;
  mapOfAttributes = &((dynamic_cast <STElement*> (symTab[entryKey]))->mapOfAttributes);	

  /// iterator for the embedded map
  std::map<std::string, STAttribute*>::iterator mapOfAttributesIterator;  
  
  switch((dynamic_cast <SymbolTableEntry*> (symTab[entryKey]))->elementId)
  {
    case K_ASSIGN:
    {
      traceST("ASSIGN\n");
      /* no mandatory attributes */        
    }
    break;

    case K_CASE:
    {
      traceST("CASE\n");

      bool conditionFlag;
      conditionFlag = false;
        
      mapOfAttributesIterator = mapOfAttributes->begin();
        
      while(mapOfAttributesIterator != mapOfAttributes->end())
      {  
        if(((*mapOfAttributesIterator).first) == A__CONDITION)
        {
          conditionFlag = true;
          traceST((*mapOfAttributesIterator).first + "\n");
        }
        ++mapOfAttributesIterator;
      }
      
      if(!conditionFlag)
      {
        printErrorMsg("attribute " + A__CONDITION + "=\"" + T__BOOLEAN_EXPR + "\" is missing");          
      }        
      
    }
    break;

    case K_CATCH:
    {
      traceST("CATCH\n");
      /* todo */
    }
    break;  

    case K_CATCHALL:
    {
      traceST("CATCHALL\n");
      /* no attributes */
    }
    break;  

    case K_COMPENSATE:
    {
      traceST("COMPENSATE\n");
      /* no mandatory attributes */        
    }
    break;

    case K_CORRELATION:
    {
      traceST("CORRELATION\n");
      
      bool setFlag;
      setFlag = false;
       
      mapOfAttributesIterator = mapOfAttributes->begin();
      
      ///
      while(mapOfAttributesIterator != mapOfAttributes->end())
      {  
        if(((*mapOfAttributesIterator).first) == A__SET)
        {
          setFlag = true;
          traceST((*mapOfAttributesIterator).first + "\n");
        }
        ++mapOfAttributesIterator;
      }
      
      if(!setFlag)
      {
        printErrorMsg("attribute " + A__SET + "=\"" + T__NCNAME + "\" is missing");        
      }
    }
    break;    

    case K_CORRELATIONSET:
    {
      traceST("CORRELATIONSET\n");
      
      bool nameFlag, propertiesFlag;
      nameFlag = propertiesFlag = false;
       
      mapOfAttributesIterator = mapOfAttributes->begin();
      
      ///
      while(mapOfAttributesIterator != mapOfAttributes->end())
      {  
        if(((*mapOfAttributesIterator).first) == A__NAME)
        {
          nameFlag = true;
          traceST((*mapOfAttributesIterator).first + "\n");
        }
        else if(((*mapOfAttributesIterator).first) == A__PROPERTIES)
        {
          propertiesFlag = true;
          traceST((*mapOfAttributesIterator).first + "\n");
        }
        ++mapOfAttributesIterator;
      }
      
      if(!nameFlag)
      {
        printErrorMsg("attribute " + A__NAME + "=\"" + T__NCNAME + "\" is missing");        
      }
      else if(!propertiesFlag)
      {
        printErrorMsg("attribute " + A__PROPERTIES + "=\"" + T__ANYURI + "\" is missing");        
      }
      
    }
    break;    

    case K_EMPTY:
    {
      traceST("EMPTY\n");
      /* */
    }
    break;  

    case K_FROM:
    {
      traceST("FROM\n");
        
      bool validFrom = false;
      unsigned int attributeCompareCounter;
              
      // array to tag found attributes
      // to store the number of found attributes -> COUNTER field
      unsigned int foundAttributes[] =
      {  /*variable, part, query, partnerLk, endpointRef, property, expression, opaque, COUNTER*/
             0,      0,     0,       0,          0,          0,         0,        0,      0
      };

      unsigned int CSIZE = 7; // number of valid combination of attributes
      unsigned int ASIZE = 9; // number of attributes + COUNTER
      unsigned int COUNTER = 8; // position of counter within the arrays 

      // matrix for valid combination of attributes
      unsigned int validAttributeCombination[][9] =
      {  /*variable, part, query, partnerLk, endpointRef, property, expression, opaque, COUNTER*/
        {   1,      0,     0,       0,          0,          0,         0,        0,      1    },
        {   1,      1,     0,       0,          0,          0,         0,        0,      2    },
        {   1,      1,     1,       0,          0,          0,         0,        0,      3    },
        {   0,      0,     0,       1,          1,          0,         0,        0,      2    },
        {   1,      0,     0,       0,          0,          1,         0,        0,      2    },
        {   0,      0,     0,       0,          0,          0,         1,        0,      1    },
        {   0,      0,     0,       0,          0,          0,         0,        1,      1    }
      };
         
      mapOfAttributesIterator = mapOfAttributes->begin();
        
        ///
      while(mapOfAttributesIterator != mapOfAttributes->end())
      {  
        if(((*mapOfAttributesIterator).first) == A__VARIABLE)
        {
          foundAttributes[0] = 1;
          foundAttributes[COUNTER]++;
          traceST((*mapOfAttributesIterator).first + "\n");
        }
        else if(((*mapOfAttributesIterator).first) == A__PART)
        {
          foundAttributes[1] = 1;
          foundAttributes[COUNTER]++;
          traceST((*mapOfAttributesIterator).first + "\n");
        }
        else if(((*mapOfAttributesIterator).first) == A__QUERY)
        {
          foundAttributes[2] = 1;
          foundAttributes[COUNTER]++;            
          traceST((*mapOfAttributesIterator).first + "\n");
        }          
        else if(((*mapOfAttributesIterator).first) == A__PARTNER_LINK)
        {
          foundAttributes[3] = 1;
          foundAttributes[COUNTER]++;            
          traceST((*mapOfAttributesIterator).first + "\n");
        }
        else if(((*mapOfAttributesIterator).first) == A__ENDPOINT_REFERENCE)
        {
          foundAttributes[4] = 1;
          foundAttributes[COUNTER]++;
          traceST((*mapOfAttributesIterator).first + "\n");
        }
        else if(((*mapOfAttributesIterator).first) == A__PROPERTY)
        {
          foundAttributes[5] = 1;
          foundAttributes[COUNTER]++;
          traceST((*mapOfAttributesIterator).first + "\n");
        }
        else if(((*mapOfAttributesIterator).first) == A__EXPRESSION)
        {
          foundAttributes[6] = 1;
          foundAttributes[COUNTER]++;            
          traceST((*mapOfAttributesIterator).first + "\n");
        }
        else if(((*mapOfAttributesIterator).first) == A__OPAQUE)
        {
          foundAttributes[7] = 1;
          foundAttributes[COUNTER]++;            
          traceST((*mapOfAttributesIterator).first + "\n");
        }                                        
        ++mapOfAttributesIterator;
      }
                
      // iteration about valid attribute combination
      for(unsigned int c = 0; c < CSIZE; c++)
      {
        // under or too much found attributes ... next combination
        if(validAttributeCombination[c][COUNTER] != foundAttributes[COUNTER]) continue;
 
        attributeCompareCounter = 0;
          
        // iteration about attributes
        for(unsigned int a = 0; a < ASIZE-1; a++)
        {  // compare scanned combination of attributes with allowed combination of attributes
          if((validAttributeCombination[c][a] == 1) && (foundAttributes[a] == 1))
          {
            attributeCompareCounter++;
          }
        }

        if(validAttributeCombination[c][COUNTER] == attributeCompareCounter)
        {
          validFrom = true;
          break;  
        }
          
      }
        
      if(!validFrom)
      {
        printErrorMsg("attribute combination within the from clause is wrong");          
      }        
    }
    break;    
 
    case K_INVOKE:
    {
      traceST("INVOKE\n");

      bool partnerLinkFlag, portTypeFlag, operationFlag, inputVariableFlag, outputVariableFlag;
      partnerLinkFlag = portTypeFlag = operationFlag = inputVariableFlag = outputVariableFlag =false;
       
      mapOfAttributesIterator = mapOfAttributes->begin();
      
        ///
      while(mapOfAttributesIterator != mapOfAttributes->end())
      {  
        if(((*mapOfAttributesIterator).first) == A__PARTNER_LINK)
        {
          partnerLinkFlag = true;
          traceST((*mapOfAttributesIterator).first + "\n");
        }
        else if(((*mapOfAttributesIterator).first) == A__PORT_TYPE)
        {
          portTypeFlag = true;
          traceST((*mapOfAttributesIterator).first + "\n");
        }
        else if(((*mapOfAttributesIterator).first) == A__OPERATION)
        {
          operationFlag = true;
          traceST((*mapOfAttributesIterator).first + "\n");
        }        
        else if(((*mapOfAttributesIterator).first) == A__INPUT_VARIABLE)
        {
          inputVariableFlag = true;
          traceST((*mapOfAttributesIterator).first + "\n");
        }
        /* optional */
        /*
        else if(((*mapOfAttributesIterator).first) == A__OUTPUT_VARIABLE)
        {
          outputVariableFlag = true;
          traceST((*mapOfAttributesIterator).first + "\n");
        }*/
        ++mapOfAttributesIterator;
      }
      
      if(!partnerLinkFlag)
      {
      printErrorMsg("attribute " + A__PARTNER_LINK + "=\"" + T__NCNAME + "\" is missing");      
      }
      else if(!portTypeFlag)
      {
        printErrorMsg("attribute " + A__PORT_TYPE + "=\"" + T__QNAME + "\" is missing");        
      }
      else if(!operationFlag)
      {
        printErrorMsg("attribute " + A__OPERATION + "=\"" + T__NCNAME + "\" is missing");        
      }            
      else if(!inputVariableFlag)
      {
        printErrorMsg("attribute " + A__INPUT_VARIABLE + "=\"" + T__NCNAME + "\" is missing");        
      }
      /* optional */
      /*
      else if(!outputVariableFlag)
      {
        printErrorMsg("attribute " + A__OUTPUT_VARIABLE + "=\"" + T__NCNAME + "\" is missing");        
      }*/
    }
    break;  

    case K_LINK:
    {
      traceST("LINK\n");

      bool nameFlag;
      nameFlag = false;
       
      mapOfAttributesIterator = mapOfAttributes->begin();
      
      ///
      while(mapOfAttributesIterator != mapOfAttributes->end())
      {  
        if(((*mapOfAttributesIterator).first) == A__NAME)
        {
          nameFlag = true;
          traceST((*mapOfAttributesIterator).first + "\n");
        }
        ++mapOfAttributesIterator;
      }
      
      if(!nameFlag)
      {
        printErrorMsg("attribute " + A__NAME + "=\"" + T__NCNAME + "\" is missing");        
      }

    }
    break;  

    case K_ONALARM:
    {
      traceST("ONALARM\n");
      
      bool forFlag, untilFlag;
      forFlag = untilFlag = false;
       
      mapOfAttributesIterator = mapOfAttributes->begin();
      
      ///
      while(mapOfAttributesIterator != mapOfAttributes->end())
      {  
        if(((*mapOfAttributesIterator).first) == A__FOR)
        {
          forFlag = true;
          traceST((*mapOfAttributesIterator).first + "\n");
        }
        else if(((*mapOfAttributesIterator).first) == A__UNTIL)
        {
          untilFlag = true;
          traceST((*mapOfAttributesIterator).first + "\n");
        }
        ++mapOfAttributesIterator;
      }

      /// only one attribute is allowed
      if((forFlag && !untilFlag) || (untilFlag && !forFlag))
      {  
        /* all okay */
      }
      else
      {
        // no attribute
        if(!forFlag && !untilFlag)
        {
          printErrorMsg("attribute " + A__FOR + "=\"" + T__DURATION_EXPR + "\"  or " + A__UNTIL + "=\"" + T__DEADLINE_EXPR + " is missing");          
        }
        // to much attributes, only one is allowed
        else
        {
          printErrorMsg("both attributes " + A__FOR + "=\"" + T__DURATION_EXPR + "\"  and " + A__UNTIL + "=\"" + T__DEADLINE_EXPR + " are not allowed");                  
        }
      }
    }
    break;    

    case K_ONMESSAGE:
    {
      traceST("ONMESSAGE\n");
      
      bool partnerLinkFlag, portTypeFlag, operationFlag, variableFlag;
      partnerLinkFlag = portTypeFlag = operationFlag = variableFlag = false;
       
      mapOfAttributesIterator = mapOfAttributes->begin();
      
      ///
      while(mapOfAttributesIterator != mapOfAttributes->end())
      {  
        if(((*mapOfAttributesIterator).first) == A__PARTNER_LINK)
        {
          partnerLinkFlag = true;
          traceST((*mapOfAttributesIterator).first + "\n");
        }
        else if(((*mapOfAttributesIterator).first) == A__PORT_TYPE)
        {
          portTypeFlag = true;
          traceST((*mapOfAttributesIterator).first + "\n");
        }
        else if(((*mapOfAttributesIterator).first) == A__OPERATION)
        {
          operationFlag = true;
          traceST((*mapOfAttributesIterator).first + "\n");
        }        
        else if(((*mapOfAttributesIterator).first) == A__VARIABLE)
        {
          variableFlag = true;
          traceST((*mapOfAttributesIterator).first + "\n");
        }        
        ++mapOfAttributesIterator;
      }
      
      if(!partnerLinkFlag)
      {
        printErrorMsg("attribute " + A__PARTNER_LINK + "=\"" + T__NCNAME + "\" is missing");        
      }
      else if(!portTypeFlag)
      {
        printErrorMsg("attribute " + A__PORT_TYPE + "=\"" + T__QNAME + "\" is missing");        
      }
      else if(!operationFlag)
      {
        printErrorMsg("attribute " + A__OPERATION + "=\"" + T__NCNAME + "\" is missing");        
      }
      else if(!variableFlag)
      {
        printErrorMsg("attribute " + A__VARIABLE + "=\"" + T__NCNAME + "\" is missing");        
      }
    }
    break;

    case K_PARTNER:
    {
      traceST("PARTNER\n");
      
      bool nameFlag;
      nameFlag = false;
       
      mapOfAttributesIterator = mapOfAttributes->begin();
      
      ///
      while(mapOfAttributesIterator != mapOfAttributes->end())
      {  
        if(((*mapOfAttributesIterator).first) == A__NAME)
        {
          nameFlag = true;
          traceST((*mapOfAttributesIterator).first + "\n");
        }
        ++mapOfAttributesIterator;
      }
      
      if(!nameFlag)
      {
        printErrorMsg("attribute " + A__NAME + "=\"" + T__NCNAME + "\" is missing");        
      }
    }
    break;    

    case K_PARTNERLINK:
    {
      traceST("PARTNERLINK\n");
      
      bool nameFlag, partnerLinkTypeFlag;
      nameFlag = partnerLinkTypeFlag = false;
       
      mapOfAttributesIterator = mapOfAttributes->begin();
      
      ///
      while(mapOfAttributesIterator != mapOfAttributes->end())
      {  
        if(((*mapOfAttributesIterator).first) == A__NAME)
        {
          nameFlag = true;
          traceST((*mapOfAttributesIterator).first + "\n");
        }
        else if(((*mapOfAttributesIterator).first) == A__PARTNER_LINK_TYPE)
        {
          partnerLinkTypeFlag = true;
          traceST((*mapOfAttributesIterator).first + "\n");
        }
        ++mapOfAttributesIterator;
      }
      
      if(!nameFlag)
      {
        printErrorMsg("attribute " + A__NAME + "=\"" + T__NCNAME + "\" is missing");        
      }
      else if(!partnerLinkTypeFlag)
      {
        printErrorMsg("attribute " + A__PARTNER_LINK_TYPE + "=\"" + T__QNAME + "\" is missing");        
      }
      
    }
    break;    
      
    case K_PICK:
    {
      traceST("PICK\n");
      /* default attribute */
    }
    break;      

    case K_PROCESS:
    {
      traceST("PROCESS\n");
      
      bool nameFlag, targetNamespaceFlag;
      nameFlag = targetNamespaceFlag = false;
       
      mapOfAttributesIterator = mapOfAttributes->begin();
      
      ///
      while(mapOfAttributesIterator != mapOfAttributes->end())
      {  
        if(((*mapOfAttributesIterator).first) == A__NAME)
        {
          nameFlag = true;
          traceST((*mapOfAttributesIterator).first + "\n");
        }
        else if(((*mapOfAttributesIterator).first) == A__TARGET_NAMESPACE)
        {
          targetNamespaceFlag = true;
          traceST((*mapOfAttributesIterator).first + "\n");
        }
        ++mapOfAttributesIterator;
      }
      
      if(!nameFlag)
      {
        printErrorMsg("attribute " + A__NAME + "=\"" + T__NCNAME + "\" is missing");        
      }
      else if(!targetNamespaceFlag)
      {
        printErrorMsg("attribute " + A__TARGET_NAMESPACE + "=\"" + T__ANYURI + "\" is missing");        
      }
      
    }
    break;    

    case K_RECEIVE:
    {
      traceST("RECEIVE\n");      

      bool partnerLinkFlag, portTypeFlag, operationFlag, variableFlag;
      partnerLinkFlag = portTypeFlag = operationFlag = variableFlag = false;
       
      mapOfAttributesIterator = mapOfAttributes->begin();
      
      ///
      while(mapOfAttributesIterator != mapOfAttributes->end())
      {  
        if(((*mapOfAttributesIterator).first) == A__PARTNER_LINK)
        {
          partnerLinkFlag = true;
          traceST((*mapOfAttributesIterator).first + "\n");
        }
        else if(((*mapOfAttributesIterator).first) == A__PORT_TYPE)
        {
          portTypeFlag = true;
          traceST((*mapOfAttributesIterator).first + "\n");
        }
        else if(((*mapOfAttributesIterator).first) == A__OPERATION)
        {
          operationFlag = true;
          traceST((*mapOfAttributesIterator).first + "\n");
        }        
        else if(((*mapOfAttributesIterator).first) == A__VARIABLE)
        {
          variableFlag = true;
          traceST((*mapOfAttributesIterator).first + "\n");
        }        
        ++mapOfAttributesIterator;
      }
      
      if(!partnerLinkFlag)
      {
        printErrorMsg("attribute " + A__PARTNER_LINK + "=\"" + T__NCNAME + "\" is missing");        
      }
      else if(!portTypeFlag)
      {
        printErrorMsg("attribute " + A__PORT_TYPE + "=\"" + T__QNAME + "\" is missing");        
      }
      else if(!operationFlag)
      {
        printErrorMsg("attribute " + A__OPERATION + "=\"" + T__NCNAME + "\" is missing");        
      }
      else if(!variableFlag)
      {
        printErrorMsg("attribute " + A__VARIABLE + "=\"" + T__NCNAME + "\" is missing");        
      }
    }
    break;    

    case K_REPLY:
    {
      traceST("REPLY\n");

      bool partnerLinkFlag, portTypeFlag, operationFlag;
      partnerLinkFlag = portTypeFlag = operationFlag = false;
       
      mapOfAttributesIterator = mapOfAttributes->begin();
      
      ///
      while(mapOfAttributesIterator != mapOfAttributes->end())
      {  
        if(((*mapOfAttributesIterator).first) == A__PARTNER_LINK)
        {
          partnerLinkFlag = true;
          traceST((*mapOfAttributesIterator).first + "\n");
        }
        else if(((*mapOfAttributesIterator).first) == A__PORT_TYPE)
        {
          portTypeFlag = true;
          traceST((*mapOfAttributesIterator).first + "\n");
        }
        else if(((*mapOfAttributesIterator).first) == A__OPERATION)
        {
          operationFlag = true;
          traceST((*mapOfAttributesIterator).first + "\n");
        }        
        ++mapOfAttributesIterator;
      }
      
      if(!partnerLinkFlag)
      {
        printErrorMsg("attribute " + A__PARTNER_LINK + "=\"" + T__NCNAME + "\" is missing");        
      }
      else if(!portTypeFlag)
      {
        printErrorMsg("attribute " + A__PORT_TYPE + "=\"" + T__QNAME + "\" is missing");        
      }
      else if(!operationFlag)
      {
        printErrorMsg("attribute " + A__OPERATION + "=\"" + T__NCNAME + "\" is missing");        
      }      
    }
    break;    

    case K_SCOPE:
    {
      traceST("SCOPE\n");
      /* default attribute */
    }
    break;

    case K_SEQUENCE:
    {
      traceST("SEQUENCE\n");
      /* no mandatory attributes */      
    }
    break;      

    case K_SOURCE:
    {
      traceST("SOURCE\n");

      bool linkNameFlag;
      linkNameFlag = false;
       
      mapOfAttributesIterator = mapOfAttributes->begin();
      
      ///
      while(mapOfAttributesIterator != mapOfAttributes->end())
      {  
        if(((*mapOfAttributesIterator).first) == A__LINK_NAME)
        {
          linkNameFlag = true;
          traceST((*mapOfAttributesIterator).first + "\n");
        }
        ++mapOfAttributesIterator;
      }
      
      if(!linkNameFlag)
      {
        printErrorMsg("attribute " + A__LINK_NAME + "=\"" + T__NCNAME + "\" is missing");        
      }

    }
    break;  

    case K_SWITCH:
    {
      traceST("SWITCH\n");
      /* no mandatory attributes */      
    }
    break;      

    case K_TARGET:
    {
      traceST("TARGET\n");

      bool linkNameFlag;
      linkNameFlag = false;
       
      mapOfAttributesIterator = mapOfAttributes->begin();
      
      ///
      while(mapOfAttributesIterator != mapOfAttributes->end())
      {  
        if(((*mapOfAttributesIterator).first) == A__LINK_NAME)
        {
          linkNameFlag = true;
          traceST((*mapOfAttributesIterator).first + "\n");
        }
        ++mapOfAttributesIterator;
      }
      
      if(!linkNameFlag)
      {
        printErrorMsg("attribute " + A__LINK_NAME + "=\"" + T__NCNAME + "\" is missing");        
      }

    }
    break;  

    case K_TERMINATE:
    {
      traceST("TERMINATE\n");
      /* no mandatory attributes */
    }
    break;    

    case K_THROW:
    {
      traceST("THROW\n");

      bool faultNameFlag;
      faultNameFlag = false;
       
      mapOfAttributesIterator = mapOfAttributes->begin();
      
      ///
      while(mapOfAttributesIterator != mapOfAttributes->end())
      {  
        if(((*mapOfAttributesIterator).first) == A__FAULT_NAME)
        {
          faultNameFlag = true;
          traceST((*mapOfAttributesIterator).first + "\n");
        }
        ++mapOfAttributesIterator;
      }
      
      if(!faultNameFlag)
      {
        printErrorMsg("attribute " + A__FAULT_NAME + "=\"" + T__QNAME + "\" is missing");        
      }

    }
    break;    

    case K_TO:
    {
      traceST("TO\n");
      
      bool validTo = false;
      unsigned int attributeCompareCounter;
            
      // array to tag found attributes
      // to store the number of found attributes -> COUNTER field
      unsigned int foundAttributes[] =
      {  /*variable, part, query, partnerLk, property, COUNTER*/
           0,      0,     0,       0,        0,      0
      };

      unsigned int CSIZE = 5; // number of valid combination of attributes
      unsigned int ASIZE = 6; // number of attributes + COUNTER
      unsigned int COUNTER = 5; // position of counter within the arrays 

      // matrix for valid combination of attributes
      unsigned int validAttributeCombination[][6] =
      {  /*variable, part, query, partnerLk, property, COUNTER*/
         {   1,      0,     0,       0,      0,      1    },
         {   1,      1,     0,       0,      0,      2    },
         {   1,      1,     1,       0,      0,      3    },
         {   0,      0,     0,       1,      0,      1    },
         {   1,      0,     0,       0,      1,      2    }
      };
       
      mapOfAttributesIterator = mapOfAttributes->begin();
      
      ///
      while(mapOfAttributesIterator != mapOfAttributes->end())
      {  
        if(((*mapOfAttributesIterator).first) == A__VARIABLE)
        {
          foundAttributes[0] = 1;
          foundAttributes[COUNTER]++;
          traceST((*mapOfAttributesIterator).first + "\n");
        }
        else if(((*mapOfAttributesIterator).first) == A__PART)
        {
          foundAttributes[1] = 1;
          foundAttributes[COUNTER]++;
          traceST((*mapOfAttributesIterator).first + "\n");
        }
        else if(((*mapOfAttributesIterator).first) == A__QUERY)
        {
          foundAttributes[2] = 1;
          foundAttributes[COUNTER]++;          
          traceST((*mapOfAttributesIterator).first + "\n");
        }        
        else if(((*mapOfAttributesIterator).first) == A__PARTNER_LINK)
        {
          foundAttributes[3] = 1;
          foundAttributes[COUNTER]++;          
          traceST((*mapOfAttributesIterator).first + "\n");
        }
        else if(((*mapOfAttributesIterator).first) == A__PROPERTY)
        {
          foundAttributes[4] = 1;
          foundAttributes[COUNTER]++;
          traceST((*mapOfAttributesIterator).first + "\n");
        }
        ++mapOfAttributesIterator;
      }
            
      // iteration about valid attribute combination
      for(unsigned int c = 0; c < CSIZE; c++)
      {
        // under or too much found attributes ... next combination
        if(validAttributeCombination[c][COUNTER] != foundAttributes[COUNTER]) continue;
  
        attributeCompareCounter = 0;
        
        // iteration about attributes
        for(unsigned int a = 0; a < ASIZE-1; a++)
        {  // compare scanned combination of attributes with allowed combination of attributes
          if((validAttributeCombination[c][a] == 1) && (foundAttributes[a] == 1))
          {
            attributeCompareCounter++;
          }
        }

        if(validAttributeCombination[c][COUNTER] == attributeCompareCounter)
        {
          validTo = true;
          break;  
        }
        
      }
      
      if(!validTo)
      {
        printErrorMsg("attribute combination within the to clause is wrong");        
      }      
      }
      break;    

    case K_VARIABLE:
    {
      traceST("VARIABLE\n");

      bool nameFlag;
      nameFlag = false;
       
      mapOfAttributesIterator = mapOfAttributes->begin();
      
      ///
      while(mapOfAttributesIterator != mapOfAttributes->end())
      {  
        if(((*mapOfAttributesIterator).first) == A__NAME)
        {
          nameFlag = true;
          traceST((*mapOfAttributesIterator).first + "\n");
        }
        ++mapOfAttributesIterator;
      }
      
      if(!nameFlag)
      {
        printErrorMsg("attribute " + A__NAME + "=\"" + T__NCNAME + "\" is missing");        
      }

    }
    break;  

    case K_WAIT:
      {
      traceST("WAIT\n");
      
      bool forFlag, untilFlag;
      forFlag = untilFlag = false;
       
      mapOfAttributesIterator = mapOfAttributes->begin();
      
      ///
      while(mapOfAttributesIterator != mapOfAttributes->end())
      {  
        if(((*mapOfAttributesIterator).first) == A__FOR)
        {
          forFlag = true;
          traceST((*mapOfAttributesIterator).first + "\n");
        }
        else if(((*mapOfAttributesIterator).first) == A__UNTIL)
        {
          untilFlag = true;
          traceST((*mapOfAttributesIterator).first + "\n");
        }
        ++mapOfAttributesIterator;
      }

      /// only one attribute is allowed
      if((forFlag && !untilFlag) || (untilFlag && !forFlag))
      {  
        /* all okay */
      }
      else
      {
        // no attribute
        if(!forFlag && !untilFlag)
        {
          printErrorMsg("attribute " + A__FOR + "=\"" + T__DURATION_EXPR + "\"  or " + A__UNTIL + "=\"" + T__DEADLINE_EXPR + " is missing");          
        }
        // to much attributes, only one is allowed
        else
        {
          printErrorMsg("both attributes " + A__FOR + "=\"" + T__DURATION_EXPR + "\"  and " + A__UNTIL + "=\"" + T__DEADLINE_EXPR + " are not allowed");                  
        }
      }
      }
      break;    

    case K_WHILE:
      {
      traceST("WHILE\n");

      bool conditionFlag;
      conditionFlag = false;
       
      mapOfAttributesIterator = mapOfAttributes->begin();
      
      ///
      while(mapOfAttributesIterator != mapOfAttributes->end())
      {  
        if(((*mapOfAttributesIterator).first) == A__CONDITION)
        {
          conditionFlag = true;
          traceST((*mapOfAttributesIterator).first + "\n");
        }
        ++mapOfAttributesIterator;
      }
      
      if(!conditionFlag)
      {
        printErrorMsg("attribute " + A__CONDITION + "=\"" + T__BOOLEAN_EXPR + "\" is missing");        
      }
  
      
    }
    break;      

  }
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
STAttribute::STAttribute(string name, string value)
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
 * implementation of CommunicationActivity CLASS
 ********************************************/

/*!
 * constructor
 */
STCommunicationActivity::STCommunicationActivity(unsigned int elementId, unsigned int entryKey)
 :STActivity(elementId, entryKey) {}

/*!
 * destructor
 */
STCommunicationActivity::~STCommunicationActivity() {}

/********************************************
 * implementation of Compensate CLASS
 ********************************************/

/*!
 * constructor
 */
STCompensate::STCompensate(unsigned int elementId, unsigned int entryKey)
 :STActivity(elementId, entryKey) {}

/*!
 * destructor
 */
STCompensate::~STCompensate() {}
 
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
 * implementation of Invoke CLASS
 ********************************************/

/*!
 * constructor
 */
STInvoke::STInvoke(unsigned int elementId, unsigned int entryKey)
 :STCommunicationActivity(elementId, entryKey) {}

/*!
 * destructor
 */
STInvoke::~STInvoke() {}

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
 * implementation of Receive CLASS
 ********************************************/

/*!
 * constructor
 */
STReceive::STReceive(unsigned int elementId, unsigned int entryKey)
 :STCommunicationActivity(elementId, entryKey) {}

/*!
 * destructor
 */
STReceive::~STReceive() {}

/********************************************
 * implementation of Reply CLASS
 ********************************************/

/*!
 * constructor
 */
STReply::STReply(unsigned int elementId, unsigned int entryKey)
 :STCommunicationActivity(elementId, entryKey) {}

/*!
 * destructor
 */
STReply::~STReply() {}

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
 * implementation of Terminate CLASS
 ********************************************/

/*!
 * constructor
 */
STTerminate::STTerminate(unsigned int elementId, unsigned int entryKey)
 :STActivity(elementId, entryKey) {}

/*!
 * destructor
 */
STTerminate::~STTerminate() {}
 
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

/********************************************
 * implementation of Wait CLASS
 ********************************************/

/*!
 * constructor
 */
STWait::STWait(unsigned int elementId, unsigned int entryKey)
 :STActivity(elementId, entryKey) {}

/*!
 * destructor
 */
STWait::~STWait() {}

