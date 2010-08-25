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

#include "recompose.h"

pnapi::PetriNet deletePattern(pnapi::PetriNet nnn){
	
	status("deleting pattern");
	pnapi::PetriNet nn=nnn;
	if (nn.findPlace("a")) {
		nn.deletePlace(*nn.findPlace("a"));
	}
	if (nn.findPlace("b")) {
		nn.deletePlace(*nn.findPlace("b"));
	}
	if (nn.findPlace("c")) {
		nn.deletePlace(*nn.findPlace("c"));
	}
	if (nn.findPlace("d")) {
		nn.deletePlace(*nn.findPlace("d"));
	}
	if (nn.findPlace("e")) {
		nn.deletePlace(*nn.findPlace("e"));
	}
	if (nn.findTransition("f")) {
		nn.deleteTransition(*nn.findTransition("f"));
	}
	if (nn.findTransition("g")) {
		nn.deleteTransition(*nn.findTransition("g"));
	}
	status("finished deleting pattern");
	return nn;
}

pnapi::PetriNet addPattern(pnapi::PetriNet nnn){
	status("adding pattern");
	pnapi::PetriNet diff=nnn;
	//cout << pnapi::io::owfn<<diff;
	
	//first check whether the pattehrn is there already
	Place *pa=&diff.createPlace("a", 1);
	Place *pb=&diff.createPlace("b",  0);
	Place *pc=&diff.createPlace("c",  0);
	Place *pd=&diff.createPlace("d",  1);
	Place *pe=&diff.createPlace("e",  0);
	Transition *tf = &diff.createTransition("f");
	Transition *tg = &diff.createTransition("g");
	diff.createArc(*pc,*tf, 1);diff.createArc(*tf,*pa, 1);
	diff.createArc(*pb,*tf, 1);diff.createArc(*tf,*pb, 1);
	diff.createArc(*pc,*tg, 1);
	diff.createArc(*tg,*pb, 1);diff.createArc(*tg,*pe, 1);diff.createArc(*pd,*tg, 1);
	diff.createArc(*tg,*pa, 1);
	
	pnapi::formula::FormulaEqual propb(pnapi::formula::FormulaEqual(*pe,1));
	diff.getFinalCondition().addProposition(propb);
	pnapi::formula::FormulaEqual prope(pnapi::formula::FormulaEqual(*pb,1));
	diff.getFinalCondition().addProposition(prope);
	pnapi::formula::FormulaEqual propa(pnapi::formula::FormulaEqual(*pa,1));
	diff.getFinalCondition().addProposition(propa);
	
	set<Label *> inputpp=diff.getInterface().getInputLabels();
	for (set<Label *>::iterator pp = inputpp.begin(); pp != inputpp.end(); ++pp){
		const set<Transition *> *stt= &(*pp)->getTransitions();
		for (set<Transition *>::iterator t = stt->begin(); t != stt->end(); ++t){
			if(diff.findArc(*pa,**t)==NULL) diff.createArc(*pa,**t, 1);
			if(diff.findArc(**t,*pc)==NULL) diff.createArc(**t,*pc, 1);
		}
	}
	set<Label *> outputpp=diff.getInterface().getOutputLabels();
	for (set<Label *>::iterator p = outputpp.begin(); p != outputpp.end(); ++p){
		const set<Transition *> *stt= &(*p)->getTransitions();
		for (set<Transition *>::iterator t = stt->begin(); t != stt->end(); ++t){
			if(diff.findArc(*pa,**t)==NULL) diff.createArc(*pa,**t, 1);
			if(diff.findArc(**t,*pc)==NULL) diff.createArc(**t,*pc, 1);
		}
	}
	
	
	return diff;
}

