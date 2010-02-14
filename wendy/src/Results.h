#ifndef _RESULTS_H
#define _RESULTS_H

#include <string>
#include <map>
#include <algorithm>
#include <vector>
#include "Output.h"

class Results {
    public:
        /// constructor
        explicit Results(std::string);
        /// destructor
        ~Results();

        /// implicit conversation operator to ostream
        operator std::ostream&() const;

        void add(std::string,              std::string);
        void add(std::string, std::string, std::string);
        void add(std::string,              char*);
        void add(std::string, std::string, char*);
        void add(std::string,              float);
        void add(std::string, std::string, float);
        void add(std::string,              unsigned int);
        void add(std::string, std::string, unsigned int);
        void add(std::string,              int);
        void add(std::string, std::string, int);
        void add(std::string,              bool);
        void add(std::string, std::string, bool);

    private:
        /// the output filename
        std::string filename;

        /// the output object
        Output *output;

        /// the stored values
        std::map<std::string, std::map<std::string, std::string> > values;
};

#endif
