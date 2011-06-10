package hub.top.uma;

import java.util.Collection;
import java.util.HashMap;
import java.util.Map;
import java.util.Set;
import java.util.Map.Entry;

import com.google.gwt.dev.util.collect.HashSet;

public class FutureEquivalence implements Map<DNode, Set<DNode>> {
  
  /**
   * Maps each cut-off event and each post-condition of a cut-off event to its
   * smallest equivalent event (condition) of the branching process.
   * 
   * Two nodes are equivalent iff their futures in the full (infinite)
   * unfolding are isomorphic. Cut-off events that are equivalent to the
   * initial cut have no entry in this map.
   * 
   * This mapping is updated by {@link #setCutOffEvent(DNode, DNode)} and
   * {@link #setCutOffEvent(DNode[], DNode[])} and is used for computing foldings
   * of the McMillan prefix ({@link #minimizeFoldingRelation()} and {@link #relaxFutureEquivalence()}) and
   * for deciding behavioral properties ({@link #checkProperties()}).
   */
  final private Map<DNode, DNode> elementary_ccPair;
  
  /**
   * Represents the folding equivalence classes of the folding equivalence.
   * Each canonical node 'd' is assigned the set of all nodes equivalent nodes
   * that can reached 'd' via {@link #elementary_ccPair}. Is set by
   * {@link #buildFoldingEquivalence()}
   */
  final private HashMap<DNode, Set<DNode> > foldingEquivalenceClasses;
  
  public FutureEquivalence() {
    this.elementary_ccPair = new HashMap<DNode, DNode>();
    this.foldingEquivalenceClasses = new HashMap<DNode, Set<DNode>>();
  }
  
  /**
   * @return the mapping of cutOff events to their equivalent counter parts
   * 
   * TODO: method is public to be called from extending plugins, check consistency of other fields when the method is re-used in another context
   */
  public Map<DNode, DNode> getElementary_ccPair() {
    // FIXME: return only after computation finished?
    return elementary_ccPair;
  }
  
  /**
   * Update the mapping that relates each new cutOff event to its equivalent
   * old event. If the equivalent event is a cutOff event as well, follow the
   * equivalence relation transitively to the first event.
   * 
   * @param newNode
   *     the new event that is a cutOff event
   * @param existing
   *     the old event to which <code>newEvent</code> is equivalent
   *     
   * TODO: method is protected to be called from extending plugins, check consistency of other fields when the method is re-used in another context
   */
  protected void putElementaryEquivalent(DNode newNode, DNode existing) {
    DNode _existing = getElementary_ccPair().get(existing);
    if (_existing == null)
      getElementary_ccPair().put(newNode, existing);
    else
      getElementary_ccPair().put(newNode, _existing);
  }

  /**
   * The equivalence relation on events and conditions {@link #elementary_ccPair}
   * assigns to each node of the branching process an equivalent node with the
   * same future. This method builds the explicit equivalence classes of this relation.
   */
  public void completeElementaryEquivalence(Iterable<DNode> allNodes) {
    
    // update the equivalence relation to point for each node to its canonic
    // equivalent node (i.e. the smallest node), and populate the equivalence classes
    for (DNode d : allNodes) {
      // create for each node an equivalence class
      if (!elementary_ccPair.containsKey(d)) {
        elementary_ccPair.put(d, d);
      } else {
        // get the least equivalent node of 'd':
        // collect all nodes reachable downwards via elementary_ccPair
        HashSet<DNode> equivSet = new HashSet<DNode>();
        equivSet.add(d);
        DNode dOther = elementary_ccPair.get(d);
        while (dOther != null && !equivSet.contains(dOther)) {
          equivSet.add(dOther);
          dOther = elementary_ccPair.get(dOther);
        }
        // and take from all reachable nodes the node with the least id
        dOther = d;
        for (DNode d2 : equivSet) {
          if (d2.globalId < dOther.globalId) dOther = d2;
        }
        
        elementary_ccPair.put(d, dOther);
      }
    }
  }
   
    
  public void initFutureEquivalenceClasses() {
    // build the equivalence classes of the equivalence relation
    for (DNode d : elementary_ccPair.keySet()) {
      if (!foldingEquivalenceClasses.containsKey(elementary_ccPair.get(d))) {
        setFoldingEquivalenceClass(elementary_ccPair.get(d), new HashSet<DNode>());
      }
      foldingEquivalenceClasses.get(elementary_ccPair.get(d)).add(d);
    }
  }
  

  /**
   * @param d1
   * @param d2
   * @return <code>true</code> iff d1 and d2 belong to the same equivalence class
   * of the folding relation
   */
  public boolean areFoldingEquivalent(DNode d1, DNode d2) {
    return getElementary_ccPair().get(d1) == getElementary_ccPair().get(d2);
  }
  
  /**
   * @param cl
   * @return <code>true</code> iff the set 'cl' is an equivalence class of
   * the {@link #foldingEquivalenceClass}
   */
  public boolean areFoldingEquivalenceClass(HashSet<DNode> cl) {
    if (cl.isEmpty()) return true;
    DNode d = cl.iterator().next();
    DNode c = getElementary_ccPair().get(d);
    
    return foldingEquivalenceClasses.get(c).equals(cl);
  }
  
