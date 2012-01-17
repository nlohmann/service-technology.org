#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tools/sara.h"
#include "tools.h"
#include "types.h"
#include "utils.h"

int sara_prepare(struct problem *problem) {
  int code;
  char cmd[256];
  
  sprintf(cmd, "echo \"%s = %s\" > %s.formula", problem->name, problem->formula, problem->name);
  code = system(cmd);
  
  sprintf(cmd, "form2sara -n %s --lola -f %s.formula > %s.sara", problem->net, problem->name, problem->name);
  code = system(cmd);
  
  return code;
}

enum VerificationState sara_interpret(int code, char *logfile) {
  enum VerificationState state = Undefined;
  int length = 1024;
  char line[length];
  FILE *fp;
  
  fp = fopen(logfile, "r");
  while(state == Undefined && fgets(line, length, fp) != NULL) {
    if(strstr(line, "INFEASIBLE") != NULL) {
      state = Negative;
    } else if(strstr(line, "SOLUTION") != NULL) {
      state = Positive;
    }
  }
  pclose(fp);
  
  return state;
}

struct tool *sara_tool(struct problem *problem) {
  struct tool *sara = malloc(sizeof(struct tool) + sizeof(char*)*4);
  sara->name = "sara";
  sara->prepare = sara_prepare;
  sara->interpret = sara_interpret;
  sara->program[0] = "sara";
  sara->program[1] = "-i";
  sara->program[2] = append(problem->name, ".sara");
  sara->program[3] = (char*)0;
  return sara;
}
