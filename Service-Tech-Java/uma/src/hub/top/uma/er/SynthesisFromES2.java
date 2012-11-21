package hub.top.uma.er;

import hub.top.petrinet.ISystemModel;
import hub.top.petrinet.PetriNet;
import hub.top.petrinet.Place;
import hub.top.petrinet.Transition;
import hub.top.petrinet.unfold.DNodeSys_OccurrenceNet;
import hub.top.scenario.OcletIO_Out;
import hub.top.uma.DNode;
import hub.top.uma.DNodeRefold;
import hub.top.uma.DNodeSet.DNodeSetElement;
import hub.top.uma.DNodeSys;
import hub.top.uma.InvalidModelException;
import hub.top.uma.Uma;
import hub.top.uma.synthesis.NetSynthesis;
import hub.top.uma.synthesis.TransitiveDependencies;
import hub.top.uma.view.ViewGeneration2;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.LinkedHashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.TreeSet;

import com.google.gwt.dev.util.collect.HashMap;
import com.google.gwt.dev.util.collect.HashSet;

public class SynthesisFromES2 {
  
  private String properNames[];
  private String path;
  
  public SynthesisFromES2(String properNames[], String path) {
    this.properNames = properNames;
    this.path = path;
  }
  
  class Pair {
    Set<Event> first;
    Set<Event> second;

    public Pair(Set<Event> f, Set<Event> s) { first = f; second = s; }
    public Set<Event> getFirst() { return first; }
    public Set<Event> getSecond() { return second; }
    public void setFirst(Set<Event> val) { first = val; }
    public void setSecond(Set<Event> val) { second = val; }
    public String toString() {
      return String.format("(%s,%s)", first, second);
    }
    public boolean equals(Object obj) {
      if (obj == null) return false;
      if (!(obj instanceof Pair)) return false;
      Pair that = (Pair) obj;

      return this.first.equals(that.first) && this.second.equals(that.second);
    }
    public int hashCode() {
      return (first == null ? 0 : first.hashCode()) + (second == null ? 0 : second.hashCode() * 37);
    }
    public Object clone() {
      return new Pair(first, new HashSet<Event>(second));
    }
  }
  
  

  
  private static String toString(int[] arr) {
    if (arr == null) return "null";
    String r = "[";
    for (int i = 0; i<arr.length; i++) {
      if (i > 0) r += ", ";
      r += arr[i];
    }
    r += "]";
    return r;
  }
  
  private void getConflictCluster(EventStructure es, Set<Event> current, Set<Set<Event>> cc) {
    
    boolean isConflictCluster = true;
    for (Event e : current) {
      
      boolean withAllInConflict = true;
      for (Event f : current) {
        if (e == f) continue;
        if (!es.inDirectConflict(e, f)) {
          withAllInConflict = false;
          break;
        }
      }
      if (!withAllInConflict) {
        isConflictCluster = false;
        // recurse down
        Set<Event> reduced = new HashSet<Event>();
        for (Event f : current) {
          if (f != e) reduced.add(f);
        }
        getConflictCluster(es, reduced, cc);
      }
    }
    
    if (isConflictCluster) {
      
      boolean alreadyContained = false;
      for (Set<Event> other : cc) {
        if (other.containsAll(current)) {
          alreadyContained = true;
          break;
        }
      }
      if (!alreadyContained)
        System.out.println("adding "+current+" as cc");
        cc.add(current);
    }
  }
  
  
  public Set<Event> jointPreEvents(EventStructure es, Set<Event> cc) {
    Set<Event> joint = null;
    
    for (Event e : cc) {
      if (joint == null) joint = es.getPrimeConfiguration(e, false);
      else {
        joint.retainAll(es.getPrimeConfiguration(e, false));
      }
    }
    
    Set<Event> maxEvents = new HashSet<Event>();
    for (Event e : joint) {
      boolean hasPostInJoint = false;
      if (e.post != null)
        for (DNode ePost : e.post) {
          if (joint.contains(ePost)) {
            hasPostInJoint = true;
            break;
          }
        }
      if (!hasPostInJoint) {
        maxEvents.add(e);
      }
    }
    
    return maxEvents;
  }
  
  public boolean isImplict(EventStructure es, Event pre, Set<Event> cc) {
    for (Event post : cc) {
      //System.out.println(pre+" -->+ "+post+"?");
      if (!es.isTransitiveDependent(pre, post)) {
        //System.out.println("no");
        return false;
      }
    }
    return true;
  }
  