PetriNet addinterfcompl(PetriNet nnn,set<string> inputplaces1, set<string> outputplaces1){
	// adds complementary places for the new interface places to a net that has just been composed
	status("add complement places to a composed net given the interface places");
	set<string> itwas;
	PetriNet nn=nnn;
	for (std::set<Place *>::iterator ssit=nn.getPlaces().begin(); ssit!=nn.getPlaces().end(); ++ssit) {
		itwas.insert((*ssit)->getName());
	}
	for (set<string>::iterator ssit=inputplaces1.begin(); ssit!=inputplaces1.end(); ++ssit) {
		//cout<<"itwas"<<(*ssit)->getName()<<endl;
		Place *p=nn.findPlace(*ssit); //cout<<"was interface"<<p->wasInterface()<<" "<<p->getName()<<endl;
		//cout<<endl<<"found"<<*itwas.find(*ssit)<<endl; && itwas.find(*ssit)!= itwas.end()
		if(p!=NULL  && itwas.find(*ssit)!=itwas.end()){ //create complementary
			//create place
			Place * place = nn.findPlace(p->getName()+"$compl");
			
			if ( place == NULL )
			{
				Place *pc = &nn.createPlace(p->getName()+"$compl",1,1);
				
				pnapi::formula::FormulaEqual prop(pnapi::formula::FormulaEqual(*pc,1));
				nn.getFinalCondition().addProposition(prop);//nets[x]->getFinalCondition().formula().output(std::cout);
				//cout<<php->getPresetArcs().size()
				set<pnapi::Arc *> preset = p->getPresetArcs();
				for (set<pnapi::Arc *>::iterator f = preset.begin(); f != preset.end(); ++f)
				{//create a post rtansition for the complementary
					//it is internal, create an arc for the complementary
					//cout << "gasit"; initially marked
					nn.createArc( *nn.findPlace(p->getName()+"$compl"),(*f)->getTransition(), (*f)->getWeight());
					//	net1.createArc((*f)->getTransition(), *net1->findPlace(p->getName()+"_compl"), (*f)->getWeight());
					//nets[x]->createArc(*netPlace, *t, (*f)->getWeight());
				}
				set<pnapi::Arc *> postset = p->getPostsetArcs();
				for (set<pnapi::Arc *>::iterator f = postset.begin(); f != postset.end(); ++f)
				{
					//it is internal, create an arc for the complementary
					//if (net1.findPlace(place->getName()+"_compl")!=NULL) {
					//cout << "gasit";
					//if(place->getTokenCount()||place->getName()==sfp) nets[x]->createArc( *t, *nets[x]->findPlace(place->getName()+"_compl"),(*f)->getWeight());
					nn.createArc( (*f)->getTransition(), *pc,(*f)->getWeight());
					//}
				}
				
			}
			
		}
	}
	for (set<string>::iterator ssit=outputplaces1.begin(); ssit!=outputplaces1.end(); ++ssit) {
		Place *p=nn.findPlace(*ssit);//cout<<"was interface"<<p->wasInterface()<<" "<<p->getName()<<endl;
		//create place
		Place *place=NULL;
		if(p!=NULL)
			place = nn.findPlace(p->getName()+"$compl");
		if ( place == NULL && itwas.find(*ssit)!=itwas.end())
		{ //cout<<"itwas"<<*ssit<<endl;
			Place *pc = &nn.createPlace(p->getName()+"$compl",  1,1);
			//cout << "built interface"<<endl;
			pnapi::formula::FormulaEqual prop(pnapi::formula::FormulaEqual(*pc,1));
			nn.getFinalCondition().addProposition(prop);//nets[x]->getFinalCondition().formula().output(std::cout);
			//cout<<php->getPresetArcs().size()
			set<pnapi::Arc *> preset = p->getPresetArcs();
			for (set<pnapi::Arc *>::iterator f = preset.begin(); f != preset.end(); ++f)
			{//create a post rtansition for the complementary
				//it is internal, create an arc for the complementary
				//cout << "gasit"; initially marked
				nn.createArc( *nn.findPlace(p->getName()+"$compl"),(*f)->getTransition(), (*f)->getWeight());
				//	net1.createArc((*f)->getTransition(), *net1->findPlace(p->getName()+"_compl"), (*f)->getWeight());
				//nets[x]->createArc(*netPlace, *t, (*f)->getWeight());
			}
			set<pnapi::Arc *> postset = p->getPostsetArcs();
			for (set<pnapi::Arc *>::iterator f = postset.begin(); f != postset.end(); ++f)
			{
				//it is internal, create an arc for the complementary
				//if (net1.findPlace(place->getName()+"_compl")!=NULL) {
				//cout << "gasit";
				//if(place->getTokenCount()||place->getName()==sfp) nets[x]->createArc( *t, *nets[x]->findPlace(place->getName()+"_compl"),(*f)->getWeight());
				nn.createArc(  (*f)->getTransition(),*pc,(*f)->getWeight());
				//}
			}
			
		}
		
	}
		status("added complement places to a composed net given the interface places");
	return nn;
}

