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
import hub.top.petrinet.ISystemModel;
import hub.top.petrinet.PetriNet;
import hub.top.petrinet.PetriNetIO;
import hub.top.petrinet.Place;
import hub.top.petrinet.Transition;
import hub.top.uma.DNode;
import hub.top.uma.DNodeBP;
import hub.top.uma.DNodeSys;
import hub.top.uma.InvalidModelException;
import hub.top.uma.Uma;
import hub.top.uma.synthesis.NetSynthesis;
import hub.top.uma.synthesis.TransitiveDependencies;

import java.io.BufferedWriter;
import java.io.FileNotFoundException;
import java.io.FileWriter;
import java.io.IOException;
import java.util.HashMap;
import java.util.LinkedList;

import com.google.gwt.dev.util.collect.HashSet;

/**
 * Simplify a mined process model using unfoldings and a number
 * of structural reductions and transformations. 
 * 
 * @author dfahland
 */
public class MineSimplify {

  private String fileName_system_sysPath; 
  private String fileName_trace;
  
  private ISystemModel sysModel;
  private LinkedList<String[]> allTraces;
  
  private PetriNet simplifiedNet;
  private PetriNet originalNet;
  
  private float netComplexity_original;
  private float netComplexity_simplified;
  
  /**
   * Read the systemModel and the traces to simplify from files.
   * 
   * @param fileName_system_sysPath
   * @param fileName_trace
   * @throws IOException
   * @throws InvalidModelException
   * @throws FileNotFoundException
   */
  public MineSimplify(String fileName_system_sysPath, String fileName_trace) 
    throws IOException, InvalidModelException, FileNotFoundException
  {
    this.fileName_system_sysPath = fileName_system_sysPath;
    this.fileName_trace = fileName_trace;
    
    sysModel = Uma.readSystemFromFile(fileName_system_sysPath);
    allTraces = ViewGeneration2.readTraces(fileName_trace);
  }
  
  public MineSimplify(ISystemModel sysModel, LinkedList<String[]> allTraces) 
  {
    this.sysModel = sysModel;
    this.allTraces = allTraces;
  }
  
  /**
   * Check the given model and transform it into a normal form if necessary.
   * For Petri nets, the model is normalized by ensuring that each transition
   * has a non-empty pre- and post-set. Additionally, the method attempts to
   * identify the labeling of a net if two different transitions are meant
   * to carry the same label (see {@link PetriNet#turnIntoLabeledNet()}.
   */
  public void prepareModel() {
    if (sysModel instanceof PetriNet) {
      originalNet = (PetriNet)sysModel;
      ((PetriNet)sysModel).makeNormalNet();
      ((PetriNet)sysModel).turnIntoLabeledNet();
    }
  }
  
  long _time_start;
  long _time_finish;
  
  
  public boolean run() throws InvalidModelException {
    
    _time_start = System.currentTimeMillis();
    
    simplifiedNet = simplifyModel(this, sysModel, allTraces, true);
    if (simplifiedNet == null)
      return false;
    
    if (sysModel instanceof PetriNet) {
      netComplexity_original = complexitySimple((PetriNet)sysModel);
    }
    _time_finish = System.currentTimeMillis();
    
    netComplexity_simplified = complexitySimple(simplifiedNet);
    Uma.out.println("finished in "+(_time_finish-_time_start)+"ms, complexity: "+netComplexity_simplified);
    
    
    return true;
  }
  
  public static float complexitySimple(PetriNet net) {
    int nodeNum = net.getPlaces().size() + net.getTransitions().size();
    int arcNum = net.getArcs().size();
    
    return (float)arcNum/(float)nodeNum;
  }
  
