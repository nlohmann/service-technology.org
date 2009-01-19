/*****************************************************************************\
 Copyright (c) 2008  Robert Danitz, Christian Gierds, Niels Lohmann,
                     Peter Massuthe, Richard Mu"ller, Janine Ott,
                     Jan Su"rmeli, Daniela Weinberg, Karsten Wolf,
                     Martin Znamirowski

 Copyright (c) 2007  Jan Bretschneider, Robert Danitz, Christian Gierds,
                     Kathrin Kaschner, Leonard Kern, Niels Lohmann,
                     Peter Massuthe, Daniela Weinberg, Karsten Wolf

 Copyright (c) 2006  Kathrin Kaschner, Peter Massuthe, Daniela Weinberg,
                     Karsten Wolf

 This file is part of Fiona, see <http://service-technology.org/fiona>.

 Fiona is free software; you can redistribute it and/or modify it under the
 terms of the GNU General Public License as published by the Free Software
 Foundation; either version 3 of the License, or (at your option) any later
 version.
 
 Fiona is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License along with
 Fiona (see file COPYING). If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

/*!
 * \file	debug.cc
 *
 * \brief	Some debugging tools for Fiona
 * 
 * \author  responsible: Daniela Weinberg <weinberg@informatik.hu-berlin.de>
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
std::string covfileToParse;

/// debug level
trace_level_fiona debug_level = TRACE_0;


#ifndef NDEBUG
//! \brief Provides output to stderr using different #trace_level_fiona
//!        (in order to regulate amount of output)
//! \param pTraceLevel	the used trace level
//! \param message	the output
void debug_trace(trace_level_fiona pTraceLevel, std::string message) {
#warning "running debug_trace;"
    if (pTraceLevel <= debug_level) {
        (*log_output) << message << std::flush;
    }
}
#endif

//! \brief Works like #trace(trace_level_fiona, std::string) with trace_level = TRACE_ALWAYS
//! \param message the output
void trace(std::string message) {
    (*log_output) << message << std::flush;
}


//! \brief turns an integer into a string
//! \param i standard C int
//! \return  C++ string representing i
std::string intToString(int i) {
    char buffer[20];
    sprintf(buffer, "%d", i);

    return std::string(buffer);
}

//! \brief turns a double value into a string
//! \param i standard C double
//! \return  C++ string representing i
std::string doubleToString(double i) {
    char buffer[20];
    sprintf(buffer, "%.2f", i);

    return std::string(buffer);
}

//! \brief tests wherther the given string shows a non negative integer
//! \param s string to test
//! \return true if so, false else
bool isNonNegativeInteger(const std::string& s) {
    for (std::string::size_type i = 0; i != s.size(); ++i) {
        if (!std::isdigit(s[i])) {
            return false;
        }
    }
    return true;
}


//! \brief turns a string into upper case
//! \param s string change
//! \return upper case version of s
std::string toUpper(const std::string& s) {
    std::string result;

    // The next line does not work for some reason. So we transform by hand.
    // std::transform(s.begin(), s.end(), result.begin(), (int(*)(int))toupper);

    for (std::string::size_type i = 0; i != s.size(); ++i) {
        result += toupper(s[i]);
    }

    return result;
}


//! \brief turns a string into lower case
//! \param s string change
//! \return lower case version of s
std::string toLower(const std::string& s) {
    std::string result;

    // The next line does not work for some reason. So we transform by hand.
    // std::transform(s.begin(), s.end(), result.begin(), (int(*)(int))tolower);

    for (std::string::size_type i = 0; i != s.size(); ++i) {
        result += tolower(s[i]);
    }

    return result;
}


//! \brief This function is invoked by the parser and the lexer during the syntax
//!        analysis. When an error occurs, it prints an accordant message and shows the
//!        lines of the input files where the error occured.
//! \param msg a message (mostly "Parse error") and some more information e.g.
//!            the location of the syntax error.
//! \param yylineno
//! \param yytext
//! \param file
//! \return 1, since an error occured
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


//! \brief function needed for the specific parser, calls yyerror
//! \param msg error message to print
//! \return returns 1
int owfn_yyerror(const char* msg) {
    /* defined by flex */
    extern int owfn_yylineno; ///< line number of current token
    extern char *owfn_yytext; ///< text of the current token

    return yyerror(msg, owfn_yylineno, owfn_yytext, owfnfileToParse.c_str());
}


//! \brief function needed for the specific parser, calls yyerror
//! \param msg error message to print
//! \return returns 1
int og_yyerror(const char* msg) {
    /* defined by flex */
    extern int og_yylineno; ///< line number of current token
    extern char *og_yytext; ///< text of the current token

    return yyerror(msg, og_yylineno, og_yytext, ogfileToParse.c_str());
}

//! \brief function needed for if an error occurs while parsing a .covog-file;
//!        the msg is passed to yyerror 
//! \param msg error message to print
//! \return returns 1
int covog_yyerror(const char* msg) {
    /* defined by flex */
    extern int covog_yylineno; ///< line number of current token
    extern char *covog_yytext; ///< text of the current token

    return yyerror(msg, covog_yylineno, covog_yytext, ogfileToParse.c_str());
}

//! \brief function needed for if an error occurs while parsing a .cov-file;
//!        the msg is passed to yyerror 
//! \param msg error message to print
//! \return returns 1
int cov_yyerror(const char* msg) {
    /* defined by flex */
    extern int cov_yylineno; ///< line number of current token
    extern char *cov_yytext; ///< text of the current token

    return yyerror(msg, cov_yylineno, cov_yytext, covfileToParse.c_str());
}


