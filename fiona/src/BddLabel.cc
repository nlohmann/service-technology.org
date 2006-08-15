#include "BddLabel.h"
#include <string>

BddLabel::BddLabel(char * c, int n, BddLabelTab * table){
	name = c;
	nbr = n;	
	next = (BddLabel *) 0;
	table->add(this);
}

BddLabel::~BddLabel()
{}

BddLabelTab::BddLabelTab(unsigned int s){
	size = s;
	table = new BddLabel * [s];
	card = 0;
	for(unsigned int i = 0; i < s;i++){
		table[i] = (BddLabel *) 0;
	}
}

BddLabelTab::~BddLabelTab(){
	BddLabel * temp;
	for(unsigned int i = 0;i < size;i++){
		while(table[i]){
			temp = table[i];
			//std::cout << table[i]->name << "  ";
			table[i] = table[i] -> next;
			delete temp;
		}	
	}
	delete [] table;
}

	
BddLabel * BddLabelTab::lookup(char * name){
  	/* 1. Hashvalue bestimmen */
  	unsigned int h,i;
  	BddLabel * lst = (BddLabel *) 0;
  	h = 0;
  	for(i = 0; i < strlen(name); i++){
      	h += (int) name[i];
      	h %= size;
   	}
  	/* 2. suchen */
  	for(lst = table[h]; lst; lst = lst -> next){
  		if(!strcmp(lst->name, name)){
	   		break;
	 	}
    }
  	return lst;
}
  
void BddLabelTab::add(BddLabel * s){
  	/* 1. Hashvalue bestimmen */
  	unsigned int h,i;
  	h = 0;
  	for(i = 0; i < strlen(s->name); i++){
      	h += (int) s->name[i];
      	h %= size;
    }
  	/* 2. eintragen */
  	s->next = table[h];
  	table[h] = s;
  	card++;
}
