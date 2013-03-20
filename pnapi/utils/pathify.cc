#include <config.h>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <string>
#include <set>
#include <map>
#include <libgen.h>
#include "pnapi.h"
#include "verbose.h"
#include "Output.h"
#include "pathify-cmdline.h"

using namespace pnapi;

/// the command line parameters
gengetopt_args_info args_info;

/// the invocation string
std::string invocation;

/// evaluate the command line parameters
void evaluateParameters(int argc, char** argv) {
    // overwrite invocation for consistent error messages
    argv[0] = basename(argv[0]);

    // store invocation in a std::string for meta information in file output
    for (int i = 0; i < argc; ++i) {
        invocation += std::string(argv[i]) + " ";
    }

    // initialize the parameters structure
    struct cmdline_parser_params* params = cmdline_parser_params_create();

    // call the cmdline parser
    if (cmdline_parser(argc, argv, &args_info) != 0) {
        abort(1, "invalid command-line parameter(s)");
    }

    free(params);
}



/* data structures for partial orders */

typedef enum {
    CONDITION, EVENT
} PO_Type;

class PO_Node {
    public:
        bool skip;
        PO_Type type;
        PO_Node(PO_Type);
};

PO_Node::PO_Node(PO_Type type) : type(type), skip(false) {}

struct PO_Event : public PO_Node {
    const Transition *t;
    explicit PO_Event(const Transition *t);
};

PO_Event::PO_Event(const Transition *t) : PO_Node(EVENT), t(t) {}

struct PO_Condition : public PO_Node {
    const Place *p;
    PO_Event *pre;
    PO_Event *post;
    bool initial;
    bool final;
    bool goal;
    explicit PO_Condition(const Place *p);
};

PO_Condition::PO_Condition(const Place *p) : PO_Node(CONDITION), p(p), pre(NULL), post(NULL), initial(false), final(false), goal(false) {}



std::string shortenName(std::string s) {
    if (s.size() <= 5 or not args_info.shorten_given) {
        return s;
    } else {
        return "..." + s.substr(s.size()-4, 4);
    }
}


