#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tools/sara.h"
#include "tools.h"
#include "problem.h"


/*!
 Uses system calls to prepare the problem for sara.

 \param problem
 \return exit code
 */
int Sara::prepare(Problem& problem) {
    int code;
    char cmd[256];

    sprintf(cmd, "echo \"%s = %s\" > %s.formula", problem.name, problem.formula, problem.name);
    code = system(cmd);

    sprintf(cmd, "form2sara -n %s --lola -f %s.formula > %s.sara", problem.net, problem.name, problem.name);
    code = system(cmd);

    return code;
}


/*!
 Reads the logfile to obtain the verification result.

 \param exit code
 \param logfile
 \return verification result
 */
enum Outcome::State Sara::interpret(int code, string logfile) {
    enum Outcome::State state = Outcome::Undefined;
    int length = 1024;
    char line[length];
    FILE* fp;

    fp = fopen(logfile.c_str(), "r");
    while (state == Outcome::Undefined && fgets(line, length, fp) != NULL) {
        if (strstr(line, "INFEASIBLE") != NULL) {
            state = Outcome::Negative;
        } else if (strstr(line, "SOLUTION") != NULL) {
            state = Outcome::Positive;
        }
    }
    fclose(fp);

    return state;
}

/*!
 \param problem
 \return struct representing sara
 */
Sara::Sara(Problem& problem) {
    name.assign("sara");
	program = new char*[4];
    program[0] = (char*)"sara";
    program[1] = (char*)"-i";
    program[2] = new char[255]; //(char*)append(problem.name, (char*)".sara");
	assert(strlen(problem.name)<248);
	strcpy(program[2],problem.name);
	strcat(program[2],(char*)".sara");
    program[3] = (char*)0;
}

Sara::~Sara() {
	if (program) delete program[2];
	delete[] program;
}
