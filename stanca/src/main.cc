#include <config.h>
#include <cassert>
#include <iostream>
#include <sstream>
#include <cstdio>
#include <map>
#include <algorithm>
#include "Formulamea.h"
#include "cmdline.h"
#include "verbose.h"
#include "config-log.h"
#include "config.h"
#include "pnapi.h"
#include <fstream>
#include <sstream>
#include <string>


#include "Output.h"



using pnapi::PetriNet;
using pnapi::Place;
using pnapi::Transition;
using pnapi::Arc;


using namespace std;
using namespace Minisat;
using std::set;
using std::map;
using std::deque;
using std::cout;
using std::cerr;
using std::endl;
using std::set;
using std::vector;
using std::map;
using std::string;
using std::ofstream;
using std::ifstream;
using std::stringstream;
using std::ostringstream;
using std::exception;

using pnapi::io::owfn;
using pnapi::io::lola;

using pnapi::io::dot;
using pnapi::io::meta;
using pnapi::io::nets;
//using pnapi::io::InputError;
using pnapi::io::CREATOR;
using pnapi::io::INPUTFILE;
using pnapi::io::OUTPUTFILE;


/// the command line parameters
gengetopt_args_info args_info;




string invocation;


string toPl(int id);


void initGlobalVariables();
//void evaluateParameters(int argc, char** argv);
/// evaluate the command line parameters
void evaluateParameters(int argc, char** argv) {
    // overwrite invocation for consistent error messages
    //argv[0] = basename(argv[0]);
	
    // store invocation in a std::string for meta information in file output
    for (int i = 0; i < argc; ++i) {
        invocation += std::string(argv[i]) + " ";
    }
	
    // initialize the parameters structure
    struct cmdline_parser_params *params = cmdline_parser_params_create();
	
    // call the cmdline parser
    if (cmdline_parser(argc, argv, &args_info) != 0) {
        abort(7, "invalid command-line parameter(s)");
    }
	
    // debug option
    /*if (args_info.bug_flag) {
        { Output debug_output("bug.log", "configuration information");
			debug_output.stream() << CONFIG_LOG << std::flush; }
        message("please send file 'bug.log' to %s!", PACKAGE_BUGREPORT);
        exit(EXIT_SUCCESS);
    }*/
	
	
	
    // check whether at most one file is given
    if (args_info.inputs_num > 1) {
        abort(4, "at most one input file must be given");
    }
	
    free(params);
}


/// a function collecting calls to organize termination (close files, ...)
void terminationHandler() {
	/* [USER] Add code here */
	
    // print statistics when stanca gets smarter
    /*if (args_info.stats_flag) {
        message("runtime: %.2f sec", (static_cast<double>(clock()) - static_cast<double>(start_clock)) / CLOCKS_PER_SEC);
        //fprintf(stderr, "%s: memory consumption: ", PACKAGE);
        //system((std::string("ps -o rss -o comm | ") + TOOL_GREP + " " + PACKAGE + " | " + TOOL_AWK + " '{ if ($1 > max) max = $1 } END { print max \" KB\" }' 1>&2").c_str());
    }*/
}