void dotPO(PetriNet &net, std::vector<const Transition*> path, std::vector<std::set<const Transition*> > clusters, std::vector<Marking> markings) {
    assert(path.size() == clusters.size());
    assert(path.size()+1 == markings.size());

    std::vector<PO_Event*> events;
    std::vector<PO_Condition*> conditions;
    std::map<const Node*, PO_Condition*> marked;
    std::map<PO_Node*, std::map<PO_Node*, int > > adjacency;

    // initial marking -> initial cut
    PNAPI_FOREACH(p, markings[0]) {
        if (p->second > 0) {
            // create condition
            PO_Condition *b = new PO_Condition(p->first);
            b->initial = true;
            conditions.push_back(b);
            marked[p->first] = b;
        }
    }

    // traverse path and create run
    for (int i = 0; i < path.size(); ++i) {
        assert(path[i]);

        // create event
        PO_Event *e = new PO_Event(path[i]);
        events.push_back(e);

        // connect preset
        PNAPI_FOREACH(p, path[i]->getPreset()) {
            assert(marked[*p]);
            marked[*p]->post = e;

            adjacency[marked[*p]][e] = 1;
        }

        // connect postset
        PNAPI_FOREACH(p, path[i]->getPostset()) {
            PO_Condition *b = new PO_Condition(dynamic_cast<Place *>(*p));
            conditions.push_back(b);
            marked[*p] = b;

            marked[*p]->pre = e;
            adjacency[e][marked[*p]] = 1;
        }
    }

    // mark final cut
    for (int i = 0; i < conditions.size(); ++i) {
        if (conditions[i]->post == NULL) {
            conditions[i]->final = true;
        }
    }

    // Floyd-Warshall
    {
        // prepare a list of all nodes
        std::vector<PO_Node*> nodes;
        PNAPI_FOREACH(b, conditions) {
            nodes.push_back(*b);
        }
        PNAPI_FOREACH(e, events) {
            nodes.push_back(*e);
        }
        
        // calculate transtive closure
        PNAPI_FOREACH(k, nodes) {
            PNAPI_FOREACH(i, nodes) {
                if (adjacency[*i][*k] == 1) {
                    PNAPI_FOREACH(j, nodes) {
                        if (adjacency[*k][*j] == 1) {
                            adjacency[*i][*j] = 1;
                        }
                    }
                }
            }
        }

        // prepare a list of conflict events
        std::vector<PO_Event*> conflict_events;
        for (int i = 0; i < events.size(); ++i) {
            if (clusters[i].size() > 1) {
                conflict_events.push_back(events[i]);
            }
        }

        // hier noch fertig: event structure der conflict events ausgeben
        for (int i = 0; i < conflict_events.size(); ++i) {
            for (int j = 0; j < conflict_events.size(); ++j) {
                if (i != j) {
                    if (adjacency[conflict_events[i]][conflict_events[j]] == 1) {
                        std::cerr << i << " -> " << j << "\n";
                    }
                }
            }
        }
    }



    if (args_info.reachability_given) {
        for (int i = 0; i < conditions.size(); ++i) {
            if (conditions[i]->post == NULL) {
                conditions[i]->goal = true;
            }
        }
    }


    if (args_info.safety_given) {
        std::set<PO_Node*> goal;
        std::map<const Place*, int> token_count;

        for (int i = 0; i < conditions.size(); ++i) {
            if (conditions[i]->post == NULL) {
                token_count[conditions[i]->p]++;
            }
        }

        PNAPI_FOREACH(t, token_count) {
            if (t->second > 1) {
                for (int i = 0; i < conditions.size(); ++i) {
                    if (conditions[i]->post == NULL and conditions[i]->p == t->first) {
                        conditions[i]->goal = true;
                        goal.insert(conditions[i]);
                    }
                }
            }
        }


        for (int i = 0; i < conditions.size(); ++i) {
            bool skip_this = true;
            PNAPI_FOREACH(b, goal) {
                if (adjacency[conditions[i]][*b] == 1) {
                    skip_this = false;
                }
            }
            if (skip_this and goal.find(conditions[i]) == goal.end()) {
                conditions[i]->skip = true;
            }
        }

        for (int i = 0; i < events.size(); ++i) {
            bool skip_this = true;
            PNAPI_FOREACH(b, goal) {
                if (adjacency[events[i]][*b] == 1) {
                    skip_this = false;
                }
            }
            if (skip_this) {
                events[i]->skip = true;
            }
        }
    }



    if (args_info.reducerun_given) {
        std::set<PO_Event*> important;
        
        for (int i = 0; i < events.size(); ++i) {
            if (clusters[i].size() > 1) {
                important.insert(events[i]);
            }
        }

        for (int i = 0; i < conditions.size(); ++i) {
            if (conditions[i]->goal and conditions[i]->pre) {
                important.insert((PO_Event*)(conditions[i]->pre));
            }
        }

        // dot
        std::cout << "digraph PO {\n";
        std::cout << "rankdir=LR;\n";
        std::cout << "node [fontname=\"Helvetica Neue\"; fontsize=10; fixedsize=true;]\n";

        PNAPI_FOREACH(e1, important) {
            std::cout << (size_t)(*e1) << " [shape=square label=\"" << shortenName((*e1)->t->getName()) << "\"";

            for (int i = 0; i < events.size(); ++i) {
                if (events[i] == *e1 and clusters[i].size() > 1) {
                    std::cout << " penwidth=4; ";
                }
            }

            std::cout << "];\n";

            PNAPI_FOREACH(e2, important) {
                if (*e1 == *e2) {
                    continue;
                }
                if (adjacency[*e1][*e2] == 1) {
                    std::cout << (size_t)(*e1) << " -> " << (size_t)(*e2) << " [style=dashed;]\n";
                }
            }
        }

        // initial cut
        std::cout << "subgraph cluster_0 {\n";
        std::cout << "color=invis;\n";
        for (int i = 0; i < conditions.size(); ++i) {
            if (conditions[i]->skip) {
                continue;
            }

            if (conditions[i]->initial and !conditions[i]->final) {
                std::cout << (size_t)conditions[i] << "\n";
            }
        }
        std::cout << "}\n\n";


        // final cut
        std::cout << "subgraph cluster_1 {\n";
        std::cout << "color=invis;\n";
        for (int i = 0; i < conditions.size(); ++i) {
            if (conditions[i]->skip and not args_info.grayskip_given) {
                continue;
            }

            if (conditions[i]->goal and !conditions[i]->initial) {
                std::cout << (size_t)conditions[i] << "\n";
            }
        }
        std::cout << "}\n\n";



        // conditions
        for (int i = 0; i < conditions.size(); ++i) {
            if (conditions[i]->skip) {
                continue;
            }

            if (conditions[i]->initial or conditions[i]->goal) {

                std::cout << (size_t)conditions[i] << " [shape=circle label=\"" << shortenName(conditions[i]->p->getName()) << "\"";

                if (conditions[i]->goal) {
                    std::cout << " color=black; penwidth=4; ";
                }
                if (conditions[i]->skip and args_info.grayskip_given) {
                    std::cout << " color=gray; fontcolor=gray; ";
                }

                std::cout << "]\n";

                if (conditions[i]->post) {
                    std::set<PO_Event*>::iterator it = important.find(conditions[i]->post);
                    if (it != important.end()) {
                        std::cout << (size_t)conditions[i] << " -> " << (size_t)(*it) << ";\n";
                    } else {
                        PNAPI_FOREACH(e, important) {
                            if (adjacency[conditions[i]][*e] == 1) {
                                std::cout << (size_t)(conditions[i]) << " -> " << (size_t)(*e) << " [style=dashed;]\n";
                            }
                        }
                    }
                }

                if (conditions[i]->pre) {
                    std::set<PO_Event*>::iterator it = important.find(conditions[i]->pre);
                    if (it != important.end() and not conditions[i]->pre->skip) {
                        std::cout << (size_t)(*it) << " -> " << (size_t)conditions[i] << ";\n";
                    }
                }
            }
        }


        std::cout << "}\n";

        return;
    }


    // dot
    std::cout << "digraph PO {\n";
    std::cout << "rankdir=LR;\n";
    std::cout << "node [fontname=\"Helvetica Neue\"; fontsize=10; fixedsize=true;]\n";
    //std::cout << "splines=false;\n";


    // initial cut
    std::cout << "subgraph cluster_0 {\n";
    std::cout << "color=invis;\n";
    for (int i = 0; i < conditions.size(); ++i) {
        if (conditions[i]->skip) {
            continue;
        }

        if (conditions[i]->initial and !conditions[i]->final) {
            std::cout << (size_t)conditions[i] << "\n";
        }
    }
    std::cout << "}\n\n";


    // final cut
    std::cout << "subgraph cluster_1 {\n";
    std::cout << "color=invis;\n";
    for (int i = 0; i < conditions.size(); ++i) {
        if (conditions[i]->skip and not args_info.grayskip_given) {
            continue;
        }

        if (conditions[i]->final and !conditions[i]->initial) {
            std::cout << (size_t)conditions[i] << "\n";
        }
    }
    std::cout << "}\n\n";


    // conditions
    for (int i = 0; i < conditions.size(); ++i) {
        if (conditions[i]->skip and not args_info.grayskip_given) {
            continue;
        }
        
        std::cout << (size_t)conditions[i] << " [shape=circle label=\"" << shortenName(conditions[i]->p->getName()) << "\"";

        if (conditions[i]->goal) {
            std::cout << " color=black; penwidth=4; ";
        }
        if (conditions[i]->skip and args_info.grayskip_given) {
            std::cout << " color=gray; fontcolor=gray; ";
        }

        std::cout << "]\n";

        if (conditions[i]->pre) {
            std::cout << (size_t)conditions[i]->pre << " -> " << (size_t)conditions[i];
            if ((conditions[i]->pre->skip or conditions[i]->skip) and args_info.grayskip_given) {
                std::cout << " [color=gray;]";
            }
            std::cout << ";\n";
        }

        if (conditions[i]->post) {
            std::cout << (size_t)conditions[i] << " -> " << (size_t)conditions[i]->post;
            if ((conditions[i]->post->skip or conditions[i]->skip) and args_info.grayskip_given) {
                std::cout << " [color=gray;]";
            }
            std::cout << ";\n";
        }
    }


    // events
    for (int i = 0; i < events.size(); ++i) {
        if (events[i]->skip and not args_info.grayskip_given) {
            continue;
        }

        std::cout << (size_t)events[i] << " [shape=square label=\"" << shortenName(events[i]->t->getName()) << "\"";
        
        if (clusters[i].size() > 1) {
            std::cout << " penwidth=4; ";
        }
        if (events[i]->skip and args_info.grayskip_given) {
            std::cout << " color=gray; fontcolor=gray; ";
        }
        
        std::cout << "]\n";
    }


    std::cout << "}\n";
}

