//////////////////////////////////////////////////////////////////////////
%uview out;

StatePredicateFormula(x) -> [out: x "\n" ];

AtomicProposition(x) -> [out: x];

AllPath(x) -> [out: "A" x ];
ExPath(x)  -> [out: "E" x ];

Eventually(x) -> [out: "F (" x ")" ];
Always(x)     -> [out: "G (" x ")" ];
Until(phi,psi) -> [out: "(" phi " U " psi ")"];
Release(phi,psi) -> [out: "(" phi " R " psi ")"];

NextState(x) -> [out: "X (" x ")"];

Negation(x) -> [out: "! (" x ")"];
Conjunction(x, y) -> [out: "(" x " && " y ")"];
Disjunction(x, y) -> [out: "(" x " || " y ")"];
Implication(x, y) -> [out: "(" x " => " y ")"];
Equivalence(x, y) -> [out: "(" x " <=> " y ")"];
True() -> [out: "TRUE"];
False() -> [out: "FALSE"];

EqualsAtomicProposition(x, y) -> [out: x " = " y];
NotEqualsAtomicProposition(x, y) -> [out: x " != " y];
GreaterAtomicProposition(x, y) -> [out: x " > " y];
GreaterEqualAtomicProposition(x, y) -> [out: x " >= " y];
LessAtomicProposition(x, y) -> [out: x " < " y];
LessEqualAtomicProposition(x, y) -> [out: x " <= " y];

Sum(x,y) -> [out: x " + " y ];
Difference(x,y) -> [out: x " - " y " SNH"];
Product(x,y) -> [out: x "*" y];

ConstProduct_list(h, NiltProduct_list()) -> [out: h];
ConstProduct_list(h, l) -> [out: h "+" l];

Node(x) -> [out: "n" x ];


BuechiAutomaton(trans,accept) -> [out: trans "\n" accept];
ExpandedBuechiRule(from, formula, to) -> [out: from "(" formula ") -> " to "\n"];
BuechiRule(i,t) -> [out: "STATE" i t];
BuechiRules(a,b) -> [out: a "&" b];

EmptyTransitionRules() -> [out: ];
TransitionRule(formu,i) -> [out: formu "->" i];
TransitionRules(a,b) -> [out: a "&" b ];


EmptyBuechiRules() -> [out: ];

AcceptingState(state) -> [out:];

AcceptingSet(set1, set2) -> [out:];

EmptyAcceptingSet() -> [out:];



//////////////////////////////////////////////////////////////////////////
%uview temporal;

f=StatePredicateFormula(x) -> [temporal: x
{
    with(x) {
        ExPath(Eventually(y)) provided (not y->containsTemporal) :
            { f->type = FORMULA_REACHABLE; }
        AllPath(Always(y)) provided (not y->containsTemporal) :
            { f->type = FORMULA_INVARIANT; }
        AllPath(Always(Negation(y))) provided (not y->containsTemporal) :
            { f->type = FORMULA_IMPOSSIBLE; }
        AllPath(Always(ExPath(Eventually(y)))) provided (not y->containsTemporal) :
            { f->type = FORMULA_LIVENESS; }
        Always(Eventually(y)) provided (not y->containsTemporal) :
            { f->type = FORMULA_FAIRNESS; }
        Eventually(Always(y)) provided (not y->containsTemporal) :
            { f->type = FORMULA_STABILIZATION; }
        Eventually(y) provided (not y->containsTemporal) :
            { f->type = FORMULA_EVENTUALLY; }
        default provided (not x->containsTemporal) :
            { f->type = FORMULA_INITIAL; }
        default provided (x->containsTemporal && x->validLTLFormula) :
            { f->type = FORMULA_LTL; }
        default provided (x->containsTemporal && x->validCTLStateFormula) :
            { f->type = FORMULA_CTL; }
        default :
            { f->type = FORMULA_MODELCHECKING; }
    }
}
];

f=AtomicProposition(x) -> [temporal:     
    {
        f->containsTemporal = false;
        f->validCTLPathFormula = false;
        f->validCTLStateFormula = true;
        f->validLTLFormula = true;
    } ];
f=Conjunction(x,y)     -> [temporal: x y
    {
        f->containsTemporal = (x->containsTemporal || y->containsTemporal);
        f->validCTLStateFormula = (x->validCTLStateFormula && y->validCTLStateFormula);
        f->validCTLPathFormula = false;
        f->validLTLFormula = (x->validLTLFormula && y->validLTLFormula);
    } ];
