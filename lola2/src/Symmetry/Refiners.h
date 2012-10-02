
#pragma once

#include <Core/Dimensions.h>

#include <Net/Net.h>
#include <Net/Marking.h>
#include <Net/Transition.h>

#include <Parser/FairnessAssumptions.h>

#include <Symmetry/SCC.h>
#include <Symmetry/PartitionRefinement.h>

////////////////////////////////////////////////////////////////////////////////
// Traits
////////////////////////////////////////////////////////////////////////////////

class DirectionalRefiner {
 protected:
  direction_t direction;
 public:
  DirectionalRefiner(direction_t dir) : direction(dir) {};
  void setDirection(direction_t dir) { direction = dir; };
};

class NetRefiner {
 protected:
  inline node_t normalize(index_t* v) {
    node_t node = (*v < Net::Card[PL] ? PL : TR);
    if(node == TR) *v -= Net::Card[PL];
    return node;
  };
};

class SCCRefiner {
 protected:
  SCC *scc;
 public:
  SCCRefiner(SCC *s) : scc(s) {};
};

class RepresentativeRefiner {
 protected:
  Partition *repr;
 public:
  RepresentativeRefiner(Partition *r) : repr(r) {};
};


typedef struct {
  index_t length;
  index_t repr;
} split_cell_t;

class NeighbourhoodRefiner {
 protected:
  Partition *partition;
  Partition **neighbours;
  LinkedList<split_cell_t>* cells;
  
  LinkedList<split_cell_t> gather(index_t i, index_t *repr);
  bool publish(index_t);
  virtual bool refineNeighbours(index_t) { return false; };
  virtual bool refineMultiplicity(index_t) { return false; };
  virtual split_cell_t getInfo(index_t i, index_t cell, index_t *repr) { split_cell_t info = {0,0}; return info; };
  virtual index_t getCardinality(index_t) { return 0; };
 public:
    NeighbourhoodRefiner(Partition *p) :  partition(p),
        neighbours(new Partition*[p->order]),
        cells(new LinkedList<split_cell_t>[p->order]),
        available(new bool[p->order]),
        lengths(new index_t[p->order]),
        reprs(new index_t[p->order]) {};
  virtual ~NeighbourhoodRefiner();
  
  bool *available;
  index_t *lengths;
  index_t *reprs;
  
  void start();
  bool next();
  bool progress(bool *suspects);
};


////////////////////////////////////////////////////////////////////////////////
// Generic refiners
////////////////////////////////////////////////////////////////////////////////

class EqualsRefiner : public PredicateRefinement {
 protected:
  index_t element;
 public:
  EqualsRefiner(Partition *p, index_t x) : PredicateRefinement(p), element(x) {};
  bool evaluate(index_t x) { return (element == x); };
};

template <typename T>
class PointerArrayRefinement : public PredicateRefinement {
 protected:
  T** array;
 public:
  PointerArrayRefinement(Partition *p, T** arr) : PredicateRefinement(p), array(arr) {};
  bool evaluate(index_t v) { return (array[v] != NULL); };
};

class BoolArrayRefinement : public PredicateRefinement {
 protected:
  bool *array;
 public:
  BoolArrayRefinement(Partition *p, bool *arr) : PredicateRefinement(p), array(arr) {};
  bool evaluate(index_t v) { return array[v]; }
};

class BoundedArrayRefinement : public CountingRefinement {
 protected:
  index_t *array;
 public:
  BoundedArrayRefinement(Partition *p, index_t *arr) : CountingRefinement(p), array(arr) {};
  index_t value(index_t v) { return array[v]; }
};

template <typename T>
class UnboundedArrayRefinement : public ComparisonRefinement {
 protected:
  T *array;
 public:
  UnboundedArrayRefinement(Partition *p, T *arr) : ComparisonRefinement(p), array(arr) {};
  cmp_t compare(T v, T w) {
    if(array[v] < array[w]) return CMP_LT;
    if(array[v] > array[w]) return CMP_GT;
    return CMP_EQ;
  };
};

////////////////////////////////////////////////////////////////////////////////
// Simple refiners
////////////////////////////////////////////////////////////////////////////////

class DegreeRefinement : public CountingRefinement, public DirectionalRefiner, public NetRefiner {
 public:
  DegreeRefinement(Partition *p, direction_t dir) : CountingRefinement(p), DirectionalRefiner(dir) {};
  index_t value(index_t v) { return Net::CardArcs[normalize(&v)][direction][v]; }
};

class FairnessRefinement : public ComparisonRefinement, public NetRefiner {
 public:
  FairnessRefinement(Partition *p) : ComparisonRefinement(p) {};
  cmp_t compare(index_t v, index_t w) {
    assert(v >= Net::Card[PL] && w >= Net::Card[PL]);
    normalize(&v); normalize(&w);
    if(Transition::Fairness[v] < Transition::Fairness[w]) return CMP_LT;
    if(Transition::Fairness[v] > Transition::Fairness[w]) return CMP_GT;
    return CMP_EQ;
  }
};

class NeighbourRefinement : public CountingRefinement, public DirectionalRefiner, public RepresentativeRefiner, public NetRefiner {
 protected:
  index_t vertex;
  node_t node;
 public:
  NeighbourRefinement(Partition *p, Partition *r, index_t v, direction_t dir) : CountingRefinement(p), DirectionalRefiner(dir),  RepresentativeRefiner(r), vertex(v), node(normalize(&vertex)) {};
  index_t value(index_t v) { 
    index_t target = Net::Arc[node][direction][vertex][v];
    if(node == PL) target += Net::Card[PL];
    return repr->representative[target];
  };
};


