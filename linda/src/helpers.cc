#include "helpers.h"
#include "eventTerm.h"

int LindaHelpers::NR_OF_EVENTS; //!< The number of events in the analyzed open nets.
pnapi::Place** LindaHelpers::EVENT_PLACES; //!< The pointers to the interface places, inducing a global ID for each events: n-th element = n-th event.
std::string* LindaHelpers::EVENT_STRINGS; //!< The names of all events, stored in an array, using the global IDs.


int LindaHelpers::getEventID(std::string* s) {
	for (int i = 0; i < NR_OF_EVENTS; ++i) {
		if (EVENT_STRINGS[i] == *s) {
			return i;
		}
	}
	return -1;
}

void LindaHelpers::initialize() {
	NR_OF_EVENTS = 0;
	EVENT_STRINGS = 0;
	EVENT_PLACES = 0;
}

const std::string LindaHelpers::intToStr(const int x) {
	std::ostringstream o;
	if (!(o << x))
		return "ERROR";
	return o.str();
}


void FlowMatrix::computeTInvariants() {

	int firstPlace = net->getInterfacePlaces().size();
	int lastPlace = firstPlace + net->getInternalPlaces().size() - 1;

	for (int i = firstPlace; i <= lastPlace; ++i) {

	//	std::cerr << "New index is " << i << std::endl;

		ListElement<int*>* newRows = 0;
		ListElement<int*>* firstNew = 0;

		ListElement<int*>* current = root;
		while (current != 0) {
			ListElement<int*>* currentComp = current->next;

			int currNr = current->element[i];

			while (currentComp != 0) {
				int currCompNr = currentComp->element[i];
				if (currNr < 0 &&  currCompNr > 0 || currNr > 0 &&  currCompNr < 0) {
					int* newRow = new int[width];

					int fac = LindaHelpers::abs(currNr);
					int facComp = LindaHelpers::abs(currCompNr);


					for (int init = 0; init < width; ++init) {
						newRow[init] = facComp*current->element[init] + fac*currentComp->element[init];
					}
					ListElement<int*>* toInsert = new ListElement<int*>(newRow,newRows,0);

					if (newRows == 0) {
						firstNew = toInsert;
						newRows = toInsert;

					} else {
						newRows->next = toInsert;
					}

					newRows = toInsert;

	//				std::cerr << "Inserted row." << std::endl;

				}

				currentComp = currentComp->next;

			}

			current = current->next;
			}

	//	std::cerr << "deleting lines.." << std::endl;
		ListElement<int*>* current2 = root;
		while (current2 != 0) {

			if (current2->element[i] != 0) {
				if (current2 == root) {
					root = current2->next;
					if (current2->next != 0) {
						current2->prev = 0;
					} else {
						last = 0;
					}
					delete[] current2->element;
					delete current2;
					current2 = root;
				} else {
					ListElement<int*>* current3 = current2->next;
					current2->prev->next = current2->next;
					if (current2->next != 0) {
						current2->next->prev = current2->prev;
					} else {
						last = current2->prev;
					}
					delete[] current2->element;
					delete current2;
					current2 = current3;
				}
			} else {

				current2 = current2->next;
			}
		}


	//	std::cerr << "adding new lines.." << std::endl;
		if (newRows != 0) {
			if (root == 0) {
				root = firstNew;
				last = newRows;
			} else {
				firstNew->prev = last;
				last->next = firstNew;
				last = newRows;
			}
		}
	/*	std::cerr << "new matrix.." << std::endl;

		this->output();
*/


	}

	width = firstPlace;


}


void FlowMatrix::createTerms() {

	ListElement<EventTerm*>* currentInList = 0;
	ListElement<int*>* current = root;
	while (current != 0) {
			for (int i = 0; i < width-1; ++i) {
				if (current->element[i] > 0) {
					int e1 = i;
					int f1 = current->element[i];
					for (int j = i+1; j < width; ++j) {
						if (current->element[j] > 0) {
							int e2 = j;
							int f2 = -1*current->element[j];

							EventTerm* term = new AddTerm(new MultiplyTerm(new BasicTerm(e1),f1), new MultiplyTerm(new BasicTerm(e2),f2));

							ListElement<EventTerm*>* newElem = new ListElement<EventTerm*>(term,0,0);

							if (terms == 0) {
								terms = newElem;
								currentInList = newElem;
							} else {
								currentInList->next = newElem;
								newElem->prev = currentInList;
								currentInList = newElem;
							}

						}
					}
				}
			}
			current = current->next;
		}
}


