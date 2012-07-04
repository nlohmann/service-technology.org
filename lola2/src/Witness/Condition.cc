#include <set>
#include <Witness/Condition.h>
#include <Net/Net.h>
#include <cmdline.h>

extern gengetopt_args_info args_info;

std::map<index_t, Condition*> Condition::current;
std::vector<Condition*> Condition::conditions;

Condition::Condition(index_t p, Event* e) : place(p), in(e), out(NULL)
{
    current[p] = this;
    conditions.push_back(this);
}

void Condition::dot(FILE* o)
{
    assert(o);
    extern std::set<index_t> target_place;

    for (std::vector<Condition*>::iterator b = conditions.begin(); b != conditions.end(); ++b)
    {
        // event structure
        if (args_info.eventstructure_flag)
        {
            if ((*b)->in != NULL)
            {
                fprintf(o, "  e%p -> e%p\n", (*b)->in, (*b)->out);
            }
            continue;
        }

        bool initial = ((*b)->in == NULL);
        bool target = ((*b)->out == NULL && target_place.find((*b)->place) != target_place.end());

        if ((*b)->out == NULL and (not target) and (not args_info.nofilterrun_flag))
        {
            continue;
        }

        if (target)
        {
            fprintf(o, "  c%p [label=\"%s\" shape=circle width=.5 fixedsize=true color=green]\n", *b, Net::Name[PL][(*b)->place]);
        }
        else
        {
            if (initial)
            {
                fprintf(o, "  c%p [label=\"%s\" shape=circle width=.5 fixedsize=true color=blue]\n", *b, Net::Name[PL][(*b)->place]);
            }
            else
            {
                fprintf(o, "  c%p [label=\"%s\" shape=circle width=.5 fixedsize=true]\n", *b, Net::Name[PL][(*b)->place]);
            }
        }

        if ((*b)->in != NULL)
        {
            fprintf(o, "  e%p -> c%p\n", (*b)->in, *b);
        }

        if ((*b)->out != NULL)
        {
            fprintf(o, "  c%p -> e%p\n", *b, (*b)->out);
        }
    }
}
