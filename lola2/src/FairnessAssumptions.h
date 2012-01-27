/*!
\author Karsten
\file FairnessAssumptions.h
\status approved 25.01.2012
\ingroup g_frontend g_symboltable

\todo Rename tFairnessAssumption to fairnessAssumption_t

This is just a type definition for the possible fairness assumptions that can
be specified for each transition. We consider weak and strong fairness as well
as the absence of fairness.
*/

#pragma once

typedef enum
{
    NO_FAIRNESS,
    WEAK_FAIRNESS,
    STRONG_FAIRNESS
}
tFairnessAssumption;
