/*****************************************************************************\
 Diane -- Decomposition of Petri nets.

 Copyright (C) 2009  Stephan Mennicke <stephan.mennicke@uni-rostock.de>

 Diane is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Diane is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Diane.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/


#include <map>
#include <set>
#include <iostream>
#include "cmdline.h"
#include "decomposition.h"
#include <typeinfo>

using pnapi::Arc;
using pnapi::Place;
using pnapi::Transition;
using std::map;
using std::set;
using std::cout;


extern gengetopt_args_info args_info;


void decomposition::MakeSet(int x, int* tree) {
    // makes singleton sets of all x given
    tree[x] = -1;
}


void decomposition::Union(int x, int y, int* tree) {
    int xr = Find(x, tree); // finding x's root
    int yr = Find(y, tree); // finding y's root
    if (tree[xr] < tree[yr]) { // then the number of children of x is greater
        tree[xr] += tree[yr];
        tree[yr] = xr;
    } else if (xr != yr) {
        tree[yr] += tree[xr];
        tree[xr] = yr;
    }
}


int decomposition::Find(int x, int* tree) {
    if (tree[x] >= 0) {
        tree[x] = Find(tree[x], tree);
    } else {
        return x;
    }
    return tree[x];
}


int decomposition::computeComponentsByUnionFind(PetriNet& net, int* tree, int size, int psize, map<int, Node*> &reremap) {
    // map to remap the integers to the Petri net's nodes
    map<Node*, int> remap;

    // init MakeSet calls
    int q = 0;
    for (set<Place*>::iterator it = net.getPlaces().begin(); it != net.getPlaces().end(); ++it) {
        remap[*it] = q;
        reremap[q] = *it;
        MakeSet(q, tree);
        q++;
    }
    for (set<Transition*>::iterator it = net.getTransitions().begin(); it != net.getTransitions().end(); ++it) {
        remap[*it] = q;
        reremap[q] = *it;
        MakeSet(q, tree);
        q++;
    }

    if (args_info.service_flag) {

        net.finalCondition().dnf();
        const pnapi::formula::Formula* f = dynamic_cast<const pnapi::formula::Formula*>(&net.finalCondition().formula());

        std::set<std::string> ss;//set of marked places final markings
        if (typeid(*f) == typeid(pnapi::formula::Disjunction)) {
            const pnapi::formula::Disjunction* fd = dynamic_cast<const pnapi::formula::Disjunction*>(f);
            for (std::set<const pnapi::formula::Formula*>::iterator cIt = fd->children().begin(); cIt != fd->children().end(); ++cIt) {
                if (((*cIt)->getType() == pnapi::formula::Formula::F_CONJUNCTION)) { //typeid(*cIt)==typeid(pnapi::formula::Conjunction)){
                    const pnapi::formula::Conjunction* fc = dynamic_cast<const pnapi::formula::Conjunction*>(*cIt);
                    for (std::set<const pnapi::formula::Formula*>::iterator ccIt = fc->children().begin(); ccIt != fc->children().end(); ++ccIt) {
                        const pnapi::formula::Proposition* fp = dynamic_cast<const pnapi::formula::Proposition*>(*ccIt);
                        //const Place *pp=new Place(fp->place());
                        if (fp->tokens() > 0) { //insert only if it is non-zero
                            ss.insert(fp->place().getName());
                        }
                    }
                }

            }
        }
        if (typeid(*f) == typeid(pnapi::formula::Conjunction)) {
            const pnapi::formula::Conjunction* fc = dynamic_cast<const pnapi::formula::Conjunction*>(f);
            for (std::set<const pnapi::formula::Formula*>::iterator cIt = fc->children().begin(); cIt != fc->children().end(); ++cIt) {
                const pnapi::formula::Proposition* fp = dynamic_cast<const pnapi::formula::Proposition*>(*cIt);
                //const Place *pp=new Place(fp->place());
                if (fp->tokens() > 0) {
                    ss.insert(fp->place().getName());
                }
            }
        }


        //std::set<const Place * > cp=net.finalCondition().concerningPlaces(); doesn't work :(
        for (set< Place*>::iterator it = net.getInternalPlaces().begin(); it != net.getInternalPlaces().end(); ++it) {
            const std::string sp = (*it)->getName();
            if ((*it)->getTokenCount() || (ss.find(sp) != ss.end())) {
                for (set<Node*>::iterator t = (*it)->getPreset().begin(); t != (*it)->getPreset().end(); ++t) {
                    Union(remap[*it], remap[*t], tree);
                }
                for (set<Node*>::iterator t = (*it)->getPostset().begin(); t != (*it)->getPostset().end(); ++t) {
                    Union(remap[*it], remap[*t], tree);
                }
            }
        }
    }

    // usage of rules 1 and 2
    for (set<Place*>::iterator p = net.getPlaces().begin(); p != net.getPlaces().end(); ++p) {
        set<Node*> preset = (*p)->getPreset();
        set<Node*>::iterator first = preset.begin();
        int f = remap[*first];
        for (set<Node*>::iterator t = preset.begin(); t != preset.end(); ++t) {
            Union(f, remap[*t], tree);
        }
        set<Node*> postset = (*p)->getPostset();
        first = postset.begin();
        f = remap[*first];
        for (set<Node*>::iterator t = postset.begin(); t != postset.end(); ++t) {
            Union(f, remap[*t], tree);
        }
    }

    // usage of rule 3 until nothing changes
    bool hasChanged;
    do {
        hasChanged = false;
        for (set<Place*>::iterator p = net.getPlaces().begin(); p != net.getPlaces().end(); ++p) {
            set<Node*> preset = (*p)->getPreset();
            set<Node*> postset = (*p)->getPostset();
            int proot = Find(remap[*p], tree);
            for (set<Node*>::iterator pt = preset.begin(); pt != preset.end(); ++pt) {
                int ptroot = Find(remap[*pt], tree);
                if (proot == ptroot) {
                    continue;
                }
                bool localChange = false;
                for (set<Node*>::iterator tp = postset.begin(); tp != postset.end(); ++tp) {
                    int tproot = Find(remap[*tp], tree);
                    if (tproot == ptroot) {
                        hasChanged = localChange = true;
                        Union(tproot, proot, tree);
                        break;
                    }
                }
                if (localChange) {
                    break;
                }
            }
        }
    } while (hasChanged);

    // output: number of components
    int n = 0;
    for (int i = 0; i < psize; ++i)
        if (tree[i] < -1) {
            n++;
        }
    for (int i = psize; i < size; ++i)
        if (tree[i] < 0) {
            n++;
        }

    return n;
}


void decomposition::createOpenNetComponentsByUnionFind(vector<PetriNet*> &nets, int* tree, int size, int psize, map<int, Node*> &reremap) {
    for (int i = 0; i < psize; ++i) {
        if (tree[i] == -1) {
            continue;    // interface place found
        }

        int x = Find(i, tree);
        if (nets[x] == NULL) {
            nets[x] = new PetriNet();
        }
        Place* p = static_cast<Place*>(reremap[i]);
        nets[x]->createPlace(p->getName(), Node::INTERNAL, p->getTokenCount());
    }

    for (int i = psize; i < size; ++i) {
        int x = Find(i, tree);
        if (nets[x] == NULL) {
            nets[x] = new PetriNet();
        }
        Transition* t = &nets[x]->createTransition(reremap[i]->getName());
        /// creating arcs and interface
        set<Arc*> preset = reremap[i]->getPresetArcs();
        for (set<Arc*>::iterator f = preset.begin(); f != preset.end(); ++f) {
            Place* place = &(*f)->getPlace();
            Place* netPlace;
            netPlace = nets[x]->findPlace(place->getName());
            if (netPlace == NULL) {
                netPlace = &nets[x]->createPlace(place->getName(), Node::INPUT);
            }
            nets[x]->createArc(*netPlace, *t, (*f)->getWeight());
        }
        set<Arc*> postset = reremap[i]->getPostsetArcs();
        for (set<Arc*>::iterator f = postset.begin(); f != postset.end(); ++f) {
            Place* place = &(*f)->getPlace();
            Place* netPlace = nets[x]->findPlace(place->getName());
            if (netPlace == NULL) {
                netPlace = &nets[x]->createPlace(place->getName(), Node::OUTPUT);
            }
            nets[x]->createArc(*t, *netPlace, (*f)->getWeight());
        }
    }
}
