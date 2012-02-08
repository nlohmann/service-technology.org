#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "tools/lola.h"
#include "tools.h"
#include "types.h"
#include "utils.h"

/*!
 This function uses a system call to prepare the problem for lola.

 \param problem
 \return exit code (exit code of system call or 0 if the problem was already prepared)

 \note regardless of how often this function is being called, the problem is prepared only once.
 */
int lola_prepare(struct problem* problem) {
    static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    static int prepared = 0;
    int code;
    char cmd[256];

    pthread_mutex_lock(&mutex);
    if (prepared == 0) {
        prepared = 1;
        sprintf(cmd, "echo \"%s = %s\" | form", problem->name, problem->formula);
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
enum VerificationState lola_interpret(int code, char* logfile) {
    return WIFEXITED(code) ? code2state(code) : Undefined;
}

/*!
 \param problem
 \return struct representing lola-findpath
 */
struct tool* findpath_tool(struct problem* problem) {
    struct tool* findpath = malloc(sizeof(struct tool) + sizeof(char*) * 5);
    findpath->name = "findpath";
    findpath->prepare = lola_prepare;
    findpath->interpret = lola_interpret;
    findpath->program[0] = "lola-findpath";
    findpath->program[1] = problem->net;
    findpath->program[2] = "-a";
    findpath->program[3] = append(problem->name, ".task");
    findpath->program[4] = (char*)0;
    return findpath;
}

/*!
 \param problem
 \return struct representing lola-statepredicate
 */
struct tool* statepredicate_tool(struct problem* problem) {
    struct tool* statepredicate = malloc(sizeof(struct tool) + sizeof(char*) * 5);
    statepredicate->name = "statepredicate";
    statepredicate->prepare = lola_prepare;
    statepredicate->interpret = lola_interpret;
    statepredicate->program[0] = "lola-statepredicate";
    statepredicate->program[1] = problem->net;
    statepredicate->program[2] = "-a";
    statepredicate->program[3] = append(problem->name, ".task");
    statepredicate->program[4] = (char*)0;
    return statepredicate;
}
