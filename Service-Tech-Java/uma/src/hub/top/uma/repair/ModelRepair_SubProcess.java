/*****************************************************************************\
 * Copyright (c) 2008-2012. Dirk Fahland. AGPL3.0
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

import hub.top.petrinet.Arc;
import hub.top.petrinet.ISystemModel;
import hub.top.petrinet.Node;
import hub.top.petrinet.PetriNet;
import hub.top.petrinet.Place;
import hub.top.petrinet.Transition;
import hub.top.petrinet.unfold.DNodeSys_PetriNet;
import hub.top.scenario.OcletIO_Out;
import hub.top.uma.DNode;
import hub.top.uma.DNodeBP;
import hub.top.uma.DNodeBP.EnablingInfo;
import hub.top.uma.DNodeBP.SyncInfo;
import hub.top.uma.DNodeRefold;
import hub.top.uma.DNodeSet;
import hub.top.uma.DNodeSet.DNodeSetElement;
import hub.top.uma.DNodeSys;
import hub.top.uma.InvalidModelException;
import hub.top.uma.Uma;
import hub.top.uma.view.ViewGeneration;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Comparator;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Scanner;
import java.util.Set;
import java.util.SortedSet;
import java.util.TreeSet;

import javax.swing.plaf.ListUI;

import com.google.gwt.dev.util.collect.HashMap;
import com.google.gwt.dev.util.collect.HashSet;

public class ModelRepair_SubProcess {
  
  private DNodeRefold   build;
  private DNodeSet      bp;
  
  private PetriNet      net;
  
  private Map<DNode, Integer> nodeOccurrences;
  
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
   * Standard constructor for view generation. The argument takes a {@link PetriNet}
   * and a branching process construction object {@link DNodeBP} of this net. 
   *  
   * @param net
   * @param build
   */
  public ModelRepair_SubProcess (PetriNet net, DNodeRefold build) {
    this.build = build;
    this.bp = build.getBranchingProcess();
    this.net = net;
    
    nodeOccurrences = new HashMap<DNode, Integer>();
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
  
  public static class Move {
    public Transition t;
    public String     e;
    public boolean isSkipStep = false;
    
    public Move(String e) {
      this.e = e;
      this.t = null;
    }
    
    public Move(Transition t) {
      this.e = null;
      this.t = t;
    }
    
    @Override
    public String toString() {
      return (t != null) ? t.toString() : e;
    }
  }
  
  private Map<DNode, DNode[]> conditionOrigin = new HashMap<DNode, DNode[]>();
  
  public static class MoveOnLog {
    
    public Set<Place> location = new HashSet<Place>();
    public String event;
    
    public Set<Place> tokensProducedBefore = null;
    public Set<Place> tokensConsumedAfter = null;
    
    public MoveOnLog(String e, Set<Place> loc) {
      location.addAll(loc);
      event = e;
    }
    
    @Override
    public String toString() {
      return event.toString();
    }
  }
  
  private List<List<MoveOnLog>> moveOnLogSequences = new LinkedList<List<MoveOnLog>>();
  private List<Set<Place>> visitedMarkings = new LinkedList<Set<Place>>();
  
  private List<List<Transition>> moveOnModelSequences = new LinkedList<List<Transition>>();

  
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
  private boolean extendByTrace(Move[] trace, Map<String, String> e2t) {
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

    Transition lastSynchronousMove = null;
    MoveOnLog lastLogMove = null;
    List<Transition> currentMoveOnModel = new LinkedList<Transition>();
    LinkedList<MoveOnLog> currentMoveOnLog = new LinkedList<MoveOnLog>();
    Set<Place> currentMoveOnLogTokens = null;
    
    for (int i = 0; i<trace.length; i++) {
      
      Move m = trace[i];

      String eventName;
      String transitionName;
      boolean isSkipStep;
      
      if (m.t == null) {
        if (e2t == null || !e2t.containsKey(trace[i])) eventName = m.e;
        else eventName = e2t.get(trace[i]);
        
        isSkipStep = false;
  
        if (eventName.startsWith("tau_")) {
          transitionName = eventName.substring(4);
          isSkipStep = true;
        } else {
          transitionName = eventName;
        }
      } else {
        transitionName = m.t.getName();
        isSkipStep = m.isSkipStep;
      }
      
      SortedSet<DNode> existingEvents = new TreeSet<DNode>(dnode_id_comparator);
      for (DNode b : state.marking) {
        if (b.post == null ) continue;
        for (DNode e : b.post) {

          // only look for events that have the same name as the next action in the trace
          if (m.t == null) {
            // ignore events that do not match the event name
            if (!build.getSystem().properNames[e.id].equals(transitionName)) continue;
          } else {
            // ignore events that do not match the transition
            if (!getOriginalTransitions(e).contains(m.t)) continue;
            //if (!build.getSystem().properNames[e.id].equals(m.t.getName())) continue;
          }
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
        
        System.out.println("reuse "+getOriginalTransitions(fireEvent)+" @ "+getOriginalTransitions(fireEvent).get(0).getPreSet());
        
        if (isSkipStep) {
          // model move
          fireEvent.isImplied = true;
          currentMoveOnModel.add(getOriginalTransitions(fireEvent).get(0));
        } else {
          // synchronous move
          if (currentMoveOnModel.size() > 0) {
            this.moveOnModelSequences.add(currentMoveOnModel);
            currentMoveOnModel = new LinkedList<Transition>();
          }
          // remember as last synchronous move
          lastSynchronousMove = getOriginalTransitions(fireEvent).get(0);
        }
        
        produced = new LinkedList<DNode>();
        for (DNode b : fireEvent.post) {
          produced.add(b);
        }
        
        if (currentMoveOnLog.size() > 0) {
          if (updateMoveOnLog(fireEvent.pre, fireEvent, transitionName, currentMoveOnLogTokens, currentMoveOnLog)) {
            currentMoveOnLog = new LinkedList<MoveOnLog>();
            currentMoveOnLogTokens = null;
          }
        }
      }
        
      if (succState == null) {
        
        if (fireEvent == null) {
        
          HashSet<DNode> matchingEvents = new HashSet<DNode>();
          for (DNode e : build.getSystem().fireableEvents) {
            
            if (m.t == null) {
              // no mapped transition, find a matching one by tranistion name
              if (build.getSystem().properNames[e.id].equals(transitionName)) {
                System.out.println("add "+e+" because of name "+transitionName);
                matchingEvents.add(e);
              }
            } else {
              DNodeSys_PetriNet sysModel = (DNodeSys_PetriNet)this.build.getSystem();
              if (sysModel.getOriginalNode(e) == m.t) {
                System.out.println("add "+e+" @ "+sysModel.getOriginalNode(e).getPreSet()+" because of transition "+m.t+" "+m.t.getPreSet());
                matchingEvents.add(e);
              } else if (sysModel.getOriginalNode(e).getName().equals(m.t.getName())) {
                System.out.println("not taking "+e+" @ "+sysModel.getOriginalNode(e).getPreSet()+" for "+m.t+" "+m.t.getPreSet());
              }
            }
          }
          
          System.out.println("matching events: "+matchingEvents);
          EnablingInfo enablingInfo = getAllEnabledEvents(state.marking, matchingEvents);
          
          if (enablingInfo.locations.size() == 0 && m.t != null) {
            System.out.println("Error: given transition "+m.t+" ("+m.t.getPreSet()+") is not enabled at "+state.marking);
            System.out.println("Trying all other transitions with the same name");
            matchingEvents.clear();
            
            for (DNode e : build.getSystem().fireableEvents) {
              DNodeSys_PetriNet sysModel = (DNodeSys_PetriNet)this.build.getSystem();
              if (sysModel.getOriginalNode(e).getName().equals(m.t.getName())) {
                System.out.println("taking "+e+" @ "+sysModel.getOriginalNode(e).getPreSet()+" for "+m.t+" "+m.t.getPreSet());
                matchingEvents.add(e);
              }
            }

            System.out.println("matching events: "+matchingEvents);
            enablingInfo = getAllEnabledEvents(state.marking, matchingEvents);
          }
  
          if (enablingInfo.locations.size() > 0)
          {
            if (enablingInfo.locations.size() > 1) {
              // we have several enabled events, issue a warning if there are
              // two events that could occur at different locations
              
              System.out.println("Warning: multiple enabled events: ");
              for (Entry< Short, SyncInfo[] > location : enablingInfo.locations.entrySet()) {
                System.out.println(build.getSystem().properNames[location.getKey()]);
                for (SyncInfo sync : location.getValue()) {
                  System.out.println("  @"+DNode.toString(sync.loc));
                }
              } // end of warning
            }
            
            Short eventId = enablingInfo.locations.keySet().iterator().next();
            enablingInfo.locations.get(eventId)[0].reduce();
            DNode[] events = enablingInfo.locations.get(eventId)[0].events;
            DNode[] loc = enablingInfo.locations.get(eventId)[0].loc;
            
            System.out.println("firing enabled transition "+build.getSystem().properNames[events[0].id]+" @ "+DNode.toString(loc));
            
            DNode[] postConditions = bp.fire(events, loc, false);
            if (postConditions != null && postConditions.length > 0) {
              fireEvent = postConditions[0].pre[0];
              
              if (isSkipStep) {
                // model move
                fireEvent.isImplied = true;
                currentMoveOnModel.add(getOriginalTransitions(fireEvent).get(0));
              }
              else {
                // synchronous move
                fireEvent.isImplied = false;
                if (currentMoveOnModel.size() > 0) {
                  this.moveOnModelSequences.add(currentMoveOnModel);
                  currentMoveOnModel = new LinkedList<Transition>();
                }
                // remember as last synchronous move
                lastSynchronousMove = getOriginalTransitions(fireEvent).get(0);
              }
              
              for (int b=0; b<postConditions.length; b++) {
                DNode []causedBy = new DNode[events.length];
                for (int o=0; o<events.length; o++) {
                  causedBy[o] = events[o].post[b];
                }
                this.conditionOrigin.put(postConditions[b], causedBy);
              }
              
              if (currentMoveOnLog.size() > 0) {
                if (updateMoveOnLog(loc, fireEvent, transitionName, currentMoveOnLogTokens, currentMoveOnLog)) {
                  currentMoveOnLog = new LinkedList<MoveOnLog>();
                  currentMoveOnLogTokens = null;
                }
              }
              
              // update co-relation for all new post-conditions
              //build.updateConcurrencyRelation(postConditions);
              
              // and set fields for the event
              //build.setCurrentPrimeConfig(newEvent, true);
  
            } else {
              System.out.println("fired event with empty post-set");
            }
          } else {
            if (currentMoveOnModel.size() > 0) {
              this.moveOnModelSequences.add(currentMoveOnModel);
              currentMoveOnModel = new LinkedList<Transition>();
            }
            
            DNodeSys_PetriNet sysModel = (DNodeSys_PetriNet)this.build.getSystem();
            Set<Place> mark = new HashSet<Place>();
            for (DNode d : state.marking) {
              List<Place> orig = getOriginalPlaces(d);
              for (Place p : orig) {
                if (p == null) {
                  System.out.println("oh");
                  System.out.println("no original places for "+d);
                  getOriginalPlaces(d);
                }
                mark.add(p);
              }              
            }
            
            if (currentMoveOnLogTokens == null) {
              currentMoveOnLogTokens = mark;
            }

            System.out.println("could not fire "+transitionName+" ("+trace[i]+") in "+mark);
            if (m.t != null) {
              System.err.println("ERROR: log move on model move or synchronous move");
              for (int ra = 0; ra<=i; ra++) {
                System.err.print(trace[ra]+" ");
              }
              System.err.println();
              return false;
            }
            currentMoveOnLog.add(new MoveOnLog(transitionName, mark));

            // remember the tokens that were produced by the preceding synchronous move
            currentMoveOnLog.getLast().tokensProducedBefore = new HashSet<Place>();
            if (lastSynchronousMove != null)
              for (Place p : lastSynchronousMove.getPostSet()) currentMoveOnLog.getLast().tokensProducedBefore.add(p);
            else
              for (DNode d : initialState.marking) currentMoveOnLog.getLast().tokensProducedBefore.addAll(getOriginalPlaces(d));
            
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
    
    if (currentMoveOnLog.size() > 0) {
      this.moveOnLogSequences.add(currentMoveOnLog);
      System.out.println("adding final "+currentMoveOnLog+" @ "+currentMoveOnLogTokens);
        
      this.visitedMarkings.add(currentMoveOnLogTokens);
      currentMoveOnLog = new LinkedList<MoveOnLog>();
      currentMoveOnLogTokens = null;
    }

    return true;
  }
  
  private boolean updateMoveOnLog(DNode[] loc, DNode fireEvent, String transitionName, Set<Place> currentMoveOnLogTokens, LinkedList<MoveOnLog> currentMoveOnLog) {
    DNodeSys_PetriNet sysModel = (DNodeSys_PetriNet)this.build.getSystem();
    Set<Place> consuming = new HashSet<Place>();
    for (DNode d : loc) {
      for (DNode org : this.conditionOrigin.get(d)) {
        consuming.add((Place)sysModel.getOriginalNode(org));
      }              
    }
    
    /*
    boolean end = false;
    for (Place p : consuming) {
      if (currentMoveOnLogTokens.contains(p)) {
        end = true;
        break;
      }
    }
    */
    HashSet<Place> remaining = new HashSet<Place>(currentMoveOnLogTokens);
    remaining.removeAll(consuming);
    boolean end = remaining.isEmpty();
    
    if (end) {
      // last log move remembers the tokens consumed immediately after this last log move
      Transition succeedingTransition = getOriginalTransitions(fireEvent).get(0);
      currentMoveOnLog.getLast().tokensConsumedAfter = new HashSet<Place>();
      for (Place p : succeedingTransition.getPreSet()) currentMoveOnLog.getLast().tokensConsumedAfter.add(p);
      
      // store move on log sequence
      this.moveOnLogSequences.add(currentMoveOnLog);
      System.out.println("adding1 "+currentMoveOnLog+" @ "+currentMoveOnLogTokens);
      this.visitedMarkings.add(currentMoveOnLogTokens);
      
      return true;
      
    } else {
      System.out.println(currentMoveOnLogTokens+" --"+transitionName+"--> "+remaining+" "+(DNode.toString(loc)));
      currentMoveOnLogTokens.removeAll(consuming);
      if (currentMoveOnLog.size() > 0 && currentMoveOnLog.getLast().tokensConsumedAfter == null) {
        currentMoveOnLog.getLast().tokensConsumedAfter = new HashSet<Place>();
        for (Place p : consuming) currentMoveOnLog.getLast().tokensConsumedAfter.add(p);
      }
      
      return false;
    }
    
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
  
  private void extendByTraces(Collection<Move[]> traces, Map<String, String> e2t) {
    for (Move[] trace : traces) {
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
   
   
   
   public List<SubLog> getExtensions() {
     
     // collect all markings at which an extension was added
     ArrayList<Set<Place>> markingClasses = new ArrayList<Set<Place>>();
     for (int i=0; i<moveOnLogSequences.size();i++) {
       Set<Place> thisIDs = new HashSet<Place>(visitedMarkings.get(i));
       markingClasses.add(thisIDs);
     }
     
     System.out.println("locations: "+ markingClasses);
     

     // cluster the found log sequences on their location, put a trace to an
     // existing cluster
     HashMap<Set<Place>, List<List<MoveOnLog>>> sublogClasses = new HashMap<Set<Place>, List<List<MoveOnLog>>>();

     for (int i = 0; i < moveOnLogSequences.size(); i++) {
       List<MoveOnLog> seq = moveOnLogSequences.get(i);
       Set<Place> marking = visitedMarkings.get(i);
       
       System.out.println("found "+ moveOnLogSequences.get(i) +" @ "+visitedMarkings.get(i));

       // scan all marking classes for the class with the largest overlap
       // in the worst case, this is the very marking the log sequence occurred at
       Set<Place> largestOverlapOn = null;
       int largestOverlap = 0;
       for (Set<Place> m : markingClasses) {
         int matchSize = 0;
         for (Place p : marking) {
           if (m.contains(p))
             matchSize++;
         }
         if (matchSize > largestOverlap) {
           largestOverlap = matchSize;
           largestOverlapOn = m;
         }
       }

       // put log sequence to this cluster
       if (!sublogClasses.containsKey(largestOverlapOn)) sublogClasses.put(largestOverlapOn, new LinkedList<List<MoveOnLog>>());
       sublogClasses.get(largestOverlapOn).add(seq);
     }
     
     // merge clusters of log sequences if their locations overlap
     // start merging with the log sequences with the largest overlap
     // and repeat until there is still two clusters that can be merged
     boolean changed = true;
     while (changed) {
       Set<Place> merge1 = null;  // location of cluster1 that shall be merged with
       Set<Place> merge2 = null;  // cluster2 at this location
       
       // largest overlap found so far
       int maxOverLap_m1_m2 = 0;

       changed = false;
       
       // iterate over all pairs of clusters and find the pair with the largest overlap 
       for (Set<Place> m1 : sublogClasses.keySet()) {

         for (Set<Place> m2 : sublogClasses.keySet()) {
           if (m2 == m1) continue;
           
           // count number of places in 'm1' and 'm2'
           int overlap_m1_m2 = 0;
           for (Place p : m1) {
             if (m2.contains(p))
               overlap_m1_m2++;
           }
           // remember largest overlap and the overlapping locations 'm1' and 'm2'
           if (overlap_m1_m2 > maxOverLap_m1_m2) {
             maxOverLap_m1_m2 = overlap_m1_m2;
             merge1 = m1;
             merge2 = m2;
           }
         }
       }
       
       // if we found any overlapping locations, then they are largest and we can merge them
       if (merge1 != null && merge2 != null && maxOverLap_m1_m2 > 0) {
         
         // compute overlap of both clusters
         HashSet<Place> m1_intersect_m2 = new HashSet<Place>(merge1);
         m1_intersect_m2.retainAll(merge2);
         
         System.out.println(merge1+" + "+merge2+" -> "+m1_intersect_m2);

         // add all traces to the overlap
         if (!sublogClasses.containsKey(m1_intersect_m2)) sublogClasses.put(m1_intersect_m2, new LinkedList<List<MoveOnLog>>());
         if (!merge1.equals(m1_intersect_m2)) sublogClasses.get(m1_intersect_m2).addAll(sublogClasses.get(merge1));
         if (!merge1.equals(m1_intersect_m2)) sublogClasses.get(m1_intersect_m2).addAll(sublogClasses.get(merge2));
         
         // remove original classes from the map
         if (!merge1.equals(m1_intersect_m2)) sublogClasses.remove(merge1);
         if (!merge2.equals(m1_intersect_m2)) sublogClasses.remove(merge2);
         
         // and find the next overlap
         changed = true;
       }
    }

    List<SubLog> subLogs = new ArrayList<ModelRepair_SubProcess.SubLog>();
     
    for (Set<Place> m : sublogClasses.keySet()) {
      SubLog subLog = new SubLog();
      subLog.location = m;
      subLog.traces = sublogClasses.get(m);
      subLogs.add(subLog);
      
      System.out.println(m + " has traces");
      for (List<MoveOnLog> tr : sublogClasses.get(m)) {
        System.out.println("  " + tr);
      }
    }

    return subLogs;
   }
   
   private static List<Set<Place>> getLocationCovering(Collection<Set<Place>> markings) {
     Map<Place, Set<Set<Place>>> covers = new HashMap<Place, Set<Set<Place>>>();
     for (Set<Place> m : markings) {
       for (Place p : m) {
         if (!covers.containsKey(p)) covers.put(p, new HashSet<Set<Place>>());
         covers.get(p).add(m);
       }
     }
     
     List<Set<Place>> covering = new LinkedList<Set<Place>>();
     while (!covers.isEmpty()) {
       // first collect the places that cover most of the markings
       int maxSize = 0;
       for (Place p : covers.keySet()) {
         if (covers.get(p).size() > maxSize) maxSize = covers.get(p).size();
       }
       Set<Place> maxP = new HashSet<Place>();
       for (Place p : covers.keySet()) {
         if (covers.get(p).size() == maxSize) maxP.add(p);
       }
       
       /*
       // and sort the found places, starting with the lexicographically smallest
       java.util.Collections.sort(maxP, new Comparator<Place>() {
         @Override
        public int compare(Place o1, Place o2) {
          return o1.getName().compareTo(o2.getName());
        }
       });
       Place chosenP = maxP.get(0);
       covering.add(chosenP);
       */
       
       for (Place chosenP : maxP) {
         // now remove all markings that contain chosenP and update the map,
         // i.e. remove all map entries pointing to no marking
         Set<Place> empty = new HashSet<Place>();
         for (Place p : covers.keySet()) {
           Set<Set<Place>> m_covered = new HashSet<Set<Place>>();
           for (Set<Place> m : covers.get(p)) {
             if (m.contains(chosenP)) m_covered.add(m);
           }
           covers.get(p).removeAll(m_covered);
           if (covers.get(p).isEmpty()) empty.add(p);
         }
         for (Place p : empty) {
           covers.remove(p);
         }
       }
       covering.add(maxP);
     }
     return covering;
   }
   

   
   public static class SubLog {
     public Set<Place>            location;
     public List<List<MoveOnLog>> traces;
     
     public List<List<String>> getEventTraces() {
       List<List<String>> eventTraces = new ArrayList<List<String>>();
       for (List<MoveOnLog> subtrace : traces) {
         List<String> eventTrace = new ArrayList<String>();
         for (MoveOnLog m : subtrace) eventTrace.add(m.event);
         eventTraces.add(eventTrace);
       }
       return eventTraces;
     }
   }
   
   private List<List<MoveOnLog>> splitTrace(List<MoveOnLog> trace, List<MoveOnLog> subtrace) {
     List<MoveOnLog> prefix = new ArrayList<ModelRepair_SubProcess.MoveOnLog>();
     List<MoveOnLog> match = new ArrayList<ModelRepair_SubProcess.MoveOnLog>();
     List<MoveOnLog> suffix = new ArrayList<ModelRepair_SubProcess.MoveOnLog>();
     
     boolean seenFirst = false;
     boolean seenLast = false;
     int subtrace_pos = 0;
     for (MoveOnLog m : trace) {
       if (!seenFirst && m.event.equals(subtrace.get(0).event)) seenFirst = true;
       if (!seenFirst) {
         prefix.add(m);
       } else if (!seenLast) {
         if (m.event.equals(subtrace.get(subtrace_pos).event)) {
           match.add(m);
           subtrace_pos++;
           
           if (subtrace_pos == subtrace.size()) seenLast = true;
         } else {
           return null; // did not see entire subtrace in trace, no splitting
         }
       } else {
         suffix.add(m);
       }
     }
     
     if (!seenLast) return null;
     if (prefix.size() == 0 && suffix.size() == 0) return null;
     
     List<List<MoveOnLog>> splitted = new LinkedList<List<MoveOnLog>>();
     if (prefix.size() > 0) splitted.add(prefix);
     if (match.size() > 0) splitted.add(match);
     if (suffix.size() > 0) splitted.add(suffix);
     return splitted;
   }
   
   private Set<String> getEventAlphabet(List<MoveOnLog> trace) {
     Set<String> eventAlphabet = new TreeSet<String>();
     for (MoveOnLog move : trace) {
       String event = move.event;
       eventAlphabet.add(event);
     }
     return eventAlphabet;
   }
   
   private void separateTracesByLength(List<List<MoveOnLog>> traces, Map<Integer, List<List<MoveOnLog>>> traces_by_length) {
     for (List<MoveOnLog> trace : traces) {
       int l = trace.size();
       if (!traces_by_length.containsKey(l)) traces_by_length.put(l, new LinkedList<List<MoveOnLog>>());
       traces_by_length.get(l).add(trace);
     }
   }
   
   private void _print_traces(Map<Integer, List<List<MoveOnLog>>> traces_by_length) {
     TreeSet<Integer> lengths = new TreeSet<Integer>(traces_by_length.keySet());
     for (int l : lengths) {
       System.out.println("length: "+l+" -----------------------------");
       for (List<MoveOnLog> trace : traces_by_length.get(l)) {
         System.out.println(trace);
       }
     }
   }
   
   public List<SubLog> alignExtensions(List<SubLog> extension) {
     
     List<SubLog> aligned = new ArrayList<SubLog>();
     
     for (SubLog sublog : extension) {
       List<List<MoveOnLog>> traces = sublog.traces;
       
       Map<Integer, List<List<MoveOnLog>>> traces_by_length = new HashMap<Integer, List<List<MoveOnLog>>>(); 
       separateTracesByLength(traces, traces_by_length);
       int maxlength = 0;
       for (Integer len : traces_by_length.keySet()) if (len > maxlength) maxlength = len;
       
//       System.out.println("--------------- original traces ------------------");
//       _print_traces(traces_by_length);
       
       // partition each long trace Long into Long = 'Prefix Short Suffix' where
       // Short is a trace from the traceset, repeat this until reaching a fixed point,
       // starting with longest traces (both for Long and for Short)
       for (int len=maxlength; len > 0; len--) {
         if (!traces_by_length.containsKey(len)) continue;
         
         //System.out.println(">>> "+len);

         // for each Long trace
         List<List<MoveOnLog>> toRemove = new LinkedList<List<MoveOnLog>>();
         for (List<MoveOnLog> long_trace : traces_by_length.get(len)) {
           
           boolean long_trace_split = false;
           Set<String> long_alph = getEventAlphabet(long_trace);

           // and each Short trace that is shorter than Long
           for (int short_len=len-1; short_len > 0; short_len--) {
             if (!traces_by_length.containsKey(short_len)) continue;
             
             Map<Integer, List<List<MoveOnLog>>> separated = new HashMap<Integer, List<List<MoveOnLog>>>(); 
             for (List<MoveOnLog> short_trace : traces_by_length.get(short_len)) {
               
               Set<String> short_alph = getEventAlphabet(short_trace);

               // if they have overlapping alphabets
               if (alphabetOverlap(long_alph, short_alph) == 0) continue;
               // and are distinct enough
               if (sameCluster(short_alph, long_alph)) continue;
               
               // try splitting, i.e. see if short_trace is a subtrace of long_trace,
               // if so also return the prefix and the suffix
               List<List<MoveOnLog>> splitted = splitTrace(long_trace, short_trace);
               if (splitted != null) {
                 System.out.println(splitted);
                 
                 // if long_trace could be split, sort them based on their lengths 
                 separateTracesByLength(splitted, separated);
                 // and remove the long trace from the trace set
                 toRemove.add(long_trace);
                 long_trace_split = true;
                 break;
               }
             }
             // add all new separated traces to the set of all traces
             for (Integer new_len : separated.keySet()) {
               if (new_len >= len)
                 System.out.println("error!");
               if (!traces_by_length.containsKey(new_len)) traces_by_length.put(new_len, new LinkedList<List<MoveOnLog>>());
               traces_by_length.get(new_len).addAll(separated.get(new_len));
             }
             if (long_trace_split) break;
           }
         }
         traces_by_length.get(len).removeAll(toRemove);
       }
//       System.out.println("--------------- splitted traces ------------------");
//       _print_traces(traces_by_length);
       
       
       // now cluster traces with similar alphabets until reaching a fixed point
       // first sort trace sets by alphabets
       Map<Set<String>, List<List<MoveOnLog>>> clusteredTraces = new HashMap<Set<String>, List<List<MoveOnLog>>>();
       TreeSet<Integer> lengths = new TreeSet<Integer>(traces_by_length.keySet());
       for (int l : lengths) {
         for (List<MoveOnLog> trace : traces_by_length.get(l)) {
           Set<String> alph = getEventAlphabet(trace);
           if (!clusteredTraces.containsKey(alph)) clusteredTraces.put(alph, new LinkedList<List<MoveOnLog>>());
           clusteredTraces.get(alph).add(trace);
         }
       }
       // then merge any two trace sets if their alphabets overlap large enough,
       // until no two trace sets can be clustered anymore
       boolean merged = true;
       while (merged) {
         merged = false;
         
//         System.out.println("--------------- clustered traces ------------------");
//         for (Set<String> alph : clusteredTraces.keySet()) {
//           System.out.println(">>> "+alph);
//           for (List<MoveOnLog> trace : clusteredTraces.get(alph)) {
//             System.out.println(trace);
//           }
//         }
         
         ArrayList<Set<String>> orderedAlphabets = new ArrayList<Set<String>>();
         for (Set<String> alphabet : clusteredTraces.keySet()) orderedAlphabets.add(alphabet);
         boolean cluster[][] = new boolean[orderedAlphabets.size()][orderedAlphabets.size()];
         for (int i=0; i<orderedAlphabets.size();i++) {
           cluster[i][i] = true;
           for (int j=i+1; j<orderedAlphabets.size();j++) {
             cluster[i][j] = sameCluster(orderedAlphabets.get(i), orderedAlphabets.get(j));
             cluster[j][i] = cluster[i][j];
           }
         }
         
         // compute transitive closure of clustering relations, as clustering relation
         // is already reflexive and symmetric, we turn cluster[][] into an equivalence relation
         for (int j=0; j<cluster.length; j++) {
           for (int i=0; i<cluster.length; i++) {
             if (i != j && cluster[i][j]) {
               for (int k = 0; k<cluster.length; k++) {
                 cluster[i][k] = cluster[i][k] || cluster[j][k];
               }
             }
           }
         }

         boolean cluster_merged[] = new boolean[orderedAlphabets.size()];
         Map<Set<String>, List<List<MoveOnLog>>> clusteredTraces_update = new HashMap<Set<String>, List<List<MoveOnLog>>>();
         for (int i=0; i<orderedAlphabets.size(); i++) {
           if (cluster_merged[i]) continue;

           Set<String> alph_i = orderedAlphabets.get(i); 
           Set<String> new_alph = new HashSet<String>(alph_i);
           List<List<MoveOnLog>> new_cluster = new LinkedList<List<MoveOnLog>>(clusteredTraces.get(alph_i));
           cluster_merged[i] = true;
           
           for (int j=0; j<orderedAlphabets.size(); j++) {
             if (j == i) continue;
             if (cluster[i][j] == true) {
               Set<String> alph_j = orderedAlphabets.get(j);
               new_alph.addAll(alph_j);
               new_cluster.addAll(clusteredTraces.get(alph_j));
               merged = true;
               cluster_merged[j] = true;
             }
           }
           
           if (!clusteredTraces_update.containsKey(new_alph)) clusteredTraces_update.put(new_alph, new_cluster);
           else clusteredTraces_update.get(new_alph).addAll(new_cluster);
         }
         clusteredTraces = clusteredTraces_update;
       }
       
       System.out.println("--------------- clustered traces ------------------");
       for (Set<String> alph : clusteredTraces.keySet()) {
         System.out.println(">>> "+alph);
         for (List<MoveOnLog> trace : clusteredTraces.get(alph)) {
           System.out.println(trace);
         }
       }

       // finally extract locations of each cluster of traces and return
       // aligned alignment
       for (Set<String> alphabet : clusteredTraces.keySet()) {
         SubLog split_sublog = new SubLog();
         split_sublog.traces = new ArrayList<List<MoveOnLog>>();
         
         List<Set<Place>> allMoveLocations = new LinkedList<Set<Place>>();
         // now factorize traces by factorized alphabets
         for (List<MoveOnLog> trace : clusteredTraces.get(alphabet)) {
           List<MoveOnLog> subTrace = new LinkedList<MoveOnLog>();
           for (MoveOnLog move : trace) {
             subTrace.add(move);
             //System.out.println(move.location+" ");
             allMoveLocations.add(move.location);
           }
           split_sublog.traces.add(subTrace);
         }
         
         List<Set<Place>> cov_sublocations = getLocationCovering(allMoveLocations);
         
         System.out.println("covering sub-locations: "+cov_sublocations);
         
         split_sublog.location = new HashSet<Place>();
         
         // for each sub-location that covers some log move on the current sub-log
         for (Set<Place> cov_sublocation : cov_sublocations) {
           
           // find the place that was most frequently marked directly before the
           // first log move of each trace, use this place as place the place that
           // covers the sub-location
           Map<Place, Integer> freq = new HashMap<Place, Integer>();
           for (Place p : cov_sublocation) freq.put(p, 0);
           int maxFreq = 0;

           for (List<MoveOnLog> trace : split_sublog.traces) {
             if (trace.get(0).tokensProducedBefore != null) {
               for (Place p : trace.get(0).tokensProducedBefore) {
                 if (freq.containsKey(p)) {
                   freq.put(p, freq.get(p)+1);
                   if (maxFreq < freq.get(p)) maxFreq = freq.get(p);
                 }
               }
             }
           }

           // there might be several best-covering places, pick the
           // lexicgraphically smallest
           LinkedList<Place> bestLoc = new LinkedList<Place>();
           for (Place p : freq.keySet()) {
             if (freq.get(p) == maxFreq) bestLoc.add(p);
           }
           java.util.Collections.sort(bestLoc, new Comparator<Place>() {
             @Override
            public int compare(Place o1, Place o2) {
              return o1.getName().compareTo(o2.getName());
            }
           });
           // add this place to the location of this sublog
           split_sublog.location.add(bestLoc.getFirst());
         }
           
         System.out.println("at "+split_sublog.location.toString());
         aligned.add(split_sublog);
       }
     }
     
     return aligned;
   }

   public List<List<MoveOnLog>> getLoopHypotheses(SubLog sublog) {
     List<List<MoveOnLog>> hypotheses = new LinkedList<List<MoveOnLog>>();
     
     Set<Place> location = sublog.location;
     for (List<MoveOnLog> trace : sublog.traces) {
       String lastEvent = trace.get(trace.size()-1).event;
       // find the transition corresponding to the last event of the trace
       // TODO: incorporate mapping between log events and net transitions
       Transition matching_trans = net.findTransition(lastEvent);
       if (matching_trans != null) {
         // check if the last event of the trace corresponds to a transition
         // that produces on the location of the trace, if so, then it could
         // be that the trace represents a loop that ends at this location
         int overlapping_places = 0;
         for (Place p : matching_trans.getPostSet()) {
           if (location.contains(p)) {
             overlapping_places++;
           }
         }
         
         if (overlapping_places > 0) {
           hypotheses.add(trace);
         }
       }
     }
     
     return hypotheses;
   }
   
   /**
    * Find the body of a loop that contains all transitions related to log moves
    * in the given loop hypotheses. The loop body consists of all transitions given
    * in the log moves, their pre- and post-places, and all nodes of the net that
    * lie on a path between any two of the given transitions. 
    * 
    * @param loopHypothesis
    * @return the set of nodes of the loop body
    */
   public Set<Node> getLoopBody(List<List<MoveOnLog>> loopHypothesis) {
     Set<String> transitions = new HashSet<String>();
     for (List<MoveOnLog> trace : loopHypothesis) {
       for (MoveOnLog move : trace) {
         transitions.add(move.event);
       }
     }
     
     Set<Node> subNetNodes = new HashSet<Node>();
     for (String transition : transitions) {
       Transition t = net.findTransition(transition);
       if (t != null) {
         subNetNodes.add(t);
         subNetNodes.addAll(t.getPreSet());
         subNetNodes.addAll(t.getPostSet());
       }
     }
     
     Set<Node> backwards_reach = new HashSet<Node>(subNetNodes);
     LinkedList<Node> queue = new LinkedList<Node>(subNetNodes); 
     while (!queue.isEmpty()) {
       Node n = queue.removeFirst();
       for (Node m : n.getPreSet()) {
         if (!backwards_reach.contains(m)) {
           backwards_reach.add(m);
           queue.addLast(m);
         }
       }
     }
     
     Set<Node> forward_reach = new HashSet<Node>(subNetNodes);
     queue = new LinkedList<Node>(subNetNodes); 
     while (!queue.isEmpty()) {
       Node n = queue.removeFirst();
       for (Node m : n.getPostSet()) {
         if (!forward_reach.contains(m)) {
           forward_reach.add(m);
           queue.addLast(m);
         }
       }
     }
     
     for (Node n : backwards_reach) {
       if (forward_reach.contains(n)) subNetNodes.add(n);
     }
     
     return subNetNodes;
   }
   
   /**
    * @param loopBody
    * @return the nodes of the loop body that have no predecessor in the loop body
    */
   public List<Place> getLoopEntry(Set<Node> loopBody) {
     List<Place> entry = new LinkedList<Place>();
     for (Node n : loopBody) {
       if (!(n instanceof Place)) continue;
       
       int pre_in_body = 0;
       for (Node m : n.getPreSet()) if (loopBody.contains(m)) pre_in_body++;
       if (pre_in_body == 0) entry.add((Place)n);
     }
     return entry;
   }
   
   /**
    * @param loopBody
    * @return the nodes of the loop body that have no successor in the loop body
    */
   public List<Place> getLoopExit(Set<Node> loopBody) {
     List<Place> exit = new LinkedList<Place>();
     for (Node n : loopBody) {
       if (!(n instanceof Place)) continue;
       
       int post_in_body = 0;
       for (Node m : n.getPostSet()) if (loopBody.contains(m)) post_in_body++;
       if (post_in_body == 0) exit.add((Place)n);
     }
     return exit;
   }
   
   public static class PetriNetWithMarkings extends PetriNet {
     public Set<Place> initialMarking = new HashSet<Place>();
     public Set<Place> finalMarking = new HashSet<Place>();
   }
   
   public PetriNetWithMarkings getLoop(Set<Node> loopBody, List<Place> loopEntry, List<Place> loopExit) {
     
     PetriNetWithMarkings loop = new PetriNetWithMarkings();
     
     HashMap<Node, Node> copied = new HashMap<Node, Node>();
     for (Place p : net.getPlaces()) {
       if (!loopBody.contains(p)) continue;
       Place p2 = loop.addPlace(p.getName());
       copied.put(p, p2);
       
       if (p.getTokens() > 0) {
         p2.setTokens(p.getTokens());
         loop.initialMarking.add(p2);
       }
     }
     
     for (Transition t : net.getTransitions()) {
       if (!loopBody.contains(t)) continue;
       Transition t2 = loop.addTransition(t.getName());
       copied.put(t, t2);
       t2.setTau(t.isTau());
     }
     
     for (Arc a : net.getArcs()) {
       if (loopBody.contains(a.getSource()) && loopBody.contains(a.getTarget())) {
         if (a.getSource() instanceof Place) {
           loop.addArc((Place)copied.get(a.getSource()), (Transition)copied.get(a.getTarget()));
         } else {
           loop.addArc((Transition)copied.get(a.getSource()), (Place)copied.get(a.getTarget()));
         }
       }
     }
     
     Transition t_loop = loop.addTransition("SILENT - LOOP");
     for (Place p : loopExit) {
       loop.addArc((Place)copied.get(p), t_loop);
       loop.finalMarking.add((Place)copied.get(p));
     }
     for (Place p : loopEntry) {
       loop.addArc(t_loop, (Place)copied.get(p));
       loop.initialMarking.add((Place)copied.get(p));
     }
     
     return loop;
   }
   
   /**
   * @param alph1
   * @param alph2
   * @return number of events in which both alphabets differ
   */
   private static int alphabetOverlap(Set<String> alph1, Set<String> alph2) {
     int overlap = 0;
     for (String a : alph1) if (alph2.contains(a)) overlap++;
     return overlap;
   }

   
   /**
   * @param alph1
   * @param alph2
   * @return number of events in which both alphabets differ
   */
   private static int alphabetDistance(Set<String> alph1, Set<String> alph2) {
     int a1_not_in_2 = 0;
     int a2_not_in_1 = 0;
     for (String a : alph1) if (!alph2.contains(a)) a1_not_in_2++;
     for (String a : alph2) if (!alph1.contains(a)) a2_not_in_1++;
     
     return a1_not_in_2+a2_not_in_1;
   }

   /**
    * @param alph1
    * @param alph2
    * @return {@code true} iff each alphabet is at least as large as the distance between both alphabets
    */
   private static boolean sameCluster(Set<String> alph1, Set<String> alph2) {
     int dist = alphabetDistance(alph1, alph2);
     //System.out.println(dist+" = "+ alph1+" - "+alph2);
     return (alph1.size() >= dist && alph2.size() >= dist);
   }
   
   private static Set<String> getExtendedAlphabet(Set<String> alph, List<MoveOnLog> trace) {
     Set<String> stillToSee = new HashSet<String>(alph);
     Set<String> eventsSeen = new HashSet<String>();
     
     boolean firstEventSeen = false;
     
     for (MoveOnLog move : trace) {
       String event = move.event;
       if (stillToSee.contains(event)) firstEventSeen = true;
       if (firstEventSeen) {
         stillToSee.remove(event);
         eventsSeen.add(event);
       }
       if (stillToSee.isEmpty()) return eventsSeen;
     }
     System.out.println("still to see: "+stillToSee);
     return null;
   }

   /**
    * @param d
    * @return for event d of the unfolding the system transitions that were involved in its creation 
    */
   private List<Transition> getOriginalTransitions(DNode d) {
     DNodeSys_PetriNet sysModel = (DNodeSys_PetriNet)this.build.getSystem();
     List<Transition> d_origin = new LinkedList<Transition>();
     // for events, the causedBy field contains the ids of fireable transitions involved in the event
     for (int e_index : d.causedBy) {
       DNode e = build.getSystem().getTransitionForID(e_index);
       Node t = sysModel.getOriginalNode(e);
       d_origin.add((Transition)t);
     }
     return d_origin;
   }
   
   private List<Place> getOriginalPlaces(DNode d) {
     DNodeSys_PetriNet sysModel = (DNodeSys_PetriNet)this.build.getSystem();
     List<Place> d_origin = new LinkedList<Place>();
     // for conditions, the involved places were recorded explicitly during unfolding
     for (DNode b : this.conditionOrigin.get(d)) {
       Node p = sysModel.getOriginalNode(b);
       d_origin.add((Place)p);
     }
     return d_origin;
   }
   
   private Map<Node, Integer> pn_nodeOccurrences;

   /**
   * @return a map that tells for each node of the original system model that
   *         occurred at least once in the unfolding, the number of occurrences
   *         of this node (across all conditions/events that represent this
   *         node)
   */
   private Map<Node, Integer> computeNodeOccurrences() {

     pn_nodeOccurrences = new HashMap<Node, Integer>();
     
     // sum up the occurrences of the nodes in the unfolding by their original node 
     for (Map.Entry<DNode, Integer> occ : this.nodeOccurrences.entrySet()) {
       // for each node d, identify the set of nodes that caused 'd' to exist in the unfolding
       DNode d = occ.getKey();
       
       List<? extends Node> d_origin;
       if (d.isEvent) d_origin = getOriginalTransitions(d);
       else d_origin = getOriginalPlaces(d);
       
       // add the number of occurrences of 'd' to the original nodes
       for (Node n : d_origin) {
         if (!pn_nodeOccurrences.containsKey(n)) pn_nodeOccurrences.put(n, 0);
         pn_nodeOccurrences.put(n, pn_nodeOccurrences.get(n)+occ.getValue());
       }
     }
     
     return pn_nodeOccurrences;
   }

   /**
    * @param net
    * @param minOccurrences
    * @return the nodes of {@code net} that did not more than {@code minOccurrences} time
    */
   public Set<Node> getInfrequentNodes(int minOccurrences) {
     HashSet<Node> infrequent = new HashSet<Node>();
     
     for (Place p : net.getPlaces()) {
       if (!pn_nodeOccurrences.containsKey(p) || pn_nodeOccurrences.get(p) <= minOccurrences) {
         infrequent.add(p);
       }
     }
     for (Transition t : net.getTransitions()) {
       if (!pn_nodeOccurrences.containsKey(t) || pn_nodeOccurrences.get(t) <= minOccurrences) {
         infrequent.add(t);
       }
     }     
     return infrequent;
   }

   public void extendModelWithMoveOnModel() {
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
       
       // count how often t has to be skipped
       int t_tau_occurrences = 0;
       for (List<Transition> seq : moveOnModelSequences) {
         for (Transition t2 : seq) if (t == t2) t_tau_occurrences++;
       }
       
       boolean parallel_silent_exists = false;
       for (Transition t_parallel : net.getTransitions()) {
         if (t_parallel == t) continue;
         if (!t_parallel.isTau()) continue;
         
         if (t.getName().equals("01_HOOFD_065_2")) {
           System.out.println(t);
         }
        
         if (t_parallel.getIncoming().size() != t.getIncoming().size()) continue;
         if (t_parallel.getOutgoing().size() != t.getOutgoing().size()) continue;
         
         parallel_silent_exists = true;
         
         for (Place p : t_parallel.getPreSet()) {
           if (!t.getPreSet().contains(p)) {
             parallel_silent_exists = false; 
             break;
           }
         }
         for (Place p : t_parallel.getPostSet()) {
           if (!t.getPostSet().contains(p)) {
             parallel_silent_exists = false;
             break;
           }
         }
         
         if (parallel_silent_exists) break;
       }
       if (parallel_silent_exists) continue;
       
       Transition t_tau = net.addTransition("SILENT "+t.getName());
       System.out.println("adding "+t_tau);
       for (Place p : t.getPreSet()) {
         net.addArc(p, t_tau);
       }
       for (Place p : t.getPostSet()) {
         net.addArc(t_tau, p);
       }
       System.out.println("tau for "+t+" occurred "+t_tau_occurrences+" times");
       pn_nodeOccurrences.put(t_tau, t_tau_occurrences);
       if (pn_nodeOccurrences.containsKey(t)) {
         pn_nodeOccurrences.put(t, pn_nodeOccurrences.get(t)-t_tau_occurrences);
       }
     }
   }
   
   /**
    * Extend Petri net with a subNet that can replay the given subLog add the given
    * location. The subNet has well-defined start and end transitions that consume from
    * and produce on the location, respectively.
    * 
    * The method sets {@link #pn_nodeOccurrences} for all new transitions and places.
    * 
    * @param subNet
    * @param start
    * @param end
    * @param subLog
    * @param location
    * @param subName
    * @param weight
    */
   public void extendModelWithMoveOnLog(PetriNet subNet, Transition start, Transition end, 
       Collection<List<String>> subLog,
       Collection<Place> location,
       String subName,
       int weight,
       boolean isIsolatedExtension)
   {
     
     boolean inSequence = true;
     for (Place p : location) {
       if (!pn_nodeOccurrences.containsKey(p)) {
         inSequence = false;
         break;
       }
       if (pn_nodeOccurrences.get(p) < weight) {
         inSequence = false;
         break;
       }
     }
     
     List<hub.top.petrinet.Arc> toRemove = new ArrayList<hub.top.petrinet.Arc>();
     toRemove.addAll(start.getIncoming());
     toRemove.addAll(end.getOutgoing());
     for (hub.top.petrinet.Arc a : toRemove) {
       subNet.removeArc(a);
     }
     
     List<Place> isolated = new ArrayList<Place>();
     for (Place p : subNet.getPlaces()) {
       if (p.getIncoming().size() == 0 && p.getOutgoing().size() == 0) {
         isolated.add(p);
       }
     }
     for (Place p : isolated) {
       subNet.removePlace(p);
     }
     
     Map<Place,Place> movedPlaces = new HashMap<Place, Place>();
     Map<Transition, Transition> movedTransitions = new HashMap<Transition, Transition>();
     
     net.addRole(subName);
     for (Place p : subNet.getPlaces()) {
       Place _p = net.addPlace(p.getName());
       _p.addRole(subName);
       net.setTokens(_p, p.getTokens());
       movedPlaces.put(p, _p);
     }
     for (Transition t : subNet.getTransitions()) {
       
       int _t_occurrence_count = 0;
       for (List<String> tr : subLog) {
         for (String e : tr) if (e.equals(t.getName())) _t_occurrence_count++;
       }
       
       Transition _t = net.addTransition(t.getName());
       _t.addRole(subName);
       movedTransitions.put(t, _t);
       pn_nodeOccurrences.put(_t, _t_occurrence_count);
       
       for (Place p : t.getPreSet()) {
         Place _p = movedPlaces.get(p);
         net.addArc(_p, _t);
       }
       for (Place p : t.getPostSet()) {
         Place _p = movedPlaces.get(p);
         net.addArc(_t, _p);
         
         if (!pn_nodeOccurrences.containsKey(_p)) pn_nodeOccurrences.put(_p, 0);
         pn_nodeOccurrences.put(_p, pn_nodeOccurrences.get(_p) + _t_occurrence_count);
       }
     }
     
     boolean singlePlaceLocation = location.size() == 1 && start.getPostSet().size() == 1 && end.getPreSet().size() == 1;
     
     if (inSequence && (isIsolatedExtension/* || singlePlaceLocation*/))
     {
       Map<Place, Place> preMap = new HashMap<Place, Place>();
       Map<Place, Place> postMap = new HashMap<Place, Place>();
       
       boolean isParallelExtension = !isIsolatedExtension;
       
       for (Place p_loc : location) {
         /*
         if (isParallelExtension) {
           Place p_pre = net.addPlace(p_loc.getName()+"_pre_"+subName);
           preMap.put(p_loc, p_pre);
           for (Transition t : p_loc.getPreSet()) {
             net.addArc(t, p_pre);
           }
           
           Place p_post = net.addPlace(p_loc.getName()+"_post_"+subName);
           postMap.put(p_loc, p_post);
           for (Transition t : p_loc.getPostSet()) {
             net.addArc(p_post, t);
           }
         } else
         */
         {
           preMap.put(p_loc, p_loc);
           
           Place p_post = net.addPlace(p_loc.getName()+"_post");
           postMap.put(p_loc, p_post);
           for (Transition t : p_loc.getPostSet()) {
             net.addArc(p_post, t);
           }
         }
         if (pn_nodeOccurrences.containsKey(p_loc) && preMap.get(p_loc) != p_loc) pn_nodeOccurrences.put(preMap.get(p_loc), pn_nodeOccurrences.get(p_loc));
         if (pn_nodeOccurrences.containsKey(p_loc) && postMap.get(p_loc) != p_loc) pn_nodeOccurrences.put(postMap.get(p_loc), pn_nodeOccurrences.get(p_loc));
       }
       
       
       //if (!isParallelExtension)
       {
         List<Arc> arcsToRemove = new LinkedList<Arc>();
         for (Place p_loc : location) {
           for (Arc a : p_loc.getOutgoing()) arcsToRemove.add(a);
         }
         for (Arc a : arcsToRemove) net.removeArc(a);
       }
       

       boolean start_in_loop = false;
       for (Place p : start.getPostSet()) {
         if (p.getPreSet().size() > 1) { start_in_loop = true; break; }
       }
       boolean end_in_loop = false;
       for (Place p : end.getPreSet()) {
         if (p.getPreSet().size() > 1) { end_in_loop = true; break; }
       }

       if (singlePlaceLocation && !start_in_loop && !end_in_loop)
       {
         Place p_loc = location.iterator().next();
         Place p_start = movedTransitions.get(start).getPostSet().get(0);
         Place p_end = movedTransitions.get(end).getPreSet().get(0);
         
         for (Transition t_start : p_start.getPostSet()) {
           net.addArc(preMap.get(p_loc), t_start);
         }
         for (Transition t_end : p_end.getPreSet()) {
           net.addArc(t_end, postMap.get(p_loc));
         }
         if (p_start != p_end)
           net.removePlace(p_start);
         net.removePlace(p_end);
         net.removeTransition(movedTransitions.get(start));
         net.removeTransition(movedTransitions.get(end));
         pn_nodeOccurrences.remove(p_start);
         pn_nodeOccurrences.remove(p_end);
         pn_nodeOccurrences.remove(movedTransitions.get(start));
         pn_nodeOccurrences.remove(movedTransitions.get(end));
         
       } else {
         
         for (Place p_loc : location) {
           net.addArc(preMap.get(p_loc), movedTransitions.get(start));
           net.addArc(movedTransitions.get(end), postMap.get(p_loc));
         }
       }
       
       Transition t_skip = net.addTransition("SILENT - SKIP "+subName);
       t_skip.setTau(true);
       for (Place p : location) {
         net.addArc(preMap.get(p), t_skip);
         net.addArc(t_skip, postMap.get(p));
       }
       
     } else {
       // add as looping sub-process
       
       if (singlePlaceLocation) {
         Place p_loc = location.iterator().next();
         Place p_start = movedTransitions.get(start).getPostSet().get(0);
         Place p_end = movedTransitions.get(end).getPreSet().get(0);
         
         for (Transition t_start : p_start.getPostSet()) {
           net.addArc(p_loc, t_start);
         }
         for (Transition t_end : p_end.getPreSet()) {
           net.addArc(t_end, p_loc);
         }
         if (p_start != p_end)
           net.removePlace(p_start);
         net.removePlace(p_end);
         net.removeTransition(movedTransitions.get(start));
         net.removeTransition(movedTransitions.get(end));
         pn_nodeOccurrences.remove(p_start);
         pn_nodeOccurrences.remove(p_end);
         pn_nodeOccurrences.remove(movedTransitions.get(start));
         pn_nodeOccurrences.remove(movedTransitions.get(end));
         
       } else {
         for (Place p : location) {
           net.addArc(p, movedTransitions.get(start));
           net.addArc(movedTransitions.get(end), p);
         }
       }
     }
   }
   
   public void removeInfrequentNodes(int occurrence_threshold) {
     Set<Node> unusedNodes = getInfrequentNodes(occurrence_threshold);
     List<Place> p_toRemove = new ArrayList<Place>();
     for (Place p : net.getPlaces()) {
       if (unusedNodes.contains(p)) p_toRemove.add(p);
     }
     List<Transition> t_toRemove = new ArrayList<Transition>();
     for (Transition t : net.getTransitions()) {
       if (unusedNodes.contains(t)) t_toRemove.add(t);
     }
     for (Place p : p_toRemove) {
       System.out.println("removing "+p);
       net.removePlace(p);
     }
     for (Transition t : t_toRemove) {
       System.out.println("removing "+t);
       net.removeTransition(t);
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

   
   public void replayAlignment(String systemFile, Collection<Move[]> traces, Map<String, String> e2t) throws InvalidModelException, IOException {

     for (DNode b : build.getBranchingProcess().allConditions) {
       conditionOrigin.put(b, new DNode[] { b });
     }
     
     extendByTraces(traces, e2t);
     computeNodeOccurrences();
     
     OcletIO_Out.writeFile(build.toDot(), systemFile+".unfolding.dot");
   }
   
   public void repair(String systemFile, Collection<Move[]> traces, Map<String, String> e2t) throws InvalidModelException, IOException {
     replayAlignment(systemFile, traces, e2t);
     
     extendModelWithMoveOnModel();
     //extendModelWithMoveOnLog(getLocationsForExtension());
     
     OcletIO_Out.writeToFile((PetriNet)net, systemFile+"_repaired.lola", OcletIO_Out.FORMAT_LOLA, 0);
     OcletIO_Out.writeToFile((PetriNet)net, systemFile+"_repaired.dot", OcletIO_Out.FORMAT_DOT, 0);

   }
   
   public static void main(String[] args) {
     
       //String systemFile = "./examples/model_correction2/a22f0n00.lola";
       //String traceFile  = "./examples/model_correction2/a22f0n00.log_20_br.txt";
     
       //String systemFile = "./examples/model_correction/a22f0n00.lola";
       //String traceFile  = "./examples/model_correction/a22f0n05.log_100.txt";
       
       //String systemFile = "./examples/model_correction/a12f0n05_alpha.lola";
       //String traceFile  = "./examples/model_correction/a12f0n05_aligned.log.txt";
       
       //String systemFile = "./examples/model_correction2/wabo1.lola";
       //String traceFile  = "./examples/model_correction2/wabo1.log.txt";
     
       String systemFile = "./examples/model_correction2/wabo_base.lola";
       String traceFile  = "./examples/model_correction2/wabo2.log.txt";

       
       try {
         
         ISystemModel sysModel = Uma.readSystemFromFile(systemFile);
         LinkedList<String[]> traces = readTraces(traceFile);
         LinkedList<Move[]> alignments = new LinkedList<ModelRepair_SubProcess.Move[]>();
         for (String[] trace : traces) {
           Move[] alignment = new Move[trace.length];
           for (int i=0; i<trace.length; i++) {
             alignment[i] = new Move(trace[i]);
           }
         }

         DNodeRefold build = getInitialUnfolding(sysModel);
         ModelRepair_SubProcess viewGen = new ModelRepair_SubProcess((PetriNet) sysModel, build);
         viewGen.replayAlignment(systemFile, alignments, null);
         
       } catch (IOException e) {
         System.err.println(e);
       } catch (InvalidModelException e) {
         System.err.println(e);
       }
     
   }
}
