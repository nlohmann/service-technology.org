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


#include "symm.h"
#include <climits>
#include "graph.h"
#include "Globals.h"
#include <cstdlib>
#ifdef DISTRIBUTE
#include "distribute.h"
#endif

// for testing purposes only
//#define SYMMPROD

using std::ofstream;

unsigned int* kanrep;
unsigned int* kanrep1;
unsigned int* compose;
unsigned int* reservecompose;
unsigned int kanhash;

// Begriffe:
// Symmetrie: Bijektion auf Plaetzen und Transitionen, also Menge
// von geordneten Paaren von Knoten
// Abstrakte Symmetrie: Menge von geordneten Paaren von Knotenmengen derart,
// dass linke Seiten und rechte Seiten jeweils Partition der Knotenmenge
// bilden. Mengen enthalten immer entweder nur Plaetze oder nur Transitionen
// Constraint: Ein geordnetes Paar einer abstrakten Symmetrie
// Symmetrie sigma passt zu abstrakter Symm, falls fuear alle Constraints
// [A,B]: sigma(A) = B

// Algorithmische Idee:
// Start: eine Abstrakte Symmetrie
// Ziel: passende Symmetrien = abstrakte Symm mit einelementigen Mengen
// Weg:
// 1. Refine = Schlussfolgerungen aus gegebener abstrakter Symmetrie
// Mehr dazu weiter unten
// 2. Define = systematisches Probieren einer weiteren Verfeinerung
// der gegebenen abstrakten Symm
// mehr dazu unter Define*


// Grundprinzip von Refine:
// - nimm zwei Constraints [A1,B1] und [A2,B2], eine Bogenrichtung und eine
//   Vielfacheit
// - zaehle fuer jeden Knoten in A1 und B1, wieviele solche Boegen er von/zu
//   einem Knoten in A2 bzw B2 hat.
// - fuer jedes i: neues Constraint [A1i,B1i], wobei A1i,B1i die Mengen der
//   Knoten aus A1,B1 mit i Hits

/*!
 Ein Reaktoreintrag dient dem Zaehlen der Hits fuer einen Knoten.
*/
class Reaktoreintrag {
    public:
        Node* node; //Knoten, fuer den gezaehlt wird
        unsigned int count; // Anzahl der Hits
        unsigned int stamp; // Rundennummer - zeigt an, ob Hitzahl in aktueller Runde erzielt wurde (spart resets fuer count)
        Reaktoreintrag();
};

Reaktoreintrag::Reaktoreintrag() : node(NULL), count(0), stamp(0) {}

// Pro Knotentyp (1. Index) je ein Zaehlarray fuer
// die Ai ("DO") und die Bi ("CO") (2. Index).
Reaktoreintrag* Reaktor[2][2];


/*!
 ToDo ist ein Element in der Liste derjenigen Constraints, mit denen noch
 nicht alle Refines ausprobiert wurden
*/
class ToDo {
    public:
        unsigned int constraint; // Die Nr des fraglichen Constraints im
        // Array aller Constraints (= Specification)
        ToDo* next;              // Listenpointer
        ToDo(NodeType, unsigned int);
};

ToDo* ToDoList[2];  // Eine für Platzconstraints, eine für Transitionsconstraints

/*!
 Kreiert neuen Eintrag und sortiert ihn am Listenanfang ein.
*/
ToDo::ToDo(NodeType TY, unsigned int co) : constraint(co), next(ToDoList[TY]) {
    ToDoList[TY] = this;
}

/*!
 Durch permanentes Umsortieren der Reaktor-Arrays wird sichergestellt, dass
 die Elemente eines Constraints immer einen zusammenhaengenden Bereich bilden
*/
class Constraint {
    public:
        unsigned int first; // Start des Bereichs im Reaktor-Array, wo Knoten dieser Klasse stehen
        unsigned int last;  // Ende -"-
        ToDo* changed;      // Ein ToDoEintrag für mich
        unsigned int parent; // Constraint, von dem ich abgespalten wurde
};

unsigned int CardSpecification[2];  // Anzahl der Constraints (für PL / für TR)

Constraint* Specification[2]; // Arrays fuer die Constraints (PL/TR)

/*!
 Hin und wieder Lebenszeichen geben, damit Nutzer dei Ctrl-C-Taste wieder
 loslasesst
*/
inline void reportprogress() {
    if (!((CardSpecification[PL] + CardSpecification[TR]) % REPORTFREQUENCY)) {
        message("Depth %d", CardSpecification[PL] + CardSpecification[TR]);
    }
}

unsigned int DeadBranches; // zaehlt rekursive Abstiege, die nicht zu einem
// Element des Ezeugendensystems fuehren
// (einzige Quelle fuer Exponentialitaet)
unsigned int CardGenerators; // Anzahl der berechneten Erzeugenden

/*!
 Fuer jeden Knoten werden die Arrays der eingehenden und ausgehenden Boegen
 nach Vielfacheit sortiert. So stehen z.B. spaeter die Boegen gleicher
 Vielfachheit als zusammenhaengende Bereiche zur Verfuegung. Alle
 Sortierroutinen in symm.cc sind Quicksort.
*/
void ArcSort(Arc** list, unsigned int from, unsigned int to) {
    unsigned int less, current, greater;
    Arc* swap;

    greater = from;
    current = from + 1;
    less = to;

    while (current <= less) {
        if (list[current]->Multiplicity > list[current - 1] -> Multiplicity) {
            swap = list[current];
            list[current++] = list[greater];
            list[greater++] = swap;
        } else {
            if (list[current]->Multiplicity == list[current - 1] -> Multiplicity) {
                current++;
            } else {
                swap = list[current];
                list[current] = list[less];
                list[less--] = swap;
            }
        }
    }
    if (greater - from > 1) {
        ArcSort(list, from, greater - 1);
    }
    if (to - less > 1) {
        ArcSort(list, less + 1, to);
    }
}

/*!
 Initialisierung der Symm-Berechnungs-Datenstrukturen
*/
void init_syms() {
    ToDoList[PL] = ToDoList[TR] = NULL; // Zunaechst mal nix zu tun

    // wir starten mit den Constraints [P,P] und [T,T]
    Specification[PL] = new Constraint [Globals::Places[0] -> cnt];
    Specification[TR] = new Constraint[Globals::Transitions[0] -> cnt];
    Specification[PL][0].first = Specification[TR][0].first = Specification[PL][0].parent
                                                              = Specification[TR][0].parent = 0;
    Specification[PL][0].last = Globals::Places[0]-> cnt - 1;
    Specification[TR][0].last = Globals::Transitions[0]-> cnt - 1;
    CardSpecification[PL] = CardSpecification[TR] = 1;

    // Anlegen der Zaehlarrays
    Reaktor[PL][DO] = new Reaktoreintrag [Globals::Places[0]->cnt];
    Reaktor[PL][CO] = new Reaktoreintrag [Globals::Places[0]->cnt];
    Reaktor[TR][DO] = new Reaktoreintrag [Globals::Transitions[0]->cnt];
    Reaktor[TR][CO] = new Reaktoreintrag [Globals::Transitions[0]->cnt];

    // Alle Bogenlisten sortieren
    for (unsigned i = 0; i < Globals::Places[0]->cnt; i++) {
        if (Globals::Places[i]->NrOfLeaving) {
            ArcSort(Globals::Places[i]->LeavingArcs, 0, Globals::Places[i]->NrOfLeaving - 1);
        }
        if (Globals::Places[i]->NrOfArriving) {
            ArcSort(Globals::Places[i]->ArrivingArcs, 0, Globals::Places[i]->NrOfArriving - 1);
        }
    }
    for (unsigned i = 0; i < Globals::Transitions[0]->cnt; i++) {
        if (Globals::Transitions[i]->NrOfLeaving) {
            ArcSort(Globals::Transitions[i]->LeavingArcs, 0, Globals::Transitions[i]->NrOfLeaving - 1);
        }
        if (Globals::Transitions[i]->NrOfArriving) {
            ArcSort(Globals::Transitions[i]->ArrivingArcs, 0, Globals::Transitions[i]->NrOfArriving - 1);
        }
    }
    DeadBranches = CardGenerators = 0;
}

// Wir sortieren Knotenmengen nach den folgenden ca 1000 verschiedenen
// Sortierkriterien. Das Sortieren dient der Separierung von Knoten-
// mengen. Wir wollen nur solche Symmetrien berechnen, die Knoten
// auf Knoten mit gleichem Wert bzgl der verwendeten Kriterien abbilden
// Diese Constraintseparierung beinhaltet Sortieren, so dass die
// Teilconstraints wiederum zusammenhaengende Bereiche bilden.
// Diese Schritte passieren zu einem Zeitpunkt, wo klar ist, dass
// fuer jeden Constraint [A,B] gilt: A=B. Daher wird noch nicht die
// Datenstruktur Reaktor genutzt, die auf A!=B gemuenzt ist. Statt dessen
// rechnen wir noch auf den Arrays Globals::Places unf Globals::Transitions selbst und
// kopieren spaeter in den Reaktor (Funktion "FuelleReaktor").

/*!
 Ist ein zu analysierender Platz p gegeben, soll [{p},{p}] ein Constraint
 bilden. So bildet jede berechnete Symm p auf p ab und die p betreffenden
 Resultate bleiben akkurat. Sortieren bewegt diesen Platz ans Ende des
 Platzarrays.
*/
unsigned int get_target_place(Node* node) {
    return (static_cast<Place*>(node) == Globals::CheckPlace) ? 1 : 0;
}

/*!
 Ist ein zu analysierender Transition t gegeben, soll [{t},{t}] ein Constraint
 bilden. So bildet jede berechnete Symm t auf t ab und die t betreffenden
 Resultate bleiben akkurat. Sortieren bewegt diese Transition ans Ende des
 Platzarrays.
*/
unsigned int get_target_transition(Node* node) {
    return (static_cast<Transition*>(node) == Globals::CheckTransition) ? 1 : 0;
}

/*!
 Ist eine zu erreichende Zielmarkierung gegeben, soll jede Symm diese auf
 sich selbst abbilden.
*/
unsigned int get_target_marking(Node* node) {
    return static_cast<Place*>(node)->target_marking;
}

/*!
 Die Anfangsmarkierung soll auf sich selbst abgebildet werden.
*/
unsigned int get_marking(Node* node) {
    return static_cast<Place*>(node)->initial_marking;
}

/*!
 Symmetrien koennen sowieso nur Knoten mit gleichem In- und Out-Grad
 aufeinander abbilden. Das kann man gleich praeprozessen.
*/
unsigned int get_card_arcs_in(Node* node) {
    return node->NrOfArriving;
}

/*!
 Symmetrien koennen sowieso nur Knoten mit gleichem In- und Out-Grad
 aufeinander abbilden. Das kann man gleich praeprozessen.
*/
unsigned int get_card_arcs_out(Node* node) {
    return node->NrOfLeaving;
}

// Zwei Variablen zur Uebergabe von Parametern an die darauffolgenden
// Sortierroutinen, die anderweitig nicht in die einheitliche Signatur
// der Sortierroutinen passen wuerden

unsigned int this_arc_nr;
unsigned int this_direction;

/*!
 Symmetrien koennen eh nur Knoten aufeinander abbilden, die, fuer eine
 gegebene Vielfachheit, gleich viele ausgehende bzw eingehende Boegen mit
 dieser Vielfachheit haben. Setzt man soertierte Bogenlisten und vorangeganges
 Sortieren nach Bogengesamtzahl voraus, reicht es nun, jeweils anhand der
 Vielfachheiten der jeweils i-ten Boegen zu sortieren und danach zu
 separieren.
*/
unsigned int get_arc_mult(Node* node) {
    return (this_direction ? node->ArrivingArcs : node->LeavingArcs)[this_arc_nr]->Multiplicity;
}

