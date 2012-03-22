/*****************************************************************************\
 * Copyright (c) 2008, 2009, 2010. Dirk Fahland. AGPL3.0
 * All rights reserved. 
 * 
 * ServiceTechnology.org - Uma, an Unfolding-based Model Analyzer
 * 
 * This program and the accompanying materials are made available under
 * the terms of the GNU Affero General private License Version 3 or later,
 * which accompanies this distribution, and is available at 
 * http://www.gnu.org/licenses/agpl.txt
 * 
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 * 
\*****************************************************************************/

package hub.top.uma.repair;

import hub.top.petrinet.ISystemModel;
import hub.top.petrinet.PetriNet;
import hub.top.petrinet.PetriNetIO;
import hub.top.petrinet.PetriNetIO_Out;
import hub.top.petrinet.Place;
import hub.top.petrinet.Transition;
import hub.top.petrinet.unfold.DNodeSys_PetriNet;
import hub.top.scenario.OcletIO_Out;
import hub.top.uma.DNode;
import hub.top.uma.DNodeBP;
import hub.top.uma.DNodeBP.EnablingInfo;
import hub.top.uma.DNodeRefold;
import hub.top.uma.DNodeSet;
import hub.top.uma.DNodeSet.DNodeSetElement;
import hub.top.uma.DNodeSys;
import hub.top.uma.InvalidModelException;
import hub.top.uma.Uma;
import hub.top.uma.synthesis.NetSynthesis;
import hub.top.uma.view.DNodeBP_View;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.io.Writer;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Comparator;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Random;
import java.util.Scanner;
import java.util.Set;
import java.util.SortedSet;
import java.util.TreeSet;

import com.google.gwt.dev.util.collect.HashMap;
import com.google.gwt.dev.util.collect.HashSet;

public class ModelRepair_Simple {
  
  private DNodeRefold   build;
  private DNodeSet  bp;
  
  private Map<DNode, Integer> nodeOccurrences;
  private Map<HashSet<DNode>, LinkedList<String[]>> equivalentTraces;
  
  private class State {
    LinkedList<DNode> marking = new LinkedList<DNode>();
    ArrayList<Short> action = new ArrayList<Short>();
    ArrayList<State> succ = new ArrayList<State>();
    int seen = 0;
    
    private List<List<DNode>> getAllVisitedMarkings() {
      List<List<DNode>> result = new LinkedList<List<DNode>>();
      LinkedList<State> stack = new LinkedList<State>();
      stack.add(this);
      while (!stack.isEmpty()) {
        State s = stack.removeFirst();
        result.add(s.marking);
        for (State s2 : s.succ) {
          stack.addFirst(s2);
        }
      }
      return result;
    }
  }
  private State initialState;
  
  /**
   * Standard constructor for view generation. The argument takes a branching
   * process construction object {@link DNodeBP} after the branching process has
   * been constructed.
   *  
   * @param build
   */
  public ModelRepair_Simple (DNodeRefold build) {
    this.build = build;
    this.bp = build.getBranchingProcess();
    
    nodeOccurrences = new HashMap<DNode, Integer>();
    equivalentTraces = new HashMap<HashSet<DNode>, LinkedList<String[]>>();
    initialState = new State();

    for (DNode b : bp.initialCut) {
      initialState.marking.add(b);
    }
    //Collections.sort(initialState.marking, dnode_id_comparator);
  }
  
  public Comparator<DNode> dnode_id_comparator = new Comparator<DNode>() {

    @Override
    public int compare(DNode o1, DNode o2) {
      if (o1.globalId < o2.globalId) return -1;
      if (o1.globalId == o2.globalId) return 0;
      return 1;
    }
  };
  
  private Map<List<DNode>,Set<String>> moveOnLog = new HashMap<List<DNode>, Set<String>>();
  private Map<String, Set<List<DNode>>> addAt = new HashMap<String, Set<List<DNode>>>();
  
