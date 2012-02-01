package hub.top.uma.er;

import hub.top.petrinet.Node;
import hub.top.petrinet.PetriNet;
import hub.top.petrinet.PetriNetIO;
import hub.top.petrinet.Place;
import hub.top.petrinet.Transition;
import hub.top.scenario.OcletIO;
import hub.top.uma.DNode;
import hub.top.uma.DNodeSys;
import hub.top.uma.Uma;
import hub.top.uma.er.EventStructure.EventCollection;
import hub.top.uma.er.SynthesisFromES;
import hub.top.uma.view.ViewGeneration2;

import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Set;


import com.google.gwt.dev.util.collect.HashSet;

public class EventStructureReplay {
  
  private PetriNet net;
  private EventStructure es;
  
  private Event artificialStart;
  
  public EventStructureReplay(PetriNet net) {
    this.net = net;
    this.es = new EventStructure();
    
    buildNameTable();
    
    artificialStart = new Event(nameToID.get(NAME_ARTIFICIAL_START), new Event[0]);
    es.add(artificialStart);
    consumedFrom = new HashMap<Event, List<Token>>();
    inTrace = new HashMap<Event, Set<Integer>>();
  }
  
  
  private static class Token {
    public Place on;
    public Event producedBy;
    public Token(Place p, Event e) {
      this.on = p;
      this.producedBy = e;
    }
    
    @Override
    public boolean equals(Object obj) {
      if (obj instanceof Token) {
        Token other = (Token)obj;
        return this.on == other.on && this.producedBy == other.producedBy;
      }
      return false;
    }
    
    @Override
    public String toString() {
      return on+" ("+producedBy+")";
    }
  }
  
  private Map<Event, List<Token> > consumedFrom;
  
  private Map<Event, Set<Integer> > inTrace;
  
  private int traceCount = 0;
  
  public void extendByTrace(String trace[]) {
    
    //System.out.println(ViewGeneration2.toString(trace));
    
    traceCount++;
    
    Set<Event> config = new HashSet<Event>();
    LinkedList<Token> marking = new LinkedList<Token>();
    
    // create initial marking
    for (Place p : net.getPlaces()) {
      for (int i=0;i<p.getTokens();i++) {
        marking.add(new Token(p,artificialStart));
      }
    }
    
    Event lastEvent = artificialStart;
    if (!inTrace.containsKey(lastEvent)) inTrace.put(lastEvent, new HashSet<Integer>());
    inTrace.get(lastEvent).add(traceCount);
    config.add(artificialStart);

    // execute the trace
    for (String event : trace) {
      
      // find the transition to fire
      Transition toFire = null;
      for (Transition t : net.getTransitions()) {
        if (t.getName().equals(event)) {
          toFire = t;
          break;
        }
      }
      
      if (toFire == null) {
        System.err.println("Error. Net does not contain transition with name "+event);
        return;
      }
      
      // find the tokens that are consumed by 'toFire'
      List<Token> toConsume = new LinkedList<Token>();
      nextplace: for (Place p : toFire.getPreSet())
      {
        for (Token m : marking) {
          if (m.on == p) {
            toConsume.add(m);
            continue nextplace;
          }
        }
        // no token on p: add new token
        Token m = new Token(p,lastEvent);
        marking.add(m);
        toConsume.add(m);
      }

      short toFireID = nameToID.get(toFire.getName());
      Event newEvent = appendEvent(toFireID, toConsume, toFire.getPostSet(), marking, config, lastEvent);
      lastEvent = newEvent;
      
      // remember that the event participates in this trace
      if (!inTrace.containsKey(lastEvent)) inTrace.put(lastEvent, new HashSet<Integer>());
      inTrace.get(lastEvent).add(traceCount);
    }
    
    short endEventId = nameToID.get(NAME_ARTIFICIAL_END);
    lastEvent = appendEvent(endEventId, marking, new LinkedList<Place>(), marking, config, lastEvent); 

    // remember that the event participates in this trace
    if (!inTrace.containsKey(lastEvent)) inTrace.put(lastEvent, new HashSet<Integer>());
    inTrace.get(lastEvent).add(traceCount);

  }
  
