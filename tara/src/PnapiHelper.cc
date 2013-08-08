/*****************************************************************************\
 Tara-- <<-- Tara -->>

 Copyright (c) <<-- 20XX Author1, Author2, ... -->>

 Tara is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Tara is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Hello.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

#include "PnapiHelper.h"

#include <pnapi/pnapi.h>
#include "verbose.h"
#include "Tara.h"

namespace pnapiHelper {
    pnapi::Transition* clone(pnapi::Transition* n) {
        pnapi::PetriNet* net = & n->getPetriNet();
        pnapi::Transition* cl = & net->createTransition();

        // copy pre-set
        {
            std::set<pnapi::Arc*>::iterator it;
            it = n->getPresetArcs().begin();
            std::set<pnapi::Arc*>::iterator end;
            end = n->getPresetArcs().end();

            for(; it != end; ++it) {
                net->createArc((*it)->getSourceNode(), *cl, (*it)->getWeight());
            }
        }

        // copy post-set
        {
            std::set<pnapi::Arc*>::iterator it;
            it = n->getPostsetArcs().begin();
            std::set<pnapi::Arc*>::iterator end;
            end = n->getPostsetArcs().end();

            for(; it != end; ++it) {
                net->createArc(*cl, (*it)->getTargetNode(), (*it)->getWeight());
            }
        }

        //copy labels
        {
            std::map<pnapi::Label*, unsigned int>::const_iterator it;
            it = n->getLabels().begin();
            std::map<pnapi::Label*, unsigned int>::const_iterator end;
            end = n->getLabels().end();

            for(; it != end; ++it) {
                cl->addLabel( *(it->first), it->second);
            }
        }
        return cl;
    }
}