/*!
 Dies ist die generische Sortierroutine fuer Plaetze
*/
void PlaceSort(unsigned int from, unsigned int to, unsigned int attribute(Node*)) {
    unsigned int less = from;
    unsigned int current = from + 1;
    unsigned int greater = to;

    while (current <= greater) {
        if (attribute((Node*) Globals::Places[current]) < attribute((Node*) Globals::Places[current - 1])) {
            Place* swap = Globals::Places[current];
            unsigned int sw = Globals::CurrentMarking[current];
            Globals::Places[current] = Globals::Places[less];
            Globals::CurrentMarking[current++] = Globals::CurrentMarking[less];
            Globals::Places[less] = swap;
            Globals::CurrentMarking[less++] = sw;
        } else {
            if (attribute((Node*) Globals::Places[current]) == attribute((Node*) Globals::Places[current - 1])) {
                current++;
            } else {
                Place* swap = Globals::Places[current];
                unsigned int sw = Globals::CurrentMarking[current];
                Globals::Places[current] = Globals::Places[greater];
                Globals::CurrentMarking[current] = Globals::CurrentMarking[greater];
                Globals::Places[greater] = swap;
                Globals::CurrentMarking[greater--] = sw;
            }
        }
    }
    if (less - from > 1) {
        PlaceSort(from, less - 1, attribute);
    }
    if (to - greater > 1) {
        PlaceSort(greater + 1, to, attribute);
    }
}

/*!
 Dies ist die generische Sortierroutine fuer Transitionen
*/
void TransitionSort(unsigned int from, unsigned int to, unsigned int attribute(Node*)) {
    unsigned int less = from;
    unsigned int current = from + 1;
    unsigned int greater = to;

    while (current <= greater) {
        if (attribute((Node*) Globals::Transitions[current]) < attribute((Node*) Globals::Transitions[current - 1])) {
            Transition* swap = Globals::Transitions[current];
            Globals::Transitions[current++] = Globals::Transitions[less];
            Globals::Transitions[less++] = swap;
        } else {
            if (attribute((Node*) Globals::Transitions[current]) == attribute((Node*) Globals::Transitions[current - 1])) {
                current++;
            } else {
                Transition* swap = Globals::Transitions[current];
                Globals::Transitions[current] = Globals::Transitions[greater];
                Globals::Transitions[greater--] = swap;
            }
        }
    }
    if (less - from > 1) {
        TransitionSort(from, less - 1, attribute);
    }
    if (to - greater > 1) {
        TransitionSort(greater + 1, to, attribute);
    }
}

// Das Splitten von Constraints nach einem mitgegebenen Kriterium
// beinhaltet das Sortieren nach diesem Kriterium, gefolgt vom
// Zusammenfassen der nun zusammenhaengenden Bereiche im Platzarray mit
// jweilse gleichem Kriteriumswert
// SplitInVorReaktor differenziert noch nicht nach linken und rechten Seiten
// der geordneten Paare, weil zum Zeitpunkt der Anwendung noch linke und
// rechte Seiten aller geordneten Paare gleich sind.

// c: zu splittendes Constraint, attribute: Splitkriterium in Form einer
// der obigen Routinen
void SplitPlacesInVorReaktor(unsigned int c, unsigned int attribute(Node*)) {
    PlaceSort(Specification[PL][c].first, Specification[PL][c].last, attribute);
    // Nun bilden die zukünftigen Teilconstraints zusammenhaengende Bereiche im
    // Originalconstraint
    unsigned int oldc = c;
    unsigned int firstc = Specification[PL][c].first;
    unsigned int lastc = Specification[PL][c].last;

    // Solange noch verschiedene Werte im verbleibenden Constraint...
    while (attribute((Node*) Globals::Places[firstc]) != attribute((Node*) Globals::Places[lastc])) {
        // Suche erste Stelle, wo Nachbarknoten unterschiedliche Werte liefern
        // (dort wird Constraint geteilt)
        unsigned int i;
        for (i = firstc + 1; attribute((Node*) Globals::Places[firstc]) == attribute((Node*) Globals::Places[i]); i++) {
            ;
        }
        unsigned int newc = CardSpecification[PL]; // naechste freie Constraintnummer
        // Teilen: wir haben ja sortiert...
        Specification[PL][oldc].last = i - 1;
        Specification[PL][newc].first = i;
        Specification[PL][newc].last = lastc;
        Specification[PL][newc].changed = NULL; // um ToDo kuemmern wir uns
        // spaeter
        Specification[PL][newc].parent = 0;    // keine der hier
        // vorgenommenen Separierungen
        // wird je zurueckgenommen,
        // also brauchen wir kein parent
        CardSpecification[PL]++;
        reportprogress();
#ifdef DISTRIBUTE
        progress();
#endif
        firstc = i;  // ... Der Rest muss ggf weiter geteilt werden
        oldc = newc;
    }
}

// Kopie der vorigen Routine, aber fuer Transitionen
void SplitTransitionsInVorReaktor(unsigned int c, unsigned int attribute(Node*)) {
    TransitionSort(Specification[TR][c].first, Specification[TR][c].last, attribute);
    unsigned int oldc = c;
    unsigned int firstc = Specification[TR][c].first;
    unsigned int lastc = Specification[TR][c].last;
    while (attribute((Node*) Globals::Transitions[firstc]) != attribute((Node*) Globals::Transitions[lastc])) {
        unsigned int i;
        for (i = firstc + 1; attribute((Node*) Globals::Transitions[firstc]) == attribute((Node*) Globals::Transitions[i]); i++) {
            ;
        }
        unsigned int newc = CardSpecification[TR];
        Specification[TR][oldc].last = i - 1;
        Specification[TR][newc].first = i;
        Specification[TR][newc].last = lastc;
        Specification[TR][newc].changed = NULL;
        Specification[TR][newc].parent = 0;
        CardSpecification[TR]++;
        reportprogress();
#ifdef DISTRIBUTE
        progress();
#endif
        firstc = i;
        oldc = newc;
    }
}

/*!
  Berechnung der initialen abstrakten Symmetrie
*/
void InitialConstraint() {
    unsigned int c, b;

    // Separierung nach Zahl der eingehenden/ausgehenden Boegen - ist nie falsch
    SplitPlacesInVorReaktor(0, get_card_arcs_in);
    SplitTransitionsInVorReaktor(0, get_card_arcs_in);
    unsigned int cmax = CardSpecification[PL];
    for (c = 0; c < cmax; c++) {
        SplitPlacesInVorReaktor(c, get_card_arcs_out);
    }
    cmax = CardSpecification[TR];
    for (c = 0; c < cmax; c++) {
        SplitTransitionsInVorReaktor(c, get_card_arcs_out);
    }
    // Separierung nach Vielfachheiten der i-ten Kanten - nie falsch
    this_direction = DO;
    unsigned int bmax = Globals::Places[Globals::Places[0]->cnt - 1]->NrOfLeaving;
    for (b = 0; b < bmax; b++) {
        this_arc_nr = b;
        cmax = CardSpecification[PL];
        for (c = 0; c < cmax; c++) {
            if (Globals::Places[Specification[PL][c].first]->NrOfLeaving > b) {
                SplitPlacesInVorReaktor(c, get_arc_mult);
            }
        }
    }
    this_direction = CO;
    bmax = Globals::Places[Globals::Places[0]->cnt - 1]->NrOfArriving;
    for (b = 0; b < bmax; b++) {
        this_arc_nr = b;
        cmax = CardSpecification[PL];
        for (c = 0; c < cmax; c++) {
            if (Globals::Places[Specification[PL][c].first]->NrOfArriving > b) {
                SplitPlacesInVorReaktor(c, get_arc_mult);
            }
        }
    }

    this_direction = DO;
    bmax = Globals::Transitions[Globals::Transitions[0]->cnt - 1]->NrOfLeaving;
    for (b = 0; b < bmax; b++) {
        this_arc_nr = b;
        cmax = CardSpecification[TR];
        for (c = 0; c < cmax; c++) {
            if (Globals::Transitions[Specification[TR][c].first]->NrOfLeaving > b) {
                SplitTransitionsInVorReaktor(c, get_arc_mult);
            }
        }
    }
    this_direction = CO;
    bmax = Globals::Transitions[Globals::Transitions[0]->cnt - 1]->NrOfArriving;
    for (b = 0; b < bmax; b++) {
        this_arc_nr = b;
        cmax = CardSpecification[TR];
        for (c = 0; c < cmax; c++) {
            if (Globals::Transitions[Specification[TR][c].first]->NrOfArriving > b) {
                SplitTransitionsInVorReaktor(c, get_arc_mult);
            }
        }
    }
    // Separierung nach Markenzahl der Anfangsmarkierung - so bildet m0
    // eigene Symmetrieklasse - notwendig fuer akkurate Analyse
    cmax = CardSpecification[PL];
    for (c = 0; c < cmax; c++) {
        SplitPlacesInVorReaktor(c, get_marking);
    }
    // according to target marking, if present
    cmax = CardSpecification[PL];
    for (c = 0; c < cmax; c++) {
        SplitPlacesInVorReaktor(c, get_target_marking);
    }
    // let target place be a fixed point, if it exists
    cmax = CardSpecification[PL];
    for (c = 0; c < cmax; c++) {
        SplitPlacesInVorReaktor(c, get_target_place);
    }
    // let target transition be fixed point, if exists
    cmax = CardSpecification[TR];
    for (c = 0; c < cmax; c++) {
        SplitTransitionsInVorReaktor(c, get_target_transition);
    }
#ifdef CYCLE
    // cycle detection transitions must be completed w.r.t transition
    // equivalence. Doing it here seems to be a good approximation.
    for (c = 0; c < CardSpecification[TR]; c++) {
        for (unsigned int i = Specification[TR][c].first; i <= Specification[TR][c].last; i++) {
            if (Globals::Transitions[i]->cyclic) {
                for (b = Specification[TR][c].first; b <= Specification[TR][c].last; b++) {
                    Globals::Transitions[b] -> cyclic = true;
                }
                break;
            }
        }
    }
#endif
}

/*!
 Kopiere Resultat von InitialConstraint in Reaktor, da ab jetzt linke und
 rechte Seiten der geordneten Paare abweichen koennen.
*/
void FuelleReaktor() {
    for (unsigned int i = 0; i < Globals::Places[0]->cnt; i++) {
        Globals::Places[i]->pos[DO] = Globals::Places[i]->pos[CO] = i;
    }
    for (unsigned int i = 0; i < Globals::Transitions[i]->cnt; i++) {
        Globals::Transitions[i]-> pos[DO] = Globals::Transitions[i]->pos[CO] = i;
    }
    for (unsigned int c = 0; c < CardSpecification[PL]; c++) {
        for (unsigned int i = Specification[PL][c].first; i <= Specification[PL][c].last; i++) {
            Reaktor[PL][DO][i].node = Reaktor[PL][CO][i].node = Globals::Places[i];
        }
        Specification[PL][c].changed = new ToDo(PL, c);
    }
    for (unsigned int c = 0; c < CardSpecification[TR]; c++) {
        for (unsigned int i = Specification[TR][c].first; i <= Specification[TR][c].last; i++) {
            Reaktor[TR][DO][i].node = Reaktor[TR][CO][i].node = Globals::Transitions[i];
        }
        Specification[TR][c].changed = new ToDo(TR, c);
    }
}

// Aufbewahrung der berechneten Generatoren
SymmStore* Store;
unsigned int  CurrentStore;
unsigned int CardStore;

