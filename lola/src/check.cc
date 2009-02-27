#include "graph.H"
#include "dimensions.H"
#include "formula.H"
#include "net.H"
#include "stubborn.H"
#include "check.H"

#include <fstream>
#include <iostream>

using std::ofstream;

unsigned int NrStates = 0;
State * initial;

#define true 1
#define false 0

unsigned int * checkstart;
unsigned int formulaindex;


void printstate(char const* c, unsigned int * st)
{
  if(Sflg)
  {
    cout << "STATE"; cout << c;
    for (unsigned int i=0,j=0; i<Places[0]->cnt; i++)
    {
      if(st[i])
      {
        if(st[i] == VERYLARGE)
        {
          cout << (j++ ? ",\n" : "\n") << Places[i]->name << " : oo" ;
        }
        else
        {
          cout << (j++ ? ",\n" : "\n") << Places[i]->name << " : " << st[i];
        }
      }
    }
    cout << "\n" << endl;
    return;
  }
  
  if(sflg)
  {
    ofstream statestream(statefile);
    if (!statestream)
    { 
      fprintf(stderr, "lola: cannot open state output file '%s'\n", statefile);
      fprintf(stderr, "      no output written\n");
      return;
    }
    
    statestream << "STATE";
    for(unsigned int i=0,j=0; i<Places[0]->cnt; i++)
    {
      if(st[i])
      {
        if(st[i] == VERYLARGE)
        {
          statestream << (j++ ? ",\n" : "\n") << Places[i]->name << " : oo" ;
        }
        else
        {
          statestream << (j++ ? ",\n" : "\n") << Places[i]->name << " : " << st[i];
        }
      }
    }
    statestream << "\n" << endl;
    return;
  }
}


#ifdef MODELCHECKING
void print_witness_eu(State *, untilformula *);
void print_witness_ef(State *, unarytemporalformula *);


void print_witness_ex(State * s, unarytemporalformula * f,ofstream &out)
{
  out  << "PATH\n";
  if(s -> witnesstransition[f->tempindex])
  {
    out << s -> witnesstransition[f->tempindex]->name << "\n";
  }
  else
  {
    out << " SELF LOOP AT TERMINAL STATE\n";
  }
}


void print_witness_ef(State * s, unarytemporalformula * f, ofstream & out)
{
  out  << "PATH\n";
  while(!s->value[f->element->index])
  {
    out << s -> witnesstransition[f->tempindex]->name << "\n";
    s = s -> witness[f->tempindex];
  }

}


void print_witness_eg(State * s, unarytemporalformula * f,ofstream & out)
{
  out  << "PATH\n";
  while(s->value[f->element->index])
  {
    out << s -> witnesstransition[f->tempindex]->name << "\n";
    s->value[f->element->index] = false;
    s = s -> witness[f->tempindex];
  }
}


void print_counterexample_af(State * s, unarytemporalformula * f, ofstream & out)
{
  out  << "PATH\n";
  while(!s->value[f->element->index])
  {
    if(!(s->witness[f->tempindex])) break;
    out << s -> witnesstransition[f->tempindex]->name << "\n";
    s->value[f->element->index] = true;
    s = s -> witness[f->tempindex];
  }
}


void print_counterexample_au(State * s, untilformula * f,ofstream & out)
{
  out  << "PATH\n";
  while(s->value[f->hold->index])
  {
    out << s -> witnesstransition[f->tempindex]->name << "\n";
    s->value[f->hold->index] = true;
    s = s -> witness[f->tempindex];
  }
}


void print_witness_eu(State * s, untilformula * f,ofstream & out)
{
  out  << "PATH\n";
  while(!s->value[f->goal->index])
  {
    out << s -> witnesstransition[f->tempindex]->name << "\n";
    s = s -> witness[f->tempindex];
  }
}


void print_counterexample_ax(State * s, unarytemporalformula * f,ofstream & out)
{
  out  << "PATH\n";
  if(s -> witnesstransition[f->tempindex])
  {
    out << s -> witnesstransition[f->tempindex]->name << "\n";
  }
  else
  {
    out << "SELF LOOP AT TERMINAL STATE\n";
  }
}


void print_counterexample_ag(State * s, unarytemporalformula * f,ofstream & out)
{
  out  << "PATH\n";
  while(s->value[f->element->index])
  {
    out << s -> witnesstransition[f->tempindex]->name << "\n";
    s = s -> witness[f->tempindex];
  }
}


void print_witness_ex(State * s, unarytemporalformula * f)
{
  cout  << "PATH\n";
  if(s -> witnesstransition[f->tempindex])
  {
    cout << s -> witnesstransition[f->tempindex]->name << "\n";
  }
  else
  {
    cout << " SELF LOOP AT TERMINAL STATE\n";
  }
}


void print_witness_ef(State * s, unarytemporalformula * f)
{
  cout  << "PATH\n";
  while(!s->value[f->element->index])
  {
    cout << s -> witnesstransition[f->tempindex]->name << "\n";
    s = s -> witness[f->tempindex];
  }
}


void print_witness_eg(State * s, unarytemporalformula * f)
{
  cout  << "PATH\n";
  while(s->value[f->element->index])
  {
    
    if(!(s -> witnesstransition[f->tempindex])) break;
    cout << s -> witnesstransition[f->tempindex]->name << "\n";
    s->value[f->element->index] = false;
    s = s -> witness[f->tempindex];
  }
}


void print_counterexample_af(State * s, unarytemporalformula * f)
{
  cout  << "PATH\n";
  while(!s->value[f->element->index])
  {
    if(!(s -> witnesstransition[f->tempindex])) break;
    cout << s -> witnesstransition[f->tempindex]->name << "\n";
    s->value[f->element->index] = true;
    s = s -> witness[f->tempindex];
  }
}


void print_counterexample_au(State * s, untilformula * f)
{
  cout  << "PATH\n";
  while(s->value[f->hold->index])
  {
    if(!(s -> witnesstransition[f->tempindex])) break;
    cout << s -> witnesstransition[f->tempindex]->name << "\n";
    s->value[f->hold->index] = true;
    s = s -> witness[f->tempindex];
  }
}


void print_witness_eu(State * s, untilformula * f)
{
  cout  << "PATH\n";
  while(!s->value[f->goal->index])
  {
    cout << s -> witnesstransition[f->tempindex]->name << "\n";
    s = s -> witness[f->tempindex];
  }
}


void print_counterexample_ax(State * s, unarytemporalformula * f)
{
  cout  << "PATH\n";
  if(s -> witnesstransition[f->tempindex])
  {
    cout << s -> witnesstransition[f->tempindex]->name << "\n";
  }
  else
  {
    cout << "SELF LOOP AT TERMINAL STATE\n";
  }
}


void print_counterexample_ag(State * s, unarytemporalformula * f)
{
  cout  << "PATH\n";
  while(s->value[f->element->index])
  {
    cout << s -> witnesstransition[f->tempindex]->name << "\n";
    s = s -> witness[f->tempindex];
  }
}

void check(State * ,formula *);


void print_check_path(State * ini,formula * form)
{
  if((form->type == ef) && ini -> value[form -> index])
  {
    print_witness_ef(ini,(unarytemporalformula *) form);
    return;
  }
  if((form->type == ag) && !ini -> value[form -> index])
  {
    print_counterexample_ag(ini,(unarytemporalformula *) form);
    return;
  }
  if((form->type == eg) && ini -> value[form -> index])
  {
    print_witness_eg(ini,(unarytemporalformula *) form);
    return;
  }
  if((form->type == eu) && ini -> value[form -> index])
  {
    print_witness_eu(ini,(untilformula *) form);
    return;
  }
  if((form->type == ex) && ini -> value[form -> index])
  {
    print_witness_ex(ini,(unarytemporalformula *) form);
    return;
  }
  if((form->type == ax) && !ini -> value[form -> index])
  {
    print_counterexample_ax(ini,(unarytemporalformula *) form);
    return;
  }
  if((form->type == af) && !ini -> value[form -> index])
  {
    print_counterexample_af(ini,(unarytemporalformula *) form);
    return;
  }
  if((form->type == au) && !ini -> value[form -> index])
  {
    print_counterexample_au(ini,(untilformula *) form);
    return;
  }
  if(form->type != neg && form -> type != conj && form -> type != disj)
  {
    cout << "NO PATH\n";
    return;
  }
  if((form->type == neg))
  {
    print_check_path(ini,((unarybooleanformula *) form) -> sub);
    return;
  }
  if(form->type == conj)
  {
    unsigned int i;
    for(i=0;i< ((booleanformula *) form) -> cardsub;i++)
    {
      cout << "CONJ. " << i << ":\n";  print_check_path(ini,((booleanformula *) form) -> sub[i]);
      if(!(ini -> value[((booleanformula *) form) -> sub[i] -> index])) break;
    }
    return;
  }
  if(form->type == disj)
  {
    unsigned int i;
    for(i=0;i< ((booleanformula *) form) -> cardsub;i++)
    {
      cout << "DISJ. " << i << ":\n";  print_check_path(ini,((booleanformula *) form) -> sub[i]);
      if(ini -> value[((booleanformula *) form) -> sub[i] -> index]) break;
    }
    return;
  }
}


void print_check_path(State * ini,formula * form, ofstream & out)
{
  if((form->type == ef) && ini -> value[form -> index])
  {
    print_witness_ef(ini,(unarytemporalformula *) form,out);
    return;
  }
  if((form->type == ag) && !ini -> value[form -> index])
  {
    print_counterexample_ag(ini,(unarytemporalformula *) form,out);
    return;
  }
  if((form->type == eg) && ini -> value[form -> index])
  {
    print_witness_eg(ini,(unarytemporalformula *) form,out);
    return;
  }
  if((form->type == eu) && ini -> value[form -> index])
  {
    print_witness_eu(ini,(untilformula *) form,out);
    return;
  }
  if((form->type == ex) && ini -> value[form -> index])
  {
    print_witness_ex(ini,(unarytemporalformula *) form,out);
    return;
  }
  if((form->type == ax) && !ini -> value[form -> index])
  {
    print_counterexample_ax(ini,(unarytemporalformula *) form,out);
    return;
  }
  if((form->type == af) && !ini -> value[form -> index],out)
  {
    print_counterexample_af(ini,(unarytemporalformula *) form,out);
    return;
  }
  if((form->type == au) && !ini -> value[form -> index])
  {
    print_counterexample_au(ini,(untilformula *) form,out);
    return;
  }
  if(form->type != neg && form -> type != conj && form -> type != disj)
  {
    out << "NO PATH\n";
    return;
  }
  if((form->type == neg))
  {
    print_check_path(ini,((unarybooleanformula *) form) -> sub,out);
    return;
  }
  if(form->type == conj)
  {
    unsigned int i;
    for(i=0;i< ((booleanformula *) form) -> cardsub;i++)
    {
      out << "CONJ. " << i << ":\n";  print_check_path(ini,((booleanformula *) form) -> sub[i],out);
      if(!(ini -> value[((booleanformula *) form) -> sub[i] -> index])) break;
    }
    return;
  }
  if(form->type == disj)
  {
    unsigned int i;
    for(i=0;i< ((booleanformula *) form) -> cardsub;i++)
    {
      out << "DISJ. " << i << ":\n";  print_check_path(ini,((booleanformula *) form) -> sub[i],out);
      if(ini -> value[((booleanformula *) form) -> sub[i] -> index]) break;
    }
    return;
  }
}


int modelcheck()
{
  if(!F)
  {
    fprintf(stderr, "lola: specify formula in analysis task file\n");
    fprintf(stderr, "      mandatory for task MODELCHECKING\n");
    _exit(4);
  }
  checkstart = new unsigned int [ F -> card ];
  for(unsigned int i=0;i<F->card;i++)
  {
    checkstart[i] =0;
  }
  currentdfsnum = 1;
  for(unsigned int i=0;i<Transitions[0]->cnt;i++)
  {
#ifdef EXTENDED
    for(unsigned int j=0;j<F -> tempcard;j++)
    {
      Transitions[i]-> lstdisabled[j] = 
      Transitions[i]-> lstfired[j] = 0;
    }
#endif
    Transitions[i]->pathrestriction = new bool [F -> tempcard];
  }
  DeadStatePathRestriction = new bool [F -> tempcard];

#ifdef WITHFORMULA
  for(unsigned int i=0;i<Places[0]->cnt;i++)
  {
    Places[i] -> propositions = new formula * [Places[i] -> cardprop];
    Places[i]->cardprop = 0;
  }
#endif
  int res;
  F = F -> reduce(& res);
  if(res < 2)
  {
    return 1 - res;
  }
  // remove "not"s in static subformulae
  F = F -> posate();
  F -> tempcard = 0;
  F -> setstatic();
  cout << "\nFormula with \n" << F -> card << " subformulas\nand " << F -> tempcard << " temporal operators.\n";
  initial = binInsert();
  F -> evaluateatomic(initial);
  check(initial,F);
  cout << "\nresult: " << (initial -> value[F -> index] ? "true" : "false") << "\n";
  statistics(NrStates+1,Edges,NonEmptyHash);
  if(pflg)
  {
    ofstream pathstream(pathfile);
    if(!pathstream)
    {
       fprintf(stderr, "lola: cannot open path output file '%s'\n", pathfile);
       fprintf(stderr, "      no output written\n");
       pflg = false;
    }
    else
    {
      print_check_path(initial,F,pathstream);
    } 
  }
  if(Pflg)
  {
    print_check_path(initial,F);
  }
  return  (initial -> value[F -> index] ? 0 : 1);
}


