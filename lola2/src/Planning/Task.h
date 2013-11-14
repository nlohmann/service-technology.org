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



class Task
{
private:
    StatePredicate *spFormula;
    CTLFormula *ctlFormula;
    BuechiAutomata *bauto;

    NetState *ns;
    ChooseTransition *choose;
    enum_search search;
    Firelist *fl;
    SimpleProperty *p;
    DFSExploration *exploration;
    LTLExploration *ltlExploration;
    CTLExploration *ctlExploration;

    int number_of_threads;
    formula_t formulaType;

    void setNet();
    void setFormula();
    void setBuechiAutomata();

public:
    Store<void> *store;
    Store<AutomataTree *> *ltlStore;
    Store<void *> *ctlStore;

    Task();
    ~Task();

    bool getResult();
    void interpreteResult(bool *);

    void setStore();
    void setProperty();

    bool hasWitness(bool result) const;
    void printWitness() const;
    void printMarking() const;

    /// return the number of stored markings
    uint64_t getMarkingCount() const;
    /// return the number of fired transitions
    uint64_t getEdgeCount() const;

    void printDot() const;

    // this function is needed to allow the random walk in the main function
    NetState *getNetState() const;

    // this method is only to test Store::popState() until its use case is implemented
    void testPopState();
};
