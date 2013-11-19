#pragma once

#include <cmdline.h>
#include <Core/Dimensions.h>
#include <Exploration/LTLExploration.h>
#include <Exploration/CTLExploration.h>

#include <Stores/NetStateEncoder/BitEncoder.h>
#include <Stores/NetStateEncoder/CopyEncoder.h>
#include <Stores/NetStateEncoder/FullCopyEncoder.h>
#include <Stores/NetStateEncoder/SimpleCompressedEncoder.h>
#include <Stores/VectorStores/HashingWrapperStore.h>
#include <Stores/VectorStores/PrefixTreeStore.h>
#include <Stores/VectorStores/VSTLStore.h>
#include <Stores/VectorStores/VBloomStore.h>
#include <Stores/PluginStore.h>
#include <Stores/EmptyStore.h>
#include <Stores/CycleStore.h>
#include <Stores/CompareStore.h>
#include <Stores/Store.h>

// forward declarations
class StatePredicate;
class CTLFormula;
class SimpleProperty;
class Firelist;
class ChooseTransition;
class DFSExploration;
class NetState;


/// generic store creator, basically a templatarized namespace for static creation methods.
/// Resolves the code duplication problem with using different templates in Task::setStore
template<typename T>
class StoreCreator
{
public:
    /// creates a new store based on the specified template and the command line arguments
    static Store<T> *createStore(int number_of_threads)
    {

        // create an encode according --encoder
        NetStateEncoder *enc = NULL;
        switch (args_info.encoder_arg)
        {
        case encoder_arg_bit:
        {
            rep->status("using a bit-perfect encoder (%s)", rep->markup(MARKUP_PARAMETER,
                        "--encoder").str());
            enc = new BitEncoder(number_of_threads);
            break;
        }

        case encoder_arg_copy:
        {
            rep->status("using a copy encoder (%s)", rep->markup(MARKUP_PARAMETER,
                        "--encoder").str());
            enc = new CopyEncoder(number_of_threads);
            break;
        }

        case encoder_arg_simplecompressed:
        {
            rep->status("using a simple compression encoder (%s)",
                        rep->markup(MARKUP_PARAMETER, "--encoder").str());
            enc = new SimpleCompressedEncoder(number_of_threads);
            break;
        }

        case encoder_arg_fullcopy:
        {
            rep->status("using a full copy encoder (%s)", rep->markup(MARKUP_PARAMETER,
                        "--encoder").str());
            enc = new FullCopyEncoder(number_of_threads);
            break;
        }

        case encoder__NULL:
        {
            rep->status("not using an encoder (%s)", rep->markup(MARKUP_PARAMETER,
                        "--encoder").str());
            break;
        }
        }

        // create a store according to --store
        Store<T> *st = NULL;
        switch (args_info.store_arg)
        {
        case store_arg_comp:
        {
            rep->status("using a compare store (%s)", rep->markup(MARKUP_PARAMETER,
                        "--store").str());
            st = new CompareStore<T>(
                new PluginStore<T>(enc, new PrefixTreeStore<T>(), number_of_threads),
                new PluginStore<T>(enc, new VSTLStore<T>(number_of_threads), number_of_threads),
                number_of_threads);
            break;
        }

        case store_arg_prefix:
        {
            rep->status("using a prefix store (%s)", rep->markup(MARKUP_PARAMETER,
                        "--store").str());
            if (args_info.bucketing_given)
            {
                st = new PluginStore<T>(enc,
                                        new HashingWrapperStore<T>(new
                                                NullaryVectorStoreCreator<T, PrefixTreeStore<T> >()), number_of_threads);
            }
            else
            {
                st = new PluginStore<T>(enc, new PrefixTreeStore<T>(), number_of_threads);
            }
            break;
        }

        case store_arg_stl:
        {
            rep->status("using an STL store (%s)", rep->markup(MARKUP_PARAMETER,
                        "--store").str());
            if (args_info.bucketing_given)
            {
                st = new PluginStore<T>(enc,
                                        new HashingWrapperStore<T>(new
                                                UnaryVectorStoreCreator<T, VSTLStore<T>, index_t>(number_of_threads)),
                                        number_of_threads);
            }
            else
            {
                st = new PluginStore<T>(enc, new VSTLStore<T>(number_of_threads),
                                        number_of_threads);
            }
            break;
        }

        default:
        {
            rep->status("using a specialized store (%s)", rep->markup(MARKUP_PARAMETER,
                        "--store").str());
            st = createSpecializedStore(number_of_threads);
            break;
        }
        }

        // cycle reduction: wrap created store
        if (args_info.cycle_given)
        {
            rep->status("using the cycle reduction (%s)", rep->markup(MARKUP_PARAMETER,
                        "--cycle").str());
            // wrap current store in CycleStore object
            st = new CycleStore<T>(number_of_threads, st, args_info.cycleheuristic_arg);
        }

        return st;
    }

private:
    /// creates specialized stores for different payload types.
    /// the default implementation raises an error, but template specializations of it may create stores with restricted payload type compatibility.
    static Store<T> *createSpecializedStore(int number_of_threads)
    {
        storeCreationError();
        return NULL;
    }
    /// auxiliary method that raises an error. Will be called by the different specializations of createSpecializedStore.
    static void storeCreationError()
    {
        rep->message(rep->markup(MARKUP_BAD,
                                 "specified store does not fit the given task").str());
        rep->abort(ERROR_COMMANDLINE);
    }
};

/// forward declaration of special store creation for stores without payload support (e.g. BloomStore)
template<>
Store<void> *StoreCreator<void>::createSpecializedStore(int number_of_threads);


/*!
\brief the verification task

This class encapsulates all information that is used to verify the task given
by the used in form of a net, an optional formula, and command-line parameters.
It is configured and evaluated in the main() method.
*/
class Task
{
public:
    Task();
    ~Task();

    /// start the state space exploration
    bool getResult();
    /// interprete and display the result
    void interpreteResult(bool);

    /// whether a witness path exists
    bool hasWitness(bool result) const;

    /// print the witness path
    void printWitnessPath() const;
    /// print the target marking
    void printMarking() const;
    /// print the witness path as distributed run
    void printRun() const;

    /// return the number of stored markings
    uint64_t getMarkingCount() const;
    /// return the number of fired transitions
    uint64_t getEdgeCount() const;

    // debug function
    void testPopState();

private:
    /// the type of the parsed formula
    formula_t formulaType;

    /// a state predicate (formula without temporal operators)
    StatePredicate *spFormula;
    /// a CTL formula
    CTLFormula *ctlFormula;
    /// a Büchi automaton (can be created from an LTL formula)
    BuechiAutomata *bauto;

    /// a simple property (deadlock, state predicate)
    SimpleProperty *p;

    /// a firelist (the used stubborn sets)
    Firelist *fl;

    /// depth first search as exploration (can be parallel)
    DFSExploration *exploration;
    /// LTL exploration
    LTLExploration *ltlExploration;
    /// CTL exploration
    CTLExploration *ctlExploration;

    /// a net state
    NetState *ns;

    /// the store for findpath, sweepline, and everything but LTL and CTL
    Store<void> *store;
    /// the store for LTL
    Store<AutomataTree *> *ltlStore;
    /// the store for CTL
    Store<void *> *ctlStore;

    /// the number of threads to use as given by `--thread`
    int number_of_threads;

    /// set the formula from `--formula`
    void setFormula();

    /// set the Büchi automaton from `--buchi`
    void setBuechiAutomata();

    /// set the store
    void setStore();

    /// set the property, firelist, and exploration
    void setProperty();
};