  private Event appendEvent(short toFireID, List<Token> toConsume, List<Place> effect, LinkedList<Token> marking, Set<Event> config, Event lastEvent) {

    // get the predecessor events based on the consumed tokens
    HashSet<Event> preEvents = new HashSet<Event>();
    for (Token m : toConsume) {
      preEvents.add(m.producedBy);
    }
    
    Event pre[] = new Event[preEvents.size()];
    pre = preEvents.toArray(pre);
    
    // see if this event was already fired
    Event newEvent = null;
    if (lastEvent.post != null) {
      for (DNode d : lastEvent.post) {
        Event e = (Event)d;
        if (e.id == toFireID) {
          newEvent = e;
        }
        // make sure that the new event is consistent with the current configuration:
        // not yet executed
        if (config.contains(newEvent)) newEvent = null;
        // all predecessor events are part of the configuration
        if (newEvent != null) {
          for (DNode pred : newEvent.getAllPredecessors()) {
            if (pred == newEvent) continue;
            if (!config.contains(pred)) { newEvent = null; break; }
          }
        }
        if (newEvent != null) break;
      }
    }
    // if not: create it and make it depend on the events from which
    // we consume a token
    if (newEvent == null) {
      newEvent = new Event(toFireID, pre);
      es.add(newEvent);
      for (Event e : pre) {
        e.addPostNode(newEvent);
      }
      consumedFrom.put(newEvent, new LinkedList<Token>(toConsume));
      
      for (Token m : toConsume) {
        if (m.producedBy.post != null) {
          for (DNode conflictingEvent : m.producedBy.post) {
            // just added, skip the newEvent
            if (conflictingEvent == newEvent) continue;
            
            boolean inConflictWithNew = false;
            done: for (Token m2 : consumedFrom.get(conflictingEvent)) {
              for (Token m3 : toConsume) {
                if (m2.equals(m3)) {
                  inConflictWithNew = true;
                  break done;
                }
              }
            }
            if (inConflictWithNew) {
              if (!es.alreadyInConflict(newEvent, (Event)conflictingEvent)) {
                es.setDirectConflict(newEvent, (Event)conflictingEvent);
              }
            }
          }
        }
      }
    }

    // update the marking
    marking.removeAll(toConsume);
    for (Place p : effect) {
      marking.add(new Token(p,newEvent));
    }
    
    return newEvent;
  }

  
  private boolean areConcurrent(Event e, Event f) {
    if (es.inConflict(e, f)) {
      return false;
    }
    if (es.getPrimeConfiguration(f, true).contains(e)) {
      return false;
    }
    if (es.getPrimeConfiguration(e, true).contains(f)) {
      return false;
    }
    return true;
  }
  
  public void refineConflicts() {
    refineConflicts(es.getAllEvents());
  }
  
  public void refineConflicts(EventCollection forEvents) {

    for (Event e : forEvents) {
      for (Event f : forEvents) {
        if (areConcurrent(e, f)) {
          boolean overlap = false;
          for (Integer t_e : inTrace.get(e)) {
            if (inTrace.get(f).contains(t_e)) {
              overlap = true;
              break;
            }
          }
          if (!overlap) {
            if (!es.alreadyInConflict(e, f))
                es.setDirectConflict(e, f);
          }
        }
        if (es.inDirectConflict(e, f) && es.alreadyInConflict(e, f)) {
          es.directConflict.get(e).remove(f);
          es.directConflict.get(f).remove(e);
        }
      }
    }
  }
  
  public String       properNames[];      // translate IDs to names
  public Map<String, Short> nameToID;     // and names to IDs
  public short currentNameID;
  private int nodeNum;
  public HashSet<Short>       terminalNodes;
  
  public static final String NAME_ARTIFICIAL_START = "start";
  public static final String NAME_ARTIFICIAL_END = "end";
  public static final String NAME_TAU = "tau"; 
  
