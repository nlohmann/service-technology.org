/*!
\file Reporter.h
\status approved 25.01.2012
*/

#pragma once

#include "Socket.h"

/*!
\todo Datei mit Fehlernummern anlegen
\todo std::string loswerden
\todo --verbose implementieren
\todo Kommentieren!
*/
class Reporter
{
    public:
        ~Reporter() {};

        /// always report
        virtual void message(const char* format, ...) = 0;

        /// only report in verbose mode
        virtual void status(const char* format, ...) = 0;

        /// display error message and abort program
        __attribute__((noreturn)) virtual void abort(unsigned short code, const char* format, ...) = 0;
};

class ReporterSocket : public Reporter
{
    private:
        Socket mySocket;

    public:
        ReporterSocket(u_short port, const char* ip);

        void message(const char* format, ...);
        void status(const char* format, ...);
        __attribute__((noreturn)) void abort(unsigned short code, const char* format, ...);
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
        const char* _ctool_(const char* s);
        /// color the name of a file
        const char* _cfilename_(const char* s);
        /// color the type of a file
        const char* _coutput_(const char* s);
        /// color some good output
        const char* _cgood_(const char* s);
        /// color some bad output
        const char* _cbad_(const char* s);
        /// color a warning
        const char* _cwarning_(const char* s);
        /// color an important message
        const char* _cimportant_(const char* s);
        /// color a command-line parameter
        const char* _cparameter_(const char* s);

    public:
        ReporterStream();
        void message(const char* format, ...);
        void status(const char* format, ...);
        __attribute__((noreturn)) void abort(unsigned short code, const char* format, ...);
};