class SCCNeighbourRefinement : public CountingRefinement, public DirectionalRefiner, public SCCRefiner, public RepresentativeRefiner {
 protected:
  index_t vertex;
 public:
  SCCNeighbourRefinement(Partition *p, SCC *s, Partition *r, index_t v, direction_t dir) : CountingRefinement(p), DirectionalRefiner(dir), SCCRefiner(s), RepresentativeRefiner(r), vertex(v) {};
  index_t value(index_t v) { return repr->representative[scc->arcs[direction][vertex][v]]; };
};

class MultRefinement : public ComparisonRefinement, public DirectionalRefiner, public NetRefiner {
 protected:
  index_t vertex;
  node_t node;
 public:
  MultRefinement(Partition *p, index_t v, direction_t dir) : ComparisonRefinement(p), DirectionalRefiner(dir), vertex(v), node(normalize(&vertex)) {};
  cmp_t compare(index_t v, index_t w) {
    if(Net::Mult[node][direction][vertex][v] < Net::Mult[node][direction][vertex][w]) return CMP_LT;
    if(Net::Mult[node][direction][vertex][v] > Net::Mult[node][direction][vertex][w]) return CMP_GT;
    return CMP_EQ;
  };
};


class SCCMultRefinement : public ComparisonRefinement, public DirectionalRefiner, public SCCRefiner {
 protected:
  index_t vertex;
 public:
  SCCMultRefinement(Partition *p, SCC *s, index_t v, direction_t dir) : ComparisonRefinement(p), DirectionalRefiner(dir), SCCRefiner(s), vertex(v) {};
  cmp_t compare(index_t v, index_t w) {
    if(scc->mult[direction][vertex][v] < scc->mult[direction][vertex][w]) return CMP_LT;
    if(scc->mult[direction][vertex][v] > scc->mult[direction][vertex][w]) return CMP_GT;
    return CMP_EQ;
  };
};

class SCCPartitionRefinement : public CountingRefinement, public RepresentativeRefiner, public NetRefiner, public SCCRefiner {
 public:
  SCCPartitionRefinement(Partition *p, SCC *s, Partition *sp) : CountingRefinement(p), RepresentativeRefiner(sp), SCCRefiner(s) {};
  index_t value(index_t v) { return repr->representative[scc->scc[normalize(&v)][v]]; };
};

class SCCSizeRefinement : public CountingRefinement, public SCCRefiner {
 public:
  SCCSizeRefinement(Partition *p, SCC *s) : CountingRefinement(p), SCCRefiner(s) {};
  index_t value(index_t v) { return 1 + scc->ranges[v].to - scc->ranges[v].from; };
};


////////////////////////////////////////////////////////////////////////////////
// Complex refiners
////////////////////////////////////////////////////////////////////////////////

class DagRefinement : public DirectionalRefiner, public SCCRefiner {
 protected:
  index_t *hits;
  index_t *queue;
  index_t head;
  index_t next;
 public:
  DagRefinement(SCC *s, direction_t dir) : DirectionalRefiner(dir), SCCRefiner(s), head(0), next(0) {
    hits = new index_t[scc->sccs];
    queue = new index_t[scc->sccs];
    
    // find SCCs without incoming (resp. outgoing) arcs
    direction_t complement = (direction == POST ? PRE : POST);
    for(index_t i = 0; i < scc->sccs; i++) if(scc->card[complement][i] == 0) queue[next++] = i;
    next %= scc->sccs;
  };
  ~DagRefinement() { delete[] hits; delete[] queue; };
  index_t* layer();
};


class SCCNeighbourhoodRefinement : public NetRefiner, public DirectionalRefiner, public SCCRefiner, public NeighbourhoodRefiner {
 protected:
  index_t getCardinality(index_t);
  split_cell_t getInfo(index_t, index_t, index_t *repr);
  bool refineNeighbours(index_t);
  bool refineMultiplicity(index_t);
 public:
  SCCNeighbourhoodRefinement(Partition *sp, SCC *s, direction_t dir) : DirectionalRefiner(dir), SCCRefiner(s), NeighbourhoodRefiner(sp) {};
};

class NeighbourhoodRefinement : public DirectionalRefiner, public NeighbourhoodRefiner, public NetRefiner {
 protected:
  index_t getCardinality(index_t);
  split_cell_t getInfo(index_t, index_t, index_t *repr);
  bool refineNeighbours(index_t);
  bool refineMultiplicity(index_t);
 public:
  NeighbourhoodRefinement(Partition *p, direction_t dir) : DirectionalRefiner(dir), NeighbourhoodRefiner(p) {};
};

template <typename T>
bool isomorphic(T* a1, T* a2, unsigned int length) {
    T b1[length]; memcpy(b1, a1, sizeof(T) * length); std::sort(b1, &b1[length]);
    T b2[length]; memcpy(b2, a2, sizeof(T) * length); std::sort(b2, &b2[length]);
    return (memcmp(b1, b2, sizeof(T) * length) == 0);
}

void refineFix(Partition *partition);
bool refineFix2(Partition*, Partition*);