f=Disjunction(x,y)     -> [temporal: x y
    {
        f->containsTemporal = (x->containsTemporal || y->containsTemporal);
        f->validCTLStateFormula = (x->validCTLStateFormula && y->validCTLStateFormula);
        f->validCTLPathFormula = false;
        f->validLTLFormula = (x->validLTLFormula && y->validLTLFormula);
    } ];
f=AllPath(x)           -> [temporal: x
    {
        assert(x->containsTemporal);

        f->containsTemporal = true;
        f->validCTLStateFormula = x->validCTLPathFormula;
        f->validCTLPathFormula = false;
        f->validLTLFormula = false;
    } ];
f=ExPath(x)            -> [temporal: x
    {
        assert(x->containsTemporal);
        
        f->containsTemporal = true;
        f->validCTLStateFormula = x->validCTLPathFormula;
        f->validCTLPathFormula = false;
        f->validLTLFormula = false;
    } ];
f=Always(x)            -> [temporal: x
    {
        f->containsTemporal = true;
        f->validCTLStateFormula = false;
        f->validCTLPathFormula = x->validCTLStateFormula;
        f->validLTLFormula = x->validLTLFormula;
    } ];
f=Eventually(x)        -> [temporal: x
    {
        f->containsTemporal = true;
        f->validCTLStateFormula = false;
        f->validCTLPathFormula = x->validCTLStateFormula;
        f->validLTLFormula = x->validLTLFormula;
    } ];
f=NextState(x)         -> [temporal: x
    {
        f->containsTemporal = true;
        f->validCTLStateFormula = false;
        f->validCTLPathFormula = x->validCTLStateFormula;
        f->validLTLFormula = x->validLTLFormula;
    } ];
f=Until(x,y)           -> [temporal: x y
    {
        f->containsTemporal = true;
        f->validCTLStateFormula = false;
        f->validCTLPathFormula = (x->validCTLStateFormula && y->validCTLStateFormula);
        f->validLTLFormula = (x->validLTLFormula && y->validLTLFormula);
    } ];
f=Release(x,y)         -> [temporal: x y
    {
        f->containsTemporal = true;
        f->validCTLStateFormula = false;
        f->validCTLPathFormula = (x->validCTLStateFormula && y->validCTLStateFormula);
        f->validLTLFormula = (x->validLTLFormula && y->validLTLFormula);
    } ];


//////////////////////////////////////////////////////////////////////////
%uview internal;

%{ KC_UNPARSE
#include <map>
#include <vector>
#include <stack>
#include <Core/Dimensions.h>
#include <Net/Net.h>
#include <Formula/StatePredicate.h>
#include <Formula/AtomicStatePredicate.h>
#include <Formula/ConjunctionStatePredicate.h>
#include <Formula/DisjunctionStatePredicate.h>
#include <Formula/TruePredicate.h>
#include <Formula/FalsePredicate.h>

// mapping from node ids to factors
std::map<int, int> current;

// mapping from tree node ids to list of intermediate formulas
std::map<int, std::vector<StatePredicate *> > formulas;

// invariant: top is always my parent
std::stack<size_t> id_stack;

// next free id
size_t nextId = 0;
%}

f=StatePredicateFormula(x) -> [internal:
    {
    	// clear all data-structures
    	current.clear();
    	formulas.clear();
    	while (!id_stack.empty())
    		id_stack.pop();
    	nextId = 0;
        //printf("I am a state predicate. I am %d\n", nextId);
        id_stack.push(nextId++);
    }
    x
    {
        nextId = id_stack.top();
        id_stack.pop();
        // return the only entry for the root
        assert(nextId == 0);
        assert(formulas[0].size() == 1);
        f->formula = formulas[0][0];
       	//rep->message("Produced a formula %d" , f->formula);
        //printf("I am a state predicate. Done!\n");
    }
];


Conjunction(a,b) -> [internal:
    {
        //printf("I am %d (a conjunction). My parent is %d\n", nextId, id_stack.top());
        id_stack.push(nextId++);
    }
    a
    b
    {
        // after returning from my childs, I am the top of the stack
        const index_t myId = id_stack.top();
        id_stack.pop();

        StatePredicate *f = new ConjunctionStatePredicate(formulas[myId].size());
        for (size_t i = 0; i < formulas[myId].size(); ++i)
        {
            //printf("conjunction %d: added a subformula to index %d\n", myId, i);
            assert(formulas[myId][i]);
            ((ConjunctionStatePredicate*)f)->addSub(i, formulas[myId][i]);
        }

        //printf("told my parent %d about me %d\n", id_stack.top(), myId);
        formulas[id_stack.top()].push_back(f);
    }
];

