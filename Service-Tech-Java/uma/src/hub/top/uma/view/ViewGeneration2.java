/*****************************************************************************\
 * Copyright (c) 2008, 2009, 2010. Dirk Fahland. AGPL3.0
 * All rights reserved. 
 * 
 * ServiceTechnology.org - Uma, an Unfolding-based Model Analyzer
 * 
 * This program and the accompanying materials are made available under
 * the terms of the GNU Affero General Public License Version 3 or later,
 * which accompanies this distribution, and is available at 
 * http://www.gnu.org/licenses/agpl.txt
 * 
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 * 
\*****************************************************************************/

package hub.top.uma.view;

import hub.top.petrinet.ISystemModel;
import hub.top.petrinet.PetriNet;
import hub.top.petrinet.PetriNetIO;
import hub.top.petrinet.Place;
import hub.top.petrinet.Transition;
import hub.top.uma.DNode;
import hub.top.uma.DNodeBP;
import hub.top.uma.DNodeRefold;
import hub.top.uma.DNodeSet;
import hub.top.uma.DNodeBP.EnablingInfo;
import hub.top.uma.DNodeSet.DNodeSetElement;
import hub.top.uma.DNodeSys;
import hub.top.uma.InvalidModelException;
import hub.top.uma.Uma;
import hub.top.uma.synthesis.NetSynthesis;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.io.Writer;
import java.util.Arrays;
import java.util.Collections;
import java.util.Comparator;

import java.util.ArrayList;
import java.util.Collection;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Random;
import java.util.Scanner;
import java.util.SortedSet;
import java.util.Map.Entry;
import java.util.Set;
import java.util.TreeSet;

import com.google.gwt.dev.util.collect.HashMap;
import com.google.gwt.dev.util.collect.HashSet;

public class ViewGeneration2 {
  
  private DNodeRefold   build;
  private DNodeSet  bp;
  
  public Map<DNode, Integer> eventOccurrences;
  public Map<HashSet<DNode>, LinkedList<String[]>> equivalentTraces;
  
  public class State {
    LinkedList<DNode> marking = new LinkedList<DNode>();
    ArrayList<Short> action = new ArrayList<Short>();
    ArrayList<State> succ = new ArrayList<State>();
    int seen = 0;
    
