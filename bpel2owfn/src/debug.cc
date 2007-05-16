/*****************************************************************************\
 * Copyright 2007       Niels Lohmann                                        *
 * Copyright 2005, 2006 Niels Lohmann, Christian Gierds                      *
 *                                                                           *
 * This file is part of GNU BPEL2oWFN.                                       *
 *                                                                           *
 * GNU BPEL2oWFN is free software; you can redistribute it and/or modify it  *
 * under the terms of the GNU General Public License as published by the     *
 * Free Software Foundation; either version 2 of the License, or (at your    *
 * option) any later version.                                                *
 *                                                                           *
 * GNU BPEL2oWFN is distributed in the hope that it will be useful, but      *
 * WITHOUT ANY WARRANTY; without even the implied warranty of                *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General  *
 * Public License for more details.                                          *
 *                                                                           *
 * You should have received a copy of the GNU General Public License along   *
 * with GNU BPEL2oWFN; see file COPYING. if not, write to the Free Software  *
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA. *
\*****************************************************************************/

/*!
 * \file    debug.cc
 *
 * \brief   debugging tools
 *
 * \author  Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          Christian Gierds <gierds@informatik.hu-berlin.de>,
 *          last changes of: \$Author: znamirow $
 *
 * \since   2005/11/09
 *          
 * \date    \$Date: 2007/05/16 11:52:39 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \version \$Revision: 1.102 $
 *
 * \ingroup debug
 */





/******************************************************************************
 * Headers
 *****************************************************************************/

#include <iostream>
#include <fstream>
#include <iomanip>

#include "debug.h"
#include "options.h"
#include "bpel2owfn.h"
#include "helpers.h"	// for toInt
#include "globals.h"
#include "colorconsole.h"

using std::cerr;
using std::clog;
using std::cout;
using std::endl;
using std::setw;
using std::setfill;
using std::flush;
using std::ofstream;





/******************************************************************************
 * Global variables
 *****************************************************************************/

/*!
 * \brief debug level
 *
 * The command-line parameter "-d" can be used to set a debug level.
 *
 * \see #trace_level
 * \see #frontend__flex_debug
 * \see #frontend_debug
 *
 * \ingroup debug
 */
trace_level debug_level = TRACE_ALWAYS;





/******************************************************************************
 * Functions to indicate errors, warnings or observations
 *****************************************************************************/

/*!
 * \brief traces a string to the log stream
 *
 * Prints the string message to the output string #log_output if the passed
 * debug level #debug_level is greater or equal to the set debug level
 * #debug_level.
 *
 * \param pTraceLevel	the #trace_level
 * \param message	the output
 *
 * \ingroup debug
 */
void trace(trace_level pTraceLevel, string message)
{
  if (pTraceLevel <= debug_level)
    (*log_output) << message << flush;
}





/*!
 * \brief traces a string to the log stream
 * \overload
 * \ingroup debug
 */
void trace(string message)
{
  trace(TRACE_ALWAYS, message);
}





void show_process_information_header()
{
  if (debug_level == TRACE_ERROR)
    return;

  cerr << endl;
  cerr << "==============================================================================" << endl;
  cerr << PACKAGE_STRING << " reading from file `" << globals::filename << "'" << endl;
  cerr << "------------------------------------------------------------------------------" << endl;
}





/*!
 * \brief print information about the proces
 */
