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