  /**
   * Translate full strings of the Petri net to ids and fill
   * {@link DNodeSys#nameToID} and {@link DNodeSys#properNames}.
   * 
   * @param net
   */
  private void buildNameTable() {
    
    nameToID = new HashMap<String, Short>();
    terminalNodes = new HashSet<Short>();
    
    nodeNum = 0;
    
    // collect all names and assign each new name a new ID
    for (Node n : net.getTransitions()) {
      if (nameToID.get(n.getName()) == null)
        nameToID.put(n.getName(), currentNameID++);
     nodeNum++;
    }
    for (Node n : net.getPlaces()) {
      if (nameToID.get(n.getName()) == null)
        nameToID.put(n.getName(), currentNameID++);
      nodeNum++;
      
      if (n.getOutgoing().size() == 0)
        // remember ID of this terminal node
        terminalNodes.add(nameToID.get(n.getName()));
    }
    
    if (!nameToID.containsKey(NAME_ARTIFICIAL_START))
      nameToID.put(NAME_ARTIFICIAL_START, currentNameID++);
    if (!nameToID.containsKey(NAME_ARTIFICIAL_END))
      nameToID.put(NAME_ARTIFICIAL_END, currentNameID++);
    if (!nameToID.containsKey(NAME_TAU))
      nameToID.put(NAME_TAU, currentNameID++);
    
    properNames = new String[nameToID.size()];
    for (Entry<String,Short> line : nameToID.entrySet()) {
      properNames[line.getValue()] = line.getKey();
    }
    
    DNode.nameTranslationTable = properNames;
  }
  
  private Set<Event> singleEvents;
  
  private void setSingleEvents() {
    singleEvents = new HashSet<Event>();
    
    for (Event e : es.getAllEvents()) {
      if (   e.pre != null && e.pre.length <= 1
          && e.post != null && e.post.length <= 1) {
        singleEvents.add(e);
      }
    }
  }
  
  boolean co[][];
  
  public void getConcurrentEvents() {
    
    boolean precedes[][] = new boolean[properNames.length][properNames.length];
    for (Event e : es.getAllEvents()) {
      Set<Event> ePre = es.getPrimeConfiguration(e, false);
      for (Event f : ePre) {
        if (f.id != e.id) {
          precedes[f.id][e.id] = true;
        }
      }
    }
    
    co = new boolean[properNames.length][properNames.length];
    for (short e1 = 0; e1 < precedes.length; e1++) {
      for (short e2 = 0; e2 < precedes.length; e2++) {
        if (precedes[e1][e2] && precedes[e2][e1]) {
          co[e1][e2] = true;
          co[e2][e1] = true;
        }
      }
    }
  }
  