    public List<List<DNode>> getAllVisitedMarkings() {
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
  public State initialState;
  
  /**
   * Standard constructor for view generation. The argument takes a branching
   * process construction object {@link DNodeBP} after the branching process has
   * been constructed.
   *  
   * @param build
   */
  public ViewGeneration2 (DNodeRefold build) {
    this.build = build;
    this.bp = build.getBranchingProcess();
    
    eventOccurrences = new HashMap<DNode, Integer>();
    equivalentTraces = new HashMap<HashSet<DNode>, LinkedList<String[]>>();
    initialState = new State();

    for (DNode b : bp.initialCut) {
      initialState.marking.add(b);
    }
    //Collections.sort(initialState.marking, dnode_id_comparator);
  }
  
  private Comparator<DNode> dnode_id_comparator = new Comparator<DNode>() {

    @Override
    public int compare(DNode o1, DNode o2) {
      if (o1.globalId < o2.globalId) return -1;
      if (o1.globalId == o2.globalId) return 0;
      return 1;
    }
  };
  
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
  public boolean extendByTrace(String[] trace, Map<String, String> e2t) {
    Set<DNode> run = new HashSet<DNode>();
    
    for (DNode b : bp.initialCut) {
      run.add(b);
    }

    State state = initialState;
    state.seen++;
    
    for (int i = 0; i<trace.length; i++) {
      
      String eventName;
      if (e2t == null || !e2t.containsKey(trace[i])) eventName = trace[i];
      else eventName = e2t.get(trace[i]);
      
      SortedSet<DNode> existingEvents = new TreeSet<DNode>(dnode_id_comparator);
      for (DNode b : state.marking) {
        if (b.post == null ) continue;
        for (DNode e : b.post) {
          
          // only look for events that have the same name as the next action in the trace
          if (!build.getSystem().properNames[e.id].equals(eventName)) continue;
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
      if (!existingEvents.isEmpty()) {
        
        // get one enabled event and fire it
        fireEvent = existingEvents.iterator().next();
        for (int s = 0; s < state.action.size(); s++) {
          if (state.action.get(s) == fireEvent.id) {
            succState = state.succ.get(s);
            break;
          }
        }
        
      }
        
      if (succState == null) {
        
        if (fireEvent == null) {
        
          HashSet<DNode> matchingEvents = new HashSet<DNode>();
          for (DNode e : build.getSystem().fireableEvents) {
            if (build.getSystem().properNames[e.id].equals(eventName))
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
            
            DNode[] postConditions = bp.fire(events, loc);
            if (postConditions != null && postConditions.length > 0) {
              fireEvent = postConditions[0].pre[0];            
  
              // update co-relation for all new post-conditions
              //build.updateConcurrencyRelation(postConditions);
              
              // and set fields for the event
              //build.setCurrentPrimeConfig(newEvent, true);
  
            } else {
              System.out.println("fired event with empty post-set");
            }
          } else {
            
            System.out.println("could not fire "+eventName+" ("+trace[i]+")");
            return false;
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
          List<DNode> produced = new LinkedList<DNode>();
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
      
      if (!eventOccurrences.containsKey(fireEvent))
        eventOccurrences.put(fireEvent, 0);
      eventOccurrences.put(fireEvent, eventOccurrences.get(fireEvent)+1);
      
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
          if (bCut.endsWith(e.pre[pre_index])) {
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
  
  public double computePrecision_bp() {
    
    int escapedEdgesSum = 0;
    int escapedEdgesSum_Weighted = 0;
    
    int enabledEventsSum = 0;
    int enabledEventsSum_Weighted = 0;
    
    float unused_alt = 0;
    
    LinkedList<State> toVisit = new LinkedList<State>();
    toVisit.add(initialState);
    
    int totalStates = 0;
    while (!toVisit.isEmpty()) {
      
      State s = toVisit.removeFirst();
      
      Set<DNode> succEvents = new HashSet<DNode>();
      
      for (DNode b : s.marking) {
        if (b.post == null) continue;
        for (DNode e : b.post) {
          boolean allInS = true;
          for (DNode bPre : e.pre) {
            if (!s.marking.contains(bPre)) {
              allInS = false;
              break;
            }
          }
          if (allInS) {
            succEvents.add(e);
          }
        }
      }

      int enabledEvents = succEvents.size();
      int firedEvents = s.succ.size();
      
      enabledEventsSum += enabledEvents;
      enabledEventsSum_Weighted += enabledEvents * s.seen;
      
      escapedEdgesSum += (enabledEvents - firedEvents);
      escapedEdgesSum_Weighted += (enabledEvents - firedEvents) * s.seen;
      
      if (firedEvents == 0 && enabledEvents == 0) {
      } else {
        unused_alt += (float)firedEvents/(float)enabledEvents;
      }
      
      toVisit.addAll(s.succ);
      totalStates++;
    }
    
    double etc_conformance_trace = 1.0f - ((double)escapedEdgesSum_Weighted / (double)enabledEventsSum_Weighted);
    return etc_conformance_trace;
  }
  
  public LinkedList<String[]>[] accumulateEquivalentTraces(int maxClasses) {
    int totalTraces = 0;
    for (LinkedList<String[]> traces : equivalentTraces.values())
      totalTraces += traces.size();
    
    float accumulationFactor = (float)totalTraces/(float)maxClasses;
    
    LinkedList<String[]> accumulatedTraces[] = new LinkedList[maxClasses];
    for (int i=0; i< accumulatedTraces.length; i++)
      accumulatedTraces[i] = new LinkedList<String[]>();
    
    int count = 0;
    for (LinkedList<String[]> traces : equivalentTraces.values()) {
      int index = (int)((float)count/accumulationFactor);
      if (index == maxClasses) index = maxClasses-1;
      accumulatedTraces[index].addAll(traces);
      count += traces.size();
    }
    
    return accumulatedTraces;
  }
  
  public void extendByTraces(LinkedList<String[]> traces) {
    for (String[] trace : traces) {
      extendByTrace(trace, null); 
    }
  }
  
  /**
   * Recompute all cut-off events of the branching process and update the
   * corresponding equivalence relation. This method needs to be called whenever
   * an existing branching process is extended, for instance by
   * 
   */
  public void identifyFoldingRelation() {
    for (DNode e : bp.allEvents) {
      // won't be considered in the following
      if (e.isAnti && e.isHot) continue;  
      
      build.setCurrentPrimeConfig(e, false);
      if (build.isCutOffEvent(e)) {
        // System.out.println(e+" is a cut-off event");
        e.isCutOff = true;
        if (e.post != null)
          for (DNode b : e.post) {
            b.isCutOff = true;
          }
      }
    }
  }
  
  public PetriNet foldView() {
    identifyFoldingRelation();
    PetriNet net = NetSynthesis.foldToNet_labeled(build, true);
    return net;
  }
  
  public HashMap<DNode, Float> calculateCovering(LinkedList<String[]> traces) {
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
  
  public DNodeSetElement getNonBlockedNodes(LinkedList<String[]> traces, float threshold) {
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
  
  public PetriNet generateViewBlocked(LinkedList<String[]> traces, float threshold) {
    DNodeSetElement showNodes = getNonBlockedNodes(traces, threshold);
    return NetSynthesis.foldToNet_labeled(build, showNodes, true);
  }
  
  /**
   * @return a representation of the initial marking of the system
   *         wrt. its {@link DNodeBP#foldingEquivalenceClass}es 
   */
  public DNode[] getInitialMarkin_equiv() {
    return bp.initialCut;
  }

  /**
   * @param marking
   * @return the transitions (wrt. {@link DNodeBP#foldingEquivalenceClass}es)
   *         that are enabled in the given marking
   */
  public HashSet<DNode> getEnabledTransitions_equiv(DNode[] marking) {

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
  public DNode[] getSuccessorMarking_equiv(DNode[] marking, DNode transition) {
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

  /**
   * Generate a trace from the constructed branching process that can be
   * executed in the system.
   * 
   * @param upperBound   maximum length of the trace
   * @return a trace of the system
   */
  public LinkedList<String> generateRandomTrace2(int upperBound) {
    LinkedList<String> trace = new LinkedList<String>();
    DNode[] marking = getInitialMarkin_equiv();
    Random r = new Random();
    
    for (int i = 0; i<upperBound; i++) {
      HashSet<DNode> enabledEvents = getEnabledTransitions_equiv(marking);
      if (enabledEvents.isEmpty()) {
        System.out.println("no more enabled events at "+trace);
        break;
      }
      
      DNode[] toChoose = new DNode[enabledEvents.size()];
      enabledEvents.toArray(toChoose);
      int chosenEvent = r.nextInt(toChoose.length);
      
      // get one enabled event and fire it
      DNode fireEvent = toChoose[chosenEvent];
      marking = getSuccessorMarking_equiv(marking, fireEvent);
      trace.addLast(build.getSystem().properNames[fireEvent.id]);
    }
    return trace;
  }

  /**
   * Generate N random traces of at most length L for this system.
   * 
   * @param traceNum number of traces N to generate
   * @param length L of traces 
   * @return list of generated traces
   */
  public LinkedList<String[]> generateRandomTraces(int traceNum, int length) {
    LinkedList<String[]> traces = new LinkedList<String[]>();
    for (int i=0; i<traceNum; i++) {
      LinkedList<String> trace = generateRandomTrace2(length);
      traces.addLast(trace.toArray(new String[trace.size()]));
    }
    return traces;
  }
  
  public static String toString(String arr[]) {
    if (arr == null) return "null";
    
    String result = "[";
    for (int i=0;i<arr.length;i++) {
      if (i > 0) result+=", ";
      result += arr[i];
    }
    return result+"]";
  }
  
  /**
   * @param traces
   * @return a representation of the traces in ProM's simple log format
   */
  public static StringBuilder generateSimpleLog(LinkedList<String[]> traces) {
    StringBuilder simpleLog = new StringBuilder();
    
    int traceNum = 0;
    for (String[] trace : traces) {
      
      simpleLog.append("1x case"+traceNum);
      for (String activity : trace) {
        simpleLog.append(" "+activity.replace(' ', '_'));
      }
      simpleLog.append("\n");
      traceNum++;
    }
    return simpleLog;
  }
  
  /**
   * Write a set of traces ProM's simple log format to the given file.
   * 
   * @param fileName
   * @param traces
   * @throws IOException
   */
  public static void writeTraces(String fileName, LinkedList<String[]> traces) throws IOException {
    Writer out = new OutputStreamWriter(new FileOutputStream(fileName));
    try {
      out.append(generateSimpleLog(traces));
    }
    finally {
      out.close();
    }
  }
  
  /**
   * Read a set of traces from a file in ProM's simple log format.
   * 
   * @param fileName
   * @return
   * @throws FileNotFoundException
   */
  public static LinkedList<String> readEvents(String fileName) throws FileNotFoundException
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
  public static LinkedList<String[]> readTraces(String fileName) throws FileNotFoundException
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
   protected static String[] processCaseLine(String caseLine){
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
   
   public static PetriNet generateView(DNodeSys sys, int bound, LinkedList<String[]> traces) {
     DNodeBP_View build = new DNodeBP_View(sys, bound);
     
     build.extendByTraces(traces);
     ViewGeneration2 viewGen = new ViewGeneration2(build);
     PetriNet net = viewGen.foldView();
     
     return net;
   }
   
   public static PetriNet generateView(String systemFileName, int bound, String traceFileName) throws IOException, InvalidModelException {
     ISystemModel sysModel = Uma.readSystemFromFile(systemFileName);
     DNodeSys sys = Uma.getBehavioralSystemModel(sysModel);
     LinkedList<String[]> traces = ViewGeneration2.readTraces(traceFileName);
     return generateView(sys, bound, traces);
   }
   
   public static void main(String[] args) {
     
     if (args.length == 0) return;
     
     if ("-gen".equals(args[0])) {
       String fromFile = args[1];
       int numTraces = Integer.parseInt(args[2]);
       int traceLength = Integer.parseInt(args[3]);
       String toFile = args[4];
       
       try {
         
         ISystemModel sysModel = Uma.readSystemFromFile(fromFile);
         DNodeSys sys = Uma.getBehavioralSystemModel(sysModel);

         DNodeRefold build = (DNodeRefold)Uma.buildPrefix(Uma.initBuildPrefix_View(sys, 3));
         
         build.futureEquivalence();
         
         ViewGeneration2 viewGen = new ViewGeneration2(build);
         LinkedList<String[]> traces = viewGen.generateRandomTraces(numTraces, traceLength);
         writeTraces(toFile, traces);
         
       } catch (IOException e) {
         System.err.println("Couldn't access file "+fromFile+"\n"+e);
       } catch (InvalidModelException e) {
         System.err.println(e);
       }
     }
     
     if ("-replay".equals(args[0])) {
       String systemFile = args[1];
       String traceFile = args[2];
       
       try {
         
         ISystemModel sysModel = Uma.readSystemFromFile(systemFile);
         DNodeSys sys = Uma.getBehavioralSystemModel(sysModel);

         DNodeRefold build = Uma.initBuildPrefix_View(sys, 3);
         
         LinkedList<String[]> traces = readTraces(traceFile);

         ViewGeneration2 viewGen = new ViewGeneration2(build);
         viewGen.extendByTraces(traces);
         
         FileWriter fstream = new FileWriter(systemFile+".traces.dot");
         BufferedWriter out = new BufferedWriter(fstream);
         out.write(build.toDot());
         out.close();
         
       } catch (IOException e) {
         System.err.println(e);
       } catch (InvalidModelException e) {
         System.err.println(e);
       }
     }
     
     if ("-view".equals(args[0])) {
       String systemFile = args[1];
       String traceFile = args[2];
       
       try {
         
         PetriNet net = generateView(systemFile, 3, traceFile);
         PetriNetIO.writeToFile(net, systemFile+".view", PetriNetIO.FORMAT_DOT, 0);
         
       } catch (IOException e) {
         System.err.println(e);
       } catch (InvalidModelException e) {
         System.err.println(e);
       }
     }
   }
}