// On-the-fly-recording der Aequivalenzklassen von Knoten
Partition* part;

// Die Aeq-Klassen werden mit dem Union/Find-Alg von Tarjan verwaltet.
void UnifyClasses(unsigned int e1, unsigned int e2) { // union von union-find
    unsigned int c1, c2, c, e;

    // Bestimme Klasse c1 von Element e1
    for (c1 = e1; !(part[c1].top); c1 = part[c1].nextorcard) {
        ;
    }
    // Bestimme Klasse c2 von Element e2
    for (c2 = e2; !(part[c2].top); c2 = part[c2].nextorcard) {
        ;
    }
    // Falls Klassen noch verschieden...
    if (c1 != c2) {
        // haenge kleinere an groessere
        if (part[c1].nextorcard > part[c2].nextorcard) {
            part[c1].nextorcard += part[c2].nextorcard;
            part[c2].nextorcard = c1;
            part[c2].top = false;
            c = c1;
        } else {
            part[c2].nextorcard += part[c1].nextorcard;
            part[c1].nextorcard = c2;
            part[c1].top = false;
            c = c2;
        }
    } else {
        c = c1;
    }
    // Pfadkomprimierung, um Komplexitaet nlog*n zu erreichen
    while (e1 != c) {
        e = part[e1].nextorcard;
        part[e1].nextorcard = c;
        e1 = e;
    }
    while (e2 != c) {
        e = part[e2].nextorcard;
        part[e2].nextorcard = c;
        e2 = e;
    }
}

// Verwaltung des Rundenstempels, der fortwaehrendes Ruecksetzen der
// Zaehlergebnisse im Reaktor erspart

unsigned int Stamp;

void NewStamp() {
    if (Stamp == UINT_MAX) {
        for (NodeType n = 0; n < 2; n++)
            for (DomType d = 0; d < 2; d++)
                for (unsigned int i = 0; i < (n ? Globals::Transitions[0]->cnt : Globals::Places[0]->cnt); i++) {
                    Reaktor[n][d][i].stamp = 0;
                }
        Stamp = 1;
    } else {
        Stamp++;
    }
}

// Sortieren von Reaktoreintraegen nach Zaehlergebnissen = erster Schritt
// zur Separierung von Constraints waehrend Refine

void CountSort(NodeType n, DomType d, unsigned int from, unsigned int to) {
    unsigned int less, current, greater;
    Reaktoreintrag swap;

    less = from;
    current = from + 1;
    greater = to;

    if (from == to) {
        if (Reaktor[n][d][from].stamp != Stamp) {
            Reaktor[n][d][from].count = 0;
        }
        return;
    }
    while (current <= greater) {
        if (Reaktor[n][d][current].stamp != Stamp) {
            Reaktor[n][d][current].count = 0;
            if (Reaktor[n][d][current - 1].stamp != Stamp) {
                Reaktor[n][d][current++ -1].count = 0;
            } else {
                Reaktor[n][d][less].node->pos[d] = current;
                Reaktor[n][d][current].node->pos[d] = less;
                swap = Reaktor[n][d][current];
                Reaktor[n][d][current++] = Reaktor[n][d][less];
                Reaktor[n][d][less++] = swap;
            }
        } else {
            if (Reaktor[n][d][current - 1].stamp != Stamp) {
                Reaktor[n][d][current - 1].count = 0;
                Reaktor[n][d][greater].node->pos[d] = current;
                Reaktor[n][d][current].node -> pos[d] = greater;
                swap = Reaktor[n][d][current];
                Reaktor[n][d][current] = Reaktor[n][d][greater];
                Reaktor[n][d][greater--] = swap;

            } else {
                if (Reaktor[n][d][current - 1].count < Reaktor[n][d][current].count) {
                    Reaktor[n][d][greater].node->pos[d] = current;
                    Reaktor[n][d][current].node -> pos[d] = greater;
                    swap = Reaktor[n][d][current];
                    Reaktor[n][d][current] = Reaktor[n][d][greater];
                    Reaktor[n][d][greater--] = swap;
                } else {
                    if (Reaktor[n][d][current - 1].count == Reaktor[n][d][current].count) {
                        current++;
                    } else {
                        Reaktor[n][d][less].node->pos[d] = current;
                        Reaktor[n][d][current].node->pos[d] = less;
                        swap = Reaktor[n][d][current];
                        Reaktor[n][d][current++] = Reaktor[n][d][less];
                        Reaktor[n][d][less++] = swap;
                    }
                }
            }
        }
    }
    if (less - from > 1) {
        CountSort(n, d, from, less - 1);
    }
    if (to - greater > 1) {
        CountSort(n, d, greater + 1, to);
    }
}


// Sortiert den Vektor der moeglichen Bilder fuer einen Knoten,
// wird in OnlineCanonize benutzt

void ImageSort(Node** vector, unsigned int from, unsigned int to) {
    unsigned int less, current, greater;
    Node* swap;

    less = from;
    current = from + 1;
    greater = to;

    while (current <= greater) {
        if (Globals::CurrentMarking[((Place*) vector[current])->index] < Globals::CurrentMarking[((Place*) vector[current - 1])->index]) {
            swap = vector[current];
            vector[current++] = vector[less];
            vector[less++] = swap;
        } else {
            if (Globals::CurrentMarking[((Place*) vector[current])->index] == Globals::CurrentMarking[((Place*) vector[current - 1])->index]) {
                current++;
            } else {
                swap = vector[greater];
                vector[greater --] = vector[current];
                vector[current] = swap;
            }
        }
    }
    if (less - from > 1) {
        ImageSort(vector, from, less - 1);
    }
    if (to - greater > 1) {
        ImageSort(vector, greater + 1, to);
    }
}

// Nachdem fuer ein Constraint und eine Vielfachheit die eingehenden
// oder ausgehenden Boegen zu allen anderen Constraints gezaehlt sind,
// muessen die anderen Constraints in kleinere zerlegt werden
// n: PL oder TR, c: Index des zu splittenden Constraints in Specification[n]

bool Split(NodeType n, unsigned int c) {
    unsigned int i, firstc, lastc, oldc, newc;

    oldc = c;
    firstc = Specification[n][c].first;
    lastc = Specification[n][c].last;
    // Precondition: Die Reaktoreintraege sind bzgl count sortiert
    while ((Reaktor[n][DO][firstc].count != Reaktor[n][DO][lastc].count)
            ||
            (Reaktor[n][CO][firstc].count != Reaktor[n][CO][lastc].count)) {
        for (i = firstc + 1; (Reaktor[n][DO][firstc].count == Reaktor[n][DO][i].count)
                && (Reaktor[n][CO][firstc].count == Reaktor[n][CO][i].count); i++) {
            ;
        }
        // Wenn fuer ein  entstehendes Constraint [A,B] |A| != |B| ist,
        // koennen keine Symms konsistent sein. Hier: entweder |A| = 0
        // oder |B| = 0.
        if (Reaktor[n][DO][firstc].count != Reaktor[n][CO][firstc].count) {
            DeadBranches++;
            return false;
        }
        newc = CardSpecification[n]++;
        reportprogress();
#ifdef DISTRIBUTE
        progress();
#endif
        //Zerschneiden des Constraints in zwei Teile
        Specification[n][oldc].last = i - 1;
        Specification[n][newc].first = i;
        Specification[n][newc].last = lastc;
        Specification[n][newc].parent = c;
        firstc = i;
        oldc = newc;
        // Fuer Schnittprodukte sollte Refine nochmals probiert werden
        Specification[n][newc].changed = new ToDo(n, newc);
    }
    if (oldc != c) {
        Specification[n][c].changed = new ToDo(n, c);
    } else {
        if (Reaktor[n][DO][firstc].count != Reaktor[n][CO][firstc].count) {
            DeadBranches++;
            return false;
        }
    }
    return true;
}

// Testausgabe der Spec
void PrintSpec(char* mess) {
    unsigned int n, c, i;

    cout << "***** " << mess << " *****" << endl;
    for (n = 0; n < 2; n++) {
        cout << (n ? "Globals::Transitions\n" : "Globals::Places\n");
        for (c = 0; c < CardSpecification[n]; c++) {
            cout << c << "(" << Specification[n][c].first << "-" << Specification[n][c].last << ": {";
            for (i = Specification[n][c].first; i <= Specification[n][c].last; i++) {
                cout << Reaktor[n][DO][i].node->name;
                //cout << "%";
                //cout << Reaktor[n][DO][i].count ;
                //cout << "&" << Reaktor[n][DO][i].stamp ;
                cout << "," ;
            }
            cout << "} --> {";
            for (i = Specification[n][c].first; i <= Specification[n][c].last; i++) {
                cout << Reaktor[n][CO][i].node->name;
                //cout << "/";
                //cout << Reaktor[n][CO][i].count;
                //cout << "&" << Reaktor[n][CO][i].stamp;
                cout << "," ;
            }
            cout << "}\n";


        }
    }
}

/*!
 Verfeinere Abstrakte Symmetrie asgehend von Constraint ref
*/
bool Refine(NodeType n, unsigned int ref) {
    unsigned int i, j, c;

    // das andere Ende eines Bogens hat immer den dualen Knotentyp
    unsigned int othern = n ? PL : TR;
    // splitte sowohl mit eingehenden als auch ausgehenden Kanten
    for (unsigned int arcdir = 0; arcdir < 2; arcdir++) {
        // unused: unsigned int otherarcdir = 1 - arcdir;
        // Anzahl der Kanten (wegen Praepozessing gleich fuer alle Knoten im
        // Constraint)
        unsigned int currentcardarc = arcdir
                                      ? Reaktor[n][DO][Specification[n][ref].first].node->NrOfArriving
                                      : Reaktor[n][DO][Specification[n][ref].first].node->NrOfLeaving;
        // Pro Durchlauf wird jeweils der (wg Praeprozessing zusammenhaengende)
        // Block von Kanten gleicher Vielfachheit abgearbeitet
        // Deshalb Reinitialisierung ausserhalb des Schleifenkopfes
        for (j = 0; j < currentcardarc;) {
            // Neue Zaehlrunde. Damit werden alle Reaktor.count-Werte
            // ungueltig
            NewStamp();
            unsigned int m1, m2;
            do {
                for (i = Specification[n][ref].first; i <= Specification[n][ref].last; i++) {
                    Reaktoreintrag* r;
                    for (unsigned int dir = 0; dir < 2; dir++) {
                        r = arcdir
                            ? &Reaktor[othern][dir][Reaktor[n][dir][i].node->ArrivingArcs[j]->Source->pos[dir]]
                            : &Reaktor[othern][dir][Reaktor[n][dir][i].node->LeavingArcs[j]->Destination->pos[dir]];
                        if (r->stamp == Stamp) {
                            r->count++;
                        } else {
                            r -> stamp = Stamp;
                            r -> count = 1;
                        }
                    }
                }
                // Nachgeholte Reinitialisierung: Teste auf Ende eines
                // zusammenhaengenden Bereiches gleicher Vielfachheit in
                // Bogenliste (wg Praeprozessing fuer alle Knoten in einem
                // Constraint einheitlich)
                m1 = arcdir
                     ? Reaktor[n][DO][Specification[n][ref].first].node->ArrivingArcs[j]-> Multiplicity
                     : Reaktor[n][DO][Specification[n][ref].first].node->LeavingArcs[j]-> Multiplicity;
                j++;
                if (j < currentcardarc) {
                    m2 = arcdir
                         ? Reaktor[n][DO][Specification[n][ref].first].node->ArrivingArcs[j]-> Multiplicity
                         : Reaktor[n][DO][Specification[n][ref].first].node->LeavingArcs[j]-> Multiplicity;
                } else {
                    m2 = m1 + 1;
                }
            } while (m1 == m2);
            // cmax: Alle Constraints in Specification mit index < cmax
            // gab es schon vor dem Splitten Neue bekommen indizes >= cmax
            unsigned int cmax = CardSpecification[othern];
            // Das Separieren von Constraints auf der Basis der Zaehlergebnisse
            for (c = 0; c < cmax; c++) {
                CountSort(othern, DO, Specification[othern][c].first, Specification[othern][c].last);
                CountSort(othern, CO, Specification[othern][c].first, Specification[othern][c].last);
                if (!Split(othern, c)) {
                    return false;
                }
            }
        }
    }
    return true;
}

