#include <errno.h>

#include <map>
#include <list>
#include <set>

#include <InputOutput/InputOutput.h>
#include <InputOutput/Reporter.h>
#include <Planning/Task.h>
#include <cmdline.h>

#include <Parser/ast-system-k.h>
#include <Parser/ast-system-rk.h>
#include <Parser/ast-system-unpk.h>

#include <Net/Net.h>
#include <Net/Marking.h>
#include <Net/NetState.h>
#include <Net/Transition.h>

#include <Exploration/DFSExploration.h>
#include <Exploration/ParallelExploration.h>

#include <Exploration/RandomWalk.h>
#include <Exploration/Firelist.h>
#include <Exploration/SimpleProperty.h>
#include <Exploration/Deadlock.h>
#include <Exploration/FirelistStubbornDeadlock.h>
#include <Exploration/FirelistStubbornStatePredicate.h>
#include <Exploration/StatePredicateProperty.h>
#include <Exploration/ChooseTransition.h>
#include <Exploration/ChooseTransitionHashDriven.h>

#include <Formula/StatePredicate.h>

#include <Stores/NetStateEncoder/BitEncoder.h>
#include <Stores/NetStateEncoder/CopyEncoder.h>
#include <Stores/NetStateEncoder/FullCopyEncoder.h>
#include <Stores/NetStateEncoder/SimpleCompressedEncoder.h>
#include <Stores/VectorStores/SuffixTreeStore.h>
#include <Stores/VectorStores/VSTLStore.h>
#include <Stores/VectorStores/VBloomStore.h>
#include <Stores/PluginStore.h>
#include <Stores/EmptyStore.h>
#include <Stores/CompareStore.h>
#include <Stores/Store.h>

#include <Witness/Condition.h>
#include <Witness/Event.h>

extern gengetopt_args_info args_info;
extern Reporter* rep;

// the parsers
extern int ptformula_parse();
extern int ptformula_lex_destroy();

// input file
extern FILE* ptformula_in;

// code to parse from a string
struct yy_buffer_state;
typedef yy_buffer_state* YY_BUFFER_STATE;
extern YY_BUFFER_STATE ptformula__scan_string(const char* yy_str);
extern void ptformula__delete_buffer(YY_BUFFER_STATE);

/// printer-function for Kimiwtu's output on stdout
// LCOV_EXCL_START
void myprinter(const char* s, kc::uview v)
{
    printf("%s", s);
}
// LCOV_EXCL_STOP

extern kc::tFormula TheFormula;

Task::Task() : sp(NULL), ns(NULL), p(NULL), s(NULL), fl(NULL), exploration(NULL), choose(NULL), search(args_info.search_arg), number_of_threads(args_info.threads_arg)
{
    setFormula();
    setNet();
}

Task::~Task()
{
    delete ns;
    delete s;
    delete p;
    delete sp;
    delete fl;
    delete exploration;
}

void Task::setNet()
{
    ns = NetState::createNetStateFromInitial();
    Transition::checkTransitions(*ns);
}

void Task::setFormula()
{
    if (not args_info.formula_given)
    {
        return;
    }

    StatePredicate* result = NULL;
    Input* formulaFile = NULL;

    // Check if the paramter of --formula is a file that we can open: if that
    // works, parse the file. If not, parse the string.
    FILE* file;
    if ((file = fopen(args_info.formula_arg, "r")) == NULL and errno == ENOENT)
    {
        YY_BUFFER_STATE my_string_buffer = ptformula__scan_string(args_info.formula_arg);
    }
    else
    {
        fclose(file);
        formulaFile = new Input("formula", args_info.formula_arg);
        ptformula_in = *formulaFile;
    }

    // parse the formula
    ptformula_parse();

    // restructure the formula: remove arrows and handle negations and tautologies
    TheFormula = TheFormula->rewrite(kc::arrows);
    TheFormula = TheFormula->rewrite(kc::neg);

    // check temporal status
    TheFormula->unparse(myprinter, kc::temporal);
    formulaType = TheFormula->type;

    switch (formulaType)
    {
    case (FORMULA_REACHABLE):
        rep->status("checking reachability");
        TheFormula = TheFormula->rewrite(kc::reachability);
        break;
    case (FORMULA_INVARIANT):
        rep->status("checking invariance");
        TheFormula = TheFormula->rewrite(kc::reachability);
        break;
    case (FORMULA_IMPOSSIBLE):
        rep->status("checking impossibility");
        TheFormula = TheFormula->rewrite(kc::reachability);
        break;
    case (FORMULA_LIVENESS):
        rep->status("checking liveness");
        break;
    case (FORMULA_FAIRNESS):
        rep->status("checking fairness");
        break;
    case (FORMULA_STABILIZATION):
        rep->status("checking stabilization");
        break;
    case (FORMULA_EVENTUALLY):
        rep->status("checking eventual occurrence");
        break;
    case (FORMULA_INITIAL):
        rep->status("checking initial satisfiability");
        break;
    case (FORMULA_MODELCHECKING):
        rep->status("checking CTL");
        break;
    }

    // restructure the formula: again tautoglies and simplification
    TheFormula = TheFormula->rewrite(kc::neg);
    TheFormula = TheFormula->rewrite(kc::leq);
    TheFormula = TheFormula->rewrite(kc::sides);
    TheFormula = TheFormula->rewrite(kc::lists);

    // debug
    // TheFormula->print();
    // TheFormula->unparse(myprinter, kc::out);

    if (formulaType == FORMULA_REACHABLE or
            formulaType == FORMULA_INVARIANT or
            formulaType == FORMULA_IMPOSSIBLE or
            formulaType == FORMULA_INITIAL)
    {
        // copy restructured formula into internal data structures
        TheFormula->unparse(myprinter, kc::internal);
        result = TheFormula->formula;
    }
    else
    {
        rep->message("check not yet implemented");
        rep->abort(ERROR_COMMANDLINE);
    }

    assert(result);
    rep->status("processed formula with %d atomic propositions", result->countAtomic());

    // tidy parser
    ptformula_lex_destroy();
    //delete TheFormula;

    if (args_info.formula_given)
    {
        delete formulaFile;
    }

    sp = result;
}

