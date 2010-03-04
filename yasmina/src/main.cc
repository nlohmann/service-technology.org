#include "config.h" 
#include <cassert>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include "cmdline.h"
#include "pnapi.h"
#include "lp_lib.h"
#include <sstream>
#include <iostream>
#include <fstream>
#include <set>
#include <typeinfo>
#include "verbose.h" 
#include <utility>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <limits.h>
#include "eventTerm.h"
#include "Output.h"

#include "adapternumerge.h"
//#include "ppl.hh"


using std::cout;
using std::endl;

using pnapi::PetriNet;
using pnapi::Place;
using pnapi::Transition;
//using pnapi::Arc;
using pnapi::Node;

using std::set;
using std::map;
using std::ofstream;
using std::ifstream;
using std::stringstream;
using std::ostringstream;
using std::exception;

using pnapi::io::owfn;
using pnapi::io::lola;
using pnapi::io::formula;
using pnapi::io::dot;
using pnapi::io::meta;
using pnapi::io::nets;
using pnapi::io::InputError;
using pnapi::io::CREATOR;
using pnapi::io::INPUTFILE;
using pnapi::io::OUTPUTFILE;
//using namespace Parma_Polyhedra_Library;
//using namespace Parma_Polyhedra_Library::IO_Operators;

/// the parser constraint vector
std::vector<EventTerm*>* term_vec = 0;

/// the set of inputPlaces and outputPlaces
std::set<std::string> inputPlaces,outputPlaces, synchrT;
/// TYPE OF THE PLACE
unsigned short pType;
/// the current place token as string
std::string IDENT_token;
/// final marking ids std::set<std::string> fm_set;
extern unsigned int nfm;
extern unsigned int nc;

typedef std::pair<int,int>  BoundsMP;
//std::vector<std::vector<BoundsMP> > bMP;
//std::vector<std::pair<int,int> > bMP;

extern int mp_yylineno;
extern int mp_yydebug;
extern int mp_yy_flex_debug;
extern FILE* mp_yyin;
extern int mp_yyerror();
extern int mp_yyparse();
std::vector<std::map<std::string, int> > fmar;

typedef struct {
	unsigned int fmi; //final marking index
	unsigned int termi; //term index
	unsigned int lb;
	unsigned int ub;
} constraintf;
set<const constraintf *> fingerprint;
//set<const pnapi::formula::Formula *>::iterator cIt=f->children().begin();cIt!=f->children().end();++cIt
//gets the bounds for a particular final marking
set<const constraintf *> getconstrforfm(set<const constraintf *> fingerprint, unsigned int fmn){
	set<const constraintf *> vc;
	for (set<const constraintf *>::iterator it=fingerprint.begin(); it!=fingerprint.end(); ++it) {
		if ((*it)->fmi==fmn) {
			vc.insert(*it);
		}
	}
	return vc;
}

/*building a multiset to store adapter rules + rightside - leftside*/
typedef struct{
	std::multiset<unsigned int> msleft;
	std::multiset<unsigned int> msright;
	std::multiset<unsigned int> canonical;
	std::set<std::string> base;
} adapterrule;


#ifdef YY_FLEX_HAS_YYLEX_DESTROY
extern int mp_yylex_destroy();
#endif

std::set<lprec *>  transform(const pnapi::PetriNet &net1, const pnapi::formula::Formula * form);
const pnapi::formula::Formula *  transformNegFormula(const pnapi::PetriNet &net1, const pnapi::formula::Formula * form);


void initialize_mp_parser() {
	mp_yylineno = 1;
	mp_yydebug = 0;
	mp_yy_flex_debug = 0;
	
}



/// the command line parameters
gengetopt_args_info args_info;

/// evaluate the command line parameters
void evaluateParameters(int argc, char** argv) {
	
	// initialize the parameters structure
	struct cmdline_parser_params *params = cmdline_parser_params_create();

	// call the cmdline parser
	cmdline_parser(argc, argv, &args_info);

	// check whether at least one .owfn file or at least a MP is given
	if (((args_info.inputs_num < 1)&&(args_info.messageProfiles_given<1) && (args_info.adapterrules_given<1)) ) {
		fprintf(stderr, "%s: too few arguments --- (at least one input .owfn file) or (at least one .owfn file and one message profile)  or (at least one adapter rule set) must be given -- aborting\n", PACKAGE);
		exit(EXIT_FAILURE);
	}


	free(params);
}



// 1. transform final markings into specific sets
// 2. build the system
// 3. solve the system for different targets

//yet another smart integer Ilp opne net analyser

// the function should 


//this function gets a formula and returns one with negations on atomic prepositions

