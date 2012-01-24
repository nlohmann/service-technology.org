#ifndef TOOLS_H
#define TOOLS_H

#include "types.h"

typedef enum VerificationState(*interpreter)(int code, char* file);
typedef int (*preparer)(struct problem* problem);

/*!
 - name: the tool's name
 - prepare: function for preparing a problem
 - interpret: function for interpreting the tool's result
 - program: array containing the system call
 */
struct tool {
    char* name;
    preparer prepare;
    interpreter interpret;
    char* program[];
};

enum VerificationState forknrun(char** args, interpreter interpret, char* logfile);
enum VerificationState run(struct problem* problem, struct tool* tool);

#endif
