package hub.top.uma.er;

import hub.top.petrinet.ISystemModel;
import hub.top.petrinet.PetriNet;
import hub.top.scenario.OcletIO_Out;
import hub.top.uma.DNode;
import hub.top.uma.DNodeRefold;
import hub.top.uma.DNodeSet;
import hub.top.uma.DNodeSys;
import hub.top.uma.Uma;
import hub.top.uma.synthesis.TransitiveDependencies;
import hub.top.uma.view.ViewGeneration2;

import java.util.Collection;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.TreeSet;

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
  
  /**
   * Remove the 'nodes' from the node set.
   * @param nodes
   */
  public void removeAll(Collection<Event> nodes) {
    
    HashSet<DNode> touched = new HashSet<DNode>();
    
    HashSet<DNode> toRemove = new HashSet<DNode>(nodes);
    
    for (DNode n : toRemove) {
      if (n.pre != null) {
        for (DNode pre : n.pre) {
          if (pre == null) continue;
          if (toRemove.contains(pre)) continue;
          
          for (int i=0; i<pre.post.length; i++) {
            if (pre.post[i] == n) {
              pre.post[i] = null;
              break;
            }
          }
          touched.add(pre);
        }
      }
      
      if (n.post != null) {
        for (DNode post : n.post) {
          if (post == null) continue;
          if (toRemove.contains(post)) continue;
          
          for (int i=0; i<post.pre.length; i++) {
            if (post.pre[i] == n) {
              post.pre[i] = null;
              break;
            }
          }
          
          touched.add(post);
        }
      }
    }
    
    for (DNode e : toRemove) {
      //System.out.println("remove "+e);
      removeFromConflicts((Event)e);
    }
    
    EventCollection newEvents = new EventCollection();
    for (Event e : allEvents) {
      if (!toRemove.contains(e)) newEvents.add(e);
    }
    allEvents = newEvents;
    
    for (DNode n : touched) {
      
      if (n.pre != null) {
        int preCount = 0;
        for (DNode pre : n.pre) {
          if (pre != null) preCount++;
        }
        
        DNode newPre[] = new DNode[preCount];
        int newPrePos = 0;
        for (DNode pre : n.pre) {
          if (pre != null) {
            newPre[newPrePos] = pre;
            newPrePos++;
          }
        }
        
        n.pre = newPre;
      }
      
      if (n.post != null) {
        int postCount = 0;
        for (DNode post : n.post) {
          if (post != null) postCount++;
        }
        
        DNode newPost[] = new DNode[postCount];
        int newPostPos = 0;
        for (DNode post : n.post) {
          if (post != null) {
            newPost[newPostPos] = post;
            newPostPos++;
          }
        }
        
        n.post = newPost;
      }
    }
  }
  

  /**
   * @param current
   * @param directConflicts consider only direct conflicts, or also transitive conflicts
   * @return all conflict clusters in the 'current' set of events
   */
  public List<Set<Event>> getConflictClusters(Set<Event> current, boolean directConflicts) {
    Event[] c_events = current.toArray(new Event[current.size()]);
    boolean adj_matrix[][] = new boolean [c_events.length][c_events.length];
    
    for (int i=0;i<c_events.length;i++) {
      for (int j=0;j<c_events.length;j++) {
        if (i == j) adj_matrix[i][j] = true;
        else {
          if (directConflicts)
            adj_matrix[i][j] = directConflict.containsKey(c_events[i]) && directConflict.get(c_events[i]).contains(c_events[j]);
          else
            adj_matrix[i][j] = inConflict(c_events[i], c_events[j]);
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
  
  /**
   * @return all conflict clusters of the event structure
   */
  public Set<Set<Event>> getConflictClusters() {
    Set<Set<Event>> cc = new HashSet<Set<Event>>();
    
    int count = 0;
    for (Event e : getAllEvents()) {
      count++;
      
      if (directConflict.containsKey(e)) {
        Set<Event> current = new HashSet<Event>();
        for (Event f : directConflict.get(e)) current.add(f);
        current.add(e);
        
        //System.out.println(count+"/"+es.allEvents.size()+":"+current.size());
        
        //System.out.println("check "+current);
        //getConflictCluster(es, current, cc);

        // find conflict clusters for the current events
        for (Set<Event> _cc : getConflictClusters(current, true)) {
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
  
  private void _setDirectConflict(Event e1, Event e2) {
    if (e1 == e2) {
      System.err.println("setting self-conflict on: "+e1);
    }
    if (!directConflict.containsKey(e1)) directConflict.put(e1, new HashSet<Event>());
    directConflict.get(e1).add(e2);
  }

  public void setDirectConflict(Event e1, Event e2) {
    //System.out.println("setting "+e1+" # "+e2);
    _setDirectConflict(e1, e2);
    _setDirectConflict(e2, e1);
  }
  
  public void removeFromConflicts(Event n) {
    if (directConflict.containsKey(n)) {
      for (DNode n2 : directConflict.get(n)) {
        if (n2 != n) {
          if (directConflict.containsKey(n2)) {
            directConflict.get(n2).remove(n);
            if (directConflict.get(n2).isEmpty()) directConflict.remove(n2);
          }
        }
      }
      directConflict.remove(n);
    }
    
    for (Event e : directConflict.keySet()) {
      if (directConflict.get(e).contains(n)) {
        System.out.println(e+" still has "+n+" in conflict");
      }
    }
  }
  
  public boolean alreadyInConflict_byOther(Event e, Event f) {
    Set<Event> e_pred = getPrimeConfiguration(e, true);
    Set<Event> f_pred = getPrimeConfiguration(e, true);
    return alreadyInConflict_byOther(e_pred, e, f_pred, f);
  }
  
  public boolean alreadyInConflict_byOther(Set<Event> e_pred, Event e, Set<Event> f_pred, Event f) {
    for (Event e_pre : e_pred) {
      for (Event f_pre : f_pred) {
        if (e_pre == e && f_pre == f) continue;
        // no need to check conflict of events that are causally dependent
        if (e_pred.contains(f_pre)) continue;
        
        if (directConflict.containsKey(e_pre) && directConflict.get(e_pre).contains(f_pre)) {
          return true;
        }
      }
    }
    
    return false;
  }
  
  /**
   * @param e
   * @param f
   * @return {@code true} iff events e and f are in direct conflict with the same set of events
   */
  public boolean conflictEquivalent(Event e, Event f) {
    int num_e = (directConflict.containsKey(e)) ? directConflict.get(e).size() : 0;
    int num_f = (directConflict.containsKey(f)) ? directConflict.get(f).size() : 0;
    
    if (num_e == 0 && num_f == 0) return true;
    if (num_e > 0 && num_e == num_f) return directConflict.get(e).equals(directConflict.get(f));
    return false;
  }
  
  public boolean reduceTransitiveConflicts() {
    return reduceTransitiveConflicts(getAllEvents());
  }
  
  public boolean reduceTransitiveConflicts(EventCollection events) {
    
    boolean changed = false;
    for (Event e : events) {
      Set<Event> e_pred = null;
      
      if (directConflict.containsKey(e)) {
        for (Event f : directConflict.get(e))
        {
          if (e_pred == null) e_pred = getPrimeConfiguration(e, true);
          Set<Event> f_pred = getPrimeConfiguration(f, true);
          
          if (alreadyInConflict_byOther(e_pred, e, f_pred, f)) {
            directConflict.get(e).remove(f);
            directConflict.get(f).remove(e);
            changed = true;
          }
        }
        
      }
    }
    return changed;
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
  
  public Set<Event> getPrimeSuffix(Event event, boolean withself) {

    // the events of the prime configuration of 'event'
    HashSet<Event> primeSuffix = new HashSet<Event>();

    // find all predecessor events of 'event' and their post-conditions
    // by backwards breadth-first search from 'event'
    LinkedList<Event> queue = new LinkedList<Event>();
    queue.add(event);
    if (withself) primeSuffix.add(event);

    // run breadth-first search
    while (!queue.isEmpty()) {

      Event first = queue.removeFirst();
      if (first.post == null) continue;
      
      for (DNode postNode : first.post) {
        Event postEvent = (Event) postNode;
        if (!primeSuffix.contains(postEvent)) {
          // only add to the queue if not queued already
          queue.add(postEvent);
          primeSuffix.add(postEvent);
        } // all predecessor events of preCondition
      }
    }
    return primeSuffix;
  }
  
  public boolean inDirectConflict(Event e1, Event e2) {
    if (directConflict.containsKey(e1) && directConflict.get(e1).contains(e2)) return true;
    return false;
  }
  
  public boolean inConflict(Event e1, Event e2) {
    Set<Event> e1_pred = getPrimeConfiguration(e1, true);
    Set<Event> e2_pred = getPrimeConfiguration(e2, true);
    return inConflict(e1_pred, e1, e2_pred, e2);
  }
  
  public boolean inConflict(Set<Event> e1_pred, Event e1, Set<Event> e2_pred, Event e2) {
    
    for (Event f : e1_pred) {
      if (directConflict.containsKey(f)) {
        for (Event f_confl : directConflict.get(f)) {
          if (e2_pred.contains(f_confl)) {
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
  
  public boolean removeDependency(Event e1, Event e2) {
    // see if the dependency exists
    if (e1.post != null) {
      boolean exists = false;
      for (DNode f : e1.post) if (f == e2) { exists = true; break; }
      if (!exists) return false;
    } else {
      return false;
    }
    
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
    return true;
  }
  
  public void setDependency(Event e1, Event e2) {
    // see if the dependency already exists
    if (e1.post != null) for (DNode f : e1.post) if (f == e2) return;
    
    //System.out.println("set "+e1+" -> "+e2);
    
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
      
      for (DNode f : toRemove) {
        //System.out.println("remove "+f+" -> "+e);
        removeDependency((Event)f, e);
      }
      
      /*
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
      }*/
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
            b.append("  e"+confl.globalId+" -> e"+n.globalId+" [weight=100.0 arrowhead=none color=red]\n");
          }
        }
      }
    }
    
    b.append("}");
    return b.toString();
  }
  
  private boolean allPostEventsVisited(Event e_equiv, Map<Event, Event> canonical) {
    if (e_equiv.post == null || e_equiv.post.length == 0) return true;
    // see if each post-event of 'eEquiv' has been visited, if not,
    // then we cannot fold e_equiv yet
    for (DNode f : e_equiv.post) {
      if (!canonical.containsKey(f)) {
        //System.out.println("not seen "+f);
        return false;
      }
    }
    
    return true;
  }
  
  public Map<Event, Set<Event>> equiv;
  public Map<Event, Event> canonical;
  
  public void foldBackwards() {
    
    canonical = new HashMap<Event, Event>();
    equiv = new HashMap<Event, Set<Event>>();
    
    LinkedList<Event> queue = new LinkedList<Event>();
    
    // collect all maximal events
    LinkedList<Event> maxEvents = new LinkedList<Event>();
    for (Event e : allEvents) {
      if (e.post == null || e.post.length == 0) maxEvents.add(e);
    }
    
    // partition into classes of events of the same label
    Map<Short, Set<Event>> label_equiv = new HashMap<Short, Set<Event>>();
    for (Event e : maxEvents) {
      if (!label_equiv.containsKey(e.id)) label_equiv.put(e.id, new HashSet<Event>());
      label_equiv.get(e.id).add(e);
    }
    
    // make each class an equivalence class with the event with the smallest global id
    // as canonical representative, add the canonical representative to the queue
    for (Set<Event> eq : label_equiv.values()) {
      Event minEvent = null;
      for (Event e : eq) {
        if (minEvent == null) minEvent = e;
        else if (minEvent.globalId > e.globalId) minEvent = e;
      }
      
      for (Event e : eq) {
        canonical.put(e, minEvent);
        queue.addLast(e);
        //System.out.println("add "+e);
      }
      equiv.put(minEvent, eq);
    }
    
    int count = 0;
    

    while (!queue.isEmpty()) {
      
      Event e = queue.removeFirst();
      
      //System.out.println(equiv.get(canonical.get(e)));

      // check that for all predecessors of 'e', each post-event has
      // been visited (otherwise we cannot fold for these events),
      // correspondingly for all events equivalent to 'e'
      boolean seen_allPosts_of_allPre_of_allEquiv = true;
      all_e_equiv: for (Event e_equiv : equiv.get(canonical.get(e))) {

        if (e_equiv.pre == null || e_equiv.pre.length == 0) continue;
        for (DNode f : e_equiv.pre) {
          if (!allPostEventsVisited((Event)f, canonical)) {
            seen_allPosts_of_allPre_of_allEquiv = false;
            break all_e_equiv;
          }
        }
        
      }
      // no, some equivalent event 'e' has some predecessor that has 
      // not all its post-events visited. push 'e' back in the queue
      if (!seen_allPosts_of_allPre_of_allEquiv) {
        /*
        queue.addLast(e);
        System.out.println("skip "+e);
        
        count++;
        if (count > 5) break;
        
        continue;
        */
        continue;
      }
      
      label_equiv.clear();
      // get all predecessors and put them into equivalence classes of the same label
      for (Event e_equiv : equiv.get(canonical.get(e))) {
        if (e_equiv.pre == null || e_equiv.pre.length == 0) continue;
        for (DNode _f : e_equiv.pre) {
          Event f = (Event)_f;
          if (!label_equiv.containsKey(f.id)) label_equiv.put(f.id, new HashSet<Event>());
          label_equiv.get(f.id).add(f);
        }
      }
      // partition equivalence classes by equivalent successors
      for (Set<Event> label_eq : label_equiv.values()) {
        Map<Set<Event>,Set<Event>> succ_classes = new HashMap<Set<Event>, Set<Event>>();
        

        for (Event e_label : label_eq) {
          Set<Event> succ = new HashSet<Event>();
          if (e_label.post != null)
            for (DNode _f : e_label.post) {
              Event f = (Event)_f;
              succ.add(canonical.get(f));
            }
          
          if (!succ_classes.containsKey(succ)) succ_classes.put(succ, new HashSet<Event>());
          succ_classes.get(succ).add(e_label);
          
          
        }
        
        // make each class an equivalence class with the event with the smallest global id
        // as canonical representative, add the canonical representative to the queue
        for (Set<Event> succ_eq : succ_classes.values()) {
          
          /*
          // perhaps some of the events are already in some equivalence class
          // take the union of all events
          Set<Event> all = new HashSet<Event>();
          for (Event e_succ : succ_eq) {
            all.add(e_succ);
            if (canonical.containsKey(e_succ)) {
              all.addAll(equiv.get(canonical.get(e_succ)));
            }
          }
          */
          
          Event minEvent = null;
          for (Event e_succ : succ_eq) {
            if (minEvent == null) minEvent = e_succ;
            else if (minEvent.globalId > e_succ.globalId) minEvent = e_succ;
          }
          
          for (Event e_succ : succ_eq) {

            // visit folded nodes - but only those that have not been visited yet
            if (!canonical.containsKey(e_succ)) {
              queue.addLast(e_succ);
              //System.out.println("add "+e_succ);
            }
            
            // clear old equivalence class
            if (equiv.containsKey(e_succ) && e_succ != minEvent) equiv.remove(e_succ);
            canonical.put(e_succ, minEvent);
          }
          
          if (!equiv.containsKey(minEvent)) equiv.put(minEvent, new HashSet<Event>());
          equiv.get(minEvent).addAll(succ_eq);
        }
      }
    }
    
    for (Event e : allEvents) {
      if (!canonical.containsKey(e)) {
        canonical.put(e, e);
        equiv.put(e, new HashSet<Event>());
        equiv.get(e).add(e);
      }
    }
    
    System.out.println("-------------------> folding relation on events");
    for (Set<Event> eq : equiv.values()) {
      System.out.println(eq);
    }
  }
  
  public void refineFoldingByConflicts() {
    
    boolean changed = true;
    while (changed) {
      changed = false;

      Map<Set<Event>,Set<Event>> conflict_sets = new HashMap<Set<Event>, Set<Event>>();
      Event considered = null;
      
      for (Event e : equiv.keySet()) {
        Set<Event> e_class = equiv.get(e);
        
        conflict_sets.clear();

        for (Event f : e_class) {
          Set<Event> conf_canonical = new HashSet<Event>();
          
          if (directConflict.containsKey(f)) {
            for (Event f_confl : directConflict.get(f)) conf_canonical.add(canonical.get(f_confl));
          }
          
          if (!conflict_sets.containsKey(conf_canonical)) conflict_sets.put(conf_canonical, new HashSet<Event>());
          conflict_sets.get(conf_canonical).add(f); 
        }
        
        if (conflict_sets.size() > 1) {
          System.out.println(e_class+" --> "+conflict_sets.values());
          considered = e;

          break;
        }
      }
      
      if (considered != null) {
        changed = true;
        
        equiv.remove(considered);
        
        for (Set<Event> conf_eq : conflict_sets.values()) {
          
          Event minEvent = null;
          for (Event e_succ : conf_eq) {
            if (minEvent == null) minEvent = e_succ;
            else if (minEvent.globalId > e_succ.globalId) minEvent = e_succ;
          }
          
          for (Event e_succ : conf_eq) {
  
            // clear old equivalence class
            if (equiv.containsKey(e_succ) && e_succ != minEvent) equiv.remove(e_succ);
            canonical.put(e_succ, minEvent);
          }
          
          if (!equiv.containsKey(minEvent)) equiv.put(minEvent, new HashSet<Event>());
          equiv.get(minEvent).addAll(conf_eq);
        }
      }
      
    }
    
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
    
    OcletIO_Out.writeFile(es.toDot(sys.properNames), fileName_system_sysPath+"_es.dot");
  }
}
