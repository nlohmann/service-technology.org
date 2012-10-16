
#include <iostream>
#include <string.h>

#include <Core/Dimensions.h>

#include <Symmetry/SCC.h>
#include <Symmetry/LinkedList.h>

SCC::SCC() {
  // initialize variables
  dfs = 0;
  elems = new scc_vertex_t[Net::Card[PL] + Net::Card[TR]]; nextElem = 0;
  tarjan = new scc_vertex_t[Net::Card[PL] + Net::Card[TR]]; nextTarjan = 0;
  
  //#pragma omp parallel for
  for(int n = PL; n <= TR; n++) {
    scc[n] = new index_t[Net::Card[n]];
    nodes[n] = new scc_node_t[Net::Card[n]];
    //#pragma omp parallel for
    for(index_t i = 0; i < Net::Card[n]; i++) {
      nodes[n][i].lowlink = -1;
      nodes[n][i].dfs = -1;
      nodes[n][i].stack = false;
    }
  }
  
  // perform searching for strongly connected components
  LinkedList<scc_range_t> list = LinkedList<scc_range_t>();
  for(int n = PL; n <= TR; n++) {
    for(index_t i = 0; i < Net::Card[n]; i++) {
      if(nodes[n][i].dfs == (index_t)-1) {
	strongconnect(list, i, (node_t)n);
      }
    }
  }
  
  // postprocessing
  sccs = list.size();
  ranges = new scc_range_t[sccs];
  list.reset();
  for(index_t i = 0; list.isValid(); list.next(), i++) {
    ranges[i] = list.getCons()->getContent();
    
    //#pragma omp parallel for
    for(index_t j = ranges[i].from; j <= ranges[i].to; j++) {
      scc[elems[j].node][elems[j].ix] = i;
    }
  }
  
  // delete variables we no longer need
  delete[] tarjan;
  delete[] nodes[PL];
  delete[] nodes[TR];
  list.deallocate();
  
  // determine graph structure
  arcs[PRE] = new index_t*[sccs]; arcs[POST] = new index_t*[sccs];
  mult[PRE] = new mult_t*[sccs]; mult[POST] = new mult_t*[sccs];
  card[PRE] = new index_t[sccs](); card[POST] = new index_t[sccs]();
  intra[PRE] = new index_t[sccs](); intra[POST] = new index_t[sccs]();
  
  //#pragma omp parallel for
  for(index_t i = 0; i < sccs; i++) {
    //#pragma omp parallel for
    for(int dir = PRE; dir <= POST; dir++) {
      index_t *tempArcs = new index_t[Net::Card[PL] + Net::Card[TR]];
      mult_t *tempMult = new mult_t[Net::Card[PL] + Net::Card[TR]];

      for(index_t j = ranges[i].from; j <= ranges[i].to; j++) {
	for(index_t k = 0; k < Net::CardArcs[elems[j].node][dir][elems[j].ix]; k++) {
	  node_t complement = (elems[j].node == PL ? TR : PL);
	  index_t component = scc[complement][Net::Arc[elems[j].node][dir][elems[j].ix][k]];
	  if(scc[elems[j].node][elems[j].ix] != component) {
	    tempArcs[card[dir][i]] = component;
	    tempMult[card[dir][i]] = Net::Mult[elems[j].node][dir][elems[j].ix][k];
	    card[dir][i]++;
	  } else {
	    intra[dir][i]++;
	  }
	}
      }
      
      arcs[dir][i] = new index_t[card[dir][i]];
      memcpy(arcs[dir][i], tempArcs, sizeof(index_t) * card[dir][i]);
      
      mult[dir][i] = new mult_t[card[dir][i]];
      memcpy(mult[dir][i], tempMult, sizeof(mult_t) * card[dir][i]);
      
      delete[] tempArcs; delete[] tempMult;
    }
  }
}

SCC::~SCC() {
  delete[] ranges;
  delete[] elems;
  delete[] scc[PRE]; delete[] scc[POST];
  delete[] card[PRE]; delete[] card[POST];
  delete[] intra[PRE]; delete[] intra[POST];
  for(index_t i = 0; i < sccs; i++) {
    delete[] arcs[PRE][i]; delete[] arcs[POST][i];
    delete[] mult[PRE][i]; delete[] mult[POST][i];
  }
}

void SCC::strongconnect(LinkedList<scc_range_t> list, index_t ix, node_t node) {
  nodes[node][ix].dfs = dfs;
  nodes[node][ix].lowlink = dfs;
  nodes[node][ix].stack = true;
  dfs++;
  scc_vertex_t vertex = { ix, node };
  tarjan[nextTarjan++] = vertex;
  
  // consider successors
  for(index_t i = 0; i < Net::CardArcs[node][POST][ix]; i++) {
    node_t complement = (node == PL ? TR : PL);
    index_t succ = Net::Arc[node][POST][ix][i];
    if(nodes[complement][succ].dfs == (index_t)-1) {
      strongconnect(list, succ, complement);
      if(nodes[node][ix].lowlink > nodes[complement][succ].lowlink) {
	nodes[node][ix].lowlink = nodes[complement][succ].lowlink;
      }
    } else if(nodes[complement][succ].stack) {
      if(nodes[node][ix].lowlink > nodes[complement][succ].dfs) {
	nodes[node][ix].lowlink = nodes[complement][succ].dfs;
      }
    }
  }
  
  // 
  if(nodes[node][ix].lowlink == nodes[node][ix].dfs) {
    scc_range_t range = { nextElem, nextElem-1 };
    do {
      elems[nextElem++] = tarjan[--nextTarjan];
      range.to++;
      nodes[elems[range.to].node][elems[range.to].ix].stack = false;
    } while(elems[range.to].node != node || elems[range.to].ix != ix);
    list.enqueue(range);
  }
}
