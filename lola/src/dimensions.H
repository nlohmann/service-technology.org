// System dependent sizes and values

#ifndef DIMENSIONS_H
#define DIMENSIONS_H

#include "userconfig.H"
#include <iostream>
#include <limits.h>

#undef APPROXIMATE_CANONIZATION
// TARJAN=evaluate dfs and min for detecting terminal scc
#ifdef SMALLSTATE
#undef TARJAN
#else
#define TARJAN
#endif
// defaults for user defined values

#ifndef REPORTFREQUENCY
#define REPORTFREQUENCY 100
#endif

#ifndef MAXPATH
#define MAXPATH 3000000
#endif

#ifndef STATEPREDICATE
#ifndef LIVEPROP
#undef RELAXED
#endif
#endif

#ifdef STABLEPROP
#define ST 1
#else
#define ST 0
#endif

#ifdef FAIRPROP
#define FA 1
#else
#define FA 0
#endif

#ifdef EVENTUALLYPROP
#define EV 1
#else
#define EV 0
#endif

#ifdef REACHABILITY
#define RC 1
#else
#define RC 0
#endif

#ifdef MODELCHECKING
#define MC 1
#else
#define MC 0
#endif

#ifdef LTLPROP
#define LT 1
#else
#define LT 0
#endif

#ifdef BOUNDEDPLACE
#define BP 1
#else
#define BP 0
#endif

#ifdef DEADTRANSITION
#define DT 1
#else
#define DT 0
#endif

#ifdef REVERSIBILITY
#define RV 1
#else
#define RV 0
#endif

#ifdef HOME
#define HO 1
#else
#define HO 0
#endif

#ifdef FINDPATH
#define FP 1
#else 
#define FP 0
#endif

#ifdef FULL
#define FU 1
#else 
#define FU 0
#endif

#ifdef BOUNDEDNET
#define BN 1
#else
#define BN 0
#endif

#ifdef DEADLOCK
#define DL 1
#else
#define DL 0
#endif

#ifdef STATEPREDICATE
#define SP 1
#else
#define SP 0
#endif

#ifdef LIVEPROP
#define LP 1
#else
#define LP 0
#endif

#ifdef NONE
#define NO 1
#else
#define NO 0
#endif

#if (LT + ST + EV + FA + RC + BP + MC + DT + RV + HO + FP + FU + BN + DL + SP + LP + NO) > 1
#error "define only one verification task!"
#endif

#if (LT + ST + EV + FA + RC + BP + MC + DT + RV + HO + FP + FU + BN + DL + SP + LP + NO) < 1
#error "define a verification task by uncommenting the corresponding line in file userconfig.H!"
#endif

#if (RC + DT + FU + DL + SP + BN + BP) < 1
#undef BREADTH_FIRST
#endif


#if (RC + DT + FU + DL + SP ) < 1
#undef BITHASH
#undef SWEEP
#undef DISTRIBUTE
#endif

#ifdef DISTRIBUTE
#define BREADTH_FIRST
#define DEPTH_FIRST
#define TARJAN
#undef RELAXED
#undef SWEEP
#undef BITHASH
#undef CYCLE
#define GRAPH depth_first
#undef COVER
#else
#ifdef BREADTH_FIRST
#define GRAPH breadth_first
#define TARJAN
#undef RELAXED
#else
#ifdef SWEEP
#undef CYCLE
#undef SYMMETRY
#undef PREDUCTION
#undef BITHASH
#define GRAPH depth_first
#undef TARJAN
#undef RELAXED
#undef COVER
#else
#define DEPTH_FIRST
#define GRAPH depth_first
#endif
#endif
#endif

/* TWOPHASE ergibt nur bei LIVEPROP oder HOME Sinn, sonst nicht */
#ifdef TWOPHASE
#if (!defined(LIVEPROP) && !defined(HOME))
#undef TWOPHASE
#endif
#endif

#undef RC
#undef BP 
#undef MC
#undef DT
#undef RV
#undef HO
#undef FP
#undef FU
#undef BN
#undef DL
#undef SP
#undef LP
#undef NO
#undef FA
#undef EV
#undef ST
#undef LT

#ifdef HASHSIZE

#if HASHSIZE < 1
#undef HASHSIZE
#define HASHSIZE 1
#endif

