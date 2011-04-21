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


#include "config.h"
#include "printnet.H"
#include "dimensions.H"
#include "net.H"
#include "symboltab.H"
#include "buchi.H"
#include "graph.H"
#include "symm.H"
#include "stubborn.H"
#include "tinv.H"
#include "formula.H"
#include "check.H"
#include "path.H"
#include "sweep.H"
#include "cmdline.h"
#include "verbose.h"
#include "reports.H"
#include "Globals.h"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <new>
#include <string>

using std::set_new_handler;
using std::string;

// a structure containing command line parameters
gengetopt_args_info args_info;

/// the invocation string
std::string invocation;


#ifdef WITHFORMULA
extern unsigned int* checkstart;
#endif


int garbagefound = 0;
char* reserve;



#ifdef MAXIMALSTATES
/*!
 \brief abort LoLA if more than MAXIMALSTATES states are processed

        This function makes LoLA abort with exit code 5 if the number of
        currently processed states exceeds the number of states defined as
        MAXIMALSTATES in file userconfig.H. This function is invoked from
        files check.cc, graph.cc, and sweep.cc.

 \param states the number of currently processed states
 \return exit LoLA with exit code 5 if states exceed MAXIMALSTATES
*/
void checkMaximalStates(unsigned int states) {
    if (states >= MAXIMALSTATES) {
        status("%d processed states, %d %s", states, MAXIMALSTATES, _cparameter_("MAXIMALSTATES"));
        abort(5, "maximal number of states reached (%s)", _cparameter_("MAXIMALSTATES"));
    }
}
#endif





inline void garbagecollection() {}

void findcyclingtransitions();
void myown_newhandler() {
    delete reserve;
    message("%s", _cwarning_("new failed"));
    throw overflow();
}


unsigned int Place::cnt = 0;
unsigned int Place::NrSignificant = 0;
void readnet();
void removeisolated();
unsigned int NonEmptyHash;