  /**
   * Compute the transitive dependencies between all transitions in all
   * oclets of the {@link #system}.
   */
  private boolean[][] computeTransitiveDependencies(DNode[] nodes) {
    
    int nodeNum = nodes.length;
    
    boolean directSucc[][] = new boolean[nodeNum][nodeNum];
    boolean transSucc[][] = new boolean[nodeNum][nodeNum];

    // extract the direct successor relations between nodes
    for (int i=0; i<nodeNum; i++) {
      DNode c = nodes[i];
      if (c.post == null) continue;
      for (DNode d : c.post) {
        int j = 0;
        while (j < nodes.length) {
          if (nodes[j] == d) break;
          j++;
        }
        if (j == nodes.length) continue; // successor 'd' of 'c' is not part of these nodes
        
        directSucc[i][j] = true;
        transSucc[i][j] = true;
      }
    }
    
    /*
     For each j from 1 to n
      For each i from 1 to n
         If T(i,j)=1, then form the Boolean or of row i and row j 
                           and replace row i by it.
         Go on to the next i-value.
      Once you have processed each i-value, go on to the next j-value.

     */
    for (int j=0; j<transSucc.length; j++) {
      for (int i=0; i<transSucc.length; i++) {
        if (i != j && transSucc[i][j]) {
          for (int k = 0; k<transSucc.length; k++) {
            transSucc[i][k] = transSucc[i][k] || transSucc[j][k];
          }
        }
      }
    }
    
    for (int j=0; j<transSucc.length; j++) {
      for (int i=0; i<transSucc.length; i++) {
        if (co[nodes[i].id][nodes[j].id]) {
          transSucc[i][j] = false;
          transSucc[j][i] = false;
        }
      }
    }
    
    // transitive reduction
    for (int j=0; j<transSucc.length; j++) {
      for (int i=0; i<transSucc.length; i++) {
        if (i != j && transSucc[i][j]) {
          for (int k = 0; k<transSucc.length; k++) {
            transSucc[i][k] = transSucc[i][k] && !transSucc[j][k];
          }
        }
      }
    }
    
    
    /*
    for (int i=0;i<nodeNum; i++) {
      for (int j=0;j<nodeNum; j++) {
        if (transSucc[i][j] && !directSucc[i][j])
          System.out.println(nodes[i]+" ==>* "+nodes[j]);
        else if (directSucc[i][j])
          System.out.println(nodes[i]+" -->  "+nodes[j]);
        else if (transSucc[i][j])
          System.out.println(nodes[i]+" -->* "+nodes[j]);
      } 
    }
    */
    
    System.out.println("---------------------------------------------");
    for (int i=0;i<nodeNum; i++) {
      for (int j=0;j<nodeNum; j++) {
        if (transSucc[i][j])
          System.out.println(nodes[i]+" --> "+nodes[j]);
      } 
    }
    return transSucc;
  }
  
  public void extendConcurrency() {
    
    Map<Event, Set<Event>> cosets = new HashMap<Event, Set<Event>>();
    LinkedList<Event> queue = new LinkedList<Event>();
    queue.add(artificialStart);
    cosets.put(artificialStart, new HashSet<Event>());
    cosets.get(artificialStart).add(artificialStart);
    
    while (!queue.isEmpty()) {
      
      EventCollection touched = new EventCollection();
      
      Event e = queue.removeFirst();
      if (singleEvents.contains(e)) {
        // check CO
        LinkedList<Event> coqueue = new LinkedList<Event>();
        LinkedList<Event> e_succ = new LinkedList<Event>();
        Set<Event> dont_change = new HashSet<Event>();
        coqueue.add(e);
        e_succ.add(e);

        Event preNew = new Event(nameToID.get(NAME_TAU), 0);
        
        touched.add(preNew);
        inTrace.put(preNew, new HashSet<Integer>());
        for (int i : inTrace.get(e)) {
          inTrace.get(preNew).add(i);
        }

        for (DNode pred : e.getAllPredecessors()) {
          if (pred == e) continue;
          es.setDependency((Event)pred, preNew);
        }
        es.setDependency(preNew, e);
        es.add(preNew);
        
        e_succ.add(preNew);
        
       
        
        while (!coqueue.isEmpty()) {
          Event f = coqueue.removeFirst();
          if (f.post != null) {
            for (DNode fPost : f.post) {
              if (singleEvents.contains(fPost)) {
                if (!e_succ.contains(fPost)) {
                  e_succ.add((Event)fPost);
                  coqueue.add((Event)fPost);
                }
              } else {
                e_succ.add((Event)fPost);
                //dont_change.add((Event)fPost);
                
                // fPost has not been visited yet, add to the outer queue
                if (!cosets.containsKey(fPost)) {
                  cosets.put((Event)fPost, new HashSet<Event>());
                  cosets.get(fPost).add((Event)fPost);
                  queue.addLast((Event)fPost);
                }
              }
            }
          }
        }
        
        DNode e_succ_array[] = e_succ.toArray(new DNode[e_succ.size()]);
        boolean depends[][] = computeTransitiveDependencies(e_succ_array);
        
        for (int k=0;k<e_succ_array.length;k++) {
          Event d_k = (Event)e_succ_array[k];
          if (dont_change.contains(d_k)) continue;
          
          for (int l=0;l<e_succ_array.length;l++) {
            Event d_l = (Event)e_succ_array[l];
            if (dont_change.contains(d_l)) continue;
            
            if (k != l) {
              if (depends[k][l]) {
                es.setDependency(d_k, d_l);
              } else {
                if (es.removeDependency(d_k, d_l)) {
                
                  if (es.directConflict.containsKey(d_k)) {
                    for (Event conf : es.directConflict.get(d_k)) {
                      if (!es.directConflict.containsKey(d_l))
                        es.directConflict.put(d_l, new HashSet<Event>());
                      es.directConflict.get(d_l).add(conf);
                      System.out.println("set "+d_l+" # "+conf +" because of "+d_k);
                    }
                  }

                }
              }
            }
          }
          touched.add((Event)e_succ_array[k]);
        }
        
        
      } else if (e.post != null) {
        for (DNode ePost : e.post) {
          // ePost has not been visited yet, add to the outer queue
          if (!cosets.containsKey(ePost)) {
            cosets.put((Event)ePost, new HashSet<Event>());
            cosets.get(ePost).add((Event)ePost);
            queue.addLast((Event)ePost);
          }
        }
      }
      
      refineConflicts(touched);
      es.reduceTransitiveConflicts();
      es.removeTransitiveDependencies();
    }

  }
  
