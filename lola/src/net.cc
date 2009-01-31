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

#include <fstream>
#include <iostream>
#include <ctype.h>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <new>

using std::set_new_handler;


unsigned int BitVectorSize = 0;
Place ** Places;
Place * CheckPlace = (Place *) 0;
Transition * CheckTransition = (Transition *) 0;
Transition ** Transitions;
unsigned int  * CurrentMarking;
unsigned int Arc::cnt = 0;
unsigned int Place::hash_value = 0;
unsigned int Transition::cnt = 0;
#ifdef STUBBORN
unsigned int Transition::NrStubborn = 0;
Transition * Transition::TarjanStack = (Transition *) 0;
Transition * Transition::CallStack = (Transition *) 0;
#endif
Transition * LastAttractor; // Last transition in static attractor sets
unsigned int Transition::NrEnabled = 0;
Transition * Transition::StartOfEnabledList = (Transition *) 0;
#ifdef EXTENDED
Transition * Transition::StartOfIgnoredList = (Transition *) 0;
#endif
#ifdef STUBBORN
Transition * Transition::StartOfStubbornList = (Transition *) 0;
Transition * Transition::EndOfStubbornList = (Transition *) 0;
#endif
#ifdef WITHFORMULA
extern unsigned int * checkstart;
#endif

  char * lownetfile;
  char * pnmlfile;
  char * netfile;
  char * analysefile;
  char * graphfile;
  char * pathfile;
  char * statefile;
  char * symmfile;
  char * netbasename;

  bool hflg, Nflg, nflg, Aflg, Sflg, Yflg, Pflg,GMflg, aflg, sflg, yflg,pflg,gmflg, cflg;
  char graphformat;

int garbagefound = 0;
char * reserve;
inline void garbagecollection()
{
}

void findcyclingtransitions();
void myown_newhandler()
{
		delete reserve;
                cerr << "new failed\n";
		throw overflow();
}


