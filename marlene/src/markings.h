/*****************************************************************************\
 Marlene -- synthesizing behavioral adapters

 Copyright (C) 2009, 2010  Christian Gierds <gierds@informatik.hu-berlin.de>

 Marlene is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Marlene is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Marlene.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

#ifndef MARKINGS_H_
#define MARKINGS_H_

#include <string>
#include <vector>
#include <map>

#include "pnapi/pnapi.h"

// forward declarations
class Marking;

class MarkingInformation
{
    private:
        unsigned int markingId;
        std::map< int, unsigned int > marking2id;
        std::map< unsigned int, int > id2marking;

    public:
        std::map< unsigned int, Marking * > markings;

        MarkingInformation(std::string & filename);

        unsigned int getIDForMarking( int nameId );
        int getMarkingForID( unsigned int id );
};

class Marking
{
    private:
        unsigned int placeId;
        std::map< std::string, unsigned int > place2id;
        std::map< unsigned int, std::string > id2place;

    public:
        std::vector< std::pair< unsigned int, unsigned int> > marking;

        Marking();

        unsigned int getIDForPlace( std::string & place );
        std::string & getPlaceForID( unsigned int id );

        std::vector< std::string > getPendingMessages(pnapi::PetriNet & net, std::string prefix = "");
        std::vector< std::string > getRequiredMessages(pnapi::PetriNet & net, std::string prefix = "");
};

#endif /* MARKINGS_H_ */
