/*****************************************************************************\
 Cosme -- Checking Simulation, Matching, and Equivalence

 Copyright (c) 2010 Andreas Lehmann

 Cosme is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Cosme is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Cosme.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

#include "config.h"
#include "Results.h"
#include "verbose.h"
#include "util.h"

Results::Results(std::string name) : filename(name), output(NULL), values() {}

/*!
 \todo the list output is very ad hoc and should be better integrated into the rest
*/
Results::~Results() {
    assert(output == NULL);
    output = new Output(filename, "results file");

    FOREACH(part, values) {
        output->stream() << part->first << ": {\n";
        FOREACH(entry, part->second) {
            output->stream() << "  " << entry->first << " = " << entry->second << ";\n";
        }
        output->stream() << "};\n\n";
    }
    FOREACH(part, lists) {
        output->stream() << part->first << ": {\n";
        FOREACH(entry, part->second) {
            output->stream() << entry->second;
        }
        output->stream() << "};\n\n";
    }

    output->stream() << std::flush;
    delete output;
}

Results::operator std::ostream&() const {
    return output->stream();
}

void Results::add(std::string partname, std::string value) {
    std::string part = partname.substr(0, partname.find_first_of("."));
    std::string name = partname.substr(partname.find_first_of(".") + 1, partname.size());
    add(part, name, value);
}

void Results::add(std::string part, std::string name, std::string value) {
    values[part][name] = "\"" + value + "\"";
}

void Results::add(std::string partname, char* value) {
    std::string part = partname.substr(0, partname.find_first_of("."));
    std::string name = partname.substr(partname.find_first_of(".") + 1, partname.size());
    add(part, name, value);
}

void Results::add(std::string part, std::string name, char* value) {
    values[part][name] = std::string("\"") + value + "\"";
}

void Results::add(std::string partname, float value) {
    std::string part = partname.substr(0, partname.find_first_of("."));
    std::string name = partname.substr(partname.find_first_of(".") + 1, partname.size());
    add(part, name, value);
}

void Results::add(std::string part, std::string name, float value) {
    std::stringstream tmp;
    tmp << value;
    values[part][name] = tmp.str();
}

void Results::add(std::string partname, int value) {
    std::string part = partname.substr(0, partname.find_first_of("."));
    std::string name = partname.substr(partname.find_first_of(".") + 1, partname.size());
    add(part, name, value);
}

void Results::add(std::string part, std::string name, int value) {
    std::stringstream tmp;
    tmp << value;
    values[part][name] = tmp.str();
}

void Results::add(std::string partname, unsigned int value) {
    std::string part = partname.substr(0, partname.find_first_of("."));
    std::string name = partname.substr(partname.find_first_of(".") + 1, partname.size());
    add(part, name, value);
}

void Results::add(std::string part, std::string name, unsigned int value) {
    std::stringstream tmp;
    tmp << value;
    values[part][name] = tmp.str();
}

void Results::add(std::string partname, bool value) {
    std::string part = partname.substr(0, partname.find_first_of("."));
    std::string name = partname.substr(partname.find_first_of(".") + 1, partname.size());
    add(part, name, value);
}

void Results::add(std::string part, std::string name, bool value) {
    std::string tmp = value ? "true" : "false";
    values[part][name] = tmp;
}

void Results::add(std::string partname, std::stringstream& value) {
    std::string part = partname.substr(0, partname.find_first_of("."));
    std::string name = partname.substr(partname.find_first_of(".") + 1, partname.size());
    add(part, name, value);
}

void Results::add(std::string part, std::string name, std::stringstream& value) {
    lists[part][name] = value.str();
}
