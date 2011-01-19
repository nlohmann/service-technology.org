#pragma once

#include <map>
#include <vector>
#include <set>
#include <string>

class Node;

class Graph {
    private:
        static unsigned int l;

    private:
        std::map<unsigned int, Node*> nodes;
        std::map<std::string, unsigned int> labels;

        void merge(unsigned int, unsigned int);
        void removeNode(unsigned int);
        void removeEdge(unsigned int, unsigned int, unsigned int);

    public:
        void initLabels();
        void addEdge(unsigned int, unsigned int, std::string);
        void print();
        void info();

        bool rule613();
        bool rule62();
        bool r();
        bool r2();
};

class Node {
    public:
        std::map<unsigned int, std::set<unsigned int> > preset;
        std::map<unsigned int, std::set<unsigned int> > postset;
};
