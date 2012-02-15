#include <string>

//! \brief turns an integer into a string
//! \param i standard C int
//! \return  C++ string representing i
std::string intToString(int i) {
    char buffer[20];
    sprintf(buffer, "%d", i);

    return std::string(buffer);
}

typedef enum { } trace_level_fiona;

void debug_trace(trace_level_fiona pTraceLevel, std::string message) { }
