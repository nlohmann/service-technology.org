/*!
\file
\brief declaration of classes IO, Input, and Output
\author Niels
\status new
\ingroup g_io
*/

#pragma once
#include <string>
#include <cstdio>


// forward declaration
class Reporter;

/*!
\brief wrapper class for file input and output

This class wraps convenience methods around a standard C-style FILE*. Its main
purpose is to automatically close files in the moment its IO object leaves
the scope.

\ingroup g_io
*/
class IO
{
protected:
    /// a reporter for status messages
    static Reporter *r;

    /// the filename (or empty in case of stdin/stdout)
    const std::string filename;

    /// the kind of the file
    const std::string kind;

    /// the filepointer
    FILE *fp;

    /// default constructor
    IO(FILE *, std::string, std::string);

    /// destructor
    ~IO();

public:
    /// a setter for the reporter to use
    static void setReporter(Reporter *);

    /// implicit cast to FILE* (return fp)
    operator FILE *() const;

    /// return filename
    const char *getFilename() const;
};


/*!
\brief wrapper class for output files
\ingroup g_io
*/
class Output : public IO
{
public:
    /// output to stdout (given kind)
    Output(std::string);

    /// output to file (given kind and filename)
    Output(std::string, std::string);
};


/*!
\brief wrapper class for input files
\ingroup g_io
*/
class Input : public IO
{
public:
    /// input from stdin (given kind)
    Input(std::string);

    /// input from file (given filename)
    Input(std::string, std::string);
};
