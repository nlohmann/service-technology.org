#include <errno.h>
#include <iostream>
#include <sstream>
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
#include <Parser/SymbolTable.h>

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
#include <Exploration/FirelistStubbornDeletion.h>
#include <Exploration/FirelistStubbornStatePredicate.h>
#include <Exploration/StatePredicateProperty.h>
#include <Exploration/ChooseTransition.h>
#include <Exploration/ChooseTransitionHashDriven.h>

#include <Formula/AtomicStatePredicate.h>
#include <Formula/StatePredicate.h>
#include <Formula/TruePredicate.h>
#include <Formula/ConjunctionStatePredicate.h>
#include <Formula/DisjunctionStatePredicate.h>

#include <Stores/Store.h>

#include <SweepLine/SweepEmptyStore.h>

#include <Witness/Condition.h>
#include <Witness/Event.h>

#include <Formula/LTL/BuechiFromLTL.h>

extern gengetopt_args_info args_info;
extern Reporter *rep;

// the parsers
extern int ptformula_parse();
extern int ptformula_lex_destroy();
// the parsers II
extern int ptbuechi_parse();
extern int ptbuechi_lex_destroy();

// input file
extern FILE *ptformula_in;
// input file
extern FILE *ptbuechi_in;

// code to parse from a string
struct yy_buffer_state;
typedef yy_buffer_state *YY_BUFFER_STATE;
extern YY_BUFFER_STATE ptformula__scan_string(const char *yy_str);
extern void ptformula__delete_buffer(YY_BUFFER_STATE);

// code to parse from a string
extern YY_BUFFER_STATE ptbuechi__scan_string(const char *yy_str);
extern void ptbuechi__delete_buffer(YY_BUFFER_STATE);

extern SymbolTable *buechiStateTable;

std::map<int, AtomicStatePredicate *> predicateMap;
extern FILE	*tl_out;

/// printer-function for Kimiwtu's output on stdout
// LCOV_EXCL_START
void myprinter(const char *s, kc::uview v)
{
    printf("%s", s);
}
// LCOV_EXCL_STOP

/// special store creation for stores without payload support (e.g. BloomStore)
template<>
Store<void> *StoreCreator<void>::createSpecializedStore(int number_of_threads)
{
    NetStateEncoder *enc = 0;
    switch (args_info.encoder_arg)
    {
    case encoder_arg_bit:
        enc = new BitEncoder(number_of_threads);
        break;
    case encoder_arg_copy:
        enc = new CopyEncoder(number_of_threads);
        break;
    case encoder_arg_simplecompressed:
        enc = new SimpleCompressedEncoder(number_of_threads);
        break;
    case encoder_arg_fullcopy:
        enc = new FullCopyEncoder(number_of_threads);
        break;
    case encoder__NULL:
        break;
    }

    switch (args_info.store_arg)
    {
    case store_arg_bloom:
        if (args_info.bucketing_given)
        {
            return new PluginStore<void>(enc,
                                         new HashingWrapperStore<void>(new BinaryVectorStoreCreator < void,
                                                 VBloomStore < BLOOM_FILTER_SIZE / SIZEOF_MARKINGTABLE + 1 > , index_t,
                                                 size_t > (number_of_threads, args_info.hashfunctions_arg)), number_of_threads);
        }
        else
        {
            return new PluginStore<void>(enc,
                                         new VBloomStore<BLOOM_FILTER_SIZE>(number_of_threads,
                                                 args_info.hashfunctions_arg), number_of_threads);
        }

    default:
        storeCreationError();
        return NULL;
    }
}

extern kc::tFormula TheFormula;
extern kc::tBuechiAutomata TheBuechi;

Task::Task() : spFormula(NULL), ctlFormula(NULL), bauto(NULL), ns(NULL),
    p(NULL), store(NULL), ctlStore(NULL), ltlStore(NULL), fl(NULL),
    exploration(NULL), ctlExploration(NULL), ltlExploration(NULL), choose(NULL),
    search(args_info.search_arg), number_of_threads(args_info.threads_arg),
    formulaType(FORMULA_REACHABLE)
{
    if (args_info.formula_given)
    {
        setFormula();
    }
    if (args_info.buechi_given)
    {
        setBuechiAutomata();
    }
    setNet();

    // prepare task
    setStore();
    setProperty();
}