void Task::setStore()
{
    if (args_info.search_arg == search_arg_findpath)
    {
        s = new EmptyStore<void>(number_of_threads);
    }
    else
    {
        // choose a store
        switch (args_info.store_arg)
        {
        case store_arg_comp:
            s = new CompareStore<void>(
            		new PluginStore<void>(new BitEncoder(number_of_threads), new SuffixTreeStore<void>(), number_of_threads),
            		new PluginStore<void>(new BitEncoder(number_of_threads), new VSTLStore<void>(number_of_threads), number_of_threads),
            		number_of_threads
            		);
            break;
        case store_arg_psbbin:
            s = new PluginStore<void>(new BitEncoder(number_of_threads), new SuffixTreeStore<void>(), number_of_threads);
            break;
        case store_arg_pscbin:
            s = new PluginStore<void>(new CopyEncoder(number_of_threads), new SuffixTreeStore<void>(), number_of_threads);
            break;
        case store_arg_psfbin:
            s = new PluginStore<void>(new FullCopyEncoder(number_of_threads), new SuffixTreeStore<void>(), number_of_threads);
            break;
        case store_arg_pssbin:
            s = new PluginStore<void>(new SimpleCompressedEncoder(number_of_threads), new SuffixTreeStore<void>(), number_of_threads);
            break;
        case store_arg_psbstl:
            s = new PluginStore<void>(new BitEncoder(number_of_threads), new VSTLStore<void>(number_of_threads), number_of_threads);
            break;
        case store_arg_pscstl:
            s = new PluginStore<void>(new CopyEncoder(number_of_threads), new VSTLStore<void>(number_of_threads), number_of_threads);
            break;
        case store_arg_psfstl:
            s = new PluginStore<void>(new FullCopyEncoder(number_of_threads), new VSTLStore<void>(number_of_threads), number_of_threads);
            break;
        case store_arg_pssstl:
            s = new PluginStore<void>(new SimpleCompressedEncoder(number_of_threads), new VSTLStore<void>(number_of_threads), number_of_threads);
            break;
        case store_arg_psbbloom:
            s = new PluginStore<void>(new BitEncoder(number_of_threads), new VBloomStore(number_of_threads, args_info.hashfunctions_arg), number_of_threads);
            break;
        case store_arg_pscbloom:
            s = new PluginStore<void>(new CopyEncoder(number_of_threads), new VBloomStore(number_of_threads, args_info.hashfunctions_arg), number_of_threads);
            break;
        case store_arg_pssbloom:
            s = new PluginStore<void>(new SimpleCompressedEncoder(number_of_threads), new VBloomStore(number_of_threads, args_info.hashfunctions_arg), number_of_threads);
            break;
        }
    }
}

void Task::setProperty()
{
    // choose a simple property
    switch (args_info.check_arg)
    {
        //        case check_arg_none:
        //            return EXIT_SUCCESS;

    case check_arg_full:
        p = new SimpleProperty();
        fl = new Firelist();
        break;

    case check_arg_deadlock:
        p = new Deadlock();
        fl = new FirelistStubbornDeadlock();
        break;

    case check_arg_statepredicate:
        p = new StatePredicateProperty(sp);
        fl = new FirelistStubbornStatePredicate(sp);
        break;
    }


    // set the correct exploration algorithm
    switch (args_info.check_arg)
    {
    case check_arg_full:
    case check_arg_deadlock:
    case check_arg_statepredicate:
        if (number_of_threads == 1)
        {
            exploration = new DFSExploration();
        }
        else
        {
            exploration = new ParallelExploration();
        }
        break;
        // now there is only one, but who knows...
    }
}

