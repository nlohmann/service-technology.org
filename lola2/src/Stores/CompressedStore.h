/*!
 \author Max Görner
 \file CompressedStore.cc
 \status new
 */

#pragma once
#include <vector>
#include <map>
#include <list>
#include "Core/Dimensions.h"
#include "Stores/Store.h"
//#include "Stores/BinStore2.h"

class CompressedStore: public Store {
	//Versuch, Speicherplatz durch Komprimierung zu gewinnen. Dafür werden die einzelnen Elemente das Vektors mittels Huffman-Codierung komprimiert. Mangels geeignetem, bitgenauen Datentyps wird alles in Char-"Arrays" gespeichert, was zu einen maximalen Verschnitt von 7 bit pro Vektor führt.
private:
	std::map<capacity_t, short> code2Compress; //takes a number and returns its amount of leading ones in the code. Note, that currently after a 1 always have to follow a 0.
	std::map<capacity_t,long> countCount; //Wer ein Wortspiel findet, darf es behalten. Wer einen Helden seiner Kindheit entdeckt, auch.
	std::map<short, capacity_t> code2Decompress; //takes an amount of leading 1 and returns the corresponding number
	std::list<char*> store;
	bool contains(std::vector<capacity_t>);
	unsigned long int getSize(std::vector<capacity_t>);
	void compress(char*, std::vector<capacity_t>);
	char bits[8];
	//Zum Test auf Korrektheit:
	//BinStore2* bin2;

public:
	CompressedStore();
	bool searchAndInsert();
	bool searchAndInsert(State**);
};
