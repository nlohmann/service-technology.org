#include "rebeccaio.h"


using std::endl;
using std::ostream;
using std::set;
using std::string;


ostream & operator <<(ostream & os, const Choreography & chor)
{
  for (int i = 0; i < (int) chor.collaboration_.size(); ++i)
    os << "PEER " << chor.collaboration_[i]->name() << endl
       << "  IN " << chor.collaboration_[i]->in() << ";" << endl
       << "  OUT " << chor.collaboration_[i]->out() << ";" << endl << endl;
  os << "NODES" << endl;
  for (set<int>::iterator q = chor.states_.begin(); q != chor.states_.end();
      ++q)
  {
    os << "  " << *q;
    if (chor.initialState_ == *q)
    {
      os << " : INITIAL";
      if (chor.isFinal(*q))
        os << ", FINAL";
    }
    else
      if (chor.isFinal(*q))
        os << " : FINAL";
    os << endl;
    set<Edge *> E = chor.edgesFrom(*q);
    for (set<Edge *>::iterator e = E.begin(); e != E.end(); ++e)
    {
      string pre = (*e)->type == CHI ? "CHI" : "";
      os << "    " << pre << (*e)->label << " -> " << (*e)->destination << endl;
    }
  }
  return os;
}


ostream & operator <<(ostream & os, const PeerAutomaton & pa)
{
  if (pa.haveInput() || pa.haveOutput() || pa.haveSynchronous())
  {
    os << "INTERFACE" << endl;
    if (pa.haveInput())
    {
      os << "  INPUT "
         << pa.input()
         << ";" << endl;
    }
    if (pa.haveOutput())
    {
      os << "  OUTPUT "
         << pa.output()
         << ";" << endl;
    }
    if (pa.haveSynchronous())
    {
      os << "  SYNCHRONOUS "
         << pa.synchronous()
         << ";" << endl;
    }
  }
  os << endl;
  os << "NODES" << endl;
  for (set<set<int> >::iterator q = pa.states_.begin(); q != pa.states_.end();
      ++q)
  {
    os << "  " << *q;
    if (pa.initialState_ == *q)
    {
      os << " : INITIAL";
      if (pa.isFinal(*q))
        os << ",FINAL";
    }
    else
      if (pa.isFinal(*q))
        os << " : FINAL";
    os << endl;
    set<PEdge *> E = pa.edgesFrom(*q);
    for (set<PEdge *>::iterator e = E.begin(); e != E.end(); ++e)
      os << "    " << (*e)->label << " -> " << (*e)->destination << endl;
    os << endl;
  }
  os << endl;

  return os;
}


ostream & operator <<(ostream & os, const set<string> & s)
{
  bool first = true;
  for (set<string>::iterator i = s.begin(); i != s.end(); ++i)
    if (first)
    {
      first = false;
      os << *i;
    }
    else
      os << "," << *i;
  return os;
}


ostream & operator <<(ostream & os, const set<int> & s)
{
  for (set<int>::iterator i = s.begin(); i != s.end(); ++i)
    os << *i;
  return os;
}
