#include <Formula/LTL/BuechiAutomata.h>
#include <cstdlib>
#include <vector>
#include <math.h>
#include <Net/Net.h>

int BuechiAutomata::getSuccessors(index_t **list,
                                  index_t currentState)
{
    index_t cardTransitionList = cardTransitions[currentState];
    uint32_t **transitionsList = transitions[currentState];

    *list = new index_t[cardEnabled[currentState]];
    int curCard = 0;

    ////rep->message("SIZE %d (curState %d)",cardEnabled[currentState], currentState);
    for (int i = 0; i < cardTransitionList; i++)
    {
        ////rep->message("checking %d (%d) -> %d",currentState, atomicPropositions[transitionsList[i][0]]->getPredicate()->value,transitionsList[i][1]);
        if (atomicPropositions[transitionsList[i][0]]->getPredicate()->value)
        {
            ////rep->message("NOW PROP %d (p:%d)--> TRUE",i,atomicPropositions[transitionsList[i][0]]);

            //rep->message("List(%d) %d = %d @ %d/%d [%d] (p: %d)",i,curCard,transitionsList[i][1], cardEnabled[currentState],cardTransitionList,currentState,atomicPropositions[transitionsList[i][0]]);
            (*list)[curCard++] = transitionsList[i][1];
        } //else
        //	//rep->message("NOW PROP %d --> FALSE",i);
    }
    ////rep->message("END");
    return curCard;
}

void BuechiAutomata::updateProperties(NetState &ns, index_t transition)
{
    //rep->message("UPDATE");
    for (int i = 0; i < cardStates; i++)
    {
        cardEnabled[i] = 0;
    }
    for (int i = 0; i < cardAtomicPropositions; i++)
        if (atomicPropositions[i]->checkProperty(ns, transition))
        {
            //rep->message("CHECK PROP %d (s = %d, p:%d) --> TRUE",i,atomicPropotions_backlist[i],atomicPropositions[i]);
            cardEnabled[atomicPropotions_backlist[i]] ++;
        } //else
    ////rep->message("CHECK PROP %d (s = %d, p:%d)--> FALSE",i,atomicPropotions_backlist[i],atomicPropositions[i]);
}

void BuechiAutomata::initProperties(NetState &ns)
{
    //rep->message("INIT");
    for (int i = 0; i < cardAtomicPropositions; i++)
    {
        //rep->message("INIT %d",i);
        if (atomicPropositions[i]->initProperty(ns))
        {
            cardEnabled[atomicPropotions_backlist[i]]++;
            //rep->message("TRUE %d",cardEnabled[atomicPropotions_backlist[i]]);
        } //else
        //rep->message("FALSE");
    }
}

void BuechiAutomata::revertProperties(NetState &ns, index_t transition)
{
    ////rep->message("REVERT");
    for (int i = 0; i < cardStates; i++)
    {
        cardEnabled[i] = 0;
    }
    for (int i = 0; i < cardAtomicPropositions; i++)
        if (atomicPropositions[i]->updateProperty(ns, transition))
        {
            cardEnabled[atomicPropotions_backlist[i]] ++;
        }
}

bool BuechiAutomata::isAcceptingState(index_t state)
{
    return isStateAccepting[state];
}

index_t BuechiAutomata::getNumberOfStates()
{
    return cardStates;
}

BuechiAutomata::~BuechiAutomata()
{
    for (index_t i = 0; i < cardAtomicPropositions; i++)
    {
        delete atomicPropositions[i]->getPredicate();
        delete atomicPropositions[i];
    }
    free(atomicPropositions);

    for (index_t i = 0; i < cardStates; i++)
    {
        for (index_t j = 0; j < cardTransitions[i]; j++)
        {
            free(transitions[i][j]);
        }
        free(transitions[i]);
    }
    free(transitions);

    free(cardTransitions);
    free(isStateAccepting);
    free(atomicPropotions_backlist);
}


int current_next_string_index_number = 1;

char *produce_next_string(int *val)
{
    current_next_string_index_number++;
    int length = log10(current_next_string_index_number) + 2;
    char *buf = (char *) calloc(length, sizeof(char));
    sprintf(buf, "%d", current_next_string_index_number);
    *val = current_next_string_index_number;
    return buf;
}