  private Map<DNode, DNode[]> conditionOrigin = new HashMap<DNode, DNode[]>();
  
  /**
   * Extend the branching process by the partially ordered run of the given
   * trace. A mapping determines which event corresponds to which transition
   * in the system. Also accumulate the number of actions in
   * {@link ViewGeneration#eventOccurrences}.
   *  
   * @param trace
   * @param e2t maps event names to transition names, if {@code null}, then
   *            event name and transition name are assumed to be equal
   * @return <code>true</code> if the branching process could be extended by
   * the trace, and <code>false</code> if one action could not be fired
   */
  private boolean extendByTrace(String[] trace, Map<String, String> e2t) {
    Set<DNode> run = new HashSet<DNode>();
    
    for (DNode b : bp.initialCut) {
      run.add(b);
      
      // count occurrences of conditions
      if (!nodeOccurrences.containsKey(b))
        nodeOccurrences.put(b, 0);
      nodeOccurrences.put(b, nodeOccurrences.get(b)+1);
    }

    State state = initialState;
    state.seen++;
    
    for (int i = 0; i<trace.length; i++) {
      
      String eventName;
      if (e2t == null || !e2t.containsKey(trace[i])) eventName = trace[i];
      else eventName = e2t.get(trace[i]);
      
      boolean isTauStep = false;
      
      String transitionName;
      if (eventName.startsWith("tau_")) {
        transitionName = eventName.substring(4);
        isTauStep = true;
      } else {
        transitionName = eventName;
      }
      
      SortedSet<DNode> existingEvents = new TreeSet<DNode>(dnode_id_comparator);
      for (DNode b : state.marking) {
        if (b.post == null ) continue;
        for (DNode e : b.post) {
          
          // only look for events that have the same name as the next action in the trace
          if (!build.getSystem().properNames[e.id].equals(transitionName)) continue;
          //System.out.print(" yes ");
          
          boolean alreadyExists = true;
          for (DNode ePre : e.pre) {
            if (!state.marking.contains(ePre)) { alreadyExists = false; break; }
          }
          if (alreadyExists) existingEvents.add(e);
        }
      }

      State succState = null;
      DNode fireEvent = null;
      List<DNode> produced = null;
      if (!existingEvents.isEmpty()) {
        
        // get one enabled event and fire it
        fireEvent = existingEvents.iterator().next();
        for (int s = 0; s < state.action.size(); s++) {
          if (state.action.get(s) == fireEvent.id) {
            succState = state.succ.get(s);
            break;
          }
        }
        
        if (isTauStep) fireEvent.isImplied = true;
        
        produced = new LinkedList<DNode>();
        for (DNode b : fireEvent.post) {
          produced.add(b);
        }
      }
        
      if (succState == null) {
        
        if (fireEvent == null) {
        
          HashSet<DNode> matchingEvents = new HashSet<DNode>();
          for (DNode e : build.getSystem().fireableEvents) {
            if (build.getSystem().properNames[e.id].equals(transitionName))
              matchingEvents.add(e);
          }
          
          EnablingInfo enablingInfo = getAllEnabledEvents(state.marking, matchingEvents);
  
          if (enablingInfo.locations.size() > 0)
          {
            if (enablingInfo.locations.size() > 1) {
              // we have several enabled events, issue a warning if there are
              // two events that could occur at different locations
              /*
              DNode[] loc = enablingInfo.enablingLocation.get(0);
              for (int j = 1; j<enablingInfo.enabledEvents.size(); j++) {
                if (!Arrays.equals(loc, enablingInfo.enablingLocation.get(j))) {
                  System.out.println("Warning: could fire two different events "+
                      enablingInfo.enabledEvents.get(0)+"@"+enablingInfo.enablingLocation.get(0)
                      +" and "
                      +enablingInfo.enabledEvents.get(j)+"@"+enablingInfo.enablingLocation.get(j));
  
                }
              } // end of warning
              */
            }
            
            Short eventId = enablingInfo.locations.keySet().iterator().next();
            enablingInfo.locations.get(eventId)[0].reduce();
            DNode[] events = enablingInfo.locations.get(eventId)[0].events;
            DNode[] loc = enablingInfo.locations.get(eventId)[0].loc;
            
            DNode[] postConditions = bp.fire(events, loc, false);
            if (postConditions != null && postConditions.length > 0) {
              fireEvent = postConditions[0].pre[0];          
              
              if (isTauStep) fireEvent.isImplied = true;
              else fireEvent.isImplied = false;
              
              for (int b=0; b<postConditions.length; b++) {
                DNode []causedBy = new DNode[events.length];
                for (int o=0; o<events.length; o++) {
                  causedBy[o] = events[o].post[b];
                }
                this.conditionOrigin.put(postConditions[b], causedBy);
              }
  
              // update co-relation for all new post-conditions
              //build.updateConcurrencyRelation(postConditions);
              
              // and set fields for the event
              //build.setCurrentPrimeConfig(newEvent, true);
  
            } else {
              System.out.println("fired event with empty post-set");
            }
          } else {
            
            System.out.println("could not fire "+transitionName+" ("+trace[i]+")");
            if (!this.moveOnLog.containsKey(state.marking)) this.moveOnLog.put(state.marking, new HashSet<String>());
            this.moveOnLog.get(state.marking).add(transitionName);
            
            if (!addAt.containsKey(transitionName)) this.addAt.put(transitionName, new HashSet<List<DNode>>());
            this.addAt.get(transitionName).add(state.marking);
            continue;
          }
        } // fireEvent == null
        // we found the fireEvent that matches the current trace event
        // now update the state information
          
        // compute successor marking
        succState = new State();
        succState.marking.addAll(state.marking);
        
        for (DNode b : fireEvent.pre) {
          succState.marking.remove(b);
        }
        if (fireEvent.post != null) {
          produced = new LinkedList<DNode>();
          for (DNode b : fireEvent.post) {
            produced.add(b);
          }
          //Collections.sort(produced, dnode_id_comparator);
          for (DNode b : produced) {
            succState.marking.addLast(b);
          }
        }
        
        // add state
        state.action.add(fireEvent.id);
        state.succ.add(succState);
        
        state = succState;
      }
      
      run.add(fireEvent);
      run.addAll(succState.marking);
      
      if (!nodeOccurrences.containsKey(fireEvent))
        nodeOccurrences.put(fireEvent, 0);
      nodeOccurrences.put(fireEvent, nodeOccurrences.get(fireEvent)+1);
      
      // count occurrences of conditions
      for (DNode b : produced) {
        if (!nodeOccurrences.containsKey(b))
          nodeOccurrences.put(b, 0);
        nodeOccurrences.put(b, nodeOccurrences.get(b)+1);
      }
      
      succState.seen++;
      state = succState;

    } // for all events in the trace
    
    HashSet<DNode> runCut_set = new HashSet<DNode>(state.marking);
    
    boolean newCut = true;
    for (HashSet<DNode> cut : equivalentTraces.keySet()) {
      if (cut.equals(runCut_set)) {
        equivalentTraces.get(cut).add(trace);
        newCut = false;
        break;
      }
    }
    if (newCut) {
      equivalentTraces.put(runCut_set, new LinkedList<String[]>());
      equivalentTraces.get(runCut_set).add(trace);
    }
    

    return true;
  }
  
