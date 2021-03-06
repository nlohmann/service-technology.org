/******************************************************************************************[Main.C]
MiniSat -- Copyright (c) 2003-2006, Niklas Een, Niklas Sorensson

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

//#include <signal.h>
//#include <zlib.h>

#include <vector>
using std::vector;

#include "Solver.h"
#include "SolverTypes.h"
#include <cstdio>

#include <bitset>
#include <vector>

/*************************************************************************************/
// #ifdef _MSC_VER
#include <ctime>

static inline double cpuTime(void) {
    return (double)clock() / CLOCKS_PER_SEC; }
// #else
// 
// #include <sys/time.h>
// #include <sys/resource.h>
// #include <unistd.h>
// 
// static inline double cpuTime(void) {
//     struct rusage ru;
//     getrusage(RUSAGE_SELF, &ru);
//     return (double)ru.ru_utime.tv_sec + (double)ru.ru_utime.tv_usec / 1000000; }
// #endif


#if defined(__linux__)
static inline int memReadStat(int field)
{
    char    name[256];
    pid_t pid = getpid();
    sprintf(name, "/proc/%d/statm", pid);
    FILE*   in = fopen(name, "rb");
    if (in == NULL) return 0;
    int     value;
    for (; field >= 0; field--)
        fscanf(in, "%d", &value);
    fclose(in);
    return value;
}
static inline uint64_t memUsed() { return (uint64_t)memReadStat(0) * (uint64_t)getpagesize(); }


#elif defined(__FreeBSD__)
static inline uint64_t memUsed(void) {
    struct rusage ru;
    getrusage(RUSAGE_SELF, &ru);
    return ru.ru_maxrss*1024; }


#else
static inline uint64_t memUsed() { return 0; }
#endif

#if defined(__linux__)
#include <fpu_control.h>
#endif

//=================================================================================================
// DIMACS Parser:

#define CHUNK_LIMIT 1048576

/*
class StreamBuffer {
    gzFile  in;
    char    buf[CHUNK_LIMIT];
    int     pos;
    int     size;

    void assureLookahead() {
        if (pos >= size) {
            pos  = 0;
            size = gzread(in, buf, sizeof(buf)); } }

public:
    StreamBuffer(gzFile i) : in(i), pos(0), size(0) {
        assureLookahead(); }

    int  operator *  () { return (pos >= size) ? EOF : buf[pos]; }
    void operator ++ () { pos++; assureLookahead(); }
};
*/

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

template<class B>
static void skipWhitespace(B& in) {
    while ((*in >= 9 && *in <= 13) || *in == 32)
        ++in; }

template<class B>
static void skipLine(B& in) {
    for (;;){
        if (*in == EOF || *in == '\0') return;
        if (*in == '\n') { ++in; return; }
        ++in; } }

template<class B>
static int parseInt(B& in) {
    int     val = 0;
    bool    neg = false;
    skipWhitespace(in);
    if      (*in == '-') neg = true, ++in;
    else if (*in == '+') ++in;
    if (*in < '0' || *in > '9') reportf("PARSE ERROR! Unexpected char: %c\n", *in), exit(3);
    while (*in >= '0' && *in <= '9')
        val = val*10 + (*in - '0'),
        ++in;
    return neg ? -val : val; }

//template<class B>
//static void readClause(B& in, Solver& S, vec<Lit>& lits) {
static void readClause(vector< int > &  in, Solver& S, vec<Lit>& lits) {
    int     parsed_lit, var1;
    lits.clear();

    int i = 0;
    while (i < in.size())
    {
        parsed_lit = in[i];
        if (parsed_lit == 0) break;
        var1 = abs(parsed_lit) - 1;
        while (var1 >= S.nVars()) S.newVar();
        Lit l = Lit(var1);
        lits.push( (parsed_lit > 0) ? Lit(var1) : ~Lit(var1) );
        ++i;
    }
/*
    for (;;){
        parsed_lit = parseInt(in);
        if (parsed_lit == 0) break;
        var = abs(parsed_lit)-1;
        while (var >= S.nVars()) S.newVar();
        lits.push( (parsed_lit > 0) ? Lit(var) : ~Lit(var) );
    }
*/
}

template<class B>
static bool match(B& in, char* str) {
    for (; *str != 0; ++str, ++in)
        if (*str != *in)
            return false;
    return true;
}


//template<class B>
static void parse_DIMACS_main(vector< vector< int > > & in, Solver& S) {
    vec<Lit> lits;

    for (int i = 0; i < in.size(); ++i)
    {
        readClause(in[i], S, lits),
        S.addClause(lits);
    }
/*
    for (;;){
        skipWhitespace(in);
        if (*in == EOF)
            break;
        else
            readClause(in, S, lits),
            S.addClause(lits);
    }
*/
}

