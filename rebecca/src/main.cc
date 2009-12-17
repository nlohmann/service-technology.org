#include <config.h>
#include <climits>
#include <cstdlib>
#include <libgen.h>
#include <string>
#include <vector>
#include "choreography.h"
#include "cmdline.h"
#include "rebeccaio.h"
#include "peerautomaton.h"
#include "verbose.h"
#include "config-log.h"
#include "Output.h"

/// the command line parameters
gengetopt_args_info args_info;

/// the invocation string
std::string invocation;

using std::string;
using std::vector;

/// the choreography
Choreography * chor;
/// the parser
extern int chor_parse();
extern FILE *chor_in;

/// evaluate the command line parameters
void evaluateParameters(int argc, char** argv)
{
  // overwrite invocation for consistent error messages
  argv[0] = basename(argv[0]);

  // store invocation in a std::string for meta information in file output
  for (int i = 0; i < argc; ++i)
  {
    invocation += std::string(argv[i]) + " ";
  }

  cmdline_parser_init(&args_info);

  // initialize the parameters structure
  struct cmdline_parser_params *params = cmdline_parser_params_create();

  // call the cmdline parser
  cmdline_parser(argc, argv, &args_info);

  // debug option
  if (args_info.bug_flag)
  {
    {
      Output debug_output("bug.log", "configuration information");
      debug_output.stream() << CONFIG_LOG;
    }
    message("Please send file '%s' to %s!", _cfilename_("bug.log"),
        _coutput_(PACKAGE_BUGREPORT));
    exit(EXIT_SUCCESS);
  }

  free(params);
}

