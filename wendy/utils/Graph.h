#pragma once

#include <pnapi/pnapi.h>
#include <map>
#include <vector>
#include <set>
#include <string>


class Node;

class Graph {
    public:
        static pnapi::PetriNet net;

    private:
        static unsigned int r1, r2, r63, r62;

    private:
        std::map<unsigned int, Node*> nodes;
        std::map<std::string, unsigned int> labels;

        void merge(unsigned int, unsigned int);
        void removeNode(unsigned int);
        void removeEdge(unsigned int, unsigned int, unsigned int);

    public:
        unsigned int init;

        void tarjan(unsigned int, bool=true);

        void initLabels();
        void addEdge(unsigned int, unsigned int, std::string);
        void print();
        void info();

        bool rule2();
        bool rule63();
        bool rule62();
};

class Node {
    public:
        std::map<unsigned int, std::set<unsigned int> > preset;
        std::map<unsigned int, std::set<unsigned int> > postset;
        unsigned int lowlink;
        unsigned int dfs;
};
