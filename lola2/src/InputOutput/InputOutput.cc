/*!
\file
\brief implementation of classes IO, Input, and Output
\author Niels
\status new
\ingroup g_io
*/

#include <cstdlib>
#include <cstring>
#include <sstream>
#include <string>
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
    if (fp != stdout)
    {
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


/*!
This function highlights a certain part of the output file, given via positions.
It rewinds the output file and skips all lines until line first_line-1. This
line is used as context and is printed without highlighting. Then, the first
"error" line is printed. While doing so, the column information are used to
highlight the wished part. Finally, this part is further highlighted using
carets.

This is how highlighting would look for 40:24 - 40:28:
\verbatim

 39      CONSUME critical.1: 1;
 40      PRODUCE idle.1: 1, sems: 1;
                            ^~~~     
\endverbatim

\param first_line    the first line where the output should be printed
\param first_column  the first column where the output should be highlighted
\param last_line     the last line where the output should be printed
\param last_column   the last column where the output should be highlighted

\pre We assume that the input file is not stdin. If it is, thus function
silently returns.
\post The output file position is at last_line + 1; that is, right after the
line given at last_line.
\note This function is const in the sense that it does not change the pointer
to the input file. It does, however, change the current position it it.

\todo Currently, the parameter last_line is ignored. As soon as we have an idea
how multi-line highlighting works, we shall implement this as well.
\todo Coloring is really basic. The line numbers should be dimmed.
\todo In the output, tabs are replaced by a single space. To resemble the input
file, we should use 8 spaces instead. This, however, also has an impact on the
position of the highlighted area.
*/
void Input::printExcerpt(int first_line, int first_column,
                         int last_line, int last_column) const
{
    // stdin cannot be rewound
    if (fp == stdin)
    {
        return;
    }
    
    char* line = NULL;
    size_t len = 0;

    rewind(fp);

    // skip lines until error
    for (int i = 0; i < first_line - 1; ++i)
    {
        getline(&line, &len, fp);
    }

    fprintf(stderr, "\n");
    
    // print last line before the error (context)
    fprintf(stderr, "%3d  %s", first_line - 1, line);

    // get error line
    getline(&line, &len, fp);

    // print line number
    fprintf(stderr, "%3d  ", first_line);

    // print error line
    for (size_t i = 0; i < strlen(line); ++i)
    {
        // replace tabs by spaces
        if (line[i] == '\t')
        {
            fprintf(stderr, " ");
            continue;
        }

        if (i >= first_column - 1 && i <= last_column - 1)
        {
            std::stringstream ss;
            ss << line[i];            
            fprintf(stderr, "%s", rep->markup(MARKUP_BAD, ss.str().c_str()).str());
        }
        else
        {
            fprintf(stderr, "%c", line[i]);
        }
    }

    fprintf(stderr, "     ");
    // print arrow indicating the error
    for (size_t i = 0; i < strlen(line); ++i)
    {
        // the beginning
        if (i == first_column - 1)
        {
            fprintf(stderr, "%s", rep->markup(MARKUP_GOOD, "^").str());
        }
        else
        {
            // following error characters
            if (i > first_column - 1 && i <= last_column - 1)
            {
                fprintf(stderr, "%s", rep->markup(MARKUP_GOOD, "~").str());
            }
            else
            {
                fprintf(stderr, " ");
            }
        }
    }
    
    fprintf(stderr, "\n\n");
}
