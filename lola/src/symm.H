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


#ifndef SYMM_H
#define SYMM_H

#include "net.H"
#include "dimensions.H"

class SymmImage
{
public:
  Node * value;
  unsigned int * vector;
};

class SymmStore
{
public:
  Node * arg;
  unsigned int argnr,length,card,current;
  unsigned int * reference;
  SymmImage * image;
};


class Partition
{
public:
  unsigned int nextorcard;
  bool top;
};

extern Partition * part;

extern SymmStore * Store;
extern unsigned int CardStore;

void canonize_on_the_fly();
void ComputePartition(void);
void ComputeSymmetries(void);
void FirstSymm(void);
unsigned int NextSymm(unsigned int);

extern unsigned int * CurrentSymm;

extern unsigned int * kanrep;
extern unsigned int * kanrep1;

extern unsigned int DeadBranches;
extern unsigned int CardGenerators;
extern unsigned int kanhash; // hash value of canonical representitive

void AllSyms();

#endif
