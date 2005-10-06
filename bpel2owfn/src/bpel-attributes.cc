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
    result = "\"\"";
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
