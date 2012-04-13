/*!
\file CompressedIO.h
\author Karsten
\status approved 21.02.2012

\brief Input and outout from/to a file in compressed format. We generate two
separate files that can be read in arbitrary order. In this version, we use an
ASCII file where data are separated by spaces and newlines.

\todo Das hier eine Klasse zuordnen - irgendwo müssen wir uns um Dateien kümmern.
*/

#pragma once
#include <cstdio>


void WriteNameFile(FILE*);
void WriteNetFile(FILE*);
void ReadNameFile(FILE*);
void ReadNetFile(FILE*);
