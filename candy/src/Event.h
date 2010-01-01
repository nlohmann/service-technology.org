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

		explicit Event(string _name, EventType _type, unsigned int _cost = 0)
		: name(_name), type(_type), cost(_cost) {};

		~Event() {};

		string getName() const {
			return name;
		};

		EventType getType() const {
			return type;
		};

		unsigned int getCost() const {
			return cost;
		};

		void setCost(unsigned int _cost) {
			cost = _cost;
		};

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

    private:

    	// the name of the event
    	string name;

		// the type of the event
		EventType type;

    	// the cost of the event (can be set later on)
		unsigned int cost;
};

#endif /* EVENT_H_ */
