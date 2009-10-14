#ifndef EVENTTERMPARSER_H_
#define EVENTTERMPARSER_H_

#include "helpers.h"
#include "eventTerm.h"

class EventTermParser {
public:
	static bool initialize();

	static std::vector<EventTerm*>* parseFile(std::string filename);

};



#endif /* EVENTTERMPARSER_H_ */