  private EnablingInfo getAllEnabledEvents(List<DNode> marking, Collection<DNode> fireableEvents) {
    DNodeBP.EnablingInfo enablingInfo = new DNodeBP.EnablingInfo();
    
    for (DNode e : fireableEvents) {
      boolean isEnabled = true;
      DNode loc[] = new DNode[e.pre.length];
      for (int pre_index = 0; pre_index < e.pre.length; pre_index++) {
        boolean endsWith_b = false;
        for (DNode bCut : marking) {
          if (e.pre[pre_index].suffixOf(bCut)) {
            loc[pre_index] = bCut;
            endsWith_b = true; 
            break;
          }
        }
        if (!endsWith_b) { isEnabled = false; break; }
      }
      if (isEnabled) {
        enablingInfo.putEnabledEvent(e, loc);
      }
    }
    return enablingInfo;
  }
  
  private void removeNoise(double threshold) {
    
    HashSet<DNode> noiseNodes = new HashSet<DNode>();
    
    for (DNode e : bp.allEvents) {
      int maxPreOcc = 1;
      int minPreOcc = Integer.MAX_VALUE;
      for (DNode b : e.pre) {
        if (nodeOccurrences.get(b) > maxPreOcc)
          maxPreOcc = nodeOccurrences.get(b);
        if (nodeOccurrences.get(b) < minPreOcc)
          minPreOcc = nodeOccurrences.get(b);
      }
      
      //System.out.print((double)nodeOccurrences.get(e) / maxPreOcc+" ");
      
      if (((double)nodeOccurrences.get(e) / minPreOcc) < threshold) {
        noiseNodes.add(e);
        for (DNode d : bp.getAllSuccessors(e)) {
          noiseNodes.add(d);
        }
      }
    }
    
    //System.out.println();
    
    Uma.out.println("removing "+noiseNodes.size()+"/"+(bp.allEvents.size()+bp.allConditions.size())+" nodes...");
    bp.removeAll(noiseNodes);
  }
  
