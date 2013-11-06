
#include <algorithm>
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <omp.h>
#include <string.h>

#include <Core/Dimensions.h>

#include <Net/Net.h>

#include <Symmetry/PartitionRefinement.h>
#include <Symmetry/Refiners.h>

////////////////////////////////////////////////////////////////////////////////
// Partition
////////////////////////////////////////////////////////////////////////////////

Partition::Partition(index_t o) : order(o), elements(NULL), representative(NULL), ranges(NULL),
    rounds(NULL), round(0), change(false)
{
    if (order == 0)
    {
        return;
    }

    elements = new index_t[order];
    //#pragma omp parallel for
    for (index_t i = 0; i < order; i++)
    {
        elements[i] = i;
    }

    representative = new index_t[order]();
    ranges = new index_t[order]();
    ranges[0] = order;
    rounds = new index_t[order]();
}


Partition::~Partition()
{
    delete[] elements;
    delete[] representative;
    delete[] ranges;
    delete[] rounds;
}

Partition *Partition::copy()
{
    Partition *partition = new Partition(order);
    memcpy(partition->elements, elements, sizeof(index_t) * order);
    memcpy(partition->representative, representative, sizeof(index_t) * order);
    memcpy(partition->ranges, ranges, sizeof(index_t) * order);
    memcpy(partition->rounds, rounds, sizeof(index_t) * order);
    partition->round = round;
    partition->change = change;
    return partition;
}

void Partition::show()
{
    for (index_t i = 0; i < order; i += ranges[i])
    {
        std::cout << "[" << i << ";" << i + ranges[i] - 1 << "] = {";
        index_t repr = representative[elements[i]];
        for (index_t j = i; j < i + ranges[i]; j++)
        {
            assert(representative[elements[j]] == repr);
            assert(representative[elements[j]] >= i && representative[elements[j]] <= i + ranges[i] - 1);
            std::cout << elements[j];
            if (j + 1 < i + ranges[i])
            {
                std::cout << ", ";
            }
        }
        std::cout << "}" << std::endl;
    }
}

void Partition::fix(index_t x)
{
    assert(x >= 0 && x < order);
    index_t from = representative[x];
    index_t to = from + ranges[from] - 1;
    if (from == to)
    {
        return;
    }

    index_t position;
    for (position = from; elements[position] != x; position++);
    exch(position, to);
    representative[x] = to;
    round++;
    ranges[from]--;
    rounds[from] = round;
    ranges[to] = 1;
    rounds[to] = round;
    change = true;
}

void Partition::reset()
{
    change = false;
    round = 0;
    memset(rounds, 0, sizeof(index_t) * order);
}

bool Partition::trivial()
{
    return (ranges[0] == order);
}

bool Partition::complete()
{
    for (index_t i = 0; i < order; i++)
    {
        if (ranges[i] == 0)
        {
            return false;
        }
    }

    return true;
}

inline void Partition::exch(index_t i, index_t j)
{
    index_t tmp = elements[i];
    elements[i] = elements[j];
    elements[j] = tmp;
}


////////////////////////////////////////////////////////////////////////////////
// Refinement
////////////////////////////////////////////////////////////////////////////////

inline void Refinement::confine(index_t left, index_t right)
{
    assert(left <= right);
    assert(partition->ranges[left] == 0 || partition->ranges[left] >= 1 + right - left);

    if (partition->ranges[left] == 1 + right - left)
    {
        return;
    }
    partition->ranges[left] = 1 + right - left;
    partition->rounds[left] = partition->round;
    partition->change = true;
}

bool Refinement::perform(index_t left, index_t right)
{
    assert(left <= right && right != (index_t) - 1);
    assert(partition->ranges[left] == 1 + right - left);
    dispatch(left, right);
    return (partition->ranges[left] < 1 + right - left);
}


////////////////////////////////////////////////////////////////////////////////
// Predicate Refinement
////////////////////////////////////////////////////////////////////////////////

void PredicateRefinement::dispatch(index_t left, index_t right)
{
    index_t *a = partition->elements;
    index_t *repr = partition->representative;

    index_t i = left, j = right;
    while (i <= j && j < (index_t) - 1) if (evaluate(a[i]))
        {
            partition->exch(i, j--);
        }
        else
        {
            i++;
        }
    for (index_t k = left; k < i; k++)
    {
        repr[a[k]] = left;
    }
    for (index_t k = i; k <= right; k++)
    {
        repr[a[k]] = i;
    }

    if (i == left || j == right)
    {
        confine(left, right);
        return;
    }
    confine(left, i - 1);
    confine(i, right);
}

////////////////////////////////////////////////////////////////////////////////
// Comparison Refinement
////////////////////////////////////////////////////////////////////////////////

