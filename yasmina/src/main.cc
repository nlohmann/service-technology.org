#include "config.h" //pentru ca asa a spus tipul ala care se crede tare
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


using std::cout;
using std::endl;

using pnapi::PetriNet;
using pnapi::Place;
using pnapi::Transition;
using pnapi::Arc;
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
std::vector<std::pair<int,int> > bMP;

extern int mp_yylineno;
extern int mp_yydebug;
extern int mp_yy_flex_debug;
extern FILE* mp_yyin;
extern int mp_yyerror();
extern int mp_yyparse();
std::vector<std::map<std::string, int> > fmar;

#ifdef YY_FLEX_HAS_YYLEX_DESTROY
extern int mp_yylex_destroy();
#endif

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
	if (((args_info.inputs_num < 1)&&(args_info.messageProfiles_given<1))) {
		fprintf(stderr, "%s: too few arguments --- (at least one input .owfn file) or (at least one .owfn file and one message profile) must be given -- aborting\n", PACKAGE);
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
	std::cout<<"Transform Negation"<<endl;
	//if(typeid(form)==typeid(pnapi::formula::Negation)) form->output(std::cout);
/*	else{ 
		cout<<"The formula is not a negation"<<endl;
		return form;
	}*/
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
	}
}

std::set<lprec *>  transform(const pnapi::PetriNet &net1, const pnapi::formula::Formula * form){
	//std::cout<<"transform"<<std::endl;
	std::set<lprec *> retlpset;
	REAL rowpl[1+ net1.getPlaces().size()];
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
		const pnapi::formula::Operator *fop=dynamic_cast<const pnapi::formula::Operator *>(form);
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
					cout<<"continue"<<previous.size()<<current.size()<<endl;continue;
				}
				//cout<<"nu vad"<<previous.size()<<current.size()<<endl;
				int ind=0;
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
							int colno[Ncol];
							REAL rowe[Ncol];
							/* create space large enough for one row */
							//colno = (int *) malloc(Ncol * sizeof(*colno));
							//rowe = (REAL *) malloc(Ncol * sizeof(*rowe));
							
							int nr_of_cols = get_rowex(*cIt2,j,rowe,colno);
							//cout<<"Sizeof "<<sizeof(rowe)<<" and j is " << j << endl;
							//for(int k=0;k<1+ net1.getPlaces().size();k++) cout<<rowpl[k]<<" ";
;							set_verbose(lpcopy,FULL);//cout<<"Constraint type "<<get_constr_type(*cIt2,j)<<endl;
							if(!add_constraintex(lpcopy, nr_of_cols ,rowe,colno ,get_constr_type(*cIt2,j), get_rh(*cIt2,j)))
								cout<<"gata"<<endl;
							set_row_name(lpcopy,get_Nrows(lpcopy), get_row_name(*cIt2,j));
							//cout<<get_nameindex(lpcopy,get_row_name(lpcopy,get_Nrows(lpcopy)-2),TRUE)<<get_row_name(lpcopy,get_Nrows(lpcopy)-1)<< "am adaugat"<<get_row_name(lpcopy,get_Nrows(lpcopy)-1)<<"si"<<get_row_name(lpcopy,get_Nrows(lpcopy))<<endl;
							set_verbose(lpcopy,NORMAL);
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
}


// 

