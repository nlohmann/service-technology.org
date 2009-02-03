#include <iostream>
#include <map>
#include <utility>
#include <string>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <sstream>
#include <iomanip>

#include "pnapi.h"

using namespace std;
using namespace pnapi;

typedef map<int, map<int, int> > matrix;


// the labels for the interface
string inputLabels[]  = {"a", "b", "c"};
string outputLabels[] = {"x", "y", "z"};


/// converts streamable data types to string
template<typename T> inline std::string toString(const T q) {
    std::ostringstream buffer;    
    buffer << q;
    
    return buffer.str();
}


// randomly generate an adjacency matrix
matrix generateAdjacency(unsigned int size, bool loopfree = true) {
    matrix A;
    
    for (unsigned int i = 0; i < size; i++) {
        for (unsigned int j = 0; j < size; j++) {
            // try to find a balance between strong connectivity and emptiness
            A[i][j] = rand() % (2 + size/5);
            
            // avoid selfloops
            if (i == j && loopfree)
                A[i][j] = 0;
            
            // avoid jumping back and forth
            if (loopfree) {
              if (A[j][i] != 0)
                A[i][j] = 0;
            }
        }
        
        // node 0 should not have incoming arcs
        if (i == 0) {
            for (unsigned int j = 0; j < size; j++) {
                A[j][i] = 0;
            }
        }
    }
    
    return A;
}


// create an oWFN from an adjacency matrix and randomly connect interface
PetriNet matrix2owfn(matrix &A) {
    PetriNet PN = PetriNet();

    // create places
    for (unsigned int i = 0; i < A.size(); i++) {
        if (i == 0) {
            PN.createPlace("p" + toString(i), Node::INTERNAL, 1);
        } else {
            PN.createPlace("p" + toString(i));
        }
    }
    
    // create transitions
    for (unsigned int i = 0; i < A.size(); i++) {
        for (unsigned int j = 0; j < A[i].size(); j++) {
            if (A[i][j] == 1) {
                string t_label = "t" + toString(i) + "_" + toString(j);
                PN.createTransition(t_label);
                PN.createArc(PN.findPlace("p" + toString(i)), PN.findTransition(t_label));
                PN.createArc(PN.findTransition(t_label), PN.findPlace("p" + toString(j))); 
            }
        }
    }
    
    // remove dead parts of the net
    PN.reduce(1);
    
    
    // add communication places
    for (unsigned int i = 0; i < (sizeof inputLabels)/(sizeof inputLabels[0]); i++) {
        PN.createPlace(inputLabels[i], Place::INPUT);
    }
    for (unsigned int i = 0; i < (sizeof outputLabels)/(sizeof outputLabels[0]); i++) {
        PN.createPlace(outputLabels[i], Place::OUTPUT);
    }
    
    
    // connect interface
    for (set<Transition *>::iterator t = PN.T.begin(); t != PN.T.end(); t++) {
        unsigned int mode = rand() % (2 + A.size()/5);
        
        switch (mode) {
            case(0): {  /* input */
                PN.createArc(PN.findPlace(inputLabels[rand() % (sizeof inputLabels)/(sizeof inputLabels[0])]), *t);
                break;
            }
            case(1): {  /* output */
                PN.createArc(*t, PN.findPlace(outputLabels[rand() % (sizeof outputLabels)/(sizeof outputLabels[0])]));
                break;
            }
            default: {}  /* internal */
        }
    }


    // remove unconnected interface places
    for (unsigned int i = 0; i < (sizeof inputLabels)/(sizeof inputLabels[0]); i++) {
        Place *p = PN.findPlace(inputLabels[i]);
        if (PN.postset(p).empty()) {
            PN.removePlace(p);
        }
    }
    for (unsigned int i = 0; i < (sizeof outputLabels)/(sizeof outputLabels[0]); i++) {
        Place *p = PN.findPlace(outputLabels[i]);
        if (PN.preset(p).empty()) {
            PN.removePlace(p);
        }
    }
    
    
    // add final marking (the file petrinet-output.cc is modified such that
    // final condition consists of the disjunction of all final places)
    for (set<Place*>::iterator p = PN.P.begin(); p != PN.P.end(); p++) {
        if (PN.postset(*p).empty())
            (*p)->isFinal = true;
    }
    
    return PN;
}


// format a string to a fixed width (for filenames)
string format(string text, unsigned int width) {
    std::ostringstream buffer;
    
    buffer << setfill('0');
    buffer << setw( toString(width).length() );
    buffer << text;
    
    return buffer.str();
}


int main(int argc, char** argv) {
    // initialize random number generator
    srand((unsigned)time(0)); 
    
    // read command line parameters
    unsigned int size = (argc < 2) ? 10 : atoi(argv[1]);
    unsigned int nets = (argc < 3) ? 100 : atoi(argv[2]);
    
    unsigned int filecount = 1;
    do {
        matrix adjacency = generateAdjacency(size);
                
        PetriNet PN = matrix2owfn(adjacency);
        
        if (!PN.P.empty() && !PN.T.empty()) {
            string filename = "generated-" + toString(size) + "-" + format(toString(filecount++), nets) + ".owfn";
            ofstream fout(filename.c_str());
            fout << io::owfn << PN;
            fout.close();
            cerr << "wrote oWFN '" << filename << "':\t" << PN << endl;
        }
            
    } while (filecount <= nets);
    
    return 1;
}
