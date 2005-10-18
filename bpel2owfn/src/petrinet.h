#ifndef PETRINET_H
#define PETRINET_H



#include <string>
#include <iostream>
#include <vector>



class Node
{
  public:
    std::string name;
    std::vector<std::string> roles;
};


class Arc
{
  public:
    Node *source;
    Node *target;    
    Arc(Node *source, Node *target);
};


class Transition: public Node
{
  private:
    unsigned int type;
  public:
    Transition(std::string name, std::string role, unsigned int type);
};



class Place: public Node
{
  private:
    unsigned int type;
  public:
    Place(std::string name, std::string role, unsigned int type);
};



class PetriNet
{
  public:
    Node *newPlace(std::string name, std::string role, unsigned int type);
    Node *newTransition(std::string name, std::string role, unsigned int type);
    Arc *newArc(Node *source, Node *target);
    void information();
    void drawDot();
    PetriNet();

  private:
    unsigned int places;
    unsigned int transitions;
    unsigned int arcs;
    std::vector<Place *> P;
    std::vector<Transition *> T;
    std::vector<Arc *> F;
};

#endif
