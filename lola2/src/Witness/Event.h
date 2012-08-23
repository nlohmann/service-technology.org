#pragma once

#include <map>
#include <vector>
#include <cstdio>
#include <Core/Dimensions.h>

class Event
{
private:
    static std::map<index_t, Event*> current;
    static std::vector<Event*> events;

    index_t transition;

public:
    bool target;

    Event(index_t);

    static void dot(FILE * = stdout);
};

