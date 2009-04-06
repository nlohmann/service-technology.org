#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>
#include <ctime>

#include "InnerMarking.h"
#include "Knowledge.h"
#include "StoredKnowledge.h"
#include "Label.h"

using std::cerr;
using std::endl;

extern FILE *graph_in;
extern int graph_parse();

/*!
 \note possible optimization: don't create a copy for the last label but use
       the object itself
 */
void calcRecursive(Knowledge *K, StoredKnowledge *SK) {
    assert(K);
    assert(SK);
    static unsigned int calls = 0;
    
//    cerr << *K << endl;
    
    if (++calls % 10000 == 0) {
        fprintf(stderr, "%8d knowledges\n", StoredKnowledge::storedKnowledges);
    }
    
    for (Label_ID l = Label::first_receive; l <= Label::last_sync; ++l) {
        Knowledge *K_new = new Knowledge(K, l);
        assert(K_new);
        
        // only process knowledges within the message bounds
        if (K_new->is_sane) {
            // create a compact version of the knowledge bubble
            StoredKnowledge *SK_new = new StoredKnowledge(K_new);
            assert(SK_new);
            
            // add it to the knowledge tree
            StoredKnowledge *SK_store = SK_new->store();
            assert(SK_store);
            
            // store an edge from the parent to this node
            SK->addSuccessor(l, SK_store);
            
            // if the node was new, check its successors
            if (SK_store == SK_new) {
                calcRecursive(K_new, SK_store);
            } else {
                delete SK_new;
            }            
        }
        // we saw K_new's successors
        delete K_new;
    }
}

int main() {
    time_t start_time, end_time;
    
    // 1. parse oWFN
    try {
        std::cin >> pnapi::io::owfn >> *(InnerMarking::net);
        std::cerr << PACKAGE << ": read net " << pnapi::io::stat << *(InnerMarking::net) << std::endl;
    } catch (pnapi::io::InputError error) {
        cerr << PACKAGE << error << endl;
        exit(EXIT_FAILURE);
    }
    // only normal nets are supported so far
    if (!InnerMarking::net->isNormal()) {
        fprintf(stderr, "%s: the input open net must be normal -- aborting\n", PACKAGE);
        exit (EXIT_FAILURE);
    }


    // 2. initialize labels and interface markings
    Label::initialize();
    InterfaceMarking::initialize(1); // message bound


    // 3. convert oWFN to LoLA and write a file
    std::ofstream lolaFile;
    lolaFile.open("tmp.lola", std::ofstream::out | std::ofstream::trunc);
    lolaFile << pnapi::io::lola << *(InnerMarking::net);
    lolaFile.close();
    

    // 4. call LoLA and parse reachability graph
    time(&start_time);

/*
    system("lola-full tmp.lola -m &> /dev/null");
    graph_in = fopen("tmp.graph", "r");
    graph_parse();
    fclose(graph_in);
    system("rm -f tmp.graph tmp.lola");
*/

    // 4. call LoLA and parse reachability graph using a pipe
    graph_in = popen("lola-full tmp.lola -M 2> /dev/null", "r");
    graph_parse();
    pclose(graph_in);
    system("rm -f tmp.lola");
    time(&end_time);


    // 5. organize reachability graph
    InnerMarking::initialize();
    fprintf(stderr, " [%.0f sec]\n", difftime(end_time, start_time));
    delete InnerMarking::net;


    // 6. construct Knowledge recursively
    time(&start_time);
    Knowledge *K0 = new Knowledge(0);
    assert(K0);
    StoredKnowledge *SK0 = new StoredKnowledge(K0);
    assert(SK0);
    SK0->store();

    calcRecursive(K0, SK0);
    delete K0;
    time(&end_time);
    
    fprintf(stderr, "%s: stored %d knowledges [%.0f sec]\n",
        PACKAGE, StoredKnowledge::storedKnowledges, difftime(end_time, start_time));
    fprintf(stderr, "%s: used %d of %d hash buckets, maximal bucket size: %d\n",
        PACKAGE, StoredKnowledge::hashTree.size(), (1 << (8*sizeof(hash_t))), StoredKnowledge::maxBucketSize);
    
//    cerr << "  Knowledge: " << Knowledge::memory_count << " stored, " << Knowledge::memory_max << " maximal" << endl;
//    cerr << "  StoredKnowledge: " << StoredKnowledge::memory_count << " stored, " << StoredKnowledge::memory_max << " maximal -- " << StoredKnowledge::entries_count << " markings stored" << endl;
//    cerr << "  InterfaceMarking: " << InterfaceMarking::memory_count << " stored, " << InterfaceMarking::memory_max << " maximal -- " << InterfaceMarking::memory_count - StoredKnowledge::entries_count << " not deleted" << endl;


    // 7. add predecessor relation
    time(&start_time);
    unsigned int edges = StoredKnowledge::addPredecessors();
    time(&end_time);
    fprintf(stderr, "%s: added predecessor relation (%d edges) [%.0f sec]\n",
        PACKAGE, edges, difftime(end_time, start_time));


    // 8. delete insane nodes
    // 9. add formulas
    
    return EXIT_SUCCESS;
}