Disjunction(a,b) -> [internal:
    {
        //printf("I am %d (a disjunction). My parent is %d\n", nextId, id_stack.top());
        id_stack.push(nextId++);
    }
    a
    b
    {
        // after returning from my childs, I am the top of the stack
        const index_t  myId = id_stack.top();
        id_stack.pop();

        StatePredicate *f = new DisjunctionStatePredicate(formulas[myId].size());
        for (size_t i = 0; i < formulas[myId].size(); ++i)
        {
            //printf("disjunction %d: added a subformula to index %d\n", myId, i);
            assert(formulas[myId][i]);
            ((DisjunctionStatePredicate*)f)->addSub(i, formulas[myId][i]);
        }

        //printf("told my parent %d about me %d\n", id_stack.top(), myId);
        formulas[id_stack.top()].push_back(f);
    }
];

AtomicProposition(a) -> [internal: a ];

LessEqualAtomicProposition(x, Number(y)) -> [internal: x
{
    //printf("I am an atomic proposition. My parent is %d\n", id_stack.top());

    index_t factors_positive = 0;
    index_t factors_negative = 0;

    // count positive and negative entries
    for (std::map<int, int>::const_iterator it = current.begin(); it != current.end(); ++it)
    {
        if (it->second > 0)
        {
            factors_positive++;
        }
        else if (it->second < 0)
        {
            factors_negative++;
        }
    }

    // create formula object
    StatePredicate *f = new AtomicStatePredicate(factors_positive, factors_negative, y->value);
    index_t position_pos = 0;
    index_t position_neg = 0;

    // add positive and negative entries
    for (std::map<int, int>::const_iterator it = current.begin(); it != current.end(); ++it)
    {
        if (it->second > 0)
        {
            ((AtomicStatePredicate*)f)->addPos(position_pos++, it->first, it->second);
        }
        else if (it->second < 0)
        {
            ((AtomicStatePredicate*)f)->addNeg(position_neg++, it->first, -it->second);
        }
    }

    // reset current
    current.clear();

    // register formula at parent
    //printf("told my parent %d about me; %p\n", id_stack.top(), f);
    formulas[id_stack.top()].push_back(f);
}
];

True() -> [internal:
{
    StatePredicate *f = new TruePredicate();
    // register formula at parent
    formulas[id_stack.top()].push_back(f);    
}
];

False() -> [internal:
{
    StatePredicate *f = new FalsePredicate();
    // register formula at parent
    formulas[id_stack.top()].push_back(f);    
}
];

Fireable(x) -> [internal:
{
    // create a conjunction with #preset elements
    StatePredicate *f = new ConjunctionStatePredicate(Net::CardArcs[TR][PRE][x->value]);
    //printf("%p=ConjunctionStatePredicate(Net::CardArcs[TR][PRE][x->value]=%d)\n", f, Net::CardArcs[TR][PRE][x->value]);

    // iterate preset
    for (index_t p = 0; p < Net::CardArcs[TR][PRE][x->value]; ++p)
    {
        // create atomic state predicate with 1 positive factor and the arc weight
        StatePredicate *a = new AtomicStatePredicate(0, 1, - Net::Mult[TR][PRE][x->value][p]);
        //printf("%p=AtomicStatePredicate(0, 1, - Net::Mult[TR][PRE][x->value][p]=%d)\n", a, - Net::Mult[TR][PRE][x->value][p]);

        // add place p to index 0 with factor 1
        ((AtomicStatePredicate*)a)->addNeg(0, Net::Arc[TR][PRE][x->value][p], 1);
        //printf("%p->addNeg(0, Net::Arc[TR][PRE][x->value][p]=%d, 1)\n", a, Net::Arc[TR][PRE][x->value][p]);

        // add atomic state predicate to conjunction
        ((ConjunctionStatePredicate*)f)->addSub(p, a);
        //printf("%p->addSub(p=%d, a=%p)\n", f, p, a);
    }

    formulas[id_stack.top()].push_back(f);
}
];

Initial() -> [internal:
{
    StatePredicate *f = new ConjunctionStatePredicate(Net::Card[PL]);
    // iterate places
    for (index_t p = 0; p < Net::Card[PL]; ++p)
    {
        /* TODO */
    }

    formulas[id_stack.top()].push_back(f);
}
];


Product(x, Node(p)) -> [internal: { current[p->value] += x->value; } ];
Node(p) -> [internal: { assert(false /* sollte nicht mehr vorkommen */); current[p->value] += 1; } ];
Number(x) -> [internal: /* do not print values */ ];