void searchAF(State *,unarytemporalformula *);
void searchEG(State *,unarytemporalformula *);
void searchAU(State *,untilformula *);
void searchEU(State *,untilformula *);
void searchAG(State *,unarytemporalformula *);
void searchEF(State *,unarytemporalformula *);
void searchEX(State *,unarytemporalformula *);
void searchAX(State *,unarytemporalformula *);

#define MIN(X,Y) ( (X) < (Y) ? (X) : (Y)) 
#define MAX(X,Y) ( (X) > (Y) ? (X) : (Y)) 


bool check_analyse_fairness(State * pool, unsigned int level, unsigned int findex)
{
  // 1. cut the (not necessarily connected) graph in pool into sccs. 
  //    All states in search space have checktarlevel[findex] = level && ! checkexpired[findex].
  //    Before return "false", all states in pool must be "checkexpired[findex]".
  
  State * C,* N; // current, new state in dfs
  State * T; // local tarjan stack;
  State * S; // local scc
  unsigned int card = 1;
  State * O; // parent of root of local scc (must be set after analysis of scc)
  unsigned int oldd, oldm, oldc;
  
  while(pool)
  {
    // proceed as long as there are states in pool
    // choose element from pool
    C = pool;
    pool = pool -> checknexttar[findex];
    if(pool == C) pool = (State *) 0;
    T = C;
    // unlink from pool and init new dfs
    C -> checknexttar[findex] -> checkprevtar[findex] = C -> checkprevtar[findex];
    C -> checkprevtar[findex] -> checknexttar[findex] = C -> checknexttar[findex];
    C -> checknexttar[findex]  = C -> checkprevtar[findex] = C;
    C -> checkcurrent[findex] = 0;
    while(C -> checkfirelist[findex][C -> checkcurrent[findex]] && ! C -> checksucc[findex][C -> checkcurrent[findex]]) C -> checkcurrent[findex]++;
    C -> checktarlevel[findex] = level + 1;
    C -> checkparent[findex] = (State *) 0;
    C -> checkddfs[findex] = C -> checkmmin[findex] = 1;
    while(C)
    {
      if(C -> checkfirelist[findex][C -> checkcurrent[findex]])
      {
        N = C -> checksucc[findex][C->checkcurrent[findex]];
        if(N -> checktarlevel[findex] == level && ! N -> checkexpired[findex])
        {
          // new state
          N -> checkmmin[findex] = N -> checkddfs[findex] = ++card;
          N -> checkcurrent[findex] =0 ;
          while(N -> checkfirelist[findex][N -> checkcurrent[findex]] && ! N -> checksucc[findex][N -> checkcurrent[findex]]) N -> checkcurrent[findex]++;
          N -> checkparent[findex] = C;
          N -> checktarlevel[findex] = level + 1;
          // put state on local tarjan stack,
          // and remove it from pool
          if(N == pool)
          {
            pool = pool -> checknexttar[findex];
            if(pool == N)
            { 
              pool = (State *) 0; 
            }
          }
          N -> checknexttar[findex] -> checkprevtar[findex] = N -> checkprevtar[findex];
          N -> checkprevtar[findex] -> checknexttar[findex] = N -> checknexttar[findex];
          if(T)
          {
            N -> checknexttar[findex] = T -> checknexttar[findex];  
            T -> checknexttar[findex] = N;
            N -> checkprevtar[findex] = T;
            N -> checknexttar[findex] -> checkprevtar[findex] = N;
          }
          else
          {
            N -> checknexttar[findex] = N -> checkprevtar[findex] = N;
          }
          T = N;
          C = N;
        }
        else
        {
          if(N -> checktarlevel[findex] == level + 1 && ! N -> checkexpired[findex])
          {
            // existing state in same scc
            C -> checkmmin[findex] = MIN(N -> checkddfs[findex], C -> checkmmin[findex]);
            C -> checkcurrent[findex]++;
            while(C -> checkfirelist[findex][C -> checkcurrent[findex]] && ! C -> checksucc[findex][C -> checkcurrent[findex]]) C -> checkcurrent[findex]++;
          }
          else
          {
            // existing state in checkexpired[findex] scc or outside
            // current search space
            C -> checkcurrent[findex]++;  
            while(C -> checkfirelist[findex][C -> checkcurrent[findex]] && ! C -> checksucc[findex][C -> checkcurrent[findex]]) C -> checkcurrent[findex]++;
          }
        }
      }
      else
      {
        // close state
        // A) check scc
        if(C -> checkmmin[findex] == C -> checkddfs[findex])
        {
          // data of C must be preserved for proper backtracking
          // after having finished this scc
          O = C -> checkparent[findex];
          oldc = C -> checkcurrent[findex];
          oldd = C -> checkddfs[findex];
          oldm = C -> checkmmin[findex];
          // B) process scc
          // B0) unlink new component from local tarjan stack
          if(C -> checkddfs[findex] > 1) // proper cut
          {
            C -> checkprevtar[findex] -> checknexttar[findex] = T -> checknexttar[findex];
            T -> checknexttar[findex] -> checkprevtar[findex] = C -> checkprevtar[findex];
            T -> checknexttar[findex] = C;
            S = C -> checkprevtar[findex];
            C -> checkprevtar[findex] = T;
            T = S;
          }
          S = C;

          // B1) check fairness of new scc
          State * ss;
#ifdef STABLEPROP
          unsigned int cardphi;
          cardphi = 0;
#endif
          unsigned int cardS;
          unsigned int i;
          for(i=0;i<Transitions[0]->cnt;i++)
          {
            Transitions[i]-> faired = Transitions[i]->fairabled = 0;
          }
          for(cardS = 1,ss = S->checknexttar[findex];;cardS++,ss = ss ->checknexttar[findex])
          {
            for(i=0;ss->checkfirelist[findex][i];i++)
            {
              ss->checkfirelist[findex][i]->fairabled ++;
              if((ss->checksucc[findex][i]-> checktarlevel[findex] == level + 1) && !(ss->checksucc[findex][i]->checkexpired[findex]))
              { 
                ss->checkfirelist[findex][i]->faired ++;
              }
            }
#ifdef STABLEPROP
            if(ss->phi) cardphi ++;
#endif
            if(ss == S) break;
          }
          
#ifdef STABLEPROP
          if(cardphi ==  cardS)
          {
            goto aftercheck;
          }
#endif
          for(i=0;i<Transitions[0]->cnt;i++)
          {
            if(Transitions[i]->fairness > 0)
            {
              if((!Transitions[i]->faired) &&
                   Transitions[i]->fairabled == cardS)
              {
                goto aftercheck;
                // no subset can be fair
              }
            }
          }
          for(i=0;i<Transitions[0]->cnt;i++)
          {
            if(Transitions[i]->fairness == 2)
            {
              if(Transitions[i]->fairabled && ! Transitions[i]->faired)
              {
                // 1. remove all transitions
                // from S that enable t[i]
                // At this point, there must
                // be some state remaining in S, 
                // otherwise the weak fairness test
                // would have failed.
                while(Transitions[i]->fairabled)
                {
                  State * E;
                  unsigned int j;
                  E = (State *) 0;
                  for(j=0;S -> checkfirelist[findex][j];j++)
                  {
                    if(S -> checkfirelist[findex][j] == Transitions[i])
                    {
                      E = S;
                      break;
                    }
                  }
                  S = S -> checknexttar[findex];
                  if(E)
                  {
                    E -> checkexpired[findex] = true;
                    E -> checknexttar[findex] -> checkprevtar[findex] = E -> checkprevtar[findex];
                    E -> checkprevtar[findex] -> checknexttar[findex] = E -> checknexttar[findex];
                    Transitions[i]->fairabled--;
                  }
                }
                if(check_analyse_fairness(S,level + 1,findex))
                {
                  return true;
                }
                goto aftercheck;
              }
            }
            Transitions[i] -> faired = Transitions[i]-> fairabled = 0;
          }
          return true; // arrived here only if all transitions have paased fairness test.
    
aftercheck:         
          for(;!(S -> checkexpired[findex]);S = S -> checknexttar[findex])
          {
            S -> checkexpired[findex] = true;
          }
          C -> checkparent[findex] = O;
          C -> checkddfs[findex] = oldd;
          C -> checkmmin[findex] = oldm;
          C -> checkcurrent[findex] = oldc;
          
        }
        // end process scc
        // C) return to previous state
        N = C;
        C = C -> checkparent[findex];
        if(C)
        {
          C -> checksucc[findex][C->checkcurrent[findex]] = N;
          C -> checkmmin[findex] = MIN(C -> checkmmin[findex], N -> checkmmin[findex]);
          C -> checkcurrent[findex]++;
          while(C -> checkfirelist[findex][C -> checkcurrent[findex]] && ! C -> checksucc[findex][C -> checkcurrent[findex]]) C -> checkcurrent[findex]++;
        }
      } // end for all new scc
    } // end for all states
  } // end, for all nodes in pool
  return false;
} // end analyse_fairness


void check(State * s, formula * f)
{
  unsigned int i;
  formulaindex = f -> index;
  if(s->known[f->index]) return;
  switch(f->type)
  {
  case neg: check(s,((unarybooleanformula *) f)->sub);
      if(s->value[((unarybooleanformula *) f) ->sub->index])
      {
      s -> value[f->index] = false;
      }
      else
      {
      s -> value[f->index] = true;
      }
      s -> known[f -> index] = true;
      return;
  case conj:
  
     checkstart[f->index] += 1;
     checkstart[f->index] %= ((booleanformula *) f) -> cardsub;
      for(i=checkstart[f->index];;)
      {
      check(s,((booleanformula *) f) -> sub[i]);
      if(!(s-> value[((booleanformula *) f)->sub[i]->index]))
      {
      s -> value[f -> index] = false;
      s -> known[f -> index] = true;
      return;
      }
      i += 1;
      i %=((booleanformula *) f)->cardsub ;
      if(i==checkstart[f->index]) break;
      }
      s -> value[f -> index] = true;
      s -> known[f -> index] = true;
      return;
  case disj:
     checkstart[f->index] += 1;
     checkstart[f->index] %= ((booleanformula *) f) -> cardsub;
     for(i=checkstart[f->index];;)
     {
      
     check(s,((booleanformula *) f) -> sub[i]);
      if(s-> value[((booleanformula *) f)->sub[i]->index])
      {
      s -> value[f -> index] = true;
      s -> known[f -> index] = true;
      return;
      }
      i += 1;
      i %=((booleanformula *) f)->cardsub ;
      if(i==checkstart[f->index]) break;
      }
      s -> value[f->index] = false;
      s -> known[f->index] = true;
      return;
  case ef:  check(s,((unarytemporalformula *) f)->element);
      if(s->value[((unarytemporalformula *) f)->element->index])
      {
      s->value[f->index] = s->known[f->index] = true;
      s -> witness[f -> tempindex] = (State *) 0;
      s -> witnesstransition[f->tempindex] = (Transition *) 0;
      return;
      }
      searchEF(s,(unarytemporalformula *) f);
      return;
  case ag:  check(s,((unarytemporalformula *) f)->element);
      if(!s->value[((unarytemporalformula *) f)->element->index])
      {
      s->value[f->index] = false;
      s->known[f->index] = true;
      s -> witness[f -> tempindex] = (State *) 0;
      s -> witnesstransition[f->tempindex] = (Transition *) 0;
      return;
      }
      searchAG(s,(unarytemporalformula *) f);
      return;
  case eu:  check(s,((untilformula *) f) ->goal);
        if(s -> value[((untilformula *) f) ->goal ->index])
        {
        s -> value[f->index] = true;
        s -> known[f->index] = true;
        s -> witness[f -> tempindex] = (State *) 0;
        s -> witnesstransition[f->tempindex] = (Transition *) 0;
        return;
        }
        check(s,((untilformula *) f) ->hold);
        if(!s->value[((untilformula *) f) ->hold->index])
        {
          s->value[f->index] = false;
          s->known[f->index] = true;
          return;
        }
        searchEU(s,((untilformula *) f) );
        return;
  case eg:  check(s,((unarytemporalformula *) f)->element);
        if(!s -> value[((unarytemporalformula *) f)->element ->index])
        {
        s -> value[f->index] = false;
        s -> known[f->index] = true;
        return;
        }
        searchEG(s,(unarytemporalformula *) f);
        return;
  case af:  check(s,((unarytemporalformula *) f)->element);
        if(s -> value[((unarytemporalformula *) f)->element ->index])
        {
        s -> value[f->index] = true;
        s -> known[f->index] = true;
        return;
        }
        searchAF(s,(unarytemporalformula *) f);
        return;
  case au:  check(s,((untilformula *) f) ->goal);
        if(s -> value[((untilformula *) f) ->goal ->index])
        {
        s -> value[f->index] = true;
        s -> known[f->index] = true;
        return;
        }
        check(s,((untilformula *) f) ->hold);
        if(!s->value[((untilformula *) f) ->hold->index])
        {
          s->value[f->index] = false;
          s->known[f->index] = true;
          s -> witness[f -> tempindex] = (State *) 0;
          s -> witnesstransition[f->tempindex] = (Transition *) 0;
          return;
        }
        searchAU(s,((untilformula *) f) );
        return;
  case ex:  searchEX(s,((unarytemporalformula *) f)); return;
  case ax:  searchAX(s,((unarytemporalformula *) f)); return;
  default: cout << "temporal operator not supported\n";
  }
}


