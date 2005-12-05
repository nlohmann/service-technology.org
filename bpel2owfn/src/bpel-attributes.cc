/*!
 * \file bpel-attributes.c
 *
 * \brief Functions for Petri nets (implementation)
 *
 * This file implements the classes and functions defined in bpel-attributes.h.
 * 
 * \author  
 *          - responsible: Dennis Reinert <reinert@informatik.hu-berlin.de>
 *          - last changes of: \$Author: reinert $
 *          
 * \date
 *          - created: 2005/10/18
 *          - last changed: \$Date: 2005/12/05 16:58:37 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universit�t zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel
 *          for details.
 *
 * \version \$Revision: 1.16 $
 *
 * \todo
 *       - (reinert) Comment this file and its classes.
 *       - (reinert) Variables for <onAlarm> are not checked.
 *       - (reinert) Check data types (e.g. is a value "yes" or "no").
 */

#include "bpel-attributes.h"
#include "bpel-syntax.h"
#include "debug.h"

/*!
 * 
 */
attributeManager::attributeManager()
{
  this->scannerResult.clear();
  this->nodeId = 0;
}

/*!
 * 
 */
kc::casestring attributeManager::read(kc::integer elementId, std::string attributeName)
{
  // "cast" Kimwitu++ to C++
  unsigned int elementIdInt = elementId->value;

  
  std::string result;
  
  if(scannerResult[elementIdInt][attributeName].empty())
  {
    // no attribute-value given
    result = "";
  }
  else
  {
    result = scannerResult[elementIdInt][attributeName];
  }

  return kc::mkcasestring(result.c_str());
}

/*!
 * 
 */
kc::casestring attributeManager::read(kc::integer elementId, std::string attributeName, kc::casestring defaultValue)
{
  // "cast" Kimwitu++ to C++
  unsigned int elementIdInt = elementId->value;

  
  std::string result;
  
  if(scannerResult[elementIdInt][attributeName].empty())
  {
	return defaultValue;
  }
  else
  {
    result = scannerResult[elementIdInt][attributeName];
    return kc::mkcasestring(result.c_str());
  }
   
}

/*!
 * 
 */
kc::integer attributeManager::nextId()
{
  this->nodeId++;
  return kc::mkinteger(this->nodeId);
}

/*!
 * 
 */
void attributeManager::define(kc::casestring attributeName, kc::casestring attributeValue)
{  	
  /*
    traceAM("define \n");
	traceAM(attributeName->name);
  	traceAM("=");
  	traceAM(attributeValue->name);
  	traceAM("\n");
  */

  if(attributeName->name == A__ABSTRACT_PROCESS)
  {
   	if((attributeValue->name != (string)"yes") && (attributeValue->name != (string)"no"))
  	{
  		printErrorMsg("wrong value of " + A__ABSTRACT_PROCESS + " attribute");
  	}
  }
  else if(attributeName->name == A__CREATE_INSTANCE)
  {
   	if((attributeValue->name != (string)"yes") && (attributeValue->name != (string)"no"))
  	{
  		printErrorMsg("wrong value of " + A__CREATE_INSTANCE + " attribute");
  	}
  }
  else if(attributeName->name == A__ENABLE_INSTANCE_COMPENSATION)
  {
   	if((attributeValue->name != (string)"yes") && (attributeValue->name != (string)"no"))
  	{
  		printErrorMsg("wrong value of " + A__ENABLE_INSTANCE_COMPENSATION + " attribute");
  	}
  }
  else if(attributeName->name == A__INITIATE)
  {
   	if((attributeValue->name != (string)"yes") && (attributeValue->name != (string)"no"))
  	{
  		printErrorMsg("wrong value of " + A__INITIATE + " attribute");
  	}
  }
  else if(attributeName->name == A__SUPPRESS_JOIN_FAILURE)
  {
   	if((attributeValue->name != (string)"yes") && (attributeValue->name != (string)"no"))
  	{
  		printErrorMsg("wrong value of " + A__SUPPRESS_JOIN_FAILURE + " attribute");
  	}
  }
  else if(attributeName->name == A__VARIABLE_ACCESS_SERIALIZABLE)
  {
   	if((attributeValue->name != (string)"yes") && (attributeValue->name != (string)"no"))
  	{
  		printErrorMsg("wrong value of " + A__VARIABLE_ACCESS_SERIALIZABLE + " attribute");
  	}
  }
  
  scannerResult[this->nodeId][attributeName->name] = attributeValue->name;
}

