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

import hub.top.uma.DNode;
import hub.top.uma.DNodeBP;
import hub.top.uma.DNodeSet;

import java.util.HashMap;
import com.google.gwt.dev.util.collect.HashSet;
import java.util.LinkedList;
import java.util.Random;
import java.util.Map.Entry;

/**
 * Generate behavioral views from systems by projecting branching processes
 * of system wrt. certain properties. Experimental feature.
 * 
 * @author Dirk Fahland
 */
public class ViewGeneration {

  private DNodeBP build;
  private DNodeSet bp;
  
  public HashMap<DNode, Integer> eventOccurrences;
  private HashSet<DNode> finalEvents;
  
  private int representedTraces;
  
  /**
   * Standard constructor for view generation. The argument takes a branching
   * process construction object {@link DNodeBP} after the branching process has
   * been constructed.
   *  
   * @param build
   */
  public ViewGeneration (DNodeBP build) {
    this.build = build;
    this.bp = build.getBranchingProcess();
    
    eventOccurrences = new HashMap<DNode, Integer>();
    finalEvents = new HashSet<DNode>();
    representedTraces = 0;
  }
  
  /**
   * Check whether the given trace can be executed and accumulate the
   * number of actions in {@link ViewGeneration#eventOccurrences}.
   * 
   * @param trace
   * @return <code>true</code> if the branching process contains a partially
   * ordered run that represents the given <code>trace</code>
   */
  public boolean canExecuteTrace(String[] trace) {
    
    HashSet<DNode> run = new HashSet<DNode>();
    HashSet<DNode> runCut = new HashSet<DNode>();
    
    for (DNode b : bp.initialCut) {
      run.add(b);
      runCut.add(b);
    }
    
    for (int i = 0; i<trace.length; i++) {
      
      HashSet<DNode> enabledEvents = new HashSet<DNode>();
      for (DNode b : runCut) {
        if (b.post == null ) continue;
        for (DNode e : b.post) {
          
          // only look for events that have the same name as the next action in the trace
          String fullActionName = build.getSystem().properNames[e.id];
          //System.out.print("'"+fullActionName+"' = '"+trace[i]+"' ? ");
          
          if (!fullActionName.equals(trace[i])) continue;
          //System.out.print(" yes ");
          
          boolean isEnabled = true;
          for (DNode ePre : e.pre) {
            if (!runCut.contains(ePre)) { isEnabled = false; break; }
          }
          if (isEnabled) enabledEvents.add(e);
        }
      }
      
      if (enabledEvents.isEmpty()) {
        for (DNode b : runCut) {
          if (b.isCutOff) {
            System.out.println("run ended at cut-off event");
            break;
          }
        }
        System.out.print("executed partial trace: ");
        for (int j=0; j < i; j++)
          System.out.print(trace[j]+" ");
        System.out.print(" | ");
        for (int j=i; j < trace.length; j++)
          System.out.print(trace[j]+" ");
        
        System.out.println("\n"+runCut);
        
        return false;
      }

      // get one enabled event and fire it
      DNode fireEvent = enabledEvents.iterator().next();
      for (DNode b : fireEvent.pre) {
        runCut.remove(b);
      }
      run.add(fireEvent);
      
      if (fireEvent.isCutOff) {
        DNode equivEvent = build.equivalentNode().get(fireEvent);
        System.out.println("shifting cut to "+equivEvent+".post");
        if (equivEvent.post != null)
          for (DNode b : equivEvent.post) {
            run.add(b);
            runCut.add(b);
          }
        
      } else {
        if (fireEvent.post != null)
          for (DNode b : fireEvent.post) {
            run.add(b);
            runCut.add(b);
          }
      }
    }
    
    return true;
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
    HashSet<DNode> run = new HashSet<DNode>();
    HashSet<DNode> runCut = new HashSet<DNode>();
    
    for (DNode b : bp.initialCut) {
      run.add(b);
      runCut.add(b);
    }
    
    for (int i = 0; i<trace.length; i++) {
      
      HashSet<DNode> enabledEvents = new HashSet<DNode>();
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
        run.add(fireEvent);
        if (fireEvent.post != null)
          for (DNode b : fireEvent.post) {
            run.add(b);
            runCut.add(b);
          }
        
        if (!eventOccurrences.containsKey(fireEvent))
          eventOccurrences.put(fireEvent, 0);
        eventOccurrences.put(fireEvent, eventOccurrences.get(fireEvent)+1);

      } else {
        
        HashSet<DNode> fireableEvents = new HashSet<DNode>();
        for (DNode e : build.getSystem().fireableEvents) {
          if (build.getSystem().properNames[e.id].equals(trace[i]))
            fireableEvents.add(e);
        }
        
        DNodeBP.EnablingInfo enablingInfo = new DNodeBP.EnablingInfo();
        
        for (DNode e : fireableEvents) {
          boolean isEnabled = true;
          DNode loc[] = new DNode[e.pre.length];
          for (int pre_index = 0; pre_index < e.pre.length; pre_index++) {
            boolean endsWith_b = false;
            for (DNode bCut : runCut) {
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

        if (enablingInfo.locations.size() > 0) {
          if (enablingInfo.locations.size() > 1) {
            System.out.print("multitple ");
          }
          
          Short eventId = enablingInfo.locations.keySet().iterator().next();
          DNode[] events = enablingInfo.locations.get(eventId)[0].events;
          DNode[] loc = enablingInfo.locations.get(eventId)[0].loc;
          
          DNode[] postConditions = bp.fire(events, loc, false);
          if (postConditions != null && postConditions.length > 0) {
            DNode newEvent = postConditions[0].pre[0];            

            // update co-relation for all new post-conditions
            build.updateConcurrencyRelation(postConditions);
            
            // and set fields for the event
            build.setCurrentPrimeConfig(newEvent, true);
            
            for (DNode b : newEvent.pre) {
              runCut.remove(b);
            }
            run.add(newEvent);
            for (DNode b : postConditions) {
              runCut.add(b);
            }
            
            if (!eventOccurrences.containsKey(newEvent))
              eventOccurrences.put(newEvent, 0);
            eventOccurrences.put(newEvent, eventOccurrences.get(newEvent)+1);
          } else {
            System.out.println("fired event with empty post-set");
          }
        } else {

          /*
          System.out.print("executed partial trace: ");
          for (int j=0; j < i; j++)
            System.out.print(trace[j]+" ");
          System.out.print(" | ");
          System.out.print(" next event "+trace[i]);
           */
          DNode t = null;
          for (DNode e : build.getSystem().fireableEvents) {
            if (build.getSystem().properNames[e.id].equals(trace[i])) { 
              t = e; break;
            }
          }
          /*
          System.out.print(" requires "+DNode.toString(t.pre)+" available "+runCut);
           */
          
          //System.out.println(" adding tau");
          
          DNode cut[] = new DNode[runCut.size()];
          runCut.toArray(cut);
          DNode.sortIDs(cut);
          
          DNode tauPre[] = new DNode[runCut.size()];
          for (int iCut = 0; iCut < cut.length; iCut++) {
            tauPre[i] = new DNode(cut[iCut].id, (DNode[])null);
          }

          DNode tauEvent = new DNode(t.id, tauPre);
          for (DNode b : tauPre) {
            b.addPostNode(tauEvent);
          }
          for (DNode b : t.pre) {
            DNode bNew = new DNode(b.id, new DNode[]{tauEvent});
            tauEvent.addPostNode(bNew);
          }
          
          DNode postConditions[] = bp.fire(tauEvent, cut, false);
          
          for (DNode b : cut) {
            runCut.remove(b);
          }
          for (DNode b : postConditions) {
            runCut.add(b);
          }
          build.updateConcurrencyRelation(postConditions);
          build.setCurrentPrimeConfig(postConditions[0].pre[0], true);
          
          /*
          for (int j=i; j < trace.length; j++)
            System.out.print(trace[j]+" ");
          */
          // return false;
          i--;      // we did not fire the i-th event yet, reset
          continue; // and retry
        }
      }
    } // for all events in the trace

    boolean hasEnd = true;
    DNode existing_end_event = null;
    for (DNode b : runCut) {
      /*
      for (DNode e : b.pre) {
        boolean allInCut = true;
        for (DNode ePost : e.post) {
          if (!runCut.contains(ePost)) { allInCut = false; break; }
        }
        if (allInCut) {
          finalEvents.add(e);
        }
      }
      */
      if (b.post == null) {
        hasEnd = false;
        continue;
      }
      
      for (DNode e : b.post) {
        if (e.id == build.getSystem().nameToID.get("uma_end_event")) {
          if (existing_end_event == null) existing_end_event = e;
          if (existing_end_event != null && existing_end_event != e) hasEnd = false;
          break;
        }
      }
    }
    
    if (hasEnd && existing_end_event != null) {
      eventOccurrences.put(existing_end_event, eventOccurrences.get(existing_end_event)+1);
    } else {


      DNode uma_end_event = new DNode(build.getSystem().nameToID.get("uma_end_event"), 0);
      DNode uma_end_post = new DNode(build.getSystem().nameToID.get("uma_end_event"), new DNode[] { uma_end_event} );
      uma_end_event.addPostNode(uma_end_post);
      
      DNode[] finalLoc = new DNode[runCut.size()];
      runCut.toArray(finalLoc);
      DNode.sortIDs(finalLoc);
      DNode[] postConditions = bp.fire(uma_end_event, finalLoc, false);
      
      if (postConditions != null && postConditions.length > 0) {
        DNode newEvent = postConditions[0].pre[0];
        
        finalEvents.add(newEvent);
        eventOccurrences.put(newEvent, 1);
        
        // update co-relation for all new post-conditions
        build.updateConcurrencyRelation(postConditions);
        // and set fields for the event
        build.setCurrentPrimeConfig(newEvent, true);
      }

      //System.out.println("create new end event");
    }
    
    representedTraces++;
    return true;
  }
  
  /**
   * Generate a trace from the constructed branching process that can be
   * executed in the system.
   * 
   * @param upperBound   maximum length of the trace
   * @return a trace of the system
   */
  public LinkedList<String> generateRandomTrace(int upperBound) {
    LinkedList<String> trace = new LinkedList<String>();
    HashSet<DNode> runCut = new HashSet<DNode>();
    Random r = new Random();
    
    for (DNode b : bp.initialCut) {
      runCut.add(b);
    }
    
    int i=0;
    while (i < upperBound) {
      
      HashSet<DNode> enabledEvents = new HashSet<DNode>();
      for (DNode b : runCut) {
        if (b.post == null ) continue;
        for (DNode e : b.post) {
          
          if (e.isAnti && e.isHot) continue;
          
          boolean isEnabled = true;
          for (DNode ePre : e.pre) {
            if (!runCut.contains(ePre)) { isEnabled = false; break; }
          }
          if (isEnabled) enabledEvents.add(e);
        }
      }
      
      if (enabledEvents.isEmpty()) {
        System.out.println("no enabled event");
        break;
      }
      
      DNode[] toChoose = new DNode[enabledEvents.size()];
      enabledEvents.toArray(toChoose);
      int chosenEvent = r.nextInt(toChoose.length);
      
      // get one enabled event and fire it
      DNode fireEvent = toChoose[chosenEvent];
      for (DNode b : fireEvent.pre) {
        runCut.remove(b);
      }

      if (fireEvent.post != null) {
        
        if (fireEvent.isCutOff) {
          DNode cutOff = build.equivalentNode().get(fireEvent);
          
          build.getBranchingProcess().getPrimeCut(fireEvent, true, true);
          HashSet<DNode> fireEvent_prime = build.getBranchingProcess().getPrimeConfiguration;
          
          DNode[] cut = new DNode[runCut.size()];
          runCut.toArray(cut);
          HashSet<DNode> fullConfig = build.getBranchingProcess().getConfiguration(cut);
          fullConfig.removeAll(fireEvent_prime);
          
          System.out.println("firing cut-off event "+cutOff+" instead of "+fireEvent);
          if (fullConfig.size() > 0) {
            System.out.println("dangling events "+fullConfig);
          }
          
          for (DNode b : cutOff.post) {
            runCut.add(b);
          }
          trace.addLast(build.getSystem().properNames[cutOff.id]);
        } else {
          for (DNode b : fireEvent.post) {
            runCut.add(b);
          }
          trace.addLast(build.getSystem().properNames[fireEvent.id]);
        }
      }
    }
    return trace;
  }
  
  /**
   * @param min
   * @return a list of sets of maximal events of the branching process where for
   * each set of events the sum of the number of
   * {@link ViewGeneration#eventOccurrences} amounts to at least <code>min</code>
   *  
   */
  public LinkedList< HashSet<DNode> > getViewConfigurations(float min) {
    LinkedList< HashSet<DNode> > viewConfigs = new LinkedList<HashSet<DNode>>();
    
    DNode[] finalEventList = new DNode[finalEvents.size()];
    finalEvents.toArray(finalEventList);
    boolean[] configCounter = new boolean[finalEvents.size()+1];
    
    int done = configCounter.length-1;
    
    int threshold = (int)(((float)representedTraces)*min);
    
    System.out.println("checking 2^"+done+" configurations");
    
    int foundConfigs = 0;
    while (!configCounter[done]) {
      
      for (int i=0;i<configCounter.length;i++) {
        if (!configCounter[i]) {
          configCounter[i] = true;
          break;
        } else {
          configCounter[i] = false;
        }
      }
      
      int traceCount = 0;
      for (int i=0;i<done;i++) {
        if (configCounter[i]) traceCount += eventOccurrences.get(finalEventList[i]);
      }
      
      if (traceCount > threshold) {
        /*
        HashSet<DNode> config = new HashSet<DNode>();
        for (int i=0;i<done;i++) {
          if (configCounter[i]) config.add(finalEventList[i]);
        }
        viewConfigs.addLast(config);
        */
        foundConfigs++;
        //System.out.print(".");
        //if (foundConfigs%80 == 0) System.out.println();
        if (foundConfigs%1000 == 0) System.out.println(foundConfigs);
      }
      
      //System.out.println(toString(configCounter));
    }
    
    System.out.println("done. Found "+foundConfigs+" configurations");
    
    return viewConfigs;
  }
  
  public static String toString(boolean arr[]) {
    if (arr == null) return "null";
    
    String result = "[";
    for (int i=0;i<arr.length;i++) {
      if (i > 0) result+=", ";
      if (arr[i]) result += "1";
      else result += "0";
    }
    return result+"]";
  }
 
  /**
   * @return all events that are the last event of a trace
   */
  public HashSet<DNode> getFinalEvents() {
    return finalEvents;
  }
  
  /**
   * @param percentageMin
   * @param percentageMax
   * @return all events that are the last event of a trace and occur at least
   * <code>percentageMin</code> and at most <code>percentageMax</code>
   */
  public HashSet<DNode> getFinalEvents_ifBetween(float percentageMin, float percentageMax) {
    HashSet<DNode> keep = new HashSet<DNode>();
    for (DNode e : finalEvents) {

      float ePercentage = (float)eventOccurrences.get(e)/(float)representedTraces;
      
      if ( percentageMin <= ePercentage && ePercentage <= percentageMax) {
        System.out.println("keeping "+e);
        if (e.post != null) {
          for (DNode b : e.post) keep.add(b);
        } else {
          keep.add(e);
        }
      }
    }
    return keep;
  }
  
  public HashSet<DNode> getDownClosure(HashSet<DNode> fromEvents) {
    HashSet<DNode> keep = new HashSet<DNode>(fromEvents);
    LinkedList<DNode> queue = new LinkedList<DNode>(keep);
    while (!queue.isEmpty()) {
      DNode d = queue.removeFirst();
      if (d.pre == null) continue;
      for (DNode dPre : d.pre) {
        // keep all predecessors of a kept event
        if (keep.contains(dPre)) continue;
        keep.add(dPre);
        queue.addLast(dPre);
        // also keep all post-conditions of all events
        if (dPre.isEvent && dPre.post != null) { 
          for (DNode b : dPre.post)
            keep.add(b);
        }
      }
    }
    return keep;
  }
    
  /**
   * Remove all events that are not part of a run occurrence less than
   * the given percentage according to {@link ViewGeneration#eventOccurrences}.
   * 
   * @param percentage
   */
  public void keepIfAbove(float percentage) {
    
    HashSet<DNode> keep = getFinalEvents_ifBetween(percentage, 1.0f);
    keep = getDownClosure(keep);
    
    for (DNode b : bp.allConditions) {
      if (!keep.contains(b)) {
        b.isAnti = true;
        b.isHot = true;
      }
    }
    for (DNode e : bp.allEvents) {
      if (!keep.contains(e)) {
        e.isAnti = true;
        e.isHot = true;
      }
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
        System.out.println(e+" is a cut-off event");
        e.isCutOff = true;
        if (e.post != null)
          for (DNode b : e.post) {
            b.isCutOff = true;
          }
      }
    }
  }
  
  /**
   * 
   */
  public void printFinalEventOccurrences() {
    
    int sumFinal = 0;
    
    for (Entry<DNode, Integer> entry : eventOccurrences.entrySet()) {
      if (finalEvents.contains(entry.getKey())) {
        System.out.print(entry.getKey()+": "+entry.getValue()+" ("+((float)entry.getValue()/(float)representedTraces)+")");
        System.out.println(" is final");
        sumFinal += entry.getValue();
      }
    }
    System.out.println("total final events: "+sumFinal+"/"+representedTraces);
  }
}
