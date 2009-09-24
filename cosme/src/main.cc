#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <cstdio>
#include <cstdlib>
#include "Label.h"
#include "Service.h"
#include "OperatingGuideline.h"
//TODO: anders einbinden?!
#include <../libs/pnapi/pnapi.h>

using std::cerr;
using std::cout;
using std::endl;
using std::ofstream;

// lexer and parser
extern int og_yyparse();
extern int graph_yyparse();
extern FILE* og_yyin;
extern FILE* graph_yyin;

Label GlobalLabels;
std::map <std::string, label_id_t> TransitionLabels;
std::set<label_id_t> OGInterface;
std::set<label_id_t> ServiceInterface;
std::map<og_service_index_t, OGMarking*> OGMarkings;
std::map<og_service_index_t, ServiceMarking*> ServiceMarkings;

pnapi::PetriNet tmpNet;

OperatingGuideline* parseOG(char* file) {
	og_yyin = fopen(file, "r");
	
	if(!og_yyin) // if an error occurred
  {
    cerr << PACKAGE << ": ERROR: failed to open input file '"
         << file << "'" << endl;
		return NULL;
  }
  
  // actual parsing
  og_yyparse();
  
  // close input (output is closed by destructor)
  fclose(og_yyin);

	OperatingGuideline *tmpOG = new OperatingGuideline(OGMarkings, OGInterface);
	OGMarkings.clear();
	OGInterface.clear();

	return tmpOG;
}

inline void parseService_LabelHelper() {

	GlobalLabels.mapLabel("Tau");
	label_id_t curLabel;
	std::pair<std::map<std::string, label_id_t>::iterator, bool> ret; 

	const std::set<pnapi::Place*> outputPlaces( tmpNet.getOutputPlaces() );
  for (std::set<pnapi::Place*>::const_iterator p = outputPlaces.begin(); p != outputPlaces.end(); ++p) {
		// save current !-event
		string tmpStr("!"); tmpStr += (*p)->getName();
		curLabel = GlobalLabels.mapLabel(tmpStr);
		ServiceInterface.insert(curLabel);

    const std::set<pnapi::Node*> preset( (*p)->getPreset() );
    for (std::set<pnapi::Node*>::const_iterator t = preset.begin(); t != preset.end(); ++t) {
      // save current Transitionlabel
			ret = TransitionLabels.insert(make_pair((*t)->getName(), curLabel));
			assert(ret.second);
    }
  }
 
  const std::set<pnapi::Place*> inputPlaces( tmpNet.getInputPlaces() );
  for (std::set<pnapi::Place*>::const_iterator p = inputPlaces.begin(); p != inputPlaces.end(); ++p) {
		// save current ?-event
    string tmpStr("?"); tmpStr += (*p)->getName();
		curLabel = GlobalLabels.mapLabel(tmpStr);
		ServiceInterface.insert(curLabel);

    const std::set<pnapi::Node*> postset( (*p)->getPostset() );
    for (std::set<pnapi::Node*>::const_iterator t = postset.begin(); t != postset.end(); ++t) {
    	ret = TransitionLabels.insert(make_pair((*t)->getName(), curLabel));
			assert(ret.second);
    }
  }

}

Service* parseService(char* file) {
	std::ifstream InputStream(file);
	if (!InputStream) {
		cerr << PACKAGE << ": ERROR: failed to open input file '"
         << file << "'" << endl;
		return NULL;
	}	
	
	// parse OWFN
	InputStream >> pnapi::io::owfn >> tmpNet;
	InputStream.close();

	//TODO: notwendig?	
	// "fix" the net in order to avoid parse errors from LoLA (see bug #14166)
  if (tmpNet.getTransitions().empty()) {
		cerr << PACKAGE << ": ERROR: net has no transitions" << endl;
		return NULL;
  }
  // only normal nets are supported so far
  if (!tmpNet.isNormal()) {
		cerr << PACKAGE << ": ERROR: the input open net must be normal" << endl;
		return NULL;
  } 
	
	parseService_LabelHelper();

	//TODO: auf Pipes umstellen
	std::ofstream OutputStream("myFile.lola");
	if (!OutputStream) {
		cerr << PACKAGE << ": ERROR: failed to open output file '"
         << "myFile.lola" << "'" << endl;
		return NULL;
	}

	OutputStream << pnapi::io::lola << tmpNet;
	OutputStream.close();

	cout << "\ncalling LoLA\n";
	int ret;
	//TODO: Aufruf flexibler machen
	ret = system("lola-statespace myFile.lola -m");
	cout << "\nLoLa called\n";

	graph_yyin = fopen("myFile.graph", "r");	
	if(!graph_yyin) // if an error occurred
  {
    cerr << PACKAGE << ": ERROR: failed to open input file '"
         << "myFile.graph" << "'" << endl;
		return NULL;
  }
  graph_yyparse();
  fclose(graph_yyin);
	
	Service *tmpService = new Service(ServiceMarkings, ServiceInterface);
	ServiceMarkings.clear();
	ServiceInterface.clear();

	return tmpService;

}

// main function
int main(int argc, char** argv)
{
	
	//TODO: cmdline-parser einbinden

	if (argc < 3) {
		cout << "usage: " << endl;
		cout << " o A.og B.og (check simulation/equivalence)" << endl;
		cout << " s C.owfn B.og (check matching)" << endl;
		return EXIT_FAILURE;
	}

	if (argv[1][0] == 'o') {
		OperatingGuideline *A, *B;

		A = parseOG(argv[2]);
		assert(A != NULL);

		cout << GlobalLabels.toString();

		B = parseOG(argv[3]);
		assert(B != NULL);

		cout << GlobalLabels.toString();

		//cout << A->toString() << "\n" << B->toString();

		cout << "\nTests:\n";
		if (A->isSimulation(*B)) cout << "B simulates A\n";
		if (B->isSimulation(*A)) cout << "A simulates B\n";
		if (A->isEquivalent(*B)) cout << "A is equivalent to B\n";

		delete A;
		delete B;
	}
	else if (argv[1][0] == 's') {

		Service* C;

		C = parseService(argv[2]);
		assert(C != NULL);

		cout << GlobalLabels.toString();
		//cout << C->toString() << "\n";

		OperatingGuideline *A;

		A = parseOG(argv[3]);
		assert(A != NULL);

		cout << GlobalLabels.toString();

		//new stuff
		C->calculateBitSets(GlobalLabels);
		A->calculateBitSets(GlobalLabels);

		//cout << C->toString() << "\n";
		//cout << A->toString() << "\n";

		/*
		cout << "\n\nTransition-Labels:\n";
		for (std::map <std::string, label_id_t>::iterator it = TransitionLabels.begin();
						it != TransitionLabels.end(); it++)
				cout << "  " << it->first << " : " << static_cast<unsigned int>(it->second) << "\n";
		*/

		cout << "\nTests:\n";
		if (A->isMatching(*C)) cout << "C matches A\n";

		delete A;
		delete C;
	}
	else {
		cout << "usage: " << endl;
		cout << " o A.og B.og (check simulation/equivalence)" << endl;
		cout << " s C.owfn B.og (check matching)" << endl;
		return EXIT_FAILURE;
	}

  return EXIT_SUCCESS;
}

