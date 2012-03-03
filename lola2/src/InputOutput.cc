#include "InputOutput.h"

Reporter* IO::r = NULL;

void IO::setReporter(Reporter* reporter)
{
    assert(reporter);
    r = reporter;
}

IO::operator FILE* ()
{
    return fp;
}

const char* IO::getFilename()
{
    return filename.c_str();
}



IO::~IO()
{
    r->status("closed %s file %s", kind.c_str(), r->markup(MARKUP_FILE, filename.c_str()).str());
    fclose(fp);
}

IO::IO(FILE* fp, std::string kind, std::string filename) :
    fp(fp), kind(kind), filename(filename)
{
    if (UNLIKELY(!fp))
    {
        r->status("could not open %s file %s", kind.c_str(), r->markup(MARKUP_FILE, filename.c_str()).str());
        r->abort(ERROR_FILE);
    }
}


Output::Output(std::string kind) :
    IO(stdout, kind, "")
{
    r->status("writing %s to %s", kind.c_str(), r->markup(MARKUP_FILE, "stdout").str());
}

Output::Output(std::string kind, std::string filename) :
    IO(fopen(filename.c_str(), "w"), kind, filename)
{
    r->status("writing %s to %s", kind.c_str(), r->markup(MARKUP_FILE, filename.c_str()).str());
}


Input::Input(std::string kind) :
    IO(stdin, kind, "")
{
    r->status("reading %s from %s", kind.c_str(), r->markup(MARKUP_FILE, "stdin").str());
}

Input::Input(std::string kind, std::string filename) :
    IO(fopen(filename.c_str(), "r"), kind, filename)
{
    r->status("reading %s from %s", kind.c_str(), r->markup(MARKUP_FILE, filename.c_str()).str());
}
