#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "cmdline.h"
#include "io.h"
#include "peerautomaton.h"
#include "verbose.h"

#include <iostream>

/// the command line parameters
gengetopt_args_info args_info;

using std::string;
using std::vector;

/// the choreography
PeerAutomaton * chor;
/// the parser
extern int chor_parse();

/// evaluate the command line parameters
void evaluateParameters(int argc, char** argv) {
    // set default values
    argv[0] = (char *)PACKAGE;

    cmdline_parser_init(&args_info);

    // initialize the parameters structure
    struct cmdline_parser_params *params = cmdline_parser_params_create();

    // call the cmdline parser
    cmdline_parser(argc, argv, &args_info);

    free(params);
}


int main(int argc, char** argv) {
    /*--------------------------------------.
    | 0. parse the command line parameters  |
    `--------------------------------------*/
    evaluateParameters(argc, argv);

    /*--------------------------------------.
    | 1. parse the chreography description  |
    `--------------------------------------*/
    chor = new PeerAutomaton();
    // * open the file
    status("parsing input choreography");
    chor_parse();
    // * close the file

    /*----------------------------.
    | 2. prepare data structures  |
    `----------------------------*/
    // * check whether choreography is sane (each input has an output)
    status("check whether choreography is sane (each input has an output)");
    if (!chor->isSane())
      abort(10, "collaboration not sane");
    status("choreography is sane");
    // * create an array to quickly check whether two events are distant
    status("create an array to quickly check whether two events are distant");
    bool ** distantMessages = chor->distantMessages();

    /*---------------------------------------------.
    | 3. main algorithm: resolution of dependency  |
    `---------------------------------------------*/
    for (int q = 0; q < (int) chor->states().size(); q++)
      for (int a = 0; a < (int) chor->events().size(); a++)
        for (int b = 0; b < (int) chor->events().size(); b++)
        {
          if (!distantMessages[a][b])
            continue;
          /// 3.1 if a disables b
          if (chor->disables(q, a, b))
          {
            int qa = chor->createState();
            int qb = chor->createState();
            std::set<Edge *> qE = chor->edges(q);
            for (std::set<Edge *>::iterator e = qE.begin(); e != qE.end(); e++)
            {
              if ((*e)->label != chor->events()[a])
                chor->createTransition(qa, (*e)->label, (*e)->destination, (*e)->type);
              if ((*e)->label != chor->events()[b])
                chor->createTransition(qb, (*e)->label, (*e)->destination, (*e)->type);
              chor->deleteTransition(*e);
            }
            chor->createTransition(q, "", qa, CHI);
            chor->createTransition(q, "", qb, CHI);
          }
          /// 3.2 if a enables b
          if (chor->enables(q, a, b))
          {
            int qab = chor->findState(q, a, b);
            chor->deleteState(qab);
          }
          /// 3.3 if q_ab and q_ba are not equivalent
          {
            int qab = chor->findState(q, a, b);
            int qba = chor->findState(q, b, a);
            if (qab != -1 && qba != -1 && !chor->equivalent(qab, qba))
            {
              // TODO
            }
          }
        }
    std::cout << *chor;

    /*--------------------------------.
    | 4. final step: peer projection  |
    `--------------------------------*/
    std::vector<PeerAutomaton *> projected;
    for (int i = 0; i < (int) chor->collaboration().size(); i++)
    {
      PeerAutomaton *a = new PeerAutomaton(PROJECTION);
      /*
       * 4.1 calculate q0
       */
      std::set<int> q0;
      q0.insert(chor->initialState());
      q0 = chor->closure(q0, i);
      a->pushState(q0);
      a->setInitialState(q0);
      if (chor->isFinal(q0))
        a->pushFinalState(q0);
      /*
       * 4.2 calculate all states
       */
      for (std::set<std::set<int> >::iterator q = a->stateSets().begin();
          q != a->stateSets().end(); q++)
      {
        std::set<Edge *> E = chor->edges(*q);
        for (std::set<Edge *>::iterator e = E.begin(); e != E.end(); e++)
        {
          std::set<int> qprime;
          if (a->isState((*e)->destination))
          {
            qprime = a->findStateSet((*e)->destination);
            if (qprime == *q)
              continue;
          }
          else
          {
            qprime.insert((*e)->destination);
            qprime = chor->closure(qprime, i);
            a->pushState(qprime);
          }
          a->createTransition(*q, (*e)->label, qprime, (*e)->type);
          if (chor->isFinal(qprime))
            a->pushFinalState(qprime);
        }
      }
      /*
       * 4.3 add peer automaton to projected peer automata
       */
      projected.push_back(a);
    }

    /*----------------.
    | 5. file output  |
    `----------------*/
    // set file prefix
    std::string fprefix = "auto_";
    std::ofstream file;
    for (int i = 0; i < (int) projected.size(); i++)
    {
      std::string filename = fprefix+chor->collaboration()[i]->name()+".sa";
      file.open(filename.c_str());
      file << *projected[i];
      file.close();
    }

    /*-------------.
    | 6. Clean up  |
    `--------------*/
    for (int i = 0; i < (int) chor->events().size(); i++)
      delete [] distantMessages[i];
    delete [] distantMessages;
    delete chor;
    for (int i = 0; i < (int) projected.size(); i++)
      delete projected[i];
    projected.clear();

    return EXIT_SUCCESS;
}
