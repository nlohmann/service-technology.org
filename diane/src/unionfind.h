/*
 * unionfind.h
 *
 *  Created on: Jun 9, 2009
 *      Author: stephan
 */

#ifndef UNIONFIND_H
#define UNIONFIND_H

#include <map>
#include <vector>
#include "pnapi/petrinet.h"

using pnapi::Node;
using pnapi::PetriNet;
using std::map;
using std::vector;


namespace unionfind
{

  /// makes a singleton set of the given element
  void MakeSet(int, int *);

  /// makes a union of both given parameters
  void Union(int, int, int *);

  /// finds the parent of the given parameter
  int Find(int, int *);


  int computeComponentsByUnionFind(PetriNet &, int *, int, int, map<int, Node *> &);

  void createOpenNetComponentsByUnionFind(vector<PetriNet *> &, int *, int, int, map<int, Node *> &);

} /* namespace unionfind */

#endif /* UNIONFIND_H */
