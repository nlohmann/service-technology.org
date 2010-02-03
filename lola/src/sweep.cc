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


#include "graph.H"
#include "net.H"
#include "stubborn.H"
#include "formula.H"
#include "tinv.H"
#include "sweep.H"

#ifdef SWEEP
void print_binDec(binDecision * ,int);
binDecision ** CurrentSweep; // < Current --> new persistents
                            // >= Current --> unprocessed, sorted by progress value modulo 3 * progress span
int * BucketSize;           // sizes of the buckets in current sweep
binDecision ** OldPersistent; // already processed persistent states
binDecision * SameValue;  // already processed states in current sweep with progress value = current
                          // need this if ex. transitions with progress 0

pendinglist ** currentpending; // unprocessed persistent states; toggle for current resp. next sweep
pendinglist ** nextpending; 
pendinglist *** pendingtail; // for inserting in pending for current;

void printstate(char *, unsigned int *);
void print_path(State *);
int NewPos, NewPos3;

long int ProgressValueHigh, ProgressValueLow;   // the actual current progress value = High * 2^progress span + Low
int CurrentSection;       // {0, progress span, 2* progress span}  ; index for processing part of current sweep

long int NrSame, NrPersistent, PeakMemUse, CurrentMemUse, TransitionsFired, NrSweeps;


void print_sweep( char * text)
{
	int i;
	// print whole state of the sweep line method's data structure
	cout << "*** SWEEP *** " << text << endl;
	cout << "ProgressSpan: " << ProgressSpan << " NrSweeps: " << NrSweeps << " CurrentSection: " << CurrentSection << endl;
 	cout << "ProgressValueHigh: " << ProgressValueHigh << " ProgressValueLow: " << ProgressValueLow <<
                 " TransitionsFired: " << TransitionsFired << endl;
	cout << "NrPersistent: " << NrPersistent << " PeakMemUse: " << PeakMemUse << " CurrentMemUse: " << CurrentMemUse <<
		"NrSame: " << NrSame << endl;
 
	// - current sweep
	cout << " *** CurrentSweep *** " << endl;
	for(i=0;i<3 * ProgressSpan;i++)
	{
		cout << "<<" << i << ">>" << endl;
		print_binDec(CurrentSweep[i],0);
	}
	// - oldpersistent
	cout << " *** OldPersistent *** " << endl;
	for(i=0;i< ProgressSpan;i++)
	{
		cout << "<<" << i << ">>" << endl;
		print_binDec(OldPersistent[i],0);
	}
	// - same value
	cout << " *** SameValue *** " << endl;
		cout << "<<" << i << ">>" << endl;
		print_binDec(SameValue,0);
    	// - current pending
	cout << " *** CurrentPending *** " << endl;
	for(i=0;i< ProgressSpan;i++)
	{
		cout << "<<" << i << ">>" << endl;
		pendinglist * p = currentpending[i];
		while(p)
		{
			cout << "value: " << p -> value << "size: " << p -> size << endl;
			print_binDec(p->content,0);
			p = p -> next;
		}
	}
	// - next pending
	cout << " *** NectPending *** " << endl;
	for(i=0;i< ProgressSpan;i++)
	{
		cout << "<<" << i << ">>" << endl;
		pendinglist * p = nextpending[i];
		while(p)
		{
			cout << "value: " << p -> value << "size: " << p -> size << endl;
			print_binDec(p->content,0);
			p = p -> next;
		}
	}
	// - pending tail
	cout << " *** PendingTail *** " << endl;
	for(i=0;i< ProgressSpan;i++)
	{
		int j;
		j=0;
		cout << "<<" << i << ">>" << endl;
		pendinglist ** p = nextpending+i;
		while(p != pendingtail[i] )
		{
			j++;
			p = &((*p) -> next);
		}
		cout << j << endl;
	}

}

// main idea: CurrentSweep is a window of states that have a progress value close to current progress value
// current pointer is cycling around in this window.
// states behind current pointer are new persistent states (never seen before). States in front of current pointer
// are unprocessed states in current sweep.
// Whenever current pointer passed boundary of one third of the window, there is another third completely
// out of reach (that's why size 3 * span). This third is tranformed from an "old" third to a "new" third:
// - pending persistent states are parked in pending[1 - currentpending] (they need to be processed in next sweep)
// - instead, states from pending[currentpending] are placed there in order to be processed.
// processing a set of states with same progress value
// 1. fill current state
// 2. remove from currentsweep
// 3. if persistent, add to oldpersistent
// 4. if some transition has progress 0, add to samevalue (in this case, too early to be removed completely)
// 5. process current:
//    - compute succ
//    - search in currentsweep, oldpersistent, samevalue
//    - if new,  - check property
//               - add to currentsweep