  public String generateResultsFile() {
    String result_string = "";
    
    
    if (originalNet != null)
      result_string += "original net: "+originalNet.getInfo()+" , complexity: "+netComplexity_original+"\n";
    else
      result_string += "original net: <unknown> , complexity:  <unknown>\n";
    result_string += "build BP(L): "+(_time_buildBP_finish - _time_buildBP_start)+" , BP: "+_size_bp+"\n";
    result_string += "folding relation: "+(_time_equiv_finish - _time_equiv_start)+" , net: "+_size_fold+" , complexity: "+_comp_fold+"\n";
    result_string += "implied places: "+(_time_implied_finish - _time_implied_start)+" , net: "+_size_implied+" , complexity: "+_comp_implied+"\n";
    result_string += "chains: "+(_time_chain_finish - _time_chain_start)+" , net: "+_size_chain+" , complexity: "+_comp_chain+"\n";
    result_string += "total time: "+(_time_finish - _time_start)+" , simplified net: "+simplifiedNet.getInfo()+" , complexity: "+netComplexity_simplified+"\n";
    result_string += "threshold: "+simplestThreshold;
    result_string += "\n";
    
    result_string += originalNet.getInfo()+";"+(int)(netComplexity_original*100)+";";
    result_string += _size_bp+";"+(_time_buildBP_finish - _time_buildBP_start)+";";
    result_string += _size_fold+";"+(int)(_comp_fold*100)+";"+(_time_equiv_finish - _time_equiv_start)+";";
    result_string += _size_implied+";"+(int)(_comp_implied*100)+";"+(_time_implied_finish - _time_implied_start)+";";
    result_string += _size_chain+";"+(int)(_comp_chain*100)+";"+(_time_chain_finish - _time_chain_start)+";";
    result_string += simplifiedNet.getInfo()+";"+(int)(netComplexity_simplified*100)+";"+(_time_chain_finish - _time_chain_start)+";";
    
    return result_string;
  }
  
  public PetriNet getSimplifiedNet() {
    return simplifiedNet;
  }
  
  public void writeResults() throws IOException {
    
    if (simplifiedNet == null)
      return;
    
    String targetPath_dot = fileName_system_sysPath+".simplified.dot";
    PetriNetIO.writeToFile(simplifiedNet, targetPath_dot, PetriNetIO.FORMAT_DOT, 0);
    
    String targetPath_lola = fileName_system_sysPath+".simplified.lola";
    PetriNetIO.writeToFile(simplifiedNet, targetPath_lola, PetriNetIO.FORMAT_LOLA, 0);
    
    String targetPath_result = fileName_system_sysPath+".simplified.result.txt";
    writeFile(targetPath_result, generateResultsFile());
  }
  
  private void writeFile(String fileName, String contents) throws IOException {
    FileWriter fstream = new FileWriter(fileName);
    BufferedWriter out = new BufferedWriter(fstream);
    out.write(contents);
    out.close();
  }
  
  private static DNodeBP _debug_lastViewBuild = null;
  private static HashMap<Object, String> _debug_colorMap2;
  
  private long _time_buildBP_start;
  private long _time_buildBP_finish;
  
  private long _time_equiv_start;
  private long _time_equiv_finish;
  
  private long _time_implied_start;
  private long _time_implied_finish;

  private long _time_fold_start;
  private long _time_fold_finish;

  private long _time_chain_start;
  private long _time_chain_finish;
  
  private float _comp_fold;
  private float _comp_implied;
  private float _comp_chain;
  
  private String _size_bp;
  private String _size_fold;
  private String _size_implied;
  private String _size_chain;
  
