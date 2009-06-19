/*
 * unionfind.cc
 *
 *  Created on: Jun 9, 2009
 *      Author: stephan
 */

#include "unionfind.h"


void MakeSet(int x, int *tree)
{
  // makes singleton sets of all x given
  tree[x] = -1;
}


void Union(int x, int y, int *tree)
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


int Find(int x, int *tree)
{
  if (tree[x] >= 0)
    tree[x] = Find(tree[x], tree);
  else
    return x;
  return tree[x];
}
