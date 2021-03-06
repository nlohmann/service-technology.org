/*****************************************************************************\
 Wendy -- Synthesizing Partners for Services

 Copyright (c) 2009 Niels Lohmann, Christian Sura, and Daniela Weinberg

 Wendy is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Wendy is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Wendy.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

#include "Cover.h"
#include "verbose.h"
#include "cmdline.h"
#include "util.h"

extern gengetopt_args_info args_info;

/******************
 * STATIC MEMBERS *
 ******************/

std::string* Cover::internalPlaces = NULL;
unsigned int Cover::internalPlaceCount = 0;
std::string* Cover::internalTransitions = NULL;
unsigned int Cover::internalTransitionCount = 0;
std::string* Cover::interfacePlaces = NULL;
unsigned int Cover::interfacePlaceCount = 0;
std::string* Cover::interfaceTransitions = NULL;
unsigned int Cover::interfaceTransitionCount = 0;
pnapi::Place** Cover::internalPlaceCache = NULL;
std::map<InnerMarking_ID, Cover::CoverData> Cover::inner2CD;
std::map<StoredKnowledge*, Cover::CoverData> Cover::knowledge2CD;
std::vector<StoredKnowledge*>* Cover::coveringInternalPlaces = NULL;
std::vector<StoredKnowledge*>* Cover::coveringInterfacePlaces = NULL;
std::vector<StoredKnowledge*>* Cover::coveringInternalTransitions = NULL;
std::vector<StoredKnowledge*>* Cover::coveringInterfaceTransitions = NULL;
Label_ID* Cover::interfacePlaceLabels = NULL;
std::map<std::string, Label_ID> Cover::labelCache;
bool Cover::satisfiable = true;
unsigned int Cover::nodeCount = 0;
std::vector<std::string> Cover::synchronousLabels;

/******************
 * STATIC METHODS *
 ******************/

/*!
 * \brief fill the set of places and transitions to cover
 *
 * Given two sets of labels this method searches for the named
 * nodes and stores their adresses in four arrays.
 */
void Cover::initialize(const std::vector<std::string> & placeNames,
                       const std::vector<std::string> & transitionNames) {
    /// determine place types
    std::vector<pnapi::Place*> inP;    // internal places
    std::vector<pnapi::Label*> comP;   // interface labels

    for (unsigned int i = 0; i < placeNames.size(); ++i) {
        pnapi::Place* p = InnerMarking::net->findPlace(placeNames[i]); // search place
        if (p != NULL) {
            inP.push_back(p); // found place
        } else {
            // "place" may be a label
            pnapi::Label* l = InnerMarking::net->getInterface().findLabel(placeNames[i]);
            if (l != NULL) {
                comP.push_back(l); // yes, it's a label
            } else {
                // no, we do not know such a label or place
                abort(16, "unknown place '%s' in file '%s'",
                      placeNames[i].c_str(), args_info.cover_arg);
            }
        }
    }

    /// determine transition types
    std::vector<pnapi::Transition*> inT;    // internal transitions
    std::vector<pnapi::Transition*> comT;   // communicating transitions

    for (unsigned int i = 0; i < transitionNames.size(); ++i) {
        pnapi::Transition* t = InnerMarking::net->findTransition(transitionNames[i]);
        if (t == NULL) {
            abort(16, "unknown transition '%s' in file '%s'",
                  transitionNames[i].c_str(), args_info.cover_arg);
        }

        if (t->getType() != pnapi::Transition::INPUT) {
            inT.push_back(t);
        } else {
            comT.push_back(t);
        }
    }

    initialize(inP, comP, inT, comT);
}

/*!
 * \brief Given all nodes to cover, seperated by type,
 *        this function initializes the arrays.
 */