  private static PetriNet simplifyModel(MineSimplify sim, ISystemModel sysModel, LinkedList<String[]> traces,
    boolean abstractCycles) throws InvalidModelException {
    
    DNodeSys sys = Uma.getBehavioralSystemModel(sysModel);
    DNodeBP build = Uma.initBuildPrefix_View(sys, 0);

    sim._time_buildBP_start = System.currentTimeMillis();
    //Uma.out.println("generating view for "+traces.size()+" traces...");
    ViewGeneration2 viewGen = new ViewGeneration2(build);
    for (String[] trace : traces) {
      viewGen.extendByTrace(trace);
    }
    boolean printDetail = (build.getBranchingProcess().allConditions.size()
        +build.getBranchingProcess().allEvents.size() > 10000) ? true : false;
    sim._time_buildBP_finish = System.currentTimeMillis();
    sim._size_bp = build.getStatistics();
    
    // viewGen.identifyFoldingRelation();
    //build.debugPrintCCpairs();
    
    sim._time_equiv_start = System.currentTimeMillis();
    if (printDetail) Uma.out.println("equivalence..");
    build.foldingEquivalence();
    
    //build.debug_printFoldingEquivalence();
    
    if (printDetail) Uma.out.println("join maximal..");
    build.extendFoldingEquivalence_maximal();
    if (printDetail) Uma.out.println("fold backwards..");
    while (build.extendFoldingEquivalence_backwards()) {
      if (printDetail) Uma.out.println("fold backwards..");
    }
    if (printDetail) Uma.out.println("relax..");
    build.relaxFoldingEquivalence();
    if (printDetail) Uma.out.println("determinize..");
    while (build.extendFoldingEquivalence_deterministic()) {
      if (printDetail) Uma.out.println("determinize..");
    }
    sim._time_equiv_finish = System.currentTimeMillis();
    
    //build.simplifyFoldingEquivalence();
    
    sim._time_implied_start = System.currentTimeMillis();
    if (printDetail) Uma.out.println("transitive dependencies..");
    TransitiveDependencies dep = new TransitiveDependencies(build);
    if (printDetail) Uma.out.println("find solution..");
    HashSet<DNode> implied = dep.getImpliedConditions_solution();
    sim._time_implied_finish = System.currentTimeMillis();
    
    sim._time_fold_start = System.currentTimeMillis();
    if (printDetail) Uma.out.println("fold net..");
    NetSynthesis synth = new NetSynthesis(build);
    PetriNet net = synth.foldToNet_labeled(false);
    sim._time_fold_finish = System.currentTimeMillis();
    sim._comp_fold = complexitySimple(net);
    sim._size_fold = net.getInfo();
    
    if (printDetail) Uma.out.println("remove implied places..");
    LinkedList<Place> impliedPlaces = new LinkedList<Place>();
    for (Place p : net.getPlaces()) {
      boolean allImplied = true;
      DNode b = synth.n2d.get(p);
      for (DNode bPrime : build.foldingEquivalence().get(build.equivalentNode().get(b))) {
        /*
        if (!implied.contains(bPrime)) {
          allImplied = false;
        }
        */
        if (implied.contains(bPrime)) {
          impliedPlaces.add(p);
          break;
        }
      }
      /*
      if (allImplied) {
        impliedPlaces.add(p);
      }
      */
    }
    
    for (Place p : impliedPlaces) {
      boolean isSinglePost = false;
      boolean isSinglePre = false;
      for (Transition t : p.getPreSet()) {
        if (t.getOutgoing().size() == 1) {
          isSinglePost = true;
          break;
        }
      }
      for (Transition t : p.getPostSet()) {
        if (t.getIncoming().size() == 1) {
          isSinglePre = true;
          break;
        }
      }

      if (!isSinglePost && !isSinglePre) {
        //Uma.out.println("removing implied place: "+p+" "+p.getPreSet()+" "+p.getPostSet());
        net.removePlace(p);
      }
    }
    
    for (Transition t : net.getTransitions()) {
      LinkedList<Place> maxPlaces = new LinkedList<Place>();
      for (Place p : t.getPostSet()) {
        if (p.getOutgoing().isEmpty())
          maxPlaces.add(p);
      }
      /*
      if (!maxPlaces.isEmpty() && maxPlaces.size() == t.getPostSet().size())
        maxPlaces.removeLast();
      */
      for (Place p : maxPlaces) {
        
        boolean isSinglePost = false;
        for (Transition t2 : p.getPreSet()) {
          if (t2.getOutgoing().size() == 1) {
            isSinglePost = true;
            break;
          }
        }
        if (!isSinglePost) {
          //Uma.out.println("removing terminal place: "+p);
          net.removePlace(p);
        }
      }
    }
    sim._comp_implied = complexitySimple(net);
    sim._size_implied = net.getInfo();
    
    _debug_lastViewBuild = build;

    sim._time_chain_start = System.currentTimeMillis();
    if (abstractCycles) {
      //while (collapseChains(net));
    }
    sim._comp_chain = complexitySimple(net);
    sim._size_chain = net.getInfo();
    
    removeFlowerPlaces(net, 0.05f);
    sim._time_chain_finish = System.currentTimeMillis();
    
    Uma.out.println("done");
    
    return net;
  }
  