const pnapi::formula::Formula *  transformNegFormula(const pnapi::PetriNet &net1, const pnapi::formula::Formula * form){
//if it is in postive form gata
//reduce negations 2 by two we first 
/*	std::cout<<"Transform Negation"<<endl;
	//if(typeid(form)==typeid(pnapi::formula::Negation)) form->output(std::cout);
//	else{ 
//		cout<<"The formula is not a negation"<<endl;
//		return form;
//	}
	const pnapi::formula::Negation *ff=dynamic_cast<const pnapi::formula::Negation *>(form);
	//first reduce negations
	const pnapi::formula::Formula* child=*ff->children().begin();
	//child->output(std::cout);
	//const pnapi::formula::Conjunction *f=dynamic_cast<const pnapi::formula::Conjunction *>(form);
	//const pnapi::formula::Formula *ff=dynamic_cast<const pnapi::formula::Negation *>(form);letterboxgrowlmail
		//for(std::set<const pnapi::formula::Formula *>::iterator cIt=f->children().begin();cIt!=f->children().end();++cIt){
	if((ff->children().size()==1) && (typeid(*ff->children().begin())!=typeid(pnapi::formula::Proposition))){
		//std::cout<<"Negation Negation"<<endl;
		//child->output(std::cout);
		const pnapi::formula::Negation *nchild=dynamic_cast<const pnapi::formula::Negation *>(child);
		const pnapi::formula::Formula* grandchild=*nchild->children().begin();
		//grandchild->output(std::cout);
		const pnapi::formula::Negation *ngrandchild=dynamic_cast<const pnapi::formula::Negation *>(grandchild);
		if(typeid(ngrandchild->children().begin())==typeid(pnapi::formula::Negation)){ cout<<"sunt aici";
			return transformNegFormula(net1, grandchild); 
		}
		else {cout<<"ori aici";return grandchild;}
	}
	//treat atomic propositions
	if(typeid(child)==typeid(pnapi::formula::FormulaEqual)){ 
		//std::cout<<"neg Equal"<<endl;
		const pnapi::formula::FormulaEqual *ff=dynamic_cast<const pnapi::formula::FormulaEqual *>(child);
		cout<<ff->place().getName()<<"="<<ff->tokens()<<endl;
		const pnapi::formula::FormulaNotEqual *nff= new pnapi::formula::FormulaNotEqual(ff->place(),ff->tokens());
		return nff;
	}
	else if(typeid(child)==typeid(pnapi::formula::FormulaGreater)){ 
		//std::cout<<"neg Greater"<<endl;
		const pnapi::formula::FormulaGreater *ff=dynamic_cast<const pnapi::formula::FormulaGreater *>(child);
		cout<<ff->place().getName()<<">"<<ff->tokens()<<endl;

		const pnapi::formula::FormulaLessEqual *nff= new pnapi::formula::FormulaLessEqual(ff->place(),ff->tokens());
		return nff;
	}
	else if(typeid(child)==typeid(pnapi::formula::FormulaGreaterEqual)){ 
		//std::cout<<"neg GreaterEqual"<<endl;
		const pnapi::formula::FormulaGreaterEqual *ff=dynamic_cast<const pnapi::formula::FormulaGreaterEqual *>(child);
		//cout<<ff->place().getName()<<">="<<ff->tokens()<<endl;
		if(ff->tokens()==0) return NULL;
		const pnapi::formula::FormulaLess *nff= new pnapi::formula::FormulaLess(ff->place(),ff->tokens());
		return nff;
	}
	else if(typeid(child)==typeid(pnapi::formula::FormulaLessEqual)){ 
		//std::cout<<"neg LessEqual"<<endl;
		const pnapi::formula::FormulaLessEqual *ff=dynamic_cast<const pnapi::formula::FormulaLessEqual *>(child);
		//cout<<ff->place().getName()<<"<="<<ff->tokens()<<endl;
		const pnapi::formula::FormulaGreater * nff= new pnapi::formula::FormulaGreater(ff->place(),ff->tokens());
		return nff;
	}
	else if(typeid(child)==typeid(pnapi::formula::FormulaLess)){
		//std::cout<<"neg Less"<<endl;
		const pnapi::formula::FormulaGreater *ff=dynamic_cast<const pnapi::formula::FormulaGreater *>(child);
		//cout<<ff->place().getName()<<"<="<<ff->tokens()<<endl;
		const pnapi::formula::FormulaGreaterEqual *nff= new pnapi::formula::FormulaGreaterEqual(ff->place(),ff->tokens());
		return nff;
	}
	else if(typeid(child)==typeid(pnapi::formula::FormulaNotEqual)){ //here I should return equal :)
		//std::cout<<"NotEqual"<<endl;
		const pnapi::formula::FormulaNotEqual *ff=dynamic_cast<const pnapi::formula::FormulaNotEqual *>(child);
		//cout<<ff->place().getName()<<"="<<ff->tokens()<<endl;
		const pnapi::formula::FormulaEqual *nff= new pnapi::formula::FormulaEqual(ff->place(),ff->tokens());
		return nff;
	}
	else if(typeid(child)==typeid(pnapi::formula::Disjunction)){ 
		//std::cout<<"neg Disjunction"<<endl;//make disjunction two by two
		const pnapi::formula::Disjunction *f=dynamic_cast<const pnapi::formula::Disjunction *>(child);
		std::set< const pnapi::formula::Formula *> conj;
		for(std::set<const pnapi::formula::Formula *>::iterator cIt=f->children().begin();cIt!=f->children().end();++cIt){	
			//(*cIt)->output(std::cout);
			//cout<<endl;
			conj.insert(transformNegFormula(net1,*cIt));
			//transformNegFormula(*cIt);
			//call neg children return the conjunction of what is returned
//			if(cIt==f->children().begin()){
//				cout<<"primul"<<endl;
				// get the set of of set of constraints and put it into previous
		}
		const pnapi::formula::Conjunction * con=new pnapi::formula::Conjunction(conj);
		return con;
	}
	else if(typeid(child)==typeid(pnapi::formula::Conjunction)){ 
		//std::cout<<"neg Conjunction"<<endl;
		const pnapi::formula::Conjunction *f=dynamic_cast<const pnapi::formula::Conjunction *>(child);
		std::set< const pnapi::formula::Formula *> disj;		
		for(std::set<const pnapi::formula::Formula *>::iterator cIt=f->children().begin();cIt!=f->children().end();++cIt){	
			//(*cIt)->output(std::cout);
			//cout<<endl;
			disj.insert(transformNegFormula(net1,*cIt));
			//call the negation for children and return the disjunction of the results
//			if(cIt==f->children().begin()){
//				cout<<"primul"<<endl;
				// get the set of of set of constraints and put it into previous
		}
		const pnapi::formula::Disjunction * dis=new pnapi::formula::Disjunction(disj);
		return dis;
	}*/
}

			
std::set<lprec *>  transform(const pnapi::PetriNet &net1, const pnapi::formula::Formula * form){
	//std::cout<<"transform"<<std::endl;
	std::set<lprec *> retlpset;
	//REAL rowpl[1+ net1.getPlaces().size()];
	lprec *lppl;
	lppl = make_lp(0,  net1.getPlaces().size());
//	for(int i=0;i<=net1.getPlaces().size();i++)

	//set_verbose(lppl,0);

	if(lppl == NULL) {
		std::cerr<< "Unable to create new LP model"<<std::endl;
	//	return(1);
	}
	// because a net has  set of places and each plac can have tokens we need a vector to associate coordinates of rows with places
/*	std::vector<std::string> vp; int i=1;
	for(std::set<pnapi::Place *>::iterator cit=net1.getPlaces().begin();cit!=net1.getPlaces().end();++cit){
		char * cstr= new char [(*cit)->getName().size()+1];
		strcpy(cstr,(*cit)->getName().c_str());
		vp.push_back((*cit)->getName());set_col_name(lppl,i,cstr);i++;
	}*/
	int i=1;
	for(std::set<pnapi::Place *>::iterator cit=net1.getPlaces().begin();cit!=net1.getPlaces().end();++cit){
		char * cstr= new char [(*cit)->getName().size()+1];
		strcpy(cstr,(*cit)->getName().c_str());
		set_col_name(lppl, i, cstr); i++;
	}
	// now vp[i] has the name of the place on position i
	//form->output(std::cout);
	if(typeid(*form)==typeid(pnapi::formula::Conjunction)){ 
		//std::cout<<"Conjunction "<<std::endl;
		//form->output(std::cout);//cout<<form->children().size()<<endl;
		//const pnapi::formula::Operator *fop=dynamic_cast<const pnapi::formula::Operator *>(form);
		//cout<<"Number of children: ";cout<<fop->children().size()<<endl;
		std::set<lprec *> previous, current;
	        lprec *lpcopy;
		lpcopy = make_lp(0,  net1.getPlaces().size());
		//	for(int i=0;i<=net1.getPlaces().size();i++)
		set_verbose(lpcopy,0);
		if(lpcopy == NULL) {
			std::cerr<< "Unable to create new LP model"<<std::endl;
		//	return(1);
		}

		//set_add_rowmode(lpcopy, TRUE);
		
		const pnapi::formula::Conjunction *f=dynamic_cast<const pnapi::formula::Conjunction *>(form);
		//cout<<endl<<"flag "<<f->flag_<<endl;
		for(std::set<const pnapi::formula::Formula *>::iterator cIt=f->children().begin();cIt!=f->children().end();++cIt){	
			//(*cIt)->output(std::cout);
			//cout<<endl;
			retlpset.clear();
			if(cIt==f->children().begin()||previous.empty()){
				//cout<<"primul"<<endl;
				previous=transform(net1,(*cIt));
				//cout<<"prev size "<<previous.size()<<endl;
				//write_LP(*previous.begin(),stdout);
				// get the set of of set of constraints and put it into previous
			}
			else{ //for each element in the firs and in the second the previous make a new set out of the intersectionconstraints togetner
				current=transform(net1,(*cIt));
				if(current.empty()) {
					//cout<<"continue"<<previous.size()<<current.size()<<endl;
					continue;
				}
				//cout<<"nu vad"<<previous.size()<<current.size()<<endl;
				
				for(std::set<lprec *>::iterator cIt1=previous.begin();cIt1!=previous.end();++cIt1){
					// get the previous set of constraints
//					lpcopy=copy_lp(*cIt1);std::cout<<"previous rows "<<get_Nrows(lpcopy)<<std::endl;
					//cout<<get_row_name(*cIt1,get_Nrows(lpcopy)-1)<<" "<<get_row_name(*cIt1,get_Nrows(lpcopy))<<endl;
					// instead of lpcopy try getting all constraints
					
					//for(int k=1;k<1+ net1.getPlaces().size();k++) cout<<get_mat((*cIt1),0,k)<<" ";
					for(std::set<lprec *>::iterator cIt2=current.begin();cIt2!=current.end();++cIt2){
						//cout<<"current rows"<<get_Nrows(*cIt2)<<endl;
						// create a new copy
						lpcopy=copy_lp(*cIt1);//std::cout<<"previous rows "<<get_Nrows(lpcopy)<<std::endl;
						// print_lp(*cIt2);
						// intersect the set of constraints that is make a new lprec with both of the records in the same set
						// make a new lprec out of prev union currnet set of constraints intersection are joined
						// to perform intersection each lp of the first problem must be intersected with a lp of the second
						//cout<<"nu vad"<<previous.size()<<current.size()<<endl;
						// add all rows from the second
						//set_add_rowmode(lpcopy, TRUE);
						for(int j=1;j<=get_Nrows(*cIt2);j++){
							//for(int k=0;k<1+ net1.getPlaces().size();k++) rowpl[k]=get_mat(*cIt2,j,k);
							int Ncol=net1.getPlaces().size();
							int * colno=new int[Ncol]();
							REAL * rowe= new REAL[Ncol]();
							/* create space large enough for one row */
							//colno = (int *) malloc(Ncol * sizeof(*colno));
							//rowe = (REAL *) malloc(Ncol * sizeof(*rowe));
							
							int nr_of_cols = get_rowex(*cIt2,j,rowe,colno);
							//cout<<"Sizeof "<<sizeof(rowe)<<" and j is " << j << endl;
							//for(int k=0;k<1+ net1.getPlaces().size();k++) cout<<rowpl[k]<<" ";
;							//set_verbose(lpcopy,FULL);//cout<<"Constraint type "<<get_constr_type(*cIt2,j)<<endl;
							if(!add_constraintex(lpcopy, nr_of_cols ,rowe,colno ,get_constr_type(*cIt2,j), get_rh(*cIt2,j)))
								cout<<"gata"<<endl;
							set_row_name(lpcopy,get_Nrows(lpcopy), get_row_name(*cIt2,j));
							//cout<<get_nameindex(lpcopy,get_row_name(lpcopy,get_Nrows(lpcopy)-2),TRUE)<<get_row_name(lpcopy,get_Nrows(lpcopy)-1)<< "am adaugat"<<get_row_name(lpcopy,get_Nrows(lpcopy)-1)<<"si"<<get_row_name(lpcopy,get_Nrows(lpcopy))<<endl;
							//set_verbose(lpcopy,NORMAL);
							//print_lp(lpcopy);
							retlpset.insert(lpcopy);
							
							//cout<<get_rh(lpcopy,j)<<" "<<" row "<<get_Nrows(lpcopy)<<std::endl;
						}
						//set_add_rowmode(lpcopy, FALSE);
						//std::cout<<"first lpcopy "<<get_Nrows(lpcopy)<<endl;
						 //print the result
					}
				}
				//std::cout<<"first "<<retlpset.size()<<" "<<get_Nrows(*retlpset.begin())<<1+ net1.getPlaces().size()<<std::endl;
				previous=retlpset;
			}
		}
		retlpset=previous;
		//std::cout<<"EXIT "<<retlpset.size()<<"Conjunction"<<std::endl;
	}
	else if(typeid(*form)==typeid(pnapi::formula::Disjunction)){ 
		//std::cout<<"Disjunction"<<endl;//make disjunction two by two
		const pnapi::formula::Disjunction *f=dynamic_cast<const pnapi::formula::Disjunction *>(form);
		std::set<lprec *> set2;		
		for(std::set<const pnapi::formula::Formula *>::iterator cIt=f->children().begin();cIt!=f->children().end();++cIt){	
			//(*cIt)->output(std::cout);
			//cout<<endl;
//			if(cIt==f->children().begin()){
//				cout<<"primul"<<endl;
				// get the set of of set of constraints and put it into previous
//			}else{ 
			set2=transform(net1,(*cIt));
			retlpset.insert(set2.begin(),set2.end());	
//			}
			//matke the union two by two
		}
		//cout<<"Size "<<retlpset.size()<<endl;
	}
	//for(std::set<const pnapi::formula::Formula *>::iterator cIt=f->children().begin();cIt!=f->children().end();++cIt){	
		//for all those atomic proposition
		//		const pnapi::formula::FormulaEqual *x=dynamic_cast<const pnapi::formula::FormulaEqual *>(*cIt);
		//		x->output(std::cout);
	//if(typeid(*cIt) != typeid(pnapi::formula::FormulaEqual)){
	else if(typeid(*form)== typeid(pnapi::formula::FormulaEqual)){
		//cout<<endl<<"New formula ";form->output(std::cout);cout<<endl;
		const pnapi::formula::FormulaEqual *ff=dynamic_cast<const pnapi::formula::FormulaEqual *>(form);
		//std::cout<<"Equal "<<ff->tokens()<<endl;
		if(ff->tokens()==0) return retlpset;
		//cout<<endl<<ff->place().getName()<<"="<<ff->tokens()<<endl;

		int Ncol=net1.getPlaces().size(), *colno = NULL, j;
		REAL *rowe = NULL;
		
		/* create space large enough for one row */
		colno = (int *) malloc(Ncol * sizeof(*colno));
		rowe = (REAL *) malloc(Ncol * sizeof(*rowe));
		//try{((colno == NULL) || (row == NULL)) ret = 2;
		
				
		//colno[j] = 2; /* second column */
		//rowe[j++] = 1;
		

		//	REAL row[1+net1.getPlaces().size()];
		//	REAL sparse[1];
		//	int colno[1];
		//set_add_rowmode(lppl, TRUE);
		
		//cout<<get_nameindex(lppl,const_cast<char *>(ff->place().getName().c_str()),FALSE)<<endl;
//		std::vector<std::string>::const_iterator it(std::find(vp.begin(), vp.end(),ff->place().getName()));
			//cout<<it-vp.begin()<<endl;
			//for(int k=0;k<1+ net1.getPlaces().size();k++) rowpl[k]=0;
			//rowpl[it-vp.begin()]=1;colno[0]=it-vp.begin();sparse[0]=1;
		//rowe[1]=1;colno[0]=it-vp.begin();//sparse[0]=1;
		j = 0;colno[j] = get_nameindex(lppl,const_cast<char *>(ff->place().getName().c_str()),FALSE); /* first column */
		
		rowe[j++] = 1; //cout<<colno[0]<<" "<<rowe[0]<<endl;
		
		//cout<<ff->tokens()<<"#";
		//add_constraint(lppl,rowpl, EQ, ff->tokens());		/* add the row to lpsolve */ if(!add_constraintex(lp, j, rowe, colno, LE, 75))
		//cout<<"before add "<<get_Nrows(lppl);
		if(!add_constraintex(lppl,j,rowe,colno, EQ, ff->tokens()))cout<<"nu merge"<<endl;//set_add_rowmode(lppl, FALSE);
		//set_add_rowmode(lppl, FALSE);
		set_row_name(lppl,1,const_cast<char *>(ff->place().getName().c_str()));
		//cout<<"row name "<<get_row_name(lppl,1)<<endl;

		set_mat(lppl,1,get_nameindex(lppl,const_cast<char *>(ff->place().getName().c_str()),FALSE),1);
		//cout<<"this doesn't work "<<get_mat(lppl,1,get_nameindex(lppl,const_cast<char *>(ff->place().getName().c_str()),FALSE)+1);
		//if((colno == NULL) || (rowe == NULL)) cout<<"hehe";
		//cout<<"ultim "<<get_Nrows(lppl);//<<get_Nrows(lppl)<<get_mat(lppl,get_Nrows(lppl), get_Ncolumns(lppl)-1);
		//for(int k=0;k<=1+ net1.getPlaces().size();k++) cout<<"   "<<get_mat(lppl,1,get_nameindex(lppl,const_cast<char *>(ff->place().getName().c_str()),FALSE))<<"   "<<get_mat(lppl,1,k)<<endl;
		//get_mat(lppl,1,get_nameindex(lppl,const_cast<char *>(ff->place().getName().c_str()),FALSE));
		retlpset.insert(lppl);
		//cout<<"ultim "<<get_Nrows(lppl)<<retlpset.size();
		
		//write_LP(lppl,stdout);
	}
	else if(typeid(*form)==typeid(pnapi::formula::FormulaGreater)){ 
			//std::cout<<"Greater"<<endl;
			const pnapi::formula::FormulaGreater *ff=dynamic_cast<const pnapi::formula::FormulaGreater *>(form);
			//cout<<ff->place().getName()<<">"<<ff->tokens()<<endl;

			int Ncol=net1.getPlaces().size(), *colno = NULL, j;
			REAL *rowe = NULL;
		
			/* create space large enough for one row */
			colno = (int *) malloc(Ncol * sizeof(*colno));
			rowe = (REAL *) malloc(Ncol * sizeof(*rowe));
			set_add_rowmode(lppl, TRUE);
		
			//cout<<get_nameindex(lppl,const_cast<char *>(ff->place().getName().c_str()),FALSE)<<endl;
			j = 0;colno[j] = get_nameindex(lppl,const_cast<char *>(ff->place().getName().c_str()),FALSE); /* first column */
			rowe[j++] = 1; //cout<<colno[0]<<" "<<rowe[0]<<endl;
			if(!add_constraintex(lppl,j,rowe,colno, GE, ff->tokens()+1))cout<<"nu merge"<<endl;//set_add_rowmode(lppl, FALSE);
			set_add_rowmode(lppl, FALSE);
			set_row_name(lppl,1,const_cast<char *>(ff->place().getName().c_str()));
			//cout<<"row name "<<get_row_name(lppl,1)<<endl;

			set_mat(lppl,1,get_nameindex(lppl,const_cast<char *>(ff->place().getName().c_str()),FALSE),1);
			retlpset.insert(lppl);
			//cout<<"ultim "<<get_Nrows(lppl)<<retlpset.size();
			//write_LP(lppl,stdout);
//			add_constraint(lppl,rowpl, GE, ff->tokens()+1);
	}
	else if(typeid(*form)==typeid(pnapi::formula::FormulaGreaterEqual)){ 
			//std::cout<<"GreaterEqual"<<endl;
			set_add_rowmode(lppl, TRUE);
			const pnapi::formula::FormulaGreaterEqual *ff=dynamic_cast<const pnapi::formula::FormulaGreaterEqual *>(form);
			//cout<<ff->place().getName()<<">="<<ff->tokens()<<endl;
			int Ncol=net1.getPlaces().size(), *colno = NULL, j;
			REAL *rowe = NULL;
		
			/* create space large enough for one row */
			colno = (int *) malloc(Ncol * sizeof(*colno));
			rowe = (REAL *) malloc(Ncol * sizeof(*rowe));
			set_add_rowmode(lppl, TRUE);
		
			//cout<<get_nameindex(lppl,const_cast<char *>(ff->place().getName().c_str()),FALSE)<<endl;
			j = 0;colno[j] = get_nameindex(lppl,const_cast<char *>(ff->place().getName().c_str()),FALSE); /* first column */
			rowe[j++] = 1; //cout<<colno[0]<<" "<<rowe[0]<<endl;
			if(!add_constraintex(lppl,j,rowe,colno, GE, ff->tokens())) cout<<"nu merge"<<endl;//set_add_rowmode(lppl, FALSE);
			set_add_rowmode(lppl, FALSE);
			set_row_name(lppl,1,const_cast<char *>(ff->place().getName().c_str()));
			//cout<<"row name "<<get_row_name(lppl,1)<<endl;

			set_mat(lppl,1,get_nameindex(lppl,const_cast<char *>(ff->place().getName().c_str()),FALSE),1);
			retlpset.insert(lppl);
			//cout<<"ultim "<<get_Nrows(lppl)<<retlpset.size();
			//write_LP(lppl,stdout);
//			add_constraint(lppl,rowpl, GE, ff->tokens());
	}
	else if(typeid(*form)==typeid(pnapi::formula::FormulaLessEqual)){ 
			//std::cout<<"LessEqual"<<endl;
			set_add_rowmode(lppl, TRUE);
			const pnapi::formula::FormulaLessEqual *ff=dynamic_cast<const pnapi::formula::FormulaLessEqual *>(form);
			//cout<<ff->place().getName()<<"<="<<ff->tokens()<<endl;
			int Ncol=net1.getPlaces().size(), *colno = NULL, j;
			REAL *rowe = NULL;
		
			/* create space large enough for one row */
			colno = (int *) malloc(Ncol * sizeof(*colno));
			rowe = (REAL *) malloc(Ncol * sizeof(*rowe));
			set_add_rowmode(lppl, TRUE);
		
			//cout<<get_nameindex(lppl,const_cast<char *>(ff->place().getName().c_str()),FALSE)<<endl;
			j = 0;colno[j] = get_nameindex(lppl,const_cast<char *>(ff->place().getName().c_str()),FALSE); /* first column */
			rowe[j++] = 1; //cout<<colno[0]<<" "<<rowe[0]<<endl;
			if(!add_constraintex(lppl,j,rowe,colno, LE, ff->tokens()))cout<<"nu merge"<<endl;//set_add_rowmode(lppl, FALSE);
			set_add_rowmode(lppl, FALSE);
			set_row_name(lppl,1,const_cast<char *>(ff->place().getName().c_str()));
			//cout<<"row name "<<get_row_name(lppl,1)<<endl;

			set_mat(lppl,1,get_nameindex(lppl,const_cast<char *>(ff->place().getName().c_str()),FALSE),1);
			retlpset.insert(lppl);
			//cout<<"ultim "<<get_Nrows(lppl)<<retlpset.size();
			//write_LP(lppl,stdout);
//			add_constraint(lppl,rowpl, LE, ff->tokens());
	}
	else if(typeid(*form)==typeid(pnapi::formula::FormulaLess)){
			//std::cout<<"Less"<<endl;
			set_add_rowmode(lppl, TRUE);
			const pnapi::formula::FormulaLess *ff=dynamic_cast<const pnapi::formula::FormulaLess *>(form);
			//cout<<ff->place().getName()<<"<"<<ff->tokens()<<endl;
			if(ff->tokens()>0){
			int Ncol=net1.getPlaces().size(), *colno = NULL, j;
			REAL *rowe = NULL;
		
			/* create space large enough for one row */
			colno = (int *) malloc(Ncol * sizeof(*colno));
			rowe = (REAL *) malloc(Ncol * sizeof(*rowe));
			set_add_rowmode(lppl, TRUE);
		
			//cout<<get_nameindex(lppl,const_cast<char *>(ff->place().getName().c_str()),FALSE)<<endl;
			j = 0;colno[j] = get_nameindex(lppl,const_cast<char *>(ff->place().getName().c_str()),FALSE); /* first column */
			rowe[j++] = 1; //cout<<colno[0]<<" "<<rowe[0]<<endl;
			if(!add_constraintex(lppl,j,rowe,colno, LE, ff->tokens()-1))cout<<"nu merge"<<endl;//set_add_rowmode(lppl, FALSE);
			set_add_rowmode(lppl, FALSE);
			set_row_name(lppl,1,const_cast<char *>(ff->place().getName().c_str()));
			//cout<<"row name "<<get_row_name(lppl,1)<<endl;

			set_mat(lppl,1,get_nameindex(lppl,const_cast<char *>(ff->place().getName().c_str()),FALSE),1);
			retlpset.insert(lppl);
			//cout<<"ultim "<<get_Nrows(lppl)<<retlpset.size();
			write_LP(lppl,stdout);
			}
//			add_constraint(lppl,rowpl, LE, ff->tokens()-1);
	}
	else if(typeid(*form)==typeid(pnapi::formula::FormulaNotEqual)){
			//std::cout<<"NotEqual"<<endl;
			lprec *lpcopy;
			lppl = make_lp(0,  net1.getPlaces().size());
			//set_verbose(lpcopy,0);
			if(lpcopy == NULL) {
				std::cerr<< "Unable to create new LP model"<<std::endl;
			//	return(1);
			}

			set_add_rowmode(lppl, TRUE);set_add_rowmode(lpcopy, TRUE);
			const pnapi::formula::FormulaNotEqual *ff=dynamic_cast<const pnapi::formula::FormulaNotEqual *>(form);
			//cout<<ff->place().getName()<<"<>"<<ff->tokens()<<endl;
			int Ncol=net1.getPlaces().size(), *colno = NULL, j;
			REAL *rowe = NULL;
		
			/* create space large enough for one row */
			colno = (int *) malloc(Ncol * sizeof(*colno));
			rowe = (REAL *) malloc(Ncol * sizeof(*rowe));
			
			//cout<<get_nameindex(lppl,const_cast<char *>(ff->place().getName().c_str()),FALSE)<<endl;
			//cout<<get_nameindex(lpcopy,const_cast<char *>(ff->place().getName().c_str()),FALSE)<<endl;

			j = 0;colno[j] = get_nameindex(lppl,const_cast<char *>(ff->place().getName().c_str()),FALSE); /* first column */
			rowe[j++] = 1; //cout<<colno[0]<<" "<<rowe[0]<<endl;
			if(!add_constraintex(lppl,j,rowe,colno, LE, ff->tokens()-1))cout<<"nu merge"<<endl;//set_add_rowmode(lppl, FALSE);
			if(!add_constraintex(lpcopy,j,rowe,colno, GE, ff->tokens()+1))cout<<"nu merge"<<endl;//set_add_rowmode(lppl, FALSE);

			set_add_rowmode(lppl, FALSE);
			set_row_name(lppl,1,const_cast<char *>(ff->place().getName().c_str()));
			set_row_name(lpcopy,1,const_cast<char *>(ff->place().getName().c_str()));
			//cout<<"row name "<<get_row_name(lppl,1)<<endl;

			set_mat(lppl,1,get_nameindex(lppl,const_cast<char *>(ff->place().getName().c_str()),FALSE),1);
			set_mat(lpcopy,1,get_nameindex(lpcopy,const_cast<char *>(ff->place().getName().c_str()),FALSE),1);
			retlpset.insert(lppl);retlpset.insert(lpcopy);
			//cout<<"ultim "<<get_Nrows(lppl)<<retlpset.size();
			write_LP(lppl,stdout);write_LP(lpcopy,stdout);
			//add_constraint(lppl,rowpl, LE, ff->tokens()-1);
			//add_constraint(lppl,rowpl, GE, ff->tokens()+1);
	}
//	if(retlpset.empty())	
	else if(typeid(*form)==typeid(pnapi::formula::Negation)){
			//std::cout<<"Negation"<<endl;
			//form->output(std::cout);
			const pnapi::formula::Negation *ff=dynamic_cast<const pnapi::formula::Negation *>(form);
			//for sake of stupidity I have to treat  not equal here because it is not recognized elsewhere; idiot students
			const pnapi::formula::Formula* child=*ff->children().begin();
			//child->output(std::cout);
			if(typeid(*child)==typeid(pnapi::formula::FormulaEqual)){
				//cout<<"on the safe side";
				//std::cout<<"NotEqual"<<endl;
				lprec *lpcopy;
				lpcopy = make_lp(0,  net1.getPlaces().size());
				int i=1;
				for(std::set<pnapi::Place *>::iterator cit=net1.getPlaces().begin();cit!=net1.getPlaces().end();++cit){
					char * cstr= new char [(*cit)->getName().size()+1];
					strcpy(cstr,(*cit)->getName().c_str());
					set_col_name(lpcopy, i, cstr); i++;
				}
				set_verbose(lpcopy,6);
				if(lpcopy == NULL) {
					std::cerr<< "Unable to create new LP model"<<std::endl;
				//	return(1);
				}

				set_add_rowmode(lppl, TRUE);set_add_rowmode(lpcopy, TRUE);
				const pnapi::formula::FormulaEqual *ff=dynamic_cast<const pnapi::formula::FormulaEqual *>(child);
				cout<<ff->place().getName()<<"<>"<<ff->tokens()<<endl;
				int Ncol=net1.getPlaces().size(), *colno = NULL, j;
				REAL *rowe = NULL;
		
				/* create space large enough for one row */
				colno = (int *) malloc(Ncol * sizeof(*colno));
				rowe = (REAL *) malloc(Ncol * sizeof(*rowe));
				
				//cout<<get_nameindex(lppl,const_cast<char *>(ff->place().getName().c_str()),FALSE)<<endl;
				cout<<get_nameindex(lpcopy,const_cast<char *>(ff->place().getName().c_str()),FALSE)<<endl;

				j = 0;colno[j] = get_nameindex(lppl,const_cast<char *>(ff->place().getName().c_str()),FALSE); /* first column */
				rowe[j++] = 1; cout<<colno[0]<<" "<<rowe[0]<<endl;
/*				if(ff->tokens()-1<0){ // process just the copy
					if(!add_constraintex(lpcopy,j,rowe,colno, LE, ff->tokens()+1))cout<<"nu merge"<<endl;//set_add_rowmode(lppl, FALSE);
					set_add_rowmode(lpcopy, FALSE);
					set_row_name(lpcopy,1,const_cast<char *>(ff->place().getName().c_str()));
					cout<<"row name "<<get_row_name(lppl,1)<<endl;
					set_mat(lpcopy,1,get_nameindex(lpcopy,const_cast<char *>(ff->place().getName().c_str()),FALSE),1);
					retlpset.insert(lpcopy);
					cout<<"ultim "<<get_Nrows(lpcopy)<<retlpset.size();
				}*/
				//else{//process both
				if(ff->tokens()==0){cout<<"reduce"<<endl;}
				else{	if(!add_constraintex(lppl,j,rowe,colno, LE, ff->tokens()-1))cout<<"nu merge"<<endl;//set_add_rowmode(lppl, FALSE);
					set_add_rowmode(lppl, FALSE);
					set_row_name(lppl,1,const_cast<char *>(ff->place().getName().c_str()));
					cout<<"row name "<<get_row_name(lppl,1)<<endl;
					set_mat(lppl,1,get_nameindex(lppl,const_cast<char *>(ff->place().getName().c_str()),FALSE),1);
					retlpset.insert(lppl);
				}
					if(!add_constraintex(lpcopy,j,rowe,colno, GE, ff->tokens()+1))cout<<"hh"<<endl;//set_add_rowmode(lppl, FALSE);
					set_add_rowmode(lpcopy, FALSE);
					set_row_name(lpcopy,1,const_cast<char *>(ff->place().getName().c_str()));
					set_mat(lpcopy,1,get_nameindex(lpcopy,const_cast<char *>(ff->place().getName().c_str()),FALSE),1);retlpset.insert(lpcopy);
					cout<<"ultim "<<get_Nrows(lppl)<<" "<<get_Nrows(lpcopy)<<retlpset.size();
				//}
				//write_LP(lppl,stdout);
				write_LP(lpcopy,stdout);


			}
			else{
				set_add_rowmode(lppl, TRUE);
			//const pnapi::formula::Negation *ff=dynamic_cast<const pnapi::formula::Negation *>(form);
			//first reduce negations
				cout<<(ff->children().size()==1)<<endl;
				retlpset=transform(net1, transformNegFormula(net1, form));
/*			std::vector<std::string>::const_iterator it(std::find(vp.begin(), vp.end(),ff->place().getName()));
			cout<<it-vp.begin()<<endl;
			rowpl[it-vp.begin()]=1;*/
			//add_constraint(lppl,rowpl, LE, ff->tokens()-1);
			//add_constraint(lppl,rowpl, GE, ff->tokens()+1);
			//retlpset.insert(lppl);
			}
	}

	//std::cout<<retlpset.size()<<"end transform"<<std::endl;
	return retlpset;

}
// idea: each conjunction represents a submarking; each disjunction a semilinear  set of markings (union of submarkings)
// once we have a conjunction we add constraints to the lp problem
// otherwise we build another lp record
// for disjunction we have union of sets of sets of constraints
// for conjunction we have for each set of constraints  we do the cartesian product and exeute the union of contraints