#if (HASHSIZE > 2) && (HASHSIZE < 4)
#undef HASHSIZE
#define HASHSIZE 2
#endif

#if (HASHSIZE > 4) && (HASHSIZE < 8)
#undef HASHSIZE
#define HASHSIZE 4
#endif

#if (HASHSIZE > 8) && (HASHSIZE < 16)
#undef HASHSIZE
#define HASHSIZE 8
#endif

#if (HASHSIZE > 16) && (HASHSIZE < 32)
#undef HASHSIZE
#define HASHSIZE 16
#endif

#if (HASHSIZE > 32) && (HASHSIZE < 64)
#undef HASHSIZE
#define HASHSIZE 32
#endif

#if (HASHSIZE > 64) && (HASHSIZE < 128)
#undef HASHSIZE
#define HASHSIZE 64
#endif

#if (HASHSIZE > 128) && (HASHSIZE < 256)
#undef HASHSIZE
#define HASHSIZE 128
#endif

#if (HASHSIZE > 256) && (HASHSIZE < 512)
#undef HASHSIZE
#define HASHSIZE 256
#endif

#if (HASHSIZE > 512) && (HASHSIZE < 1024)
#undef HASHSIZE
#define HASHSIZE 512
#endif

#if (HASHSIZE > 1024) && (HASHSIZE < 2048)
#undef HASHSIZE
#define HASHSIZE 1024
#endif

#if (HASHSIZE > 2048) && (HASHSIZE < 4096)
#undef HASHSIZE
#define HASHSIZE 2048
#endif

#if (HASHSIZE > 4096) && (HASHSIZE < 8192)
#undef HASHSIZE
#define HASHSIZE 4096
#endif

#if (HASHSIZE > 8192) && (HASHSIZE < 16384)
#undef HASHSIZE
#define HASHSIZE 8192
#endif

#if (HASHSIZE > 16384) && (HASHSIZE < 32768)
#undef HASHSIZE
#define HASHSIZE 16384
#endif

#if (HASHSIZE > 32768) && (HASHSIZE < 65536)
#undef HASHSIZE
#define HASHSIZE 32768
#endif

#if (HASHSIZE > 65536) && (HASHSIZE < 131072)
#undef HASHSIZE
#define HASHSIZE 65536
#endif

#if (HASHSIZE > 131072) && (HASHSIZE < 262144)
#undef HASHSIZE
#define HASHSIZE 131072
#endif

#if (HASHSIZE > 262144) && (HASHSIZE < 524288)
#undef HASHSIZE
#define HASHSIZE 262144
#endif

#if (HASHSIZE > 524288) && (HASHSIZE < 1048576)
#undef HASHSIZE
#define HASHSIZE 524288
#endif

#if (HASHSIZE > 1048576) && (HASHSIZE < 2097152)
#undef HASHSIZE
#define HASHSIZE 1048576
#endif

#if (HASHSIZE > 2097152) && (HASHSIZE < 4194304)
#undef HASHSIZE
#define HASHSIZE 2097152
#endif

#if (HASHSIZE > 4194304) && (HASHSIZE < 8388608)
#undef HASHSIZE
#define HASHSIZE 4194304
#endif

#if (HASHSIZE > 8388608) && (HASHSIZE < 16777216)
#undef HASHSIZE
#define HASHSIZE 8388608
#endif

#if (HASHSIZE > 16777216) && (HASHSIZE < 33554432)
#undef HASHSIZE
#define HASHSIZE 16777216
#endif

#if (HASHSIZE > 33554432) && (HASHSIZE < 67108864)
#undef HASHSIZE
#define HASHSIZE 33554432
#endif

#if (HASHSIZE > 67108864) && (HASHSIZE < 134217728)
#undef HASHSIZE
#define HASHSIZE 67108864
#endif

#if (HASHSIZE > 134217728) && (HASHSIZE < 268435456)
#undef HASHSIZE
#define HASHSIZE 134217728
#endif

#if (HASHSIZE > 268435456) && (HASHSIZE < 536870912)
#undef HASHSIZE
#define HASHSIZE 268435456
#endif

#if (HASHSIZE > 536870912) && (HASHSIZE < 1073741824)
#undef HASHSIZE
#define HASHSIZE 536870912
#endif

