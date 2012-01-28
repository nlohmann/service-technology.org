/*!
\author Karsten
\file Dimensions.h
\status approved 27.01.2012

\brief collection of arbitrarily chosen but fixed constants
*/

#pragma once

// the macro _CONFIGURING is set while executing the configure script to avoid
// inclusion of config.h before creating it
#ifndef _CONFIGURING
#include <config.h>
#endif

#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif

/// number of entries in a symbol table
#define SIZEOF_SYMBOLTABLE 0x10000

/// number of entries in a symbol table
#define SIZEOF_MARKINGTABLE 0x1000000

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
typedef uint32_t index_t;

/// Type for multiplicities, limits the max. multiplicity at any arc
typedef uint16_t mult_t;

/// Type for capacities, limits the max. capacity of a place (to
/// sizeof(used type) -1, since max(type) is needed for representing
/// unboundedness in coverability graphs
typedef uint32_t capacity_t;
#define MAX_CAPACITY (((1 << 31)-1) + (1 << 31))

/// Type for hash values (32 bit plus enough space to calculate)
typedef int64_t hash_t;
#define MAX_HASH (1 << 31)

/// Type for the number of bits required to store a place's marking
typedef uint8_t cardbit_t;

/// size of string buffer in Socket class
#define UDP_BUFFER_SIZE 1024