  public boolean isImplict(EventStructure es, Set<Event> pre, Set<Event> cc) {
    for (Event post : cc) {
      //System.out.println(pre+" -->+ "+post+"?");
      for (Event e : pre) {
        if (!es.isTransitiveDependent(e, post)) {
          //System.out.println("no");
          return false;
        }
      }
    }
    return true;
  }
  
  private void addPlace(PetriNet pnet, EventStructure es, Set<Event> e, Set<Event> cc, Set<Place> implicitPlaces, Set<Place> implicitPlaces_2ormore, Map<Pair, Place> B, Map<Place, Pair> B2) {
    Pair pair = new Pair(e, cc);
    
    String srcTransitionLabel = "";
    for (Event f : e) {
      String tgtTransitionLabel = properNames[f.id];
      srcTransitionLabel += ","+tgtTransitionLabel;
      
      //System.out.println(f+" maps to "+E.get(f));
    }

    // translate the IDs of the defining events to the labels
    // so that conditions are consistently labeled throughout the occurrence net
    // then we can identify equivalent conditions based on their labels
    String placeLabel = "("+srcTransitionLabel;
    
    int successors = 0;
    //String placeLabel = "(";
    for (Event f : cc) {
      String tgtTransitionLabel = properNames[f.id];
      placeLabel += ","+tgtTransitionLabel;
      successors++;
      
      //System.out.println(f+" maps to "+E.get(f));
    }
    
    placeLabel += ")";
    
    Place place = pnet.addPlace(placeLabel);
    B.put(pair, place);
    B2.put(place, pair);
    //System.out.printf("<%s, %s>\n", e, cc);

    boolean isImplicit = isImplict(es, e, cc);

    if (isImplicit) {
      if (successors == 1) implicitPlaces.add(place);
      else implicitPlaces_2ormore.add(place);
      System.out.printf("implicit "+placeLabel+"\n");
    }
  }
  
  private boolean isAValidFoldedCluster(EventStructure es, Set<Event> cc) {
    
    if (cc.size() == 1) return true;
    
    for (Event e : cc) {
      for (Event e_equiv : es.equiv.get(e)) {
        
        if (!es.directConflict.containsKey(e_equiv)) {
          return false;
        }
        
        HashSet<Event> e_confl_equiv = new HashSet<Event>();
        for (Event f : es.directConflict.get(e_equiv)) {
          e_confl_equiv.add(es.canonical.get(f));
        }
        
        for (Event e_confl : cc) {
          if (e == e_confl) continue;

          if (!e_confl_equiv.contains(e_confl)) {
            return false;
          }
        }
      }
    }
    return true;
  }
  
