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
 *          - last changed: \$Date: 2005/11/17 13:13:11 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universit� zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel
 *          for details.
 *
 * \version \$Revision: 1.4 $
 *
 */

#include "bpel-attributes.h"



attributeManager::attributeManager()
{
  this->attributeArray.clear();
  this->nodeId = 0;
}



kc::casestring attributeManager::read(kc::integer elementId, std::string attributeName)
{
  // "cast" Kimwitu++ to C++
  unsigned int elementIdInt = elementId->value;

  
  std::string result;
  
  if(attributeArray[elementIdInt][attributeName].empty())
  {
    // no attribute-value given
    result = "";
  }
  else
  {
    result = attributeArray[elementIdInt][attributeName];
  }

  return kc::mkcasestring(result.c_str());
}



kc::integer attributeManager::nextId()
{
  this->nodeId++;
  return kc::mkinteger(this->nodeId);
}



void attributeManager::define(kc::casestring attributeName, kc::casestring attributeValue)
{
  attributeArray[this->nodeId][attributeName->name] = attributeValue->name;
}



void attributeManager::check(kc::integer elementId, unsigned int type)
{

}
