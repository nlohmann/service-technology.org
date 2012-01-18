
#include <fcntl.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/wait.h>
#include <unistd.h>

#include "tools.h"
#include "types.h"
#include "utils.h"

/*!
 Forks a process that runs the given program and returns the verification result.
 
 \param program that is executed
 \param function for interpreting the results
 \param logfile
 \return verification state (Undefined on error)
 */
enum VerificationState forknrun(char** args, interpreter interpret, char* logfile) {
    int state;
    int out = open(logfile, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
    pid_t pid;

    pid = fork();
    if (pid == 0) {
        debug_print("%s: running %s\n", __func__, args[0]);
        dup2(out, 1);
        dup2(out, 2);
        execvp(args[0], args);
        return Undefined; // this should never happen
    }

    pthread_cleanup_push(killProcess, (void*)&pid);
    debug_print("%s: waiting for pid %d\n", __func__, pid);
    waitpid(pid, &state, 0);
    debug_print("%s: pid %d returned\n", __func__, pid);
    pthread_cleanup_pop(1);
    return interpret(state, logfile);
}

/*!
 Runs a tool and returns the verification result.
 
 \param problem
 \param tool to solve the problem
 \return verification state (Undefined on error)
 */
enum VerificationState run(struct problem* problem, struct tool* tool) {
    char* log = append(append(append(tool->name, "."), problem->name), ".log");

    debug_print("%s: preparing (%s)\n", __func__, tool->name);
    if (tool->prepare(problem) == 0) {
        debug_print("%s: preparing (%s) succeeded\n", __func__, tool->name);
        return forknrun(tool->program, tool->interpret, log);
    }

    debug_print("%s: preparing (%s) failed\n", __func__, tool->name);
    return Undefined;
}
