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

#include "Reset.h"
#include "Tara.h"
#include "PnapiHelper.h"

namespace Reset {
    void transformNet() {

        // three places:
        // init, count_costs, finished
        pnapi::Place* init      = & Tara::net->createPlace("reset_init", 0);
        pnapi::Place* doCount   = & Tara::net->createPlace("reset_doCount", 0);
        pnapi::Place* dontCount = & Tara::net->createPlace("reset_dontCount", 0);

        // go through all non-free transitions
        {
            std::map<pnapi::Transition*, unsigned int>::iterator it;
            it = Tara::partialCostFunction.begin();
            std::map<pnapi::Transition*, unsigned int>::iterator end;
            end = Tara::partialCostFunction.end();

            for(; it != end; ++it) {
                if(Tara::isReset(it->first)) {
                    continue;
                }
                // create a noCost copy
                // loop to "dont Count" - place
                pnapi::Transition* newT = pnapiHelper::clone(it->first);

                Tara::net->createArc(*dontCount, *newT, 1);
                Tara::net->createArc(*newT, *dontCount, 1);

                // loop to "doCount" - place
                Tara::net->createArc(*doCount, *(it->first));
                Tara::net->createArc(*(it->first), *doCount);

            }
        }

        // go through all reset-transitions
        {
            std::map<pnapi::Transition*, bool>::iterator it;
            it = Tara::resetMap.begin();
            std::map<pnapi::Transition*, bool>::iterator end;
            end = Tara::resetMap.end();

            for(; it != end; ++it) {

                if(!it->second) continue;
                status("%s is Reset- Transition", it->first->getName().c_str());

                // 1
                // create a noCost copy
                // keep in dont count
                pnapi::Transition* newT = dynamic_cast<pnapi::Transition*>(pnapiHelper::clone(it->first));
                Tara::net->createArc(*dontCount, *newT);
                Tara::net->createArc(*newT, *dontCount);
                // create a noCost copy
                
                // 2
                // keep in dont count
                pnapi::Transition* newT2 = dynamic_cast<pnapi::Transition*>(pnapiHelper::clone(it->first));
                Tara::net->createArc(*init, *newT2);
                Tara::net->createArc(*dontCount, *newT2);
                Tara::net->createArc(*newT2, *doCount);

                // 3
                // switch from counting to not counting
                Tara::net->createArc(*doCount, *(it->first));
                Tara::net->createArc(*(it->first), *dontCount);

            }
        }

        // now handle the initial marking
        {
            // create two starting transition
            pnapi::Place* preInit =      & Tara::net->createPlace("reset_preInit", 0);
            pnapi::Transition* initYes = & Tara::net->createTransition("reset_initCount");
            pnapi::Transition* initNo  = & Tara::net->createTransition("reset_initDontCount");

            // the first starts with counting
            Tara::net->createArc(*preInit, *initYes);
            Tara::net->createArc(*init, *initYes);
            Tara::net->createArc(*initYes, *doCount);

            // the second starts without counting
            Tara::net->createArc(*preInit, *initNo);
            Tara::net->createArc(*initNo, *dontCount);
            

            // now clone the init marking
            std::set<pnapi::Place*, bool>::iterator it;
            it = Tara::net->getPlaces().begin();
            std::set<pnapi::Place*>::iterator end;
            end = Tara::net->getPlaces().end();
            for(; it != end; ++it) {
                int tokenCount = (*it)->getTokenCount();
                if( tokenCount == 0) continue;

                Tara::net->createArc(*initYes, **it, tokenCount);
                Tara::net->createArc(*initNo, **it, tokenCount);

                (*it)->setTokenCount(0);

            }
            // the only starting markings
            preInit->setTokenCount(1);
            init->setTokenCount(1);
        }
    }
}
