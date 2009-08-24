#ifndef TYPES_H
#define TYPES_H

#include <string>


enum EventType_enum
{
  SND, RCV, SYN, CHI
};
/// where SND = send, RCV = receive, and SYN = synchrone

typedef enum EventType_enum EventType;


struct Edge_struct
{
  int source;
  int destination;
  std::string label;
  EventType type;
};

typedef Edge_struct Edge;


struct PEdge_struct
{
  std::set<int> source;
  std::set<int> destination;
  std::string label;
  EventType type;
};

typedef PEdge_struct PEdge;


enum PeerAutomatonType_enum
{
  CHOREOGRAPHY, PROJECTION
};

typedef enum PeerAutomatonType_enum PeerAutomatonType;

#endif
