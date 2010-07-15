/** @file cmdline.h
 *  @brief The header file for the command line option parser
 *  generated by GNU Gengetopt version 2.22.2
 *  http://www.gnu.org/software/gengetopt.
 *  DO NOT modify this file, since it can be overwritten
 *  @author GNU Gengetopt by Lorenzo Bettini */

#ifndef CMDLINE_H
#define CMDLINE_H

/* If we use autoconf.  */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h> /* for FILE */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef CMDLINE_PARSER_PACKAGE
/** @brief the program name (used for printing errors) */
#define CMDLINE_PARSER_PACKAGE "petri"
#endif

#ifndef CMDLINE_PARSER_PACKAGE_NAME
/** @brief the complete program name (used for help and version) */
#define CMDLINE_PARSER_PACKAGE_NAME "petri"
#endif

#ifndef CMDLINE_PARSER_VERSION
/** @brief the program version */
#define CMDLINE_PARSER_VERSION "4.02-unreleased"
#endif

enum enum_input { input_arg_owfn = 0 , input_arg_lola, input_arg_pnml, input_arg_sa, input_arg_tpn };
enum enum_output { output_arg_owfn = 0 , output_arg_lola, output_arg_pnml, output_arg_sa, output_arg_tpn, output_arg_dot, output_arg_png, output_arg_pdf, output_arg_eps, output_arg_svg };
enum enum_check { check_arg_freechoice = 0 , check_arg_normal, check_arg_workflow };
enum enum_reduce { reduce_arg_0 = 0 , reduce_arg_1, reduce_arg_2, reduce_arg_3, reduce_arg_4, reduce_arg_5, reduce_arg_6, reduce_arg_starke, reduce_arg_pillat, reduce_arg_dead_nodes, reduce_arg_identical_places, reduce_arg_identical_transitions, reduce_arg_series_places, reduce_arg_series_transitions, reduce_arg_self_loop_places, reduce_arg_self_loop_transitions, reduce_arg_equal_places, reduce_arg_starke3p, reduce_arg_starke3t, reduce_arg_starke4, reduce_arg_starke5, reduce_arg_starke6, reduce_arg_starke7, reduce_arg_starke8, reduce_arg_starke9, reduce_arg_once, reduce_arg_k_boundedness, reduce_arg_boundedness, reduce_arg_liveness };