  private void extendByTraces(Collection<String[]> traces, Map<String, String> e2t) {
    for (String[] trace : traces) {
      extendByTrace(trace, e2t); 
    }
  }
  
  private HashMap<DNode, Float> calculateCovering(LinkedList<String[]> traces) {
    HashMap<DNode, Integer> countMap = new HashMap<DNode, Integer>();

    for (String[] trace : traces) {
      DNode[] marking = getInitialMarkin_equiv();
      
      for (DNode b : marking) {
        for (DNode bPrime : build.futureEquivalence().get(build.equivalentNode().get(b))) {
          if (!countMap.containsKey(bPrime))
            countMap.put(bPrime, 0);
          countMap.put(bPrime, countMap.get(bPrime)+1);
        }
      }
      
      for (int i = 0; i<trace.length; i++) {
        HashSet<DNode> enabledEvents = getEnabledTransitions_equiv(marking);
        if (enabledEvents.isEmpty()) {
          System.out.println("no more enabled events at "+DNode.toString(marking));
          break;
        }

        // get one enabled event and fire it
        DNode fireEvent = null;
        for (DNode e : enabledEvents) {
          if (build.getSystem().nameToID.get(trace[i]) == e.id) {
            fireEvent = e; break;
          }
        }
        
        if (fireEvent == null) {
          System.out.println("action "+trace[i]+" not enabled at "+DNode.toString(marking));
          break;
        }
        
        marking = getSuccessorMarking_equiv(marking, fireEvent);
        for (DNode b : marking) {
          for (DNode bPrime : build.futureEquivalence().get(build.equivalentNode().get(b))) {
            if (!countMap.containsKey(bPrime))
              countMap.put(bPrime, 0);
            countMap.put(bPrime, countMap.get(bPrime)+1);
          }
        }
        for (DNode ePrime : build.futureEquivalence().get(build.equivalentNode().get(fireEvent))) {
          if (!countMap.containsKey(ePrime))
            countMap.put(ePrime, 0);
          countMap.put(ePrime, countMap.get(ePrime)+1);
        }
      }
    }
    
    int max = 0;
    for (Integer count : countMap.values()) {
      if (max < count) max = count;
    }
    
    HashMap<DNode, Float> coverMap = new HashMap<DNode, Float>();
    for (Entry<DNode, Integer> count : countMap.entrySet()) {
      coverMap.put(count.getKey(), (float)count.getValue()/(float)max);
    }
    
    return coverMap;
  }
  