#if (HASHSIZE > 1073741824) && (HASHSIZE < 2147483648)
#undef HASHSIZE
#define HASHSIZE 1073741824
#endif

#if (HASHSIZE > 2147483648)
#undef HASHSIZE
#define HASHSIZE  2147483648
#endif



#endif

#ifndef HASHSIZE
#ifdef BITHASH
#define HASHSIZE 16777216
#else
#define HASHSIZE 65536;
#endif
#endif

#ifdef SYMMETRY
#ifndef SYMMINTEGRATION
#define SYMMINTEGRATION 3
#endif

#if SYMMINTEGRATION > 5
#undef SYMMINTEGRATION
#define SYMMINTEGRATION 3
#endif

#ifdef DISTRIBUTE
#undef SYMMINTEGRATION
#define SYMMINTEGRATION 3
#endif

#if SYMMINTEGRATION == 1
#define SEARCHPROC bin_symm_search_marking
#define INSERTPROC binInsert
#elif SYMMINTEGRATION == 2
#define SEARCHPROC bin_symm_search_marking2
#define INSERTPROC binInsert
#undef PREDUCTION
#elif SYMMINTEGRATION == 3
#define SEARCHPROC canonical_representitive
#define INSERTPROC binInsert
//#define INSERTPROC insert_marking
#else 
#define SEARCHPROC canonical_representitive_on_the_fly
#define INSERTPROC insert_marking
#endif


#if SYMMINTEGRATION == 4
#define APPROXIMATE_CANONIZATION
#endif
#endif

#ifndef MAXUNSAVED
#define MAXUNSAVED 5
#endif

#if defined(SYMMETRY) && SYMMINTEGRATION < 2
#undef BITHASH
#endif

#ifdef BOUNDEDPLACE
#define COVER
#endif

#ifdef BOUNDEDNET
#define COVER
#endif

#if !defined(FULL) && !defined(BOUNDEDPLACE) && !defined(DEADTRANSITION) && !defined(BOUNDEDNET)
#undef COVER
#endif

#ifdef COVER
// tscc are not important for cover, but succ is used for path generation
#define TARJAN
#undef CAPACITY
#undef CYCLE
#undef BITHASH
#endif

#ifdef FINDPATH
#define WITHFORMULA
#undef SYMMETRY
#define FIRELIST firelist
#endif

#ifdef HOME
#undef SYMMETRY
#ifndef STUBBORN
#define STUBBORN
#endif
#define FIRELIST tsccstubbornlist
//EXTENDED = enable ignored transitions management
#define EXTENDED
//TSCC = retrieve representitives for all TSCC
#define TSCC
#define TARJAN
#endif

#ifdef REVERSIBILITY
#ifndef STUBBORN
#define STUBBORN
#endif
#define EXTENDED
#define TSCC
#define TARJAN
#define FIRELIST tsccstubbornlist
#endif

#ifdef STABLEPROP
#undef STUBBORN
#undef SYMMETRY
#undef CYCLE
#define FULLTARJAN
#define TARJAN
#define FIRELIST firelist
#define WITHFORMULA
#endif

#ifdef FAIRPROP
#undef STUBBORN
#undef SYMMETRY
#undef CYCLE
#define FULLTARJAN
#define TARJAN
#define FIRELIST firelist
#define WITHFORMULA
#endif

#ifdef EVENTUALLYPROP
#undef STUBBORN
#undef SYMMETRY
#undef CYCLE
#define FULLTARJAN
#define TARJAN
#define FIRELIST firelist
#define WITHFORMULA
#endif

#ifdef LIVEPROP
#ifndef STUBBORN
#define STUBBORN
#endif
#define WITHFORMULA
#ifndef EXTENDED
#define EXTENDED
#endif
#define TARJAN
#ifdef TWOPHASE
#define TSCC
#define FIRELIST tsccstubbornlist
#else
#define STATEPREDICATE
#define TSCC
#define RELAXED
#endif
#endif

#ifdef DEADTRANSITION
#ifdef STUBBORN
#define FIRELIST stubbornfireliststatic
#else
#define FIRELIST firelist
#endif
#endif

#ifdef BOUNDEDPLACE
#ifdef STUBBORN
#define FIRELIST stubbornfireliststatic
#else
#define FIRELIST firelist
#endif
#endif