  private static HashMap<Object, String> _debug_cycleAbstraction_coloring;
  
  /**
   * Collapse chains of consecutive transitions with the same label
   * to a loop of length 1. The loop produces on all post-places of the
   * chain except the places between the transitions of the chain.
   * 
   * One call will search for and collapse the longest chain in the net.
   * 
   * @param net
   * @return <code>true</code> iff there was a chain and it was collapsed
   */
  private static boolean collapseChains(PetriNet net) {
    
    HashSet<Transition> longestChain = null;
    HashSet<Place> longestChain_pre = null;
    HashSet<Place> longestChain_post = null;
    Transition chainStart = null;
    Transition chainEnd = null;
    
    // iterate over all transitions and search for the transition that is
    // the start of the longest chain in the net
    for (Transition t : net.getTransitions()) {
      
      //if (_debug_cycleAbstraction_coloring.containsKey(t)) continue;
      
      // 't' is in the chain
      HashSet<Transition> chain = new HashSet<Transition>();
      chain.add(t);

      // Determine the names of the places that connect any two transitions
      // of the chain. For loops of length 1, these are the names that occur
      // as labels of a pre- and of a post-place of 't'. Build the intersection
      // of the set of names in the pre- and in the post-set of 't'
      HashSet<String> chainPlaces = new HashSet<String>();
      for (Place p : t.getPreSet()) {
        chainPlaces.add(p.getName());
      }
      HashSet<String> placeNamesPost = new HashSet<String>();
      for (Place p : t.getPostSet()) {
        placeNamesPost.add(p.getName());
      }
      // intersection of the names in the pre- and post-set of t
      chainPlaces.retainAll(placeNamesPost);  

      // collect all pre-conditions and post-conditions of the chain
      // for abstracting the chain to a smaller pattern
      HashSet<Place> jointPreConditions = new HashSet<Place>();
      HashSet<Place> jointPostConditions = new HashSet<Place>();
      
      // take all places that do not constitute the chain
      for (Place p : t.getPreSet())
        if (!chainPlaces.contains(p.getName())) jointPostConditions.add(p);
      for (Place p : t.getPostSet())
        if (!chainPlaces.contains(p.getName())) jointPreConditions.add(p);

      // we do a breadth-first search to build the chain
      LinkedList<Transition> chainQueue = new LinkedList<Transition>();
      chainQueue.addLast(t);
      
      Transition tLast = t;
      while (!chainQueue.isEmpty()) {
        Transition s = chainQueue.removeFirst();
        tLast = s;
        for (Arc a : s.getOutgoing()) {
          for (Arc a2 : a.getTarget().getOutgoing()) {
            Transition r = (Transition)a2.getTarget();
            
            // transition 'r' matches the chain and is not yet in it
            if (!chain.contains(r) && r.getName().equals(t.getName())) {
              //if (_debug_cycleAbstraction_coloring.containsKey(r)) continue;
              
              // add all post-places of 'r' to the joint post-places of the chain
              // (abstraction by building the union of the effects)
              for (Place p : r.getPostSet()) {
                if (!chainPlaces.contains(p.getName())) jointPostConditions.add(p);
              }

              // add 'r' to the queue and the chain if not already added
              chain.add(r);
              chainQueue.add(r);
            }
          }
        }
      } // finished building the chain
      
      // remember the longest chain in the net 
      if (longestChain == null || longestChain.size() < chain.size()) {
        longestChain = chain;
        longestChain_pre = jointPreConditions;
        longestChain_post = jointPostConditions;
        chainStart = t;
        chainEnd = tLast;
      }
    }
    // we abstract to chains of length 1, so return false if there is nothing left to abstract
    if (longestChain.size() <= 1) return false;
    
    // abstract the chain
    // remember all places to keep: the pre-set of the first transition,
    // and all joint effects
    HashSet<Place> keepPlace = new HashSet<Place>();
    keepPlace.addAll(chainStart.getPreSet());
    keepPlace.addAll(longestChain_pre);
    keepPlace.addAll(longestChain_post);

    // we abstract the chain by letting the first transition loop and
    // produce tokens on all post-places of the chain
    // create a loop of the first transition to its pre-set
    for (Place p : chainStart.getPreSet()) {
      net.addArc(chainStart, p);
      //_debug_cycleAbstraction_coloring.put(p, "green");
    }
    // and let it produce on all post-places of the chain
    for (Place p : longestChain_post) {
      net.addArc(chainStart, p);
      //_debug_cycleAbstraction_coloring.put(p, "green");
    }
    //_debug_cycleAbstraction_coloring.put(chainStart, "green");
    
    // finally remove the rest of the chain except the start transition
    // and places we need to keep
    for (Transition t : longestChain) {
      for (Place p : t.getPostSet()) {
        if (!keepPlace.contains(p)) {
          net.removePlace(p);
          //_debug_cycleAbstraction_coloring.put(p, "red");
        }
      }
      for (Place p : t.getPreSet()) {
        if (!keepPlace.contains(p)) {
          net.removePlace(p);
          //_debug_cycleAbstraction_coloring.put(p, "red");
        }
      }
      if (t != chainStart) {
        net.removeTransition(t);
        //_debug_cycleAbstraction_coloring.put(t, "red");
      }
    }
    return true;
  }
  
