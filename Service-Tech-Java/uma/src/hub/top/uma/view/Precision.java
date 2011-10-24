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
import hub.top.uma.DNode;
import hub.top.uma.DNodeBP;
import hub.top.uma.DNodeCutGenerator;
import hub.top.uma.DNodeSet;
import hub.top.uma.DNodeSys;
import hub.top.uma.InvalidModelException;
import hub.top.uma.Uma;
import hub.top.uma.view.MineSimplify.Configuration;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileWriter;
import java.io.IOException;
import java.util.Collection;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedHashSet;
import java.util.LinkedList;
import java.util.Scanner;
import java.util.Set;

public class Precision {
  
  private DNodeBP   build;
  private DNodeSet  bp;
  
  private HashSet<HashSet<DNode>> reachedMarkings;
  private HashMap<HashSet<DNode>, Integer> visited;
  
  /**
   * Standard constructor for view generation. The argument takes a branching
   * process construction object {@link DNodeBP} after the branching process has
   * been constructed.
   *  
   * @param build
   */
  public Precision (DNodeBP build) {
    this.build = build;
    this.bp = build.getBranchingProcess();
    
    reachedMarkings = new HashSet<HashSet<DNode>>();
    visited = new HashMap<HashSet<DNode>, Integer>();
  }
  
  public Precision(PetriNet net) throws InvalidModelException {
    
    DNodeSys sys = Uma.getBehavioralSystemModel(net);
    DNodeBP build = Uma.initBuildPrefix_View(sys, 0);
    
    this.build = build;
    this.bp = build.getBranchingProcess();
    
    reachedMarkings = new HashSet<HashSet<DNode>>();
    visited = new HashMap<HashSet<DNode>, Integer>();
  }
  
  /**
   * Extend the branching process by the partially ordered run of the given
   * trace. The names in the trace must correspond to names of actions in the
   * system that generated the branching process. Also accumulate the
   * number of actions in {@link ViewGeneration#eventOccurrences}.
   *  
   * @param trace
   * @return <code>true</code> if the branching process could be extended by
   * the trace, and <code>false</code> if one action could not be fired
   */
  public boolean extendByTrace(String[] trace) {
    LinkedList<DNode> runCut = new LinkedList<DNode>();
    
    for (DNode b : bp.initialCut) {
      runCut.add(b);
    }
    HashSet<DNode> runCuts = new HashSet<DNode>(runCut);
    reachedMarkings.add(runCuts);
    if (!visited.containsKey(runCuts)) visited.put(runCuts, 0);
    visited.put(runCuts, visited.get(runCuts)+1);
    
    for (int i = 0; i<trace.length; i++) {

      LinkedHashSet<DNode> enabledEvents = new LinkedHashSet<DNode>();
      for (DNode b : runCut) {
        if (b.post == null ) continue;
        for (DNode e : b.post) {
          
          // only look for events that have the same name as the next action in the trace
          if (!build.getSystem().properNames[e.id].equals(trace[i])) continue;
          //System.out.print(" yes ");
          
          boolean isEnabled = true;
          for (DNode ePre : e.pre) {
            if (!runCut.contains(ePre)) { isEnabled = false; break; }
          }
          if (isEnabled) enabledEvents.add(e);
        }
      }
      
      if (!enabledEvents.isEmpty()) {
        
        // get one enabled event and fire it
        DNode fireEvent = enabledEvents.iterator().next();
        for (DNode b : fireEvent.pre) {
          runCut.remove(b);
        }
        if (fireEvent.post != null)
          for (DNode b : fireEvent.post) {
            runCut.addLast(b);
          }

      } else {
        
        LinkedList<DNode> fireableEvents = new LinkedList<DNode>();
        for (DNode e : build.getSystem().fireableEvents) {
          if (build.getSystem().properNames[e.id].equals(trace[i]))
            fireableEvents.add(e);
        }
        
        DNode[] loc = null;
        DNode[] events = null;
        for (DNode event : fireableEvents) {
          loc = getSmallestLocation(event, runCut);
          if (loc != null) {
            events = new DNode[] { event };
            break;
          }
        }
        
        if (loc != null)
        {
          DNode[] postConditions = bp.fire(events, loc);
          if (postConditions != null && postConditions.length > 0) {
            DNode newEvent = postConditions[0].pre[0];            

            for (DNode b : newEvent.pre) {
              runCut.remove(b);
            }
            for (DNode b : postConditions) {
              runCut.addLast(b);
            }
          } else {
            System.out.println("fired event with empty post-set");
          }
        } else {
          
          System.out.println("could not fire "+trace[i]);
          return false;
        }
      }
      
      HashSet<DNode> runCut_set = new HashSet<DNode>(runCut);
      reachedMarkings.add(runCut_set);
      if (!visited.containsKey(runCut_set)) {
        visited.put(runCut_set, 0);
        //System.out.println("new marking "+runCut_set);
      }
      visited.put(runCut_set, visited.get(runCut_set)+1);
      
    } // for all events in the trace
    
    numCases++;
    totalCaseLength += trace.length;
    
    return true;
  }
  
