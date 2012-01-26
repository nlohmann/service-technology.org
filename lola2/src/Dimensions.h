/*!
\author Karsten
\file Dimensions.h
\status approved 15.01.2012

\brief collection of arbitrarily chosen but fixed constants
*/

#pragma once

/// number of entries in a symbol table
#define SIZEOF_SYMBOLTABLE 0x10000

/// number of entries in a symbol table
#define SIZEOF_MARKINGTABLE 0x1000000

// Some useful constants, can be used as indices in arrays

/// Index for "Places" in arrays
#define PL 0

/// Index for "Transitions" in arrays
#define TR 1

/// Index for "Pre Set" in arrays
#define PRE 0

/// Index for "Post Set" in arrays
#define POST 1

// Some type definitions for dimensioning index structures

/// Type for node indices, limit the number of places/ transitions
typedef unsigned int index_type;

/// Type for multiplicities, limits the max. multiplicity at any arc
typedef unsigned int mult_type;

/// Type for capacities, limits the max. capacity of a place (to
/// sizeof(used type) -1, since max(type) is needed for representing
/// unboundedness in coverability graphs
typedef unsigned int capacity_type;


/// size of string buffer in Socket class
#define UDP_BUFFER_SIZE 1024