bool Task::getResult()
{
    assert(ns);
    assert(s);
    assert(p);
    assert(exploration);
    assert(fl);

    bool result;
    switch (args_info.search_arg)
    {
    case search_arg_depth:
        result = exploration->depth_first(*p, *ns, *s, *fl, number_of_threads);
        break;

    case search_arg_findpath:
        if (args_info.retrylimit_arg == 0)
        {
            rep->status("starting infinite tries of depth %d", args_info.depthlimit_arg);
        }
        else
        {
            rep->status("starting at most %d tries of depth %d", args_info.retrylimit_arg, args_info.depthlimit_arg);
        }

        choose = new ChooseTransitionHashDriven();
        result = exploration->find_path(*p, *ns, args_info.retrylimit_arg, args_info.depthlimit_arg, *fl, *((EmptyStore<void>*)s), *choose);
        delete choose;
        break;

    default:
        assert(false);
        break;
    }

    return result;
}

void Task::interpreteResult(bool result)
{
    // in case AG is used, the result needs to be negated
    if (formulaType == FORMULA_INVARIANT or formulaType == FORMULA_IMPOSSIBLE)
    {
        result = not result;
    }

    // make result three-valued
    trinary_t final_result = result ? TRINARY_TRUE : TRINARY_FALSE;

    // if the Bloom store did not find anything, the result is unknown
    if (args_info.store_arg == store_arg_bloom)
    {
        switch (args_info.check_arg)
        {
        case (check_arg_deadlock):
        case (check_arg_statepredicate):
            if (not result)
            {
                final_result = TRINARY_UNKNOWN;
            }
            break;
        }
    }

    switch (final_result)
    {
    case (TRINARY_TRUE):
        rep->message("result: %s", rep->markup(MARKUP_GOOD, "yes").str());
        break;
    case (TRINARY_FALSE):
        rep->message("result: %s", rep->markup(MARKUP_BAD, "no").str());
        break;
    case (TRINARY_UNKNOWN):
        rep->message("result: %s", rep->markup(MARKUP_WARNING, "unknown").str());
        break;
    }
}

void Task::printWitness()
{
    rep->message("%s", rep->markup(MARKUP_IMPORTANT, "witness path:").str());
    index_t c;
    index_t* f;
    while (p->stack.StackPointer > 0)
    {
        SimpleStackEntry & s = p->stack.top();
        rep->message("%s", Net::Name[TR][s.fl[s.current]]);
        p->stack.pop();
    }
}

void Task::printMarking()
{
    rep->message("%s", rep->markup(MARKUP_IMPORTANT, "witness state:").str());
    for (index_t p = 0; p < Net::Card[PL]; ++p)
    {
        if (ns->Current[p] > 0)
        {
            rep->message("%s : %d", Net::Name[PL][p], ns->Current[p]);
        }
    }
}


NetState* Task::getNetState()
{
    return ns;
}


void Task::printDot()
{
    FILE* o = stdout;

    // add conditions for initial marking
    for (index_t i = 0; i < Net::Card[PL]; ++i)
    {
        if (Marking::Initial[i] > 0)
        {
            new Condition(i);
        }
    }

    // process the witness path
    std::list<index_t> path;
    index_t c;
    index_t* f;
    while (p->stack.StackPointer > 0)
    {
        SimpleStackEntry & s = p->stack.top();
        path.push_front(s.fl[s.current]);
        p->stack.pop();
    }

    // add all events (with surrounding conditions)
    for (std::list<index_t>::iterator it = path.begin(); it != path.end(); ++it)
    {
        new Event(*it);
    }

    // if a tranision is the target, add it
    extern std::set<index_t> target_transition;
    for (std::set<index_t>::iterator it = target_transition.begin(); it != target_transition.end(); ++it)
    {
        Event* e = new Event(*it);
        e->target = true;
    }

    // actual dot output
    fprintf(o, "digraph d {\n");
    fprintf(o, "  rankdir=LR;\n\n");
    Event::dot(o);
    Condition::dot(o);
    fprintf(o, "}\n");
}


#include <sstream>
/*!
 * \brief this method is only to test Store::popState() until its use case is implemented
 * \todo  remove me!
 */
void Task::testPopState()
{
  rep->message("%s", rep->markup(MARKUP_IMPORTANT, "iterating over all stored states:").str());
  uint64_t counter = 0;

  while(s->popState(*ns))
  {
    // increment counter
    ++counter;
    std::stringstream msg;
    for (index_t p = 0; p < Net::Card[PL]; ++p)
    {
        if (ns->Current[p] > 0)
        {
            msg << Net::Name[PL][p] << ": " << ns->Current[p] << ", ";
        }
    }
    rep->message("[State] %s", msg.str().c_str());
  }

  rep->message("%s", rep->markup(MARKUP_IMPORTANT, "states collected: %u", counter).str());
}
