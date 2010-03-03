#include "OperatingGuideline.h"

OGMarking::OGMarking(const std::map<label_id_t, std::pair<label_id_t, og_service_index_t> > &successors,
								unsigned has_FBit, unsigned has_SBit, unsigned has_TBit) {

	std::map<label_id_t, std::pair<label_id_t, og_service_index_t> >::const_iterator it;
	label_index_t i = 0;	

	this->mOutDegree = successors.size();
	this->mSuccessors = new og_service_index_t[this->mOutDegree];
	this->mLabels = new label_id_t[this->mOutDegree];

	for (it = successors.begin(); it != successors.end(); it++, i++) {
		this->mLabels[i] = it->first;
		this->mSuccessors[i] = it->second.second;
	}

	this->mFBit = has_FBit;
	this->mSBit = has_SBit;
	this->mTBit = has_TBit;

	this->mCheckedPaths = NULL;

}

OGMarking::~OGMarking() {
	delete[] this->mSuccessors;
	delete[] this->mLabels;
	delete this->mCheckedPaths;
}

std::string OGMarking::toString() const {
  std::stringstream tmpStr;
	tmpStr << std::endl;
	if (this->mSBit) {tmpStr << " S: 1";} else {tmpStr << " S: 0";}
	if (this->mFBit) {tmpStr << " F: 1";} else {tmpStr << " F: 0";}
	if (this->mTBit) {tmpStr << " T: 1";} else {tmpStr << " T: 0";}
	tmpStr << std::endl;
	if (this->mCheckedPaths != NULL) {
		tmpStr << " p(q): " << this->mCheckedPaths->toString() << std::endl;
	}
	for (label_index_t i = 0; i < this->mOutDegree; i++) {
		tmpStr << " " << static_cast<unsigned int>(this->mLabels[i]);
		tmpStr << " -> ";
		tmpStr << static_cast<unsigned int>(this->mSuccessors[i]) << std::endl;
	}
	return tmpStr.str();
}

///
///

OperatingGuideline::OperatingGuideline(const std::map<og_service_index_t, OGMarking*> &markings,
																				const std::set<label_id_t> &interface) {

	this->mMarkings = new OGMarking*[markings.size()];
	this->mSize = markings.size();
	
	std::map<og_service_index_t, OGMarking*>::const_iterator it;
	
	for (it = markings.begin(); it != markings.end(); it++) {
		this->mMarkings[it->first] = it->second;
	}

	std::set<label_id_t>::const_iterator it2;
	
	for (it2 = interface.begin(); it2 != interface.end(); it2++) {
		this->mInterface.insert((*it2));
	}

	this->mInputInterface = NULL;
	this->mOutputInterface = NULL;

}

OperatingGuideline::~OperatingGuideline() {}

void OperatingGuideline::finalize() {
  for (og_service_index_t i = 0; i < this->mSize; i++) {
		delete this->mMarkings[i];
	}
	delete[] this->mMarkings;
	delete this->mInputInterface;
	delete this->mOutputInterface;
	this->mInterface.clear();
  status("OperatingGuideline: deleted %d markings", this->mSize);
}

void OperatingGuideline::calculateBitSets(Label &interface) {
	this->mInputInterface = new BitSetC(interface.size(), true);
	this->mOutputInterface = new BitSetC(interface.size(), true);

	std::set<label_id_t>::const_iterator it;	
	std::string label;

	for (it = this->mInterface.begin(); it != this->mInterface.end(); it++) {
		assert(interface.ID2Name((*it), label));
		if (label[0] == '!') {
			this->mOutputInterface->setBit((*it));
		}
		else {
			this->mInputInterface->setBit((*it));
		}
	}

	for (og_service_index_t i = 0; i < this->mSize; i++) {
		this->mMarkings[i]->mCheckedPaths = new BitSetC(interface.size(), false);
		for (label_index_t j = 0; j < this->mMarkings[i]->outDegree(); j++) {
			this->mMarkings[i]->mCheckedPaths->clearBit(this->mMarkings[i]->label(j));
		}
		this->mMarkings[i]->mCheckedPaths->makeUnion(this->mInputInterface);
	}

}

