/*****************************************************************************\
 LoLA -- a Low Level Petri Net Analyzer

 Copyright (C)  1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007,
                2008, 2009  Karsten Wolf <lola@service-technology.org>

 LoLA is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 LoLA is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with LoLA.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/


#include "net.H"
#include "printnet.H"

void printnet() {
    unsigned int i, j;
    cout << "PLACE";
    for (i = 0; i < Places[0]->cnt; i++) {
        if (i) {
            cout << ",\n";
        } else {
            cout << "\n";
        }
        cout << "\t" << Places[i]->name;
    }
    cout << ";\nMARKING";
    j = 0;
    for (i = 0; i < Places[i]->cnt; i++) {
        if (CurrentMarking[i]) {
            if (j) {
                cout << ",\n";
            } else {
                cout << "\n";
            }
            j++;
            cout << "\t" << Places[i]->name << " : "
                 << CurrentMarking[i] ;
        }
    }
    cout << ";\n";
    for (i = 0; i < Transitions[0]->cnt; i++) {
        cout << "TRANSITION " << Transitions[i] -> name << "\n\tCONSUME";
        for (j = 0; j < Transitions[i]->NrOfArriving; j++) {
            if (j) {
                cout << ",\n";
            } else {
                cout << "\n";
            }
            cout << "\t\t" << Transitions[i]->ArrivingArcs[j]->pl->name << " : " <<
                 Transitions[i]->ArrivingArcs[j]->Multiplicity ;
        }
        cout << ";\n\tPRODUCE";
        for (j = 0; j < Transitions[i]->NrOfLeaving; j++) {
            if (j) {
                cout << ",\n";
            } else {
                cout << "\n";
            }
            cout << "\t\t" << Transitions[i]->LeavingArcs[j]->pl->name << " : " <<
                 Transitions[i]->LeavingArcs[j]->Multiplicity ;
        }
        cout << ";\n";


    }
}

void printnettofile(ofstream& f) {
    unsigned int i, j;
    f << "PLACE";
    for (i = 0; i < Places[0]->cnt; i++) {
        if (i) {
            f << ",\n";
        } else {
            f << "\n";
        }
        f << "\t" << Places[i]->name;
    }
    f << ";\nMARKING";
    j = 0;
    for (i = 0; i < Places[i]->cnt; i++) {
        if (CurrentMarking[i]) {
            if (j) {
                f << ",\n";
            } else {
                f << "\n";
            }
            j++;
            f << "\t" << Places[i]->name << " : "
              << CurrentMarking[i] ;
        }
    }
    f << ";\n";
    for (i = 0; i < Transitions[0]->cnt; i++) {
        f << "TRANSITION " << Transitions[i] -> name << "\n\tCONSUME";
        for (j = 0; j < Transitions[i]->NrOfArriving; j++) {
            if (j) {
                f << ",\n";
            } else {
                f << "\n";
            }
            f << "\t\t" << Transitions[i]->ArrivingArcs[j]->pl->name << " : " <<
              Transitions[i]->ArrivingArcs[j]->Multiplicity ;
        }
        f << ";\n\tPRODUCE";
        for (j = 0; j < Transitions[i]->NrOfLeaving; j++) {
            if (j) {
                f << ",\n";
            } else {
                f << "\n";
            }
            f << "\t\t" << Transitions[i]->LeavingArcs[j]->pl->name << " : " <<
              Transitions[i]->LeavingArcs[j]->Multiplicity ;
        }
        f << ";\n";


    }
}

void printpnml() {
    unsigned int i, j, k;
    cout << "<pnml xmlns=" << '"' << "http://www.informatik.hu-berlin.de/top/pnml/ptNetb" << '"' << ">" << endl;
    cout << "\t<net id=\"" << "LoLA_Ausgabe" << '"' << " type=\"http://www.informatik.hu-berlin.de/top/pntd/ptNetb\">" << endl;
    for (i = 0; i < Places[0]->cnt; i++) {
        Places[i]->nr = i;
        cout << "\t\t<place id=" << '"' << "p" << i << '"' << ">" << endl;
        cout << "\t\t\t<name>" << endl;
        cout << "\t\t\t\t<text>" << Places[i]->name << "</text>" << endl;
        cout << "\t\t\t</name>" << endl;
        cout << "\t\t\t<initialMarking>" << endl;
        cout << "\t\t\t\t<text>" << CurrentMarking[i] << "</text>" << endl;
        cout << "\t\t\t</initialMarking>" << endl;
        cout << "\t\t</place>" << endl;
    }
    for (i = 0; i < Transitions[0]->cnt; i++) {
        cout << "\t\t<transition id=" << '"' << "t" << i << '"' << ">" << endl;
        cout << "\t\t\t<name>" << endl;
        cout << "\t\t\t\t<text>" << Transitions[i]->name << "</text>" << endl;
        cout << "\t\t\t</name>" << endl;
        cout << "\t\t</transition>" << endl;
    }
    for (i = 0, k = 0; i < Transitions[0]->cnt; i++) {
        for (j = 0; j < Transitions[i]->NrOfLeaving; j++, k++) {
            cout << "\t\t<arc id=" << '"' << "a" << k << '"' << " source=" << '"' << "t" << i << '"' << " target=" << '"' << "p" << Transitions[i]->LeavingArcs[j]->pl->nr << '"' << ">" << endl;
            cout << "\t\t\t<inscription>" << endl;
            cout << "\t\t\t\t<text>" << Transitions[i]->LeavingArcs[j]->Multiplicity << "</text>" << endl;
            cout << "\t\t\t</inscription>" << endl;
            cout << "\t\t</arc>" << endl;
        }

    }
    for (i = 0, k++; i < Transitions[0]->cnt; i++) {
        for (j = 0; j < Transitions[i]->NrOfArriving; j++, k++) {
            cout << "\t\t<arc id=" << '"' << "a" << k << '"' << " source=" << '"' << "p" << Transitions[i]->ArrivingArcs[j]->pl->nr << '"' << " target=" << '"' << "t" << i << '"' << ">" << endl;
            cout << "\t\t\t<inscription>" << endl;
            cout << "\t\t\t\t<text>" << Transitions[i]->ArrivingArcs[j]->Multiplicity << "</text>" << endl;
            cout << "\t\t\t</inscription>" << endl;
            cout << "\t\t</arc>" << endl;
        }

    }
    cout << "\t\t<name>" << endl;
    cout << "\t\t\t<text>" << netfile << "</text>" << endl;
    cout << "\t\t</name>" << endl;
    cout << "\t</net>" << endl;
    cout << "</pnml>" << endl;
}

void printpnmltofile(ofstream& f) {
    unsigned int i, j, k;
    f << "<pnml xmlns=" << '"' << "http://www.informatik.hu-berlin.de/top/pnml/ptNetb" << '"' << ">" << endl;
    f << "\t<net id=\"" << "LoLA_Ausgabe" << '"' << " type=\"http://www.informatik.hu-berlin.de/top/pntd/ptNetb\">" << endl;
    for (i = 0; i < Places[0]->cnt; i++) {
        Places[i]->nr = i;
        f << "\t\t<place id=" << '"' << "p" << i << '"' << ">" << endl;
        f << "\t\t\t<name>" << endl;
        f << "\t\t\t\t<text>" << Places[i]->name << "</text>" << endl;
        f << "\t\t\t</name>" << endl;
        f << "\t\t\t<initialMarking>" << endl;
        f << "\t\t\t\t<text>" << CurrentMarking[i] << "</text>" << endl;
        f << "\t\t\t</initialMarking>" << endl;
        f << "\t\t</place>" << endl;
    }
    for (i = 0; i < Transitions[0]->cnt; i++) {
        f << "\t\t<transition id=" << '"' << "t" << i << '"' << ">" << endl;
        f << "\t\t\t<name>" << endl;
        f << "\t\t\t\t<text>" << Transitions[i]->name << "</text>" << endl;
        f << "\t\t\t</name>" << endl;
        f << "\t\t</transition>" << endl;
    }
    for (i = 0, k = 0; i < Transitions[0]->cnt; i++) {
        for (j = 0; j < Transitions[i]->NrOfLeaving; j++, k++) {
            f << "\t\t<arc id=" << '"' << "a" << k << '"' << " source=" << '"' << "t" << i << '"' << " target=" << '"' << "p" << Transitions[i]->LeavingArcs[j]->pl->nr << '"' << ">" << endl;
            f << "\t\t\t<inscription>" << endl;
            f << "\t\t\t\t<text>" << Transitions[i]->LeavingArcs[j]->Multiplicity << "</text>" << endl;
            f << "\t\t\t</inscription>" << endl;
            f << "\t\t</arc>" << endl;
        }

    }
    for (i = 0, k++; i < Transitions[0]->cnt; i++) {
        for (j = 0; j < Transitions[i]->NrOfArriving; j++, k++) {
            f << "\t\t<arc id=" << '"' << "a" << k << '"' << " source=" << '"' << "p" << Transitions[i]->ArrivingArcs[j]->pl->nr << '"' << " target=" << '"' << "t" << i << '"' << ">" << endl;
            f << "\t\t\t<inscription>" << endl;
            f << "\t\t\t\t<text>" << Transitions[i]->ArrivingArcs[j]->Multiplicity << "</text>" << endl;
            f << "\t\t\t</inscription>" << endl;
            f << "\t\t</arc>" << endl;
        }

    }
    f << "\t\t<name>" << endl;
    f << "\t\t\t<text>" << netfile << "</text>" << endl;
    f << "\t\t</name>" << endl;
    f << "\t</net>" << endl;
    f << "</pnml>" << endl;
}