unsigned int Place::cnt = 0;
unsigned int Place::NrSignificant = 0;
void readnet();
void removeisolated();
unsigned int NonEmptyHash;
int main(int argc, char ** argv){
  // handling "lola --bug" (for debug purposes)
  if (argc == 2 && std::string(argv[1]) == "--bug") {
    printf("\n\n");
    printf("Please email the following information to %s:\n", PACKAGE_BUGREPORT);
    printf("- tool:               %s\n", PACKAGE_NAME);
    printf("- version:            %s\n", PACKAGE_VERSION);
    printf("- compilation date:   %s\n", __DATE__);
    printf("- compiler version:   %s\n", __VERSION__);
    printf("- platform:           %s\n", BUILDSYSTEM);
    printf("- config ASSERT:      %s\n", CONFIG_ENABLEASSERT);
    printf("- config UNIVERSAL:   %s\n", CONFIG_ENABLEUNIVERSAL);
    printf("- config ENABLE64BIT: %s\n", CONFIG_ENABLE64BIT);
    printf("- config WIN32:       %s\n", CONFIG_ENABLEWIN32);
    printf("- config MPI:         %s\n", CONFIG_ENABLEMPI);
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
  
  // Options:
  // File without option: Input net (stdin if not specified)
  // -h output configuration
  // -n [file] output file for LL net
  // -c Master in distributed search
  // -a [file] detailed spec. of analysis task (target marking/place/transtion/
  //                                             /formula)
  // -s [file] output file for witness state
  // -y [file] output file for symmetry group generating set
  // -p [file] output file for witness path
  // -[gm] [file] output file for graph -g only state number and transitions
  //									 -m markings and transitions
  // if [file] is omitted, the following standard names are used:
  // (<base> refers to the net file name without extension)
  // -a: <base>.task
  // -s: <base>.state
  // -y: <base>.symm
  // -p: <base>.path
  // -[gm]: <base>.graph
  // Using corresponding capital letters refers to output to the
  // standard output stream. Output to stdout will also happen if
  // an output file cannot be opened.
  // if no net file is specified, net is read from stdin, and
  // <base> is set to "unknown_net"

  hflg = Nflg = nflg = Aflg = Sflg = Yflg = Pflg = GMflg = aflg = sflg = yflg = pflg = gmflg = cflg = false;
  lownetfile = pnmlfile = netfile = analysefile = graphfile = pathfile = statefile = symmfile = 
  netbasename = (char *) 0;
  graphformat = '\0';
  for(i = 1; i < argc; i++)
  {
	char * file;
	file = (char *) 0;
	if(argv[i][0] == '-')
	{
		// option
		switch(argv[i][1])
		{
		case 'n': if(nflg || Nflg) 
				  {
					cerr << "multiple use of options -n/-N not allowed\n";
					return(4);
				  }
				  nflg = true;
				  if(argv[i][2])
				  {
					file = argv[i] + 2;
				  }
				  else
				  {
					if((i+1 < argc) && (argv[i+1][0] != '-'))
					{	
						i++;
						file = argv[i];
					}
				  }
				  if(file)
				  {
					lownetfile = new char [strlen(file) + 2];
					strcpy(lownetfile,file);
					pnmlfile = new char [strlen(file) + 20];
					strcpy(pnmlfile,file);
					strcpy(pnmlfile + strlen(pnmlfile),".pnml");
				  }
				  break;
		case 'N': if(nflg || Nflg)
				  {
					cerr << "multiple use of options -n/-N not allowed\n";
					return(4);
				  }
				  Nflg = true;
				  break;
		case 'c':         cflg = true;
				  break;
		case 'h':         hflg = true;
				  break;
		case 'a': if(aflg || Aflg) 
				  {
					cerr << "multiple use of options -a/-A not allowed\n";
					return(4);
				  }
				  aflg = true;
				  if(argv[i][2])
				  {
					file = argv[i] + 2;
				  }
				  else
				  {
					if((i+1 < argc) && (argv[i+1][0] != '-'))
					{	
						i++;
						file = argv[i];
					}
				  }
				  if(file)
				  {
					analysefile = new char [strlen(file) + 2];
					strcpy(analysefile,file);
				  }
				  break;
		case 'A': if(aflg || Aflg)
				  {
					cerr << "multiple use of options -a/-A not allowed\n";
					return(4);
				  }
				  Aflg = true;
				  break;
		case 's': if(sflg || Sflg) 
				  {
					cerr << "multiple use of options -s/-S not allowed\n";
					return(4);
				  }
				  sflg = true;
				  if(argv[i][2])
				  {
					file = argv[i] + 2;
				  }
				  else
				  {
					if( (i+1 < argc) && (argv[i+1][0] != '-'))
					{	
						i++;
						file = argv[i];
					}
				  }
				  if(file)
				  {
					statefile = new char [strlen(file) + 2];
					strcpy(statefile,file);
				  }
				  break;
		case 'S': if(sflg || Sflg)
				  {
					cerr << "multiple use of options -s/-S not allowed\n";
					return(4);
				  }
				  Sflg = true;
				  break;
		case 'y': if(yflg || Yflg) 
				  {
					cerr << "multiple use of options -y/-Y not allowed\n";
					return(4);
				  }
				  yflg = true;
				  if(argv[i][2])
				  {
					file = argv[i] + 2;
				  }
				  else
				  {
					if((i+1 < argc) && (argv[i+1][0] != '-'))
					{	
						i++;
						file = argv[i];
					}
				  }
				  if(file)
				  {
					symmfile = new char [strlen(file) + 2];
					strcpy(symmfile,file);
				  }
				  break;
		case 'Y': if(yflg || Yflg)
				  {
					cerr << "multiple use of options -y/-Y not allowed\n";
					return(4);
				  }
				  Yflg = true;
				  break;
		case 'p': if(pflg || Pflg) 
				  {
					cerr << "multiple use of options -p/-P not allowed\n";
					return(4);
				  }
				  pflg = true;
				  if(argv[i][2])
				  {
					file = argv[i] + 2;
				  }
				  else
				  {
					if((i+1 < argc) && (argv[i+1][0] != '-'))
					{	
						i++;
						file = argv[i];
					}
				  }
				  if(file)
				  {
					pathfile = new char [strlen(file) + 2];
					strcpy(pathfile,file);
				  }
				  break;
		case 'P': if(pflg || Pflg)
				  {
					cerr << "multiple use of options -p/-P not allowed\n";
					return(4);
				  }
				  Pflg = true;
				  break;
		case 'f':
		case 'm':
		case 'g': graphformat = argv[i][1];
				  if(gmflg || GMflg) 
				  {
					cerr << "multiple use of options -g/-m-f/-G/-M/-F not allowed\n";
					return(4);
				  }
				  gmflg = true;
				  if(argv[i][2])
				  {
					file = argv[i] + 2;
				  }
				  else
				  {
					if((i+1 < argc) &&(argv[i+1][0] != '-'))
					{	
						i++;
						file = argv[i];
					}
				  }
				  if(file)
				  {
					graphfile = new char [strlen(file) + 2];
					strcpy(graphfile,file);
				  }
				  break;
		case 'F':
		case 'M':
		case 'G': graphformat = tolower(argv[i][1]);
				  if(gmflg || GMflg)
				  {
					cerr << "multiple use of options -g/-m-f/-G/-M/-F not allowed\n";
					return(4);
				  }
				  GMflg = true;
				  break;
		default: cerr << "unrecognised option: -" << argv[i][1] << "\n";
				 return(4);
		}
	}
	else
	{
		// argument without option - must be net file;
		if(netfile)
		{
			cerr << "multiple net file argument\n";
			return(4);
		}
		netfile = new char[strlen(argv[i])+2 ];
		netbasename = new char [strlen(argv[i]) + 2];
		strcpy(netfile,argv[i]);
		strcpy(netbasename,argv[i]);
		for(h=strlen(netbasename);;h--)
		{
			if(netbasename[h] == '.')
			{	
				netbasename[h] = '\0';
				break;
			}
			if(h == 0) break;
		}
	}
  }
  // all options processed, now standard names for omitted file options
  if(!netbasename)
  {
	netbasename = new char[ strlen("unknown_net") + 2];
	strcpy(netbasename,"unknown_net");
  }
  if(nflg && !lownetfile)
  {	
	lownetfile = new char [strlen(netbasename) + 7];
	strcpy(lownetfile,netbasename);
	strcpy(lownetfile+strlen(netbasename),".llnet");
	pnmlfile = new char [strlen(netbasename) + 20];
	strcpy(pnmlfile,lownetfile);
	strcpy(pnmlfile+strlen(pnmlfile),".pnml");
  }
  if(aflg && !analysefile)
  {	
	analysefile = new char [strlen(netbasename) + 7];
	strcpy(analysefile,netbasename);
	strcpy(analysefile+strlen(netbasename),".task");
  }
  if(sflg && !statefile)
  {	
	statefile = new char [strlen(netbasename) + 8];
	strcpy(statefile,netbasename);
	strcpy(statefile+strlen(netbasename),".state");
  }
  if(yflg && !symmfile)
  {	
	symmfile = new char [strlen(netbasename) + 7];
	strcpy(symmfile,netbasename);
	strcpy(symmfile+strlen(netbasename),".symm");
  }
  if(pflg && !pathfile)
  {	
	pathfile = new char [strlen(netbasename) + 7];
	strcpy(pathfile,netbasename);
	strcpy(pathfile+strlen(netbasename),".path");
  }
  if(gmflg && !graphfile)
  {	
	graphfile = new char [strlen(netbasename) + 8];
	strcpy(graphfile,netbasename);
	strcpy(graphfile+strlen(netbasename),".graph");
  }

  // 2. Initialisierung
	if(hflg) reportconfiguration();

  NonEmptyHash = 0;
  try {
#ifdef DISTRIBUTE
	if(!init_communication()) _exit(5);
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
		char mess[] = "\nhash table too large!\n";
		//write(2,mess,sizeof(mess));
		cerr << mess;
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
          binHashTable[i] = (binDecision *) 0;
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
		char mess[] = "\nnot enough space to read net\n";
		//write(2,mess,sizeof(mess));
		cerr << mess;
#ifdef DISTRIBUTE
		end_communication();
#endif
		_exit(2);
	}
  try{
#ifdef DISTRIBUTE
	  rapport("reading net");
#endif
	  readnet();
	  removeisolated();
#ifdef DISTRIBUTE
	StateLength = Places[0]->cnt;
	MaxComponentValue = CAPACITY;
#endif
     }
  catch(overflow)
  {
	char mess [] = "\nnot enough space to store net\n";
	//write(2,mess,sizeof(mess));
	cerr << mess;
#ifdef DISTRIBUTE
		end_communication();
#endif
	_exit(2);
  }
  delete PlaceTable;
  delete TransitionTable;
unsigned int j;
  for(j=0; j < Places[0]->cnt;j++)
    {
      Places[j] -> set_hash(rand());
    }
  cout << Places[0]->cnt << " Places\n";
  cout << Transitions[0]->cnt << " Transitions\n";
  Places[0]->NrSignificant = Places[0]->cnt;



#ifdef SYMMETRY
  try{
	if(SYMMINTEGRATION == 1 || SYMMINTEGRATION == 3)
	{
#ifdef DISTRIBUTE
		rapport("computing symmetries");
#endif
		ComputeSymmetries();
	}
	else
	{
#ifdef DISTRIBUTE
		rapport("computing partition w.r.t. symmetries");
#endif
		ComputePartition();
	}
  for(i=0;i<Places[0]->cnt;i++)
  {
	Places[i]-> index = i;
        CurrentMarking[i] = Places[i]->initial_marking;
  }
  for(i=0;i<Transitions[0]->cnt;i++)
  {
	Transitions[i]->enabled = false;
  }
  for(i=0;i<Transitions[0]->cnt;i++)
  {
	Transitions[i]->initialize();
  }
  for(i=0;i<Transitions[0]->cnt;i++)
  {
	Transitions[i]->PrevEnabled = (i == 0 ? (Transition *) 0 : Transitions[i-1]);
	Transitions[i]->NextEnabled = (i == Transitions[0]->cnt - 1 ? (Transition *) 0 : Transitions[i+1]);
	Transitions[i]->enabled = true;
  }
  Transitions[0]->StartOfEnabledList = Transitions[0];
  Transitions[0]->NrEnabled = Transitions[0]->cnt;
  for(i=0;i<Transitions[0]->cnt;i++)
  {
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
	while(1)
	{
		if(Places[i]->significant) break;
		if(i == 0) break;
		i--;
	}
	for(h=0;h<i;h++)
	{
		Places[h]->significant = true;
	}
	Places[0]->NrSignificant = h+1;
	cout << "\n" << Places[0]->NrSignificant << " significant places\n";
#endif
	
  }
  catch(overflow)
  {
	char mess [] = "\nnot enough space to store generating set for symmetries!\ntry again without use of symmetries!\n";
	//write(2,mess,sizeof(mess));
	cerr << mess;
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
	for(i=0, h = Places[0]->cnt;;i++,h--)
	{
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
  for(j=0;j<Places[0]->cnt;j++)
  {
	Places[j]-> index = j;
        CurrentMarking[j] = Places[j]->initial_marking;
  }
  for(j=0;j<Transitions[0]->cnt;j++)
  {
	Transitions[j]->enabled = false;
  }
  for(j=0;j<Transitions[0]->cnt;j++)
  {
	Transitions[j]->initialize();
  }
  for(j=0;j<Transitions[0]->cnt;j++)
  {
	Transitions[j]->PrevEnabled = (j == 0 ? (Transition *) 0 : Transitions[j-1]);
	Transitions[j]->NextEnabled = (j == Transitions[0]->cnt - 1 ? (Transition *) 0 : Transitions[j+1]);
 	Transitions[j]->enabled = true;
  }
  Transitions[0]->StartOfEnabledList = Transitions[0];
  Transitions[0]->NrEnabled = Transitions[0]->cnt;
  for(j=0;j<Transitions[0]->cnt;j++)
  {
	Transitions[j]->check_enabled();
  }
#endif
#if defined(CYCLE) || defined(STRUCT)
	tsolve();
	for(i=0;i<Transitions[0]->cnt;i++)
	{
		if(Transitions[i]->cyclic)
		{
			cout << Transitions[i]->name << endl;
		}
	}
#endif
  unsigned int ii;
  for(ii=0;ii < Transitions[0]->cnt;ii++)
  {
	Transitions[ii]->check_enabled();
  }
  for(ii=0;ii < Places[0]->cnt;ii++)
  {
	CurrentMarking[ii] = Places[ii]->initial_marking;
	Places[ii]->index = ii;
  }
#ifdef LTLPROP
	print_buchi();
#endif
  if(Nflg) { printnet(); printpnml();}
  if(nflg)
  {
	ofstream lownetstream(lownetfile);
	if(!lownetstream)
	{
		cerr << "Cannot open net output file: " << lownetfile <<
		"\nno output written";
    }
	else
	{
		printnettofile(lownetstream);
	}
	ofstream pnmlstream(pnmlfile);
	if(!pnmlstream)
	{
		cerr << "Cannot open net output file: " << pnmlfile <<
		"\nno output written";
    }
	else
	{
		printpnmltofile(pnmlstream);
	}
  }
#ifdef DISTRIBUTE
        SignificantLength = Places[0] -> NrSignificant;
	if(!init_storage()) _exit(5);
#endif
  for(j=0,BitVectorSize = 0;j<Places[0]->NrSignificant;j++)
  {
        BitVectorSize += Places[j]->nrbits;
  }
#ifdef WITHFORMULA
	if(F)
	{
		checkstart = new unsigned int[F->card+5];
		for(i=0;i<F->card;i++) 
		{
			checkstart[i] = 0;
		}
	}
#endif

		
	try{
//siphontrapproperty();
#ifdef NONE
	return 0;
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
return  1 - GRAPH();
#endif
}
catch(overflow)
{
	//char mess [] = "memory exhausted\n";
	//cerr << mess;
	_exit(2);
}
}
catch(overflow)
{
	char mess [] = "memory exhausted\n";
	cerr << mess;
	_exit(2);
}
}



void removeisolated()
{
	unsigned int i;

	Place * p;

	i=0;
	while(i<Places[0]->cnt)
	{
		if(Places[i]->references == 0) // Place isolated
		{
			p = Places[Places[0]->cnt - 1];
			Places[Places[0]->cnt - 1] = Places[i];
			Places[i] = p;
			Places[0]->cnt --;
		}
		else
		{
			i++;
		}
	}
	for(i=0;i<Transitions[0]->cnt;i++)
	{
		Transitions[i]->nr = i;
	}
}

#ifdef CYCLE
void findcyclingtransitions()
{
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
	currentnode -> parent = (Node *) 0 ; // bottom stack element
	currentnode -> pos[1] = 9 ; // transition, on stack, not selected

	// loop

	while(currentnode)
	{
		// is there another successor ?
		newnode = (Node *) 0;
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

