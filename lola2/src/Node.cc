/*
\file Node.cc
\author Karsten
\status new
\brief basic routines for handling nodes
*/

#include <cstdlib>
#include "Node.h"

index_type Node::Card[2] = {0};
index_type* Node::CardArcs[2][2] = {{NULL, NULL}, {NULL, NULL}};
index_type** Node::Arc[2][2] = {{NULL, NULL}, {NULL, NULL}};
mult_type** Node::Mult[2][2] = {{NULL, NULL}, {NULL, NULL}};
char** Node::Name[2] = {NULL, NULL};

/// Aufräumen der Netzstruktur -- service für valgrind
void deleteNodes()
{
    for (int type = PL; type < TR; ++type)
    {
        for (int direction = PRE; direction < POST; ++direction)
        {
            for (index_type i = 0; i < Node::Card[type]; i++)
            {
                free(Node::Arc[type][direction][i]);
                free(Node::Mult[type][direction][i]);
            }
            free(Node::CardArcs[type][direction]);
        }
        for (index_type i = 0; i < Node::Card[type]; i++)
        {
            free(Node::Name[type][i]);
        }
        free(Node::Name[type]);
    }
}

index_type Node::Card[2] = {0}; 
index_type * Node::CardArcs[2][2] = {{NULL,NULL},{NULL,NULL}};
index_type ** Node::Arc[2][2] = {{NULL,NULL},{NULL,NULL}};
mult_type ** Node::Mult[2][2] = {{NULL,NULL},{NULL,NULL}};
char ** Node::Name[2] = {NULL};

