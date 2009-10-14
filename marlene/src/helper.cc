/*****************************************************************************\
 Marlene -- synthesizing behavioral adapters

 Copyright (C) 2009  Christian Gierds <gierds@informatik.hu-berlin.de>

 Marlene is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Marlene is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Marlene.  If not, see <http://www.gnu.org/licenses/>. 
\*****************************************************************************/

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <cerrno>
#include <sstream>

#include "config.h"
#include "config-log.h"
//#include "subversion.h"

#include "helper.h"
#include "macros.h"
#include "cmdline.h"

/// the command line parameters
gengetopt_args_info args_info;

// some initial values, which are the default values
Adapter::ControllerType contType = Adapter::SYNCHRONOUS;
unsigned int messageBound = 1;
bool useCompPlaces = true;
bool veryverbose = false;
unsigned int veryverboseindent = 0;

void print_version (void)
{
#if defined(VERSION_SVN)
  printf ("%s %s (rev. %s)\n", PACKAGE, VERSION, VERSION_SVN);
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
__attribute__((noreturn)) void abort(unsigned short code, const char* format, ...) 
{
    fprintf(stderr, "%s: ", PACKAGE);

    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);

    fprintf(stderr, " -- aborting [#%02d]\n", code);

    status("see manual for a documentation of this error");

    if (errno != 0) {
        status("last error message: %s", strerror(errno));
    }

    exit(EXIT_FAILURE);
}

/*!
 \param format  the status message formatted as printf string
 
 \note use this function rather sparsely in order not to spam the output
*/
// code by Niels
void message(const char* format, ...) {
    fprintf(stderr, "%s: ", PACKAGE);

    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);

    fprintf(stderr, "\n");
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
    // argv[0] = PACKAGE;

    // set default values
    cmdline_parser_init(&args_info);

    // initialize the parameters structure
    struct cmdline_parser_params *params = cmdline_parser_params_create();

    // call the cmdline parser
    if (cmdline_parser(argc, argv, &args_info) != 0) {
        abort(7, "invalid command-line parameter(s)");
    }

    // debug option
    if (args_info.bug_flag) {
        FILE *debug_output = fopen("bug.log", "w");
        fprintf(debug_output, "%s\n", CONFIG_LOG);
        fclose(debug_output);
        fprintf(stderr, "Please send file 'bug.log' to %s.\n", PACKAGE_BUGREPORT);
        exit(EXIT_SUCCESS);
    }

    // read a configuration file if necessary
    if (args_info.config_given) {
        // initialize the config file parser
        params->initialize = 0;
        params->override = 0;

        // call the config file parser
        if (cmdline_parser_config_file (args_info.config_arg, &args_info, params) != 0) {
            abort(14, "error reading configuration file '%s'", args_info.config_arg);
        } else {
            status("using configuration file '%s'", args_info.config_arg);
        }
    } else {
        // check for configuration files
        std::string conf_filename = fileExists("marlene.conf") ? "marlene.conf" :
                               (fileExists(std::string(SYSCONFDIR) + "/marlene.conf") ?
                               (std::string(SYSCONFDIR) + "/marlene.conf") : "");

        if (conf_filename != "") {
            // initialize the config file parser
            params->initialize = 0;
            params->override = 0;
            if (cmdline_parser_config_file ((char*)conf_filename.c_str(), &args_info, params) != 0) {
                abort(14, "error reading configuration file '%s'", conf_filename.c_str());
            } else {
                status("using configuration file '%s'", conf_filename.c_str());
            }
        } else {
            status("not using a configuration file");
        }
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
    
    if (args_info.nocomplementplaces_given)
    {
        useCompPlaces = (args_info.nocomplementplaces_flag == 0);
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

bool fileExists(std::string filename) {
    FILE *tmp = fopen(filename.c_str(), "r");
    if (tmp) {
        fclose(tmp);
        return true;
    } else {
        return false;
    }
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