  private DNodeSetElement getNonBlockedNodes(LinkedList<String[]> traces, float threshold) {
    build.futureEquivalence();
    
    HashMap<DNode, Float> covering = calculateCovering(traces);
    DNodeSetElement showNodes = new DNodeSetElement();
    for (DNode d : covering.keySet()) {
      if (d.isAnti && d.isHot) continue;
      if (covering.get(d) >= threshold) {
        showNodes.add(d);
      }
    }
    return showNodes;
  }
  
  /**
   * @return a representation of the initial marking of the system
   *         wrt. its {@link DNodeBP#foldingEquivalenceClass}es 
   */
  private DNode[] getInitialMarkin_equiv() {
    return bp.initialCut;
  }

  /**
   * @param marking
   * @return the transitions (wrt. {@link DNodeBP#foldingEquivalenceClass}es)
   *         that are enabled in the given marking
   */
  private HashSet<DNode> getEnabledTransitions_equiv(DNode[] marking) {

    // collect all events that are post-event of a condition represented
    // by the marking
    HashSet<DNode> postEvents = new HashSet<DNode>();
    // the set of all conditions that could be represented by 'marking'
    HashSet<DNode> possibleConditions = new HashSet<DNode>();
    for (DNode b : marking) {
      // retrieve all conditions equivalent to 'b'
      for (DNode bPrime : build.futureEquivalence().get(build.equivalentNode().get(b))) {
        possibleConditions.add(bPrime);
        if (bPrime.post == null) continue;
        for (DNode e : bPrime.post)
          postEvents.add(e);
      }
    }
    
    // now determine all enabled events: those, which have all their
    // pre-conditions represented by 'marking'
    HashSet<DNode> enabledEvents = new HashSet<DNode>();
    for (DNode e : postEvents) {
      boolean isEnabled = true;
      for (DNode b : e.pre) {
        // pre-condition 'b' of 'e' is not represented by 'marking'
        if (!possibleConditions.contains(b)) {
          isEnabled = false; break;
        }
      }
      
      if (isEnabled) // story the equivalent representative of 'e'
        enabledEvents.add(build.equivalentNode().get(e));
    }
    return enabledEvents;
  }
  
  /**
   * Fire 'transition' at the given 'marking' (wrt. {@link DNodeBP#foldingEquivalenceClass}es).
   * 
   * @param marking
   * @param transition
   * @return the successor marking
   */
  private DNode[] getSuccessorMarking_equiv(DNode[] marking, DNode transition) {
    // determine the tokens that will be consumed by the transition
    boolean consume[] = new boolean[marking.length];
    for (DNode p : transition.pre) {
      for (int i=0;i<marking.length;i++) {
        if (consume[i]) continue;
        // token i is consumed if the corresponding condition marking[i]
        // is equivalent to the pre-place 'p' of 'transition'
        if (build.futureEquivalence().areFoldingEquivalent(p, marking[i])) {
          consume[i] = true;
          break;
        }
      }
    }
    
    // compute successor marking
    DNode[] successorMarking = new DNode[marking.length - transition.pre.length + transition.post.length];
    // keep all previous tokens except the consumed ones
    int succ_i = 0;
    for (int i = 0; i < marking.length; i++) {
      if (consume[i]) continue; // consumed: do not copy to successor marking
      successorMarking[succ_i++] = marking[i];
    }
    // now put a token on each post-place of 'transition'
    for (DNode p : transition.post) {
      // token on 'p' represented by the canonical representative of 'p' 
      successorMarking[succ_i++] = build.equivalentNode().get(p);
    }
    return successorMarking;
  }

  
  private static String toString(String arr[]) {
    if (arr == null) return "null";
    
    String result = "[";
    for (int i=0;i<arr.length;i++) {
      if (i > 0) result+=", ";
      result += arr[i];
    }
    return result+"]";
  }
  
