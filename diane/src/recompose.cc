/*
 *  recompose.cc
 *  
 *
 *  Created by Olivia Oanea on 8/14/10.
 *  Copyright 2010 University of Rostock. All rights reserved.
 *
 */

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
	// adds complement places to a net that has just been composed
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
			
			//if ( php != NULL) cout<<"naspa";
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
			place = nn.findPlace(p->getName()+"$compl");//cout<<*ssit<<endl;
		//cout<<endl<<"not found"<<itwas.size()<<endl;//cout<<"input"<<*ssit<<endl;
		//if(itwas.find(*ssit)!= itwas.end()) cout<<endl<<"help " <<endl;
		//set<string>::iterator it=itwas.find(*ssit);
		//if (it==itwas.end())cout<<endl<<"not found"<<itwas.size()<<endl;else cout<<"found"<<endl;
		//if ( php != NULL) cout<<"naspa";&& itwas.find(*ssit)!= itwas.end()
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


PetriNet complementnet(PetriNet diffc, PetriNet part){
	//given a net (with complementary places) and a component part compute the complement wrt net
	//and add the pattern?  

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
	//delete places which will become interface places
	for (std::set<pnapi::Place *>::iterator p = part.getPlaces().begin(); p != part.getPlaces().end(); ++p){
		if (diff.findPlace((*p)->getName())) {
			//cout<<"deleting "<<(*p)->getName()<<endl;
			//Place *pp=diff.findPlace((*p)->getName());//diff
			diff.deletePlace(*diff.findPlace((*p)->getName()));
		}
	}	
	//status("deleted places and transitions");
	
	//build global port
	//pnapi::Port globalp(diff, "global");
	// add interface labels to transitions which are opposite to  the initial open net
	// delete interface places and their complement places
	std::set<pnapi::Label *> async=part.getInterface().getAsynchronousLabels();
	for (std::set<pnapi::Label *>::iterator p = async.begin(); p != async.end(); ++p){
		if((*p)->getType()==Label::INPUT)
			if (diff.findPlace((*p)->getName())) {
				Place *pp=diff.findPlace((*p)->getName());
				set<pnapi::Arc *> preset = pp->getPostsetArcs();
				for (set<pnapi::Arc *>::iterator f = preset.begin(); f != preset.end(); ++f)
				{
					//avoiding global ports
					pnapi::Port & newport=diff.createPort((*p)->getName());
					// newport = nets[x]->getPort(place->getName());
					Label  &l=newport.addOutputLabel((*p)->getName());
					//Label &l=diff.getInterface().addOutputLabel((*p)->getName());
					//Label *l=new Label(diff,globalp,(*p)->getName(),(*p)->getType());l->mirror();
					Transition *t = &(*f)->getTransition();t->addLabel(l);
					//nets[j]->createArc(**p ,*t, (*f)->getWeight());
					//if(net.findArc(*nplace,*t)==NULL) net.createArc( *net.findPlace(nplace->getName()), *t,(*f)->getWeight());
					//cout << "created arc from "<<nplace->getName()+"$compl"<<" to "<<t->getName()<<endl;
				}	
				//add interface labels to all transitions in diff in the postset of pp (pp->setType(Node::OUTPUT);)
				if(diff.findPlace((*p)->getName()+"$compl")) diff.deletePlace(*diff.findPlace((*p)->getName()+"$compl"));
			}
		if((*p)->getType()==Label::OUTPUT)
			if (diff.findPlace((*p)->getName())) {
				Place *pp=diff.findPlace((*p)->getName());
				set<pnapi::Arc *> preset = pp->getPresetArcs();
				for (set<pnapi::Arc *>::iterator f = preset.begin(); f != preset.end(); ++f)
				{
					pnapi::Port & newport=diff.createPort((*p)->getName());
					// newport = nets[x]->getPort(place->getName());
					Label  &l=newport.addInputLabel((*p)->getName());
					//Label &l=diff.getInterface().addInputLabel((*p)->getName());
					//Label *l=new Label(diff,globalp,(*p)->getName(),(*p)->getType());l->mirror();
					Transition *t = &(*f)->getTransition();t->addLabel(l);
				}	
				//Place *pp=diff.findPlace((*p)->getName());pp->setType(Node::INPUT);
				if(diff.findPlace((*p)->getName()+"$compl")) diff.deletePlace(*diff.findPlace((*p)->getName()+"$compl"));
			}
		if(diff.findPlace((*p)->getName())!=NULL) diff.deletePlace(*diff.findPlace((*p)->getName()));
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
			//	syntb=true;break;
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

PetriNet deleteEmptyPorts(PetriNet net1){
	status("deleting empty ports");
	PetriNet net=net1;
	//net.getInterface().clear();
	//add all non-empty ports to n
	set<string> sp;
	std::map< std::string, pnapi::Port * >  xx=net1.getInterface().getPorts();
	for (std::map< std::string, pnapi::Port *>::iterator ti=xx.begin(); ti!=xx.end(); ++ti) {
		if (!((*ti).second)->isEmpty())
			sp.insert((*ti).first);
	}
	//merge all empty ports
	//merge last empty port with the rest
	pnapi::Port *prevp=net.getInterface().getPort(*sp.begin());
	if(prevp!=NULL){
	set<string>::iterator second=++sp.begin();
	for (std::set<string>::iterator si=second;si!=sp.end();++si){
		pnapi::Port * currp=net.getInterface().getPort(*si);
		net.getInterface().mergePorts(*prevp,*currp,*si);
		cout<<"deleting "<<*si<<endl;
		prevp=net.getInterface().getPort(*si);//contains the last empty port
	}
	std::map< std::string, pnapi::Port * >  xxx=net.getInterface().getPorts();
	for (std::map< std::string, pnapi::Port *>::iterator ti=xxx.begin(); ti!=xxx.end(); ++ti) {
		if (!(*ti).second->isEmpty()&&((*ti).first!=prevp->getName())){
			//cout<<(*ti).first<<prevp->getName()<<endl;
			net.getInterface().mergePorts(*prevp,*(*ti).second,(*ti).first);break;
		}
	}
	pnapi::Port * last=net.getInterface().getPort(*sp.begin());
	}
	return net;
}

bool noInterfaceLabel(set<string> sset, string place){
	//finds 
	if (sset.size()==0) return true;
	for (set<string>::iterator sit=sset.begin(); sit!=sset.end(); ++sit) {
		if(strstr(place.c_str(),sit->c_str())==NULL) return true; 
	}
	return false;
}

pnapi::PetriNet separatePorts(pnapi::PetriNet net){
	
	/*string sn;// new label to be added
	net.createPort(sn);//create a port with the same name
	pnapi::Port & newport=net.getPort(sn);
	Label * iflabel = NULL;
	if (labeltype == Label::INPUT)
	{
		iflabel = &newport.addOutputLabel(labelname);
	}
	else
	{
		iflabel = &newport.addInputLabel(labelname);
	}*/
	
	
	//pnapi::Interface ii=net.getInterface();
	//pnapi::Interface newi=net.getInterface();
	//newi.clear();//cleaning old ports because there is no deleteport
	/*std::map< std::string, pnapi::Port * > pm=net.getInterface().getPorts();
	//for each port gets its labels if there is more than one...
	for (std::map< std::string, pnapi::Port * >::iterator pi=pm.begin(); pi!=pm.end(); ++pi) {
		if(pi->second->getAllLabels().size()>1){
			//split the port
			set<Label *> sl=pi->second->getAllLabels();
			for (set<Label *>::iterator i=sl.begin(); i!=sl.end(); ++i) {
				stringstream sins(stringstream::in);
				sins<<(i);
				string s=pi->first+"_"+sins.str();
				//new port
				Port newp=newi.addPort(s);
				Label &newl=newp.addLabel((*i)->getName(),(*i)->getType());//name and type
			}
		}
		else {
			Port &p=newi.addPort(pi->second);
		
		}
	}*/
	return net; 
}