void searchEF(State * s,unarytemporalformula * f)
{
  State * tarjanroot;
  bool found;
  unsigned int i;
  unsigned int cardstates;
  State * CurrentState, * NewState;

#ifdef EXTENDEDCTL
  TemporalIndex = f -> tempindex;
#endif
  found = false;
  cardstates = 1;
  CurrentState = s;
  tarjanroot = s;
  // End of Stack = Self reference! Not on stack -> next = nil
  CurrentState -> checknexttar[f->tempindex] = CurrentState;
  CurrentState -> checkcurrent[f->tempindex] = 0;
  CurrentState -> checkfirelist[f->tempindex] = 
#ifdef STUBBORN
stubbornfirelist(s,f->element);
#else
firelist();
#endif
  CurrentState -> checkparent[f->tempindex] = (State *) 0;
  CurrentState -> checkdfs[f->tempindex] = CurrentState -> checkmin[f->tempindex] = 0;
  
  // process marking until returning from initial state
  
  while(CurrentState)
    {
  CurrentState -> value[f->index] = false;
        CurrentState -> known[f->index] = true;
      if(CurrentState -> checkfirelist[f->tempindex][CurrentState -> checkcurrent[f->tempindex]])
  {
    // there is a next state that needs to be explored
    CurrentState -> checkfirelist[f->tempindex][CurrentState -> checkcurrent[f->tempindex]] -> fire();
    if(NewState = binSearch())
      {
    Edges ++;
     if(!(Edges % REPORTFREQUENCY))
             cout << "st: " << NrStates << "     edg: " << Edges << "\n";
        if(NewState -> known[f->index])
        {
    if(NewState -> value[f->index])
                {
      // Value of new state known, true
      //Whitness for EF phi found!
                  found = true;
                  CurrentState -> value[f->index] = CurrentState -> known[f->index] = true;
      CurrentState -> witness[f->tempindex] = NewState;
      CurrentState -> witnesstransition[f->tempindex] = CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]];
      CurrentState -> checkfirelist[f->tempindex][CurrentState -> checkcurrent[f->tempindex]] -> backfire();
      break;
                }
          else
                {
          // Value of new state known, false
                  // NewState either on Stack or does not model EF phi
      if(NewState->checknexttar[f->tempindex])
        {
      // on Stack: set min, update witness
              if(CurrentState -> checkmin[f->tempindex] > NewState -> checkdfs[f->tempindex])
      { 
        CurrentState -> checkmin[f->tempindex] = NewState -> checkdfs[f->tempindex];
        CurrentState -> witness[f->tempindex] = NewState;
        CurrentState -> witnesstransition[f->tempindex] = CurrentState -> checkfirelist[f->tempindex][CurrentState -> checkcurrent[f->tempindex]];
      }
      }
      CurrentState -> checkfirelist[f->tempindex][CurrentState -> checkcurrent[f->tempindex]] -> backfire();
      CurrentState -> checkcurrent[f->tempindex]++;
    }
              }
        else
        {
    // value of new state unknown
    check(NewState,f->element);
    if(NewState->value[f->element->index])
    {
                        NewState -> value[f->index] = true;
                        NewState -> known[f->index] = true;
                        CurrentState -> value[f->index] = true;
                        CurrentState -> known[f->index] = true;
      found = true;
      CurrentState -> checkfirelist[f->tempindex][CurrentState -> checkcurrent[f->tempindex]] -> backfire();
            CurrentState -> witness[f->tempindex] = NewState;
            CurrentState -> witnesstransition[f->tempindex] = CurrentState->checkfirelist[f->tempindex][CurrentState ->checkcurrent[f->tempindex]];
      break;
    }
    else
    {
                        CurrentState -> value[f->index] = false;
                        CurrentState -> known[f->index] = true;
      NewState -> checkparent[f->tempindex] = CurrentState;
#ifdef EXTENDEDCTL
      TemporalIndex = f -> tempindex;
#endif
      NewState -> checkfirelist[f -> tempindex] = 
#ifdef STUBBORN
stubbornfirelist(NewState,f->element);
#else
firelist();
#endif
      NewState -> checkdfs[f->tempindex] = NewState -> checkmin[f->tempindex] = cardstates++;
      NewState -> checkcurrent[f -> tempindex] = 0;
      NewState -> checknexttar[f->tempindex] = tarjanroot;
      tarjanroot = NewState;
      CurrentState = NewState;
    }
        }
      }
    else
      {
        NewState = binInsert();
      NewState -> checknexttar[f->tempindex] = tarjanroot;
      tarjanroot = NewState;
      NrStates ++;

#ifdef MAXIMALSTATES      
          checkMaximalStates(NrStates); ///// LINE ADDED BY NIELS
#endif
      
      Edges ++;
       if(!(Edges % REPORTFREQUENCY))
             cout << "st: " << NrStates << "     edg: " << Edges << "\n";
        NewState -> checkcurrent[f->tempindex] = 0;
        NewState -> checkparent[f->tempindex] = CurrentState;
        NewState -> checkdfs[f->tempindex] = NewState -> checkmin[f->tempindex] = cardstates++;
        check(NewState,f->element);
        if(NewState->value[f->element->index])
        {
    NewState -> value[f->index] = true;
    NewState -> known[f->index] = true;
    CurrentState -> value[f->index] = true;
    CurrentState -> known[f->index] = true;
    found = true;
    CurrentState -> witness[f->tempindex] = NewState;
    CurrentState -> witnesstransition[f->tempindex] = CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]];
      CurrentState -> checkfirelist[f->tempindex][CurrentState -> checkcurrent[f->tempindex]] -> backfire();
    break;
        }
        else
        {
#ifdef EXTENDEDCTL
      TemporalIndex = f -> tempindex;
#endif
        NewState -> checkfirelist[f->tempindex] = 
#ifdef STUBBORN
stubbornfirelist(NewState,f->element);
#else
firelist();
#endif
    NewState -> checkcurrent[f->tempindex] = 0;
    CurrentState -> value[f->index] = false;
    CurrentState -> known[f->index] = true;
    CurrentState = NewState;
        }
      }
  }
      else
  {
    // test scc, update checknexttar, return to previous state
    if(CurrentState->checkmin[f->tempindex] == CurrentState-> checkdfs[f->tempindex])
          {
    // scc closed 
    CurrentState -> value[f->index] = false;
    CurrentState -> known[f->index] = true;
    while((tarjanroot->checkdfs[f->tempindex] >= CurrentState -> checkdfs[f->tempindex]))
    {
      State * tmp;
      tmp = tarjanroot;
      tarjanroot -> witness[f->tempindex] = (State *) 0;
      if(tarjanroot == tarjanroot ->checknexttar[f->tempindex])
      { 
        tarjanroot -> checknexttar[f->tempindex] = (State *) 0;
        break;
      }
      tarjanroot = tarjanroot -> checknexttar[f -> tempindex];
      tmp -> checknexttar[f -> tempindex] = (State *) 0;
    }
    }
    // return to previous state
    if(CurrentState->checkparent[f -> tempindex])
      {
        if(CurrentState -> checkparent[f->tempindex]->checkmin[f->tempindex] > CurrentState -> checkmin[f->tempindex])
        {
    CurrentState -> checkparent[f->tempindex]->checkmin[f->tempindex] = CurrentState -> checkmin[f->tempindex];
    CurrentState -> checkparent[f->tempindex] -> witness[f->tempindex] = CurrentState;
     CurrentState -> checkparent[f->tempindex] -> witnesstransition[f->tempindex] =
      CurrentState -> checkparent[f->tempindex] -> checkfirelist[f->tempindex][ CurrentState -> checkparent[f->tempindex] -> checkcurrent[f->tempindex]];
              }
        CurrentState = CurrentState -> checkparent[f->tempindex];
        CurrentState -> checkfirelist[f->tempindex][CurrentState -> checkcurrent[f->tempindex]] -> backfire();
        CurrentState -> checkcurrent[f -> tempindex] ++;
      }
    else
      {
    CurrentState = (State *) 0;
            }
  }
    }
    if(found)
    {
  while(CurrentState -> checkparent[f -> tempindex])
  {
    CurrentState -> checkparent[f -> tempindex] -> witness[f -> tempindex] = CurrentState;
    CurrentState -> checkparent[f -> tempindex] -> witnesstransition[f -> tempindex]
      = CurrentState -> checkparent[f -> tempindex]-> checkfirelist[f -> tempindex][CurrentState -> checkparent[f -> tempindex] -> checkcurrent[f -> tempindex]];
    
    CurrentState = CurrentState -> checkparent[f -> tempindex];
    CurrentState -> value[f-> index] = true;
      CurrentState -> checkfirelist[f->tempindex][CurrentState -> checkcurrent[f->tempindex]] -> backfire();
  }
  while(1)
  {
    State * tmp;

    tmp = tarjanroot;
    tarjanroot -> value[f->index] = true;
    if(tarjanroot == tarjanroot -> checknexttar[f->tempindex])
    { 
      tarjanroot -> checknexttar[f->tempindex] = (State *) 0;
      break;
    }
    tarjanroot = tarjanroot -> checknexttar[f->tempindex];
    tmp -> checknexttar[f->tempindex] = (State *) 0;
        }
     }
}


void searchAX(State * s,unarytemporalformula * f)
{
  State * NewState;

#ifdef EXTENDEDCTL
  TemporalIndex = f -> tempindex;
#endif
    s -> checkfirelist[f->tempindex] = firelist();
    s -> value[f->index] = true;
    s -> known[f->index] = true;

    if(!(s -> checkfirelist[f->tempindex][0]))
  {
    if(Transitions[0]->NrEnabled)
    {
      // no tau-successors, but state not dead
      return;
    }
    // dead state: this state counts as successor state
    if(DeadStatePathRestriction[f -> tempindex])
    {
    check(s,f -> element);
    if(!(s->value[f->element->index]))
    {
      s -> value[f->index] = false;
      s -> witness[f->tempindex] = (State *) 0;
      s -> witnesstransition[f->tempindex] = (Transition *) 0;
    }
    }
    return;
  }
    s -> checkcurrent[f->tempindex] = 0;
  do
  {
    // there is a next state that needs to be explored
    s -> checkfirelist[f->tempindex][s -> checkcurrent[f->tempindex]] -> fire();
    if(!(NewState = binSearch()))
    {
      NewState = binInsert();
      NrStates ++;
#ifdef MAXIMALSTATES      
        checkMaximalStates(NrStates); ///// LINE ADDED BY NIELS
#endif
    }
    Edges ++;
    if(!(Edges % REPORTFREQUENCY))
         cout << "st: " << NrStates << "     edg: " << Edges << "\n";
    check(NewState,f->element);
    if(!(NewState->value[f->element->index]))
    {
      s -> value[f->index] = false;
      s -> witness[f->tempindex] = NewState;
      s -> witnesstransition[f->tempindex] = s->checkfirelist[f->tempindex][s->checkcurrent[f->tempindex]];
      s -> checkfirelist[f->tempindex][s -> checkcurrent[f->tempindex]] -> backfire();
      return;
    }
    // return to previous state
    s -> checkfirelist[f->tempindex][s -> checkcurrent[f -> tempindex]] -> backfire();
    s -> checkcurrent[f -> tempindex] ++;
  }
    while(s -> checkfirelist[f->tempindex][s -> checkcurrent[f->tempindex]]);
}


void searchEX(State * s,unarytemporalformula * f)
{
  State * NewState;

#ifdef EXTENDEDCTL
  TemporalIndex = f -> tempindex;
#endif
    s -> checkfirelist[f->tempindex] = firelist();
    s -> value[f->index] = false;
    s -> known[f->index] = true;

    if(!(s -> checkfirelist[f->tempindex][0]))
  {
    if(Transitions[0]-> NrEnabled)
    {
      // no tau-successors, but state not dead
      return;
    }
    // dead state: this state counts as successor state
    if(DeadStatePathRestriction[f -> tempindex])
    {
    check(s,f -> element);
    if(s->value[f->element->index])
    {
      s -> value[f->index] = true;
      s -> witness[f->tempindex] = (State *) 0;
      s -> witnesstransition[f->tempindex] = (Transition *) 0;
    }
    }
    return;
  }
    s -> checkcurrent[f->tempindex] = 0;
  do
  {
    // there is a next state that needs to be explored
    s -> checkfirelist[f->tempindex][s -> checkcurrent[f->tempindex]] -> fire();
    if(!(NewState = binSearch()))
    {
      NewState = binInsert();
      NrStates ++;
#ifdef MAXIMALSTATES      
        checkMaximalStates(NrStates); ///// LINE ADDED BY NIELS
#endif
    }
    Edges ++;
    if(!(Edges % REPORTFREQUENCY))
         cout << "st: " << NrStates << "     edg: " << Edges << "\n";
    check(NewState,f->element);
    if(NewState->value[f->element->index])
    {
      s -> value[f->index] = true;
      s -> witness[f->tempindex] = NewState;
      s -> witnesstransition[f->tempindex] = s->checkfirelist[f->tempindex][s->checkcurrent[f->tempindex]];
      s -> checkfirelist[f->tempindex][s -> checkcurrent[f->tempindex]] -> backfire();
      return;
    }
    // return to previous state
    s -> checkfirelist[f->tempindex][s -> checkcurrent[f -> tempindex]] -> backfire();
    s -> checkcurrent[f -> tempindex] ++;
  }
    while(s -> checkfirelist[f->tempindex][s -> checkcurrent[f->tempindex]]);
}