int main(int argc, char **argv) {
	time_t start_time;
	
    // set the function to call on normal termination
    atexit(terminationHandler);
	
    /*--------------------------------------.
	 | 0. parse the command line parameters  |
	 `--------------------------------------*/
    evaluateParameters(argc, argv);
    Output::setTempfileTemplate(args_info.tmpfile_arg);
    Output::setKeepTempfiles(args_info.noClean_flag);
	
    /*----------------------.
	 | 1. parse the open net |
	 `----------------------*/
	
	pnapi::PetriNet net;
	
    //try {
        // parse either from standard input or from a given file
        if (args_info.inputs_num == 0) {
        	// parse the open net from standard input
            std::cin >> pnapi::io::owfn >> net;
        } else {
            // strip suffix from input filename (if necessary: uncomment next line for future use)
            //inputFilename = std::string(args_info.inputs[0]).substr(0, std::string(args_info.inputs[0]).find_last_of("."));
			
        	// open input file as an input file stream
            std::ifstream inputStream(args_info.inputs[0]);
            // ... and abort, if an error occurs
            if (!inputStream) {
                abort(1, "could not open file '%s'", args_info.inputs[0]);
            }
			
            // parse the open net from the input file stream
            inputStream >> meta(pnapi::io::INPUTFILE, args_info.inputs[0])
			>> pnapi::io::owfn >> net;
        }
        // additional output if verbose-flag is set
        if (args_info.verbose_flag) {
            std::ostringstream s;
            s << pnapi::io::stat << net;
            status("read net: %s", s.str().c_str());
        }
    //} catch(pnapi::io::InputError error) {
     //   std::ostringstream s;
      //  s << error;
       // abort(2, "\b%s", s.str().c_str());
    //}
		set<std::string> setsiphon;	
	if (args_info.semisiphon_arg){
		// set of strings involved
		
		for (unsigned i = 0; i < args_info.semisiphon_given; ++i){
 			std::string s=args_info.semisiphon_arg[i];
	 			setsiphon.insert(s);
 		}
		if (!args_info.semisiphon_given) {
			//printout all solutions
			status("print out all siphons");
		}
	}
	
	
	set<std::string> settrap;
	if (args_info.semitrap_arg){
		// set of strings involved
		
		for (unsigned i = 0; i < args_info.semitrap_given; ++i){
 			std::string s=args_info.semitrap_arg[i];
			settrap.insert(s);
 		}
		if (!args_info.semitrap_given) {
			status("print out all traps");
				
		}
		
	}
	
	vector<bool> rs;//in case a formula is satisfiable, a satisfying assignment is returned
	vector<int> conflict;//in case a formula is unsatisfiable, a conflict clause is returned
	vector<vector<bool>* >* rsall=NULL;//in case a formula is satisfiable, more satisfying assignments are returned
	
	//some statistics no of places and max id
	
	start_time=clock();
	map<string, vector<unsigned int> > mp;///m(p)(1) is the id of a place
	// id are generated in the order of appearance
    // build formula
	
	//first build the set of all siphons ----
	
	// first initialize variables of order 0
	//int iteration=0;//iterations give the size of the vector
	int id=0;///initial identifier 1
	vector<unsigned int> iv;
	iv.push_back(++id);//this is for the id 0
	//cout <<id;
	PlLit* f = new PlLit(+id,(*net.getPlaces().begin())->getName(),0);// the first id must be used
	for (set<Place *>::iterator itn=net.getPlaces().begin(); itn!= net.getPlaces().end(); ++itn) {
		iv.push_back(++id);//cout<<id;
	}
	iv.push_back(++id);//cout<<id;
	
	mp.insert(pair<string,vector<unsigned int> >((*net.getPlaces().begin())->getName(), iv));
	set<Place *>::iterator next=++net.getPlaces().begin();
/*	for (set<Place *>::iterator it=net.getPlaces().begin(); it!= net.getPlaces().end(); ++it) {
		cout << (*it)->getName()<< " ";
		for(int k=0; k<mp.find((*it)->getName())->second.size(); ++k)
			cout<<mp.find((*it)->getName())->second[k]<<" ";
		cout << endl;
	}
	cout << mp.size()<<endl;*/
	
	MFormula *previousl=NULL, *previous=NULL;
	//here we do two things at a time the first formula plus initialize all copies of the variables with ids
	//int iter=0;
	if (net.getPlaces().size()==1) {
		previousl=f;
	}
	else {
	for (set<Place *>::iterator it=next; it!= net.getPlaces().end(); ++it) {
		///prepare previous formula
		vector<unsigned int> iv;
		iv.push_back(++id);//this is for the id 0
		PlLit* f1 = new PlLit(id,(*it)->getName(),0);//this one must be used
		for (set<Place *>::iterator itn=net.getPlaces().begin(); itn!= net.getPlaces().end(); ++itn) {
			iv.push_back(++id);
		}
		iv.push_back(++id);
		//cout << iv.size();
		mp.insert(pair<string,vector<unsigned int> >((*it)->getName(), iv));
		if (it==next) {
			MDisjunction *d = new MDisjunction(f,f1);
			previousl=d;
		}
		else{
			MDisjunction *d=new MDisjunction(previousl,f1);
			previousl=d;
		}
	}}
	
	cout << "Number of places "<<net.getPlaces().size() << "max id" <<id<<endl;
	
	/*for (set<Place *>::iterator it=net.getPlaces().begin(); it!= net.getPlaces().end(); ++it) {
		cout << (*it)->getName()<< " ";
		for(int k=0; k<mp.find((*it)->getName())->second.size(); ++k)
			cout<<mp.find((*it)->getName())->second[k]<<" ";
		cout << endl;
	}
	cout << mp.size()<<endl;*/
	/// at least one place ///
	if ( args_info.verbose_flag ){cout << "First "  <<previousl->toString()<<endl;}
	if ( args_info.trap_flag || args_info.semitrap_arg){
		//traps need to be computed
		cout << "trap"<<endl;
		for (set<Transition*>::iterator tit=net.getTransitions().begin(); tit!=net.getTransitions().end(); ++tit) {
			//cout << (*tit)->getName()<<" : ";
			///build current net.findTransition((*tit)->getName())
			MFormula  *inprev=NULL;  pnapi::Transition &t=*net.findTransition((*tit)->getName());//const std::string s=t->getName();std::set<Node *> sn;//
			//sn=net.findTransition((*tit)->getName())->getPostset();
			for (std::set<pnapi::Node *>::iterator nit=t.getPreset().begin(); nit!=t.getPreset().end(); ++nit){
				Place * pl = dynamic_cast<Place *> ((*nit));
				/// here find the corresponding place in the mapping  and build an Implication 
				//cout << pl->getName();
				//cout << " ";
				int pid=mp[pl->getName()][0];// the first id for the place; we have to create more
				MFormula *pvs=NULL;/// disjunction of pre-places
				///here build a disjunction
				for (std::set<pnapi::Node *>::iterator nnit=net.findTransition((*tit)->getName())->getPostset().begin(); nnit!=net.findTransition((*tit)->getName())->getPostset().end(); ++nnit){
					Place * pli = dynamic_cast<Place *> (*nnit); 
					int ppid=mp[pli->getName()][0];
					PlLit* fp = new PlLit(ppid,pli->getName(),0);
					if (nnit==net.findTransition((*tit)->getName())->getPostset().begin()) {
						pvs=fp;
					}
					else {
						pvs=new MDisjunction(pvs,fp);
						//if(pvs!=NULL) else pvs=fp;
					}
					
					//cout << pli->getName();
				}
				///implication or nothing
				MFormula *pinp;
				if(!net.findTransition((*tit)->getName())->getPostset().empty())
				pinp = new MImplication(new PlLit(pid,pl->getName(),0), pvs);
				else {pinp =new MNegation(new PlLit(pid,pl->getName(),0));}
				/// make a conjunction with the previous set  watch out it might not exist
				if (nit==net.findTransition((*tit)->getName())->getPreset().begin()) {
					inprev=pinp;
				}
				else if (pinp != NULL) {
					inprev=new MConjunction(inprev,pinp);
				}
				
				//cout << endl;
			}
			if (tit==net.getTransitions().begin()) {
				previous=inprev;
			}
			else if (inprev!= NULL) {////build  the conjunction
				previous = new MConjunction(previous,inprev);
				
			}
			
		}
		if (args_info.semitrap_arg){
			//add  conjunction where all involved variables are 1
			for (set<string>::iterator it=setsiphon.begin(); it!= setsiphon.end(); ++it) {
				//find place
				int pid=mp[*it][0];
				PlLit* fp = new PlLit(pid,*it,0);
				previous=new MConjunction(previous,fp);
			}
		}
	}
	else{
		//siphons have to be computed 
	for (set<Transition*>::iterator tit=net.getTransitions().begin(); tit!=net.getTransitions().end(); ++tit) {
		//cout << (*tit)->getName()<<" : ";
		///build current net.findTransition((*tit)->getName())
		MFormula  *inprev=NULL;  pnapi::Transition &t=*net.findTransition((*tit)->getName());//const std::string s=t->getName();std::set<Node *> sn;//
		//sn=net.findTransition((*tit)->getName())->getPostset();
		for (std::set<pnapi::Node *>::iterator nit=t.getPostset().begin(); nit!=t.getPostset().end(); ++nit){
			Place * pl = dynamic_cast<Place *> ((*nit));
			/// here find the corresponding place in the mapping  and build an Implication 
			//cout << pl->getName();
			//cout << " ";
			int pid=mp[pl->getName()][0];// the first id for the place; we have to create more
			MFormula *pvs=NULL;/// disjunction of pre-places
			///here build a disjunction
			for (std::set<pnapi::Node *>::iterator nnit=net.findTransition((*tit)->getName())->getPreset().begin(); nnit!=net.findTransition((*tit)->getName())->getPreset().end(); ++nnit){
				Place * pli = dynamic_cast<Place *> (*nnit); 
				int ppid=mp[pli->getName()][0];
				PlLit* fp = new PlLit(ppid,pli->getName(),0);
				if (nnit==net.findTransition((*tit)->getName())->getPreset().begin()) {
					pvs=fp;
				}
				else {
					pvs=new MDisjunction(pvs,fp);
				}
				
				//cout << pli->getName();
			}
			///implication
			MImplication *pinp=new MImplication(new PlLit(pid,pl->getName(),0), pvs);
			/// make a conjunction with the previous set  watch out it might not exist
			if (nit==net.findTransition((*tit)->getName())->getPostset().begin()) {
				inprev=pinp;
			}
			else if (pinp !=NULL){
				inprev=new MConjunction(inprev,pinp);
			}
			
			//cout << endl;
		}
		if (tit==net.getTransitions().begin()) {
			previous=inprev;
		}
		else if (inprev!=NULL){////build  the conjunction
			previous = new MConjunction(previous,inprev);
			
		}
		}
		if (args_info.semisiphon_arg){
			//add  conjunction where all involved variables are 1
			for (set<string>::iterator it=settrap.begin(); it!= settrap.end(); ++it) {
				//find place
				int pid=mp[*it][0];
				PlLit* fp = new PlLit(pid,*it,0);
				previous=new MConjunction(previous,fp);
			}
		}
		
	}
	MFormula *ersteit;
	if(previous!=NULL)	ersteit=new MConjunction(previousl, previous);
	else ersteit=previousl;
	
	//
	if(args_info.verbose_flag) cout << "First formula " << ersteit->toString()<<endl;
/*	const MFormula *nef=ersteit->computeIFF();
	cout <<endl<<" IFF "<< nef->toString();
	const MFormula *negf=nif->computeNFF();
*/
	MFormula *fffinal=NULL;
	if (args_info.siphon_flag && args_info.trap_flag ){
	  // find a trap inside a siphon means that we have to check first that there are traps at all
		//and then uses some implications adding the siphon check
		fffinal=ersteit;
		const MFormula *cnf=NULL; //cnf=computeCNF(ersteit);//
		cnf=computeCNF(fffinal);
		cout<<"AfterCNF";
		if(cnf!=NULL) {if(args_info.verbose_flag) cout<<endl<<cnf->toString()<<endl;}
		else {
			cout << "NULL";exit(0);
		}
		vector<vector<int> > inms=CNF2MSF(cnf);
		
		
		if (!minisat(inms,rs,conflict)){ cout << "No marked trap inside a siphon. Reason: the net does not contain any trap"<<endl; exit(0);}
		else {
			//add the marked property to the actual trap formula
			cout << "The net contains a trap"<<endl;
			//cout << rs->size()<<endl;
			bool marked=false;
			for (unsigned int k=0; k<rs.size(); ++k) {
				if(rs.at(k)){ //cout << "rs["<<k<<"] = "<<rs->at(k)<<endl;
					for (set<Place *>::iterator it=net.getPlaces().begin(); it!= net.getPlaces().end(); ++it) {
						// cout << (*it)->getName()<< " ";
						for(unsigned int kk=0; kk<mp.find((*it)->getName())->second.size(); ++kk)
							if(mp.find((*it)->getName())->second[kk]==k+1){
								cout << (*it)->getName()<<" ";//<<"^"<<kk;
									if ((*it)->getTokenCount()) {
										marked=true; 
									}
							}
					}
					//cout << mp.size()<<endl;
					//cout <<" "<< rs->at(k)<<endl;
				}
			}
			cout << endl;
			if (marked==false){ cout << "No marked trap inside a siphon. Reason: the net does not contain any marked trap"<<endl; exit(0);
			}
			//check for siphons with different variables and add corresponding implication (inclusion)
			MFormula *previouss=NULL;
			for (set<Transition*>::iterator tit=net.getTransitions().begin(); tit!=net.getTransitions().end(); ++tit) {
				//cout << (*tit)->getName()<<" : ";
				///build current net.findTransition((*tit)->getName())
				MFormula  *inprev=NULL;  pnapi::Transition &t=*net.findTransition((*tit)->getName());//const std::string s=t->getName();std::set<Node *> sn;//
				//sn=net.findTransition((*tit)->getName())->getPostset();
				for (std::set<pnapi::Node *>::iterator nit=t.getPostset().begin(); nit!=t.getPostset().end(); ++nit){
					Place * pl = dynamic_cast<Place *> ((*nit));
					/// here find the corresponding place in the mapping  and build an Implication 
					//cout << pl->getName();
					//cout << " ";
					int pid=mp[pl->getName()][1];// the first id for the place; we have to create more
					MFormula *pvs=NULL;/// disjunction of pre-places
					///here build a disjunction
					for (std::set<pnapi::Node *>::iterator nnit=net.findTransition((*tit)->getName())->getPreset().begin(); nnit!=net.findTransition((*tit)->getName())->getPreset().end(); ++nnit){
						Place * pli = dynamic_cast<Place *> (*nnit); 
						int ppid=mp[pli->getName()][1];
						PlLit* fp = new PlLit(ppid,pli->getName(),1);
						if (nnit==net.findTransition((*tit)->getName())->getPreset().begin()) {
							pvs=fp;
						}
						else {
							pvs=new MDisjunction(pvs,fp);
						}
						
						//cout << pli->getName();
					}
					///implication
					MImplication *pinp=new MImplication(new PlLit(pid,pl->getName(),1), pvs);
					/// make a conjunction with the previous set  watch out it might not exist
					if (nit==net.findTransition((*tit)->getName())->getPostset().begin()) {
						inprev=pinp;
					}
					else if (pinp!=NULL){
						inprev=new MConjunction(inprev,pinp);
					}
					
					//cout << endl;
				}
				if (tit==net.getTransitions().begin()) {
					previouss=inprev;
				}
				else if(inprev!=NULL){////build  the conjunction
					previouss = new MConjunction(previouss,inprev);
					
				}
				
			}
			MFormula *conjf=NULL;
			if (previouss!=NULL) conjf=new MConjunction(ersteit,previouss);
			//for all places add an implication
			for (set<Place *>::iterator it=net.getPlaces().begin(); it!= net.getPlaces().end(); ++it) {
				int ppidtrap=mp[(*it)->getName()][0];int ppidsiphon=mp[(*it)->getName()][1];
				PlLit* fptrap = new PlLit(ppidtrap,(*it)->getName(),0);PlLit* fpsiphon = new PlLit(ppidsiphon,(*it)->getName(),1);
				conjf=new MConjunction(conjf,new MDisjunction(new MConjunction(fptrap,fpsiphon),new MConjunction(new MNegation(fptrap),fpsiphon)));
			}
			const MFormula *cnfst=NULL; //cnf=computeCNF(ersteit);//
			cnfst=computeCNF(conjf);
			cout<<"AfterCNF";
			if(cnfst!=NULL) {if(args_info.verbose_flag) cout<<endl<<cnfst->toString()<<endl;}
			else {
				cout << "NULL";exit(0);
			}
			vector<vector<int> > inmsst=CNF2MSF(cnf);
			vector<bool> rsst;
			
			if (minisat(inmsst,rsst, conflict)) {
			    cout << "The net contains a siphon with a marked trap inside: ";
				//print out the solution
				for (unsigned int k=0; k<rsst.size(); ++k) {
					if(rsst.at(k)){ //cout << "rsst["<<k<<"] = "<<rsst->at(k)<<endl;
						for (set<Place *>::iterator it=net.getPlaces().begin(); it!= net.getPlaces().end(); ++it) {
							// cout << (*it)->getName()<< " ";
							for(unsigned int kk=0; kk<2; ++kk)//mp.find((*it)->getName())->second.size()
								if(mp.find((*it)->getName())->second[kk]==k+1)
									cout << (*it)->getName()<<" ";//<<"^"<<kk;
							
						}
						//cout << mp.size()<<endl;
						//cout <<" "<< rs->at(k)<<endl;
					}
				}
				cout << endl;
				exit(0);
			}
			else {
				cout << "The net contains no marked trap inside a siphon"<<endl;exit(0);
			}

		}
	}
	if ( args_info.siphon_flag || args_info.semisiphon_arg || args_info.trap_flag || args_info.semitrap_arg ){
		if(args_info.verbose_flag) cout << "First formula " << ersteit->toString()<<endl;
		//call minisat and exit
		fffinal=ersteit;
		const MFormula *cnf=NULL; //cnf=computeCNF(ersteit);//
		cnf=computeCNF(fffinal);
		cout<<"AfterCNF";
		if(cnf!=NULL) {if(args_info.verbose_flag) cout<<endl<<cnf->toString()<<endl;}
		else {
			cout << "NULL";exit(0);
		}
		
		///compute minisat form
		//vector<vector<int> > inms=CNF2MSF(cnf); //
		vector<vector<int> > inms=CNF2MSF(cnf); ///input for minisat
		/*cout <<endl <<"CNF2MSF size "<< inms.size()<<endl;
		for (int r=0; r<inms.size(); ++r) {
			vector<int> vin=inms.at(r);
			for (int q=0; q<vin.size(); ++q) {
				cout << vin.at(q)<<" ";
			}
			cout << endl;
		}
		*/
		if (args_info.all_flag) {
			
			//rs=minisatsimp(inms);
			
			rsall=minisatsimp(inms);
			if (rsall!=NULL) {
				cout << "Print all "<<endl;
				for (unsigned int p=0; p<rsall->size(); ++p) {
					vector<bool> * rr=rsall->at(p);
					for (unsigned int k=0; k<rr->size(); ++k) {
						if(rr->at(k)){ //cout << "rs["<<k<<"] = "<<rs->at(k)<<endl;
						for (set<Place *>::iterator it=net.getPlaces().begin(); it!= net.getPlaces().end(); ++it) {
							// cout << (*it)->getName()<< " ";
							for(unsigned int kk=0; kk<mp.find((*it)->getName())->second.size(); ++kk)
								if(mp.find((*it)->getName())->second[kk]==k+1)
									cout << (*it)->getName()<<" ";//<<"^"<<kk;
							
						}
						//cout << mp.size()<<endl;
						//cout <<" "<< rs->at(k)<<endl;
						}
					}cout<<endl;
				}
				cout<<endl;
				exit(0);
			}
			//}	
			else{
				exit(0);
			}
		}
		else{ 
			//;
			if (minisat(inms,rs,conflict)) {
			
				if( args_info.siphon_flag) cout << "The net contains a siphon"<<endl;
				if( args_info.trap_flag)  cout << "The net contains a trap"<<endl;
				if( args_info.semisiphon_arg) cout << "The net contains a semisiphon"<<endl;
				if( args_info.semitrap_arg)  cout << "The net contains a semitrap"<<endl;
			//cout << rs->size()<<endl;
				for (unsigned int k=0; k<rs.size(); ++k) {
					if(rs.at(k)){ //cout << "rs["<<k<<"] = "<<rs->at(k)<<endl;
						for (set<Place *>::iterator it=net.getPlaces().begin(); it!= net.getPlaces().end(); ++it) {
							// cout << (*it)->getName()<< " ";
							for(unsigned int kk=0; kk<mp.find((*it)->getName())->second.size(); ++kk)
								if(mp.find((*it)->getName())->second[kk]==k+1)
									cout << (*it)->getName()<<" ";//<<"^"<<kk;
						}
				//cout << mp.size()<<endl;
				//cout <<" "<< rs->at(k)<<endl;
					}
				}
				cout << endl;exit(0);
			//rs contains an assignement; we have to map it back to the original places (the formaula does not hold)
			}
			else if (!args_info.all_flag){
			if( args_info.siphon_flag) {cout << "The net does not contain any siphon"<<endl;}
			if( args_info.trap_flag) { cout << "The net does not contain any trap" <<conflict.size()<<endl; }
			if( args_info.semisiphon_arg) {cout << "The net does not contain any semisiphon"<<endl;}
			if( args_info.semitrap_arg) { cout << "The net does not contain any semitrap"<<endl;}
			//printout the conflict
			for (unsigned int k=0; k<conflict.size(); ++k) {
				cout << conflict.at(k);
			}
			cout << endl; exit(0);
		}
			exit(0);}
	}
	
	///first iteration over
	const int n=net.getPlaces().size();
    
	
	MFormula *pit=NULL;//pit=new MDisjunction(fpR,fpR);cout<<pit->toString();
	cout << "Second formula "<<endl;
	for (int i=0; i<n+1; ++i) { //for all identifiers
		//cout << "Iteration "<<i<<endl;
		
		MFormula *schwartzepit=NULL;
		//if (pit!=NULL) cout << "second formula in formation "<< pit->toString()<<endl;cout << "iteration"<<i<<"started"<<endl;
		//for all places build equivalences
		//cout << n<<endl;
		for (std::set<pnapi::Place *>::iterator it=net.getPlaces().begin(); it!= net.getPlaces().end(); ++it) {
			//cout<<"in"<<endl;
			//cout << (*it)->getName() <<" "<<i;
			MFormula *ppit=NULL;
			//cout << " ";
			MFormula *previousc=NULL;//contains the consjunction between pi and the implication conjunction
			int ppid=mp.find((*it)->getName())->second[i];
			PlLit* fp = new PlLit(ppid,(*it)->getName(),i);//preplace
			if (fp==NULL) {
				cout << "help ";
			}			
			// for all postarcs
			const set<Arc *>  postarcs=(*it)->getPostsetArcs();//cout<<"Number of postarcs "<<postarcs.size()<<endl;
			for (std::set<pnapi::Arc *>::iterator ipost=postarcs.begin(); ipost!=postarcs.end(); ++ipost){
				//get posttransitions
				Transition &t=(*ipost)->getTransition();
				//cout<<t.getName()<<endl;
				MFormula *pvs=NULL;/// disjunction of pre-places
				///here build a disjunction with the postplaces of the transition
				for (std::set<pnapi::Node *>::iterator nnit=net.findNode(t.getName())->getPostset().begin(); nnit!=net.findNode(t.getName())->getPostset().end(); ++nnit){
					//Place * pli = dynamic_cast<Place *> (*nnit); 
					int ppid=mp.find((*nnit)->getName())->second[i];
					PlLit* fp = new PlLit(ppid,(*nnit)->getName(),i);
					if (pvs==NULL) {
						pvs=fp;
					}
					else {
						pvs=new MDisjunction(pvs,fp);
					}					
				}
				if (previousc==NULL) {
					previousc=pvs;//new MImplication(fp,pvs);
				}
				else if (pvs!=NULL){////build  the conjunction 
					previousc = new MConjunction(previousc,pvs);//,new MImplication(fp,pvs));
					//cout<<previousc->toString()<<endl;}
				}
				
				
				
			}	
			//new MConjunction(new PlLit(mp.find((*it)->getName())->second[i],(*it)->getName(),i),previousc)
			//cout << "here";
			//if (previousc!=NULL) {cout<<previousc->toString()<<endl;}
			if (ppit==NULL) {
				if (previousc==NULL) {
					ppit = new Equivalence(new PlLit(mp.find((*it)->getName())->second[i+1],(*it)->getName(),i+1), new PlLit(mp.find((*it)->getName())->second[i],(*it)->getName(),i));
				}
				else {
					ppit = new Equivalence(new PlLit(mp.find((*it)->getName())->second[i+1],(*it)->getName(),i+1), new MConjunction(new PlLit(mp.find((*it)->getName())->second[i],(*it)->getName(),i),previousc));
				}

			}
			else {
				if (previousc==NULL) {
					//ppit = new MConjunction(ppit,new Equivalence(new PlLit(mp.find((*it)->getName())->second[i+1],(*it)->getName(),i+1), new PlLit(mp.find((*it)->getName())->second[i],(*it)->getName(),i)));
					ppit = new Equivalence(new PlLit(mp.find((*it)->getName())->second[i+1],(*it)->getName(),i+1), new PlLit(mp.find((*it)->getName())->second[i],(*it)->getName(),i));
				}
				else {
					//ppit = new MConjunction(ppit,new Equivalence(new PlLit(mp.find((*it)->getName())->second[i+1],(*it)->getName(),i+1), new MConjunction(new PlLit(mp.find((*it)->getName())->second[i],(*it)->getName(),i), previousc)));
					ppit = new Equivalence(new PlLit(mp.find((*it)->getName())->second[i+1],(*it)->getName(),i+1), new MConjunction(new PlLit(mp.find((*it)->getName())->second[i],(*it)->getName(),i),previousc));
				}
			}
			//cout<<"end place "<<  (*it)->getName()<< ppit->toString()<<endl;
			
			if(schwartzepit==NULL){
				schwartzepit=ppit; //cout <<schwartzepit->toString()<<"schwartze"<<endl;
			}
			else if (ppit!=NULL){
				//cout <<schwartzepit->toString()<<"kein schwartze"<<endl;
					schwartzepit=new MConjunction(schwartzepit,ppit);
			}
			delete fp;
		}
		if (pit==NULL) {//cout<<"primul";
			pit=schwartzepit;
		}
		else {
			pit= new MConjunction(pit,schwartzepit);
		}
		//cout << pit->toString()<<endl;cout << "iteration"<<i<<"finished"<<endl;
		
	}
	//cout <<endl << "Second formula finished" <<endl;
	//cout << pit->toString()<<endl;
	

	//const MFormula *negf=nif->computeNFF();
	///last part
	MFormula * unmarked=NULL;
		//cout<<endl;
	for (set<Place *>::iterator it=net.getPlaces().begin(); it!= net.getPlaces().end(); ++it) {
		//cout << (*it)->getName()<<endl;
		if ((*it)->getTokenCount()>0) {
			//cout << (*it)->getName()<<endl;
			if (unmarked==NULL) {
				PlLit * hh=new PlLit(mp.find((*it)->getName())->second[n+1],(*it)->getName(),n+1);
				//final=new MNegation(new PlLit(mp.find((*it)->getName())->second[n+1]));
				//cout<<"first";
				unmarked=new MNegation(hh);//cout<<unmarked->toString();
			}
			else {
				//cout<<mp.find((*it)->getName())->second[n+1]<<endl;
				PlLit * hh=new PlLit(mp.find((*it)->getName())->second[n+1],(*it)->getName(),n+1);
				unmarked=new MConjunction(unmarked, new MNegation(hh));
			}
			
		}
	}
	if (unmarked==NULL) {
		cout << "unmarked: empty formula";
	}
//	else {
//		cout<<unmarked->toString();
//	}

	/*cout << "Third formula " <<endl;
	cout << "Fourth formula"<< endl;//
	MFormula *forth;
	for (set<Place *>::iterator it=net.getPlaces().begin(); it!= net.getPlaces().end(); ++it) {
	for (int x=0; x<n+2; ++x) {
		PlLit * hh=new PlLit(mp.find((*it)->getName())->second[x],(*it)->getName(),x);
		forth=new MConjunction(forth, hh);
	}
	
	}&*/
	//
	//cout << unmarked->toString()<<endl;
	//cout << "Final formula "<<endl;
	MFormula *ffinal=NULL;
	/*if ( args_info.siphon_flag ){
		cout << "First formula " <<ersteit->ft<< ersteit->toString()<<endl;
		//call minisat
		ffinal=ersteit;
	}
	else {*/
	 MConjunction *mc1=new MConjunction(ersteit,pit);
	
	if (unmarked!=NULL) {
		ffinal=new MConjunction(mc1,unmarked);
	}
	else {
		ffinal=mc1;
	}

	if (ffinal==NULL) {
		cout << "final formula: something is wrong";
	}
	else{	
		if (args_info.verbose_flag) {
//	else {
		cout<<"Final formula "<<ffinal->toString()<<endl;
	}
	
	}
	//}
	const MFormula *cnf=NULL; //cnf=computeCNF(ersteit);//
	cnf=computeCNF(ffinal);
	cout<<"AfterCNF";
	//if(cnf!=NULL) cout<<endl<<cnf->toString()<<endl;
	///compute minisat form
	vector<vector<int> > inms=CNF2MSF(cnf); ///input for minisat
	cout <<endl <<"CNF2MSF size "<< inms.size()<<endl;
	  /*  for(unsigned int i = 0; i < inms.size(); ++i) {
	        for(unsigned j = 0; j < inms[i].size(); ++j) {
	            printf("%d ", inms[i][j]);
	        }
			cout<<endl;
	    }*/
	/*for (int i=0; i<inms.size(); ++i) {
		vector<int> inside=inms.at(i);
		cout << i<<" clause ";
		for (int j=0; j<inside.at(j); ++j) {
			cout <<" " <<inside.at(j);
		}
		cout << endl;
	}*/
	cout << endl;
	//printout vector to see what happens
	
	//callsat
	//bool r=false;
	
	cout <<endl << "Start SAT"<<endl;
	std::cerr << endl << "Minisat time : runtime: " << ((double(clock()) - double(start_time)) / CLOCKS_PER_SEC) << " sec\n";
	
	bool snmt=false;
	if (minisat(inms,rs,conflict)) {
		//if (args_info.verbose_flag ) {
		cout << rs.size()<<endl;
		
		
		cout << "Siphon ";
		for (unsigned int k=0; k<rs.size(); ++k) {
			if(rs.at(k)){ //cout << "rs["<<k<<"] = "<<rs->at(k)<<endl;
				snmt=true;
				for (set<Place *>::iterator it=net.getPlaces().begin(); it!= net.getPlaces().end(); ++it) {
					// cout << (*it)->getName()<< " ";
					for(unsigned int kk=0; kk<mp.find((*it)->getName())->second.size(); ++kk)
						if(mp.find((*it)->getName())->second[kk]==k+1){
							//cout << (*it)->getName()<<"^"<<kk<<" ";
							if (kk==0) cout << (*it)->getName()<<" ";
							//if (kk>maxkk) maxkk=kk;
						}
				}
				//cout << mp.size()<<endl;
				//cout <<" "<< rs->at(k)<<endl;
			}
			// if there is no trap and the maximal k has been reached warn about it
			else {
				//get minimal kk 
			}
		}
		if (snmt) {cout << "with no trap inside it.";
		cout << endl;
		}//}
		if (!snmt) {// there is a false assignment solution; try to find another one
			//cout << "the formula is satisfiable with empty assignment"<<endl;
			//first add a clause with 1 assignments
			//vector<int> vi;
			//for (int i=0; i<=maxid; ++i) {
			//	vi.push_back(i);
			//}
			//inms.push_back(vi);
			vector<bool>* rsin=minisat2(inms);
			bool snmt=false;
			if (rsin!=NULL) {
				for (unsigned int k=0; k<rsin->size(); ++k) {
					if(rsin->at(k)){ //cout << "rs["<<k<<"] = "<<rs->at(k)<<endl;
						snmt=true;
						for (set<Place *>::iterator it=net.getPlaces().begin(); it!= net.getPlaces().end(); ++it) {
							// cout << (*it)->getName()<< " ";
							for(unsigned int kk=0; kk<mp.find((*it)->getName())->second.size(); ++kk)
								if(mp.find((*it)->getName())->second[kk]==k+1){
									//cout << (*it)->getName()<<"^"<<kk<<" ";
									if (kk==0) cout << (*it)->getName()<<" ";
									else if (k==1) cout << "trap " << (*it)->getName()<<" ";
									else cout << "unmarked " << (*it)->getName()<<" ";
									//if (kk>maxkk) maxkk=kk;
								}
						}
						//cout << mp.size()<<endl;
						//cout <<" "<< rs->at(k)<<endl;
					}
					// ? if there is no trap and the maximal k has been reached warn about it
				}
				cout <<endl<< "the formula is satisfiable"<<endl;exit(0);
			}
			else{
				cout <<endl<< "the formula is not satisfiable"<<endl;exit(0);
			}
			
		}
		else {cout <<endl << "the formula is satisfiable"<<endl;exit(0);
		}
		//for (unsigned int k=0; k<rs->size(); ++k) {
		//	for (set<Place *>::iterator it=net.getPlaces().begin(); it!= net.getPlaces().end(); ++it) {
			// cout << (*it)->getName()<< " ";
		//	 for(unsigned int kk=0; kk<mp.find((*it)->getName())->second.size(); ++kk)
		//	 if(mp.find((*it)->getName())->second[kk]==k)
		//	 cout << (*it)->getName()<<"^"<<kk;
		//	 }
			 //cout << mp.size()<<endl;
		//	cout <<" "<< rs->at(k)<<endl;
		//}
		//r contains an assignement; we have to map it back to the original places (the formaula does not hold)
	}
	else{
		cout << "the formula is not satisfiable"<<endl;exit(0);
		//if(emptytrap) cout<<": no siphon contains a trap. The siphon-trap property does not hold."<<endl;
	}
/*	for (set<Place *>::iterator it=net.getPlaces().begin(); it!= net.getPlaces().end(); ++it) {
		cout << (*it)->getName()<< " ";
		for(int k=0; k<mp.find((*it)->getName())->second.size(); ++k)
		 cout<<mp.find((*it)->getName())->second[k]<<" ";
		cout << endl;
	}
	cout << mp.size()<<endl;*/
	// time
	delete f;
	std::cerr << endl << "Stanca : runtime: " << ((double(clock()) - double(start_time)) / CLOCKS_PER_SEC) << " sec\n";
	//printf("runtime: %.2f sec", (static_cast<double>(clock()) - static_cast<double>(start_time)) / CLOCKS_PER_SEC);
	cout << endl;
	exit(0);
}

