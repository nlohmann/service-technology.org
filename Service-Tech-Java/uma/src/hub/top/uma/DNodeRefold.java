package hub.top.uma;

import java.awt.List;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.HashMap;
import java.util.LinkedHashSet;
import java.util.LinkedList;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Set;

import com.google.gwt.dev.util.collect.HashSet;

import hub.top.uma.synthesis.EquivalenceRefineLabel;
import hub.top.uma.synthesis.EquivalenceRefineSuccessor;
import hub.top.uma.synthesis.IEquivalentNodesRefine;

public class DNodeRefold extends DNodeBP {

  public DNodeRefold(DNodeSys system, Options options) {
    super(system, options);
  }
  
  /**
   * Remove all nodes from the branching process that are not part of
   * its minimal complete finite prefix. 
   */
  public void minimizeFoldingRelation() {

    //System.out.println("minimize()");

    // reference locally
    final Map<DNode, DNode> elementary_ccPair = futureEquivalence().getElementary_ccPair();
    
    // run a breadth-first search from the maximal nodes of the branching
    // process to its initial nodes, for each node that has no equivalent
    // smaller node yet, seek an equivalent image node,  
    LinkedList<DNode> queue = new LinkedList<DNode>();
    queue.addAll(bp.getCurrentMaxNodes());
    
    while (!queue.isEmpty()) {
      DNode d = queue.removeFirst();
      DNode e = elementary_ccPair.get(d);
      
      if (e == null) {   // d has no equivalent partner yet, see if there is one

        if (d.post == null || d.post.length == 0) {
          // d has no successor, search in all maximal nodes for an equivalent node
          for (DNode d_img : bp.getCurrentMaxNodes()) {
            if (d_img == d) continue;
            if (d_img.id != d.id) continue;
            // do not map normal nodes to non-existing nodes
            if (d_img.isAnti != d.isAnti) continue; 
            
            // d and d0 are nodes with the same ID and without successor
            if (elementary_ccPair.get(d_img) != null) continue;
            // d0 also has no equivalent node, e.g. is not a cut-off node
            // then d and d0 are equivalent
            
            //System.out.println(d+" --> "+d_img+" (max nodes)");
            elementary_ccPair.put(d, d_img);
            d.isCutOff = true;
          }
          continue;
          // TODO: shouldn't we add 'd's successors to the queue?
        }
        
        // 'd' has successor nodes, an equivalent node 'd_img' must have the
        // corresponding equivalent successors, we find 'd_img' by searching in
        // the equivalent nodes of 'd's successors for a node with the same id
        // as the node 'd'
        DNode d_img = null;
        
        // see if all successors of 'd0' have an equivalent node
        // and whether the equivalent nodes have all the same predecessor
        
        // for each successor 'd.post' of 'd'
        // find the equivalent image node 'dPost_img'
        DNode[] dPost_img = new DNode[d.post.length];
        for (int i=0; i<d.post.length; i++) {
          dPost_img[i] = elementary_ccPair.get(d.post[i]);
        }
        
        // then see if all images of the successors of 'd' have a common
        // predecessor
        
        // there is no image of the first successor of 'd': we cannot minimize
        if (dPost_img[0] == null)
          continue;  // no equivalent partner of the first successor

        // find the predecessor of the image of 'd's first successor  that has
        // same id (name) as DNode 'd', this predecessor should be 'd_img'
        //System.out.print("checking 0: "+dPost_img[0]+"...");
        for (int j=0; j<dPost_img[0].pre.length; j++)
          if (dPost_img[0].pre[j].id == d.id) {
            d_img = dPost_img[0].pre[j];
            break;
          }
        //System.out.println(" found "+d_img);
        
        // the image of the first successor of 'd' has no predecessor with
        // the same id as 'd', there is no node we can make equivalent to 'd'
        if (d_img == null) continue;  // no match :(
        
        // the found image has a different number of successors,
        // they cannot be equivalent
        if (d_img.post.length != d.post.length) continue;
        
        // now check whether each image of all successors of 'd' also have
        // 'd_img' as their predecessor
        boolean notFound = false;
        for (int i=1; i<dPost_img.length; i++) {
          boolean foundPreFor_i = false;
          //System.out.print("checking "+i+": "+dPost_img[i]+"...");
          if (dPost_img[i] != null)
            for (int j=0; j<dPost_img[i].pre.length; j++) {
              if (dPost_img[i].pre[j] == d_img) {
                // the i-th successor has 'd_img' as predecessor
                //System.out.println(" found again");
                foundPreFor_i = true; break; 
              }
            }
          if (!foundPreFor_i) {
            //System.out.println(" found not");
            // if the ith successor does not have 'd_img' as predecessor,
            // then we have no node we can make equivalent to 'd' 
            notFound = true;
            break;
          }
        }
        
        if (notFound)
          continue;  // no match :(
        
        //System.out.println(d+" --> "+d_img+" (successor)");
        elementary_ccPair.put(d, d_img);
        d.isCutOff = true;
        // extended equivalence relation by 'd' ~ 'd_img' 
      }
      // and continue with all predecessors of 'd'
      for (DNode dPre : d.pre)
        if (!queue.contains(dPre)) queue.addLast(dPre);
    } // END: search all nodes of the branching process 
    
    HashSet<DNode> seen = new HashSet<DNode>();
    queue.addAll(bp.initialConditions);
    while (!queue.isEmpty()) {
      DNode d = queue.removeFirst();
      seen.add(d);
      
      if (!d.isCutOff && (d.pre.length > 0)) {
        boolean allPreAreCutOff = true;
        for (DNode dPre : d.pre) {
          if (!dPre.isCutOff) { allPreAreCutOff = false; break; }
        }
        if (allPreAreCutOff) {
          // all predecessors are marked as cutOff, but this one not
          // this node actually resides beyond the minimal prefix (there
          // is some other configuration that is a cut-off configuration and
          // this event extends the other configuration)
          
          // so map this node to the corresponding node
          // take any predecessor of d, get its corresponding equivalent 
          // node 'preEquiv' and map 'd' to the corresponding successor
          // of 'preEquiv'
          DNode preEquiv = elementary_ccPair.get(d.pre[0]);
          for (DNode d0 : preEquiv.post) {
            if (d0.id == d.id) {
              //System.out.println(d+" --> "+d0+" (initial)");
              d.isCutOff = true;
              setCutOffEvent(d, d0);
              break;
            }
          }
          // done, continue with d's successors iteratively
        }
      }
    
      // add all successors of d of which all predecessors have already
      // been visited
      if (d.post != null)
        for (DNode dPost : d.post) {
          boolean seenAll = true;
          for (DNode dPostPre : dPost.pre)
            if (!seen.contains(dPostPre)) { seenAll = false; break; }
          if (seenAll) {
            queue.addLast(dPost);
          }
        }
    }
  }
  
  
  /**
   * Extend the {@link #foldingEquivalenceClass}es by exploring the unfolding forward
   * and making nodes with equivalent pre-sets and equal labels equivalent until no
   * more change in the equivalence relation is made.
   */
  private void extendFutureEquivalence_forward () {
    
    boolean extended = true;
    while (extended) {
      extended = false;
      
      LinkedList<HashSet<DNode>> newClasses = new LinkedList<HashSet<DNode>>();
      
      // extend folding equivalence for post-conditions of equivalent events
      for (Entry<DNode, Set<DNode>> cl : futureEquivalence().entrySet()) {
        if (!cl.getKey().isEvent) continue;
        
        DNode e = cl.getKey();
        if (e.isAnti) continue; // do not do anything for anti-events
        if (e.post == null) continue;
        for (int i=0; i<e.post.length; i++) {
          
          HashSet<DNode> newEquivalenceClass = new HashSet<DNode>();

          for (DNode e2 : cl.getValue()) {
            if (e2.isAnti) continue; // do not do anything for anti-events
            
            if (!futureEquivalence().areFoldingEquivalent(e.post[i], e2.post[i])) {
              //System.out.println("extending: post-condition "+e.post[i]+" of "+e+" and "+e2.post[i]+" of "+e2+" are not equivalent yet");
              extended = true;
            } else {
              for (DNode b : futureEquivalence().get(futureEquivalence().getElementary_ccPair().get(e2.post[i]))) {
                if (!futureEquivalence().areFoldingEquivalent(b, e2.post[i])) {
                  System.out.println("NOT extending: post-conditions "+i+" of "+e+" and "+e2+" are equivalent");
                  System.out.println("   but "+b+" ~ "+e2.post[i]+" are not");
                }
              }
            }
            newEquivalenceClass.add(e2.post[i]);
          }
          newClasses.addLast(newEquivalenceClass);
        }
      }
      
      for (HashSet<DNode> newClass : newClasses)
        futureEquivalence().joinEquivalenceClasses(newClass);
      
      // extend folding equivalence for post-events of equivalent conditions
      newClasses.clear();
      
      // we group all events if they have equivalent pre-conditions
      // this map stores the sets of events that have equivalent pre-conditions
      HashMap<DNode[], HashSet<DNode>> preSetClasses = new HashMap<DNode[], HashSet<DNode>>();
      for (Entry<DNode, Set<DNode>> cl : futureEquivalence().entrySet()) {
        
        // compare events
        if (!cl.getKey().isEvent) continue;
        // check each event of an equivalence class
        for (DNode e : cl.getValue()) {
          if (e.isAnti) continue; // do not do anything for anti-events
          
          // determine the pre-set in 'preSetClasses' that equivalently represents
          // the pre-set of 'e'
          DNode[] e_preSet_equiv = null;
          for (DNode[] preSet : preSetClasses.keySet()) {
            if (preSet.length != e.pre.length) continue;
            
            int i=0;
            for (; i < e.pre.length; i++) {
              if (!futureEquivalence().areFoldingEquivalent(e.pre[i], preSet[i]))
                break;
            }
            if (i == e.pre.length) {
              // each pre-decessor of 'e' is equivalent to the corresponding predecessor
              // of 'preSet'
              e_preSet_equiv = preSet;
            }
          }
          
          if (e_preSet_equiv == null) {
            // equivalence class of the pre-set of 'e' is not represented yet
            //  in 'preSetClasses', create a new equivalence class
            e_preSet_equiv = e.pre;
            preSetClasses.put(e_preSet_equiv, new HashSet<DNode>());
          }
          // 'd' belongs to the subequivalence class of its pre-set
          preSetClasses.get(e_preSet_equiv).add(e);
        }
      }

      for (HashSet<DNode> cl : preSetClasses.values()) {
        // all events in 'cl' have a pre-set from the same equivalence class
        
        // split each class by labels
        HashMap<Short, HashSet<DNode>> cl_lab = new HashMap<Short, HashSet<DNode>>();
        for (DNode e : cl) {
          if (!cl_lab.containsKey(e.id))
            cl_lab.put(e.id, new HashSet<DNode>());
          cl_lab.get(e.id).add(e);
        }
        
        for (HashSet<DNode> sub_cl : cl_lab.values()) {
          if (!futureEquivalence().areFoldingEquivalenceClass(sub_cl)) {
            //System.out.println("extending folding equialence to "+sub_cl);
            newClasses.addLast(sub_cl);
            extended = true;
          }
        }
      }
      
      for (HashSet<DNode> newClass : newClasses)
        futureEquivalence().joinEquivalenceClasses(newClass);
    }
  }
  