// given a net, add the complementary places for internal places
PetriNet addComplementaryPlaces(PetriNet c){

	PetriNet cmplfordiff=c;//add complementary places for internal places
	
	for (std::set<pnapi::Place *>::iterator p = cmplfordiff.getPlaces().begin(); p != cmplfordiff.getPlaces().end(); ++p){

		Place * place = cmplfordiff.findPlace((*p)->getName()+"$compl");
		
		if ( (place == NULL && (*p)->getTokenCount()==0))//initial place
		{	
			if ((!(*p)->getPresetArcs().empty())&&(!(*p)->getPostsetArcs().empty())) {
				
				
				Place * nplace = &cmplfordiff.createPlace((*p)->getName()+"$compl", 1,1);
				pnapi::formula::FormulaEqual prop(pnapi::formula::FormulaEqual(*nplace,1));
				cmplfordiff.getFinalCondition().addProposition(prop);
				
				if (cmplfordiff.findPlace((*p)->getName()+"$compl")!=NULL) {
					
					//if(place->getTokenCount()||place->getName()==sfp) nets[x]->createArc( *t, *nets[x]->findPlace(place->getName()+"_compl"),(*f)->getWeight());
					set<pnapi::Arc *> preset = (*p)->getPresetArcs();
					for (set<pnapi::Arc *>::iterator f = preset.begin(); f != preset.end(); ++f)
					{
						
						Transition *t = &(*f)->getTransition();
						//nets[j]->createArc(**p ,*t, (*f)->getWeight());
						if(cmplfordiff.findArc(*nplace,*t)==NULL) {
							cmplfordiff.createArc( *nplace, *t,(*f)->getWeight());
						}
						//if(cmplfordiff.findArc(*nplace,*t)==NULL) cout << "created arc from "<<nplace->getName()+"$compl"<<" to "<<t->getName()<<endl;
					}	
					preset = (*p)->getPostsetArcs();
					for (set<pnapi::Arc *>::iterator f = preset.begin(); f != preset.end(); ++f)
					{
						Transition *t = &(*f)->getTransition();
						//nets[j]->createArc(**p ,*t, (*f)->getWeight());
						if(cmplfordiff.findArc( *t,*nplace)==NULL) cmplfordiff.createArc( *t,*nplace, (*f)->getWeight());
						//cout << "created arc from "<<place->getName()+"$compl"<<" to "<<t->getName()<<endl;
					}			
					
				}}
			
		}
		
	}	
	return cmplfordiff;
}