void show_process_information()
{
  extern int frontend_nerrs;

  if (debug_level == TRACE_ERROR)
    return;

  if (frontend_nerrs + globals::static_analysis_errors + globals::other_errors > 0)
    cerr << "------------------------------------------------------------------------------" << endl;

  cerr << globals::process_information.basic_activities +
    globals::process_information.structured_activities +
    globals::process_information.scopes << " activities";
  cerr << " (" << globals::process_information.basic_activities <<
    " basic, ";
  cerr << globals::process_information.structured_activities <<
    " structured, ";
  cerr << globals::process_information.scopes << " scopes) + ";
  cerr << globals::process_information.implicit_activities << " implicit activities" << endl;

  cerr << globals::process_information.fault_handlers +
    globals::process_information.termination_handlers +
    globals::process_information.compensation_handlers +
    globals::process_information.event_handlers << " handlers (";
  cerr << globals::process_information.fault_handlers << " FH, ";
  cerr << globals::process_information.termination_handlers << " TH, ";
  cerr << globals::process_information.compensation_handlers << " CH, ";
  cerr << globals::process_information.event_handlers << " EH) + ";
  cerr << globals::process_information.implicit_handlers << " implicit handlers" << endl;

  cerr << globals::process_information.links << " links, ";
  cerr << globals::process_information.variables << " variables" << endl;

  cerr << endl;


  if (frontend_nerrs == 0)
  {
    cerr << colorconsole::fg_green << "[SYNTAX ANALYSIS] " << colorconsole::fg_standard;
    cerr << "No syntax errors found." << endl;
  }
  else
  {
    cerr << colorconsole::fg_red << "[SYNTAX ANALYSIS] " << colorconsole::fg_standard;
    cerr << "Found " << frontend_nerrs << " syntax errors." << endl;
  }


  if (globals::static_analysis_errors == 0)
  {
    cerr << colorconsole::fg_green << "[STATIC ANALYSIS] " << colorconsole::fg_standard;
    cerr << "No errors found checking 51 statics analysis requirements." << endl;
  }
  else if (globals::abstract_process)
  {
    cerr << colorconsole::fg_magenta << "[STATIC ANALYSIS] " << colorconsole::fg_standard;
    cerr << "Found " << globals::static_analysis_errors << " warnings using static analysis." << endl;
  }
  else
  {
    cerr << colorconsole::fg_red << "[STATIC ANALYSIS] " << colorconsole::fg_standard;
    cerr << "Found " << globals::static_analysis_errors << " errors using static analysis. A WS-BPEL engine must reject this process." << endl;
  }

  if (globals::other_errors == 0)
  {
    cerr << colorconsole::fg_green << "[OTHER ANALYSIS]  " << colorconsole::fg_standard;
    cerr << "No other errors found." << endl;
  }
  else
  {
    cerr << colorconsole::fg_red << "[OTHER ANALYSIS]  " << colorconsole::fg_standard;
    cerr << "Found " << globals::other_errors << " further errors." << endl;
  }

  if (modus == M_PETRINET || modus == M_CONSISTENCY)
    cerr << "------------------------------------------------------------------------------" << endl;
}





/*!
 * \brief print information about the proces
 */
void show_wsdl_information()
{
  if (debug_level == TRACE_ERROR)
    return;

  cerr << globals::wsdl_information.imports << " imports, ";
  cerr << globals::wsdl_information.types << " types, ";
  cerr << globals::wsdl_information.bindings << " bindings, ";
  cerr << globals::wsdl_information.properties << " properties" << endl;
  cerr << globals::wsdl_information.messages << " messages, ";
  cerr << globals::wsdl_information.portTypes << " portTypes, ";
  cerr << globals::wsdl_information.operations << " operations, ";
  cerr << globals::wsdl_information.partnerLinkTypes << " partnerLinkTypes" << endl;  
}





/*!
 * \brief signal a syntax error
 *
 * This function is invoked by the parser and the lexer during the syntax
 * analysis. When an error occurs, it prints an accordant message and shows
 * the lines of the input files where the error occured.
 *
 * \param msg a message (mostly "Parse error") and some more information e.g.
 *            the location of the syntax error.
 * \return 1, since an error occured
 *
 * \ingroup debug
 */
int frontend_error(const char *msg)
{
  /* defined by flex */
  extern int frontend_lineno;      // line number of current token
  extern char *frontend_text;      // text of the current token

  cerr << colorconsole::fg_blue;
  cerr << globals::filename << ":" << frontend_lineno+1 << " - [SYNTAX]\n";
  cerr << colorconsole::fg_standard;

  if (debug_level == TRACE_ERROR)
    return 1;

  cerr << string(msg);

  if (debug_level >= TRACE_WARNINGS)
    cerr << "; last token read: `" << string(frontend_text) << "'" << endl << endl;
  else
    cerr << endl << endl;

  // remember the last token
  globals::last_error_token = string(frontend_text);
  globals::last_error_line = toString(frontend_lineno);

  return 1;
}





/*!
 * \brief prints a generic error message
 *
 * \param information   additional information about the error
 * \param line          the position in the source file
 * \param error         when set to true, prefixes the message with "[ERROR]",
 *                      otherwise prefixes the message with "[WARNING]"
 *                      (standard)
 *
 * \todo  Comment me
 */
