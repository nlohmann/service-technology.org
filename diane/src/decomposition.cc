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
#include "verbose.h"
#include "cmdline.h"
//#include "recompose.h"


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

	if (args_info.service_flag) {
		
        net.getFinalCondition().dnf();
        const pnapi::formula::Formula* f = dynamic_cast<const pnapi::formula::Formula*>(&net.getFinalCondition().getFormula());
		
        std::set<std::string> ss;//set of marked places final markings
        if (typeid(*f) == typeid(pnapi::formula::Disjunction)) {
            const pnapi::formula::Disjunction* fd = dynamic_cast<const pnapi::formula::Disjunction*>(f);
            for (std::set<const pnapi::formula::Formula*>::iterator cIt = fd->getChildren().begin(); cIt != fd->getChildren().end(); ++cIt) {
                if (((*cIt)->getType() == pnapi::formula::Formula::F_CONJUNCTION)) { //typeid(*cIt)==typeid(pnapi::formula::Conjunction)){
                    const pnapi::formula::Conjunction* fc = dynamic_cast<const pnapi::formula::Conjunction*>(*cIt);
                    for (std::set<const pnapi::formula::Formula*>::iterator ccIt = fc->getChildren().begin(); ccIt != fc->getChildren().end(); ++ccIt) {
                        const pnapi::formula::Proposition* fp = dynamic_cast<const pnapi::formula::Proposition*>(*ccIt);
                        //const Place *pp=new Place(fp->getPlace());
                        if (fp->getTokens() > 0) { //insert only if it is non-zero
                            ss.insert(fp->getPlace().getName());
                        }
                    }
                }
				
            }
        }
        if (typeid(*f) == typeid(pnapi::formula::Conjunction)) {
            const pnapi::formula::Conjunction* fc = dynamic_cast<const pnapi::formula::Conjunction*>(f);
            for (std::set<const pnapi::formula::Formula*>::iterator cIt = fc->getChildren().begin(); cIt != fc->getChildren().end(); ++cIt) {
                const pnapi::formula::Proposition* fp = dynamic_cast<const pnapi::formula::Proposition*>(*cIt);
                //const Place *pp=new Place(fp->getPlace());
                if (fp->getTokens() > 0) {
                    ss.insert(fp->getPlace().getName());
                }
            }
        }
		
		
        //std::set<const Place * > cp=net.finalCondition().concerningPlaces(); doesn't work :(
		//threshold
		//if (args_info.threshold_given) {
		//	cout << args_info.threshold_arg<< endl;
		//}
		//else{cout << args_info.threshold_arg<< endl;}
		//threshold: it may be given by the user or we may make it net dependent
		unsigned int k=0;
		unsigned int threshold=3;//net.getInternalPlaces().size()/5;
		if (args_info.threshold_given) {
			threshold=args_info.threshold_arg;
		}		
		set<std::string> datas;	
		if (args_info.interface_arg){
			// set of strings involved
			for (unsigned i = 0; i < args_info.interface_given; ++i){
				std::string s=args_info.interface_arg[i];
	 			datas.insert(s);
			}
		}
		
        for (set< Place*>::iterator it = net.getPlaces().begin(); it != net.getPlaces().end(); ++it) {
            const std::string sp = (*it)->getName();
			//if we hit an initially marked place or an finally marked place
            if ((*it)->getTokenCount() || (ss.find(sp) != ss.end())) {
                for (set<Node*>::iterator t = (*it)->getPreset().begin(); t != (*it)->getPreset().end(); ++t) {
                    Union(remap[*it], remap[*t], tree);
                }
                for (set<Node*>::iterator t = (*it)->getPostset().begin(); t != (*it)->getPostset().end(); ++t) {
                    Union(remap[*it], remap[*t], tree);
                }
            }//avoiding interface places and using threshold
			else if(args_info.threshold_given && noInterfaceLabel(datas,sp))//(strstr(sp.c_str(),"Data")==NULL )
					{//(strstr(sp.c_str(),"Control")!=NULL)&&
						//status("threshold");
						if((k!=threshold)) k++;
						else {
							k=0;continue;
						}
				for (set<Node *>::iterator t = (*it)->getPreset().begin(); t != (*it)->getPreset().end(); ++t)
					Union(remap[*it], remap[*t], tree);
				for (set<Node *>::iterator t = (*it)->getPostset().begin(); t != (*it)->getPostset().end(); ++t)
					Union(remap[*it], remap[*t], tree);//}
					}
			}
        }
    
	
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


