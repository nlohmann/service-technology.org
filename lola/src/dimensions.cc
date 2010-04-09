/*****************************************************************************\
 LoLA -- a Low Level Petri Net Analyzer

 Copyright (C)  1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007,
                2008, 2009  Karsten Wolf <lola@service-technology.org>

 LoLA is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 LoLA is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with LoLA.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/


#include <config.h>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include "dimensions.H"

using std::cout;
using std::endl;


/// writes the configuration into the configuration file
void configurationResult(FILE* res) {
    fprintf(res, "configuration: {\n");

    fprintf(res, "  verification = \"");
#ifdef LTLPROP
    fprintf(res, "LTLPROP");
#endif
#ifdef REACHABILITY
    fprintf(res, "REACHABILITY");
#endif
#ifdef BOUNDEDNET
    fprintf(res, "BOUNDEDNET");
#endif
#ifdef BOUNDEDPLACE
    fprintf(res, "BOUNDEDPLACE");
#endif
#ifdef DEADTRANSITION
    fprintf(res, "DEADTRANSITION");
#endif
#ifdef DEADLOCK
    fprintf(res, "DEADLOCK");
#endif
#ifdef HOME
    fprintf(res, "HOME");
#endif
#ifdef REVERSIBILITY
    fprintf(res, "REVERSIBILITY");
#endif
#ifdef MODELCHECKING
#ifdef EXTENDEDCTL
    fprintf(res, "MODELCHECKING+EXTENDEDCTL");
#else
    fprintf(res, "MODELCHECKING");
#endif
#endif
#ifdef FINDPATH
    fprintf(res, "FINDPATH");
#endif
#ifdef FULL
    fprintf(res, "FULL");
#endif
#ifdef STATESPACE
    fprintf(res, "STATESPACE");
#endif
#ifdef NONE
    fprintf(res, "NONE");
#endif
#if defined(STATEPREDICATE) && !defined(LIVEPROP)
    fprintf(res, "STATEPREDICATE");
#endif
#ifdef LIVEPROP
    fprintf(res, "LIVEPROP");
#endif
    fprintf(res, "\";\n");

    fprintf(res, "  exploration = \"");
#ifdef DEPTH_FIRST
    fprintf(res, "DEPTH_FIRST");
#else
    fprintf(res, "BREADTH_FIRST");
#endif
    fprintf(res, "\";\n");

    fprintf(res, "  reduction = {\n");


#ifdef STUBBORN
    fprintf(res, "    STUBBORN = true;\n");
#endif
#ifdef STATEPREDICATE
#ifdef RELAXED
    fprintf(res, "    RELAXED = true;\n");
#else
    fprintf(res, "    RELAXED = false;\n");
#endif
#endif
#ifdef SYMMETRY
    fprintf(res, "    SYMMETRY = true;\n");
    fprintf(res, "    SYMMINTEGRATION = %d;\n", SYMMINTEGRATION);
#endif
#ifdef COVER
    fprintf(res, "    COVER = true;\n");
#endif
#ifdef CYCLE
    fprintf(res, "    CYCLE = true;\n");
#endif
#ifdef PREDUCTION
    fprintf(res, "    PREDUCTION = true;\n");
#endif
#ifdef SMALLSTATE
    fprintf(res, "    SMALLSTATE = true;\n");
#endif
#ifdef BITHASH
    fprintf(res, "    BITHASH = true;\n");
#endif
#ifdef LIVEPROP
#ifdef TWOPHASE
    fprintf(res, "    TWOPHASE = true;\n");
#ifdef RELAXED
    fprintf(res, "    RELAXED = true;\n");
#else
    fprintf(res, "    RELAXED = false;\n");
#endif
#else
    fprintf(res, "    TWOPHASE = false;\n");
#endif
#endif
    fprintf(res, "  };\n");

    fprintf(res, "  parameters = {\n");
    fprintf(res, "    HASHSIZE = %d;\n", HASHSIZE);
    fprintf(res, "    REPORTFREQUENCY = %d;\n", REPORTFREQUENCY);
#if defined(CAPACITY) && CAPACITY > 0
    fprintf(res, "    CAPACITY = %d;\n", CAPACITY);
#endif
#ifdef FINDPATH
    fprintf(res, "    MAXPATH = %d;\n", MAXPATH);
#endif
#ifdef CYCLE
    fprintf(res, "    MAXUNSAVED = %d;\n", MAXUNSAVED);
#ifdef NONBRANCHINGONLY
    fprintf(res, "    NONBRANCHINGONLY = true;\n");
#endif
#endif
#if defined(SYMMETRY) && SYMMINTEGRATION == 4
    fprintf(res, "    MAXATTEMPT = %d;\n", MAXATTEMPT);
#endif
#ifdef MAXIMALSTATES
    fprintf(res, "    MAXIMALSTATES = %d;\n", MAXIMALSTATES);
#endif
    fprintf(res, "  };\n");

    fprintf(res, "};\n");
}

void reportconfiguration() {
    cout << "LoLA - A low level Analyzer" << endl;
    cout << "---------------------------" << endl << endl;
    cout << "state space based verification of low level Petri nets." << endl;
    cout << "written by Karsten Wolf" << endl;
    cout << "NO WARRANTY!" << endl;
    cout << "http://wwwteo.informatik.uni-rostock.de/ls_tpp/lola" << endl << endl;
    cout << "This executable is compiled in the following configuration:" << endl;
    cout << "* Verification problem: " << endl;
#ifdef LTLPROP
    cout << "  LTLPROP: check if the system satisfies" << endl;
    cout << "                a LTL property (given as Buchi automaton)" << endl;
#endif
#ifdef REACHABILITY
    cout << "  REACHABILITY: check if a (completely specified) marking" << endl;
    cout << "                is reachable from the initial marking" << endl;
#endif
#ifdef BOUNDEDNET
    cout << "  BOUNDEDNET: check if the net is bounded" << endl;
#endif
#ifdef BOUNDEDPLACE
    cout << "  BOUNDEDPLACE: check if a particular place is bounded" << endl;
#endif
#ifdef DEADTRANSITION
    cout << "  DEADTRANSITION: check if a particular transition is dead" << endl;
#endif
#ifdef DEADLOCK
    cout << "  DEADLOCK: check if a dead marking is reachable" << endl;
#endif
#ifdef HOME
    cout << "  HOME: check if the net has home states" << endl;
#endif
#ifdef REVERSIBILITY
    cout << "  REVERSIBILITY: check if the net is reversible" << endl;
#endif
#ifdef MODELCHECKING
#ifdef EXTENDEDCTL
    cout << "  MODELCHECKING+EXTENDEDCTL: verify an ECTL-formula" << endl;
#else
    cout << "  MODELCHECKING: verify a CTL-formula" << endl;
#endif
#endif
#ifdef FINDPATH
    cout << "  FINDPATH: simulate the net repeatedly to find a marking" << endl;
    cout << "            that satisfies a given state predicate" << endl;
#endif
#ifdef FULL
    cout << "  FULL: compute a graph without evaluating any property" << endl;
#endif
#ifdef STATESPACE
    cout << "  STATESPACE: compute a graph with information about" << endl;
    cout << "              strongly connected components" << endl;
#endif
#ifdef NONE
    cout << "  NONE: do not compute any graph" << endl;
#endif
#if defined(STATEPREDICATE) && !defined(LIVEPROP)
    cout << "  STATEPREDICATE: check if a state is reachable that" << endl;
    cout << "                  satisfies a given property." << endl;
#endif
#ifdef LIVEPROP
    cout << "  LIVEPROP: check if a given statepredicate is live" << endl;
#endif
    cout << endl << "* Graph exploration strategy: " << endl;
#ifdef SWEEP
    cout << "  SWEEP: search order controlled by the sweep line method" << endl;
#else
#ifdef DEPTH_FIRST
    cout << "  DEPTH_FIRST: depth first search order" << endl;
#else
    cout << "  BREADTH_FIRST: breadth first search order" << endl;
#endif
#endif

#ifdef TARJAN
    cout << "  TARJAN" << endl;
#endif
#ifdef FULLTARJAN
    cout << "  FULLTARJAN" << endl;
#endif

    cout << endl << "* Reduction techniques applied:" << endl;
#ifdef STUBBORN
    cout << "  STUBBORN   : partial order reduction using stubborn sets" << endl;
#endif
#ifdef STATEPREDICATE
#ifdef RELAXED
    cout << "               according to Kristensen/Valmari, 2000" << endl;
#else
    cout << "               according to Schmidt,1999" << endl;
#endif
#endif
#ifdef SYMMETRY
    cout << "  SYMMETRY   : state space condensation based on net automorphisms" << endl;
#endif
#ifdef COVER
    cout << "  COVER      : infinite sequence abstraction using " << endl;
    cout << "               the coverability graph technique" << endl;
#endif
#ifdef CYCLE
    cout << "  CYCLE      : store only cycle closing states, based on  " << endl;
    cout << "               T-invariants analysis " << endl;
#endif
#ifdef PREDUCTION
    cout << "  PREDUCTION : compress states using P-invariants" << endl;
#endif
#ifdef SMALLSTATE
    cout << "  SMALLSTATE : use light weight data structure for states" << endl;
#endif
#ifdef BITHASH
    cout << "  BITHASH    : store only hash value of states" << endl;
#endif
#ifdef SYMMETRY
    cout << endl << "* Way of using symmetries in graph exploration:" << endl;
    switch (SYMMINTEGRATION) {
        case 1:
            cout << "  1 - (pre-compute generating set) iterate symmetries " << endl;
            cout << "      and look up images" << endl;
            break;
        case 2:
            cout << "  2 - (no generating set) iterate states and try to map" << endl;
            cout << "      them to current state" << endl;
            break;
        case 3:
            cout << "  3 - (pre-compute generating set) use generators to" << endl;
            cout << "      transform current marking into (lx.) small one" << endl;
            break;
        case 4:
            cout << "  4 - (no generating set) compute approximation of " << endl;
            cout << "      canonical representative on the fly." << endl;
            break;
        case 5:
            cout << "  5 - (no generating set) compute canonical " << endl;
            cout << "      representative on the fly." << endl;
            break;
    }
#endif
#ifdef LIVEPROP
    cout << endl << "* Method of checking for liveness: " << endl;
#ifdef TWOPHASE
    cout << "  TWOPHASE: use two phased algorithm with" << endl;
#ifdef RELAXED
    cout << "  relaxed stubborn sets in phase two" << endl;
#else
    cout << "  attractor set based stubborn sets in phase two" << endl;
#endif
#else
    cout << "  use one phased algorithm with home property preserving stubborn sets" << endl;
#endif
#endif
    cout << endl << "* Values of numerical parameters: " << endl;
    cout << "  HASHSIZE        : " << HASHSIZE << " entries in hash table." << endl;
    cout << "  REPORTFREQUENCY : status report every " << REPORTFREQUENCY << " transitions fired." << endl;
#ifdef CAPACITY
    cout << "  CAPACITY        : at most " << CAPACITY << " tokens on any place." << endl;
#endif
#ifdef FINDPATH
    cout << "  MAXPATH         : reset to initial state after " << MAXPATH << " transition occurrences." << endl;
#endif
#ifdef CYCLE
    cout << "  MAXUNSAVED      : store, in addition to cycle closing states," << endl;
    cout << "                    states that have more than " << MAXUNSAVED <<
         " unsaved predecessors." << endl;
#ifdef NONBRANCHINGONLY
    cout << "  NONBRANCHINGONLY: store all states with more than one successor," << endl;
#endif
#endif
#if defined(SYMMETRY) && SYMMINTEGRATION == 4
    cout << "  MAXATTEMPT      : " << "perform at most " << MAXATTEMPT << " loops of approximizing the canonical representative." << endl;
#endif
#ifdef MAXIMALSTATES
    cout << "  MAXIMALSTATES   : " << "generate at most " << MAXIMALSTATES << " states" << endl;
#endif

}





/*!
 \brief write a userconfig.H that would create the same LoLA binary

 In contrast to the result of reportconfiguration(), this function generates
 pure C code that can directly used to compile a LoLA binary.

 This function can be handy if binaries need to be transfered to a different
 operating system or architecture.

 \param suffix  a suffix used to create the file `userconfig.H.suffix', the
                suffix is set by the command line paramter `--offspring'
 */