// Inserts problem into solver.
//
/*
static void parse_DIMACS(gzFile input_stream, Solver& S) {
    StreamBuffer in(input_stream);
    parse_DIMACS_main(in, S); }
*/

//=================================================================================================


void printStats(Solver& solver)
{
    double   cpu_time = cpuTime();
    uint64_t mem_used = memUsed();
    reportf("restarts              : %lld\n", solver.starts);
    reportf("conflicts             : %-12lld   (%.0f /sec)\n", solver.conflicts   , solver.conflicts   /cpu_time);
    reportf("decisions             : %-12lld   (%4.2f %% random) (%.0f /sec)\n", solver.decisions, (float)solver.rnd_decisions*100 / (float)solver.decisions, solver.decisions   /cpu_time);
    reportf("propagations          : %-12lld   (%.0f /sec)\n", solver.propagations, solver.propagations/cpu_time);
    reportf("conflict literals     : %-12lld   (%4.2f %% deleted)\n", solver.tot_literals, (solver.max_literals - solver.tot_literals)*100 / (double)solver.max_literals);
    if (mem_used != 0) reportf("Memory used           : %.2f MB\n", mem_used / 1048576.0);
    reportf("CPU time              : %g s\n", cpu_time);
}

Solver* solver;
//static void SIGINT_handler(int signum) {
//    reportf("\n"); reportf("*** INTERRUPTED ***\n");
//    printStats(*solver);
//    reportf("\n"); reportf("*** INTERRUPTED ***\n");
//    exit(1); }


//=================================================================================================
// Main:

void printUsage(char** argv)
{
    reportf("USAGE: %s [options] <input-file> <result-output-file>\n\n  where input may be either in plain or gzipped DIMACS.\n\n", argv[0]);
    reportf("OPTIONS:\n\n");
    reportf("  -polarity-mode = {true,false,rnd}\n");
    reportf("  -decay         = <num> [ 0 - 1 ]\n");
    reportf("  -rnd-freq      = <num> [ 0 - 1 ]\n");
    reportf("  -verbosity     = {0,1,2}\n");
    reportf("\n");
}


const char* hasPrefix(const char* str, const char* prefix)
{
    int len = strlen(prefix);
    if (strncmp(str, prefix, len) == 0)
        return str + len;
    else
        return NULL;
}


//int minisat(gzFile in)
int minisat(vector< vector< int > > & in)
{
  //assert(in);

    Solver      S;
    S.verbosity = 1;

#if defined(__linux__)
    fpu_control_t oldcw, newcw;
    _FPU_GETCW(oldcw); newcw = (oldcw & ~_FPU_EXTENDED) | _FPU_DOUBLE; _FPU_SETCW(newcw);
    reportf("WARNING: for repeatability, setting FPU to use double precision\n");
#endif
    double cpu_time = cpuTime();

    solver = &S;
//    signal(SIGINT,SIGINT_handler);
//    signal(SIGHUP,SIGINT_handler);

    parse_DIMACS_main(in, S);
    //gzclose(in);

    if (!S.simplify()){
        return(0); // UNSAT
    }

    return S.solve(); // result of SOLVE
}

vector<bool>* minisat2(vector< vector< int > > & in){
  //assert(in);

    Solver* S = new Solver();
    S->verbosity = 1;

#if defined(__linux__)
    fpu_control_t oldcw, newcw;
    _FPU_GETCW(oldcw); newcw = (oldcw & ~_FPU_EXTENDED) | _FPU_DOUBLE; _FPU_SETCW(newcw);
    reportf("WARNING: for repeatability, setting FPU to use double precision\n");
#endif
    double cpu_time = cpuTime();

    solver = S;
//    signal(SIGINT,SIGINT_handler);
//    signal(SIGHUP,SIGINT_handler);

    parse_DIMACS_main(in, *S);
    //gzclose(in);



    if (!S->simplify()){
    	delete S;
        return NULL; // UNSAT
    }

    if(S->solve()){  //if there is a satisfying assignment
    	assert(S->model);
    	vector<bool> *assignment = new vector<bool>(S->model.size()+1);
//    	printf("minisat2(): satisfying assignment: ");

    	for(unsigned int i = 0; i < S->model.size(); ++i) {
//    		printf("%d  ", toInt(S->model[i]));
    		//assignment->at(i) = bool(toInt(S->model[i])+1);  //-1 has the meaning of false; 1 has the meaning og true
    		if (toInt(S->model[i]) == -1){ //-1 has the meaning of false; 1 has the meaning og true
    			assignment->at(i+1) = false;
    		}
    		else{
    			assert(toInt(S->model[i]) == 1);
    			assignment->at(i+1) = true;
    		}
    	}
//    	printf("\n");
    	delete S;
    	return assignment;
    }
    else {
//    	printf("minisat2(): no satisfying assignment\n");
    	delete S;
    	return NULL;
    }

}
