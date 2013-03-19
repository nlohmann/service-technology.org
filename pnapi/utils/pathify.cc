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
        PO_Type type;
        PO_Node(PO_Type);
};

PO_Node::PO_Node(PO_Type type) : type(type) {}

struct PO_Event : public PO_Node {
    const Transition *t;
    explicit PO_Event(const Transition *t);
};

PO_Event::PO_Event(const Transition *t) : PO_Node(EVENT), t(t) {}

struct PO_Condition : public PO_Node {
    const Place *p;
    PO_Event *pre;
    PO_Event *post;
    explicit PO_Condition(const Place *p);
};

PO_Condition::PO_Condition(const Place *p) : PO_Node(CONDITION), p(p), pre(NULL), post(NULL) {}


void dotPO(PetriNet &net, std::vector<const Transition*> path, std::vector<std::set<const Transition*> > clusters, std::vector<Marking> markings) {
    assert(path.size() == clusters.size());
    assert(path.size()+1 == markings.size());

    std::vector<PO_Event*> events;
    std::vector<PO_Condition*> conditions;
    std::map<const Node*, PO_Condition*> marked;
    std::map<PO_Node*, std::map<PO_Node*, int > > adjacency;

    // initial marking
    PNAPI_FOREACH(p, markings[0]) {
        if (p->second > 0) {
            // create condition
            PO_Condition *b = new PO_Condition(p->first);
            conditions.push_back(b);
            marked[p->first] = b;
        }
    }

    // traverse path
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

    /*
    const Place *target_p = net.findPlace("c1");
    assert(target_p);
    PO_Condition *target_b = marked[target_p];
    assert(target_b);
    */

    // dot
    std::cout << "digraph PO {\n";

    for (int i = 0; i < conditions.size(); ++i) {
        /*
        if (adjacency[conditions[i]][target_b] != 1 and conditions[i] != target_b) {
            continue;
        }
        */
        
        std::cout << (size_t)conditions[i] << " [shape=circle label=\"" << conditions[i]->p->getName() << "\"]\n";
        if (conditions[i]->pre)
            std::cout << (size_t)conditions[i]->pre << " -> " << (size_t)conditions[i] << ";\n";
        if (conditions[i]->post)
            std::cout << (size_t)conditions[i] << " -> " << (size_t)conditions[i]->post << ";\n";
    }

    for (int i = 0; i < events.size(); ++i) {
        /*
        if (adjacency[events[i]][target_b] != 1) {
            continue;
        }
        */

        std::cout << (size_t)events[i] << " [shape=box label=\"" << events[i]->t->getName() << "\"";
        
        if (clusters[i].size() > 1) {
            std::cout << " penwidth=5";
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

        // std::cerr << "net is " << (net.isFreeChoice() ? "" : "not ") << "free choice\n";
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

       //  std::cerr << "parsed path of length " << path.size() << "\n\n";
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

        // skip singleton clusters
        //if (reduce_conflicts && cluster_transitions.size() == 1) {
        //    continue;
        //}

        filtered_path.push_back(*t);
        filtered_markings.push_back(m);
        filtered_clusters.push_back(cluster_transitions);
    }

    std::cerr << "STATS: " << path.size() << "," << filtered_path.size() << "\n";

    if (args_info.printpath_given) {
        printPath(filtered_path, filtered_clusters);
    }

    if (args_info.run_given) {
        dotPO(net, filtered_path, filtered_clusters, filtered_markings);
    }

    //dotPath(filtered_path, filtered_clusters);

    return EXIT_SUCCESS;
}