  public PetriNet synthesize(EventStructure es) {
    
    Map<Event, Set<Event>> equiv = es.equiv;
    Map<Event, Event> canonical = es.canonical;
    
    PetriNet pnet = new PetriNet();

    System.out.println("generate events");
    
    // ---------------------------------------------------------
    // 2. COMPUTE CE
    // ---------------------------------------------------------
    
    System.out.println("generate conflict clusters");
    Set<Set<Event>> CC = es.getConflictClusters();
    
    Set<Set<Event>> CC_equiv = new HashSet<Set<Event>>();
    for (Set<Event> cc : CC) {
      HashSet<Event> cc_equiv = new HashSet<Event>();
      for (Event e : cc) {
        cc_equiv.add(canonical.get(e));
      }
      CC_equiv.add(cc_equiv);
    }
    
    System.out.println("---------- conflict clusters equiv ------------");
    for (Set<Event> cc : CC_equiv) {
      System.out.println(cc);
    }
    System.out.println("---------- /conflict clusters equiv ------------");
    
    // ---------------------------------------------------------
    // 1. Initialize E
    // ---------------------------------------------------------
    Map<Event, Transition> E = new LinkedHashMap<Event, Transition>();
    Map<Transition, Event> E2 = new LinkedHashMap<Transition, Event>();
    Map<Integer, Event> E_index = new LinkedHashMap<Integer, Event>();
    
    System.out.println("---------- generating transitions ------------");
    for (Event n: equiv.keySet()) {
      
      String transitionLabel = properNames[n.id];
      E_index.put(n.globalId, n);
      Transition t = pnet.addTransition(transitionLabel);
      E.put(n, t);
      E2.put(t, n);
      
      System.out.println(equiv.get(n)+" --> "+t);
    }
    
    //System.out.println(reducedFlow);
    //System.out.println(implicitFlow);
    
    // implicit places with one successor
    Set<Place> implicitPlaces = new HashSet<Place>();
    // implicit places with 2 or more successors
    Set<Place> implicitPlaces_2ormore = new HashSet<Place>();
    
    System.out.println("generate conditions");
    
    // ---------------------------------------------------------
    // 3. COMPUTE B
    // ---------------------------------------------------------

    Map<Set<Event>, Set<Event>> dependencyMap = new HashMap<Set<Event>, Set<Event>>();
    for (Event e : es.allEvents) {
      for (Set<Event> cc : CC) {
        
        Set<Event> jointPreEvents = jointPreEvents(es, cc);
        
        /*
        boolean isCondition = true;
        boolean isImplicit = true;
        for (Integer e2 = ce.nextSetBit(0);  e2 >= 0; e2 = ce.nextSetBit(e2 + 1)) {
          Event _e2 = E_index.get(e2);
          if (!es.getPrimeConfiguration(_e2, false).contains(e)) isCondition = false;
          if (!_e2.hasPred(e)) isImplicit = false;
          
          if (!isCondition) break;
        }
        */
        
        /* produces only conditions for conflicts which root in direct pre-events
        boolean isCondition = true;
        boolean isImplicit = false;
        for (Event f : cc) {
          if (!f.hasPred(e)) isCondition = false;
          if (!isCondition) break;
        }
        */
        
        boolean isCondition = jointPreEvents.contains(e);
        boolean isImplicit = isImplict(es, e, cc);
        if (isCondition && !isImplicit) {
          Set<Event> cc_equiv = new HashSet<Event>();
          for (Event e_cc : cc) cc_equiv.add(canonical.get(e_cc));
          
          if (isAValidFoldedCluster(es, cc_equiv)) {
            if (!dependencyMap.containsKey(cc_equiv)) dependencyMap.put(cc_equiv, new HashSet<Event>());
            dependencyMap.get(cc_equiv).add(canonical.get(e));
          }
        }
      }
    }
      
      /*
      if (e.post != null) {
        for (DNode ePost : e.post) {
          if (es.isTransitiveDependent(e, (Event)ePost)) {
            Set<Event> cc = new HashSet<Event>();
            
            cc.add((Event)ePost);
            addPlace(pnet, es, e, cc, implicitPlaces, implicitPlaces_2ormore, B, B2);
          }
        }
      }
      */
      
      
      Map<Set<Event>, List<Set<Event>>> places = new HashMap<Set<Event>, List<Set<Event>>>();
      
      for (Map.Entry<Set<Event>,Set<Event>> dep : dependencyMap.entrySet()) {
        
        //System.out.println(dep.getValue() +" --> "+dep.getKey());
        
        Event[] pre_events = dep.getValue().toArray(new Event[dep.getValue().size()]);
        boolean co[][] = new boolean[pre_events.length][pre_events.length];
        for (int i=0; i<pre_events.length; i++) {
          for (int j=0; j<pre_events.length; j++) {
            if (j == i) co[i][j] = true;
            co[i][j] = es.inConflict(pre_events[i], pre_events[j]);
            //if (i < j) System.out.println(pre_events[i]+" # "+pre_events[j]+" == "+co[i][j]);
          }
        }
        
        //BronKerbosch bk_confl = new BronKerbosch();
        //TreeSet<int[]> cosets = bk_confl.findCliques(co);
        
        List<Set<Event>> confs = es.getConflictClusters(dep.getValue(), false);
        
        for (Set<Event> _preSet: confs) {
          //System.out.println("  "+_preSet +" --> "+dep.getKey());
          if (!places.containsKey(_preSet)) places.put(_preSet, new ArrayList<Set<Event>>());
          places.get(_preSet).add(dep.getKey());
        }
        
        Set<Event> addMissing = new HashSet<Event>();
        for (Set<Event> _preSet: confs) {
          for (Event preEvent : _preSet) {

            for (Event e : dep.getKey()) {
              boolean add_to_others = false;
              for (Event eEquiv : equiv.get(e)) {
                boolean hasPre = false;
                if (eEquiv.pre != null)
                  for (int i=0; i<eEquiv.pre.length; i++) {
                    if (eEquiv.pre[i].id == preEvent.id) {
                      hasPre = true;
                    }
                  }
                
                if (!hasPre) {
                  //System.out.println(">> "+preEvent+" of "+_preSet+" is not known to "+eEquiv);
                  add_to_others = true;
                }

              }
              
              if (add_to_others) {
                addMissing.add(preEvent);
              }
            }
          }
        }
        
        for (Event missing : addMissing) {
          for (Set<Event> _preSet: confs) {
            _preSet.add(missing);
          }
        }

        
        for (Set<Event> _preSet: confs) {
          //System.out.println("  "+_preSet +" --> "+dep.getKey());
          if (!places.containsKey(_preSet)) places.put(_preSet, new ArrayList<Set<Event>>());
          places.get(_preSet).add(dep.getKey());
        }

        /*
        int[][] _cosets = cosets.toArray(new int[cosets.size()][]);
        List<int[]> preSets = getAllCombinations(_cosets);
        for (int[] preSet : preSets) {
          Set<Event> _preSet = new HashSet<Event>();
          for (int i=0; i<preSet.length; i++) {
            _preSet.add(pre_events[preSet[i]]);
          }
          System.out.println("  "+_preSet +" --> "+dep.getKey());
        }*/
      }
      
      Map<Pair, Place> B = new LinkedHashMap<Pair, Place>();
      Map<Place, Pair> B2 = new LinkedHashMap<Place,Pair>();

      for (Set<Event> preSet : places.keySet()) {

        // filter subsumed places
        List<Set<Event>> postSets = new ArrayList<Set<Event>>();
        for (Set<Event> postSet : places.get(preSet)) {
          boolean subsumed = false;
          for (Set<Event> postSet2 : places.get(preSet)) {
            if (postSet2 == postSet) continue;
            if (postSet2.containsAll(postSet)) {
              subsumed = true;
              break;
            }
          }
          if (!subsumed) postSets.add(postSet);
        }
        
        for (Set<Event> postSet : postSets) {
          //System.out.println("  "+preSet +" -nonsubSumed-> "+postSet);
          addPlace(pnet, es, preSet, postSet, implicitPlaces, implicitPlaces_2ormore, B, B2);
        }
      }
    
    System.out.println("generate arcs");

    // ---------------------------------------------------------
    // 4. COMPUTE F
    // ---------------------------------------------------------
    for (Pair pair: B.keySet()) {
      Place place = B.get(pair);
      Set<Event> pre_events = pair.getFirst();
      Set<Event> cc = pair.getSecond();
      for (Event f : cc) {
        if (canonical.get(f) != f) continue;
        
        Transition trans = E.get(f);
        pnet.addArc(place, trans);
      }
      for (Event e : pre_events) {
        Transition trans = E.get(e);
        pnet.addArc(trans, place);
      }
    }
    
    
    /*
    // --- Remove transitive conflicts
    // A condition b is a transitive conflict between two events e_1 and e_2, iff there exist events f_1 < e_1 and f_2 < e_2 that are in conflict.
    // A condition b is a transitive conflict, iff it is a transitive conflict between e_1 and e_2 for any two different post-events e_1, e_2 of b.
    // !!! And there exists a path from pre-event of b to every post event of b which does not contain b (causality is preserved) 
    // !!! In free-choice nets can be checked directly on postsets of conflict conditions
    
    Set<Place> transitiveConflicts = new HashSet<Place>();
    
    System.out.println("searching for transitive conflicts in "+pnet.getInfo(false));
    boolean iterate = true;
    while (iterate)
    {
      iterate = false;
      for (Place b: pnet.getPlaces()) { // check if b is a transitive conflict
        if (b.getPostSet().size()==1) continue; // b is not a conflict
        if (transitiveConflicts.contains(b)) continue;
        
        //System.out.println("checking "+b+" "+b.getPostSet());
        
        boolean is_tr_conflict = true;
        for (Transition e1 : b.getPostSet()) {
          for (Transition e2 : b.getPostSet()) {
            // for any two different post-events e_1, e_2 of b ...
            if (e1 == e2) continue;
            
            //System.out.println("   "+e1+","+e2+" transitive on "+b+"?");
            
            // ... b is a transitive conflict between e_1 and e_2
            is_tr_conflict &= isTransitiveConflict(b,e1,e2,pnet);
            //if (!is_tr_conflict) System.out.println("   "+e1+","+e2+" --> "+is_tr_conflict);
            
            if (!is_tr_conflict) break;
          }
          if (!is_tr_conflict) break;
        }
        
        if (is_tr_conflict && b.getPreSet().size() > 0) {
          //System.out.println("is transitive: "+b);
          
          Transition pre = b.getPreSet().get(0);
          boolean can_be_removed = true;
          for (Transition t : b.getPostSet()) {
            can_be_removed &= existsPathWithoutCondition(pre, t, b, pnet, transitiveConflicts);
            if (!can_be_removed) break;
          }
          
          if (can_be_removed) {
            //System.out.println("   remove "+b);
            //pnet.removePlace(b);
            transitiveConflicts.add(b);
            iterate = true;
            break;
          }
        }
      } 
    }
    
    for (Place place: transitiveConflicts) {
      System.out.println("removing transitive conflicts "+place);
      pnet.removePlace(place);
    }
    */
    
    LinkedList<Place> toRemove = new LinkedList<Place>();
    for (Place p : pnet.getPlaces()) {
      if (p.getIncoming().size() == 0 && p.getOutgoing().size() == 0) {
        toRemove.add(p);
      }
    }
    for (Place p : toRemove) {
      pnet.removePlace(p);
    }
    
    
    // --- Add source place

    Place initialPlace = pnet.addPlace("start");
    initialPlace.setTokens(1);

    for (Event e : E.keySet())
    {
      if (e.pre == null || e.pre.length == 0) {
        Transition src = E.get(e);
        pnet.addArc(initialPlace, src);
      }
      
      if (e.post == null || e.post.length == 0) {
        Transition trans = E.get(e);
        Place place = pnet.addPlace(trans.getName());
        pnet.addArc(trans, place);
      }
    }
    
    try {
      EventStructureMiner_BP.writeFile(pnet.toDot(), path+"_bp.dot");
    } catch (IOException e1) {
      // TODO Auto-generated catch block
      e1.printStackTrace();
    }
    
    System.out.println("folded to "+pnet.getInfo(false));
    
    return pnet;
  }
  