// one more time
// for each member of the conjunction/disjunction
// group them by two and perform the operation
// so w should make a set of lp problems I hate sets so male a list 
 	//const pnapi::formula::Formula * f=dynamic_cast<const pnapi::formula::Formula *>(&net1.finalCondition().formula());
//	const pnapi::formula::Formula & f= net1.finalCondition().formula();
//		std::cout<<formula<<f<<endl;
//	if(typeid(f)==typeid(pnapi::formula::Disjunction));
 	//std::cout<<typeid(&f)<<endl;
//	if(typeid(f)==typeid(pnapi::formula::Conjunction)) std::cout<<"Conjunctio"<<endl;
	

// set_presolve(lp, PRESOLVE_LINDEP, get_presolveloops(lp));


/*

const pnapi::formula::Formula * unfoldFlags(const pnapi::PetriNet &net1, const pnapi::formula::Formula * form){
	//if it is a disjunction return the result recursively
	//if it is a consjunction simply unfold the flags = 0 
	// if it is a negation ignore again
	set<const Place *> pl=form->places();
	if(typeid(*form)==typeid(pnapi::formula::Conjunction)){ 
		const pnapi::formula::Conjunction *f=dynamic_cast<const pnapi::formula::Conjunction *>(form);
		//cout<<endl<<"flag "<<f->flag_<<endl;
		//the default case is when there is no flag
		switch (f->flag_) {
			case 3: {cout<<"all other internal places empty";
				set<Place *> inpl=net1.getInternalPlaces();
				for(set<Place *>::const_iterator p = inpl.begin();p!=inpl.end();p++){unsigned int tt=0;
					const pnapi::formula::FormulaEqual* prep=new pnapi::formula::FormulaEqual((**p),tt);
 					form=new pnapi::formula::Conjunction(*form, *dynamic_cast<const pnapi::formula::Formula *>(prep));//now add ...
				}
				}
			case 5: {cout<<"all other places empty";//the toughest one
				set<Place *> allpl=net1.getPlaces();//minus concerning places and 
				set<const Place *> cp=net1.finalCondition().concerningPlaces();
				set<std::string> cps;
				for(set<Place *>::const_iterator p = allpl.begin();p!=allpl.end();p++){
					//if(p->getName()==)
					unsigned int tt=0;
					const pnapi::formula::FormulaEqual* prep=new pnapi::formula::FormulaEqual((**p),tt);
 					form=new pnapi::formula::Conjunction(*form, *dynamic_cast<const pnapi::formula::Formula *>(prep));//now add ...
				}
				}
			case 6: {cout<<"all other external places empty";
				set<Place *> expl=net1.getInterfacePlaces();
				for(set<Place *>::const_iterator p = expl.begin();p!=expl.end();p++){unsigned int tt=0;
					const pnapi::formula::FormulaEqual* prep=new pnapi::formula::FormulaEqual((**p),tt);
 					form=new pnapi::formula::Conjunction(*form, *dynamic_cast<const pnapi::formula::Formula *>(prep));//now add ...
				}
				}
			case 7: {cout<<"all places empty";
				set<Place *> allpl=net1.getPlaces();
				for(set<Place *>::const_iterator p = allpl.begin();p!=allpl.end();p++){unsigned int tt=0;
					const pnapi::formula::FormulaEqual* prep=new pnapi::formula::FormulaEqual((**p),tt);
 					form=new pnapi::formula::Conjunction(*form, *dynamic_cast<const pnapi::formula::Formula *>(prep));//now add ...
				}
				}
		}
	}
	return form;
}
*/

