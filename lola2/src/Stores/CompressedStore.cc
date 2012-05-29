/*!
\author Max Görner
\file CompressedStore.cc
\status new
*/

#include <algorithm>
#include <iostream>
#include <cstring> //needed for memset
#include <cmath> //needed for ceil
#include <Net/Net.h>
#include <Net/Place.h>
#include <Net/Marking.h>
#include <Stores/CompressedStore.h>

CompressedStore::CompressedStore()
{
    for (int i = 0; i < 8; i++)
    {
        bits[i] = 1 << i;
    }

    //Testkonstruktor für sichere Netzwerke!!!
    code2Compress[0] = 0;
    code2Compress[1] = 1;
    code2Decompress[0] = 0;
    code2Decompress[1] = 1;

    //Zum Test auf Korrektheit:
    //BinStore2* bin2 = new BinStore2();
}

bool CompressedStore::searchAndInsert()
{
    //Zum Test auf Korrektheit:
    //bool korrekteAntwort = bin2.searchAndInsert();

    // organize vector as intermediate data structure: set size on first call
    static std::vector<capacity_t> m;
    static char* compressedVector;
    if (calls == 0)
    {
        m.resize(Place::CardSignificant);
    }
    ++calls;
    // copy current marking to vector
    std::copy(Marking::Current, Marking::Current + Place::CardSignificant, m.begin());

    // add vector to marking store
    if (!contains(m))
    {
        unsigned long int size = getSize(m);
        compressedVector = (char*) malloc(size);// wir verlassen uns darauf, dass der Speicherbereich genullt wird.
        memset(compressedVector, 0, size); //Speicher nullen, sicher ist sicher

        //Markierung komprimieren und in Liste einfügen
        compress(compressedVector, m);
        store.push_back(compressedVector);

        //Zählung aktualisieren
        for (std::vector<capacity_t>::iterator it = m.begin(); it != m.end(); it++)
        {
            countCount[*it]++;
        }
        ++markings;
        //assert(true);
        return false; //marking was new
    }
    //assert(korrekteAntwort);
    return true; //marking was old
}

bool CompressedStore::searchAndInsert(State**)
{
    assert(false);
    return false;
}

bool CompressedStore::contains(std::vector<capacity_t> elem)
{
    std::vector<capacity_t> temp;
    for (std::list<char*>::iterator it = store.begin(); it != store.end(); it++)
    {
        //Vektor dekomprimieren
        short bytePos = 0;
        char offset = 0;
        do
        {
            short onesCount = 0;
            while (*(*it + bytePos)&bits[offset])
            {
                onesCount++;
                if (offset == 7)
                {
                    bytePos++;
                    offset = 0;
                }
                else
                {
                    offset++;
                }
            }
            //anschließende 0 wegkonsumieren:
            if (offset == 7)
            {
                bytePos++;
                offset = 0;
            }
            else
            {
                offset++;
            }
            temp.push_back(code2Decompress[onesCount]);
        }
        while (temp.size() < Place::CardSignificant);

        if (temp == elem)
        {
            return true;
        }
    }
    return false;
}

unsigned long int CompressedStore::getSize(std::vector<capacity_t> elem)
{
    short size = 0;
    for (std::vector<capacity_t>::iterator it = elem.begin(); it != elem.end(); it++)
    {
        size += code2Compress[*it] + 1;
    }
    return ceil(size / 8.0);
}

void CompressedStore::compress(char* comprVec, std::vector<capacity_t> uncomprVec)
{
    short bytePos = 0;
    char offset = 0;
    for (std::vector<capacity_t>::iterator it = uncomprVec.begin(); it != uncomprVec.end(); it++)
    {
        for (short i = 0; i < code2Compress[*it]; i++)
        {
            //schreibe Einsen in den Speicher
            *(comprVec + bytePos) = *(comprVec + bytePos) | bits[offset];
            if (offset == 7)
            {
                bytePos++;
                offset = 0;
            }
            else
            {
                offset++;
            }
        }
        //Füge die abschließende Null an
        if (offset == 7)
        {
            bytePos++;
            offset = 0;
        }
        else
        {
            offset++;
        }
    }
}