void Cover::initialize(const std::vector<pnapi::Place*>& inP,
                       const std::vector<pnapi::Label*>& comP,
                       const std::vector<pnapi::Transition*>& inT,
                       const std::vector<pnapi::Transition*>& comT) {
    /// fill arrays
    internalPlaceCount = inP.size();
    internalPlaces = new std::string[internalPlaceCount];
    internalPlaceCache = new pnapi::Place*[internalPlaceCount];
    for (unsigned int i = 0; i < internalPlaceCount; ++i) {
        internalPlaces[i] = inP[i]->getName();
        internalPlaceCache[i] = inP[i];
    }

    interfacePlaceCount = comP.size();
    interfacePlaces = new std::string[interfacePlaceCount];
    interfacePlaceLabels = new Label_ID[interfacePlaceCount];
    for (unsigned int i = 0; i < interfacePlaceCount; ++i) {
        interfacePlaces[i] = comP[i]->getName();
        interfacePlaceLabels[i] = labelCache[interfacePlaces[i]];
    }

    internalTransitionCount = inT.size();
    internalTransitions = new std::string[internalTransitionCount];
    for (unsigned int i = 0; i < internalTransitionCount; ++i) {
        internalTransitions[i] = inT[i]->getName();
    }

    interfaceTransitionCount = comT.size();
    interfaceTransitions = new std::string[interfaceTransitionCount];
    for (unsigned int i = 0; i < interfaceTransitionCount; ++i) {
        interfaceTransitions[i] = comT[i]->getName();
    }

    /// clean up
    labelCache.clear();

    /// initialize knowledge vectors
    coveringInternalPlaces = new std::vector<StoredKnowledge*>[internalPlaceCount];
    coveringInterfacePlaces = new std::vector<StoredKnowledge*>[interfacePlaceCount];
    coveringInternalTransitions = new std::vector<StoredKnowledge*>[internalTransitionCount];
    coveringInterfaceTransitions = new std::vector<StoredKnowledge*>[interfaceTransitionCount];

    nodeCount = internalPlaceCount + interfacePlaceCount
                + internalTransitionCount + interfaceTransitionCount;
}

/*!
 * \brief covers all nodes
 */
void Cover::coverAll() {
    std::vector<pnapi::Place*> inP;    // internal places
    std::vector<pnapi::Label*> comP;   // interface labels
    std::vector<pnapi::Transition*> inT;    // internal transitions
    std::vector<pnapi::Transition*> comT;   // interface transitions

    // determine place types
    FOREACH(p, InnerMarking::net->getPlaces()) {
        inP.push_back(*p);
    }

    std::set<pnapi::Label*> asynchronousLabels = InnerMarking::net->getInterface().getAsynchronousLabels();
    FOREACH(l, asynchronousLabels) {
        comP.push_back(*l);
    }

    // determine transition types
    FOREACH(t, InnerMarking::net->getTransitions()) {
        if ((*t)->getType() == pnapi::Transition::INPUT) {
            comT.push_back(*t);
        } else {
            inT.push_back(*t);
        }
    }

    initialize(inP, comP, inT, comT);
}

/*!
 * \brief checks for a given inner marking, which nodes are covered
 */
void Cover::checkInnerMarking(InnerMarking_ID im,
                              std::map<const pnapi::Place*, unsigned int>& m,
                              const std::set<std::string> & t) {
    /// check internal places
    for (unsigned int i = 0; i < internalPlaceCount; ++i) {
        if (m[internalPlaceCache[i]] > 0) {
            inner2CD[im].inP.push_back(i);
        }
    }

    /// check internal transitions
    for (unsigned int i = 0; i < internalTransitionCount; ++i) {
        if (t.find(internalTransitions[i]) != t.end()) {
            inner2CD[im].inT.push_back(i);
        }
    }

    /// check interface transitions
    for (unsigned int i = 0; i < interfaceTransitionCount; ++i) {
        if (t.find(interfaceTransitions[i]) != t.end()) {
            inner2CD[im].comT.push_back(i);
        }
    }
}

/*!
 * \brief deletes cache
 */
void Cover::clear() {
    delete internalPlaceCache;
}

/*!
 * \brief checks for a given knowledge, which nodes are covered
 *
 * An intenal place or an internal transition is covered,
 * if there exists an inner marking in this bubble, that covers it.
 * An interface place is covered, if there exists an interface marking
 * in this bubble, that covers it.
 * An interface transition is covered, if there exists an inner marking
 * in this bubble that covers it, _and_ this inner marking has an
 * interface marking, that marks the preplace of this transiton.
 *
 */
