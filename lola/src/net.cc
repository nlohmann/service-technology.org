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

#include <fstream>
#include <iostream>
#include <cctype>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <new>
#include <string>

using std::set_new_handler;
using std::string;


unsigned int BitVectorSize = 0;
Place ** Places;
Place * CheckPlace = NULL;
Transition * CheckTransition = NULL;
Transition ** Transitions;
unsigned int  * CurrentMarking;
unsigned int Arc::cnt = 0;
unsigned int Place::hash_value = 0;
unsigned int Transition::cnt = 0;

#ifdef STUBBORN
unsigned int Transition::NrStubborn = 0;
Transition * Transition::TarjanStack = NULL;
Transition * Transition::CallStack = NULL;
#endif

Transition * LastAttractor; // Last transition in static attractor sets
unsigned int Transition::NrEnabled = 0;
Transition * Transition::StartOfEnabledList = NULL;

#ifdef EXTENDED
Transition * Transition::StartOfIgnoredList = NULL;
#endif

#ifdef STUBBORN
Transition * Transition::StartOfStubbornList = NULL;
Transition * Transition::EndOfStubbornList = NULL;
#endif

#ifdef WITHFORMULA
extern unsigned int * checkstart;
#endif

char * lownetfile = NULL;
char * pnmlfile = NULL;
char * netfile = NULL;
char * analysefile = NULL;
char * graphfile = NULL;
char * pathfile = NULL;
char * statefile = NULL;
char * symmfile = NULL;
char * netbasename = NULL;
FILE *resultfile = NULL;

bool hflg, Nflg, nflg, Aflg, Sflg, Yflg, Pflg,GMflg, aflg, sflg, yflg,pflg,gmflg, cflg = false;
char graphformat = '\0';

int garbagefound = 0;
char * reserve;



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
    fprintf(stderr, "\nlola: maximal number of states reached\n");
    fprintf(stderr, "      %d processed states, %d MAXIMALSTATES\n", states, MAXIMALSTATES);
    _exit(5);
  }
}
#endif





inline void garbagecollection() {}

void findcyclingtransitions();
void myown_newhandler()
{
  delete reserve;
  fprintf(stderr, "\nlola: new failed\n");
  throw overflow();
}


unsigned int Place::cnt = 0;
unsigned int Place::NrSignificant = 0;
void readnet();
void removeisolated();
unsigned int NonEmptyHash;


