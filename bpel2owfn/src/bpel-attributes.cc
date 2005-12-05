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
 *          - last changed: \$Date: 2005/12/05 14:49:27 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universit�t zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel
 *          for details.
 *
 * \version \$Revision: 1.15 $
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
  scannerResult[this->nodeId][attributeName->name] = attributeValue->name;
}

/*!
 *  
 */
void attributeManager::printCheckErrorMsg(std::string errorMsg)
{
	yyerror(string("[AttributeManager]: " + errorMsg + "\n").c_str());
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
				trace(TRACE_DEBUG,"PROCESS!!! \n");
				
				bool nameFlag, targetNamespaceFlag;
				nameFlag = targetNamespaceFlag = false;
				 
				scannerResultDataIterator = this->scannerResult[elementIdInt].begin();
				
				///
				while(scannerResultDataIterator != scannerResult[elementIdInt].end())
				{	
					if(((*scannerResultDataIterator).first) == A__NAME)
					{
						nameFlag = true;
						trace(TRACE_DEBUG,(*scannerResultDataIterator).first + "\n");
					}
					else if(((*scannerResultDataIterator).first) == A__TARGET_NAMESPACE)
					{
						targetNamespaceFlag = true;
						trace(TRACE_DEBUG,(*scannerResultDataIterator).first + "\n");
					}
					++scannerResultDataIterator;
				}
				
				if(!nameFlag)
				{
					printCheckErrorMsg("attribute " + A__NAME + "=\"" + T__NCNAME + "\" is missing");					
				}
				else if(!targetNamespaceFlag)
				{
					printCheckErrorMsg("attribute " + A__TARGET_NAMESPACE + "=\"" + T__ANYURI + "\" is missing");					
				}
				
			}
			break;		

		case K_RECEIVE:
			{
				trace(TRACE_DEBUG,"RECEIVE!!! \n");			

				bool partnerLinkFlag, portTypeFlag, operationFlag;
				partnerLinkFlag = portTypeFlag = operationFlag = false;
				 
				scannerResultDataIterator = this->scannerResult[elementIdInt].begin();
				
				///
				while(scannerResultDataIterator != scannerResult[elementIdInt].end())
				{	
					if(((*scannerResultDataIterator).first) == A__PARTNER_LINK)
					{
						partnerLinkFlag = true;
						trace(TRACE_DEBUG,(*scannerResultDataIterator).first + "\n");
					}
					else if(((*scannerResultDataIterator).first) == A__PORT_TYPE)
					{
						portTypeFlag = true;
						trace(TRACE_DEBUG,(*scannerResultDataIterator).first + "\n");
					}
					else if(((*scannerResultDataIterator).first) == A__OPERATION)
					{
						operationFlag = true;
						trace(TRACE_DEBUG,(*scannerResultDataIterator).first + "\n");
					}					
					++scannerResultDataIterator;
				}
				
				if(!partnerLinkFlag)
				{
					printCheckErrorMsg("attribute " + A__PARTNER_LINK + "=\"" + T__NCNAME + "\" is missing");					
				}
				else if(!portTypeFlag)
				{
					printCheckErrorMsg("attribute " + A__PORT_TYPE + "=\"" + T__QNAME + "\" is missing");					
				}
				else if(!operationFlag)
				{
					printCheckErrorMsg("attribute " + A__OPERATION + "=\"" + T__NCNAME + "\" is missing");					
				}
			}
			break;		

		case K_REPLY:
			{
				trace(TRACE_DEBUG,"REPLY!!! \n");

				bool partnerLinkFlag, portTypeFlag, operationFlag;
				partnerLinkFlag = portTypeFlag = operationFlag = false;
				 
				scannerResultDataIterator = this->scannerResult[elementIdInt].begin();
				
				///
				while(scannerResultDataIterator != scannerResult[elementIdInt].end())
				{	
					if(((*scannerResultDataIterator).first) == A__PARTNER_LINK)
					{
						partnerLinkFlag = true;
						trace(TRACE_DEBUG,(*scannerResultDataIterator).first + "\n");
					}
					else if(((*scannerResultDataIterator).first) == A__PORT_TYPE)
					{
						portTypeFlag = true;
						trace(TRACE_DEBUG,(*scannerResultDataIterator).first + "\n");
					}
					else if(((*scannerResultDataIterator).first) == A__OPERATION)
					{
						operationFlag = true;
						trace(TRACE_DEBUG,(*scannerResultDataIterator).first + "\n");
					}					
					++scannerResultDataIterator;
				}
				
				if(!partnerLinkFlag)
				{
					printCheckErrorMsg("attribute " + A__PARTNER_LINK + "=\"" + T__NCNAME + "\" is missing");					
				}
				else if(!portTypeFlag)
				{
					printCheckErrorMsg("attribute " + A__PORT_TYPE + "=\"" + T__QNAME + "\" is missing");					
				}
				else if(!operationFlag)
				{
					printCheckErrorMsg("attribute " + A__OPERATION + "=\"" + T__NCNAME + "\" is missing");					
				}				
			}
			break;		

		case K_INVOKE:
			{
				trace(TRACE_DEBUG,"INVOKE!!! \n");

				bool partnerLinkFlag, portTypeFlag, operationFlag;
				partnerLinkFlag = portTypeFlag = operationFlag = false;
				 
				scannerResultDataIterator = this->scannerResult[elementIdInt].begin();
				
				///
				while(scannerResultDataIterator != scannerResult[elementIdInt].end())
				{	
					if(((*scannerResultDataIterator).first) == A__PARTNER_LINK)
					{
						partnerLinkFlag = true;
						trace(TRACE_DEBUG,(*scannerResultDataIterator).first + "\n");
					}
					else if(((*scannerResultDataIterator).first) == A__PORT_TYPE)
					{
						portTypeFlag = true;
						trace(TRACE_DEBUG,(*scannerResultDataIterator).first + "\n");
					}
					else if(((*scannerResultDataIterator).first) == A__OPERATION)
					{
						operationFlag = true;
						trace(TRACE_DEBUG,(*scannerResultDataIterator).first + "\n");
					}					
					++scannerResultDataIterator;
				}
				
				if(!partnerLinkFlag)
				{
					printCheckErrorMsg("attribute " + A__PARTNER_LINK + "=\"" + T__NCNAME + "\" is missing");					
				}
				else if(!portTypeFlag)
				{
					printCheckErrorMsg("attribute " + A__PORT_TYPE + "=\"" + T__QNAME + "\" is missing");					
				}
				else if(!operationFlag)
				{
					printCheckErrorMsg("attribute " + A__OPERATION + "=\"" + T__NCNAME + "\" is missing");					
				}								
			}
			break;		

		case K_ASSIGN:
			{
				trace(TRACE_DEBUG,"ASSIGN!!! \n");

				/* no mandatory attributes */				

			}
			break;		

		case K_THROW:
			{
				trace(TRACE_DEBUG,"THROW!!! \n");

				bool faultNameFlag;
				faultNameFlag = false;
				 
				scannerResultDataIterator = this->scannerResult[elementIdInt].begin();
				
				///
				while(scannerResultDataIterator != scannerResult[elementIdInt].end())
				{	
					if(((*scannerResultDataIterator).first) == A__FAULT_NAME)
					{
						faultNameFlag = true;
						trace(TRACE_DEBUG,(*scannerResultDataIterator).first + "\n");
					}
					++scannerResultDataIterator;
				}
				
				if(!faultNameFlag)
				{
					printCheckErrorMsg("attribute " + A__FAULT_NAME + "=\"" + T__QNAME + "\" is missing");					
				}

			}
			break;		

		case K_WAIT:
			{
				trace(TRACE_DEBUG,"WAIT!!! \n");
				
				bool forFlag, untilFlag;
				forFlag = untilFlag = false;
				 
				scannerResultDataIterator = this->scannerResult[elementIdInt].begin();
				
				///
				while(scannerResultDataIterator != scannerResult[elementIdInt].end())
				{	
					if(((*scannerResultDataIterator).first) == A__FOR)
					{
						forFlag = true;
						trace(TRACE_DEBUG,(*scannerResultDataIterator).first + "\n");
					}
					else if(((*scannerResultDataIterator).first) == A__UNTIL)
					{
						untilFlag = true;
						trace(TRACE_DEBUG,(*scannerResultDataIterator).first + "\n");
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
						printCheckErrorMsg("attribute " + A__FOR + "=\"" + T__DURATION_EXPR + "\"  or " + A__UNTIL + "=\"" + T__DEADLINE_EXPR + " is missing");						
					}
					// to much attributes, only one is allowed
					else
					{
						printCheckErrorMsg("both attributes " + A__FOR + "=\"" + T__DURATION_EXPR + "\"  and " + A__UNTIL + "=\"" + T__DEADLINE_EXPR + " are not allowed");												
					}
				}
			}
			break;		

		case K_EMPTY:
			{
				trace(TRACE_DEBUG,"EMPTY!!! \n");
				
				/* no mandatory attributes */
				
			}
			break;		

		case K_TERMINATE:
			{
				trace(TRACE_DEBUG,"TERMINATE!!! \n");
				
				/* no mandatory attributes */
				
			}
			break;		
		case K_SCOPE:
			{
				trace(TRACE_DEBUG,"SCOPE!!! \n");
				
				/* default attribute */
				
			}
			break;
		case K_COMPENSATE:
			{
				trace(TRACE_DEBUG,"COMPENSATE!!! \n");

				/* no mandatory attributes */				
				
			}
			break;				
		case K_PICK:
			{
				trace(TRACE_DEBUG,"PICK!!! \n");
				
				/* default attribute */
			}
			break;				
		case K_SEQUENCE:
			{
				trace(TRACE_DEBUG,"SEQUENCE!!! \n");

				/* no mandatory attributes */				
				
			}
			break;				
		case K_SWITCH:
			{
				trace(TRACE_DEBUG,"SWITCH!!! \n");

				/* no mandatory attributes */				

			}
			break;				
		case K_WHILE:
			{
				trace(TRACE_DEBUG,"WHILE!!! \n");

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
					printCheckErrorMsg("attribute " + A__CONDITION + "=\"" + T__BOOLEAN_EXPR + "\" is missing");					
				}
	
				
			}
			break;				
	}

}