PetriNet complementnet(PetriNet diffc, PetriNet part){
	//given a net (with complementary places) and a component part compute the complement wrt net
	//and add the pattern  

	status("complement an open net with respect to a process from which it is derived and adds pattern");
	//call 
	PetriNet diff=diffc;
	
	//delete transitions
	for (std::set<pnapi::Transition *>::iterator t = diffc.getTransitions().begin(); t != diffc.getTransitions().end(); ++t){
		
		if (part.findTransition((*t)->getName())!=NULL) {
			Transition *tt=diff.findTransition((*t)->getName());//cout<<(*t)->getName();
			diff.deleteTransition(*tt);//cout<<(tt)->getName()<<endl;
		}
	}	
	//delete (internal) places and complement places of the component
	for (std::set<pnapi::Place *>::iterator p = part.getPlaces().begin(); p != part.getPlaces().end(); ++p){
		if (diff.findPlace((*p)->getName())) {
			//cout<<"deleting "<<(*p)->getName()<<endl;
			//Place *pp=diff.findPlace((*p)->getName());//diff
			diff.deletePlace(*diff.findPlace((*p)->getName()));
		}
	}	
	status("deleted places and transitions");
	
	//build global port
	//pnapi::Port globalp(diff, "global");
	// add interface labels to transitions which are opposite to  the initial open net
	// delete interface places and their complement places
	std::set<pnapi::Label *> async=part.getInterface().getAsynchronousLabels();
	for (std::set<pnapi::Label *>::iterator p = async.begin(); p != async.end(); ++p){
		if((*p)->getType()==Label::INPUT)
			if (diffc.findPlace((*p)->getName())) {
				//delete place and compl in diff
				diff.deletePlace(*diff.findPlace((*p)->getName()));
				if(diff.findPlace((*p)->getName()+"$compl")) diff.deletePlace(*diff.findPlace((*p)->getName()+"$compl"));
				Place *pp=diffc.findPlace((*p)->getName());				
				set<pnapi::Arc *> preset = pp->getPostsetArcs();
				for (set<pnapi::Arc *>::iterator f = preset.begin(); f != preset.end(); ++f)
				{
					//avoiding global ports
					//pnapi::Port & newport=diff.createPort((*p)->getName());
					// newport = nets[x]->getPort(place->getName());
					//Label  &l=newport.addOutputLabel((*p)->getName());
					//the label may be there; check it first
					Label *l=diff.getInterface().findLabel((*p)->getName());
					if(l==NULL) 
						l=&diff.getInterface().addOutputLabel((*p)->getName());
					//Label *l=new Label(diff,globalp,(*p)->getName(),(*p)->getType());l->mirror();
					Transition *t = &(*f)->getTransition();
					Transition *tdiff=diff.findTransition(t->getName());
					if (tdiff!=NULL) tdiff->addLabel(*l,(*f)->getWeight());
					//nets[j]->createArc(**p ,*t, (*f)->getWeight());
					//if(net.findArc(*nplace,*t)==NULL) net.createArc( *net.findPlace(nplace->getName()), *t,(*f)->getWeight());
					//cout << "created arc from "<<nplace->getName()+"$compl"<<" to "<<t->getName()<<endl;
				}	
			}
		if((*p)->getType()==Label::OUTPUT)
			if (diffc.findPlace((*p)->getName())) {
				if(diff.findPlace((*p)->getName())) diff.deletePlace(*diff.findPlace((*p)->getName()));
				if(diff.findPlace((*p)->getName()+"$compl")) diff.deletePlace(*diff.findPlace((*p)->getName()+"$compl"));
				Place *pp=diffc.findPlace((*p)->getName());
				set<pnapi::Arc *> preset = pp->getPresetArcs();
				for (set<pnapi::Arc *>::iterator f = preset.begin(); f != preset.end(); ++f)
				{
					Label *l=diff.getInterface().findLabel((*p)->getName());
					if(l==NULL) 
						l=&diff.getInterface().addInputLabel((*p)->getName());
					//Label *l=new Label(diff,globalp,(*p)->getName(),(*p)->getType());l->mirror();
					Transition *t = &(*f)->getTransition();
					Transition *tdiff=diff.findTransition(t->getName());
					if(tdiff!=NULL) tdiff->addLabel(*l,(*f)->getWeight());

				}	
				//Place *pp=diff.findPlace((*p)->getName());pp->setType(Node::INPUT);
				if(diff.findPlace((*p)->getName()+"$compl")) diff.deletePlace(*diff.findPlace((*p)->getName()+"$compl"));
			}
		//if(diff.findPlace((*p)->getName())!=NULL) diff.deletePlace(*diff.findPlace((*p)->getName()));
	}
	
	// call add pattern
	diff=addPattern(diff);
	return diff;
}

