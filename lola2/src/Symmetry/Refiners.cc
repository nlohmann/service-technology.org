

#include <algorithm>
#include <iostream>
#include <string.h>

#include <Core/Dimensions.h>

#include <Symmetry/LinkedList.h>
#include <Symmetry/Refiners.h>


index_t* DagRefinement::layer() {
    if(head == next) return NULL;
  
    index_t until = next;
    memset(hits, 0, sizeof(index_t)*scc->sccs);
  
    while(head != until) {
	for(index_t i = 0; i < scc->card[direction][queue[head]]; i++) {
	    index_t target = scc->arcs[direction][queue[head]][i];
	    if(hits[target] == 0) { queue[next] = target; next = (next+1) % scc->sccs; }
	    hits[target]++;
	}
    
	head = (head+1) % scc->sccs;
    }
  
    return hits;
}

NeighbourhoodRefiner::~NeighbourhoodRefiner() {
    for(index_t i = 0; i < partition->order; i++) cells[i].deallocate();
    delete[] available;
    delete[] lengths;
    delete[] reprs;
    delete[] neighbours;
}

void NeighbourhoodRefiner::start() {
    #pragma omp parallel for
    for(index_t i = 0; i < partition->order; i++) {
	neighbours[i] = new Partition(getCardinality(i));
	cells[i] = LinkedList<split_cell_t>();
	refineMultiplicity(i);
    }
}

LinkedList<split_cell_t> NeighbourhoodRefiner::gather(index_t i, index_t *repr) {
    LinkedList<split_cell_t> list = LinkedList<split_cell_t>();
    for(index_t j = 0; j < neighbours[i]->order; j += neighbours[i]->ranges[j]) list.enqueue(getInfo(i, j, repr));
    return list;
}

bool NeighbourhoodRefiner::publish(index_t i) {
    if(!cells[i].isValid()) {
	available[i] = false;
	lengths[i] = reprs[i] = 0;
	return false;
    }
  
    available[i] = true;
    lengths[i] = cells[i].getCons()->getContent().length;
    reprs[i] = cells[i].getCons()->getContent().repr;
    return true;
}

bool NeighbourhoodRefiner::next() {
    bool nonEmpty = false;
    
    #pragma omp parallel for
    for(index_t i = 0; i < partition->order; i++) {
	if(cells[i].isValid()) cells[i].next();
	if(publish(i)) nonEmpty = true;
    }
  
    return nonEmpty;
}

bool NeighbourhoodRefiner::progress(bool *suspects) {
    bool emergence = false;
    
    #pragma omp parallel for
    for(index_t i = 0; i < partition->order; i++) {
	// refine neighbours
	neighbours[i]->reset();
	bool change = refineNeighbours(i);
	
	// check whether one of the neighbours changed
	LinkedList<split_cell_t> list = cells[i];
	cells[i] = gather(i, partition->representative);
	for(list.reset(), cells[i].reset(); !change && list.isValid() && cells[i].isValid(); list.next(), cells[i].next())
	    change |= (list.getCons()->getContent().repr != cells[i].getCons()->getContent().repr);
	list.deallocate();
	
	// add suspect
	if(change) { emergence = true; suspects[partition->representative[i]] = true; }
	
	// publish information
	cells[i].reset(); publish(i);
    }
    
    return emergence;
}



bool SCCNeighbourhoodRefinement::refineMultiplicity(index_t i) {
    return refineAll<SCCMultRefinement>(SCCMultRefinement(neighbours[i], scc, i, direction));
}

index_t SCCNeighbourhoodRefinement::getCardinality(index_t i) { 
    return scc->card[direction][i];
}

/*
  SCCNeighbourhoodRefinement::~SCCNeighbourhoodRefinement() {
  delete[] neighbours;
  delete[] cells;
  delete[] available;
  delete[] lengths;
  delete[] reprs;
  }*/


split_cell_t SCCNeighbourhoodRefinement::getInfo(index_t i, index_t cell, index_t *repr) {
    split_cell_t info = { neighbours[i]->ranges[cell], repr[scc->arcs[direction][i][cell]] };
    return info;  
}

bool SCCNeighbourhoodRefinement::refineNeighbours(index_t i) {
    return refineAll<SCCNeighbourRefinement>(SCCNeighbourRefinement(neighbours[i], scc, partition, i, direction));
}


index_t NeighbourhoodRefinement::getCardinality(index_t v) {
    return Net::CardArcs[normalize(&v)][direction][v];
}

split_cell_t NeighbourhoodRefinement::getInfo(index_t i, index_t cell, index_t *repr) {
    index_t v = i; node_t node = normalize(&v);
    index_t t = Net::Arc[node][direction][v][neighbours[i]->elements[cell]];
    if(node == PL) t += Net::Card[PL];
    split_cell_t info = { neighbours[i]->ranges[cell], repr[t] };
    return info;
}

bool NeighbourhoodRefinement::refineNeighbours(index_t i) {
    return refineAll<NeighbourRefinement>(NeighbourRefinement(neighbours[i], partition, i, direction));
}

bool NeighbourhoodRefinement::refineMultiplicity(index_t i) {
    return refineAll<MultRefinement>(MultRefinement(neighbours[i], i, direction));
}