/// process the command line options using GNU gengetopt
void processCommandLine(int argc, char** argv) {
    // call the cmdline parser, initiate args_info
    if (cmdline_parser(argc, argv, &args_info) != 0) {
        abort(4, "wrong command line option");
    }

    // check if at most one net file is given
    if (args_info.inputs_num > 1) {
        abort(4, "more than one net file given");
    }

    // check if output parameters are given at most once
    if (args_info.Net_given + args_info.net_given > 1) {
        abort(4, "more than one '%s' / '%s' option given", _cparameter_("-n"), _cparameter_("-N"));
    }
    if (args_info.Analysis_given + args_info.analysis_given > 1) {
        abort(4, "more than one '%s' / '%s' option given", _cparameter_("-a"), _cparameter_("-A"));
    }
    if (args_info.State_given + args_info.state_given > 1) {
        abort(4, "more than one '%s' / '%s' option given", _cparameter_("-s"), _cparameter_("-S"));
    }
    if (args_info.Automorphisms_given + args_info.automorphisms_given > 1) {
        abort(4, "more than one '%s' / '%s' option given", _cparameter_("-y"), _cparameter_("-Y"));
    }
    if (args_info.Graph_given + args_info.graph_given + args_info.Marking_given + args_info.marking_given > 1) {
        abort(4, "more than one '%s' / '%s' / '%s' / '%s' option given", _cparameter_("-g"), _cparameter_("-G"), _cparameter_("-m"), _cparameter_("-M"));
    }
    if (args_info.Path_given + args_info.path_given > 1) {
        abort(4, "more than one '%s' / '%s' option given", _cparameter_("-p"), _cparameter_("-P"));
    }


    // process --offspring option
    if (args_info.offspring_given) {
        if (!strcmp(args_info.offspring_arg, "")) {
            abort(4, "option '%s' must not have an empty argument", _cparameter_("--offspring"));
        }
        createUserconfigFile(args_info.offspring_arg);
        exit(EXIT_SUCCESS);
    }


    // set LoLA's flag variables
    Globals::hflg = args_info.userconfig_given;
    Globals::Nflg = args_info.Net_given;
    Globals::nflg = args_info.net_given;
    Globals::Aflg = args_info.Analysis_given;
    Globals::aflg = args_info.analysis_given;
    Globals::Sflg = args_info.State_given;
    Globals::sflg = args_info.state_given;
    Globals::Yflg = args_info.Automorphisms_given;
    Globals::yflg = args_info.automorphisms_given;
    Globals::Pflg = args_info.Path_given;
    Globals::pflg = args_info.path_given;
    Globals::GMflg = args_info.Graph_given + args_info.Marking_given;
    Globals::gmflg = args_info.graph_given + args_info.marking_given;
    Globals::cflg = args_info.Master_given;

    // set graph format
    if (args_info.Graph_given || args_info.graph_given) {
        Globals::graphformat = 'g';
    }
    if (args_info.Marking_given || args_info.marking_given) {
        Globals::graphformat = 'm';
    }

    // determine net file name and its basename
    if (args_info.inputs_num == 1) {
        string temp = string(args_info.inputs[0]);
        Globals::netfile = (char*)realloc(Globals::netfile, temp.size() + 1);
        strcpy(Globals::netfile, temp.c_str());

        temp = temp.substr(0, temp.find_last_of("."));
        Globals::Globals::netbasename = (char*)realloc(Globals::Globals::netbasename, temp.size() + 1);
        strcpy(Globals::Globals::netbasename, temp.c_str());
    } else {
        string temp = "unknown_net";
        Globals::Globals::netbasename = (char*)realloc(Globals::Globals::netbasename, temp.size() + 1);
        strcpy(Globals::Globals::netbasename, temp.c_str());
    }

    // set output filenames for "-n" option
    if (args_info.net_given) {
        if (args_info.net_arg) {
            string temp = string(args_info.net_arg);
            Globals::lownetfile = (char*)realloc(Globals::lownetfile, temp.size() + 1);
            strcpy(Globals::lownetfile, temp.c_str());
            temp = string(args_info.net_arg) + ".pnml";
            Globals::pnmlfile = (char*)realloc(Globals::pnmlfile, temp.size() + 1);
            strcpy(Globals::pnmlfile, temp.c_str());
        } else {
            string temp = string(Globals::Globals::netbasename) + ".llnet";
            Globals::lownetfile = (char*)realloc(Globals::lownetfile, temp.size() + 1);
            strcpy(Globals::lownetfile, temp.c_str());
            temp = string(Globals::Globals::netbasename) + ".pnml";
            Globals::pnmlfile = (char*)realloc(Globals::pnmlfile, temp.size() + 1);
            strcpy(Globals::pnmlfile, temp.c_str());
        }
    }

    // set input filename for "-a" option
    if (args_info.analysis_given) {
        if (args_info.analysis_arg) {
            string temp = string(args_info.analysis_arg);
            Globals::analysefile = (char*)realloc(Globals::analysefile, temp.size() + 1);
            strcpy(Globals::analysefile, temp.c_str());
        } else {
            string temp = string(Globals::Globals::netbasename) + ".task";
            Globals::analysefile = (char*)realloc(Globals::analysefile, temp.size() + 1);
            strcpy(Globals::analysefile, temp.c_str());
        }
    }

    // set output filename for "-p" option
    if (args_info.path_given) {
        if (args_info.path_arg) {
            string temp = string(args_info.path_arg);
            Globals::pathfile = (char*)realloc(Globals::pathfile, temp.size() + 1);
            strcpy(Globals::pathfile, temp.c_str());
        } else {
            string temp = string(Globals::netbasename) + ".path";
            Globals::pathfile = (char*)realloc(Globals::pathfile, temp.size() + 1);
            strcpy(Globals::pathfile, temp.c_str());
        }
    }

    // set output filename for "-s" option
    if (args_info.state_given) {
        if (args_info.state_arg) {
            string temp = string(args_info.state_arg);
            Globals::statefile = (char*)realloc(Globals::statefile, temp.size() + 1);
            strcpy(Globals::statefile, temp.c_str());
        } else {
            string temp = string(Globals::netbasename) + ".state";
            Globals::statefile = (char*)realloc(Globals::statefile, temp.size() + 1);
            strcpy(Globals::statefile, temp.c_str());
        }
    }

    // set output filename for "-y" option
    if (args_info.automorphisms_given) {
        if (args_info.automorphisms_arg) {
            string temp = string(args_info.automorphisms_arg);
            Globals::symmfile = (char*)realloc(Globals::symmfile, temp.size() + 1);
            strcpy(Globals::symmfile, temp.c_str());
        } else {
            string temp = string(Globals::netbasename) + ".symm";
            Globals::symmfile = (char*)realloc(Globals::symmfile, temp.size() + 1);
            strcpy(Globals::symmfile, temp.c_str());
        }
    }


    // set output filename for "-g"/"-m" option
    if (args_info.graph_given || args_info.marking_given) {
        if (args_info.graph_arg) {
            string temp = string(args_info.graph_arg);
            Globals::graphfile = (char*)realloc(Globals::graphfile, temp.size() + 1);
            strcpy(Globals::graphfile, temp.c_str());
        } else {
            if (args_info.marking_arg) {
                string temp = string(args_info.marking_arg);
                Globals::graphfile = (char*)realloc(Globals::graphfile, temp.size() + 1);
                strcpy(Globals::graphfile, temp.c_str());
            } else {
                string temp = string(Globals::netbasename) + ".graph";
                Globals::graphfile = (char*)realloc(Globals::graphfile, temp.size() + 1);
                strcpy(Globals::graphfile, temp.c_str());
            }
        }
    }
    // Initialize reports object for graph output

    if (args_info.graph_given || args_info.marking_given) {
        TheGraphReport = new graphreport_yes(Globals::graphfile);
    } else {
        if (args_info.Graph_given || args_info.Marking_given) {
            TheGraphReport = new graphreport_yes();
        } else {
            TheGraphReport = new graphreport_no();
        }
    }




    // set output filename for "-r" option
    if (args_info.resultFile_given) {
        if (args_info.resultFile_arg) {
            Globals::resultfile = fopen(args_info.resultFile_arg, "w");
        } else {
            Globals::resultfile = fopen((string(Globals::netbasename) + ".result").c_str(), "w");
        }
        if (!Globals::resultfile) {
            abort(4, "could not create result file");
        }
        configurationResult(Globals::resultfile);
    }

    // release memory
    cmdline_parser_free(&args_info);
}



