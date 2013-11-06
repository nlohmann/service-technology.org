/*!
\file InputOutput.cc
\author Niels
\status new
*/

#pragma once
#include <string>
#include <cstdio>


class Reporter;

class IO
{
protected:
    /// a reporter for status messages
    static Reporter *r;

    /// the filename (or empty in case of stdin/stdout)
    std::string filename;

    /// the kind of the file
    std::string kind;

    /// the filepointer
    FILE *fp;

public:
    static void setReporter(Reporter *);

    /// implicit cast to FILE* (return fp)
    operator FILE *();

    /// return filename
    const char *getFilename();

    /// default constructor
    IO(FILE *, std::string, std::string);

    /// destructor
    ~IO();
};

class Output : public IO
{
public:
    /// output to stdout (given kind)
    Output(std::string);

    /// output to file (given kind and filename)
    Output(std::string, std::string);
};

class Input : public IO
{
public:
    /// input from stdin (given kind)
    Input(std::string);

    /// input from file (given filename)
    Input(std::string, std::string);
};