void Cover::checkKnowledge(StoredKnowledge* K,
                           const std::map<InnerMarking_ID, std::vector<InterfaceMarking*> > & bubble) {
    /*
     * Sets for node collection in order to avoid dublicates.
     * Order is:
     * 0 - internal places
     * 1 - interface places
     * 2 - internal transitions
     * 3 - interdace transitions
     */
    std::set<unsigned int> coveredNodes[4];

    /// iterate through the knowledge bubble
    FOREACH(current, bubble) {
        /// copy internal places
        for (unsigned int i = 0; i < inner2CD[current->first].inP.size(); ++i) {
            coveredNodes[0].insert(inner2CD[current->first].inP[i]);
        }

        /// copy internal transitions
        for (unsigned int i = 0; i < inner2CD[current->first].inT.size(); ++i) {
            coveredNodes[2].insert(inner2CD[current->first].inT[i]);
        }

        /// check interface markings
        for (unsigned int i = 0; i < current->second.size(); ++i) {
            /// check interface places
            for (unsigned int j = 0; j < interfacePlaceCount; ++j) {
                if (current->second[i]->marked(interfacePlaceLabels[j])) {
                    coveredNodes[1].insert(j);
                }
            }

            /// check interface transitions
            for (unsigned int j = 0; j < inner2CD[current->first].comT.size(); ++j) {
                if (current->second[i]->marked(Label::name2id[interfaceTransitions[inner2CD[current->first].comT[j]]])) {
                    coveredNodes[3].insert(inner2CD[current->first].comT[j]);
                }
            }
        }
    }

    /// copy temporary data
    for (int i = 0; i < 4; ++i) {
        std::vector<unsigned int>* current;
        switch (i) {
            case 0:
                current = &(knowledge2CD[K].inP);
                break;
            case 1:
                current = &(knowledge2CD[K].comP);
                break;
            case 2:
                current = &(knowledge2CD[K].inT);
                break;
            case 3:
                current = &(knowledge2CD[K].comT);
                break;
        }

        FOREACH(it, coveredNodes[i]) {
            current->push_back(*it);
        }
    }
}

/*!
 * \brief removes knowledges when deleted
 */
void Cover::removeKnowledge(StoredKnowledge* K) {
    knowledge2CD.erase(K);
}

/*!
 * \brief calculates the global contraint
 *
 * This method iterates over all knowledge bubbles and stores
 * for each node of interest, which bubbles cover this node.
 */
void Cover::calculate(const std::set<StoredKnowledge*> & knowledges) {
    /// iterate over all knowledge bubbles
    FOREACH(K, knowledges) {
        for (int i = 0; i < 4; ++i) {
            std::vector<unsigned int>* currentSource;
            std::vector<StoredKnowledge*>* currentTarget;
            switch (i) {
                case 0:
                    currentSource = &(knowledge2CD[*K].inP);
                    currentTarget = coveringInternalPlaces;
                    break;
                case 1:
                    currentSource = &(knowledge2CD[*K].comP);
                    currentTarget = coveringInterfacePlaces;
                    break;
                case 2:
                    currentSource = &(knowledge2CD[*K].inT);
                    currentTarget = coveringInternalTransitions;
                    break;
                case 3:
                    currentSource = &(knowledge2CD[*K].comT);
                    currentTarget = coveringInterfaceTransitions;
                    break;
            }

            for (unsigned int j = 0; j < currentSource->size(); ++j) {
                currentTarget[(*currentSource)[j]].push_back(*K);
            }
        }
    }

    /// check for empty clauses
    for (int i = 0; i < 4; ++i) {
        std::vector<StoredKnowledge*>* current;
        unsigned int currentCount;
        switch (i) {
            case 0:
                current = coveringInternalPlaces;
                currentCount = internalPlaceCount;
                break;
            case 1:
                current = coveringInterfacePlaces;
                currentCount = interfacePlaceCount;
                break;
            case 2:
                current = coveringInternalTransitions;
                currentCount = internalTransitionCount;
                break;
            case 3:
                current = coveringInterfaceTransitions;
                currentCount = interfaceTransitionCount;
                break;
        }

        for (unsigned int j = 0; j < currentCount; ++j) {
            satisfiable = satisfiable && (!current[j].empty());
        }
    }
}

/*!
 * \brief writes the contraint in an output stream
 */
