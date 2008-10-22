#include "graph.H"
#include "net.H"
#include "graph.H"
#include "formula.H"

void stubbornclosure();
void stubborninsert(Transition *);
Transition ** stubbornfirelist(State *s, formula * f);
Transition ** stubbornfirelistneg(State *s, formula * f);
Transition ** stubbornfireliststatic();
Transition ** stubbornfirelistnogoal();
Transition ** stubbornfirelistpredicate();
Transition ** stubbornfirelistreach();
Transition ** tsccstubbornlist();
Transition ** structstubbornset();
Transition ** stubbornfirelistctl();
Transition ** relaxedstubbornset();
Transition ** structreachstubbornset();
#if defined(MODELCHECKING) && defined(STUBBORN)
void sortscapegoats();
#endif









