#include <string>
#include <iostream>
#include <map>

#include "bpel2owfn.h"
#include "options.h"
#include "debug.h"

// some file names and pointers

/// Filename of input file.
std::string filename = "<STDIN>";

/// Filename of dot output file
std::string dot_filename = "";
/// Pointer to dot output file
std::ostream * dot_output = &std::cout;

/// Filename of APPN output file
std::string appn_filename = "";
/// Pointer to APPN output file
std::ostream * appn_output = &std::cout;

/// Filename of lola output file
std::string lola_filename = "";
/// Pointer to lola output file
std::ostream * lola_output = &std::cout;

/// Filename of owfn output file
std::string owfn_filename = "";
/// Pointer to owfn output file
std::ostream * owfn_output = &std::cout;

/// Filename of PN info file
std::string info_filename = "";
/// Pointer to PN info file
std::ostream * info_output = &std::cout;

// different modes controlled by command line

bool par_help = false;
bool par_version = false;

/// read from file
bool mode_file = false;
/// print the Petri Net
bool mode_petri_net = false;
/// simplify Petri Net
bool mode_simplify_petri_net = false;
/// paint Petri Net in APPN format
bool mode_appn_petri_net = false;
/// paint Petri Net in APPN format and output to file
bool mode_appn_2_file = false;
/// paint Petri Net for lola
bool mode_lola_petri_net = false;
/// paint Petri Net for lola and output to file
bool mode_lola_2_file = false;
/// paint Petri Net for owfn
bool mode_owfn_petri_net = false;
/// paint Petri Net for owfn and output to file
bool mode_owfn_2_file = false;
/// paint Petri Net with dot
bool mode_dot_petri_net = false;
/// paint Petri Net with dot and output to file
bool mode_dot_2_file = false;
/// CFG
bool mode_cfg = false;
/// pretty printer
bool mode_pretty_printer = false;
/// print AST
bool mode_ast = false;
/// print the "low level" Petri Net
bool mode_low_level_petri_net = false;

// long options
/*
static struct option longopts[] =
{
  { "help",       no_argument,       NULL, 'h' },
  { "version",    no_argument,       NULL, 'v' },
  { "mode",       required_argument, NULL, 'm' },
  { "log",        optional_argument, NULL, 'l' },
  { "input",      required_argument, NULL, 'i' },
  { "inputfile",  required_argument, NULL, 'i' },
  { "output",     optional_argument, NULL, 'o' },
  { "outputfile", optional_argument, NULL, 'o' },
  { "format",     required_argument, NULL, 'f' },
  { "parameter",  required_argument, NULL, 'p' },
  { "debug",      required_argument, NULL, 'd' }
};

const char * par_string = "hvm:l:i:of:p:d:";
*/
map<std::string, bool> parameters;


// --------------------- functions for command line evaluation ------------------------
/**
 * Prints an overview of all commandline arguments.
 *
 */
/*
void print_help() 
{
  // 80 chars
  //    "--------------------------------------------------------------------------------"
  trace("\n");
  trace(std::string(PACKAGE_STRING) + "\n");
  trace("\n");
  trace("Options: \n");
  trace("\n");
  trace("For more information see:\n");
  trace("  http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel\n");
  trace("\n");

}
*/
/**
 * Prints some version information
 *
 * \param name commandline name of the this program
 *
 *
 */
/*
void print_version(std::string name)
{
  trace(std::string(PACKAGE_STRING) + " -- ");
  trace("Translating BPEL Processes to Open Workflow Nets\n");
  trace("\n");
  trace("Copyright (C) 2005, 2006 Niels Lohmann, Christian Gierds and Dennis Reinert\n");
  trace("This is free software; see the source for copying conditions. There is NO\n");
  trace("warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n");
}
*/
/*
void parse_command_line(int argc, char* argv[])
{

  string progname = string(argv[0]);
  int optc = 0;
  while ((optc = getopt_long (argc, argv, par_string, longopts, (int *) 0))
         != EOF)
  {
    switch (optc)
      {
      case 'h':
	      parameters["help"] = true;
  	      break;
      case 'v':
	      par_version = true;
      	      break;
      case 'm':
	trace("-> mode\n");
	trace(optarg);
	trace("\n");
        break;
      case 'l':
	trace("-> log\n");
	if (optarg != NULL)
	{
	  trace(optarg);
	  trace("\n");
	}
        break;
      case 'i':
	trace("-> input\n");
	trace(optarg);
	trace("\n");
        break;
      case 'o':
	trace("-> output\n");
	if (optarg != NULL)
	{
	  trace(optarg);
	  trace("\n");
	}
        break;
      case 'f':
	trace("-> format\n");
	trace(optarg);
	trace("\n");
        break;
      case 'p':
	trace("-> parameter\n");
	trace(optarg);
	trace("\n");
        break;
      case 'd':
	trace("-> debug\n");
	trace(optarg);
	trace("\n");
        break;
      default:
	trace("unknown\n");
        break;
      }
      */
/*  
  if (optind == argc - 1 && strcmp (argv[optind], "sailor") == 0)
    z = 1;
  else if (lose || optind < argc)
    {
      if (optind < argc)
        fputs (_("Too many arguments\n"), stderr);
      fprintf (stderr, _("Try `%s --help' for more information.\n"), 
               progname);
      exit (1);
    }
*/
/*
  }

  if (parameters["help"])
  {
    print_help();
    exit(0);
  }
  if (par_version)
  {
    print_version("");
    exit(0);
  }
  exit(0);
}
*/