void searchAG(State * s,unarytemporalformula * f)
{
  State * tarjanroot;
  bool found;
  unsigned int i;
  unsigned int cardstates;
  State * CurrentState, * NewState;

  found = false;
  cardstates = 1;
  CurrentState = s;
  tarjanroot = s;
  // End of Stack = Self reference! Not on stack -> next = nil
  CurrentState -> checknexttar[f->tempindex] = CurrentState;
  CurrentState -> checkcurrent[f->tempindex] = 0;
#ifdef EXTENDEDCTL
  TemporalIndex = f -> tempindex;
#endif
  CurrentState -> checkfirelist[f->tempindex] = 
#ifdef STUBBORN
  stubbornfirelistneg(s,f -> element);
#else
  firelist();
#endif
  CurrentState -> checkparent[f->tempindex] = (State *) 0;
  CurrentState -> checkdfs[f->tempindex] = CurrentState -> checkmin[f->tempindex] = 0;
  
  // process marking until returning from initial state
  
  while(CurrentState)
    {
  CurrentState -> value[f->index] = true;
        CurrentState -> known[f->index] = true;
      if(CurrentState -> checkfirelist[f->tempindex][CurrentState -> checkcurrent[f->tempindex]])
  {
    // there is a next state that needs to be explored
    CurrentState -> checkfirelist[f->tempindex][CurrentState -> checkcurrent[f->tempindex]] -> fire();
    if(NewState = binSearch())
      {
    Edges ++;
     if(!(Edges % REPORTFREQUENCY))
             cout << "st: " << NrStates << "     edg: " << Edges << "\n";
        if(NewState -> known[f->index])
        {
    if(!(NewState -> value[f->index]))
                {
      // Value of new state known, true
      //Counterexample for AG phi found!
                  found = true;
                  CurrentState -> value[f->index] = false;
      CurrentState -> witness[f->tempindex] = NewState;
      CurrentState -> witnesstransition[f->tempindex] = CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]];
      CurrentState -> checkfirelist[f->tempindex][CurrentState -> checkcurrent[f->tempindex]] -> backfire();
      break;
                }
          else
                {
          // Value of new state known, false
                  // NewState either on Stack or does not model EF phi
      if(NewState->checknexttar[f->tempindex])
        {
      // on Stack: set min, update witness
              if(CurrentState -> checkmin[f->tempindex] > NewState -> checkdfs[f->tempindex])
      { 
        CurrentState -> checkmin[f->tempindex] = NewState -> checkdfs[f->tempindex];
        CurrentState -> witness[f->tempindex] = NewState;
        CurrentState -> witnesstransition[f->tempindex] = CurrentState -> checkfirelist[f->tempindex][CurrentState -> checkcurrent[f->tempindex]];
      }
      }
      CurrentState -> checkfirelist[f->tempindex][CurrentState -> checkcurrent[f->tempindex]] -> backfire();
      CurrentState -> checkcurrent[f->tempindex]++;
    }
              }
        else
        {
    // value of new state unknown
    check(NewState,f->element);
    if(!(NewState->value[f->element->index]))
    {
                        NewState -> value[f->index] = false;
                        NewState -> known[f->index] = true;
                        CurrentState -> value[f->index] = false;
                        CurrentState -> known[f->index] = true;
      found = true;
      CurrentState -> checkfirelist[f->tempindex][CurrentState -> checkcurrent[f->tempindex]] -> backfire();
            CurrentState -> witness[f->tempindex] = NewState;
            CurrentState -> witnesstransition[f->tempindex] = CurrentState->checkfirelist[f->tempindex][CurrentState ->checkcurrent[f->tempindex]];
      break;
    }
    else
    {
                        CurrentState -> value[f->index] = true;
                        CurrentState -> known[f->index] = true;
      NewState -> checkparent[f->tempindex] = CurrentState;
#ifdef EXTENDEDCTL
      TemporalIndex = f -> tempindex;
#endif
      NewState -> checkfirelist[f -> tempindex] = 
#ifdef STUBBORN
stubbornfirelistneg(NewState,f->element);
#else
firelist();
#endif
      NewState -> checkdfs[f->tempindex] = NewState -> checkmin[f->tempindex] = cardstates++;
      NewState -> checkcurrent[f -> tempindex] = 0;
      NewState -> checknexttar[f->tempindex] = tarjanroot;
      tarjanroot = NewState;
      CurrentState = NewState;
    }
        }
      }
    else
      {
        NewState = binInsert();
      NewState -> checknexttar[f->tempindex] = tarjanroot;
      tarjanroot = NewState;
      NrStates ++;
#ifdef MAXIMALSTATES      
          checkMaximalStates(NrStates); ///// LINE ADDED BY NIELS
#endif
      Edges ++;
       if(!(Edges % REPORTFREQUENCY))
             cout << "st: " << NrStates << "     edg: " << Edges << "\n";
        NewState -> checkcurrent[f->tempindex] = 0;
        NewState -> checkparent[f->tempindex] = CurrentState;
        NewState -> checkdfs[f->tempindex] = NewState -> checkmin[f->tempindex] = cardstates++;
        check(NewState,f->element);
        if(!(NewState->value[f->element->index]))
        {
    NewState -> value[f->index] = false;
    NewState -> known[f->index] = true;
    CurrentState -> value[f->index] = false;
    CurrentState -> known[f->index] = true;
    found = true;
    CurrentState -> witness[f->tempindex] = NewState;
    CurrentState -> witnesstransition[f->tempindex] = CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]];
      CurrentState -> checkfirelist[f->tempindex][CurrentState -> checkcurrent[f->tempindex]] -> backfire();
    break;
        }
        else
        {
#ifdef EXTENDEDCTL
      TemporalIndex = f -> tempindex;
#endif
        NewState -> checkfirelist[f->tempindex] = 
#ifdef STUBBORN
stubbornfirelistneg(NewState,f->element);
#else
firelist();
#endif
    NewState ->checkcurrent[f->tempindex] = 0;
    CurrentState -> value[f->index] = true;
    CurrentState -> known[f->index] = true;
    CurrentState = NewState;
        }
      }
  }
      else
  {
    // test scc, update checknexttar, return to previous state
    if(CurrentState->checkmin[f->tempindex] == CurrentState-> checkdfs[f->tempindex])
          {
    // scc closed 
    CurrentState -> value[f->index] = true;
    CurrentState -> known[f->index] = true;
    while((tarjanroot->checkdfs[f->tempindex] >= CurrentState -> checkdfs[f->tempindex]))
    {
      State * tmp;
      tmp = tarjanroot;
      tarjanroot -> witness[f->tempindex] = (State *) 0;
      if(tarjanroot == tarjanroot -> checknexttar[f->tempindex])
      {
        tarjanroot -> checknexttar[f->tempindex] = (State *) 0;
        break;
      }
      tarjanroot = tarjanroot -> checknexttar[f -> tempindex];
      tmp -> checknexttar[f -> tempindex] = (State *) 0;
    }
    }
    // return to previous state
    if(CurrentState->checkparent[f -> tempindex])
      {
        if(CurrentState -> checkparent[f->tempindex]->checkmin[f->tempindex] > CurrentState -> checkmin[f->tempindex])
        {
    CurrentState -> checkparent[f->tempindex]->checkmin[f->tempindex] = CurrentState -> checkmin[f->tempindex];
    CurrentState -> checkparent[f->tempindex] -> witness[f->tempindex] = CurrentState;
     CurrentState -> checkparent[f->tempindex] -> witnesstransition[f->tempindex] =
      CurrentState -> checkparent[f->tempindex] -> checkfirelist[f->tempindex][ CurrentState -> checkparent[f->tempindex] -> checkcurrent[f->tempindex]];
              }
        CurrentState = CurrentState -> checkparent[f->tempindex];
        CurrentState -> checkfirelist[f->tempindex][CurrentState -> checkcurrent[f->tempindex]] -> backfire();
        CurrentState -> checkcurrent[f -> tempindex] ++;
      }
    else
      {
    CurrentState = (State *) 0;
            }
  }
    }
    if(found)
    {
  while(CurrentState -> checkparent[f -> tempindex])
  {
    CurrentState -> checkparent[f -> tempindex] -> witness[f -> tempindex] = CurrentState;
    CurrentState -> checkparent[f -> tempindex] -> witnesstransition[f -> tempindex]
      = CurrentState -> checkparent[f -> tempindex]-> checkfirelist[f -> tempindex][CurrentState -> checkparent[f -> tempindex] -> checkcurrent[f -> tempindex]];
    
    CurrentState = CurrentState -> checkparent[f -> tempindex];
    CurrentState -> value[f-> index] = false;
      CurrentState -> checkfirelist[f->tempindex][CurrentState -> checkcurrent[f->tempindex]] -> backfire();
  }
  while(1)
  {
    State * tmp;

    tmp = tarjanroot;
    tarjanroot -> value[f->index] = false;
    if(tarjanroot == tarjanroot -> checknexttar[f->tempindex])
    { 
      tarjanroot -> checknexttar[f->tempindex] = (State *) 0;
      break;
    }
    tarjanroot = tarjanroot -> checknexttar[f->tempindex];
    tmp -> checknexttar[f->tempindex] = (State *) 0;
        }
     }
}
    
extern unsigned int currentdfsnum;