  /**
   * Read a set of traces from a file in ProM's simple log format.
   * 
   * @param fileName
   * @return
   * @throws FileNotFoundException
   */
  private static LinkedList<String> readEvents(String fileName) throws FileNotFoundException
  {
    File fFile = new File(fileName);  
    Scanner scanner = new Scanner(fFile);
    scanner.useDelimiter(" ");
    
    LinkedList<String> events = new LinkedList<String>();
    try {
      // get each line and parse the line to extract the case information
      while ( scanner.hasNext() ){
        events.add(scanner.next());
      }
    }
    finally {
      //ensure the underlying stream is always closed
      scanner.close();
    }
    
    return events;
  }
  
  /**
   * Read a set of traces from a file in ProM's simple log format.
   * 
   * @param fileName
   * @return
   * @throws FileNotFoundException
   */
  private static LinkedList<String[]> readTraces(String fileName) throws FileNotFoundException
  {
    File fFile = new File(fileName);  
    Scanner scanner = new Scanner(fFile);
    LinkedList<String[]> cases = new LinkedList<String[]>();
    try {
      // get each line and parse the line to extract the case information
      while ( scanner.hasNextLine() ){
        cases.addLast(
            processCaseLine( scanner.nextLine() )
        );
      }
    }
    finally {
      //ensure the underlying stream is always closed
      scanner.close();
    }
    
    return cases;
  }
  
  /** 
   * @param caseLine a string representing one case in the format
   *        NUMx caseID activity1 activity2 ... activityN
   * @return the sequence of activities represented by the 'aLine'
   */
   private static String[] processCaseLine(String caseLine){
     //use a second Scanner to parse the content of each line 
     Scanner scanner = new Scanner(caseLine);
     scanner.useDelimiter(" ");
     
     // NUMx
     String caseCount = scanner.next();
     // caseID
     String caseName = scanner.next();
     // activity1 activity2 ... activityN
     LinkedList<String> activities = new LinkedList<String>();
     while ( scanner.hasNext() ){
       activities.add(scanner.next());
     }
     //(no need for finally here, since String is source)
     scanner.close();
     
     return activities.toArray(new String[activities.size()]);
   }
   
   public Map<String, List<Set<Place>>> getLocationsForExtension() {
     
     if (!(this.build.getSystem() instanceof DNodeSys_PetriNet)) return null;
     
     DNodeSys_PetriNet sysModel = (DNodeSys_PetriNet)this.build.getSystem();
     
     Map<String, List<Set<Place>>> result = new HashMap<String, List<Set<Place>>>();
     
     for (String missing : addAt.keySet()) {
       List<Set<Place>> jointIDs = new ArrayList<Set<Place>>();
       System.out.println("add "+missing+" at:");
       
       for (List<DNode> mark : addAt.get(missing)) {
         System.out.println("  "+mark);
         
         Set<Place> thisIDs = new HashSet<Place>();
         for (DNode d : mark) {
           for (DNode org : this.conditionOrigin.get(d)) {
             System.out.println(d+" -> "+org+" -> "+sysModel.getOriginalNode(org));
             thisIDs.add((Place)sysModel.getOriginalNode(org));
           }
         }
         
         int bestMatchIndex = -1;
         int bestMatchSize = 0;
         for (int i=0; i<jointIDs.size(); i++) {
           int matchSize = 0;
           for (Place p : thisIDs) {
             if (jointIDs.get(i).contains(p)) matchSize++;
           }
           if (matchSize > bestMatchSize) {
             bestMatchSize = matchSize;
             bestMatchIndex = i;
           }
         }
         if (bestMatchIndex == -1) {
           jointIDs.add(thisIDs);
         } else {
           jointIDs.get(bestMatchIndex).retainAll(thisIDs);
         }
       }
       result.put(missing, jointIDs);
       System.out.println("joint place: "+jointIDs);
     }
     return result;
   }
   
