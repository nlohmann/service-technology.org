#include "FullMarking.h"


/***************
 * CONSTRUCTOR *
 ***************/

FullMarking::FullMarking(InnerMarking_ID _inner) :
    inner(_inner), interface() {}

FullMarking::FullMarking(InnerMarking_ID _inner, InterfaceMarking _interface) :
    inner(_inner), interface(_interface) {}


/*************
 * OPERATORS *
 *************/

bool FullMarking::operator< (const FullMarking &other) const {
    // first compare inner markings
    if (inner < other.inner) {
        return true;
    }
    if (inner > other.inner) {
        return false;
    }
    
    // in case the inner markings are equal, compare interface markings
    return (interface < other.interface);
}

bool FullMarking::operator!= (const FullMarking &other) const {
    // first compare inner markings
    if (inner != other.inner) {
        return true;
    }

    // in case the inner markings are equal, compare interface markings
    return (interface != other.interface);
}

std::ostream& operator<< (std::ostream &o, const FullMarking &m) {
    return o << "[m" << static_cast<unsigned int>(m.inner) << ", " << m.interface << "]";
}
