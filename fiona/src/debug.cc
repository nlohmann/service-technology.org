/*****************************************************************************
 * Copyright 2005, 2006 Niels Lohmann, Peter Massuthe, Daniela Weinberg      *
 *                                                                           *
 * This file is part of Fiona.                                               *
 *                                                                           *
 * Fiona is free software; you can redistribute it and/or modify it          *
 * under the terms of the GNU General Public License as published by the     *
 * Free Software Foundation; either version 2 of the License, or (at your    *
 * option) any later version.                                                *
 *                                                                           *
 * Fiona is distributed in the hope that it will be useful, but WITHOUT      *
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or     *
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for  *
 * more details.                                                             *
 *                                                                           *
 * You should have received a copy of the GNU General Public License along   *
 * with Fiona; if not, write to the Free Software Foundation, Inc., 51       *
 * Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.                      *
 *****************************************************************************/

/*!
 * \file	debug.cc
 *
 * \brief	Some debugging tools for Fiona
 * 
 * \author  responsible: Daniela Weinberg <weinberg@informatik.hu-berlin.de>
 *
 * \note    This file is part of the tool Fiona and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel
 *          for details.
 */

#include "mynew.h"
#include "options.h"
#include "debug.h"

#include <string>
#include <cctype>
#include <iostream>
#include <algorithm>

std::string ogfileToParse;
std::string owfnfileToParse;

/// debug level
trace_level_fiona debug_level = TRACE_0;


/**
 * Provides output to stderr using different #trace_level_fiona
 * (in order to regulate amount of output)
 *
 * \param pTraceLevel	the used trace level
 * \param message	the output
 *
 */
void trace(trace_level_fiona pTraceLevel, std::string message) {
    if (pTraceLevel <= debug_level) {
        (*log_output) << message << std::flush;
    }
}


/**
 * Works like #trace(trace_level_fiona, std::string) with trace_level = TRACE_ALWAYS
 *
 * \param message the output
 *
 */
void trace(std::string message) {
    trace(TRACE_0, message);
}


/*!
 * \param i standard C int
 * \return  C++ string representing i
 */
std::string intToString(int i) {
    char buffer[20];
    sprintf(buffer, "%d", i);

    return std::string(buffer);
}


bool isNonNegativeInteger(const std::string& s) {
    for (std::string::size_type i = 0; i != s.size(); ++i) {
        if (!std::isdigit(s[i])) {
            return false;
        }
    }
    return true;
}


std::string toUpper(const std::string& s) {
    std::string result;

    // The next line does not work for some reason. So we transform by hand.
    // std::transform(s.begin(), s.end(), result.begin(), (int(*)(int))toupper);

    for (std::string::size_type i = 0; i != s.size(); ++i) {
        result += toupper(s[i]);
    }

    return result;
}


std::string toLower(const std::string& s) {
    std::string result;

    // The next line does not work for some reason. So we transform by hand.
    // std::transform(s.begin(), s.end(), result.begin(), (int(*)(int))tolower);

    for (std::string::size_type i = 0; i != s.size(); ++i) {
        result += tolower(s[i]);
    }

    return result;
}


/*!
 * This function is invoked by the parser and the lexer during the syntax
 * analysis. When an error occurs, it prints an accordant message and shows the
 * lines of the input files where the error occured.
 *
 * \param msg a message (mostly "Parse error") and some more information e.g.
 *            the location of the syntax error.
 * \param yylineno
 * \param yytext
 * \param file
 * 
 * \return 1, since an error occured
 */
int yyerror(const char* msg, int yylineno, const char* yytext, const char* file) {
    trace("Error while parsing!\n\n");
    trace(msg);
    trace("\n");

    // display passed error message
    trace("Error in '" + std::string(file) + "' in line ");
    trace(intToString(yylineno));
    trace(":\n");
    trace("  token/text last read was '");
    trace(yytext);
    trace("'\n\n");

    // if (filename != "<STDIN>")
    {
        trace("-------------------------------------------------------------------------------\n");

        // number of lines to print before and after errorneous line
        int environment = 4;

        unsigned int firstShowedLine =
            ((yylineno-environment)>0) ? (yylineno-environment) : 1;

        std::ifstream inputFile(file);
        std::string errorLine;
        for (unsigned int i=0; i<firstShowedLine; i++) {
            getline(inputFile, errorLine);
        }

        // print the erroneous line (plus/minus three more)
        for (unsigned int i=firstShowedLine; i<=firstShowedLine+(2*environment); i++) {
            trace(intToString(i) + ": " + errorLine + "\n");
            getline(inputFile, errorLine);
            if (inputFile.eof()) {
                break;
            }
        }
        inputFile.close();

        trace("-------------------------------------------------------------------------------\n");
    }

    exit(1);
    return 1;
}


int owfn_yyerror(const char* msg) {
    /* defined by flex */
    extern int owfn_yylineno; ///< line number of current token
    extern char *owfn_yytext; ///< text of the current token

    return yyerror(msg, owfn_yylineno, owfn_yytext, owfnfileToParse.c_str());
}


int og_yyerror(const char* msg) {
    /* defined by flex */
    extern int og_yylineno; ///< line number of current token
    extern char *og_yytext; ///< text of the current token

    return yyerror(msg, og_yylineno, og_yytext, ogfileToParse.c_str());
}
