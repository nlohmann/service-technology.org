
#include "Label.h"

class PossibleSendEvents {

private:

    /// a byte array to store the possible sending events
    uint8_t *storage;

public:

	PossibleSendEvents operator&& (const PossibleSendEvents &);

	/// returns array of all sending events (possible or not)
	char * decode();

	/// constructor; initially, all sending events are not reachable
	PossibleSendEvents();

	void labelPossible(Label_ID l);

};
