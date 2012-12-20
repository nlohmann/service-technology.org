//////////////////////////////////////////////////////////////////////////
%uview out;

StatePredicateFormula(x) -> [out: x "\n"];

AtomicProposition(x) -> [out: x];

AllPath(x) -> [out: "A" x ];
ExPath(x)  -> [out: "E" x ];

Eventually(x) -> [out: "F (" x ")" ];
Always(x)     -> [out: "G (" x ")" ];

Negation(x) -> [out: "! (" x ")"];
Conjunction(x, y) -> [out: "(" x " && " y ")"];
Disjunction(x, y) -> [out: "(" x " || " y ")"];
Implication(x, y) -> [out: "(" x " => " y ")"];
Equivalence(x, y) -> [out: "(" x " <=> " y ")"];

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
std::map<std::pair<uint32_t,StatePredicate*>,uint32_t>* transitions = new std::map<std::pair<uint32_t,StatePredicate*>,uint32_t>;
std::set<uint32_t>* acceptingset = new std::set<uint32_t>;
%}


b=BuechiNull -> [buechi: { b->automata = 0; } ];
b=BuechiAutomaton -> [buechi:
    {
        b->automata = new BuechiAutomata();
        b->automata->transitions = transitions;
        b->automata->acceptingset = acceptingset;
    }
];


ExpandedBuechiRule(from, formula, to) -> [buechi:
    formula
    {
        (*transitions)[std::make_pair(from->value,formula->formula)] = to->value;
    } 
];

EmptyBuechiRules() -> [buechi: ];

BuechiRules(lrules, rrules) -> [buechi:
    lrules
    rrules
];

