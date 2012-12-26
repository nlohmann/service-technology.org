#include <config.h>
#include <Witness/Event.h>
#include <Witness/Condition.h>
#include <Net/Net.h>

std::map<index_t, Event*> Event::current;
std::vector<Event*> Event::events;

Event::Event(index_t t) : transition(t), target(false)
{
    current[t] = this;
    events.push_back(this);

    for (index_t pre = 0; pre < Net::CardArcs[TR][PRE][t]; ++pre)
    {
        const index_t p = Net::Arc[TR][PRE][t][pre];
        assert(Condition::current[p]);
        assert(Condition::current[p]->out == NULL);
        Condition::current[p]->out = this;
    }

    for (index_t post = 0; post < Net::CardArcs[TR][POST][t]; ++post)
    {
        const index_t p = Net::Arc[TR][POST][t][post];
        new Condition(p, this);
    }
}

void Event::dot(FILE* o)
{
    assert(o);

    for (std::vector<Event*>::iterator e = events.begin(); e != events.end(); ++e)
    {
        if ((*e)->target)
        {
            fprintf(o, "  e%p [label=\"%s\" shape=box width=.5 fixedsize=true color=green]\n", (void*)*e, Net::Name[TR][(*e)->transition]);
        }
        else
        {
            fprintf(o, "  e%p [label=\"%s\" shape=box width=.5 fixedsize=true]\n", (void*)*e, Net::Name[TR][(*e)->transition]);
        }
    }
}
