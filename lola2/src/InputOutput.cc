#include "InputOutput.h"

IO::operator FILE* ()
{
    return fp;
}

IO::IO(FILE *fp) : fp(fp)
{}

IO::~IO()
{
    fclose(fp);
}

/*
IO::IO(FILE *fp, String kind, String filename) :
    fp(fp), kind(kind), filename(filename)
{
}


Output::Output(String kind) :
    IO(stdout, kind, String(""))
{
}

Output::Output(String kind, String filename) :
    IO(fopen(filename.str(), "w"), kind, filename)
{
}
*/