  /**
   * Split the {@link #foldingEquivalenceClass}es. The nodes 'equiv_to_set'
   * must be equivalent already. Make all nodes in 'equiv_to_set' a new
   * equivalence class.
   * 
   * @param equiv_to_set
   */
  public void splitFoldingEquivalence(Set<DNode> equiv_to_set) {
    
    if (equiv_to_set.isEmpty()) return;
    
    DNode someNode = equiv_to_set.iterator().next();
    
    // all nodes of the current equivalence class of 'equiv_to_set' which
    // will remain in the old equivalence class
    HashSet<DNode> otherNodes = new HashSet<DNode>();
    for (DNode d : foldingEquivalenceClasses.get(getElementary_ccPair().get(someNode))) {
      if (!equiv_to_set.contains(d)) {
        otherNodes.add(d);
      }
    }
    
    updateFoldingEquivalence(equiv_to_set);
    updateFoldingEquivalence(otherNodes);
  }

  /**
   * Join the equivalence classes of all nodes in 'to_be_equivalent' 
   * into one new equivalence class.
   * 
   * @param to_be_equivalent
   */
  public void joinEquivalenceClasses(Collection<DNode> to_be_equivalent) {
    System.out.println("join: "+to_be_equivalent);
    
    // get for each node of 'to_be_equivalent' all its equivalent nodes
    // and put them in:
    HashSet<DNode> newEquivalenceClass = new HashSet<DNode>();
    for (DNode d : to_be_equivalent) {
      newEquivalenceClass.addAll(foldingEquivalenceClasses.get(getElementary_ccPair().get(d)));
    }
    updateFoldingEquivalence(newEquivalenceClass);
  }
  
  /**
   * Update the folding equivalence to make all nodes in 'new_class' an
   * equivalence class. This method does not update other equivalence classes. To
   * ensure consistency, use {@link #splitFoldingEquivalence(HashSet)} or
   * {@link #joinEquivalenceClasses(HashSet)}.
   * 
   * @param new_class
   */
  private void updateFoldingEquivalence(Set<DNode> new_class) {
    
    if (new_class.isEmpty()) return;
     
    // determine the least node in this class as canonical representative
    DNode sub_d_min = null;
    for (DNode sub_d : new_class) {
      if (sub_d_min == null) sub_d_min = sub_d;
      else if (sub_d.globalId < sub_d_min.globalId) sub_d_min = sub_d;
    }
    
    // update the mappings for all nodes in the new equivalence class and set the
    // explicit equivalence class
    for (DNode d : new_class) {
      // set equivalent 
      getElementary_ccPair().put(d, sub_d_min);
      // clear old equivalence class information
      foldingEquivalenceClasses.remove(d);
    }
    // set new equivalence class information
    setFoldingEquivalenceClass(sub_d_min, new_class);    
  }
  
  /**
   * Define a new folding equivalence class: the nodes 'folding_class' are equivalent,
   * the node 'canonical' is a member of 'folding class', i.e., 
   * <code>folding_class.contains(canonical) == true</code>, and represents the canonical
   * node of this class s.t. <code>elementary_ccPair.get(d) == canonical</code> for each
   * node 'd' of 'folding_class'.
   * 
   * This method must be called in a proper context, i.e., when initializing 
   * {@link #foldingEquivalenceClass} in {@link #buildFoldingEquivalence()} or when
   * properly updating the folding equivalence in {@link #updateFoldingEquivalence(HashSet)}.
   * 
   * @param canonical
   * @param folding_class
   */
  private void setFoldingEquivalenceClass(DNode canonical, Set<DNode> folding_class) {
    if (canonical == null) {
      System.err.println("trying to set invalid folding equivalence class: "+folding_class);
    }
    foldingEquivalenceClasses.put(canonical, folding_class);
  }

  public void clear() {
    foldingEquivalenceClasses.clear();
  }

  public boolean containsKey(Object key) {
    return foldingEquivalenceClasses.containsKey(key);
  }

  public boolean containsValue(Object value) {
    return foldingEquivalenceClasses.containsValue(value);
  }

  public Set<java.util.Map.Entry<DNode, Set<DNode>>> entrySet() {
    return foldingEquivalenceClasses.entrySet();
  }

  public Set<DNode> get(Object key) {
    return foldingEquivalenceClasses.get(key);
  }

  public boolean isEmpty() {
    return foldingEquivalenceClasses.isEmpty();
  }

  public Set<DNode> keySet() {
    return foldingEquivalenceClasses.keySet();
  }

  public Set<DNode> put(DNode d, Set<DNode> equivSet) {
    return foldingEquivalenceClasses.put(d, equivSet);
  }

  public void putAll(Map<? extends DNode, ? extends Set<DNode>> arg0) {
    foldingEquivalenceClasses.putAll(arg0);
  }

  public Set<DNode> remove(Object key) {
    return foldingEquivalenceClasses.remove(key);
  }

  public int size() {
    return foldingEquivalenceClasses.size();
  }

  public Collection<Set<DNode>> values() {
    return foldingEquivalenceClasses.values();
  }

  public void _debugPrintCutOffPairs() {
    System.out.println("----- CC-pairs -----");
    for (Entry<DNode, DNode> cc : getElementary_ccPair().entrySet()) {
      System.out.println(cc.getKey()+" --> "+cc.getValue());
    }
    System.out.println("---- /CC-pairs -----");
  }
  
  /**
   * print which events and which conditions are equivalent according
   * to {@link #elementary_ccPair}.
   */
  public void _debug_printFoldingEquivalence() {
    System.out.println("--- equivalence relation ---");
    System.out.println("EVENTS:");
    for (DNode e : this.keySet()) {
      if (e.isEvent)
        System.out.println(e+": "+this.get(e));
    }
    System.out.println("CONDITIONS:");
    for (DNode b : this.keySet()) {
      if (!b.isEvent)
        System.out.println(b+": "+this.get(b));
    }
    System.out.println("--- /equivalence relation ---");
  }
  
}
