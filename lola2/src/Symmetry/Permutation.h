/*!
  \file Permutation.h
  \author Andre
  \status new
  \brief Containing permutations and symmetries.


*/


#pragma once

#include <algorithm>
#include <string.h>
#include <stack>
#include <iostream>

#include <Core/Dimensions.h>

#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif

#include <Symmetry/PartitionRefinement.h>
#include <Symmetry/Refiners.h>

using namespace std;

/*!
  \brief

*/

template <class T>
class Perm
{
private:
public:
    Perm(T permutation) : perm(permutation) {};
    T perm;
    T composeLeft(T permutation)
    {
        return perm.compose(permutation);
    }
    T composeRight(T permutation)
    {
        return permutation.compose(perm);
    }
    T inverse()
    {
        return perm.inverse();
    }
    T power(int n)
    {
        return perm.power(n);
    }
    index_t stabilizes()
    {
        return perm.stabilizes();
    };
    void show()
    {
        perm.show();
    }
    static Perm<T> *find(Partition *partition, index_t x, index_t y);
};

template <typename T>
Perm<T> *Perm<T>::find(Partition *partition, index_t x, index_t y)
{
    assert(x != y); // we could just return the identity permutation
    if (partition->representative[x] != partition->representative[y])
    {
        return NULL;
    }

    // split codomain on x and domain on y
    Partition cod = *partition->copy();
    cod.fix(x);
    Partition dom = *partition->copy();
    dom.fix(y);
    if (!refineFix2(&cod, &dom))
    {
        std::cout << "Inconsistent!" << std::endl;
        return NULL;
    }

    // search for symmetry
    cell_t *split = NULL;
    index_t *mapping = NULL;
    stack<index_t *> cods;
    cods.push(new index_t[partition->order]);
    memcpy(cods.top(), cod.ranges, sizeof(index_t) * partition->order);
    stack<index_t *> doms;
    doms.push(new index_t[partition->order]);
    memcpy(doms.top(), dom.ranges, sizeof(index_t) * partition->order);
    stack<index_t *> candidates;
    while (!cods.empty() && !doms.empty())
    {
        // we assume that both partitions are consistent at the moment

        // find splitable cell (i.e., a cell with more than one candidate)
        if (split != NULL)
        {
            delete split;
        }
        split = NULL;
        for (index_t i = 0; i < partition->order; i += cods.top()[i])
        {
            if (split != NULL && cods.top()[i] > 1)
            {
                split = new cell_t;
                split->from = i;
                split->to = i + doms.top()[i] - 1;
            }
        }

        // there is no splitable cell => we have found a symmetry
        if (split == NULL)
        {
            mapping = new index_t[partition->order];
            for (index_t i = 0; i < partition->order; i++)
            {
                mapping[cod.elements[i]] = dom.elements[i];
            }

            while (!candidates.empty())
            {
                /*delete[] candidates.top();*/ candidates.pop();
            }
            while (!cods.empty())
            {
                delete[] cods.top();
                cods.pop();
            }
            while (!doms.empty())
            {
                delete[] doms.top();
                doms.pop();
            }
            break;
        }

        // determine possible mappings
        index_t preimage = cod.elements[split->from];
        index_t length = 1 + split->to - split->from;
        candidates.push(new index_t[length + 1]);
        memcpy(candidates.top(), &dom.elements[split->from], sizeof(index_t) * length);
        candidates.top()[length] = -1;

        //
        std::cout << "Fixing " << preimage << "(cod)" << std::endl;
        memcpy(cod.ranges, cods.top(), sizeof(index_t) * partition->order);
        cod.fix(preimage);
        cods.push(new index_t[partition->order]);
        memcpy(cods.top(), cod.ranges, sizeof(index_t) * partition->order);

        std::cout << "Fixing " << *candidates.top() << "(dom)" << std::endl;
        memcpy(dom.ranges, doms.top(), sizeof(index_t) * partition->order);
        dom.fix(*candidates.top());
        doms.push(new index_t[partition->order]);
        memcpy(doms.top(), dom.ranges, sizeof(index_t) * partition->order);
        candidates.top()++; // next pointer

        // the partitions are inconsistent
        while (!refineFix2(&cod, &dom))
        {
            std::cout << "Found inconsistency!" << std::endl;
            // drop until reaching branch with available candidates
            while (!candidates.empty() && *candidates.top() == (index_t) - 1)
            {
                /*delete[] candidates.top();*/ candidates.pop();
                delete[] cods.top();
                cods.pop();
                delete[] doms.top();
                doms.pop();
            }

            if (candidates.empty())
            {
                return NULL;    // dellocate properly
            }

            // try next candidate
            memcpy(cod.ranges, cods.top(), sizeof(index_t) * partition->order);
            delete[] doms.top();
            doms.pop();
            memcpy(dom.ranges, doms.top(), sizeof(index_t) * partition->order);
            dom.fix(*candidates.top());
            doms.push(new index_t[partition->order]);
            memcpy(doms.top(), dom.ranges, sizeof(index_t) * partition->order);
            candidates.top()++; // next pointer
        }
    }

    if (split != NULL)
    {
        delete split;
    }
    if (mapping == NULL)
    {
        return NULL;
    }
    return new Perm<T>(T::create(partition->order, mapping));
}


class Bijection
{
private:
    index_t size;
    index_t *map;
public:
    Bijection(index_t n, index_t *mapping) : size(n), map(mapping) {};
    Bijection compose(Bijection f);
    Bijection inverse();
    Bijection power(int n);
    index_t stabilizes();
    void show();

    static Bijection create(index_t n, index_t *mapping)
    {
        return Bijection(n, mapping);
    }
};

/*
  class CyclicPerm {
  // here comes a linked list!
  public:
  CyclicPerm(index_t n, index_t* mapping);
  CyclicPerm compose(Bijection f);
  CyclicPerm inverse();
  CyclicPerm power(int n);
  }


  template <class T>
  class Symmetry {
  T[2];
  public:
  index_t stabilizes();
  index_t
  }
*/
