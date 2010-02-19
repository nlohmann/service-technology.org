#ifndef _OPERATINGGUIDELINE_H
#define _OPERATINGGUIDELINE_H

#include <config.h>
#include <map>
#include <set>
#include <string>
#include <sstream>
#include <iostream>
#include "types.h"
#include "BitSetC.h"
#include "Label.h"
#include "RBTreeStack.h"
#include "Service.h"
#include "Output.h"

class OGMarking {
    public:
	
			OGMarking(const std::map<label_id_t, std::pair<label_id_t, og_service_index_t> > &successors,
								unsigned has_FBit, unsigned has_SBit, unsigned has_TBit);
			~OGMarking();

			inline unsigned FBit() const { return this->mFBit; }
			inline unsigned SBit() const { return this->mSBit; }
			inline unsigned TBit() const { return this->mTBit; }
			inline label_index_t outDegree() const { return this->mOutDegree; }
			inline og_service_index_t successor(label_index_t pos) const {
				assert(pos < this->mOutDegree);
				return this->mSuccessors[pos];
			}
			inline label_id_t label(label_index_t pos) const {
				assert(pos < this->mOutDegree);
				return this->mLabels[pos];
			}
			std::string toString() const;

			BitSetC *mCheckedPaths;

    private:
		
			label_index_t mOutDegree;
			og_service_index_t *mSuccessors;
			label_id_t *mLabels;
			unsigned mFBit;
			unsigned mSBit;
			unsigned mTBit;			

};

class OperatingGuideline {
	public:
		OperatingGuideline(const std::map<og_service_index_t, OGMarking*> &markings,
												const std::set<label_id_t> &interface);
		~OperatingGuideline();

		void finalize();

		inline const OGMarking* marking(og_service_index_t pos) const {
			assert(pos < this->mSize);
			return this->mMarkings[pos];
		}
		void calculateBitSets(Label &interface);
		inline void clean() { 
			delete this->mInputInterface;
			delete this->mOutputInterface;
			this->mInputInterface = NULL;
			this->mOutputInterface = NULL;
			for (og_service_index_t i = 0; i < this->mSize; i++) {
				delete this->mMarkings[i]->mCheckedPaths;
				this->mMarkings[i]->mCheckedPaths = NULL;
			}
		}
		inline og_service_index_t size() const { return this->mSize; } 
		bool isSimulation(OperatingGuideline &B) const;
		bool isEquivalent(OperatingGuideline &B) const;
		bool isMatching(Service &C);
		std::string toString() const;

		BitSetC *mInputInterface;
		BitSetC *mOutputInterface;

	private:
		
		inline bool isStateMatching(og_service_index_t indexC, og_service_index_t indexB, Service &C) {
			if (this->marking(indexB)->TBit()) return true;
			if (!C.marking(indexC)->mEnabledTransitions->isIntersectionEmpty(this->mOutputInterface)) return true;
			if (this->marking(indexB)->FBit() && !C.marking(indexC)->isFinal()) return false;
			if (this->marking(indexB)->SBit()) return false;

			BitSetC allowed(*this->mInputInterface);
			allowed.makeDifference(C.marking(indexC)->mEnabledTransitions);
			if (isPathExisting(indexB, &allowed)) return false;

			return true;
		}

		inline bool isPathExisting(og_service_index_t indexB, BitSetC *allowed) {
			if (allowed->isSubset(this->marking(indexB)->mCheckedPaths)) return false;
			this->marking(indexB)->mCheckedPaths->makeUnion(allowed);

			RBTreeStack pTodo;

			for (label_index_t x = 0; x < this->marking(indexB)->outDegree(); x++) {
				if (allowed->getBit(this->marking(indexB)->label(x)))
					pTodo.add(this->marking(indexB)->successor(x), this->marking(indexB)->successor(x));
			}

			while (pTodo.pop(indexB, indexB)) {
				if (this->marking(indexB)->SBit() || this->marking(indexB)->FBit()) return true;
				if (allowed->isSubset(this->marking(indexB)->mCheckedPaths)) return false;
				this->marking(indexB)->mCheckedPaths->makeUnion(allowed);
				for (label_index_t x = 0; x < this->marking(indexB)->outDegree(); x++) {
					if (allowed->getBit(this->marking(indexB)->label(x)))
						pTodo.add(this->marking(indexB)->successor(x), this->marking(indexB)->successor(x));
				}
			}
			
			return false;
		}

		OGMarking **mMarkings;
		og_service_index_t mSize;
		std::set<label_id_t> mInterface;

};

#endif
