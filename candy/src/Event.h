#ifndef EVENT_H_
#define EVENT_H_

#include "settings.h"



/// possible event types
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

    	/// the event's name
        string name;

        /// the event's cost
        unsigned int cost;

        /// the event's type
        EventType type;



        /*--------.
		| methods |
		`--------*/

        /// basic constructor
        explicit Event(string _name, unsigned int _cost, EventType _type) :
        	name(_name), cost(_cost), type(_type) {};

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
            file << endl;
        };

};

#endif /* EVENT_H_ */