  //public static float foldThreshold = .1f;
  //public static boolean ignoreFoldThreshold = true;
  
  /**
   * Put all nodes with the same {@link DNode#id} that have no successor
   * into the same equivalence class.
   */
  public void extendFutureEquivalence_maximal() {
    
    int idFrequency[] = new int[dNodeAS.currentNameID];  //
    for (DNode d : bp.getAllNodes()) {                   //
      idFrequency[d.id]++;                               //
    }
    
    //float foldThreshold = 0.05f;                          //

    boolean changed = true;
    while (changed) {
      
      changed = false;
      
      ArrayList<DNode> maximalNodes = new ArrayList<DNode>();
      
      for (Entry<DNode, Set<DNode>> cl : futureEquivalence().entrySet()) {
        boolean allMaximal = true;
        for (DNode d : cl.getValue()) {
          if (d.post != null && d.post.length > 0) {
            allMaximal = false;
            break;
          }
        }
        if (allMaximal) {
          
          //if (ignoreFoldThreshold || cl.getValue().size() < idFrequency[cl.getKey().id] * foldThreshold)  //
             maximalNodes.add(cl.getKey());
        }
      }
      
      for (int i=0; i<maximalNodes.size(); i++) {
        DNode d = maximalNodes.get(i);
        LinkedList<DNode> dEquiv = new LinkedList<DNode>();
        
        dEquiv.add(d);
        int equivSize = futureEquivalence().get(d).size();                   //
        
        for (int j=i+1; j<maximalNodes.size(); j++) {
          DNode e = maximalNodes.get(j);
          if (d.id == e.id) {
            dEquiv.add(e);
            
            equivSize += futureEquivalence().get(e).size();                  //
            //if (!ignoreFoldThreshold && equivSize >= idFrequency[d.id] * foldThreshold) break;             //
          }
        }
        
        if (dEquiv.size() > 1) {
          futureEquivalence().joinEquivalenceClasses(dEquiv);
          changed = true;
          break;
        }
      }
    }
  }
  
  public boolean extendFutureEquivalence_backwards() {
    boolean changed = false;
    
    int idFrequency[] = new int[dNodeAS.currentNameID];  //
    for (DNode d : bp.getAllNodes()) {                   //
      idFrequency[d.id]++;                               //
    }
    //float foldThreshold = .1f;                          //
    
    LinkedHashSet<DNode> top = new LinkedHashSet<DNode>();
    for (DNode d : bp.getAllNodes()) {
      if (d.isAnti) continue;
      if ((d.post == null || d.post.length == 0) && d.pre != null) {
        for (DNode e : d.pre)
          top.add(e);
      }
    }

    LinkedList<DNode> to_join = new LinkedList<DNode>();
    LinkedList<DNode []> joinedEvents = new LinkedList<DNode[]>();
    HashSet<DNode> dSucc = new HashSet<DNode>();
    HashSet<DNode> d2Succ = new HashSet<DNode>();
    
    // reference locally
    final Map<DNode, DNode> elementary_ccPair = futureEquivalence().getElementary_ccPair();

    //Uma.out.println("top: "+top);
    
    int print_count = 1;
    while (!top.isEmpty()) {
      
      if ((print_count % 100) == 0) Uma.out.print(top.size()+" ");
      if (print_count++ > 1000) {
        Uma.out.print("\n");
        print_count = 0;
      }
      
      //Uma.out.println("top: "+top);
      DNode d = top.iterator().next();
      top.remove(d);
      Uma.out.println("removed "+d);

      to_join.clear();
      to_join.add(d);

      Set<DNode> dCl = futureEquivalence().get(elementary_ccPair.get(d));

      //if (!ignoreFoldThreshold && dCl.size() > idFrequency[d.id] * foldThreshold) continue;     //
      
      // collect the for each successor 'e' of each node in the class of 'd',
      // the canonical representative of 'e'
      dSucc.clear();
      for (DNode dPrime : dCl) {
        if (dPrime.post == null) continue;
        for (DNode e : dPrime.post) {
          if (e.isAnti) continue;
          if (e.isImplied) continue;
          dSucc.add(elementary_ccPair.get(e));
        }
      }
      Uma.out.println("  has successors: "+dSucc);
      
      int joint_size = dCl.size();    //
      
      for (DNode d2 : futureEquivalence().keySet()) {

        if (d == d2) continue;
        if (d2.isAnti) continue;
        if (d.id != d2.id) continue;
        if (d.isEvent != d2.isEvent) continue;
        if (futureEquivalence().areFoldingEquivalent(d, d2)) continue;
        
        Uma.out.println("  comparing to "+d2);
        
        if (areConcurrent_struct(d, d2) == REL_CO) {
          //System.out.println("    are CONCURRENT, NOT joining");
          continue;
        } else {
          // System.out.println("    are NOT concurrent");
        }

        
        Set<DNode> d2Cl = futureEquivalence().get(elementary_ccPair.get(d2));
        
        // collect the for each successor 'e2' of each node in the class of 'd2',
        // the canonical representative of 'e2'
        d2Succ.clear();
        for (DNode d2Prime : d2Cl) {
          if (d2Prime.post == null) continue;
          for (DNode e2 : d2Prime.post) {
            if (e2.isAnti) continue;
            if (e2.isImplied) continue;
            d2Succ.add(elementary_ccPair.get(e2));
          }
        }
        Uma.out.println("    has successors: "+d2Succ);
        
        // Check if we can join the classes of 'd' and 'd2': this is possible
        // if each successor of the class of 'd' has an equivalent successor of
        // the class of 'd2'. This is the case if each canonical representative
        // of a successor of 'd' is also a canonical representative of a 
        // successor of 'd2'.
        boolean all_dSucc_HaveEquivalent = true;
        for (DNode e : dSucc) {
          
          if (!d2Succ.contains(e)) {
            all_dSucc_HaveEquivalent = false;
            Uma.out.println("    -> "+e+" not in post "+d2);
            break;
          }          
        }
        
        boolean all_d2Succ_HaveEquivalent = true;
        for (DNode e2 : d2Succ) {
          
          if (!dSucc.contains(e2)) {
            all_dSucc_HaveEquivalent = false;
            Uma.out.println("    -> "+e2+" not in post "+d);
            break;
          }          
        }
        
        // finally check the case where one of the successor sets was empty
        // --> the forall loops would not detect a mismatch in the futures
        if (dSucc.isEmpty() && !d2Succ.isEmpty() || !dSucc.isEmpty() && d2Succ.isEmpty()) {
          all_dSucc_HaveEquivalent = false;
          all_d2Succ_HaveEquivalent = false;
        }
        
        if (all_dSucc_HaveEquivalent && all_d2Succ_HaveEquivalent) {
          to_join.add(d2);
          Uma.out.println("  JOINING "+d+" and "+d2);
          joint_size += d2Cl.size();    //
        } else {
          Uma.out.println("  NOT joining "+d+" and "+d2);
        }
        
        //if (!ignoreFoldThreshold && joint_size > foldThreshold * idFrequency[d.id]) break;      //
      }
      
      if (to_join.size() > 1) {
        futureEquivalence().joinEquivalenceClasses(to_join);
        changed = true;
        
        // fix the order of events to merge, so that each event has a unique index
        // the index will be used to relate pre-conditions of different events to each other
        DNode je[] = new DNode[to_join.size()];
        je = to_join.toArray(je);
                
        joinedEvents.add(je);
        
        top.removeAll(futureEquivalence().get(elementary_ccPair.get(d)));
      }
      
      if (top.isEmpty()) {
        // join pre-conditions of previously joined events, and put the
        // pre-events of those conditions back into the queue
        top.addAll(extendFutureEquivalence_backwards_conditions2(joinedEvents));
        //top.addAll(extendFutureEquivalence_backwards_conditions_futureBased(joinedEvents));
        Uma.out.println("----Joining events----");
        joinedEvents.clear();
      }
    }
    return changed;
  }

