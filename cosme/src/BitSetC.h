#ifndef __BITSETC_H
#define __BITSETC_H

#include <config.h>
#include <limits>
#include <string>
#include <sstream>

#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif

#define bitset_base_t uint32_t
#define BITSET_BASE_T_BITS 32
#define BITSET_BASE_T_MAX  0xffffffff
#define BITSET_BASE_T_MIN  0x0

#define BITMASK(b) (1 << ((b) % BITSET_BASE_T_BITS))
#define BITSLOT(b) ((b) / BITSET_BASE_T_BITS)
#define BITSET(a, b) ((a)[BITSLOT(b)] |= BITMASK(b))
#define BITCLEAR(a, b) ((a)[BITSLOT(b)] &= ~BITMASK(b))
#define BITTEST(a, b) ((a)[BITSLOT(b)] & BITMASK(b))
#define BITNSLOTS(nb) ((nb + BITSET_BASE_T_BITS - 1) / BITSET_BASE_T_BITS)

class BitSetC {
    public:

        BitSetC(size_t bits, bool makeEmpty);
        BitSetC(const BitSetC& other);
        ~BitSetC();

        void makeUnion(BitSetC* a);
        void makeIntersection(BitSetC* a);
        void makeDifference(BitSetC* a);
        inline bool getBit(size_t bit) const {
            assert(bit < this->mBitCount);
            return BITTEST(this->mBits, bit);
        }
        inline void setBit(size_t bit) {
            assert(bit < this->mBitCount);
            BITSET(this->mBits, bit);
        }
        inline void clearBit(size_t bit) {
            assert(bit < this->mBitCount);
            BITCLEAR(this->mBits, bit);
        }
        bool isIntersectionEmpty(BitSetC* a) const;
        bool isSubset(BitSetC* a) const;
        inline bool isEmpty() const {
            for (size_t i = 0; i < BITNSLOTS(this->mBitCount); i++) {
                if (this->mBits[i] != 0) {
                    return false;
                }
            }
            return true;
        }
        bool isEqual(BitSetC* a) const;
        inline size_t bitCount() const {
            return this->mBitCount;
        }
        inline bitset_base_t getSlot(size_t slot) const {
            assert(slot <= BITSLOT(this->mBitCount));
            return this->mBits[slot];
        }
        std::string toString() const;

    private:

        bitset_base_t* mBits;
        size_t mBitCount;

};

#endif //__BITSETC_H
