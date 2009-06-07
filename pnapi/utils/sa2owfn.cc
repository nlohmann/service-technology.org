#include "pnapi.h"

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>

using namespace pnapi;
using std::cerr;
using std::cout;
using std::endl;


int main(int argc, char *argv[])
{
    if (argc < 2) {
        cerr << "No input file given. Expecting SA file as parameter #2." << endl;
        return EXIT_FAILURE;
    }

    std::string input_filename;
    input_filename.assign(argv[1]);
    if (!input_filename.compare(input_filename.size()-4, input_filename.size(), ".sa")) {
        cerr << "Expected file ending '.sa' in parameter #1." << endl;
        return EXIT_FAILURE;
    }

    Automaton source;
    std::ifstream file;
    file.open(input_filename.c_str(), std::ios_base::in);
    file >> io::sa >> source;
    file.close();

    PetriNet target(source);

    if (argc < 3) {
        cout << io::owfn << target;
    } else {
        std::ofstream output;
        output.open(argv[2], std::ios_base::trunc);
        output << io::owfn << target;
        output.close();
    }

    return EXIT_SUCCESS;
}