%uview buechi;
%{ KC_UNPARSE
std::map<uint32_t,std::vector<std::pair<uint32_t,uint32_t> > > transitions ;
std::set<uint32_t> acceptingset;
std::map<uint32_t,StatePredicate*> predicates;
std::map<uint32_t,uint32_t> states_to_interal;

uint32_t get_state_number(uint32_t state){
	if (states_to_interal.find(state) == states_to_interal.end()){
		// get a new state number
		uint32_t new_state_number = states_to_interal.size();
		states_to_interal[state] = new_state_number;
		return new_state_number;
	}
	return states_to_interal[state];
}
/// printer-function for Kimiwtu's output on stdout
// LCOV_EXCL_START
void mylocalprinter(const char* s, kc::uview v)
{
    printf("%s", s);
}
// LCOV_EXCL_STOP

%}


b=BuechiNull -> [buechi: { rep->message("NULL BUECHI"); b->automata = 0; } ];
b=BuechiAutomaton(trans,accept) -> [buechi:
	trans
	accept
    {
    	//rep->message("REAL BUECHI");
    	
    	// get number of states
    	uint32_t number_of_states = states_to_interal.size();
		
		// construction of the buechi automata
		b->automata = new BuechiAutomata();
		b->automata->cardStates = number_of_states;

		//rep->message("transfer predicates");
    	// transfer all state predicates
    	b->automata->cardAtomicPropositions = predicates.size();
    	b->automata->atomicPropositions = new StatePredicateProperty*[predicates.size()]();
    	b->automata->atomicPropotions_backlist = new index_t[predicates.size()]();
    	for (uint32_t i = 0; i < predicates.size(); i++)
    		b->automata->atomicPropositions[i] = new StatePredicateProperty(predicates[i]);	
    	
    	
    	//rep->message("transfer transitions");
    	// build the transition-data-structure
    	b->automata->cardTransitions = new uint32_t[number_of_states]();
    	b->automata->transitions = new uint32_t**[number_of_states]();
    	b->automata->cardEnabled = new index_t[number_of_states]();
    	for (uint32_t i = 0; i < number_of_states; i++){
    		b->automata->cardTransitions[i] = transitions[i].size();
    		b->automata->transitions[i] = new uint32_t*[transitions[i].size()]();
    		for (uint32_t j = 0; j < transitions[i].size(); j++){
    			//rep->message("TRANSITION %d %d %d", i,j,transitions[i][j].second);
    			b->automata->transitions[i][j] = new uint32_t[2]();
    			b->automata->transitions[i][j][0] = transitions[i][j].first;
    			b->automata->transitions[i][j][1] = transitions[i][j].second;
    			b->automata->atomicPropotions_backlist[transitions[i][j].first] = i;
    		}
    	}
    	
    	//rep->message("transfer accepting states");
        // produce a list for the accepting states
        b->automata->isStateAccepting = new bool[number_of_states]();
        for (__typeof__(acceptingset.begin()) i = acceptingset.begin(); i != acceptingset.end(); i++)
        	b->automata->isStateAccepting[*i] = true;
    }
];


ExpandedBuechiRule(from, formula, to) -> [buechi:
    {
    
    	formula->unparse(mylocalprinter, kc::internal);
        StatePredicate* result = formula->formula;
        //formula->free(true);
        
        
        
    	//rep->message("RULE %d -> %d (%d)", from, to,formula->formula);
    	// get a number for the formula
    	uint32_t formula_number = (uint32_t)(predicates.size());
    	//assert();
    	predicates[formula_number] = result;
    	// get numbers for the from and to state
    	uint32_t from_number = get_state_number(from->value);
    	uint32_t to_number = get_state_number(to->value);
    	// insert transition
        transitions[from_number].push_back(std::make_pair(formula_number,to_number));
    } 
];

EmptyBuechiRules() -> [buechi: ];

BuechiRules(lrules, rrules) -> [buechi:
    lrules
    rrules
];

AcceptingState(state) -> [buechi:
    {
        acceptingset.insert(get_state_number(state->value));
    }
];

AcceptingSet(set1, set2) -> [buechi:
  set1
  set2
];

EmptyAcceptingSet() -> [buechi: ];