  /**
   * Fold preconditions of events that were previously folded in
   * {@link #extendFutureEquivalence_backwards()}. There, a list of
   * arratys of events that were folded was built. 
   * 
   * The pre-events of the folded conditions are returned to be processed again
   * by {@link #extendFutureEquivalence_backwards()} for further folding.
   * 
   * Folding of pre-conditions is not strictly based on their labeling. The
   * method also allows to fold conditions with different labels but equivalent
   * futures. Further, the folding allows the branching process to contain
   * implicit conditions; if an implicit condition is not required for folding,
   * it gets removed from the branching process.
   * 
   * @param allJoinedEvents
   *          the list of arrays of events that have just been joined
   * 
   * @return the list of pre-events of the joined conditions
   * 
   */
  private LinkedHashSet<DNode> extendFutureEquivalence_backwards_conditions(LinkedList<DNode []> allJoinedEvents) {

    // list of pre-events of the folded conditions, returned by this method
    LinkedHashSet<DNode> preEvents = new LinkedHashSet<DNode>();
    
    // reference locally
    final Map<DNode, DNode> elementary_ccPair = futureEquivalence().getElementary_ccPair();
    
    System.out.println("----Joining conditions----");
    
    for (DNode[] joinedEvents : allJoinedEvents) {
      
      for (DNode e : joinedEvents) {
        System.out.println("  "+e+" has pred: "+DNode.toString(e.pre));
      }
      
      // all pre-conditions of all events that we join
      // later, we remove all conditions that have been joined with some other
      // condition. the remaining conditions are implied conditions that are
      // no longer required in the net and hence will be removed
      HashSet<DNode> nonJoinedConditions = new HashSet<DNode>();
      for (DNode d2 : joinedEvents) {
        // initialize, remove joined conditions afterwards
        for (DNode b : d2.pre) nonJoinedConditions.add(b); 
      }
      
      Set<DNode> consideredEvents = new HashSet<DNode>();
      
      while (consideredEvents.size() != joinedEvents.length)
      {
        System.out.println("considered: "+consideredEvents);
        System.out.println("conditions: "+nonJoinedConditions);

        // find the event with the largest preset and remember its index
        int maxDirectPreSize = 0;
        int directPreSize[] = new int[joinedEvents.length];
        
        for (int i=0; i<joinedEvents.length; i++) {
          
          if (consideredEvents.contains(joinedEvents[i])) continue;
          
          // for the remaining events, determine the events with the largest pre-set of non-implied conditions
          directPreSize[i] = 0;
          for (int j=0; j<joinedEvents[i].pre.length; j++) {
            if (!joinedEvents[i].pre[j].isImplied) directPreSize[i]++;
          }
          
          if (maxDirectPreSize < directPreSize[i]) {
            maxDirectPreSize = directPreSize[i];
          }
        }
        if (maxDirectPreSize == 0) break; // all events have been considered, done joining presets
        
        // build and index-array to indicate events with the largest pre-set of non-implied conditions
        boolean[] isMaxIndex = new boolean[joinedEvents.length];
        for (int i=0; i<joinedEvents.length; i++) {
          isMaxIndex[i] = (directPreSize[i] == maxDirectPreSize);
          if (isMaxIndex[i]) System.out.println("merge for: "+joinedEvents[i]);
        }
  
        // collect conditions: for each condition of the event with the most pre-conditions
        // we have to find an equivalent condition of every other event
        DNode[][] joinConditions = new DNode[maxDirectPreSize][joinedEvents.length];
        
        // now for all events that provide a maximal direct pre-set,
        // try to match each direct precondition to other preconditions of the equivalent events
        // use implied conditions of other events to fill up the equivalence for the maximal pre-set
        for (int maxIndex = 0; maxIndex < joinedEvents.length; maxIndex++) {
          if (!isMaxIndex[maxIndex]) continue;
          
          consideredEvents.add(joinedEvents[maxIndex]);
          
          int maxIndex_preIndex[] = new int[joinedEvents[maxIndex].pre.length];
          
          // Match all conditions with the same label: these are definitely equivalent.
          // The event with the least pre-conditions defines the distribution;
          // all other pre-conditions must be matched here.
          for (int j=0,j2=0; j<joinedEvents[maxIndex].pre.length; j++) {
            if (!joinedEvents[maxIndex].pre[j].isImplied) {
              System.out.println("adding "+joinedEvents[maxIndex].pre[j]);
              maxIndex_preIndex[j] = j2;
              joinConditions[j2][maxIndex] = joinedEvents[maxIndex].pre[j];
              nonJoinedConditions.remove(joinConditions[j2][maxIndex]);
              j2++;
            } else {
              System.out.println("skipping "+joinedEvents[maxIndex].pre[j]);
              maxIndex_preIndex[j] = -1;
            }
          }
          
          for (int i=0; i<joinedEvents.length; i++) {
            if (i == maxIndex) continue;
    
            // find for each pre-condition of event i 
            for (int k=0; k<joinedEvents[i].pre.length; k++) {
              // the corresponding pre-condition of event maxIndex with the same label
              // if it exists
              for (int j=0; j<joinedEvents[maxIndex].pre.length; j++) {
                if (joinedEvents[maxIndex].pre[j].isImplied) continue;
                
                if (joinedEvents[i].pre[k].id == joinedEvents[maxIndex].pre[j].id) {
                  joinConditions[maxIndex_preIndex[j]][i] = joinedEvents[i].pre[k];
                  nonJoinedConditions.remove(joinConditions[maxIndex_preIndex[j]][i]);
                  System.out.println("  joining "+joinedEvents[maxIndex].pre[j]+" and "+joinConditions[maxIndex_preIndex[j]][i]);
                  consideredEvents.add(joinedEvents[i]);
                  break;
                }
              }
            }
          }
          // now, we have found all pre-conditions of all events that match by label
          // next: find the missing conditions based on futures
          
          for (int j=0; j<joinConditions.length; j++) {
            
            for (int i=0; i<joinConditions[j].length; i++) {
              // the j-th precondition of 'minIndex' already has an equivalent
              // precondition of event 'i', done
              if (joinConditions[j][i] != null) continue;
              
              // the j-th precondition of 'minIndex' has no equivalent
              // precondition of event 'i' yet. find one
              DNode toMatch = joinConditions[j][maxIndex];
              
              boolean foundMatch = false;
              
              // check all preconditions of event 'i' 
              
              // first check the non-implied pre-conditions with the same name
              LinkedList<DNode> matchConditions = new LinkedList<DNode>();
              // then the non-implied pre-conditions with a different name 
              for (DNode pre : joinedEvents[i].pre) {
                if (!nonJoinedConditions.contains(pre)) continue;
                if (areConcurrent_struct(toMatch, pre) == REL_CO) continue;
                if (!pre.isImplied) matchConditions.addLast(pre);
              }
              // then the implied conditions
              for (DNode pre : joinedEvents[i].pre) {
                if (!nonJoinedConditions.contains(pre)) continue;
                if (areConcurrent_struct(toMatch, pre) == REL_CO) continue;
                if (pre.isImplied) matchConditions.addLast(pre);
              }
              
              System.out.println("match "+toMatch+" to "+matchConditions);
              
              for (DNode matchCondition : matchConditions) {
                // check if 'toMatch' and 'matchCondition' are successor equivalent
                // get future equivalence classes
                DNode[] future_toMatch = EquivalenceRefineSuccessor.getSuccessorEquivalence(this, toMatch);
                DNode[] future_matchCon = EquivalenceRefineSuccessor.getSuccessorEquivalence(this, matchCondition);
                
                System.out.println("can "+toMatch+" join with "+matchCondition+"?");
                System.out.println("  future: "+DNode.toString(future_toMatch));
                System.out.println("  future: "+DNode.toString(future_matchCon));
                
                // and compare
                if (!Arrays.equals(future_toMatch, future_matchCon)) {
                  System.out.println("  are not equal");
                  continue;
                }
                
                for (DNode e : matchCondition.post) {
                  consideredEvents.add(e);
                }
                
                // make 'toMatch' and 'matchCondition' equivalent
                joinConditions[j][i] = matchCondition;
                nonJoinedConditions.remove(matchCondition);
                foundMatch = true;
                break;
              }
              
              if (!foundMatch) {
                
                
                for (DNode matchCondition : matchConditions) {
                  // check if 'toMatch' and 'matchCondition' are successor equivalent
                  // get future equivalence classes
                  DNode[] future_toMatch = EquivalenceRefineSuccessor.getSuccessorEquivalence(this, toMatch);
                  DNode[] future_matchCon = EquivalenceRefineSuccessor.getSuccessorEquivalence(this, matchCondition);
                  
                  System.out.println("can "+toMatch+" subsume "+matchCondition+"?");
                  System.out.println("  future: "+DNode.toString(future_toMatch));
                  System.out.println("  future: "+DNode.toString(future_matchCon));
                  
                  // and compare
                  if (!DNode.containedIn(future_toMatch, future_matchCon) && !DNode.containedIn(future_matchCon, future_toMatch)) {
                    System.out.println("  do not subsume");
                    continue;
                  }
                  
                  for (DNode e : matchCondition.post) {
                    consideredEvents.add(e);
                  }
                  
                  // make 'toMatch' and 'matchCondition' equivalent
                  joinConditions[j][i] = matchCondition;
                  nonJoinedConditions.remove(matchCondition);
                  foundMatch = true;
                  break;
                }
                
                System.out.println("no match "+toMatch);
                nonJoinedConditions.add(toMatch);
              }
            }
          }
        } // for (maxIndex)
        
        for (int j=0; j<joinConditions.length; j++) {
          LinkedList<DNode> _joinConditions = new LinkedList<DNode>();
          for (int i=0; i<joinConditions[j].length; i++) {
            if (joinConditions[j][i] != null) { 
              _joinConditions.add(elementary_ccPair.get(joinConditions[j][i]));
              joinConditions[j][i].isImplied = false;
  
              // and add the predecessors of the joined pre-sets to the queue 
              if (joinConditions[j][i].pre != null) {
                for (DNode e : joinConditions[j][i].pre) {
                  preEvents.add(e);
                }
              }
            }
          }
            System.out.println("joining "+_joinConditions);
          futureEquivalence().joinEquivalenceClasses(_joinConditions);
        }
      } // while (still events to be considered)
      
      for (DNode b : nonJoinedConditions) {
        boolean isSinglePre = false;
        for (DNode e : b.post) {
          if (e.pre.length == 1) {
            isSinglePre = true;
            break;
          }
        }
        boolean isSinglePost = false;
        for (DNode e : b.pre) {
          if (e.post.length == 1) {
            isSinglePost = true;
            break;
          }
        }
        
        int sharedBy = 0;

        for (DNode e : b.post) {
          for (DNode ePrime : joinedEvents) {
            if (e == ePrime) sharedBy++;
          }
        }
        if (sharedBy > 1) {
          System.out.println("shared: "+b);
        }
        
        if (!isSinglePre && !isSinglePost && sharedBy <= 1) {
          //this.bp.remove(b);
          b.isImplied = true;
          System.out.println("non-joined: "+b);
          
        } else {
          if (b.pre != null) {
            for (DNode e : b.pre) {
              preEvents.add(e);
            }
          }
        }
      }
    }
    
    return preEvents;
  }
  