bool OperatingGuideline::isSimulation(OperatingGuideline &B) const {
	RBTreeStack todo;
	todo.add(0, 0);
	
	og_service_index_t indexA, indexB;
	label_index_t y;

	while (todo.pop(indexA, indexB)) {	
		if ((this->marking(indexA)->TBit() && !B.marking(indexB)->TBit()) || 
				(B.marking(indexB)->SBit() && !this->marking(indexA)->SBit()) ||
				(B.marking(indexB)->FBit() && !this->marking(indexA)->FBit() && !this->marking(indexA)->SBit())) {
			status("bit compare failed in pair A[%d], B[%d]", indexA, indexB);
			status("compared %d pairs of states", todo.size());
			return false;
		}
		if (this->marking(indexA)->outDegree() > B.marking(indexB)->outDegree()) {
			status("too few transitions in pair A[%d], B[%d]", indexA, indexB);
			status("compared %d pairs of states", todo.size());
			return false;
		}
		y = 0;
		for (label_index_t x = 0; x < this->marking(indexA)->outDegree(); x++) {
			while (y < B.marking(indexB)->outDegree()) {
				if (B.marking(indexB)->label(y) >= this->marking(indexA)->label(x)) break;
				y++;
			}
			if (B.marking(indexB)->label(y) == this->marking(indexA)->label(x)) {
				todo.add(this->marking(indexA)->successor(x), B.marking(indexB)->successor(y));
			}
			else {
				status("transition %d is missing in pair A[%d], B[%d]", B.marking(indexB)->label(y), indexA, indexB);
				status("compared %d pairs of states", todo.size());		
				return false;
			}
		}
	}
	status("compared %d pairs of states", todo.size());
	return true;
}

bool OperatingGuideline::isEquivalent(OperatingGuideline &B) const {
	RBTreeStack todo;
	todo.add(0, 0);
	
	og_service_index_t indexA, indexB;

	while (todo.pop(indexA, indexB)) {
		if ((this->marking(indexA)->SBit() != B.marking(indexB)->SBit()) || 
				(this->marking(indexA)->FBit() != B.marking(indexB)->FBit()) ||
				(this->marking(indexA)->TBit() != B.marking(indexB)->TBit())) {
			status("bit compare failed in pair A[%d], B[%d]", indexA, indexB);
			status("compared %d pairs of states", todo.size());
			return false;
		}
		if (this->marking(indexA)->outDegree() != B.marking(indexB)->outDegree()) {
			status("too few transitions in pair A[%d], B[%d]", indexA, indexB);
			status("compared %d pairs of states", todo.size());			
			return false;
		}
		for (label_index_t x = 0; x < this->marking(indexA)->outDegree(); x++) {
			if (this->marking(indexA)->label(x) != B.marking(indexB)->label(x)) {
				status("transition %d is missing in pair A[%d], B[%d]", B.marking(indexB)->label(x), indexA, indexB);
				status("compared %d pairs of states", todo.size());
				return false;
			}
			todo.add(this->marking(indexA)->successor(x), B.marking(indexB)->successor(x));
		}
	}
	status("compared %d pairs of states", todo.size());
	return true;
}

bool OperatingGuideline::isMatching(Service &C) {
	RBTreeStack todo;
	todo.add(0, 0);
	
	og_service_index_t indexB, indexC;
	label_index_t y;

	while (todo.pop(indexC, indexB)) {
		if (!isStateMatching(indexC, indexB, C)) {
			status("state match failed in pair A[%d], C[%d]", indexB, indexC);
			status("compared %d pairs of states", todo.size());
			return false;
		}		
		y = 0;
		for (label_index_t x = 0; x < C.marking(indexC)->outDegree(); x++) {
			if (C.marking(indexC)->label(x) == 0) {
				todo.add(C.marking(indexC)->successor(x), indexB);
			}
			else {
				while (y < this->marking(indexB)->outDegree()) {
					if (this->marking(indexB)->label(y) >= C.marking(indexC)->label(x)) break;
					y++;
				}
				if (this->marking(indexB)->label(y) == C.marking(indexC)->label(x)) {
					todo.add(C.marking(indexC)->successor(x), this->marking(indexB)->successor(y));
				}
				else {
					status("transition %d is missing in pair A[%d], C[%d]", C.marking(indexC)->label(x), indexB, indexC);
					status("compared %d pairs of states", todo.size());						
					return false;
				}
			}
		}
	}
	status("compared %d pairs of states", todo.size());
	return true;
}

std::string OperatingGuideline::toString() const {
  std::stringstream tmpStr;
	tmpStr << std::endl << "Nodes: " << this->mSize << std::endl;
	if (this->mInputInterface != NULL) {
		tmpStr << " I: " << this->mInputInterface->toString() << std::endl;
	}
	if (this->mOutputInterface != NULL) {
		tmpStr << " O: " << this->mOutputInterface->toString() << std::endl;
	}
  for (og_service_index_t i = 0; i < this->mSize; i++) {
		tmpStr << "Node " << i << ": ";
		tmpStr << this->mMarkings[i]->toString();
	}	
	return tmpStr.str();
}
