#include <string>
#include <iostream>
#include "options.h"

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

struct option longopts[] =
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