%uview ctl;
%{ KC_UNPARSE

#include <map>
#include <vector>
#include <stack>
#include <Core/Dimensions.h>
#include <Net/Net.h>
#include <Formula/StatePredicate.h>
#include <Formula/AtomicStatePredicate.h>
#include <Formula/ConjunctionStatePredicate.h>
#include <Formula/DisjunctionStatePredicate.h>
#include <Formula/TruePredicate.h>
#include <Formula/FalsePredicate.h>

#include <Exploration/StatePredicateProperty.h>

#include <Formula/CTL/CTLFormula.h>
#include <Formula/CTL/AFFormula.h>
#include <Formula/CTL/AGFormula.h>
#include <Formula/CTL/ARFormula.h>
#include <Formula/CTL/AUFormula.h>
#include <Formula/CTL/AXFormula.h>
#include <Formula/CTL/EFFormula.h>
#include <Formula/CTL/EGFormula.h>
#include <Formula/CTL/ERFormula.h>
#include <Formula/CTL/EUFormula.h>
#include <Formula/CTL/EXFormula.h>
#include <Formula/CTL/NotFormula.h>
#include <Formula/CTL/TrueFormula.h>
#include <Formula/CTL/DisjunctionFormula.h>
#include <Formula/CTL/ConjunctionFormula.h>
#include <Formula/CTL/AtomicFormula.h>

// mapping from node ids to factors
std::map<int, int> ctl_current;

// mapping from tree node ids to list of intermediate formulas
std::map<int, std::vector<CTLFormula_p> > ctl_formulas;

// invariant: top is always my parent
std::stack<size_t> ctl_id_stack;

// next free id
size_t ctl_nextId = 0;

//needs dfs-storage;
size_t ctl_dfs_id = 0;

//needs cached results id
size_t ctl_result_cache_id = 0;

//temporal storage to be able to include the LessEqualStatePredicate into an AtomicFormula
StatePredicateProperty* spp;
%}

//EINSTIEG

f=Conjunction provided (!f->containsTemporal) , 
f=Disjunction provided (!f->containsTemporal) , 
f=Negation provided (!f->containsTemporal) , 
f=AtomicProposition provided (!f->containsTemporal) -> [ctl:
    {
    	//We call the unparse process again to avoid unneccessary code replication.
    	//Furthermore this may decrease evaluation time, since now some parts of some formula can be checked without taking care of temporal operators.
        with(StatePredicateFormula(f))	//Here a little memory leak may occurs. A new object "StatePredicateFormula" is created but never deconstructed.
        								//Since kimwitu++ has memory leaks nonetheless and this one is very small, we considered it to be acceptable.
        {
            x=StatePredicateFormula(*) :
            {
                x->unparse(mylocalprinter,kc::internal); //unparse using code of different goal to reuse existing code
                AtomicFormula* af = new AtomicFormula();
                af->inner = new StatePredicateProperty(x->formula);
		        ctl_formulas[ctl_id_stack.top()].push_back(af);
		    }
		}
    }
];

f=StatePredicateFormula(x) -> [ctl:
	{
	    f->unparse(mylocalprinter, kc::temporal);	//making sure that correct information about contained temporal operators is available at every node
        											//Note that this information is _inevietable_ in order to unparse CTL-Formulas correctly!
        //printf("CONTAINS_TEMPORAL: %d\n",x->containsTemporal);
		//printf("I am a CTL state predicate. I am %lu\n", ctl_nextId);
        ctl_id_stack.push(ctl_nextId++);
    }
    x
    {
        ctl_nextId = ctl_id_stack.top();
        ctl_id_stack.pop();
        // return the only entry for the root
        assert(ctl_nextId == 0);
        f->ctl_formula = ctl_formulas[0][0];
        f->ctl_formula->payloadsize = ctl_dfs_id*sizeof(statenumber_t) + (2*ctl_result_cache_id+7)/8;
		//printf("I am a CTL state predicate. Done!\n");
    }
];

//Boolsche Operatoren
f=Conjunction(a,b) provided (f->containsTemporal) -> [ctl:
	  {
       	//printf("I am %lu (a CTL-conjunction). My parent is %lu\n", ctl_nextId, ctl_id_stack.top());
        ctl_id_stack.push(ctl_nextId++);
    }
    a
    b
    {
        // after returning from my childs, I am the top of the stack
        const index_t myId = ctl_id_stack.top();
        ctl_id_stack.pop();

        ConjunctionFormula *cf = new ConjunctionFormula();
        cf->subs = new CTLFormula*[ctl_formulas[myId].size()]();
        for (size_t i = 0; i < ctl_formulas[myId].size(); ++i)
        {
            //printf("conjunction %d: added a subformula to index %d\n", myId, i);
            assert(ctl_formulas[myId][i]);
            cf->subs[i] = ctl_formulas[myId][i];
            //((ConjunctionStatePredicate*)f)->addSub(i, ctl_formulas[myId][i]);
        }
        cf->cardSubs = ctl_formulas[myId].size();

		//printf("told my parent %lu about me %d\n", ctl_id_stack.top(), myId);
        ctl_formulas[ctl_id_stack.top()].push_back(cf);
    }
];