bool areComposable(pnapi::PetriNet net1, pnapi::PetriNet net2){
	status("are composable");
	bool syntb=false;
	set<string> inputplaces1,inputplaces2,outputplaces1,outputplaces2;
	std::set<Label *> inlabels=net1.getInterface().getInputLabels();
	for (std::set<Label *>::iterator ssit=inlabels.begin(); ssit!=inlabels.end(); ++ssit) {
		string sl;
		if ((*ssit)!=NULL) {sl=(*ssit)->getName();
			inputplaces1.insert(sl);}
	}
	std::set<Label *> outlabels=net1.getInterface().getOutputLabels();
	for (std::set<Label *>::iterator ssit=outlabels.begin(); ssit!=outlabels.end(); ++ssit) {
		outputplaces1.insert((*ssit)->getName());
	}
	std::set<Label *> inlabelsp=net2.getInterface().getInputLabels();
	for (std::set<Label *>::iterator ssit=inlabelsp.begin(); ssit!=inlabelsp.end(); ++ssit) {
		inputplaces2.insert((*ssit)->getName());
	}
	std::set<Label *> outlabelsp=net2.getInterface().getOutputLabels();
	for (std::set<Label *>::iterator ssit=outlabelsp.begin(); ssit!=outlabelsp.end(); ++ssit) {
		outputplaces2.insert((*ssit)->getName());
	}
	
	std::set<Label *> asyncl=net1.getInterface().getAsynchronousLabels();
	for (std::set<Label *>::iterator ssit=asyncl.begin(); ssit!=asyncl.end(); ++ssit) {
		//see whether it is an input place 
		string ps=(*ssit)->getName();
		if ((inputplaces1.find(ps)!=inputplaces1.end())&&(outputplaces2.find(ps)!=outputplaces2.end())) {
			//if ((net1.getInputPlaces().find(*ssit)!=net1.getInputPlaces().end())&&(net2.getOutputPlaces().find(*ssit)!=net2.getOutputPlaces().end())) {
			//cout << "we consider only simple composition; we are considering instance composition for next versions of the tool";
				syntb=true;break;
		}
		else if ((inputplaces2.find(ps)!=inputplaces2.end())&&(outputplaces1.find(ps)!=outputplaces1.end()))
		{ syntb=true;break;//matching
		}
	}
	std::set<Label *> synclabels=net1.getInterface().getSynchronousLabels();
	for(std::set<pnapi::Label *>::iterator cit=synclabels.begin();cit!=synclabels.end();++cit){
		if (net2.getInterface().findLabel((*cit)->getName())!=NULL) {syntb=true;break;//not sync
		}
	}
	return syntb;

}



bool noInterfaceLabel(set<string> sset, string place){
	//finds 
	if (sset.size()==0) return true;
	for (set<string>::iterator sit=sset.begin(); sit!=sset.end(); ++sit) {
		if(strstr(place.c_str(),sit->c_str())==NULL) return true; 
	}
	return false;
}



// checks whether two components are composable based on their interfaces (ports are liberal)
// we only consider components with asynchronous interface
bool areAsyncComposable(Component c1, Component c2)
{
	status("are composable");
	bool syntb=false;
	//all async labels
	std::set<string> asyncl1;
	for (std::set<string>::iterator ssit=c1.inputl.begin(); ssit!=c1.inputl.end(); ++ssit) {
		asyncl1.insert(*ssit);
	}
	for (std::set<string>::iterator ssit=c1.outputl.begin(); ssit!=c1.outputl.end(); ++ssit) {
		asyncl1.insert(*ssit);
	}
	
	for (std::set<string>::iterator ssit=asyncl1.begin(); ssit!=asyncl1.end(); ++ssit) {
		//see whether it is an input place in the first net and an output place in the second
		if ((c1.inputl.find(*ssit)!=c1.inputl.end())&&(c2.outputl.find(*ssit)!=c2.outputl.end())) {
				syntb=true;
				break;
		}
		else // the other way 
			if ((c2.inputl.find(*ssit)!=c2.inputl.end())&&(c1.outputl.find(*ssit)!=c1.outputl.end()))
			{ 
				syntb=true;
				break;//matching the other way
			}
	}
	return syntb;
}