/*!
 Wiederholt Refine solnge bis alle ToDo abgearbeitet sind
*/
bool RefineUntilNothingChanges(NodeType n) {
    do {
        do {
            if (!Refine(n, ToDoList[n]->constraint)) {
                for (n = PL; n < 2; n++) {
                    while (ToDoList[n]) {
                        Specification[n][ToDoList[n]->constraint].changed = NULL;
                        ToDo* tmp = ToDoList[n];
                        ToDoList[n] = ToDoList[n]-> next;
                        delete tmp;
                    }
                }
                return false;
            }
            Specification[n][ToDoList[n]->constraint].changed = NULL;
            ToDo* tmp = ToDoList[n];
            ToDoList[n] = ToDoList[n]-> next;
            delete tmp;
        } while (ToDoList[n]);
        n = 1 - n;
    } while (ToDoList[n]);
    return true;
}

// Beim Backtracking von einem Split muessen Constraints wieder
// zusammengefuegt werden. Wir verlassen uns darauf, dass die Teilcpnstraints
// in ihrem Reaktor noch die gleichen Index-Bereiche einnehmen. Das ist so,
// weil Splitten im Reaktor nur am Ort sortiert,
// d.h. ggf. INNERHALB des Indexbereiches
// Knoten permutiert
// Also reicht es, first und last zu aktualisieren und den Constraint-Eintrag
// des abgeforkten Constraints zu loeschen. Dieser in der letzte in
// Specification, da Specification sich stack-artig enwickelt

/*!
  \param plpegel, trpegel: alle Constraints > = pegel werden mit ihren Eltern
                           wiedervereinigt
*/
void ReUnify(unsigned int plpegel, unsigned int trpegel) {
    for (unsigned int c = CardSpecification[PL] - 1; c >= plpegel; c--) {
        if (Specification[PL][c].first < Specification[PL][Specification[PL][c].parent].first) {
            Specification[PL][Specification[PL][c].parent].first = Specification[PL][c].first;
        }
        if (Specification[PL][c].last > Specification[PL][Specification[PL][c].parent].last) {
            Specification[PL][Specification[PL][c].parent].last = Specification[PL][c].last;
        }
    }
    CardSpecification[PL] = plpegel;
    reportprogress();
#ifdef DISTRIBUTE
    progress();
#endif
    for (unsigned int c = CardSpecification[TR] - 1; c >= trpegel; c--) {
        if (Specification[TR][c].first < Specification[TR][Specification[TR][c].parent].first) {
            Specification[TR][Specification[TR][c].parent].first = Specification[TR][c].first;
        }
        if (Specification[TR][c].last > Specification[TR][Specification[TR][c].parent].last) {
            Specification[TR][Specification[TR][c].parent].last = Specification[TR][c].last;
        }
    }
    CardSpecification[TR] = trpegel;
    reportprogress();
#ifdef DISTRIBUTE
    progress();
#endif
}

#if defined(SYMMPROD)
unsigned int gcd(unsigned int n, unsigned int m) {
    return m == 0 ? n : gcd(m, n % m);
}

void MinimizeCarrier(unsigned int pos) {
    unsigned int orbit(Store[CurrentStore].image[pos].value->nr); // number of the orbit
    unsigned int level(Store[CurrentStore].argnr); // offset for permutation vector
    unsigned int ocyclen(1); // length of the orbit cycle (containing the orbit number)
    unsigned int* ovec(Store[CurrentStore].image[pos].vector); // permutation vector
    bool* visited = new bool[Globals::Places[0]->cnt - level + 1]; // to mark visited cycles
    unsigned int* cycle = new unsigned int[Globals::Places[0]->cnt - level + 1]; // temporary memory for a cycle
    for (unsigned int j = 0; j < Globals::Places[0]->cnt - level; ++j) {
        visited[j] = false;
    }
    unsigned int i(orbit);
    visited[i - level] = true;

    while ((i = ovec[i - level]) != orbit) {
        ++ocyclen;
        visited[i - level] = true;
    } // calculate the orbit cycle length

    for (unsigned int element = 0; element < Globals::Places[0]->cnt - level; ++element) { // go through the possible elements of non-trivial cycles
        if (visited[element]) {
            continue;    // don't do this element again
        }
        unsigned int ecyclen(1); // length of the element's cycle
        visited[element] = true;
        cycle[0] = i = element + level; // start saving this cycle
        while ((i = ovec[i - level]) != element + level) {
            cycle[ecyclen++] = i;
            visited[i - level] = true;
        } // calculate the element's cycle length and save the cycle
        if (ecyclen == 1) {
            continue;    // an identity; nothing to do
        }
        unsigned int ecycles(ecyclen); // the number of cycles the element's cycle will be split into
        while ((i = gcd(ecycles, ocyclen)) > 1) {
            ecycles /= i;    // remove prime factors of ocyclen from ecycles
        }
        if (ecycles > 1) // the ecycle can be split
            for (i = 0; i < ecyclen; ++i) { // now store the replacement cycles
                ovec[cycle[i] - level] = cycle[(i + ecycles) % ecyclen];
            }
    }

    delete[] visited;
    delete[] cycle;
}
#endif

// Eintragen einer gefundenen Symmetrie in den Speicher fuer das
// Erzeugendensystem

void StoreSymmetry(unsigned int pos) {
    unsigned int offset, c, *v;
    // unused: unsigned int i;
    // offset: Wir speichern nur den Teil der Symm, von dem nicht klar ist dass
    // dass er per Konstruktion sowieso id sein muss
    offset = Store[CurrentStore].arg->nr;
    v = Store[CurrentStore].image[pos].vector = new unsigned int [Globals::Places[0]->cnt - offset];
#if defined(SYMMPROD)
    for (c = 0; c < CardSpecification[PL]; ++c) {
        if (Reaktor[PL][DO][Specification[PL][c].first].node->nr >= offset) {
            v[Reaktor[PL][DO][Specification[PL][c].first].node->nr - offset] =
                Reaktor[PL][CO][Specification[PL][c].first].node->nr;
        }
    }
    MinimizeCarrier(pos);
    for (c = 0; c < Globals::Places[0]->cnt - offset; ++c) {
        UnifyClasses(c + offset, v[c]);
    }
#else
    for (c = 0; c < CardSpecification[PL]; c++) {
        // Vektor eintragen
        if (Reaktor[PL][DO][Specification[PL][c].first].node->nr >= offset) {
            v[Reaktor[PL][DO][Specification[PL][c].first].node->nr - offset] =
                Reaktor[PL][CO][Specification[PL][c].first].node->nr;
            // Klassen der Knotenpartition aktualisieren
            UnifyClasses(Reaktor[PL][DO][Specification[PL][c].first].node->nr, Reaktor[PL][CO][Specification[PL][c].first].node->nr);
        }
    }
#endif
}

// Alle Syms in Zyklenschreibweise ausgeben
void WriteSymms() {
    if (Globals::Yflg) {
        unsigned int etage, raum, x, y;
        // aufsteigende Etagen = aufsteigend erzwungene id (Struktur des
        // Erzeugendensystems)
        for (etage = 0; etage < CardStore; etage++) {
            // Raum: Ein Generator der aktuellen Etage
            for (raum = 0; raum < Store[etage].card; raum++) {
                cout << "GENERATOR # " << etage + 1 << "." << raum + 1 << "\n";
                for (x = Store[etage].argnr; x < Globals::Places[0]->cnt; x++) {
                    // write cycle of x iff x is smallest el. of its cycle

                    // 1. find out whether x is smallest cycle member
                    for (y = Store[etage].image[raum].vector[x - Store[etage].argnr]; y > x ; y = Store[etage].image[raum].vector[y - Store[etage].argnr]) {
                        ;
                    }
                    if ((y == x) && (Store[etage].image[raum].vector[x - Store[etage].argnr] != x)) {
                        // print cycle of x
                        cout << "(";
                        cout << Globals::Places[x] -> name;
                        for (y = Store[etage].image[raum].vector[x - Store[etage].argnr]; y > x ; y = Store[etage].image[raum].vector[y - Store[etage].argnr]) {
                            cout << " " << Globals::Places[y] -> name;
                        }
                        cout << ")\n";
                    }
                }
                cout << "\n";
            }
        }
    }
    if (Globals::yflg) {
        ofstream symmstream(Globals::symmfile);
        if (!symmstream) {
            abort(4, "cannot open symmetry output file '%s' - no output written", _cfilename_(Globals::symmfile));
        }
        unsigned int etage, raum, x, y;
        for (etage = 0; etage < CardStore; etage++) {
            for (raum = 0; raum < Store[etage].card; raum++) {
                symmstream << "GENERATOR # " << etage + 1 << "." << raum + 1 << "\n";
                for (x = Store[etage].argnr; x < Globals::Places[0]->cnt; x++) {
                    // write cycle of x iff x is smallest el. of its cycle

                    // 1. find out whether x is smallest cycle member
                    for (y = Store[etage].image[raum].vector[x - Store[etage].argnr]; y > x ; y = Store[etage].image[raum].vector[y - Store[etage].argnr]) {
                        ;
                    }
                    if ((y == x) && (Store[etage].image[raum].vector[x - Store[etage].argnr] != x)) {
                        // print cycle of x
                        symmstream << "(";
                        symmstream << Globals::Places[x] -> name;
                        for (y = Store[etage].image[raum].vector[x - Store[etage].argnr]; y > x ; y = Store[etage].image[raum].vector[y - Store[etage].argnr]) {
                            symmstream << " " << Globals::Places[y] -> name;
                        }
                        symmstream << ")\n";
                    }
                }
                symmstream << "\n";
            }
        }
    }
}


// Define = mutwilliges Spalten von [A,B] in [{x},{y}] und [A\{x},B\{y}].
// Fuer ein gegebenes x aus A werden dabei alle y aus B durchprobiert.
// Wir unterscheiden den Fall A=B und x=y (--> DefineToId) von allen
// anderen Faellen (--> DefineToOther) wegen unterschiedlichem
// Backtrackingverhalten. Aus DefineToOther kann sofort komplett
// ausgestiegen werden sobald eine Symm gefunden wurde. In DefineToId
// muss backgetrackt werden, um fehlende Slots des Erzeugendensystems
// zu fuellen
// Define findet grundsaetzlich auf Place-Constraints statt, weil eine
// konsistente, auf den Plaetzen einelementige abstrakte Symm sich immer
// auch auf den Transitionen fortsetzen laesst

