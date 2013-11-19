/*!
\file
\brief implementation of classes IO, Input, and Output
\author Niels
\status new
\ingroup g_io
*/

#include <InputOutput/InputOutput.h>
#include <InputOutput/Reporter.h>

// static variable
Reporter *IO::r = NULL;

/*!
The opening and closing of files is reported using the reporter IO::r. This
function sets this static variable initially for all objects.

\param[in] reporter  the reporter to use for objects of IO, Input, our Output

\pre given reporter is initialized
\post static variable IO::r is set to the given reporter
*/
void IO::setReporter(Reporter *reporter)
{
    assert(reporter);
    r = reporter;
}


/*!
Operator to use IO objects with the same syntax as FILE pointers.

\return The FILE pointer wrapped by the IO object.
*/
IO::operator FILE *() const
{
    return fp;
}


/*!
Getter for the filename.

\return The filename as null-terminated string.
*/
const char *IO::getFilename() const
{
    return filename.c_str();
}



/*!
\pre the reporter was set using IO::setReporter
\post closes file and errors with ERROR_FILE if this fails
*/
IO::~IO()
{
    assert(r);
    r->status("closed %s file %s", r->markup(MARKUP_OUTPUT, kind.c_str()).str(),
              r->markup(MARKUP_FILE, filename.c_str()).str());

    // try to close file
    const int ret = fclose(fp);

    // fclose returns 0 on success
    if (UNLIKELY(ret != 0))
    // LCOV_EXCL_START
    {
        r->status("could not close %s file %s", r->markup(MARKUP_OUTPUT, kind.c_str()).str(),
                  r->markup(MARKUP_FILE, filename.c_str()).str());
        r->abort(ERROR_FILE);
    }
    // LCOV_EXCL_STOP
}


/*!
\param fp  the FILE* to wrap
\param kind  the kind of the file (used in verbose output)
\param filename  the name of the file (used only in verbose output here)

\pre the reporter was set using IO::setReporter
\pre File fp was opened using fopen by the constructor of the child classes
Input or Output
\post If an error occurred opening the file, error with ERROR_FILE
*/
IO::IO(FILE *fp, std::string kind, std::string filename) :
    filename(filename), kind(kind), fp(fp)
{
    assert(r);

    // fopen returns NULL on error
    // LCOV_EXCL_START
    if (UNLIKELY(fp == NULL))
    {
        r->status("could not open %s file %s", r->markup(MARKUP_OUTPUT, kind.c_str()).str(),
                  r->markup(MARKUP_FILE, filename.c_str()).str());
        r->abort(ERROR_FILE);
    }
    // LCOV_EXCL_STOP
}


/*!
\param _kind  the kind of the file (used in verbose output)
\param _filename  the name of the output file to open; if no filename or "-" is
given, the file written to the standard output

\pre the reporter was set using IO::setReporter
\post the file given with the file name is opened to write
\note errors are handled by the IO constructor
*/
Output::Output(std::string _kind, std::string _filename) :
    IO(_filename == "-" ? stdout : fopen(_filename.c_str(), "w"),
       _kind,
       _filename == "-" ? "stdout" : _filename)
{
    assert(r);
    r->status("writing %s to %s", r->markup(MARKUP_OUTPUT, kind.c_str()).str(),
              r->markup(MARKUP_FILE, filename.c_str()).str());
}


/*!
\param _kind  the kind of the file (used in verbose output)
\param _filename  the name of the input file to open; if no filename or "-" is
given, the file read from the standard input

\pre the reporter was set using IO::setReporter
\post the file given with the file name is opened to read
\note errors are handled by the IO constructor
*/
Input::Input(std::string _kind, std::string _filename) :
    IO(_filename == "-" ? stdin : fopen(_filename.c_str(), "r"),
       _kind,
       _filename == "-" ? "stdin" : _filename)
{
    assert(r);
    r->status("reading %s from %s", r->markup(MARKUP_OUTPUT, kind.c_str()).str(),
              r->markup(MARKUP_FILE, filename.c_str()).str());
}
