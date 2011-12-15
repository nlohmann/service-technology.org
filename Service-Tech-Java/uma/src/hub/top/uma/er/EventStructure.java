package hub.top.uma.er;

import hub.top.petrinet.ISystemModel;
import hub.top.petrinet.PetriNet;
import hub.top.scenario.OcletIO;
import hub.top.uma.DNode;
import hub.top.uma.DNodeRefold;
import hub.top.uma.DNodeSet;
import hub.top.uma.DNodeSys;
import hub.top.uma.InvalidModelException;
import hub.top.uma.Uma;
import hub.top.uma.synthesis.TransitiveDependencies;
import hub.top.uma.view.ViewGeneration2;

import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Set;

import com.google.gwt.dev.util.collect.HashMap;
import com.google.gwt.dev.util.collect.HashSet;

public class EventStructure {
  
  /**
   * This shall become an efficient data structure for storing and indexing
   * sets of {@link Events}s.
   * 
   * TODO implement
   * 
   * @author Dirk Fahland
   */
  public static class EventCollection extends java.util.LinkedList<Event>  {

    private static final long serialVersionUID = -1007001237122556557L;

  }

  public EventCollection allEvents;
  public Set<Event> maxNodes;
  public Map<Event, Set<Event> > directConflict;
  
  public EventStructure() {
    allEvents = new EventCollection();
    maxNodes = new HashSet<Event>();
    directConflict = new HashMap<Event, Set<Event>>();
  }
  
  public void add(Event newNode) {
    assert newNode != null : "Error! Trying to insert null node into EventStructure";
    
    // the predecessors of newNode are no longer maxNodes in this set
    // remove all nodes from this set's maxNodes that are in the pre-set of newNode
    EventCollection toRemove = new EventCollection();
    for (Event e : maxNodes) {
      if (newNode.preContains_identity(e)) {
        toRemove.add(e);
      }
    }
    for (Event e : toRemove) maxNodes.remove(e);
    
    // and add the newNode to maxNodes
    maxNodes.add(newNode);
    
    // and store the newNode as event
    allEvents.add(newNode);
  }
  
  private void _setDirectConflict(Event e1, Event e2) {
    if (e1 == e2)  System.err.println("setting self-conflict on: "+e1);
    if (!directConflict.containsKey(e1)) directConflict.put(e1, new HashSet<Event>());
    directConflict.get(e1).add(e2);
  }

  public void setDirectConflict(Event e1, Event e2) {
    _setDirectConflict(e1, e2);
    _setDirectConflict(e2, e1);
  }
  
  public boolean alreadyInConflict(Event e, Event f) {
    Set<Event> e_pres = getPrimeConfiguration(e, true);
    Set<Event> f_pres = getPrimeConfiguration(f, true);
    
    for (Event e_pre : e_pres) {
      for (Event f_pre : f_pres) {
        if (e_pre == e && f_pre == f) continue;
        if (e_pres.contains(f_pre)) continue;
        
        if (directConflict.containsKey(e_pre) && directConflict.get(e_pre).contains(f_pre)) {
          return true;
        }
      }
    }
    
    return false;
  }
  
  public void reduceTransitiveConflicts() {
    for (Event e : getAllEvents()) {
      for (Event f : getAllEvents()) {
        
        if (directConflict.containsKey(e) && directConflict.get(e).contains(f)) {
          if (alreadyInConflict(e, f)) {
            directConflict.get(e).remove(f);
            directConflict.get(f).remove(e);
          }
        }
        
      }
    }
  }
  


  public Set<Event> getPrimeConfiguration(Event event, boolean withself) {

    // the events of the prime configuration of 'event'
    HashSet<Event> primeConfiguration = new HashSet<Event>();

    // find all predecessor events of 'event' and their post-conditions
    // by backwards breadth-first search from 'event'
    LinkedList<Event> queue = new LinkedList<Event>();
    queue.add(event);
    if (withself) primeConfiguration.add(event);

    // run breadth-first search
    while (!queue.isEmpty()) {

      Event first = queue.removeFirst();

      for (DNode preNode : first.pre) {
        Event preEvent = (Event) preNode;
        if (!primeConfiguration.contains(preEvent)) {
          // only add to the queue if not queued already
          queue.add(preEvent);
          primeConfiguration.add(preEvent);
        } // all predecessor events of preCondition
      }
    }
    return primeConfiguration;
  }
  