Task::~Task()
{
    delete ns;
    delete store;
    delete ltlStore;
    delete ctlStore;
    delete p;
    delete spFormula;
    delete fl;
    delete exploration;
    delete ltlExploration;
    delete ctlExploration;
    delete bauto;
}

void Task::setNet()
{
    ns = NetState::createNetStateFromInitial();
    Transition::checkTransitions(*ns);
}


/* prints the content of a set for spin */
StatePredicate *buildPropertyFromList(int *pos, int *neg)
{
    int i, j = 1;
    std::vector<StatePredicate *> subForms;
    // bad hack from library
    int mod = 8 * sizeof(int);
    for (i = 0; i < sym_size; i++)
        for (j = 0; j < mod; j++)
        {
            if (pos[i] & (1 << j))
                // the compiler doens't have a clue whcih function i mean, so tell him
                if (atoi(sym_table[mod * i + j]) > 1)
                {
                    subForms.push_back(
                        predicateMap[atoi(sym_table[mod * i + j])]->StatePredicate::copy());
                }
            if (neg[i] & (1 << j))
                if (atoi(sym_table[mod * i + j]) > 1)
                {
                    subForms.push_back(predicateMap[atoi(sym_table[mod * i + j])]->negate());
                }
        }
    if (subForms.size() == 0)
    {
        return new TruePredicate();
    }
    ConjunctionStatePredicate *result = new ConjunctionStatePredicate(subForms.size());
    for (int i = 0 ; i < subForms.size(); i++)
    {
        result->addSub(i, subForms[i]);
    }
    return result;
}


