/*****************************************************************************\
 Copyright (c) 2008  Robert Danitz, Christian Gierds, Niels Lohmann,
                     Peter Massuthe, Richard Mu"ller, Janine Ott,
                     Jan Su"rmeli, Daniela Weinberg, Karsten Wolf,
                     Martin Znamirowski

 Copyright (c) 2007  Jan Bretschneider, Robert Danitz, Christian Gierds,
                     Kathrin Kaschner, Leonard Kern, Niels Lohmann,
                     Peter Massuthe, Daniela Weinberg, Karsten Wolf

 Copyright (c) 2006  Kathrin Kaschner, Peter Massuthe, Daniela Weinberg,
                     Karsten Wolf

 This file is part of Fiona, see <http://service-technology.org/fiona>.

 Fiona is free software; you can redistribute it and/or modify it under the
 terms of the GNU General Public License as published by the Free Software
 Foundation; either version 3 of the License, or (at your option) any later
 version.
 
 Fiona is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License along with
 Fiona (see file COPYING). If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/


// User configuration file for Fiona

// specify whether each new operation shall be logged
//#define LOG_NEW

// enable Fiona to detect (T)SCC, when computing the reachability graph of each marking
// the states shown in the nodes of the graphic are then annotated with information
// about the SCC they are in and whether they are a representative of their SCC
//#define TSCC	

// CAPACITY is the largest expected number of tokens on any place.
// Set CAPACITY to 1 for safe nets. Do not define CAPACITY if
// you do not know upper limits for the number of tokens on places.

//#define CAPACITY 4
// #define CHECKCAPACITY

// REPORTFREQUENCY controls the output of progress messages to the
// terminal (to be precise: to the standard error stream).
// Progress messages are written during graph generation 
// after every REPORTFREQUENCY transition occurrences and during
// symmetry calculation after passing a search depth that is
// a multiple of REOPRTFREQUENCY. If  REPORTFREQUENCY is not
// defined, a standard value applies.

#define REPORTFREQUENCY 1000


// HASHSIZE is the number of entries in the table that hashes the 
// reachable states.  Larger values should speed up search but
// consume more memory. This variable must be set to a power of 2

//! \TODO: remove this definition if no hash tables are used
#define HASHSIZE 65536

// Defining COVER and/or SYMMETRY and/or STUBBORN enables the application of the
// corresponding reduction technique (if more than one value is defined,
// the corresponding techniques are applied jointly). 
// If a reduction technique does not fit to the analysis task, the reduction
// technique will be switched off automatically.

//! \TODO: Is this reduction really used??? It triggers code fragments, but it is not clear, whether there are ever used.
#define STUBBORN

// In connection with STATEPREDICATE (below), there exist two versions of
// stubborn set. RELAXED selects the Kristensen/Valmari method (Petri net
// conference 2000). Without RELAXED, the Schmidt method (Petri net conference
// 1999) is selected. Kristensen/Valmari behaves significantly better for 
// non-satisfiable predicates while Schmidt tends to approach target states more
// directly if there exist any. Without STUBBORN, or without STATEPREDICATE,
// setting or removing RELAXED does not influence anything.

//#define RELAXED

