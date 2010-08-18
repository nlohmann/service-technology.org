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


#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include "cmdline.h"
#include "config.h"
#include "config-log.h"
#include "decomposition.h"
#include "pnapi/pnapi.h"
#include "verbose.h"
//#include "recompose.h"

using std::cerr;
using std::cin;
using std::cout;
using std::endl;
using std::ifstream;
using std::map;
using std::ofstream;
using std::setfill;
using std::setw;
using std::string;
using std::stringstream;
using std::vector;
using std::set;
using pnapi::PetriNet;
using pnapi::Place;
using pnapi::Label;
using pnapi::Transition;


/// statistics structure
struct Statistics {
    // runtime
    time_t t_total_;
    time_t t_computingComponents_;
    time_t t_buildingComponents_;

    // net statistics
    int netP_;
    int netT_;
    int netF_;

    // biggest fragment
    int largestNodes_;
    int largestP_;
    int largestT_;
    int largestF_;
    int largestPi_;
    int largestPo_;

    // average fragment
    // each must be devided by #fragments
    int averageP_;
    int averageT_;
    int averageF_;
    int averagePi_;
    int averagePo_;

    // #fragments
    int fragments_;
    // #trivial fragments
    int trivialFragments_;

    // clearing the statistics
    void clear() {
        t_total_ = t_computingComponents_ = t_buildingComponents_ = 0;
        netP_ = netT_ = netF_ = 0;
        largestP_ = largestT_ = largestF_ = largestPi_ = largestPo_ = 0;
        averageP_ = averageT_ = averageF_ = averagePi_ = averagePo_ = 0;
        fragments_ = trivialFragments_ = 0;
        largestNodes_ = 0;
    }
} _statistics;


/// the command line parameters
gengetopt_args_info args_info;

/// evaluate the command line parameters
void evaluateParameters(int, char**);
/// statistical output
void statistics(PetriNet&, vector<PetriNet*> &);






