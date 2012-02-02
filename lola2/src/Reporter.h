/*!
\file Reporter.h
\status approved 25.01.2012
*/

#pragma once

#include "Socket.h"

/// error codes for the abort() function
typedef enum
{
    ERROR_SYNTAX,
    ERROR_COMMANDLINE
} errorcode_t;


/*!
\todo Datei mit Fehlernummern anlegen
\todo --verbose implementieren
\todo Kommentieren!
*/
class Reporter
{
    protected:
        /// string class to avoid STL's std::string
        class String
        {
            private:
                /// payload - is freed in destructor
                char* s;

            public:
                /// constructor (does only copy pointer, not content)
                String(char* s);

                /// destructor - frees payload
                ~String();

                /// operator for implicit cast to char*
                char* str() const;
        };

    private:
        /// error messages
        static const char* error_messages[];

    public:
        virtual ~Reporter() {}

        /// always report
        virtual void message(const char*, ...) const = 0;

        /// only report in verbose mode
        virtual void status(const char*, ...) const = 0;

        /// display error message and abort program
        __attribute__((noreturn)) virtual void abort(errorcode_t, const char*, ...) const = 0;
};

class ReporterSocket : public Reporter
{
    private:
        Socket mySocket;

    public:
        ReporterSocket(u_short port, const char* ip);
        ~ReporterSocket();

        void message(const char*, ...) const;
        void status(const char*, ...) const;
        __attribute__((noreturn)) void abort(errorcode_t, const char*, ...) const;
};

class ReporterStream : public Reporter
{
    private:
        /// whether to use colored output
        const bool useColor;

    public:
        /// set foreground color to red
        const char* _cr_;
        /// set foreground color to green
        const char* _cg_;
        /// set foreground color to yellow
        const char* _cy_;
        /// set foreground color to blue
        const char* _cb_;
        /// set foreground color to magenta
        const char* _cm_;
        /// set foreground color to cyan
        const char* _cc_;
        /// set foreground color to light grey
        const char* _cl_;

        /// set foreground color to red (underlined)
        const char* _cr__;
        /// set foreground color to green (underlined)
        const char* _cg__;
        /// set foreground color to yellow (underlined)
        const char* _cy__;
        /// set foreground color to blue (underlined)
        const char* _cb__;
        /// set foreground color to magenta (underlined)
        const char* _cm__;
        /// set foreground color to cyan (underlined)
        const char* _cc__;
        /// set foreground color to light grey (underlined)
        const char* _cl__;

        /// set foreground color to red (bold)
        const char* _cR_;
        /// set foreground color to green (bold)
        const char* _cG_;
        /// set foreground color to yellow (bold)
        const char* _cY_;
        /// set foreground color to blue (bold)
        const char* _cB_;
        /// set foreground color to magenta (bold)
        const char* _cM_;
        /// set foreground color to cyan (bold)
        const char* _cC_;
        /// set foreground color to light grey (bold)
        const char* _cL_;

        /// reset foreground color
        const char* _c_;
        /// make text bold
        const char* _bold_;
        /// unterline text
        const char* _underline_;

        /// color the name of a tool
        Reporter::String _ctool_(const char* s) const;
        /// color the name of a file
        Reporter::String _cfilename_(const char* s) const;
        /// color the type of a file
        Reporter::String _coutput_(const char* s) const;
        /// color some good output
        Reporter::String _cgood_(const char* s) const;
        /// color some bad output
        Reporter::String _cbad_(const char* s) const;
        /// color a warning
        Reporter::String _cwarning_(const char* s) const;
        /// color an important message
        Reporter::String _cimportant_(const char* s) const;
        /// color a command-line parameter
        Reporter::String _cparameter_(const char* s) const;

    public:
        ReporterStream();
        ~ReporterStream();
        void message(const char*, ...) const;
        void status(const char*, ...) const;
        __attribute__((noreturn)) void abort(errorcode_t, const char*, ...) const;
};