  public void coarsenCausality() {
    boolean changed = true;
    while (changed) {
      changed = false;
      
      EventCollection newNodes = new EventCollection();
      EventCollection touched = new EventCollection();
      
      for (Event e : es.getAllEvents()) {
        if (!singleEvents.contains(e)) continue;
        if (e.pre != null) {
          for (DNode f : e.pre) {
            if (!singleEvents.contains(f)) continue;
            
            // f --> e
            if (co[f.id][e.id]) {
              es.removeDependency((Event)f, e);
              
              changed = true;
              
              Event preNew = new Event(nameToID.get(NAME_TAU), 0);
              Event postNew = new Event(nameToID.get(NAME_TAU), 0);
              
              newNodes.add(preNew);
              newNodes.add(postNew);
              touched.add(preNew);
              touched.add(postNew);
              inTrace.put(preNew, new HashSet<Integer>());
              inTrace.put(postNew, new HashSet<Integer>());
              for (int i : inTrace.get(e)) {
                inTrace.get(preNew).add(i);
                inTrace.get(postNew).add(i);
              }
              for (int i : inTrace.get(f)) {
                inTrace.get(preNew).add(i);
                inTrace.get(postNew).add(i);
              }

              if (f.pre != null) {
                DNode fPres[] = f.pre;
                for (DNode fPre : fPres) {
                  es.setDependency((Event)fPre, preNew);
                  //es.removeDependency((Event)fPre, (Event)f);
                }
              }
              if (e.post != null) {
                DNode ePosts[] = e.post;
                for (DNode ePost : ePosts) {
                  es.setDependency(postNew, (Event)ePost);
                  //es.removeDependency(e, (Event)ePost);
                }
              }
              
              es.setDependency(preNew, e);
              es.setDependency(preNew, (Event)f);
              es.setDependency(e, postNew);
              es.setDependency((Event)f, postNew);

              touched.add(e);
              touched.add((Event)f);
            }
          }
        }
      }
      for (Event e : newNodes) es.add(e);
      refineConflicts(touched);
    }
  }
  
  
  public boolean inConflictOutsideFolding(Event e1, Event e2, Map<Event,Event> canonical) {
    Set<Event> e1_pre = es.getPrimeConfiguration(e1, true);
    Set<Event> e2_pre = es.getPrimeConfiguration(e2, true);
    
    for (Event f : e1_pre) {
      if (es.directConflict.containsKey(f)) {
        for (Event f_confl : es.directConflict.get(f)) {
          
          // the conflict pair is represented after the folding, don't consider
          if (   canonical.containsKey(f) && canonical.get(f) == f
              && canonical.containsKey(f_confl) && canonical.get(f_confl) == f_confl) continue;
          
          if (e2_pre.contains(f_confl)) {
            return true;
          }
        }
      }
    }
    return false;
  }

  
  public void foldForward() {
    Map<Event, Set<Event>> foldingEquivalence = new HashMap<Event, Set<Event>>();
    Map<Event, Event> canonical = new HashMap<Event, Event>();
    
    LinkedList<Event> queue = new LinkedList<Event>();
    queue.add(artificialStart);
    foldingEquivalence.put(artificialStart, new HashSet<Event>());
    foldingEquivalence.get(artificialStart).add(artificialStart);
    canonical.put(artificialStart, artificialStart);
    
    while (!queue.isEmpty()) {
      Event e = queue.removeFirst();
      
      Map<Short, Set<Event>> id_equivalent = new HashMap<Short, Set<Event>>();
      
      for (Event eEquiv : foldingEquivalence.get(e)) {
        if (eEquiv.post == null) continue;
        
        for (DNode ePost : eEquiv.post) {
          Event f = (Event)ePost;
          
          boolean allPredecessorsSeen = true;
          for (DNode fPre : f.pre) {
            if (!canonical.containsKey(fPre)) {
              allPredecessorsSeen = false;
            }
          }
          
          if (allPredecessorsSeen) {

            // TODO: and all predecessors must be mutually equivalent
            if (!id_equivalent.containsKey(f.id)) id_equivalent.put(f.id, new HashSet<Event>());
            for (DNode f_other : id_equivalent.get(f.id)) {
              if (!es.inConflict(f, (Event)f_other)) {
                System.err.println("merging two concurrent events "+f+" and "+f_other);
              }
            }
            id_equivalent.get(f.id).add(f);
          }
        }
      }
      
      for (Map.Entry<Short, Set<Event>> id_equiv_class : id_equivalent.entrySet()) {
        
        Map<Set<Event>, Set<Event>> pred_equivalent = new HashMap<Set<Event>, Set<Event>>();
        for (Event f : id_equiv_class.getValue()) {
          Set<Event> f_pred_canonical = new HashSet<Event>();
          for (DNode fPred : f.pre) {
            f_pred_canonical.add(canonical.get(fPred));
          }
          
          if (!pred_equivalent.containsKey(f_pred_canonical)) pred_equivalent.put(f_pred_canonical, new HashSet<Event>());
          pred_equivalent.get(f_pred_canonical).add(f);
        }
        
        for (Set<Event> equivs : pred_equivalent.values()) {
          Event f = getCanonical(equivs);
          foldingEquivalence.put(f, new HashSet<Event>(equivs));
          queue.add(f);
          for (Event fEquiv : equivs) canonical.put(fEquiv, f);
        }
      }
    }
    
    for (Map.Entry<Event, Set<Event>> equiv : foldingEquivalence.entrySet()) {
      System.out.println(equiv);
    }
    
    /*
    queue.clear();
    queue.add(artificialStart);
    Set<Event> seen = new HashSet<Event>();
    seen.add(artificialStart);
    
    while (!queue.isEmpty()) {
      Event e = queue.removeFirst();

      //if (canonical.containsKey(e) && canonical.get(e) != e)
      {
        // materialize conflict inheritance in the model for all events
        // that will be removed
        if (es.directConflict.containsKey(e) && e.post != null) {
          for (DNode ePost : e.post) {
            for (Event eConf : es.directConflict.get(e)) {
              // each successor of e gets all conflicting events of e 
              if (!es.directConflict.containsKey(ePost)) es.directConflict.put((Event)ePost, new HashSet<Event>());
              es.directConflict.get(ePost).add(eConf);
              es.directConflict.get(eConf).add((Event)ePost);
              System.out.println("pushing "+ePost+" # "+eConf);
            }
          }
        }
      }

      if (e.post == null) continue;
      for (DNode ePost : e.post) {
        boolean allPreSeen = true;
        for (DNode ePostPre : ePost.pre) {
          if (!seen.contains(ePostPre)) {
            allPreSeen = false; break;
          }
        }
        if (allPreSeen) {
          seen.add((Event)ePost);
          queue.addLast((Event)ePost);
        }
      }
    }
    */
    
    for (Event e : es.allEvents) {
      if (canonical.containsKey(e) && canonical.get(e) == e)
      {
        for (Event f : es.allEvents) {
          if (f == e) continue;
          if (canonical.containsKey(f) && canonical.get(f) == f) {
            
            if (inConflictOutsideFolding(e, f,canonical))
            {
              if (!es.directConflict.containsKey(e)) es.directConflict.put(e, new HashSet<Event>());
              if (!es.directConflict.containsKey(f)) es.directConflict.put(f, new HashSet<Event>());
              es.directConflict.get(e).add(f);
              es.directConflict.get(f).add(e);
            }
          }
        }
      }
    }
    
    for (Event e : es.allEvents) {
      
      if (canonical.containsKey(e) && canonical.get(e) == e) {
        // remove stale conflict relations that hold because predecessors
        // of 'e' are merged with conflicting predecessors but these
        // predecessors were in conflict with 'e'
        if (es.directConflict.containsKey(e)) {
          List<Event> removeFromConflicts = new LinkedList<Event>();
          for (Event eConfl : es.directConflict.get(e)) {
            
            // the events e and f are in conflict, look whether there are
            // equivalent events e* and f* merged into e and f that are 
            // not in conflict
            boolean mergedWithNonConflict = false;
            f_equivalent_events: for (Event eConfl_equiv : foldingEquivalence.get(canonical.get(eConfl))) {
              if (eConfl == eConfl_equiv) continue;
              for (Event eEquiv : foldingEquivalence.get(e)) {
                if (eEquiv == eConfl_equiv) continue;
                if (!es.inConflict(eEquiv, eConfl_equiv)) {
                  System.out.println(eConfl+" #! "+e+"  non-conflict by "+eEquiv+" #! "+eConfl_equiv);
                  mergedWithNonConflict = true;
                  break f_equivalent_events;
                }
              }
            }
            if (mergedWithNonConflict) {
              removeFromConflicts.add(eConfl);
            }
          }
          for (Event f : removeFromConflicts) {
            es.directConflict.get(e).remove(f);
            es.directConflict.get(f).remove(e);
          }
        }
      }
    }
    
    
    List<Event> toRemove = new LinkedList<Event>();
    for (Event e : es.allEvents) {
      if (canonical.containsKey(e) && canonical.get(e) != e) {
        toRemove.add(e);
      } else {
        if (e.pre != null)
          for (int i=0; i<e.pre.length; i++) {
            if (canonical.containsKey(e.pre[i]))
              e.pre[i] = canonical.get(e.pre[i]);
          }
        if (e.post != null)
          for (int i=0; i<e.post.length; i++) {
            if (canonical.containsKey(e.post[i]))
              e.post[i] = canonical.get(e.post[i]);
          }
      }
        
    }

    es.removeAll(toRemove);
  }
  