  public boolean inDirectConflict(Event e1, Event e2) {
    if (directConflict.containsKey(e1) && directConflict.get(e1).contains(e2)) return true;
    return false;
  }
  
  public boolean inConflict(Event e1, Event e2) {
    Set<Event> e1_pre = getPrimeConfiguration(e1, true);
    Set<Event> e2_pre = getPrimeConfiguration(e2, true);
    
    for (Event f : e1_pre) {
      if (directConflict.containsKey(f)) {
        for (Event f_confl : directConflict.get(f)) {
          if (e2_pre.contains(f_confl)) {
            return true;
          }
        }
      }
    }
    return false;
  }
  
  public boolean isTransitiveDependent(Event e1, Event e2) {
    boolean directDependent = false;
    for (int i=0;i<e2.pre.length; i++) {
      if (e2.pre[i] == e1) {
        directDependent = true;
        break;
      }
    }
    if (!directDependent) return false;
    
    // collect all direct predecessor events of 'e2'
    HashSet<Event> seen = new HashSet<Event>();
    LinkedList<Event> queue = new LinkedList<Event>();
    for (DNode d : e2.pre) {
      // skip 'e1' as direct predecessor of 'e2'
      if (d != e1) {
        queue.add((Event)d);
        seen.add((Event)d);
      }
    }
    // run breadth-first search: if we find 'e1', then the
    // dependency from 'e1' to 'e2' is transitive
    while (!queue.isEmpty()) {

      Event first = queue.removeFirst();

      for (DNode preNode : first.pre) {
        Event preEvent = (Event) preNode;
        if (!seen.contains(preEvent)) {
          
          if (preEvent == e1) return true;
          
          // only add to the queue if not queued already
          queue.add(preEvent);
          seen.add(preEvent);
        } // all predecessor events of preCondition
      }
    }
    // 'e1' not seen
    return false;
  }
  
  public void removeDependency(Event e1, Event e2) {
    DNode newPre[] = new DNode[e2.pre.length-1];
    int prePos = 0;
    for (DNode f : e2.pre) {
      if (f != e1) newPre[prePos++] = f;
    }
    e2.pre = newPre;

    DNode newPost[] = new DNode[e1.post.length-1];
    int postPos = 0;
    for (DNode f : e1.post) {
      if (f != e2) newPost[postPos++] = f;
    }
    e1.post = newPost;
  }
  
  public void setDependency(Event e1, Event e2) {
    // see if the dependency already exists
    for (DNode f : e1.post) if (f == e2) return;
    
    e2.addPreNode(e1);
    e1.addPostNode(e2);
  }

  
  public void removeTransitiveDependencies() {
    for (Event e : allEvents) {
      if (e.pre == null) continue;
      List<DNode> toRemove = new LinkedList<DNode>();
      for (DNode f : e.pre) {
        if (isTransitiveDependent((Event)f, e)) {
          toRemove.add(f);
        }
      }
      
      DNode newPre[] = new DNode[e.pre.length-toRemove.size()];
      int prePos = 0;
      for (DNode f : e.pre) {
        if (!toRemove.contains(f)) newPre[prePos++] = f;
      }
      e.pre = newPre;
      for (DNode f : toRemove) {
        DNode newPost[] = new DNode[f.post.length-1];
        int postPos = 0;
        for (DNode g : f.post) {
          if (g != e) newPost[postPos++] = g;
        }
        f.post = newPost;
      }
    }
  }
  