void refineFix(Partition *partition) {
    NeighbourhoodRefinement *neighbourhood[2];
    bool **suspects = new bool*[2];
    for(int dir = PRE; dir <= POST; dir++) {
        suspects[dir] = new bool[partition->order];
	neighbourhood[dir] = new NeighbourhoodRefinement(partition, (direction_t)dir);
	neighbourhood[dir]->start();

	memset(suspects[dir], false, sizeof(bool) * partition->order);
	neighbourhood[dir]->progress(suspects[dir]);
	for(index_t i = 0; i < partition->order; i += partition->ranges[i]) suspects[dir][i] = true;
    }
    
    do {
	partition->reset();
	
	for(int dir = PRE; dir <= POST; dir++) {
	    do {
		refineWhen<BoolArrayRefinement>(BoolArrayRefinement(partition, neighbourhood[dir]->available), suspects[dir]);
		
		// cells with unavailable nodes are indistinguishable
		for(index_t i = 0; i < partition->order; i += partition->ranges[i])
		    suspects[dir][i] = neighbourhood[dir]->available[partition->elements[i]];
		
		refineWhen<BoundedArrayRefinement>(BoundedArrayRefinement(partition, neighbourhood[dir]->lengths), suspects[dir]);
		refineWhen<BoundedArrayRefinement>(BoundedArrayRefinement(partition, neighbourhood[dir]->reprs), suspects[dir]);
	    } while(neighbourhood[dir]->next());
	    
	    for(index_t i = 0; i < partition->order; i += partition->ranges[i]) suspects[dir][i] = false;
	    neighbourhood[dir]->progress(suspects[dir]);
	}
	
	if(!partition->change) for(index_t i = 0; i < partition->order; i += partition->ranges[i]) assert(partition->rounds[i] < partition->round);
    } while(partition->change);
    
    delete neighbourhood[PRE]; delete neighbourhood[POST];
    delete[] suspects[PRE]; delete[] suspects[POST];
}

bool refineFix2(Partition *p1, Partition *p2) {
    assert(p1->order == p2->order);
    index_t order = p1->order;
    Partition *p[2] = {p1, p2};
    NeighbourhoodRefinement *neighbourhood[2][2];
    bool ***suspects = new bool**[2];
    for(int i = 0; i < 2; i++) {
        suspects[i] = new bool*[2];
	for(int dir = PRE; dir <= POST; dir++) {
            suspects[i][dir] = new bool[order];
	    neighbourhood[i][dir] = new NeighbourhoodRefinement(p[i], (direction_t)dir);
	    neighbourhood[i][dir]->start();
	    
	    memset(suspects[i][dir], false, sizeof(bool) * order);
	    neighbourhood[i][dir]->progress(suspects[i][dir]);
	    for(index_t j = 0; j < order; j += p[i]->ranges[j]) suspects[i][dir][j] = true;
	}
    }
    
    do {
	p[0]->reset(); p[1]->reset();
	
	for(int dir = PRE; dir <= POST; dir++) {
	    bool advance;
	    do {
		if(!isomorphic(neighbourhood[0][dir]->available, neighbourhood[1][dir]->available, order)) return false;
		if(!isomorphic(neighbourhood[0][dir]->lengths, neighbourhood[1][dir]->lengths, order)) { std::cout << "lengths\n"; return false; }
		if(!isomorphic(neighbourhood[0][dir]->reprs, neighbourhood[1][dir]->reprs, order)) {
		    for(int i = 0; i < 2; i++) {
			for(index_t j = 0; j < order; j++) {
			    std::cout << neighbourhood[i][dir]->reprs[j];
			    if(j+1 < order) std::cout << ", ";
			}
			std::cout << std::endl;
		    }
		    return false;
		}
		
		for(int i = 0; i < 2; i++) {
		    refineWhen<BoolArrayRefinement>(BoolArrayRefinement(p[i], neighbourhood[i][dir]->available), suspects[i][dir]);
		    
		    // cells with unavailable nodes are indistinguishable
		    for(index_t j = 0; j < order; j += p[i]->ranges[j])
			suspects[i][dir][j] = neighbourhood[i][dir]->available[p[i]->elements[j]];
		    
		    refineWhen<BoundedArrayRefinement>(BoundedArrayRefinement(p[i], neighbourhood[i][dir]->lengths), suspects[i][dir]);
		    refineWhen<BoundedArrayRefinement>(BoundedArrayRefinement(p[i], neighbourhood[i][dir]->reprs), suspects[i][dir]);
		}
		
		advance = neighbourhood[0][dir]->next();
		if(neighbourhood[1][dir]->next() != advance) return false;
	    } while(advance);
	    
	    for(int i = 0; i < 2; i++) {
		for(index_t j = 0; j < order; j += p[i]->ranges[j]) suspects[i][dir][j] = false;
		neighbourhood[i][dir]->progress(suspects[i][dir]);
	    }
	}
	
	assert(p[0]->change == p[1]->change);
    } while(p[0]->change);
    
    for(int i = 0; i < 2; i++) { delete neighbourhood[i][PRE]; delete neighbourhood[i][POST]; }
    return true;
}
