#ifndef EVENT_H_
#define EVENT_H_

#include "settings.h"



/// TODO comment
enum EventType {
  T_INPUT,
  T_OUTPUT,
  T_SYNC
};


class Event {

    public:

    	/*-----------.
		| attributes |
		`-----------*/

    	// TODO comment
        string name;
        unsigned int cost;
        EventType type;

        bool flag;


        /*--------.
		| methods |
		`--------*/

        /// basic constructor
        explicit Event(string _name, unsigned int _cost, EventType _type, bool _flag) :
        	name(_name), cost(_cost), type(_type), flag(_flag) {};

        /// basic destructor
        ~Event() {};

        /// print information about this event on std::cout
        void outputDebug(std::ostream& file) {
        	file << "event '" << name << "', " << cost << ", ";
        	if ( type == T_INPUT ) {
        		file << "input";
        	} else if ( type == T_OUTPUT ) {
        		file << "output";
        	} else if ( type == T_SYNC ) {
        		file << "sync";
        	} else {
                file << "invalid";
            }
        	file << ", flag " << (flag ? "true" : "false") << endl;
        };

};

#endif /* EVENT_H_ */