void ComparisonRefinement::dispatch(index_t left, index_t right)
{
    index_t *a = partition->elements;
    index_t *repr = partition->representative;

    if (left == right)
    {
        repr[a[left]] = left;
        confine(left, right);
        return;
    }
    if (right  < left || right == (index_t) - 1)
    {
        return;
    }

    index_t i = left + 1, j = right, p = left + 1;
    partition->exch(left + rand() % (1 + right - left), left);
    index_t v = a[left];
    repr[v] = left;

    do
    {
        while (compare(a[j], v) == CMP_GT)
        {
            j--;
        }
        while (compare(a[i], v) == CMP_LT) if (++i > right)
            {
                break;
            }

        if (i == j)
        {
            partition->exch(i++, p++);
        }
        if (i  > j)
        {
            break;
        }

        if (compare(a[i], v) == CMP_EQ)
        {
            partition->exch(i++, p++);
        }
        partition->exch(i, j);
        if (compare(a[i], v) == CMP_EQ)
        {
            partition->exch(i++, p++);
        }

    }
    while (i <= right);

    // swap EQ partition and LT partition to preserve order
    if (i > p) for (index_t k = left, l = i - 1; k < p; k++, l--)
        {
            partition->exch(k, l);
        }
    for (index_t k = left + i - p; k < i; k++)
    {
        repr[a[k]] = left + i - p;
    }

    dispatch(left, left + i - p - 1);
    confine(left + i - p, i - 1);
    dispatch(j + 1, right);
}

////////////////////////////////////////////////////////////////////////////////
// Counting Refinement
////////////////////////////////////////////////////////////////////////////////

void CountingRefinement::dispatch(index_t left, index_t right)
{
    if (right < left)
    {
        return;
    }

    index_t *a = partition->elements;
    index_t *repr = partition->representative;
    index_t num = 1 + (Net::Card[PL] + Net::Card[TR]) / BUCKET_SIZE;
    if (num == 1)
    {
        counting(left, right, 0, Net::Card[PL] + Net::Card[TR]);
        return;
    }

    // count hits for each bin
    int *hits = new int[num];
    memset(hits, 0, sizeof(index_t) * num);
    for (index_t i = left; i <= right; i++)
    {
        hits[value(a[i]) / BUCKET_SIZE]++;
    }

    // calculate prefix sum
    int used = hits[0] > 0 ? 1 : 0;
    int *reprs = new int[num];
    memset(reprs, 0, sizeof(index_t) * num);
    int *bins = new int[num];
    memset(bins, 0, sizeof(index_t) * num);
    bins[0] = left + hits[0];
    for (index_t i = 1; i < num; i++)
    {
        if (hits[i] > 0)
        {
            used++;
        }
        reprs[i] = hits[i - 1];
        bins[i] = hits[i] + bins[i - 1];
    }

    // move elements to their segment
    if (used > 1)
    {
        for (index_t i = left; i <= right; i++)
        {
            while ((int)i < bins[value(a[i]) / BUCKET_SIZE])
            {
                repr[a[i]] = reprs[value(a[i]) / BUCKET_SIZE];
                partition->exch(i, --bins[value(a[i]) / BUCKET_SIZE]);
            }
        }
    }

    // counting sort on each bin
    for (index_t i = 0, j = left; i < num; j += hits[i], i++)
    {
        counting(j, j + hits[i] - 1, BUCKET_SIZE * i, BUCKET_SIZE * (i + 1) - 1);
    }

    delete[] bins;
    delete[] reprs;
    delete[] hits;
}


void CountingRefinement::counting(index_t left, index_t right, index_t low, index_t high)
{
    index_t *a = partition->elements;
    index_t *repr = partition->representative;

    if (right == left)
    {
        repr[a[left]] = left;
        confine(left, right);
        return;
    }
    if (right  < left || right == (index_t) - 1)
    {
        return;
    }

    // count hits
    int hits[BUCKET_SIZE] = {0};
    for (index_t i = left; i <= right; i++)
    {
        hits[value(a[i]) - low]++;
    }

    // calculate prefix sum and insert new cells
    int reprs[BUCKET_SIZE] = {0};
    int used = 0;
    if (hits[0] > 0)
    {
        confine(left, left + hits[0] - 1);
        used++;
    }
    hits[0] += left;
    reprs[0] = left;
    for (int i = 1; i < BUCKET_SIZE; i++)
    {
        if (hits[i] > 0)
        {
            confine(hits[i - 1], hits[i - 1] + hits[i] - 1);
            used++;
        }
        reprs[i] = hits[i - 1];
        hits[i] += hits[i - 1];
    }

    if (used > 1)
    {
        for (index_t i = left; i <= right; i++)
        {
            while ((int)i < hits[value(a[i]) - low])
            {
                repr[a[i]] = reprs[value(a[i]) - low];
                partition->exch(i, --hits[value(a[i]) - low]);
            }
        }
    }
}