  /**
   * Split flower places that have more than the given threshold of
   * transitions in their pre-set and post-set. A flower place is split
   * into several places, one for each transition t that is in the
   * pre- and post-set of p; splitting preserves the loop. If t has
   * other pre-places, then the split place for t is removed. 
   * 
   * @param net
   * @param threshold fraction of the transitions of the net to be
   * in the pre and post-set of a place to characterize it as flower place 
   */
  private static void removeFlowerPlaces(PetriNet net, float threshold) {
    
    LinkedList<Place> places = new LinkedList<Place>(net.getPlaces());
    for (Place p : places) {
      if (   p.getIncoming().size() > net.getTransitions().size() * threshold
          && p.getOutgoing().size() > net.getTransitions().size() * threshold)
      {
        Uma.out.println("flower place: "+p);
        
        HashSet<Transition> prePost = new HashSet<Transition>(p.getPreSet());
        prePost.retainAll(p.getPostSet());
        
        for (Transition t : prePost) {
          
          Uma.out.println(t);
          if (t.getIncoming().size() > 1 && t.getOutgoing().size() > 1 ) {
            Uma.out.println(t.getIncoming());
            Uma.out.println(t.getOutgoing());
            for (Arc a : t.getIncoming()) {
              if (a.getSource() == p) {
                Uma.out.println("in "+a.getSource()+" --> "+a.getTarget()); 
                net.removeArc(a); 
                break; 
              }
            }
            for (Arc a : t.getOutgoing()) {
              if (a.getTarget() == p) { 
                Uma.out.println("out "+a.getSource()+" --> "+a.getTarget()); 
                net.removeArc(a); 
                break; 
              }
            }
          }
        }
      }
    }
  }
  
  private static float simplestThreshold;
  
