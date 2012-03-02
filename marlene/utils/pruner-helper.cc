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
#include <cstring>
#include <string>
#include <cerrno>
#include <sstream>

#include "config.h"
#include "config-log.h"
//#include "subversion.h"

#include "pruner-helper.h"
// #include "pruner-macros.h"
#include "pruner-cmdline.h"

/// the command line parameters
gengetopt_args_info args_info;

// some initial values, which are the default values

void print_version (void)
{
#if defined(VERSION_SVN)
    printf ("%s %s (rev. %s)\n", PACKAGE, VERSION, VERSION_SVN);
#else
    printf ("%s %s\n", PACKAGE, VERSION);
#endif
}

void evaluate_command_line(int argc, char* argv[])
{
    // overwrite invokation for consistent error messages
    // argv[0] = PACKAGE;

    // set default values
    //cmdline_parser_init(&args_info);

    // initialize the parameters structure
    struct cmdline_parser_params *params = cmdline_parser_params_create();

    // call the cmdline parser
    if (cmdline_parser(argc, argv, &args_info) != 0)
    {
        abort(7, "invalid command-line parameter(s)");
    }

    // read a configuration file if necessary
    if (args_info.config_given)
    {
        // initialize the config file parser
        params->initialize = 0;
        params->override = 0;

        // call the config file parser
        if (cmdline_parser_config_file (args_info.config_arg, &args_info, params) != 0)
        {
            abort(14, "error reading configuration file '%s'", args_info.config_arg);
        }
        else
        {
            status("using configuration file '%s'", args_info.config_arg);
        }
    }
    else
    {
        // check for configuration files
        std::string conf_filename = fileExists("pruner.conf") ? "pruner.conf" :
                                    (fileExists(std::string(SYSCONFDIR) + "/pruner.conf") ?
                                     (std::string(SYSCONFDIR) + "/pruner.conf") : "");

        if (conf_filename != "")
        {
            // initialize the config file parser
            params->initialize = 0;
            params->override = 0;
            if (cmdline_parser_config_file ((char*)conf_filename.c_str(), &args_info, params) != 0)
            {
                abort(14, "error reading configuration file '%s'", conf_filename.c_str());
            }
            else
            {
                status("using configuration file '%s'", conf_filename.c_str());
            }
        }
        else
        {
            status("not using a configuration file");
        }
    }

    if (args_info.version_given)
    {
        print_version();
        exit(EXIT_SUCCESS);
    }

    free(params);

}

bool fileExists(std::string filename)
{
    FILE *tmp = fopen(filename.c_str(), "r");
    if (tmp)
    {
        fclose(tmp);
        return true;
    }
    else
    {
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

