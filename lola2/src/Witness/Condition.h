#pragma once

#include <map>
#include <vector>
#include <cstdio>
#include <Core/Dimensions.h>

class Event;

class Condition
{
public:
    static std::vector<Condition *> conditions;
    index_t place;

    static std::map<index_t, Condition *> current;

    Event *in;
    Event *out;

    Condition(index_t, Event * = NULL);

    static void dot(FILE * = stdout);
};