/// process the command line options using GNU gengetopt
void processCommandLine(int argc, char **argv) {
  // a structure containing command line parameters
  gengetopt_args_info args_info;

  // call the cmdline parser, initiate args_info
  if (cmdline_parser (argc, argv, &args_info) != 0) {
    fprintf(stderr, "      see 'lola --help' for more information\n");
    exit(4);
  }

  // check if at most one net file is given
  if (args_info.inputs_num > 1) {
    fprintf(stderr, "lola: more than one net file given\n");
    fprintf(stderr, "      see 'lola --help' for more information\n");
    exit(4);
  }

  // check if output parameters are given at most once
  if (args_info.Net_given + args_info.net_given > 1) {
    fprintf(stderr, "lola: more than one '-n' / '-N' option given\n");
    fprintf(stderr, "      see 'lola --help' for more information\n");
    exit(4);
  }
  if (args_info.Analysis_given + args_info.analysis_given > 1) {
    fprintf(stderr, "lola: more than one '-a' / '-A' option given\n");
    fprintf(stderr, "      see 'lola --help' for more information\n");
    exit(4);
  }
  if (args_info.State_given + args_info.state_given > 1) {
    fprintf(stderr, "lola: more than one '-s' / '-S' option given\n");
    fprintf(stderr, "      see 'lola --help' for more information\n");
    exit(4);
  }
  if (args_info.Automorphisms_given + args_info.automorphisms_given > 1) {
    fprintf(stderr, "lola: more than one '-y' / '-Y' option given\n");
    fprintf(stderr, "      see 'lola --help' for more information\n");
    exit(4);
  }
  if (args_info.Graph_given + args_info.graph_given + args_info.Marking_given + args_info.marking_given > 1) {
    fprintf(stderr, "lola: more than one '-g' / '-G' / '-m' / '-M' option given\n");
    fprintf(stderr, "      see 'lola --help' for more information\n");
    exit(4);
  }
  if (args_info.Path_given + args_info.path_given > 1) {
    fprintf(stderr, "lola: more than one '-p' / '-P' option given\n");
    fprintf(stderr, "      see 'lola --help' for more information\n");
    exit(4);
  }


  // process --offspring option
  if (args_info.offspring_given) {
    if (!strcmp(args_info.offspring_arg, "")) {
      fprintf(stderr, "lola: option '--offspring' must not have an empty argument\n");
      fprintf(stderr, "      see 'lola --help' for more information\n");
      exit(4);
    }
    createUserconfigFile(args_info.offspring_arg);
    exit(EXIT_SUCCESS);
  }


  // set LoLA's flag variables
  hflg = args_info.userconfig_given;
  Nflg = args_info.Net_given;
  nflg = args_info.net_given;
  Aflg = args_info.Analysis_given;
  aflg = args_info.analysis_given;
  Sflg = args_info.State_given;
  sflg = args_info.state_given;
  Yflg = args_info.Automorphisms_given;
  yflg = args_info.automorphisms_given;
  Pflg = args_info.Path_given;
  pflg = args_info.path_given;
  GMflg = args_info.Graph_given + args_info.Marking_given;
  gmflg = args_info.graph_given + args_info.marking_given;
  cflg = args_info.Master_given;

  // set graph format
  if (args_info.Graph_given || args_info.graph_given) {
    graphformat = 'g';
  }
  if (args_info.Marking_given || args_info.marking_given) {
    graphformat = 'm';
  }

  // determine net file name and its basename
  if (args_info.inputs_num == 1) {
    string temp = string(args_info.inputs[0]);
    netfile = (char*)realloc(netfile, temp.size()+1);
    strcpy(netfile, temp.c_str());

    temp = temp.substr(0, temp.find_last_of("."));
    netbasename = (char*)realloc(netbasename, temp.size()+1);
    strcpy(netbasename, temp.c_str());
  } else {
    string temp = "unknown_net";
    netbasename = (char*)realloc(netbasename, temp.size()+1);
    strcpy(netbasename, temp.c_str());
  }

  // set output filenames for "-n" option
  if (args_info.net_given) {
    if (args_info.net_arg) {
      string temp = string(args_info.net_arg);
      lownetfile = (char*)realloc(lownetfile, temp.size()+1);
      strcpy(lownetfile, temp.c_str());
      temp = string(args_info.net_arg) + ".pnml";
      pnmlfile = (char*)realloc(pnmlfile, temp.size()+1);
      strcpy(pnmlfile, temp.c_str());
    } else {
      string temp = string(netbasename) + ".llnet";
      lownetfile = (char*)realloc(lownetfile, temp.size()+1);
      strcpy(lownetfile, temp.c_str());
      temp = string(netbasename) + ".pnml";
      pnmlfile = (char*)realloc(pnmlfile, temp.size()+1);
      strcpy(pnmlfile, temp.c_str());
    }
  }

  // set input filename for "-a" option
  if (args_info.analysis_given) {
    if (args_info.analysis_arg) {
      string temp = string(args_info.analysis_arg);
      analysefile = (char*)realloc(analysefile, temp.size()+1);
      strcpy(analysefile, temp.c_str());
    } else {
      string temp = string(netbasename) + ".task";
      analysefile = (char*)realloc(analysefile, temp.size()+1);
      strcpy(analysefile, temp.c_str());
    }
  }

  // set output filename for "-p" option
  if (args_info.path_given) {
    if (args_info.path_arg) {
      string temp = string(args_info.path_arg);
      pathfile = (char*)realloc(pathfile, temp.size()+1);
      strcpy(pathfile, temp.c_str());
    } else {
      string temp = string(netbasename) + ".path";
      pathfile = (char*)realloc(pathfile, temp.size()+1);
      strcpy(pathfile, temp.c_str());
    }
  }

  // set output filename for "-s" option
  if (args_info.state_given) {
    if (args_info.state_arg) {
      string temp = string(args_info.state_arg);
      statefile = (char*)realloc(statefile, temp.size()+1);
      strcpy(statefile, temp.c_str());
    } else {
      string temp = string(netbasename) + ".state";
      statefile = (char*)realloc(statefile, temp.size()+1);
      strcpy(statefile, temp.c_str());
    }
  }

  // set output filename for "-y" option
  if (args_info.automorphisms_given) {
    if (args_info.automorphisms_arg) {
      string temp = string(args_info.automorphisms_arg);
      symmfile = (char*)realloc(symmfile, temp.size()+1);
      strcpy(symmfile, temp.c_str());
    } else {
      string temp = string(netbasename) + ".symm";
      symmfile = (char*)realloc(symmfile, temp.size()+1);
      strcpy(symmfile, temp.c_str());
    }
  }

  // set output filename for "-g"/"-m" option
  if (args_info.graph_given || args_info.marking_given) {
    if (args_info.graph_arg) {
      string temp = string(args_info.graph_arg);
      graphfile = (char*)realloc(graphfile, temp.size()+1);
      strcpy(graphfile, temp.c_str());
    } else {
      if (args_info.marking_arg) {
        string temp = string(args_info.marking_arg);
        graphfile = (char*)realloc(graphfile, temp.size()+1);
        strcpy(graphfile, temp.c_str());
      } else {
        string temp = string(netbasename) + ".graph";
        graphfile = (char*)realloc(graphfile, temp.size()+1);
        strcpy(graphfile, temp.c_str());
      }
    }
  }

  // set output filename for "-r" option
  if (args_info.resultFile_given) {
    if (args_info.resultFile_arg) {
      resultfile = fopen(args_info.resultFile_arg, "w");
    } else {
      resultfile = fopen((string(netbasename) + ".result").c_str(), "w");
    }
    if(!resultfile) {
      fprintf(stderr, "lola: could not create result file\n");
      exit(4);
    }
    configurationResult(resultfile);
  }

  // release memory
  cmdline_parser_free (&args_info);
}