f=Disjunction(a,b) provided (f->containsTemporal) -> [ctl:
	{
		//printf("I am %lu (a CTL-disjunction). My parent is %lu\n", ctl_nextId, ctl_id_stack.top());
        ctl_id_stack.push(ctl_nextId++);
    }
    a
    b
    {
        // after returning from my childs, I am the top of the stack
        const index_t myId = ctl_id_stack.top();
        ctl_id_stack.pop();

        DisjunctionFormula *df = new DisjunctionFormula();
        df->subs = new CTLFormula*[ctl_formulas[myId].size()]();
        for (size_t i = 0; i < ctl_formulas[myId].size(); ++i)
        {
            //printf("disjunction %d: added a subformula to index %d\n", myId, i);
            assert(ctl_formulas[myId][i]);
            df->subs[i] = ctl_formulas[myId][i];
            //((ConjunctionStatePredicate*)f)->addSub(i, ctl_formulas[myId][i]);
        }
        df->cardSubs = ctl_formulas[myId].size();

		//printf("told my parent %lu about me %d\n", ctl_id_stack.top(), myId);
        ctl_formulas[ctl_id_stack.top()].push_back(df);
    }
];

True() -> [ctl:
	{
		ctl_formulas[ctl_id_stack.top()].push_back(new TrueFormula());
	}
];

False() -> [ctl:
	{
		NotFormula* nf = new NotFormula();
		nf->inner = new TrueFormula();
		ctl_formulas[ctl_id_stack.top()].push_back(nf);
	}
];

//CTL-Operatoren
//AG(x)
AllPath(Always(x)) -> [ctl:
	{
		//printf("CTL-AllPath_Always\n");
		ctl_id_stack.push(ctl_nextId++);
		size_t myDFSId = ctl_dfs_id++;
		size_t myCacheId = ctl_result_cache_id++;
	}
		x
	{
	    // after returning from my childs, I am the top of the stack
	    const index_t myId = ctl_id_stack.top();
	    ctl_id_stack.pop();
		
		AGFormula* agf = new AGFormula();
		assert(ctl_formulas[myId].size() == 1);
		agf->inner = ctl_formulas[myId][0];
		agf->index = 2*myCacheId;
		agf->dfsindex = myDFSId*sizeof(statenumber_t);
		
		ctl_formulas[ctl_id_stack.top()].push_back(agf);
	}
];

//AF(x)
AllPath(Eventually(x)) -> [ctl:
	{
		//printf("CTL-AllPath_Eventually\n");
        ctl_id_stack.push(ctl_nextId++);
		size_t myDFSId = ctl_dfs_id++;
		size_t myCacheId = ctl_result_cache_id++;
	}
	x
	{
		// after returning from my childs, I am the top of the stack
		const index_t myId = ctl_id_stack.top();
		ctl_id_stack.pop();

		AFFormula* aff = new AFFormula();
		assert(ctl_formulas[myId].size() == 1);
		aff->inner = ctl_formulas[myId][0];
		aff->index = 2*myCacheId;
		aff->dfsindex = myDFSId*sizeof(statenumber_t);

		ctl_formulas[ctl_id_stack.top()].push_back(aff);
	}
];

//AX(x)
AllPath(NextState(x)) -> [ctl:
	{
		//printf("CTL-AllPath_NextState\n");
        ctl_id_stack.push(ctl_nextId++);
    }
	x
	{
		// after returning from my childs, I am the top of the stack
		const index_t myId = ctl_id_stack.top();
		ctl_id_stack.pop();

		AXFormula* axf = new AXFormula();
		assert(ctl_formulas[myId].size() == 1);
		axf->inner = ctl_formulas[myId][0];
		
		ctl_formulas[ctl_id_stack.top()].push_back(axf);
	}
];

//AU(a,b)
AllPath(Until(a,b)) -> [ctl:
	{
		//printf("CTL-AllPath_Until\n");
        ctl_id_stack.push(ctl_nextId++);
		size_t myDFSId = ctl_dfs_id++;
		size_t myCacheId = ctl_result_cache_id++;
    }
	a
	b
	{
	    // after returning from my childs, I am the top of the stack
	    const index_t myId = ctl_id_stack.top();
	    ctl_id_stack.pop();
		assert(ctl_formulas[myId].size() == 2);

		AUFormula* auf = new AUFormula();
		auf->phi = ctl_formulas[myId][0];
		auf->psi = ctl_formulas[myId][1];
		auf->index = 2*myCacheId;
		auf->dfsindex = myDFSId*sizeof(statenumber_t);

		ctl_formulas[ctl_id_stack.top()].push_back(auf);
	}
];