void printMarking(const Marking &m) {
    PNAPI_FOREACH(p, m) {
        if (p->second > 0) {
            std::cerr << p->first->getName() << ": " << p->second << "\n";
        }
    }
}

void printPath(std::vector<const Transition*> path, std::vector<std::set<const Transition*> > clusters) {
    assert(path.size() == clusters.size());

    for (int i = 0; i < path.size(); ++i) {
        assert(clusters[i].size() > 0);
        if (!args_info.no_reduce_conflicts_given and clusters[i].size() == 1) {
            continue;
        }

        std::cerr << i+1 << ". fired " << path[i]->getName();
        
        if (args_info.branches_given) {
            std::cerr << " - alternatives: " << clusters[i].size()-1;
        }

        std::cerr << "\n";
    }
}

void dotPath(std::vector<const Transition*> path, std::vector<std::set<const Transition*> > clusters) {
    assert(path.size() == clusters.size());
    
    std::cout << "digraph path {\n";

    for (int i = 0; i < path.size(); ++i) {
        std::cout << "m" << i << " -> " << "m" << i+1 << " [label=\"" << path[i]->getName() << "\"];\n";
    }

    std::cout << "}\n";
}


int main(int argc, char** argv) {
    evaluateParameters(argc, argv);

    // global variables
    PetriNet net;
    std::vector<const Transition*> path;
    std::vector<const Transition*> filtered_path;
    std::vector<Marking> filtered_markings;
    std::vector<std::set<const Transition*> > filtered_clusters;

    // read net
    {
        std::ifstream i;
        i.open(args_info.net_arg, std::ios_base::in);
        i >> io::lola >> net;
    }

    // read path
    {
        std::ifstream i;
        std::string line;
        bool first = true;
        i.open(args_info.path_arg, std::ios_base::in);
        while(std::getline(i, line)) {
            if (first) {
                first = false;
                continue;
            }

            Transition *t = net.findTransition(line);
            path.push_back(t);
        }
    }

    // process path
    PNAPI_FOREACH(t, path) {
        static Marking m(net);

        if (filtered_markings.empty()) {
            filtered_markings.push_back(m);
        }

        // get the activated transitions of the conflict cluster
        std::set<Node*> cluster_nodes = net.getConflictCluster(**t);
        std::set<const Transition*> cluster_transitions;
        PNAPI_FOREACH(ct, cluster_nodes) {
            const Transition* temp = dynamic_cast<Transition *>(*ct);
            if (temp and m.activates(*temp)) {
                cluster_transitions.insert(temp);
            }
        }

        // fire t
        m = m.getSuccessor(**t);

        // collect intermediate results
        if (cluster_transitions.size() > 1)
            filtered_path.push_back(*t);
        filtered_markings.push_back(m);
        filtered_clusters.push_back(cluster_transitions);
    }

    std::cerr << "STATS: " << path.size() << "," << filtered_path.size() << "\n";

    if (args_info.printpath_given) {
        printPath(filtered_path, filtered_clusters);
    }

    if (args_info.run_given) {
        dotPO(net, path, filtered_clusters, filtered_markings);
    }

    return EXIT_SUCCESS;
}
