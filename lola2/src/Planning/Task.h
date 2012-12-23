#pragma once

#include <cmdline.h>
#include <Core/Dimensions.h>
#include <Exploration/LTLExploration.h>
#include <Exploration/CTLExploration.h>

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
	static Store<T>* createStore(int number_of_threads) {
        switch (args_info.store_arg)
        {
        case store_arg_comp:
       		return new CompareStore<T>(
           		new PluginStore<T>(new BitEncoder(number_of_threads), new SuffixTreeStore<T>(), number_of_threads),
           		new PluginStore<T>(new BitEncoder(number_of_threads), new VSTLStore<T>(number_of_threads), number_of_threads),
           		number_of_threads
           		);
        case store_arg_psbbin:
           	return new PluginStore<T>(new BitEncoder(number_of_threads), new SuffixTreeStore<T>(), number_of_threads);
        case store_arg_pscbin:
           	return new PluginStore<T>(new CopyEncoder(number_of_threads), new SuffixTreeStore<T>(), number_of_threads);
        case store_arg_psfbin:
           	return new PluginStore<T>(new FullCopyEncoder(number_of_threads), new SuffixTreeStore<T>(), number_of_threads);
        case store_arg_pssbin:
        	return new PluginStore<T>(new SimpleCompressedEncoder(number_of_threads), new SuffixTreeStore<T>(), number_of_threads);
        case store_arg_psbstl:
        	return new PluginStore<T>(new BitEncoder(number_of_threads), new VSTLStore<T>(number_of_threads), number_of_threads);
        case store_arg_pscstl:
        	return new PluginStore<T>(new CopyEncoder(number_of_threads), new VSTLStore<T>(number_of_threads), number_of_threads);
        case store_arg_psfstl:
        	return new PluginStore<T>(new FullCopyEncoder(number_of_threads), new VSTLStore<T>(number_of_threads), number_of_threads);
        case store_arg_pssstl:
        	return new PluginStore<T>(new SimpleCompressedEncoder(number_of_threads), new VSTLStore<T>(number_of_threads), number_of_threads);
        default:
        	return createSpecializedStore(number_of_threads);
        }
	}

private:
	/// creates specialized stores for different payload types.
	/// the default implementation raises an error, but template specializations of it may create stores with restricted payload type compatibility.
	static Store<T>* createSpecializedStore(int number_of_threads) {
		storeCreationError();
	    return NULL;
	}
	/// auxiliary method that raises an error. Will be called by the different specializations of createSpecializedStore.
	static void storeCreationError() {
		rep->markup(MARKUP_BAD,"specified store does not fit the given task");
		rep->abort(ERROR_COMMANDLINE);
	}
};

/// forward declaration of special store creation for stores without payload support (e.g. BloomStore)
template<>
Store<void>* StoreCreator<void>::createSpecializedStore(int number_of_threads);



class Task
{
private:
    StatePredicate* spFormula;
    CTLFormula* ctlFormula;
    NetState* ns;
    ChooseTransition* choose;
    enum_search search;
    Firelist* fl;
    SimpleProperty* p;
    BuechiAutomata* bauto;
    DFSExploration* exploration;
    LTLExploration* ltlExploration;
    CTLExploration* ctlExploration;
    int number_of_threads;
    formula_t formulaType;
    void setNet();
    void setFormula();
    void setBuechiAutomata();

public:
    Store<void>* store;
    Store<AutomataTree*>* ltlStore;
    Store<void*>* ctlStore;

    Task();
    ~Task();

    bool getResult();
    void interpreteResult(bool);

    void setStore();
    void setProperty();

    void printWitness();
    void printMarking();

    void printDot();

    // this function is needed to allow the random walk in the main function
    NetState* getNetState();

    // this method is only to test Store::popState() until its use case is implemented
    void testPopState();
};