AcceptingState(state) -> [buechi:
    {
        acceptingset->insert(state->value);
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
#include <Formula/CTL/DFSFormula.h>
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
f=StatePredicateFormula(x) -> [ctl:
	{
        printf("I am a CTL state predicate. I am %d\n", ctl_nextId);
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
        printf("I am a CTL state predicate. Done!\n");
    }
];

//Boolsche Operatoren
Conjunction(a,b) -> [ctl:
	  {
       	printf("I am %d (a CTL-conjunction). My parent is %d\n", ctl_nextId, ctl_id_stack.top());
        ctl_id_stack.push(ctl_nextId++);
    }
    a
    b
    {
        // after returning from my childs, I am the top of the stack
        const index_t myId = ctl_id_stack.top();
        ctl_id_stack.pop();

        ConjunctionFormula *cf = new ConjunctionFormula();
        CTLFormula_p subs = ((CTLFormula*) malloc(sizeof(CTLFormula)*ctl_formulas[myId].size()));
        cf->subs = &subs;
        for (size_t i = 0; i < ctl_formulas[myId].size(); ++i)
        {
            //printf("conjunction %d: added a subformula to index %d\n", myId, i);
            assert(ctl_formulas[myId][i]);
            subs[i] = *ctl_formulas[myId][i];
            //((ConjunctionStatePredicate*)f)->addSub(i, ctl_formulas[myId][i]);
        }
        cf->cardSubs = ctl_formulas[myId].size();

        printf("told my parent %d about me %d\n", ctl_id_stack.top(), myId);
        ctl_formulas[ctl_id_stack.top()].push_back(cf);
    }
];

Disjunction(a,b) -> [ctl:
	{
        printf("I am %d (a CTL-disjunction). My parent is %d\n", ctl_nextId, ctl_id_stack.top());
        ctl_id_stack.push(ctl_nextId++);
    }
    a
    b
    {
        // after returning from my childs, I am the top of the stack
        const index_t myId = ctl_id_stack.top();
        ctl_id_stack.pop();

        DisjunctionFormula *df = new DisjunctionFormula();
        CTLFormula_p subs = ((CTLFormula*) malloc(sizeof(CTLFormula)*ctl_formulas[myId].size()));
        df->subs = &subs;
        for (size_t i = 0; i < ctl_formulas[myId].size(); ++i)
        {
            //printf("disjunction %d: added a subformula to index %d\n", myId, i);
            assert(ctl_formulas[myId][i]);
            subs[i] = *ctl_formulas[myId][i];
            //((ConjunctionStatePredicate*)f)->addSub(i, ctl_formulas[myId][i]);
        }
        df->cardSubs = ctl_formulas[myId].size();

        printf("told my parent %d about me %d\n", ctl_id_stack.top(), myId);
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
		NotFormula * nf;
		nf->inner = new TrueFormula();
		ctl_formulas[ctl_id_stack.top()].push_back(nf);
	}
];

//CTL-Operatoren
//AG(x)
AllPath(Always(x)) -> [ctl:
	{
		printf("CTL-AllPath_Always\n");
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
		printf("CTL-AllPath_Eventually\n");
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
		printf("CTL-AllPath_NextState\n");
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

//AU(a,b) -- wirklich so?
AllPath(Until(a,b)) -> [ctl:
	{
		printf("CTL-AllPath_Until\n");
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

//EG(x)
ExPath(Always(x)) -> [ctl:
	{
		printf("CTL-ExPath_Always\n");
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
		printf("CTL-ExPath_Eventually\n");
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
		printf("CTL-ExPath_Next\n");
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

//EU(x) -- wirklich so?
ExPath(Until(a,b)) -> [ctl:
	{
		printf("CTL-ExPath_Until\n");
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

AtomicProposition(a) -> [ctl:
	{
		printf("CTL-AtomicProposition\n");
        ctl_id_stack.push(ctl_nextId++);
	}
	a
	{
		const index_t myId = ctl_id_stack.top();
		ctl_id_stack.pop();
		assert(ctl_formulas[myId].size() < 2);
		if(ctl_formulas[myId].size() == 0){
			AtomicFormula *af = new AtomicFormula();
			af->inner = spp;
	
			ctl_formulas[ctl_id_stack.top()].push_back(af);
		}
		else{
			ctl_formulas[ctl_id_stack.top()].push_back(ctl_formulas[myId][0]);
		}
	}
];

LessEqualAtomicProposition(x, Number(y)) -> [ctl:
{
	printf("I am an atomic (<=) CTL-proposition. My parent is %d\n", ctl_id_stack.top());

    index_t factors_positive = 0;
    index_t factors_negative = 0;

    // count positive and negative entries
    for (std::map<int, int>::const_iterator it = ctl_current.begin(); it != ctl_current.end(); ++it)
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
    for (std::map<int, int>::const_iterator it = ctl_current.begin(); it != ctl_current.end(); ++it)
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
    ctl_current.clear();

    // register formula at parent
    printf("told my parent %d about me; %p\n", ctl_id_stack.top(), f);
    spp = new StatePredicateProperty (f);
}
];



//Werden nachfolgende Konstrukte benötigt?
//Bis jetzt nur kopiert und Datenstrukturen um ctl-Präfix erweitert
Fireable(x) -> [ctl:
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

	AtomicFormula *af = new AtomicFormula();
	af->inner = (new StatePredicateProperty (f));
    ctl_formulas[ctl_id_stack.top()].push_back(af);
}
];

Initial() -> [ctl:
{
    /*StatePredicate *f = new ConjunctionStatePredicate(Net::Card[PL]);
    // iterate places
    for (index_t p = 0; p < Net::Card[PL]; ++p)
    {
        /* TODO */
    }

    ctl_formulas[ctl_id_stack.top()].push_back(f);
    */
}
];

Product(x, Node(p)) -> [ctl: { ctl_current[p->value] += x->value; } ];
Node(p) -> [ctl: { assert(false /* sollte nicht mehr vorkommen */); ctl_current[p->value] += 1; } ];
Number(x) -> [ctl: /* do not print values */ ];