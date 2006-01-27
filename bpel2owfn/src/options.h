#ifndef OPTIONS_H
#define OPTIONS_H

#include <getopt.h>

// some file names and pointers (in main.cc)

/// Filename of input file.
extern std::string filename;

/// Filename of dot output file
extern std::string dot_filename;
/// Pointer to dot output file
extern std::ostream * dot_output;

/// Filename of APPN output file
extern std::string appn_filename;
/// Pointer to APPN output file
extern std::ostream * appn_output;

/// Filename of LoLA output file
extern std::string lola_filename;
/// Pointer to LoLA output file
extern std::ostream * lola_output;

/// Filename of oWFN output file
extern std::string owfn_filename;
/// Pointer to oWFN output file
extern std::ostream * owfn_output;

/// Filename of PN info file
extern std::string info_filename;
/// Pointer to PN info file
extern std::ostream * info_output;


// different modes controlled by command line (in main.cc)

/// read from file ?
extern bool mode_file;
/// print the Petri Net
extern bool mode_petri_net;
/// simplify Petri Net
extern bool mode_simplify_petri_net;
/// output Petri Net for APPN
extern bool mode_appn_petri_net;
/// output Petri Net for APPN to file
extern bool mode_appn_2_file;
/// output Petri Net for LoLA
extern bool mode_lola_petri_net;
/// output Petri Net for LoLA to file
extern bool mode_lola_2_file;
/// output Petri Net for oWFN
extern bool mode_owfn_petri_net;
/// output Petri Net for oWFN to file
extern bool mode_owfn_2_file;
/// paint Petri Net with dot
extern bool mode_dot_petri_net;
/// paint Petri Net with dot and output to file
extern bool mode_dot_2_file;
/// pretty printer
extern bool mode_cfg;
/// pretty printer
extern bool mode_pretty_printer;
/// print AST
extern bool mode_ast;
/// print the "low level" Petri Net
extern bool mode_low_level_petri_net;

/// debug level (in debug.cc)
extern int debug_level;


/* defined by Bison */
extern int yydebug;

/* defined by flex */
extern int yy_flex_debug;
extern FILE *yyin;

// long options


#endif

