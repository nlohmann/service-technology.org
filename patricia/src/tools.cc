
#include <fcntl.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sstream>
#include <string>

#include "cmdline.h"
#include "verbose.h"
#include "tools.h"
#include "outcome.h"
#include "problem.h"
#include "ptask.h"

using std::stringstream;
using std::string;

extern gengetopt_args_info args_info;

/*!
	Virtual Destructor.
*/
Tool::~Tool() {}

/*!
 Forks a process that runs the given program and returns the verification result.

 \param program that is executed
 \param logfile
 \return verification state (Undefined on error)
 */
enum Outcome::State Tool::forknrun(char** args, string logfile) {
    int state;
    int cancel;
    int out = open(logfile.c_str(), O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
    pid_t pid = (pid_t)NULL;

    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &cancel);
    pthread_cleanup_push(killProcess, (void*)&pid);

    pid = fork();
    if (pid == 0) {
        pthread_setcancelstate(cancel, NULL); // TODO: Is this necessary?
		status("%s: running %s", __func__, args[0]);
        dup2(out, 1);
        dup2(out, 2);
        execvp(args[0], args);
        return Outcome::Undefined; // this should never happen
    }

    // enable cancellation
    pthread_setcancelstate(cancel, NULL);
	status("%s: waiting for pid %d", __func__, pid);
    waitpid(pid, &state, 0);
    status("%s: pid %d returned", __func__, pid);
    pthread_cleanup_pop(1);
    return interpret(state, logfile);
}

/*!
 Runs a tool and returns the verification result.

 \param problem
 \param tool to solve the problem
 \return verification state (Undefined on error)
 */
enum Outcome::State Tool::run(Problem& problem) {
	stringstream sstr;
	sstr << name << "." << problem.name << ".log";
	string log;
	sstr >> log;

    status("%s: preparing (%s)", __func__, name.c_str());
    if (args_info.inputs_num<3 || prepare(problem) == 0) {
        status("%s: preparing (%s) succeeded", __func__, name.c_str());
        return forknrun(program, log);
    }

    status("%s: preparing (%s) failed", __func__, name.c_str());
    return Outcome::Undefined;
}

