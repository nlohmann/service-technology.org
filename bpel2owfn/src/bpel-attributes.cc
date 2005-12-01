/*!
 * \file bpel-attributes.c
 *
 * \brief Functions for Petri nets (implementation)
 *
 * This file implements the classes and functions defined in bpel-attributes.h.
 * 
 * \author  
 *          - responsible: Niels Lohmann <nlohmann@informatik.hu-berlin.de>
 *          - last changes of: \$Author: reinert $
 *          
 * \date
 *          - created: 2005/10/18
 *          - last changed: \$Date: 2005/12/01 14:05:32 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universit� zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel
 *          for details.
 *
 * \version \$Revision: 1.6 $
 *
 */

#include "bpel-attributes.h" // ...
#include "bpel-syntax.h" // K_PROCESS ...
#include "debug.h" // trace() ... 


/********************************************
 * implementation of attributeManager CLASS
 ********************************************/

/*!
 * contructor
 */
attributeManager::attributeManager()
{
  this->scannerResult.clear();
  this->attributeDB.clear();
  this->nodeId = 0;
  initAttributeDB();
}

/*!
 * 
 */
kc::casestring attributeManager::read(kc::integer elementId, std::string attributeName)
{
  // "cast" Kimwitu++ to C++
  unsigned int elementIdInt = elementId->value;
  	
  //trace(attributeName + "=" + intToString(elementIdInt) + "\n");
  
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
  trace(attributeName->name);
  trace("=");
  trace(attributeValue->name);
  trace("\n");
}


/*!
 * to initialize the attributeDB
 */ 