void setpersistentbit(binDecision * d)
{
	if(d -> nextold) setpersistentbit(d -> nextold);
	if(d -> nextnew) setpersistentbit(d -> nextnew);
#ifdef SWEEP
	d -> persistent = 1;
#endif
}


int sweep()
{
#ifdef SWEEP
	int i;
	int j;
	int NrAhead;
	int NrBehind;
	int pendinglistempty;

	PeakMemUse = CurrentMemUse = NrSweeps = 1;
	NrSame = NrPersistent = TransitionsFired = 0;
	CurrentState = new State; // not used, but is needed as return value != 0 in binSearch
	progress_measure();
	CurrentSweep = new binDecision * [3 * ProgressSpan + 12];
	BucketSize = new int [3 * ProgressSpan + 12] ;

	// init stuff for checking properties and outputting
	for(j=0;j<Transitions[0]->cnt;j++)
	{
		Transitions[j]->check_enabled();
	}
  ofstream * graphstream;
  if(gmflg)
  {
	  graphstream = new ofstream(graphfile);
		if(!graphstream)
		{
			cerr << "Cannot open graph output file: " << graphfile
			<< "\nno output written\n";
			gmflg = false;
		}
  }
		

	int jj;
	if(gmflg)
	{
	  (*graphstream) << "STATE x" << "; PROGRESS 0";
	  j=0;
	  if(graphformat == 'm')
	  {
		 for(jj=0;jj<Places[0]->cnt;jj++)
		 {
			 if(CurrentMarking[jj])
			 {
				 if(CurrentMarking[jj] == VERYLARGE)
				 {
				 (*graphstream) << (j++ ? ",\n" : "\n") << Places[jj]->name << " : " << "oo" ;
				 }
				 else
				 {
				 (*graphstream) << (j++ ? ",\n" : "\n") << Places[jj]->name << " : " << CurrentMarking[jj];
				 }
			 }
		 }

	  }
	  (*graphstream) << "\n\n";
	}
	if(GMflg)
	{
	  cout << "STATE x" << "; PROGRESS 0";
	  j=0;
	  if(graphformat == 'm')
	  {
		 for(jj=0;jj<Places[0]->cnt;jj++)
		 {
			 if(CurrentMarking[jj])
			 {
				 if(CurrentMarking[jj] == VERYLARGE)
				 {
				 cout << (j++ ? ",\n" : "\n") << Places[jj]->name << " : " << "oo" ;
				 }
				 else
				 {
				 cout << (j++ ? ",\n" : "\n") << Places[jj]->name << " : " << CurrentMarking[jj];
				 }
			 }
		 }

	  }
	  cout << "\n\n";
	}
#ifdef DEADTRANSITION
	if(CheckTransition -> enabled)
	{
	      cout << "\ntransition " << CheckTransition -> name << " is not dead!\n";
		  printstate("",CurrentMarking);
				// statistics
				cerr << NrSweeps << "sweeps, " 
				     << TransitionsFired << " transitions fired, " 
				     << "peak " << PeakMemUse << " States, " 
				     << NrPersistent << " States persistent." 
				     << endl; 
	return 1;
	}
#endif
#ifdef DEADLOCK 
	if(Transitions[0]->NrEnabled == 0)
	{
		 // early abortion 
		 cout << "\ndead state found!\n";
		 printstate("",CurrentMarking);
				// statistics
				cerr << NrSweeps << "sweeps, " 
				     << TransitionsFired << " transitions fired, " 
				     << "peak " << PeakMemUse << " States, " 
				     << NrPersistent << " States persistent." 
				     << endl; 
		 return 1;
	}
#endif
#ifdef STATEPREDICATE
	int res;
	if(!F)
	{	
    fprintf(stderr, "lola: specify predicate in analysis task file!\n");
    fprintf(stderr, "      mandatory for task STATEPREDICATE\n");
		_exit(4);
	}
	F = F -> reduce(&res);
	if(res<2) return res;	
	F = F -> posate();
	F -> tempcard = 0;
	F -> setstatic();
	if(F ->  tempcard) 
	{
    fprintf(stderr, "lola: temporal operators are not allowed in state predicates\n");
    fprintf(stderr, "      not allowed for task STATEPREDICATE\n");
		exit(3);
	}
	cout << "\n Formula with\n" << F -> card << " subformula.\n";
	F -> parent = NULL;
	if(F -> initatomic())
	{
		cout << "\nstate found!\n";
		printstate("",CurrentMarking);
				// statistics
				cerr << NrSweeps << "sweeps, " 
				     << TransitionsFired << " transitions fired, " 
				     << "peak " << PeakMemUse << " States, " 
				     << NrPersistent << " States persistent." 
				     << endl; 
		return 1;
	}
#endif

#ifdef REACHABILITY 
	for(jj=0;jj<Places[0]->cnt;jj++)
	{
		 if(CurrentMarking[jj] != Places[jj]->target_marking)
			 break;
	}
	if(jj >= Places[0]->cnt) // target_marking found!
	{
			// early abortion  
		      cout << "\nstate found!\n";
			  printstate("",CurrentMarking);
			print_path(CurrentState);
				// statistics
				cerr << NrSweeps << "sweeps, " 
				     << TransitionsFired << " transitions fired, " 
				     << "peak " << PeakMemUse << " States, " 
				     << NrPersistent << " States persistent." 
				     << endl; 
		 return 1; 
	}
#endif
	// end of init stuff for checking properties

	if(!MonotoneProgress)
	{
		OldPersistent = new binDecision * [ProgressSpan + 9];
		currentpending = new pendinglist * [ProgressSpan + 9];
		nextpending = new pendinglist * [ProgressSpan + 9];
		pendingtail = new pendinglist ** [ProgressSpan + 9];
	}
	for(i=0;i<ProgressSpan;i++)
	{
		CurrentSweep[i] = CurrentSweep[ProgressSpan + i] = CurrentSweep[2 * ProgressSpan + i] = NULL;
		BucketSize[i] = BucketSize[ProgressSpan + i] = BucketSize[2 * ProgressSpan + i] = 0;
		if(!MonotoneProgress) 
		{
			OldPersistent[i] = NULL;
			currentpending[i] = nextpending[i] = NULL;
			pendingtail[i] = nextpending + i;
		}
	}
	SameValue = 0;
	ProgressValueHigh = ProgressValueLow = 0;
	CurrentSection = 0;
	binSearch(CurrentSweep[0]);
	binInsert(CurrentSweep);     // insert initial state at progress value 0
	CurrentMemUse = 1;
	NrAhead = 1; NrBehind = 0;
	BucketSize[0] = 1;

	//print_sweep("Start");
	while(1)
	{
		// every iteration is processing all states with same progress value
		while(CurrentSweep[CurrentSection * ProgressSpan + ProgressValueLow])
		{

			//print_sweep("New Round");
			// fill top level state of tree into current marking
  			unsigned int pm, p,pb,t,k,byte,s;
			unsigned  char * v;
  			bool spl;
		        p = Places[0] -> cnt - 1;
		        v = CurrentSweep[CurrentSection * ProgressSpan + ProgressValueLow]->vector;
		        t = BitVectorSize / 8;
	    		s = BitVectorSize % 8;
		        pb = Places[p] -> nrbits;
		        byte = v[t] >> (8 - s);
		    	pm = 0;

		        while(1)
		    	{
              
				// compute marking pm of place p 
				if(pb > s)
				{
					// place has more bits left than remaining in current byte
					if(s)
					{
						// at least, there _are_ bits left in current byte
						pm += byte << (Places[p]->nrbits - pb);
						pb -= s;
						s = 0;
					}
					else
					{
						// current byte empty
						// there must be another byte in current vector
						t--;
						s = 8;
						byte = v[t];
					}
				}
				else
				{
					// all bits left for this place are in current byte
					pm += (byte % (1 << pb)) << (Places[p]->nrbits - pb);
					byte = byte >> pb;
					s -= pb;
				        MARKINGVECTOR[p] = pm;
		      			if(p==0)
					{
						  break;
					}
					p--;
					pb = Places[p]->nrbits;
					pm = 0;
				}
			}
#ifdef STATEPREDICATE
			F -> initatomic();
#endif
			//print_sweep("State extracted");
			// remove from current sweep
			//cout << "process "; print_marking(); cout << endl;
			int persistent;
			if(!MonotoneProgress)
			{
				persistent = CurrentSweep[CurrentSection * ProgressSpan + ProgressValueLow] -> persistent;
			}
			binDelete(CurrentSweep + CurrentSection * ProgressSpan + ProgressValueLow);
			CurrentMemUse--;
			BucketSize[CurrentSection * ProgressSpan + ProgressValueLow]--;
			if((!MonotoneProgress) && persistent)
			{
				binSearch(OldPersistent[ProgressValueLow]);
				binInsert(OldPersistent + ProgressValueLow);
				CurrentMemUse ++;
#ifdef MAXIMALSTATES
                checkMaximalStates(CurrentMemUse); ///// LINE ADDED BY NIELS
#endif
			}
			else
			{
				// if exists progress value 0, move to samevalue
				if(ZeroProgress)
				{
					binSearch(SameValue);
					binInsert(&SameValue);	
					NrSame++;
					CurrentMemUse++;
#ifdef MAXIMALSTATES
                    checkMaximalStates(CurrentMemUse); ///// LINE ADDED BY NIELS
#endif
				}
			}
			//print_sweep("After Removal");
			// process 
			//   init transitions	
			for(jj=0;jj<Transitions[0]->cnt;jj++)
			{
				Transitions[jj] -> check_enabled();
			}
			//   compute successors
			Transition ** fl = FIRELIST();
			int currentfired;
			for(currentfired=0;fl[currentfired];currentfired++)
			{
				// fire
				TransitionsFired++;
				if(!(TransitionsFired % REPORTFREQUENCY))
				{
					cerr << "Sw # " << NrSweeps << ", " 
					     << TransitionsFired << " fired, " 
					     << "curr. " << CurrentMemUse << " st, " 
					     << "peak " << PeakMemUse << " st, " 
					     << NrPersistent << " pers." 
					     << "prog.val " << ProgressValueHigh * ProgressSpan + ProgressValueLow
					     << endl; 
				}
				fl[currentfired] -> fire();
#ifdef STATEPREDICATE
				update_formula(fl[currentfired]);
#endif
				//cout << "fire " << fl[currentfired]->name << " yields " << endl;
				//print_marking(); cout << endl;
				NewPos = ProgressValueLow + fl[currentfired] -> progress_value;
				if(NewPos < 0) NewPos += ProgressSpan;
				if(NewPos >= ProgressSpan) NewPos -= ProgressSpan;
				NewPos3 = ProgressValueLow + fl[currentfired] -> progress_value + CurrentSection * ProgressSpan;
				if(NewPos3 < 0) NewPos3 += 3 * ProgressSpan;
				if(NewPos3 >= 3*ProgressSpan) NewPos3 -= 3*ProgressSpan;
				// search
				if((!(fl[currentfired]->progress_value)) &&  binSearch(SameValue)) goto back;
				if((!MonotoneProgress) && binSearch(OldPersistent[NewPos])) goto back;
				if(binSearch(CurrentSweep[NewPos3])) goto back;
					
				// output and check property
	if(gmflg)
	{
	  (*graphstream) << "STATE x BY " << fl[currentfired]->name << "; PROGRESS " << ProgressValueHigh * ProgressSpan + ProgressValueLow;
	  j=0;
	  if(graphformat == 'm')
	  {
		 for(jj=0;jj<Places[0]->cnt;jj++)
		 {
			 if(CurrentMarking[jj])
			 {
				 if(CurrentMarking[jj] == VERYLARGE)
				 {
				 (*graphstream) << (j++ ? ",\n" : "\n") << Places[jj]->name << " : " << "oo" ;
				 }
				 else
				 {
				 (*graphstream) << (j++ ? ",\n" : "\n") << Places[jj]->name << " : " << CurrentMarking[jj] ;
				 }
			  }
		 }

	  }
	  (*graphstream) << "\n\n";
	}
	if(GMflg)
	{
	  cout << "STATE x BY " << fl[currentfired]->name << "; PROGRESS " << ProgressValueHigh * ProgressSpan + ProgressValueLow;
	  j=0;
	  if(graphformat == 'm')
	  {
		 for(jj=0;jj<Places[0]->cnt;jj++)
		 {
			 if(CurrentMarking[jj])
			 {
				 if(CurrentMarking[jj] == VERYLARGE)
				 {
				 cout << (j++ ? ",\n" : "\n") << Places[jj]->name << " : " << "oo" ;
				 }
				 else
				 {
				 cout << (j++ ? ",\n" : "\n") << Places[jj]->name << " : " << CurrentMarking[jj] ;
				 }
			 }
		 }

	  }
	  cout << "\n\n";
	}
#ifdef DEADTRANSITION
                if(CheckTransition -> enabled)
	            {
		        // early abortion  
	      cout << "\ntransition " << CheckTransition -> name << " is not dead!\n";
					printstate("",CurrentMarking);
				// statistics
				cerr << NrSweeps << "sweeps, " 
				     << TransitionsFired << " transitions fired, " 
				     << "peak " << PeakMemUse << " States, " 
				     << NrPersistent << " States persistent." 
				     << endl; 
		        return 1;
                    }
#endif
#ifdef DEADLOCK
                if(!(Transitions[0]-> StartOfEnabledList))
	            {
		        // early abortion  
	                cout << "\ndead state found!\n";
					printstate("",CurrentMarking);
				// statistics
				cerr << NrSweeps << "sweeps, " 
				     << TransitionsFired << " transitions fired, " 
				     << "peak " << PeakMemUse << " States, " 
				     << NrPersistent << " States persistent." 
				     << endl; 
		        return 1;
                    }
#endif
#ifdef STATEPREDICATE
			update_formula(fl[currentfired]);
		if(F->value)
		{
			// early abortion
	                cout << "\nstate found!\n";
					printstate("",CurrentMarking);
				// statistics
				cerr << NrSweeps << "sweeps, " 
				     << TransitionsFired << " transitions fired, " 
				     << "peak " << PeakMemUse << " States, " 
				     << NrPersistent << " States persistent." 
				     << endl; 
		        return 1;
                    }
#endif
#ifdef REACHABILITY 
for(jj=0;jj<Places[0]->cnt;jj++)
{
	 if(CurrentMarking[jj]!= Places[jj]->target_marking)
		 break;
}
if(jj < Places[0]->cnt) // target_marking found!
{
		// early abortion  
	      cout << "\nstate found!\n";
		  printstate("",CurrentMarking);
				// statistics
				cerr << NrSweeps << "sweeps, " 
				     << TransitionsFired << " transitions fired, " 
				     << "peak " << PeakMemUse << " States, " 
				     << NrPersistent << " States persistent." 
				     << endl; 
	 return 1; 
}
#endif

// end check property
				//print_sweep("Before Insert");
				// insert
				binInsert(CurrentSweep + NewPos3);
				CurrentMemUse++;
#ifdef MAXIMALSTATES
                checkMaximalStates(CurrentMemUse); ///// LINE ADDED BY NIELS
#endif
				if(CurrentMemUse > PeakMemUse) PeakMemUse = CurrentMemUse ;
				BucketSize[NewPos3] ++;
				if(fl[currentfired] -> progress_value >= 0)  NrAhead++; else {NrBehind ++;NrPersistent++;};
				//print_sweep("After Insert");
				// return to original state
back:                           fl[currentfired] -> backfire();
#ifdef STATEPREDICATE
				update_formula(fl[currentfired]);
#endif
			}
			NrAhead--;
		}
		// move to next progress value or exit
		ProgressValueLow++;
		if(!(ProgressValueLow % ProgressSpan))
		{
			ProgressValueLow = 0;		
			ProgressValueHigh++;
			CurrentSection = (CurrentSection + 1);
			if(CurrentSection > 2) CurrentSection = 0;
		}
		//print_sweep("New Progress Value");
		// delete SameValue
		if(ZeroProgress) 
		{
			binDeleteAll(SameValue);
			SameValue = NULL;
			CurrentMemUse -= NrSame; NrSame = 0;
		}
		//print_sweep("Processed Same Value");

		// if new third, exchange old stuff
		if(!ProgressValueLow)
		{
			if(!MonotoneProgress)
			{
				int opposite = ((CurrentSection + 1) % 3) * ProgressSpan;
				if(NrBehind)
				{
					for(i=0;i<ProgressSpan;i++)
					{
						// collect new persistents
						if(CurrentSweep[opposite + i])
						{
							pendinglist * last = new pendinglist;
							last -> content = CurrentSweep[opposite + i];
							last -> next = NULL;
							last -> value = ProgressValueHigh - 2;
							last -> size = BucketSize[opposite + i];
							setpersistentbit(last -> content);
							* (pendingtail[i]) = last;
							pendingtail[i] = &(last -> next);
						}
						CurrentSweep[opposite + i] = NULL;
						NrBehind -= BucketSize[opposite + i];
						BucketSize[opposite + i] = 0;
					}
				}
				pendinglistempty = 1;
				for(i=0;i<ProgressSpan;i++)
				{
					// fill next persistents
					
					if(currentpending[i]) pendinglistempty = 0;
					if(currentpending[i] && currentpending[i] -> value == ProgressValueHigh + 1)
					{
						CurrentSweep[opposite + i] = currentpending[i] -> content;
						NrAhead += (BucketSize[opposite + i] = currentpending[i] -> size);
						pendinglist * tmp = currentpending[i];
						currentpending[i] = tmp -> next;
						delete tmp;
					}
					else
					{
						CurrentSweep[opposite + i] = NULL;
						BucketSize[opposite + i] = 0;
					}
				}
				//print_sweep("Switched to new Section");
			}
			// check for end of sweep and end it properly
			if(MonotoneProgress) 
			{
#ifdef REACHABILITY
	cout << "\n state is not reachable!\n";
#endif
#ifdef DEADLOCK
	cout << "\nnet does not have deadlocks!\n";
#endif
#if defined(STATEPREDICATE) 
	cout << "\n predicate is not satisfiable!\n";
#endif
#ifdef DEADTRANSITION
	cout << "\ntransition " << CheckTransition -> name << " is dead!\n";
#endif
				// statistics
				cerr << NrSweeps << "sweeps, " 
				     << TransitionsFired << " transitions fired, " 
				     << "peak " << PeakMemUse << " States, " 
				     << NrPersistent << " States persistent." 
				     << endl; 
	return 0;
			}
			if((!NrAhead) && pendinglistempty)
			{
				if(!MonotoneProgress && NrBehind)
				{
					int opposite = ((CurrentSection + 2) % 3) * ProgressSpan;
					ProgressValueHigh++;
					for(i=0;i<ProgressSpan;i++)
					{
						// collect new persistents
						if(CurrentSweep[opposite + i])
						{
							pendinglist * last = new pendinglist;
							last -> content = CurrentSweep[opposite + i];
							last -> next = NULL;
							last -> value = ProgressValueHigh - 2;
							last -> size = BucketSize[opposite + i];
							setpersistentbit(last -> content);
							* (pendingtail[i]) = last;
							pendingtail[i] = &(last -> next);
						}
						CurrentSweep[opposite + i] = NULL;
						NrBehind -= BucketSize[opposite + i];
						BucketSize[opposite + i] = 0;
					}
				}
				// init new sweep or exit
				//print_sweep("Init new or Exit");
				// switch check pending persistents
				pendinglist ** tmpp = currentpending;
				currentpending = nextpending;
				nextpending = tmpp;
				for(i=0;i<ProgressSpan;i++)
				{
					pendingtail[i] = nextpending + i;
				}
				for(i=0;i<ProgressSpan;i++)
				{
					if(currentpending[i]) break;
				}
				if(i >= ProgressSpan)
				{
#ifdef REACHABILITY
	cout << "\n state is not reachable!\n";
#endif
#ifdef DEADLOCK
	cout << "\nnet does not have deadlocks!\n";
#endif
#if defined(STATEPREDICATE) 
	cout << "\n predicate is not satisfiable!\n";
#endif
#ifdef DEADTRANSITION
	cout << "\ntransition " << CheckTransition -> name << " is dead!\n";
#endif
					// statistics
					cerr << NrSweeps << "sweeps, " 
					     << TransitionsFired << " transitions fired, " 
					     << "peak " << PeakMemUse << " States, " 
					     << NrPersistent << " States persistent." 
					     << endl; 
					return 0;
				}
				ProgressValueHigh = currentpending[i] -> value;
				i++;
				for(;i<ProgressSpan;i++)
				{
					if(currentpending[i] && ProgressValueHigh > currentpending[i] -> value)
					{
						ProgressValueHigh = currentpending[i] -> value;
					}
				}
				for(i=0;i<ProgressSpan;i++)
				{
					if(currentpending[i])
					{
						if(currentpending[i] -> value == ProgressValueHigh)
						{
							NrAhead += (BucketSize[i] = currentpending[i] -> size);
							CurrentSweep[i] = currentpending[i] -> content;
							pendinglist * tmppp = currentpending[i];	
							currentpending[i] = tmppp -> next;
							delete tmppp;
						}	
						if(currentpending[i] && currentpending[i] -> value == ProgressValueHigh + 1)
						{
							NrAhead += (BucketSize[ProgressSpan + i] = currentpending[i] -> size);
							CurrentSweep[ProgressSpan + i] = currentpending[i] -> content;
							pendinglist * tmppp = currentpending[i];	
							currentpending[i] = tmppp -> next;
							delete tmppp;
						}	
					}
				}
				CurrentSection = 0;
				ProgressValueLow = 0;
				NrSweeps++;
				//print_sweep("Last Code");
			}
		}
	}
#endif
}




#endif