void createUserconfigFile(char* suffix) {
    assert(suffix);
    std::string filename = "userconfig.H." + std::string(suffix);

    // open file
    FILE* userconfig = fopen(filename.c_str(), "w");
    if (!userconfig) {
        fprintf(stderr, "lola: cannot write to userconfig offspring file '%s'\n", filename.c_str());
        fprintf(stderr, "      no output written\n");
        exit(4);
    }

    // print header
    fprintf(userconfig, "// This file 'userconfig.H.%s' was created by %s.\n", suffix, PACKAGE_STRING);
    fprintf(userconfig, "// To compile a binary with this configuration, copy this file into the folder\n");
    fprintf(userconfig, "// 'src/configs' of the LoLA distribution and execute 'make lola-%s'.\n", suffix);
    fprintf(userconfig, "// See the manual for a description and all possible parameters.\n\n");

    fprintf(userconfig, "// NUMERICAL PARAMETERS\n");

#ifdef CAPACITY
    if (CAPACITY != -1) {
        fprintf(userconfig, "#define CAPACITY %u\n", CAPACITY);
#ifdef CHECKCAPACITY
        fprintf(userconfig, "#define CHECKCAPACITY\n");
#endif
    }
#endif

#ifdef REPORTFREQUENCY
    if (REPORTFREQUENCY != 100) {
        fprintf(userconfig, "#define REPORTFREQUENCY %u\n", REPORTFREQUENCY);
    }
#endif

#ifdef MAXIMALSTATES
    fprintf(userconfig, "#define MAXIMALSTATES %u\n", MAXIMALSTATES);
#endif

#ifdef HASHSIZE
    fprintf(userconfig, "#define HASHSIZE %u\n", HASHSIZE);
#endif

    fprintf(userconfig, "\n// REDUCTION TECHNIQUES\n");

#ifdef COVER
    fprintf(userconfig, "#define COVER\n");
#endif

#ifdef STUBBORN
    fprintf(userconfig, "#define STUBBORN\n");
#if defined(STATEPREDICATE) || defined(LIVEPROP)
#ifdef RELAXED
    fprintf(userconfig, "#define RELAXED\n");
#endif
#endif
#endif

#ifdef SYMMETRY
    fprintf(userconfig, "#define SYMMETRY\n");
#ifdef SYMMINTEGRATION
    fprintf(userconfig, "#define SYMMINTEGRATION %u\n", SYMMINTEGRATION);
#ifdef define(MAXATTEMPT) && SYMMINTEGRATION == 4
    fprintf(userconfig, "#define MAXATTEMPT %u\n", MAXATTEMPT);
#endif
#endif
#endif

#ifdef CYCLE
    fprintf(userconfig, "#define CYCLE\n");
#ifdef NONBRANCHINGONLY
    fprintf(userconfig, "#define NONBRANCHINGONLY\n");
#endif
#ifdef MAXUNSAVED
    fprintf(userconfig, "#define MAXUNSAVED %u\n", MAXUNSAVED);
#endif

#endif

#ifdef PREDUCTION
    fprintf(userconfig, "#define PREDUCTION\n");
#endif
#ifdef SMALLSTATE
    fprintf(userconfig, "#define SMALLSTATE\n");
#endif
#ifdef BITHASH
    fprintf(userconfig, "#define BITHASH\n");
#endif
#ifdef DISTRIBUTE
    fprintf(userconfig, "#define DISTRIBUTE\n");
#endif
#ifdef SWEEP
    fprintf(userconfig, "#define SWEEP\n");
#endif

    fprintf(userconfig, "\n// GRAPH EXPLORATION STRATEGY\n");

#ifdef DEPTH_FIRST
    fprintf(userconfig, "#define DEPTH_FIRST\n");
#endif
#ifdef BREADTH_FIRST
    fprintf(userconfig, "#define BREADTH_FIRST\n");
#endif

    fprintf(userconfig, "\n// VERIFICATION PROBLEM\n");

#ifdef REACHABILITY
    fprintf(userconfig, "#define REACHABILITY\n");
#endif

#ifdef MODELCHECKING
    fprintf(userconfig, "#define MODELCHECKING\n");
#ifdef EXTENDEDCTL
    fprintf(userconfig, "#define EXTENDEDCTL\n");
#endif
#endif

#ifdef BOUNDEDPLACE
    fprintf(userconfig, "#define BOUNDEDPLACE\n");
#endif
#ifdef BOUNDEDNET
    fprintf(userconfig, "#define BOUNDEDNET\n");
#endif
#ifdef DEADTRANSITION
    fprintf(userconfig, "#define DEADTRANSITION\n");
#endif
#ifdef REVERSIBILITY
    fprintf(userconfig, "#define REVERSIBILITY\n");
#endif
#ifdef HOME
    fprintf(userconfig, "#define HOME\n");
#endif

#ifdef FINDPATH
    fprintf(userconfig, "#define FINDPATH\n");
#ifdef MAXPATH
    fprintf(userconfig, "#define MAXPATH %u\n", MAXPATH);
#endif
#endif

#ifdef FULL
    fprintf(userconfig, "#define FULL\n");
#endif
#ifdef DEADLOCK
    fprintf(userconfig, "#define DEADLOCK\n");
#endif
#ifdef NONE
    fprintf(userconfig, "#define NONE\n");
#endif
#ifdef STATEPREDICATE
    fprintf(userconfig, "#define STATEPREDICATE\n");
#endif

#ifdef LIVEPROP
    fprintf(userconfig, "#define LIVEPROP\n");
#ifdef TWOPHASE
    fprintf(userconfig, "#define TWOPHASE\n");
#endif
#ifdef STRUCT
    fprintf(userconfig, "#define STRUCT\n");
#endif
#endif

#ifdef FAIRPROP
    fprintf(userconfig, "#define FAIRPROP\n");
#endif
#ifdef STABLEPROP
    fprintf(userconfig, "#define STABLEPROP\n");
#endif
#ifdef EVENTUALLYPROP
    fprintf(userconfig, "#define EVENTUALLYPROP\n");
#endif

#ifdef STATESPACE
    fprintf(userconfig, "#define STATESPACE\n");
#endif

    fprintf(stderr, "lola: successfully wrote userconfig offspring file '%s'\n", filename.c_str());
    fclose(userconfig);
}
