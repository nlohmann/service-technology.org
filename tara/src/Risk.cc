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

#include "Risk.h"
#include "Tara.h"
#include <cmath>
#include <climits>
unsigned int riskFactor = 1;
int base = 1;

// if the most expensive path is more expensive than UINT_MAX / DIVIDE, we get
// an overflow
#define DIVIDE 1000

void transformRiskCosts(std::map<pnapi::Transition*, unsigned int>* partialCostfunction, int baseIn) {
    base = baseIn;

    status("minRiskBase %d", base);
    unsigned int minRisk = base;

    std::map<pnapi::Transition*, unsigned int>::iterator it;
    for(it = partialCostfunction->begin(); it != partialCostfunction->end(); ++it) {
        if(minRisk > it->second) {
            minRisk = it->second;
        }
    }
    double minOutRisk = log( (1.0 * base) / (1.0 * minRisk));

    riskFactor = ((UINT_MAX * 1.0) / (DIVIDE * 1.0)) / (1.0 * minOutRisk);
    status("factor: %d", riskFactor);

    double curInRisk;
    double curLogRisk;
    unsigned int newCost;

    for (it = partialCostfunction->begin(); it != partialCostfunction->end(); ++it) {
        // -1 * log ( x/100 ) = log ( 100 / x )
        const unsigned int oldCost = it->second;
        
        if(oldCost > base) {
            message("Risk of %s is %d, but cannot be greater than riskBase (%d)", it->first->getName().c_str(), it->second, base);
            abort();
        }
        curInRisk = (1.0 * base) / (1.0 * oldCost);
        curLogRisk = log(curInRisk);
        newCost = static_cast<unsigned int>(riskFactor * curLogRisk);
        it->second = newCost;
        status("new cost for %s: %d", (it->first)->getName().c_str(), it->second);
        if(oldCost == minRisk) {
            status("found highest trans cost: %d", newCost);
            Tara::highestTransitionCosts = newCost;
        }
    }
}

double backtransformRiskCost(long cost) {
    return exp((cost * -1.0) / (1.0 * riskFactor));
}

double backtransformRiskCostPercent(long cost) {
    return  100.0 * (exp((cost * -1.0) / (1.0 * riskFactor)));
}
