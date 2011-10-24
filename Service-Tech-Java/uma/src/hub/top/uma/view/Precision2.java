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

import hub.top.petrinet.Arc;
import hub.top.petrinet.PetriNet;
import hub.top.petrinet.PetriNetIO;
import hub.top.petrinet.Place;
import hub.top.petrinet.Transition;
import hub.top.uma.DNode;
import hub.top.uma.DNodeBP;
import hub.top.uma.DNodeCutGenerator;
import hub.top.uma.DNodeSet;

import java.io.File;
import java.io.FileNotFoundException;
import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedHashSet;
import java.util.LinkedList;
import java.util.Scanner;
import java.util.Set;

public class Precision2 {

  private PetriNet net;

  private class State {
    LinkedList<Place> marking = new LinkedList<Place>();
    ArrayList<Transition> action = new ArrayList<Transition>();
    ArrayList<State> succ = new ArrayList<State>();
    int seen = 1;
  }
  
  private State initialState;
  
  /**
   * Standard constructor for view generation. The argument takes a branching
   * process construction object {@link DNodeBP} after the branching process has
   * been constructed.
   *  
   * @param build
   */
  public Precision2 (PetriNet net) {
    this.net = net;
    initialState = new State();
    for (Place p : net.getPlaces()) {
      for (int i=0;i<p.getTokens();i++) initialState.marking.add(p);
    }
  }
  

  public boolean extendByTrace(String[] trace) {
    State s = initialState;
    
    for (int j=0;j<trace.length;j++) {
      
      LinkedList<Transition> ts = new LinkedList<Transition>();
      for (Transition t : net.getTransitions()) {
        if (t.getName().equals(trace[j])) {
          
          boolean preSetMarked = true;
          for (Place p : t.getPreSet()) {
            if (!s.marking.contains(p)) {
              preSetMarked = false;
              System.out.println(t+" is missing token on "+p);
              break;
            }
          }
          
          if (preSetMarked)
            ts.add(t);
        }
      }

      if (ts.isEmpty()) {
        System.out.println("P2: could not fire "+trace[j]+" at "+j+" in "+s.marking);
        System.out.println(toString(trace));
        return false;
      }
      
      Transition to_fire = ts.get(0);
      
      boolean found = false;
      for (int i=0; i<s.action.size(); i++) {
        if (s.action.get(i) == to_fire) {
          s = s.succ.get(i);
          s.seen++;
          found = true;
        }
      }
      if (found) continue;

      State s2 = new State();
      s2.marking = new LinkedList<Place>();
      s2.marking.addAll(s.marking);
      
      for (Place p : to_fire.getPreSet()) {
        s2.marking.remove(p);
      }
      for (Place p : to_fire.getPostSet()) {
        s2.marking.add(p);
      }
      
      s.action.add(to_fire);
      s.succ.add(s2);
      s = s2;
    }
    
    numCases++;
    totalCaseLength += trace.length;
    
    return true;
  }
  
  public LinkedList<Transition> getAllEnabledTransitions(State s) {

    LinkedList<Transition> enabled = new LinkedList<Transition>();
    for (Transition t : net.getTransitions()) {
      if (s.marking.containsAll(t.getPreSet()))
        enabled.add(t);
    }
    return enabled;
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
  
  private float etc_conformance_global;
  private float etc_conformance_trace;
  
  private float unused_alternatives;
  private float averageDeviation;
  
  private int numCases = 0;
  private int totalCaseLength = 0;
  
  public String error = null;
  
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
}
