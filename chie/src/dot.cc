#include "dot.h"
#include "Output.h"
#include "verbose.h"
#include "util.h"
#include <queue>


/*!
 * \brief get the shortest path to the "bad node"
 */
std::map<unsigned int, unsigned int> getPathToBadNode(ProductAutomaton & a)
{
  // BFS queue
  std::queue<unsigned int> bfs;
  // predecessor relation
  std::map<unsigned int, unsigned int> predecessors;
  // set of seen nodes
  std::set<unsigned int> seen;

  // init queue
  bfs.push(1);
  seen.insert(1);

  // ID of current node
  unsigned int currentNodeID;

  // perform BFS
  while(true)
  {
    // get next node
    currentNodeID = bfs.front();
    bfs.pop();

    // breaking condition
    if(currentNodeID == a.badNode)
    {
      // bad node found
      break;
    }

    // get successors
    FOREACH(trans, a.transitions[currentNodeID])
    {
      FOREACH(succ, trans->second)
      {
        if(seen.count(succ->second) == 0)
        {
          seen.insert(succ->second);
          predecessors[succ->second] = currentNodeID;
          bfs.push(succ->second);
        }
      }
    }
  }

  // collect path
  std::map<unsigned int, unsigned int> result;
  result[currentNodeID = a.badNode] = a.badNode; // self loop so the bad node is in the result as well
  while(currentNodeID != 1)
  {
    result[predecessors[currentNodeID]] = currentNodeID;
    currentNodeID = predecessors[currentNodeID];
  }

  return result;
}

/*!
 * \brief write automaton do dot file with given filename
 */
void writeToDotFile(ProductAutomaton & a, std::string & fileName, bool writePath)
{
  // variable(s) used later (do not allocate memory within loops)
  bool notFirst;

  // path to bad node
  std::map<unsigned int, unsigned int> path;

  if(writePath)
  {
    path = getPathToBadNode(a);
  }

  // create output file
  Output out(fileName, "dot file");

  // write head
  out.stream() << "digraph G {\n"
               << " node [fontname=\"Helvetica\" fontsize=10]\n"
               << " edge [fontname=\"Helvetica\" fontsize=10]\n"
               << " INIT [label=\"\" height=\"0.01\" style=\"invis\"]\n"
               << " INIT -> \"1\" [minlen=\"0.5\"]\n";

  // iterate over product automaton statespace
  FOREACH(state, a.states)
  {
    // write current state
    out.stream() << " \"" << (state->first) << "\" [label=\""
                 << (state->first) << ": "
                 << (a.states[state->first].internalState.first) << " | "
                 << (a.states[state->first].internalState.second) << " | {";

    // write channel state
    ChannelState & cs = a.channelStates[state->second.interfaceStateID];
    notFirst = false;
    FOREACH(m, cs)
    {
      if(notFirst)
      {
        out.stream() << ", ";
      }
      out.stream() << a.events[*m];
      notFirst = true;
    }
    out.stream() << "}\"";

    // mark path to "bad node" red
    if(path.count(state->first) > 0)
    {
      out.stream() << " color=red";
    }

    out.stream() << "]\n";

    // write successors
    FOREACH(trans, a.transitions[state->first])
    {
      FOREACH(succ, trans->second)
      {
        out.stream() << " \"" << (state->first) << "\" -> \"" << (succ->second)
                     << "\" [label=\"";
        switch(succ->first)
        {
          case SENDING:
            out.stream() << "!" << (trans->first) << "\"";
            break;
          case RECEIVING:
            out.stream() << "?" << (trans->first) << "\"";
            break;
          case INTERNAL:
            out.stream() << "&tau;\"";
            break;
          default:
            assert(false); // should not happen
        }

        // mark path to "bad node" red
        if((path.count(state->first) > 0) && (path[state->first] == succ->second))
        {
          out.stream() << " color=red";
        }

        out.stream() << "]\n";
      }
    }
  }

  // finish file
  out.stream() << "}";
}