void Task::setFormula()
{
    Input *formulaFile = NULL;

    // Check if the paramter of --formula is a file that we can open: if that
    // works, parse the file. If not, parse the string.
    FILE *file;
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

    //printf("FormulaType: %d\n",formulaType);
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
        rep->status("liveness not yet implemented, converting to CTL...");
        formulaType = FORMULA_CTL;
        break;
    case (FORMULA_FAIRNESS):
        rep->status("checking fairness");
        rep->status("fairness not yet implemented, converting to LTL...");
        formulaType = FORMULA_LTL;
        break;
    case (FORMULA_STABILIZATION):
        rep->status("checking stabilization");
        rep->status("stabilization not yet implemented, converting to LTL...");
        formulaType = FORMULA_LTL;
        break;
    case (FORMULA_EVENTUALLY):
        rep->status("checking eventual occurrence");
        rep->status("eventual occurrence not yet implemented, converting to LTL...");
        formulaType = FORMULA_LTL;
        break;
    case (FORMULA_INITIAL):
        rep->status("checking initial satisfiability");
        break;
    case (FORMULA_LTL):
        rep->status("checking LTL");
        break;
    case (FORMULA_CTL):
        rep->status("checking CTL");
        break;
    case (FORMULA_MODELCHECKING):
        rep->status("checking CTL*");
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

    switch (formulaType)
    {
    case FORMULA_REACHABLE:
    case FORMULA_INVARIANT:
    case FORMULA_INITIAL:
    {
        // copy restructured formula into internal data structures
        TheFormula->unparse(myprinter, kc::internal);
        StatePredicate *result = TheFormula->formula;

        assert(result);
        rep->status("processed formula with %d atomic propositions",
                    result->countAtomic());

        spFormula = result;
        break;
    }

    case FORMULA_CTL:
    {
        //TheFormula->unparse(myprinter, kc::out);
        TheFormula->unparse(myprinter, kc::ctl);
        ctlFormula = TheFormula->ctl_formula;

        // debug output
        //printf("Formula: ");
        //ctlFormula->DEBUG_print();
        //printf("\n");

        assert(ctlFormula);
        break;
    }

    case FORMULA_LTL:
    {
        rep->message("transforming LTL-Formula into an Büchi-Automaton");
        // print the formula
        TheFormula->unparse(myprinter, kc::out);
        // extract the Node*
        TheFormula->unparse(myprinter, kc::ltl);

        tl_Node *n = TheFormula->ltl_tree;
        //n = bin_simpler(n);
        assert(n);
        tl_out = stdout;
        trans(n);
        // build the buechi-automation structure needed for LTL model checking
        // put the state predicates
        bauto = new BuechiAutomata();

        // extract the states from the ltl2ba data structures
        if (bstates->nxt == bstates)
        {
            // TODO the search result is FALSE!
            rep->message("Not yet implemented, result FALSE");
            rep->abort(ERROR_COMMANDLINE);
        }

        if (bstates->nxt->nxt == bstates && bstates->nxt->id == 0)
        {
            // TODO the search result is TRUE!
            rep->message("Not yet implemented, result TRUE");
            rep->abort(ERROR_COMMANDLINE);
        }

        bauto->cardStates = 0;
        // map-> final,id
        std::map<int, std::map<int, int > > state_id;
        BState *s;
        BTrans *t;
        for (s = bstates->prv; s != bstates; s = s->prv)
        {
            state_id[s->final][s->id] = bauto->cardStates;
            bauto->cardStates++;
        }

        //rep->message("Buechi-automaton has %d states", bauto->cardStates);
        // now i do know the number of states
        bauto->cardTransitions = new uint32_t[bauto->cardStates]();
        bauto->transitions = new uint32_t **[bauto->cardStates]();
        bauto->cardEnabled = new index_t[bauto->cardStates]();
        bauto->isStateAccepting = new bool[bauto->cardStates]();

        std::vector<StatePredicate *> neededProperties;
        std::map<StatePredicate *, int> neededProperties_backmap;

        // read out the datastructure
        int curState = -1;
        int curProperty = 0;
        for (s = bstates->prv; s != bstates; s = s->prv)
        {
            curState++;
            if (s->id == 0)
            {
                // build a TRUE-loop
                bauto->isStateAccepting[curState] = true;
                bauto->cardTransitions[curState] = 1;
                bauto->transitions[curState] = new uint32_t *[1]();
                bauto->transitions[curState][0] = new uint32_t[2]();
                bauto->transitions[curState][0][0] = neededProperties.size();
                bauto->transitions[curState][0][1] = curState;
                curProperty++;
                neededProperties.push_back(new TruePredicate());
                neededProperties_backmap[neededProperties.back()] = curState;
                continue;
            }
            if (s->final == accepting_state)
            {
                bauto->isStateAccepting[curState] = true;
            }






            // build the successor list
            bauto->cardTransitions[curState] = 0;
            for (t = s->trans->nxt; t != s->trans; t = t->nxt)
            {
                // now build the property
                std::vector<StatePredicate *> disjunctionproperty;
                disjunctionproperty.push_back(buildPropertyFromList(t->pos, t->neg));
                BTrans *t1;
                for (t1 = t; t1->nxt != s->trans; )
                {
                    if (t1->nxt->to->id == t->to->id && t1->nxt->to->final == t->to->final)
                    {
                        disjunctionproperty.push_back(buildPropertyFromList(t1->nxt->pos,
                                                      t1->nxt->neg));
                        t1->nxt = t1->nxt->nxt;
                    }
                    else
                    {
                        t1 = t1->nxt;
                    }
                }

                if (disjunctionproperty.size() == 1)
                {
                    neededProperties.push_back(disjunctionproperty[0]);
                }
                else
                {
                    DisjunctionStatePredicate *disjucntion = new DisjunctionStatePredicate(
                        disjunctionproperty.size());
                    for (int i = 0; i < disjunctionproperty.size(); i++)
                    {
                        disjucntion->addSub(i, disjunctionproperty[i]);
                    }
                    neededProperties.push_back(disjucntion);
                }
                //rep->message("CREATE %d -> %d", neededProperties.size(), curState);
                neededProperties_backmap[neededProperties.back()] = curState;

                // increment number of transitions
                bauto->cardTransitions[curState]++;
            }

            bauto->transitions[curState] = new uint32_t *[bauto->cardTransitions[curState]]();
            int current_on_trans = -1;
            for (t = s->trans->nxt; t != s->trans; t = t->nxt)
            {
                // bauto data structures
                current_on_trans++;
                bauto->transitions[curState][current_on_trans] = new uint32_t[2]();
                //rep->message("Transition %d -> %d", curState, state_id[t->to->final][t->to->id]);
                bauto->transitions[curState][current_on_trans][0] = curProperty++;
                bauto->transitions[curState][current_on_trans][1] =
                    state_id[t->to->final][t->to->id];
                //rep->message("FROM TO %d %d", curState, state_id[t->to->final][t->to->id]);
            }
        }

        //
        // build a list of all needed propositions
        //

        // if the automata contains an all-accepting state
        bauto->cardAtomicPropositions = neededProperties.size();
        bauto->atomicPropositions = new StatePredicateProperty *[bauto->cardAtomicPropositions]();
        bauto->atomicPropotions_backlist = new index_t[bauto->cardAtomicPropositions]();
        for (int i = 0; i < neededProperties.size(); i++)
        {
            bauto->atomicPropositions[i] = new StatePredicateProperty(neededProperties[i]);
            //rep->message("BL %d %d", i, neededProperties_backmap[neededProperties[i]]);
            bauto->atomicPropotions_backlist[i] =
                neededProperties_backmap[neededProperties[i]];
        }
        break;
    }

    default:
        rep->message("check not yet implemented");
        rep->abort(ERROR_COMMANDLINE);
    }

    // tidy parser
    ptformula_lex_destroy();

    delete TheFormula;

    if (args_info.formula_given)
    {
        delete formulaFile;
    }
}