  /**
   * Fold preconditions of events that were previously folded in
   * {@link #extendFutureEquivalence_backwards()}. There, a list of
   * arratys of events that were folded was built. 
   * 
   * The pre-events of the folded conditions are returned to be processed again
   * by {@link #extendFutureEquivalence_backwards()} for further folding.
   * 
   * Folding of pre-conditions is not strictly based on their labeling. The
   * method also allows to fold conditions with different labels but equivalent
   * futures. Further, the folding allows the branching process to contain
   * implicit conditions; if an implicit condition is not required for folding,
   * it gets removed from the branching process.
   * 
   * @param allJoinedEvents
   *          the list of arrays of events that have just been joined
   * 
   * @return the list of pre-events of the joined conditions
   * 
   */
  private LinkedHashSet<DNode> extendFutureEquivalence_backwards_conditions2(LinkedList<DNode []> allJoinedEvents) {

    // list of pre-events of the folded conditions, returned by this method
    LinkedHashSet<DNode> preEvents = new LinkedHashSet<DNode>();
    
    // reference locally
    final Map<DNode, DNode> elementary_ccPair = futureEquivalence().getElementary_ccPair();
    
    System.out.println("----Joining conditions----");
    
    for (DNode[] joinedEvents : allJoinedEvents) {
      
      for (DNode e : joinedEvents) {
        System.out.println("  "+e+" has pred: "+DNode.toString(e.pre));
      }
      
      // all pre-conditions of all events that we join
      // later, we remove all conditions that have been joined with some other
      // condition. the remaining conditions are implied conditions that are
      // no longer required in the net and hence will be removed
      HashSet<DNode> nonJoinedConditions = new HashSet<DNode>();
      for (DNode d2 : joinedEvents) {
        // initialize, remove joined conditions afterwards
        for (DNode b : d2.pre) nonJoinedConditions.add(b); 
      }
      
      Set<DNode> consideredEvents = new HashSet<DNode>();
      
      while (consideredEvents.size() != joinedEvents.length)
      {
        System.out.println("considered: "+consideredEvents);
        System.out.println("conditions: "+nonJoinedConditions);

        // find the event with the largest preset and remember its index
        int maxDirectPreSize = 0;
        int directPreSize[] = new int[joinedEvents.length];
        
        for (int i=0; i<joinedEvents.length; i++) {
          
          if (consideredEvents.contains(joinedEvents[i])) continue;
          
          // for the remaining events, determine the events with the largest pre-set of non-implied conditions
          directPreSize[i] = 0;
          for (int j=0; j<joinedEvents[i].pre.length; j++) {
            if (!joinedEvents[i].pre[j].isImplied) directPreSize[i]++;
          }
          
          if (maxDirectPreSize < directPreSize[i]) {
            maxDirectPreSize = directPreSize[i];
          }
        }
        if (maxDirectPreSize == 0) break; // all events have been considered, done joining presets
        
        // build and index-array to indicate events with the largest pre-set of non-implied conditions
        boolean[] isMaxIndex = new boolean[joinedEvents.length];
        for (int i=0; i<joinedEvents.length; i++) {
          isMaxIndex[i] = (directPreSize[i] == maxDirectPreSize);
          if (isMaxIndex[i]) System.out.println("merge for: "+joinedEvents[i]+" "+maxDirectPreSize);
        }
  
        // collect conditions: for each condition of the event with the most pre-conditions
        // we have to find an equivalent condition of every other event
        Set<DNode>[] joinConditions = (Set<DNode>[])new Set[maxDirectPreSize];
        for (int i=0; i<maxDirectPreSize; i++) joinConditions[i] = new HashSet<DNode>();
        
        // now for all events that provide a maximal direct pre-set,
        // try to match each direct precondition to other preconditions of the equivalent events
        // use implied conditions of other events to fill up the equivalence for the maximal pre-set
        for (int eventIndex = 0; eventIndex < joinedEvents.length; eventIndex++) {
          if (!isMaxIndex[eventIndex]) continue;
          
          DNode event = joinedEvents[eventIndex];
          consideredEvents.add(event);
          
          System.out.println("start "+event);
          
          // Match all conditions with the same label: these are definitely equivalent.
          // The event with the least pre-conditions defines the distribution;
          // all other pre-conditions must be matched here.
          for (int b=0,b_direct=0; b<event.pre.length; b++) {
            if (!joinedEvents[eventIndex].pre[b].isImplied) {
              System.out.println("adding "+joinedEvents[eventIndex].pre[b]+" to "+b_direct+" by start");
              joinConditions[b_direct].add(joinedEvents[eventIndex].pre[b]);
              nonJoinedConditions.remove(joinedEvents[eventIndex].pre[b]);
              b_direct++;
            } else {
              System.out.println("skipping "+joinedEvents[eventIndex].pre[b]);
            }
          }
          
          // now match the pre-conditions of all other (previously unconsidered events) to 'e'
          for (int e=0; e<joinedEvents.length; e++) {
            if (e == eventIndex) continue;
            if (consideredEvents.contains(joinedEvents[e])) continue;
            
            System.out.println("match "+joinedEvents[e]);

            // sort all pre-conditions of event 'e', first direct, then implied
            LinkedList<DNode> pre_conditions_ordered = new LinkedList<DNode>();
            for (int b=0; b<joinedEvents[e].pre.length; b++) {
              if (!joinedEvents[e].pre[b].isImplied) pre_conditions_ordered.add(joinedEvents[e].pre[b]);
            }
            for (int b=0; b<joinedEvents[e].pre.length; b++) {
              if (joinedEvents[e].pre[b].isImplied) pre_conditions_ordered.add(joinedEvents[e].pre[b]);
            }
            
            System.out.println("to match "+pre_conditions_ordered);
            
            // list for each bucket whether one pre-condition of 'e' was put there already
            boolean bucket_used[] = new boolean[maxDirectPreSize];
    
            // find for each pre-condition of event 'e' (up to maxDirectPreSize)
            // a set of matching pre-conditions in 'joinConditions'
            int matched_preconditions = 0;
            int matched_direct_preconditions = 0;
            for (DNode b : pre_conditions_ordered) {
              
              // for this pre-condition, find the matching bucket
              boolean found = false;
              
              // first look for a bucket having a condition with the same id
              for (int b_direct=0;b_direct<maxDirectPreSize;b_direct++) {
                if (bucket_used[b_direct]) continue;
                
                boolean put_here = false;
                for (DNode b_put : joinConditions[b_direct]) {
                  // cannot join with a bucket having a concurrent condition
                  int rel = areConcurrent_struct(b_put, b);
                  if (rel == REL_CO) break;  
                  
                  if (b_put.id == b.id) {
                    put_here = true;
                    break;
                  }
                }
                if (put_here) {
                  joinConditions[b_direct].add(b);
                  bucket_used[b_direct] = true;
                  nonJoinedConditions.remove(b);
                  System.out.println("adding "+b+" to "+b_direct+" by id");
                  found = true;
                  break;
                }
              }
              
              if (!found) {
                // second look for a bucket having a condition with equivalent futures
                // where this bucket was not used yet
                
                // first look for a bucket having a condition with the same id 
                for (int b_direct=0;b_direct<maxDirectPreSize;b_direct++) {
                  if (bucket_used[b_direct]) continue;
                  
                  boolean put_here = true;
                  for (DNode b_put : joinConditions[b_direct]) {
                    // cannot join with a bucket having a concurrent condition
                    int rel = areConcurrent_struct(b_put, b);
                    if (rel == REL_CO) {
                      put_here = false;
                      break;  
                    }

                    // check if all 'b_put' and 'b' are successor equivalent
                    // get future equivalence classes
                    DNode[] future_toMatch = EquivalenceRefineSuccessor.getSuccessorEquivalence(this, b_put);
                    DNode[] future_matchCon = EquivalenceRefineSuccessor.getSuccessorEquivalence(this, b);
                    
                    System.out.println("can "+b+" join with "+b_put+"?");
                    System.out.println("  future: "+DNode.toString(future_toMatch));
                    System.out.println("  future: "+DNode.toString(future_matchCon));
                    
                    // and compare
                    if (!Arrays.equals(future_toMatch, future_matchCon)) {
                      System.out.println("  are not equal");
                      put_here = false;
                      break;
                    }
                  }
                  if (put_here) {
                    joinConditions[b_direct].add(b);
                    bucket_used[b_direct] = true;
                    nonJoinedConditions.remove(b);
                    System.out.println("adding "+b+" to "+b_direct+" by exact future");
                    found = true;
                    break;
                  }
                }
              }
              
              if (!found) {
                // second look for a bucket having a condition with equivalent futures
                // where this bucket was not used yet

                int put_at = -1;
                
                // first look for a bucket having a condition with the same id 
                for (int b_direct=0;b_direct<maxDirectPreSize;b_direct++) {
                  if (!bucket_used[b_direct]) {
                    put_at = b_direct;
                    break;
                  }
                }
                
                int max_overlap = 0;

                // first look for a bucket having a condition with the same id 
                for (int b_direct=0;b_direct<maxDirectPreSize;b_direct++) {
                  if (bucket_used[b_direct]) continue;

                  int overlap_with_bucket = 0;
                  
                  next_bucket: for (DNode b_put : joinConditions[b_direct]) {
                    // cannot join with a bucket having a concurrent condition
                    int rel = areConcurrent_struct(b_put, b);
                    if (rel == REL_CO) {
                      break;  
                    }
  
                    // check if all 'b_put' and 'b' are successor equivalent
                    // get future equivalence classes
                    DNode[] future_toMatch = EquivalenceRefineSuccessor.getSuccessorEquivalence(this, b_put);
                    DNode[] future_matchCon = EquivalenceRefineSuccessor.getSuccessorEquivalence(this, b);
                    
                    for (DNode f1 : future_toMatch) {
                      if (f1 == joinedEvents[e]) continue next_bucket;
                    }

                    for (DNode f1 : future_toMatch) {
                      for (DNode f2 : future_matchCon) {
                        if (f1 == f2) overlap_with_bucket++;
                      }
                    }
                  }
                  
                  if (overlap_with_bucket > max_overlap) {
                    max_overlap = overlap_with_bucket;
                    put_at = b_direct;
                  }
                }
                  
                if (put_at != -1) {
                  joinConditions[put_at].add(b);
                  bucket_used[put_at] = true;
                  nonJoinedConditions.remove(b);
                  System.out.println("adding "+b+" to "+put_at+" by similar future");
                  found = true;
                }
              }
              
              /*
                 for (DNode matchCondition : matchConditions) {
                  // check if 'toMatch' and 'matchCondition' are successor equivalent
                  // get future equivalence classes
                  DNode[] future_toMatch = EquivalenceRefineSuccessor.getSuccessorEquivalence(this, toMatch);
                  DNode[] future_matchCon = EquivalenceRefineSuccessor.getSuccessorEquivalence(this, matchCondition);
                  
                  System.out.println("can "+toMatch+" subsume "+matchCondition+"?");
                  System.out.println("  future: "+DNode.toString(future_toMatch));
                  System.out.println("  future: "+DNode.toString(future_matchCon));
                  
                  // and compare
                  if (!DNode.containedIn(future_toMatch, future_matchCon) && !DNode.containedIn(future_matchCon, future_toMatch)) {
                    System.out.println("  do not subsume");
                    continue;
                  }
                  
                  for (DNode e : matchCondition.post) {
                    consideredEvents.add(e);
                  }
                  
                  // make 'toMatch' and 'matchCondition' equivalent
                  joinConditions[j][i] = matchCondition;
                  nonJoinedConditions.remove(matchCondition);
                  foundMatch = true;
                  break;
                }
                
                System.out.println("no match "+toMatch);
                nonJoinedConditions.add(toMatch);
              }
              */
              
              if (found) {
                matched_preconditions++;
              } else {
                
                if (!b.isImplied) {
                  System.out.println("ERROR: no match for "+b);
                  break;
                } else {
                  System.out.println("no match for implied "+b);
                }
              }
              
              if (matched_preconditions == maxDirectPreSize) {
                System.out.println("finished matching for "+joinedEvents[e]);
                consideredEvents.add(joinedEvents[e]);
                break; // found a match for each pre-condition that 'e' has to match
              }
            } // for each pre-condition of e
            
            if (matched_preconditions != maxDirectPreSize) {
              System.out.println("undoing: "+matched_preconditions+" != "+maxDirectPreSize);
              consideredEvents.remove(joinedEvents[e]);
              for (int i=0; i<joinConditions.length; i++) {
                for (DNode b : joinedEvents[e].pre) {
                  joinConditions[i].remove(b);
                }
              }
            }
            
            
          } // for all events 'e'
          
        } // for each event of pre-set size 'maxSize'
          
        // now join the equivalence classes that have been found
        for (int i=0; i<maxDirectPreSize; i++) {
          // and add the predecessors of the joined pre-sets to the queue 
          for (DNode b : joinConditions[i]) {
            b.isImplied = false;
            for (DNode e : b.pre) {
              preEvents.add(e);
            }
          }
          System.out.println(">> joining "+joinConditions[i]);
          futureEquivalence().joinEquivalenceClasses(joinConditions[i]);
        }
      } // while (still events to be considered)
      
      for (DNode b : nonJoinedConditions) {
        boolean isSinglePre = false;
        for (DNode e : b.post) {
          if (e.pre.length == 1) {
            isSinglePre = true;
            break;
          }
        }
        boolean isSinglePost = false;
        for (DNode e : b.pre) {
          if (e.post.length == 1) {
            isSinglePost = true;
            break;
          }
        }
        
        int sharedBy = 0;

        for (DNode e : b.post) {
          for (DNode ePrime : joinedEvents) {
            if (e == ePrime) sharedBy++;
          }
        }
        if (sharedBy > 1) {
          System.out.println("shared: "+b);
        }
        
        if (!isSinglePre && !isSinglePost && sharedBy <= 1) {
          //this.bp.remove(b);
          b.isImplied = true;
          System.out.println("non-joined: "+b);
          
        } else {
          if (b.pre != null) {
            for (DNode e : b.pre) {
              preEvents.add(e);
            }
          }
        }
      }
    }
    
    return preEvents;
  }
  
