/*!
  \author Andre
  \file PartitionRefinement.h
  \status new

  \brief 
*/

#pragma once

#include <Core/Dimensions.h>
#include <Net/Net.h>

/*!
  \brief A cell describes which elements it contains by giving a range [from,to].
  Although a cell does not explicitly state in which array these elements reside,
  this information is usually clear from the context due to passing a partition and
  a node type.
*/
typedef struct Cell {
    index_t from;
    index_t to;
} cell_t;


/*!
  \brief 

*/
class Partition {
  public:
    Partition(index_t order);
    ~Partition();
    Partition *copy();
    void show();
    void fix(index_t);
    index_t find(index_t);
    void split(index_t x);
    void reset();
    bool trivial();
    bool complete();
    
    void exch(index_t, index_t);
    
    index_t order;
    index_t *elements;
    index_t *representative;
    index_t *ranges;
    index_t *rounds;
    index_t round;
    bool change;
};

/*!
  \brief Base class for refinements.
*/
class Refinement {
  protected:
    Partition *partition;
  
    void confine(index_t left, index_t right);
    virtual void dispatch(index_t left, index_t right) {};
  public:
    Refinement(Partition *p) : partition(p) {};
    virtual ~Refinement() {};
    Partition *getPartition() { return partition; };
    bool perform(index_t left, index_t right);
};

/*!
  \brief Refinement using predicate-based partitioning.
*/
class PredicateRefinement : public Refinement {
  protected:
    void dispatch(index_t left, index_t right);
  public:
    PredicateRefinement(Partition *p) : Refinement(p) {};
    virtual bool evaluate(index_t) { return false; };
};

/*!
  \brief Refinement using comparison-based partitioning.
*/
class ComparisonRefinement : public Refinement {
  protected:
    void dispatch(index_t left, index_t right);
  public:
    ComparisonRefinement(Partition *p) : Refinement(p) {};
    virtual cmp_t compare(index_t, index_t) { return CMP_EQ; };
};

/*!
  \brief Refinement using counting-based partitioning.
*/
class CountingRefinement : public Refinement {
  protected:
    void dispatch(index_t left, index_t right);
    void counting(index_t left, index_t right, index_t low, index_t high);
  public:
    CountingRefinement(Partition *p) : Refinement(p) {};
    virtual index_t value(index_t) { return 0; };
};


template <class T>
bool refineWhen(T prototype, bool *cond) {
    // each cell which is completely contained in the range
    bool emergence = false;
    Partition *partition = prototype.getPartition();
    partition->round++;
//    #pragma omp parallel for schedule(dynamic)
    #pragma omp parallel for schedule(dynamic, 32)
    for(index_t i = 0; i < partition->order; i++) {
        if(partition->ranges[i] == 0 || partition->rounds[i] == partition->round) continue;
	if(!cond[i] || !prototype.perform(i, i + partition->ranges[i] - 1)) continue;
	assert(partition->change);
	emergence = true;
	// new cells emerged; their condition must be satisfied in the next round
	for(index_t j = i; j < i + partition->ranges[i]; j += partition->ranges[j]) cond[j] = true;
    }
    
    return emergence;
}


template <class T>
bool refineRange(T prototype, index_t from, index_t to) {
    if(to < from || to == (index_t)-1) return false;
    
    // each cell which is completely contained in the range
    bool emergence = false;
    
    Partition *partition = prototype.getPartition();
    partition->round++;
//    #pragma omp parallel for schedule(dynamic)
    #pragma omp parallel for schedule(dynamic, 32)
    for(index_t i = from; i < to; i++) {
        index_t last = i + partition->ranges[i] - 1;
	if(partition->ranges[i] == 0 || partition->rounds[i] == partition->round || last > to) continue;
	if(prototype.perform(i, last)) emergence = true;
    }
    
    return emergence;
}

template <class T>
bool refineAll(T prototype) { return refineRange<T>(prototype, 0, prototype.getPartition()->order-1); }

template <class T>
bool refinePlaces(T prototype) { return refineRange<T>(prototype, 0, Net::Card[PL]-1); }

template <class T>
bool refineTransitions(T prototype) { return refineRange<T>(prototype, Net::Card[PL], Net::Card[PL] + Net::Card[TR] - 1); }

