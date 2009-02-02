/*
 BOM-ANONYMIZER was written by Niels Lohmann <niels.lohmann@service-technology.org>.

 It is licensed under the Creative Commons Attribution 2.0 Germany License
 (http://creativecommons.org/licenses/by/2.0/de/).
*/

#include <cstdio>
#include <map>
#include <string>
#include <iomanip>
#include <sstream>
#include <cstdlib>

#include "config.h"

using std::string;
using std::map;
using std::ostringstream;
using std::setw;
using std::setfill;

string anonymize(string &s);
string processString(const char *text);
string toString(unsigned int q);

unsigned int nextId = 1;
unsigned int currentView;
map<string, string> string_cache;

// a list of standard values that is not anonymized to avoid parse errors (array must end with empty string)
string value_whitelist[] = {"Boolean", "Byte", "Date", "DateTime", "Double", "Duration", "Float", "Integer", "Long", "Short", "String", "Time", "Communication Service", "Equipment", "Facility", "General Service", "Machine", "Tool", "Person", "Staff", "acres", "centimeters", "feet", "gallons (UK)", "gallons (US)", "grams", "hectares", "inches", "kilograms", "kilometers", "liters", "meters", "miles", "ounces", "pints (UK)", "pints (US)", "pounds", "units", "yards", "true", "false", ""};

extern int yylex();

// split a string into two parts if "##" is found; then anonymize it
string processString(const char *text) {
    // strip the enclosing quotes
    string s(text);
    s = s.substr(1,s.length()-2);

    // strip newlines at beginning or end of string
    if (s[0] == '\n')
        s = s.substr(1,s.length()-1);

    if (s[s.length()-1] == '\n')
        s = s.substr(0,s.length()-1);

    // specially handle strings with "##" separator
    if (s.find("##") == string::npos) {
        return anonymize(s);
    } else {
        // split the string s into two parts: s1 before "##", s2 after
        string s1 = s.substr(0, s.find_first_of("##"));
        string s2 = s.substr(s.find_first_of("##")+2, s.length());

        return anonymize(s1)+"##"+anonymize(s2);
    }

    return s;
}


// anonymize a string by replacing it with some numbered string
string anonymize(string &s) {
    // only if string was not already anonymized, create a new name
    if (string_cache[s] == "") {
        string_cache[s] = "s" + toString(nextId++);
    }

    return string_cache[s];
}


// convert an unsigned int to a string
string toString(unsigned int q) {
    ostringstream buffer;
    buffer << setw(8) << setfill('0') << q;

    return buffer.str();
}


int main(int argc, char** argv) {
    // for debugging purposes
    if (argc == 2 && string(argv[1]) == "--bug") {
        printf("\n\n");
        printf("Please email the following information to %s:\n", PACKAGE_BUGREPORT);
        printf("- tool:              %s\n", PACKAGE_NAME);
        printf("- version:           %s\n", PACKAGE_VERSION);
        printf("- compilation date:  %s\n", __DATE__);
        printf("- compiler version:  %s\n", __VERSION__);
        printf("- platform:          %s\n", BUILDSYSTEM);
        printf("\n\n");
        return EXIT_SUCCESS;
    }

    // initialize string cache with value whitelist to avoid anonymizing thereof
    for (unsigned int i = 0; value_whitelist[i] != ""; i++) {
        string_cache[value_whitelist[i]] = value_whitelist[i];
    }

    int flex_result = yylex();

    // reverse the cache for output of the mapping
    map<string,string> reversed_cache;
    for (map<string,string>::iterator it = string_cache.begin();
         it != string_cache.end(); it++) {
        reversed_cache[it->second] = it->first;
    }

    for (unsigned int i = 1; i < nextId; i++) {
        fprintf(stderr, "\"%s\" <- \"%s\"\n",
                ("s" + toString(i)).c_str(),
                reversed_cache["s" + toString(i)].c_str());
    }

    string_cache.clear();
    reversed_cache.clear();

    return flex_result;
}
