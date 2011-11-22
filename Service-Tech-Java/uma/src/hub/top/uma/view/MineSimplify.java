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
import hub.top.petrinet.Node;
import hub.top.petrinet.PetriNet;
import hub.top.petrinet.PetriNetIO;
import hub.top.petrinet.Place;
import hub.top.petrinet.Transition;
import hub.top.uma.DNode;
import hub.top.uma.DNodeBP;
import hub.top.uma.DNodeRefold;
import hub.top.uma.DNodeSys;
import hub.top.uma.InvalidModelException;
import hub.top.uma.Uma;
import hub.top.uma.synthesis.ImplicitPlaces;
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

  protected String fileName_system_sysPath; 
  private String fileName_trace;
  
  private ISystemModel sysModel;
  private LinkedList<String[]> allTraces;

  protected ViewGeneration2 viewGen;                   // the class used to construct the log-induced branching prefix
  private DNodeRefold build = null;                    // the branching process of sysModel
  private HashMap<Place, Set<DNode>> place_conditions; // all conditions labeled with the given place
  
  protected PetriNet simplifiedNet;
  protected PetriNet originalNet;
  
  /**
   * Configuration for the simplification procedure in {@link MineSimplify}.
   */
  public static class Configuration {
    
    public static final int REMOVE_IMPLIED_OFF = 0;
    public static final int REMOVE_ILP = 1;
    public static final int REMOVE_IMPLIED_PRESERVE_ALL = 2;
    public static final int REMOVE_IMPLIED_PRESERVE_VISIBLE = 3;
    public static final int REMOVE_IMPLIED_PRESERVE_CONNECTED = 4;
    public static final int REMOVE_IMPLIED_PRESERVE_CONNECTED2 = 5;
    
    public boolean unfold_refold = true; 
    public double filter_threshold = 0.05;
    public int remove_implied = REMOVE_IMPLIED_PRESERVE_ALL;
    public boolean abstract_chains = false;
    public boolean remove_flower_places = false;
    
    public Map<String, String> eventToTransition = new HashMap<String, String>();
    
    @Override
    public String toString() {
      return unfold_refold+" "+filter_threshold+" "+remove_implied+" "+abstract_chains+" "+remove_flower_places;
    }
  }
  
  /**
   * configuration used for simplification
   */
  private Configuration config;
  
  /**
   * complete and partial results collected throughout the computation process
   */
  public static class Result {
    
    public String netSize_original;
    public String netSize_final;

    /**
     * places that could be identified in some situations as implied
     * and in other situations as not implied
     */
    public LinkedList<Place> weak_impliedPlaces;
    
    /**
     * places removed by {@link MineSimplify#removeMaximalPlaces(PetriNet)}
     */
    public LinkedList<Place> removedMaximalPlaces;
    
    /**
     * places removed by {@link MineSimplify#removeImpliedPlaces(PetriNet, List)}
     */
    public LinkedList<Place> removedImpliedPlaces;
    
    /**
     * mapping from nodes of the unfolding to nodes of the net
     */
    public HashMap<DNode, Node> d2n;
    
    protected long _time_start[] = new long[FINAL+1];
    protected long _time_finish[] = new long[FINAL+1];
    
    protected float _complexity[] = new float[FINAL+1];
    protected String _net_size[] = new String[FINAL+1];
    
    protected PetriNet[] _nets = new PetriNet[FINAL+1];

    public static final int ORIGINAL = 0;
    public static final int STEP_UNFOLD = 1;
    public static final int STEP_FILTER = 2;
    public static final int STEP_EQUIV = 3;
    public static final int STEP_IMPLIED = 4;
    public static final int STEP_FOLD = 5;
    public static final int STEP_CHAINS = 6;
    public static final int STEP_FLOWER = 7;
    public static final int FINAL = 8;
        
    public long getRuntime(int step) {
      return _time_finish[step]-_time_start[step];
    }
    
    @Override
    public String toString() {
      String result_string = "";
      
      result_string += "original net: "+netSize_original+" , complexity: "+_complexity[Result.ORIGINAL]+"\n";
      result_string += "build BP(L): "+getRuntime(Result.STEP_UNFOLD)+" , BP: "+_net_size[Result.STEP_UNFOLD]+"\n";
      result_string += "folding relation: "+getRuntime(Result.STEP_EQUIV)+getRuntime(Result.STEP_FOLD)+" , net: "+_net_size[Result.STEP_FOLD]+" , complexity: "+_complexity[Result.STEP_FOLD]+"\n";
      result_string += "implied places: "+getRuntime(Result.STEP_IMPLIED)+" , net: "+_net_size[Result.STEP_IMPLIED]+" , complexity: "+_complexity[Result.STEP_IMPLIED]+"\n";
      result_string += "chains: "+getRuntime(Result.STEP_CHAINS)+" , net: "+_net_size[Result.STEP_CHAINS]+" , complexity: "+_complexity[Result.STEP_CHAINS]+"\n";
      result_string += "flower places: "+getRuntime(Result.STEP_FLOWER)+" , net: "+_net_size[Result.STEP_FLOWER]+" , complexity: "+_complexity[Result.STEP_FLOWER]+"\n";
      result_string += "total time: "+getRuntime(Result.FINAL)+" , simplified net: "+netSize_final+" , complexity: "+_complexity[Result.FINAL]+"\n";
      result_string += "threshold: "+simplestThreshold;
      result_string += "\n";
      

      result_string += "removed implied places:\n";
      if (this.removedImpliedPlaces != null)
        for (Place p : this.removedImpliedPlaces) {
          result_string += p.toString()+"\n";
        }
      
      return result_string;
    }
    
  }
  

  
  /**
   * complete and partial results collected throughout the computation process
   */
  public Result result = new Result();
  

  
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
  
  
  public boolean run() throws InvalidModelException {
    
    Uma.out.println("Simplifying "+fileName_system_sysPath+"...");
    Uma.out.println("Configuration: "+config);
    
    result._time_start[Result.FINAL] = System.currentTimeMillis();
    
    // execute the simplification algorithm
    simplifiedNet = simplifyModel(sysModel, allTraces, config);
    if (simplifiedNet == null)
      return false;

    // some statistics
    if (sysModel instanceof PetriNet) {
      result._complexity[Result.ORIGINAL] = complexitySimple((PetriNet)sysModel);
      result._net_size[Result.ORIGINAL] = ((PetriNet)sysModel).getInfo(true);
      result.netSize_original = ((PetriNet)sysModel).getInfo(false);
      result._nets[Result.ORIGINAL] = (PetriNet)sysModel;
    } else {
      result._complexity[Result.ORIGINAL] = Float.MAX_VALUE;
      result._net_size[Result.ORIGINAL] = "0;0;0";
      result.netSize_original = "<unknown>";
      result._nets[Result.ORIGINAL] = new PetriNet();
    }
    result._time_finish[Result.FINAL] = System.currentTimeMillis();

    result._complexity[Result.FINAL] = complexitySimple(simplifiedNet);
    result._net_size[Result.FINAL] = simplifiedNet.getInfo(true);
    result.netSize_final = simplifiedNet.getInfo(false);
    result._nets[Result.FINAL] = simplifiedNet;
    
    // print statistics result
    Uma.out.println("finished in "+result.getRuntime(Result.FINAL)+"ms, complexity went from "+result._complexity[Result.ORIGINAL]+" to  "+result._complexity[Result.FINAL]);
    
    /*
    ViewGeneration3 v = new ViewGeneration3(simplifiedNet);
    int failed = 0;
    for (String[] trace : allTraces) {
      if (!v.validateTrace(trace)) failed++;
    }
    if (failed > 0) System.err.println(fileName_system_sysPath+" cannot replay "+failed+"/"+allTraces.size()+" traces");
    */
    
    return true;
  }
  
  public static float complexitySimple(PetriNet net) {
    int nodeNum = net.getPlaces().size() + net.getTransitions().size();
    int arcNum = net.getArcs().size();
    
    return (float)arcNum/(float)nodeNum;
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
  
  public List<String[]> getTraces() {
    return allTraces;
  }
  
  /**
   * Write results to files. Requires that this simplification object was
   * instantiated with {@link MineSimplify#MineSimplify(String, String)}.
   *   
   * @throws IOException
   */
  public synchronized void writeResults() throws IOException {
    
    if (simplifiedNet == null)
      return;
    
    String targetPath_lola = fileName_system_sysPath+".simplified.lola";
    PetriNetIO.writeToFile(simplifiedNet, targetPath_lola, PetriNetIO.FORMAT_LOLA, 0);

    for (Transition t : simplifiedNet.getTransitions())
      t.setName("");
    for (Place p : simplifiedNet.getPlaces())
      p.setName("");
    
    String targetPath_dot = fileName_system_sysPath+".simplified.dot";
    PetriNetIO.writeToFile(simplifiedNet, targetPath_dot, PetriNetIO.FORMAT_DOT, 0);
    
    String targetPath_result = fileName_system_sysPath+".simplified.result.txt";
    writeFile(targetPath_result, result.toString(), false);
    
    //PetriNet bp = NetSynthesis.convertToPetriNet(debug._lastViewBuild, debug._lastViewBuild.getBranchingProcess().getAllNodes(), false);
    
    //String targetPath_bp = fileName_system_sysPath+".bp.lola";
    //PetriNetIO.writeToFile(bp, targetPath_bp, PetriNetIO.FORMAT_LOLA, 0);
    
    String targetPath_bp2 = fileName_system_sysPath+".bp.dot";
    writeFile(targetPath_bp2, build.toDot(debug._getColoringImplied()), false);
  }
  
  /**
   * Helper function to write a string to a file.
   * 
   * @param fileName
   * @param contents
   * @throws IOException
   */
  protected static void writeFile(String fileName, String contents, boolean append) throws IOException {
    FileWriter fstream = new FileWriter(fileName, append);
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
    result._time_start[Result.STEP_IMPLIED] = System.currentTimeMillis();
    if (config.remove_implied != Configuration.REMOVE_IMPLIED_OFF) {
      
      if (config.remove_implied == Configuration.REMOVE_ILP) {
        ImplicitPlaces.findImplicitPlaces(net);
      } else {
        assertBranchingProcess(net, sysModel, traces);
        HashSet<DNode> impliedConditions = findImpliedConditions();
        List<Place> impliedPlaces = getImpliedPlaces(net, place_conditions, impliedConditions);
        removeImpliedPlaces(net, impliedPlaces);
      }
    }
    removeMaximalPlaces(net); // always remove maximal places
    result._time_finish[Result.STEP_IMPLIED] = System.currentTimeMillis();
    result._complexity[Result.STEP_IMPLIED] = complexitySimple(net);
    result._net_size[Result.STEP_IMPLIED] = net.getInfo(true);
    result._nets[Result.STEP_IMPLIED] = new PetriNet(net);
    
    // step 3) collapse chains of transitions
    result._time_start[Result.STEP_CHAINS] = System.currentTimeMillis();
    if (config.abstract_chains) {
      while (collapseChains(net));
    }
    result._time_finish[Result.STEP_CHAINS] = System.currentTimeMillis();
    result._complexity[Result.STEP_CHAINS] = complexitySimple(net);
    result._net_size[Result.STEP_CHAINS] = net.getInfo(true);
    result._nets[Result.STEP_CHAINS] = new PetriNet(net);
    
    // step 4) remove flower places
    result._time_start[Result.STEP_FLOWER] = System.currentTimeMillis();
    if (config.remove_flower_places) {
      removeFlowerPlaces(net, 0.05f);
    }
    result._time_finish[Result.STEP_FLOWER] = System.currentTimeMillis();
    result._complexity[Result.STEP_FLOWER] = complexitySimple(net);
    result._net_size[Result.STEP_FLOWER] = net.getInfo(true);
    result._nets[Result.STEP_FLOWER] = new PetriNet(net);
    
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

    result._time_start[Result.STEP_UNFOLD] = System.currentTimeMillis();
    Uma.out.println("generating view for "+traces.size()+" traces...");
    viewGen = new ViewGeneration2(build);
    for (String[] trace : traces) {
      viewGen.extendByTrace(trace, config.eventToTransition);
    }
    boolean printDetail = (build.getBranchingProcess().allConditions.size()
        +build.getBranchingProcess().allEvents.size() > 10000) ? true : false;
    result._time_finish[Result.STEP_UNFOLD] = System.currentTimeMillis();
    result._net_size[Result.STEP_UNFOLD] = build.getStatistics(true);
    result._nets[Result.STEP_UNFOLD] = null;
    
    // viewGen.identifyFoldingRelation();
    //build.debugPrintCCpairs();
    
    if (config.filter_threshold > 0) {
      Uma.out.println("remove noise < "+config.filter_threshold+" ...");
      result._time_start[Result.STEP_FILTER] = System.currentTimeMillis();
      viewGen.removeNoise(config.filter_threshold);
      result._time_finish[Result.STEP_FILTER] = System.currentTimeMillis();
      result._net_size[Result.STEP_FILTER] = build.getStatistics(true);
      result._nets[Result.STEP_FILTER] = null;
    }
    
    result._time_start[Result.STEP_EQUIV] = System.currentTimeMillis();
    
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
    
    result._time_finish[Result.STEP_EQUIV] = System.currentTimeMillis();
    result._net_size[Result.STEP_EQUIV] = build.getStatistics(true);
    result._nets[Result.STEP_EQUIV] = null;
    
    //build.simplifyFoldingEquivalence();
    
    result._time_start[Result.STEP_FOLD] = System.currentTimeMillis();
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
    result.d2n = synth.d2n;
    
    result._time_finish[Result.STEP_FOLD] = System.currentTimeMillis();
    result._complexity[Result.STEP_FOLD] = complexitySimple(net);
    result._net_size[Result.STEP_FOLD] = net.getInfo(true);
    result._nets[Result.STEP_FOLD] = new PetriNet(net);
    
    debug._lastViewBuild = build;

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

    Uma.out.println("assert existence of branching process...");
    
    DNodeSys sys = Uma.getBehavioralSystemModel(sysModel);
    build = Uma.initBuildPrefix_View(sys, 0);

    result._time_start[Result.STEP_UNFOLD] = System.currentTimeMillis();
    viewGen = new ViewGeneration2(build);
    for (String[] trace : traces) {
      viewGen.extendByTrace(trace, config.eventToTransition);
    }
    result._time_finish[Result.STEP_UNFOLD] = System.currentTimeMillis();
    result._net_size[Result.STEP_UNFOLD] = build.getStatistics(true);
    debug._lastViewBuild = build;
    
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
   * @return the set of implied conditions in the branching process of the system model
   */
  private HashSet<DNode> findImpliedConditions() {
    
    //boolean printDetail = (build.getBranchingProcess().allConditions.size()
    //    +build.getBranchingProcess().allEvents.size() > 10000) ? true : false;

    Uma.out.println("transitive dependencies..");
    TransitiveDependencies dep = new TransitiveDependencies(build);
    HashSet<DNode> impliedConditions;
    if (config.remove_implied == Configuration.REMOVE_IMPLIED_PRESERVE_CONNECTED2) {
      // locally find all implied conditions, regardless of equivalence classes
      Uma.out.println("find solution..");
      impliedConditions = dep.getAllImpliedConditions();
      //implied = dep.getAllImpliedConditions();
    }
    else 
    if (config.remove_implied == Configuration.REMOVE_IMPLIED_PRESERVE_CONNECTED) {
      // locally find all implied conditions, regardless of equivalence classes
      Uma.out.println("find solution..");
      impliedConditions = dep.getImpliedConditions_solutionLocal();
      //implied = dep.getAllImpliedConditions();
    } else if (config.remove_implied == Configuration.REMOVE_IMPLIED_PRESERVE_VISIBLE) {
      // find equivalence classes of implied conditions
      Uma.out.println("find solution..");
      impliedConditions = dep.getImpliedConditions_solutionGlobal();
    } else if (config.remove_implied == Configuration.REMOVE_IMPLIED_PRESERVE_ALL) {
      // find equivalence classes of implied conditions ensuring that no
      // new behavior is introduced by considering also future behavior
      
      // extend the branching process with events that are enabled, but
      // did not fire
      Uma.out.println("temporarily extend branching process..");
      for (DNode d : build.getBranchingProcess().getAllNodes()) d._isNew = false;
      List<DNode> extendedNodes = dep.extendBranchingProcessWithNextEvents(viewGen.initialState.getAllVisitedMarkings());
      debug._extendedNodes = extendedNodes;
      // identify implied conditions in this branching process
      Uma.out.println("find solution..");
      impliedConditions = dep.getImpliedConditions_solutionGlobal();
      // and remove the added events again
      //Uma.out.println("undo extension..");
      //for (DNode e : extendedNodes) impliedConditions.remove(e);
      //dep.removeExtendedNodes();
    } else {
      impliedConditions = new HashSet<DNode>();
      System.out.println("Error! Unknown mode for finding implied conditions: "+config.remove_implied);
    }
    return impliedConditions;
  }



  
  /**
   * Identify all implied places of the net. The implied places are computed from
   * the set of implied conditions of the branching process ({@link #findImpliedConditions()}).
   * 
   * @param net
   * @param place_conditions
   * @param impliedConditions
   * 
   * @return the identified implied places
   */
  private List<Place> getImpliedPlaces(PetriNet net, Map<Place, Set<DNode> > place_conditions, Set<DNode> impliedConditions)
  {
    debug._impliedConditions = impliedConditions;
    
    boolean findAllImplied;
    HashSet<DNode> implied_local;
    if (config.remove_implied == Configuration.REMOVE_IMPLIED_PRESERVE_CONNECTED
        || config.remove_implied == Configuration.REMOVE_IMPLIED_PRESERVE_CONNECTED2) {
      findAllImplied = false;
      implied_local = new HashSet<DNode>();
    } else {
      findAllImplied = true;
      
      //TransitiveDependencies dep = new TransitiveDependencies(build);
      //implied_local = dep.getImpliedConditions_solutionLocal();
    }
    
    Uma.out.println("remove implied places..");
    LinkedList<Place> impliedPlaces = new LinkedList<Place>();
    result.weak_impliedPlaces = new LinkedList<Place>();
    
    // check for each place whether it is implied, depending
    // on whether all its constituting conditions are implied
    for (Place p : net.getPlaces()) {
      
      boolean place_p_isImplied = findAllImplied;
      
      for (DNode bPrime : place_conditions.get(p)) {

        if (findAllImplied) {
          // remove place only if every condition is implied
          if (!impliedConditions.contains(bPrime)) {
            place_p_isImplied = false;
/*
            // remember those places which could be removed when allowing
            // for a different setting
            for (DNode bPrime2 : place_conditions.get(p)) {
              if (implied_local.contains(bPrime2))
                result.weak_impliedPlaces.add(p);
            }
*/
            break;
          }

        } else {
          // remove place as soon as one condition is implied
          if (impliedConditions.contains(bPrime)) {
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
    //debug._color_weak_impliedPlaces(implied_local);
    
    return impliedPlaces;
  }
  
  /**
   * Remove the given implied places from the net. Does not remove a place
   * if this would disconnect the net (a transition would get an empty
   * pre-set or post-set). Finally removed places are stored in
   * {@link Result#removedImpliedPlaces}.
   * 
   * @param net
   * @param impliedPlaces
   */
  private void removeImpliedPlaces(PetriNet net, List<Place> impliedPlaces) {
    
    result.removedImpliedPlaces = new LinkedList<Place>();
    
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
        //Uma.out.println("removing implied place: "+p+" "+p.getPreSet()+" "+p.getPostSet());
        net.removePlace(p);
        result.removedImpliedPlaces.add(p);
        debug._color_weak_impliedConditions(p);
      }
    }

  }
  

  /**
   * Remove maximal places of the net. Ensure that a transition that
   * has one or more post-places keeps at least one post-place.
   * 
   * @param net
   */
  private void removeMaximalPlaces(PetriNet net) {
    
    result.removedMaximalPlaces = new LinkedList<Place>();
    
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
          //Uma.out.println("removing terminal place: "+p+" "+p.getPreSet());
          net.removePlace(p);
          result.removedMaximalPlaces.add(p);
        }
      }
    }

  }
  

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
        //Uma.out.println("flower place: "+p);
        
        HashSet<Transition> prePost = new HashSet<Transition>(p.getPreSet());
        prePost.retainAll(p.getPostSet());
        
        for (Transition t : prePost) {
          
          //Uma.out.println(t);
          if (t.getIncoming().size() > 1 && t.getOutgoing().size() > 1 ) {
            //Uma.out.println(t.getIncoming());
            //Uma.out.println(t.getOutgoing());
            for (Arc a : t.getIncoming()) {
              if (a.getSource() == p) {
                //Uma.out.println("in "+a.getSource()+" --> "+a.getTarget()); 
                net.removeArc(a); 
                break; 
              }
            }
            for (Arc a : t.getOutgoing()) {
              if (a.getTarget() == p) { 
                //Uma.out.println("out "+a.getSource()+" --> "+a.getTarget()); 
                net.removeArc(a); 
                break; 
              }
            }
          }
        }
      }
    }
  }
  
  /**
   * Object collecting all debug information used in this class.
   */
  public class Debug {
    
    /**
     * the last branching process that was constructed
     */
    public DNodeBP _lastViewBuild = null;
    
    /**
     * list of nodes that were added to the branching process to ensure that
     * only implied conditions that do not change the behavior get removed
     */
    public List<DNode> _extendedNodes = new LinkedList<DNode>();
    
    /**
     * conditions of the branching process identified as implied conditions
     */
    private Set<DNode> _impliedConditions;
    
    public HashMap<Object, String> _weakImpliedPlaces = new HashMap<Object, String>();
    public HashMap<DNode, String> _weakImpliedConditions = new HashMap<DNode, String>();
    
    private void _color_weak_impliedConditions(Place p) {
      for (DNode d : place_conditions.get(p)) {
        _weakImpliedConditions.put(d, "red");
      }
    }

    private void _color_weak_impliedPlaces(HashSet<DNode> implied_local) {
      Set<Place> _removed_weak_impliedPlaces = new HashSet<Place>();
      for (Place p : result.weak_impliedPlaces) {
        boolean isSinglePost = false;
        boolean isSinglePre = false;
        // ensure that a place is only removed if it does not
        // partition the net (by removing the last pre-place
        // or post-place of a transition)
        for (Transition t : p.getPreSet()) {
          int nonRemovedPost = 0;
          // count how many post-places transition 't' still has
          // which are not yet considered as implied places
          for (Place t_post : t.getPostSet()) {
            if (!_removed_weak_impliedPlaces.contains(t_post)) nonRemovedPost++;
          }
          if (nonRemovedPost <= 1) {
            isSinglePost = true;
            break;
          }
        }
        for (Transition t : p.getPostSet()) {
          int nonRemovedPre = 0;
          // count how many pre-places transition 't' still has
          // which are not yet considered as implied places
          for (Place t_pre : t.getPreSet()) {
            if (!_removed_weak_impliedPlaces.contains(t_pre)) nonRemovedPre++;
          }
          if (nonRemovedPre <= 1) {
            isSinglePre = true;
            break;
          }
        }

        if (!isSinglePost && !isSinglePre) {
          _removed_weak_impliedPlaces.add(p);
        }
      }
      
      for (Place p : _removed_weak_impliedPlaces) {
        debug._weakImpliedPlaces.put(p, "orange");
        for (Arc a : p.getIncoming())
          debug._weakImpliedPlaces.put(a, "orange");
        for (Arc a : p.getOutgoing())
          debug._weakImpliedPlaces.put(a, "orange");
        for (DNode d : place_conditions.get(p)) {
          if (implied_local.contains(d))
            debug._weakImpliedConditions.put(d, "orange");
          else
            debug._weakImpliedConditions.put(d, "green");
        }
      }
    }
    
    /**
     * @return map assigning each implied condition a GraphViz color, conditions
     *         of the same equivalence class get the same color
     */
    public HashMap<DNode, String> _getColoringImplied() {
      HashMap<DNode, String> colorMap = new HashMap<DNode, String>();
      LinkedList<Set<DNode>> v = new LinkedList<Set<DNode>>(build.futureEquivalence().values());
      for (int i=0; i<v.size(); i++) {
        for (DNode d : v.get(i)) {
          if (_impliedConditions.contains(d)) {
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
  }

  /**
   * collected debug information
   */
  public Debug debug = new Debug();
  
  

  
  private static float simplestThreshold;
  
  public static void simplify(String system, String log) throws IOException, InvalidModelException {
    simplify(system, log, new Configuration());
  }
  
  public synchronized static void simplify(String system, String log, Configuration config) throws IOException, InvalidModelException {
    
    MineSimplify sim = new MineSimplify(system, log, config);
    sim.prepareModel();
    sim.run();
    
    sim.writeResults();
    Uma.out.println(sim.result.toString());
  }
  
  public static void main(final String args[]) throws Exception {
    
      Configuration config = new Configuration();
      config.unfold_refold = true;
      config.remove_implied = Configuration.REMOVE_IMPLIED_PRESERVE_ALL;
      config.abstract_chains = true;
      config.remove_flower_places = false;
      config.filter_threshold = 0.30;//30;

      simplify(args[0], args[1], config);
  }
}