void searchEU(State * s, untilformula * f)
{
  unsigned int MinBookmark;
  State * tarjanroot;
  unsigned int i;
  State * CurrentState, * NewState;
  bool found;

  MinBookmark = 0;
  found = false;
  CurrentState = s;
  s -> checknexttar[f->tempindex] = s;
  tarjanroot = s; // End of Stack is recognised as self-loop
          // Thus, states not on stack can be detected through
          // nil-pointer in checknexttar
#ifdef EXTENDEDCTL
  TemporalIndex = f -> tempindex;
#endif
  s -> checkfirelist[f->tempindex] =
#ifdef STUBBORN
  stubbornfirelistctl();
#else
  firelist();
#endif
  s -> checkparent[f->tempindex] = (State *) 0;
  s -> checkdfs[f -> tempindex] = s -> checkmin[f->tempindex] = currentdfsnum;
  currentdfsnum++;
  s->checkcurrent[f->tempindex] = 0;
  
  while(CurrentState)
  {
    CurrentState -> value[f->index] = false;
    CurrentState -> known[f->index] = true;

    if(CurrentState -> checkfirelist[f->tempindex][CurrentState ->
                checkcurrent[f->tempindex]])
    {
      // explore next state
      CurrentState -> checkfirelist[f->tempindex][CurrentState ->
                checkcurrent[f->tempindex]] -> fire();
      Edges ++;
      if(!(Edges % REPORTFREQUENCY))
        cout << "st: " << NrStates << "    edg: " << Edges
        << "\n";
      if(NewState = binSearch())
      {
        // state exists
        if(NewState -> known[f->index])
        {
          // state exists, value known
          if(NewState -> value[f->index])
          {
            // state exists, value true --> witness found!
            found = true;
            CurrentState -> witness[f->tempindex] = NewState;
            CurrentState -> witnesstransition[f->tempindex]
             = CurrentState -> checkfirelist[f->tempindex]
             [CurrentState -> checkcurrent[f->tempindex]];
             CurrentState -> checkfirelist[f->tempindex]
             [CurrentState -> checkcurrent[f->tempindex]]
             ->backfire();
            break;
          }
          else
          {
            // state exists, value false -> on stack or
            // does not model phi
            if(NewState->checknexttar[f->tempindex])
            {
              // state exists, on stack -> set min,
              // update witness
              if(CurrentState -> checkmin[f->tempindex]
                 > NewState -> checkdfs[f->tempindex])
              {
                CurrentState -> checkmin[f->tempindex] =
                NewState -> checkdfs[f->tempindex];
                CurrentState -> witness[f->tempindex]
                = NewState;
                CurrentState -> witnesstransition[f->tempindex]
                = CurrentState -> checkfirelist[f->tempindex]
                  [CurrentState -> checkcurrent[f->tempindex]];
              }
            }
            else
            {
              // new state belongs to other scc ->
              // current state does not belong to tscc
              MinBookmark = CurrentState -> checkdfs[f->tempindex];
            }
            CurrentState -> checkfirelist[f->tempindex]
            [CurrentState -> checkcurrent[f->tempindex]]
            -> backfire();
            CurrentState -> checkcurrent[f->tempindex]++;
          }
        }
        else
        {
          // state exists, value unknown
          check(NewState,f->goal);
          if(NewState->value[f->goal->index])
          {
            // right subformula true -> witness found!
            found = true;
            CurrentState -> witness[f->tempindex] = NewState;
            CurrentState -> witnesstransition[f->tempindex]
            =  CurrentState -> checkfirelist[f->tempindex]
               [CurrentState -> checkcurrent[f->tempindex]];
            CurrentState -> checkfirelist[f->tempindex]
             [CurrentState -> checkcurrent[f->tempindex]]
             ->backfire();
            break;
          }
          else
          {
            // state exists, right subformula false
            check(NewState,f->hold);
            if(NewState->value[f->hold->index])
            {
              // state exists, left true, right false
              // -> continue search
              NewState -> checkparent[f->tempindex] 
              = CurrentState;
#ifdef EXTENDEDCTL
              TemporalIndex = f -> tempindex;
#endif
              NewState -> checkfirelist[f->tempindex] =
#ifdef STUBBORN
                stubbornfirelistctl();
#else
                firelist();
#endif
              NewState -> checkdfs[f->tempindex]
              = NewState -> checkmin[f->tempindex]
              = currentdfsnum;
              currentdfsnum++;
              NewState -> checkcurrent[f->tempindex] = 0;
              NewState -> checknexttar[f->tempindex] =
              tarjanroot;
              tarjanroot = NewState;
              CurrentState = NewState;
            }
            else
            {
              // state exists, left false, right false
              // -> backtracking
              CurrentState -> checkfirelist[f->tempindex]
              [CurrentState -> checkcurrent[f->tempindex]]
              -> backfire();
              CurrentState -> checkcurrent[f->tempindex]++;
            }
          }
        }
      }
      else
      {
        // state is new
        NewState = binInsert();
        NrStates ++;
#ifdef MAXIMALSTATES      
                checkMaximalStates(NrStates); ///// LINE ADDED BY NIELS
#endif
        NewState -> checkcurrent[f->tempindex] = 0;
        NewState -> checkdfs[f->tempindex] 
        = NewState -> checkmin[f->tempindex] = currentdfsnum;
        currentdfsnum ++;
        check(NewState,f->goal);
        if(NewState->value[f->goal->index])
        {
          // right subformula true -> witness found!
          found = true;
          CurrentState -> witness[f->tempindex] = NewState;
          CurrentState -> witnesstransition[f->tempindex]
          =  CurrentState -> checkfirelist[f->tempindex]
             [CurrentState -> checkcurrent[f->tempindex]];
          CurrentState -> checkfirelist[f->tempindex]
           [CurrentState -> checkcurrent[f->tempindex]]
           ->backfire();
          break;
        }
        else
        {
          // right subformula false
          check(NewState,f->hold);
          if(NewState->value[f->hold->index])
          {
            // left true, right false
            // -> continue search
            NewState -> checkparent[f->tempindex] 
            = CurrentState;
#ifdef EXTENDEDCTL
            TemporalIndex = f -> tempindex;
#endif
            NewState -> checkfirelist[f->tempindex] =
#ifdef STUBBORN
              stubbornfirelistctl();
#else
              firelist();
#endif
            NewState -> checkcurrent[f->tempindex] = 0;
            NewState -> checknexttar[f->tempindex] =
            tarjanroot;
            tarjanroot = NewState;
            CurrentState = NewState;
          }
          else
          {
            // left false, right false
            // -> backtracking
            CurrentState -> checkfirelist[f->tempindex]
            [CurrentState -> checkcurrent[f->tempindex]]
            -> backfire();
            CurrentState -> checkcurrent[f->tempindex]++;
          }
        }
      }
    }
    else
    {
      // firelist finished: test scc, update checknexttar,
      // check for ignored transitions,
      // return to previous state
      if(CurrentState -> checkdfs[f->tempindex] ==
         CurrentState -> checkmin[f->tempindex])
      {

        // check if scc is tscc
        if(CurrentState -> checkdfs[f->tempindex] > MinBookmark)
        {
          // is tscc -> check for ignored transitions
          unsigned int CardIgnored;
          Transition * ignored;

          CardIgnored  = 0;
          for(ignored = ignored -> StartOfEnabledList;ignored;
            ignored = ignored -> NextEnabled)
          {
            if((ignored -> lstdisabled[f->tempindex] >=
               CurrentState -> checkdfs[f->tempindex]) &&
              (ignored -> lstfired[f->tempindex] <
              CurrentState -> checkdfs[f->tempindex]))
            {
              CardIgnored ++;
              break;
            }
          }
          if(CardIgnored)
          {
            // there are ignored transitions
            // -> fire all transitions
            int i;
#ifdef EXTENDEDCTL
            TemporalIndex = f -> tempindex;
#endif
            Transition ** newFL = firelist();
            // in ctl modelchecking, ignoring implies that
            // current firelist was reduced, i.e. singleton.
            // thus, we sort the unique already
            //fired transition to the beginning of the firelist.

            for(i=0; newFL[i] != CurrentState->checkfirelist[f->tempindex][0];i++);
            newFL[i] = newFL[0];
            newFL[0] = CurrentState -> checkfirelist[f->tempindex][0];
            delete [] CurrentState -> checkfirelist[f->tempindex];
            CurrentState -> checkfirelist[f->tempindex] = newFL;
            continue;
          }
        }
            
        // scc closed: remove members from checknexttar, value = F

        CurrentState -> value[f->index] = false;
        CurrentState -> known[f-> index] = true;
        while(tarjanroot -> checkdfs[f->tempindex] >=
            CurrentState -> checkdfs[f->tempindex])
        {
          State * tmp;
          tmp = tarjanroot;
          tarjanroot -> witness[f->tempindex] = (State *) 0;
          if(tarjanroot == tarjanroot -> checknexttar[f->tempindex])
          {
            tarjanroot -> checknexttar[f->tempindex] = (State *) 0;
            break;
          }
          tarjanroot = tarjanroot -> checknexttar[f->tempindex];
          tmp -> checknexttar[f->tempindex] = (State *) 0;
        }
      }
      // return to previous state
      if(NewState = CurrentState -> checkparent[f->tempindex])
      {
        if(NewState -> checkmin[f->tempindex] >
           CurrentState -> checkmin[f->tempindex])
        {
          NewState -> checkmin[f->tempindex] 
          = CurrentState -> checkmin[f->tempindex];
          NewState -> witness[f->tempindex] = CurrentState;
          NewState -> witnesstransition[f->tempindex] =
          NewState -> checkfirelist[f->tempindex]
              [NewState -> checkcurrent[f->tempindex]];
        }
        NewState -> checkfirelist[f->tempindex]
            [NewState -> checkcurrent[f->tempindex]]->backfire();
        NewState -> checkcurrent[f->tempindex]++;
      }
      CurrentState = NewState;
    }
  }
  if(found)
  {
    // set witness for current path
    while(NewState = CurrentState -> checkparent[f->tempindex])
    {
      NewState -> witness[f->tempindex] = CurrentState;
      NewState -> witnesstransition[f->tempindex] 
      = NewState -> checkfirelist[f->tempindex]
        [NewState -> checkcurrent[f->tempindex]];
      NewState -> value[f->index] = true;
      NewState -> checkfirelist[f->tempindex]
        [NewState -> checkcurrent[f->tempindex]]->backfire();
      CurrentState = NewState;
    }
    while(1)
    {
      State * tmp;
      tmp = tarjanroot;
      tarjanroot -> value[f->index] = true;
      if(tarjanroot == tarjanroot -> checknexttar[f->tempindex]) 
      {
        tarjanroot -> checknexttar[f->tempindex] = (State *) 0;
        break;
      }
      tarjanroot = tarjanroot -> checknexttar[f->tempindex];
      tmp -> checknexttar[f->tempindex] = (State *) 0;
    }
  }
}
        