/*!
 *  printing formatted attribute manager error message
 */
void attributeManager::printErrorMsg(std::string errorMsg)
{
	yyerror(string("[AttributeManager]: " + errorMsg + "\n").c_str());
}

void attributeManager::traceAM(std::string traceMsg)
{
	trace(TRACE_DEBUG, traceMsg);	
}
/*!
 * 
 */
void attributeManager::check(kc::integer elementId, unsigned int activityId)
{
	// "cast" Kimwitu++ to C++
  	unsigned int elementIdInt = elementId->value; 
	/// iterator for the embedded map
    std::map<std::string, string>::iterator scannerResultDataIterator;
	
	switch(activityId) {
		case K_PROCESS:
			{
				traceAM("PROCESS!!! \n");
				
				bool nameFlag, targetNamespaceFlag;
				nameFlag = targetNamespaceFlag = false;
				 
				scannerResultDataIterator = this->scannerResult[elementIdInt].begin();
				
				///
				while(scannerResultDataIterator != scannerResult[elementIdInt].end())
				{	
					if(((*scannerResultDataIterator).first) == A__NAME)
					{
						nameFlag = true;
						traceAM((*scannerResultDataIterator).first + "\n");
					}
					else if(((*scannerResultDataIterator).first) == A__TARGET_NAMESPACE)
					{
						targetNamespaceFlag = true;
						traceAM((*scannerResultDataIterator).first + "\n");
					}
					++scannerResultDataIterator;
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
				traceAM("RECEIVE!!! \n");			

				bool partnerLinkFlag, portTypeFlag, operationFlag;
				partnerLinkFlag = portTypeFlag = operationFlag = false;
				 
				scannerResultDataIterator = this->scannerResult[elementIdInt].begin();
				
				///
				while(scannerResultDataIterator != scannerResult[elementIdInt].end())
				{	
					if(((*scannerResultDataIterator).first) == A__PARTNER_LINK)
					{
						partnerLinkFlag = true;
						traceAM((*scannerResultDataIterator).first + "\n");
					}
					else if(((*scannerResultDataIterator).first) == A__PORT_TYPE)
					{
						portTypeFlag = true;
						traceAM((*scannerResultDataIterator).first + "\n");
					}
					else if(((*scannerResultDataIterator).first) == A__OPERATION)
					{
						operationFlag = true;
						traceAM((*scannerResultDataIterator).first + "\n");
					}					
					++scannerResultDataIterator;
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

		case K_REPLY:
			{
				traceAM("REPLY!!! \n");

				bool partnerLinkFlag, portTypeFlag, operationFlag;
				partnerLinkFlag = portTypeFlag = operationFlag = false;
				 
				scannerResultDataIterator = this->scannerResult[elementIdInt].begin();
				
				///
				while(scannerResultDataIterator != scannerResult[elementIdInt].end())
				{	
					if(((*scannerResultDataIterator).first) == A__PARTNER_LINK)
					{
						partnerLinkFlag = true;
						traceAM((*scannerResultDataIterator).first + "\n");
					}
					else if(((*scannerResultDataIterator).first) == A__PORT_TYPE)
					{
						portTypeFlag = true;
						traceAM((*scannerResultDataIterator).first + "\n");
					}
					else if(((*scannerResultDataIterator).first) == A__OPERATION)
					{
						operationFlag = true;
						traceAM((*scannerResultDataIterator).first + "\n");
					}					
					++scannerResultDataIterator;
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

		case K_INVOKE:
			{
				traceAM("INVOKE!!! \n");

				bool partnerLinkFlag, portTypeFlag, operationFlag;
				partnerLinkFlag = portTypeFlag = operationFlag = false;
				 
				scannerResultDataIterator = this->scannerResult[elementIdInt].begin();
				
				///
				while(scannerResultDataIterator != scannerResult[elementIdInt].end())
				{	
					if(((*scannerResultDataIterator).first) == A__PARTNER_LINK)
					{
						partnerLinkFlag = true;
						traceAM((*scannerResultDataIterator).first + "\n");
					}
					else if(((*scannerResultDataIterator).first) == A__PORT_TYPE)
					{
						portTypeFlag = true;
						traceAM((*scannerResultDataIterator).first + "\n");
					}
					else if(((*scannerResultDataIterator).first) == A__OPERATION)
					{
						operationFlag = true;
						traceAM((*scannerResultDataIterator).first + "\n");
					}					
					++scannerResultDataIterator;
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

		case K_ASSIGN:
			{
				traceAM("ASSIGN!!! \n");

				/* no mandatory attributes */				

			}
			break;		

		case K_THROW:
			{
				traceAM("THROW!!! \n");

				bool faultNameFlag;
				faultNameFlag = false;
				 
				scannerResultDataIterator = this->scannerResult[elementIdInt].begin();
				
				///
				while(scannerResultDataIterator != scannerResult[elementIdInt].end())
				{	
					if(((*scannerResultDataIterator).first) == A__FAULT_NAME)
					{
						faultNameFlag = true;
						traceAM((*scannerResultDataIterator).first + "\n");
					}
					++scannerResultDataIterator;
				}
				
				if(!faultNameFlag)
				{
					printErrorMsg("attribute " + A__FAULT_NAME + "=\"" + T__QNAME + "\" is missing");					
				}

			}
			break;		

		case K_WAIT:
			{
				traceAM("WAIT!!! \n");
				
				bool forFlag, untilFlag;
				forFlag = untilFlag = false;
				 
				scannerResultDataIterator = this->scannerResult[elementIdInt].begin();
				
				///
				while(scannerResultDataIterator != scannerResult[elementIdInt].end())
				{	
					if(((*scannerResultDataIterator).first) == A__FOR)
					{
						forFlag = true;
						traceAM((*scannerResultDataIterator).first + "\n");
					}
					else if(((*scannerResultDataIterator).first) == A__UNTIL)
					{
						untilFlag = true;
						traceAM((*scannerResultDataIterator).first + "\n");
					}
					++scannerResultDataIterator;
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

		case K_EMPTY:
			{
				traceAM("EMPTY!!! \n");
				
				/* no mandatory attributes */
				
			}
			break;		

		case K_TERMINATE:
			{
				traceAM("TERMINATE!!! \n");
				
				/* no mandatory attributes */
				
			}
			break;		
		case K_SCOPE:
			{
				traceAM("SCOPE!!! \n");
				
				/* default attribute */
				
			}
			break;
		case K_COMPENSATE:
			{
				traceAM("COMPENSATE!!! \n");

				/* no mandatory attributes */				
				
			}
			break;				
		case K_PICK:
			{
				traceAM("PICK!!! \n");
				
				/* default attribute */
			}
			break;				
		case K_SEQUENCE:
			{
				traceAM("SEQUENCE!!! \n");

				/* no mandatory attributes */				
				
			}
			break;				
		case K_SWITCH:
			{
				traceAM("SWITCH!!! \n");

				/* no mandatory attributes */				

			}
			break;				
		case K_WHILE:
			{
				traceAM("WHILE!!! \n");

				bool conditionFlag;
				conditionFlag = false;
				 
				scannerResultDataIterator = this->scannerResult[elementIdInt].begin();
				
				///
				while(scannerResultDataIterator != scannerResult[elementIdInt].end())
				{	
					if(((*scannerResultDataIterator).first) == A__CONDITION)
					{
						conditionFlag = true;
						trace(TRACE_DEBUG,(*scannerResultDataIterator).first + "\n");
					}
					++scannerResultDataIterator;
				}
				
				if(!conditionFlag)
				{
					printErrorMsg("attribute " + A__CONDITION + "=\"" + T__BOOLEAN_EXPR + "\" is missing");					
				}
	
				
			}
			break;				
	}

}