// Fix argv such that inputs like "lola net.lola -a analysisfile.task" are
// allowed even though gengetopt would complain about the space after "-a".
void fix_argv(int& argc, char** &argv) {
    // store invocation in a std::string for meta information in file output
    for (int i = 0; i < argc; ++i) {
        invocation += (i == 0 ? "" : " ") + std::string(argv[i]);
    }

    std::vector<std::string> new_argv;
    std::string temp;
    for (unsigned int i = 0; i < argc; ++i) {
        // collect the whole entry
        unsigned int j = 0;
        while (argv[i][j] != '\0') {
            temp += argv[i][j];
            j++;
        }

        // only start a new argv entry if this one does not end with a "-x"
        // option and the next one does not start with "-"
        if (not(j > 1 and argv[i][j - 2] == '-' and strchr("apsgmnyrh", argv[i][j - 1]) != NULL and(i == argc - 1 or(i < argc - 1 and argv[i + 1][0] != '-')))) {
            new_argv.push_back(temp);
            temp.clear();
        }
    }

    // don't forget the last entry
    if (not temp.empty()) {
        new_argv.push_back(temp);
    }

    // copy result to argv
    for (size_t i = 0; i < new_argv.size(); ++i) {
        strcpy(argv[i], new_argv[i].c_str());
    }

    // also adjust argc
    argc = new_argv.size();
}