void Task::setBuechiAutomata()
{
    Input *buechiFile = NULL;
    buechiStateTable = new SymbolTable();

    // Check if the paramter of --buechi is a file that we can open: if that
    // works, parse the file. If not, parse the string.
    FILE *file;
    if ((file = fopen(args_info.buechi_arg, "r")) == NULL and errno == ENOENT)
    {
        YY_BUFFER_STATE my_string_buffer = ptbuechi__scan_string(args_info.buechi_arg);
    }
    else
    {
        fclose(file);
        buechiFile = new Input("Buechi", args_info.buechi_arg);
        ptbuechi_in = *buechiFile;
    }

    //rep->message("Parsing Büchi-Automaton");
    // parse the formula
    ptbuechi_parse();

    //rep->message("Finished Parsing");

    // restructure the formula: remove arrows and handle negations and tautologies
    TheBuechi = TheBuechi->rewrite(kc::arrows);
    TheBuechi = TheBuechi->rewrite(kc::neg);
    // restructure the formula: again tautoglies and simplification
    TheBuechi = TheBuechi->rewrite(kc::neg);
    TheBuechi = TheBuechi->rewrite(kc::leq);
    TheBuechi = TheBuechi->rewrite(kc::sides);
    TheBuechi = TheBuechi->rewrite(kc::lists);

    // expand the transitions rules
    TheBuechi = TheBuechi->rewrite(kc::rbuechi);

    //rep->message("parsed Buechi");
    //TheBuechi->unparse(myprinter, kc::out);

    //rep->message("checking LTL");

    // copy restructured formula into internal data structures
    TheBuechi->unparse(myprinter, kc::buechi);
    bauto = TheBuechi->automata;
    // XXX: this _must_ work according to the kimwitu docu, but it does not, kimwitu produces memory leaks!
    TheBuechi->free(true);
    //delete TheBuechi;
    delete buechiStateTable;

    //rep->message("Processed Büchi-Automaton");

    // tidy parser
    ptbuechi_lex_destroy();
    //delete TheFormula;

    if (args_info.buechi_given)
    {
        delete buechiFile;
    }

    // reading the buechi automata
    assert(bauto);
}