/** @brief Where the command line options are stored */
struct gengetopt_args_info
{
  const char *help_help; /**< @brief Print help and exit help description.  */
  const char *detailed_help_help; /**< @brief Print help, including all details and hidden options, and exit help description.  */
  const char *full_help_help; /**< @brief Print help, including hidden options, and exit help description.  */
  const char *version_help; /**< @brief Print version and exit help description.  */
  enum enum_input input_arg;	/**< @brief Select the input file format (default='owfn').  */
  char * input_orig;	/**< @brief Select the input file format original value given at command line.  */
  const char *input_help; /**< @brief Select the input file format help description.  */
  enum enum_output *output_arg;	/**< @brief Select the output file format(s).  */
  char ** output_orig;	/**< @brief Select the output file format(s) original value given at command line.  */
  unsigned int output_min; /**< @brief Select the output file format(s)'s minimum occurreces */
  unsigned int output_max; /**< @brief Select the output file format(s)'s maximum occurreces */
  const char *output_help; /**< @brief Select the output file format(s) help description.  */
  int canonicalNames_flag;	/**< @brief Renames nodes in Woflan nets to avoid problems with conversion to other formats (default=off).  */
  const char *canonicalNames_help; /**< @brief Renames nodes in Woflan nets to avoid problems with conversion to other formats help description.  */
  int formula_flag;	/**< @brief Places a final condition at the end of LoLA files. Has no effect to other output formats. (default=off).  */
  const char *formula_help; /**< @brief Places a final condition at the end of LoLA files. Has no effect to other output formats. help description.  */
  int guessFormula_flag;	/**< @brief Derive the final condition from the structure (i.e., the sink place) of the net. (default=off).  */
  const char *guessFormula_help; /**< @brief Derive the final condition from the structure (i.e., the sink place) of the net. help description.  */
  char * genet_arg;	/**< @brief Use Genet; specify the path (default='genet').  */
  char * genet_orig;	/**< @brief Use Genet; specify the path original value given at command line.  */
  const char *genet_help; /**< @brief Use Genet; specify the path help description.  */
  char * petrify_arg;	/**< @brief Use Petrify; specify the path (default='petrify').  */
  char * petrify_orig;	/**< @brief Use Petrify; specify the path original value given at command line.  */
  const char *petrify_help; /**< @brief Use Petrify; specify the path help description.  */
  char * compose_arg;	/**< @brief Compose each net given in the input section with this net..  */
  char * compose_orig;	/**< @brief Compose each net given in the input section with this net. original value given at command line.  */
  const char *compose_help; /**< @brief Compose each net given in the input section with this net. help description.  */
  char * produce_arg;	/**< @brief Calculate the product..  */
  char * produce_orig;	/**< @brief Calculate the product. original value given at command line.  */
  const char *produce_help; /**< @brief Calculate the product. help description.  */
  int isFreeChoice_flag;	/**< @brief Check if the Petri net is free choice, i.e. if conflicting transitions share all pre-places. (default=off).  */
  const char *isFreeChoice_help; /**< @brief Check if the Petri net is free choice, i.e. if conflicting transitions share all pre-places. help description.  */
  int isNormal_flag;	/**< @brief Check if the Petri net is normal, i.e. every transition is connect to at most one interface place. (default=off).  */
  const char *isNormal_help; /**< @brief Check if the Petri net is normal, i.e. every transition is connect to at most one interface place. help description.  */
  int isWorkflow_flag;	/**< @brief Check if the Petri net has workflow structure, i.e. it (i) has a distinct source place, (ii) has a district sink place, and (iii) every node lays on a path between the source place and the sink place. (default=off).  */
  const char *isWorkflow_help; /**< @brief Check if the Petri net has workflow structure, i.e. it (i) has a distinct source place, (ii) has a district sink place, and (iii) every node lays on a path between the source place and the sink place. help description.  */
  enum enum_check check_arg;	/**< @brief Check a structural property of the Petri net, see `isFreeChoice', `isNormal', and `isWorkflow' for details..  */
  char * check_orig;	/**< @brief Check a structural property of the Petri net, see `isFreeChoice', `isNormal', and `isWorkflow' for details. original value given at command line.  */
  const char *check_help; /**< @brief Check a structural property of the Petri net, see `isFreeChoice', `isNormal', and `isWorkflow' for details. help description.  */
  enum enum_reduce *reduce_arg;	/**< @brief Apply structural reduction rules, i.e. reduce the structure of the net while preserving liveness and boundedness..  */
  char ** reduce_orig;	/**< @brief Apply structural reduction rules, i.e. reduce the structure of the net while preserving liveness and boundedness. original value given at command line.  */
  unsigned int reduce_min; /**< @brief Apply structural reduction rules, i.e. reduce the structure of the net while preserving liveness and boundedness.'s minimum occurreces */
  unsigned int reduce_max; /**< @brief Apply structural reduction rules, i.e. reduce the structure of the net while preserving liveness and boundedness.'s maximum occurreces */
  const char *reduce_help; /**< @brief Apply structural reduction rules, i.e. reduce the structure of the net while preserving liveness and boundedness. help description.  */
  int normalize_flag;	/**< @brief Normalize the Petri net, i.e. change to structure such that every transition is connected to at most one interface place. (default=off).  */
  const char *normalize_help; /**< @brief Normalize the Petri net, i.e. change to structure such that every transition is connected to at most one interface place. help description.  */
  int negate_flag;	/**< @brief Negate the final condition of the net, i.e. every specified final marking is now non-final. The result is the 'anti open net'. (default=off).  */
  const char *negate_help; /**< @brief Negate the final condition of the net, i.e. every specified final marking is now non-final. The result is the 'anti open net'. help description.  */
  int mirror_flag;	/**< @brief Mirror the interface, i.e. change the direction of communication (default=off).  */
  const char *mirror_help; /**< @brief Mirror the interface, i.e. change the direction of communication help description.  */
  int dnf_flag;	/**< @brief Convert final condition to disjunctive normal form (default=off).  */
  const char *dnf_help; /**< @brief Convert final condition to disjunctive normal form help description.  */
  char * config_arg;	/**< @brief Read configuration from file..  */
  char * config_orig;	/**< @brief Read configuration from file. original value given at command line.  */
  const char *config_help; /**< @brief Read configuration from file. help description.  */
  char * dot_arg;	/**< @brief Set the path an binary of dot. (default='dot').  */
  char * dot_orig;	/**< @brief Set the path an binary of dot. original value given at command line.  */
  const char *dot_help; /**< @brief Set the path an binary of dot. help description.  */
  char * tmpfile_arg;	/**< @brief Set the path and name of temporary files (default='/tmp/petri-XXXXXX').  */
  char * tmpfile_orig;	/**< @brief Set the path and name of temporary files original value given at command line.  */
  const char *tmpfile_help; /**< @brief Set the path and name of temporary files help description.  */
  int noClean_flag;	/**< @brief Do not delete temporary files. (default=off).  */
  const char *noClean_help; /**< @brief Do not delete temporary files. help description.  */
  int verbose_flag;	/**< @brief Show verbose output (default=off).  */
  const char *verbose_help; /**< @brief Show verbose output help description.  */
  char ** suffix_owfn_arg;	/**< @brief Suffix for open net files.  */
  char ** suffix_owfn_orig;	/**< @brief Suffix for open net files original value given at command line.  */
  unsigned int suffix_owfn_min; /**< @brief Suffix for open net files's minimum occurreces */
  unsigned int suffix_owfn_max; /**< @brief Suffix for open net files's maximum occurreces */
  const char *suffix_owfn_help; /**< @brief Suffix for open net files help description.  */
  char ** suffix_sa_arg;	/**< @brief Suffix for service automaton files.  */
  char ** suffix_sa_orig;	/**< @brief Suffix for service automaton files original value given at command line.  */
  unsigned int suffix_sa_min; /**< @brief Suffix for service automaton files's minimum occurreces */
  unsigned int suffix_sa_max; /**< @brief Suffix for service automaton files's maximum occurreces */
  const char *suffix_sa_help; /**< @brief Suffix for service automaton files help description.  */
  int stats_flag;	/**< @brief Display time and memory consumption on termination. (default=off).  */
  const char *stats_help; /**< @brief Display time and memory consumption on termination. help description.  */
  