//AR(a,b)
AllPath(Release(a,b)) -> [ctl:
	{
		//printf("CTL-AllPath_Release\n");
        ctl_id_stack.push(ctl_nextId++);
		size_t myDFSId = ctl_dfs_id++;
		size_t myCacheId = ctl_result_cache_id++;
    }
	a
	b
	{
	    // after returning from my childs, I am the top of the stack
	    const index_t myId = ctl_id_stack.top();
	    ctl_id_stack.pop();
		assert(ctl_formulas[myId].size() == 2);

		ARFormula* arf = new ARFormula();
		arf->phi = ctl_formulas[myId][0];
		arf->psi = ctl_formulas[myId][1];
		arf->index = 2*myCacheId;
		arf->dfsindex = myDFSId*sizeof(statenumber_t);

		ctl_formulas[ctl_id_stack.top()].push_back(arf);
	}
];

//EG(x)
ExPath(Always(x)) -> [ctl:
	{
		//printf("CTL-ExPath_Always\n");
        ctl_id_stack.push(ctl_nextId++);
		size_t myDFSId = ctl_dfs_id++;
		size_t myCacheId = ctl_result_cache_id++;
    }
	x
	{
		// after returning from my childs, I am the top of the stack
		const index_t myId = ctl_id_stack.top();
		ctl_id_stack.pop();

		EGFormula* egf = new EGFormula();
		assert(ctl_formulas[myId].size() == 1);
		egf->inner = ctl_formulas[myId][0];
		egf->index = 2*myCacheId;
		egf->dfsindex = myDFSId*sizeof(statenumber_t);

		ctl_formulas[ctl_id_stack.top()].push_back(egf);
	}
];

//EF(x)
ExPath(Eventually(x)) -> [ctl:
	{
		//printf("CTL-ExPath_Eventually\n");
        ctl_id_stack.push(ctl_nextId++);
		size_t myDFSId = ctl_dfs_id++;
		size_t myCacheId = ctl_result_cache_id++;
    }
	x
	{
		// after returning from my childs, I am the top of the stack
		const index_t myId = ctl_id_stack.top();
		ctl_id_stack.pop();

		EFFormula* eff = new EFFormula();
		assert(ctl_formulas[myId].size() == 1);
		eff->inner = ctl_formulas[myId][0];
		eff->index = 2*myCacheId;
		eff->dfsindex = myDFSId*sizeof(statenumber_t);

		ctl_formulas[ctl_id_stack.top()].push_back(eff);
	}
];

//EX(x)
ExPath(NextState(x)) -> [ctl:
	{
		//printf("CTL-ExPath_NextState\n");
        ctl_id_stack.push(ctl_nextId++);
    }
	x
	{
		// after returning from my childs, I am the top of the stack
		const index_t myId = ctl_id_stack.top();
		ctl_id_stack.pop();

		EXFormula* exf = new EXFormula();
		assert(ctl_formulas[myId].size() == 1);
		exf->inner = ctl_formulas[myId][0];

		ctl_formulas[ctl_id_stack.top()].push_back(exf);
	}
];

//EU(x)
ExPath(Until(a,b)) -> [ctl:
	{
		//printf("CTL-ExPath_Until\n");
        ctl_id_stack.push(ctl_nextId++);
		size_t myDFSId = ctl_dfs_id++;
		size_t myCacheId = ctl_result_cache_id++;
    }
	a
	b
	{
	    // after returning from my childs, I am the top of the stack
	    const index_t myId = ctl_id_stack.top();
	    ctl_id_stack.pop();
		assert(ctl_formulas[myId].size() == 2);

		EUFormula* euf = new EUFormula();
		euf->phi = ctl_formulas[myId][0];
		euf->psi = ctl_formulas[myId][1];
		euf->index = 2*myCacheId;
		euf->dfsindex = myDFSId*sizeof(statenumber_t);

		ctl_formulas[ctl_id_stack.top()].push_back(euf);
	}
];

//ER(x)
ExPath(Release(a,b)) -> [ctl:
	{
		//printf("CTL-ExPath_Release\n");
        ctl_id_stack.push(ctl_nextId++);
		size_t myDFSId = ctl_dfs_id++;
		size_t myCacheId = ctl_result_cache_id++;
    }
	a
	b
	{
	    // after returning from my childs, I am the top of the stack
	    const index_t myId = ctl_id_stack.top();
	    ctl_id_stack.pop();
		assert(ctl_formulas[myId].size() == 2);

		ERFormula* erf = new ERFormula();
		erf->phi = ctl_formulas[myId][0];
		erf->psi = ctl_formulas[myId][1];
		erf->index = 2*myCacheId;
		erf->dfsindex = myDFSId*sizeof(statenumber_t);

		ctl_formulas[ctl_id_stack.top()].push_back(erf);
	}
];

