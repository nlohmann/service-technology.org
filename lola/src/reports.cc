
#include "reports.H"
#include "graph.H"

void printstate(const char*, unsigned int*);

graphreport_yes::graphreport_yes(char* name) {
    graphstream = new std::ofstream(name);
    if (graphstream->fail()) {
        fprintf(stderr, "lola: cannot open graph output file '%s'\n", name);
        fprintf(stderr, "      no output written\n");
        _exit(4);
    }
}

graphreport_yes::graphreport_yes() { // stdout
    graphstream = &std::cout;
}
void graphreport_yes::tell() {
    *graphstream << "STATE";
    for (unsigned int i = 0, j = 0; i < Globals::Places[0]->cnt; i++) {
        if (Globals::CurrentMarking[i]) {
            if (Globals::CurrentMarking[i] == VERYLARGE) {
                *graphstream << (j++ ? ",\n" : "\n") << Globals::Places[i]->name << " : oo" ;
            } else {
                *graphstream << (j++ ? ",\n" : "\n") << Globals::Places[i]->name << " : " << Globals::CurrentMarking[i];
            }
        }
    }
    *graphstream << "\n" << endl;
    *graphstream << "Fire list: \n";
    for (int i = 0; i < Globals::Transitions[0]->cnt; ++i) {
        *graphstream << Globals::Transitions[i] -> name << "\n";
    }
}

graphreport* TheGraphReport = NULL;
