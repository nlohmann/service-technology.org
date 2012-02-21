/*!
\file CompressedIO.h
\author Karsten
\status new

Input and outout from/to a file in compressed format. We generate two separate
files that can be read in arbitrary order. In this version, we use an ASCII
file where data are separated by spaces and newlines
*/

#pragma once

#include <cstdio>

void WriteNameFile(FILE * );
void WriteNetFile(FILE * );
void ReadNameFile(FILE * );
void ReadNetFile(FILE * );
