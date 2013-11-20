/*!
\file
\brief declaration of class String
\author Niels
\status new
\ingroup g_reporting
*/

#pragma once


/*!
\brief string class to avoid STL's std::string

This class wraps a standard NULL-terminated string to have better control over
its memory deallocation.

\note An operator like `const char *String::str() const` is not provided, as
it would not be usable inside printf-calls.

\ingroup g_reporting
*/
class String
{
private:
    /// payload - is freed in destructor
    char *s;

public:
    /// constructor (does only copy pointer, not content)
    explicit String(char *s);

    /// destructor - frees payload
    ~String();

    /// getter for s
    const char *str() const;
};