int main(int argc, char ** argv){
  // handling "lola --bug" (for debug purposes)
  if (argc == 2 && string(argv[1]) == "--bug") {
    printf("\n\n");
    printf("Please email the following information to %s:\n", PACKAGE_BUGREPORT);
    printf("- tool:                 %s\n", PACKAGE_NAME);
    printf("- version:              %s\n", PACKAGE_VERSION);
    printf("- compilation date:     %s\n", __DATE__);
    printf("- compiler version:     %s\n", __VERSION__);
    printf("- platform:             %s\n", BUILDSYSTEM);
    printf("- config MPI:           %s\n", CONFIG_ENABLEMPI);
#ifdef STANDARDCONFIG
    printf("- chosen userconfig.H:  predefined\n");
#else
    printf("- chosen userconfig.H:  user-defined\n");
#endif
    printf("\n\n");
    exit(EXIT_SUCCESS);
  }

  int i,h;


  // 0. eigenen New-Handler installieren
  try {
    set_new_handler(&myown_newhandler);
    reserve = new char[10000];
    garbagefound = 0;

    // 1. Fileoptionen holen und auswerten
    processCommandLine(argc, argv);

    if (hflg)
      reportconfiguration();

    // 2. Initialisierung
    NonEmptyHash = 0;
    try {
#ifdef DISTRIBUTE
      if(!init_communication()) _exit(6);
      rapport("initializing");
#else
#ifdef BITHASH
      BitHashTable = new unsigned int [ HASHSIZE];
#else
#ifndef SWEEP
      binHashTable = new binDecision * [HASHSIZE];
#endif
#endif
#endif
    }
    catch(overflow)
    {
      fprintf(stderr, "lola: hash table too large\n");
#ifdef DISTRIBUTE
      end_communication();
#endif
      _exit(2);
    }
#ifndef DISTRIBUTE
    for(i=0;i < HASHSIZE;i++)
    {
#ifdef BITHASH
      BitHashTable[i] = 0;
#else
#ifndef SWEEP
      binHashTable[i] = NULL;
#endif
#endif
    }
#endif
    try{
      PlaceTable = new SymbolTab(65536);
      TransitionTable = new SymbolTab(65536);
    }
    catch(overflow)
    {
    //write(2,mess,sizeof(mess));
      fprintf(stderr, "lola: not enough space to read net\n");
#ifdef DISTRIBUTE
      end_communication();
#endif
      _exit(2);
    }
    try{

#ifdef DISTRIBUTE
      rapport("reading net");
#endif

      // read the Petri net
      readnet();

      // remove isolated nodes from the net
      removeisolated();

#ifdef DISTRIBUTE
      StateLength = Places[0]->cnt;
      MaxComponentValue = CAPACITY;
#endif
    }
    catch(overflow) {
      fprintf(stderr, "lola: not enough space to store net\n");
#ifdef DISTRIBUTE
      end_communication();
#endif
      _exit(2);
    }

    delete PlaceTable;
    delete TransitionTable;
    unsigned int j;

    for(j=0; j < Places[0]->cnt; ++j) {
      Places[j] -> set_hash(rand());
    }
#ifndef STATESPACE
    cout << Places[0]->cnt << " Places\n";
    cout << Transitions[0]->cnt << " Transitions\n";
#endif
    Places[0]->NrSignificant = Places[0]->cnt;


#ifdef SYMMETRY
    try{
      if(SYMMINTEGRATION == 1 || SYMMINTEGRATION == 3) {
#ifdef DISTRIBUTE
        rapport("computing symmetries");
#endif
        ComputeSymmetries();
      } else {
#ifdef DISTRIBUTE
        rapport("computing partition w.r.t. symmetries");
#endif
        ComputePartition();
      }

      for(i=0; i<Places[0]->cnt; ++i) {
        Places[i]-> index = i;
        CurrentMarking[i] = Places[i]->initial_marking;
      }

      for(i=0; i<Transitions[0]->cnt; ++i) {
        Transitions[i]->enabled = false;
      }

      for(i=0; i<Transitions[0]->cnt; ++i) {
        Transitions[i]->initialize();
      }

      for(i=0; i<Transitions[0]->cnt; ++i) {
        Transitions[i]->PrevEnabled = (i == 0 ? NULL : Transitions[i-1]);
        Transitions[i]->NextEnabled = (i == Transitions[0]->cnt - 1 ? NULL : Transitions[i+1]);
        Transitions[i]->enabled = true;
      }
      
      Transitions[0]->StartOfEnabledList = Transitions[0];
      Transitions[0]->NrEnabled = Transitions[0]->cnt;
      
      for(i=0; i<Transitions[0]->cnt; ++i) {
        Transitions[i]->check_enabled();
      }

#ifdef PREDUCTION
#ifdef DISTRIBUTE
      rapport("investigating place invariants");
#endif
      psolve();

      // close significant bit upwards, since we cannot permute places after
      // symmetry calculation
      i = Places[0]->cnt - 1;
      while(1) {
        if(Places[i]->significant) break;
        if(i == 0) break;
        i--;
      }

      for(h=0; h<i; ++h) {
        Places[h]->significant = true;
      }
      Places[0]->NrSignificant = h+1;
      // If we did not find a significant place, we set the number to 0 anyway
      // to avoid subsequent problems.
      if (Places[0]->NrSignificant) {
        Places[0]->NrSignificant = 1;
      }
      cout << "\n" << Places[0]->NrSignificant << " significant places\n";
#endif
    }
    catch(overflow) {
      fprintf(stderr, "lola: not enough space to store generating set for symmetries!\n");
      fprintf(stderr, "      try again without use of symmetries!\n");
#ifdef DISTRIBUTE
      end_communication();
#endif
      _exit(2);
    }
#else
#ifdef PREDUCTION
#ifdef DISTRIBUTE
    rapport("investigating place invariants");
#endif
    psolve();
    // sort places according to significance. This must not happen in the presence of
    // symmetry reduction since places have been sorted by discretion of the symmetry
    // calculation algorithm and subsequent procedure depend on that order.
    for(i=0, h = Places[0]->cnt;;i++,h--) {
      Place * tmpPlace;
      while(i < Places[0]->cnt && Places[i]->significant) i++;
      if(i >= Places[0]->cnt) break;
      while(h > 0 && ! (Places[h-1]->significant)) h--;
      if(h <= 0) break;
      if(h <= i) break;
      tmpPlace = Places[h-1];
      Places[h-1] = Places[i];
      Places[i] = tmpPlace;
    }
    Places[0]->NrSignificant = i;
    cout << "\n" << Places[0]->NrSignificant << " significant places\n";
#endif
    for(j=0; j<Places[0]->cnt; ++j) {
      Places[j]-> index = j;
      CurrentMarking[j] = Places[j]->initial_marking;
    }

    for(j=0; j<Transitions[0]->cnt; ++j) {
      Transitions[j]->enabled = false;
    }

    for(j=0; j<Transitions[0]->cnt; ++j) {
      Transitions[j]->initialize();
    }

    for(j=0; j<Transitions[0]->cnt; ++j) {
      Transitions[j]->PrevEnabled = (j == 0 ? NULL : Transitions[j-1]);
      Transitions[j]->NextEnabled = (j == Transitions[0]->cnt - 1 ? NULL : Transitions[j+1]);
      Transitions[j]->enabled = true;
    }

    Transitions[0]->StartOfEnabledList = Transitions[0];
    Transitions[0]->NrEnabled = Transitions[0]->cnt;

    for(j=0; j<Transitions[0]->cnt; ++j) {
      Transitions[j]->check_enabled();
    }
#endif

#if defined(CYCLE) || defined(STRUCT)
    tsolve();
    for(i=0; i<Transitions[0]->cnt; ++i) {
      if(Transitions[i]->cyclic) {
        cout << Transitions[i]->name << endl;
      }
    }
#endif

    unsigned int ii;
    for(ii=0; ii < Transitions[0]->cnt; ++ii) {
      Transitions[ii]->check_enabled();
    }
    for(ii=0; ii < Places[0]->cnt; ++ii) {
      CurrentMarking[ii] = Places[ii]->initial_marking;
      Places[ii]->index = ii;
    }

#ifdef LTLPROP
    print_buchi();
#endif

    // print nets to stdout or file
    if(Nflg) {
      printnet();
      printpnml();
    }
    if(nflg) {
      ofstream lownetstream(lownetfile);
      if(!lownetstream) {
        fprintf(stderr, "lola: cannot open net output file '%s'\n", lownetfile);
        fprintf(stderr, "      no output written\n");
      } else {
        printnettofile(lownetstream);
      }
      ofstream pnmlstream(pnmlfile);
      if(!pnmlstream) {
        fprintf(stderr, "lola: cannot open net output file '%s'\n", pnmlfile);
        fprintf(stderr, "      no output written\n");
      } else {
        printpnmltofile(pnmlstream);
      }
    }

#ifdef DISTRIBUTE
    SignificantLength = Places[0] -> NrSignificant;
    if(!init_storage()) _exit(6);
#endif

    for(j=0,BitVectorSize = 0; j<Places[0]->NrSignificant; ++j) {
      BitVectorSize += Places[j]->nrbits;
    }

#ifdef WITHFORMULA
    if(F) {
      checkstart = new unsigned int[F->card+5];
      for(i=0;i<F->card; ++i) {
        checkstart[i] = 0;
      }
    }
#endif


    try{
//siphontrapproperty();

#ifdef NONE
      return EXIT_SUCCESS;
#endif

#ifdef STATESPACE
      return compute_scc();
#endif

#ifdef SWEEP
      return 1 - sweep();
      exit(888);
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

#ifdef DISTRIBUTE
      int vvv;
      switch(MyRole)
      {
        case masterT:   rapport("starting breadth first search");
        vvv = breadth_first();
        end_communication();
        return 1 - vvv;
        case clientT:   rapport("starting depth first search");
        vvv = depth_first();
        end_communication();
        return 1 - vvv;
        case monitorT: return 0;
        case dispatcherT: return 0;
      }
#endif

#ifndef MODELCHECKING
      return 1 - GRAPH();
#endif

    }
    catch(overflow)
    {
      fprintf(stderr, "lola: memory exhausted\n");
      _exit(2);
    }
  }
  catch(overflow)
  {
    fprintf(stderr, "lola: memory exhausted\n");
    _exit(2);
  }
}






