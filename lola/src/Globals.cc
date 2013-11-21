#include "Globals.h"

Place** Globals::Places = NULL;
Transition** Globals::Transitions = NULL;

unsigned int Globals::BitVectorSize = 0;
unsigned int* Globals::CurrentMarking;

Place* Globals::CheckPlace = NULL;
Transition* Globals::CheckTransition = NULL;

char* Globals::lownetfile = NULL;
char* Globals::pnmlfile = NULL;
char* Globals::netfile = NULL;
char* Globals::analysefile = NULL;
char* Globals::graphfile = NULL;
char* Globals::pathfile = NULL;
char* Globals::statefile = NULL;
char* Globals::symmfile = NULL;
char* Globals::netbasename = NULL;
FILE* Globals::resultfile = NULL;

bool Globals::hflg = false;
bool Globals::Nflg = false;
bool Globals::nflg = false;
bool Globals::Aflg = false;
bool Globals::Sflg = false;
bool Globals::Yflg = false;
bool Globals::Pflg = false;
bool Globals::GMflg = false;
bool Globals::aflg = false;
bool Globals::sflg = false;
bool Globals::yflg = false;
bool Globals::pflg = false;
bool Globals::gmflg = false;
bool Globals::cflg = false;
char Globals::graphformat = '\0';

bool Globals::capflg = false;
int Globals::capacity = -1;