//composes two components into a new one
Component asyncCompose(Component c1, Component c2){
	//we assume that they are already composable and come from the same process
	//find common labels
	Component result;
	//set the process of the result
	result.process=c1.process;status("%d places in compose", c2.process.getPlaces().size());
	std::set<string> asyncl1;
	for (std::set<string>::iterator ssit=c1.inputl.begin(); ssit!=c1.inputl.end(); ++ssit) {
		asyncl1.insert(*ssit);
	}
	for (std::set<string>::iterator ssit=c1.outputl.begin(); ssit!=c1.outputl.end(); ++ssit) {
		asyncl1.insert(*ssit);
	}
	std::set<string> common;
	for (std::set<string>::iterator ssit=asyncl1.begin(); ssit!=asyncl1.end(); ++ssit) {
		//see whether it is an input place in the first net and an output place in the second
		if ((c1.inputl.find(*ssit)!=c1.inputl.end())&&(c2.outputl.find(*ssit)!=c2.outputl.end())) {
			common.insert(*ssit);
		}
		else // the other way 
			if ((c2.inputl.find(*ssit)!=c2.inputl.end())&&(c1.outputl.find(*ssit)!=c1.outputl.end()))
			{ 
				common.insert(*ssit);//matching the other way
			}
	}
	//set input labels
	for (std::set<string>::iterator ssit=c1.inputl.begin(); ssit!=c1.inputl.end(); ++ssit) {
		//if not common add it
		if (common.find(*ssit)==common.end()){
			result.inputl.insert(*ssit);
		}
	}
	for (std::set<string>::iterator ssit=c2.inputl.begin(); ssit!=c2.inputl.end(); ++ssit) {
		//if not common add it
		if (common.find(*ssit)==common.end()){
			result.inputl.insert(*ssit);
		}		
	}
	//set output labels
	for (std::set<string>::iterator ssit=c1.outputl.begin(); ssit!=c1.outputl.end(); ++ssit) {
		//if not common add it
		if (common.find(*ssit)==common.end()){
			result.outputl.insert(*ssit);
		}
	}
	for (std::set<string>::iterator ssit=c2.outputl.begin(); ssit!=c2.outputl.end(); ++ssit) {
		//if not common add it
		if (common.find(*ssit)==common.end()){
			result.outputl.insert(*ssit);
		}		
	}	
	//set the string (bitwise)
    string vb;//(c1.cname.size());
	for (int i=0; i<c1.cname.size(); ++i) {
		if((c1.cname.substr(i,1)=="1")||((c2.cname.substr(i,1)=="1")) )
			result.cname+="1";//cout << "help"<<endl;
			else result.cname+="0";
	}
	//set the size
	result.sizep=c1.sizep+c2.sizep;
	//and the original process
	result.process=c1.process;
	return result;
}



//creates a component given the representative combination, the Petri net component and the process itself
Component makeComponent(std::pair<vector<bool>, PetriNet> c, PetriNet process){
	Component result;
	result.process=process;//initial process
	//interface of the component
	std::set<Label *> inputl=c.second.getInterface().getInputLabels();
	for (std::set<Label *>::iterator ssit=inputl.begin(); ssit!=inputl.end(); ++ssit) {
		result.inputl.insert((*ssit)->getName());
	}
	cout << inputl.size()<<endl;
	std::set<Label *> outputl=c.second.getInterface().getOutputLabels();
	for (std::set<Label *>::iterator ssit=outputl.begin(); ssit!=outputl.end(); ++ssit) {
		result.outputl.insert((*ssit)->getName());
	}
	//component name from the original combination
	stringstream ss;
	string num;
	for (unsigned int z=0;z<c.first.size();++z ) {
		if (c.first[z]) {
			ss<<c.first[z];
		}
		else ss<<0;
	}
	ss >> num; 
	result.cname=num;
	result.sizep=c.second.getPlaces().size();//place size
	return result;
}

