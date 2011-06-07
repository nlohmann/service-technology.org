package hub.top.uma;

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
      //Uma.out.println("removed "+d);

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
          dSucc.add(elementary_ccPair.get(e));
        }
      }
      //Uma.out.println("  has successors: "+dSucc);
      
      int joint_size = dCl.size();    //
      
      for (DNode d2 : futureEquivalence().keySet()) {

        if (d == d2) continue;
        if (d2.isAnti) continue;
        if (d.id != d2.id) continue;
        if (futureEquivalence().areFoldingEquivalent(d, d2)) continue;
        
        //Uma.out.println("  comparing to "+d2);
        
        if (areConcurrent_struct(d, d2)) {
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
            d2Succ.add(elementary_ccPair.get(e2));
          }
        }
        //Uma.out.println("    has successors: "+d2Succ);
        
        // Check if we can join the classes of 'd' and 'd2': this is possible
        // if each successor of the class of 'd' has an equivalent successor of
        // the class of 'd2'. This is the case if each canonical representative
        // of a successor of 'd' is also a canonical representative of a 
        // successor of 'd2'.
        boolean all_dSucc_HaveEquivalent = true;
        for (DNode e : dSucc) {
          
          if (!d2Succ.contains(e)) {
            all_dSucc_HaveEquivalent = false;
            //Uma.out.println("    -> "+e+" not in post "+d2);
            break;
          }
          
          /*
          boolean e_has_equivalent = false;
          for (DNode d2Prime : d2Cl) {
            if (d2Prime.post == null) continue;
            for (int i = 0; i<d2Prime.post.length; i++) {
              if (d2Prime.post[i].isAnti) continue;
              // post-sets are ordered by IDs, nothing to find beyond this point
              if (d2Prime.post[i].id > e.id) break;     
              // post-sets are ordered by IDs, nothing to find here
              if (d2Prime.post[i].id != e.id) continue; 
              // same ID, check for equivalence
              if (elementary_ccPair.get(d2Prime.post[i].id) == e) {
                e_has_equivalent = true;
                break;
              }
            }
            if (e_has_equivalent) break;
          }
          if (!e_has_equivalent) {
            all_dSucc_HaveEquivalent = false;
            break;
          }*/
        }
        
        boolean all_d2Succ_HaveEquivalent = true;
        /*
        for (DNode e2 : d2Succ) {
          boolean e2_has_equiv = false;
          for (DNode e : dSucc) {
            if (areFoldingEquivalent(e2, e)) {
              e2_has_equiv = true;
              break;
            }
          }
          if (!e2_has_equiv) {
            all_d2Succ_HaveEquivalent = false;
            break;
          }
        }
        */
        for (DNode e2 : d2Succ) {
          
          if (!dSucc.contains(e2)) {
            all_dSucc_HaveEquivalent = false;
            //Uma.out.println("    -> "+e2+" not in post "+d);
            break;
          }
          
          /*
          boolean e2_has_equivalent = false;
          for (DNode dPrime : dCl) {
            if (dPrime.post == null) continue;
            for (int i = 0; i<dPrime.post.length; i++) {
              if (dPrime.post[i].isAnti) continue;
              // post-sets are ordered by IDs, nothing to find beyond this point
              if (dPrime.post[i].id > e2.id) break;     
              // post-sets are ordered by IDs, nothing to find here
              if (dPrime.post[i].id != e2.id) continue; 
              // same ID, check for equivalence
              if (elementary_ccPair.get(dPrime.post[i].id) == e2) {
                e2_has_equivalent = true;
                break;
              }
            }
            if (e2_has_equivalent) break;
          }
          if (!e2_has_equivalent) {
            all_d2Succ_HaveEquivalent = false;
            break;
          }*/
        }
        
        
        if (dSucc.isEmpty() && !d2Succ.isEmpty() || !dSucc.isEmpty() && d2Succ.isEmpty()) {
          all_dSucc_HaveEquivalent = false;
          all_d2Succ_HaveEquivalent = false;
        }
        
        if (all_dSucc_HaveEquivalent && all_d2Succ_HaveEquivalent) {
          to_join.add(d2);
          //Uma.out.println("  JOINING "+d+" and "+d2);
          joint_size += d2Cl.size();    //
        } else {
          //Uma.out.println("  NOT joining "+d+" and "+d2);
        }
        
        //if (!ignoreFoldThreshold && joint_size > foldThreshold * idFrequency[d.id]) break;      //
      }
      
      if (to_join.size() > 1) {
        futureEquivalence().joinEquivalenceClasses(to_join);
        changed = true;
        
        /* join pre-sets assuming ordered labels of predecessors
        
        // join the pre-set of all joined transitions
        LinkedList<DNode> preJoin = new LinkedList<DNode>();
        for (int i=0; i<d.pre.length; i++) {
          preJoin.clear();
          for (DNode d2 : to_join) {
            preJoin.add(elementary_ccPair.get(d2.pre[i]));
            // and add the predecessors of the joined pre-sets to the queue 
            if (d2.pre[i].pre != null) {
              for (DNode e : d2.pre[i].pre) {
                top.add(e);
              }
            }
          }
          joinEquivalenceClasses(preJoin);
        }
        top.removeAll(foldingEquivalence().get(elementary_ccPair.get(d)));
        */
        
        // fix the order of events to merge, so that each event has a unique index
        // the index will be used to relate pre-conditions of different events to each other
        DNode[] _to_join = new DNode[to_join.size()];
        _to_join = to_join.toArray(_to_join);
        
        /*
        int minPreSize = Integer.MAX_VALUE;
        int minIndex = -1;
        for (int i=0; i<_to_join.length; i++) {
          if (minPreSize > _to_join[i].pre.length) {
            minPreSize = _to_join[i].pre.length;
            minIndex = i;
          }
        }
        */
        
        int maxDirectPreSize = 0;
        int maxIndex = -1;
        for (int i=0; i<_to_join.length; i++) {
          int i_pre_direct = 0;
          for (int j=0; j<_to_join[i].pre.length; j++) {
            if (!_to_join[i].pre[j].isImplied) i_pre_direct++;
          }
          
          if (maxDirectPreSize < i_pre_direct) {
            maxDirectPreSize = i_pre_direct;
            maxIndex = i;
          }
        }
        
        // all pre-conditions of all events that we join
        // later, we remove all conditions that have been joined with some other
        // condition. the remaining conditions are implied conditions that are
        // no longer required in the net and hence will be removed
        HashSet<DNode> nonJoinedConditions = new HashSet<DNode>();
        for (DNode d2 : to_join) {
          // initialize, remove joined conditions afterwards
          for (DNode b : d2.pre) nonJoinedConditions.add(b); 
        }
        
        // collect conditions: for each condition of the event with the least pre-conditions
        // we have to find an equivalent condition of each other event
        DNode[][] joinNodes = new DNode[maxDirectPreSize][_to_join.length];
        
        int maxIndex_preIndex[] = new int[_to_join[maxIndex].pre.length];
        
        // Match all conditions with the same label: these are definitely equivalent.
        // The event with the least pre-conditions defines the distribution;
        // all other pre-conditions must be matched here.
        for (int j=0,j2=0; j<_to_join[maxIndex].pre.length; j++) {
          if (!_to_join[maxIndex].pre[j].isImplied) {
            maxIndex_preIndex[j] = j2;
            joinNodes[j2][maxIndex] = _to_join[maxIndex].pre[j];
            nonJoinedConditions.remove(joinNodes[j2][maxIndex]);
            j2++;
          } else {
            maxIndex_preIndex[j] = -1;
          }
        }
        
        for (int i=0; i<_to_join.length; i++) {
          if (i == maxIndex) continue;

          // find for each pre-condition of event i 
          for (int k=0; k<_to_join[i].pre.length; k++) {
            // the corresponding pre-condition of event maxIndex with the same label
            // if it exists
            for (int j=0; j<_to_join[maxIndex].pre.length; j++) {
              if (_to_join[maxIndex].pre[j].isImplied) continue;
              
              if (_to_join[i].pre[k].id == _to_join[maxIndex].pre[j].id) {
                joinNodes[maxIndex_preIndex[j]][i] = _to_join[i].pre[k];
                nonJoinedConditions.remove(joinNodes[maxIndex_preIndex[j]][i]);
                break;
              }
            }
          }
        }
        // now, we have found all pre-conditions of all events that match by label
        // next: find the missing conditions using Kiepuziewski et al
        
        for (int j=0; j<joinNodes.length; j++) {
          for (int i=0; i<joinNodes[j].length; i++) {
            // the j-th precondition of 'minIndex' already has an equivalent
            // precondition of event 'i', done
            if (joinNodes[j][i] != null) continue;
            
            // the j-th precondition of 'minIndex' has no equivalent
            // precondition of event 'i' yet. find one
            DNode toMatch = joinNodes[j][maxIndex];

            // check all preconditions of event 'i', first check the non-implied conditions
            LinkedList<DNode> matchConditions = new LinkedList<DNode>();
            for (DNode pre : _to_join[i].pre) {
              if (!nonJoinedConditions.contains(pre)) continue;
              if (areConcurrent_struct(toMatch, pre)) continue;
              if (!pre.isImplied) matchConditions.addLast(pre);
            }
            // then the implied conditions
            for (DNode pre : _to_join[i].pre) {
              if (!nonJoinedConditions.contains(pre)) continue;
              if (areConcurrent_struct(toMatch, pre)) continue;
              if (pre.isImplied) matchConditions.addLast(pre);
            }
            
            for (DNode matchCondition : matchConditions) {
              // check if 'toMatch' and 'matchCondition' are successor equivalent
              // get future equivalence classes
              DNode[] future_toMatch = EquivalenceRefineSuccessor.getSuccessorEquivalence(this, toMatch);
              DNode[] future_matchCon = EquivalenceRefineSuccessor.getSuccessorEquivalence(this, matchCondition);
              // and compare
              if (!Arrays.equals(future_toMatch, future_matchCon)) continue;
                
              // make 'toMatch' and 'matchCondition' equivalent
              joinNodes[j][i] = matchCondition;
              nonJoinedConditions.remove(matchCondition);
              break;
            }

            /*
            HashSet<DNode> grey = bp.getAllPredecessors(toMatch);
            // get all predecessors of the event for which we want to find the
            // precondition to match 'j'
            HashSet<DNode> potentiallyBlack = bp.getAllPredecessors(_to_join[i]);
            
            for (DNode greyNode : grey) {
              if (greyNode.isEvent) continue;
              
              for (DNode black : greyNode.post) {
                if (potentiallyBlack.contains(black)) {
                  for (DNode matchCondition : black.post) {
                    
                    // we can only match to implied conditions
                    if (!matchCondition.isImplied) continue;
                    if (!nonJoinedConditions.contains(matchCondition)) continue;
                    
                    // check if 'toMatch' and 'matchCondition' are successor equivalent
                    // get future equivalence classes
                    DNode[] future_toMatch = EquivalenceRefineSuccessor.getSuccessorEquivalence(this, toMatch);
                    DNode[] future_matchCon = EquivalenceRefineSuccessor.getSuccessorEquivalence(this, matchCondition);
                    // and compare
                    if (!Arrays.equals(future_toMatch, future_matchCon)) continue;
                      
                    // make 'toMatch' and 'matchCondition' equivalent
                    joinNodes[j][i] = matchCondition;
                    nonJoinedConditions.remove(matchCondition);
                  }
                }
              }
            }
            */
          }
        }
        

        
        for (int j=0; j<joinNodes.length; j++) {
          LinkedList<DNode> joinConditions = new LinkedList<DNode>();
          for (int i=0; i<joinNodes[j].length; i++) {
            if (joinNodes[j][i] != null) { 
              joinConditions.add(elementary_ccPair.get(joinNodes[j][i]));
              joinNodes[j][i].isImplied = false;

              // and add the predecessors of the joined pre-sets to the queue 
              if (joinNodes[j][i].pre != null) {
                for (DNode e : joinNodes[j][i].pre) {
                  top.add(e);
                }
              }
            }
          }
          futureEquivalence().joinEquivalenceClasses(joinConditions);
        }
        
        for (DNode b : nonJoinedConditions) {
          
          this.bp.remove(b);
          
          //System.out.println("non-joined: "+b);
        }

        
        top.removeAll(futureEquivalence().get(elementary_ccPair.get(d)));
      }
    }
    return changed;
  }
  
  public boolean extendFutureEquivalence_backwards_complete() {
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
      HashSet<DNode> dSucc = new HashSet<DNode>();
      HashSet<DNode> d2Succ = new HashSet<DNode>();
      
      //Uma.out.println("top: "+top);
      // reference locally
      final Map<DNode, DNode> elementary_ccPair = futureEquivalence().getElementary_ccPair();

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
        //Uma.out.println("removed "+d);

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
            dSucc.add(elementary_ccPair.get(e));
          }
        }
        //Uma.out.println("  has successors: "+dSucc);
        
        int joint_size = dCl.size();    //
        
        for (DNode d2 : futureEquivalence().keySet()) {

          if (d == d2) continue;
          if (d2.isAnti) continue;
          if (d.id != d2.id) continue;
          if (futureEquivalence().areFoldingEquivalent(d, d2)) continue;
          
          //Uma.out.println("  comparing to "+d2);
          
          Set<DNode> d2Cl = futureEquivalence().get(elementary_ccPair.get(d2));
          
          // collect the for each successor 'e2' of each node in the class of 'd2',
          // the canonical representative of 'e2'
          d2Succ.clear();
          for (DNode d2Prime : d2Cl) {
            if (d2Prime.post == null) continue;
            for (DNode e2 : d2Prime.post) {
              if (e2.isAnti) continue;
              d2Succ.add(elementary_ccPair.get(e2));
            }
          }
          //Uma.out.println("    has successors: "+d2Succ);
          
          // Check if we can join the classes of 'd' and 'd2': this is possible
          // if each successor of the class of 'd' has an equivalent successor of
          // the class of 'd2'. This is the case if each canonical representative
          // of a successor of 'd' is also a canonical representative of a 
          // successor of 'd2'.
          boolean all_dSucc_HaveEquivalent = true;
          for (DNode e : dSucc) {
            if (!d2Succ.contains(e)) {
              all_dSucc_HaveEquivalent = false;
              //Uma.out.println("    -1-> "+e+" not in post "+d2);
              //Uma.out.println("    -1-> "+DNode.toString(e.post));
              break;
            }
          }
          
          boolean all_d2Succ_HaveEquivalent = true;
          for (DNode e2 : d2Succ) {
            if (!dSucc.contains(e2)) {
              all_dSucc_HaveEquivalent = false;
              //Uma.out.println("    -2-> "+e2+" not in post "+d);
              //Uma.out.println("    -2-> "+DNode.toString(e2.post));
              break;
            }
          }
          
          
          if (dSucc.isEmpty() && !d2Succ.isEmpty() || !dSucc.isEmpty() && d2Succ.isEmpty()) {
            all_dSucc_HaveEquivalent = false;
            all_d2Succ_HaveEquivalent = false;
          }
          
          if (all_dSucc_HaveEquivalent && all_d2Succ_HaveEquivalent) {
            to_join.add(d2);
            //Uma.out.println("  JOINING "+d+" and "+d2);
            joint_size += d2Cl.size();    //
          } else {
            //Uma.out.println("  NOT joining "+d+" and "+d2);
          }
          
          //if (!ignoreFoldThreshold && joint_size > foldThreshold * idFrequency[d.id]) break;      //
        }
        
        if (to_join.size() > 1) {
          futureEquivalence().joinEquivalenceClasses(to_join);
          changed = true;
          
          // join the pre-set of all joined transitions
          LinkedList<DNode> preJoin = new LinkedList<DNode>();
          for (int i=0; i<d.pre.length; i++) {
            preJoin.clear();
            for (DNode d2 : to_join) {
              preJoin.add(elementary_ccPair.get(d2.pre[i]));
              // and add the predecessors of the joined pre-sets to the queue 
              if (d2.pre[i].pre != null) {
                for (DNode e : d2.pre[i].pre) {
                  top.add(e);
                }
              }
            }
            futureEquivalence().joinEquivalenceClasses(preJoin);
          }
          
          top.removeAll(futureEquivalence().get(elementary_ccPair.get(d)));
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
      Uma.out.println("splitting by successors: "+cl_new);
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
      Uma.out.println("splitting "+cl_new);
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
