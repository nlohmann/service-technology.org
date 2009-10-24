#include "peer.h"

#include <iostream>

using std::set;
using std::string;


Peer::Peer(const string & name) :
  name_(name)
{
}


Peer::~Peer()
{
  in_.clear();
  out_.clear();
  input_.clear();
  output_.clear();
}


void Peer::pushIn(const string & inLabel)
{
  in_.insert(inLabel);
}


void Peer::pushOut(const string & outLabel)
{
  out_.insert(outLabel);
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


const set<string> & Peer::in() const
{
  return in_;
}


const set<string> & Peer::out() const
{
  return out_;
}


const set<string> & Peer::input() const
{
  return input_;
}


const set<string> & Peer::output() const
{
  return output_;
}


void Peer::deriveEvent(const string & label, CommunicationType t)
{
  switch (t)
  {
  case SYNCH:
    {
      if (in_.count(label))
        input_.insert("#"+label);
      if (out_.count(label))
        output_.insert("#"+label);
      break;
    }
  case ASYNCH:
    {
      if (in_.count(label))
        input_.insert("?"+label);
      if (out_.count(label))
        output_.insert("!"+label);
      break;
    }
  default:
    break;
  }
}