int main(int argc, char** argv) {
    // handling "lola --bug" (for debug purposes)
    if (argc == 2 && string(argv[1]) == "--bug") {
        printf("\n\n");
        printf("Please email the following information to %s:\n", PACKAGE_BUGREPORT);
        printf("- tool:                 %s\n", PACKAGE_NAME);
        printf("- version:              %s\n", PACKAGE_VERSION);
        printf("- compilation date:     %s\n", __DATE__);
        printf("- compiler version:     %s\n", __VERSION__);
        printf("- platform:             %s\n", CONFIG_BUILDSYSTEM);
        printf("- config MPI:           %s\n", CONFIG_ENABLEMPI);
#ifdef STANDARDCONFIG
        printf("- chosen userconfig.H:  predefined\n");
#else
        printf("- chosen userconfig.H:  user-defined\n");
#endif
        printf("\n\n");
        exit(EXIT_SUCCESS);
    }

    int i, h;


    // 0. eigenen New-Handler installieren
    try {
        set_new_handler(&myown_newhandler);
        reserve = new char[10000];
        garbagefound = 0;

        // 1. Fileoptionen holen und auswerten
        fix_argv(argc, argv);
        processCommandLine(argc, argv);

        if (Globals::hflg) {
            reportconfiguration();
        }

        // 2. Initialisierung
        NonEmptyHash = 0;
        try {
#ifdef BITHASH
            BitHashTable = new unsigned int [HASHSIZE];
#else
#ifndef SWEEP
            binHashTable = new binDecision * [HASHSIZE];
#endif
#endif
        } catch (overflow) {
            abort(2, "hash table too large (see parameter %s)", _cparameter_("HASHSIZE"));
        }
        for (i = 0; i < HASHSIZE; i++) {
#ifdef BITHASH
            BitHashTable[i] = 0;
#else
#ifndef SWEEP
            binHashTable[i] = NULL;
#endif
#endif
        }
        try {
            PlaceTable = new SymbolTab(65536);
            TransitionTable = new SymbolTab(65536);
        } catch (overflow) {
            abort(2, "not enough memory to read net");
            //write(2,mess,sizeof(mess));
        }
        try {


            // read the Petri net
            readnet();

            // remove isolated nodes from the net
            removeisolated();

#if defined(STATESPACE) && defined(STUBBORN)
            if (F) {
                // throw away temporal operators formula
                assert(((unarytemporalformula*)F));
                assert(((unarytemporalformula*)F)->element);
                F = ((unarytemporalformula*)F)->element;

                // initialze the formulae behind EF
                initialize_statepredicate();
            }
#endif

        } catch (overflow) {
            abort(2, "not enough memory to store net");
        }

        delete PlaceTable;
        delete TransitionTable;
        unsigned int j;

        for (j = 0; j < Globals::Places[0]->cnt; ++j) {
            Globals::Places[j] -> set_hash(rand());
        }
#ifndef STATESPACEw
        message("%d places", Globals::Places[0]->cnt);
        message("%d transitions", Globals::Transitions[0]->cnt);
#endif
        Globals::Places[0]->NrSignificant = Globals::Places[0]->cnt;


#ifdef SYMMETRY
        try {
            //message("calculating symmetries");
            if (SYMMINTEGRATION == 1 || SYMMINTEGRATION == 3) {
                ComputeSymmetries();
            } else {
                ComputePartition();
            }

            for (i = 0; i < Globals::Places[0]->cnt; ++i) {
                Globals::Places[i]-> index = i;
                Globals::CurrentMarking[i] = Globals::Places[i]->initial_marking;
            }

            for (i = 0; i < Globals::Transitions[0]->cnt; ++i) {
                Globals::Transitions[i]->enabled = false;
            }

            for (i = 0; i < Globals::Transitions[0]->cnt; ++i) {
                Globals::Transitions[i]->initialize();
            }

            for (i = 0; i < Globals::Transitions[0]->cnt; ++i) {
                Globals::Transitions[i]->PrevEnabled = (i == 0 ? NULL : Globals::Transitions[i - 1]);
                Globals::Transitions[i]->NextEnabled = (i == Globals::Transitions[0]->cnt - 1 ? NULL : Globals::Transitions[i + 1]);
                Globals::Transitions[i]->enabled = true;
            }

            Globals::Transitions[0]->StartOfEnabledList = Globals::Transitions[0];
            Globals::Transitions[0]->NrEnabled = Globals::Transitions[0]->cnt;

            for (i = 0; i < Globals::Transitions[0]->cnt; ++i) {
                Globals::Transitions[i]->check_enabled();
            }

#ifdef PREDUCTION
            psolve();

            // close significant bit upwards, since we cannot permute places after
            // symmetry calculation
            i = Globals::Places[0]->cnt - 1;
            while (1) {
                if (Globals::Places[i]->significant) {
                    break;
                }
                if (i == 0) {
                    break;
                }
                i--;
            }

            for (h = 0; h < i; ++h) {
                Globals::Places[h]->significant = true;
            }
            Globals::Places[0]->NrSignificant = h + 1;
            // If we did not find a significant place, we set the number to 0 anyway
            // to avoid subsequent problems.
            if (Globals::Places[0]->NrSignificant) {
                Globals::Places[0]->NrSignificant = 1;
            }
            status("removed %d implicit places (%s)", Globals::Places[0]->cnt - Globals::Places[0]->NrSignificant, _cparameter_("PREDUCTION"));
            message("%d significant places", Globals::Places[0]->NrSignificant);
#endif
        } catch (overflow) {
            abort(2, "not enough space to store generating set for symmetries - try again without use of %d", _cparameter_("SYMMETRY"));
        }
#else
#ifdef PREDUCTION
        psolve();
        // sort places according to significance. This must not happen in the presence of
        // symmetry reduction since places have been sorted by discretion of the symmetry
        // calculation algorithm and subsequent procedure depend on that order.
        for (i = 0, h = Globals::Places[0]->cnt;; i++, h--) {
            Place* tmpPlace;
            while (i < Globals::Places[0]->cnt && Globals::Places[i]->significant) {
                i++;
            }
            if (i >= Globals::Places[0]->cnt) {
                break;
            }
            while (h > 0 && !(Globals::Places[h - 1]->significant)) {
                h--;
            }
            if (h <= 0) {
                break;
            }
            if (h <= i) {
                break;
            }
            tmpPlace = Globals::Places[h - 1];
            Globals::Places[h - 1] = Globals::Places[i];
            Globals::Places[i] = tmpPlace;
        }
        Globals::Places[0]->NrSignificant = i;
        status("%d implicit places removed (%s)", Globals::Places[0]->cnt - Globals::Places[0]->NrSignificant, _cparameter_("PREDUCTION"));
        message("%d significant places", Globals::Places[0]->NrSignificant);
#endif
        for (j = 0; j < Globals::Places[0]->cnt; ++j) {
            Globals::Places[j]-> index = j;
            Globals::CurrentMarking[j] = Globals::Places[j]->initial_marking;
        }

        for (j = 0; j < Globals::Transitions[0]->cnt; ++j) {
            Globals::Transitions[j]->enabled = false;
        }

        for (j = 0; j < Globals::Transitions[0]->cnt; ++j) {
            Globals::Transitions[j]->initialize();
        }

        for (j = 0; j < Globals::Transitions[0]->cnt; ++j) {
            Globals::Transitions[j]->PrevEnabled = (j == 0 ? NULL : Globals::Transitions[j - 1]);
            Globals::Transitions[j]->NextEnabled = (j == Globals::Transitions[0]->cnt - 1 ? NULL : Globals::Transitions[j + 1]);
            Globals::Transitions[j]->enabled = true;
        }

        Globals::Transitions[0]->StartOfEnabledList = Globals::Transitions[0];
        Globals::Transitions[0]->NrEnabled = Globals::Transitions[0]->cnt;

        for (j = 0; j < Globals::Transitions[0]->cnt; ++j) {
            Globals::Transitions[j]->check_enabled();
        }
#endif

#if defined(CYCLE) || defined(STRUCT)
        tsolve();
        for (i = 0; i < Globals::Transitions[0]->cnt; ++i) {
            if (Globals::Transitions[i]->cyclic) {
                cout << Globals::Transitions[i]->name << endl;
            }
        }
#endif

        for (unsigned int ii = 0; ii < Globals::Transitions[0]->cnt; ++ii) {
            Globals::Transitions[ii]->check_enabled();
        }
        for (unsigned int ii = 0; ii < Globals::Places[0]->cnt; ++ii) {
            Globals::CurrentMarking[ii] = Globals::Places[ii]->initial_marking;
            Globals::Places[ii]->index = ii;
        }

#ifdef LTLPROP
        print_buchi();
#endif

        // print nets to stdout or file
        if (Globals::Nflg) {
            printnet();
            printpnml();
        }
        if (Globals::nflg) {
            ofstream lownetstream(Globals::lownetfile);
            if (!lownetstream) {
                abort(4, "cannot open net output file '%s' - no output written", _cfilename_(Globals::lownetfile));
            } else {
                printnettofile(lownetstream);
            }
            ofstream pnmlstream(Globals::pnmlfile);
            if (!pnmlstream) {
                abort(4, "cannot open net output file '%s' - no output written", _cfilename_(Globals::pnmlfile));
            } else {
                printpnmltofile(pnmlstream);
            }
        }

#ifdef DISTRIBUTE
        SignificantLength = Globals::Places[0] -> NrSignificant;
        if (!init_storage()) {
            _exit(6);
        }
#endif

        for (j = 0, Globals::BitVectorSize = 0; j < Globals::Places[0]->NrSignificant; ++j) {
            Globals::BitVectorSize += Globals::Places[j]->nrbits;
        }
        status("bit vector size: %d bit", Globals::BitVectorSize);

//#ifdef WITHFORMULA
//        if (F) {
//            checkstart = new unsigned int[F->card+5];
//            for (i = 0; i < F->card; ++i) {
//                checkstart[i] = 0;
//            }
//        }
//#endif


        try {

#ifdef NONE
            return EXIT_SUCCESS;
#endif

#ifdef STATESPACE
            return compute_scc();
#endif

#ifdef SWEEP
            return 1 - sweep();
//            exit(888);
#endif

#ifdef MODELCHECKING
#ifdef STUBBORN
            sortscapegoats();
#endif
            return modelcheck();
#endif

#if defined(LIVEPROP) && defined(TWOPHASE)
            return liveproperty();
#endif

#ifdef REVERSIBILITY
            return reversibility();
#endif

#if defined(HOME) && defined(TWOPHASE)
            return home();
#endif

#ifdef FINDPATH
            find_path();
            return 0;
#endif


#ifndef MODELCHECKING
            return 1 - GRAPH();
#endif

        } catch (overflow) {
            abort(2, "memory exhausted");
        }
    } catch (overflow) {
        abort(2, "memory exhausted");
    }
}