  /**
   * Fold preconditions of events that were previously folded in
   * {@link #extendFutureEquivalence_backwards()}. There, a list of
   * arratys of events that were folded was built. 
   * 
   * The pre-events of the folded conditions are returned to be processed again
   * by {@link #extendFutureEquivalence_backwards()} for further folding.
   * 
   * Folding of pre-conditions is not strictly based on their labeling. The
   * method also allows to fold conditions with different labels but equivalent
   * futures.
   * 
   * @param allJoinedEvents
   *          the list of arrays of events that have just been joined
   * 
   * @return the list of pre-events of the joined conditions
   * 
   */
  private LinkedHashSet<DNode> extendFutureEquivalence_backwards_conditions_futureBased(LinkedList<DNode []> allJoinedEvents) {

    // list of pre-events of the folded conditions, returned by this method
    LinkedHashSet<DNode> preEvents = new LinkedHashSet<DNode>();
    
    // reference locally
    final Map<DNode, DNode> elementary_ccPair = futureEquivalence().getElementary_ccPair();
    
    System.out.println("----Joining conditions----");
    
    for (DNode[] joinedEvents : allJoinedEvents) {
      
      for (DNode e : joinedEvents) {
        System.out.println("  "+e+" has pred: "+DNode.toString(e.pre));
      }
      
      // all pre-conditions of all events that we join
      // later, we remove all conditions that have been joined with some other
      // condition. the remaining conditions are implied conditions that are
      // no longer required in the net and hence will be removed
      HashSet<DNode> nonJoinedConditions = new HashSet<DNode>();
      for (DNode d2 : joinedEvents) {
        // initialize, remove joined conditions afterwards
        for (DNode b : d2.pre) nonJoinedConditions.add(b); 
      }
      
      Map<DNode[], Set<DNode>> equivalenceClasses = new HashMap<DNode[],  Set<DNode>>();
      
      for (DNode b : nonJoinedConditions) {
        DNode[] future = EquivalenceRefineSuccessor.getSuccessorEquivalence(this, b);
        boolean added = false;
        for (DNode[] f : equivalenceClasses.keySet()) {
          if (DNode.containedIn(future, f)) {
            equivalenceClasses.get(f).add(b);
            added = true;
            break;
          }
        }
        
        if (!added) {
          equivalenceClasses.put(future, new HashSet<DNode>());
          equivalenceClasses.get(future).add(b);
          
          LinkedList<DNode[]> remove = new LinkedList<DNode[]>();
          for (DNode[] f : equivalenceClasses.keySet()) {
            if (f == future) continue;
            
            if (DNode.containedIn(f, future)) {
              equivalenceClasses.get(future).addAll(equivalenceClasses.get(f));
              remove.add(f);
            }
          }
          for (DNode[] f : remove) equivalenceClasses.remove(f);
        }
      }
      
      for (Set<DNode> toJoin : equivalenceClasses.values()) {
        
        Set<DNode> _joinConditions = new HashSet<DNode>();
        for (DNode b : toJoin) {
          nonJoinedConditions.remove(b);
          
          _joinConditions.add(elementary_ccPair.get(b));
          b.isImplied = false;

          // and add the predecessors of the joined pre-sets to the queue 
          if (b.pre != null) {
            for (DNode e : b.pre) {
              preEvents.add(e);
            }
          }
        }
        futureEquivalence().joinEquivalenceClasses(_joinConditions);
      }
              
      
      for (DNode b : nonJoinedConditions) {
        boolean isSinglePre = false;
        for (DNode e : b.post) {
          if (e.pre.length == 1) {
            isSinglePre = true;
            break;
          }
        }
        boolean isSinglePost = false;
        for (DNode e : b.pre) {
          if (e.post.length == 1) {
            isSinglePost = true;
            break;
          }
        }
        
        int sharedBy = 0;

        for (DNode e : b.post) {
          for (DNode ePrime : joinedEvents) {
            if (e == ePrime) sharedBy++;
          }
        }
        if (sharedBy > 1) {
          System.out.println("shared: "+b);
        }
        
        if (!isSinglePre && !isSinglePost && sharedBy <= 1) {
          //this.bp.remove(b);
          b.isImplied = true;
          System.out.println("non-joined: "+b);
          
        } else {
          if (b.pre != null) {
            for (DNode e : b.pre) {
              preEvents.add(e);
            }
          }
        }
      }
    }
    
    return preEvents;
  }
  
  
  public boolean extendFutureEquivalence_backwards_complete() {
      boolean changed = false;
      
      int idFrequency[] = new int[dNodeAS.currentNameID];  //
      for (DNode d : bp.getAllNodes()) {                   //
        idFrequency[d.id]++;                               //
      }
      //float foldThreshold = .1f;                          //
      
      LinkedHashSet<DNode> top = new LinkedHashSet<DNode>();
      for (DNode b : bp.getAllNodes()) {
        if (b.isAnti) continue;
        if (b.isEvent) continue;
        if ((b.post == null || b.post.length == 0) && b.pre != null) {
          for (DNode e : b.pre)
            top.add(e);
        }
      }

      LinkedList<DNode> to_join = new LinkedList<DNode>();
      HashSet<DNode> eSucc = new HashSet<DNode>();
      HashSet<DNode> e2Succ = new HashSet<DNode>();
      
      // map to associate a post-condition of an event other post-conditions of
      // merge events in case one of the post-conditions has an empty post-set and
      // has not been considered as equivalent to other post-conditions yet
      HashMap<DNode, HashSet<DNode> > postMerge = new HashMap<DNode, HashSet<DNode>>();
      
      //Uma.out.println("top: "+top);
      // reference locally
      final Map<DNode, DNode> elementary_ccPair = futureEquivalence().getElementary_ccPair();

      //int print_count = 1;
      while (!top.isEmpty()) {
        
        //if ((print_count % 100) == 0) Uma.out.print(top.size()+" ");
        //if (print_count++ > 1000) {
        //  Uma.out.print("\n");
        //  print_count = 0;
        //}
        
        //Uma.out.println("top: "+top);
        DNode e = top.iterator().next();
        top.remove(e);
        //Uma.out.println("removed "+e);

        to_join.clear();
        to_join.add(e);
        postMerge.clear();

        Set<DNode> eCl = futureEquivalence().get(elementary_ccPair.get(e));

        //if (!ignoreFoldThreshold && dCl.size() > idFrequency[d.id] * foldThreshold) continue;     //
        
        // collect the for each successor 'e' of each node in the class of 'd',
        // the canonical representative of 'e'
        eSucc.clear();
        for (DNode ePrime : eCl) {
          if (ePrime.post == null) continue;
          for (DNode b : ePrime.post) {
            if (b.isAnti) continue;
            eSucc.add(elementary_ccPair.get(b));
          }
        }
        //Uma.out.println("  has successors: "+eSucc);
        
        int joint_size = eCl.size();    //
        
        for (DNode e2 : futureEquivalence().keySet()) {

          if (e == e2) continue;
          if (e2.isAnti) continue;
          if (e.id != e2.id) continue;
          if (futureEquivalence().areFoldingEquivalent(e, e2)) continue;
          
          //Uma.out.println("  comparing to "+e2);
          
          Set<DNode> e2Cl = futureEquivalence().get(elementary_ccPair.get(e2));
          
          // collect for each successor 'e2' of each node in the class of 'd2',
          // the canonical representative of 'e2'
          e2Succ.clear();
          for (DNode e2Prime : e2Cl) {
            if (e2Prime.post == null) continue;
            for (DNode b2 : e2Prime.post) {
              if (b2.isAnti) continue;
              e2Succ.add(elementary_ccPair.get(b2));
            }
          }
          //Uma.out.println("    has successors: "+e2Succ);
          
          // Check if we can join the classes of 'd' and 'd2': this is possible
          // if each successor of the class of 'd' has an equivalent successor of
          // the class of 'd2'. This is the case if each canonical representative
          // of a successor of 'd' is also a canonical representative of a 
          // successor of 'd2'.
          boolean all_eSucc_HaveEquivalent = true;
          for (DNode b : eSucc) {
            // allow to merge conditions if one of them has an empty post-set
            if (!e2Succ.contains(b)) {
              
              if (b.post == null) {
                // merging of 'e' and 'e2' could fail because 'b' has an empty post-set
                // and hence is not equivalent to the post-conditions of 'e2', extend
                // equivalence to post-conditions: find matching post-condition for 'b'
                if (!postMerge.containsKey(b)) {
                  postMerge.put(b, new HashSet<DNode>());
                  postMerge.get(b).add(b);
                }
                for (DNode b2 : e2Succ) {
                  if (b.id == b2.id) {
                    postMerge.get(b).add(b2);
                  }
                }
              } else {
                // not equivalent: cannot merge 'e' and 'e2'
                all_eSucc_HaveEquivalent = false;
                //Uma.out.println("    -1-> "+b+" not in post "+e2);
                //Uma.out.println("    -1-> "+DNode.toString(b.post));
                break;
              }
            }
          }
          
          boolean all_e2Succ_HaveEquivalent = true;
          for (DNode b2 : e2Succ) {
            // allow to merge conditions if one of them has an empty post-set
            if (!eSucc.contains(b2)) {
              
              if (b2.post == null) {
                // merging of 'e' and 'e2' could fail because 'b2' has an empty post-set
                // and hence is not equivalent to the post-conditions of 'e', extend
                // equivalence to post-conditions: find matching post-condition of 'e' for 'b2'
                for (DNode b : eSucc) {
                  if (b.id == b2.id) {
                    if (!postMerge.containsKey(b)) {
                      postMerge.put(b, new HashSet<DNode>());
                      postMerge.get(b).add(b);
                    }
                    postMerge.get(b).add(b2);
                  }
                }
              } else {
                // not equivalent: cannot merge 'e' and 'e2'
                all_eSucc_HaveEquivalent = false;
                //Uma.out.println("    -2-> "+b2+" not in post "+e);
                //Uma.out.println("    -2-> "+DNode.toString(b2.post));
                break;
              }
            }
          }
          
          
          if (eSucc.isEmpty() && !e2Succ.isEmpty() || !eSucc.isEmpty() && e2Succ.isEmpty()) {
            all_eSucc_HaveEquivalent = false;
            all_e2Succ_HaveEquivalent = false;
          }
          
          if (all_eSucc_HaveEquivalent && all_e2Succ_HaveEquivalent) {
            to_join.add(e2);
            //Uma.out.println("  JOINING "+e+" and "+e2);
            joint_size += e2Cl.size();    //
          } else {
            //Uma.out.println("  NOT joining "+e+" and "+e2);
          }
          
          //if (!ignoreFoldThreshold && joint_size > foldThreshold * idFrequency[d.id]) break;      //
        }
        
        if (to_join.size() > 1) {
          futureEquivalence().joinEquivalenceClasses(to_join);
          changed = true;
          
          // join the pre-set of all joined transitions
          LinkedList<DNode> preJoin = new LinkedList<DNode>();
          for (int i=0; i<e.pre.length; i++) {
            preJoin.clear();
            for (DNode e2 : to_join) {
              preJoin.add(elementary_ccPair.get(e2.pre[i]));
              // and add the predecessors of the joined pre-sets to the queue 
              if (e2.pre[i].pre != null) {
                for (DNode e3 : e2.pre[i].pre) {
                  top.add(e3);
                }
              }
            }
            futureEquivalence().joinEquivalenceClasses(preJoin);
          }
          
          // join the post-set of the joined transitions that were not joined yet,
          // e.g. because of conditions with empty post-sets
          for (HashSet<DNode> postJoin : postMerge.values()) {
            futureEquivalence().joinEquivalenceClasses(postJoin);
          }
          
          top.removeAll(futureEquivalence().get(elementary_ccPair.get(e)));
        } 
      }
      return changed;
    }
  
