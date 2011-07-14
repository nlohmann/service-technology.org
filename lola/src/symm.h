/*****************************************************************************\
 LoLA -- a Low Level Petri Net Analyzer

 Copyright (C)  1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007,
                2008, 2009  Karsten Wolf <lola@service-technology.org>

 LoLA is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 LoLA is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with LoLA.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/


#pragma once

#include "net.h"
#include "dimensions.h"

#define NodeType int
#define PL 0  /* place */
#define TR 1  /* transition */
#define DomType int
#define DO 0 /* domain */
#define CO 1 /* codomain */


class SymmImage {
    public:
        Node* value;             // Knoten (**)
        unsigned int* vector;   // Eine Symmetrie, die Knoten (*) auf Knoten (**) abb.
};

class SymmStore {
    public:
        Node* arg;              // Knoten (*) ; alles drüber ist id
        unsigned int argnr, length, card, current; // Nr von (*), Länge von Image, Anz der benutzten Einträge, zum Iterieren
        unsigned int* reference;
        SymmImage* image;  // Alle möglichen Bilder für Knoten (*)
};

// union-find Datenstruktur
class Partition {
    public:
        unsigned int nextorcard;  // top + --> nr des Parent im Baum
        bool top;                 // top - --> is root und Anzahl der Elemente im Baum
};

extern Partition* part;

extern SymmStore* Store;                     // Das Erzeugendensystem
extern unsigned int CardStore;               // Länge des Vektors Store

void canonize_on_the_fly();                 // Kan. Rep ausrechnen ohne Erz System
void ComputePartition(void);                //Aeq Klassen ausrechnen
void ComputeSymmetries(void);               // Erz System ausrechnen
void FirstSymm(void);                      // Iterator Initialisierung
unsigned int NextSymm(unsigned int);       // Weiterschalten, so dass mind. bei <arg> neuer Funktionswert entsteht

extern unsigned int* CurrentSymm;

extern unsigned int* kanrep;  // Kanonischer Repräsentant
extern unsigned int* kanrep1;

extern unsigned int DeadBranches; // tote Zweige im REFINE/DEFINE Suchbaum
extern unsigned int CardGenerators; // Anzahl der Erzeugenden für alle Symms
extern unsigned int kanhash; // hash value of canonical representitive

void AllSyms();
void PrintStore();
void check();

