#ifndef KNOWLEDGE_H_
#define KNOWLEDGE_H_

#include "productAutomaton.h"

/*!
 * \brief Knowledge class
 */
class Knowledge {
    public: /* public static methods */
        // hash function
        static unsigned int hash(Knowledge&);

        // deletes all knowledge bubbles
        static void deleteAll();

    private: /* private static variables */
        // set of stored knowledges
        static HashMap<Knowledge, Knowledge::hash> storedKnowledge_;

        // pointer to related product automaton
        static ProductAutomaton* productAutomaton_;

    private: /* private variables */
        // knowledge "bubble"
        std::vector<ProductAutomatonState*> bubble_;

        // predecessors
        std::map<unsigned int, std::vector<Knowledge*> > predecessors_;

    public: /* public methods */
        // constructor
        Knowledge(ProductAutomaton&);

        // check weak receivability
        bool checkWeakReceivability();

        // comparison operator
        bool operator==(Knowledge&);

    private: /* private methods */
        // copy constructor
        Knowledge(std::set<ProductAutomatonState*>&);

        // compute closure
        void closure();

        // compute successor bubble
        std::map<unsigned int, Knowledge*> genereateSuccessors();
};


#endif /* KNOWLEDGE_H_ */
