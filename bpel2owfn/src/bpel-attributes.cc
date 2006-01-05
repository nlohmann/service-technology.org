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
 * \file bpel-attributes.c
 *
 * \brief This file implements the classes and functions defined in bpel-attributes.h.
 * 
 * \author  
 *          - responsible: Dennis Reinert <reinert@informatik.hu-berlin.de>
 *          - last changes of: \$Author: reinert $
 *          
 * \date
 *          - created: 2005/10/18
 *          - last changed: \$Date: 2006/01/05 16:23:32 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universit�t zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel
 *          for details.
 *
 * \version \$Revision: 1.27 $
 *
 * \todo
 *       - (reinert) Comment this file and its classes.
 *       - (reinert) Check data types (e.g. is a value "yes" or "no").
 */

#include "bpel-attributes.h"
#include "bpel-syntax.h"
#include "debug.h"

/*!
 * constructor
 */
attributeManager::attributeManager()
{
  this->scannerResult.clear();
  this->nodeId = 0;
}

/*!
 * returns the value of an attribute which was stored in the AM array
 * \param elementId key to identification the BPEL-element in the AM array
 * \param attributeName key to attribute value
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
 * returns the value of an attribute which was stored in the AM array
 * \param elementId key to identification the BPEL-element in the AM array
 * \param attributeName key to attribute value
 * \param defaultValue
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
 * increase the elementId of the AM array 
 */
kc::integer attributeManager::nextId()
{
  this->nodeId++;
  return kc::mkinteger(this->nodeId);
}

/*!
 * yes-no domain check
 * \param attributeName
 * \param attributeValue 
 */
void attributeManager::checkAttributeValueYesNo(std::string attributeName, std::string attributeValue)
{
   	if((attributeValue != (string)"yes") && (attributeValue != (string)"no"))
  	{
  		printErrorMsg("wrong value of " + attributeName + " attribute");
  	}	
}

/*!
 * returns valid or unvalid depending on attribute value
 * \param attributeName
 * \param attributeValue 
 */
bool attributeManager::isValidAttributeValue(std::string attributeName, std::string attributeValue)
{

  /// check all attributes with yes or no domain
  if(attributeName == A__ABSTRACT_PROCESS)
  {
	checkAttributeValueYesNo(attributeName, attributeValue);
  }
  else if(attributeName == A__CREATE_INSTANCE)
  {
	checkAttributeValueYesNo(attributeName, attributeValue);  	
  }
  else if(attributeName == A__ENABLE_INSTANCE_COMPENSATION)
  {
	checkAttributeValueYesNo(attributeName, attributeValue);
  }
  else if(attributeName == A__INITIATE)
  {
	checkAttributeValueYesNo(attributeName, attributeValue);  	
  }
  else if(attributeName == A__SUPPRESS_JOIN_FAILURE)
  {
	checkAttributeValueYesNo(attributeName, attributeValue);  	
  }
  else if(attributeName == A__VARIABLE_ACCESS_SERIALIZABLE)
  {
	checkAttributeValueYesNo(attributeName, attributeValue);  	
  }
  
  return true;
}

/*!
 * 
 */
bool attributeManager::isDuplicate(std::string attributeName)
{

	std::map<std::string, std::string>::iterator scannerResultDataIterator;
	scannerResultDataIterator = this->scannerResult[this->nodeId].begin();
	
	/// iteration loop about all xml-element attributes
	while(scannerResultDataIterator != this->scannerResult[this->nodeId].end())
	{
		if(attributeName.compare((*scannerResultDataIterator).first) == 0)
		{
			return true;
		}
		++scannerResultDataIterator;
	}	
	return false;	
}

/*!
 * Stored an attribute with his name and value into the AM array under
 * the current BPEL-element id. The BPEL element id is managed by the AM.
 * \param attributeName
 * \param attributeValue 
 */
void attributeManager::define(kc::casestring attributeName, kc::casestring attributeValue)
{  	
	  
  //traceAM("define\n" + string(attributeName->name) + "=" + string(attributeValue->name) + "\n");

  if(isDuplicate(attributeName->name))
  {
  	//traceAM("define DUPLIKAT GEFUNDEN\n" + string(attributeName->name) + "=" + string(attributeValue->name) + "\n");
  	printErrorMsg("attribute <" + (string)attributeName->name + "> already exist");
  }

  if(isValidAttributeValue(attributeName->name, attributeValue->name))  
  {
  	scannerResult[this->nodeId][attributeName->name] = attributeValue->name;
  }
}