void futuresearchAU(State * s, untilformula * f)
{
  unsigned int MinBookmark;
  State * tarjanroot;
  unsigned int i;
  State * CurrentState, * NewState;
  int found;

  MinBookmark = 0;
  found = 0;
  CurrentState = s;
  s -> checkprevtar[ f -> tempindex] = s -> checknexttar[f->tempindex] = s;
  tarjanroot = s; // End of Stack is recognised as loop back to root
          // Thus, states not on stack can be detected through
          // nil-pointer in checknexttar
#ifdef EXTENDEDCTL
  TemporalIndex = f -> tempindex;
#endif
  s -> checkfirelist[f->tempindex] =
#ifdef STUBBORN
  stubbornfirelistctl();
#else
  firelist();
#endif
  s -> checksucc[f->tempindex] = new State * [f -> tempcard];
  for(i=0;i<f -> tempcard;i++)
  {
    s -> checksucc[f->tempindex][i] = (State *) 0;
  }
  s -> checkparent[f->tempindex] = (State *) 0;
  s -> checkdfs[f -> tempindex] = s -> checkmin[f->tempindex] = currentdfsnum++;
  s->checkcurrent[f->tempindex] = 0;
  if(!(s->checkfirelist[f->tempindex]) || !(s->checkfirelist[f->tempindex][0]))
  {
    found = true;
    s->witness[f->tempindex] = (State *) 0;
    s->witnesstransition[f->tempindex] = (Transition *) 0;
  }
  else
  {
    while(CurrentState)
    {
      CurrentState -> value[f->index] = true;
      CurrentState -> known[f->index] = true;

      if(CurrentState -> checkfirelist[f->tempindex][CurrentState ->
                  checkcurrent[f->tempindex]])
      {
        // explore next state
        CurrentState -> checkfirelist[f->tempindex][CurrentState ->
                  checkcurrent[f->tempindex]] -> fire();
        Edges ++;
        if(!(Edges % REPORTFREQUENCY))
          cout << "st: " << NrStates << "    edg: " << Edges
          << "\n";
        if(NewState = binSearch())
        {
          // state exists
          if(NewState -> known[f->index])
          {
            // state exists, value known
            if(NewState -> value[f->index])
            {
              // state exists, value true -> on tarjan stack or
              // does not model phi
              if(NewState->checknexttar[f->tempindex])
              {
                // state exists, on tarjan stack -> set min,
                // update counterexample path
                if(CurrentState -> checkmin[f->tempindex]
                   > NewState -> checkdfs[f->tempindex])
                {
                  CurrentState -> checkmin[f->tempindex] =
                  NewState -> checkdfs[f->tempindex];
                  CurrentState -> witness[f->tempindex]
                  = NewState;
                  CurrentState -> witnesstransition[f->tempindex]
                  = CurrentState -> checkfirelist[f->tempindex]
                    [CurrentState -> checkcurrent[f->tempindex]];
                }
              }
              else
              {
                // new state belongs to other scc ->
                // current state does not belong to tscc
                MinBookmark = CurrentState -> checkdfs[f->tempindex];
              }
              CurrentState -> checkfirelist[f->tempindex]
              [CurrentState -> checkcurrent[f->tempindex]]
              -> backfire();
              CurrentState -> checkcurrent[f->tempindex]++;
            }
            else
            {
              // state exists, value false --> counterexample found!
              found = 1;
              CurrentState -> witness[f->tempindex] = NewState;
              CurrentState -> witnesstransition[f->tempindex]
               = CurrentState -> checkfirelist[f->tempindex]
               [CurrentState -> checkcurrent[f->tempindex]];
               CurrentState -> checkfirelist[f->tempindex]
               [CurrentState -> checkcurrent[f->tempindex]]
               ->backfire();
              break;
            }
          }
          else
          {
            // state exists, value unknown
            check(NewState,f->goal);
            if(NewState->value[f->goal->index])
            {
              // state exists, right true
              // -> backtracking
              CurrentState -> checkfirelist[f->tempindex]
              [CurrentState -> checkcurrent[f->tempindex]]
              -> backfire();
              CurrentState -> checkcurrent[f->tempindex]++;
            }
            else
            {
              // state exists, right subformula false
              check(NewState,f->hold);
              if(NewState->value[f->hold->index])
              {
                // state exists, left true, right false
                // -> continue search
                NewState -> checkparent[f->tempindex] 
                = CurrentState;
#ifdef EXTENDEDCTL
                TemporalIndex = f -> tempindex;
#endif
                NewState -> checkfirelist[f->tempindex] =
#ifdef STUBBORN
                  stubbornfirelistctl();
#else
                  firelist();
#endif
                if(!(NewState->checkfirelist[f->tempindex]) || !(NewState->checkfirelist[f->tempindex][0]))
                {
                  // deadlock  -> counterexample found!
                  found = 1;
                  NewState -> witness[f->tempindex] = (State *) 0;
                  CurrentState -> witnesstransition[f->tempindex]
                  =  (Transition *) 0;
                  CurrentState -> witness[f->tempindex] = NewState;
                  CurrentState -> witnesstransition[f->tempindex]
                  =  CurrentState -> checkfirelist[f->tempindex]
                     [CurrentState -> checkcurrent[f->tempindex]];
                  CurrentState -> checkfirelist[f->tempindex]
                   [CurrentState -> checkcurrent[f->tempindex]]
                   ->backfire();
                  break;
                }
                NewState -> checksucc[f->tempindex] = new State * [f -> tempcard];
                for(i=0;i<f -> tempcard;i++)
                {
                  NewState -> checksucc[f->tempindex][i] = (State *) 0;
                }
                NewState -> checkdfs[f->tempindex]
                = NewState -> checkmin[f->tempindex]
                = currentdfsnum;
                currentdfsnum++;
                NewState -> checkcurrent[f->tempindex] = 0;
                //NewState -> checknexttar[f->tempindex] =
                //tarjanroot;
                //tarjanroot = NewState;
                NewState -> checkprevtar[f->tempindex] = tarjanroot;
                NewState -> checknexttar[f->tempindex] = tarjanroot -> checknexttar[f->tempindex];
                tarjanroot = tarjanroot -> checknexttar[f->tempindex] = NewState;
                NewState -> checknexttar[f->tempindex] -> checkprevtar[f->tempindex] = NewState;

                CurrentState -> checksucc[f->tempindex][CurrentState -> checkcurrent[f->tempindex]] = NewState;
                CurrentState = NewState;
              }
              else
              {
                // left+right false  -> counterexample found!
                found = 1;
                CurrentState -> witness[f->tempindex] = NewState;
                CurrentState -> witnesstransition[f->tempindex]
                =  CurrentState -> checkfirelist[f->tempindex]
                   [CurrentState -> checkcurrent[f->tempindex]];
                CurrentState -> checkfirelist[f->tempindex]
                 [CurrentState -> checkcurrent[f->tempindex]]
                 ->backfire();
                break;
              }
            }
          }
        }
        else
        {
          // state is new
          NewState = binInsert();
          NrStates ++;
#ifdef MAXIMALSTATES      
                    checkMaximalStates(NrStates); ///// LINE ADDED BY NIELS
#endif
          NewState -> checkcurrent[f->tempindex] = 0;
          NewState -> checkdfs[f->tempindex] 
          = NewState -> checkmin[f->tempindex] = currentdfsnum;
          currentdfsnum ++;
          check(NewState,f->goal);
          if(NewState->value[f->goal->index])
          {
            // right true
            // -> backtracking
            CurrentState -> checkfirelist[f->tempindex]
            [CurrentState -> checkcurrent[f->tempindex]]
            -> backfire();
            CurrentState -> checkcurrent[f->tempindex]++;
          }
          else
          {
            // right subformula false
            check(NewState,f->hold);
            if(NewState->value[f->hold->index])
            {
              // left true, right false
              // -> continue search
              NewState -> checkparent[f->tempindex] 
              = CurrentState;
#ifdef EXTENDEDCTL
              TemporalIndex = f -> tempindex;
#endif
              NewState -> checkfirelist[f->tempindex] =
#ifdef STUBBORN
                stubbornfirelistctl();
#else
                firelist();
#endif
              if(!(NewState->checkfirelist[f->tempindex]) || !(NewState->checkfirelist[f->tempindex][0]))
              {
                // deadlock  -> counterexample found!
                found = 1;
                NewState -> witness[f->tempindex] = (State *) 0;
                CurrentState -> witnesstransition[f->tempindex]
                =  (Transition *) 0;
                CurrentState -> witness[f->tempindex] = NewState;
                CurrentState -> witnesstransition[f->tempindex]
                =  CurrentState -> checkfirelist[f->tempindex]
                   [CurrentState -> checkcurrent[f->tempindex]];
                CurrentState -> checkfirelist[f->tempindex]
                 [CurrentState -> checkcurrent[f->tempindex]]
                 ->backfire();
                break;
              }
              NewState -> checksucc[f->tempindex] = new State * [f -> tempcard];
              for(i=0;i<f -> tempcard;i++)
              {
                NewState -> checksucc[f->tempindex][i] = (State *) 0;
              }
              NewState -> checkcurrent[f->tempindex] = 0;
              //NewState -> checknexttar[f->tempindex] =
              //tarjanroot;
              //tarjanroot = NewState;
              NewState -> checkprevtar[f->tempindex] = tarjanroot;
              NewState -> checknexttar[f->tempindex] = tarjanroot -> checknexttar[f->tempindex];
              tarjanroot = tarjanroot -> checknexttar[f->tempindex] = NewState;
              NewState -> checknexttar[f->tempindex] -> checkprevtar[f->tempindex] = NewState;
              CurrentState -> checksucc[f->tempindex][CurrentState -> checkcurrent[f->tempindex]] = NewState;
              CurrentState = NewState;
            }
            else
            {
              // left+right false -> counterexample found!
              found = 1;
              CurrentState -> witness[f->tempindex] = NewState;
              CurrentState -> witnesstransition[f->tempindex]
              =  CurrentState -> checkfirelist[f->tempindex]
                 [CurrentState -> checkcurrent[f->tempindex]];
              CurrentState -> checkfirelist[f->tempindex]
               [CurrentState -> checkcurrent[f->tempindex]]
               ->backfire();
              break;
            }
          }
        }
      }
      else
      {
        // firelist finished: test scc, update checknexttar,
        // check for ignored transitions,
        // return to previous state
        if(CurrentState -> checkdfs[f->tempindex] ==
           CurrentState -> checkmin[f->tempindex])
        {

          // scc closed
          // check if scc is tscc
          if(CurrentState -> checkdfs[f->tempindex] > MinBookmark)
          {
            // is tscc -> check for ignored transitions
            unsigned int CardIgnored;
            Transition * ignored;

            CardIgnored  = 0;
            for(ignored = ignored -> StartOfEnabledList;ignored;
              ignored = ignored -> NextEnabled)
            {
              if((ignored -> lstdisabled[f->tempindex] >=
                 CurrentState -> checkdfs[f->tempindex]) &&
                (ignored -> lstfired[f->tempindex] <
                CurrentState -> checkdfs[f->tempindex]))
              {
                CardIgnored ++;
                break;
              }
            }
            if(CardIgnored)
            {
              // there are ignored transitions
              // -> fire all transitions
              int i;
#ifdef EXTENDEDCTL
              TemporalIndex = f -> tempindex;
#endif
              Transition ** newFL = firelist();
              // in ctl modelchecking, ignoring implies that
              // current firelist was reduced, i.e. singleton.
              // thus, we sort the unique already
              //fired transition to the beginning of the firelist.

              for(i=0; newFL[i] != CurrentState->checkfirelist[f->tempindex][0];i++);
              newFL[i] = newFL[0];
              newFL[0] = CurrentState -> checkfirelist[f->tempindex][0];
              delete [] CurrentState -> checkfirelist[f->tempindex];
              CurrentState -> checkfirelist[f->tempindex] = newFL;
              continue;
            }
          }
              
          // scc closed: remove members from tarjanstack, search fair sc set

          //while(tarjanroot -> checkdfs[f->tempindex] >=
          //    CurrentState -> checkdfs[f->tempindex])
          //{
          //  State * tmp;
          //  tmp = tarjanroot;
          //  tarjanroot -> witness[f->tempindex] = (State *) 0;
          //  if(tarjanroot == tarjanroot -> checknexttar[f->tempindex])
          //  {
          //    tarjanroot -> checknexttar[f->tempindex] = (State *) 0;
          //    break;
          //  }
          //  tarjanroot = tarjanroot -> checknexttar[f->tempindex];
          //  tmp -> checknexttar[f->tempindex] = (State *) 0;
          //}
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // unlink scc and check it for counterexample sc sets
  //                        if(CurrentState != tarjanroot -> checknexttar[f->tempindex]) // current != bottom(stack)
  //                        {
  //                                State * newroot;
  //                                newroot = CurrentState -> checkprevtar[f->tempindex];
  //                                newroot -> checknexttar[f->tempindex] = tarjanroot -> checknexttar[f->tempindex];
  //                                tarjanroot -> checknexttar[f->tempindex] -> checkprevtar[f->tempindex] = newroot;
  //                                tarjanroot -> checknexttar[f->tempindex] = CurrentState;
  //                                CurrentState -> checkprevtar[f->tempindex] = tarjanroot;
  //                                tarjanroot = newroot;
  //                        }
  //      State * start;
  //                               for(s = CurrentState -> checknexttar[f->tempindex]; s != CurrentState; s = s -> checknexttar[f->tempindex])
  //                                {
  //     s -> checktarlevel[f->tempindex] = 1;
  //                 }
  //start -> checktarlevel[f->tempindex] = 1;
  //                                // analyze this
  //                                State * oldpar;
  //                                unsigned int oldc;
  //                                oldpar = CurrentState -> checkparent[f->tempindex];
  //                                oldc = CurrentState -> checkcurrent[f->tempindex];
  //                                if(check_analyse_fairness(start,1,f->tempindex))
  //                                {
  //          // value false; infinite counterexample
  //          found = 2;
  //          break;
  //        }
  //                                CurrentState -> checkparent[f->tempindex] = oldpar;
  //                                CurrentState -> checkcurrent[f->tempindex] = oldc;
  //
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        }
        // return to previous state
        if(NewState = CurrentState -> checkparent[f->tempindex])
        {
          if(NewState -> checkmin[f->tempindex] >
             CurrentState -> checkmin[f->tempindex])
          {
            NewState -> checkmin[f->tempindex] 
            = CurrentState -> checkmin[f->tempindex];
            NewState -> witness[f->tempindex] = CurrentState;
            NewState -> witnesstransition[f->tempindex] =
            NewState -> checkfirelist[f->tempindex]
                [NewState -> checkcurrent[f->tempindex]];
          }
          NewState -> checkfirelist[f->tempindex]
              [NewState -> checkcurrent[f->tempindex]]->backfire();
          NewState -> checkcurrent[f->tempindex]++;
        }
        CurrentState = NewState;
      }
    }
  }
  if(found)
  {
    // set witness for current path
    while(NewState = CurrentState -> checkparent[f->tempindex])
    {
      NewState -> witness[f->tempindex] = CurrentState;
      NewState -> witnesstransition[f->tempindex] 
      = NewState -> checkfirelist[f->tempindex]
        [NewState -> checkcurrent[f->tempindex]];
      NewState -> value[f->index] = true;
      NewState -> checkfirelist[f->tempindex]
        [NewState -> checkcurrent[f->tempindex]]->backfire();
      CurrentState = NewState;
    }
    while(1)
    {
      State * tmp;
      tmp = tarjanroot;
      tarjanroot -> value[f->index] = true;
      if(tarjanroot == tarjanroot -> checknexttar[f->tempindex]) 
      {
        tarjanroot -> checkprevtar[f->tempindex] = (State *) 0;
        break;
      }
      tarjanroot = tarjanroot -> checkprevtar[f->tempindex];
      tmp -> checkprevtar[f->tempindex] = tmp -> checknexttar[f->tempindex]= (State *) 0;
    }
  }
}

    
void searchAU(State * s, untilformula * f)
{
  unsigned int i;
  State * CurrentState, * NewState;
  bool found;

  found = false;
  CurrentState = s;
#ifdef EXTENDEDCTL
  TemporalIndex = f -> tempindex;
#endif
  s -> checkfirelist[f->tempindex] =
#ifdef STUBBORN
  stubbornfirelistctl();
#else
  firelist();
#endif
  s->checkcurrent[f->tempindex] = 0;
  s -> checkparent[f->tempindex] = (State *) 0;
  
  while(CurrentState)
  {
    CurrentState -> value[f->index] = false;
    CurrentState -> known[f->index] = true;

    if(CurrentState -> checkfirelist[f->tempindex][CurrentState ->
                checkcurrent[f->tempindex]])
    {
      // explore next state
      CurrentState -> checkfirelist[f->tempindex][CurrentState ->
                checkcurrent[f->tempindex]] -> fire();
      Edges ++;
      if(!(Edges % REPORTFREQUENCY))
        cout << "st: " << NrStates << "    edg: " << Edges
        << "\n";
      if(NewState = binSearch())
      {
        // state exists
        if(NewState -> known[f->index])
        {
          // state exists, value known
          if(!NewState -> value[f->index])
          {
            // state exists, value f --> counterex found!
            // state either on stack or does not model A phi U psi
            found = true;
            CurrentState -> witness[f->tempindex] = NewState;
            CurrentState -> witnesstransition[f->tempindex]
             = CurrentState -> checkfirelist[f->tempindex]
             [CurrentState -> checkcurrent[f->tempindex]];
             CurrentState -> checkfirelist[f->tempindex]
             [CurrentState -> checkcurrent[f->tempindex]]
             ->backfire();
            break;
          }
          else
          {
            // state exists, value true -> continue search
            CurrentState -> checkfirelist[f->tempindex]
            [CurrentState -> checkcurrent[f->tempindex]]
            -> backfire();
            CurrentState -> checkcurrent[f->tempindex]++;
          }
        }
        else
        {
          // state exists, value unknown
          check(NewState,f->goal);
          if(NewState->value[f->goal->index])
          {
            // right subformula true -> backtracking
            CurrentState -> checkfirelist[f->tempindex]
            [CurrentState -> checkcurrent[f->tempindex]]
            -> backfire();
            CurrentState -> checkcurrent[f->tempindex]++;
          }
          else
          {
            // state exists, right subformula true
            check(NewState,f->hold);
            if(NewState->value[f->hold->index])
            {
              // state exists, left true, right false
              // -> continue search
              NewState -> checkparent[f->tempindex] 
              = CurrentState;
#ifdef EXTENDEDCTL
              TemporalIndex = f -> tempindex;
#endif
              NewState -> checkfirelist[f->tempindex] =
#ifdef STUBBORN
                stubbornfirelistctl();
#else
                firelist();
#endif
              NewState -> checkcurrent[f->tempindex] = 0;
              CurrentState = NewState;
            }
            else
            {
              // state exists, left false, right false
              // -> counterex found
              found = true;
              CurrentState -> witness[f->tempindex] = NewState;
              CurrentState -> witnesstransition[f->tempindex]
              =  CurrentState -> checkfirelist[f->tempindex]
                 [CurrentState -> checkcurrent[f->tempindex]];
              CurrentState -> checkfirelist[f->tempindex]
               [CurrentState -> checkcurrent[f->tempindex]]
               ->backfire();
              break;
            }
          }
        }
      }
      else
      {
        // state is new
        NewState = binInsert();
        NrStates ++;
#ifdef MAXIMALSTATES      
                checkMaximalStates(NrStates); ///// LINE ADDED BY NIELS
#endif
        NewState -> checkcurrent[f->tempindex] = 0;
        check(NewState,f->goal);
        if(NewState->value[f->goal->index])
        {
          // right subformula true -> backtracking!
          CurrentState -> checkfirelist[f->tempindex]
          [CurrentState -> checkcurrent[f->tempindex]]
          -> backfire();
          CurrentState -> checkcurrent[f->tempindex]++;
        }
        else
        {
          // right subformula false
          check(NewState,f->hold);
          if(NewState->value[f->hold->index])
          {
            // left true, right false
            // -> continue search
            NewState -> checkparent[f->tempindex] 
            = CurrentState;
#ifdef EXTENDEDCTL
            TemporalIndex = f -> tempindex;
#endif
            NewState -> checkfirelist[f->tempindex] =
#ifdef STUBBORN
              stubbornfirelistctl();
#else
              firelist();
#endif
            NewState -> checkcurrent[f->tempindex] = 0;
            CurrentState = NewState;
          }
          else
          {
            // left false, right false
            // -> backtracking
            found = true;
            CurrentState -> witness[f->tempindex] = NewState;
            CurrentState -> witnesstransition[f->tempindex]
            =  CurrentState -> checkfirelist[f->tempindex]
               [CurrentState -> checkcurrent[f->tempindex]];
            CurrentState -> checkfirelist[f->tempindex]
             [CurrentState -> checkcurrent[f->tempindex]]
             ->backfire();
            break;
          }
        }
      }
    }
    else
    {
      // return to previous state
      CurrentState -> value[f->index] = true;
      if(NewState = CurrentState -> checkparent[f->tempindex])
      {
        NewState -> checkfirelist[f->tempindex]
            [NewState -> checkcurrent[f->tempindex]]->backfire();
        NewState -> checkcurrent[f->tempindex]++;
      }
      CurrentState = NewState;
    }
  }
  if(found)
  {
    // set witness for current path
    while(NewState = CurrentState -> checkparent[f->tempindex])
    {
      NewState -> witness[f->tempindex] = CurrentState;
      NewState -> witnesstransition[f->tempindex] 
      = NewState -> checkfirelist[f->tempindex]
        [NewState -> checkcurrent[f->tempindex]];
      NewState -> checkfirelist[f->tempindex]
        [NewState -> checkcurrent[f->tempindex]]->backfire();
      CurrentState = NewState;
    }
  }
}


