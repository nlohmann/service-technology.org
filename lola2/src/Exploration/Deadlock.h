/*!
\file Deadlock.h
\author Karsten
\status approved 18.04.2012

\brief derives deadlock checking from SimpleProperty
*/

#pragma once

#include <Exploration/SimpleProperty.h>

class Firelist;
class Store;

/// derives deadlock checking from SimpleProperty
class Deadlock : public SimpleProperty
{
    public:
        Deadlock() {}

    private:
        /// prepare for search
        virtual void initProperty();

        /// check property in Marking::Current, use after fire. Argument is transition just fired.
        virtual void checkProperty(index_t);

        /// check property in Marking::Current, use after backfire. Argument is transition just backfired.
        void updateProperty(index_t) {}
};