  unsigned int help_given ;	/**< @brief Whether help was given.  */
  unsigned int detailed_help_given ;	/**< @brief Whether detailed-help was given.  */
  unsigned int full_help_given ;	/**< @brief Whether full-help was given.  */
  unsigned int version_given ;	/**< @brief Whether version was given.  */
  unsigned int input_given ;	/**< @brief Whether input was given.  */
  unsigned int output_given ;	/**< @brief Whether output was given.  */
  unsigned int canonicalNames_given ;	/**< @brief Whether canonicalNames was given.  */
  unsigned int formula_given ;	/**< @brief Whether formula was given.  */
  unsigned int guessFormula_given ;	/**< @brief Whether guessFormula was given.  */
  unsigned int genet_given ;	/**< @brief Whether genet was given.  */
  unsigned int petrify_given ;	/**< @brief Whether petrify was given.  */
  unsigned int compose_given ;	/**< @brief Whether compose was given.  */
  unsigned int produce_given ;	/**< @brief Whether produce was given.  */
  unsigned int isFreeChoice_given ;	/**< @brief Whether isFreeChoice was given.  */
  unsigned int isNormal_given ;	/**< @brief Whether isNormal was given.  */
  unsigned int isWorkflow_given ;	/**< @brief Whether isWorkflow was given.  */
  unsigned int check_given ;	/**< @brief Whether check was given.  */
  unsigned int reduce_given ;	/**< @brief Whether reduce was given.  */
  unsigned int normalize_given ;	/**< @brief Whether normalize was given.  */
  unsigned int negate_given ;	/**< @brief Whether negate was given.  */
  unsigned int mirror_given ;	/**< @brief Whether mirror was given.  */
  unsigned int dnf_given ;	/**< @brief Whether dnf was given.  */
  unsigned int config_given ;	/**< @brief Whether config was given.  */
  unsigned int dot_given ;	/**< @brief Whether dot was given.  */
  unsigned int tmpfile_given ;	/**< @brief Whether tmpfile was given.  */
  unsigned int noClean_given ;	/**< @brief Whether noClean was given.  */
  unsigned int verbose_given ;	/**< @brief Whether verbose was given.  */
  unsigned int suffix_owfn_given ;	/**< @brief Whether suffix_owfn was given.  */
  unsigned int suffix_sa_given ;	/**< @brief Whether suffix_sa was given.  */
  unsigned int stats_given ;	/**< @brief Whether stats was given.  */

  char **inputs ; /**< @brief unamed options (options without names) */
  unsigned inputs_num ; /**< @brief unamed options number */
  int Automaton_Converter_group_counter; /**< @brief Counter for group Automaton_Converter */
} ;

/** @brief The additional parameters to pass to parser functions */
struct cmdline_parser_params
{
  int override; /**< @brief whether to override possibly already present options (default 0) */
  int initialize; /**< @brief whether to initialize the option structure gengetopt_args_info (default 1) */
  int check_required; /**< @brief whether to check that all required options were provided (default 1) */
  int check_ambiguity; /**< @brief whether to check for options already specified in the option structure gengetopt_args_info (default 0) */
  int print_errors; /**< @brief whether getopt_long should print an error message for a bad option (default 1) */
} ;

/** @brief the purpose string of the program */
extern const char *gengetopt_args_info_purpose;
/** @brief the usage string of the program */
extern const char *gengetopt_args_info_usage;
/** @brief all the lines making the help output */
extern const char *gengetopt_args_info_help[];
/** @brief all the lines making the full help output (including hidden options) */
extern const char *gengetopt_args_info_full_help[];
/** @brief all the lines making the detailed help output (including hidden options and details) */
extern const char *gengetopt_args_info_detailed_help[];