void genericError(unsigned int code, string information, string line, error_level level)
{
  switch (level)
  {
    case(ERRORLEVEL_NOTICE):	cerr << colorconsole::fg_gray; break;
    case(ERRORLEVEL_SYNTAX):	cerr << colorconsole::fg_blue; break;
    case(ERRORLEVER_WARNING):	cerr << colorconsole::fg_magenta; break;
    case(ERRORLEVEL_ERROR):	cerr << colorconsole::fg_red; globals::other_errors++; break;
    case(ERRORLEVEL_CRITICAL):	cerr << colorconsole::fg_red; globals::other_errors++; break;
  }

  cerr << globals::filename;
 
  if (line != "")
    cerr << ":" << line;
 
  cerr  << " - ";

  switch (level)
  {
    case(ERRORLEVEL_NOTICE):	cerr << "[N"; break;
    case(ERRORLEVEL_SYNTAX):	cerr << "[S"; break;
    case(ERRORLEVER_WARNING):	cerr << "[W"; break;
    case(ERRORLEVEL_ERROR):	cerr << "[E"; break;
    case(ERRORLEVEL_CRITICAL):	cerr << "[C"; break;
  }

  cerr << setfill('0') << setw(5) << code;
  cerr << "]\n";
  cerr << colorconsole::fg_standard;

  if (debug_level == TRACE_ERROR)
    return;

  switch (code)
  {
    case(100): // skipped misplaced element
      { cerr << "skipped <" << information << ">: non-standard or misplaced element" << endl;
	globals::unknown_elements.insert(information);
	break; }

    case(101): // skipped error in <partners>
      { cerr << "skipped <partners> due to syntax error" << endl;
	break; }

    case(102): // skipped error in <from> or <to>
      { cerr << "skipped <" << information << "> due to syntax error" << endl;
	break; }

    case(103): // skipped error in <condition>
      { cerr << "skipped <condition> due to syntax error" << endl;
	break; }

    case(104): // abort due syntax error
      { cerr << "cannot proceed due to syntax errors";

	if (!globals::unknown_elements.empty())
	{
	  cerr << "; try to remove these elements:" << endl;
	  for (set<string>::iterator el = globals::unknown_elements.begin(); el != globals::unknown_elements.end(); el++)
	  {
	    if (*el != "scope")
	      cerr << "  <" << *el << ">" << endl;
	  }
	}
	else
	  cerr << endl;

	break; }

    case(105): // process despite syntax error
      { cerr << "some elements were skipped due to syntax errors" << endl;
	break; }

    case(106): // conflicting receives
      { cerr << information << endl;
	break; }

    case(107): // attribute missing
      { cerr << "required attribute " << information << endl;
	break; }
	  
    case(108): // attribute type error
      { cerr << "type error: " << information << endl;
	break; }

    case(109): // undefined variable
      { cerr << information << endl;
	break; }

    case(110): // undefined partner link
      { cerr << information << endl;
	break; }

    case(111): // undefined correlation set
      { cerr << information << endl;
	break; }

    case(112): // skipped error in literal
      { cerr << "skipped <literal> due to syntax error" << endl;
	break; }

    case(113): // skipped UTF-8 character
      { cerr << "skipped unsupported UTF-8 character `" << information << "'; this message is only shown once" << endl;
	break; }

    case(114): // uninitialized variable
	{ cerr << information << endl;
	  break; }
	
    case(115): // abstract process
	{ cerr << "process is abstract; missing attributes might hamper translation and result in unnecessary warnings" << endl;
	  break; }

    case(116): // <opaqueActivity> replaced by <empty>
	{ cerr << "assuming <empty> semantics for <opaqueActivity>" << endl;
	  break; }

    case(117): // ignoring attribute suppressJoinFailure="no"
	{ cerr << "ignoring attribute `suppressJoinFailure' set to \"no\" in <" << information << ">; modeling \"yes\"" << endl;
	  break; }

    case(118): // ignoring user-defined transition condition (set to true instead)
	{ cerr << "ignoring user-defined <transitionCondition> in <" << information << ">; modeling \"true\"" << endl;
	  break; }

    case(119): // ignoring user-defined transition condition (set to XOR instead)
	{ cerr << "ignoring user-defined <transitionCondition> in <" << information << ">; modeling \"1-out-of-n\"" << endl;
	  break; }

    case(120): // ignoring <scope> handlers in mode communicationonly
	{ cerr << "FTC handlers of <scope> activity are not modeled" << endl;
	  break; }

    case(121): // ignoring activity in mode communicationonly
	{ cerr << "<" << information << "> activity is modeled by <empty> activity" << endl;
	  break; }

    case(122): // syntax error in chorography file
	{ cerr << "syntax error in chorography file" << endl;
	  break; }

    case(123): // syntax error in WSDL file
	{ cerr << "syntax error in WSDL file" << endl;
	  break; }

    case(124): // ignoring XML Schema
	{ if (information == "/")
	    cerr << "skipped empty XML Schema element" << endl;
	  else
	    cerr << "skipped XML Schema element: <" << information << ">" << endl;
	  break; }

    case(125): // ignoring variable property element
	{ cerr << "skipped variable property element: <" << information << ">" << endl;
	  break; }

    case(126): // WSDL message not found
	{ cerr << "<message> `" << information << "' referenced in <operation> not defined before" << endl;
	  break; }

    case(127): // WSDL portType not found
	{ cerr << "<portType> `" << information << "' referenced in <role> not defined before" << endl;
	  break; }

    case(128): // WSDL operation not found
	{ cerr << information << endl;
	  break; }

    case(129): // WSDL partnerLinkType was found, but role was not found
	{ cerr << information << endl;
	  break; }	  

    case(130): // WSDL partnerLinkType was not found
	{ cerr << information << endl;
	  break; }	  

    case(131): // BPEL4Chor activity name is not part of a messageLink
	{ cerr << information << endl;
	  break; }

    case(132): // activity has neither a name nor a BPEL4Chor id
	{ cerr << "activity <" + information + "> has neither a `name' nor an `id' attribute to reference a BPEL4Chor <messageLink>" << endl;
	  break; }

    case(133): // <extensionActivity> replaced by <opaqueActivity>
	{ cerr << "replacing <extensionActivity> with <opaqueActivity>" << endl;
	  break; }

    case(134): // <participantType> already definied
	{ cerr << "BPEL4Chor <participantType> `" << information << "' already defined" << endl;
	  break; }

    case(135): // BPEL4Chor <participantType> not found for a <participant>
	{ cerr << information << endl;
	  break; }

    case(136): // BPEL4Chor forEach id not found
	{ cerr << "<forEach> id or name `" << information << "' does not reference a BPEL4Chor <participant> or <participantSet>" << endl;
	  break; }

    case(137): // BPEL4Chor XMLNS not found (either in BPEL4Chor file or in process)
	{ cerr << information << endl;
	  break; }

    case(138): // no "partnerLink" or "operation" attribute found -- using BPEL4Chor attributes instead
        { cerr << "using identifier `" << information << "' as a channel name; use BPEL4Chor topology for exact channel names" << endl;
	  break; }
  }

  cerr << endl;

  if (level == ERRORLEVEL_CRITICAL)
  {
    cleanup();
    exit(1);
  }
}





