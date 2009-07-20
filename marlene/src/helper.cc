
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <sstream>

#include "config.h"
#include "subversion.h"

#include "helper.h"
#include "macros.h"
#include "cmdline.h"

/// the command line parameters
gengetopt_args_info args_info;

// some initial values, which are the default values
Adapter::ControllerType contType = Adapter::SYNCHRONOUS;
unsigned int messageBound = 1;
bool veryverbose = false;
unsigned int veryverboseindent = 0;

void print_version (void)
{
#if defined(SVNREV)
  printf ("%s %s (rev. %s)\n", PACKAGE, VERSION, SVNREV);
#else
  printf ("%s %s\n", PACKAGE, VERSION);
#endif
}


/*!
 \brief abort with an error message and an error code
 
 The codes are documented in Marlenes's manual.
 
 \param code    the error code
 \param format  the error message formatted as printf string
*/
// code by Niels
void abort(unsigned int code, const char* format, ...) {
    fprintf(stderr, "%s: ", PACKAGE);

    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end (args);

    fprintf(stderr, " -- aborting [#%02d]\n", code);

    if (args_info.verbose_given) {
        fprintf(stderr, "%s: see manual for a documentation of this error\n", PACKAGE);
    }

    exit(EXIT_FAILURE);
}

/*!
 \brief
*/
// code by Niels
void status(const char* format, ...) {
    if (args_info.verbose_flag == 0) {
        return;
    }

    fprintf(stderr, "%s: ", PACKAGE);

    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end (args);

    fprintf(stderr, "\n");    
}

void evaluate_command_line(int argc, char* argv[])
{
    // overwrite invokation for consistent error messages
    argv[0] = PACKAGE;

    // set default values
    cmdline_parser_init(&args_info);

    // initialize the parameters structure
    struct cmdline_parser_params *params = cmdline_parser_params_create();

    // call the cmdline parser
    if (cmdline_parser(argc, argv, &args_info) != 0) {
        abort(7, "invalid command-line parameter(s)");
    }

    if (args_info.version_given)
    {
        print_version();
        exit(EXIT_SUCCESS);
    }
    
    // evaluation of options
    
    if (args_info.messagebound_given)
    {
        messageBound = args_info.messagebound_arg;
    }
    
    if (args_info.asyncif_given && args_info.asyncif_flag)
    {
        contType = Adapter::ASYNCHRONOUS;
    }
    
    if (args_info.veryverbose_given && args_info.veryverbose_flag)
    {
        veryverbose = true;
    }
    
    free(params);
    
}

/*!
 * Converts a C++ int to a C++ string object.
 *
 * \param i standard C int
 * \return  C++ string object representing i
 */
std::string toString(int i)
{
  std::ostringstream buffer;
  
  buffer << i;
  
  return buffer.str();
}

