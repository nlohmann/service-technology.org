#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#include "tools/lola.h"
#include "tools.h"
#include "problem.h"


/*!
 This function uses a system call to prepare the problem for lola.

 \param problem
 \return exit code (exit code of system call or 0 if the problem was already prepared)

 \note regardless of how often this function is being called, the problem is prepared only once.
 */
int Lola::prepare(Problem& problem) {
    static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    static int prepared = 0;
    int code;
    char cmd[256];

    pthread_mutex_lock(&mutex);
    if (prepared == 0) {
        prepared = 1;
        sprintf(cmd, "echo \"%s = %s\" | form", problem.name, problem.formula);
        code = system(cmd);
    } else {
        code = 0;
    }
    pthread_mutex_unlock(&mutex);

    return code;
}

/*!
 Interprets the exit code of lola to determine the verification result.

 \param exit code of lola
 \param logfile
 \return verification state
 */
enum Outcome::State Lola::interpret(int code, string logfile) {
    return WIFEXITED(code) ? Outcome::code2state(code) : Outcome::Undefined;
}

/*!
 \param problem
 \return struct representing lola-findpath
 */
Findpath::Findpath(Problem& problem) {
    name.assign("findpath");
	program = new char*[5];
    program[0] = (char*)"lola-findpath";
    program[1] = problem.net;
    program[2] = (char*)"-a";
    program[3] = new char[255]; //append(problem.name, (char*)".task");
	assert(strlen(problem.name)<250);
	strcpy(program[3],problem.name);
	strcat(program[3],(char*)".task");
    program[4] = (char*)0;
}

Findpath::~Findpath() {
	if (program) delete program[3];
	delete[] program;
}

/*!
 \param problem
 \return struct representing lola-statepredicate
 */
Statepredicate::Statepredicate(Problem& problem) {
    name.assign("statepredicate");
	program = new char*[4];
    program[0] = (char*)"lola-statepredicate";
    program[1] = problem.net;
//    program[2] = (char*)"-a";
    program[2] = new char[255]; //append((char*)"-a",append(problem.name, (char*)".task"));
	assert(strlen(problem.name)<248);
	strcpy(program[2],(char*)"-a");
	strcat(program[2],problem.name);
	strcat(program[2],(char*)".task");
    program[3] = (char*)0;
//    program[4] = (char*)0;
}

Statepredicate::~Statepredicate() {
	if (program) delete program[2];
	delete[] program;
}

