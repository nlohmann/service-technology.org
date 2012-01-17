
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "actions.h"
#include "tools/lola.h"
#include "tools/sara.h"
#include "types.h"


struct action *plan(struct problem *problem) {
  struct action *findpath = tool2action(findpath_tool(problem), 200);
  struct action *sara = tool2action(sara_tool(problem), 0);
  struct action *statepredicate = tool2action(statepredicate_tool(problem), 0);
  return tryThen(findpath, parallelize(2, sara, statepredicate));
}


int main(int argc, char* argv[]) {
  struct outcome *outcome;
  struct problem *problem;
  
  problem = mkProblem(argc, argv);
  if(problem == NULL) {
    printf("%s <name> <net> <formula>\n", argv[0]);
    exit(1);
  }
  
  outcome = perform(problem, plan(problem));
  if(outcome->state == Positive) {
    printf("Verified (%s)\n", outcome->tool);
  } else if(outcome->state == Negative) {
    printf("Refuted (%s)\n", outcome->tool);
  } else {
    printf("Could neither verify nor refute");
  }
  
  // ensure that each cleanup handler is called!
  pthread_exit(NULL);
}