/*!
 * \brief prints static analysis error messages
 *
 * Outputs error messages triggered by several static tests. The static
 * anaylsis codes are taken from the WS-BPEL 2.0 specification. For each error
 * code, a standard problem description is printed.
 *
 * \param code		code of the static analysis error
 * \param information	additional information about the error
 * \param lineNumber	a line number to locate the error
 *
 * \ingroup debug
 */
void SAerror(unsigned int code, string information, int lineNumber)
{
  globals::static_analysis_errors++;

  if (!globals::abstract_process)
    cerr << colorconsole::fg_red;
  else
    cerr << colorconsole::fg_magenta;

  cerr << globals::filename;
  if (lineNumber != 0)
   cerr << ":" << lineNumber;
  cerr << " - ";

  cerr << "[SA";
  cerr << setfill('0') << setw(5) << code;
  cerr << "]\n";

  cerr << colorconsole::fg_standard;

  // [SA00065] is critical
  if (code == 65)
  {
    cleanup();
    exit(1);
  }

  if (debug_level == TRACE_ERROR)
    return;

  switch (code)
  {
    case(2):
      { cerr << "operation `" << information << "' is overloaded in the WSDL file" << endl;
	break; }
 
    case(3):
      { cerr << "<catch> must not be applied to the standard fault `" << information << "' when parent scope has attribute `exitOnStandardFault' set to `yes'" << endl;
	break; }
	
    case(5):
      { cerr << information << " does not match the portType specified by the combination of partnerLink and role in the WSDL description" << endl;
	break; }

    case(6):
      { cerr << "<rethrow> activity must only be used within <catch> or <catchAll>" << endl;
	break; }

    case(7):
      { cerr << "<compensateScope> activity must only be used within an FTC-handler" << endl;
	break; }

    case(8):
      { cerr << "<compensate> activity must only be used within an FTC-handler" << endl;
	break; }
	
    case(15):
      { cerr << "<process> does not contain a start activity" << endl;
	break; }

    case(16):
      { cerr << "<partnerLink> `" << information << "' neither specifies `myRole' nor `partnerRole'" << endl;
	break; }

    case(17):
      { cerr << "<partnerLink> `" << information << "' uses `initializePartnerRole' but does not have a partner role" << endl;
	break; }

    case(18):
      { cerr << "<partnerLink> `" << information << "' already defined in this scope/process" << endl;
	break; }

    case(23):
      { cerr << "<variable> `" << information << "' already defined in this scope/process" << endl;
	break; }

    case(24):
      { cerr << "attribute `" << information << "' must be of type `BPELVariableName'" << endl;
	break; }

    case(25):
      { cerr << "<variable> `" << information << "' must either use `messageType', `type' or `element' attribute" << endl;
	break; }
	
    case(32):
      { cerr << "<" << information << "> has an invalid combination of attributes and/or elements" << endl;
	break; }

    case(35):
      { cerr << "attribute `endpointReference' in <from> element is only allowed to be `myRole' if a `myRole' attribute is definied for the used <partnerLink> `" << information << "'" << endl;
	break; }

    case(36):
      { cerr << "attribute `endpointReference' in <from> element is only allowed to be `partnerRole' if a `partnerRole' attribute is definied for the used <partnerLink> `"<< information << "'"<< endl;
	break; }

    case(37):
      { cerr << "<partnerLink> `" << information << "' referenced in a <to> element must define the attribute `partnerRole'" << endl;
	break; }

    case(44):
      { cerr << "<correlationSet> `" << information << "' already defined in this scope/process" << endl;
	break; }

    case(48):
      { cerr << "<" << information << "> is using a variable of a wrong messageType" << endl;
	break; }

    case(51):
      { cerr << "<invoke> must not be used with an inputVariable AND <toPart> elements" << endl;
	break; }

    case(52):
      { cerr << "<invoke> must not be used with an outputVariable AND <fromPart> elements" << endl;
	break; }

    case(55):
      { cerr << "<receive> must not be used with a variable AND <fromPart> elements" << endl;
	break; }

    case(56):
      { cerr << information << " start activity must not be preceeded by another activity" << endl;
	break; }

    case(57):
      { cerr << "all start activities must share a correlation set with `initiate' set to \"join\"" << endl;
	break; }

    case(58):
      { cerr << "<" << information << "> is using a variable of a wrong messageType" << endl;
	break; }

    case(59):
      { cerr << "<reply> must not be used with a variable AND <toPart> elements" << endl;
	break; }

    case(62):
      { cerr << "<pick> start activity must only contain <onMessage> branches" << endl;
	break; }

    case(63):
      { cerr << "<onMessage> must not be used with a variable AND <fromPart> elements" << endl;
	break; }

    case(64):
      { cerr << "<link> `" << information << "' already defined in this <flow>" << endl;
	break; }
	
    case(65):
      { cerr << "<link> `" << information << "' was not defined before" << endl;
	break; }

    case(66):
      { cerr << "<link> `" << information << "' must be used exactly once as source and target" << endl;
	break; }

    case(67):
      { cerr << "<link> " << information << " connect the same activity" << endl;
	break; }

    case(68):
      { cerr << information << " more than once as source" << endl;
	break; }

    case(69):
      { cerr << information << " more than once as target" << endl;
	break; }

    case(70):
      { cerr << "<link> `" << information << "' crosses boundary of a repeatable construct or of a compensation handler" << endl;
	break; }

    case(71):
      { cerr << "<link> `" << information << "' crosses boundary of a fault handler or a termination handler, but is inbound" << endl;
	break; }

    case(72):
      { cerr << "<link> `" << information << "' closes a control cycle" << endl;
	break; }

    case(73):
      { cerr << "<link> `" << information << "' used in a join condition is not an incoming link of this activity" << endl;
	break; }

    case(74):
      { cerr << information << endl;
	break; }

    case(75):
      { cerr << "<forEach>'s constant <completionCondition> can never be fulfilled" << endl;
	break; }

    case(76):
      { cerr << "<variable> `" << information << "' hides enclosing <forEach>'s `counterName'" << endl;
	break; }

    case(77):
      { cerr << "<scope> `" << information << "' is not immediately enclosed to current scope" << endl;
	break; }

    case(78):
      { cerr << "`" << information << "' does not refer to a <scope> or an <invoke> activity" << endl;
	break; }

    case(79):
      { cerr << "root <scope> inside <" << information << "> must not have a <compensationHandler>" << endl;
	break; }

    case(80):
       { cerr << "<faultHandlers> have no <catch> or <catchAll> element" << endl;
	 break; }

    case(81):
       { cerr << "<catch> attribute " << information << endl;
	 break; }

    case(82):
       { cerr << "peer-scope dependency relation must not include cycles" << endl;
	 break; }

    case(83):
       { cerr << "<eventHandlers> have neither <onEvent> nor <onAlarm> element" << endl;
	 break; }

    case(84):
       { cerr << "partnerLink reference `" << information << "' of <onEvent> cannot be resolved" << endl;
	 break; }

    case(87):
      { cerr << "<" << information << "> is using a variable of a wrong messageType" << endl;
	break; }

    case(88):
       { cerr << "<correlation> reference `" << information << "' of <onEvent> cannot be resolved" << endl;
	 break; }

    case(91):
       { cerr << "an isolated <scope> may not contain other isolated scopes" << endl;
	 break; }

    case(92):
       { cerr << "<scope> with name `" << information << "' defined twice" << endl;
	 break; }

    case(93):
       { cerr << "<catch> construct defined twice" << endl;
	 break; }

    default:
	 cerr << endl;
  }

  cerr << endl;
}