void attributeManager::initAttributeDB()
{
	/*
	 *  [activity-id][attribute-name][optional][tagged][content type][default content]
	 * 	
	 * 	[optional] -> true <optional attribute> | false <mandotory attribute>
	 * 	[tagged] -> true | false
	 * 	[content type] -> 0 <not empty> | 1 <yes | no>
	 */
	
	// process
	this->attributeDB[K_PROCESS][A__NAME] = new attributeDBData(false, false, 0);
	this->attributeDB[K_PROCESS][A__TARGET_NAMESPACE] = new attributeDBData(false, false, 0);
	this->attributeDB[K_PROCESS][A__QUERY_LANGUAGE] = new attributeDBData(true, false, 0);
	this->attributeDB[K_PROCESS][A__EXPRESSION_LANGUAGE] = new attributeDBData(true, false, 0);
	this->attributeDB[K_PROCESS][A__SUPPRESS_JOIN_FAILURE] = new attributeDBData(true, false, 1);
	this->attributeDB[K_PROCESS][A__ENABLE_INSTANCE_COMPENSATION] = new attributeDBData(true, false, 1);
	this->attributeDB[K_PROCESS][A__ABSTRACT_PROCESS] = new attributeDBData(true, false, 1);
	this->attributeDB[K_PROCESS][A__XMLNS] = new attributeDBData(false, false, 0);
	
	// receive
	this->attributeDB[K_RECEIVE][A__PARTNER_LINK] = new attributeDBData(false, false, 0);
	this->attributeDB[K_RECEIVE][A__PORT_TYPE] = new attributeDBData(false, false, 0);
	this->attributeDB[K_RECEIVE][A__OPERATION] = new attributeDBData(false, false, 0);
	this->attributeDB[K_RECEIVE][A__VARIABLE] = new attributeDBData(true, false, 0);
	this->attributeDB[K_RECEIVE][A__CREATE_INSTANCE] = new attributeDBData(true, false, 1);
	//	standard-attributes
	this->attributeDB[K_RECEIVE][A__NAME] = new attributeDBData(true, false, 0);
	this->attributeDB[K_RECEIVE][A__JOIN_CONDITION] = new attributeDBData(true, false, 0);
	this->attributeDB[K_RECEIVE][A__SUPPRESS_JOIN_FAILURE] = new attributeDBData(true, false, 1);	
	
	// reply
	this->attributeDB[K_REPLY][A__PARTNER_LINK] = new attributeDBData(false, false, 0);
	this->attributeDB[K_REPLY][A__PORT_TYPE] = new attributeDBData(false, false, 0);
	this->attributeDB[K_REPLY][A__OPERATION] = new attributeDBData(false, false, 0);
	this->attributeDB[K_REPLY][A__VARIABLE] = new attributeDBData(true, false, 0);
	this->attributeDB[K_REPLY][A__FAULT_NAME] = new attributeDBData(true, false, 0);
	//	standard-attributes
	this->attributeDB[K_REPLY][A__NAME] = new attributeDBData(true, false, 0);
	this->attributeDB[K_REPLY][A__JOIN_CONDITION] = new attributeDBData(true, false, 0);
	this->attributeDB[K_REPLY][A__SUPPRESS_JOIN_FAILURE] = new attributeDBData(true, false, 1);	

	// invoke
	this->attributeDB[K_REPLY][A__PARTNER_LINK] = new attributeDBData(false, false, 0);
	this->attributeDB[K_REPLY][A__PORT_TYPE] = new attributeDBData(false, false, 0);
	this->attributeDB[K_REPLY][A__OPERATION] = new attributeDBData(false, false, 0);
	this->attributeDB[K_REPLY][A__INPUT_VARIABLE] = new attributeDBData(true, false, 0);
	this->attributeDB[K_REPLY][A__OUTPUT_VARIABLE] = new attributeDBData(true, false, 0);
	//	standard-attributes
	this->attributeDB[K_RECEIVE][A__NAME] = new attributeDBData(true, false, 0);
	this->attributeDB[K_RECEIVE][A__JOIN_CONDITION] = new attributeDBData(true, false, 0);
	this->attributeDB[K_RECEIVE][A__SUPPRESS_JOIN_FAILURE] = new attributeDBData(true, false, 1);	
	
	// assign
	//	standard-attributes
	this->attributeDB[K_ASSIGN][A__NAME] = new attributeDBData(true, false, 0);
	this->attributeDB[K_ASSIGN][A__JOIN_CONDITION] = new attributeDBData(true, false, 0);
	this->attributeDB[K_ASSIGN][A__SUPPRESS_JOIN_FAILURE] = new attributeDBData(true, false, 1);	
	
	// throw
	this->attributeDB[K_THROW][A__FAULT_NAME] = new attributeDBData(false, false, 0);
	this->attributeDB[K_THROW][A__FAULT_VARIABLE] = new attributeDBData(true, false, 0);
	//	standard-attributes
	this->attributeDB[K_RECEIVE][A__NAME] = new attributeDBData(true, false, 0);
	this->attributeDB[K_RECEIVE][A__JOIN_CONDITION] = new attributeDBData(true, false, 0);
	this->attributeDB[K_RECEIVE][A__SUPPRESS_JOIN_FAILURE] = new attributeDBData(true, false, 1);	
	
	// wait
	this->attributeDB[K_WAIT][A__FOR] = new attributeDBData(false, false, 0);	
	this->attributeDB[K_WAIT][A__UNTIL] = new attributeDBData(false, false, 0);
	//	standard-attributes
	this->attributeDB[K_RECEIVE][A__NAME] = new attributeDBData(true, false, 0);
	this->attributeDB[K_RECEIVE][A__JOIN_CONDITION] = new attributeDBData(true, false, 0);
	this->attributeDB[K_RECEIVE][A__SUPPRESS_JOIN_FAILURE] = new attributeDBData(true, false, 1);	
	
	// empty
	//	standard-attributes
	this->attributeDB[K_EMPTY][A__NAME] = new attributeDBData(true, false, 0);
	this->attributeDB[K_EMPTY][A__JOIN_CONDITION] = new attributeDBData(true, false, 0);
	this->attributeDB[K_EMPTY][A__SUPPRESS_JOIN_FAILURE] = new attributeDBData(true, false, 1);	
		
	// terminate
	//	standard-attributes
	this->attributeDB[K_TERMINATE][A__NAME] = new attributeDBData(true, false, 0);
	this->attributeDB[K_TERMINATE][A__JOIN_CONDITION] = new attributeDBData(true, false, 0);
	this->attributeDB[K_TERMINATE][A__SUPPRESS_JOIN_FAILURE] = new attributeDBData(true, false, 1);
	
	// flow
	//	standard-attributes
	this->attributeDB[K_RECEIVE][A__NAME] = new attributeDBData(true, false, 0);
	this->attributeDB[K_RECEIVE][A__JOIN_CONDITION] = new attributeDBData(true, false, 0);
	this->attributeDB[K_RECEIVE][A__SUPPRESS_JOIN_FAILURE] = new attributeDBData(true, false, 1);	
	
	// switch
	//	standard-attributes
	this->attributeDB[K_SWITCH][A__NAME] = new attributeDBData(true, false, 0);
	this->attributeDB[K_SWITCH][A__JOIN_CONDITION] = new attributeDBData(true, false, 0);
	this->attributeDB[K_SWITCH][A__SUPPRESS_JOIN_FAILURE] = new attributeDBData(true, false, 1);	
	
	// while
	this->attributeDB[K_WHILE][A__CONDITION] = new attributeDBData(false, false, 0);
	//	standard-attributes
	this->attributeDB[K_WHILE][A__NAME] = new attributeDBData(true, false, 0);
	this->attributeDB[K_WHILE][A__JOIN_CONDITION] = new attributeDBData(true, false, 0);
	this->attributeDB[K_WHILE][A__SUPPRESS_JOIN_FAILURE] = new attributeDBData(true, false, 1);	
	
	// sequence
	//	standard-attributes
	this->attributeDB[K_SEQUENCE][A__NAME] = new attributeDBData(true, false, 0);
	this->attributeDB[K_SEQUENCE][A__JOIN_CONDITION] = new attributeDBData(true, false, 0);
	this->attributeDB[K_SEQUENCE][A__SUPPRESS_JOIN_FAILURE] = new attributeDBData(true, false, 1);	
	
	// pick
	this->attributeDB[K_PICK][A__CREATE_INSTANCE] = new attributeDBData(true, false, 1);
	//	standard-attributes
	this->attributeDB[K_PICK][A__NAME] = new attributeDBData(true, false, 0);
	this->attributeDB[K_PICK][A__JOIN_CONDITION] = new attributeDBData(true, false, 0);
	this->attributeDB[K_PICK][A__SUPPRESS_JOIN_FAILURE] = new attributeDBData(true, false, 1);	
	
	// scope
	this->attributeDB[K_SCOPE][A__VARIABLE_ACCESS_SERIALIZABLE] = new attributeDBData(false, false, 1);	
	//	standard-attributes
	this->attributeDB[K_SCOPE][A__NAME] = new attributeDBData(true, false, 0);
	this->attributeDB[K_SCOPE][A__JOIN_CONDITION] = new attributeDBData(true, false, 0);
	this->attributeDB[K_SCOPE][A__SUPPRESS_JOIN_FAILURE] = new attributeDBData(true, false, 1);	
	
	// compensate
	this->attributeDB[K_COMPENSATE][A__SCOPE] = new attributeDBData(true, false, 0);	
	//	standard-attributes
	this->attributeDB[K_COMPENSATE][A__NAME] = new attributeDBData(true, false, 0);
	this->attributeDB[K_COMPENSATE][A__JOIN_CONDITION] = new attributeDBData(true, false, 0);
	this->attributeDB[K_COMPENSATE][A__SUPPRESS_JOIN_FAILURE] = new attributeDBData(true, false, 1);	
	
}