/*!
 \param imagepos: die Stelle wo ggf eine gefundene Symm in Store einzutragen ist
*/
void DefineToOther(unsigned int imagepos) {
    // possibleImages: die in Frage kommenden y, also alle Elemente aus B
    Node** possibleImages;
    unsigned int cntriv, i, j;
    // unused: unused int intriv, k;
    // unused: NodeType type;
    Reaktoreintrag swap;
    // unused: DomType dir;
    unsigned int MyCardSpecification[2];

    // suche mehrelementiges Platzconstraint, Index -> cntriv
    for (cntriv = 0; Specification[PL][cntriv].first == Specification[PL][cntriv].last; cntriv++) {
        ;
    }
    // speichere B in possibleImages
    possibleImages = new Node * [Specification[PL][cntriv].last - Specification[PL][cntriv].first + 2];
    for (i = Specification[PL][cntriv].first; i <= Specification[PL][cntriv].last; i++) {
        possibleImages[i - Specification[PL][cntriv].first] = Reaktor[PL][CO][i].node;
    }
    possibleImages[i - Specification[PL][cntriv].first] = NULL;

    // Spalte [{x},{y}] fuer erstes y
    Specification[PL][CardSpecification[PL]].first = Specification[PL][CardSpecification[PL]].last
                                                     = Specification[PL][cntriv].first;
    Specification[PL][CardSpecification[PL]].parent = cntriv;
    Specification[PL][CardSpecification[PL]].changed = new ToDo(PL, CardSpecification[PL]);
    CardSpecification[PL]++;
    reportprogress();
#ifdef DISTRIBUTE
    progress();
#endif
    Specification[PL][cntriv].first++;
    Specification[PL][cntriv].changed = new ToDo(PL, cntriv);
    // Pegelstand in Specification merken wegen spaeterem Backtracking
    MyCardSpecification[PL] = CardSpecification[PL];
    MyCardSpecification[TR] = CardSpecification[TR];
    // probiere alle y aus B (erstes ist schon geladen)
    for (j = 0; possibleImages[j]; j++) {
        if (RefineUntilNothingChanges(PL)) {
            if (CardSpecification[PL] == Globals::Places[0]->cnt) {
                StoreSymmetry(imagepos);
                break;
            } else {
                DefineToOther(imagepos);
                if (Store[CurrentStore].image[imagepos].vector) {
                    break;
                }
            }
        }
        // Reinitialisierung dadurch, dass im Constraint [{x},{y}] das
        // alte y einfach gegen den naechsten Wert aus possibleImages
        // getauscht wird
        if (possibleImages[j + 1]) {
            ReUnify(MyCardSpecification[PL], MyCardSpecification[TR]);
            i = possibleImages[j + 1]->pos[CO];
            Reaktor[PL][CO][i].node->pos[CO] =
                Specification[PL][CardSpecification[PL] - 1].first;
            Reaktor[PL][CO][Specification[PL][CardSpecification[PL] - 1].first].node->pos[CO] = i;
            swap = Reaktor[PL][CO][i];
            Reaktor[PL][CO][i] = Reaktor[PL][CO][Specification[PL][CardSpecification[PL] - 1].first];
            Reaktor[PL][CO][Specification[PL][CardSpecification[PL] - 1].first] = swap;
            Specification[PL][CardSpecification[PL] - 1].changed = new ToDo(PL, CardSpecification[PL] - 1);
            Specification[PL][cntriv].changed = new ToDo(PL, cntriv);

        }
    }
    delete possibleImages;
}


bool found;

// Ein defineToOther fuer den Fall, dass kein Erzeugendensystem berechnet wird
// sondern bei der State-Space-Exploration Symmetrien direkt berechnet werden
// Einziger Unterschied: kein Eintragen einer gefundenen Symm, alos auch kein
// imagepos: Stattdessen Setzen von found

void OnlineDefineToOther() {
    Node** possibleImages;
    unsigned int cntriv, i, j;
    // unused: unsigned int intriv, k;
    // unused: NodeType type;
    Reaktoreintrag swap;
    // unused: DomType dir;
    unsigned int MyCardSpecification[2];

    for (cntriv = 0; Specification[PL][cntriv].first == Specification[PL][cntriv].last; cntriv++) {
        ;
    }
    possibleImages = new Node * [Specification[PL][cntriv].last - Specification[PL][cntriv].first + 2];
    for (i = Specification[PL][cntriv].first; i <= Specification[PL][cntriv].last; i++) {
        possibleImages[i - Specification[PL][cntriv].first] = Reaktor[PL][CO][i].node;
    }
    possibleImages[i - Specification[PL][cntriv].first] = NULL;
    Specification[PL][CardSpecification[PL]].first = Specification[PL][CardSpecification[PL]].last
                                                     = Specification[PL][cntriv].first;
    Specification[PL][CardSpecification[PL]].parent = cntriv;
    Specification[PL][CardSpecification[PL]].changed = new ToDo(PL, CardSpecification[PL]);
    CardSpecification[PL]++;
    reportprogress();
#ifdef DISTRIBUTE
    progress();
#endif
    Specification[PL][cntriv].first++;
    Specification[PL][cntriv].changed = new ToDo(PL, cntriv);
    MyCardSpecification[PL] = CardSpecification[PL];
    MyCardSpecification[TR] = CardSpecification[TR];
    for (j = 0; possibleImages[j]; j++) {
        if (RefineUntilNothingChanges(PL)) {
            if (CardSpecification[PL] == Globals::Places[0]->cnt) {
                found = true;
                break;
            } else {
                OnlineDefineToOther();
                if (found) {
                    break;
                }
            }
        }
        ReUnify(MyCardSpecification[PL], MyCardSpecification[TR]);
        if (possibleImages[j + 1]) {
            i = possibleImages[j + 1]->pos[CO];
            Reaktor[PL][CO][i].node->pos[CO] =
                Specification[PL][CardSpecification[PL] - 1].first;
            Reaktor[PL][CO][Specification[PL][CardSpecification[PL] - 1].first].node->pos[CO] = i;
            swap = Reaktor[PL][CO][i];
            Reaktor[PL][CO][i] = Reaktor[PL][CO][Specification[PL][CardSpecification[PL] - 1].first];
            Reaktor[PL][CO][Specification[PL][CardSpecification[PL] - 1].first] = swap;
            Specification[PL][cntriv].changed = new ToDo(PL, cntriv);
            Specification[PL][CardSpecification[PL] - 1].changed = new ToDo(PL, CardSpecification[PL]);
        }
    }
    delete possibleImages;
}

// Approximative Berechnung eines kanonischn Repraesentanten einer
// Markierung ohne vorherige Berechnung eines Erzeugendensystems

// Anzahl der Versuche: Gross = Viel Laufzeit, akkurateres Ergebnis
// Kann exponentiell gross werden, deshalb Beschraenkung in userconfig setzbar
unsigned int Attempt;

void OnlineCanonize() {
    unsigned int* swaprep;
    Node** possibleImages;
    unsigned int cn, cntriv, i, j;
    Reaktoreintrag swap;
    unsigned int MyCardSpecification[2];
    unsigned int SourceIndex;
    unsigned int maxmarking, checkmarking;

    // get non-trivial class
    SourceIndex = CardSpecification[PL] - 1;
    for (cn = 0; cn < CardSpecification[PL]; cn ++) {
        if (Specification[PL][cn].first == Specification[PL][cn].last) {
            continue;
        }
        for (i = Specification[PL][cn].first; i <= Specification[PL][cn].last; i++) {
            if (Reaktor[PL][DO][i].node -> nr < Reaktor[PL][DO][SourceIndex].node -> nr) {
                SourceIndex = i;
                cntriv = cn;
            }
        }
    }
    // get all images
    possibleImages = new Node * [Specification[PL][cntriv].last - Specification[PL][cntriv].first + 2];
    for (i = Specification[PL][cntriv].first; i <= Specification[PL][cntriv].last; i++) {
        possibleImages[i - Specification[PL][cntriv].first] = Reaktor[PL][CO][i].node;
    }
    possibleImages[i - Specification[PL][cntriv].first] = NULL;
    ImageSort(possibleImages, 0, Specification[PL][cntriv].last - Specification[PL][cntriv].first);
    maxmarking = Globals::CurrentMarking[((Place*) possibleImages[Specification[PL][cntriv].last - Specification[PL][cntriv].first])->index];
    Reaktor[PL][DO][SourceIndex].node -> pos[DO] = Specification[PL][cntriv].first;
    Reaktor[PL][DO][Specification[PL][cntriv].first].node -> pos[DO] = SourceIndex;
    swap = Reaktor[PL][DO][SourceIndex];
    Reaktor[PL][DO][SourceIndex] = Reaktor[PL][DO][Specification[PL][cntriv].first];
    Reaktor[PL][DO][Specification[PL][cntriv].first] = swap;


    i = possibleImages[0]->pos[CO];
    Reaktor[PL][CO][i].node -> pos[CO] = Specification[PL][cntriv].first;
    Reaktor[PL][CO][Specification[PL][cntriv].first].node -> pos[CO] = i;
    swap = Reaktor[PL][CO][i];
    Reaktor[PL][CO][i] = Reaktor[PL][CO][Specification[PL][cntriv].first];
    Reaktor[PL][CO][Specification[PL][cntriv].first] = swap;

    // split new class
    Specification[PL][CardSpecification[PL]].first = Specification[PL][CardSpecification[PL]].last
                                                     = Specification[PL][cntriv].first;
    Specification[PL][CardSpecification[PL]].parent = cntriv;
    Specification[PL][CardSpecification[PL]].changed = new ToDo(PL, CardSpecification[PL]);
    CardSpecification[PL]++;
    reportprogress();
#ifdef DISTRIBUTE
    progress();
#endif
    Specification[PL][cntriv].first++;
    Specification[PL][cntriv].changed = new ToDo(PL, cntriv);
    MyCardSpecification[PL] = CardSpecification[PL];
    MyCardSpecification[TR] = CardSpecification[TR];
    for (j = 0; possibleImages[j]; j++) {
        checkmarking = Globals::CurrentMarking[((Place*) possibleImages[j])->index];
        if (checkmarking > maxmarking) {
            break;
        }
        NewStamp();
        if (RefineUntilNothingChanges(PL)) {
            if (CardSpecification[PL] == Globals::Places[0]->cnt) {
                Attempt++;
                found = true;
                maxmarking = checkmarking;
                for (i = 0; i < Globals::Places[0]->cnt; i++) {
                    kanrep1[Reaktor[PL][DO][i].node -> nr] =
                        Globals::CurrentMarking[((Place*) Reaktor[PL][CO][i].node)->index];
                }
                for (i = 0; i < Globals::Places[0]->cnt; i++) {
                    if (kanrep[i] != kanrep1[i]) {
                        break;
                    }
                }
                if (i < Globals::Places[0]->cnt && kanrep1[i] < kanrep[i]) {
                    swaprep = kanrep;
                    kanrep = kanrep1;
                    kanrep1 = swaprep;
                }

#ifdef APPROXIMATE_CANONIZATION
                if (Attempt >= MAXATTEMPT) {
                    break;
                }
#endif
            } else {
                OnlineCanonize();
#ifdef APPROXIMATE_CANONIZATION
                if (found && Attempt >= MAXATTEMPT) {
                    break;
                }
#endif
            }
        }
        ReUnify(MyCardSpecification[PL], MyCardSpecification[TR]);
        if (possibleImages[j + 1]) {
            i = possibleImages[j + 1]->pos[CO];
            Reaktor[PL][CO][i].node->pos[CO] =
                Specification[PL][CardSpecification[PL] - 1].first;
            Reaktor[PL][CO][Specification[PL][CardSpecification[PL] - 1].first].node->pos[CO] = i;
            swap = Reaktor[PL][CO][i];
            Reaktor[PL][CO][i] = Reaktor[PL][CO][Specification[PL][CardSpecification[PL] - 1].first];
            Reaktor[PL][CO][Specification[PL][CardSpecification[PL] - 1].first] = swap;
            Specification[PL][cntriv].changed = new ToDo(PL, cntriv);
            Specification[PL][CardSpecification[PL] - 1].changed = new ToDo(PL, CardSpecification[PL] - 1);
        }
    }
    delete possibleImages;
    ReUnify(MyCardSpecification[PL] - 1, MyCardSpecification[TR]);
}


