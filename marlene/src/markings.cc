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

#include "config.h"

#include <utility> // for make_pair

#include "markings.h"
#include "verbose.h"
#include "macros.h"

MarkingInformation::MarkingInformation(std::string & filename) : markingId(0)
{
    FUNCIN
    extern FILE * marking_information_yyin;
    extern int marking_information_yyparse();
    extern int marking_information_yylineno;
#ifdef YY_FLEX_HAS_YYLEX_DESTROY
    extern int marking_information_yylex_destroy();
#endif
    extern MarkingInformation * mi;

    status("reading marking information from file \"%s\"", filename.c_str());
    if(not (marking_information_yyin = fopen(filename.c_str(),"r")))
    {
        abort(2, "Marking information file %s could not be opened for reading", filename.c_str());
    }

    marking_information_yylineno = 1;
    mi = this;

    marking_information_yyparse();
    fclose(marking_information_yyin);
    marking_information_yyin = NULL;
#ifdef YY_FLEX_HAS_YYLEX_DESTROY
    marking_information_yylex_destroy();
#endif

    FUNCOUT

}

unsigned int MarkingInformation::getIDForMarking( int nameId )
{
    FUNCIN
    unsigned int tempId = marking2id[nameId];
    if ( tempId != 0 )
    {
        return tempId;
    }
    tempId = ++markingId;

    // status("Marking %d got id %d", nameId, tempId);

    marking2id[nameId] = tempId;
    id2marking[tempId] = nameId;
    FUNCOUT
    return tempId;
}

int MarkingInformation::getMarkingForID( unsigned int id )
{
    FUNCIN
    FUNCOUT
    return id2marking[id];
}

Marking::Marking() : placeId(0)
{
    FUNCIN
    FUNCOUT
}

unsigned int Marking::getIDForPlace( std::string & place )
{
    FUNCIN
    unsigned int tempId = place2id[place];
    if ( tempId != 0 )
    {
        return tempId;
    }
    tempId = ++placeId;

    // status("Place %s got id %d", place.c_str(), tempId);

    place2id[place] = tempId;
    id2place[tempId] = place;
    FUNCOUT
    return tempId;
}

std::string & Marking::getPlaceForID( unsigned int id )
{
    FUNCIN
    FUNCOUT
    return id2place[id];
}

std::vector< std::string > Marking::getPendingMessages(pnapi::PetriNet & net, std::string prefix, unsigned int messageBound)
{
    FUNCIN
    std::vector< std::string > result;

    for ( unsigned int k = 0; k < this->marking.size(); ++k )
    {
        // get all pending messages on internal places starting with <prefix>
        std::string name = getPlaceForID(marking[k].first);
        if (name.find(prefix) == 0)
        {
            name = name.substr(prefix.length());
            pnapi::Place * place = net.findPlace(name);

            if ( place != NULL )
            {
                name = name.substr(0,name.length() - 4);
                unsigned int token = count(result.begin(), result.end(), name);
                // add the appropriate amount of messages
                for ( int i = 1; i <= marking[k].second and i <= (messageBound - token); ++i)
                {
                    result.push_back(name);
                }
            }
        }
        else
        {
            // interface place?
            pnapi::Label * label = net.getInterface().findLabel(name);

            if ( label != NULL )
            {
                unsigned int token = count(result.begin(), result.end(), name);
                // add the appropriate amount of messages
                for ( int i = 1; i <= marking[k].second and i <= (messageBound - token); ++i)
                {
                    result.push_back(name);
                }
            }
        }
    }

    FUNCOUT
    return result;
}

std::vector< std::string > Marking::getRequiredMessages(pnapi::PetriNet & net, std::string prefix)
{
    FUNCIN
    std::vector< std::string > result;

    const std::set< pnapi::Place * > & places = net.getPlaces();

    //! \TODO: direct mapping for Marking()
    PNAPI_FOREACH(place, places)
    {
       (*place)->setTokenCount(0);
    }

    for ( unsigned int k = 0; k < this->marking.size(); ++k )
    {
        std::string name = getPlaceForID(marking[k].first);
        if (name.find(prefix) == 0)
        {
            name = name.substr(prefix.length());
            pnapi::Place * place = net.findPlace(name);

            if ( place != NULL )
            {
                place->setTokenCount(marking[k].second);
                // status("Place %s in %s is marked", name.c_str(), prefix.c_str());
            }
        }
    }

    pnapi::Marking marking(net);
    std::set< pnapi::Label *> labels = net.getInterface().getInputLabels();

    PNAPI_FOREACH(label, labels)
    {
        PNAPI_FOREACH(transition, (*label)->getTransitions())
        {
            if (marking.activates(**transition))
            {
//                std::cerr << (*label)->getName() << " ";
                result.push_back((*label)->getName());
            }
        }
    }
//    std::cerr << std::endl;
    pnapi::Condition & cond = net.getFinalCondition();

    if ( cond.isSatisfied(marking) )
    {
        result.push_back("yes");
    }
    else
    {
        result.push_back("no");
    }

    FUNCOUT
    return result;
}