void Cover::write(std::ostream& os) {
    os << "\nCOVER\n"
       << "  PLACES\n";

    if (internalPlaceCount > 0) {
        os << "    " << internalPlaces[0];
        if (interfacePlaceCount > 0) {
            os << ",\n    " << interfacePlaces[0];
        }
    } else {
        if (interfacePlaceCount > 0) {
            os << "    " << interfacePlaces[0];
        } else {
            os << "    NONE";
        }
    }

    for (unsigned int i = 1; i < internalPlaceCount; ++i) {
        os << ",\n    " << internalPlaces[i];
    }
    for (unsigned int i = 1; i < interfacePlaceCount; ++i) {
        os << ",\n    " << interfacePlaces[i];
    }

    os << ";\n  TRANSITIONS\n";

    if (internalTransitionCount > 0) {
        os << "    " << internalTransitions[0];
        if (interfaceTransitionCount > 0) {
            os << ",\n    " << interfaceTransitions[0];
        }
    } else {
        if (interfaceTransitionCount > 0) {
            os << "    " << interfaceTransitions[0];
        } else {
            os << "    NONE";
        }
    }

    for (unsigned int i = 1; i < internalTransitionCount; ++i) {
        os << ",\n    " << internalTransitions[i];
    }
    for (unsigned int i = 1; i < interfaceTransitionCount; ++i) {
        os << ",\n    " << interfaceTransitions[i];
    }

    os << ";\n  CONSTRAINT\n";

    if (!satisfiable) {
        os << "FALSE;\n";
    } else {
        os << "  (\n";

        unsigned int lastClause = internalPlaceCount + interfacePlaceCount
                                  + internalTransitionCount + interfaceTransitionCount;

        /// for each node a clause
        for (int i = 0; i < 4; ++i) {
            std::vector<StoredKnowledge*>* current;
            unsigned int currentCount;
            switch (i) {
                case 0:
                    current = coveringInternalPlaces;
                    currentCount = internalPlaceCount;
                    break;
                case 1:
                    current = coveringInterfacePlaces;
                    currentCount = interfacePlaceCount;
                    break;
                case 2:
                    current = coveringInternalTransitions;
                    currentCount = internalTransitionCount;
                    break;
                case 3:
                    current = coveringInterfaceTransitions;
                    currentCount = interfaceTransitionCount;
                    break;
            }

            for (unsigned int j = 0; j < currentCount; ++j) {
                os << "    (";

                if (current[j].size() > 0) {
                    os << reinterpret_cast<size_t>(current[j][0]);
                }

                for (unsigned int k = 1; k < current[j].size(); ++k) {
                    os << " + " << reinterpret_cast<size_t>(current[j][k]);
                }

                if (--lastClause > 0) {
                    os << ") *\n";
                } else {
                    os << ")\n";
                }
            }
        }

        os << "  )";

        /// synchronous labels
        for (unsigned int i = 0; i < synchronousLabels.size(); ++i) {
            os << " *\n  #" << synchronousLabels[i];
        }

        os << ";\n\n";
    }

    /// verbose output
    bool firstBubble = true;
    FOREACH(it, knowledge2CD) {
        if (!firstBubble) {
            os << ",\n";
        } else {
            firstBubble = false;
        }

        os << "  " << reinterpret_cast<size_t>(it->first) << " :";
        if (it->second.inP.empty() &&
                it->second.comP.empty() &&
                it->second.inT.empty() &&
                it->second.comT.empty()) {
            os << " none";
        } else {
            for (int i = 0; i < 4; ++i) {
                std::vector<unsigned int>* current;
                std::string* currentName;
                switch (i) {
                    case 0:
                        current = &(it->second.inP);
                        currentName = internalPlaces;
                        break;
                    case 1:
                        current = &(it->second.comP);
                        currentName = interfacePlaces;
                        break;
                    case 2:
                        current = &(it->second.inT);
                        currentName = internalTransitions;
                        break;
                    case 3:
                        current = &(it->second.comT);
                        currentName = interfaceTransitions;
                        break;
                }

                for (unsigned int j = 0; j < current->size(); ++j) {
                    os << " " << currentName[(*current)[j]];
                }
            }
        }
    }

    os << ";\n";
}
