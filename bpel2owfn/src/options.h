#ifndef OPTIONS_H
#define OPTIONS_H

#include <map>
#include <list>
#include "getopt.h"


using namespace std;


typedef enum
{
   O_HELP,
   O_VERSION,
   O_INPUT,
   O_OUTPUT,
   O_MODE,
   O_BPEL2PN,
   O_LOG,
   O_FORMAT,
   O_PARAMETER,
   O_DEBUG
} possibleOptions;

typedef enum
{
  M_AST = 1,
  M_PRETTY,
  M_PETRINET,
  M_CONSISTENCY,
  M_CFG
} possibleModi;

typedef enum
{
  F_LOLA,
  F_OWFN,
  F_DOT,
  F_PEP,
  F_APNN,
  F_INFO,
  F_PNML,
  F_TXT,
  F_XML
} possibleFormats;

typedef enum
{
  P_SIMPLIFY,		///< structural reduction rules
  P_FINALLOOP,		///< live-lock "well-formed" systems
  P_NOSTANDARDFAULTS,	///< only user-defined faults can occur
  P_CYCLICWHILE,	///< use the original <while> pattern
  P_CYCLICEH,		///< use the original <eventHandlers> pattern
  P_NOFHFAULTS,		///< activities inside the FH throw no faults
  P_NOVARIABLES,	///< removes all variables from the model
  P_COMMUNICATIONONLY,	///< creates the smallest possible net
  P_NEWLINKS		///< enables the new link concept
} possibleParameters;


/******************************************************************************
 * External variables
 *****************************************************************************/

// some file names and pointers (in options.cc)

extern void parse_command_line(int argc, char* argv[]);


/// Filename of input file
extern string filename;
/// Filename of first input file
extern list <string> inputfiles;
/// Filename of input file
extern string output_filename;
/// Filename of log file
extern string log_filename;

/// pointer to input stream
extern istream * input;
/// pointer to output stream
extern ostream * output;
/// pointer to log stream
extern ostream * log_output;

extern bool createOutputFile;

// different modes controlled by command line (in options.cc)

extern possibleModi modus;

extern map<possibleOptions,    bool> options;
extern map<possibleParameters, bool> parameters;
extern map<possibleFormats,    bool> formats;
// suffixes are defined in parse_command_line();
extern map<possibleFormats,  string> suffixes;


/* defined by Bison */
extern int yydebug;

/* defined by flex */
extern int yy_flex_debug;
extern FILE *yyin;

// returns an open file pointer
extern ostream * openOutput(string name);
extern void closeOutput(ostream * file);

#endif