  private List<int[]> getAllCombinations(int[][] sets) {
    ArrayList<int[]> combinations = new ArrayList<int[]>();
    int [] comb = new int[sets.length];
    addCombination(sets, 0, comb, combinations);
    return combinations;
  }
  
  private void addCombination(int[][] sets, int level, int[] current, ArrayList<int[]> combinations) {
    if (level == current.length) {
      combinations.add(Arrays.copyOf(current, current.length));
      return;
    }
    for (int i=0; i<sets[level].length; i++) {
      current[level] = sets[level][i];
      addCombination(sets, level+1, current, combinations);
    }
  }
  
  
  
  private boolean existsPathWithoutCondition(Transition t1, Transition t2, Place b, PetriNet pnet, Set<Place> toRemove) {
    if (t1.equals(t2)) return true;
    Set<Transition> visited = new HashSet<Transition>();
    Set<Transition> heap = getPostTransitions(t1, b, toRemove);
    if (heap.contains(t2)) return true;
    
    while (heap.size()>0) {
      Transition tt = heap.iterator().next();
      heap.remove(tt);
      visited.add(tt);
      Set<Transition> post = getPostTransitions(tt, b, toRemove);
      if (post.contains(t2)) return true;
      post.removeAll(visited);
      heap.addAll(post);
    }
  
    return false;
  }
  