int main(int argc, char* argv[]) {
    evaluateParameters(argc, argv);
    // initial clearing
    _statistics.clear();

    string invocation;
    // store invocation in a string for meta information in file output
    for (int i = 0; i < argc; ++i) {
        invocation += string(argv[i]) + " ";
    }

    time_t start_total = time(NULL);
    /*
     * 1. Reading Petri net via PNAPI
     */
    PetriNet net;
    try {
        ifstream inputfile;
        inputfile.open(args_info.inputs[0]);
        if (inputfile.fail()) {
            abort(1, "could not open file '%s'", args_info.inputs[0]);
        }
        switch (args_info.mode_arg) {
            case(mode_arg_standard): {
                //find out whether it is a .lola or a .owfn (for the final marking)
                std::string s = args_info.inputs[0];
                if (s.find(".owfn") != string::npos) { //compare(s.size()-4,4, "owfn")==0)
                    inputfile >> pnapi::io::owfn >> net;
                } else {
                    inputfile >> pnapi::io::lola >> net;
                }
                break;
            }
            case(mode_arg_freechoice):
                inputfile >> pnapi::io::owfn >> net;
                break;
            default:
                break;
        }
        inputfile.close();
    } catch (pnapi::exception::InputError e) {
        abort(2, "could not parse file '%s': %s", args_info.inputs[0], e.message.c_str());
    }
    status("reading file '%s'", args_info.inputs[0]);

    /*
     * 2. Determine the mode and compute the service parts.
     */
    int* tree = NULL;
    int size, psize;
    map<int, Node*> reremap;
    size = net.getNodes().size();
    psize = net.getPlaces().size();
    tree = new int [size];

    time_t start_computing = time(NULL);
    switch (args_info.mode_arg) {
        case(mode_arg_standard):
            _statistics.fragments_ = decomposition::computeComponentsByUnionFind(net,
                                                                                 tree, size, psize, reremap);
            break;
        case(mode_arg_freechoice):
            _statistics.fragments_ = 0;
            break;
        default:
            break; /* do nothing */
    }
    time_t finish_computing = time(NULL);
    _statistics.t_computingComponents_ = finish_computing - start_computing;
    status("computed components of net '%s'", args_info.inputs[0]);

    /*
     * 3. Build Fragments.
     */
    vector<PetriNet*> nets(size);
    for (int j = 0; j < size; j++) {
        nets[j] = NULL;
    }

    // save start time of building
    time_t start_building = time(NULL);
    switch (args_info.mode_arg) {
        case(mode_arg_standard):
            decomposition::createOpenNetComponentsByUnionFind(nets,net, tree, size, psize, reremap);
            break;
        case(mode_arg_freechoice):
            break;
        default:
            break;
    }
    // save the finish time and make statistics
    time_t finish_building = time(NULL);
    _statistics.t_buildingComponents_ = finish_building - start_building;
    status("created net fragments of '%s'", args_info.inputs[0]);

	//composes components until a medium sized component is reached, then computes its complement
	if(args_info.compose_flag){
		
		status("compose %d components until a medium sized component is reached, then computes its complement",  _statistics.fragments_);
		string fileprefix;
		string filepostfix = ".owfn";
		if (args_info.prefix_given)
			fileprefix += args_info.prefix_arg;
		fileprefix += args_info.inputs[0];
		fileprefix += ".part";
		
		unsigned int ksmall=0;
        /// leading zeros
		//build a bitset
		//vector<bool> in(args_info.inputs_num), com(args_info.inputs_num);
		status("created net fragments of '%s'", args_info.inputs[0]);
		//std::bitset<16> *in;//in->reset(); //com.reset();
		
		std::vector<pnapi::PetriNet  > parts; //vector of pn2
		
		PetriNet cmplfordiff=net;//this 
		
		for (std::set<pnapi::Place *>::iterator p = cmplfordiff.getPlaces().begin(); p != cmplfordiff.getPlaces().end(); ++p){
			//PNAPI_FOREACH(p, net.getInternalPlaces()) {
			//Place &p1=net.findPlace((*p)->getName());
			//net2.deletePlace(p1);
			//cout << (*p)->getName()<<endl;
			Place * place = cmplfordiff.findPlace((*p)->getName()+"$compl");
			//Place * php= net.findPlace(p->getName());
			//if ( php != NULL) cout<<"naspa";
			if ( (place == NULL && (*p)->getTokenCount()==0))//initial place
			{	
				if ((!(*p)->getPresetArcs().empty())&&(!(*p)->getPostsetArcs().empty())) {
					
					
					Place * nplace = &cmplfordiff.createPlace((*p)->getName()+"$compl", 1,1);
					pnapi::formula::FormulaEqual prop(pnapi::formula::FormulaEqual(*nplace,1));
					cmplfordiff.getFinalCondition().addProposition(prop);//nets[x]->getFinalCondition().formula().output(std::cout);
					//if(p->getName()=="omega") {pnapi::formula::FormulaEqual prop(pnapi::formula::FormulaEqual(*place,0));nets[x]->getFinalCondition().addProposition(prop);}
					//else {pnapi::formula::FormulaEqual prop(pnapi::formula::FormulaEqual(*place,1));nets[x]->getFinalCondition().addProposition(prop);}
					//nets[x]->getFinalCondition().formula().output(std::cout);
					//cout<<php->getPresetArcs().size();
					
					if (cmplfordiff.findPlace((*p)->getName()+"$compl")!=NULL) {
						
						
						//if(place->getTokenCount()||place->getName()==sfp) nets[x]->createArc( *t, *nets[x]->findPlace(place->getName()+"_compl"),(*f)->getWeight());
						set<pnapi::Arc *> preset = (*p)->getPresetArcs();
						for (set<pnapi::Arc *>::iterator f = preset.begin(); f != preset.end(); ++f)
						{
							
							Transition *t = &(*f)->getTransition();
							//nets[j]->createArc(**p ,*t, (*f)->getWeight());
							if(cmplfordiff.findArc(*nplace,*t)==NULL) {cmplfordiff.createArc( *nplace, *t,(*f)->getWeight());
								//cout<<"noyesno"<<endl;
							}
							//else cout<<"nonono"<<endl;
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
		std::ofstream ocompl;
		//ox << pnapi::io::owfn << diff;
		status("creating complement  net fragments of '%s'", args_info.inputs[0]);
		ocompl.open((std::string(fileprefix+"."+filepostfix + ".complement.owfn").c_str()), std::ios_base::trunc);
		//PetriNet net2;
		ocompl << pnapi::io::owfn << cmplfordiff;
		//cout << pnapi::io::owfn << cmplfordiff;
		unsigned int maxi=0,nc=0;
		
		for (int j = 0; j < (int) nets.size(); j++)
			if (nets[j] == NULL)
				continue;
			else
			{		
				++nc;parts.push_back(*nets.at(j));
				//if (net->getInternalPlaces().size()>maxi) {
				maxi+=nets[j]->getPlaces().size();
				//	cout<<nets[j]->getInternalPlaces().size()<<endl;
				//}
			}
		status("%d components", nc);
		std::vector<bool> in(nc,false),com(nc,false);in.flip();
		std::set< std::vector<bool> >all;//stores component compositions (without complement duplicates)
		int averp=maxi/nc; cout<<maxi<<"/"<<nc<<nets.size()<<"="<<averp<<endl;
		map<vector<bool>, PetriNet> small;//stores all smaller composition from previous iterations
		//map<vector<bool>, PetriNet> alln;//stores all balanced components without duplicates  
		
		//since average does not relate to the size of the whole process, we consider medium sizes (fraction) of the original process
		for (int i = 0; i < (int) parts.size(); ++i){			
			//if the component is medium compute the complement and if it si not that big keepit
			/*	if(parts.at(i).getInternalPlaces().size()<=maxi/2){
			 //it is still small store it for future compositions
			 vector<bool> in(nc,false);
			 in[i]=true;//in[j]=true;
			 all.insert(in);
			 //add complement
			 }*/
			
			// if it is medium sized compute the complement
			if((maxi/3<=parts.at(i).getPlaces().size())&&(parts.at(i).getPlaces().size()<=2*maxi/3)){
				//compose
				cout<<maxi/3<<" "<<parts.at(i).getPlaces().size()<<" "<<2*maxi/3<<endl;
				//int digits=parts.size();
				//int z = digits-num.length();
				//ss << setfill('0') << setw(z) << i;
				stringstream ss;
				string num;
				//ss<<i;
				for (int z=0;z<nc;++z ) {
					if ((z==i)){//in[z]) {
						ss<<1;
					}else ss<<0; 
				}
				vector<bool> in(nc,false),inc(nc,false);
				in[i]=true;inc[i]=true;inc.flip();
				if((all.find(in)==all.end()) && (all.find(inc)==all.end()))
					all.insert(in);
				else {
					cout <<endl << "red allert "<<all.size()<<endl;continue;
				}
				
				
				ss >> num; //cout<<"stupid number "<<num<<endl;
				//cout << num<<" "<<filepostfix <<endl;
				std::ofstream o;
				o.open((std::string(fileprefix+"_"+num+filepostfix).c_str()), std::ios_base::trunc);
				o << pnapi::io::owfn << parts.at(i);
				
				//output the net and its complement and the composition first complement interface places
				//PetriNet net=parts.at(i);
				
				//Output output((std::string(argv[0]) + "compl.owfn").c_str(), "open net");
				//output.stream() << pnapi::io::owfn << net; exit(0);
				//ofstream outputfile((std::string(argv[0]) + "compl.owfn").c_str());
				//compute the difference for  each place / transition of the bign net delete the one from the second net
				
				//int z = digits-num.length();
				//ss << setfill('0') << setw(z) << i;
				//ss >> num;
				
				//cout << num<<" "<<filepostfix <<endl;
				///o.open((std::string(fileprefix+num+filepostfix + ".compl.owfn").c_str()), std::ios_base::trunc);
				//PetriNet net2;
				PetriNet diff=complementnet(cmplfordiff, parts.at(i));
				
				
				
				std::ofstream ox;
				//ox << pnapi::io::owfn << diff;
				
				ox.open((std::string(fileprefix+"_"+num+filepostfix + ".compl.owfn").c_str()), std::ios_base::trunc);
				//PetriNet net2;
				ox << pnapi::io::owfn << diff;
				//cout<<fileprefix+"_"+num+filepostfix<<endl;
				std::ofstream oc;
				//ox << pnapi::io::owfn << diff;
				oc.open((std::string(fileprefix+"_"+num+filepostfix + ".composed.owfn").c_str()), std::ios_base::trunc);
				PetriNet net2=parts.at(i);
				set<string> inputplaces1,outputplaces1;
				std::set<Label *> inputlr=parts.at(i).getInterface().getInputLabels();
				for (std::set<Label *>::iterator ssit=inputlr.begin(); ssit!=inputlr.end(); ++ssit) {
					inputplaces1.insert((*ssit)->getName());
				}
				std::set<Label *> outputlr=parts.at(i).getInterface().getOutputLabels();
				for (std::set<Label *>::iterator ssit=outputlr.begin(); ssit!=outputlr.end(); ++ssit) {
					outputplaces1.insert((*ssit)->getName());
				}
				//cout << pnapi::io::owfn<<net2;
				//cout << pnapi::io::owfn<<diff;
				//PetriNet net2=addPattern(net1);
				try{
					status("net2+diff");
					net2.compose(diff, "1", "2");
				}
				catch (pnapi::exception::UserCausedError e) {
					e.output(cout);
				}
				
				
				//inputplaces1, outputplaces1 for the original net2
				//cout<<"not done"<<endl;
				PetriNet net3=addinterfcompl(net2,inputplaces1,outputplaces1);
				
				oc << pnapi::io::owfn << net3;
				
				//compute complement
				//in->reset();
				//in[i]=1;com=in; com.flip();
				//all.insert(in); //allc.insert(com);
				//for(unsigned int i=args_info.inputs_num;i<16;i++){
				//	com.reset(i);
				//}
				//com.reset(i);//
			}
			if (maxi/3>parts.at(i).getPlaces().size()||((maxi/3<=parts.at(i).getPlaces().size())&&(parts.at(i).getPlaces().size()<=2*maxi/3))){
				//cout<<maxi/3<<">"<<parts.at(i).getInternalPlaces().size()<<endl;
				//cout <<i<< "compose with"<<i<<endl;
				//it is small, compose it  // we do not need to store it in the smaller set as we are already taking care of it 
				for(unsigned int j=i+1; j<(int) parts.size();++j){
					//check if ij are composable (code from yasmina)
					//cout <<i<< "compose with"<<j;
					
					
					if(!areComposable(parts.at(i),parts.at(j))){	
						std::cout<<endl<<"not syntactically compatible"<<endl; 
					}
					else if(parts.at(i).getPlaces().size()+parts.at(j).getPlaces().size()>maxi*2/3){
						std::cout<<endl<<"unbalanced composition skipped (>2/3)"<<endl;
					}
					else { std::cout<<endl<<2*maxi/3<<"> "<<parts.at(i).getPlaces().size()+parts.at(j).getPlaces().size()<<endl;
						//store the small ones
						
						
						//if (maxi/2>=parts.at(i).getInternalPlaces().size()+parts.at(j).getInternalPlaces().size()) {
						//	
						//}
						//compose them (adding complementary places for former interface places
						PetriNet nnb=deletePattern(parts.at(i));
						set<string> inputplaces1,outputplaces1;
						std::set<Label *>  inputl=parts.at(i).getInterface().getInputLabels();
						for (std::set<Label *>::iterator ssit=inputl.begin(); ssit!=inputl.end(); ++ssit) {
							inputplaces1.insert((*ssit)->getName());
						}
						std::set<Label *>  outputl=parts.at(i).getInterface().getOutputLabels();
						for (std::set<Label *>::iterator ssit=outputl.begin(); ssit!=outputl.end(); ++ssit) {
							outputplaces1.insert((*ssit)->getName());
						}
						
						//first delete the enforcing patterns in both nets
						
						PetriNet nnj=parts.at(j);
						PetriNet dnnj=deletePattern(nnj);
						cout << pnapi::io::owfn << nnb;
						cout << pnapi::io::owfn << dnnj;
						try {
							//nnb.compose(dnnj);
							dnnj=deleteEmptyPorts(dnnj);
							nnb=deleteEmptyPorts(nnb);
							status("dnnj+nnb");
							dnnj.compose(nnb, "1", "2");
							cout << "hello"<<endl;
						}
						catch (pnapi::exception::UserCausedError e) {
							cout << "error"<<endl;
							e.output(cout);
						}
						
						//cout << pnapi::io::owfn << nnb;dnnj
						PetriNet nn=addinterfcompl(nnb, inputplaces1,outputplaces1);
						
						vector<bool> in(nc,false);
						in[i]=true;in[j]=true;
						//all.insert(in);
						small[in]=nn;////store it for the future also the composition
						cout<<"small="<<small.size()<<endl;
						
						if(maxi/3>=parts.at(i).getPlaces().size()+parts.at(j).getPlaces().size()){
							std::cout<<endl<<"unbalanced composition (too small)"<<endl;
							continue;
							
						}
						
						//after composition one needs to add complement places for the former interface places
						
						stringstream ss;
						string num;
						for (int z=0;z<nc;++z ) {
							if ((z==i)||(z==j)){//in[z]) {
								ss<<in[z];
							}else ss<<0; 
						}
						ss >> num;
						// check
						vector<bool> inc(nc,false);//complement
						in[i]=true;inc[i]=true;in[j]=true;inc[j]=true;inc.flip();
						if((all.find(in)==all.end()) && (all.find(inc)==all.end()))
							all.insert(in);
						else {
							cout <<endl << "red allert"<<endl;
						}
						
						//output the net , complement and composition
						
						ss >> num; //cout<<"stupid number "<<num<<endl;
						//cout << num<<" "<<filepostfix <<endl;
						std::ofstream o;
						
						//status("add pattern");
						PetriNet nnp=addPattern(nn);
						//status("finished adding pattern");
						o.open((std::string(fileprefix+"_"+num+filepostfix).c_str()), std::ios_base::trunc);
						o << pnapi::io::owfn << nnp;//output component with pattern (composition ij)
						
						cout<<fileprefix+"_"+num+filepostfix<<endl;
						
						//compute complement 
						PetriNet diff=complementnet(cmplfordiff, nn);
						PetriNet xdiff=diff;
						/*for (std::set<pnapi::Place *>::iterator t = diff.getPlaces().begin(); t != diff.getPlaces().end(); ++t){
						 
						 if ((*t)->getPostsetArcs().empty() && (*t)->getPresetArcs().empty()) {
						 cout<<"throw out "<<(*t)->getName()<<endl;
						 Place *pp=xdiff.findPlace((*t)->getName());
						 if(pp!=NULL)xdiff.deletePlace(*pp);//
						 
						 }
						 }*/
						//delete some extra accumulated complement places from former interfaces
						for (std::set<pnapi::Place *>::iterator p = diff.getPlaces().begin(); p != diff.getPlaces().end(); ++p){
							if (nn.findPlace((*p)->getName())!=NULL) {
								cout << "help"<<endl;
							}
							if ((*p)->getPresetArcs().empty() && (*p)->getPostsetArcs().empty()) {
								cout << "help"<<endl;
							}
						}	
						std::ofstream ox;
						ox.open((std::string(fileprefix+"_"+num+filepostfix + ".compl.owfn").c_str()), std::ios_base::trunc);
						cout<<fileprefix+"_"+num+filepostfix + ".compl.owfn"<<endl;
						ox << pnapi::io::owfn << xdiff;//its complement
						std::ofstream oc;
						//ox << pnapi::io::owfn << diff;
						oc.open((std::string(fileprefix+"_"+num+filepostfix + ".composed.owfn").c_str()), std::ios_base::trunc);
						cout<<fileprefix+"_"+num+filepostfix + ".composed.owfn"<<endl;
						//PetriNet net2;//whole system again and again
						set<string> inputplaces2,outputplaces2;
						std::set<Label *> inputlnn=nn.getInterface().getInputLabels();
						for (std::set<Label *>::iterator ssit=inputlnn.begin(); ssit!=inputlnn.end(); ++ssit) {
							inputplaces2.insert((*ssit)->getName());
						}
						std::set<Label *> outputlnn=nn.getInterface().getOutputLabels();
						for (std::set<Label *>::iterator ssit=outputlnn.begin(); ssit!=outputlnn.end(); ++ssit) {
							outputplaces2.insert((*ssit)->getName());
						}
						try{
							status("nnp+diff");
							nnp.compose(diff, "1", "2");//nnp.compose(diff, "1", "2");
						}
						catch (pnapi::exception::UserCausedError e) {
							e.output(cout);
						}
						//compose net with pattern 
						PetriNet xn=addinterfcompl(nnp, inputplaces2,outputplaces2);
						
						oc << pnapi::io::owfn << xn;
						
						
						//this is already done should not be here
						//if(2*maxi/3>parts.at(i).getInternalPlaces().size()+parts.at(j).getInternalPlaces().size()){
						//	std::cout<<endl<<"unbalanced composition store for future use"<<endl;
						//	vector<bool> in(nc,false);
						//	in[i]=true;in[j]=true;small[in]=nn;
						//map<bitset
						
						//	all.insert(in);// all contains the set of combinations that will be used in the future to obtain  boolsets  with 3 bit set
						//store it for the future
						//all other with less than 2 bits set will be deleted (they are not needed)
						//}
						//}
					}
					
					
					
					//if(nets.at(j)->getInternalPlaces().size()>=averp){
					//compute complement
					//in->reset();in->set(i);in->set(j);//com=in.flip();
					//all.insert(in); //all.insert(com);
					//for(unsigned int i=args_info.inputs_num;i<16;i++){
					//	com.reset(i);
					//}
					//com.reset(i);//
					//}
					//else ;//COMPOSE IJ
					//}
				}
			}			
			
		}
		map<vector<bool>,PetriNet> smallp;
		
		unsigned int d=nc/2-2;
		cout <<"n components "<<nc<<" small comp of size 2 " << small.size()<<" no of iterations"<< d <<endl;
		while(d!=0){
			--d;
			for (int i=0;i < (int) parts.size(); i++) {
				//for all members in the map
				
				map<vector<bool>,PetriNet>::iterator it;
				for ( it=small.begin() ; it != small.end(); ++it ){
					cout << i <<"composed with "; 
					
					//(*it).first << " => " << (*it).second << endl;
					if (!(*it).first[i]) {// i+i is forbidden by default 
						//cannot combine anything
						//do the same checks with (*it).second and parts.at(i);
						//modify smallp the previous then small
						PetriNet pp=(*it).second;
						for (int z=0;z<nc;++z ) {
							//if ((*it).first(z==i)||(z==j)){//in[z]) {
							cout<<(*it).first[z];
							//}else ss<<0; 
						}cout <<endl;
						
						if (!areComposable(parts.at(i),pp)){
							std::cout<<endl<<"not syntactically compatible"<<endl; 
						}
						else if(parts.at(i).getPlaces().size()+pp.getPlaces().size()>maxi*2/3){
							std::cout<<endl<<"unbalanced composition skipped (greater)"<<endl;
						}
						else {
							std::cout<<endl<<2*maxi/3<<"> "<<parts.at(i).getPlaces().size()+pp.getPlaces().size()<<endl;
							//store the small ones
							//if (maxi/2>=parts.at(i).getInternalPlaces().size()+parts.at(j).getInternalPlaces().size()) {
							//	
							//}
							//compose them (adding complementary places for former interface places
							PetriNet nnn=deletePattern(parts.at(i));
							set<string> inputplaces1,outputplaces1;
							
							std::set<Label *> inputl=parts.at(i).getInterface().getInputLabels();
							for (std::set<Label *>::iterator ssit=inputl.begin(); ssit!=inputl.end(); ++ssit) {
								inputplaces1.insert((*ssit)->getName());
							}
							std::set<Label *> outputl=parts.at(i).getInterface().getOutputLabels();
							for (std::set<Label *>::iterator ssit=outputl.begin(); ssit!=outputl.end(); ++ssit) {
								outputplaces1.insert((*ssit)->getName());
							}
							try{
								nnn=deleteEmptyPorts(nnn);
								pp=deleteEmptyPorts(pp);
								status("nnn+pp");
								nnn.compose(pp, "1", "2");
								nnn=deleteEmptyPorts(nnn);
							}
							catch (pnapi::exception::UserCausedError e) {
								//e.output(cout);
							}
							//nnn.compose(pp,"","");
							PetriNet nn=addinterfcompl(nnn, inputplaces1,outputplaces1);
							
							//after composition one needs to add complement places for the former interface places
							///here it may not be right because we do not need to add all enforcing conditions (write a special procedure for that)
							vector<bool> in(nc,false);
							//in[i]=true;in[j]=true;
							in=(*it).first;in[i]=true;
							//all.insert(in);do not insert here it, not yet compared
							smallp[in]=nn;////store it for the future also the composition
							cout<<"smallp size ="<<smallp.size()<<endl;
							
							if(maxi/3>=parts.at(i).getPlaces().size()+pp.getPlaces().size()){
								std::cout<<endl<<"unbalanced composition (too small)"<<endl;
								continue;
								
							}						
							stringstream ss;
							string num;
							for (int z=0;z<nc;++z ) {
								if (in[z]) {
									ss<<in[z];
								}else ss<<0;
							}
							ss >> num;
							//output the net , complement and composition
							
							
							vector<bool> inc=in;
							inc.flip();
							if((all.find(in)==all.end()) && (all.find(inc)==all.end()))
							{	all.insert(in);
								cout <<endl << "increase "<<all.size()<<endl;
								cout<<"output"<<fileprefix+"_"+num+filepostfix<<endl;
								
								//cout << num<<" "<<filepostfix <<endl;
								std::ofstream o;
								PetriNet nnp=addPattern(nn);
								o.open((std::string(fileprefix+"_"+num+filepostfix).c_str()), std::ios_base::trunc);
								o << pnapi::io::owfn << nnp;//composition
								
								PetriNet diff=complementnet(cmplfordiff, nn);
								PetriNet xdiff=diff;
								
								//compute complement
								//PetriNet diff=complementnet(cmplfordiff, nn);
								
								std::ofstream ox;
								ox.open((std::string(fileprefix+"_"+num+filepostfix + ".compl.owfn").c_str()), std::ios_base::trunc);
								ox << pnapi::io::owfn << xdiff;//its complement
								std::ofstream oc;
								//ox << pnapi::io::owfn << diff;
								oc.open((std::string(fileprefix+"_"+num+filepostfix + ".composed.owfn").c_str()), std::ios_base::trunc);
								//PetriNet net2;//whole system again and again
								set<string> inputplaces2,outputplaces2;
								std::set<Label *> inputl=nn.getInterface().getInputLabels();
								for (std::set<Label *>::iterator ssit=inputl.begin(); ssit!=inputl.end(); ++ssit) {
									inputplaces2.insert((*ssit)->getName());
								}
								std::set<Label *> outputl=nn.getInterface().getOutputLabels();
								for (std::set<Label *>::iterator ssit=outputl.begin(); ssit!=outputl.end(); ++ssit) {
									outputplaces2.insert((*ssit)->getName());
								}
								try{
									status("nnp+diff");
									nnp.compose(diff, "1", "2");
								}
								catch (pnapi::exception::UserCausedError e) {
									e.output(cout);
								}
								//nnp.compose(diff, "1", "2");
								PetriNet nnnn=addinterfcompl(nnp, inputplaces2,outputplaces2);
								cout <<endl << "all size="<<all.size()<<endl;
								oc << pnapi::io::owfn << nnnn;
								
								cout<<"adding... "<<fileprefix+"_"+num+filepostfix<<endl;
							}
							else {
								cout <<endl << "red allert "<<all.size()<<endl;
								cout<<"skipping "<<fileprefix+"_"+num+filepostfix<<endl;
								//continue;
							}
							
							
						}
						
					}
				}
				
			}
			small=smallp;
		}
		
		//for components of size 1 and 2 we have obtained all combinations which are less than 1/3 of the average size
		//and have stored them into a set of mappings
		cout<<"Total ="<<all.size()<<endl;
		exit(1);//
	}
	
	
	
    /*
     * 4. Write output files.
     */
    if (!args_info.quiet_flag) {
        // writing file output
        string fileprefix;
        string filepostfix = ".owfn";
        if (args_info.prefix_given) {
            fileprefix += args_info.prefix_arg;
        }
        fileprefix += args_info.inputs[0];
        fileprefix += ".part";

        int netnumber = 0;

        int digits;
        string digits_s;
        stringstream ds;
        ds << _statistics.fragments_;
        ds >> digits_s;
        digits = digits_s.length();

        for (int j = 0; j < (int) nets.size(); j++)
            if (nets[j] == NULL) {
                continue;
            } else {
                stringstream ss;
                string num;
                /// leading zeros
                int z = digits - num.length();
                ss << setfill('0') << setw(z) << netnumber;
                ss >> num;
                ofstream outputfile((fileprefix + num + filepostfix).c_str());
                outputfile << pnapi::io::owfn
                           << meta(pnapi::io::INPUTFILE, args_info.inputs[0])
                           << meta(pnapi::io::CREATOR, PACKAGE_STRING)
                           << meta(pnapi::io::INVOCATION, invocation)
                           << *nets[j];
                netnumber++;
				//if complement compute difference between original net and the 
				//respective complement then complement the difference and add the construction
				if (args_info.complement_flag)
				{
					//for each internal place create a complement
					for (std::set<pnapi::Place *>::iterator p = net.getPlaces().begin(); p != net.getPlaces().end(); ++p){
						//PNAPI_FOREACH(p, net.getInternalPlaces()) {
						//Place &p1=net.findPlace((*p)->getName());
						//net2.deletePlace(p1);
						//cout << (*p)->getName()<<endl;
						Place * place = net.findPlace((*p)->getName()+"$compl");
						//Place * php= net.findPlace(p->getName());
						
						if ( (place == NULL && (*p)->getTokenCount()==0))//initial place
						{	
							if ((!(*p)->getPresetArcs().empty())&&(!(*p)->getPostsetArcs().empty())) {
								
								
								Place * nplace = &net.createPlace((*p)->getName()+"$compl", 1,1);
								pnapi::formula::FormulaEqual prop(pnapi::formula::FormulaEqual(*nplace,1));
								net.getFinalCondition().addProposition(prop);//nets[x]->getFinalCondition().formula().output(std::cout);
								//if(p->getName()=="omega") {pnapi::formula::FormulaEqual prop(pnapi::formula::FormulaEqual(*place,0));nets[x]->getFinalCondition().addProposition(prop);}
								//else {pnapi::formula::FormulaEqual prop(pnapi::formula::FormulaEqual(*place,1));nets[x]->getFinalCondition().addProposition(prop);}
								//nets[x]->getFinalCondition().formula().output(std::cout);
								//cout<<php->getPresetArcs().size();
								
								if (net.findPlace((*p)->getName()+"$compl")!=NULL) {
									//cout << "gasit";
									
									//if(place->getTokenCount()||place->getName()==sfp) nets[x]->createArc( *t, *nets[x]->findPlace(place->getName()+"_compl"),(*f)->getWeight());
									set<pnapi::Arc *> preset = (*p)->getPresetArcs();
									for (set<pnapi::Arc *>::iterator f = preset.begin(); f != preset.end(); ++f)
									{
										
										Transition *t = &(*f)->getTransition();
										//nets[j]->createArc(**p ,*t, (*f)->getWeight());
										if(net.findArc(*nplace,*t)==NULL) net.createArc( *net.findPlace(nplace->getName()), *t,(*f)->getWeight());
										//cout << "created arc from "<<nplace->getName()+"$compl"<<" to "<<t->getName()<<endl;
									}	
									preset = (*p)->getPostsetArcs();
									for (set<pnapi::Arc *>::iterator f = preset.begin(); f != preset.end(); ++f)
									{
										Transition *t = &(*f)->getTransition();
										//nets[j]->createArc(**p ,*t, (*f)->getWeight());
										if(net.findArc( *t,*nplace)==NULL) net.createArc( *t,*net.findPlace(nplace->getName()), (*f)->getWeight());
										//cout << "created arc from "<<place->getName()+"$compl"<<" to "<<t->getName()<<endl;
									}			
									
								}}
							
						}
						
					}		
					//Output output((std::string(argv[0]) + "compl.owfn").c_str(), "open net");
					//output.stream() << pnapi::io::owfn << net; exit(0);
					//ofstream outputfile((std::string(argv[0]) + "compl.owfn").c_str());
					//compute the difference for  each place / transition of the bign net delete the one from the second net
					PetriNet diff=net;//delete internal places
					
					PetriNet rest=complementnet(diff, *nets[j]);

					
					 
					std::ofstream o;
					o.open((std::string(fileprefix+num+filepostfix + ".compl.owfn").c_str()), std::ios_base::trunc);
					//PetriNet net2;
					o << pnapi::io::owfn << rest;
					//cout <<pnapi::io::owfn << net<<endl;
					
					//exit(EXIT_SUCCESS);
				}
				//compose it with the complement
            }
    }

    time_t finish_total = time(NULL);
    _statistics.t_total_ = finish_total - start_total;

    /*
     * 5. Make statistical output.
     */
    statistics(net, nets);

    // memory cleaner
    delete [] tree;
    _statistics.clear();


    exit(EXIT_SUCCESS);
}


/*!
 * Evaluates gengetopt command-line parameters
 */
void evaluateParameters(int argc, char** argv) {
    // set default values
    cmdline_parser_init(&args_info);

    // initialize the parameters structure
    struct cmdline_parser_params* params = cmdline_parser_params_create();

    // call the cmdline parser
    cmdline_parser(argc, argv, &args_info);

    // debug option
    if (args_info.bug_flag) {
        FILE* debug_output = fopen("bug.log", "w");
        fprintf(debug_output, "%s\n", CONFIG_LOG);
        fclose(debug_output);
        fprintf(stderr, "Please send file 'bug.log' to %s.\n", PACKAGE_BUGREPORT);
        exit(EXIT_SUCCESS);
    }

    free(params);
}


/*!
 * Provides statistical output.
 */
void statistics(PetriNet& net, vector<PetriNet*> &nets) {
    if (args_info.statistics_flag) {
        /*
         * 1. Net Statistics
         */
        _statistics.netP_ = (int) net.getPlaces().size() + net.getInterface().getAsynchronousLabels().size();
        _statistics.netT_ = (int) net.getTransitions().size();
        _statistics.netF_ = (int) net.getArcs().size();
        /*
         * 2. Fragment Statistics
         */
        for (int j = 0; j < (int) nets.size(); j++) {
            if (nets[j] == NULL) {
                continue;
            }
            if ((int) nets[j]->getNodes().size() > _statistics.largestNodes_) {
                _statistics.largestNodes_ = nets[j]->getNodes().size() + nets[j]->getInterface().getAsynchronousLabels().size();
                _statistics.largestP_ = nets[j]->getPlaces().size() + nets[j]->getInterface().getAsynchronousLabels().size();
                _statistics.largestT_ = nets[j]->getTransitions().size();
                _statistics.largestF_ = nets[j]->getArcs().size();
                _statistics.largestPi_ = nets[j]->getInterface().getInputLabels().size();
                _statistics.largestPo_ = nets[j]->getInterface().getOutputLabels().size();
            }

            if (nets[j]->getNodes().size() == 1) {
                _statistics.trivialFragments_++;
            }

            _statistics.averageP_ += nets[j]->getPlaces().size() + nets[j]->getInterface().getAsynchronousLabels().size();
            _statistics.averagePi_ += nets[j]->getInterface().getInputLabels().size();
            _statistics.averagePo_ += nets[j]->getInterface().getOutputLabels().size();
            _statistics.averageT_ += nets[j]->getTransitions().size();
            _statistics.averageF_ += nets[j]->getArcs().size();
        }
        /*
         * 3. Statistics Output
         */
        if (!args_info.csv_flag) {
            cout << "*******************************************************" << endl;
            cout << "* " << PACKAGE << ": " << args_info.inputs[0] << endl;
            cout << "* Statistics:" << endl;
            cout << "*******************************************************" << endl;
            cout << "* Petri net Statistics:" << endl;
            cout << "*   |P|= " << (net.getPlaces().size() + net.getInterface().getAsynchronousLabels().size())
                 << " |T|= " << net.getTransitions().size()
                 << " |F|= " << net.getArcs().size() << endl;
            cout << "*******************************************************" << endl;
            cout << "* Number of fragments:         " << _statistics.fragments_ << endl;
            cout << "* Number of trivial fragments: " << _statistics.trivialFragments_ << endl;
            cout << "* Largest fragment:" << endl;
            cout << "*   |P|= " << _statistics.largestP_
                 << " |P_in|= " << _statistics.largestPi_
                 << " |P_out|= " << _statistics.largestPo_
                 << " |T|= " << _statistics.largestT_
                 << " |F|= " << _statistics.largestF_ << endl;
            cout << "* Average fragment:" << endl;
            cout << "*   |P|= " << std::setprecision(2) << (float) _statistics.averageP_ / _statistics.fragments_
                 << " |P_in|= " << std::setprecision(2) << (float) _statistics.averagePi_ / _statistics.fragments_
                 << " |P_out|= " << std::setprecision(2) << (float) _statistics.averagePo_ / _statistics.fragments_
                 << " |T|= " << std::setprecision(2) << (float) _statistics.averageT_ / _statistics.fragments_
                 << " |F|= " << std::setprecision(2) << (float) _statistics.averageF_ / _statistics.fragments_ << endl;
            cout << "*******************************************************" << endl;
            cout << "* Runtime:" << endl;
            cout << "*   Total:     " << _statistics.t_total_ << endl;
            cout << "*   Computing: " << _statistics.t_computingComponents_ << endl;
            cout << "*   Buildung:  " << _statistics.t_buildingComponents_ << endl;
            cout << "*******************************************************"
                 << endl << endl;
        } else {
            /*
             * filename,|P| net,|T| net,|F| net,#fragments,#trivial fragments,
             * |P| lf, |P_in| lf, |P_out| lf, |T| lf, |F| lf,
             * |P| af, |P_in| af, |P_out| af, |T| af, |F| af,
             * total runtime, computing runtime, building runtime
             *
             * where lf = largest fragment, af = average fragment
             */
            cout << args_info.inputs[0] << ","
                 << (net.getPlaces().size() + net.getInterface().getAsynchronousLabels().size()) << ","
                 << net.getTransitions().size() << ","
                 << net.getArcs().size() << ","
                 << _statistics.fragments_ << ","
                 << _statistics.trivialFragments_ << ","
                 << _statistics.largestP_ << ","
                 << _statistics.largestPi_ << ","
                 << _statistics.largestPo_ << ","
                 << _statistics.largestT_ << ","
                 << _statistics.largestF_ << ","
                 << std::setprecision(2) << (float) _statistics.averageP_ / _statistics.fragments_ << ","
                 << std::setprecision(2) << (float) _statistics.averagePi_ / _statistics.fragments_ << ","
                 << std::setprecision(2) << (float) _statistics.averagePo_ / _statistics.fragments_ << ","
                 << std::setprecision(2) << (float) _statistics.averageT_ / _statistics.fragments_ << ","
                 << std::setprecision(2) << (float) _statistics.averageF_ / _statistics.fragments_ << ","
                 << _statistics.t_total_ << ","
                 << _statistics.t_computingComponents_ << ","
                 << _statistics.t_buildingComponents_
                 << endl;
        }
    } else {
        cout << PACKAGE_NAME << ": " << args_info.inputs[0]
             << " - number of components: " << _statistics.fragments_
             << endl;
    }
}
