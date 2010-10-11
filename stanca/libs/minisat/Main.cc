/*****************************************************************************************[Main.cc]
Copyright (c) 2003-2006, Niklas Een, Niklas Sorensson
Copyright (c) 2007-2010, Niklas Sorensson

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT
OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**************************************************************************************************/

#include <ctime>
#include <cstring>
#include <stdint.h>
#include <errno.h>

#include <signal.h>
//#include <zlib.h>

#include "System.h"
#include "ParseUtils.h"
#include "Options.h"
#include "Dimacs.h"
#include "Solver.h"
#include "SimpSolver.h"


#include <vector>
#include <iostream>

using std::vector;

using namespace Minisat;

//=================================================================================================


void printStats(Solver& solver)
{
    double cpu_time = cpuTime();
   // double mem_used = memUsedPeak();
    //printf("restarts              : %"PRIu64"\n", solver.starts);
    //printf("conflicts             : %-12"PRIu64"   (%.0f /sec)\n", solver.conflicts   , solver.conflicts   /cpu_time);
    //printf("decisions             : %-12"PRIu64"   (%4.2f %% random) (%.0f /sec)\n", solver.decisions, (float)solver.rnd_decisions*100 / (float)solver.decisions, solver.decisions   /cpu_time);
    //printf("propagations          : %-12"PRIu64"   (%.0f /sec)\n", solver.propagations, solver.propagations/cpu_time);
    //printf("conflict literals     : %-12"PRIu64"   (%4.2f %% deleted)\n", solver.tot_literals, (solver.max_literals - solver.tot_literals)*100 / (double)solver.max_literals);
    //if (mem_used != 0) printf("Memory used           : %.2f MB\n", mem_used);
    printf("CPU time              : %g s\n", cpu_time);
}


//static 
Solver* solver;
// Terminate by notifying the solver and back out gracefully. This is mainly to have a test-case
// for this feature of the Solver as it may take longer than an immediate call to '_exit()'.
static void SIGINT_interrupt(int signum) { solver->interrupt(); }

// Note that '_exit()' rather than 'exit()' has to be used. The reason is that 'exit()' calls
// destructors and may cause deadlocks if a malloc/free function happens to be running (these
// functions are guarded by locks for multithreaded use).
static void SIGINT_exit(int signum) {
    printf("\n"); printf("*** INTERRUPTED ***\n");
    if (solver->verbosity > 0){
        printStats(*solver);
        printf("\n"); printf("*** INTERRUPTED ***\n"); }
    _exit(1); }