#if defined(SYMMPROD)
void BuildInconsistencies(unsigned int orbit, bool* inconsistent) {
    if (!Store[CurrentStore].image[orbit].vector) {
        unsigned int k, l;
        inconsistent[orbit] = true;
        for (l = Store[CurrentStore].image[orbit].value->nr; !(part[l].top); l = part[l].nextorcard) {
            ;
        }
        for (unsigned int i = orbit + 1; i < Store[CurrentStore].length; ++i) {
            for (k = Store[CurrentStore].image[i].value->nr; !(part[k].top); k = part[k].nextorcard) {
                ;
            }
            if (k == l) {
                inconsistent[i] = true;
            }
        }
    }
}
#endif

#if defined(SYMMPROD)
void BuildProducts(unsigned int orbit, bool* inconsistent) {
    unsigned int StorePos(CurrentStore);
    if (!Store[StorePos].image[orbit].vector) {
        return;    // empty orbit, nothing to do
    }

    unsigned int j;
    unsigned int bpstore, bporbit, bpnext, bpval, bplevel, k, bpnull(0);
    unsigned int* bpcomp; // a permutation to compose the orbit's permutation(representative) with
    unsigned int* bpnxvc; // the result of this composition
    unsigned int level(Store[StorePos].argnr); // virtual offset of this orbit's vector

    for (j = 0; j < Store[StorePos].length; ++j) // count the number of still missing orbits
        if (!(Store[StorePos].image[j].vector || inconsistent[j])) {
            ++bpnull;
        }
    if (!bpnull) {
        return;    // all orbits are there, nothing to do
    }
    unsigned int* bplist(new unsigned int[bpnull + 1]); // a list of all orbits to work on
    bplist[0] = orbit; // initially only the one given as parameter of this function
    unsigned int bplength(0); // length minus one of the bplist

    for (unsigned int bp = 0; bp <= bplength; ++bp) {
        orbit = bplist[bp]; // the orbit to compose now
        compose = Store[StorePos].image[orbit].vector; // the representative of the orbit
        for (bpstore = CardStore; Store[bpstore - 1].argnr > Store[StorePos].image[orbit].value->nr ; --bpstore) {
            ;
        }
        for (; bpstore > StorePos; --bpstore) // go through all earlier built representatives (other groups and orbits)
            for (bporbit = 0; bporbit < Store[bpstore - 1].length; ++bporbit) {
                bpcomp = Store[bpstore - 1].image[bporbit].vector; // get a previously produced vector(permutation)
                if (!bpcomp) {
                    continue;    // and check if it exists, if not: no composition is possible
                }
                bplevel = Store[bpstore - 1].argnr; // offset (the virtual beginning of the vector bpcomp)
                if (Store[StorePos].image[orbit].value->nr < bplevel) {
                    continue;    // this permutation leads into same orbit, nothing new
                }
                bpval = bpcomp[Store[StorePos].image[orbit].value->nr - bplevel]; // get the new composed orbit number
                if (bpval <= level) {
                    continue;    // we ran into the symmetry group; no new orbit
                }
                for (bpnext = 0; Store[StorePos].image[bpnext].value->nr != bpval; ++bpnext) {
                    ;    // find bpval's position in the store (the orbit number)
                }
                if (Store[StorePos].image[bpnext].vector) {
                    continue;    // if a representative already exists for this orbit do nothing
                }
                bplist[++bplength] = bpnext; // a new orbit is found, we have to build products for it later
                bpnxvc = Store[StorePos].image[bpnext].vector = new unsigned int[Globals::Places[0]->cnt - level + 1];
                for (k = 0 ; k < Globals::Places[0]->cnt - level; ++k) { // now build the product vector bpcomp[compose[]] as new representative
                    bpnxvc[k] = (compose[k] < bplevel ? compose[k] : bpcomp[compose[k] - bplevel]);
                }
                MinimizeCarrier(bpnext); // remove cycles if possible (by building powers)
                if (--bpnull == 0) {
                    delete[] bplist;    // if this was the last missing orbit: stop immediately
                    return;
                }
            }
    }
    delete[] bplist;
}
#else
void BuildProducts(unsigned int orbit) {
    unsigned int StorePos(CurrentStore);

    if (Store[StorePos].image[orbit].vector) {
        compose = Store[StorePos].image[orbit].vector;
        while (true) {
            unsigned int val = compose[Store[StorePos].image[orbit].value->nr - Store[StorePos].argnr];
            if (val == Store[StorePos].argnr) {
                break;
            }

            unsigned int composed;
            for (composed = 0; Store[StorePos].image[composed].value->nr != val; ++composed) {
                ;
            }
            for (unsigned int k = 0; k < Globals::Places[0]->cnt - Store[StorePos].argnr; k++) {
                reservecompose[k] = Store[StorePos].image[orbit].vector[compose[k] - Store[StorePos].argnr];
            }
            if ((Store[StorePos].image[composed].vector)) {
                compose = reservecompose;
            } else {
                Store[StorePos].image[composed].vector = new unsigned int [Globals::Places[0]->cnt - Store[StorePos].argnr + 1];
                for (unsigned int k = 0; k < Globals::Places[0]->cnt - Store[StorePos].argnr; k++) {
                    Store[StorePos].image[composed].vector[k] = reservecompose[k];
                }
                compose = Store[StorePos].image[composed].vector;
            }
        }
    }
}
#endif


/*!
 Define in einem Setting, wo noch alle Constraints die Form [A,A] haben.
*/
void DefineToId() {
    unsigned int cntriv; ///< ein nichttriviales Constraint
    unsigned int intriv; ///< dessen Pos im Reaktor

    unsigned int i, j, k;

    // suche dasjenige mehrelementige Constraint, das den kleinsten
    // Knoten enthält

    /// Nr des kleinsten Knotens eines nichttrivialen Constraints
    unsigned int nrmin = UINT_MAX; // groesser als jeder korrekte Wert

    for (unsigned int c = 0; c < CardSpecification[PL]; c++) {
        if (Specification[PL][c].first != Specification[PL][c].last) {
            // bin in mehrelementigem Constraint
            for (i = Specification[PL][c].first; i <= Specification[PL][c].last; i++) {
                if (Reaktor[PL][DO][i].node->nr < nrmin) {
                    nrmin = Reaktor[PL][DO][i].node->nr;
                    intriv = i;
                    cntriv = c;
                }
            }
        }
    }
    // bereite Symmetriespeicher auf die neuen Elemente vor
    // Speichere potentielle Images in Store
    CardStore++;
    unsigned int oldstorenr = CurrentStore;
    CurrentStore = CardStore - 1;
    Store[CurrentStore].arg = Reaktor[PL][DO][intriv].node;
    Store[CurrentStore].argnr = nrmin;
    Store[CardStore - 1].image = new SymmImage [Specification[PL][cntriv].last - Specification[PL][cntriv].first + 1];
    Store[CurrentStore].length = Specification[PL][cntriv].last - Specification[PL][cntriv].first;
    for (j = 0, k = 0; j <= Store[CurrentStore].length; j++, k++) {
        if (j + Specification[PL][cntriv].first == intriv) {
            // skip arg as image
            k--;
        } else {
            Store[CurrentStore].image[k].vector = NULL;
            Store[CurrentStore].image[k].value = Reaktor[PL][CO][j + Specification[PL][cntriv].first].node;
        }

    }

    // separiere Knoten nrmin innerhalb Constraint cntriv
    for (DomType dir = 0; dir < 2; dir++) {
        Reaktor[PL][dir][Specification[PL][cntriv].first].node->pos[dir] = intriv;
        Reaktor[PL][dir][intriv].node->pos[dir] = Specification[PL][cntriv].first;
        Reaktoreintrag swap = Reaktor[PL][dir][Specification[PL][cntriv].first];
        Reaktor[PL][dir][Specification[PL][cntriv].first] = Reaktor[PL][dir][intriv];
        Reaktor[PL][dir][intriv] = swap;
    }
    Specification[PL][CardSpecification[PL]].last = Specification[PL][CardSpecification[PL]].first = Specification[PL][cntriv].first;
    Specification[PL][CardSpecification[PL]].changed = new ToDo(PL, CardSpecification[PL]);
    Specification[PL][CardSpecification[PL]].parent = cntriv;
    CardSpecification[PL]++;
    (Specification[PL][cntriv].first)++;
    Specification[PL][cntriv].changed = new ToDo(PL, cntriv);
    unsigned int MyCardSpecification[2] = {CardSpecification[PL], CardSpecification[TR]};
    reportprogress();
#ifdef DISTRIBUTE
    progress();
#endif
//    MyStorePosition = CurrentStore; // always the same!!
    if (!RefineUntilNothingChanges(PL)) {
        cout << " magic error\n";
    }
    if (CardSpecification[PL] != Globals::Places[0]->cnt) {
        DefineToId();
    }
    ReUnify(MyCardSpecification[PL], MyCardSpecification[TR]);
#if defined(SYMMPROD)
    bool* inconsistent = new bool[Store[CurrentStore].length];
    for (j = 0; j < Store[CurrentStore].length; ++j) {
        inconsistent[j] = false;
    }
#endif
    for (j = 0; j < Store[CurrentStore].length; j++) {
        SymmImage* sigma = Store[CurrentStore].image + j;
#if defined(SYMMPROD)
        if (!(sigma->vector || inconsistent[j])) // only try to find a new representative if the orbit is empty and consistent so far
#else
        if (!(sigma->vector)) // try to find a new representative if the orbit is empty so far
#endif
        {
            i = sigma->value->pos[CO];
            Reaktor[PL][CO][i].node->pos[CO] = Specification[PL][CardSpecification[PL] - 1].first;
            Reaktor[PL][CO][Specification[PL][CardSpecification[PL] - 1].first].node ->pos[CO] = i;
            Reaktoreintrag swap = Reaktor[PL][CO][i];
            Reaktor[PL][CO][i] = Reaktor[PL][CO][Specification[PL][CardSpecification[PL] - 1].first];
            Reaktor[PL][CO][Specification[PL][CardSpecification[PL] - 1].first] = swap;
            Specification[PL][cntriv].changed = new ToDo(PL, cntriv);
            Specification[PL][MyCardSpecification[PL] - 1].changed = new ToDo(PL, MyCardSpecification[PL] - 1);
            if (RefineUntilNothingChanges(PL)) { // no inconsistency so far
                if (CardSpecification[PL] == Globals::Places[0]->cnt) { // found a representative = full permutation
                    StoreSymmetry(j); // so store it away
                } else {
                    DefineToOther(j); // try to complete the current abstract permutation to a full one
                }
#if defined(SYMMPROD)
                BuildProducts(j, inconsistent);
#else
                BuildProducts(j); // we build products with earlier found generators and/or this generator itself here
#endif
            }
#if defined(SYMMPROD)
            BuildInconsistencies(j, inconsistent); // check if orbit is inconsistent; it may lead to more inconsistent orbits
#endif
            ReUnify(MyCardSpecification[PL], MyCardSpecification[TR]);
        }
    }
    CurrentStore = oldstorenr;
#if defined(SYMMPROD)
    delete[] inconsistent;
#endif
}


/// Die aktuelle Symm bei Iteration mittels FirstSymm() und NextSymm()
unsigned int* CurrentSymm;