int main(int argc, char** argv) {
	
	//time_t start_time, end_time;
	clock_t start_clock = clock();
	evaluateParameters(argc, argv);
	std::cerr << PACKAGE << " processing ";
	if(args_info.inputs_num>0) {
		for (int i = 0; i < args_info.inputs_num; i++)
        	{
                	std::cerr << " " << args_info.inputs[i]<<" ";
        	}
		std::cerr  <<std::endl;
	}
	//the right way using only the open nets
	//the stupid way using the compose function which is not implemented correctly
	// parse the open nets and obtain the incidence matrix of the composition and the corresponding final marking(s)
	pnapi::PetriNet net1, net2;
	
	std::string s1("0");
	std::ifstream inputStream;

	//time(&start_time);
	int i = 0;
	//initialization of parameter structures	
 	map<int, set<std::string> > enforcedT;
 	map<int, set<set<std::string> > >  enforcedFC;	
	int nPlaces;
	int nTransitions;
	std::set<lprec *> retlpset;//set of all final markings of the composition
 	if(args_info.enforceEvents_given){
		for (i = 0; i < args_info.enforceEvents_given; ++i){
 		std::string s=args_info.enforceEvents_arg[i];std::string st,cs;
  		if(s.find("@")!=std::string::npos) {st=s.substr(0,s.find("@"));//cout<<st<<endl;
			cs=s.substr(s.find("@")+1); 		
 			stringstream ss;ss<<st;
 			int net; ss>>net;//cout<<net;
 			const int netc=net;//const std::string cs;
//		if(s.find(".")!=std::string::npos) cs=s.substr(s.find(".")+1);//cout<<" "<<s.substr(s.find(".")+1)<<endl;
 			set<std::string> sets;sets.insert(cs);
  			if(enforcedT.find(net)==enforcedT.end()) {
				enforcedT.insert(std::pair<unsigned, set<std::string> >(net,sets));
				cout<<"new";
			}
  			else{set<std::string> se=(enforcedT.find(net))->second;
	  			se.insert(cs);
	  			enforcedT.find(net)->second=se;//cout<<"old"<<endl;
// 				//enforcedT.insert(std::pair<unsigned, set<std::string> >)(net,s.substr(s.find(".")+1 ));		
			}
		}
		else abort(2, "wrong format for enforce events. Read the manual.\n");
 	}
	}
	if(args_info.enforceEvents_given){
 	for (i = 0; i < args_info.enforceFC_given; ++i){
 //		std::string s=args_info.enforceFC_arg[i];
//  		std::string st=s.substr(0,s.find("."));stringstream ss;ss<<st;
// 		int net; ss>>net;
// 		const int netc=net;const std::string cs=s.substr(s.find(".")+1);//cout<<" "<<s.substr(s.find(".")+1)<<endl;
// 		set<std::string> sets;sets.insert(cs);
// 		if(enforcedT.find(net)!=enforcedT.end()) 
// 			{enforcedT.insert(std::pair<unsigned, set<std::string> >(net,sets));cout<<"new";}
// 		else{set<std::string> se=(enforcedT.find(net))->second;
// 			se.insert(cs);
// 			enforcedT.find(net)->second=se;cout<<"old"<<endl;}
// 				//enforcedT.insert(std::pair<unsigned, set<std::string> >)(net,s.substr(s.find(".")+1 ));		
 	}
	}
	
	
	//parse the input nets
	if(args_info.inputs_num>0){
//		try {
		// std::string nets[args_info.inputs_num];	
		for(int i=0; i<args_info.inputs_num;i++){
			if(i==0){			
				ifstream ifs1(args_info.inputs[i]);
				inputStream.open(args_info.inputs[i]);
				try { ifs1 >> owfn >> net1;}
				catch (InputError e) { std::cerr <<"net " <<i<<" failed" << endl << e << endl; assert(false); }
				ifs1.close();
				//const int zero=0; now check whether these are real transitions
				if(enforcedT.find(0)!=enforcedT.end()&&(args_info.enforceEvents_given>0)){
					set<std::string> se=(enforcedT.find(0))->second;
					for (std::set<std::string>::iterator it=se.begin(); it!=se.end(); it++)
						if(net1.findTransition(*it)!=NULL) cout<<(*it);
						else abort(2, "the transitions do not belong to the net");
				}
				
				if(args_info.inputs_num==1) break;
			}
			else{
			  std::stringstream s;s<<(i); cout<< s.str();
			  std::string s2; s2=s.str()+"@";//"net"+s.str();
			  ifstream ifs2(args_info.inputs[i]);
			  try { ifs2 >> owfn >> net2;}
			  catch (InputError e) { std::cerr <<"net " <<i<<" failed"<< std::endl << e << endl; assert(false); }
			  ifs2.close();
			  if((enforcedT.find(i)!=enforcedT.end())&&(args_info.enforceEvents_given>0)){
					set<std::string> se=(enforcedT.find(i))->second;
				  for (std::set<std::string>::iterator it=se.begin(); it!=se.end(); it++)
					  if(net1.findTransition(*it)!=NULL) cout<<(*it);
					  else abort(2, "the transitions do not belong to the net");
			  }
			   //else abort(3, "the transitions do not belong to the net");
			if(i==1) s1="0@"; else s1=""; net1.compose(net2, s1, s2);
			//renew the prefixes: s1 has the old 
		//	if(!net1.isClosed()){cout<<" is not closed"<<std::endl;} else 
			}
		}
		//std::cout << owfn << net1<<"end of composition"<<std::endl;
			nPlaces = (int) net1.getPlaces().size();
			nTransitions = (int) net1.getTransitions().size();
			
			// get the initial marking
			pnapi::Marking m(net1);
			const pnapi::formula::Formula * f=dynamic_cast<const pnapi::formula::Formula *>(&net1.finalCondition().formula());
			retlpset=transform(net1, f);//cout<<"size of "<<retlpset.size()<<endl;
				
		//}
//	catch (pnapi::io::InputError error) {
//		std::cerr << PACKAGE << error << std::endl;
//		exit(EXIT_FAILURE);}
	}
// print result
	//fflush(stdin);
	int res;//the result of the system
	//parse the message profile files
	if(args_info.messageProfiles_given>0){
		std::multimap<std::string,int> mpinvocation;set<std::string> hh;
		for (int imp = 0; imp <args_info.messageProfiles_given; ++imp) {//hh.insert(args_info.messageProfiles_arg[imp]);
			size_t found=std::string(args_info.messageProfiles_arg[imp]).find(".output");
			hh.insert(std::string(args_info.messageProfiles_arg[imp]).substr(0,found));
			mpinvocation.insert(std::pair<std::string,int>(std::string(args_info.messageProfiles_arg[imp]).substr(0,found),imp));
		}
		std::vector<lprec *> lpmps;// lprec *lpmc=copy_lp(lp);
		//get the variables of the previous set and try to compose, if it works add the xtra variables
		for(int c=0;c<lpmps.size();++c){
			
		} 
		std::set<std::string> resultinp, resultout,resultsyn, result, resintern;//interface of nets

		cout<<endl;
		for (set<std::string>::iterator sit=hh.begin(); sit!=hh.end(); ++sit) {
			cout <<"Net id: "<< *sit;int no=0;//i=0;
			std::vector<lprec *> mps;//here add constraints
			//if necessary do the union over the set of events
			
			for(std::multimap<std::string,int>::iterator it=mpinvocation.begin();it!=mpinvocation.end();++it){
				cout<<" argument "<<(*it).second<<endl;//cout<<"no"<<no<<endl;//
				no++;bMP.clear();
				std::cerr << PACKAGE << ": Message profile file <" << args_info.messageProfiles_arg[(*it).second] << ">" << std::endl;
				initialize_mp_parser();
				mp_yyin = fopen(args_info.messageProfiles_arg[(*it).second], "r");
				if (!mp_yyin) {
					std::cerr << "cannot open message profile file '" << args_info.messageProfiles_arg[(*it).second] << "' for reading'\n" << std::endl;exit(1);
				}
				term_vec = new std::vector<EventTerm*>();  ///initalize the event terms
				mp_yyparse();
				cout << inputPlaces.size()<< outputPlaces.size()<<synchrT.size()<<endl;
				//inputp,outputp the set of places from the previous composition
			/*	std::set<std::string> resultinp, resultout,resultsyn, result,synct,inputp,outputp;
				for (std::set<std::string>::iterator ssit=inputPlaces.begin(); ssit!=inputPlaces.end(); ++ssit) {
					if (inputp.find(*ssit)!=inputp.end()) {
						cout << "for now we consider only simple composition; we are considering instance composition for next versions of the tool";
						exit(1);
					}
					else result.insert(*ssit);
					if(outputp.find(*ssit)!=outputp.end()){ outputp.erase(outputp.find(*ssit));//matching
					}
					else resultinp.insert(*ssit);
					
				}
				for (std::set<std::string>::iterator ssit=outputPlaces.begin(); ssit!=outputPlaces.end(); ++ssit) {
					if (outputp.find(*ssit)!=outputp.end()) {
						cout << "for now we consider only simple composition; we are considering instance composition for next versions of the tool";
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
				resultinp.insert(inputp.begin(),outputp.end());resultout.insert(outputp.begin(),outputp.end());
				resultsyn.insert(synct.begin(),synct.end());
				//std::set_intersection(inputp.begin(), inputp.end(),inputPlaces.begin(), inputPlaces.end(), std::insert_iterator(resultinp));
	*/			//std::set_intersection(outputp.begin(), outputp.end(),outputPlaces.begin(), outputPlaces.end(), std::insert_iterator(resultout));
				lprec *lpt;//obtain 
				lpt = make_lp(0, result.size());
				for(i=1;i<=get_Ncolumns(lpt);i++){
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
				
				cout <<"no of final markings" << nfm<<fmar.size()<<"and"<<nc<<endl;

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
				for (int ifm=1;ifm<nfm+1;ifm++){//std::set<lprec *> ::iterator ifmi=mps.begin(); ifmi!=mps.end(); ++ifmi) {
					//if(no==1){
					int k=1;cout <<"before";
					lprec *lpmp=copy_lp(lpt);
					if(no==1) mps.push_back(lpmp);
					int kn=1;cout<<"hello"<<term_vec->size()<<endl;
					//int k=1;//print_lp(mps.at(ifm-1));
					//cout<<ifm-1<<"in"<<get_Nrows(mps.at(0))<<get_Nrows(mps.at(1))<<get_Nrows(mps.at(2))<<endl;
					REAL roww[1+inputPlaces.size()+ outputPlaces.size()+synchrT.size()],rowobj[1+inputPlaces.size()+ outputPlaces.size()+synchrT.size()];
				//initialize roww ith zero values
					for (std::vector<EventTerm*>::iterator it = term_vec->begin(); it != term_vec->end(); ++it) {
						//need to initialize the roww set_add_rowmode(lpmp, TRUE);
						for (k=1; k<1+inputPlaces.size()+ outputPlaces.size()+synchrT.size(); k++) {
							roww[k]=0;
						}
					//cout<<"Constraint#"<<kn<<": ";
						//cout <<"help" << bMP.size() << ifm << kn <<endl;
						//cout<<ifm-1<<"in"<<get_Nrows(mps.at(0))<<get_Nrows(mps.at(1))<<get_Nrows(mps.at(2))<<endl;
						std::pair<int,int> bd=bMP.at(term_vec->size()*(ifm-1)+kn-1);cout<<"bMP"<<bMP.size()<<endl;
						std::map<std::string const,int>* ee=EventTerm::termToMap((*it));
						//std::pair<int,int> b=bd.at(kn);
						cout << bd.first<<" "<<bd.second<<endl;
						
						for(std::map<std::string const,int>::iterator ite=ee->begin();ite!=ee->end();++ite){
						//cout<<ite->first<<" "<<ite->second<<std::endl;
							char * cstr= new char [ite->first.size()+1];
							strcpy(cstr,ite->first.c_str());
							roww[get_nameindex(mps.at(ifm-1), cstr,FALSE)]=ite->second;
						}
						int rhs=bd.first;
						if(!add_constraint(mps.at(ifm-1), roww,GE, rhs))
						cout<<"gata"<<endl;
						if (bd.second!=USHRT_MAX) {
							set_rh_range(mps.at(ifm-1),get_Nrows(mps.at(ifm-1)), bd.second-rhs);
						}
						else {
							set_rh_range(mps.at(ifm-1),get_Nrows(mps.at(ifm-1)), get_infinite(mps.at(ifm-1)));
						}
					//set_add_rowmode(lpmp, FALSE);
						
						kn++;
					}
					cout<<"out"<<get_Nrows(mps.at(ifm-1))<<endl;//cout<<"coloane:"<<get_Nrows(mps.at(ifm-1));//ifm++;
					//lp=lpmc;cout<<get_col_name(lp,get_Ncolumns(lp))<<"help"<<endl;
				//aici adauga constraints
					//print_lp(mps.at(ifm-1));
				}
			//for (int ifm=1;ifm<nfm+1;++ifm){	print_lp(mps.at(ifm-1));};cout<<mps.size()<<endl;	
				//add here to the previous set of constraints
				
			}	
			//here add final marking constraints;
			for (int i=0; i<nfm; ++i) {
				
				for (std::map<std::string,int> ::iterator itmar=fmar.at(i).begin(); itmar!=fmar.at(i).end(); ++itmar) {
					char * cstr= new char [(*itmar).first.size()+1];
					strcpy(cstr,(*itmar).first.c_str());
					int index=get_nameindex(mps.at(i),cstr,FALSE);set_bounds(mps.at(i),index, (*itmar).second,(*itmar).second);
				}
			}
			//here construct new constraints from the previous
			if(sit==hh.begin()) lpmps=mps;
			else{ //combine all final markings
				for(int i=0;i<lpmps.size();++i){
					lprec * lpr=copy_lp(lpmps.at(i));
					for(int j=0;j<nfm;++j){
						//add constraints
						//REAL roww[?];	
					}
				}
			}
		}
		//
		//solve
		for(int ifm=0;ifm<nfm;ifm++){
			//res=solve(mps.at(ifm));
			//cout<<"Solve "<<solve(lp)<<endl;
			if(res==0){
				cout<<endl<<"Compatibility check inconclusive"<<endl;
				cout<<endl<<"The constraint system has at least a solution:"<<endl;
				REAL sol[nTransitions];
				/*			get_variables(lp,sol);
				 for(int s=0;s<nTransitions;s++){
				 cout<<get_col_name(lp,s+1)<<" "<<sol[s]<<" "<<endl;//" "<<static_cast<int>(sol[s]) not needed anymore
				 }
				 cout<<endl;
				 bool gata=true;
				 /*			pnapi::Marking m(net1);
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
		}
	}


//	ofstream ofs2("r.owfn");
//	ofstream odot("r.dot");
//	std::ostringstream rfile("result.owfn");
//	std::ostringstream pfile("result.png");
//	ofs2<<owfn<<net1;
//	odot<<dot<<net1;
	/*ostringstream d;
	d<<pnapi::io::dot<<net1;
	std::string call=string(CONFIG_DOT)+"-t"+ "result.owfn"+"-q-o "+"t.png";
	FILE *s=popen(call.c_str(),"w");
	assert(s);
	fprintf(s, "%s\n", d.str().c_str());
	assert(!ferror(s));
	pclose(s);*/
	

	


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
	int ip=1;
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
	
	for(set<lprec *>::iterator citr = retlpset.begin();citr!=retlpset.end();citr++){
		lprec *lp;//lp = make_lp(0, nTransitions);
		
		if(args_info.inputs_num>0){
			lp = make_lp(0, nTransitions);
			//set_verbose(lp,0);
			REAL roww[get_Ncolumns(lp)];
			for(i=1;i<=nTransitions;i++){
				set_int(lp,i,TRUE);
			}
		
			if(lp == NULL) {
				std::cerr<< "Unable to create new LP model"<<std::endl;
				return(1);
			}
		
		//set_add_rowmode(lp, TRUE);
		//set the col/variable names
			int k=1;
			for(std::set<pnapi::Transition *>::iterator cit=net1.getTransitions().begin();cit!=net1.getTransitions().end();++cit){
			//cout<<(*cit)->getName()<<" ";
				char * cstr= new char [(*cit)->getName().size()+1];
				strcpy(cstr,(*cit)->getName().c_str());
					set_col_name(lp, k, cstr);k++;
			}
			int ii=0; 
			for(set<Place *>::const_iterator p = net1.getPlaces().begin();p!=net1.getPlaces().end();p++){
			//add the constraint's left side
			//cout<<(*p)->getName()<<"="<<(*p)->getTokenCount()<<std::endl;
				int j=1;
		//	cout<<"m["<<(*p)->getName()<<"]"<<"="<<(*p)->getTokenCount()<<std::endl;
				Place & l= *net1.findPlace((*p)->getName());
				if((args_info.inputs_num==1)&&(l.getType()!=pnapi::Node::INTERNAL)) continue;
				for(set<Transition *>::const_iterator t = net1.getTransitions().begin();t!=net1.getTransitions().end();t++){
				//cout<<(*t)->getName();
					int fp=0,fm=0;
					Transition & r= *net1.findTransition((*t)->getName());
					if(net1.findArc(r,l)!=NULL) fp=net1.findArc(r,l)->getWeight();
					if(net1.findArc(l,r)!=NULL) fm=net1.findArc(l,r)->getWeight();
					roww[j]=fp-fm;
				//cout<<roww[j]<<"="<<fp<<" minus "<<fm<<std::endl;
				//net1.findArc( net1.findPlace((*p)->getName()), net1.findTransition((*t)->getName()));
					j++;	
				}
			//now add row
				char * cstr= new char [(*p)->getName().size()+1];
				strcpy(cstr,(*p)->getName().c_str());
				REAL rhs=0.0-((*p)->getTokenCount());
			//cout<<"rhs "<<rhs<<" for "<<(*p)->getTokenCount()<<endl;
			//here is the variant part that might change due to the final marking 
				if(get_nameindex(*citr,cstr,TRUE)==-1){ 
				//get flag if it is all other external places empty, no flag
					if(!add_constraint(lp, roww,EQ, rhs))
								cout<<"gata"<<endl;
				}
				else{ 
					//cout<<(*p)->getName()<<"?"<<get_rh(*citr, get_nameindex(*citr,cstr,TRUE))<<" AND ";
					rhs=rhs+get_rh(*citr, get_nameindex(*citr,cstr,TRUE));
					if(!add_constraint(lp, roww,get_constr_type(*citr,get_nameindex(*citr,cstr,TRUE)), rhs))
								cout<<"gata"<<endl;
				}
				set_row_name(lp,get_Nrows(lp), cstr);
			//add the constrain's right side
			//get the row with the palce row name
			//cout<<endl<<get_nameindex(*citr,cstr,TRUE)<<" "<<get_rh(*citr, get_nameindex(*citr,cstr,TRUE))<<endl;
// //			set_row_name(lp, i, cstr);			
		//add_constraint(lp,row, EQ, 2.0);
		////for(int k=1;k<=get_Ncolumns(lp);k++){set_mat(lp,i,k,roww[i]);} cout<<endl;
				ii++;
			}
		}
		// add constraints for message profiles (variables included)
		if(args_info.messageProfiles_given>0){
		/*	std::multimap<std::string,int> mpinvocation;set<std::string> hh;
			for (int imp = 0; imp <args_info.messageProfiles_given; ++imp) {//hh.insert(args_info.messageProfiles_arg[imp]);
				size_t found=std::string(args_info.messageProfiles_arg[imp]).find(".output");
				hh.insert(std::string(args_info.messageProfiles_arg[imp]).substr(0,found));
				mpinvocation.insert(std::pair<std::string,int>(std::string(args_info.messageProfiles_arg[imp]).substr(0,found),imp));
			}
			lprec *lpmc=copy_lp(lp);
			cout<<endl;
			for (set<std::string>::iterator sit=hh.begin(); sit!=hh.end(); ++sit) {
				cout <<"Net id: "<< *sit;
				for(std::multimap<std::string,int>::iterator it=mpinvocation.begin();it!=mpinvocation.end();++it){
					cout<<" argument "<<(*it).second<<endl;//
					std::cerr << PACKAGE << ": Message profile file <" << args_info.messageProfiles_arg[i] << ">" << std::endl;
					initialize_mp_parser();
					mp_yyin = fopen(args_info.messageProfiles_arg[i], "r");
					if (!mp_yyin) {
						std::cerr << "cannot open message profile file '" << args_info.messageProfiles_arg[i] << "' for reading'\n" << std::endl;exit(1);
					}
					
				}	
				
			}
		*/	
			/*
			for (i = 0; i < args_info.messageProfiles_given; ++i){
				std::cerr << PACKAGE << ": Message profile file <" << args_info.messageProfiles_arg[i] << ">" << std::endl;
				initialize_mp_parser();
				mp_yyin = fopen(args_info.messageProfiles_arg[i], "r");
				if (!mp_yyin) {
					std::cerr << "cannot open message profile file '" << args_info.messageProfiles_arg[i] << "' for reading'\n" << std::endl;exit(1);
				}
				
				term_vec = new std::vector<EventTerm*>();  ///initalize the event terms
				//for(set<std::string>::const_iterator t = net1.getTransitions().begin();t!=net1.getTransitions().end();t++)
				mp_yyparse();
				cout << inputPlaces.size()<< outputPlaces.size()<<synchrT.size()<<endl;
				cout << nfm<<endl;
				//for each final marking
				//cout <<k-1<< nfm << " bla " << nc <<endl;
				//cout <<"BMP: "<< boundsMP.size()<<endl;
				//for each constraint get the bounds and add the constraint into the system
				//iff the interface profile is the same simply add the constraints otherwise 
				set<lprec *> mps;
				mps.insert(lp);lp=lpmc;cout<<get_col_name(lp,get_Ncolumns(lp))<<"help"<<endl;
				lprec *lpmp;
				for (int ifm=1; ifm<nfm+1; ifm++) {int kn=1;
					lpmp = make_lp(0, inputPlaces.size()+ outputPlaces.size()+synchrT.size());
					for(int i=1;i<=inputPlaces.size()+ outputPlaces.size()+synchrT.size();i++){
						set_int(lpmp,i,TRUE);
					}
					
					if(lpmp == NULL) {
						std::cerr<< "Unable to create new LP model"<<std::endl;
						return(1);
					}
					int k=1;
					REAL roww[1+inputPlaces.size()+ outputPlaces.size()+synchrT.size()],rowobj[1+inputPlaces.size()+ outputPlaces.size()+synchrT.size()];
					//initialize roww ith zero values
					for(std::set<std::string>::iterator cit=inputPlaces.begin();cit!=inputPlaces.end();++cit){
						char * cstr= new char [(*cit).size()+1];
						strcpy(cstr,(*cit).c_str());
						set_col_name(lpmp, k, cstr);k++;
					}
					for(std::set<std::string>::iterator cit=outputPlaces.begin();cit!=outputPlaces.end();++cit){
						char * cstr= new char [(*cit).size()+1];
						strcpy(cstr,(*cit).c_str());
						set_col_name(lpmp, k, cstr);k++;
					}
					for(std::set<std::string>::iterator cit=synchrT.begin();cit!=synchrT.end();++cit){
						char * cstr= new char [(*cit).size()+1];
						strcpy(cstr,(*cit).c_str());
						set_col_name(lpmp, k, cstr);k++;
					}
					for (std::vector<EventTerm*>::iterator it = term_vec->begin(); it != term_vec->end(); ++it) {
						//need to initialize the roww set_add_rowmode(lpmp, TRUE);
						for (k=1; k<1+inputPlaces.size()+ outputPlaces.size()+synchrT.size(); k++) {
							roww[k]=0;
						}
						//cout<<"Constraint#"<<kn<<": ";
						std::pair<int,int> bd=boundsMP.at(term_vec->size()*(ifm-1)+kn-1);
						std::map<std::string const,int>* ee=EventTerm::termToMap((*it));
						//cout << bd.first<<" "<<bd.second<<endl;
						for(std::map<std::string const,int>::iterator ite=ee->begin();ite!=ee->end();++ite){
							//cout<<ite->first<<" "<<ite->second<<std::endl;
							char * cstr= new char [ite->first.size()+1];
							strcpy(cstr,ite->first.c_str());
							roww[get_nameindex(lpmp, cstr,FALSE)]=ite->second;
						}
						int rhs=bd.first;
						if(!add_constraint(lpmp, roww,GE, rhs))
							cout<<"gata"<<endl;
						if (bd.second!=USHRT_MAX) {
							set_rh_range(lpmp,get_Nrows(lpmp), bd.second-rhs);
						}
						else {
							set_rh_range(lpmp,get_Nrows(lpmp), get_infinite(lpmp));
						}
						//set_add_rowmode(lpmp, FALSE);
						//print_lp(lpmp);
						kn++;
        				  }
//					mps.insert(lp);lp=lpmc;cout<<get_col_name(lp,get_Ncolumns(lp))<<"help"<<endl;
					//aici adauga constraints

				}
				if (args_info.inputs_num>0) { //adauga variabile 
					int nvars=get_Ncolumns(lpmp);
					for(int j=0;j<nvars;j++){
						Place & l= *net1.findPlace(get_col_name(lpmp,j+1));// cout<<l.getName()<<" ";
						if(&l==NULL) continue;// we have a sync transition the variable is already there
						REAL column[1+get_Nrows(lp)];//the set of events which is supposed to become variables in the SE
						for(int i=0;i<=get_Nrows(lp);i++){
							column[i]=0.0;	
						}
						add_column(lp,column);//then add constraint iff there is some connection between 
						set_col_name(lp, get_Ncolumns(lp),get_col_name(lpmp,j+1));
						REAL roww[1+get_Ncolumns(lp)];roww[get_Ncolumns(lp)]=1.0;
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
						add_constraint(lp,roww,EQ,0.0);
						set_row_name(lp, get_Nrows(lp),get_col_name(lpmp,j+1));
					}
					
				}//set_add_rowmode(lp,TRUE);
				int hh=1+get_Ncolumns(lp);set_verbose(lp,5);print_lp(lpmp); //cout<<"col"<<hh<<"dd";
				for(int i=0;i<get_Nrows(lpmp);i++){
					REAL rowp[1+get_Ncolumns(lpmp)], rowwpp[hh];//int *colp;
					char grex=get_row(lpmp,i+1,rowp);
					for(int j=0;j<hh;j++)rowwpp[j]=0;
				        for(int j=0;j<get_Ncolumns(lpmp);j++){rowwpp[j+hh-get_Ncolumns(lpmp)]=rowp[j+1];}
					//for(int j=0;j<hh;j++){cout<<rowwpp[j]<<" ";}
						//if( grex==-1) cout<<"gata";
					add_constraint(lp,rowwpp,get_constr_type(lpmp,i+1),get_rh(lpmp,i+1));
//					for(int j=0;j<get_Ncolumns(lpmp);j++){set_mat(lp,get_Nrows(lp),1+get_Ncolumns(lp)-(get_Ncolumns(lpmp)-j),rowp[j]);cout<<rowwpp[j]<<" ";}
//					set_rh_range(lp,get_Nrows(lpmp),get_rh_range(lpmp,i+1));
				//set_add_rowmode(lp,FALSE);
				}
				set_verbose(lp,0);
				fclose(mp_yyin);print_lp(lp);
			}
			*/
		}
		//add constraints for cover places and transitions
		if(args_info.enforceEvents_given>0){
			for(map<int,set<std::string> >::const_iterator itr=enforcedT.begin();itr!=enforcedT.end();++itr){
				cout<<"enforced "<<itr->first;
				for(set<std::string>::const_iterator it=(itr->second).begin();it!=(itr->second).end();++it){
					std::string gg;stringstream ss;ss<<itr->first<<"@"<<(*it); ss>>gg;cout<<gg;
					char * cstr= new char [gg.size()+1];
					strcpy(cstr,gg.c_str());
					cout<<*it<<it->c_str()<<"  bla "<<get_nameindex(lp,cstr,FALSE)<<endl;
					set_lowbo(lp, get_nameindex(lp,cstr,FALSE),1);
					//if(!add_constraint(lp, roww,EQ, rhs))								cout<<"gata"<<endl;
				}
			}
		}
		REAL rowobj[get_Ncolumns(lp)];
		for(int s=1;s<=get_Ncolumns(lp)+1;s++){
			rowobj[s]=0.0;
		}
		set_obj_fn(lp,rowobj);
		set_verbose(lp,IMPORTANT);
		for(int i=1;i<=get_Ncolumns(lp);++i){
			set_int(lp,i,TRUE);
		}
		print_lp(lp);
		res=solve(lp);
		//cout<<"Solve "<<solve(lp)<<endl;
		if(res==0){
			cout<<endl<<"Compatibility check inconclusive"<<endl;
			cout<<endl<<"The constraint system has at least a solution:"<<endl;
			REAL sol[nTransitions];
			get_variables(lp,sol);
			for(int s=0;s<nTransitions;s++){
				cout<<get_col_name(lp,s+1)<<" "<<sol[s]<<" "<<endl;//" "<<static_cast<int>(sol[s]) not needed anymore
			}
			cout<<endl;
			bool gata=true;
/*			pnapi::Marking m(net1);
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
	if(res!=0){std::cout<<endl<<"Not compatible wrt weak termination"<<endl;}

	

		//put tugether the message profiles i, constr,ulb and theobtained final markins
		//for all final markings
	//}

/*	if(lp == NULL) {
		std::cerr<< "Unable to create new LP model"<<std::endl;
		return(1);
	}
		
	}

//	set_add_rowmode(lp, TRUE);
//	cout<<get_Nrows(lp)<<"";
//	for(int p=0;p<=get_Nrows(lp);p++){
//	for(int k=1;k<=get_Ncolumns(lp);k++){
//	cout<<get_mat(lp,p,k)<<" ";}}
	//REAL obj[nTransitions+1];
	//for(int k=1;k<=nTransitions;k++){obj[k]=1;}
	//set_obj_fn(lp,obj);
	//FILE *out;
	//out=fopen("cc.lp","w");
	//write_LP(lp,out);
	// to compute the free term we need to know the number of representatnts of final markings
	// then we add extra constraints for 
	// according to the compatibility requirement, add the corresponding conditions
	// solve the corresponding 

//lprec *lp;
  int Ncol, *colno = NULL, j, ret = 0;
  REAL *row = NULL;

  /* We will build the model row by row
     So we start with creating a model with 0 rows and 2 columns *
  Ncol = nTransitions+nPlaces+1; /* there are two variables in the model *
  lp = make_lp(0, Ncol);
  for(int i=1;i<Ncol;i++){
	set_int(lp,i,TRUE);
  }

  if(lp == NULL) {
	std::cerr<< "Unable to create new LP model"<<std::endl;
	return(1);
  }

    /* let us name our variables. the name of each column correspond to the name of the places and transitions respectively *
	int i=1;
	for(std::set<pnapi::Place *>::iterator cit=net1.getPlaces().begin();cit!=net1.getPlaces().end();++cit){
		char * cstr= new char [(*cit)->getName().size()+1];
		strcpy(cstr,(*cit)->getName().c_str());
		set_col_name(lp, i, cstr); i++;
	}
	for(std::set<pnapi::Transition *>::iterator cit=net1.getTransitions().begin();cit!=net1.getTransitions().end();++cit){
		char * cstr= new char [(*cit)->getName().size()+1];
		strcpy(cstr,(*cit)->getName().c_str());
		set_col_name(lp, i, cstr);i++;
	}


    //set_col_name(lp, 2, "y");

    /* create space large enough for one row *


  if(lp != NULL) {
    /* clean up such that all used memory by lpsolve is freed *
    delete_lp(lp);
  }*/	
	//time(&end_time);
	//status("checked necessary condition for weak termination  in [%.0f sec]", difftime(end_time,start_time));
	std::cerr << PACKAGE << ": runtime: " << ((double(clock()) - double(start_clock)) / CLOCKS_PER_SEC) << " sec\n";
	//std::cerr << PACKAGE << ": memory consumption: "; system((string("ps | ") + TOOL_GREP + " " + PACKAGE + " | " + TOOL_AWK + " '{ if ($1 > max) max = $1 } END { print max \" KB\" }' 1>&2").c_str());

	
}