/*!
 * 
 */
bool attributeManager::isValidElement(unsigned int elementId, unsigned int activityId) {
	/// iterator for the embedded map
	std::map<std::string, std::string>::iterator scannerResultDataIterator;
	scannerResultDataIterator = this->scannerResult[elementId].begin();
	
	/// iteration loop about all xml-element attributes
	while(scannerResultDataIterator != this->scannerResult[elementId].end())
	{
		/// to tag all found accurate attributes
		if(isAttribute(activityId, (*scannerResultDataIterator).first))
		{
			(this->attributeDB[activityId][(*scannerResultDataIterator).first])->setTag(true);
		}
		else
		{
			/* ignore
			 * 	- wrong spelling
			 *  - unknown attributes
			 *  */	
		}	
		++scannerResultDataIterator;
	}
	
	///	
	if(!isAccurateCombinationOfAttributes(activityId))
	{
		return false;
	}
	
	///
	if(!checkAttributeContent(activityId))
	{
		return false;
	}
		
	return true;
}


/*!
 * 
 */
bool attributeManager::isAttribute(unsigned int activityId, std::string attributeName)
{	
	/// iterator for the embedded map
	std::map<std::string, attributeDBData*>::iterator attributeDBDataIterator;
	attributeDBDataIterator = this->attributeDB[activityId].begin();
	
	while(attributeDBDataIterator != attributeDB[activityId].end())
	{	
		if(attributeName.compare((*attributeDBDataIterator).first) == 0)
		{
			return true;
		}
		++attributeDBDataIterator;
	}	
	return false;
}