/*//=================================================================================================
// Main:


int main(int argc, char** argv)
{
    try {
        setUsageHelp("USAGE: %s [options] <input-file> <result-output-file>\n\n  where input may be either in plain or gzipped DIMACS.\n");
        // printf("This is MiniSat 2.0 beta\n");
        
#if defined(__linux__)
        fpu_control_t oldcw, newcw;
        _FPU_GETCW(oldcw); newcw = (oldcw & ~_FPU_EXTENDED) | _FPU_DOUBLE; _FPU_SETCW(newcw);
        printf("WARNING: for repeatability, setting FPU to use double precision\n");
#endif
        // Extra options:
        //
        IntOption    verb   ("MAIN", "verb",   "Verbosity level (0=silent, 1=some, 2=more).", 1, IntRange(0, 2));
        IntOption    cpu_lim("MAIN", "cpu-lim","Limit on CPU time allowed in seconds.\n", INT32_MAX, IntRange(0, INT32_MAX));
        IntOption    mem_lim("MAIN", "mem-lim","Limit on memory usage in megabytes.\n", INT32_MAX, IntRange(0, INT32_MAX));
        
        parseOptions(argc, argv, true);

        Solver S;
        double initial_time = cpuTime();

        S.verbosity = verb;
        
        solver = &S;
        // Use signal handlers that forcibly quit until the solver will be able to respond to
        // interrupts:
        signal(SIGINT, SIGINT_exit);
        signal(SIGXCPU,SIGINT_exit);

        // Set limit on CPU-time:
        if (cpu_lim != INT32_MAX){
            rlimit rl;
            getrlimit(RLIMIT_CPU, &rl);
            if (rl.rlim_max == RLIM_INFINITY || (rlim_t)cpu_lim < rl.rlim_max){
                rl.rlim_cur = cpu_lim;
                if (setrlimit(RLIMIT_CPU, &rl) == -1)
                    printf("WARNING! Could not set resource limit: CPU-time.\n");
            } }

        // Set limit on virtual memory:
        if (mem_lim != INT32_MAX){
            rlim_t new_mem_lim = (rlim_t)mem_lim * 1024*1024;
            rlimit rl;
            getrlimit(RLIMIT_AS, &rl);
            if (rl.rlim_max == RLIM_INFINITY || new_mem_lim < rl.rlim_max){
                rl.rlim_cur = new_mem_lim;
                if (setrlimit(RLIMIT_AS, &rl) == -1)
                    printf("WARNING! Could not set resource limit: Virtual memory.\n");
            } }
        
        if (argc == 1)
            printf("Reading from standard input... Use '--help' for help.\n");
        
        gzFile in = (argc == 1) ? gzdopen(0, "rb") : gzopen(argv[1], "rb");
        if (in == NULL)
            printf("ERROR! Could not open file: %s\n", argc == 1 ? "<stdin>" : argv[1]), exit(1);
        
        if (S.verbosity > 0){
            printf("============================[ Problem Statistics ]=============================\n");
            printf("|                                                                             |\n"); }
        
        parse_DIMACS(in, S);
        gzclose(in);
        FILE* res = (argc >= 3) ? fopen(argv[2], "wb") : NULL;
        
        if (S.verbosity > 0){
            printf("|  Number of variables:  %12d                                         |\n", S.nVars());
            printf("|  Number of clauses:    %12d                                         |\n", S.nClauses()); }
        
        double parsed_time = cpuTime();
        if (S.verbosity > 0){
            printf("|  Parse time:           %12.2f s                                       |\n", parsed_time - initial_time);
            printf("|                                                                             |\n"); }
 
        // Change to signal-handlers that will only notify the solver and allow it to terminate
        // voluntarily:
        signal(SIGINT, SIGINT_interrupt);
        signal(SIGXCPU,SIGINT_interrupt);
       
        if (!S.simplify()){
            if (res != NULL) fprintf(res, "UNSAT\n"), fclose(res);
            if (S.verbosity > 0){
                printf("===============================================================================\n");
                printf("Solved by unit propagation\n");
                printStats(S);
                printf("\n"); }
            printf("UNSATISFIABLE\n");
            exit(20);
        }
        
        vec<Lit> dummy;
        lbool ret = S.solveLimited(dummy);
        if (S.verbosity > 0){
            printStats(S);
            printf("\n"); }
        printf(ret == l_True ? "SATISFIABLE\n" : ret == l_False ? "UNSATISFIABLE\n" : "INDETERMINATE\n");
        if (res != NULL){
            if (ret == l_True){
                fprintf(res, "SAT\n");
                for (int i = 0; i < S.nVars(); i++)
                    if (S.model[i] != l_Undef)
                        fprintf(res, "%s%s%d", (i==0)?"":" ", (S.model[i]==l_True)?"":"-", i+1);
                fprintf(res, " 0\n");
            }else if (ret == l_False)
                fprintf(res, "UNSAT\n");
            else
                fprintf(res, "INDET\n");
            fclose(res);
        }
        
#ifdef NDEBUG
        exit(ret == l_True ? 10 : ret == l_False ? 20 : 0);     // (faster than "return", which will invoke the destructor for 'Solver')
#else
        return (ret == l_True ? 10 : ret == l_False ? 20 : 0);
#endif
    } catch (OutOfMemoryException&){
        printf("===============================================================================\n");
        printf("INDETERMINATE\n");
        exit(0);
    }
}*/
bool minisat(vector< vector< int > > & in, vector<bool>& vb, vector<int>& conflict)
{
	
	//assert(in);
	try {
		Solver      S;
		S.verbosity = 0;
		IntOption    verb   ("MAIN", "verb",   "Verbosity level (0=silent, 1=some, 2=more).", 0, IntRange(0, 2));
		IntOption    cpu_lim("MAIN", "cpu-lim","Limit on CPU time allowed in seconds.\n", INT32_MAX, IntRange(0, INT32_MAX));
		IntOption    mem_lim("MAIN", "mem-lim","Limit on memory usage in megabytes.\n", INT32_MAX, IntRange(0, INT32_MAX));
		
/*		// Set limit on CPU-time:
        if (cpu_lim != INT32_MAX){
            rlimit rl;
            getrlimit(RLIMIT_CPU, &rl);
            if (rl.rlim_max == RLIM_INFINITY || (rlim_t)cpu_lim < rl.rlim_max){
                rl.rlim_cur = cpu_lim;
                if (setrlimit(RLIMIT_CPU, &rl) == -1)
                    printf("WARNING! Could not set resource limit: CPU-time.\n");
            } }
		
        // Set limit on virtual memory:
        if (mem_lim != INT32_MAX){
            rlim_t new_mem_lim = (rlim_t)mem_lim * 1024*1024;
            rlimit rl;
            getrlimit(RLIMIT_AS, &rl);
            if (rl.rlim_max == RLIM_INFINITY || new_mem_lim < rl.rlim_max){
                rl.rlim_cur = new_mem_lim;
                if (setrlimit(RLIMIT_AS, &rl) == -1)
                    printf("WARNING! Could not set resource limit: Virtual memory.\n");
            } }
		
*/		

		double cpu_time = cpuTime();
		
		solver = &S;
		//signal(SIGINT,SIGINT_handler);
		//signal(SIGHUP,SIGINT_handler);
//		fprintf(stdout,"No variables  %d\n",(int)in.size());//return vb;

		parse_DIMACS_main(in, S);
		//gzclose(in);
		
		/*if (!S.simplify()){
			//return NULL;//(0); // UNSAT
			std::cout<<"formula not simplified";
			fprintf(stdout,"UNSAT\nConflicting clause");
			//vb=NULL;
			conflict= vector<int>(S.conflict.size());
			for (int i = 0; i < S.conflict.size(); i++)
				//if (S.conflict[i] != l_Undef)
			{
				conflict.at(i)=toInt(S.conflict[i]);
				//fprintf(stdout, "%s%s%d", (i==0)?"":" ", (S.model[i]==l_True)?"":"-", i+1);
				//vb->at(i)=toInt(S.model[i])-1;
				std::cout<< conflict.at(i)<<std::endl;
			}
			return false;
		}*/
		double initial_time = cpuTime();
		
		S.verbosity = verb;
		
		//solver = &S;
		// Use signal handlers that forcibly quit until the solver will be able to respond to
		// interrupts:
		signal(SIGINT, SIGINT_exit);
		//signal(SIGXCPU,SIGINT_exit);
		
/*		// Set limit on CPU-time:
		if (cpu_lim != INT32_MAX){
			rlimit rl;
			getrlimit(RLIMIT_CPU, &rl);
			if (rl.rlim_max == RLIM_INFINITY || (rlim_t)cpu_lim < rl.rlim_max){
				rl.rlim_cur = cpu_lim;
				if (setrlimit(RLIMIT_CPU, &rl) == -1)
					printf("WARNING! Could not set resource limit: CPU-time.\n");
			} }
		
		// Set limit on virtual memory:
		if (mem_lim != INT32_MAX){
			rlim_t new_mem_lim = (rlim_t)mem_lim * 1024*1024;
			rlimit rl;
			getrlimit(RLIMIT_AS, &rl);
			if (rl.rlim_max == RLIM_INFINITY || new_mem_lim < rl.rlim_max){
				rl.rlim_cur = new_mem_lim;
				if (setrlimit(RLIMIT_AS, &rl) == -1)
					printf("WARNING! Could not set resource limit: Virtual memory.\n");
			} }
			
		double parsed_time = cpuTime();
*/		
		// Change to signal-handlers that will only notify the solver and allow it to terminate
		// voluntarily:
		signal(SIGINT, SIGINT_interrupt);
		//signal(SIGXCPU,SIGINT_interrupt);
//		fprintf(stdout,"No variables  %d\n",S.nVars());//return vb;
		
		if (!S.simplify()){
			//if (res != NULL) 
			fprintf(stdout,"UNSAT\n");// fclose(res);
			if (S.verbosity > 0){
				//printf("===============================================================================\n");
				//fprintf(stdout,"Solved by unit propagation\n");
				//printStats(S);
				fprintf(stdout,"\n"); }
			fprintf(stdout,"MINISAT: UNSATISFIABLE, %d\n",S.conflict.size());
			conflict= vector<int>(S.conflict.size());
			for (int i = 0; i < S.conflict.size(); i++)
				//if (S.conflict[i] != l_Undef)
			{
				conflict.at(i)=toInt(S.conflict[i]);
				//fprintf(stdout, "%s%s%d", (i==0)?"":" ", (S.model[i]==l_True)?"":"-", i+1);
				//vb->at(i)=toInt(S.model[i])-1;
				std::cout<< conflict.at(i)<<std::endl;
			}
			return false;//exit(20);
		}
		
		vec<Lit> dummy;
		lbool ret = S.solveLimited(dummy);
		if (S.verbosity > 0){
			printStats(S);
			fprintf(stdout,"\n"); }
		printf(ret == l_True ? "MINISAT: SATISFIABLE\n" : ret == l_False ? "MINISAT: UNSATISFIABLE\n" : "INDETERMINATE\n");
		//if (res != NULL){

		if (ret == l_True){
			//conflict=NULL;
			fprintf(stdout,"SAT\n");//vector<bool> *
			vb=vector<bool> (S.model.size());
			for (int i = 0; i < S.nVars(); i++)
				if (S.model[i] != l_Undef){
					//fprintf(stdout, "%s%s%d", (i==0)?"":" ", (S.model[i]==l_True)?"":"-", i+1);
					vb.at(i)=toInt(S.model[i])-1;std::cout<< vb.at(i)<<std::endl;
				}
				else vb.at(i)=-1;
			return true;
		}
		else if (ret == l_False){
			fprintf(stdout,"MINISAT: UNSAT\nConflicting clause");
			//vb=NULL;
			conflict= vector<int>(S.conflict.size());
			for (int i = 0; i < S.conflict.size(); i++)
				//if (S.conflict[i] != l_Undef)
			{
					conflict.at(i)=toInt(S.conflict[i]);
					//fprintf(stdout, "%s%s%d", (i==0)?"":" ", (S.model[i]==l_True)?"":"-", i+1);
					//vb->at(i)=toInt(S.model[i])-1;
					std::cout<< conflict.at(i)<<std::endl;
				}
				//else vb->at(i)=-1;
			//fprintf(stdout," 0\n");//return vb;
			return false;
		}
		
		//return NULL;
			else
				fprintf(stdout, "INDET\n");return false;
		//	fclose(res);
		//}
		
		
	} catch (OutOfMemoryException&){
		printf("===============================================================================\n");
		printf("INDETERMINATE\n");
		exit(0);
	}
	
	
	//printf(" \n");
	/*for (int k=0; k<S.model.size(); ++k) {
	 lbool val=S.model[k];
	 printf("%d %d\n",k+1,val.toInt());
	 }*/
	//printing the clauses
	//return result;
}






