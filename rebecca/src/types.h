#ifndef TYPES_H
#define TYPES_H

#include <set>
#include <string>


enum EventType
{
  SND, RCV, SYN, CHI
};
/// where SND = send, RCV = receive, and SYN = synchrone


enum CommunicationType
{
  SYNCH, ASYNCH
};


struct Edge
{
  int source;
  int destination;
  std::string label;
  EventType type;
};


struct PEdge
{
  std::set<int> source;
  std::set<int> destination;
  std::string label;
  EventType type;
};

#endif
