
#pragma once

#include <Core/Dimensions.h>

#include <Net/Net.h>

#include <Symmetry/LinkedList.h>

typedef struct
{
    index_t ix;
    node_t node;
} scc_vertex_t;

typedef struct
{
    index_t lowlink;
    index_t dfs;
    bool stack;
} scc_node_t;

typedef struct
{
    index_t from;
    index_t to;
} scc_range_t;

class SCC
{
private:
    // variables used for determining the SCC
    index_t dfs;           // current dfs number
    scc_node_t *nodes[2];  // node information
    scc_vertex_t *tarjan;  // array representing stack
    index_t nextTarjan;    // next free item on tarjan stack
    index_t nextElem;      // next free item in elems

    void strongconnect(LinkedList<scc_range_t>, index_t, node_t);
public:
    SCC();
    ~SCC();

    index_t sccs;        // number of SCCs
    scc_range_t *ranges; // ranges[i] denotes the range for the (i+1)-th SCC
    scc_vertex_t *elems; // elems[ranges[i].from], ..., elems[ranges[i].to] form the (i+1)-th SCC
    index_t *scc[2];     // node i of node-type t is element of the (scc[t][i]+1)-th SCC
    index_t *card[2];    // the (i+1)-th SCC has card[direction][i] arcs to other SCCs w.r.t. to the direction
    index_t *intra[2];   // the (i+1)-th SCC comprises intra[direction][i] arcs with the given direction
    index_t **arcs[2];   // arcs[direction][i][j] denotes i's (j+1)-th neighbour w.r.t. the direction
    mult_t **mult[2];    // mult[direction][i][j] denotes the multiplicity of arcs[direction][i][j]
};