int main(int argc, char** argv)
{
  // set a standard filename
  std::string filename = std::string(PACKAGE) + "_output";

  /*--------------------------------------.
   | 0. parse the command line parameters  |
   `--------------------------------------*/
  evaluateParameters(argc, argv);

  /*--------------------------------------.
   | 1. parse the chreography description  |
   `--------------------------------------*/
  if (args_info.inputs_num > 0)
  {
    chor_in = fopen(args_info.inputs[0], "r");
    if (!chor_in)
    {
      abort(1, "could not open the input file '%s'", args_info.inputs[0]);
    }
  }
  chor = new Choreography();
  status("parsing input choreography");
  chor_parse();
  fclose(chor_in);

  /*----------------------------.
   | 2. prepare data structures  |
   `----------------------------*/
  // * check whether choreography is sane (each input has an output)
  status("checking whether input model is a choreography");
  if (not chor->isChoreography())
  {
    abort(10, "choreography definition violated");
  }
  else
  {
    status("input model is a choreography");
  }

  // * create an array to quickly check whether two events are distant
  status("creating an array to quickly check whether two events are distant");
  bool** distantMessages = chor->distantEvents();

  std::vector<PeerAutomaton *> projected;
  for (size_t i = 0; i < chor->collaboration().size(); ++i)
  {
    PeerAutomaton * at = new PeerAutomaton();
    for (size_t a = 0; a < chor->events().size(); ++a)
    {
      if (chor->collaboration()[i]->input().count(chor->events()[a]))
      {
        for (std::set<Edge *>::iterator e = chor->edges().begin(); e
            != chor->edges().end(); ++e)
          if ((*e)->label == chor->events()[a])
          {
            if ((*e)->type == SYN)
            {
              at->pushSynchronous(chor->events()[a]);
            }
            else
            {
              at->pushInput(chor->events()[a]);
            }
            break;
          }
      }
      if (chor->collaboration()[i]->output().count(chor->events()[a]))
      {
        for (std::set<Edge *>::iterator e = chor->edges().begin(); e
            != chor->edges().end(); ++e)
        {
          if ((*e)->label == chor->events()[a])
          {
            if ((*e)->type == SYN)
            {
              at->pushSynchronous(chor->events()[a]);
            }
            else
            {
              at->pushOutput(chor->events()[a]);
            }
            break;
          }
        }
      }
    }
    projected.push_back(at);
  }

  bool complete = true;
  bool partial = true;
  bool distributed = true;

  /*---------------------------------------------.
   | 3. main algorithm: resolution of dependency  |
   `---------------------------------------------*/
  bool changed = true;
  while (changed)
  {
    changed = false;
    /// 3.1 if a disables b
    for (std::set<int>::iterator q = chor->states().begin(); q
        != chor->states().end(); ++q)
    {
      for (size_t a = 0; a < chor->events().size(); ++a)
      {
        for (size_t b = 0; b < chor->events().size(); ++b)
        {
          if (!distantMessages[a][b])
          {
            continue;
          }
          if (chor->disables(*q, a, b))
          {
            status("event %s disables event %s in state %d",
                chor->events()[a].c_str(), chor->events()[b].c_str(), *q);
            complete = false;
            changed = true;
            int qa = chor->createState();
            int qb = chor->createState();
            std::set<Edge *> qE = chor->edgesFrom(*q);
            for (std::set<Edge *>::iterator e = qE.begin(); e != qE.end(); ++e)
            {
              if ((*e)->label != chor->events()[a])
              {
                chor->createEdge(qa, (*e)->label, (*e)->destination, (*e)->type);
              }
              if ((*e)->label != chor->events()[b])
              {
                chor->createEdge(qb, (*e)->label, (*e)->destination, (*e)->type);
              }
              chor->deleteEdge(*e);
            }
            chor->createEdge(*q, "", qa, CHI);
            chor->createEdge(*q, "", qb, CHI);
          }
        }
      }
    }
    /// 3.2 if a enables b
    for (std::set<int>::iterator q = chor->states().begin(); q
        != chor->states().end(); ++q)
    {
      for (size_t a = 0; a < chor->events().size(); ++a)
      {
        for (size_t b = 0; b < chor->events().size(); ++b)
        {
          if (!distantMessages[a][b])
          {
            continue;
          }
          if (chor->enables(*q, a, b))
          {
            status("event %s enables event %s in state %d",
                chor->events()[a].c_str(), chor->events()[b].c_str(), *q);
            complete = distributed = false;
            changed = true;
            int qab = chor->findState(*q, a, b);
            chor->deleteState(qab);
          }
        }
      }
    }
    /// 3.3 if q_ab and q_ba are not equivalent
    for (std::set<int>::iterator q = chor->states().begin(); q
        != chor->states().end(); ++q)
    {
      for (size_t a = 0; a < chor->events().size(); ++a)
      {
        for (size_t b = 0; b < chor->events().size(); ++b)
        {
          if (!distantMessages[a][b])
          {
            continue;
          }
          int qab = chor->findState(*q, a, b);
          int qba = chor->findState(*q, b, a);
          if (qab != -1 && qba != -1 && !chor->equivalent(qab, qba))
          {
            status("states %d and %d are not equivalent", qab, qba);
            complete = distributed = false;
            changed = true;
            chor->unite(qab, qba);
          }
        }
      }
    }
  }

  if (chor->initialState() == INT_MIN)
  {
    partial = false;
  }

  // display analysis result
  message("choreography is completely realizable:    %s",
      (complete ? _cgood_("YES") : _cbad_("NO")));
  message("choreography is distributedly realizable: %s",
      (distributed ? _cgood_("YES") : _cbad_("NO")));
  message("choreography is partially realizable:     %s",
      (partial ? _cgood_("YES") : _cbad_("NO")));

  // output resolved choreography if requested
  if (args_info.chor_given)
  {
    std::string chor_filename = args_info.chor_arg ? args_info.chor_arg
        : filename + ".chor";
    Output output(chor_filename, "choreography");
    output.stream() << *chor;
  }

  /*--------------------------------.
   | 4. final step: peer projection  |
   `--------------------------------*/
  for (size_t i = 0; i < chor->collaboration().size(); ++i)
  {
    PeerAutomaton * a = projected[i];
    /*
     * 4.1 calculate q0
     */
    std::set<int> q0;
    q0.insert(chor->initialState());
    q0 = chor->closure(q0, i);
    a->pushState(q0);
    a->setInitialState(q0);
    if (chor->isFinal(q0))
    {
      a->pushFinalState(q0);
    }
    /*
     * 4.2 calculate all other states
     */
    for (std::set<std::set<int> >::iterator q = a->states().begin(); q
        != a->states().end(); ++q)
    {
      std::set<Edge *> E = chor->edgesFrom(*q);
      for (std::set<Edge *>::iterator e = E.begin(); e != E.end(); ++e)
      {
        std::set<int> qprime;
        if (a->isState((*e)->destination))
        {
          qprime = a->findState((*e)->destination);
          if (qprime == *q)
          {
            continue;
          }
        }
        else
        {
          qprime.insert((*e)->destination);
          qprime = chor->closure(qprime, i);
          a->pushState(qprime);
        }
        a->createEdge(*q, (*e)->label, qprime, (*e)->type);
        if (chor->isFinal(qprime))
        {
          a->pushFinalState(qprime);
        }
      }
    }
  }

  /*----------------.
   | 5. file output  |
   `----------------*/
  if (!args_info.quiet_flag)
  {
    // set file prefix
    std::string fprefix = "";
    if (args_info.prefix_given)
    {
      fprefix = args_info.prefix_arg;
      fprefix.push_back('.');
    }
    for (size_t i = 0; i < projected.size(); ++i)
    {
      std::string peer_filename(fprefix + "peer"
          + chor->collaboration()[i]->name() + ".sa");
      Output output(peer_filename, "peer automaton "
          + chor->collaboration()[i]->name());
      output.stream() << *projected[i];
    }
  }

  /*-------------.
   | 6. Clean up  |
   `--------------*/
  for (size_t i = 0; i < chor->events().size(); ++i)
  {
    delete[] distantMessages[i];
  }
  delete[] distantMessages;
  delete chor;
  for (size_t i = 0; i < projected.size(); ++i)
  {
    delete projected[i];
  }

  return EXIT_SUCCESS;
}