  private Set<Transition> getPostTransitions(Transition t) {
    Set<Transition> result = new HashSet<Transition>();
    
    for (Place p: t.getPostSet())
      result.addAll(p.getPostSet());
    
    return result;
  }
  
  private Set<Transition> getPostTransitions(Transition t, Place b, Set<Place> toRemove) {
    Set<Transition> result = new HashSet<Transition>();
    
    for (Place p: t.getPostSet())
      if (!p.equals(b) && !toRemove.contains(p))
        result.addAll(p.getPostSet());
    
    return result;
  }
  
  // A condition b is a transitive conflict between two events e_1 and e_2, iff there exist events f_1 < e_1 and f_2 < e_2 that are in conflict.
  private boolean isTransitiveConflict(Place b, Transition e1, Transition e2, PetriNet pnet) {
    
    // Two events with the same name are never in conflict, i.e.
    // two different post-events of condition b with the same name denote
    // the case where these events occur in alternative executions that
    // branched concurrently at condition b but now manifests on these
    // events. Keep the condition.

    if (e1.getName().equals(e2.getName())) {
      
      Set<Transition> directPre_e1 = new HashSet<Transition>();
      for (hub.top.petrinet.Arc a : e1.getIncoming()) {
        for (hub.top.petrinet.Arc a2 : a.getSource().getIncoming()) {
          directPre_e1.add((Transition)a2.getSource());
        }
      }
      Set<Transition> directPre_e2 = new HashSet<Transition>();
      for (hub.top.petrinet.Arc a : e2.getIncoming()) {
        for (hub.top.petrinet.Arc a2 : a.getSource().getIncoming()) {
          directPre_e2.add((Transition)a2.getSource());
        }
      }
      directPre_e1.retainAll(directPre_e2);
      
      for (Transition jointPre : directPre_e1) {
        if (!existsPathWithoutCondition(jointPre, e1, b, pnet, new HashSet<Place>()))
          return false;
        if (!existsPathWithoutCondition(jointPre, e2, b, pnet, new HashSet<Place>()))
          return false;
      }
    }
    
    Set<Place> prePlaces = new HashSet<Place>();
    LinkedList<Place> queue = new LinkedList<Place>();
    
    if (b.getIncoming().size() == 0) {
      prePlaces.add(b);
      queue.add(b);
    } else {
      Transition b_pre = b.getPreSet().get(0);
      for (Place b_sibling : b_pre.getPostSet()) {
        prePlaces.add(b_sibling);
        queue.add(b_sibling);
      }
    }
    
    
    while(!queue.isEmpty()) {
      Place p = queue.removeFirst();
      
      // explore all predecessor places of p, *(*(p))
      for (hub.top.petrinet.Arc a : p.getIncoming()) {
        for (hub.top.petrinet.Arc a2 : a.getSource().getIncoming()) {
          if (!prePlaces.contains(a2.getSource())) {
            prePlaces.add((Place)a2.getSource());
            queue.add((Place)a2.getSource());
          }
        }
      }
      
      // and check for the current place p, whether it defines a conflict for e1 and e2
      if (p == b || p.getPostSet().size()<=1) continue;
      //System.out.println("  on "+p);
      for (Transition f1 : p.getPostSet()) {
        for (Transition f2 : p.getPostSet()) {
          if (f1 == f2) continue;
          
          //System.out.println("  1: "+f1+" -> "+e1+" "+existsPath(f1,e1,pnet));
          //System.out.println("  2: "+f2+" -> "+e2+" "+existsPath(f2,e2,pnet));
          
          if (existsPath(f1,e1,pnet) && existsPath(f2, e2, pnet)) {
            //System.out.println("  found");
            return true;
          } else {
            //System.out.println("  not found");
          }
        }
      } 

    }

    return false;
  }
  