/*!
 * print formatted attribute manager error message
 * \param errorMsg
 */
void attributeManager::printErrorMsg(std::string errorMsg)
{
	yyerror(string("[AttributeManager]: " + errorMsg + "\n").c_str());
}

/*!
 * trace method for the AM class
 * \param traceMsg
 */
void attributeManager::traceAM(std::string traceMsg)
{
	trace(TRACE_DEBUG, "[AM] " + traceMsg);	
}

/*!
 * checked the attributes and the value of BPEL-elements
 * \param elementId
 * \param elementValue
 * \param elementType
 */
void attributeManager::check(kc::integer elementId, kc::casestring elementValue, unsigned int elementType)
{	
	if(elementType == K_FROM)
	{	
		std::string lit = elementValue->name;

		/* without attributes */
		if(scannerResult[elementId->value].size() == 0)
		{	/* literal value is empty */
			if(lit.empty())
			{
				printErrorMsg("from clause is wrong");
			}
		}
		/* with attributes */
		else
		{	/* literal value is empty */
			if(lit.empty())
			{
				/* element with attributes and without literal value */
				check(elementId, elementType);
			}
			else
			{
				printErrorMsg("from clause is wrong");				
			}
		}
	}
	else
	{	
		check(elementId, elementType);			
	}
}

/*!
 * checked the attributes of BPEL-elements
 * \param elementId
 * \param elementType
 */
