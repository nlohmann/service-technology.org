#include <sstream>
#include "config.h"
#include "Results.h"
#include "verbose.h"
#include "util.h"

Results::Results(std::string name) : filename(name), output(NULL), values() {}

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

    output->stream() << std::flush;
    delete output;
}

Results::operator std::ostream&() const {
    return output->stream();
}

void Results::add(std::string partname, std::string value) {
    std::string part = partname.substr(0, partname.find_first_of("."));
    std::string name = partname.substr(partname.find_first_of(".")+1, partname.size());
    add(part, name, value);
}

void Results::add(std::string part, std::string name, std::string value) {
    values[part][name] = "\"" + value + "\"";
}

void Results::add(std::string partname, char *value) {
    std::string part = partname.substr(0, partname.find_first_of("."));
    std::string name = partname.substr(partname.find_first_of(".")+1, partname.size());
    add(part, name, value);
}

void Results::add(std::string part, std::string name, char *value) {
    values[part][name] = std::string("\"") + value + "\"";
}

void Results::add(std::string partname, float value) {
    std::string part = partname.substr(0, partname.find_first_of("."));
    std::string name = partname.substr(partname.find_first_of(".")+1, partname.size());
    add(part, name, value);
}

void Results::add(std::string part, std::string name, float value) {
    std::stringstream tmp;
    tmp << value;
    values[part][name] = tmp.str();
}

void Results::add(std::string partname, int value) {
    std::string part = partname.substr(0, partname.find_first_of("."));
    std::string name = partname.substr(partname.find_first_of(".")+1, partname.size());
    add(part, name, value);
}

void Results::add(std::string part, std::string name, int value) {
    std::stringstream tmp;
    tmp << value;
    values[part][name] = tmp.str();
}

void Results::add(std::string partname, unsigned int value) {
    std::string part = partname.substr(0, partname.find_first_of("."));
    std::string name = partname.substr(partname.find_first_of(".")+1, partname.size());
    add(part, name, value);
}

void Results::add(std::string part, std::string name, unsigned int value) {
    std::stringstream tmp;
    tmp << value;
    values[part][name] = tmp.str();
}

void Results::add(std::string partname, bool value) {
    std::string part = partname.substr(0, partname.find_first_of("."));
    std::string name = partname.substr(partname.find_first_of(".")+1, partname.size());
    add(part, name, value);
}

void Results::add(std::string part, std::string name, bool value) {
    std::string tmp = value ? "true" : "false";
    values[part][name] = tmp;
}
