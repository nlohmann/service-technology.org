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

    std::string input_filename(argv[1]);
    if (!input_filename.compare(input_filename.size()-4, input_filename.size(), ".sa")) {
        cerr << "Expected file ending '.sa' in parameter #1." << endl;
        return EXIT_FAILURE;
    }

    Automaton source;
    //PetriNet target;
    std::ifstream file(input_filename.c_str(), std::ios_base::in);
    file >> io::sa >> source;
    //file >> io::sa2sm >> target;

    PetriNet target = source.stateMachine();

    if (argc < 3) {
        cout << io::owfn << target;
    } else {
        std::string output_filename(argv[2]);
        std::ofstream output(output_filename.c_str(), std::ios_base::trunc);
        output << io::owfn << target;
    }

    return EXIT_SUCCESS;
}