//get a second solution which is different than the first
//solve 2 times
vector<bool>*  minisat2(vector< vector< int > > & in)
{
	//assert(in);
	
    Solver      S;
    S.verbosity = 1;
	
	

    double cpu_time = cpuTime();
	
    solver = &S;
    //signal(SIGINT,SIGINT_handler);
    //signal(SIGHUP,SIGINT_handler);
	
    parse_DIMACS_main(in, S);
    //gzclose(in);
	
    if (!S.simplify()){
        return NULL;//(0); // UNSAT
    }
	std::cout << "first solve"<<std::endl;
    int result=S.solve(); // result of SOLVE
	vector<bool> *vb=new vector<bool> (S.model.size());
	for (int i = 0; i < S.nVars(); i++)
		if (S.model[i] != l_Undef){
			//fprintf(stdout, "%s%s%d", (i==0)?"":" ", (S.model[i]==l_True)?"":"-", i+1);
			vb->at(i)=toInt(S.model[i])-1;std::cout<< vb->at(i)<<std::endl;
		}
		else vb->at(i)=-1;
	
	vec<Lit> cl;
	cl.clear();
	vec<lbool> cc;
	int mt;
	Lit ml;
	S.model.copyTo(cc);
	for (int r=0; r<cc.size(); ++r) {
		mt=toInt(cc[r]);
		//negate solution
		ml=cc[r]==l_True ?~mkLit(r): mkLit(r); 
		cl.push(ml);
	}
	S.addClause(cl);
	result = S.solve();
		std::cout << "second solve"<<std::endl;
	if(result){
		vector<bool> *vb=new vector<bool> (S.model.size());
		for (int k=0; k<S.model.size(); ++k) {
			vb->at(k)=toInt(S.model[k])-1;std::cout<< vb->at(k)<<std::endl;
			//vb->at(k)=bool(toInt(S.model[k])+1);
			//lbool val=S.model[k].;
			//printf(" %s", val);
		}
		return vb;
	}
	else {
		return NULL;
	}
}