void Task::setStore()
{
    if (args_info.search_arg == search_arg_findpath)
    {
        store = new EmptyStore<void>(number_of_threads);
    }
    else if (args_info.search_arg == search_arg_sweepline)
    {
        // dummy store for the sweepline method, only counts markings and calls
        store = new SweepEmptyStore();
    }
    else
    {
        // choose a store
        if (bauto)
        {
            ltlStore = StoreCreator<AutomataTree *>::createStore(number_of_threads);
        }
        else if (ctlFormula)
        {
            ctlStore = StoreCreator<void *>::createStore(number_of_threads);
        }
        else
        {
            store = StoreCreator<void>::createStore(number_of_threads);
        }
    }
}

void Task::setProperty()
{
    // choose a simple property
    switch (args_info.check_arg)
    {
    case check_arg_full:
        p = new SimpleProperty();
        fl = new Firelist();
        break;

    case check_arg_deadlock:
        p = new Deadlock();
        if (args_info.stubborn_arg == stubborn_arg_deletion)
        {
            fl = new FirelistStubbornDeletion();
        }
        else
        {
            fl = new FirelistStubbornDeadlock();
        }
        break;

    case check_arg_modelchecking:
        if (bauto)
        {
            fl = new Firelist();
        }
        else if (ctlFormula)
        {
            fl = new Firelist();
        }
        else if (spFormula)
        {
            p = new StatePredicateProperty(spFormula);
            if (args_info.stubborn_arg == stubborn_arg_deletion)
            {
                fl = new FirelistStubbornDeletion(spFormula);
            }
            else
            {
                fl = new FirelistStubbornStatePredicate(spFormula);
            }
        }
        break;

    case check__NULL:
    case check_arg_none:
        break;
    }


    // set the correct exploration algorithm
    switch (args_info.check_arg)
    {
    case check_arg_full:
    case check_arg_deadlock:
    case check_arg_modelchecking:
        if (ctlFormula)
        {
            ctlExploration = new CTLExploration();
        }
        else if (bauto)
        {
            ltlExploration = new LTLExploration(args_info.ltlmode_arg == ltlmode_arg_tree);
        }
        else     // state predicate checking
        {
            if (number_of_threads == 1)
            {
                exploration = new DFSExploration();
            }
            else
            {
                exploration = new ParallelExploration();
            }
        }
        break;

    case check__NULL:
    case check_arg_none:
        break;
    }
}

bool Task::getResult()
{
    assert(ns);
    assert(!ctlFormula || ctlStore); //ctlFormula ==> ctlStore
    assert(!bauto || ltlStore); //bauto ==> ltlStore
    assert(ctlFormula || bauto || store);
    assert(!ctlFormula || ctlExploration);
    assert(!bauto || ltlExploration); //bauto ==> ltlExploration
    assert(ctlFormula || bauto || exploration);
    assert(ctlFormula || bauto || p);
    assert(fl);

    bool result;
    if (ctlFormula)
    {
        result = ctlExploration->checkProperty(ctlFormula, *ctlStore, *fl, *ns);
    }
    else if (bauto)
    {
        result = ltlExploration->checkProperty(*bauto, *ltlStore, *fl, *ns);
    }
    else
    {
        if (formulaType == FORMULA_INITIAL)
        {
            result = p->initProperty(*ns);
        }
        else
        {
            switch (args_info.search_arg)
            {
            case search_arg_depth:
                result = exploration->depth_first(*p, *ns, *store, *fl, number_of_threads);
                break;

            case search_arg_findpath:
                if (args_info.retrylimit_arg == 0)
                {
                    rep->status("starting infinite tries of depth %d", args_info.depthlimit_arg);
                }
                else
                {
                    rep->status("starting at most %d tries of depth %d", args_info.retrylimit_arg,
                                args_info.depthlimit_arg);
                }

                choose = new ChooseTransitionHashDriven();
                result = exploration->find_path(*p, *ns, args_info.retrylimit_arg,
                                                args_info.depthlimit_arg, *fl, *((EmptyStore<void> *)store), *choose);
                delete choose;
                break;

            case search_arg_sweepline:
                // no choice of stores for sweepline method here
                result = exploration->sweepline(*p, *ns, *(SweepEmptyStore *)(store), *fl,
                                                args_info.sweepfronts_arg, number_of_threads);
                break;

            default:
                assert(false);
                break;
            }
        }
    }

    return result;
}

