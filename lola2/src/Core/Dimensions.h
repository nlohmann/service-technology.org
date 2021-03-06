/*!
\file
\brief collection of arbitrarily chosen but fixed constants
\author Karsten
\status approved 27.01.2012
\ingroup g_runtime
*/

#pragma once

// the macro _CONFIGURING is set while executing the configure script to avoid
// inclusion of config.h before creating it
#ifndef _CONFIGURING
#include <config.h>

// make sure this bug will never happen again
#if SIZEOF_INDEX_T == 0 || SIZEOF_MULT_T == 0 || SIZEOF_CAPACITY_T == 0 || SIZEOF_CARDBIT_T == 0 || SIZEOF_HASH_T == 0 || SIZEOF_FAIRNESSASSUMPTION_T == 0 || SIZEOF_SOCKADDR_IN == 0
#error The size of a user-defined data type could not be determined by the configure script.
#endif
#endif

#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif

#ifdef HAVE_INDEX_T
#include <sys/types.h>
#endif

/// number of entries in a symbol table
#define SIZEOF_SYMBOLTABLE 0x10000

/// number of entries in a hash table for markings
#define SIZEOF_MARKINGTABLE 0x100000

/// number of stack frames in one chunk of the search stack, better be power of two
#define SIZEOF_STACKCHUNK 0x1000

// Some useful constants, can be used as indices in arrays

typedef enum
{
    PL = 0, ///< Index for "Places" in arrays
    TR = 1  ///< Index for "Transitions" in arrays
} node_t;

typedef enum
{
    PRE = 0, ///< Index for "Pre Set" in arrays
    POST = 1 ///< Index for "Post Set" in arrays
} direction_t;

// Some type definitions for dimensioning index structures

/// Type for node indices, limit the number of places/ transitions
#ifndef HAVE_INDEX_T /* need to cover this line from Solaris */
typedef uint32_t index_t;
#endif

/// type for numbering/counting the states of the petri net while processing
typedef uint32_t statenumber_t;

/// Type for bitvector indices, should be large enough for card(places) * log maxcapacity
typedef uint32_t bitindex_t;

/// Type for multiplicities, limits the max. multiplicity at any arc
typedef uint16_t mult_t;

/// Type for capacities, limits the max. capacity of a place (to
/// sizeof(used type) -1, since max(type) is needed for representing
/// unboundedness in coverability graphs
typedef uint32_t capacity_t;
#define MAX_CAPACITY ((uint32_t)-1)

/// Type for hash values (32 bit plus enough space to calculate)
typedef int64_t hash_t;
#define MAX_HASH (1 << 31)

/// Type for the number of bits required to store a place's marking
typedef uint8_t cardbit_t;

/// size of string buffer in Socket class
#define UDP_BUFFER_SIZE 1024

/*!
The exit codes of LoLA:
0   specified state or deadlock found/net or place unbounded/home marking
    exists/net is reversible/predicate is live/CTL formula true/transition
    not dead/liveness property does not hold;
1   the opposite verification result as a thumb rule, if the outcome of a
    verification result can be supported by a counterexample or witness
    path, that case corresponds to return value 0;
2   any kind of error (syntax error, wrong command line, ...) occurred
3   premature termination (ctrl+C, kill, remote termination, ...)
*/
typedef enum
{
    EXIT_EARLY       = 0, ///< counterexample found
    EXIT_FULL        = 1, ///< no counterexample found
    EXIT_ERROR       = 2, ///< error
    EXIT_TERMINATION = 3  ///< premature termination
} exit_t;

/*!
The most special type of a formula, so LoLA can use dedicated routines.
*/
typedef enum
{
    FORMULA_REACHABLE     = 0, ///< EF
    FORMULA_INVARIANT     = 1, ///< AG
    FORMULA_IMPOSSIBLE    = 2, ///< AG not
    FORMULA_LIVENESS      = 3, ///< AGEF
    FORMULA_FAIRNESS      = 4, ///< GF
    FORMULA_STABILIZATION = 5, ///< FG
    FORMULA_EVENTUALLY    = 6, ///< F
    FORMULA_INITIAL       = 7, ///< no temporal operator
    FORMULA_LTL           = 8, ///< LTL formulae
    FORMULA_CTL           = 9, ///< CTL formulae
    FORMULA_MODELCHECKING = 10 ///< CTL* formulae
} formula_t;

/// three-valued logic
typedef enum
{
    TRINARY_FALSE = 0,
    TRINARY_TRUE = 1,
    TRINARY_UNKNOWN = 2
} trinary_t;

/// number of seconds between report outputs
#define REPORT_FREQUENCY 5

/// the size of the bloom filter (2 GB)
//#define BLOOM_FILTER_SIZE 0x400000000

/// the size of the bloom filter (800 MB for the model checking contest)
#define BLOOM_FILTER_SIZE 0x190000000

/////////////
// types used in SuffixTreeStore and in BinStore2
typedef uint32_t vectordata_t;

// maximum size (in bits) of a place
#define PLACE_WIDTH (SIZEOF_CAPACITY_T * 8)

// maximum size (in bits) of a vector item
#define VECTOR_WIDTH (SIZEOF_VECTORDATA_T * 8)


/////////////
// types for Tarjan
typedef uint64_t dfsnum_t;

/////////////
// types used for symmetry reduction

// bucket size for bucketized counting sort
#define BUCKET_SIZE 3000

// Comparison
typedef enum
{
    CMP_LT = 0,
    CMP_EQ = 1,
    CMP_GT = 2
} cmp_t;

/*!
This is just a type definition for the possible fairness assumptions that can
be specified for each transition. We consider weak and strong fairness as well
as the absence of fairness.
*/
typedef enum
{
    NO_FAIRNESS     = 0,  ///< no fairness
    WEAK_FAIRNESS   = 1,  ///< weak fairness
    STRONG_FAIRNESS = 2   ///< strong fairness
}
fairnessAssumption_t;