/*!
 * 
 */
bool attributeManager::isAccurateCombinationOfAttributes(unsigned int activityId)
{
	/// iterator for the embedded map
	std::map<std::string, attributeDBData*>::iterator attributeDBDataIterator;
	attributeDBDataIterator = this->attributeDB[activityId].begin();

	while(attributeDBDataIterator != attributeDB[activityId].end())
	{	
		/// check of mandatory attribute
		if(!(!isOptional(activityId, (*attributeDBDataIterator).first) ==
			  isTagged(activityId, (*attributeDBDataIterator).first)));
		{
			/* doesn't scanned mandatory attribute was found */
			return false;	
		}
		
		/*blabla="naja"
		 * more checks are necessary
		 *	-  
		 *
		 */
		
		++attributeDBDataIterator;
	}
	
	return true;	
}

/*!
 * 
 */
bool attributeManager::isOptional(unsigned int activityId, std::string attributeName)
{	
	return (this->attributeDB[activityId][attributeName])->getOptional();
}

/*!
 * 
 */
bool attributeManager::isTagged(unsigned int activityId, std::string attributeName)
{	
	return (this->attributeDB[activityId][attributeName])->getTag();
}

/*!
 * 
 */
bool attributeManager::checkAttributeContent(unsigned int activityId)
{
	return true;
}

/*!
 * 
 * \param elementId	
 * \param type
 */
void attributeManager::check(kc::integer elementId, unsigned int activityId)
{	
//	trace("AttrM-ID: " + intToString(elementId->value) + "\n");
//	trace("Type: " + intToString(activityId) + "\n");

	if(!isValidElement(elementId->value, activityId))
	{
		//yyerror("attribute error");
	}
}


/********************************************
 * implementation of attributeDBData CLASS
 ********************************************/

/*!
 * 
 */
void attributeDBData::setOptional(bool val)
{
	this->optional = val;
}

/*!
 * 
 */
void attributeDBData::setTag(bool val)
{
	this->tag = val;
}

/*!
 * 
 */
void attributeDBData::setContentType(unsigned int val)
{
	this->contentType = val;
}

/*!
 * 
 */
bool attributeDBData::getOptional()
{
	return this->optional;	
}

/*!
 * 
 */
bool attributeDBData::getTag(){
	return this->tag;	
}

/*!
 * 
 */
unsigned int attributeDBData::getContentType(){
	return this->contentType;	
}


/*

	switch(activityId) {
		case K_PROCESS:
			{
				trace("PROCESS!!! \n");
				if(!isValidElement(elementId->value, activityId))
				{
				}				
			}
			break;		

		case K_RECEIVE:
			{
				trace("RECEIVE!!! \n");
				if(!isValidElement(elementId->value, activityId))
				{
					yyerror("FEHLER");
				}
			}
			break;		

		case K_REPLY:
			{
				trace("REPLY!!!");
				if(!isValidElement(elementId->value, activityId))
				{
				}				
			}
			break;		

		case K_INVOKE:
			{
				trace("INVOKE!!!");
				if(!isValidElement(elementId->value, activityId))
				{
				}				
			}
			break;		

		case K_ASSIGN:
			{
				trace("ASSIGN!!!");
			}
			break;		

		case K_THROW:
			{
				trace("THROW!!!");
			}
			break;		

		case K_WAIT:
			{
				trace("WAIT!!!");
			}
			break;		

		case K_EMPTY:
			{
				trace("EMPTY!!!");
			}
			break;		

		case K_TERMINATE:
			{
				trace("TERMINATE!!!");
			}
			break;		

	}



*/