//use simp solver perhaps eliminating variables 

vector<vector<bool>*>*  minisatsimp(vector< vector< int > > & in)
{
	//assert(in);
	
    SimpSolver      S;
    S.verbosity = 1;
	
	
    double cpu_time = cpuTime();
	
    solver = &S;
    //signal(SIGINT,SIGINT_handler);
    //signal(SIGHUP,SIGINT_handler);
	
    parse_DIMACS_main(in, S);
    //gzclose(in);
	
    if (!S.simplify()){
        return NULL;//(0); // UNSAT
    }
	
	vec<Lit> dummy;
	S.eliminate();
	S.toDimacs(stdout, dummy);
	int mt;
	Lit ml;
	bool ret = S.solve(dummy);vector<vector<bool> *>* x=NULL;
	int k=0;
	while (ret == true && k < 10 ){
		fprintf(stdout,"MINISAT: SAT\n");
		vector<bool> *vb=new vector<bool> (S.model.size());
		vec<Lit> cl;
		for (int i = 0; i < S.nVars(); i++)
			if (S.model[i] != l_Undef){
				//fprintf(stdout, "%s%s%d", (i==0)?"":" ", (S.model[i]==l_True)?"":"-", i+1);
				vb->at(i)=toInt(S.model[i])-1;//std::cout<< vb->at(i)<<std::endl;
				if(vb->at(i)) {
					Lit ml=mkLit(i,false);cl.push(ml);
				}
				else {
					Lit ml=mkLit(i,false);cl.push(ml);
				}
				
			}
			else {vb->at(i)=-1;Lit ml=mkLit(i,false);cl.push(ml);}
		fprintf(stdout," \n");x->push_back(vb);//return vb;
		S.addClause(cl);
		++k;
	}
	if (ret == false)
	{fprintf(stdout,"MINISAT: UNSAT\n");return NULL;}
	else return x;
	
	/* int result=S.solve(); // result of SOLVE
	 vector<vector<bool> *>* x;
	 while(result){
	 vec<Lit> cl;
	 cl.clear();
	 vec<lbool> cc;
	 int mt;
	 Lit ml;
	 S.model.copyTo(cc);
	 for (int r=0; r<cc.size(); ++r) {
	 mt=cc[r].toInt();
	 ml=cc[r]==l_True ?~Lit(r): Lit(r); 
	 cl.push(ml);
	 }
	 S.addClause(cl);
	 result = S.solve();
	 //vec<lbool> mod=S.model;
	 //printf(" %d ", S.nVars());
	 if(result){
	 vector<bool> *vb=new vector<bool> (S.model.size());
	 for (int k=0; k<S.model.size(); ++k) {
	 vb->at(k)=bool(S.model[k].toInt()+1);
	 //lbool val=S.model[k].;
	 std::cout<<" "<<vb->at(k);
	 }
	 x->push_back(vb);//return vb;
	 std::cout<<std::endl;
	 }
	 }
	 //else {
	 return NULL;
	 //}
	 
	 */
	//printf(" \n");
	/*for (int k=0; k<S.model.size(); ++k) {
	 lbool val=S.model[k];
	 printf("%d %d\n",k+1,val.toInt());
	 }*/
	//printing the clauses
	//return result;
}