/**
 * The command line parser
 * @param argc the number of command line options
 * @param argv the command line options
 * @param args_info the structure where option information will be stored
 * @return 0 if everything went fine, NON 0 if an error took place
 */
int cmdline_parser (int argc, char * const *argv,
  struct gengetopt_args_info *args_info);

/**
 * The command line parser (version with additional parameters - deprecated)
 * @param argc the number of command line options
 * @param argv the command line options
 * @param args_info the structure where option information will be stored
 * @param override whether to override possibly already present options
 * @param initialize whether to initialize the option structure my_args_info
 * @param check_required whether to check that all required options were provided
 * @return 0 if everything went fine, NON 0 if an error took place
 * @deprecated use cmdline_parser_ext() instead
 */
int cmdline_parser2 (int argc, char * const *argv,
  struct gengetopt_args_info *args_info,
  int override, int initialize, int check_required);

/**
 * The command line parser (version with additional parameters)
 * @param argc the number of command line options
 * @param argv the command line options
 * @param args_info the structure where option information will be stored
 * @param params additional parameters for the parser
 * @return 0 if everything went fine, NON 0 if an error took place
 */
int cmdline_parser_ext (int argc, char * const *argv,
  struct gengetopt_args_info *args_info,
  struct cmdline_parser_params *params);

/**
 * Save the contents of the option struct into an already open FILE stream.
 * @param outfile the stream where to dump options
 * @param args_info the option struct to dump
 * @return 0 if everything went fine, NON 0 if an error took place
 */
int cmdline_parser_dump(FILE *outfile,
  struct gengetopt_args_info *args_info);

/**
 * Save the contents of the option struct into a (text) file.
 * This file can be read by the config file parser (if generated by gengetopt)
 * @param filename the file where to save
 * @param args_info the option struct to save
 * @return 0 if everything went fine, NON 0 if an error took place
 */
int cmdline_parser_file_save(const char *filename,
  struct gengetopt_args_info *args_info);

/**
 * Print the help
 */
void cmdline_parser_print_help(void);
/**
 * Print the full help (including hidden options)
 */
void cmdline_parser_print_full_help(void);
/**
 * Print the detailed help (including hidden options and details)
 */
void cmdline_parser_print_detailed_help(void);
/**
 * Print the version
 */
void cmdline_parser_print_version(void);

/**
 * Initializes all the fields a cmdline_parser_params structure 
 * to their default values
 * @param params the structure to initialize
 */
void cmdline_parser_params_init(struct cmdline_parser_params *params);

/**
 * Allocates dynamically a cmdline_parser_params structure and initializes
 * all its fields to their default values
 * @return the created and initialized cmdline_parser_params structure
 */
struct cmdline_parser_params *cmdline_parser_params_create(void);

/**
 * Initializes the passed gengetopt_args_info structure's fields
 * (also set default values for options that have a default)
 * @param args_info the structure to initialize
 */
void cmdline_parser_init (struct gengetopt_args_info *args_info);
/**
 * Deallocates the string fields of the gengetopt_args_info structure
 * (but does not deallocate the structure itself)
 * @param args_info the structure to deallocate
 */
void cmdline_parser_free (struct gengetopt_args_info *args_info);

/**
 * The config file parser (deprecated version)
 * @param filename the name of the config file
 * @param args_info the structure where option information will be stored
 * @param override whether to override possibly already present options
 * @param initialize whether to initialize the option structure my_args_info
 * @param check_required whether to check that all required options were provided
 * @return 0 if everything went fine, NON 0 if an error took place
 * @deprecated use cmdline_parser_config_file() instead
 */
int cmdline_parser_configfile (const char *filename,
  struct gengetopt_args_info *args_info,
  int override, int initialize, int check_required);

/**
 * The config file parser
 * @param filename the name of the config file
 * @param args_info the structure where option information will be stored
 * @param params additional parameters for the parser
 * @return 0 if everything went fine, NON 0 if an error took place
 */
int cmdline_parser_config_file (const char *filename,
  struct gengetopt_args_info *args_info,
  struct cmdline_parser_params *params);

/**
 * Checks that all the required options were specified
 * @param args_info the structure to check
 * @param prog_name the name of the program that will be used to print
 *   possible errors
 * @return
 */
int cmdline_parser_required (struct gengetopt_args_info *args_info,
  const char *prog_name);

extern const char *cmdline_parser_input_values[];  /**< @brief Possible values for input. */
extern const char *cmdline_parser_output_values[];  /**< @brief Possible values for output. */
extern const char *cmdline_parser_check_values[];  /**< @brief Possible values for check. */
extern const char *cmdline_parser_reduce_values[];  /**< @brief Possible values for reduce. */


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* CMDLINE_H */
