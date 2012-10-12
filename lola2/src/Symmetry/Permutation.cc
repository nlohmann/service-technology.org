
#include <Net/Net.h>

#include <Symmetry/Permutation.h>
#include <Symmetry/PartitionRefinement.h>


Bijection Bijection::compose(Bijection f) {
    assert(f.size == size);
    index_t *mapping = new index_t[size];
    for(index_t i = 0; i < size; i++) mapping[i] = map[f.map[i]];
    return Bijection(size, mapping);
}

Bijection Bijection::inverse() {
    index_t *mapping = new index_t[size];
    for(index_t i = 0; i < size; i++) mapping[map[i]] = i;
    return Bijection(size, mapping);
}

Bijection Bijection::power(int n) {
    index_t *mapping = new index_t[size];
  
    assert(n > 0); // just for now!
    if(n > 0) {
	for(index_t i = 0; i < size; i++) {
	    mapping[i] = i;
	    for(int j = 0; j < n; j++) mapping[i] = map[mapping[i]];
	}
    }
  
    return Bijection(size, mapping);
}

index_t Bijection::stabilizes() {
    for(index_t i = 0; i < size; i++) if(map[i] != i) return i;
    return size;
}

void Bijection::show() {
    bool *touched = new bool[size]; memset(touched, false, sizeof(bool) * size);
    
    for(index_t i = 0; i < size; i++) {
	if(touched[i]) continue;
	touched[i] = true; 
	if(map[i] == i) continue;
    
	index_t ix = i;
	node_t node = (ix < Net::Card[PL] ? PL : TR);
	if(node == TR) ix -= Net::Card[PL];
	std::cout << "(" << Net::Name[node][ix];
    
	for(index_t current = map[i]; !touched[current]; touched[current] = true, current = map[current]) {
	    ix = current; node = (ix < Net::Card[PL] ? PL : TR);
	    if(node == TR) ix -= Net::Card[PL];
	    std::cout << " " << Net::Name[node][ix];
	}
	std::cout << ")\n";
    }
    
    delete[] touched;
}
