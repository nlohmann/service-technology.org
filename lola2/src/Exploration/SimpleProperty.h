/*!
 \file SimpleProperty.h
 \author Karsten
 \status new

 \brief Evaluates simple property (only SET of states needs to be computed).
 Actual property is virtual, default (base class) is full exploration
 */

#pragma once

#include <config.h>
#include <Exploration/SearchStack.h>
#include <Exploration/ChooseTransition.h>
#include <Net/NetState.h>
#include <Exploration/Firelist.h>

class Firelist;
class Store;
class EmptyStore;

class SimpleProperty {
public:

	virtual ~SimpleProperty() {}

    /// the witness path
    SearchStack stack;

    /// value of property in current state
    bool value;

    /// prepare for search
    virtual bool initProperty(NetState &ns) {
        return false;
    }

    /// check property in Marking::Current, use after fire. Argument is transition just fired.
    virtual bool checkProperty(NetState &ns, index_t) {
        return false;
    }

    /// check property in Marking::Current, use after backfire. Argument is transition just backfired.
    virtual bool updateProperty(NetState &ns, index_t) {
        return false;
    }

    virtual SimpleProperty* copy() {
        return new SimpleProperty();
    }
};