  public boolean blockedFolding() {

    // reference locally
    final Map<DNode, DNode> elementary_ccPair = futureEquivalence().getElementary_ccPair();
    
    // print all conditions where folding stopped and why
    boolean changed = false;

    LinkedList<HashSet<DNode>> toJoin = new LinkedList<HashSet<DNode>>();
    
    for (DNode b : futureEquivalence().keySet()) {
      if (b.isEvent) continue;
      
      HashSet<DNode> predecessors_b[] = new HashSet[dNodeAS.currentNameID];
      for (int i=0;i<dNodeAS.currentNameID;i++) {
        predecessors_b[i] = new HashSet<DNode>();
      }
      for (DNode bPrime : futureEquivalence().get(b)) {
        if (bPrime.pre == null) continue;
        for (DNode e : bPrime.pre) {
          predecessors_b[e.id].add(elementary_ccPair.get(e));
        }
      }
      
      for (int i=0;i<predecessors_b.length;i++) {
        if (predecessors_b[i].size() > 1) {
          System.out.println("folding stopped at "+b+": "+predecessors_b[i]);
          
          for (DNode e : predecessors_b[i]) {
            boolean postMax = false;
            for (DNode b2 : e.post) {
              if (b2.post == null || b2.post.length == 0) {
                System.out.println(e+" has maximal post-condition "+b2);
                postMax = true;
              }
            }
            for (DNode e2 : predecessors_b[i]) {
              if (e == e2) continue;
              if (elementary_ccPair.get(e) == elementary_ccPair.get(e2)) {
                System.out.println("error: equivalent events");
                continue;
              }

              boolean allPostEquiv_exceptMax = true;
              for (int j=0;j<e.post.length;j++) {
                if (futureEquivalence().areFoldingEquivalent(e.post[j],e2.post[j])) continue;
                if (e.post[j].post == null || e.post[j].post.length == 0) continue;
                if (e2.post[j].post == null || e2.post[j].post.length == 0) continue;
                allPostEquiv_exceptMax = false;
                break;
              }
              
              if (allPostEquiv_exceptMax) {
                for (int j=0;j<e.post.length;j++) {
                  if (!futureEquivalence().areFoldingEquivalent(e.post[j],e2.post[j])) {
                    HashSet<DNode> nodes_toJoin = new HashSet<DNode>();
                    nodes_toJoin.add(e.post[j]);
                    nodes_toJoin.add(e2.post[j]);
                    toJoin.add(nodes_toJoin);
                  }
                }
              }
            }
          }
        }
      }
    }
    
    for (HashSet<DNode> n : toJoin) {
      futureEquivalence().joinEquivalenceClasses(n);
      changed = true;
    }
    
    return changed;
  }
  