   private void extendModelWithMoveOnLog(PetriNet net, Map<String, List<Set<Place>>> extensions) {
     for (String ext : extensions.keySet()) {
       for (Set<Place> loc : extensions.get(ext)) {
         Transition t = net.addTransition(ext);
         for (Place p : loc) {
           net.addArc(p, t);
           net.addArc(t, p);
         }
       }
     }
   }
   
   private void extendModelWithMoveOnModel(PetriNet net) {
     if (!(this.build.getSystem() instanceof DNodeSys_PetriNet)) return;
     
     DNodeSys_PetriNet sysModel = (DNodeSys_PetriNet)this.build.getSystem();
     
     Set<Transition> need_tau = new HashSet<Transition>();
     for (DNode e : bp.getAllEvents()) {
       if (e.isImplied) {
         DNode sys_d = sysModel.getTransitionForID(e.causedBy[0]);
         need_tau.add((Transition)sysModel.getOriginalNode(sys_d));
       }
     }
     
     for (Transition t : need_tau) {
       Transition t_tau = net.addTransition("SILENT "+t.getName());
       for (Place p : t.getPreSet()) {
         net.addArc(p, t_tau);
       }
       for (Place p : t.getPostSet()) {
         net.addArc(t_tau, p);
       }
       System.out.println("tau for "+t);
     }
   }
   
   public static DNodeRefold getInitialUnfolding(ISystemModel sysModel) throws InvalidModelException {
     if (sysModel instanceof PetriNet) {
       ((PetriNet) sysModel).turnIntoLabeledNet();
       ((PetriNet) sysModel).makeNormalNet();
     }
     
     DNodeSys sys = Uma.getBehavioralSystemModel(sysModel);
     return Uma.initBuildPrefix_View(sys, 0);
   }

   public void repairModel(String systemFile, PetriNet sysModel, Collection<String[]> traces, Map<String, String> e2t) throws InvalidModelException, IOException {

     for (DNode b : build.getBranchingProcess().allConditions) {
       conditionOrigin.put(b, new DNode[] { b });
     }
     
     extendByTraces(traces, e2t);
     
     OcletIO_Out.writeFile(build.toDot(), systemFile+".unfolding.dot");
     extendModelWithMoveOnModel((PetriNet)sysModel);
     extendModelWithMoveOnLog((PetriNet)sysModel, getLocationsForExtension());

     
     OcletIO_Out.writeToFile((PetriNet)sysModel, systemFile+"_repaired.lola", OcletIO_Out.FORMAT_LOLA, 0);
     OcletIO_Out.writeToFile((PetriNet)sysModel, systemFile+"_repaired.dot", OcletIO_Out.FORMAT_DOT, 0);
   }
   
   public static void main(String[] args) {
     
       //String systemFile = "./examples/model_correction2/a22f0n00.lola";
       //String traceFile  = "./examples/model_correction2/a22f0n00.log_20_br.txt";
     
       //String systemFile = "./examples/model_correction/a22f0n00.lola";
       //String traceFile  = "./examples/model_correction/a22f0n05.log_100.txt";
       
       //String systemFile = "./examples/model_correction/a12f0n05_alpha.lola";
       //String traceFile  = "./examples/model_correction/a12f0n05_aligned.log.txt";
       
       String systemFile = "./examples/model_correction2/wabo1.lola";
       String traceFile  = "./examples/model_correction2/wabo1.log.txt";
       
       try {
         
         ISystemModel sysModel = Uma.readSystemFromFile(systemFile);
         LinkedList<String[]> traces = readTraces(traceFile);

         DNodeRefold build = getInitialUnfolding(sysModel);
         ModelRepair_Simple viewGen = new ModelRepair_Simple(build);
         viewGen.repairModel(systemFile, (PetriNet)sysModel, traces, null);
         
       } catch (IOException e) {
         System.err.println(e);
       } catch (InvalidModelException e) {
         System.err.println(e);
       }
     
   }
}
