#pragma once

#include <map>
#include <vector>
#include <set>
#include <string>
#include "Output.h"


class GraphNode;

class Graph {
    private:
        static unsigned int r1, r2, r63, r62;

    public:
        static Graph *g;

    private:
        std::map<unsigned int, GraphNode*> nodes;

        /// nodes, ordered during the depth-first search
        std::vector<unsigned int> orderedNodes;

        /// mappings to map between ids and labels
        std::map<std::string, unsigned int> labels;
        std::map<unsigned int, std::string> labels2;

        /// a mapping to organize the reenumeration
        std::map<unsigned int, unsigned int> translate;

        /// the ids of deleted nodes (used to reenumerate)
        std::set<unsigned int> deleted;

        void mergeNode(unsigned int&, unsigned int&);
        void renameNode(unsigned int, unsigned int);
        void removeNode(unsigned int);
        void removeEdge(unsigned int, unsigned int, unsigned int);

        void tarjan(unsigned int = 0, bool = true);

        void init();
        void initLabels();
        Output *out();
        void shortInfo();
        void info();
        void reenumerate();
        void reenumerate2();

        bool metaRule();
        bool rule2();
        bool rule63();
        bool rule62();

    public:
        void addEdge(unsigned int, unsigned int, const char*);
        void addMarking(unsigned int, const char*, unsigned int);
        void addFinal(unsigned int);

        static Output *internalReduction(FILE *fullGraph);
        ~Graph();
};

class GraphNode {
    public:
        GraphNode();

        std::map<unsigned int, std::set<unsigned int> > preset;
        std::map<unsigned int, std::set<unsigned int> > postset;

        /// the lowlink value (calculated by depth-first search)
        unsigned int lowlink;

        /// the depth-first number
        unsigned int dfs;

        std::string scc;
        std::map<std::string, unsigned int> markings;

        bool isFinal;

        void addMarking(const char*, unsigned int);
};