/*!
 \brief remove isolated places from the net

 \note The places are actually copied to the end of the array "Places" and
       made unaccessible by decreasing the place count.
*/
void removeisolated() {
  unsigned int i=0;
#ifndef STATESPACE
  while(i<Places[0]->cnt) {
    if(Places[i]->references == 0) { // Place isolated
      Place *p = Places[Places[0]->cnt - 1];
      Places[Places[0]->cnt - 1] = Places[i];
      Places[i] = p;
      --(Places[0]->cnt);
    }
    else {
      ++i;
    }
  }
#endif
  for(i=0; i<Transitions[0]->cnt; ++i) {
    Transitions[i]->nr = i;
  }
}





#ifdef CYCLE

void findcyclingtransitions() {
	// depth first search for cycles in net
 	// use Node::parent pointer as stack, pos[0] = current succ, pos[1] for status info
	// 0 - never seen, 1 - on stack, 2 - explored, x+4 - already selected as cyclic
 	// x+8 - transition

Node * currentnode, * newnode, * maxnode, * searchnode;
unsigned int maxfan;
Transition * currenttransition;
bool IsTransition ;

	// init
	currentnode = Transitions[0];
	IsTransition = true;
	currentnode -> pos[0] = 0 ;
	currentnode -> parent = NULL ; // bottom stack element
	currentnode -> pos[1] = 9 ; // transition, on stack, not selected

	// loop

	while(currentnode)
	{
		// is there another successor ?
		newnode = NULL;
		if(currentnode->pos[1] < 8)
		{
			// successor of place
			for(;currentnode->pos[0] < currentnode -> NrOfLeaving;currentnode -> pos[0]++)
			{
				// Successor = transition that consumes more than it produces
				Transition * t;
				unsigned int i;
				t = (Transition *) (currentnode->LeavingArcs[currentnode->pos[0]]->Destination);
				for(i=0;t -> DecrPlaces[i];i++)
				{
					if(t -> DecrPlaces[i] == currentnode -> pos[0]) break;
				}
				if(t -> DecrPlaces[i])
				{
					newnode = t;
					IsTransition = true;
					break;
				}
			}
		}
		else
		{
			// successor of transition
			currenttransition = (Transition *) currentnode;
			if(currenttransition -> IncrPlaces[0])
			{
				// no sink transition
				newnode = Transitions[currenttransition -> IncrPlaces[currenttransition->pos[0]]];
				IsTransition = false;
			}
			else
			{
				// sink transition -> successors are source transitions
				for(;currenttransition->pos[0] < Transitions[0]->cnt;
                                     currenttransition->pos[0] ++)
				{
					if(!(Transitions[currenttransition->pos[0]]->DecrPlaces[0]))
					{
						break;
					}
				}
				if(currenttransition->pos[0] < Transitions[0]->cnt)
				{
					newnode = Transitions[currenttransition->pos[0]];
					IsTransition = true;
				}
			}
		}
		// now successor is determined
		if(newnode)
		{
			// there is another successor
			switch(newnode->pos[1] % 4)
			{
			case 0: // newnode never seen
					// mark node type and stack type "on stack"
				newnode -> pos[1] = 1 + (IsTransition ? 8 : 0);
				newnode -> parent = currentnode;
				newnode -> pos[0] = 0;
				currentnode = newnode; //explore newnode
			break;
			case 1: // newnode on stack
				maxfan = 0;
				for(searchnode = currentnode;searchnode !=newnode;searchnode = searchnode -> parent)
				{
					if(searchnode -> pos[1] < 8) continue; // jump over places
					if(searchnode -> pos[1] > 11)
					{
						//already selected
						maxnode = searchnode;
						break;
					}
					if(searchnode->NrOfLeaving + searchnode -> NrOfArriving > maxfan)
					{
						maxfan = searchnode->NrOfLeaving + searchnode -> NrOfArriving;
						maxnode = searchnode;
					}
				}
				if(maxnode->pos[1] < 12)
				{
					maxnode -> pos[1] += 4; // mark selected
					((Transition *) maxnode) -> cyclic = true;
				}
				currentnode -> pos[0] ++;
			break;
			case 2: // newnode already explored
				currentnode -> pos[0] ++; // check next succ
			break;
			}

		}
		else
		{
			//currentnode does not have further successors
			currentnode -> pos[1]++; // mark as explored;
			currentnode = currentnode -> parent;
			if(currentnode) currentnode -> pos[0]++;
		}
	}
	for(int j = 0; j < Transitions[0]->cnt;j++)
	{
		if(Transitions[j]-> cyclic)
		{
			cerr << "\n" << Transitions[j] -> name;
		}
	}
}

#endif