  public static void simplify(String system, String log, boolean repeat) throws IOException, InvalidModelException {
    
    MineSimplify bestSim = null;
    
    long startTime = System.currentTimeMillis();
    
    for (int i=0; i<10; i++) {
      
      //DNodeBP.foldThreshold = (float)(i+1)/10;
      
      Uma.out.print(system+"... ");
      MineSimplify sim = new MineSimplify(system, log);
      sim.prepareModel();
      sim.run();
      
      if (bestSim == null) {
        bestSim = sim;
        //simplestThreshold = DNodeBP.foldThreshold;
        //if(!repeat) break;
      } else {
        if (complexitySimple(sim.getSimplifiedNet()) < complexitySimple(bestSim.getSimplifiedNet())) {
          bestSim = sim;
          //simplestThreshold = DNodeBP.foldThreshold;
        }
        
        if (System.currentTimeMillis() - startTime > 1000*60*30) {
          break;
        }
      }
    }
    bestSim.writeResults();
  }
  
  public static void main(String[] args) throws IOException, InvalidModelException {
    
    //DNodeBP.ignoreFoldThreshold = false;
    
    // AMC
    simplify("./examples/caise11/Aandoening_A.lola", "./examples/caise11/Aandoening_A.log.txt", true); // F
    simplify("./examples/caise11/Aandoening_B.lola", "./examples/caise11/Aandoening_B.log.txt", true); // F
    simplify("./examples/caise11/Aandoening_C.lola", "./examples/caise11/Aandoening_C.log.txt", true); // F
    simplify("./examples/caise11/AMC.lola", "./examples/caise11/AMC.log.txt", true); // F
    
    // Catharina
    simplify("./examples/caise11/Complications.filtered80.lola", "./examples/caise11/Complications.filtered80.log.txt", true); // F
    
    // Heusden
    simplify("./examples/caise11/Afschriften.lola", "./examples/caise11/Afschriften.log.txt", true); // F
    simplify("./examples/caise11/BezwaarWOZ_filtered_All.lola", "./examples/caise11/BezwaarWOZ_filtered_All.log.txt", true); // F

    simplify("./examples/caise11/a12f0n00.lola", "./examples/caise11/a12f0n00.log.txt", true); // F
    simplify("./examples/caise11/a12f0n05.lola", "./examples/caise11/a12f0n05.log.txt", true); // F
    simplify("./examples/caise11/a12f0n10.lola", "./examples/caise11/a12f0n10.log.txt", true); // F
    simplify("./examples/caise11/a12f0n20.lola", "./examples/caise11/a12f0n20.log.txt", true); // F
    simplify("./examples/caise11/a12f0n50.lola", "./examples/caise11/a12f0n50.log.txt", true); // F

    simplify("./examples/caise11/a22f0n00.lola", "./examples/caise11/a22f0n00.log.txt", true); // F
    simplify("./examples/caise11/a22f0n05.lola", "./examples/caise11/a22f0n05.log.txt", true); // F
    simplify("./examples/caise11/a22f0n10.lola", "./examples/caise11/a22f0n10.log.txt", false); // slow
    simplify("./examples/caise11/a22f0n20.lola", "./examples/caise11/a22f0n20.log.txt", false); // slow
    simplify("./examples/caise11/a22f0n50.lola", "./examples/caise11/a22f0n50.log.txt", false); // slow
    
    simplify("./examples/caise11/a32f0n00.lola", "./examples/caise11/a32f0n00.log.txt", true);
    simplify("./examples/caise11/a32f0n05.lola", "./examples/caise11/a32f0n05.log.txt", true); // F
    simplify("./examples/caise11/a32f0n10.lola", "./examples/caise11/a32f0n10.log.txt", false); // slow 
    simplify("./examples/caise11/a32f0n20.lola", "./examples/caise11/a32f0n20.log.txt", false); // slow 
    simplify("./examples/caise11/a32f0n50.lola", "./examples/caise11/a32f0n50.log.txt", false); // slow
    
    //simplify("./examples/caise11/t32f0n05.lola", "./examples/caise11/t32f0n05.log.txt", false); // slow
  }
}