void LindaHelpers::printAsciiArtImage(){
	std::cerr << "/==========================================================================\\" <<  std::endl;
	std::cerr << "|                                                                          |" <<  std::endl;
	std::cerr << "| .x++++++++++++x++?!!!+??```?`   .`,.;,j;. ````` .``?+!!!!!!!!?j?x+++++u; |" <<  std::endl;
	std::cerr << "| .b1??????+1c`1Z^^:``+.!`...`  .odMMx^JMMMx ````  .. `..^^^^^^^^^`,vc?;dr |" <<  std::endl;
	std::cerr << "| .b+=?zz=cz?J,7.^`^^.`,!...,   JJWMM.MMWMMb   ``` `...`..`,+.^^^^^^.`I+dr |" <<  std::endl;
	std::cerr << "| .b???1u?!?$!^^`^`:`?. `....` .MMMM#5.,.dMM . ```` `........ `.^^^^^^.1dr |" <<  std::endl;
	std::cerr << "| .b??+z?..`^`^^`:..?+?? ....  JMF...,!.,.MMF  ```` .............`..^^^.Jt |" <<  std::endl;
	std::cerr << "| .b+zj:!`^:^+^^:..!.`....... .JM$!5r J?!.7J   .```` `............ :+^^^J; |" <<  std::endl;
	std::cerr << "| .b+?2!``^:^+^..:.! `..^..^` . hi``,,,^,`M.n  .````` .`..........` ;.^^J: |" <<  std::endl;
	std::cerr << "| .b1jr.``^:^+:^!..?.`.`....`  .NM.`!,!?`.M.?,  ``.``  ....`.......``...J: |" <<  std::endl;
	std::cerr << "| .b??.^:`^:^+^`.,!,`.....^., `.MMN.dTY..MMM..  .`````. .  ........... !J: |" <<  std::endl;
	std::cerr << "| .Kv:^```^:^+:...`..``` ..` . JMMMMx..JMMMMNMa. ` ```  ```..`.......`..?: |" <<  std::endl;
	std::cerr << "| .b ^.`::^^^+^^:^?.......``....MMMMMMMMMMMMMM@4Ma  ``   .............`+J: |" <<  std::endl;
	std::cerr << "| .F,`.Y.^::^+^^:^::++ .`!.NNeFrMF.YWMMMMMYWMMMNxYh    ..............`.`J: |" <<  std::endl;
	std::cerr << "| .b!R/.:^^:^+^:^^^^+`.`..MMM#J.Mb ^,,?.?J..MMMMMaM5.``.................j; |" <<  std::endl;
	std::cerr << "| .L^v..:^^^^+^^::^.: .` JMMMNaMJMa,...`.` MMMMMMM@ / . .............. .+: |" <<  std::endl;
	std::cerr << "| .$!`:^:^::^+^^:^:? ..! i?MMMMFMM&^`? ! +.MMMMMMM$./ ...............``:J: |" <<  std::endl;
	std::cerr << "| .;:^^::^^:^+^:^^::;`.` Z.?MMMNMMF `+ `.`,MMMMMM@ ./................. :J: |" <<  std::endl;
	std::cerr << "| .l:^^^:^^^^+^^:^^^+...:0`.M8DJMM...pi...`MMMMMMF ?` ...............``:?: |" <<  std::endl;
	std::cerr << "| .l:^:::.::^+^:^::^+.`. t JMM Jbr ` ````  MMMMMMF.. .................. .: |" <<  std::endl;
	std::cerr << "| .l:^^^:^^:^+^^:^^:^+`,.3 JMN.MM `` `` ` JMMMMMM. `c...................J: |" <<  std::endl;
	std::cerr << "| .l:`^`.`^`^.^^`^`^`.`..` MMMMMF `` . ...MMMMMM$.!.F ..................j; |" <<  std::endl;
	std::cerr << "| .,``^```^`^`^^`^`^`^J!D. #WMMM6.  ...`.NMMMMMFJ. .J ................. ,/ |" <<  std::endl;
	std::cerr << "| .#c`^```^`^`:^`^`^^.rJ!. F.MMMM  .!.`.JMMMM#8 JJ..?,...............`..?/ |" <<  std::endl;
	std::cerr << "| .@.`^```^`^`^^`^`^^J J.`.F.MMM#  .` ..MMMMMh . #..!u ..........`..:!.^J: |" <<  std::endl;
	std::cerr << "| .bJ ^```^`^`^^`^`^J'.6Z.J `JMMb.....`gMMMMMM   W .:`b   .YYYp ,`^^`^^^J: |" <<  std::endl;
	std::cerr << "| .b+3,```^`^`:..^.?`.E?1j/`.JMMt. ..`JMMMMMMM   .r``:J.``F   M ``````.^J: |" <<  std::endl;
	std::cerr << "| .b+zzJ:`^:.`......:J1=:P.^`JMMF .!F!!!M!!!JF!!!Y7!TaJ7!!!   M@^!!!!?T,J: |" <<  std::endl;
	std::cerr << "| .b+z?+13,``+.`...` #11d ^..MMMb  .F   M   JF       JF       M        F.: |" <<  std::endl;
	std::cerr << "| .b+z?1+j2l``?: .` .++q^.^`JMMMJ.  F   M7774F   M   JF   F   Ma&&JF   F.: |" <<  std::endl;
	std::cerr << "| .8+zz!!+!`.`JhMx.:Jzu$.`./JMMMMMx.F   M   JF   M   JF   F   M^       F.: |" <<  std::endl;
	std::cerr << "| .t.:=.``^`..WMMb/.4?P..^, MMMMMMM#F   M   JF   M   JF   F   M`  .x   F.: |" <<  std::endl;
	std::cerr << "| .l:`^````i..JMMM.rJM,```:.MMMMMMMMF   M   JF   M   JF   !   M`   !   F.: |" <<  std::endl;
	std::cerr << "| .l`.^```^`^.JFMMJMM@ .,??.MNMMMMMMF   M   JF   M   Jb.      Mp       F.: |" <<  std::endl;
	std::cerr << "| .@JJ,`.`^`^.MM$jMMMb ^^^`MMMMMMFJ F   !!!!!MMMM&jjuJJMMMM0?x&,jj?&j?x,`: |" <<  std::endl;
	std::cerr << "| .b1,`^``^``JM4N#MMMF.....4MMMMMN&JF        MilJJ.b..x./i4..llh...h..;#.: |" <<  std::endl;
	std::cerr << "| .D2r!:``^`.JMJMbM#W ...!`JhF7MMMMMMMMMMMMMMMMMMN..MNpMMMF.....,!!...?:J: |" <<  std::endl;
	std::cerr << "| .J?.^+``^``MMFMJMMN....`..MMMMMNNMMMMMMMMMMMMMMM  MMNMMMM .^^^`.^^^^^^?: |" <<  std::endl;
	std::cerr << "| .l.`^```^. MMNMJMJMb`.` b7MMMMMMMMMM..MN.`..FJMF.`MMF4MMM!..^.`..`^...`: |" <<  std::endl;
	std::cerr << "| .F:`..``^`JMMMBJYuMMMx``4.MMM#MYT=MNMMMMMN..JMM#JhJMMN4MF ``.... `,.^`.; |" <<  std::endl;
	std::cerr << "| .b1O&0Zj..4WMMF? ..!TM 7.MF.,JM.R:?MMM#HMMMMbMMNMMMMMMM#  ........`..../ |" <<  std::endl;
	std::cerr << "| .bJ3. ?7c;5MMMF!`+:`..@u&JM .MMMa?:MMM..,MJhMMMMMM^JMMMr.`....`...`^.^J: |" <<  std::endl;
	std::cerr << "| .c`^^^+!../4JM5`+`2+;.HMMMMMMMMMMMMMMMMMMMMMMMMMM^^.WMNM .``.``^^^^`^:J: |" <<  std::endl;
	std::cerr << "| .bv.`..``Hb.MM `.!.``..WMMMMMMMMMMMMMMMWWWMMMMM# .``.MMM!,!```.`^^^^`^J; |" <<  std::endl;
	std::cerr << "| .b+z1I1zzv.M#Y `^`^`:^`JMMN.4MMMMMMMMME,.MMFsMM^... .JMMb....J..i``....r |" <<  std::endl;
	std::cerr << "| .N,??i???j!MF,+++1zCI..MMMMJJMWMMMMMMMMMMMMNMM# ``J..:W@#.b!!!!!!--!!!dr |" <<  std::endl;
	std::cerr << "| .WWWWWWW?,:W.11+++++++iTWWWWWWWWWWHWWHWWWWW3HW$?11++1+,WW8?11111111111?/ |" <<  std::endl;
	std::cerr << "|                                                                          |" <<  std::endl;
	std::cerr << "|==========================================================================|" <<  std::endl;
	std::cerr << "|--------------- http://service-technology.org/tools/linda ----------------|" <<  std::endl;
	std::cerr << "|---------------------- linda@service-technology.org ----------------------|" <<  std::endl;
	std::cerr << "\\==========================================================================/" <<  std::endl;
}