/*!
 Berechnung des Erzeugendensystems
*/
void ComputeSymmetries() {
    status("calculating symmetries");
    init_syms();
    reservecompose = new unsigned int [Globals::Places[0]->cnt];
    Stamp = 1;
    InitialConstraint();
    FuelleReaktor();

    // Refine kann hier nicht fehlschlagen, weil id auf jeden Fall
    // konsistent sein muss
    if (!RefineUntilNothingChanges(PL)) {
        cout << " Was komisches ist passiert";
    }
    for (unsigned int i = 0; i < Globals::Places[0]->cnt; i++) {
        Globals::Places[i] = (Place*) Reaktor[PL][DO][i].node;
        Globals::Places[i]-> nr = Globals::Places[i] -> index = i;
        Globals::CurrentMarking[i] = Globals::Places[i]->initial_marking;

    }

    // Lege Aeq-Klassen an
    part = new Partition [Globals::Places[0]-> cnt];
    for (unsigned int i = 0; i < Globals::Places[0]->cnt; i++) {
        part[i].nextorcard = 1;
        part[i].top = true;
    }

    // Lege Speicher fuer Generatoren an
    Store = new SymmStore [Globals::Places[0]->cnt];
    CardStore = 0;
    unsigned int plp = CardSpecification[PL];
    unsigned int trp = CardSpecification[TR];

    // Starte Suche nach Generatoren
    if (CardSpecification[PL] != Globals::Places[0]->cnt) {
        DefineToId();
    }
    ReUnify(plp, trp);

    // Berechne Generatoren- und Symmetriezahl
    CardGenerators = 0;
    unsigned long int CardSymm = 1;
    for (unsigned int i = 0; i < CardStore; i++) {
        Store[i].card = 0;
        for (unsigned int j = 0; j < Store[i].length; j++) {
            if (Store[i].image[j].vector) {
                if (j > Store[i].card) {
                    Store[i].image[Store[i].card] = Store[i].image[j];

                }
                Store[i].card++;
            }
        }
        CardGenerators += Store[i].card;
        CardSymm *= Store[i].card + 1;

    }
    for (unsigned int i = 0; i < CardStore;) {
        if (Store[i].card) {
            Store[i].reference = new unsigned int[Globals::Places[0]->cnt - Store[i].argnr + 1];
            i++;
        } else {
            for (unsigned int j = i + 1; j < CardStore; j++) {
                Store[j - 1] = Store[j];
            }
            CardStore--;
        }
    }

    message("%d generators in %d groups for %d symmetries found", CardGenerators, CardStore, CardSymm);
    message("%d dead branches entered during symmetrie calculation", DeadBranches);

#if SYMMINTEGRATION == 2
    // Reaktoren auf Markierungsabb.suche vorbereiten, indem Aeq.klassen
    // zu Constraints werden. Countsort wird misbraucht.
    NewStamp();
    for (unsigned int i = 0; i < Globals::Places[0]->cnt; i++) {
        unsigned int j;
        for (j = i; !part[j].top; j = part[j].nextorcard) {
            ;
        }
        Reaktor[PL][DO][Globals::Places[i]->pos[DO]].stamp = Reaktor[PL][CO][Globals::Places[i]->pos[CO]].stamp = Stamp;
        Reaktor[PL][DO][Globals::Places[i]->pos[DO]].count = Reaktor[PL][CO][Globals::Places[i]->pos[CO]].count = j;
    }
    for (unsigned int i = 0; i < CardSpecification[PL]; i++) {
        CountSort(PL, DO, Specification[PL][i].first, Specification[PL][i].last);
        CountSort(PL, CO, Specification[PL][i].first, Specification[PL][i].last);
        Split(PL, i);
    }
    if (ToDoList[PL]) {
        RefineUntilNothingChanges(PL);
    }
#endif
    // Hashfaktoren eintragen: aeq. Plaetze bekommen gleichen Hashwert,
    // damit nur in einem Bucker gesucht werden muss
#if SYMMINTEGRATION < 3
    for (unsigned int i = 0; i < Globals::Places[i]->cnt; i++) {
        if (part[i].top) {
            part[i].nextorcard = rand();
        }
    }
    for (unsigned int i = 0; i < Globals::Places[0]->cnt; i++) {
        unsigned int j;
        for (j = i; !part[j].top; j = part[j].nextorcard) {
            ;
        }
        Globals::Places[i]->set_hash(part[j].nextorcard);
    }
    CurrentSymm = new unsigned int [Globals::Places[0]->cnt];
#else
    kanrep = new unsigned int [Globals::Places[0]->cnt];
    kanrep1 = new unsigned int [Globals::Places[0]->cnt];
#endif
    WriteSymms();
}

void ComputePartition() {
    message("partitioning nodes wrt symmetries...");
    init_syms();
    Stamp = 1;
    InitialConstraint();
    FuelleReaktor();

    if (!RefineUntilNothingChanges(PL)) {
        cout << " Was komisches ist passiert";
    }

    for (unsigned int i = 0; i < Globals::Places[0]->cnt; i++) {
        Globals::Places[i] = (Place*) Reaktor[PL][DO][i].node;
        Globals::Places[i]-> nr = Globals::Places[i] -> index = i;
        Globals::CurrentMarking[i] = Globals::Places[i]->initial_marking;
    }

    // partition places
    part = new Partition [Globals::Places[0]-> cnt];
    for (unsigned int i = 0; i < Globals::Places[0]->cnt; i++) {
        part[i].nextorcard = 1;
        part[i].top = true;
    }

    for (unsigned int c = 0; c < CardSpecification[PL]; c++) {
        for (unsigned int i = Specification[PL][c].first; i <= Specification[PL][c].last; i++) {
            UnifyClasses(Reaktor[PL][DO][i].node -> nr, Reaktor[PL][DO][Specification[PL][c].first].node->nr);
        }
    }

    //cout << "\n" << CardSpecification[PL] << " classes computed.\n";
    message("%d classes computed (%s)", CardSpecification[PL], _cparameter_("SYMMETRY"));

    // Reaktoren auf Markierungsabb.suche vorbereiten, indem Aeq.klassen
    // zu Constraints werden. Countsort wird misbraucht.
    NewStamp();
    for (unsigned int i = 0; i < Globals::Places[0]->cnt; i++) {
        unsigned int j;
        for (j = i; !part[j].top; j = part[j].nextorcard) {
            ;
        }
        Reaktor[PL][DO][Globals::Places[i]->pos[DO]].stamp = Reaktor[PL][CO][Globals::Places[i]->pos[CO]].stamp = Stamp;
        Reaktor[PL][DO][Globals::Places[i]->pos[DO]].count = Reaktor[PL][CO][Globals::Places[i]->pos[CO]].count = j;
    }
    for (unsigned int i = 0; i < CardSpecification[PL]; i++) {
        CountSort(PL, DO, Specification[PL][i].first, Specification[PL][i].last);
        CountSort(PL, CO, Specification[PL][i].first, Specification[PL][i].last);
        Split(PL, i);
    }
    if (ToDoList[PL]) {
        RefineUntilNothingChanges(PL);
    }
#if SYMMINTERGATION < 3
    // Hashfaktoren eintragen
    for (unsigned int i = 0; i < Globals::Places[i]->cnt; i++) {
        if (part[i].top) {
            part[i].nextorcard = rand();
        }
    }
    for (unsigned int i = 0; i < Globals::Places[0]->cnt; i++) {
        unsigned int j;
        for (j = i; !part[j].top; j = part[j].nextorcard) {
            ;
        }
        Globals::Places[i]->set_hash(part[j].nextorcard);
    }
#else
    kanrep = new unsigned int [Globals::Places[0]->cnt];
    kanrep1 = new unsigned int [Globals::Places[0]->cnt];
#endif
}


void FirstSymm() {
    for (unsigned int i = 0; i < CardStore; i++) {
        Store[i].current = Store[i].card;
        for (unsigned int j = Store[i].argnr; j < Globals::Places[i]->cnt; j++) {
            Store[i].reference[j - Store[i].argnr] = j;
        }
    }
    for (unsigned int i = 0; i < Globals::Places[0]->cnt; i++) {
        CurrentSymm[i] = i;
    }
}

/*!
 Identify next symm where at least one value between 0 and scg could have
 changed.

 \return return the smallest value where something has changed
 \return return value > #PL --> no more symm.
*/
unsigned int NextSymm(unsigned int scg) {
    int m;

    // search the largest argument less or equal to scg
    if (Store[0].argnr > scg) {
        return Globals::Places[0]->cnt + 27;
    }
    int l = 0;
    int r;
    if ((Store[r = CardStore - 1].argnr) <= scg) {
        m = r;
    } else {
        m = l;
        while ((r - l) > 1) {
            m = l + (scg - Store[l].argnr) * (r - l) / (Store[r].argnr - Store[l].argnr);
            if (Store[m].argnr == scg) {
                break;
            }
            if (m == l) {
                m++;
            }
            if (m == r) {
                m--;
            }
            if (Store[m].argnr == scg) {
                break;
            }
            if (Store[m].argnr < scg) {
                if (Store[m + 1].argnr > scg) {
                    break;
                } else {
                    l = m;
                }
            } else {
                r = m;
            }
        }
    }
    while (!(Store[m].current)) {
        if (m == 0) {
            return Globals::Places[0]->cnt + 27;
        }
        Store[m].current = Store[m--].card;
    };
    SymmStore* sigma = Store + m;
    sigma ->current--;
    for (unsigned int i = sigma->argnr; i < Globals::Places[0]->cnt; i++) {
        CurrentSymm[i] = sigma->reference[sigma->image[sigma->current].vector[i - sigma->argnr] - sigma->argnr];
    }
    for (unsigned int i = m + 1; i < CardStore; i++) {
        for (unsigned int j = Store[i].argnr; j < Globals::Places[0]->cnt; j++) {
            Store[i].reference[j - Store[i].argnr] = CurrentSymm[j];
        }
    }
    return m;
}

void AllSyms() {
    FirstSymm();
    unsigned int i = 1;
    while (NextSymm(Globals::Places[0]->cnt) <= Globals::Places[0]->cnt) {
        i++;
    }
    cout << i << "\n";
}

void check() {
    for (unsigned int i = 0; i < Globals::Places[0]->cnt; i++) {
        if (Globals::Places[i] != Reaktor[PL][DO][Globals::Places[i]->pos[DO]].node) {
            cout << "aua\n";
        }
        if (Globals::Places[i] != Reaktor[PL][CO][Globals::Places[i]->pos[CO]].node) {
            cout << "weia\n";
        }
    }

    for (unsigned int i = 0; i < Globals::Places[0]->cnt; i++) {
        if (!Reaktor[PL][DO][i].node) {
            cout << "aah\n";
        }
        if (!Reaktor[PL][CO][i].node) {
            cout << "uuh\n";
        }
    }

    for (unsigned int i = 0; i < Globals::Transitions[0]->cnt; i++) {
        if (Globals::Transitions[i] != Reaktor[TR][DO][Globals::Transitions[i]->pos[DO]].node) {
            cout << "aua\n";
        }
        if (Globals::Transitions[i] != Reaktor[TR][CO][Globals::Transitions[i]->pos[CO]].node) {
            cout << "weia\n";
        }
    }

    for (unsigned int i = 0; i < Globals::Transitions[0]->cnt; i++) {
        if (!Reaktor[TR][DO][i].node) {
            cout << "aah\n";
        }
        if (!Reaktor[TR][CO][i].node) {
            cout << "uuh\n";
        }
    }

    if (CardSpecification[PL] > Globals::Places[0]->cnt) {
        cout << "sclimm\n";
    }

    if (CardSpecification[TR] > Globals::Transitions[0]->cnt) {
        cout << "auch sclimm\n";
    }

    for (unsigned int i = 0; i < CardSpecification[PL]; i++) {
        if (Specification[PL][i].first > Specification[PL][i].last) {
            cout << "boese\n";
        }
    }

    for (unsigned int i = 0; i < CardSpecification[TR]; i++) {
        if (Specification[TR][i].first > Specification[TR][i].last) {
            cout << "auch boese\n";
        }
    }

    for (unsigned int i = 0; i < Globals::Places[0]->cnt; i++) {
        unsigned int k = 0;
        for (unsigned int j = 0; j < CardSpecification[PL]; j++) {
            if ((Specification[PL][j].first <= i)  && (i <= Specification[PL][j].last)) {
                k++;
            }
        }
        if (k != 1) {
            cout << "unfassbar\n";
        }
    }

    for (unsigned int i = 0; i < Globals::Transitions[0]->cnt; i++) {
        unsigned int k = 0;
        for (unsigned int j = 0; j < CardSpecification[TR]; j++) {
            if ((Specification[TR][j].first <= i)  && (i <= Specification[TR][j].last)) {
                k++;
            }
        }
        if (k != 1) {
            cout << "auch unfassbar\n";
        }
    }
}

