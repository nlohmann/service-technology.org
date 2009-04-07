#ifndef _KNOWLEDGE_H
#define _KNOWLEDGE_H

#include <map>
#include <set>
#include <vector>
#include <queue>
#include <ostream>

#include "FullMarking.h"
#include "InnerMarking.h"
#include "InterfaceMarking.h"
#include "Label.h"


/*!
 \brief knowledge (data structure for building knowledges)
 */
class Knowledge {
    public: /* static attributes */
        static int memory_count;
        static int memory_max;
    
    public: /* member functions */

        /// construct knowledge from (initial) inner marking
        Knowledge(InnerMarking_ID);

        /// construct knowledge from a given knowledge and a label
        Knowledge(Knowledge*, Label_ID);
        
        /// destructor
        ~Knowledge();
        
        /// stream output operator
        friend std::ostream& operator<< (std::ostream&, const Knowledge&);

    public: /* attributes */

        /// whether this knowledge is sane
        unsigned is_sane : 1;
        
        /// the number of markings stored in the bubble
        unsigned int size;

        /// primary data structure
        std::map<InnerMarking_ID, std::vector<InterfaceMarking*> > bubble;

    private: /* member functions */

        /// calculate the closure of this knowledge
        void closure();

    private: /* attributes */

        /// a queue of recently added markings that might have successors
        std::queue<FullMarking> todo;
};

#endif
