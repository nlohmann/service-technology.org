
#ifndef STTYPES_H
#define STTYPES_H

#include<vector>
#include<set>
#include<map>

using std::pair;
using std::map;
using std::vector;
using std::set;

typedef unsigned int PlaceID;
typedef unsigned int TransitionID;
typedef map<PlaceID,int> TArcs;
typedef map<TransitionID,int> PArcs;
typedef vector<TransitionID> FiringSequence;
typedef map<TransitionID,int> FiringVector;
typedef map<PlaceID,int> Marking;

typedef unsigned int SiphonID;
typedef int ExtSiphonID;
typedef vector<PlaceID> Siphon;
typedef vector<SiphonID> SiphonList;
typedef SiphonList Wrapping;

typedef unsigned int TrapID;
typedef vector<PlaceID> Trap;
typedef set<PlaceID> Generators;
typedef vector<TrapID> TrapList;

typedef set<PlaceID> Interface;
typedef unsigned int InterfaceID;
typedef int ExtInterfaceID;
typedef vector<InterfaceID> InterfaceList;

typedef pair<InterfaceList,InterfaceList> Matching;
typedef unsigned int MatchingID;
typedef int ExtMatchingID;
typedef vector<MatchingID> MatchingList;

typedef int ExtGenerator;
typedef int Literal;
typedef int ExtTrapID;

#endif

