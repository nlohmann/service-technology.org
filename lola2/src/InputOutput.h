#pragma once

#include <stdio.h>
#include "String.h"

class IO
{
    protected:
        /// the filepointer
        FILE *fp;

    public:
        operator FILE* ();

        IO(FILE*);

        /// destructor
        ~IO();
};

/*
class Output : public IO
{
    private:
        /// it's a temp file if its not NULL
        char *temp;

    public:
        /// output to stdout (given kind)
        Output(String);

        /// output to file (given kind and filename)
        Output(String, String);
};

class Input : public IO
{
    public:
        /// input from stdin (given kind)
        Input(String);

        /// input from file (given kind and filename)
        Input(String, String);

        /// input from open file (given filepointer, kind, and filename)
        Input(FILE*, String, String);
};
*/