/*!
 \brief remove isolated places from the net

 \note The places are actually copied to the end of the array "Places" and
       made unaccessible by decreasing the place count.
*/
void removeisolated() {
#ifndef STATESPACE
    unsigned int isolatedPlacesRemoved = 0;
    unsigned int i = 0;
    while (i < Globals::Places[0]->cnt) {
        if (Globals::Places[i]->references == 0) { // Place isolated
            Place* p = Globals::Places[Globals::Places[0]->cnt - 1];
            Globals::Places[Globals::Places[0]->cnt - 1] = Globals::Places[i];
            Globals::Places[i] = p;
            --(Globals::Places[0]->cnt);
            ++isolatedPlacesRemoved;
        } else {
            ++i;
        }
    }
    status("%d isolated places removed", isolatedPlacesRemoved);
#endif
    for (unsigned int i = 0; i < Globals::Transitions[0]->cnt; ++i) {
        Globals::Transitions[i]->nr = i;
    }
}





#ifdef CYCLE

void findcyclingtransitions() {
    // depth first search for cycles in net
    // use Node::parent pointer as stack, pos[0] = current succ, pos[1] for status info
    // 0 - never seen, 1 - on stack, 2 - explored, x+4 - already selected as cyclic
    // x+8 - transition

    Node* currentnode, * newnode, * maxnode, * searchnode;
    unsigned int maxfan;
    Transition* currenttransition;
    bool IsTransition ;

    // init
    currentnode = Globals::Transitions[0];
    IsTransition = true;
    currentnode -> pos[0] = 0 ;
    currentnode -> parent = NULL ; // bottom stack element
    currentnode -> pos[1] = 9 ; // transition, on stack, not selected

    // loop

    while (currentnode) {
        // is there another successor ?
        newnode = NULL;
        if (currentnode->pos[1] < 8) {
            // successor of place
            for (; currentnode->pos[0] < currentnode -> NrOfLeaving; currentnode -> pos[0]++) {
                // Successor = transition that consumes more than it produces
                Transition* t;
                unsigned int i;
                t = (Transition*)(currentnode->LeavingArcs[currentnode->pos[0]]->Destination);
                for (i = 0; t -> DecrPlaces[i]; i++) {
                    if (t -> DecrPlaces[i] == currentnode -> pos[0]) {
                        break;
                    }
                }
                if (t -> DecrPlaces[i]) {
                    newnode = t;
                    IsTransition = true;
                    break;
                }
            }
        } else {
            // successor of transition
            currenttransition = (Transition*) currentnode;
            if (currenttransition -> IncrPlaces[0]) {
                // no sink transition
                newnode = Globals::Transitions[currenttransition -> IncrPlaces[currenttransition->pos[0]]];
                IsTransition = false;
            } else {
                // sink transition -> successors are source transitions
                for (; currenttransition->pos[0] < Globals::Transitions[0]->cnt;
                        currenttransition->pos[0] ++) {
                    if (!(Globals::Transitions[currenttransition->pos[0]]->DecrPlaces[0])) {
                        break;
                    }
                }
                if (currenttransition->pos[0] < Globals::Transitions[0]->cnt) {
                    newnode = Globals::Transitions[currenttransition->pos[0]];
                    IsTransition = true;
                }
            }
        }
        // now successor is determined
        if (newnode) {
            // there is another successor
            switch (newnode->pos[1] % 4) {
                case 0: // newnode never seen
                    // mark node type and stack type "on stack"
                    newnode -> pos[1] = 1 + (IsTransition ? 8 : 0);
                    newnode -> parent = currentnode;
                    newnode -> pos[0] = 0;
                    currentnode = newnode; //explore newnode
                    break;
                case 1: // newnode on stack
                    maxfan = 0;
                    for (searchnode = currentnode; searchnode != newnode; searchnode = searchnode -> parent) {
                        if (searchnode -> pos[1] < 8) {
                            continue;    // jump over places
                        }
                        if (searchnode -> pos[1] > 11) {
                            //already selected
                            maxnode = searchnode;
                            break;
                        }
                        if (searchnode->NrOfLeaving + searchnode -> NrOfArriving > maxfan) {
                            maxfan = searchnode->NrOfLeaving + searchnode -> NrOfArriving;
                            maxnode = searchnode;
                        }
                    }
                    if (maxnode->pos[1] < 12) {
                        maxnode -> pos[1] += 4; // mark selected
                        ((Transition*) maxnode) -> cyclic = true;
                    }
                    currentnode -> pos[0] ++;
                    break;
                case 2: // newnode already explored
                    currentnode -> pos[0] ++; // check next succ
                    break;
            }

        } else {
            //currentnode does not have further successors
            currentnode -> pos[1]++; // mark as explored;
            currentnode = currentnode -> parent;
            if (currentnode) {
                currentnode -> pos[0]++;
            }
        }
    }
    for (int j = 0; j < Globals::Transitions[0]->cnt; j++) {
        if (Globals::Transitions[j]-> cyclic) {
            cerr << "\n" << Globals::Transitions[j] -> name;
        }
    }
}

#endif
