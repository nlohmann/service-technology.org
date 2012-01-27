/*
\file Net.cc
\author Karsten
\status approved 27.01.2012
\brief basic routines for handling nodes
*/

#include <cstdlib>
#include "Net.h"

index_t Net::Card[2] = {0, 0};
index_t* Net::CardArcs[2][2] = {{NULL, NULL}, {NULL, NULL}};
index_t** Net::Arc[2][2] = {{NULL, NULL}, {NULL, NULL}};
mult_t** Net::Mult[2][2] = {{NULL, NULL}, {NULL, NULL}};
char** Net::Name[2] = {NULL, NULL};

/// Aufräumen der Netzstruktur -- service für valgrind
void Net::deleteNodes()
{
    for (int type = PL; type <= TR; ++type)
    {
        for (int direction = PRE; direction <= POST; ++direction)
        {
            for (index_t i = 0; i < Net::Card[type]; i++)
            {
                free(Net::Arc[type][direction][i]);
                free(Net::Mult[type][direction][i]);
            }
            free(Net::CardArcs[type][direction]);
        }
        for (index_t i = 0; i < Net::Card[type]; i++)
        {
            free(Net::Name[type][i]);
        }
        free(Net::Name[type]);
    }
}
