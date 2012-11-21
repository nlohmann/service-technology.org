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
import java.util.LinkedHashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.TreeSet;

import com.google.gwt.dev.util.collect.HashSet;

public class SynthesisFromES {
  
  private String properNames[];
  private String path;
  
  public SynthesisFromES(String properNames[], String path) {
    this.properNames = properNames;
    this.path = path;
  }
  
  class Pair {
    Event first;
    Set<Event> second;

    public Pair(Event f, Set<Event> s) { first = f; second = s; }
    public Event getFirst() { return first; }
    public Set<Event> getSecond() { return second; }
    public void setFirst(Event val) { first = val; }
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
  
  
  public void getConflictCluster(EventStructure es, Set<Event> current, Set<Set<Event>> cc) {
    
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
  
  public List<Set<Event>> getConflictClusters(EventStructure es, Set<Event> current, boolean directConflicts) {
    Event[] c_events = current.toArray(new Event[current.size()]);
    boolean adj_matrix[][] = new boolean [c_events.length][c_events.length];
    
    for (int i=0;i<c_events.length;i++) {
      for (int j=0;j<c_events.length;j++) {
        if (i == j) adj_matrix[i][j] = true;
        else {
          if (directConflicts)
            adj_matrix[i][j] = es.directConflict.containsKey(c_events[i]) && es.directConflict.get(c_events[i]).contains(c_events[j]);
          else
            adj_matrix[i][j] = es.inConflict(c_events[i], c_events[j]);
        }
      }
    }
    
    List<Set<Event>> cc = new LinkedList<Set<Event>>();

    BronKerbosch bk = new BronKerbosch();
    TreeSet<int[]> cliques = bk.findCliques(adj_matrix);
    //System.out.println("found "+cliques.size()+" cliques");
    for (int[] c : cliques) {
      Set<Event> conf = new HashSet<Event>();
      for (int i=0; i<c.length; i++) {
        conf.add(c_events[c[i]]);
      }
      //System.out.println("adding "+conf);
      cc.add(conf);
    }
    
    return cc;
  }
  
  public Set<Set<Event>> getConflictClusters(EventStructure es) {
    Set<Set<Event>> cc = new HashSet<Set<Event>>();
    
    int count = 0;
    for (Event e : es.getAllEvents()) {
      count++;
      
      if (es.directConflict.containsKey(e)) {
        Set<Event> current = new HashSet<Event>();
        for (Event f : es.directConflict.get(e)) current.add(f);
        current.add(e);
        
        //System.out.println(count+"/"+es.allEvents.size()+":"+current.size());
        
        //System.out.println("check "+current);
        //getConflictCluster(es, current, cc);

        // find conflict clusters for the current events
        for (Set<Event> _cc : getConflictClusters(es, current, true)) {
          cc.add(_cc);
        }
      }

      Set<Event> self = new HashSet<Event>();
      self.add(e);

      boolean alreadyContained = false;
      for (Set<Event> other : cc) {
        if (other.containsAll(self)) {
          alreadyContained = true;
          break;
        }
      }
      //if (!alreadyContained)
      {
        cc.add(self);
      }
    }
    return cc;
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
  
  private void addPlace(PetriNet pnet, EventStructure es, Event e, Set<Event> cc, Set<Place> implicitPlaces, Set<Place> implicitPlaces_2ormore, Map<Pair, Place> B, Map<Place, Pair> B2) {
    Pair pair = new Pair(e, cc);

    String srcTransitionLabel = properNames[e.id];

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
  
  public PetriNet synthesize(EventStructure es) {
    
    PetriNet pnet = new PetriNet();

    System.out.println("generate events");
    
    // ---------------------------------------------------------
    // 1. Initialize E
    // ---------------------------------------------------------
    Map<Event, Transition> E = new LinkedHashMap<Event, Transition>();
    Map<Transition, Event> E2 = new LinkedHashMap<Transition, Event>();
    Map<Integer, Event> E_index = new LinkedHashMap<Integer, Event>();
    for (Event n: es.allEvents) {
      String transitionLabel = properNames[n.id];
      E_index.put(n.globalId, n);
      Transition t = pnet.addTransition(transitionLabel);
      E.put(n, t);
      E2.put(t, n);
    }

    // ---------------------------------------------------------
    // 2. COMPUTE CE
    // ---------------------------------------------------------
    
    System.out.println("generate conflict clusters");
    Set<Set<Event>> CC = getConflictClusters(es);
    
    //for (Set<Event> cc : CC) {
    //  System.out.println(cc);
    //}
    
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
    Map<Pair, Place> B = new LinkedHashMap<Pair, Place>();
    Map<Place, Pair> B2 = new LinkedHashMap<Place,Pair>();
    for (Event e : E.keySet()){
      for (Set<Event> cc : CC) {
        
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
        
        boolean isCondition = jointPreEvents(es, cc).contains(e);

        if (isCondition) {
          addPlace(pnet, es, e, cc, implicitPlaces, implicitPlaces_2ormore, B, B2);
        }
      }
      
      if (e.post != null) {
        for (DNode ePost : e.post) {
          if (es.isTransitiveDependent(e, (Event)ePost)) {
            Set<Event> cc = new HashSet<Event>();
            
            cc.add((Event)ePost);
            addPlace(pnet, es, e, cc, implicitPlaces, implicitPlaces_2ormore, B, B2);
          }
        }
      }
    }
    
    System.out.println("generate arcs");

    // ---------------------------------------------------------
    // 4. COMPUTE F
    // ---------------------------------------------------------
    for (Pair pair: B.keySet()) {
      Place place = B.get(pair);
      Event e = pair.getFirst();
      Set<Event> cc = pair.getSecond();
      for (Event f : cc) {
        Transition trans = E.get(f);
        pnet.addArc(place, trans);
      }
      Transition trans = E.get(e);
      pnet.addArc(trans, place);
    }
    
    try {
      EventStructureMiner_BP.writeFile(pnet.toDot(), path+"_bp.dot");
    } catch (IOException e1) {
      // TODO Auto-generated catch block
      e1.printStackTrace();
    }
    

    Set<Place> final_implicitPlaces = new HashSet<Place>();
    
    System.out.println("remove subsumed conditions");
    // ---------------------------------------------------------
    // 5. CLEAN THE NET
    // ---------------------------------------------------------
    
    // --- Remove subsumed conditions
    Set<Place> subsumedConditions = new HashSet<Place>();
    for (Transition src: pnet.getTransitions())
      if (src.getPostSet().size() > 1)
        for (Place p1: src.getPostSet()) {
          
          for (Place p2: src.getPostSet()) {
            
            if (p1 != p2 && p1.getPostSet().containsAll(p2.getPostSet())) {
              //System.out.println(p1+" subsumes "+p2);
              subsumedConditions.add(p2);
            }
            
          }
        }
    
    //System.out.println(subsumedConditions);
    for (Place place: subsumedConditions) {
      System.out.println("removing subsumed "+place);
      pnet.removePlace(place);
      //final_implicitPlaces.add(place);
    }
    
    implicitPlaces_2ormore.removeAll(subsumedConditions);
    for (Place place: implicitPlaces_2ormore) {
      System.out.println("removing transitive conflicts 1 "+place);
      //pnet.removePlace(place);
      final_implicitPlaces.add(place);
    }
    
    /*
    System.out.println("implicit places: "+implicitPlaces);
    
    System.out.println("generating additional conflict clusters");
    // compute conflict clusters that have to be materialized because of transitive dependencies
    for (Transition t : pnet.getTransitions()) {
      Set<Transition> post = new HashSet<Transition>();
      for (hub.top.petrinet.Arc a : t.getOutgoing()) {
        for (hub.top.petrinet.Arc a2 : a.getTarget().getOutgoing()) {
          post.add((Transition)a2.getTarget());
        }
      }
      
      List<Place> t_postPlaces = t.getPostSet();
      System.out.println("check "+t+" "+t_postPlaces);
      
      Set<Set<Event>> CCs = new HashSet<Set<Event>>();
      for (Transition t2 : post) {
        
        Place implicitPrePlace = null;
        for (Place p : t2.getPreSet()) {
          if (p.getIncoming().size() == 1 && p.getOutgoing().size() == 1 && t_postPlaces.contains(p)) {
            if (implicitPlaces.contains(p)) {
              implicitPrePlace = p;
              break;
            }
          }
        }
        if (implicitPrePlace == null) continue;
        
        System.out.println("  implicit arc: "+implicitPrePlace);
        
        // t -> t2 is a transitive dependency, make sure that is comes with
        // a conflict
        
        Set<Transition> conflicts = new HashSet<Transition>();
        conflicts.add(t2);
        for (Transition t3 : post) {
          if (t2 != t3 && es.inConflict(E2.get(t2), E2.get(t3)))
          {
            Place connectingPlace = null;
            for (Place p : t2.getPreSet()) {
              if (p.getIncoming().size() == 1 && p.getOutgoing().size() == 1 && t_postPlaces.contains(p)) {
                connectingPlace = p;
                break;
              }
            }
            if (connectingPlace == null) continue;
            conflicts.add(t3);
          }
        }
        
        System.out.println("  conflicts: "+conflicts);

        if (conflicts.size() > 1) {
          
          Set<Event> current = new HashSet<Event>();
          for (Transition t3 : conflicts) current.add(E2.get(t3));
          
          // find all conflict clusters in this bunch of transitions
          List<Set<Event>> current_cc = getConflictClusters(es, current, false);
          
          for (Set<Event> cc : current_cc) {
            
            System.out.println("  cc: "+cc);
            
            // for each conflict cluster, create a place (unless such a place already exists)
            // see if t has a post-place that materializes the conflict between the transitions
            boolean conflictExists = false;
            boolean oneNonImplicit = false;
            for (Place p : t_postPlaces) {
              int cc_PostEvents = 0;
              for (Transition t3 : p.getPostSet()) {
                if (cc.contains(E2.get(t3))) {
                  if (!implicitPlaces.contains(p)) oneNonImplicit = true;
                  cc_PostEvents++;
                }
              }
              if (cc_PostEvents == cc.size()) {
                conflictExists = true;
                break;
              }
            }
            
            System.out.println("exists: "+conflictExists+" non-trans: "+oneNonImplicit);
            
            // no: remember conflict cluster and 
            if (oneNonImplicit && !conflictExists) {
              CCs.add(cc);
            }
          }
        }
      }
      
      for (Set<Event> cc : CCs) {
        
        List<Place> toRemove = new LinkedList<Place>();
        for (Place p : t.getPostSet()) {
          if (p.getOutgoing().size() == 1 && cc.contains(E2.get(p.getPostSet().get(0))) ) {
            toRemove.add(p);
          }
        }
        
        if (toRemove.size() != cc.size()) {
          System.out.println(cc+" / "+toRemove);
          continue; // does not merge the right amount of arcs
        }
        
        String srcTransitionLabel = t.getName();

        // translate the IDs of the defining events to the labels
        // so that conditions are consistently labeled throughout the occurrence net
        // then we can identify equivalent conditions based on their labels
        String placeLabel = "("+srcTransitionLabel;
        
        //String placeLabel = "(";
        for (Event f : cc) {
          String tgtTransitionLabel = properNames[f.id];
          placeLabel += ","+tgtTransitionLabel;
          //System.out.println(f+" maps to "+E.get(f));
        }
        
        placeLabel += ")";

        System.out.println("generating additional "+placeLabel+" for "+toRemove);

        Place place = pnet.addPlace(placeLabel);
        pnet.addArc(t, place);
        for (Event e : cc) pnet.addArc(place, E.get(e));
        
        for (Place p : toRemove) {
          pnet.removePlace(p);
          implicitPlaces.remove(p);
        }
      }
    }*/
    
    
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
        
        if (is_tr_conflict) {
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
      //final_implicitPlaces.add(place);
    }

    System.out.println("searching for implicit places in "+pnet.getInfo(false));
    // after removing subsumed conditions and transitive conflicts,
    // some of the original implicit conditions are maybe no longer
    // implicit. Check here.
    for (Place p : implicitPlaces) {
      if (!pnet.getPlaces().contains(p)) continue;
      if (p.getPreSet().isEmpty()) continue;
      if (p.getPostSet().isEmpty()) continue;
      
      //System.out.println("is "+p+" still implied?");
      boolean isImplicit = true;
      
      Transition t = p.getPreSet().get(0);
      for (Transition t2 : p.getPostSet()) {

        if (!existsPathWithoutCondition(t, t2, p, pnet, new HashSet<Place>())) {
          isImplicit = false;
          //System.out.println("  no path "+t+" --> "+t2);
        }
      }
      if (isImplicit) {
        //System.out.println("  yes, adding");
        final_implicitPlaces.add(p);
      }
    }
  
    // --- Add source place

    Place initialPlace = pnet.addPlace("start");
    initialPlace.setTokens(1);

    for (Event e : es.getAllEvents())
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
      //PetriNetIO.writeToFile(pnet, path+"_bp2", PetriNetIO.FORMAT_LOLA, 0);
      //PetriNetIO.writeToFile(pnet, path+"_bp2", PetriNetIO.FORMAT_DOT, 0);
      EventStructureMiner_BP.writeFile(pnet.toDot(), path+"_bp2.dot");
    } catch (IOException e1) {
      // TODO Auto-generated catch block
      e1.printStackTrace();
    }

    PetriNet folded = fold(pnet, final_implicitPlaces);
    
    System.out.println("done: "+pnet.getInfo(false)+" -> "+folded.getInfo(false));
    
    return folded;
  }
  
  private PetriNet fold(PetriNet occnet, Set<Place> implicitPlaces) {
    try {
      
      DNodeSys_OccurrenceNet sys = new DNodeSys_OccurrenceNet(occnet, implicitPlaces);
      
      System.out.println("implicit places: "+implicitPlaces);

      DNodeRefold build = Uma.initBuildPrefix_View(sys, 0);
      
      try {
        OcletIO_Out.writeFile(build.toDot(), path+"_bp2_b.dot");
      } catch (Exception e) {
        System.err.println(e);
      }

      Uma.out.println("equivalence..");
      build.futureEquivalence();

      //build.debug_printFoldingEquivalence();

      Uma.out.println("join maximal..");
      build.extendFutureEquivalence_maximal();

      Uma.out.println("fold backwards..");

      while (build.extendFutureEquivalence_backwards()) {
        Uma.out.println("fold backwards..");
      }
      
      //while (build.refineFoldingEquivalence_removeSuperfluous()) {
      //  Uma.out.println("remove superfluous..");
      //}

      hub.top.uma.synthesis.EquivalenceRefineSuccessorGeneralize splitter = new hub.top.uma.synthesis.EquivalenceRefineSuccessorGeneralize(build); 

      Uma.out.println("relax..");
      build.relaxFutureEquivalence(splitter);
      Uma.out.println("determinize..");
      //while (build.extendFoldingEquivalence_deterministic()) {
      //  Uma.out.println("determinize..");
      //}
      
      //build.futureEquivalence()._debug_printFoldingEquivalence();
      
      /*
      TransitiveDependencies dep = new TransitiveDependencies(build);
      HashSet<DNode> implicit = dep.getImpliedConditions_solutionGlobal();
      for (DNode d : implicit) {
        d.isImplied = true;
      }*/
      
      NetSynthesis synth = new NetSynthesis(build);
      DNodeSetElement nonImplied = new DNodeSetElement();
      for (DNode d : build.getBranchingProcess().getAllNodes()) {
        if (nonImplied.contains(d)) {
          System.out.println("duplicate node "+d);
          continue;
        }
        if (!d.isImplied) {
          System.out.println("node "+d);
          nonImplied.add(d);
        } else {
          System.out.println("implicit "+d);
          //nonImplied.add(d);
        }
      }
      PetriNet net = synth.foldToNet_labeled(nonImplied, false);
      
      return net;

    } catch (InvalidModelException e) {
      System.err.println(e);
      e.printStackTrace();
    }

    return null;
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
    
    SynthesisFromES synth = new SynthesisFromES(sys.properNames, fileName_system_sysPath);
    PetriNet net = synth.synthesize(es);
    
    //PetriNetIO.writeToFile(net, fileName_system_sysPath+"_refold", PetriNetIO.FORMAT_DOT, 0);
    EventStructureMiner_BP.writeFile(net.toDot(), fileName_system_sysPath+"_refold.dot");
  }
  
}
