
#include <assert.h>
#include <stdlib.h>
#include <pthread.h>

#include "types.h"

enum VerificationState code2state(int code) {
  switch(WEXITSTATUS(code)) {
    case  0: return Positive;
    case  1: return Negative;
    default: return Undefined;
  }
}

int state2code(enum VerificationState state) {
  assert(state == Positive || state == Negative || state == Undefined);
  
  if(state == Positive) {
    return 0;
  } else if(state == Negative) {
    return 1;
  } else {
    return 2;
  }
}

struct problem *mkProblem(int argc, char* argv[]) {
  struct problem *result;
  
  if(argc < 3) {
    return NULL;
  }
  
  result = malloc(sizeof(struct problem));
  result->name = argv[1];
  result->net = argv[2];
  result->formula = argv[3];
  return result;
}

void publish(struct outcome *outcome, enum VerificationState result, char *tool) {
  pthread_mutex_lock(&outcome->mutex);
  outcome->state = result;
  outcome->tool = tool;
  pthread_cond_signal(&outcome->cond);
  pthread_mutex_unlock(&outcome->mutex);
}