void decomposition::createOpenNetComponentsByUnionFind(vector<PetriNet*> &nets, PetriNet &net,int* tree, int size, int psize, map<int, Node*> &reremap) {
	const pnapi::formula::FormulaGreater *fge=NULL;const pnapi::formula::FormulaEqual *fe=NULL;
	const pnapi::formula::Proposition *fp;
	std::string sfp;
	std::set< const Place * > ep=net.getFinalCondition().getFormula().getEmptyPlaces();
	
	
	if (args_info.service_flag)
	{ // find out the final conditions for the services
		
		net.getFinalCondition().dnf(); 
		const pnapi::formula::Formula * f=dynamic_cast<const pnapi::formula::Formula *>(&net.getFinalCondition().getFormula());
		if(typeid(*f)==typeid(pnapi::formula::Conjunction)){
			const pnapi::formula::Conjunction *fc=dynamic_cast<const pnapi::formula::Conjunction *>(f);
			for(std::set<const pnapi::formula::Formula *>::iterator cIt=fc->getChildren().begin();cIt!=fc->getChildren().end();++cIt){
				fp=dynamic_cast<const pnapi::formula::Proposition *>(*cIt);
				if(ep.find(&fp->getPlace())!=ep.end()) continue;//cout<<fp->getPlace().getName()<<endl;//continue;
				sfp=fp->getPlace().getName();//fp->output(std::cout);cout<<"greater"<<endl;
				//find out whether it is an equality or an inequality
				if(typeid(**cIt)==typeid(pnapi::formula::FormulaEqual)){//cout<<"hallo"<<sfp<<endl;
					fe=dynamic_cast<const pnapi::formula::FormulaEqual *>(*cIt);
					//fe->output(cout);
				}
				if(typeid(**cIt)==typeid(pnapi::formula::FormulaGreater)){
					fge=dynamic_cast<const pnapi::formula::FormulaGreater *>(*cIt);//fge->output(cout);
					//cout<<"hallo"<<sfp<<endl;
					
				}				
				
				//const Place *pp=new Place(fp->getPlace());
				//if(fp->getTokens()>0)
				//	ss.insert( fp->getPlace().getName() );
			}
		}
	}	
	//cout<<"sfp"<<fe->getTokens()<<endl;
    //create components, and services respectively
	for (int i = 0; i < psize; ++i) {
        if (tree[i] == -1) {
            continue;    // interface place found
        }

        int x = Find(i, tree);
        if (nets[x] == NULL) {
            nets[x] = new PetriNet();
        }
        Place* p = static_cast<Place*>(reremap[i]);
        Place *pi=&nets[x]->createPlace(p->getName(), p->getTokenCount());
		//Place *pi=&nets[x]->createPlace(p->getName(), Node::INTERNAL, p->getTokenCount());
		
		//the final marking and the initial marking
		if (args_info.service_flag){
			/*const Place *pp=&fp->getPlace();*/
			//cout<<"hallo"<<sfp<<endl;//
			if (p->getName()==sfp) {
				if (fe!=NULL) {
					pnapi::formula::FormulaEqual prop(pnapi::formula::FormulaEqual(*pi,fe->getTokens()));
					nets[x]->getFinalCondition().addProposition(prop);
					//cout<<"hallo"<<sfp<<endl;//
					//nets[x]->getFinalCondition().formula().output(std::cout);
				}
				else {cout<<"greater"<<std::endl;
					pnapi::formula::FormulaGreater prop(pnapi::formula::FormulaGreater(*pi,fge->getTokens()));
					nets[x]->getFinalCondition().addProposition(prop);///fge->output(cout);
					
				}
			}
			//net[x]->getFinalCondition()=net->getFinalCondition().getFormula();
			// add a place and arcs
			//add complement places -- //make it safe
			if (args_info.threshold_given){
				Place * place = nets[x]->findPlace(p->getName()+"$compl");
				//Place * php= net.findPlace(p->getName());
				//if ( php != NULL) cout<<"naspa";
				if ( (place == NULL && p->getTokenCount()==0))
				{
					place = &nets[x]->createPlace(p->getName()+"$compl", 1,1);
					pnapi::formula::FormulaEqual prop(pnapi::formula::FormulaEqual(*place,1));
					nets[x]->getFinalCondition().addProposition(prop);//nets[x]->getFinalCondition().formula().output(std::cout);
				//if(p->getName()=="omega") {pnapi::formula::FormulaEqual prop(pnapi::formula::FormulaEqual(*place,0));nets[x]->getFinalCondition().addProposition(prop);}
				//else {pnapi::formula::FormulaEqual prop(pnapi::formula::FormulaEqual(*place,1));nets[x]->getFinalCondition().addProposition(prop);}
				//nets[x]->getFinalCondition().formula().output(std::cout);
				//cout<<php->getPresetArcs().size();
				}
			}
		}
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
            Place* netPlace = nets[x]->findPlace(place->getName());
            if (netPlace == NULL) { //it's not a place
				// but it could be an output label already, check that first
					bool islabel=false;
				if(args_info.threshold_given){
				set<pnapi::Label *> outputl=nets[x]->getInterface().getOutputLabels();
				for (set<pnapi::Label *>::iterator oi=outputl.begin(); oi!=outputl.end(); ++oi) {
					if ((*oi)->getName()==place->getName()) {
						islabel=true;
						//cout<<"is label"<<std::endl;
						//t->removeLabel(**oi);
						std::map< std::string, pnapi::Port * >  xx=nets[x]->getInterface().getPorts();
						for (std::map< std::string, pnapi::Port *>::iterator ti=xx.begin(); ti!=xx.end(); ++ti) {
							pnapi::Port * p=(*ti).second;
							p->removeLabel((*oi)->getName());
						}
						
						//make a place from it and create complement place
						try{
							netPlace=&nets[x]->createPlace(place->getName(),0);
						}
						catch (pnapi::exception::UserCausedError e) {
							e.output(cout);cout<<"create"<<std::endl;
						}
						if (nets[x]->findPlace(place->getName()+"$compl")==NULL) {
							Place *placec = &nets[x]->createPlace(netPlace->getName()+"$compl", 1,1);
							pnapi::formula::FormulaEqual prop(pnapi::formula::FormulaEqual(*placec,1));
							nets[x]->getFinalCondition().addProposition(prop);
						}
						nets[x]->createArc(*netPlace, *t, (*f)->getWeight());
						//build a complement 
						//if (args_info.complementary_flag){
						if (nets[x]->findPlace(place->getName()+"$compl")==NULL) {
							Place *placec = &nets[x]->createPlace(netPlace->getName()+"$compl", 1,1);
							pnapi::formula::FormulaEqual prop(pnapi::formula::FormulaEqual(*placec,1));
							nets[x]->getFinalCondition().addProposition(prop);
						}
						//it is internal, create an arc for the complementary
						//make it internal 
						if (nets[x]->findPlace(place->getName()+"$compl")!=NULL) {
							//cout <<endl<<place->getName()<< "gasit";// initially marked
							//if(place->getTokenCount()||place->getName()==sfp) nets[x]->createArc( *nets[x]->findPlace(place->getName()+"_compl"),*t, (*f)->getWeight());
							nets[x]->createArc(*t, *nets[x]->findPlace(place->getName()+"$compl"), (*f)->getWeight());
							//cout << "created arc from "<<t->getName()<<" to "<<place->getName()+"$compl"<<endl;
						}
					}
				}
				}
				//make an input label
                if(!islabel) {
					if(nets[x]->getInterface().findLabel(place->getName())==NULL){
					pnapi::Label& netLabel = nets[x]->getInterface().addInputLabel(place->getName());
					t->addLabel(netLabel, (*f)->getWeight());
					//cout << "input"<<netLabel.getName()<<std::endl;
					}
				}
            } else {
				//it is an internal place create its complement
              nets[x]->createArc(*netPlace, *t, (*f)->getWeight());
				//build a complement 			
				if(args_info.threshold_given){//if (args_info.complementary_flag){
					if (nets[x]->findPlace(place->getName()+"$compl")==NULL) {
						Place *placec = &nets[x]->createPlace(netPlace->getName()+"$compl", 1,1);
						pnapi::formula::FormulaEqual prop(pnapi::formula::FormulaEqual(*placec,1));
						nets[x]->getFinalCondition().addProposition(prop);
					}
				}
			}
		}
		//if(args_info.threshold_given){
		set<Arc*> postset = reremap[i]->getPostsetArcs();
		for (set<Arc*>::iterator f = postset.begin(); f != postset.end(); ++f) {
            Place* place = &(*f)->getPlace();
            Place* netPlace = nets[x]->findPlace(place->getName());
            if (netPlace == NULL) {
				//check if it is already a label
				bool islabel=false;
				if(args_info.threshold_given){
				//check if it is an input label
				set<pnapi::Label *> outputl=nets[x]->getInterface().getInputLabels();
				for (set<pnapi::Label *>::iterator oi=outputl.begin(); oi!=outputl.end(); ++oi) {
					if ((*oi)->getName()==place->getName()) {
						islabel=true;
						//cout<<"is label"<<std::endl;
						std::map< std::string, pnapi::Port * >  xx=nets[x]->getInterface().getPorts();
						for (std::map< std::string, pnapi::Port *>::iterator ti=xx.begin(); ti!=xx.end(); ++ti) {
							pnapi::Port * p=(*ti).second;
							p->removeLabel((*oi)->getName());
						}
						//delete the label then make a place from it 
						//const std::set<Transition*> set=(*oi)->getTransitions();
						//for (std::set<Transition*>::iterator ti=set.begin(); ti!=set.end(); ++ti) {
						//	(*ti)->removeLabel(**oi);
						//}
						//t->removeLabel(**oi);
						//if (nets[x]) {
						//	
						//}
						try{
						netPlace=&nets[x]->createPlace(place->getName(),0);
						}
						catch (pnapi::exception::UserCausedError e) {
							e.output(cout);cout<<"create down"<<std::endl;
						}
						if(netPlace!=NULL){if (nets[x]->findPlace(place->getName()+"$compl")==NULL) {
							try{
							Place *placec = &nets[x]->createPlace(netPlace->getName()+"$compl", 1,1);
							
							pnapi::formula::FormulaEqual prop(pnapi::formula::FormulaEqual(*placec,1));
							nets[x]->getFinalCondition().addProposition(prop);
							}catch (pnapi::exception::UserCausedError e) {
								e.output(cout);
							}
						}
						nets[x]->createArc(*t, *netPlace, (*f)->getWeight());
						if (nets[x]->findPlace(place->getName()+"$compl")!=NULL) {
							//cout << "gasit";
							//if(place->getTokenCount()||place->getName()==sfp) nets[x]->createArc( *t, *nets[x]->findPlace(place->getName()+"_compl"),(*f)->getWeight());
							nets[x]->createArc( *nets[x]->findPlace(place->getName()+"$compl"), *t,(*f)->getWeight());
							//cout << "created arc from "<<place->getName()+"$compl"<<" to "<<t->getName()<<endl;
						}}
					}
				}
				}
				if (!islabel){ //make it an output label
					if(nets[x]->getInterface().findLabel(place->getName())==NULL){
					pnapi::Label& netLabel = nets[x]->getInterface().addOutputLabel(place->getName());
						t->addLabel(netLabel, (*f)->getWeight());}
					//cout <<"output"<< netLabel.getName()<<std::endl;
				}
            } 
			else {//it is an internal place
				nets[x]->createArc(*t, *netPlace, (*f)->getWeight());
				if(args_info.threshold_given){
					if (nets[x]->findPlace(place->getName()+"$compl")==NULL) {
						try{
						Place *placec = &nets[x]->createPlace(netPlace->getName()+"$compl", 1,1);
						pnapi::formula::FormulaEqual prop(pnapi::formula::FormulaEqual(*placec,1));
						nets[x]->getFinalCondition().addProposition(prop);
						}
						catch (pnapi::exception::UserCausedError e) {
							e.output(cout);
						}
					}
				
					if (nets[x]->findPlace(place->getName()+"$compl")!=NULL) {
					//cout << "gasit";
					//if(place->getTokenCount()||place->getName()==sfp) nets[x]->createArc( *t, *nets[x]->findPlace(place->getName()+"_compl"),(*f)->getWeight());
						nets[x]->createArc( *nets[x]->findPlace(place->getName()+"$compl"), *t,(*f)->getWeight());
					//cout << "created arc from "<<place->getName()+"$compl"<<" to "<<t->getName()<<endl;
					}
				}	
			}
            }
        }
    
	if (args_info.compose_flag){
	unsigned int xx=0;
	//create the rest of complement arcs
	for (int j = 0; j < (int) nets.size(); j++)
	{ 
		if (nets[j] == NULL)
			continue;
		++xx;
		Place *sink = NULL;
		
		for (set<Place *>::iterator p = nets[j]->getPlaces().begin(); p != nets[j]->getPlaces().end(); ++p){
			//for(p, nets[j]->getPlaces()) {
			sink = *p;
			if ((*p)->getPostset().empty()&&((*p)->getName()!="omega")) {
				status("component '%d' has more than one sink place: '%s'", xx, (*p)->getName().c_str());
				// if (sink != NULL) {
				//status("component '%d' has more than one sink place: '%s' and '%s'", j,sink->getName().c_str(), (*p)->getName().c_str());
				if((*p)->getName().find("$")!=std::string::npos) {//st=s.substr(0,s.find("@"));
					std::string st=(*p)->getName().substr(0,(*p)->getName().find("$"));
					Place *netPlace = nets[j]->findPlace(st);
					if (netPlace != NULL){//if(place->getName()=="Control_s00004783_3") cout<<place->getName()<<"postsetset"<<endl;
						//for all pre transitions build arc from compl to tr
						set<Arc *> preset = netPlace->getPresetArcs();
						for (set<Arc *>::iterator f = preset.begin(); f != preset.end(); ++f)
						{
							Transition *t = &(*f)->getTransition();
							nets[j]->createArc(**p ,*t, (*f)->getWeight());
							//cout << "created arc from "<<t->getName()<<" to "<<(*p)->getName()+"#compl"<<endl;
							if(!(*p)->getPostset().empty()) status("component '%d' does not have more than one sink place: '%s' and '%s'", xx,sink->getName().c_str(), (*p)->getName().c_str());
							else status("error");
						}
					}}
				//}
				
				
			}
			sink=NULL;
			
			///	}
			//	for (set<Place *>::iterator p = nets[j]->getPlaces().begin(); p != nets[j]->getPlaces().end(); ++p){
			//for(p, nets[j]->getPlaces()) {
			if ((*p)->getPreset().empty()&&((*p)->getName()!="alpha")) {
				//if (sink != NULL) {
				status("component '%d' has more than one start place: '%s'", xx, (*p)->getName().c_str());
				sink = *p;//status("component '%d' has more than one start place: '%s' and '%s'", j,sink->getName().c_str(), (*p)->getName().c_str());
				if((*p)->getName().find("$")!=std::string::npos) {//st=s.substr(0,s.find("@"));
					std::string st=(*p)->getName().substr(0,(*p)->getName().find("$"));
					//status("component '%d' has more than one start place: '%s'", x, (*p)->getName().c_str());//"Data_s00000003##s00000720_1#compl"
					Place *netPlace = nets[j]->findPlace(st);status("%s",st.c_str());
					if (netPlace != NULL){//if(place->getName()=="Control_s00004783_3") cout<<place->getName()<<"postsetset"<<endl;
						//for all pre transitions build arc from compl to tr
						set<Arc *> preset = netPlace->getPostsetArcs();
						for (set<Arc *>::iterator f = preset.begin(); f != preset.end(); ++f)
						{
							Transition *t = &(*f)->getTransition();
							nets[j]->createArc(*t,**p, (*f)->getWeight());
							//cout << "created arc from "<<t->getName()<<" to "<<(*p)->getName()+"#compl"<<endl;
							//if(!(*p)->getPreset().empty()) status("component '%d' does not have more than one start place: '%s' and '%s'", x,sink->getName().c_str(), (*p)->getName().c_str());
							//else status("error");
						}
					}}
				//}
				
			}
		}
	}	
	}	
		// add pattern before composing
	if (args_info.compose_flag) { 
		//for each net ensure fireability of transitions
		
		for (int j = 0; j < (int) nets.size(); j++)
		{ 
			if (nets[j] == NULL)
				continue;
		//++x;
		// add to structure
			nets[j]= new PetriNet(addPattern(*nets[j]));		

	}
	
	}
}