  private static boolean equivalentConditions(DNode b, HashSet<DNode> conds) {
    if (b.post == null || b.post.length == 0) return true;
    if (conds.contains(b)) return true;
    for (DNode bPrime : conds) {
      if (b.id == bPrime.id) {
        if (bPrime.post == null || bPrime.post.length == 0) return true;
        else return false;
      }
    }
    return false;
  }

  
  /**
   * Remove a superfluous condition from the branching process after computing the
   * folding equivalence. A condition is superfluous if it expresses a conflict
   * between events that all have been folded into the same equivalence class, and
   * this equivalence class of events has more pre-conditions to express enabling.
   * 
   * In this case, the superfluous condition has become an implied condition. Call
   * this method repeatedly to remove all superfluous conditions.
   * 
   * @return <code>true</code> iff a superfluous condition was found and removed from
   * the branching process.
   */
  public boolean refineFutureEquivalence_removeSuperfluous () {
    
    // reference locally
    final Map<DNode, DNode> elementary_ccPair = futureEquivalence().getElementary_ccPair();
    
    for (Entry<DNode, Set<DNode>> cl : futureEquivalence().entrySet()) {
      DNode e = cl.getKey();
      if (!e.isEvent) continue;
      if (e.pre == null || e.pre.length <= 1) continue;
      
      HashSet<DNode> superFluousConflicts = new HashSet<DNode>();
      for (DNode b : e.pre) {
        if (b.post != null && b.post.length > 1) superFluousConflicts.add(elementary_ccPair.get(b));
      }
      
      for (DNode ePrime : cl.getValue()) {
        if (ePrime == e) continue;
        
        HashSet<DNode> superFluousConflictsPrime = new HashSet<DNode>();
        for (DNode bPrime : ePrime.pre) superFluousConflictsPrime.add(elementary_ccPair.get(bPrime));
        superFluousConflicts.retainAll(superFluousConflictsPrime);
      }
      
      for (DNode b : superFluousConflicts) {
        boolean allPostInCl = true;
        for (DNode bPost : b.post) {
          if (!cl.getValue().contains(bPost)) {
            allPostInCl = false;
            break;
          }
        }
        
        if (allPostInCl) {
          // found a former conflict between events that are now folded into one event
          // the conflict is superfluous
          
          boolean isSingleSuccessorOfSomeEvent = false;
          for (DNode bPrime : futureEquivalence().get(elementary_ccPair.get(b))) {
            if (bPrime.pre != null) {
              for (DNode bPre : bPrime.pre) {
                if (bPre.post.length == 1) {
                  isSingleSuccessorOfSomeEvent = true;
                  break;
                }
              }
            }
          }
          
          if (!isSingleSuccessorOfSomeEvent) {
            for (DNode bPrime : futureEquivalence().get(elementary_ccPair.get(b))) {
              this.bp.remove(bPrime);
            }
            return true;
          }
        }
      }
    }
    
    return false;
  }

