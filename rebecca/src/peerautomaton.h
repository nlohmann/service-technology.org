#ifndef PEERAUTOMATON_H
#define PEERAUTOMATON_H

#include "peer.h"
#include "types.h"
#include <map>
#include <ostream>
#include <set>
#include <string>
#include <vector>


class PeerAutomaton {
        friend std::ostream& operator <<(std::ostream&, const PeerAutomaton&);

    private:
        // set of states
        std::set<std::set<int> > states_;

        // initial state
        std::set<int> initialState_;

        // set of final states
        std::set<std::set<int> > finalStates_;

        // transition relation
        std::set<PEdge*> edges_;

        // interface
        std::set<std::string> input_;
        std::set<std::string> output_;
        std::set<std::string> synchronous_;

    public:
        PeerAutomaton();
        ~PeerAutomaton();

        PEdge* createEdge(std::set<int>, std::string, std::set<int>, EventType);

        void deleteState(int);
        void deleteEdge(Edge*);

        /// setter
        void pushState(std::set<int>);
        void setInitialState(std::set<int>);
        void pushFinalState(std::set<int>);

        /// getter
        const std::set<std::set<int> > & states() const;
        const std::set<PEdge*> edgesFrom(std::set<int>) const;

        const std::set<int> findState(int) const;

        bool isState(int) const;

        bool isFinal(std::set<int>) const;

        bool haveInput() const;
        bool haveOutput() const;
        bool haveSynchronous() const;

        void pushInput(const std::string&);
        void pushOutput(const std::string&);
        void pushSynchronous(const std::string&);
        const std::set<std::string> & input() const;
        const std::set<std::string> & output() const;
        const std::set<std::string> & synchronous() const;

};

#endif /* PEERAUTOMATON_H */