  private DNode[] getSmallestLocation(DNode event, LinkedList<DNode> runCut) {
    DNode[] loc = new DNode[event.pre.length];
    
    for (int i=0; i<loc.length; i++) {
      boolean found = false;
      for (int j=0;j<runCut.size();j++) {
        if (event.pre[i].id == runCut.get(j).id) {
          loc[i] = runCut.get(j);
          found = true;
          break;
        }
      }
      if (!found) return null;
    }
    return loc;
  }
  
  public DNodeBP.EnablingInfo getAllEnabledEvents(Collection<DNode> fireableEvents, Collection<DNode> runCut) {

    DNodeBP.EnablingInfo enablingInfo = new DNodeBP.EnablingInfo();
    
    HashMap<DNode, Set<DNode>> co = new HashMap<DNode, Set<DNode>>();
    for (DNode b : runCut) {
      HashSet<DNode> coOther = new HashSet<DNode>(runCut);
      coOther.remove(b);
      co.put(b, coOther);
    }
    
    for (DNode e : fireableEvents) {
      
      LinkedList<DNode> candidates[] = new LinkedList[e.pre.length];
      for (int i=0; i<candidates.length;i++) {
        candidates[i] = new LinkedList<DNode>();
        for (DNode b : runCut) {
          if (b.id == e.pre[i].id) candidates[i].add(b);
        }
      }
      
      LinkedList<DNode[]> cuts = DNodeCutGenerator.generateCuts(candidates, co);
      for (DNode[] loc : cuts) {
        enablingInfo.putEnabledEvent(e, loc);
      }
    }
    return enablingInfo;
  }
  