  public boolean extendFutureEquivalence_deterministic() {
    
    // reference locally
    final Map<DNode, DNode> elementary_ccPair = futureEquivalence().getElementary_ccPair();
    
    boolean changed = false;
    
    LinkedList<DNode> overlapping = new LinkedList<DNode>();
    HashSet<DNode> to_join = new HashSet<DNode>();
    
    ArrayList<DNode> eventList = new ArrayList<DNode>();
    for (DNode e : futureEquivalence().keySet()) {
      if (!e.isEvent) continue;
      if (e.isAnti) continue;
      
      eventList.add(e);
    }    
    
    for (int k=0;k<eventList.size(); k++) {
      DNode e = eventList.get(k);
      // We update the equivalence relation in this loop. So it may
      // happen that some event in 'eventList' is no longer a canonical
      // event when we reach it. In this case, skip the event.
      if (elementary_ccPair.get(e) != e) continue;
      
      overlapping.clear();
      overlapping.add(e);
      
      for (int l=k+1; l<eventList.size(); l++) {
        DNode e2 = eventList.get(l);
        if (e.id != e2.id) continue;
        if (e.pre.length != e2.pre.length) continue;
        // We update the equivalence relation in this loop. So it may
        // happen that some event in 'eventList' is no longer a canonical
        // event when we reach it. In this case, skip the event.
        if (elementary_ccPair.get(e2) != e2) continue;
        
        for (int i=0; i<e.pre.length; i++) {
          if (futureEquivalence().areFoldingEquivalent(e.pre[i], e2.pre[i])) {
            overlapping.add(e2);
            break;
          }
        }
      }
      
      if (overlapping.size() > 1) {
        for (int i=0; i<e.pre.length; i++) {
          to_join.clear();
          for (DNode e2 : overlapping)
            to_join.add(elementary_ccPair.get(e2.pre[i]));
          futureEquivalence().joinEquivalenceClasses(to_join);
        }
        
        futureEquivalence().joinEquivalenceClasses(overlapping);
        
        for (int i=0; i<e.post.length; i++) {
          to_join.clear();
          for (DNode e2 : overlapping)
            to_join.add(elementary_ccPair.get(e2.post[i]));
          futureEquivalence().joinEquivalenceClasses(to_join);
        }
        changed = true;
      }
    }

    return changed;
  }
  
  public Collection<Short> simplifyFutureEquivalence() {
    
    // reference locally
    final Map<DNode, DNode> elementary_ccPair = futureEquivalence().getElementary_ccPair();

    
    HashSet<Short> complexCond = new HashSet<Short>();

    for (Entry<DNode, Set<DNode>> cl : futureEquivalence().entrySet()) {
      DNode d = cl.getKey();
      if (d.isAnti) continue;
      if (d.isEvent) continue;
      
      HashSet<DNode> dPre = new HashSet<DNode>();
      HashSet<DNode> foldedPre = new HashSet<DNode>();
      for (DNode dPrime : cl.getValue()) {
        if (dPrime.pre == null) continue;
        for (DNode pre : dPrime.pre) {
          foldedPre.add(elementary_ccPair.get(pre));
          dPre.addAll(futureEquivalence().get(elementary_ccPair.get(pre)));
        }
      }
      
      if (foldedPre.size() > 0 && dPre.size()/foldedPre.size() > 2)
        complexCond.add(d.id);

      System.out.println(d+" has complexity "+dPre.size()+"/"+foldedPre.size());
    }
    return complexCond;
  }
  
   /**
   * The equivalence relation on events and conditions {@link #elementary_ccPair}
   * is generated based on local knowledge and may be too coarse, e.g. by the
   * {@link #minimizeFoldingRelation()} function. We occasionally set two events as equivalent
   * while their pre-conditions are not pairwise equivalent. For these events
   * we have to drop the equivalence relation. This is done by this method.
   */
  public void relaxFutureEquivalence() {
    relaxFutureEquivalence(EquivalenceRefineLabel.instance);
  }
  
  /**
   * The equivalence relation on events and conditions
   * {@link #elementary_ccPair} is generated based on local knowledge and may be
   * too coarse, e.g. by the {@link #minimizeFoldingRelation()} function. We
   * occasionally set two events as equivalent while their pre-conditions are
   * not pairwise equivalent. For these events we have to drop the equivalence
   * relation. This is done by this method.
   * 
   * @param splitter
   *          the equivalence splitter that is used to refine equivalence
   *          classes, the given splitter may implement another equivalence
   *          criterion that the default {@link EquivalenceRefineLabel}
   */
  public void relaxFutureEquivalence(IEquivalentNodesRefine splitter) {
    
    // --------------------------------------------------------------------
    // split equivalence classes by labels
    // --------------------------------------------------------------------
    
    // compute splitted equivalence classes
    LinkedList<Set<DNode>> equiv_to_set = new LinkedList<Set<DNode>>();
    
    for (Entry<DNode, Set<DNode>> cl : futureEquivalence().entrySet()) {

      Collection<Set<DNode>> refined = splitter.splitIntoEquivalenceClasses(cl.getValue());
      // split only if there is something to split
      if (refined.size() > 1) equiv_to_set.addAll(refined);
    }
    
    // update the equivalence classes as computed earlier
    for (Set<DNode> cl_new : equiv_to_set) {
      //Uma.out.println("splitting by successors: "+cl_new);
      futureEquivalence().splitFoldingEquivalence(cl_new);
    }
    
    // --------------------------------------------------------------------
    // split equivalence classes of events by pre-sets
    // --------------------------------------------------------------------
    
    // compute splitted equivalence classes
    equiv_to_set.clear();
    for (Entry<DNode, Set<DNode>> cl : futureEquivalence().entrySet()) {
      // split only events
      if (!cl.getKey().isEvent) continue;
      
      HashMap<DNode[], HashSet<DNode>> preSetClasses = new HashMap<DNode[], HashSet<DNode>>();
      for (DNode d : cl.getValue()) {
        
        DNode[] d_preSet_equiv = null;
        for (DNode[] preSet : preSetClasses.keySet()) {
          if (preSet.length != d.pre.length) continue;
          
          int i=0;
          for (; i < d.pre.length; i++) {
            if (!futureEquivalence().areFoldingEquivalent(d.pre[i], preSet[i]))
              break;
          }
          if (i == d.pre.length) {
            // each pre-decessor of 'd' is equivalent to the corresponding predecessor
            // of 'preSet'
            d_preSet_equiv = preSet;
          }
        }
        
        if (d_preSet_equiv == null) {
          // equivalence class of the pre-set of 'd' is not represented yet
          //  in 'preSetClasses', create a new equivalence class
          d_preSet_equiv = d.pre;
          preSetClasses.put(d_preSet_equiv, new HashSet<DNode>());
        }
        // 'd' belongs to the subequivalence class of its pre-set
        preSetClasses.get(d_preSet_equiv).add(d);
      }
      
      if (preSetClasses.keySet().size() > 1) {
        // there are several nodes with different labels in the same equivalence class
        // we cannot fold them to the same node, so split the class
        //System.out.println("splitting equivalence class "+cl.getValue() +" by pre-sets");
        for (Entry<DNode[], HashSet<DNode>> sub_cl : preSetClasses.entrySet()) {
          // each set 'sub_cl.getValue()' becomes a new equivalence class
          equiv_to_set.add(sub_cl.getValue());
        }
      }
    }
    
    // update the equivalence classes as computed earlier
    for (Set<DNode> cl_new : equiv_to_set) {
      //Uma.out.println("splitting "+cl_new);
      futureEquivalence().splitFoldingEquivalence(cl_new);
    }
  }

  /*
   * (non-Javadoc)
   * @see hub.top.uma.DNodeBP#computeFutureEquivalence()
   */
  @Override
  protected void initializeFutureEquivalence() {

    // basic initialization
    super.initializeFutureEquivalence();
    
    // relax equivalence relations
    relaxFutureEquivalence();
    
    // extend future equivalence forward (beyond the prime configurations)
    // identified by the cut-off criteria
    // required for synthesis, check with simplification
    if (getOptions().folding_extendEquivalence_forward)
      extendFutureEquivalence_forward(); 
    
    //extendFoldingEquivalence_maximal();
    //extendFoldingEquivalence_backwards();
    //relaxFoldingEquivalence();
  }
  
}