/*
Diese Funktion scheint von niemandem gerufen zu werden. Ich habe sie daher
auskommentiert.    Niels, 27.6.2011

State* symm_search_marking2() {
    State* s;
    if ((s = search_marking())) {
        return s;
    }
    Decision* ld = LastDecision;
    Statevector* lv = LastVector;
    unsigned int li = Scapegoat;
    unsigned int lc = LastChoice;
    if (HashTable[Globals::Places[0]->hash_value]) {
        NewStamp();
        for (unsigned int i = 0; i < Globals::Places[0]->cnt; i++) {
            Reaktor[PL][DO][i].count = Globals::CurrentMarking[((Place*) Reaktor[PL][DO][i].node)->index];
            Reaktor[PL][DO][i].stamp = Stamp;
        }
        for (unsigned int i = 0; i < CardSpecification[PL]; i++) {
            CountSort(PL, DO, Specification[PL][i].first, Specification[PL][i].last);
        }
        LastDecision = ld;
        LastVector = lv;
        Scapegoat = li;
        LastChoice = lc;
        return symm_search2(HashTable[Globals::Places[0]->hash_value]);
    }
    LastDecision = ld;
    LastVector = lv;
    Scapegoat = li;
    LastChoice = lc;

    return NULL;
}
*/


#if defined(SYMMETRY) && SYMMINTEGRATION == 2

///! \todo Rückgabewert!
State* symm_search2(Decision* d) {
    State* s;
    unsigned int MyCardSpecification[2] = {CardSpecification[PL], CardSpecification[TR]};
    for (unsigned int i = 0; i < d -> size; i++) {
        if (d -> next[i]) {
            if ((s = symm_search2(d->next[i]))) {
                return s;
            }
        } else {
            Statevector* v;
            // try to calculate a symmetry from current to this state
            NewStamp();
            v = d -> vector[i];
            for (unsigned int k = Globals::Places[0]->cnt - 1;; k--) {
                Reaktor[PL][DO][k].count = Globals::CurrentMarking[((Place*) Reaktor[PL][DO][k].node)->index];
                Reaktor[PL][DO][k].stamp = Reaktor[PL][CO][k].stamp = Stamp;
                Reaktor[PL][CO][Globals::Places[k]->pos[CO]].count = (*v)[v->length + k - Globals::Places[0]->cnt];
                if (v->length + k == Globals::Places[0]->cnt) {
                    v = v -> prev;
                }
                if (k == 0) {
                    break;
                }
            }
            // Jetzt sortieren, splitten, Symmetrie suchen, reunifizieren, zurueckkehren
            bool spl = true;
            for (unsigned int k = 0; k < MyCardSpecification[PL]; k++) {
                CountSort(PL, CO, Specification[PL][k].first, Specification[PL][k].last);
                if (!Split(PL, k)) {
                    spl = false;
                    while (ToDoList[PL]) {
                        ToDo* tmp = ToDoList[PL];
                        ToDoList[PL] = ToDoList[PL] -> next;
                        delete tmp;
                    }
                    break;
                }

            }
            if (!spl) {
                ReUnify(MyCardSpecification[PL], MyCardSpecification[TR]);
                continue;
            }
            if (ToDoList[PL]) {
                if (!RefineUntilNothingChanges(PL)) {
                    ReUnify(MyCardSpecification[PL], MyCardSpecification[TR]);
                    continue;
                }
            }
            if (CardSpecification[PL] == Globals::Places[0]->cnt) {
                ReUnify(MyCardSpecification[PL], MyCardSpecification[TR]);
#if defined(TARJAN) || defined(COVER) || defined(MODELCHECKING)
                return d -> vector[i]->state;
#else
                return CurrentState; // Hauptsache, was anderes als Null
#endif
            }
            OnlineDefineToOther();
            ReUnify(MyCardSpecification[PL], MyCardSpecification[TR]);
            if (found) {
#if defined(TARJAN) || defined(COVER) || defined(MODELCHECKING)
                return d -> vector[i]->state;
#else
                return CurrentState; // Hauptsache, was anderes als Null
#endif
            }
        }
    }
    ReUnify(MyCardSpecification[PL], MyCardSpecification[TR]);
    return NULL;
}

///! \todo Rückgabewert!
State* bin_symm_search2(binDecision* d) {
#if defined(SYMMETRY) && SYMMINTEGRATION == 2
    State* st;
    unsigned int MyCardSpecification[2] = {CardSpecification[PL], CardSpecification[TR]};
    if (d -> nextold) {
        if ((st = bin_symm_search2(d->nextold))) {
            return st;
        }
    }
    if (d -> nextnew) {
        if ((st = bin_symm_search2(d->nextnew))) {
            return st;
        }
    }
    NewStamp();
#if defined(TARJAN) || defined(COVER) || defined(MODELCHECKING)
    st = d -> state;
#endif

    // try to calculate a symmetry from current to this state
    unsigned int p = Globals::Places[0] -> cnt - 1;
    unsigned char* v = d -> vector;
    unsigned int t = (Globals::BitVectorSize - (d -> bitnr + 1)) / 8;
    unsigned int s = (Globals::BitVectorSize - (d -> bitnr + 1)) % 8;
    unsigned int pb = Globals::Places[p] -> nrbits;
    unsigned int byte = v[t] >> (8 - s);
    unsigned int pm = 0;

    while (true) {

        // compute marking pm of place p corr. to decision entry d
        if (pb > s) {
            // place has more bits left than remaining in current byte
            if (s) {
                // at least, there _are_ bits left in current byte
                pm += byte << (Globals::Places[p]->nrbits - pb);
                pb -= s;
                s = 0;
            } else {
                // current byte empty
                if (t) {
                    // there is another byte in current vector
                    t--;
                    s = 8;
                    byte = v[t];
                } else {
                    // proceed to previous vector
                    int oldbitnr = d -> bitnr;
                    d = d -> prev;
                    v = d -> vector;
                    t = (oldbitnr - (d -> bitnr)) / 8;
                    s = (oldbitnr - (d -> bitnr)) % 8;
                    /// pay attn to implicit bit!
                    if (s == 0) {
                        s = 8;
                        t--;
                    }
                    byte = v[t] >> (8 - s) ;
                    byte = byte ^ 1;
                }
            }
        } else {
            // all bits left for this place are in current byte
            pm += (byte % (1 << pb)) << (Globals::Places[p]->nrbits - pb);
            byte = byte >> pb;
            s -= pb;

            Reaktor[PL][DO][Globals::Places[p]->pos[DO]].count = MARKINGVECTOR[p];
            Reaktor[PL][DO][p].stamp =  Reaktor[PL][CO][p].stamp = Stamp;
            Reaktor[PL][CO][Globals::Places[p]->pos[CO]].count = pm;
            if (p == 0) {
                break;
            }
            p--;
            pb = Globals::Places[p]->nrbits;
            pm = 0;
        }
    }
    // Jetzt sortieren, splitten, Symmetrie suchen, reunifizieren, zurueckkehren
    bool spl = true;
    for (unsigned int k = 0; k < MyCardSpecification[PL]; k++) {
//        CountSort(PL,DO,Specification[PL][k].first,Specification[PL][k].last);
        CountSort(PL, CO, Specification[PL][k].first, Specification[PL][k].last);
    }
    for (unsigned int k = 0; k < CardSpecification[PL]; k++) {
        if (!Split(PL, k)) {
            spl = false;
            while (ToDoList[PL]) {
                ToDo* tmp = ToDoList[PL];
                ToDoList[PL] = ToDoList[PL] -> next;
                delete tmp;
            }
            break;
        }

    }
    if (!spl) {
        ReUnify(MyCardSpecification[PL], MyCardSpecification[TR]);
        return NULL;
    }
    if (ToDoList[PL]) {
        if (!RefineUntilNothingChanges(PL)) {
            ReUnify(MyCardSpecification[PL], MyCardSpecification[TR]);
            return NULL;
        }
    }
    if (CardSpecification[PL] == Globals::Places[0]->cnt) {
        ReUnify(MyCardSpecification[PL], MyCardSpecification[TR]);
#if defined(TARJAN) || defined(COVER) || defined(MODELCHECKING)
        return st;
#else
        return CurrentState; // Hauptsache, was anderes als Null
#endif
    }
    OnlineDefineToOther();
    ReUnify(MyCardSpecification[PL], MyCardSpecification[TR]);
    if (found) {
#if defined(TARJAN) || defined(COVER) || defined(MODELCHECKING)
        return st;
#else
        return CurrentState; // Hauptsache, was anderes als Null
#endif
    }
    ReUnify(MyCardSpecification[PL], MyCardSpecification[TR]);
    return NULL;
#endif
}

State* bin_symm_search_marking2() {
    State* s;
    if ((s = binSearch())) {
        return s;
    }

    if (binHashTable[Globals::Places[0]->hash_value]) {
        NewStamp();
        for (unsigned i = 0; i < Globals::Places[0]->cnt; i++) {
            Reaktor[PL][DO][i].count = Globals::CurrentMarking[((Place*) Reaktor[PL][DO][i].node)->index];
            Reaktor[PL][DO][i].stamp = Stamp;
        }
        for (unsigned i = 0; i < CardSpecification[PL]; i++) {
            CountSort(PL, DO, Specification[PL][i].first, Specification[PL][i].last);
        }
        return bin_symm_search2(binHashTable[Globals::Places[0]->hash_value]);
    }

    return NULL;
}

#endif

/*!
 Return canonical_representitive of current marking as int vector. Use
 initialized Reaktor (i.e. rely on RefineUntilNothingChanges on initial
 partition).
*/
void canonize_on_the_fly() {
    Attempt = 0;
    found = false;
    for (unsigned int i = 0; i < Globals::Places[0]->cnt; i++) {
        kanrep[i] = Globals::CurrentMarking[i];
    }
    if (CardSpecification[PL] >= Globals::Places[0] -> cnt) {
        kanhash = Globals::Places[0]->hash_value;
        return;
    }

    OnlineCanonize();
    kanhash = 0;
    for (unsigned int i = 0; i < Globals::Places[0]->cnt; i++) {
        kanhash += Globals::Places[i] -> hash_factor * kanrep[i];
        kanhash %= HASHSIZE;
    }
}

State* canonical_representitive_on_the_fly() {
    canonize_on_the_fly();
    return binSearch();
}

void PrintStore() {
    for (unsigned int etage = 0; etage < CardStore; ++etage) {
        printf("Etage %d; arg=%s card=%d\n", etage, Store[etage].arg->name, Store[etage].card);
    }
}