void searchAF(State * s, unarytemporalformula * f)
{
  unsigned int i;
  State * CurrentState, * NewState;
  bool found;

  found = false;
  CurrentState = s;
#ifdef EXTENDEDCTL
  TemporalIndex = f -> tempindex;
#endif
  s -> checkfirelist[f->tempindex] =
#ifdef STUBBORN
  stubbornfirelistctl();
#else
  firelist();
#endif
  s -> checkcurrent[f->tempindex] = 0;
  s -> checkparent[f->tempindex] = (State *) 0;
  
  while(CurrentState)
  {
    CurrentState -> value[f->index] = false;
    CurrentState -> known[f->index] = true;

    if(CurrentState -> checkfirelist[f->tempindex][CurrentState ->
                checkcurrent[f->tempindex]])
    {
      // explore next state
      CurrentState -> checkfirelist[f->tempindex][CurrentState ->
                checkcurrent[f->tempindex]] -> fire();
      Edges ++;
      if(!(Edges % REPORTFREQUENCY))
        cout << "st: " << NrStates << "    edg: " << Edges
        << "\n";
      if(NewState = binSearch())
      {
        // state exists
        if(NewState -> known[f->index])
        {
          // state exists, value known
          if(!NewState -> value[f->index])
          {
            // state exists, value f --> counterex found!
            // state either on stack or does not model A phi U psi
            found = true;
            CurrentState -> witness[f->tempindex] = NewState;
            CurrentState -> witnesstransition[f->tempindex]
             = CurrentState -> checkfirelist[f->tempindex]
             [CurrentState -> checkcurrent[f->tempindex]];
             CurrentState -> checkfirelist[f->tempindex]
             [CurrentState -> checkcurrent[f->tempindex]]
             ->backfire();
            break;
          }
          else
          {
            // state exists, value true -> continue search
            CurrentState -> checkfirelist[f->tempindex]
            [CurrentState -> checkcurrent[f->tempindex]]
            -> backfire();
            CurrentState -> checkcurrent[f->tempindex]++;
          }
        }
        else
        {
          // state exists, value unknown
          check(NewState,f->element);
          if(NewState->value[f->element->index])
          {
            // subformula true -> backtracking
            CurrentState -> checkfirelist[f->tempindex]
            [CurrentState -> checkcurrent[f->tempindex]]
            -> backfire();
            CurrentState -> checkcurrent[f->tempindex]++;
          }
          else
          {
            // subformula false -> continue search
            NewState -> checkparent[f->tempindex] 
            = CurrentState;
#ifdef EXTENDEDCTL
            TemporalIndex = f -> tempindex;
#endif
            NewState -> checkfirelist[f->tempindex] =
#ifdef STUBBORN
              stubbornfirelistctl();
#else
              firelist();
#endif
            NewState -> checkcurrent[f->tempindex] = 0;
            CurrentState = NewState;
          }
        }
      }
      else
      {
        // state is new
        NewState = binInsert();
        NrStates ++;
#ifdef MAXIMALSTATES      
                checkMaximalStates(NrStates); ///// LINE ADDED BY NIELS
#endif
        NewState -> checkcurrent[f->tempindex] = 0;
        check(NewState,f->element);
        if(NewState->value[f->element->index])
        {
          // subformula true -> backtracking!
          CurrentState -> checkfirelist[f->tempindex]
          [CurrentState -> checkcurrent[f->tempindex]]
          -> backfire();
          CurrentState -> checkcurrent[f->tempindex]++;
        }
        else
        {
          // subformula false -> continue search
          NewState -> checkparent[f->tempindex] 
          = CurrentState;
#ifdef EXTENDEDCTL
          TemporalIndex = f -> tempindex;
#endif
          NewState -> checkfirelist[f->tempindex] =
#ifdef STUBBORN
            stubbornfirelistctl();
#else
            firelist();
#endif
          NewState -> checkcurrent[f->tempindex] = 0;
          CurrentState = NewState;
        }
      }
    }
    else
    {
      // return to previous state
      CurrentState -> value[f->index] = true;
      if(NewState = CurrentState -> checkparent[f->tempindex])
      {
        NewState -> checkfirelist[f->tempindex]
            [NewState -> checkcurrent[f->tempindex]]->backfire();
        NewState -> checkcurrent[f->tempindex]++;
      }
      CurrentState = NewState;
    }
  }
  if(found)
  {
    // set witness for current path
    while(NewState = CurrentState -> checkparent[f->tempindex])
    {
      NewState -> witness[f->tempindex] = CurrentState;
      NewState -> witnesstransition[f->tempindex] 
      = NewState -> checkfirelist[f->tempindex]
        [NewState -> checkcurrent[f->tempindex]];
      NewState -> checkfirelist[f->tempindex]
        [NewState -> checkcurrent[f->tempindex]]->backfire();
      CurrentState = NewState;
    }
  }
}


void searchEG(State * s, unarytemporalformula * f)
{
  unsigned int i;
  State * CurrentState, * NewState;
  bool found;

  found = false;
  CurrentState = s;
#ifdef EXTENDEDCTL
  TemporalIndex = f -> tempindex;
#endif
  s -> checkfirelist[f->tempindex] =
#ifdef STUBBORN
  stubbornfirelistctl();
#else
  firelist();
#endif
  s -> checkcurrent[f->tempindex] = 0;
  s -> checkparent[f->tempindex] = (State *) 0;
  s -> checkdfs[f -> tempindex] = s -> checkmin[f->tempindex] = 0;
  
  while(CurrentState)
  {
    CurrentState -> value[f->index] = true;
    CurrentState -> known[f->index] = true;

    if(CurrentState -> checkfirelist[f->tempindex][CurrentState ->
                checkcurrent[f->tempindex]])
    {
      // explore next state
      CurrentState -> checkfirelist[f->tempindex][CurrentState ->
                checkcurrent[f->tempindex]] -> fire();
      Edges ++;
      if(!(Edges % REPORTFREQUENCY))
        cout << "st: " << NrStates << "    edg: " << Edges
        << "\n";
      if(NewState = binSearch())
      {
        // state exists
        if(NewState -> known[f->index])
        {
          // state exists, value known
          if(NewState -> value[f->index])
          {
            // state exists, value t --> witness found!
            // state either on stack or models EG phi
            found = true;
            CurrentState -> witness[f->tempindex] = NewState;
            CurrentState -> witnesstransition[f->tempindex]
             = CurrentState -> checkfirelist[f->tempindex]
             [CurrentState -> checkcurrent[f->tempindex]];
             CurrentState -> checkfirelist[f->tempindex]
             [CurrentState -> checkcurrent[f->tempindex]]
             ->backfire();
            break;
          }
          else
          {
            // state exists, value false -> continue search
            CurrentState -> checkfirelist[f->tempindex]
            [CurrentState -> checkcurrent[f->tempindex]]
            -> backfire();
            CurrentState -> checkcurrent[f->tempindex]++;
          }
        }
        else
        {
          // state exists, value unknown
          check(NewState, f ->element);
          if(!NewState->value[ f ->element->index])
          {
            // subformula false -> backtracking
            CurrentState -> checkfirelist[f->tempindex]
            [CurrentState -> checkcurrent[f->tempindex]]
            -> backfire();
            CurrentState -> checkcurrent[f->tempindex]++;
          }
          else
          {
            // subformula true -> continue search
            NewState -> checkparent[f->tempindex] 
            = CurrentState;
#ifdef EXTENDEDCTL
            TemporalIndex = f -> tempindex;
#endif
            NewState -> checkfirelist[f->tempindex] =
#ifdef STUBBORN
              stubbornfirelistctl();
#else
              firelist();
#endif
            NewState -> checkcurrent[f->tempindex] = 0;
            CurrentState = NewState;
          }
        }
      }
      else
      {
        // state is new
        NewState = binInsert();
        NrStates ++;
#ifdef MAXIMALSTATES      
                checkMaximalStates(NrStates); ///// LINE ADDED BY NIELS
#endif
        NewState -> checkcurrent[f->tempindex] = 0;
        check(NewState,f ->element);
        if(!NewState->value[f ->element->index])
        {
          // subformula false -> backtracking!
          CurrentState -> checkfirelist[f->tempindex]
          [CurrentState -> checkcurrent[f->tempindex]]
          -> backfire();
          CurrentState -> checkcurrent[f->tempindex]++;
        }
        else
        {
          // subformula true -> continue search
          NewState -> checkparent[f->tempindex] 
          = CurrentState;
#ifdef EXTENDEDCTL
          TemporalIndex = f -> tempindex;
#endif
          NewState -> checkfirelist[f->tempindex] =
#ifdef STUBBORN
            stubbornfirelistctl();
#else
            firelist();
#endif
          NewState -> checkcurrent[f->tempindex] = 0;
          CurrentState = NewState;
        }
      }
    }
    else
    {
      // return to previous state
      CurrentState -> value[f->index] = false;
      if(NewState = CurrentState -> checkparent[f->tempindex])
      {
        NewState -> checkfirelist[f->tempindex]
            [NewState -> checkcurrent[f->tempindex]]->backfire();
        NewState -> checkcurrent[f->tempindex]++;
      }
      CurrentState = NewState;
    }
  }
  if(found)
  {
    // set witness for current path
    while(NewState = CurrentState -> checkparent[f->tempindex])
    {
      NewState -> witness[f->tempindex] = CurrentState;
      NewState -> witnesstransition[f->tempindex] 
      = NewState -> checkfirelist[f->tempindex]
        [NewState -> checkcurrent[f->tempindex]];
      NewState -> checkfirelist[f->tempindex]
        [NewState -> checkcurrent[f->tempindex]]->backfire();
      CurrentState = NewState;
    }
  }
}