//get all solutions 
//solve till thre are no more solutions
vector<vector<bool> *>*  minisatall(vector< vector< int > > & in)
{
	//assert(in);
	
    Solver      S;
    S.verbosity = 1;
	
	

    double cpu_time = cpuTime();
	
    solver = &S;
    //signal(SIGINT,SIGINT_handler);
    //signal(SIGHUP,SIGINT_handler);
	
    parse_DIMACS_main(in, S);
    //gzclose(in);
	
    if (!S.simplify()){
        return NULL;//(0); // UNSAT
    }
	
	/* int result=S.solve(); // result of SOLVE
	 vector<vector<bool> *>* x;
	 while(result){
	 vec<Lit> cl;
	 cl.clear();
	 vec<lbool> cc;
	 int mt;
	 Lit ml;
	 S.model.copyTo(cc);
	 for (int r=0; r<cc.size(); ++r) {
	 mt=cc[r].toInt();
	 ml=cc[r]==l_True ?~Lit(r): Lit(r); 
	 cl.push(ml);
	 }
	 S.addClause(cl);
	 result = S.solve();
	 //vec<lbool> mod=S.model;
	 //printf(" %d ", S.nVars());
	 if(result){
	 vector<bool> *vb=new vector<bool> (S.model.size());
	 for (int k=0; k<S.model.size(); ++k) {
	 vb->at(k)=bool(S.model[k].toInt()+1);
	 //lbool val=S.model[k].;
	 std::cout<<" "<<vb->at(k);
	 }
	 x->push_back(vb);//return vb;
	 std::cout<<std::endl;
	 }
	 }
	 //else {
	 return NULL;
	 //}
	 
	 */
	//printf(" \n");
	/*for (int k=0; k<S.model.size(); ++k) {
	 lbool val=S.model[k];
	 printf("%d %d\n",k+1,val.toInt());
	 }*/
	//printing the clauses
	//return result;
}



