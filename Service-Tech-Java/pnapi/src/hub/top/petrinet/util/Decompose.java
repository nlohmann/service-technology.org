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
    
    HashSet<Transition> left_globally = new HashSet<Transition>(scenario_net.getTransitions());

    HashMap<Transition, HashSet<Transition> > toSplit = new HashMap<Transition, HashSet<Transition>>();
    for (Transition t : scenario_net.getTransitions()) {
      toSplit.put(t, new HashSet<Transition>());
    }

    for (Place p : scenario_net.getPlaces()) {
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
    
    HashMap<Transition, Integer> scenarioAssignment = new HashMap<Transition, Integer>();
    
    int scenarioNum = 0;

    while (!left_globally.isEmpty()) {
      Transition t = left_globally.iterator().next();  // get some transition
      scenarioNum++;
      
      HashSet<Transition> inCurrentScenario = new HashSet<Transition>();
      LinkedList<Transition> queue = new LinkedList<Transition>();
      queue.addLast(t);
      while (!queue.isEmpty()) {
        Transition s = queue.removeFirst();
        scenarioAssignment.put(s, scenarioNum);
        left_globally.remove(s);
        inCurrentScenario.add(s);
        
        for (Place p : s.getPreSet()) {
          if (p.getPreSet().size() == 1 && p.getPostSet().size() == 1) {
            Transition r = p.getPreSet().get(0);
            if (r == s) continue;
            if (!left_globally.contains(r)) continue;
            if (queue.contains(r)) continue;
            
            boolean wouldConflict = false;
            for (Transition conflicting : toSplit.get(r)) {
              if (inCurrentScenario.contains(conflicting)) {
                wouldConflict = true; break;
              }
            }
            if (wouldConflict) continue;
            
            queue.addLast(r);
          }
        }
        for (Place p : s.getPostSet()) {
          if (p.getPostSet().size() == 1 && p.getPreSet().size() == 1) {
            Transition r = p.getPostSet().get(0);
            if (r == s) continue;
            if (!left_globally.contains(r)) continue;
            if (queue.contains(r)) continue;
            
            boolean wouldConflict = false;
            for (Transition conflicting : toSplit.get(r)) {
              if (inCurrentScenario.contains(conflicting)) {
                wouldConflict = true; break;
              }
            }
            if (wouldConflict) continue;
            
            queue.addLast(r);
          }
        }
      }
    }

    // materialize scenarios
    PetriNet scenario_net2 = new PetriNet();
    for (Transition t : scenario_net.getTransitions()) {
      
      String sRole = Integer.toString(scenarioAssignment.get(t));
      
      Transition t2 = scenario_net2.addTransition(t.getName());
      t2.addRole(sRole);
      for (Place p : t.getPreSet()) {
        // see if place already exists in the net
        Place p2 = scenario_net2.findPlace(p.getName());
        if (p2 == null || !p2.getRoles().contains(sRole)) {
          // if not: create it
          p2 = scenario_net2.addPlace(p.getName());
          p2.addRole(sRole);
        }
        scenario_net2.addArc(p2, t2);
      }
      for (Place p : t.getPostSet()) {
        // see if place already exists in the net
        Place p2 = scenario_net2.findPlace(p.getName());
        if (p2 == null || !p2.getRoles().contains(sRole)) {
          // if not: create it
          p2 = scenario_net2.addPlace(p.getName());
          p2.addRole(sRole);
        }
        scenario_net2.addArc(t2, p2);
      }

    }
    
    return scenario_net2;
  }
  
  
  public static void decomposeToScenarios2(String fileName) {

    try {
        
      System.out.println("decomposing: "+fileName);
      PetriNet net = PetriNetIO.readNetFromFile(fileName);
      System.out.println("size of net: "+net.getInfo());
      
      net.setRoles_unassigned();
      PetriNet scenarioNet = toScenarios3(net);
      scenarioNet.spreadRolesToPlaces_union();
      PetriNetIO.writeToFile(scenarioNet, fileName+"_scenario", PetriNetIO.FORMAT_DOT);
      
      /*
      LinkedList<PetriNet> scenarios = toScenarios(net);
      for (int i=0; i<scenarios.size(); i++) {
        System.out.println("size of net "+i+": "+scenarios.get(i).getInfo());
        PetriNetIO.writeToFile(scenarios.get(i), args[0]+"_"+i, PetriNetIO.FORMAT_DOT);
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