void futuresearchAF(State * s, unarytemporalformula * f)
{
  unsigned int i;
  State * CurrentState, * NewState;
  bool found;

  found = false;
  CurrentState = s;
#ifdef EXTENDEDCTL
  TemporalIndex = f -> tempindex;
#endif
  s -> checkfirelist[f->tempindex] =
#ifdef STUBBORN
  stubbornfirelistctl();
#else
  firelist();
#endif
  s -> checkcurrent[f->tempindex] = 0;
  s -> checkparent[f->tempindex] = (State *) 0;
  s->value[f->index] = false;
  s->known[f->index] = true;
  
  if(!(s->checkfirelist[f->tempindex]) || !(s->checkfirelist[f->tempindex][0]))
  {
    found = true;
    s->witness[f->tempindex] = (State *) 0;
    s->witnesstransition[f->tempindex] = (Transition *) 0;
  }
  else
  {
    while(CurrentState)
    {
      CurrentState -> value[f->index] = false;
      CurrentState -> known[f->index] = true;

      if(CurrentState -> checkfirelist[f->tempindex][CurrentState ->
                  checkcurrent[f->tempindex]])
      {
        // explore next state
        CurrentState -> checkfirelist[f->tempindex][CurrentState ->
                  checkcurrent[f->tempindex]] -> fire();
        Edges ++;
        if(!(Edges % REPORTFREQUENCY))
          cout << "st: " << NrStates << "    edg: " << Edges
          << "\n";
        if(NewState = binSearch())
        {
          // state exists
          if(NewState -> known[f->index])
          {
            // state exists, value known
            if(!NewState -> value[f->index])
            {
              // state exists, value f --> counterex found!
              // state either on stack or does not model AF phi
              found = true;
              CurrentState -> witness[f->tempindex] = NewState;
              CurrentState -> witnesstransition[f->tempindex]
               = CurrentState -> checkfirelist[f->tempindex]
               [CurrentState -> checkcurrent[f->tempindex]];
               CurrentState -> checkfirelist[f->tempindex]
               [CurrentState -> checkcurrent[f->tempindex]]
               ->backfire();
              break;
            }
            else
            {
              // state exists, value true -> continue search
              CurrentState -> checkfirelist[f->tempindex]
              [CurrentState -> checkcurrent[f->tempindex]]
              -> backfire();
              CurrentState -> checkcurrent[f->tempindex]++;
            }
          }
          else
          {
            // state exists, value unknown
            check(NewState,f->element);
            if(NewState->value[f->element->index])
            {
              // subformula true -> backtracking
              CurrentState -> checkfirelist[f->tempindex]
              [CurrentState -> checkcurrent[f->tempindex]]
              -> backfire();
              CurrentState -> checkcurrent[f->tempindex]++;
            }
            else
            {
              // subformula false -> continue search
              NewState -> checkparent[f->tempindex] 
              = CurrentState;
#ifdef EXTENDEDCTL
              TemporalIndex = f -> tempindex;
#endif
              NewState -> checkfirelist[f->tempindex] =
#ifdef STUBBORN
                stubbornfirelistctl();
#else
                firelist();
#endif
              if(!(NewState -> checkfirelist[f->tempindex]) || !(NewState -> checkfirelist[f->tempindex][0]))
              {
                // deadlock --> counterex found!
                found = true;
                CurrentState -> witness[f->tempindex] = NewState;
                NewState -> witness[f->tempindex] = (State *) 0;
                NewState -> witnesstransition[f->tempindex] = (Transition *) 0;
                CurrentState -> witnesstransition[f->tempindex]
                 = CurrentState -> checkfirelist[f->tempindex]
                 [CurrentState -> checkcurrent[f->tempindex]];
                 CurrentState -> checkfirelist[f->tempindex]
                 [CurrentState -> checkcurrent[f->tempindex]]
                 ->backfire();
                break;
              }
              else
              {
                NewState -> checkcurrent[f->tempindex] = 0;
                CurrentState = NewState;
              }
            }
          }
        }
        else
        {
          // state is new
          NewState = binInsert();
          NrStates ++;
#ifdef MAXIMALSTATES      
                    checkMaximalStates(NrStates); ///// LINE ADDED BY NIELS
#endif
          NewState -> checkcurrent[f->tempindex] = 0;
          check(NewState,f->element);
          if(NewState->value[f->element->index])
          {
            // subformula true -> backtracking!
            CurrentState -> checkfirelist[f->tempindex]
            [CurrentState -> checkcurrent[f->tempindex]]
            -> backfire();
            CurrentState -> checkcurrent[f->tempindex]++;
          }
          else
          {
            // subformula false -> continue search
            NewState -> checkparent[f->tempindex] 
            = CurrentState;
#ifdef EXTENDEDCTL
            TemporalIndex = f -> tempindex;
#endif
            NewState -> checkfirelist[f->tempindex] =
#ifdef STUBBORN
              stubbornfirelistctl();
#else
              firelist();
#endif
            if(!(NewState -> checkfirelist[f->tempindex]) || !(NewState -> checkfirelist[f->tempindex][0]))
            {
              // deadlock --> counterex found!
              found = true;
              CurrentState -> witness[f->tempindex] = NewState;
                NewState -> witness[f->tempindex] = (State *) 0;
                NewState -> witnesstransition[f->tempindex] = (Transition *) 0;
              CurrentState -> witnesstransition[f->tempindex]
               = CurrentState -> checkfirelist[f->tempindex]
               [CurrentState -> checkcurrent[f->tempindex]];
               CurrentState -> checkfirelist[f->tempindex]
               [CurrentState -> checkcurrent[f->tempindex]]
               ->backfire();
              break;
            }
            else
            {
              NewState -> checkcurrent[f->tempindex] = 0;
              CurrentState = NewState;
            }
          }
        }
      }
      else
      {
        // return to previous state
        CurrentState -> value[f->index] = true;
        if(NewState = CurrentState -> checkparent[f->tempindex])
        {
          NewState -> checkfirelist[f->tempindex]
              [NewState -> checkcurrent[f->tempindex]]->backfire();
          NewState -> checkcurrent[f->tempindex]++;
        }
        CurrentState = NewState;
      }
    }
  }
  if(found)
  {
    // set witness for current path
    while(NewState = CurrentState -> checkparent[f->tempindex])
    {
      NewState -> witness[f->tempindex] = CurrentState;
      NewState -> witnesstransition[f->tempindex] 
      = NewState -> checkfirelist[f->tempindex]
        [NewState -> checkcurrent[f->tempindex]];
      NewState -> checkfirelist[f->tempindex]
        [NewState -> checkcurrent[f->tempindex]]->backfire();
      CurrentState = NewState;
    }
  }
}


void futuresearchEG(State * s, unarytemporalformula * f)
{
  unsigned int i;
  State * CurrentState, * NewState;
  bool found;

  found = false;
  CurrentState = s;
#ifdef EXTENDEDCTL
  TemporalIndex = f -> tempindex;
#endif
  s -> checkfirelist[f->tempindex] =
#ifdef STUBBORN
  stubbornfirelistctl();
#else
  firelist();
#endif
  s -> checkcurrent[f->tempindex] = 0;
  s -> checkparent[f->tempindex] = (State *) 0;
  s -> checkdfs[f -> tempindex] = s -> checkmin[f->tempindex] = 0;
  s->value[f->index] = true;
  s->known[f->index] = true;
  
  if(!(s->checkfirelist[f->tempindex]) || !(s->checkfirelist[f->tempindex][0]))
  {
    found = true;
    s->witness[f->tempindex] = (State *) 0;
    s->witnesstransition[f->tempindex] = (Transition *) 0;
  }
  else
  {
    while(CurrentState)
    {
      CurrentState -> value[f->index] = true;
      CurrentState -> known[f->index] = true;

      if(CurrentState -> checkfirelist[f->tempindex][CurrentState ->
                  checkcurrent[f->tempindex]])
      {
        // explore next state
        CurrentState -> checkfirelist[f->tempindex][CurrentState ->
                  checkcurrent[f->tempindex]] -> fire();
        Edges ++;
        if(!(Edges % REPORTFREQUENCY))
          cout << "st: " << NrStates << "    edg: " << Edges
          << "\n";
        if(NewState = binSearch())
        {
          // state exists
          if(NewState -> known[f->index])
          {
            // state exists, value known
            if(NewState -> value[f->index])
            {
              // state exists, value t --> witness found!
              // state either on stack or models EG phi
              found = true;
              CurrentState -> witness[f->tempindex] = NewState;
              CurrentState -> witnesstransition[f->tempindex]
               = CurrentState -> checkfirelist[f->tempindex]
               [CurrentState -> checkcurrent[f->tempindex]];
               CurrentState -> checkfirelist[f->tempindex]
               [CurrentState -> checkcurrent[f->tempindex]]
               ->backfire();
              break;
            }
            else
            {
              // state exists, value false -> continue search
              CurrentState -> checkfirelist[f->tempindex]
              [CurrentState -> checkcurrent[f->tempindex]]
              -> backfire();
              CurrentState -> checkcurrent[f->tempindex]++;
            }
          }
          else
          {
            // state exists, value unknown
            check(NewState, f ->element);
            if(!NewState->value[ f ->element->index])
            {
              // subformula false -> backtracking
              CurrentState -> checkfirelist[f->tempindex]
              [CurrentState -> checkcurrent[f->tempindex]]
              -> backfire();
              CurrentState -> checkcurrent[f->tempindex]++;
            }
            else
            {
              // subformula true -> continue search
              NewState -> checkparent[f->tempindex] 
              = CurrentState;
#ifdef EXTENDEDCTL
              TemporalIndex = f -> tempindex;
#endif
              NewState -> checkfirelist[f->tempindex] =
#ifdef STUBBORN
                stubbornfirelistctl();
#else
                firelist();
#endif
              if(!(NewState -> checkfirelist[f->tempindex]) || !(NewState -> checkfirelist[f->tempindex][0]))
              {
                // deadlock --> witness found!
                found = true;
                CurrentState -> witness[f->tempindex] = NewState;
                CurrentState -> witnesstransition[f->tempindex]
                 = CurrentState -> checkfirelist[f->tempindex]
                 [CurrentState -> checkcurrent[f->tempindex]];
                 CurrentState -> checkfirelist[f->tempindex]
                 [CurrentState -> checkcurrent[f->tempindex]]
                 ->backfire();
                break;
              }
              else
              {
                NewState -> checkcurrent[f->tempindex] = 0;
                CurrentState = NewState;
              }
            }
          }
        }
        else
        {
          // state is new
          NewState = binInsert();
          NrStates ++;
#ifdef MAXIMALSTATES      
                    checkMaximalStates(NrStates); ///// LINE ADDED BY NIELS
#endif
          NewState -> checkcurrent[f->tempindex] = 0;
          check(NewState,f ->element);
          if(!NewState->value[f ->element->index])
          {
            // subformula false -> backtracking!
            CurrentState -> checkfirelist[f->tempindex]
            [CurrentState -> checkcurrent[f->tempindex]]
            -> backfire();
            CurrentState -> checkcurrent[f->tempindex]++;
          }
          else
          {
            // subformula true -> continue search
            NewState -> checkparent[f->tempindex] 
            = CurrentState;
#ifdef EXTENDEDCTL
            TemporalIndex = f -> tempindex;
#endif
            NewState -> checkfirelist[f->tempindex] =
#ifdef STUBBORN
              stubbornfirelistctl();
#else
              firelist();
#endif
            if(!(NewState -> checkfirelist[f->tempindex]) || !(NewState -> checkfirelist[f->tempindex][0]))
            {
              // deadlock --> witness found!
              found = true;
              CurrentState -> witness[f->tempindex] = NewState;
              CurrentState -> witnesstransition[f->tempindex]
               = CurrentState -> checkfirelist[f->tempindex]
               [CurrentState -> checkcurrent[f->tempindex]];
               CurrentState -> checkfirelist[f->tempindex]
               [CurrentState -> checkcurrent[f->tempindex]]
               ->backfire();
              break;
            }
            else
            {
              NewState -> checkcurrent[f->tempindex] = 0;
              CurrentState = NewState;
            }
          }
        }
      }
      else
      {
        // return to previous state
        CurrentState -> value[f->index] = false;
        if(NewState = CurrentState -> checkparent[f->tempindex])
        {
          NewState -> checkfirelist[f->tempindex]
              [NewState -> checkcurrent[f->tempindex]]->backfire();
          NewState -> checkcurrent[f->tempindex]++;
        }
        CurrentState = NewState;
      }
    }
  }
  if(found)
  {
    // set witness for current path
    while(NewState = CurrentState -> checkparent[f->tempindex])
    {
      NewState -> witness[f->tempindex] = CurrentState;
      NewState -> witnesstransition[f->tempindex] 
      = NewState -> checkfirelist[f->tempindex]
        [NewState -> checkcurrent[f->tempindex]];
      NewState -> checkfirelist[f->tempindex]
        [NewState -> checkcurrent[f->tempindex]]->backfire();
      CurrentState = NewState;
    }
  }
}

#endif
