/*!
 * 	\file owfnPlace.cc -- implementation file
    \brief class for the places of an owfn net

	\author Daniela Weinberg
	\par History
		- 2006-01-16 creation

 */ 
#include "petriNetNode.h"
#include "owfn.h"
#include "dimensions.h"
#include "owfnPlace.h"

//! \fn owfnPlace::owfnPlace(char * name, placeType _type, oWFN * _net)
//! \param name
//! \param _type
//! \param _net
//! \brief constructor
owfnPlace::owfnPlace(char * name, placeType _type, oWFN * _net) : 
	Node(name), type(_type), index(0), capacity(0), nrbits(0), cardprop(0), proposition(NULL) {
  references = initial_marking = hash_factor = 0;
  net = _net;
}

//! \fn owfnPlace::~owfnPlace()
//! \brief constructor
owfnPlace::~owfnPlace() {
}

void owfnPlace::operator += (unsigned int i)
{
  initial_marking += i;
  net->placeHashValue += i*hash_factor;
  net->placeHashValue %= HASHSIZE;
/*  
  hash_value += i*hash_factor;
  hash_value %= HASHSIZE;
*/
}

void owfnPlace::operator -= (unsigned int i)
{
  initial_marking -= i;

  net->placeHashValue -= i*hash_factor;
  net->placeHashValue %= HASHSIZE;
/*
  hash_value -= i*hash_factor;
  hash_value %= HASHSIZE;
  */
}

bool owfnPlace::operator >= (unsigned int i)
{
  return((initial_marking >= i) ? 1 : 0);
}

void owfnPlace::set_hash(unsigned int i)
{
	
  net->placeHashValue -= hash_factor * initial_marking;
  hash_factor = i;
  net->placeHashValue += hash_factor * initial_marking;
  net->placeHashValue %= HASHSIZE;
/*	
  hash_value -= hash_factor * initial_marking;
  hash_factor = i;
  hash_value += hash_factor * initial_marking;
  hash_value %= HASHSIZE;
  */
}

void owfnPlace::set_marking(unsigned int i)
{
  net->placeHashValue -= hash_factor * initial_marking;
  initial_marking = i;
 net->placeHashValue += hash_factor * initial_marking;
  net->placeHashValue %= HASHSIZE;
  
/*
  hash_value -= hash_factor * initial_marking;
  initial_marking = i;
 hash_value += hash_factor * initial_marking;
  hash_value %= HASHSIZE;
*/
}  

void owfnPlace::set_cmarking(unsigned int i)
{
/*  hash_value -= hash_factor * CurrentMarking[index];
  CurrentMarking[index] = i;
 hash_value += hash_factor *CurrentMarking[index];
  hash_value %= HASHSIZE;*/
}  

placeType owfnPlace::getType() {
	return type;
}