void Task::interpreteResult(bool *result)
{
    // in case AG is used, the result needs to be negated
    if (args_info.formula_given)
        if (formulaType == FORMULA_INVARIANT or formulaType == FORMULA_IMPOSSIBLE
                or formulaType == FORMULA_LTL)
        {
            *result = not * result;
        }

    // make result three-valued
    trinary_t final_result = *result ? TRINARY_TRUE : TRINARY_FALSE;

    // if the Bloom store did not find anything, the result is unknown
    if (args_info.store_arg == store_arg_bloom)
    {
        switch (args_info.check_arg)
        {
        case (check_arg_deadlock):
        case (check_arg_modelchecking):
            if (not * result)
            {
                final_result = TRINARY_UNKNOWN;
            }
            break;

        case check__NULL:
        case check_arg_none:
        case check_arg_full:
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

bool Task::hasWitness(bool result)
{
    if (ctlFormula)
    {
        return ctlExploration->witness.size();
    }
    if (bauto and args_info.formula_given)
    {
        return !result;
    }
    return result;
}

void Task::printWitness()
{

    rep->message("%s", rep->markup(MARKUP_IMPORTANT, "witness path:").str());

    if (ctlFormula)
    {
        for (std::vector<int>::iterator it = ctlExploration->witness.begin();
                it != ctlExploration->witness.end(); ++it)
        {
            if (*it == -1)
            {
                rep->message("===begin of cycle===");
            }
            else
            {
                rep->message("%s", Net::Name[TR][*it]);
            }
        }
        ctlExploration->witness.clear();
    }
    else if (bauto)
    {
        while (ltlExploration->witness.StackPointer > 0)
        {
            index_t &s = ltlExploration->witness.top();
            if (s == -1)
            {
                rep->message("===begin of cycle===");
            }
            else
            {
                rep->message("%s", Net::Name[TR][s]);
            }
            ltlExploration->witness.pop();
        }
    }
    else
    {
        while (p->stack.StackPointer > 0)
        {
            SimpleStackEntry &s = p->stack.top();
            rep->message("%s", Net::Name[TR][s.fl[s.current]]);
            p->stack.pop();
        }
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


NetState *Task::getNetState()
{
    return ns;
}


void Task::printDot()
{
    FILE *o = stdout;

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
    while (p->stack.StackPointer > 0)
    {
        SimpleStackEntry &s = p->stack.top();
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
    for (std::set<index_t>::iterator it = target_transition.begin();
            it != target_transition.end(); ++it)
    {
        Event *e = new Event(*it);
        e->target = true;
    }

    // actual dot output
    fprintf(o, "digraph d {\n");
    fprintf(o, "  rankdir=LR;\n\n");
    Event::dot(o);
    Condition::dot(o);
    fprintf(o, "}\n");
}


/*!
 * \brief this method is only to test Store::popState() until its use case is implemented
 * \todo  remove me!
 */
void Task::testPopState()
{
    rep->message("%s", rep->markup(MARKUP_IMPORTANT,
                                   "iterating over all stored states:").str());
    uint64_t counter = 0;

    while (store->popState(*ns))
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

    rep->message("%s", rep->markup(MARKUP_IMPORTANT, "states collected: %u",
                                   counter).str());
}

uint64_t Task::getMarkingCount() const
{
    if (store)
    {
        return store->get_number_of_markings();
    }
    else if (ltlStore)
    {
        return ltlStore->get_number_of_markings();
    }
    else if (ctlStore)
    {
        return ctlStore->get_number_of_markings();
    }
    return 0;
}

uint64_t Task::getEdgeCount() const
{
    uint64_t result = 0;
    if (store)
    {
        result = store->get_number_of_calls();
    }
    else if (ltlStore)
    {
        result = ltlStore->get_number_of_calls();
    }
    else if (ctlStore)
    {
        result = ctlStore->get_number_of_calls();
    }
    if (result)
    {
        result--;
    }
    return result;
}
