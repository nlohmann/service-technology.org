#include "peer.h"

#include <iostream>

using std::set;
using std::string;


Peer::Peer(const string & name) :
  name_(name)
{
}


void Peer::pushInput(const string & inputLabel)
{
  input_.insert(inputLabel);
}


void Peer::pushOutput(const string & outputLabel)
{
  output_.insert(outputLabel);
}


const string Peer::name() const
{
  return name_;
}


const set<string> & Peer::input() const
{
  return input_;
}


const set<string> & Peer::output() const
{
  return output_;
}
