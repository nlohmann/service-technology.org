/*!
\file
\brief declaration of compressed file input/output methods
\author Karsten
\status approved 21.02.2012
\ingroup g_io

Input and outout from/to a file in compressed format. We generate two
separate files that can be read in arbitrary order. In this version, we use an
ASCII file where data are separated by spaces and newlines.

\todo These functions should be moved into a struct -- maybe even Net.
*/

#pragma once

#include <cstdio>
#include <Frontend/Parser/ParserPTNet.h>

/// write names to file
void WriteNameFile(FILE *);

/// write compressed net structure to file
void WriteNetFile(FILE *);

/// read names from file and annotated parsed net
void ReadNameFile(FILE *, ParserPTNet *);

/// read compressed net structure from file
void ReadNetFile(FILE *);