  public void removeSuperFluousTauEvents() {
    
    List<Event> toRemove = new LinkedList<Event>();
    for (Event e : es.allEvents) {
      if (properNames[e.id].equals(NAME_TAU)) {
        if (es.directConflict.containsKey(e)) continue;
        
        for (DNode ePre : e.pre) {
          for (DNode ePost : e.post) {
            ePre.addPostNode(ePost);
            ePost.addPreNode(ePre);
          }
        }
        toRemove.add(e);
      }
    }
    
    es.removeAll(toRemove);
  }
  
  public static Event getCanonical(Collection<Event> events) {
    Event min = null;
    for (Event e : events) {
      if (min == null) min = e;
      else {
        if (e.globalId < min.globalId) min = e;
      }
    }
    return min;
  }
  
  public static void main(String args[]) throws Throwable {
    
    //String fileName_system_sysPath = args[0];
    //String fileName_trace = args[1];
    
    //String fileName_system_sysPath = "./examples/model_correction/a12f0n00_alpha.lola";
    //String fileName_trace  = "./examples/model_correction/a12f0n00.log.txt";

    //String fileName_system_sysPath = "./examples/model_correction/a12f0n00_alpha.lola";
    //String fileName_trace  = "./examples/model_correction/a12f0n05_aligned_to_00.log.txt";
    
    //String fileName_system_sysPath = "./examples/model_correction/a12f0n05_alpha.lola";
    //String fileName_trace  = "./examples/model_correction/a12f0n05_aligned.log.txt";
    //String fileName_trace  = "./examples/model_correction/a12f0n05_aligned.log.txt";
    
    String fileName_system_sysPath = "./examples/model_correction/a22f0n00.lola";
    String fileName_trace  = "./examples/model_correction/a22f0n05.log_50.txt";

    
    PetriNet sysModel = PetriNetIO.readNetFromFile(fileName_system_sysPath);
    List<String[]> allTraces = ViewGeneration2.readTraces(fileName_trace);
    
    sysModel.makeNormalNet();
    sysModel.turnIntoLabeledNet();
    
    EventStructureReplay replay = new EventStructureReplay(sysModel);

    Uma.out.println("build up event structure");
    for (String[] trace : allTraces) {
      replay.extendByTrace(trace);
    }
    Uma.out.println(replay.es.allEvents.size()+" events");

    Uma.out.println("remove transitive dependencies");
    replay.es.removeTransitiveDependencies();
    OcletIO.writeFile(replay.es.toDot(replay.properNames), fileName_system_sysPath+"_es.dot");
    
    Uma.out.println("refining conflicts");
    replay.refineConflicts();
    Uma.out.println("reduce transitive conflicts");
    replay.es.reduceTransitiveConflicts();
    OcletIO.writeFile(replay.es.toDot(replay.properNames), fileName_system_sysPath+"_es2.dot");

    Uma.out.println("set single events");
    replay.setSingleEvents();
    Uma.out.println("identify ordering relations");
    replay.getConcurrentEvents();
    Uma.out.println("maximizing concurrency");
    replay.extendConcurrency();
    

    Uma.out.println("refining conflicts");
    replay.refineConflicts();
    Uma.out.println("reducing transitive conflicts");
    replay.es.reduceTransitiveConflicts();
    Uma.out.println("reducing transitive dependencies");
    replay.es.removeTransitiveDependencies();
    Uma.out.println("removing tau events");
    replay.removeSuperFluousTauEvents();
    Uma.out.println("refining conflicts");
    replay.refineConflicts();
    Uma.out.println("reducing transitive conflicts");
    replay.es.reduceTransitiveConflicts();
    Uma.out.println("reducing transitive dependencies");
    replay.es.removeTransitiveDependencies();
    
    OcletIO.writeFile(replay.es.toDot(replay.properNames), fileName_system_sysPath+"_es3.dot");
    
    Uma.out.println("folding forward");
    replay.foldForward();
    //replay.removeSuperFluousTauEvents();
    //replay.refineConflicts();
    Uma.out.println("reducing transitive conflicts");
    replay.es.reduceTransitiveConflicts();
    Uma.out.println("reducing transitive dependencies");
    replay.es.removeTransitiveDependencies();
    
    OcletIO.writeFile(replay.es.toDot(replay.properNames), fileName_system_sysPath+"_es4.dot");
    
    for (Event e : replay.es.allEvents) {
      Set<DNode> ev = e.getAllPredecessors();
      for (DNode e2 : ev) {
        if (replay.es.inConflict(e, (Event)e2)) {
          System.err.println("Error4: "+e+" #  "+e2);
        }
        if (replay.es.inDirectConflict(e, (Event)e2)) {
          System.err.println("Error4: "+e+" #! "+e2);
        }

      }
    }
    
    Uma.out.println("synthesizing Petri net from "+replay.es.allEvents.size()+" events");
    SynthesisFromES synth = new SynthesisFromES(replay.properNames, fileName_system_sysPath);
    PetriNet net = synth.synthesize(replay.es);
    
    PetriNetIO.writeToFile(net, fileName_system_sysPath+"_refold", PetriNetIO.FORMAT_DOT, 0);
    PetriNetIO.writeToFile(net, fileName_system_sysPath+"_refold", PetriNetIO.FORMAT_LOLA, 0);
  }

}
