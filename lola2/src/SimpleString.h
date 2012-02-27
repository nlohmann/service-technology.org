#pragma once

/// string class to avoid STL's std::string
class String
{
    private:
        /// payload - is freed in destructor
        char* s;

    public:
        /// constructor (does only copy pointer, not content)
        String(char* s);

        /// constructor (actually copying content)
        String(char* s, bool);

        /// destructor - frees payload
        ~String();

        /// getter for s
        char* str() const;
};
