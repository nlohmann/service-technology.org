/*!
\file SimpleProperty.h
\author Karsten
\status new

\brief simple property (only SET of states needs to be computed to check it).
*/

#pragma once

#include <config.h>
#include <Exploration/SearchStack.h>
#include <Exploration/ChooseTransition.h>
#include <Net/NetState.h>
#include <Exploration/Firelist.h>
#include <Stores/Store.h>
#include <Stores/EmptyStore.h>

class Firelist;
//class Store;
//class EmptyStore;
extern int foo_bar;
/*!
\brief one element on the stack for simple properties

A simple stack entry contains two elements
- a firelist (as an array of numbers)
- the current index on the firelist
 */
class SimpleStackEntry
{
public:
	/// ordinary constructor for entry
    SimpleStackEntry(index_t * f,index_t c) {
        assert(f);    //firelist, first processed in firelist
        current = c;
        //fl = new index_t[current+1];
        //memcpy(fl,f,(current+1)*SIZEOF_INDEX_T);
        fl = f;
        //rep->message("argu cr: create %p", (void*)fl);
    }
    /// copy constructor used by the search stack
    SimpleStackEntry(SimpleStackEntry& src) {
        current = src.current;
        fl = new index_t[current+1];
        //rep->message("copy cr: create %p", (void*)fl);
        assert(fl);
        assert(src.fl);
        memcpy(fl,src.fl,(current+1)*SIZEOF_INDEX_T);
    }
    ~SimpleStackEntry() {
    	int foo = foo_bar++;
    	//rep->message("Start deletion (%d) on %p",foo,(void*)this);
    	//rep->message("Delete (%d): %p",foo,(void*)fl);
        if (fl) delete[] fl;
        fl = NULL;
    }
    index_t * fl; // array to take a firelist
    index_t current; // index of first processed element of fl
};


/*!
\brief simple property, beeing always false

This property is one of the simplest possible properties, beeing false for every marking.
Also this class is the base class for all property, that only need to be evaluated on one single marking.
*/
class SimpleProperty
{
public:

    virtual ~SimpleProperty() { }

    /// the witness path
    SearchStack<SimpleStackEntry> stack;

    /// value of property in current state
    bool value;

    /// prepare for search
    virtual bool initProperty(NetState &ns)
    {
        return false;
    }

    /// check property in Marking::Current, use after fire. Argument is transition just fired.
    virtual bool checkProperty(NetState &ns, index_t)
    {
        return false;
    }

    /// check property in Marking::Current, use after backfire. Argument is transition just backfired.
    virtual bool updateProperty(NetState &ns, index_t)
    {
        return false;
    }

    /// create a new simple property exactly as the current one
    virtual SimpleProperty* copy()
    {
        return new SimpleProperty();
    }
};