  public DNodeBP.EnablingInfo getAllEnabledTransitions(Collection<DNode> fireableEvents, Collection<DNode> runCut) {

    DNodeBP.EnablingInfo enablingInfo = new DNodeBP.EnablingInfo();
    
    for (DNode e : fireableEvents) {
      boolean isEnabled = true;
      DNode loc[] = new DNode[e.pre.length];
      for (int pre_index = 0; pre_index < e.pre.length; pre_index++) {
        boolean endsWith_b = false;
        for (DNode bCut : runCut) {
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
  
  public void extendByTraces(LinkedList<String[]> traces) {
    for (String[] trace : traces) {
      extendByTrace(trace); 
    }
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
  private float etc_conformance_global_transition;
  private float etc_conformance_trace;
  private float etc_conformance_trace_transition;
  
  private float unused_alternatives;
  private float averageDeviation;
  
  private int numCases = 0;
  private int totalCaseLength = 0;
  
  public String error = null;
  
  public void computePrecision() {
    
    int escapedEdgesSum = 0;
    int escapedEdgesSum_Weighted = 0;
    
    int escapedEdgesSum2 = 0;
    int escapedEdgesSum2_Weighted = 0;

    int enabledEventsSum = 0;
    int enabledEventsSum_Weighted = 0;
    
    float unused_alt = 0;
    
    for (HashSet<DNode> cut : reachedMarkings) {
      //DNodeBP.EnablingInfo enabled = getAllEnabledEvents(build.getSystem().fireableEvents, cut);
      DNodeBP.EnablingInfo enabled = getAllEnabledTransitions(build.getSystem().fireableEvents, cut);

      int enabledEvents = 0;
      for (Short e_id : enabled.locations.keySet()) {
        
        HashSet<HashSet<DNode>> locs = new HashSet<HashSet<DNode>>();        
        for (int i=0; i<enabled.locations.get(e_id).length; i++) {
          if (enabled.locations.get(e_id)[i] == null) continue;
          
          HashSet<DNode> loc = new HashSet<DNode>();
          for (DNode b : enabled.locations.get(e_id)[i].loc) {
            loc.add(b);
          }
          locs.add(loc);          
        }
        enabledEvents += locs.size();
        
        //enabledEvents += 1;
      }

      
      HashSet<DNode> postEvents = new HashSet<DNode>();
      for (DNode b : cut) {
        if (b.post == null) continue;
        for (DNode e : b.post) {
          
          boolean complete = true;
          for (DNode c : e.pre) {
            if (!cut.contains(c)) {
              complete = false;
              break;
            }
          }
          if (complete) postEvents.add(e);
        }
      }
      HashSet<Short> postTransitions = new HashSet<Short>();
      for (DNode e : postEvents)
        postTransitions.add(e.id);
      
      int firedEvents = postEvents.size();
      int firedTransitions = postTransitions.size();
      
      enabledEventsSum += enabledEvents;
      enabledEventsSum_Weighted += enabledEvents * visited.get(cut);
      
      escapedEdgesSum += (enabledEvents - firedEvents);
      escapedEdgesSum_Weighted += (enabledEvents - firedEvents) * visited.get(cut);

      escapedEdgesSum2 += (enabledEvents - firedTransitions);
      escapedEdgesSum2_Weighted += (enabledEvents - firedTransitions) * visited.get(cut);

      if (firedEvents == 0 && enabledEvents == 0) {
      } else {
        unused_alt += (float)firedEvents/(float)enabledEvents;
      }
    }
    
    etc_conformance_global = 1.0f - ((float)escapedEdgesSum / (float)enabledEventsSum);
    etc_conformance_trace = 1.0f - ((float)escapedEdgesSum_Weighted / (float)enabledEventsSum_Weighted);
    etc_conformance_global_transition = 1.0f - ((float)escapedEdgesSum2 / (float)enabledEventsSum);
    etc_conformance_trace_transition = 1.0f - ((float)escapedEdgesSum2_Weighted / (float)enabledEventsSum_Weighted);

    unused_alternatives = unused_alt / reachedMarkings.size();
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
  
  public float getPrecisionGlobalTrans() {
    return etc_conformance_global_transition;
  }
  
  public float getPrecisionTraceTrans() {
    return etc_conformance_trace_transition;
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
   
   public static String measure(String system, String log, boolean simplified) throws InvalidModelException, IOException {

     if (simplified) system += ".simplified.lola";
     
     System.out.println(system);
     
     try {
       ISystemModel         sysModel = Uma.readSystemFromFile(system);
       LinkedList<String[]> allTraces = ViewGeneration2.readTraces(log);
       PetriNet net = (PetriNet)sysModel;
       net.makeNormalNet();
       net.turnIntoLabeledNet();
       
       Precision precision = new Precision((PetriNet)net);
       Precision2 precision2 = new Precision2((PetriNet)net);
       
       boolean failed1 = false;
       boolean failed2 = false;
       for (String[] trace : allTraces) {
         if (!precision.extendByTrace(trace)) failed1 = true;
         if (!precision2.extendByTrace(trace)) failed2 = true;
       }
       
       precision.computePrecision();
       precision2.computePrecision();
       
       return system+";"+precision.getPrecisionGlobalTrans()+";"+precision.getUnusedAlternatives()+";"+precision2.getPrecisionGlobal()+";"+precision2.getPrecisionTrace()+";"+failed1+";"+failed2+"\n";
     } catch (Exception e) {
       return system+";-\n";
     }
   }

   
   public static String runExperiment(String path, boolean simplified) throws IOException, InvalidModelException {
     //DNodeBP.ignoreFoldThreshold = false;
     
     StringBuilder sb = new StringBuilder();
     
     sb.append(measure(path+"/a12f0n00.lola", path+"/a12f0n00.log.txt", simplified));
     sb.append(measure(path+"/a12f0n05.lola", path+"/a12f0n05.log.txt", simplified));
     sb.append(measure(path+"/a12f0n10.lola", path+"/a12f0n10.log.txt", simplified));
     sb.append(measure(path+"/a12f0n20.lola", path+"/a12f0n20.log.txt", simplified));
     sb.append(measure(path+"/a12f0n50.lola", path+"/a12f0n50.log.txt", simplified));
     
     sb.append(measure(path+"/a22f0n00.lola", path+"/a22f0n00.log.txt", simplified));
     sb.append(measure(path+"/a22f0n05.lola", path+"/a22f0n05.log.txt", simplified));
     sb.append(measure(path+"/a22f0n10.lola", path+"/a22f0n10.log.txt", simplified));
     sb.append(measure(path+"/a22f0n20.lola", path+"/a22f0n20.log.txt", simplified));
     sb.append(measure(path+"/a22f0n50.lola", path+"/a22f0n50.log.txt", simplified));
     
     sb.append(measure(path+"/a32f0n00.lola", path+"/a32f0n00.log.txt", simplified));
     sb.append(measure(path+"/a32f0n05.lola", path+"/a32f0n05.log.txt", simplified));
     sb.append(measure(path+"/a32f0n10.lola", path+"/a32f0n10.log.txt", simplified)); 
     sb.append(measure(path+"/a32f0n20.lola", path+"/a32f0n20.log.txt", simplified)); 
     sb.append(measure(path+"/a32f0n50.lola", path+"/a32f0n50.log.txt", simplified));
     
     // AMC
     sb.append(measure(path+"/Aandoening_A.lola", path+"/Aandoening_A.log.txt", simplified));
     sb.append(measure(path+"/Aandoening_B.lola", path+"/Aandoening_B.log.txt", simplified));
     sb.append(measure(path+"/Aandoening_C.lola", path+"/Aandoening_C.log.txt", simplified));
     sb.append(measure(path+"/AMC.lola", path+"/AMC.log.txt", simplified));
     
     // Catharina
     sb.append(measure(path+"/Complications.filtered80.lola", path+"/Complications.filtered80.log.txt", simplified));
     
     // Heusden
     sb.append(measure(path+"/Afschriften.lola", path+"/Afschriften.log.txt", simplified));
     sb.append(measure(path+"/BezwaarWOZ_filtered_All.lola", path+"/BezwaarWOZ_filtered_All.log.txt", simplified));
     
     return sb.toString();
   }
   
   public static void main(String[] args) throws IOException, InvalidModelException {

     StringBuilder sb = new StringBuilder();
     
     sb.append(runExperiment("./examples/bpm11/original", false));
     //sb.append(runExperiment("./examples/bpm11/results2/exp1_foldRefold", true));
     //sb.append(runExperiment("./examples/bpm11/results2/exp2_foldRefold_implied", true));
     //sb.append(runExperiment("./examples/bpm11/results2/exp3_implied", true));
     //sb.append(runExperiment("./examples/bpm11/results2/exp4_foldRefold_implied_chains", true));
     //sb.append(runExperiment("./examples/bpm11/results2/exp5_complete", true));
     sb.append(runExperiment("./examples/bpm11/exp5_complete", true));

     //FileWriter fstream = new FileWriter("./examples/bpm11/results2/precision.csv");
     FileWriter fstream = new FileWriter("./examples/bpm11/exp5_complete/precision.csv");
     BufferedWriter out = new BufferedWriter(fstream);
     out.write(sb.toString());
     out.close();
   }

}