  private boolean existsPath(Transition t1, Transition t2, PetriNet pnet) {
    if (t1.equals(t2)) return true;
    Set<Transition> visited = new HashSet<Transition>();
    Set<Transition> heap = getPostTransitions(t1);
    if (heap.contains(t2)) return true;
    
    while (heap.size()>0) {
      Transition tt = heap.iterator().next();
      heap.remove(tt);
      visited.add(tt);
      Set<Transition> post = getPostTransitions(tt);
      if (post.contains(t2)) return true;
      for (Transition t : post) {
        if (!visited.contains(t)) heap.add(t);
      }
    }
  
    return false;
  }
  

  public static void main(String args[]) throws Throwable {
    String fileName_system_sysPath = args[0];
    String fileName_trace = args[1];
    
    ISystemModel sysModel = Uma.readSystemFromFile(fileName_system_sysPath);
    List<String[]> allTraces = ViewGeneration2.readTraces(fileName_trace);
    
    if (sysModel instanceof PetriNet) {
      ((PetriNet)sysModel).makeNormalNet();
      ((PetriNet)sysModel).turnIntoLabeledNet();
    }
    
    DNodeSys sys = Uma.getBehavioralSystemModel(sysModel);
    DNodeRefold build = Uma.initBuildPrefix_View(sys, 0);
    
    ViewGeneration2 viewGen = new ViewGeneration2(build);
    viewGen.extendByTraces(allTraces);
    
    TransitiveDependencies dep = new TransitiveDependencies(build);
    HashSet<DNode> impliedConditions = dep.getImpliedConditions_solutionLocal();
    build.getBranchingProcess().removeAll(impliedConditions);
    
    EventStructure es = new EventStructure(build.getBranchingProcess());
    
    SynthesisFromES2 synth = new SynthesisFromES2(sys.properNames, fileName_system_sysPath);
    PetriNet net = synth.synthesize(es);
    
    //PetriNetIO.writeToFile(net, fileName_system_sysPath+"_refold", PetriNetIO.FORMAT_DOT, 0);
    EventStructureMiner_BP.writeFile(net.toDot(), fileName_system_sysPath+"_refold.dot");
  }
  
}
