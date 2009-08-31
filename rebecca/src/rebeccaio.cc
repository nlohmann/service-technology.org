#include "rebeccaio.h"


using std::endl;
using std::ostream;
using std::set;
using std::string;


ostream & operator <<(ostream & os, const PeerAutomaton & pa)
{
  switch (pa.type_)
  {
  case CHOREOGRAPHY:
    {
      for (int i = 0; i < (int) pa.collaboration_.size(); i++)
        os << "PEER " << pa.collaboration_[i]->name() << endl
           << "  IN " << pa.collaboration_[i]->input() << ";" << endl
           << "  OUT " << pa.collaboration_[i]->output() << ";" << endl << endl;
      os << "NODES" << endl;
      for (set<int>::iterator q = pa.states_->begin(); q != pa.states_->end(); q++)
      {
        os << "  " << *q;
        if (*pa.initialState_ == *q)
        {
          os << " : INITIAL";
          if (pa.isFinal(*q))
            os << ", FINAL";
        }
        else
          if (pa.isFinal(*q))
            os << " : FINAL";
        os << endl;
        set<Edge *> E = pa.edgesFrom(*q);
        for (set<Edge *>::iterator e = E.begin(); e != E.end(); e++)
        {
          string pre = (*e)->type == SND ? "!" : (*e)->type == RCV ? "?" : (*e)->type == SYN ? "#" : "CHI";
          os << "    " << pre << (*e)->label << " -> " << (*e)->destination << endl;
        }
      }
      break;
    }
  case PROJECTION:
    {
      if (pa.haveInput() || pa.haveOutput() || pa.haveSynchronous())
      {
        os << "INTERFACE" << endl;
        if (pa.haveInput())
        {
          os << "  INPUT ";
          bool first = true;
          set<string> seen;
          for (set<PEdge *>::iterator e = pa.pEdges_->begin(); e != pa.pEdges_->end(); e++)
          {
            if ((*e)->type == RCV)
            {
              if (first)
              {
                os << (*e)->label;
                seen.insert((*e)->label);
                first = false;
              }
              else
                if (!seen.count((*e)->label))
                {
                  os << "," << (*e)->label;
                  seen.insert((*e)->label);
                }
            }
          }
          os << ";" << endl;
        }
        if (pa.haveOutput())
        {
          os << "  OUTPUT ";
          bool first = true;
          set<string> seen;
          for (set<PEdge *>::iterator e = pa.pEdges_->begin(); e != pa.pEdges_->end(); e++)
          {
            if ((*e)->type == SND)
            {
              if (first)
              {
                os << (*e)->label;
                seen.insert((*e)->label);
                first = false;
              }
              else
                if (!seen.count((*e)->label))
                {
                  os << "," << (*e)->label;
                  seen.insert((*e)->label);
                }
            }
          }
          os << ";" << endl;
        }
        if (pa.haveSynchronous())
        {
          os << "  SYNCHRONOUS ";
          bool first = true;
          set<string> seen;
          for (set<PEdge *>::iterator e = pa.pEdges_->begin(); e != pa.pEdges_->end(); e++)
          {
            if ((*e)->type == SYN)
            {
              if (first)
              {
                os << (*e)->label;
                seen.insert((*e)->label);
                first = false;
              }
              else
                if (!seen.count((*e)->label))
                {
                  os << "," << (*e)->label;
                  seen.insert((*e)->label);
                }
            }
          }
          os << ";" << endl;
        }
      }
      os << endl;
      os << "NODES" << endl;
      for (set<set<int> >::iterator q = pa.stateSets_->begin(); q != pa.stateSets_->end(); q++)
      {
        os << "  " << *q;
        if (*pa.initialStateSet_ == *q)
        {
          os << " : INITIAL";
          if (pa.isFinal(*q))
            os << ",FINAL";
        }
        else
          if (pa.isFinal(*q))
            os << " : FINAL";
        os << endl;
        set<PEdge *> E = pa.pEdges(*q);
        for (set<PEdge *>::iterator e = E.begin(); e != E.end(); e++)
          os << "    " << (*e)->label << " -> " << (*e)->destination << endl;
        os << endl;
      }
      os << endl;
      break;
    }
  default:
    break;
  }

  return os;
}


ostream & operator <<(ostream & os, const set<string> & s)
{
  bool first = true;
  for (set<string>::iterator i = s.begin(); i != s.end(); i++)
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
  for (set<int>::iterator i = s.begin(); i != s.end(); i++)
    os << *i;
  return os;
}
