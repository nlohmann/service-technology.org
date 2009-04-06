#ifndef _FULLMARKING_H
#define _FULLMARKING_H

#include "InnerMarking.h"
#include "InterfaceMarking.h"

/*!
 \brief full marking (a pair of an inner and an interface marking)
 */
class FullMarking {
    public: /* member functions */
    
        /// constructor
        FullMarking(InnerMarking_ID);

        /// constructor
        FullMarking(InnerMarking_ID, InterfaceMarking);
        
        /// comparison operator
        bool operator< (const FullMarking &other) const;

        /// comparison operator
        bool operator!= (const FullMarking &other) const;

        /// stream output operator
        friend std::ostream& operator<< (std::ostream&, const FullMarking&);

    public: /* member attributes */
    
        /// an inner marking
        InnerMarking_ID inner;
        
        /// an interface marking
        InterfaceMarking interface;        
};

#endif