void attributeManager::check(kc::integer elementId, unsigned int elementType)
{
	// "cast" Kimwitu++ to C++
  	unsigned int elementIdInt = elementId->value; 
	/// iterator for the embedded map
    std::map<std::string, string>::iterator scannerResultDataIterator;
	
	switch(elementType) {
		case K_ASSIGN:
			{
				traceAM("ASSIGN\n");

				/* no mandatory attributes */				

			}
			break;

		case K_CASE:
			{
				traceAM("CASE\n");

				bool conditionFlag;
				conditionFlag = false;
				 
				scannerResultDataIterator = this->scannerResult[elementIdInt].begin();
				
				///
				while(scannerResultDataIterator != scannerResult[elementIdInt].end())
				{	
					if(((*scannerResultDataIterator).first) == A__CONDITION)
					{
						conditionFlag = true;
						traceAM((*scannerResultDataIterator).first + "\n");
					}
					++scannerResultDataIterator;
				}
				
				if(!conditionFlag)
				{
					printErrorMsg("attribute " + A__CONDITION + "=\"" + T__BOOLEAN_EXPR + "\" is missing");					
				}				
				
			}
			break;

		case K_CATCH:
			{
				traceAM("CATCH\n");
				
				/* no mandatory attributes */
				
			}
			break;	

		case K_COMPENSATE:
			{
				traceAM("COMPENSATE\n");

				/* no mandatory attributes */				
				
			}
			break;

		case K_FROM:
			{
				traceAM("FROM\n");
				
				bool validFrom = false;
				unsigned int attributeCompareCounter;
							
				// array to tag found attributes
				// to store the number of found attributes -> COUNTER field
				unsigned int foundAttributes[] =
				{	/*variable, part, query, partnerLk, endpointRef, property, expression, opaque, COUNTER*/
						 0,      0,     0,       0,          0,          0,         0,        0,      0
				};

				unsigned int CSIZE = 7; // number of valid combination of attributes
				unsigned int ASIZE = 9; // number of attributes + COUNTER
				unsigned int COUNTER = 8; // position of counter within the arrays 

				// matrix for valid combination of attributes
				unsigned int validAttributeCombination[][9] =
				{	/*variable, part, query, partnerLk, endpointRef, property, expression, opaque, COUNTER*/
				 	{	 1,      0,     0,       0,          0,          0,         0,        0,      1    },
				 	{	 1,      1,     0,       0,          0,          0,         0,        0,      2    },
				 	{	 1,      1,     1,       0,          0,          0,         0,        0,      3    },
				 	{	 0,      0,     0,       1,          1,          0,         0,        0,      2    },
				 	{	 1,      0,     0,       0,          0,          1,         0,        0,      2    },
				 	{	 0,      0,     0,       0,          0,          0,         1,        0,      1    },
				 	{	 0,      0,     0,       0,          0,          0,         0,        1,      1    }
				};
				 
				scannerResultDataIterator = this->scannerResult[elementIdInt].begin();
				
				///
				while(scannerResultDataIterator != scannerResult[elementIdInt].end())
				{	
					if(((*scannerResultDataIterator).first) == A__VARIABLE)
					{
						foundAttributes[0] = 1;
						foundAttributes[COUNTER]++;
						traceAM((*scannerResultDataIterator).first + "\n");
					}
					else if(((*scannerResultDataIterator).first) == A__PART)
					{
						foundAttributes[1] = 1;
						foundAttributes[COUNTER]++;
						traceAM((*scannerResultDataIterator).first + "\n");
					}
					else if(((*scannerResultDataIterator).first) == A__QUERY)
					{
						foundAttributes[2] = 1;
						foundAttributes[COUNTER]++;						
						traceAM((*scannerResultDataIterator).first + "\n");
					}					
					else if(((*scannerResultDataIterator).first) == A__PARTNER_LINK)
					{
						foundAttributes[3] = 1;
						foundAttributes[COUNTER]++;						
						traceAM((*scannerResultDataIterator).first + "\n");
					}
					else if(((*scannerResultDataIterator).first) == A__ENDPOINT_REFERENCE)
					{
						foundAttributes[4] = 1;
						foundAttributes[COUNTER]++;
						traceAM((*scannerResultDataIterator).first + "\n");
					}
					else if(((*scannerResultDataIterator).first) == A__PROPERTY)
					{
						foundAttributes[5] = 1;
						foundAttributes[COUNTER]++;
						traceAM((*scannerResultDataIterator).first + "\n");
					}
					else if(((*scannerResultDataIterator).first) == A__EXPRESSION)
					{
						foundAttributes[6] = 1;
						foundAttributes[COUNTER]++;						
						traceAM((*scannerResultDataIterator).first + "\n");
					}
					else if(((*scannerResultDataIterator).first) == A__OPAQUE)
					{
						foundAttributes[7] = 1;
						foundAttributes[COUNTER]++;						
						traceAM((*scannerResultDataIterator).first + "\n");
					}																				
					++scannerResultDataIterator;
				}
								
				// iteration about valid attribute combination
				for(unsigned int c = 0; c < CSIZE; c++)
				{
					// under or too much found attributes ... next combination
					if(validAttributeCombination[c][COUNTER] != foundAttributes[COUNTER]) continue;
	
					attributeCompareCounter = 0;
					
					// iteration about attributes
					for(unsigned int a = 0; a < ASIZE-1; a++)
					{	// compare scanned combination of attributes with allowed combination of attributes
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
				traceAM("INVOKE\n");

				bool partnerLinkFlag, portTypeFlag, operationFlag, inputVariableFlag, outputVariableFlag;
				partnerLinkFlag = portTypeFlag = operationFlag = inputVariableFlag = outputVariableFlag =false;
				 
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
					else if(((*scannerResultDataIterator).first) == A__INPUT_VARIABLE)
					{
						inputVariableFlag = true;
						traceAM((*scannerResultDataIterator).first + "\n");
					}
					/* optional */
					/*
					else if(((*scannerResultDataIterator).first) == A__OUTPUT_VARIABLE)
					{
						outputVariableFlag = true;
						traceAM((*scannerResultDataIterator).first + "\n");
					}*/
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

		case K_ONALARM:
			{
				traceAM("ONALARM\n");
				
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

		case K_ONMESSAGE:
			{
				traceAM("ONMESSAGE\n");
				
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
				
		case K_PICK:
			{
				traceAM("PICK\n");
				
				/* default attribute */
			}
			break;				

		case K_PROCESS:
			{
				traceAM("PROCESS\n");
				
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
				traceAM("RECEIVE\n");			

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
				traceAM("REPLY\n");

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

		case K_SCOPE:
			{
				traceAM("SCOPE\n");
				
				/* default attribute */
				
			}
			break;

		case K_SEQUENCE:
			{
				traceAM("SEQUENCE\n");

				/* no mandatory attributes */				
				
			}
			break;				

		case K_SOURCE:
			{
				traceAM("SOURCE\n");

				bool linkNameFlag;
				linkNameFlag = false;
				 
				scannerResultDataIterator = this->scannerResult[elementIdInt].begin();
				
				///
				while(scannerResultDataIterator != scannerResult[elementIdInt].end())
				{	
					if(((*scannerResultDataIterator).first) == A__LINK_NAME)
					{
						linkNameFlag = true;
						traceAM((*scannerResultDataIterator).first + "\n");
					}
					++scannerResultDataIterator;
				}
				
				if(!linkNameFlag)
				{
					printErrorMsg("attribute " + A__LINK_NAME + "=\"" + T__NCNAME + "\" is missing");					
				}

			}
			break;	


		case K_SWITCH:
			{
				traceAM("SWITCH\n");

				/* no mandatory attributes */				

			}
			break;				

		case K_TARGET:
			{
				traceAM("TARGET\n");

				bool linkNameFlag;
				linkNameFlag = false;
				 
				scannerResultDataIterator = this->scannerResult[elementIdInt].begin();
				
				///
				while(scannerResultDataIterator != scannerResult[elementIdInt].end())
				{	
					if(((*scannerResultDataIterator).first) == A__LINK_NAME)
					{
						linkNameFlag = true;
						traceAM((*scannerResultDataIterator).first + "\n");
					}
					++scannerResultDataIterator;
				}
				
				if(!linkNameFlag)
				{
					printErrorMsg("attribute " + A__LINK_NAME + "=\"" + T__NCNAME + "\" is missing");					
				}

			}
			break;	

		case K_TERMINATE:
			{
				traceAM("TERMINATE\n");
				
				/* no mandatory attributes */
				
			}
			break;		

		case K_THROW:
			{
				traceAM("THROW\n");

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

		case K_TO:
			{
				traceAM("TO\n");
				
				bool validTo = false;
				unsigned int attributeCompareCounter;
							
				// array to tag found attributes
				// to store the number of found attributes -> COUNTER field
				unsigned int foundAttributes[] =
				{	/*variable, part, query, partnerLk, property, COUNTER*/
						 0,      0,     0,       0,          0,      0
				};

				unsigned int CSIZE = 5; // number of valid combination of attributes
				unsigned int ASIZE = 6; // number of attributes + COUNTER
				unsigned int COUNTER = 5; // position of counter within the arrays 

				// matrix for valid combination of attributes
				unsigned int validAttributeCombination[][6] =
				{	/*variable, part, query, partnerLk, property, COUNTER*/
				 	{	 1,      0,     0,       0,        0,        1    },
				 	{	 1,      1,     0,       0,        0,        2    },
				 	{	 1,      1,     1,       0,        0,        3    },
				 	{	 0,      0,     0,       1,        0,        1    },
				 	{	 1,      0,     0,       0,        1,        2    }
				};
				 
				scannerResultDataIterator = this->scannerResult[elementIdInt].begin();
				
				///
				while(scannerResultDataIterator != scannerResult[elementIdInt].end())
				{	
					if(((*scannerResultDataIterator).first) == A__VARIABLE)
					{
						foundAttributes[0] = 1;
						foundAttributes[COUNTER]++;
						traceAM((*scannerResultDataIterator).first + "\n");
					}
					else if(((*scannerResultDataIterator).first) == A__PART)
					{
						foundAttributes[1] = 1;
						foundAttributes[COUNTER]++;
						traceAM((*scannerResultDataIterator).first + "\n");
					}
					else if(((*scannerResultDataIterator).first) == A__QUERY)
					{
						foundAttributes[2] = 1;
						foundAttributes[COUNTER]++;						
						traceAM((*scannerResultDataIterator).first + "\n");
					}					
					else if(((*scannerResultDataIterator).first) == A__PARTNER_LINK)
					{
						foundAttributes[3] = 1;
						foundAttributes[COUNTER]++;						
						traceAM((*scannerResultDataIterator).first + "\n");
					}
					else if(((*scannerResultDataIterator).first) == A__PROPERTY)
					{
						foundAttributes[4] = 1;
						foundAttributes[COUNTER]++;
						traceAM((*scannerResultDataIterator).first + "\n");
					}
					++scannerResultDataIterator;
				}
								
				// iteration about valid attribute combination
				for(unsigned int c = 0; c < CSIZE; c++)
				{
					// under or too much found attributes ... next combination
					if(validAttributeCombination[c][COUNTER] != foundAttributes[COUNTER]) continue;
	
					attributeCompareCounter = 0;
					
					// iteration about attributes
					for(unsigned int a = 0; a < ASIZE-1; a++)
					{	// compare scanned combination of attributes with allowed combination of attributes
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

		case K_WAIT:
			{
				traceAM("WAIT\n");
				
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

		case K_WHILE:
			{
				traceAM("WHILE\n");

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