// builds a net from a Component structure (i.e. from the interface places and the original process)
PetriNet makeComponentNet(Component c){
	
  	PetriNet cn;
	set<string> setn; //builds up the set of nodes of the new component
	deque<string> sdq; //contains a set of nodes of the new component
	// for each asynchronous label get the transitions and add respective label
	std::set<string> asyncl1;cout<< c.inputl.size()<<endl;
	for (std::set<string>::iterator ssit=c.inputl.begin(); ssit!=c.inputl.end(); ++ssit) {
		asyncl1.insert(*ssit);
		Place *porig=c.process.findPlace(*ssit);
		if (porig==NULL) {
			status("%d input place %s not found", c.process.getPlaces().size(),ssit->c_str());
		}
		//label might be there already
		pnapi::Label *netLabel = cn.getInterface().findLabel(*ssit);
		if(netLabel==NULL) netLabel = &cn.getInterface().addInputLabel(*ssit);
		set<pnapi::Arc *> postset = porig->getPostsetArcs();
		for (set<pnapi::Arc *>::iterator f = postset.begin(); f != postset.end(); ++f) {
			//create labels and transitions in cn
			Node &torig=(*f)->getTargetNode();
			//check whether it can be created
			Transition *t=cn.findTransition(torig.getName());
			if(cn.findTransition(torig.getName())==NULL){
				t = &cn.createTransition(torig.getName());
				sdq.push_front(t->getName());
			}
			t->addLabel(*netLabel, 1);
		}	
	}
	
	for (std::set<string>::iterator ssit=c.outputl.begin(); ssit!=c.outputl.end(); ++ssit) {
		asyncl1.insert(*ssit);
		Place *porig=c.process.findPlace(*ssit);
		if (porig==NULL) {status("%d output place %s not found", c.process.getPlaces().size(),ssit->c_str());}
		set<pnapi::Arc *> preset = porig->getPresetArcs();
		pnapi::Label *netLabel=cn.getInterface().findLabel(*ssit);
		if(cn.getInterface().findLabel(*ssit)==NULL ){
			netLabel = &cn.getInterface().addOutputLabel(*ssit);
		}
		for (set<pnapi::Arc*>::iterator f = preset.begin(); f != preset.end(); ++f) {
			//create labels and transitions in cn
			Node & torig=(*f)->getSourceNode();
			Transition *t=cn.findTransition(torig.getName());
			//check whether it can be created
			if(cn.findTransition(torig.getName())==NULL){
				t = &cn.createTransition(torig.getName());
				sdq.push_front(t->getName());
			}
			//the label might be there already

			t->addLabel(*netLabel, 1);//if(*ssit=="merge.s00000815.activated") cout<<torig.getName()<<"merge.s00000815.activateddown"<<endl;
		}	
	}
	setn=asyncl1;
	while(!sdq.empty()){
		//for each transition create the adjacent places and compute successors
		Transition *yt=c.process.findTransition(sdq.front());//in the process
		Transition *ytc=cn.findTransition(sdq.front());//in the component
		set<Node *> adjacentn;//prer,postr,prer1,postr1;
		set<pnapi::Arc*> postset = yt->getPostsetArcs();
		for (set<pnapi::Arc*>::iterator f = postset.begin(); f != postset.end(); ++f) {
			Node  &torig=(*f)->getTargetNode();
			Place *pi=cn.findPlace(torig.getName());
			if((cn.findPlace(torig.getName())==NULL)&&(asyncl1.find(torig.getName())==asyncl1.end())) {// and not interface
				Place *r=c.process.findPlace(torig.getName());
				//if it is there then skip it else create it, compute successors and predecessors
				adjacentn=setUnion(adjacentn,r->getPreset());adjacentn=setUnion(adjacentn,r->getPostset()); 
				pi=&cn.createPlace(r->getName(), r->getTokenCount());// create place and arc
			}
			if(asyncl1.find(torig.getName())==asyncl1.end()) cn.createArc(*ytc, *pi, (*f)->getWeight());
		}
		set<pnapi::Arc*> preset = yt->getPresetArcs();
		for (set<pnapi::Arc*>::iterator f = preset.begin(); f != preset.end(); ++f) {
			Node & torig=(*f)->getSourceNode();
			Place *pi=cn.findPlace(torig.getName());
			if((cn.findPlace(torig.getName())==NULL)&&(asyncl1.find(torig.getName())==asyncl1.end())) {
				Place *r=c.process.findPlace(torig.getName());
				adjacentn=setUnion(adjacentn,r->getPreset());adjacentn=setUnion(adjacentn,r->getPostset());
				//prer1=r->getPreset();postr1=r->getPostset();
				pi=&cn.createPlace(r->getName(), r->getTokenCount());// create place and arc
			}
			if(asyncl1.find(torig.getName())==asyncl1.end()) 
				cn.createArc(*pi, *ytc, (*f)->getWeight());
		}	
		sdq.pop_front();
		//for these places only get the set of of adjacent transitions that have not been visited
		//add them to sdq for the next iteration and create them in the component
		for (set<Node*>::iterator f = adjacentn.begin(); f != adjacentn.end(); ++f)
			if(cn.findTransition((*f)->getName())==NULL) {
				sdq.push_back((*f)->getName());
				Transition &t = cn.createTransition((*f)->getName());
			}
		//set<Node *> prer,postr,prer1,postr1;
		//delete visited
		
	}
	cout<<"size of the set of places"<<cn.getTransitions().size()<<endl;
	return cn;
}

