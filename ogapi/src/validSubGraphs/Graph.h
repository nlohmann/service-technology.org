#ifndef _GRAPH_H
#define _GRAPH_H

#include <map>
#include <cstdio>
#include <iostream>
#include "Node.h"
#include "types.h"
#include "helpers.h"

class Graph {
    public:
        enum Type { BIT = 1, FORMULA = 0, NOTDEFINED };

    private:
        /// the root node of the graph
        Node *root;

        /// the nodes indexed by an identifier
        std::map<unsigned int, Node*> nodes;

        /// the labels
        Labels labels;

        /// the maximal node id
        static unsigned int maxId;

        Type type;

    public:

        Cycles cycles;

        /// constructor
        Graph();

        GraphFormula graphformula;

//        int testAllAssignments(std::map<unsigned int, Labels> AssignmentMap, std::vector<Node*> nodes);

        /// set the root of the graph
        void setRoot(Node *n);

        /// set the graph's type
        void setType(Type t);

        /// adds a label to the graphs
        void addLabel(std::string l);

        /// add a node to the graph
        void addNode(Node *n);

        ClauseList* calculateGraphFormula(int noCycle_flag);

        ClauseList* addFormulaNotReachable(Node *node);

        ClauseList* addFormulaReachable(Node *node);

        ClauseList* addFormulaCycleReachable(Cycle *cycle, std::vector<int> cycleSeen);

        ClauseList* addFormulaCycleNotReachable(ClauseList* input);

        ClauseList* ClauseIntoNegClauseList(Clause clause);

        std::vector<int> edgeBelongsToCycles(NodeAndInEdge edge);

        ClauseList* combineClauses(ClauseList *list1, ClauseList *list2);

        void printFormulas();

        void printFormulasCNF();

        void printFormulasDNF();

        bool findCycles(Node *node, std::vector<NodeAndOutEdge> pathToNode);

        std::vector<NodeAndOutEdge> restVector(Node *node, std::vector<NodeAndOutEdge> nodeVector);

        bool insertCycleWithoutDuplicates(std::vector<NodeAndOutEdge> nodeVector);

        void computeIncomingEdgesForCycles();

        /// count the valid subgraphs of the given OG
        unsigned int countValidSubgraphs();

        /// help function
        AssignCount countFromNode(Node *node, std::vector<Node*> path);

        int wasVisitedByNode(Node *node, Node *from);

    private:
        /// input check
        Labels input() const;

        /// output check
        Labels output() const;

        /// synchronous check
        Labels synchronous() const;
};

#endif