/*!
 * \brief prints static analysis error messages
 * \overload
 * \ingroup debug
 */
void SAerror(unsigned int code, string information, string lineNumber)
{
  if (lineNumber != "")
    SAerror(code, information, toInt(lineNumber));
  else
    SAerror(code, information, 0);
}





/******************************************************************************
 * Error handling functions
 *****************************************************************************/

/*!
 * Some output in case an error has occured.
 *
 * \post all goals from #cleanup
 * \post programm terminated
 *
 * \todo move this to debug.cc and debug.h
 *
 * \ingroup debug
 */
void error()
{
  trace("\nAn error has occured while parsing \"" + globals::filename + "\"!\n\n");
  trace(TRACE_WARNINGS, "-> Any output file might be in an undefinded state.\n");

  cleanup();

  if (log_filename != "")
  {
    trace("\nProgramme aborted due to error.\n\n");
    trace("Please check logfile for detailed information!\n\n");
  }

  exit(4);
}





/*!
 * Cleans up. Afterwards we should have an almost defined state.
 *
 * \post input file closed
 * \post current output file closed
 * \post log file closed
 *
 * \todo move this to debug.cc and debug.h
 *
 * \ingroup debug
 */
void cleanup()
{
  trace(TRACE_INFORMATION,"Cleaning up ...\n");

  if ( globals::filename != "<STDIN>" && frontend_in != NULL)
  {
    trace(TRACE_INFORMATION," + Closing input file: " + globals::filename + "\n");
    fclose(frontend_in);
  }
 
  if ( output != &cout && output != &clog && output != log_output && output != NULL )
  {
    trace(TRACE_INFORMATION," + Closing output file: " + globals::output_filename + ".X\n");
    (*output) << flush;
    (static_cast<ofstream*>(output))->close();
    delete(output);
    output = NULL;
  }
 
  if ( log_output != &clog )
  {
    trace(TRACE_INFORMATION," + Closing log file: " + log_filename + "\n");
    (*log_output) << flush;
    (static_cast<ofstream*>(log_output))->close();
    delete(log_output);
    log_output = &cerr;
  }
}
