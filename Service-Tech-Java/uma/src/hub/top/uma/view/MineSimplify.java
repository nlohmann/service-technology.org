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
import hub.top.uma.DNodeRefold;
import hub.top.uma.DNodeSet;
import hub.top.uma.DNodeSys;
import hub.top.uma.DotColors;
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
import java.util.List;
import java.util.Map;
import java.util.Set;

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

  private ViewGeneration2 viewGen;                     // the class used to construct the log-induced branching prefix
  private DNodeRefold build = null;                    // the branching process of sysModel
  private HashMap<Place, Set<DNode>> place_conditions; // all conditions labeled with the given place
  
  private PetriNet simplifiedNet;
  private PetriNet originalNet;
  
  private float netComplexity_original;
  private float netComplexity_simplified;
  
  /**
   * Configuration for the simplification procedure in {@link MineSimplify}.
   */
  public static class Configuration {
    
    public static final int REMOVE_IMPLIED_OFF = 0;
    public static final int REMOVE_IMPLIED_PRESERVE_ALL = 1;
    public static final int REMOVE_IMPLIED_PRESERVE_VISIBLE = 2;
    public static final int REMOVE_IMPLIED_PRESERVE_CONNECTED = 3;
    
    public boolean unfold_refold = true; 
    public int remove_implied = REMOVE_IMPLIED_PRESERVE_CONNECTED;
    public boolean abstract_chains = true;
    public boolean remove_flower_places = true;
    
    public Map<String, String> eventToTransition = new HashMap<String, String>();
  }
  
  /**
   * configuration used for simplification
   */
  private Configuration config;
  
  /**
   * Simplify a system model wrt. a given set of traces in the standard configuration.
   * Read the systemModel to simplify and the corresponding traces from files.
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
    this(fileName_system_sysPath, fileName_trace, new Configuration());
  }
  
  /**
   * Simplify a system model wrt. a given set of traces.
   * Read the systemModel to simplify and the corresponding traces from files.
   * 
   * @param fileName_system_sysPath
   * @param fileName_trace
   * @param config
   * @throws IOException
   * @throws InvalidModelException
   * @throws FileNotFoundException
   */
  public MineSimplify(String fileName_system_sysPath, String fileName_trace, Configuration config) 
    throws IOException, InvalidModelException, FileNotFoundException
  {
    this.fileName_system_sysPath = fileName_system_sysPath;
    this.fileName_trace = fileName_trace;
    this.config = config;
    
    sysModel = Uma.readSystemFromFile(fileName_system_sysPath);
    allTraces = ViewGeneration2.readTraces(fileName_trace);
  }
  
  /**
   * Simplify a system model wrt. a given set of traces
   * 
   * @param sysModel
   * @param allTraces
   */
  public MineSimplify(ISystemModel sysModel, LinkedList<String[]> allTraces) 
  {
    this.sysModel = sysModel;
    this.allTraces = allTraces;
    this.config = new Configuration();  // standard configuration
  }

  /**
   * Simplify a system model wrt. a given set of traces. The simplification
   * steps can be configured using the {@link Configuration} object 'config'. 
   * 
   * @param sysModel
   * @param allTraces
   * @param config
   */
  public MineSimplify(ISystemModel sysModel, LinkedList<String[]> allTraces, Configuration config) 
  {
    this.sysModel = sysModel;
    this.allTraces = allTraces;
    this.config = config;               // user-defined config
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
    
    simplifiedNet = simplifyModel(sysModel, allTraces, config);
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
  
  public String generateCSVString() {
    String result_string = "";
    result_string += originalNet.getInfo()+";"+(int)(netComplexity_original*100)+";";
    result_string += _size_bp+";"+(_time_buildBP_finish - _time_buildBP_start)+";";
    result_string += _size_fold+";"+(int)(_comp_fold*100)+";"+(_time_equiv_finish - _time_equiv_start)+";";
    result_string += _size_implied+";"+(int)(_comp_implied*100)+";"+(_time_implied_finish - _time_implied_start)+";";
    result_string += _size_chain+";"+(int)(_comp_chain*100)+";"+(_time_chain_finish - _time_chain_start)+";";
    result_string += simplifiedNet.getInfo()+";"+(int)(netComplexity_simplified*100)+";"+(_time_chain_finish - _time_chain_start)+";";
    result_string += "\n";
    return result_string;
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
    
    result_string += generateCSVString();
    
    return result_string;
  }
  
  /**
   * @return original Petri net
   */
  public PetriNet getOriginalNet() {
    return originalNet;
  }
  
  /**
   * @return resulting simplified net after executing {@link #run()}
   */
  public PetriNet getSimplifiedNet() {
    return simplifiedNet;
  }
  
  public DNodeRefold getBuild() {
    return build;
  }
  
  /**
   * Write results to files. Requires that this simplification object was
   * instantiated with {@link MineSimplify#MineSimplify(String, String)}.
   *   
   * @throws IOException
   */
  public void writeResults() throws IOException {
    
    if (simplifiedNet == null)
      return;
    
    String targetPath_dot = fileName_system_sysPath+".simplified.dot";
    PetriNetIO.writeToFile(simplifiedNet, targetPath_dot, PetriNetIO.FORMAT_DOT, 0);
    
    String targetPath_lola = fileName_system_sysPath+".simplified.lola";
    PetriNetIO.writeToFile(simplifiedNet, targetPath_lola, PetriNetIO.FORMAT_LOLA, 0);
    
    String targetPath_result = fileName_system_sysPath+".simplified.result.txt";
    writeFile(targetPath_result, generateResultsFile());
    
    PetriNet bp = NetSynthesis.convertToPetriNet(_debug_lastViewBuild, _debug_lastViewBuild.getBranchingProcess().getAllNodes(), false);
    
    String targetPath_bp = fileName_system_sysPath+".bp.lola";
    PetriNetIO.writeToFile(bp, targetPath_bp, PetriNetIO.FORMAT_LOLA, 0);
  }
  
  /**
   * Helper function to write a string to a file.
   * 
   * @param fileName
   * @param contents
   * @throws IOException
   */
  private void writeFile(String fileName, String contents) throws IOException {
    FileWriter fstream = new FileWriter(fileName);
    BufferedWriter out = new BufferedWriter(fstream);
    out.write(contents);
    out.close();
  }

  /**
   * Main routine to simplify the given sysModel wrt. the traces. Simplification
   * steps are configured (see {@link Configuration} ).
   * 
   * @param sysModel
   * @param traces
   * @param config
   * @return the simplified Petri net
   * @throws InvalidModelException
   */
  private PetriNet simplifyModel(ISystemModel sysModel, LinkedList<String[]> traces,
    Configuration config) throws InvalidModelException {
    
    // step 1) unfold and refold-model
    PetriNet net = null;
    if (config.unfold_refold) {
      net = unfoldRefold(sysModel, traces);
    } else if (sysModel instanceof PetriNet) {
      net = new PetriNet((PetriNet)sysModel);
    } else {
      throw new InvalidModelException(InvalidModelException.UNKNOWN_MODEL_TYPE, sysModel);
    }

    // step 2) find implied places
    _time_implied_start = System.currentTimeMillis();
    if (config.remove_implied != Configuration.REMOVE_IMPLIED_OFF) {
      assertBranchingProcess(net, sysModel, traces);
      HashSet<DNode> implied = findImpliedConditions();
      removeImpliedPlaces(net, place_conditions, implied);
    }
    _time_implied_finish = System.currentTimeMillis();
    
    // step 3) collapse chains of transitions
    _time_chain_start = System.currentTimeMillis();
    if (config.abstract_chains) {
      while (collapseChains(net));
    }
    _comp_chain = complexitySimple(net);
    _size_chain = net.getInfo();

    // step 3) remove flower places
    if (config.remove_flower_places) {
      removeFlowerPlaces(net, 0.05f);
    }
    _time_chain_finish = System.currentTimeMillis();
    
    Uma.out.println("done");
    return net;
  }
  
  /**
   * Compute the unfolding of the system model up to the given traces and fold
   * the resulting unfolding back to a Petri net. Stores the computed branching
   * process for later use.
   * 
   * @param sysModel
   * @param traces
   * @return refolded Petri net
   * @throws InvalidModelException
   */
  private PetriNet unfoldRefold(ISystemModel sysModel, LinkedList<String[]> traces)  throws InvalidModelException {
    DNodeSys sys = Uma.getBehavioralSystemModel(sysModel);
    build = Uma.initBuildPrefix_View(sys, 0);

    _time_buildBP_start = System.currentTimeMillis();
    //Uma.out.println("generating view for "+traces.size()+" traces...");
    viewGen = new ViewGeneration2(build);
    for (String[] trace : traces) {
      viewGen.extendByTrace(trace, config.eventToTransition);
    }
    boolean printDetail = (build.getBranchingProcess().allConditions.size()
        +build.getBranchingProcess().allEvents.size() > 10000) ? true : false;
    _time_buildBP_finish = System.currentTimeMillis();
    _size_bp = build.getStatistics();
    
    // viewGen.identifyFoldingRelation();
    //build.debugPrintCCpairs();
    
    _time_equiv_start = System.currentTimeMillis();
    if (printDetail) Uma.out.println("equivalence..");
    build.futureEquivalence();
    
    //build.debug_printFoldingEquivalence();
    
    if (printDetail) Uma.out.println("join maximal..");
    build.extendFutureEquivalence_maximal();
    
    int changed;
    do {
      changed = 0;
      
      if (printDetail) Uma.out.println("fold backwards..");
      while (build.extendFutureEquivalence_backwards_complete()) {
        changed++;
        if (printDetail) Uma.out.println("fold backwards..");
      }
      if (printDetail) Uma.out.println("relax..");
      build.relaxFutureEquivalence();
      if (printDetail) Uma.out.println("determinize..");
      while (build.extendFutureEquivalence_deterministic()) {
        changed++;
        if (printDetail) Uma.out.println("determinize..");
      }
      
      //if (build.blockedFolding()) changed++;
    } while (changed > 0);
    
    _time_equiv_finish = System.currentTimeMillis();
    
    //build.simplifyFoldingEquivalence();
    
    _time_fold_start = System.currentTimeMillis();
    if (printDetail) Uma.out.println("fold net..");
    NetSynthesis synth = new NetSynthesis(build);
    PetriNet net = synth.foldToNet_labeled(false);
    
    // remember for each place, which conditions represent a token on this place
    // this information will be needed when checking for implied places
    place_conditions = new HashMap<Place, Set<DNode>>();
    for (Place p : net.getPlaces()) {
      DNode b = synth.n2d.get(p);
      place_conditions.put(p, build.futureEquivalence().get(build.equivalentNode().get(b)));
    }
    
    _time_fold_finish = System.currentTimeMillis();
    _comp_fold = complexitySimple(net);
    _size_fold = net.getInfo();
    
    _debug_lastViewBuild = build;

    return net;
  }
  
  /**
   * Assert that for the given system model, the branching process up to the given
   * traces is available ({@link #build}). If it has not been computed before,
   * then compute it now.
   * 
   * @param net
   * @param sysModel
   * @param traces
   * @throws InvalidModelException
   */
  private void assertBranchingProcess(PetriNet net, ISystemModel sysModel, LinkedList<String[]> traces) throws InvalidModelException {
    if (build != null) return;

    DNodeSys sys = Uma.getBehavioralSystemModel(sysModel);
    build = Uma.initBuildPrefix_View(sys, 0);

    _time_buildBP_start = System.currentTimeMillis();
    //Uma.out.println("assert existence of branching process...");
    viewGen = new ViewGeneration2(build);
    for (String[] trace : traces) {
      viewGen.extendByTrace(trace, config.eventToTransition);
    }
    _time_buildBP_finish = System.currentTimeMillis();
    _size_bp = build.getStatistics();
    _debug_lastViewBuild = build;
    
    if (sysModel instanceof PetriNet) {
      // collect for each place of the net, all conditions that represent a token on the place
      // required for finding implicit places
      place_conditions = new HashMap<Place, Set<DNode>>();
      for (Place p : net.getPlaces()) {
        
        place_conditions.put(p, new HashSet<DNode>());
        for (DNode b : build.getBranchingProcess().getAllConditions()) {
          if (sys.properNames[b.id].equals(p.getName())) {
            place_conditions.get(p).add(b);
          }
        }
      }
    }
  }
  
  /**
   * list of nodes that were added to the branching process to ensure that
   * only implied conditions that do not change the behavior get removed
   */
  private List<DNode> _extendedNodes = new LinkedList<DNode>();
  
  /**
   * @return the set of implied conditions in the branching process of the system model
   */
  private HashSet<DNode> findImpliedConditions() {
    
    boolean printDetail = (build.getBranchingProcess().allConditions.size()
        +build.getBranchingProcess().allEvents.size() > 10000) ? true : false;

    if (printDetail) Uma.out.println("transitive dependencies..");
    TransitiveDependencies dep = new TransitiveDependencies(build);
    if (printDetail) Uma.out.println("find solution..");
    HashSet<DNode> implied;
    if (config.remove_implied == Configuration.REMOVE_IMPLIED_PRESERVE_CONNECTED) {
      // locally find all implied conditions, regardless of equivalence classes
      System.out.println("finding all");
      implied = dep.getImpliedConditions_solutionLocal();
      //implied = dep.getAllImpliedConditions();
    } else if (config.remove_implied == Configuration.REMOVE_IMPLIED_PRESERVE_VISIBLE) {
      // find equivalence classes of implied conditions
      implied = dep.getImpliedConditions_solutionGlobal();
    } else if (config.remove_implied == Configuration.REMOVE_IMPLIED_PRESERVE_ALL) {
      // find equivalence classes of implied conditions ensuring that no
      // new behavior is introduced by considering also future behavior
      
      // extend the branching process with events that are enabled, but
      // did not fire
      _extendedNodes = dep.extendBranchingProcessWithNextEvents(viewGen.visitedMarkings);
      // identify implied conditions in this branching process
      implied = dep.getImpliedConditions_solutionGlobal();
      // and remove the added events again
      implied.removeAll(_extendedNodes);
      dep.removeExtendedNodes();
    } else {
      implied = new HashSet<DNode>();
      System.out.println("Error! Unknown mode for finding implied conditions: "+config.remove_implied);
    }
    return implied;
  }
  
  /**
   * conditions of the branching process identified as implied conditions
   */
  private Set<DNode> _implied;

  /**
   * @return map assigning each implied condition a GraphViz color, conditions
   *         of the same equivalence class get the same color
   */
  public HashMap<DNode, String> _getColoringImplied() {
    HashMap<DNode, String> colorMap = new HashMap<DNode, String>();
    LinkedList<Set<DNode>> v = new LinkedList<Set<DNode>>(build.futureEquivalence().values());
    for (int i=0; i<v.size(); i++) {
      for (DNode d : v.get(i)) {
        if (_implied.contains(d)) {
          //colorMap.put(d, DotColors.colors[i]);
          colorMap.put(d, "red");
        }
      }
    }
    
    for (DNode d : _extendedNodes) {
      colorMap.put(d, "black");
    }
    
    return colorMap;
  }
  
  /**
   * Remove all implied places from the net. The implied places are computed from
   * the set of implied conditions of the branching process ({@link #findImpliedConditions()}).
   * 
   * @param net
   * @param place_conditions
   * @param implied
   */
  private void removeImpliedPlaces(PetriNet net, Map<Place, Set<DNode> > place_conditions, Set<DNode> implied)
  {
    this._implied = implied;
    
    boolean printDetail = (build.getBranchingProcess().allConditions.size()
        +build.getBranchingProcess().allEvents.size() > 10000) ? true : false;
    
    boolean findAllImplied;
    if (config.remove_implied == Configuration.REMOVE_IMPLIED_PRESERVE_CONNECTED) {
      findAllImplied = false;
    } else {
      findAllImplied = true;
    }
    
    Uma.out.println("remove implied places..");
    LinkedList<Place> impliedPlaces = new LinkedList<Place>();
    
    LinkedList<Place> _weak_impliedPlaces = new LinkedList<Place>();
    
    TransitiveDependencies dep = new TransitiveDependencies(build);
    HashSet<DNode> implied_local = dep.getImpliedConditions_solutionLocal();
    
    for (Place p : net.getPlaces()) {
      
      boolean place_p_isImplied = findAllImplied;
      
      for (DNode bPrime : place_conditions.get(p)) {

        if (findAllImplied) {
          // remove place only if every condition is implied
          if (!implied.contains(bPrime)) {
            place_p_isImplied = false;
            
            for (DNode bPrime2 : place_conditions.get(p)) {
              if (implied_local.contains(bPrime2))
                _weak_impliedPlaces.add(p);
            }

            break;
          }

        } else {
          // remove place as soon as one condition is implied
          if (implied.contains(bPrime)) {
            place_p_isImplied = true;
            break;
          }
        }
      }
      
      if (place_p_isImplied) {
        
        //for (DNode bPrime : place_conditions.get(p)) {
        //  if (!implied.contains(bPrime)) {
        //    System.out.println(bPrime+" is not implied");
        //  }
        //}
        impliedPlaces.add(p);
      }
    }
    
    for (Place p : impliedPlaces) {
      boolean isSinglePost = false;
      boolean isSinglePre = false;
      // ensure that a place is only removed if it does not
      // partition the net (by removing the last pre-place
      // or post-place of a transition)
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

      if (!isSinglePost && !isSinglePre)
      {
        Uma.out.println("removing implied place: "+p+" "+p.getPreSet()+" "+p.getPostSet());
        net.removePlace(p);
        for (DNode d : place_conditions.get(p)) {
          _debug_weakImpliedConditions.put(d, "red");
        }
      }
    }
    
    List<Place> _removed_weak_impliedPlaces = new LinkedList<Place>();
    for (Place p : _weak_impliedPlaces) {
      boolean isSinglePost = false;
      boolean isSinglePre = false;
      // ensure that a place is only removed if it does not
      // partition the net (by removing the last pre-place
      // or post-place of a transition)
      for (Transition t : p.getPreSet()) {
        List<Place> t_post = t.getPostSet();
        t_post.removeAll(_removed_weak_impliedPlaces);
        if (t_post.size() == 1) {
          isSinglePost = true;
          break;
        }
      }
      for (Transition t : p.getPostSet()) {
        List<Place> t_pre = t.getPreSet();
        t_pre.removeAll(_removed_weak_impliedPlaces);
        if (t_pre.size() == 1) {
          isSinglePre = true;
          break;
        }
      }

      if (!isSinglePost && !isSinglePre) {
        _removed_weak_impliedPlaces.add(p);
      }
    }
    
    for (Place p : _removed_weak_impliedPlaces) {
      _debug_weakImpliedPlaces.put(p, "orange");
      for (Arc a : p.getIncoming())
        _debug_weakImpliedPlaces.put(a, "orange");
      for (Arc a : p.getOutgoing())
        _debug_weakImpliedPlaces.put(a, "orange");
      for (DNode d : place_conditions.get(p)) {
        if (implied_local.contains(d))
          _debug_weakImpliedConditions.put(d, "orange");
        else
          _debug_weakImpliedConditions.put(d, "green");
      }
    }
    
    
    for (Transition t : net.getTransitions()) {
      LinkedList<Place> maxPlaces = new LinkedList<Place>();
      for (Place p : t.getPostSet()) {
        if (p.getOutgoing().isEmpty())
          maxPlaces.add(p);
      }
      
      //if (!maxPlaces.isEmpty() && maxPlaces.size() == t.getPostSet().size())
      //  maxPlaces.removeLast();

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
    
    _comp_implied = complexitySimple(net);
    _size_implied = net.getInfo();
  }
  
  public HashMap<Object, String> _debug_weakImpliedPlaces = new HashMap<Object, String>();
  public HashMap<DNode, String> _debug_weakImpliedConditions = new HashMap<DNode, String>();
  
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
    if (longestChain == null || longestChain.size() <= 1) return false;
    
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
  
  // fields for recording times and sizes for benchmark experiments
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
  
  private static float simplestThreshold;
  
  public static void simplify(String system, String log, boolean repeat) throws IOException, InvalidModelException {
    simplify(system, log, repeat, new Configuration());
  }
  
  public static String simplify(String system, String log, boolean repeat, Configuration config) throws IOException, InvalidModelException {
    
    MineSimplify bestSim = null;
    
    long startTime = System.currentTimeMillis();
    
    for (int i=0; i<10; i++) {
      
      //DNodeBP.foldThreshold = (float)(i+1)/10;
      
      Uma.out.print(system+"... ");
      MineSimplify sim = new MineSimplify(system, log, config);
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
    return bestSim.generateCSVString();
  }
  
  public static void runExperiment(String path, Configuration config) throws IOException, InvalidModelException {
    //DNodeBP.ignoreFoldThreshold = false;
    
    StringBuilder sb = new StringBuilder();
    
    sb.append(simplify(path+"/a12f0n00.lola", path+"/a12f0n00.log.txt", true, config)); // F
    sb.append(simplify(path+"/a12f0n05.lola", path+"/a12f0n05.log.txt", true, config)); // F
    sb.append(simplify(path+"/a12f0n10.lola", path+"/a12f0n10.log.txt", true, config)); // F
    sb.append(simplify(path+"/a12f0n20.lola", path+"/a12f0n20.log.txt", true, config)); // F
    sb.append(simplify(path+"/a12f0n50.lola", path+"/a12f0n50.log.txt", true, config)); // F
    
    sb.append(simplify(path+"/a22f0n00.lola", path+"/a22f0n00.log.txt", true, config)); // F
    sb.append(simplify(path+"/a22f0n05.lola", path+"/a22f0n05.log.txt", true, config)); // F
    sb.append(simplify(path+"/a22f0n10.lola", path+"/a22f0n10.log.txt", false, config)); // slow
    sb.append(simplify(path+"/a22f0n20.lola", path+"/a22f0n20.log.txt", false, config)); // slow
    sb.append(simplify(path+"/a22f0n50.lola", path+"/a22f0n50.log.txt", false, config)); // slow
    
    sb.append(simplify(path+"/a32f0n00.lola", path+"/a32f0n00.log.txt", true, config));
    sb.append(simplify(path+"/a32f0n05.lola", path+"/a32f0n05.log.txt", true, config)); // F
    sb.append(simplify(path+"/a32f0n10.lola", path+"/a32f0n10.log.txt", false, config)); // slow 
    sb.append(simplify(path+"/a32f0n20.lola", path+"/a32f0n20.log.txt", false, config)); // slow 
    sb.append(simplify(path+"/a32f0n50.lola", path+"/a32f0n50.log.txt", false, config)); // slow
    
    //simplify(path+"/t32f0n05.lola", path+"/t32f0n05.log.txt", false, config); // slow

    // AMC
    sb.append(simplify(path+"/Aandoening_A.lola", path+"/Aandoening_A.log.txt", true, config)); // F
    sb.append(simplify(path+"/Aandoening_B.lola", path+"/Aandoening_B.log.txt", true, config)); // F
    sb.append(simplify(path+"/Aandoening_C.lola", path+"/Aandoening_C.log.txt", true, config)); // F
    sb.append(simplify(path+"/AMC.lola", path+"/AMC.log.txt", true, config)); // F
    
    // Heusden
    sb.append(simplify(path+"/Afschriften.lola", path+"/Afschriften.log.txt", true, config)); // F
    sb.append(simplify(path+"/BezwaarWOZ_filtered_All.lola", path+"/BezwaarWOZ_filtered_All.log.txt", true, config)); // F

    // Catharina
    sb.append(simplify(path+"/Complications.filtered80.lola", path+"/Complications.filtered80.log.txt", true, config)); // F
  }
  
  public static void main(String[] args) throws IOException, InvalidModelException {

    Configuration c = new Configuration();
    /*
    c.unfold_refold = true;
    c.remove_implied = Configuration.REMOVE_IMPLIED_OFF;
    c.abstract_chains = false;
    c.remove_flower_places = false;
    runExperiment("./examples/bpm11/exp1_foldRefold", c);

    c.unfold_refold = true;
    c.remove_implied = Configuration.REMOVE_IMPLIED_PRESERVE_ALL;
    c.abstract_chains = false;
    c.remove_flower_places = false;
    runExperiment("./examples/bpm11/exp2_foldRefold_implied", c);

    c.unfold_refold = false;
    c.remove_implied = Configuration.REMOVE_IMPLIED_PRESERVE_ALL;
    c.abstract_chains = false;
    c.remove_flower_places = false;
    runExperiment("./examples/bpm11/exp3_implied", c);

    c.unfold_refold = true;
    c.remove_implied = Configuration.REMOVE_IMPLIED_PRESERVE_ALL;
    c.abstract_chains = true;
    c.remove_flower_places = false;
    runExperiment("./examples/bpm11/exp4_foldRefold_implied_chains", c);
    */
    c.unfold_refold = true;
    c.remove_implied = Configuration.REMOVE_IMPLIED_PRESERVE_ALL;
    c.abstract_chains = true;
    c.remove_flower_places = true;
    runExperiment("./examples/bpm11/exp5_complete", c);

    Precision.main(null);
  }
  
}
