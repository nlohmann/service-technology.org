#pragma once

#include <cstdlib>
#include <cstdio>

class Place;
class Transition;

class Globals {
    public:
        /* NET STRUCTURE */

        /// array containing all places of the net (in this array, significant places are sorted to the beginning)
        static Place** Places;

        /// array containing all transitions in the net
        static Transition** Transitions;


        /* MARKINGS */

        /// nr of bits for storing a single marking
        static unsigned int BitVectorSize;

        /// the current marking used in state space explorations
        static unsigned int* CurrentMarking;


        /* VERIFICATION */

        /// place that selected verification problem refers to
        static Place* CheckPlace;

        /// transition that selected verification problem refers to
        static Transition* CheckTransition;


        /* FILE NAMES */

        static char* lownetfile;

        static char* pnmlfile;

        /// name of file containing LoLA net input
        static char* netfile;

        /// name of file containing parameters of verification problem input
        static char* analysefile;

        /// name of file for reachability graph output
        static char* graphfile;

        /// name of file for witness/counterexample path output
        static char* pathfile;

        /// name of file for witness/counterexample state output
        static char* statefile;

        /// name of file for output of generating set of all symmetries
        static char* symmfile;

        /// base of net file name (used for generic output file names)
        static char* netbasename;

        static FILE* resultfile;


        /* FLAGS */

        static bool hflg;
        static bool Nflg;
        static bool nflg;
        static bool Aflg;
        static bool Sflg;
        static bool Yflg;
        static bool Pflg;
        static bool GMflg;
        static bool aflg;
        static bool sflg;
        static bool yflg;
        static bool pflg;
        static bool gmflg;
        static bool cflg;

        /// command line option (verbatim) for controlling reachability graph output format
        static char graphformat;
};