#ifdef BOUNDEDNET
#ifdef STUBBORN
#define FIRELIST stubbornfireliststatic
#else
#define FIRELIST firelist
#endif
#endif

#ifdef REACHABILITY
#ifdef STUBBORN
#define FIRELIST stubbornfirelistreach
#else
#define FIRELIST firelist
#endif
#endif

#ifdef MODELCHECKING
#define FIRELIST firelist
#define WITHFORMULA
#ifdef STUBBORN
#define EXTENDED
#endif
#endif

#ifdef LTLPROP
#define FIRELIST firelist
#define WITHFORMULA
#endif

#ifdef DEADLOCK
#ifdef STUBBORN
#define FIRELIST tsccstubbornlist
#else
#define FIRELIST firelist
#endif
#endif

#ifdef STATEPREDICATE
#undef SYMMETRY
#define WITHFORMULA
#ifdef STUBBORN
#ifdef STRUCT
#undef RELAXED
#define RELAXED
#endif
#ifdef RELAXED
#ifdef STRUCT
#define FIRELIST structreachstubbornset
#undef TARJAN
#else
#define EXTENDED
#define TARJAN
#define FIRELIST relaxedstubbornset
#endif
#else

#define FIRELIST stubbornfirelistpredicate

#endif
#else
#define FIRELIST firelist
#endif
#endif


#if defined(FULL) || defined(NONE)
#ifdef STUBBORN
#ifdef STRUCT
#define FIRELIST structstubbornset
#else
#define FIRELIST tsccstubbornlist
#endif
#else
#define FIRELIST firelist
#endif
#endif

/// Create dimensions for storing markings
// MSIZE = Number of bits for the marking of a place
// MPERWORD = places packed into an unsigned int
// MASK = 2^MSIZE -1

#ifdef BITHASH
#define MSIZE 1
#define MPERWORD  32
#define MASK 1
#else
#ifdef CAPACITY

#if CAPACITY < 2
#define MSIZE 1
#define MPERWORD 32
#define MASK 1
#endif

#if (CAPACITY > 1) && (CAPACITY < 4)
#define MSIZE 2
#define MPERWORD 16
#define MASK 3
#endif

#if (CAPACITY > 3) && (CAPACITY < 8)
#define MSIZE 3
#define MPERWORD 10
#define MASK 7
#endif

#if (CAPACITY > 7) && (CAPACITY < 16)
#define MSIZE 4
#define MPERWORD 8
#define MASK 15
#endif

#if (CAPACITY > 15) && (CAPACITY < 32)
#define MSIZE 5
#define MPERWORD 6
#define MASK 31
#endif

#if (CAPACITY > 31) && (CAPACITY < 64)
#define MSIZE 6
#define MPERWORD 5
#define MASK 63
#endif

#if (CAPACITY > 63) && (CAPACITY < 256)
#define MSIZE 7
#define MPERWORD 4
#define MASK 255
#endif

#if (CAPACITY > 255) && (CAPACITY < 1024)
#define MSIZE 10
#define MPERWORD 3
#define MASK 1023
#endif

#if (CAPACITY > 1023) && (CAPACITY < 65536)
#define MSIZE 16
#define MPERWORD 2
#define MASK 65535
#endif

#if (CAPACITY > 65535)
#undef CAPACITY
#define MSIZE 32
#define MPERWORD 1
#define MASK UINT_MAX
#endif

#else
#define MSIZE 32
#define MPERWORD 1
#define MASK UINT_MAX
#endif
#endif
/////////////////////////////

#ifndef MODELCHECKING
#undef EXTENDEDCTL
#endif

#ifndef MAXATTEMPT 
#define MAXATTEMPT 0
#endif

#ifndef SYMMETRY
#define SEARCHPROC binSearch
//#define SEARCHPROC search_marking
#define INSERTPROC binInsert
//#define INSERTPROC insert_marking
#endif

#ifdef BITHASH
#undef SEARCHPROC
#undef INSERTPROC
#define SEARCHPROC bit_search
#define INSERTPROC bit_insert
#endif

#define VERYLARGE UINT_MAX
#ifndef CAPACITY
#define CAPACITY -1
#endif

void reportconfiguration();
void createUserconfigFile(char *filename);

#endif
