#ifndef EVENT_H_
#define EVENT_H_

#include <iostream>
#include <string>

using std::cout;
using std::endl;
using std::string;


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
        bool written;


        /*--------.
		| methods |
		`--------*/

        /// basic constructor
        explicit Event(string _name, unsigned int _cost, EventType _type, bool _written) :
        	name(_name), cost(_cost), type(_type), written(_written) {};

        /// basic destructor
        ~Event() {};

        /// print information about this event on std::cout
        void printToStdout() {
        	cout << "event '" << name << "', " << cost << ", ";
        	if ( type == T_INPUT ) {
        		cout << "input";
        	} else if ( type == T_OUTPUT ) {
        		cout << "output";
        	} else {
        		cout << "sync";
        	}
        	cout << ", " << (written ? "true" : "false") << endl;
        };

};

#endif /* EVENT_H_ */
