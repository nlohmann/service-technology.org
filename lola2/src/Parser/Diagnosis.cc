#include <cstdlib>
#include <cstring>
#include <sstream>
#include <string>
#include <Parser/Diagnosis.h>
#include <InputOutput/InputOutput.h>
#include <InputOutput/Reporter.h>

extern Input *netFile;

void printExcerpt(int first_line, int first_column, int last_line, int last_column)
{
    char* line = NULL;
    size_t len = 0;

    rewind(*netFile);

    // skip lines until error
    for (int i = 0; i < first_line - 1; ++i) {
        getline(&line, &len, *netFile);
    }

    fprintf(stderr, "\n");
    
    // print last line before the error (context)
    fprintf(stderr, "%3d  %s", first_line - 1, line);

    // get error line
    getline(&line, &len, *netFile);

    // print line number
    fprintf(stderr, "%3d  ", first_line);

    // print error line
    for (size_t i = 0; i < strlen(line); ++i) {
        // replace tabs by spaces
        if (line[i] == '\t') {
            fprintf(stderr, " ");
            continue;
        }

        if (i >= first_column - 1 && i <= last_column - 1) {
            std::stringstream ss;
            ss << line[i];            
            fprintf(stderr, "%s", rep->markup(MARKUP_BAD, ss.str().c_str()).str());
        } else {
            fprintf(stderr, "%c", line[i]);
        }
    }

    fprintf(stderr, "     ");
    // print arrow indicating the error
    for (size_t i = 0; i < strlen(line); ++i) {
        // the beginning
        if (i == first_column - 1) {
            fprintf(stderr, "%s", rep->markup(MARKUP_GOOD, "^").str());
        } else {
            // following error characters
            if (i > first_column - 1 && i <= last_column - 1) {
                fprintf(stderr, "%s", rep->markup(MARKUP_GOOD, "~").str());
            } else {
                fprintf(stderr, " ");
            }
        }
    }
    
    fprintf(stderr, "\n\n");
}