//



int main(int argc, char** argv) {
	
	//time_t start_time, end_time;
	clock_t start_clock;// = clock();
	evaluateParameters(argc, argv);
	Output::setTempfileTemplate(args_info.tmpfile_arg);
	Output::setKeepTempfiles(args_info.noClean_flag);
	std::cerr << PACKAGE << " processing ";
	if(args_info.inputs_num>0) {
		for (unsigned int i = 0; i < args_info.inputs_num; i++)
        	{
                	std::cerr << " " << args_info.inputs[i]<<" ";
        	}
		std::cerr  <<std::endl;
	}
	//the right way using only the open nets
	//the stupid way using the compose function which is not implemented correctly
	// parse the open nets and obtain the incidence matrix of the composition and the corresponding final marking(s)
	pnapi::PetriNet net1, net2;
	std::vector<pnapi::PetriNet *> netps;//for the sake of adapters
	

	bool coverall=false;
	std::string s1("0");
	std::ifstream inputStream;

	//time(&start_time);
	int i = 0;
	//initialization of parameter structures	
 	map<int, std::set<std::string> > enforcedT;//for enforcing transitions/messages
	map<int, std::set<std::string> > excludeE;//for excluding transitions
 	map<int, std::set<std::set<std::string> > >  enforcedFC;//enforcing free-choice clusters
	int nPlaces;
	int nTransitions;
	std::set<lprec *> retlpset;//set of all final markings of the composition
 	
	
	if (args_info.enforceEvents_arg){
		for (unsigned i = 0; i < args_info.enforceEvents_given; ++i){
 			std::string s=args_info.enforceEvents_arg[i];std::string st,cs;
			if(s=="all") {coverall=true;status("Cover all nodes");break;}
  			if(s.find("@")!=std::string::npos) {st=s.substr(0,s.find("@"));//cout<<st<<endl;
				cs=s.substr(s.find("@")+1); 		
	 			stringstream ss;ss<<st;
 				int net; ss>>net;//cout<<net;
 				//const int netc=net;//const std::string cs;
				if(net==0){abort(2, "wrong format for enforce events. Read the manual.\n");}
//		if(s.find(".")!=std::string::npos) cs=s.substr(s.find(".")+1);//cout<<" "<<s.substr(s.find(".")+1)<<endl;
	 			set<std::string> sets;sets.insert(cs);
  				if(enforcedT.find(net)==enforcedT.end()) {
					enforcedT.insert(std::pair<unsigned int, set<std::string> >(net,sets));
				}
  				else{set<std::string> se=(enforcedT.find(net))->second;
	  				se.insert(cs);
	  				enforcedT.find(net)->second=se;//cout<<"old"<<endl;
// 				//enforcedT.insert(std::pair<unsigned, set<std::string> >)(net,s.substr(s.find(".")+1 ));		
				}
			}
			else {//interface 
				if(enforcedT.find(0)==enforcedT.end()) {
					std::set<std::string> sets;sets.insert(s);
					enforcedT.insert(std::pair<unsigned int, set<std::string> >(0,sets));	
				}
				else{   std::set<std::string> se=(enforcedT.find(0))->second;
	  				se.insert(cs);
	  				enforcedT.find(0)->second=se;//cout<<"old"<<endl;
// 				//enforcedT.insert(std::pair<unsigned, set<std::string> >)(net,s.substr(s.find(".")+1 ));		
				}	
			}
 		}
	}

	//exclude events
	if (args_info.excludeE_arg){
		for (unsigned i = 0; i < args_info.excludeE_given; ++i){
 			std::string s=args_info.excludeE_arg[i];std::string st,cs;
			if(s=="all") {coverall=true;status("Cover all nodes");break;}
  			if(s.find("@")!=std::string::npos) {st=s.substr(0,s.find("@"));//cout<<st<<endl;
				cs=s.substr(s.find("@")+1); 		
	 			stringstream ss;ss<<st;
 				int net; ss>>net;//cout<<net;
 				//const int netc=net;//const std::string cs;
				if(net==0){abort(2, "wrong format for exclude events. Read the manual.\n");}
				//		if(s.find(".")!=std::string::npos) cs=s.substr(s.find(".")+1);//cout<<" "<<s.substr(s.find(".")+1)<<endl;
	 			set<std::string> sets;sets.insert(cs);
  				if(excludeE.find(net)==excludeE.end()) {
					excludeE.insert(std::pair<unsigned int, std::set<std::string> >(net,sets));
				}
  				else{set<std::string> se=(excludeE.find(net))->second;
	  				se.insert(cs);
	  				excludeE.find(net)->second=se;//cout<<"old"<<endl;
					// 				//enforcedT.insert(std::pair<unsigned, set<std::string> >)(net,s.substr(s.find(".")+1 ));		
				}
			}
			else {//interface 
				if(excludeE.find(0)==excludeE.end()) {
					std::set<std::string> sets;sets.insert(s);
					excludeE.insert(std::pair<unsigned int, std::set<std::string> >(0,sets));	
				}
				else{   std::set<std::string> se=(excludeE.find(0))->second;
	  				se.insert(cs);
	  				excludeE.find(0)->second=se;//cout<<"old"<<endl;
					// 				//enforcedT.insert(std::pair<unsigned, set<std::string> >)(net,s.substr(s.find(".")+1 ));		
				}	
			}
 		}
	}
	
	

	
	//parse the input nets
	
//		try {
		// std::string nets[args_info.inputs_num];	
	if(args_info.inputs_num){
	  for(unsigned int i=0; i<args_info.inputs_num;i++){
		if(i==0){			
			ifstream ifs1(args_info.inputs[i]);
			inputStream.open(args_info.inputs[i]);
			try { ifs1 >> owfn >> net1;}
			catch (InputError e) { std::cerr <<"net " <<i<<" failed" << endl << e << endl; assert(false); }
			ifs1.close();

			//const int zero=0; now check whether these are real transitions
			if(enforcedT.find(1)!=enforcedT.end()&&(args_info.enforceEvents_given>0)){
				std::set<std::string> se=(enforcedT.find(1))->second;
				for (std::set<std::string>::iterator it=se.begin(); it!=se.end(); it++)
					if((net1.findTransition(*it)==NULL) && (net1.findPlace(*it)==NULL) )
						abort(2, "the node does not belong to the net");
			}
			if(excludeE.find(1)!=excludeE.end()&&(args_info.excludeE_given>0)){
				std::set<std::string> se=(excludeE.find(1))->second;
				for (std::set<std::string>::iterator it=se.begin(); it!=se.end(); it++)
					if((net1.findTransition(*it)==NULL) && (net1.findPlace(*it)==NULL) )
						abort(2, "the node does not belong to the net");
			}
			if ( args_info.adapterrules_given )
			{//adapter
				pnapi::PetriNet *pnet=new PetriNet(net1);
				netps.push_back(pnet);cout<<endl<<netps.size()<<endl;
			}
		}
		else{
			std::stringstream s;s<<(i+1); //cout<< s.str();
			std::string s2; s2=s.str()+"@";//"net"+s.str();
			ifstream ifs2(args_info.inputs[i]);
			try { ifs2 >> owfn >> net2;}
			catch (InputError e) { std::cerr <<"net " <<i<<" failed"<< std::endl << e << endl; assert(false); }
			ifs2.close();
			if((enforcedT.find(i+1)!=enforcedT.end())&&(args_info.enforceEvents_given>0)){
				std::set<std::string> se=(enforcedT.find(i))->second;
				for (std::set<std::string>::iterator it=se.begin(); it!=se.end(); it++)
				  if((net1.findTransition(*it)==NULL)&&(net1.findPlace(*it)==NULL))
					  abort(2, "the node does not belong to the net");
			}
			if((excludeE.find(i+1)!=excludeE.end())&&(args_info.excludeE_given>0)){
				std::set<std::string> se=(excludeE.find(i))->second;
				for (std::set<std::string>::iterator it=se.begin(); it!=se.end(); it++)
					if((net1.findTransition(*it)==NULL)&&(net1.findPlace(*it)==NULL))
						abort(2, "the node does not belong to the net");
			}
			   //else abort(3, "the transitions do not belong to the net");
			if(i==1) s1="1@"; else s1=""; net1.compose(net2, s1, s2);
			if ( args_info.adapterrules_given )
			{//adapter
				pnapi::PetriNet *pnet=new PetriNet(net2);
				netps.push_back(pnet);
			}
			//renew the prefixes: s1 has the old 
		//	if(!net1.isClosed()){cout<<" is not closed"<<std::endl;} else 
		}
		  /*if ( args_info.adapterrules_given ) not needed anymore
		  {//adapter
			  pnapi::PetriNet *pnet=new PetriNet(net1);//cout<<endl<<netps.size();
			  netps.push_back(pnet);cout<<endl<<netps.size()<<endl;
		  }*/
	  }

	  //find free-choice sending clusters
		if (args_info.enforceFC_given){// detect all sending FC cluster for the transitions
			std::set<std::set<std::string> > fcc;//iterate all transitions to find free choice clusters
			std::set<pnapi::Transition *> fct=net1.getTransitions();
			do{
				pnapi::Transition *t=*fct.begin();std::string s=t->getName();//=args_info.enforceFC_args[0];
				//std::string st=s.substr(0,s.find("@"));stringstream ss;ss<<st;
				//int net; ss>>net;;
				//if(net1.findTransition(s)){
				std::set<Node *> past, curr;
				bool init=true;//initial set of post-transitions	
				for (std::set<Node *>::iterator nit=net1.findTransition(s)->getPreset().begin(); nit!=net1.findTransition(s)->getPreset().end(); ++nit) {
					if (init) {
						curr=(*nit)->getPostset();init=false;
						for (std::set<Node *>::iterator sendit=curr.begin(); sendit!=curr.end(); ++sendit) {
							const std::set<Node *> sending=(*sendit)->getPostset();
							for (std::set<Node *>::iterator on=sending.begin(); on!=sending.end(); ++on) {
								if (net1.getOutputPlaces().find(dynamic_cast<Place *>(*on))!=net1.getOutputPlaces().end()) {
									break;
								}
							}
						}
					}
					else if(past!=curr) {cout<<"Non-free choice cluster delete all";}
					past=curr;
				}
			}
			while(!fct.empty());
			/*	}
			 else{
			 abort(2,"The free-choice sending cluster does not exist");
			 }*/
		}

    }
		//std::cout << owfn << net1<<"end of composition"<<std::endl;

	
	
    if ( args_info.adapterrules_given )
    {
        //time(&start_time);
		
		RuleSet rs;
		// ! #adapter //	Adapter adapter(net2,rs,ASYNCHRONOUS,1,false);
			
        status("reading transformation rules from file \"%s\"", args_info.adapterrules_arg[0]);
		
        FILE * rulefile (NULL);
        if((rulefile = fopen(args_info.adapterrules_arg[0],"r")))
        {
			rs.addRules(rulefile);
			std::cout<<std::endl<<"Rules in the engine "<<rs.getRules().size()<<std::endl;
			
            //fclose(rulefile);
            rulefile = NULL;
        }
        else
        {
            abort(2, "Rule file %s could not be opened for reading", args_info.adapterrules_arg);
        }
        //time(&end_time);
		std::cout<<std::endl<<"pn before building engine "<<netps.size()<<std::endl;
		//std::cout<<std::endl<<"pn before building engine "<<netps[0]->getTransitions().size()<<std::endl;
        //status("reading all rules file done [%.0f sec]", difftime(end_time, start_time));
		Adapter *adapter=new Adapter(netps, rs, Adapter::ASYNCHRONOUS, 1, false);
		const PetriNet *benetp=adapter->buildEngine();
		//give this tho the state equation
		net1=PetriNet(*benetp);
		//const PetriNet *senet=adapter->composeNetsEngine();
		std::cout<<std::endl<<"pn after building engine "<<benetp->getPlaces().size()<<std::endl;
	    std::cout<<std::endl<<"pn after building engine "<<benetp->getTransitions().size()<<std::endl;
		//printing out
		
		//std::string s(args_info.inputs[0]);
		//std::string outname=s+".adapter.owfn";
		//pnapi::util::Output outfile(outname, "Output file");
		//status("creating file '%s'...", outname.c_str());
		// outfile.stream() << meta(pnapi::io::CREATOR, std::string(PACKAGE_STRING) + " Frontend (" + CONFIG_BUILDSYSTEM + ")");
		//outfile.stream() << meta(pnapi::io::OUTPUTFILE, outname);
	    //outfile.stream() 
			std::cout<< pnapi::io::owfn << net1;
	
	
	// string outname = objects[i].filename + suffix + "." + args_info.output_orig[j];
	  //              Output outfile(outname, std::string(args_info.output_orig[j]) +  " output");

	    //            outfile.stream() << meta(io::CREATOR, std::string(PACKAGE_STRING) + " Frontend (" + CONFIG_BUILDSYSTEM + ")");
	      //          outfile.stream() << meta(io::OUTPUTFILE, outname);
		//for ( unsigned int index = 0; index < netps.size(); ++index)
		//{
		//	delete netps[index];
		//}
		//netps.clear();
		//std::cout <<"help"<< std::endl;	
		//return 0;
	}

	
	
	
	//get the initial marking
//	engine.compose(net1, "","");int nPlaces = (int) engine.getPlaces().size();
//	int nTransitions = (int) engine.getTransitions().size();
//	std::cout << owfn << engine<<"end of adaptation with"<<nTransitions<<std::endl;
	
	
		//}
//	catch (pnapi::io::InputError error) {
//		std::cerr << PACKAGE << error << std::endl;
//		exit(EXIT_FAILURE);}
	
// print result
	//fflush(stdin);
	//clock_t start_clock = clock();	
	start_clock = clock();int res=0;//the result of the system
	std::vector<lprec *> lpmps;// 
	std::set<std::string> resultinp, resultout,resultsyn, result, resintern;//interface 
	//parse the message profile files
	if(args_info.messageProfiles_given>0){
		std::multimap<std::string,int> mpinvocation;set<std::string> hh;
		for (unsigned int imp = 0; imp <args_info.messageProfiles_given; ++imp) {//hh.insert(args_info.messageProfiles_arg[imp]);
			size_t found=std::string(args_info.messageProfiles_arg[imp]).find(".output");
			hh.insert(std::string(args_info.messageProfiles_arg[imp]).substr(0,found));
			mpinvocation.insert(std::pair<std::string,int>(std::string(args_info.messageProfiles_arg[imp]).substr(0,found),imp));
			
		}
		
		std::set<std::string> inputp,outputp, syncp; // variables necessary for the composition
		for (set<std::string>::iterator sit=hh.begin(); sit!=hh.end(); ++sit) { //for each net
			cout <<"Net id: "<< *sit;int no=0;//i=0;
			std::vector<lprec *> mps;//mps for each net
			std::set<std::string> resultinp, resultout,resultsyn, result, synct;//variables (initially they are)
			
			//build the set of constraints from the for the current net, result is the set of variables
			for(std::multimap<std::string,int>::iterator it=mpinvocation.begin();it!=mpinvocation.end();++it){ //for each mp
				if ((*it).first!=*sit) {
					continue;
				}
				cout<<" argument "<<(*it).second<<endl;//cout<<"no"<<no<<endl;//
				no++;//bMP.clear();
				std::cerr << PACKAGE << ": Message profile file <" << args_info.messageProfiles_arg[(*it).second] << ">" << std::endl;
				initialize_mp_parser();
				mp_yyin = fopen(args_info.messageProfiles_arg[(*it).second], "r");
				if (!mp_yyin) {
					std::cerr << "cannot open message profile file '" << args_info.messageProfiles_arg[(*it).second] << "' for reading'\n" << std::endl;exit(1);
				}
				term_vec = new std::vector<EventTerm*>();  ///initialize the event terms
				mp_yyparse();
				//cout << inputPlaces.size()<< outputPlaces.size()<<synchrT.size()<<endl;
				//inputp,outputp the set of places from the previous composition				
				if (no == 1) { //get interface it==mpinvocation.begin()
					for (std::set<std::string>::iterator ssit=inputPlaces.begin(); ssit!=inputPlaces.end(); ++ssit) {
						if (inputp.find(*ssit)!=inputp.end()) {
							cout << "we consider only simple composition; we are considering instance composition for next versions of the tool";
							exit(1);
						}
						else result.insert(*ssit);
						if(outputp.find(*ssit)!=outputp.end()){ outputp.erase(outputp.find(*ssit));//matching
						}
						else resultinp.insert(*ssit);
					
					}
					for (std::set<std::string>::iterator ssit=outputPlaces.begin(); ssit!=outputPlaces.end(); ++ssit) {
						if (outputp.find(*ssit)!=outputp.end()) {
							cout << "we consider only simple composition; we are considering instance composition for next versions of the tool";
							exit(1);
						}
						else result.insert(*ssit);
						if(inputp.find(*ssit)!=inputp.end()){ inputp.erase(inputp.find(*ssit));//matching
						}
						else resultout.insert(*ssit);
					}
					for(std::set<std::string>::iterator cit=synchrT.begin();cit!=synchrT.end();++cit){
						if (synct.find(*cit)==synct.end()) {resultsyn.insert(*sit);//not sync
						}
						else synct.erase(*sit);
						result.insert(*sit);
					}
					//resultinp.insert(inputp.begin(),outputp.end());resultout.insert(outputp.begin(),outputp.end());
					//resultsyn.insert(synct.begin(),synct.end());
					lprec *lpt;//current set of constraints 
					lpt = make_lp(0, result.size());
					for(int i=1;i<=get_Ncolumns(lpt);i++){
						set_int(lpt,i,TRUE);
					}
					if(lpt == NULL) {
						std::cerr<< "Unable to create new LP model"<<std::endl;
						return(1);
					}
					int k=1;
					for(std::set<std::string>::iterator cit=result.begin();cit!=result.end();++cit){
						char * cstr= new char [(*cit).size()+1];
						strcpy(cstr,(*cit).c_str());
						set_col_name(lpt, k, cstr);k++;
					}
		/*		for(std::set<std::string>::iterator cit=outputPlaces.begin();cit!=outputPlaces.end();++cit){
					char * cstr= new char [(*cit).size()+1];
					strcpy(cstr,(*cit).c_str());
					set_col_name(lpmp, k, cstr);k++;
				}
				for(std::set<std::string>::iterator cit=synchrT.begin();cit!=synchrT.end();++cit){
					char * cstr= new char [(*cit).size()+1];
					strcpy(cstr,(*cit).c_str());
					set_col_name(lpmp, k, cstr);k++;
				}*/
				
				//cout <<"no of final markings" << nfm<<fmar.size()<<"and"<<nc<<endl;

			//for each final marking
			//cout <<k-1<< nfm << " bla " << nc <<endl;
			//cout <<"BMP: "<< boundsMP.size()<<endl;
			//for each constraint get the bounds and add the constraint into the system
			//iff the interface profile is the same simply add the constraints otherwise 
			//mps.insert(lp);lp=lpmc;cout<<get_col_name(lp,get_Ncolumns(lp))<<"help"<<endl;
				//std::set<lprec *> mps;
// 				for (int ifm=0; ifm<nfm; ++ifm) {cout <<ifm;	mps.push_back(lpmpcp);		}
				//cout <<"size" <<nfm<< (int) mps.size() <<endl;
				//}//this is the first time for the 
				//int ifm=1;
				for (unsigned int ifm=1;ifm<nfm+1;ifm++){
					//if(no==1){
					unsigned int k=1;//cout <<"before";
					lprec *lpmp=copy_lp(lpt);
					if(no==1) mps.push_back(lpmp);
					int kn=1;//cout<<"hello"<<term_vec->size()<<endl;
					//int k=1;//
					//print_lp(mps.at(ifm-1));
					//cout<<ifm-1<<"in"<<get_Nrows(mps.at(0))<<get_Nrows(mps.at(1))<<get_Nrows(mps.at(2))<<endl;
					REAL* roww=new REAL[1+inputPlaces.size()+ outputPlaces.size()+synchrT.size()]();
				//initialize roww ith zero values
					set<const constraintf *> fingerprinti=getconstrforfm(fingerprint, ifm);//get the terms for the final marking
					//std::cerr << "computing " << fingerprint.size();
					for (set<const constraintf *>::iterator fi=fingerprinti.begin(); fi!=fingerprinti.end(); ++fi) {
						
						std::map<std::string const,int>* ee=EventTerm::termToMap((term_vec->at((*fi)->termi-1)));
						//std::cerr << (*fi)->fmi-1<<(*fi)->termi-1<<endl;
						//cout << EventTerm::toPrettyString((term_vec->at((*fi)->termi-1)));
					//}
					//for (std::vector<EventTerm*>::iterator it = term_vec->begin(); it != term_vec->end(); ++it) {
						//need to initialize the roww set_add_rowmode(lpmp, TRUE);
						//REAL *roww;//[1+inputPlaces.size()+ outputPlaces.size()+synchrT.size()];
						for (k=1; k<1+inputPlaces.size()+ outputPlaces.size()+synchrT.size(); k++) {
							roww[k]=0;
						}
						//cout<<"Constraint#"<<kn<<": ";
						//cout <<"help" << bMP.size() << ifm << kn <<endl;
						//cout<<ifm-1<<"in"<<get_Nrows(mps.at(0))<<get_Nrows(mps.at(1))<<get_Nrows(mps.at(2))<<endl;
						/*std::pair<int,int> bd=bMP.at(term_vec->size()*(ifm-1)+kn-1);//cout<<"bMP"<<bMP.size()<<endl;
						std::map<std::string const,int>* ee=EventTerm::termToMap((*it));*/
						//cout << EventTerm::toPrettyString((*it));
						//std::pair<int,int> b=bd.at(kn);
						//cout << bd.first<<" "<<bd.second<<endl;
						//std::cerr<<" parsing ";
						for(std::map<std::string const,int>::iterator ite=ee->begin();ite!=ee->end();++ite){
						//cout<<ite->first<<" "<<ite->second<<std::endl;
							char * cstr= new char [ite->first.size()+1];
							strcpy(cstr,ite->first.c_str());
							roww[get_nameindex(mps.at(ifm-1), cstr,FALSE)]=ite->second;
						}//cout << (*fi)->lb<<" "<<(*fi)->ub<<endl;
						int rhs=(*fi)->lb;//bd.first;
						
						if(!add_constraint(mps.at(ifm-1), roww,GE, rhs))
						cout<<"gata"<<endl;
						if ((*fi)->ub!=USHRT_MAX) {//if (bd.second!=USHRT_MAX) {
							//set_rh_range(mps.at(ifm-1),get_Nrows(mps.at(ifm-1)), bd.second-rhs);
							set_rh_range(mps.at(ifm-1),get_Nrows(mps.at(ifm-1)), (*fi)->ub-rhs);
						}
						else {
							set_rh_range(mps.at(ifm-1),get_Nrows(mps.at(ifm-1)), get_infinite(mps.at(ifm-1)));
						}
					//set_add_rowmode(mps.at(ifm-1), FALSE);
						
						kn++;
					}
					//cout<<"out"<<get_Nrows(mps.at(ifm-1))<<endl;//cout<<"coloane:"<<get_Nrows(mps.at(ifm-1));//ifm++;
					//lp=lpmc;cout<<get_col_name(lp,get_Ncolumns(lp))<<"help"<<endl;
				//aici adauga constraints
					//cout<<ifm-1<<" "; print_lp(mps.at(ifm-1));
				}
			//for (int ifm=1;ifm<nfm+1;++ifm){	print_lp(mps.at(ifm-1));};cout<<mps.size()<<endl;	
				//add here to the previous set of constraints
				
				}	
				
			} //cout<<mps.size();
			//here add final marking constraints; we can skip these ones
			/*for (unsigned int i=0; i<nfm; ++i) {
				
				for (std::map<std::string,int> ::iterator itmar=fmar.at(i).begin(); itmar!=fmar.at(i).end(); ++itmar) {
					char * cstr= new char [(*itmar).first.size()+1];
					strcpy(cstr,(*itmar).first.c_str());
					int index=get_nameindex(mps.at(i),cstr,FALSE);set_bounds(mps.at(i),index, (*itmar).second,(*itmar).second);
				}
			}*/
			//here add the new constraints to the previous computed ones
			if(sit==hh.begin()){lpmps=mps;}
			else {//here combine 
				//cout<<mps.size()<<lpmps.size();
				for (int i=0; i<get_Ncolumns(lpmps.at(0)); ++i) { //add variables from previous
					std::string pr(get_col_name(lpmps.at(0),i+1));
					if (result.find(pr)==result.end()) {//add columns from old composition to the new interface
						result.insert(pr);
						//REAL column[1+get_Nrows(lpmps.at(0))];
						//for(int r=0;r<=get_Nrows(lpmps.at(0));r++){
						//	column[r]=0.0;	
						//}
						for (unsigned int j=0; j<nfm; j++) {
							//add_column(mps.at(j),column);
							add_columnex(mps.at(j),0,NULL,NULL);
							set_col_name(mps.at(j), get_Ncolumns(mps.at(j)),get_col_name(lpmps.at(0),i+1));
						}
					}
				}
				//combine all final markings and add the rows of lpmps
				std::vector<lprec *> st;
				for(unsigned int i=0;i<mps.size();++i){
					//lprec * lpr=copy_lp(mps.at(i)); cout<<get_Nrows(mps.at(i));//make copies
					for(unsigned int j=0;j<lpmps.size();++j){// add lpr too
						//get a lpr copy and then for each other mp make a new syste
						lprec * lpr=copy_lp(mps.at(i)); 
						//cout<<get_Nrows(lpmps.at(j));
						for (int r=0; r<get_Nrows(lpmps.at(j)); ++r) {
							/*
							 int Ncol=net1.getPlaces().size();
							 
							//colno = (int *) malloc(Ncol * sizeof(*colno));
							//rowe = (REAL *) malloc(Ncol * sizeof(*rowe));
							
							int nr_of_cols = get_rowex(*cIt2,j,rowe,colno);
							//cout<<"Sizeof "<<sizeof(rowe)<<" and j is " << j << endl;
							//for(int k=0;k<1+ net1.getPlaces().size();k++) cout<<rowpl[k]<<" ";
							;							//set_verbose(lpcopy,FULL);//cout<<"Constraint type "<<get_constr_type(*cIt2,j)<<endl;
							if(!add_constraintex(lpcopy, nr_of_cols ,rowe,colno ,get_constr_type(*cIt2,j), get_rh(*cIt2,j)))
								cout<<"gata"<<endl;
							set_row_name(lpcopy,get_Nrows(lpcopy), get_row_name(*cIt2,j));
							 */
							int * colno=new int[1+get_Ncolumns(lpr)]();
							REAL * rowe= new REAL[1+get_Ncolumns(lpr)]();
							/*REAL *rowp=new REAL[1+get_Ncolumns(lpr)]();//int *colp;
							if(get_row(lpmps.at(j),r+1,rowp)==0) abort(2, "lpsolve	");//for(int b=0;b<get_Ncolumns(lpr);b++){cout<<rowp[b]<<" ";}
							*/
							int count=get_rowex(lpmps.at(j),r+1,rowe,colno);
							if (count==0)  abort(2, "lpsolve	");//for(int b=0;b<get_Ncolumns(lpr);b++){cout<<rowp[b]<<" ";}
							//for(int j=0;j<get_Ncolumns(lpmp);j++){rowwpp[j+hh-get_Ncolumns(lpmp)]=rowp[j+1];}
							//for(int j=0;j<hh;j++){cout<<rowwpp[j]<<" ";}

							//if( grex==-1) cout<<"gata";
							//add_constraint(lpr,rowp,get_constr_type(lpmps.at(j),r+1),get_rh(lpmps.at(j),r+1));
							add_constraintex(lpr,count,rowe,colno,get_constr_type(lpmps.at(j),r+1),get_rh(lpmps.at(j),r+1));
							//set_rh_range(lpr,get_Nrows(lpmps.at(j)),get_rh_range(lpmps.at(j),r+1));
							//print_lp(lpr);
						}st.push_back(lpr);
					}//print_lp(lpr);
				}
				lpmps=st;
			}	
		}
		
		//mps+lpmps
		// union of variables

		
		//pt diferenta adauga variabile
		//insert 
		//solve
		if (args_info.inputs_num==0) {
			
		for(unsigned int ifm=0;ifm<lpmps.size();ifm++){
			for(i=1;i<=get_Ncolumns(lpmps.at(ifm));i++){
				set_int(lpmps.at(ifm),i,TRUE);
			}
			//set_outputfile(lpmps.at(ifm), "log.txt");
			set_verbose(lpmps.at(ifm),NORMAL);
			//write_LP(lpmps.at(ifm), stdout);
			//print_lp(lpmps.at(ifm));
			res=solve(lpmps.at(ifm));
			//write_LP(lpmps.at(ifm), stdout);
			//cout<<"Solve "<<solve(lp)<<endl;
			if(res==0){
				//cout<<endl<<"Compatibility check inconclusive"<<endl;
				if (args_info.verbose_flag) {
					cout<<endl<<"The constraint system has at least a solution:"<<endl;
					REAL *sol;//[get_Ncolumns(lpmps.at(ifm))];
					get_ptr_variables(lpmps.at(ifm),&sol);
					for(int s=0;s<get_Ncolumns(lpmps.at(ifm));s++){
						cout<<get_col_name(lpmps.at(ifm),s+1)<<" "<<sol[s]<<" "<<endl;//" "<<static_cast<int>(sol[s]) not needed anymore
					}
				}	
				break;
			}
			else {
				//std::cout<<endl<<"Not compatible wrt weak termination"<<endl;
			}

		}
		}
	}


	

	


//compute the set of final marmings as sets of constraints
//in order to see whether there a re redundant final markings solve linear equations two by two and they should have no common solution
// in case they have we should choose the mostrestrictive ones

	//find out all possible free terms which basically determine how many equations should be solved
 	
	//net1.finalCondition().formula().output(std::cout);//&net1.finalCondition().formula()
	//cout<<net1.finalCondition().formula().getChildren().size()<<endl;

/*	for(set<lprec *>::iterator citr = retlpset.begin();citr!=retlpset.end();citr++){
			//write_LP((*citr),stdout);
			lprec* cpp=copy_lp(*citr);
			set_presolve(cpp,PRESOLVE_ROWS,10);
			//REAL rowe[nPlaces];
			//for(int i=0;i<nPlaces+1;i++) rowe[i]=1;
			//if(!set_obj_fn(cpp, rowe)) cout<<"eroare";
			solve(cpp);
			print_constraints(cpp,1);
			write_LP(cpp,stdout);
			cout<<get_Nrows(cpp)<<" "<<get_Nrows(cpp)<<"fhfhf"<<endl;}
//trial here we suppose that we have a conjunction only
*/

		// map places to row indices position i in the set corresponds to position
	
	//std::size_t index_of_element(std::vector<std::string&>& vec, const std::string& elem);

	// for starters  no set
	//set<const lprec *> lpset;
//	const pnapi::formula::Formula* f=&net1.finalCondition().formula();
	

	//adding a lp given a previous one which is not complete (for disjunction)
	//disjunction always builds different lp problems
	//conjunction adds to one or more previously built lp problems - if there is non build the first one all the constraints should be
	// pentru fiecare copil 
//	for(set<const Formula *>::iterator it = form.children.begin();f!=form.children.end();it++)


	// initialize each row with the difference between the output and the input
	// here we compute F+ - F- and then we ad all possible termination values according to the final constraints computed above
	// further we should be able to add
 	//computeTInvariants(net1);

    
	// here we prepare for the the solving of the equation
	// for each final marking one builds and solves  the state equation

	if(args_info.inputs_num>0){//args_info.adapterrules_given){//args_info.messageProfiles_given>0
		cout << endl <<"State equation involved"<<endl;
		nPlaces = (int) net1.getPlaces().size();cout << "No of places "<< net1.getInternalPlaces().size() << endl;
		
		nTransitions = (int) net1.getTransitions().size();
		pnapi::Marking m(net1);
		net1.finalCondition().dnf(); 
		const pnapi::formula::Formula * f=dynamic_cast<const pnapi::formula::Formula *>(&net1.finalCondition().formula());
		
		set< map<std::string const, unsigned int> > fmset;//set of final markings (set of maps)
		if(typeid(*f)==typeid(pnapi::formula::Disjunction)){ 
			//std::cout<<"Disjunction"<<endl;//make disjunction two by two
			const pnapi::formula::Disjunction *fd=dynamic_cast<const pnapi::formula::Disjunction *>(f);			
			for(std::set<const pnapi::formula::Formula *>::iterator cIt=fd->children().begin();cIt!=fd->children().end();++cIt){
				//here we should have a conjunction; but philosophers do not agree with it :(
				//const pnapi::formula::Conjunction *fc=dynamic_cast<const pnapi::formula::Disjunction *>(*cIt);
				//(*cIt)->output(cout);cout<<endl;
				if(((*cIt)->getType()==pnapi::formula::Formula::F_CONJUNCTION)){//typeid(*cIt)==typeid(pnapi::formula::Conjunction)){ 
					const pnapi::formula::Conjunction *fc=dynamic_cast<const pnapi::formula::Conjunction *>(*cIt);
					map<std::string const, unsigned int>  cfm;//build a marking
					for(std::set<const pnapi::formula::Formula *>::iterator ccIt=fc->children().begin();ccIt!=fc->children().end();++ccIt){
						const pnapi::formula::Proposition *fp=dynamic_cast<const pnapi::formula::Proposition *>(*ccIt);
						//const Place *pp=new Place(fp->place());
						if(fp->tokens()>0)//insert only if it is non-zero
						cfm.insert( std::pair<std::string const, unsigned int>(fp->place().getName(),fp->tokens()) );
					}
					fmset.insert(cfm);
				}

			}
		}
		if(typeid(*f)==typeid(pnapi::formula::Conjunction)){ 
			map<std::string const, unsigned int>  cfm;//build a marking
			const pnapi::formula::Conjunction *fc=dynamic_cast<const pnapi::formula::Conjunction *>(f);
			for(std::set<const pnapi::formula::Formula *>::iterator cIt=fc->children().begin();cIt!=fc->children().end();++cIt){
				const pnapi::formula::Proposition *fp=dynamic_cast<const pnapi::formula::Proposition *>(*cIt);
				//const Place *pp=new Place(fp->place());
				if(fp->tokens()>0)
				cfm.insert( std::pair<std::string const, unsigned int>(fp->place().getName(),fp->tokens()) );
			}
			fmset.insert(cfm);
		}
		cout << "No of final markings "<< fmset.size() << endl;
		
		//now transform them into sets of vectors
		
		//retlpset=transform(net1, f);//cout<<"size of "<<retlpset.size()<<endl;
		//start_clock = clock();
		if(coverall) {
			std::set<std::string> sets;// fill in the set of nodes of the composition
			std::set<pnapi::Place*> pl;
			for(std::set<pnapi::Node*>::iterator p=net1.getNodes().begin();p!=net1.getNodes().end();++p){
				Place * pl = dynamic_cast<Place *> ((*p));
				if(pl==NULL){	sets.insert((*p)->getName());//cout<<(*p)->getName()<<endl;//transition
					std::string st,cs;
 					if((*p)->getName().find("@")!=std::string::npos) {
						std::string st,cs;
						st=(*p)->getName().substr(0,(*p)->getName().find("@"));
						cs=(*p)->getName().substr((*p)->getName().find("@")+1);
	 					stringstream ss;ss<<st;
 						int net; ss>>net;
 						//const int netc=net;//const std::string cs;
						if(net==0){abort(2, "wrong format for enforce events. Read the manual.\n");}
//		if(s.find(".")!=std::string::npos) cs=s.substr(s.find(".")+1);//cout<<" "<<s.substr(s.find(".")+1)<<endl;
						std::set<std::string> sets;sets.insert(cs);
  						if(enforcedT.find(net)==enforcedT.end()) {
							enforcedT.insert(std::pair<unsigned int, set<std::string> >(net,sets));
						}
	  					else{std::set<std::string> se=(enforcedT.find(net))->second;
		  					se.insert(cs);
			  				enforcedT.find(net)->second=se;//cout<<"old"<<endl;
// 					//enforcedT.insert(std::pair<unsigned, set<std::string> >)(net,s.substr(s.find(".")+1 ));			
						}
					}
					else {//interface 
						if(enforcedT.find(0)==enforcedT.end()) {
							std::set<std::string> sets;sets.insert(pl->getName());
							enforcedT.insert(std::pair<unsigned int, set<std::string> >(0,sets));	
						}
						else{   set<std::string> se=(enforcedT.find(0))->second;
	  						se.insert(cs);
	  						enforcedT.find(0)->second=se;//cout<<"old"<<endl;
// 				//enforcedT.insert(std::pair<unsigned, set<std::string> >)(net,s.substr(s.find(".")+1 ));	
						}
					}
				}
				else if ((pl->getPresetArcs().size()==0)&&(pl->getTokenCount()==0)) 
					abort(2, "input place is unmarked");
			}
		}
	
			//enforcedT.insert(std::pair<unsigned int, set<std::string> >(1,sets));
	//iterating all clauses
	//for(set<lprec *>::iterator citr = retlpset.begin();citr!=retlpset.end();citr++){
		for(set<map<std::string const ,unsigned int> >::iterator sfm=fmset.begin();sfm!=fmset.end();++sfm){
			lprec *lp;//lp = make_lp(0, nTransitions);
		
			lp = make_lp(0, nTransitions);
			if(lp == NULL) {
				std::cerr<< "Unable to create new LP model"<<std::endl;
				return(1);
			}
			//first add the objective function
			set_add_rowmode(lp,TRUE);
			set_obj_fnex(lp,0, NULL,NULL);//set_verbose(lp,FULL);//IMPORTANT);
			//get_Ncolumns(lp)
			
			
		
		    //set the col/variable names
			int k=1;
			for(std::set<pnapi::Transition *>::iterator cit=net1.getTransitions().begin();cit!=net1.getTransitions().end();++cit){
			//cout<<(*cit)->getName()<<" ";
				char * cstr= new char [(*cit)->getName().size()+1];
				strcpy(cstr,(*cit)->getName().c_str());
					set_col_name(lp, k, cstr);k++;
			}
			int ii=0; 
			//for(set<const pnapi::Place *>::iterator p=sfm->begin();p!=sfm->end();++p){
			for(std::set<Place *>::const_iterator p = net1.getPlaces().begin();p!=net1.getPlaces().end();p++){
			//add the constraint's left side
			//cout<<(*p)->getName()<<"="<<(*p)->getTokenCount()<<std::endl;
				int j=1;
		//	cout<<"m["<<(*p)->getName()<<"]"<<"="<<(*p)->getTokenCount()<<std::endl;
				Place & l= *net1.findPlace((*p)->getName());
				bool de=((*p)->getName().find("_int") != std::string::npos)&& ((*p)->getPresetArcs().empty()||(*p)->getPostsetArcs().empty());
				bool ccob=((*p)->getName().find("observe_") != std::string::npos)|| ((*p)->getName().find("control_") != std::string::npos);
				if(args_info.adapterrules_given&& ccob){
					//if it has no inputs or outputs it can be diconsidered for the state equation 
					//_engine->deletePlace(**plit);
					std::cout<<std::endl<<(*p)->getName()<<std::endl;
					continue;
				}
				if(args_info.adapterrules_given&& de&& args_info.inputs_num>0){
					//if it has no inputs or outputs it can be diconsidered for the state equation 
						//_engine->deletePlace(**plit);
						std::cout<<std::endl<<(*p)->getName()<<std::endl;
						continue;
				}
				
				REAL * roww=new REAL[l.getPresetArcs().size() + l.getPostsetArcs().size()]();
				if((args_info.inputs_num==1)&&(l.getType()!=pnapi::Node::INTERNAL)) continue;
				unsigned int k=0;
				int * colno=new int[l.getPresetArcs().size() + l.getPostsetArcs().size()]();
				REAL * rowe= new REAL[l.getPresetArcs().size() + l.getPostsetArcs().size()]();
				// iterate post arcs of p and add rowe[transition] the arc weight
				for (std::set<pnapi::Arc *>::iterator pait=l.getPresetArcs().begin(); pait!= l.getPresetArcs().end(); ++pait) {
					Transition & t=(*pait)->getTransition();
						//cout<<t.getName();
					unsigned int fp=(*pait)->getWeight();
					colno[k] = get_nameindex(lp,const_cast<char *>(t.getName().c_str()),FALSE);
					rowe[k++]= fp+0.0;
				}
				for (std::set<pnapi::Arc *>::iterator pait=l.getPostsetArcs().begin(); pait!= l.getPostsetArcs().end(); ++pait) {
					Transition & t=(*pait)->getTransition();
					unsigned int fm=(*pait)->getWeight();
					//if some of the transitions have been visited in the previous iteration then 
					if (net1.findArc(t,l)!=NULL) {
						//iterate previously visited transitions and get the id of the column
						// and for that column do some special operations
						for (unsigned inti=0; i<k; ++i) {
							if (colno[i]==get_nameindex(lp,const_cast<char *>(t.getName().c_str()),FALSE)) {
								rowe[i]=rowe[i]- 1*(int)fm;
							}
						}
					}
					else{ 
						colno[k] = get_nameindex(lp,const_cast<char *>(t.getName().c_str()),FALSE);
						rowe[k++]= - 1*(int)fm ; 
					}
				}
				//iterate prearcs of p and substract from rowe[transition] the arc weight
				
/*				for(std::set<Transition *>::const_iterator t = net1.getTransitions().begin();t!=net1.getTransitions().end();t++){
				//cout<<(*t)->getName();
					
					Transition & r= *net1.findTransition((*t)->getName());
					if(net1.findArc(r,l)!=NULL) fp=net1.findArc(r,l)->getWeight();
					if(net1.findArc(l,r)!=NULL) fm=net1.findArc(l,r)->getWeight();
					if (fp!=fm) {
						colno[k] = get_nameindex(lp,const_cast<char *>((*t)->getName().c_str()),FALSE);
						rowe[k++]= fp-fm;
					}
					//roww[j]=fp-fm;
				//cout<<roww[j]<<"="<<fp<<" minus "<<fm<<std::endl;
				//net1.findArc( net1.findPlace((*p)->getName()), net1.findTransition((*t)->getName()));
					j++;	
				}
*/			//now add row
				//char * cstr= new char [(*p)->getName().size()+1];
				//strcpy(cstr,(*p)->getName().c_str());const std::string ssss=(*p)->getName();
				REAL rhs;
				char *cstr= new char [(*p)->getName().size()+1];
				strcpy(cstr,(*p)->getName().c_str());
				std::map<std::string const, unsigned int> mmm=(*sfm); ///mit=sfm->find(ssss);
				if(mmm.find((*p)->getName())!=mmm.end()){
					rhs=0.0+mmm[cstr]-1*(int)((*p)->getTokenCount());//final marking - initial marking
				}
				else {
					rhs = 0.0-1*(int)((*p)->getTokenCount());
				}

				//REAL rhs=((*p)->getTokenCount());
			//cout<<"rhs "<<rhs<<" for "<<(*p)->getTokenCount()<<mmm[ssss]<<endl;
			//here is the variant part that might change due to the final marking 
			//instead only eq is needed for the respective place and rsh is the number of tokens (0/1 usually)
				/*if(get_nameindex(*citr,cstr,TRUE)==-1){ 
				//get flag if it is all other external places empty, no flag
					if(!add_constraint(lp, roww,EQ, rhs))
								cout<<"gata"<<endl;
				}
				else{ 
					//cout<<(*p)->getName()<<"?"<<get_rh(*citr, get_nameindex(*citr,cstr,TRUE))<<" AND ";
					rhs=rhs+get_rh(*citr, get_nameindex(*citr,cstr,TRUE));
					if(!add_constraint(lp, roww,get_constr_type(*citr,get_nameindex(*citr,cstr,TRUE)), rhs))
								cout<<"gata"<<endl;
				}*/
			if(!add_constraintex(lp,k,rowe,colno, EQ, rhs)) cout<<"nu merge"<<endl;
			//if(!add_constraint(lp, roww,EQ, rhs))
			//	cout<<"gata"<<endl;
				set_row_name(lp,get_Nrows(lp), cstr);
			//add the constrain's right side
			//get the row with the palce row name
			//cout<<endl<<get_nameindex(*citr,cstr,TRUE)<<" "<<get_rh(*citr, get_nameindex(*citr,cstr,TRUE))<<endl;
// //			set_row_name(lp, i, cstr);			
		//add_constraint(lp,row, EQ, 2.0);
		////for(int k=1;k<=get_Ncolumns(lp);k++){set_mat(lp,i,k,roww[i]);} cout<<endl;
				ii++;
			}
			
			set_add_rowmode(lp,FALSE);//for the mixed check collums are added, so we tun this off
			//can ve improved by estimating the number of columns later
			// add constraints for message profiles (variables included)
			if(args_info.messageProfiles_given>0){
			cout<<"Fingerprint+state equation check"<<endl;
			lprec *lpmc=copy_lp(lp);
			
		/*	
			//match interface for composition
			std::set<std::string> inputp,outputp;
			for (std::set<pnapi::Place *>::iterator pit=net1.getInputPlaces().begin(); pit!=net1.getInputPlaces().end(); ++pit) {
				inputp.insert((*pit)->getName());
			}
			for (std::set<pnapi::Place *>::iterator pit=net1.getOutputPlaces().begin(); pit!=net1.getOutputPlaces().end(); ++pit) {
				outputp.insert((*pit)->getName());
			}
			for (std::set<std::string>::iterator ssit=inputPlaces.begin(); ssit!=inputPlaces.end(); ++ssit) {
				if (inputp.find(*ssit)!=inputp.end()) {
					cout << "input places should be disjunct"<<std::endl;
					cout<<*ssit<<endl;
					exit(1);
				}
			}
			for (std::set<std::string>::iterator ssit=outputPlaces.begin(); ssit!=outputPlaces.end(); ++ssit) {
				if (outputp.find(*ssit)!=outputp.end()) {
					cout<<*ssit<<endl;cout << "output places should be disjunct"<<std::endl;
					exit(1);
				}
			}
			*/
			//
			set<lprec *> mps;
			mps.insert(lp);lp=lpmc;
			
			//match the interfaces
			//for each final marking
			for (unsigned int ifm=0; ifm<lpmps.size(); ifm++) {//int kn=1;
//					mps.insert(lp);lp=lpmc;cout<<get_col_name(lp,get_Ncolumns(lp))<<"help"<<endl;
					//aici adauga constraints	
				//adauga variabiles for places to relate them to their input and output transitions
				//cout << endl<<"for each final marking"<<endl;
				unsigned int nvars=get_Ncolumns(lpmps.at(ifm));
				for(unsigned int j=0;j<nvars;j++){//to do if it is already there skip
					//cout<<endl<<get_col_name(lpmps.at(ifm),j+1)<<endl;
					std::string colname=get_col_name(lpmps.at(ifm),j+1); 
					//for adapter's sake add an internal place
					if(args_info.adapterrules_given) {colname+="_int";
						//find the index of the row and set it to 1
					}
					Place & l= *net1.findPlace(colname); //the place is in the net/adapter
					if(&l==NULL) {
						continue;
					} //place is already there or it is a sync transition 
					//cout<<get_col_name(lpmps.at(ifm),j+1)<<" ";
					//REAL column[1+get_Nrows(lp)];//the set of events which is supposed to become variables in the SE
					//for(int i=0;i<=get_Nrows(lp);i++){
					//	column[i]=0.0;	
					//}
					add_columnex(lp,0,NULL,NULL);//then add constraint iff there is some connection between 
					//and also find the index of the row in the adapter setting
					set_col_name(lp, get_Ncolumns(lp),get_col_name(lpmps.at(ifm),j+1));
					if(args_info.adapterrules_given>0) {
						//cout<<endl<<"hello adapter "<<endl;
						char * cstr= new char [colname.size()+1];
						strcpy(cstr,colname.c_str());
						set_mat(lp, get_nameindex(lp, cstr,TRUE),get_Ncolumns(lp),1);
					}
					if(args_info.adapterrules_given==0){//when adapters do no exist the interface places are not there
						//so new rows have to be created for them
					REAL * roww= new REAL[1+get_Ncolumns(lp)];
					for(int r=0;r<get_Ncolumns(lp);r++) roww[r+1]=0.0;
					roww[get_Ncolumns(lp)]=1.0;
					for(std::set<pnapi::Transition *>::iterator cit=net1.getTransitions().begin();cit!=net1.getTransitions().end();++cit){
						//cout<<(*cit)->getName()<<" ";
						char * cstr= new char [(*cit)->getName().size()+1];
						strcpy(cstr,(*cit)->getName().c_str());
						int fp=0,fm=0;
						Transition & r= *net1.findTransition((*cit)->getName());
						if(net1.findArc(r,l)!=NULL){fp=net1.findArc(r,l)->getWeight();}
						if(net1.findArc(l,r)!=NULL) {fm=net1.findArc(l,r)->getWeight();}
						roww[get_nameindex(lp, cstr,FALSE)]=-fabs(fp-fm);//j is the current place
					}
					
					add_constraint(lp,roww,EQ,0.0);//cout<<get_Ncolumns(lp)<<endl;
					set_row_name(lp, get_Nrows(lp),get_col_name(lpmps.at(ifm),j+1));
					}
				}
				//now add the constraints	(? from the message profile ...)
				//set_add_rowmode(lp,TRUE); add the constraints of the message profiles
				int hh=1+get_Ncolumns(lp);//set_verbose(lp,5);//print_lp(lpmps.at(ifm)); //cout<<"col"<<hh<<"dd";
				for(int i=0;i<get_Nrows(lpmps.at(ifm));i++){
					REAL * rowp=new REAL[1+get_Ncolumns(lpmps.at(ifm))](); 
					REAL * rowwpp=new REAL[hh]();//int *colp;
					char grex=get_row(lpmps.at(ifm),i+1,rowp);
					for(int j=0;j<hh-1;j++)rowwpp[j]=0;
				        for(int j=0;j<get_Ncolumns(lpmps.at(ifm));j++){rowwpp[j+hh-get_Ncolumns(lpmps.at(ifm))]=rowp[j+1];}
					//cout<<endl;for(int j=0;j<hh;j++){cout<<rowwpp[j]<<" ";}
						//if( grex==-1) cout<<"gata";
					add_constraint(lp,rowwpp,get_constr_type(lpmps.at(ifm),i+1),get_rh(lpmps.at(ifm),i+1));
//					for(int j=0;j<get_Ncolumns(lpmp);j++){set_mat(lp,get_Nrows(lp),1+get_Ncolumns(lp)-(get_Ncolumns(lpmp)-j),rowp[j]);cout<<rowwpp[j]<<" ";}
//					set_rh_range(lp,get_Nrows(lpmp),get_rh_range(lpmp,i+1));
					 //set_add_rowmode(lp,FALSE);
				}
				set_verbose(lp,IMPORTANT);
				
			}
			
		}
			//add constraints for cover places and transitions
		
			if(args_info.enforceEvents_given>0){
			bool globres=1;
			for(map<int,std::set<std::string> >::const_iterator itr=enforcedT.begin();itr!=enforcedT.end();++itr){
				//cout<<"enforced "<<itr->first;
				for(set<std::string>::const_iterator it=(itr->second).begin();it!=(itr->second).end();++it){
					lprec* nn=copy_lp(lp);
					std::string gg;
					if(itr->first==0) gg=*it;
					else{ stringstream ss;ss<<itr->first<<"@"<<(*it); ss>>gg;}
					char * cstr= new char [gg.size()+1];
					strcpy(cstr,gg.c_str());strcpy(cstr,gg.c_str());//cout<<gg<<cstr<<endl;
					if(get_nameindex(nn,cstr,FALSE)!=-1) set_lowbo(nn, get_nameindex(nn,cstr,FALSE),1);
					else{	//internal place which needs extra variable and constraint
						if(net1.findPlace(gg)!=NULL) cout<<gg;
						else abort(2,"non-identifiable node for cover");
						add_columnex(nn,0,NULL,NULL);REAL *roww=new REAL[get_Ncolumns(nn)+1]();
						set_col_name(nn,get_Ncolumns(nn), cstr);
						for(int rr=0;rr<get_Ncolumns(nn);rr++) roww[rr+1]=0.0;
						roww[get_Ncolumns(nn)]=1.0;
						Place & l= *net1.findPlace(gg);
						for(std::set<pnapi::Transition *>::iterator cit=net1.getTransitions().begin();cit!=net1.getTransitions().end();++cit){
						//cout<<(*cit)->getName()<<" ";
							char * cstr= new char [(*cit)->getName().size()+1];
							strcpy(cstr,(*cit)->getName().c_str());
							int fp=0,fm=0;
							Transition & r= *net1.findTransition((*cit)->getName());
							//if(net1.findArc(r,l)!=NULL){fp=net1.findArc(r,l)->getWeight();}
							if(net1.findArc(l,r)!=NULL) {fm=net1.findArc(l,r)->getWeight();}
							roww[get_nameindex(nn, cstr,FALSE)]=-fabs(fp-fm);//j is the current place
						}
						if(!add_constraint(nn, roww,EQ, 0.0))	cout<<"gata"<<endl;
						set_lowbo(nn, get_nameindex(nn,cstr,FALSE),1);
					}
					
					set_obj_fnex(nn,0, NULL,NULL);//nn,rowobj);
					set_verbose(nn,IMPORTANT);
					for(int i=1;i<=get_Ncolumns(nn);++i){
						set_int(nn,i,TRUE);
					}
					//print_lp(nn);
					res=solve(nn);	
					if(res==0){
						
						if(args_info.verbose_flag){
							//cout<<endl<<"Compatibility check inconclusive"<<endl;
							cout<<endl<<"The constraint system has at least a solution including "<<gg<<endl;
							REAL *sol;//[get_Ncolumns(nn)];
							get_ptr_variables(nn,&sol);
							for(int s=0;s<get_Ncolumns(nn);s++){
								cout<<get_col_name(nn,s+1)<<" "<<sol[s]<<" "<<endl;//" "<<static_cast<int>(sol[s]) not needed anymore
							}
							cout<<endl;
							globres=0;
						}					
					}
				}
				if (globres==0) {
					res=0; break;
				}
			}}
			else{
				//here solve the SE/SE and SE/FP system
				cout << "Number of interface places: "<< net2.getInterfacePlaces().size()<<endl;
				cout << "Number of variables (transitions): "<< net1.getTransitions().size()<<endl;
				cout<<"Number of constraints (places): "<<get_Nrows(lp)<<"; Number of variables:"<<get_Ncolumns(lp)<<endl;
				
				set_verbose(lp,NORMAL);//,IMPORTANT);
				for(int i=1;i<=get_Ncolumns(lp);++i){
					set_int(lp,i,TRUE);
				}
				//cout<<"Printing the system: "<<endl;print_lp(lp);
				res=solve(lp);//write_lp(lp, "model.mp");
				//cout<<"Solve "<<solve(lp)<<endl;
				if(res==0){
					//cout<<endl<<"Compatibility check inconclusive"<<endl;
					if (args_info.verbose_flag) {
						cout<<endl<<"The constraint system has at least a solution:"<<endl;
						REAL *sol;//[get_Ncolumns(nn)];
						get_ptr_variables(lp,&sol);//REAL sol[nTransitions];
				//get_variables(lp,&sol);
						for(int s=0;s<nTransitions;s++){
							cout<<get_col_name(lp,s+1)<<" "<<sol[s]<<" "<<endl;//" "<<static_cast<int>(sol[s]) not needed anymore
						}
						cout<<endl;
					}
/*			bool gata=true;
			pnapi::Marking m(net1);
			do{
				for(int s=0;s<nTransitions;s++){
					const pnapi::Transition* t=net1.findTransition(get_col_name(lp,s+1));
					if(m.activates(*t)){ 
						m=m.successor(*t);//see whether it is the final marking
						if(net1.finalCondition().isSatisfied(m)) gata=true;
					}
					if((sol[s]==0)&&(s==nTransitions-1)) {gata=true;break;}
					cout<<get_col_name(lp,s+1)<<" "<<sol[s]<<" "<<endl;//" "<<static_cast<int>(sol[s]) not needed anymore
				}
			}
			while(gata!=true);*/
			//check reachability
				break;
		}
		else{std::cout<<endl<<"So far no solution  ..."<<endl;}
		}
	}
	}
	if(res!=0){std::cout<<endl<<"Not compatible wrt weak termination"<<endl;}
	else cout<<endl<<"Compatibility check inconclusive"<<endl;
	

	
	//time(&end_time);
	//status("checked necessary condition for weak termination  in [%.0f sec]", difftime(end_time,start_time));
	
	
	std::cerr << PACKAGE << ": runtime: " << ((double(clock()) - double(start_clock)) / CLOCKS_PER_SEC) << " sec\n";
	std::cerr << PACKAGE << ": memory consumption: "; system((std::string("ps | ") + TOOL_GREP + " " + PACKAGE + " | " + TOOL_AWK + " '{ if ($1 > max) max = $1 } END { print max \" KB\" }' 1>&2").c_str());
	return 0;
	
}
