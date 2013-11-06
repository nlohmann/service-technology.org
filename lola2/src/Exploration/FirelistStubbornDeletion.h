/*!
\file FirelistStubbornDeletion.h
\author Harro
\status new

\brief Class for firelist generation by the deletion algorithm for stubborn sets.
*/

#pragma once
#include <Core/Dimensions.h>
#include <Exploration/Firelist.h>
#include <Formula/StatePredicate.h>
#include <vector>
#include <set>
#include <map>

using std::vector;
using std::set;
using std::map;

/// a stubborn firelist for the search for deadlocks and state predicates
class FirelistStubbornDeletion : public Firelist
{
public:
    /// constructor for deadlock search
    FirelistStubbornDeletion();

    /// constructor for state predicates
    FirelistStubbornDeletion(StatePredicate *sp);

    /// destructor
    ~FirelistStubbornDeletion();

    /// return value contains number of elements in fire list, argument is reference
    /// parameter for actual list
    virtual index_t getFirelist(NetState &ns, index_t **);

    /// to make a copy of this object
    virtual Firelist *createNewFireList(SimpleProperty *property);

private:
    /// build the unchanging part of the deletion graph
    void buildStaticGraph();

    /// execute the deletion algorithm
    index_t deletion(NetState &ns, index_t **);

    /// initialise the deletion graph
    void init();

    /// make a single run of the deletion algorithm
    void cnstr(NetState &ns, index_t *prt, index_t prtnr);

    /// temporarily remove a node from the graph and calculate the consequences
    void speculate(index_t node);

    /// put a node back into the graph and check the consequences
    void rehabilitate(index_t node);

    /// colors of nodes
    enum { WHITE = 0, GRAY, BLACK };

    /// protection
    enum { UNPROTECTED = 0, PROTECTED };

    /// root flag
    enum { PENDING = 0, DONE };

    /// and-or-nodes
    enum { ANDNODE = 0, ORNODE };

    /// max number of <t,s>-pairs
    index_t fieldsize;

    /// max number of nodes in the and-or-graph
    index_t size;

    /// lists of predecessors for all nodes
    index_t **predecessor;

    /// number of predecessors for each node
    index_t *predcnt;

    /// counter for white successors of or-nodes
    index_t *counter;

    /// maximum value for the counter for white successors of or-nodes for a given state
    index_t *maxcounter;

    /// offset of the first <t,s>-pair for a given t
    index_t *offset;

    /// the transition of a <t,s>-pair (given by offset)
    index_t *transition;

    /// for each place a list of transitions decreasing its token count
    vector<index_t> *consumer;

    /// for each place the token decrease by firing the corresponding transition in the consumer list
    vector<index_t> *consumption;

    /// for each place a set of transitions decreasing its token count
    set<index_t> *producer;

    /// color of a node (0=white, 1=gray, 2=black)
    unsigned char *color;

    /// if a node is protected from deletion
    unsigned char *protect;

    /// if a node has already been tried for deletion
    unsigned char *root;

    /// if a node is an or-node (true) or and-node (false)
    unsigned char *andor;

    /// pointer to the state predicate if it exists
    StatePredicate *sp;

    /// offset to where the formula nodes start
    index_t formula_offset;

    /// list of formula nodes in the same order as in the graph
    vector<const StatePredicate *> f_nodes;

    /// indices of formula and-nodes (= graph or-nodes) with successors
    map<index_t, vector<index_t> > f_and_nodes;

    /// indices of formula leafs (atomic, TRUE, FALSE)
    vector<index_t> f_leaf_nodes;

    /// vector of all enabled transitions
    index_t *enabled;
};

