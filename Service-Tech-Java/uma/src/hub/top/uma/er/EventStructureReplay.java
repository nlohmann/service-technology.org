package hub.top.uma.er;

import hub.top.petrinet.Node;
import hub.top.petrinet.PetriNet;
import hub.top.petrinet.PetriNetIO;
import hub.top.petrinet.Place;
import hub.top.petrinet.Transition;
import hub.top.scenario.OcletIO;
import hub.top.uma.DNode;
import hub.top.uma.DNodeSys;
import hub.top.uma.er.EventStructure.EventCollection;
import hub.top.uma.er.SynthesisFromES;
import hub.top.uma.view.ViewGeneration2;

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
  }
  
  private Map<Event, List<Token> > consumedFrom;
  
  private Map<Event, Set<Integer> > inTrace;
  
  private int traceCount = 0;
  
  public void extendByTrace(String trace[]) {
    
    traceCount++;
    
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

      // get the predecessor events based on the consumed tokens
      HashSet<Event> preEvents = new HashSet<Event>();
      for (Token m : toConsume) {
        preEvents.add(m.producedBy);
      }
      
      Event pre[] = new Event[preEvents.size()];
      pre = preEvents.toArray(pre);
      
      short toFireID = nameToID.get(toFire.getName());
      
      // see if this event was already fired
      Event newEvent = null;
      if (lastEvent.post != null) {
        for (DNode d : lastEvent.post) {
          Event e = (Event)d;
          if (e.id == toFireID) {
            newEvent = e;
          }
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
                if (!es.alreadyInConflict(newEvent, (Event)conflictingEvent))
                  es.setDirectConflict(newEvent, (Event)conflictingEvent);
              }
            }
          }
        }
      }

      // update the marking
      marking.removeAll(toConsume);
      for (Place p : toFire.getPostSet()) {
        marking.add(new Token(p,newEvent));
      }
      
      lastEvent = newEvent;
      
      // remember that the event participates in this trace
      if (!inTrace.containsKey(lastEvent)) inTrace.put(lastEvent, new HashSet<Integer>());
      inTrace.get(lastEvent).add(traceCount);
    }
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
  
  public static void main(String args[]) throws Throwable {
    
    //String fileName_system_sysPath = args[0];
    //String fileName_trace = args[1];
    
    //String fileName_system_sysPath = "./examples/model_correction/a12f0n00_alpha.lola";
    //String fileName_trace  = "./examples/model_correction/a12f0n00.log.txt";

    //String fileName_system_sysPath = "./examples/model_correction/a12f0n00_alpha.lola";
    //String fileName_trace  = "./examples/model_correction/a12f0n05_aligned_to_00.log.txt";
    
    String fileName_system_sysPath = "./examples/model_correction/a12f0n05_alpha.lola";
    String fileName_trace  = "./examples/model_correction/a12f0n05_aligned.log.txt";
    
    PetriNet sysModel = PetriNetIO.readNetFromFile(fileName_system_sysPath);
    List<String[]> allTraces = ViewGeneration2.readTraces(fileName_trace);
    
    sysModel.makeNormalNet();
    sysModel.turnIntoLabeledNet();
    
    EventStructureReplay replay = new EventStructureReplay(sysModel);
    
    for (String[] trace : allTraces) {
      replay.extendByTrace(trace);
    }
    replay.es.removeTransitiveDependencies();
    OcletIO.writeFile(replay.es.toDot(replay.properNames), fileName_system_sysPath+"_es.dot");
    replay.refineConflicts();
    replay.es.reduceTransitiveConflicts();
    OcletIO.writeFile(replay.es.toDot(replay.properNames), fileName_system_sysPath+"_es2.dot");
    
    replay.setSingleEvents();
    replay.getConcurrentEvents();
    replay.coarsenCausality();
    //replay.refineConflicts();
    replay.es.removeTransitiveDependencies();
    OcletIO.writeFile(replay.es.toDot(replay.properNames), fileName_system_sysPath+"_es3.dot");
    
    SynthesisFromES synth = new SynthesisFromES(replay.properNames, fileName_system_sysPath);
    PetriNet net = synth.synthesize(replay.es);
    
    PetriNetIO.writeToFile(net, fileName_system_sysPath+"_refold", PetriNetIO.FORMAT_DOT, 0);
    PetriNetIO.writeToFile(net, fileName_system_sysPath+"_refold", PetriNetIO.FORMAT_LOLA, 0);
  }

}