//All following Rules should be covered by the internal parsing with goal "internal"
//Hence, all rules assert false to make it more easy to detect errors if I was mistaken.   
Product(x, Node(p)) -> [ctl: { assert(false);} ];
Node(p) -> [ctl: { assert(false); } ];
Number(x) -> [ctl: { assert(false); } ];
AtomicProposition(a) -> [ctl: { assert(false); } ];
LessEqualAtomicProposition(x, Number(y)) -> [ctl: { assert(false); } ];
Fireable(x) -> [ctl: { assert(false); } ];
Initial() -> [ctl: { assert(false); } ];





//////////////////////////////////////////////////////////////////////////
%uview ltl;
%{ KC_UNPARSE
//#include <Formula/LTL/BuechiFromLTL.h>
#include <Frontend/Parser/ast-system-rk.h>

#include <vector>

std::vector<LTLTree_p> ltlstack;

%}


f=StatePredicateFormula(x) -> [ltl:
	x
	{
		// put top of stack into memoy
		f->ltl_tree = bin_simpler(push_negation(tl_nn(NOT,ltlstack.back(),NULL)));
		//f->ltl_tree = ltlstack.back();
	} ];

AtomicProposition(x) -> [ltl:
	{
		// parse atomic propostion with internal
		kc::tFormula internalFormula = StatePredicateFormula(AtomicProposition(x));
		internalFormula->unparse(mylocalprinter, kc::internal);
        StatePredicate* result = internalFormula->formula;
        
        // produce a new string
        int num;
        char_p sym = produce_next_string(&num);
        // I know what i am doing, the current sub-tree is an atomic propositon
        predicateMap[num] = (AtomicStatePredicate*)result;
        LTLTree_p newTree = tl_nn(PREDICATE,NULL,NULL);
        newTree->sym = tl_lookup(sym);
        newTree = bin_simpler(newTree);
        //rep->message("SYM %x %x",newTree,newTree->sym);
        assert(newTree->sym);
        ltlstack.push_back(newTree);
	}
];

Eventually(x) -> [ltl: 
	x
	{
		LTLTree_p a = ltlstack.back();
		ltlstack.pop_back();
		ltlstack.push_back(bin_simpler(tl_nn(U_OPER,tl_nn(TRUE,NULL,NULL),a)));
	}
 ];
Always(x)     -> [ltl:
	x
	{
		LTLTree_p a = ltlstack.back();
		ltlstack.pop_back();
		ltlstack.push_back(bin_simpler(tl_nn(V_OPER,tl_nn(FALSE,NULL,NULL),a)));
	}
];
Until(phi,psi) -> [ltl: 
	phi
	psi
	{
		LTLTree_p a = ltlstack.back();
		ltlstack.pop_back();
		LTLTree_p b = ltlstack.back();
		ltlstack.pop_back();
		ltlstack.push_back(bin_simpler(tl_nn(U_OPER,a,b)));
	}
];

Negation(x) -> [ltl:
	x
	{
		LTLTree_p a = ltlstack.back();
		ltlstack.pop_back();
		ltlstack.push_back(bin_simpler(tl_nn(NOT,a,NULL)));
	}
];
Conjunction(x, y) -> [ltl: 
	x
	y
	{
		LTLTree_p a = ltlstack.back();
		ltlstack.pop_back();
		LTLTree_p b = ltlstack.back();
		ltlstack.pop_back();
		ltlstack.push_back(bin_simpler(tl_nn(AND,a,b)));
	}
];
Disjunction(x, y) -> [ltl:
	x
	y
	{
		LTLTree_p a = ltlstack.back();
		ltlstack.pop_back();
		LTLTree_p b = ltlstack.back();
		ltlstack.pop_back();
		ltlstack.push_back(bin_simpler(tl_nn(OR,a,b)));
	}
];
Implication(x, y) -> [ltl:
 	x
	y
	{
		LTLTree_p a = ltlstack.back();
		ltlstack.pop_back();
		LTLTree_p b = ltlstack.back();
		ltlstack.pop_back();
		ltlstack.push_back(bin_simpler(tl_nn(IMPLIES,a,b)));
	}
];
Equivalence(x, y) -> [ltl:
	x
	y
	{
		LTLTree_p a = ltlstack.back();
		ltlstack.pop_back();
		LTLTree_p b = ltlstack.back();
		ltlstack.pop_back();
		ltlstack.push_back(bin_simpler(tl_nn(EQUIV,a,b)));
	}
];