  public EventStructure(DNodeSet set) {
    
    this();
    
    Map<DNode, Event> map = new HashMap<DNode, Event>();
    
    for (DNode d : set.getAllEvents()) {
      
      Set<Event> preSet = new HashSet<Event>();
      if (d.pre != null) {
        for (DNode b : d.pre) {
          if (b.pre != null) {
            for (DNode d2 : b.pre) preSet.add(map.get(d2));
          }
        }
      }
      
      Event pre[] = new Event[preSet.size()];
      preSet.toArray(pre);
      DNode.sortIDs(pre);
      
      Event e = new Event(d.id, pre);
      add(e);
      
      for (DNode f : pre) f.addPostNode(e);
      
      map.put(d, e);
    }
    
    for (DNode b : set.getAllConditions()) {

      if (b.post != null && b.post.length > 1) {
        for (int i=0;i<b.post.length;i++) {
          for (int j=i+1;j<b.post.length;j++) {
            setDirectConflict(map.get(b.post[i]), map.get(b.post[j]));
          }
        }
      }
    }
  }
  
  public EventCollection getAllEvents() {
    return allEvents;
  }
  
  public String toDot (String properNames[]) {
    return toDot(properNames, new HashMap<DNode, String>());
  }
  
  /**
   * Create a GraphViz' dot representation of this branching process.
   * 
   * @return 
   */
  public String toDot (String properNames[], HashMap<DNode,String> coloring) {
    StringBuilder b = new StringBuilder();
    b.append("digraph BP {\n");
    
    // standard style for nodes and edges
    b.append("graph [fontname=\"Helvetica\" nodesep=0.3 ranksep=\"0.2 equally\" fontsize=10];\n");
    b.append("node [fontname=\"Helvetica\" fontsize=8 fixedsize width=\".3\" height=\".3\" label=\"\" style=filled fillcolor=white];\n");
    b.append("edge [fontname=\"Helvetica\" fontsize=8 color=white arrowhead=none weight=\"20.0\"];\n");

    //String tokenFillString = "fillcolor=black peripheries=2 height=\".2\" width=\".2\" ";
    String cutOffFillString = "fillcolor=gold";
    String antiFillString = "fillcolor=red";
    String impliedFillString = "fillcolor=violet";
    String hiddenFillString = "fillcolor=grey";
    
    // print all events
    b.append("\n\n");
    b.append("node [shape=box];\n");
    for (DNode n : getAllEvents()) {
      
      if (coloring.containsKey(n)) {
        String rgbColorFillString = "fillcolor="+coloring.get(n);
        b.append("  e"+n.globalId+" ["+rgbColorFillString+"]\n");
      }
      else if (n.isAnti && n.isHot)
        b.append("  e"+n.globalId+" ["+antiFillString+"]\n");
      else if (n.isAnti && !n.isHot)
        b.append("  e"+n.globalId+" ["+hiddenFillString+"]\n");
      else if (n.isImplied)
        b.append("  e"+n.globalId+" ["+impliedFillString+"]\n");
      else if (n.isCutOff)
        b.append("  e"+n.globalId+" ["+cutOffFillString+"]\n");
      else
        b.append("  e"+n.globalId+" []\n");
      
      String auxLabel = "";
      String antiLabel = n.isAnti ? "--" : "";
      String nodeLabel = "'"+properNames[n.id]+"'"+antiLabel+" ("+n.id+")["+n.globalId+"]";

      b.append("  e"+n.globalId+"_l [shape=none];\n");
      b.append("  e"+n.globalId+"_l -> e"+n.globalId+" [headlabel=\""+nodeLabel+" "+auxLabel+"\"]\n");
    }
    
    b.append(" edge [fontsize=8 arrowhead=normal color=black];\n");
    for (DNode n : getAllEvents()) {
      for (int i=0; i<n.pre.length; i++) {
        if (n.pre[i] == null) continue;
        
        String rgbColorString = "";
        if (coloring.containsKey(n)) {
          rgbColorString = "color="+coloring.get(n);
        }
        
        b.append("  e"+n.pre[i].globalId+" -> e"+n.globalId+" [weight=10000.0 "+rgbColorString+"]\n");
      }
      
      if (directConflict.containsKey(n)) {
        for (Event confl : directConflict.get(n)) {
          if (confl.globalId < n.globalId) {
            b.append("  e"+confl.globalId+" -> e"+n.globalId+" [weight=10000.0 arrowhead=none color=red]\n");
          }
        }
      }
    }
    
    b.append("}");
    return b.toString();
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
    
    OcletIO.writeFile(es.toDot(sys.properNames), fileName_system_sysPath+"_es.dot");
  }
}
