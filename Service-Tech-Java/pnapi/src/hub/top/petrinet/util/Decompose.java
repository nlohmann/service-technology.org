/*****************************************************************************\
 * Copyright (c) 2010. All rights reserved. Dirk Fahland. AGPL3.0
 * 
 * ServiceTechnology.org - Petri Net API/Java
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

package hub.top.petrinet.util;

import hub.top.petrinet.PetriNet;
import hub.top.petrinet.PetriNetIO;
import hub.top.petrinet.Place;
import hub.top.petrinet.Transition;

import java.io.File;
import java.io.IOException;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedList;

public class Decompose {

  public static LinkedList<PetriNet> toSwimlanes(PetriNet net) {
    LinkedList<PetriNet> swimlane_nets = new LinkedList<PetriNet>();
    
    HashSet< HashSet<String> > swimlanes = new HashSet< HashSet<String> >();
    for (Transition t : net.getTransitions()) {
      swimlanes.add(t.getRoles());
    }
    
    // for each swimlaine
    for (HashSet<String> lane : swimlanes) {
      //System.out.println(PetriNet.toString(lane));
      
      // create a new Petri net
      PetriNet lane_net = new PetriNet();
      for (Transition t : net.getTransitions()) {
        // consisting of all transitions that belong to this swimlane
        if (lane.equals(t.getRoles())) {
          Transition t_ = lane_net.addTransition(t.getName());
          for (String role : t.getRoles()) t_.addRole(role);
          
          // together with their pre-sets
          for (Place p : t.getPreSet()) {
            // see if place already exists in the net
            Place p_ = lane_net.findPlace(p.getName());
            if (p_ == null) // if not: create it
              p_ = lane_net.addPlace(p.getName());
            lane_net.addArc(p_, t_);
          }

          // and post-sets
          for (Place p : t.getPostSet()) {
            // see if place already exists in the net
            Place p_ = lane_net.findPlace(p.getName());
            if (p_ == null) // if not: create it
              p_ = lane_net.addPlace(p.getName());
            lane_net.addArc(t_, p_);
          }
        }
      }
      swimlane_nets.add(lane_net);
    }
    
    return swimlane_nets;
  }
  
  public static PetriNet compose(PetriNet net1, PetriNet net2) {
    PetriNet result = new PetriNet();
    
    for (Transition t : net1.getTransitions()) {
      Transition t_ = result.addTransition(t.getName());
      for (String role : t.getRoles()) t_.addRole(role);
      
      // together with their pre-sets
      for (Place p : t.getPreSet()) {
        // see if place already exists in the net
        Place p_ = result.findPlace(p.getName());
        if (p_ == null) // if not: create it
          p_ = result.addPlace(p.getName());
        result.addArc(p_, t_);
      }

      // and post-sets
      for (Place p : t.getPostSet()) {
        // see if place already exists in the net
        Place p_ = result.findPlace(p.getName());
        if (p_ == null) // if not: create it
          p_ = result.addPlace(p.getName());
        result.addArc(t_, p_);
      }
    }
    
    for (Transition t : net2.getTransitions()) {
      Transition t_ = result.findTransition(t.getName());
      if (t_ == null)
        t_ = result.addTransition(t.getName());
      for (String role : t.getRoles()) t_.addRole(role);
      
      // together with their pre-sets
      for (Place p : t.getPreSet()) {
        // see if place already exists in the net
        Place p_ = result.findPlace(p.getName());
        if (p_ == null) // if not: create it
          p_ = result.addPlace(p.getName());
        result.addArc(p_, t_);
      }

      // and post-sets
      for (Place p : t.getPostSet()) {
        // see if place already exists in the net
        Place p_ = result.findPlace(p.getName());
        if (p_ == null) // if not: create it
          p_ = result.addPlace(p.getName());
        result.addArc(t_, p_);
      }
    }
    
    return result;
  }
  
  public static LinkedList<PetriNet> toScenarios(PetriNet net) {
    
    LinkedList<PetriNet> result = new LinkedList<PetriNet>();
    HashMap<Transition, PetriNet> pieces = new HashMap<Transition, PetriNet>();
    
    for (Transition t : net.getTransitions()) {
      PetriNet piece = new PetriNet();
      Transition t_ = piece.addTransition(t.getName());
      for (String role : t.getRoles()) t_.addRole(role);
      
      // together with their pre-sets
      for (Place p : t.getPreSet()) {
        // see if place already exists in the net
        Place p_ = piece.findPlace(p.getName());
        if (p_ == null) // if not: create it
          p_ = piece.addPlace(p.getName());
        piece.addArc(p_, t_);
      }

      // and post-sets
      for (Place p : t.getPostSet()) {
        // see if place already exists in the net
        Place p_ = piece.findPlace(p.getName());
        if (p_ == null) // if not: create it
          p_ = piece.addPlace(p.getName());
        piece.addArc(t_, p_);
      }
      pieces.put(t, piece);
    }
    
    HashSet<Transition> left = new HashSet<Transition>(pieces.keySet());
    while (!left.isEmpty()) {
      Transition t = left.iterator().next();  // get some transition
      PetriNet intermediate = new PetriNet();
      
      System.out.println("from "+t.getName());
      
      LinkedList<Transition> queue = new LinkedList<Transition>();
      queue.addLast(t);
      while (!queue.isEmpty()) {
        Transition s = queue.removeFirst();
        intermediate = compose(intermediate, pieces.get(s));
        left.remove(s);
        
        for (Place p : s.getPreSet()) {
          for (Transition r : p.getPreSet()) {
            if (r == s) continue;
            if (!left.contains(r)) continue;
            if (queue.contains(r)) break;
            queue.addLast(r);
            System.out.println("adding "+r.getName());
            break;
          }
        }
        /*
        for (Place p : s.getPostSet()) {
          for (Transition r : p.getPostSet()) {
            if (r == s) continue;
            if (!left.contains(r)) continue;
            if (queue.contains(r)) break;
            queue.addLast(r);
            System.out.println("adding "+r.getName());
            break;
          }
        }*/
      }
      
      result.add(intermediate);
    }
    return result;
  }
  
  public static PetriNet toScenarios2(PetriNet net) {
    // create a new Petri net
    PetriNet scenario_net = new PetriNet();
    for (Transition t : net.getTransitions()) {
        // consisting of all transitions that belong to this swimlane
    
      Transition t_ = scenario_net.addTransition(t.getName());
      
      // together with their pre-sets
      for (Place p : t.getPreSet()) {
        // see if place already exists in the net
        Place p_ = scenario_net.findPlace(p.getName());
        if (p_ == null) // if not: create it
          p_ = scenario_net.addPlace(p.getName());
        scenario_net.addArc(p_, t_);
      }
    
      // and post-sets
      for (Place p : t.getPostSet()) {
        // see if place already exists in the net
        Place p_ = scenario_net.findPlace(p.getName());
        if (p_ == null) // if not: create it
          p_ = scenario_net.addPlace(p.getName());
        scenario_net.addArc(t_, p_);
      }
    }

    int scenarioNum = 0;

    HashSet<Transition> left = new HashSet<Transition>(scenario_net.getTransitions());
    while (!left.isEmpty()) {
      Transition t = left.iterator().next();  // get some transition
      scenarioNum++;
      
      LinkedList<Transition> queue = new LinkedList<Transition>();
      queue.addLast(t);
      while (!queue.isEmpty()) {
        Transition s = queue.removeFirst();
        s.addRole(Integer.toString(scenarioNum));
        left.remove(s);
        
        for (Place p : s.getPreSet()) {
          if (p.getPreSet().size() == 1 && p.getPostSet().size() == 1) {
            Transition r = p.getPreSet().get(0);
            if (r == s) continue;
            if (!left.contains(r)) continue;
            if (queue.contains(r)) continue;
            queue.addLast(r);
          }
        }
        for (Place p : s.getPostSet()) {
          if (p.getPostSet().size() == 1 && p.getPreSet().size() == 1) {
            Transition r = p.getPostSet().get(0);
            if (r == s) continue;
            if (!left.contains(r)) continue;
            if (queue.contains(r)) continue;
            queue.addLast(r);
          }
        }
      }
    }
    
    return scenario_net;
  }
  
  public static PetriNet toScenarios3(PetriNet net) {
    HashMap<Transition, Integer> sa = getScenarioAssignment_disjoint(net);
    return materializeScenarios_roles(net, sa);
  }
  
  /**
   * Compute an assignment of the net's transitions into disjoint scenarios.
   * @param net
   * @return
   */
  public static HashMap<Transition, Integer> getScenarioAssignment_disjoint(PetriNet net) {
    
    // set of transitions that are not assigned yet to some scenario
    HashSet<Transition> left_globally = new HashSet<Transition>(net.getTransitions());

    // map that stores for each transition which other transitions must not be in the
    // same scenario as this transition (at least), because these transitions consume from
    // or produce on the same places
    HashMap<Transition, HashSet<Transition> > toSplit = new HashMap<Transition, HashSet<Transition>>();
    for (Transition t : net.getTransitions()) {
      // init map for each transition
      toSplit.put(t, new HashSet<Transition>());
    }

    // then add for each transition 't' its conflicting transitions 't2'
    for (Place p : net.getPlaces()) {
      for (Transition t : p.getPreSet()) {
        for (Transition t2 : p.getPreSet()) {
          toSplit.get(t).add(t2);
        }
      }
      for (Transition t : p.getPostSet()) {
        for (Transition t2 : p.getPostSet()) {
          toSplit.get(t).add(t2);
        }
      }
    }

    // assign each transition of the net to one scenario
    HashMap<Transition, Integer> scenarioAssignment = new HashMap<Transition, Integer>();
    int scenarioNum = 0;
    // as long as there is an unassigned transition
    while (!left_globally.isEmpty()) {
      Transition t = left_globally.iterator().next();  // get some transition
      scenarioNum++;      // put it in a new scenario
      
      // and augment this new scenario with other compatible
      // transitions by simultaneous forward and backward
      // breadth-first search
      HashSet<Transition> inCurrentScenario = new HashSet<Transition>();
      LinkedList<Transition> queue = new LinkedList<Transition>();
      queue.addLast(t);
      // as long as there is some reachable compatible transition
      while (!queue.isEmpty()) {
        // take it and add it to the current scenario
        Transition s = queue.removeFirst();     
        scenarioAssignment.put(s, scenarioNum);
        left_globally.remove(s);
        inCurrentScenario.add(s);
        
        // then explore all direct predecessor transitions
        for (Place p : s.getPreSet()) {
          if (p.getPreSet().size() == 1 && p.getPostSet().size() == 1) {
            // only a place with one pre- and one post-transition can be
            // within the same scenario as transition s
            Transition r = p.getPreSet().get(0);
            // skip the current transition s
            if (r == s) continue;
            // skip transitions that have already been considered
            if (!left_globally.contains(r)) continue;
            if (queue.contains(r)) continue;
            // and stop exploration when reaching a conflicting transition
            boolean wouldConflict = false;
            for (Transition conflicting : toSplit.get(r)) {
              if (inCurrentScenario.contains(conflicting)) {
                wouldConflict = true; break;
              }
            }
            if (wouldConflict) continue;
            // breadth-first search
            queue.addLast(r);
          }
        }
        for (Place p : s.getPostSet()) {
          if (p.getPostSet().size() == 1 && p.getPreSet().size() == 1) {
            // only a place with one pre- and one post-transition can be
            // within the same scenario as transition s
            Transition r = p.getPostSet().get(0);
            // skip the current transition s
            if (r == s) continue;
            // skip transitions that have already been considered
            if (!left_globally.contains(r)) continue;
            if (queue.contains(r)) continue;
            // and stop exploration when reaching a conflicting transition
            boolean wouldConflict = false;
            for (Transition conflicting : toSplit.get(r)) {
              if (inCurrentScenario.contains(conflicting)) {
                wouldConflict = true; break;
              }
            }
            if (wouldConflict) continue;
            // breadth-first search            
            queue.addLast(r);
          }
        }
      } // end of breadth-first search from unassigned transition
    } // until all transitions are assigned

    return scenarioAssignment;
  }
  
  /**
   * Map the given scenario assignment into roles of the net.
   *  
   * @param net
   * @param scenarioAssignment
   * @return a new Petri net
   */
  public static PetriNet materializeScenarios_roles(PetriNet net, HashMap<Transition, Integer> scenarioAssignment) {
    // materialize scenarios
    PetriNet scenario_net = new PetriNet(net);  // make a copy of the net
    // write roles to the net
    for (Transition t : scenario_net.getTransitions()) {
      Transition tOld = net.findTransition(t.getName());
      String sRole = Integer.toString(scenarioAssignment.get(tOld));
      t.addRole(sRole);
    }
    // assign roles to places
    scenario_net.spreadRolesToPlaces_union();
    
    return scenario_net;    
  }
  
  /**
   * Decompose the given Petri net into components according to the scenario
   * assignment. Each connected component is a scenario.
   * 
   * @param net
   * @param scenarioAssignment
   * @return a new Petri net
   */
  public static PetriNet materializeScenarios_split(PetriNet net, HashMap<Transition, Integer> scenarioAssignment) {
    
    // materialize scenarios
    PetriNet scenario_net = new PetriNet();
    for (Transition t : net.getTransitions()) {
      
      String sRole = Integer.toString(scenarioAssignment.get(t));
      
      Transition t2 = scenario_net.addTransition(t.getName());
      t2.addRole(sRole);
      for (Place p : t.getPreSet()) {
        // see if place already exists in the net
        Place p2 = scenario_net.findPlace(p.getName());
        if (p2 == null || !p2.getRoles().contains(sRole)) {
          // does note exist yet or in the wrong scenario: create new place
          p2 = scenario_net.addPlace(p.getName());
          p2.addRole(sRole);  // put place into current scenario
        }
        scenario_net.addArc(p2, t2);
      }
      for (Place p : t.getPostSet()) {
        // see if place already exists in the net
        Place p2 = scenario_net.findPlace(p.getName());
        if (p2 == null || !p2.getRoles().contains(sRole)) {
          // does note exist yet or in the wrong scenario: create new place
          p2 = scenario_net.addPlace(p.getName());
          p2.addRole(sRole);  // put place into current scenario
        }
        scenario_net.addArc(t2, p2);
      }
    }
    
    // create initial state
    for (Place p : net.getPlaces()) {
      for (int i=0; i< p.getTokens(); i++) {
        Place p2 = scenario_net.addPlace(p.getName());
        p2.addRole("initial");
        p2.setTokens(1);
      }
    }
    
    scenario_net.spreadRolesToPlaces_union();
    return scenario_net;    
  }
  
  public static PetriNet decomposeToScenarios_split(PetriNet net) {
    HashMap<Transition, Integer> sa = getScenarioAssignment_disjoint(net);
    return materializeScenarios_split(net, sa);
  }
  
  public static void decomposeToScenarios2(String fileName) {

    try {
        
      System.out.println("decomposing: "+fileName);
      PetriNet net = PetriNetIO.readNetFromFile(fileName);
      System.out.println("size of net: "+net.getInfo());
      
      net.setRoles_unassigned();
      PetriNet scenarioNet = decomposeToScenarios_split(net);
      scenarioNet.anonymizeNet();
      PetriNetIO.writeToFile(scenarioNet, fileName+"_scenario", PetriNetIO.FORMAT_DOT, PetriNetIO.PARAM_SWIMLANE);
      
      /*
      LinkedList<PetriNet> scenarios = toScenarios(net);
      for (int i=0; i<scenarios.size(); i++) {
        System.out.println("size of net "+i+": "+scenarios.get(i).getInfo());
        PetriNetIO.writeToFile(scenarios.get(i), fileName+"_"+i, PetriNetIO.FORMAT_DOT);
      }
      */
      
    } catch (IOException e) {
      System.err.println(e);
      System.exit(1);
    }
  }
  
  public static void main(String args[]) {
    if (args.length == 0) return;
    
    if (args[0].endsWith("lola") || args[0].endsWith("owfn")) {
      // decompose single file
      decomposeToScenarios2(args[0]);
      
    } else {
      // decompose all files in the given directory
      File dir = new File(args[0]);
      if (dir.isDirectory()) {
        for (String child : dir.list()) {
          File childFile = new File(dir, child);
          if (childFile.isFile()) {
            if (childFile.getPath().endsWith("lola") || childFile.getPath().endsWith("owfn")) {
              decomposeToScenarios2(childFile.getPath());
            }
          }
        }
      }
    }
      
  }
}
