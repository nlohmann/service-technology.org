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

import hub.top.petrinet.Node;
import hub.top.petrinet.PetriNet;
import hub.top.petrinet.Place;
import hub.top.petrinet.Transition;
import hub.top.uma.Uma;

import java.util.ArrayList;
import java.util.Collection;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.SortedSet;
import java.util.TreeSet;

public class Precision_ETC {

  private PetriNet net;

  private class State {
    LinkedList<Place> marking = new LinkedList<Place>();
    ArrayList<Transition> action = new ArrayList<Transition>();
    ArrayList<State> succ = new ArrayList<State>();
    int seen = 0;
  }
  
  private State initialState;
  
  public Precision_ETC (PetriNet net) {
    this.net = net;
    initialState = new State();
    for (Place p : net.getPlaces()) {
      for (int i=0;i<p.getTokens();i++) initialState.marking.add(p);
    }
  }
  

  public Precision_ETC(PetriNet net, Collection<String[]> traces) {
    this(net);
    
    int nonReplayedCases = 0;
    for (String[] trace : traces) {
      if (!extendByTrace(trace)) nonReplayedCases++;
    }
    if (nonReplayedCases > 0) System.out.println("[Uma/Precision_ETC] Warning: could not replay "+nonReplayedCases+" cases.");
  }
  

  public boolean extendByTrace(String[] trace) {
    State s = initialState;
    
    List<State> matched_trace = new LinkedList<State>();
    if (extendByTrace(trace, 0, s, matched_trace)) {
      for (State s2 : matched_trace) {
        s2.seen++;
      }
      numCases++;
      totalCaseLength += trace.length;
      return true;
    } else {
      System.out.println("could not replay: "+toString(trace));
      return false;                   
    }
  }
  
  private static boolean matchingTransitionName(String transitionName, String eventName) {
    if (transitionName.equals(eventName)) return true;
    if (transitionName.indexOf(eventName) >= 0) return true;
    return false;
  }

  /**
   * Replay the trace using backtracking, i.e. try all enabled transitions in
   * the execution tree, branch, and backtrack to find a path that can replay
   * the trace.
   * 
   * @param trace
   * @param j current event in the trace
   * @param s state in which the event shall be fired
   * @param matched_trace filled when ascending the recursion tree, contains the
   *                      states which constitute a matching trace for replay
   * @return {@code true} iff the trace could be replayed
   */
  public boolean extendByTrace(String[] trace, int j, State s, List<State> matched_trace) {
    
    if (j == trace.length) {
      matched_trace.add(s);
      return true;
    }
    
    SortedSet<Transition> candidateTransitions = new TreeSet<Transition>(new Node.Comparator());
    for (Place p : s.marking) {
      for (Transition t : p.getPostSet()) {
        candidateTransitions.add(t);
      }
    }
    
    //for (Transition t : candidateTransitions) {
    //  System.out.println(t+" "+t.getPreSet());
    //}
    //System.out.println(s.marking);
    
    SortedSet<Transition> ts = new TreeSet<Transition>(new Node.Comparator());
    for (Transition t : candidateTransitions) {
      if (matchingTransitionName(t.getName(), trace[j])) {
        
        boolean preSetMarked = true;
        for (Place p : t.getPreSet()) {
          if (!s.marking.contains(p)) {
            preSetMarked = false;
            //System.out.println(t+" is missing token on "+p);
            break;
          }
        }
        
        if (preSetMarked)
          ts.add(t);
      }
    }

    if (ts.isEmpty()) {
      /*
      System.out.println("P2: could not fire "+trace[j]+" at "+j+" in "+s.marking);
      System.out.println(toString(trace));
      for (Transition t : candidateTransitions) {
        if (t.getName().equals(trace[j])) {
          
          for (Place p : t.getPreSet()) {
            if (!s.marking.contains(p)) {
              System.out.println(t+" is missing token on "+p);
            }
          }
        }
      }
      */
      return false;
    }
    
    for (Transition to_fire : ts) {
      //System.out.println("  fire "+to_fire );
      
      State s2 = null;
      
      for (int i=0; i<s.action.size(); i++) {
        if (s.action.get(i) == to_fire) {
          s2 = s.succ.get(i);
          break;
        }
      }
      if (s2 != null) {
        if (extendByTrace(trace, j+1, s2, matched_trace)) {
          matched_trace.add(s);
          return true;
        }
      } else {

        s2 = new State();
        s2.marking = new LinkedList<Place>();
        s2.marking.addAll(s.marking);
        
        for (Place p : to_fire.getPreSet()) {
          //System.out.println("  consume "+p);
          s2.marking.remove(p);
        }
        for (Place p : to_fire.getPostSet()) {
          //System.out.println("  produce "+p);
          s2.marking.add(p);
        }
        
        s.action.add(to_fire);
        s.succ.add(s2);
        
        if ( extendByTrace(trace, j+1, s2, matched_trace)) {
          matched_trace.add(s);
          return true;
        } else {
          // undo extension of execution tree
          s.action.remove(to_fire);
          s.succ.remove(s2);
        }
        //System.out.print(j+"/"+ts.size()+" ");
      }
    }
    return false;
  }
  
  private LinkedList<Transition> getAllEnabledTransitions(State s) {

    LinkedList<Transition> enabled = new LinkedList<Transition>();
    for (Transition t : net.getTransitions()) {
      if (s.marking.containsAll(t.getPreSet()))
        enabled.add(t);
    }
    return enabled;
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
  
  private float etc_conformance_global;
  private float etc_conformance_trace;
  
  private float unused_alternatives;
  private float averageDeviation;
  
  private int numCases = 0;
  private int totalCaseLength = 0;
  
  public void computePrecision() {
    
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

      int enabledEvents = getAllEnabledTransitions(s).size();
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
    
    etc_conformance_global = 1.0f - ((float)escapedEdgesSum / (float)enabledEventsSum);
    etc_conformance_trace = 1.0f - ((float)escapedEdgesSum_Weighted / (float)enabledEventsSum_Weighted);

    unused_alternatives = unused_alt / totalStates;
    int avg_case_length = totalCaseLength/numCases;
    averageDeviation = 1.0f;    
    for (int i=0; i<avg_case_length; i++) {
      averageDeviation *= unused_alternatives;
    }
    
  }
  
  public float getPrecisionGlobal() {
    return etc_conformance_global;
  }
  
  public float getPrecisionTrace() {
    return etc_conformance_trace;
  }

  public float getUnusedAlternatives() {
    return unused_alternatives;
  }
  
  public float getAverageDeviation() {
    return averageDeviation;
  }
  
  public static void main(String[] args) throws Exception {
    PetriNet net = (PetriNet)Uma.readSystemFromFile(args[0]);
    //net.turnIntoLabeledNet();
    List<String[]> log = ViewGeneration2.readTraces(args[1]);
    
    Precision_ETC prec = new Precision_ETC(net, log);
    prec.computePrecision();
    System.out.println(prec.getPrecisionTrace());
  }
}
