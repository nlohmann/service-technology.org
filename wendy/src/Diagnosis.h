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


#ifndef _DIAGNOSIS_H
#define _DIAGNOSIS_H

#include <fstream>
#include "StoredKnowledge.h"
#include "InnerMarking.h"
#include "Results.h"


class DiagnosisObject {
    public:
        /// output diagnosis information as results file
        std::string output_results();

        std::set<Label_ID> violatedChannels;
        std::set<InnerMarking_ID> internalDeadlocks;
        std::set<std::set<InnerMarking_ID> > internalLivelocks;
        std::set<InnerMarking_ID> unresolvableWaitstates;
};


/*!
 \brief diagnosis information for uncontrollable services
 */
class Diagnosis {
    public:
        /// a mapping from an inner marking to its SCC
        static std::map<InnerMarking_ID, std::set<InnerMarking_ID> > markings2scc;

        /// print a dot representation for diagnosis
        static void output_diagnosedot(std::ostream&);

        /// output diagnosis information as results file
        static void output_results(Results& r);

        /// return the last marking from which two given markings are reachable
        static InnerMarking_ID lastCommonPredecessor(InnerMarking_ID, InnerMarking_ID);
};

#endif
