/*
 * unionfind.cc
 *
 *  Created on: Jun 9, 2009
 *      Author: stephan
 */

#include <map>
#include <set>
#include "cmdline.h"
#include "unionfind.h"

using pnapi::Arc;
using pnapi::Place;
using pnapi::Transition;
using std::map;
using std::set;

extern gengetopt_args_info args_info;


void unionfind::MakeSet(int x, int *tree)
{
  // makes singleton sets of all x given
  tree[x] = -1;
}


void unionfind::Union(int x, int y, int *tree)
{
  int xr = Find(x, tree); // finding x's root
  int yr = Find(y, tree); // finding y's root
  if (tree[xr] < tree[yr]) // then the number of children of x is greater
  {
    tree[xr] += tree[yr];
    tree[yr] = xr;
  }
  else
    if (xr != yr)
    {
      tree[yr] += tree[xr];
      tree[xr] = yr;
    }
}


int unionfind::Find(int x, int *tree)
{
  if (tree[x] >= 0)
    tree[x] = Find(tree[x], tree);
  else
    return x;
  return tree[x];
}


int unionfind::computeComponentsByUnionFind(PetriNet &net, int *tree, int size, int psize, map<int, Node *> &reremap)
{
  // map to remap the integers to the Petri net's nodes
  map<Node *, int> remap;

  // init MakeSet calls
  int q = 0;
  for (set<Place *>::iterator it = net.getPlaces().begin(); it != net.getPlaces().end(); it++)
  {
    remap[*it] = q;
    reremap[q] = *it;
    MakeSet(q, tree);
    q++;
  }
  for (set<Transition *>::iterator it = net.getTransitions().begin(); it != net.getTransitions().end(); it++)
  {
    remap[*it] = q;
    reremap[q] = *it;
    MakeSet(q, tree);
    q++;
  }

  if (args_info.service_flag)
  {
    for (set<Place *>::iterator it = net.getInternalPlaces().begin(); it != net.getInternalPlaces().end(); it++)
      if ((*it)->getTokenCount())
      {
        for (set<Node *>::iterator t = (*it)->getPreset().begin(); t != (*it)->getPreset().end(); t++)
          Union(remap[*it], remap[*t], tree);
        for (set<Node *>::iterator t = (*it)->getPostset().begin(); t != (*it)->getPostset().end(); t++)
          Union(remap[*it], remap[*t], tree);
      }
  }

  // usage of rules 1 and 2
  for (set<Place *>::iterator p = net.getPlaces().begin(); p != net.getPlaces().end(); p++)
  {
    set<Node *> preset = (*p)->getPreset();
    set<Node *>::iterator first = preset.begin();
    int f = remap[*first];
    for (set<Node *>::iterator t = preset.begin(); t != preset.end(); t++)
    {
      Union(f, remap[*t], tree);
    }
    set<Node *> postset = (*p)->getPostset();
    first = postset.begin();
    f = remap[*first];
    for (set<Node *>::iterator t = postset.begin(); t != postset.end(); t++)
    {
      Union(f, remap[*t], tree);
    }
  }

  // usage of rule 3 until nothing changes
  bool hasChanged;
  do
  {
    hasChanged = false;
    for (set<Place *>::iterator p = net.getPlaces().begin(); p != net.getPlaces().end(); p++)
    {
      set<Node *> preset = (*p)->getPreset();
      set<Node *> postset = (*p)->getPostset();
      int proot = Find(remap[*p], tree);
      for (set<Node *>::iterator pt = preset.begin(); pt != preset.end(); pt++)
      {
        int ptroot = Find(remap[*pt], tree);
        if (proot == ptroot)
          continue;
        bool localChange = false;
        for (set<Node *>::iterator tp = postset.begin(); tp != postset.end(); tp++)
        {
          int tproot = Find(remap[*tp], tree);
          if (tproot == ptroot)
          {
            hasChanged = localChange = true;
            Union(tproot, proot, tree);
            break;
          }
        }
        if (localChange)
          break;
      }
    }
  } while (hasChanged);

  // output: number of components
  int n = 0;
  for (int i = 0; i < psize; i++)
    if (tree[i] < -1)
      n++;
  for (int i = psize; i < size; i++)
    if (tree[i] < 0)
      n++;

  return n;
}


void unionfind::createOpenNetComponentsByUnionFind(vector<PetriNet *> &nets, int *tree, int size, int psize, map<int, Node *> &reremap)
{
  for (int i = 0; i < psize; i++)
  {
    if (tree[i] == -1)
      continue; // interface place found

    int x = Find(i, tree);
    if (nets[x] == NULL)
      nets[x] = new PetriNet();
    Place *p = static_cast<Place *>(reremap[i]);
    nets[x]->createPlace(p->getName(), Node::INTERNAL, p->getTokenCount());
  }

  for (int i = psize; i < size; i++)
  {
    int x = Find(i, tree);
    if (nets[x] == NULL)
      nets[x] = new PetriNet();
    Transition *t = &nets[x]->createTransition(reremap[i]->getName());
    /// creating arcs and interface
    set<Arc *> preset = reremap[i]->getPresetArcs();
    for (set<Arc *>::iterator f = preset.begin(); f != preset.end(); f++)
    {
      Place *place = &(*f)->getPlace();
      Place *netPlace;
      netPlace = nets[x]->findPlace(place->getName());
      if (netPlace == NULL)
        netPlace = &nets[x]->createPlace(place->getName(), Node::INPUT);
      nets[x]->createArc(*netPlace, *t, (*f)->getWeight());
    }
    set<Arc *> postset = reremap[i]->getPostsetArcs();
    for (set<Arc *>::iterator f = postset.begin(); f != postset.end(); f++)
    {
      Place *place = &(*f)->getPlace();
      Place *netPlace = nets[x]->findPlace(place->getName());
      if (netPlace == NULL)
        netPlace = &nets[x]->createPlace(place->getName(), Node::OUTPUT);
      nets[x]->createArc(*t, *netPlace, (*f)->getWeight());
    }
  }
}
