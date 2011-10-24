/*****************************************************************************\
 * Copyright (c) 2008, 2009 Dirk Fahland. All rights reserved. EPL1.0/AGPL3.0
 * 
 * ServiceTechnolog.org - Greta
 *                       (Graphical Runtime Environment for Adaptive Processes) 
 * 
 * This program and the accompanying materials are made available under
 * the terms of the Eclipse Public License v1.0, which accompanies this
 * distribution, and is available at http://www.eclipse.org/legal/epl-v10.html
 * 
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 * 
 * The Original Code is this file as it was released on December 04, 2008.
 * The Initial Developer of the Original Code are
 *    Manja Wolf
 *    Dirk Fahland
 * 
 * Portions created by the Initial Developer are Copyright (c) 2008, 2009
 * the Initial Developer. All Rights Reserved.
 * 
 * Contributor(s):
 * 
 * Alternatively, the contents of this file may be used under the terms of
 * the GNU Affero General Public License Version 3 or later (the "GPL") in
 * which case the provisions of the AGPL are applicable instead of those above.
 * If you wish to allow use of your version of this file only under the terms
 * of the AGPL and not to allow others to use your version of this file under
 * the terms of the EPL, indicate your decision by deleting the provisions
 * above and replace them with the notice and other provisions required by the 
 * AGPL. If you do not delete the provisions above, a recipient may use your
 * version of this file under the terms of any one of the EPL or the AGPL.
\*****************************************************************************/

package hub.top.uma.ui;

import hub.top.editor.eclipse.ActionHelper;
import hub.top.petrinet.Arc;
import hub.top.petrinet.ISystemModel;
import hub.top.petrinet.PetriNet;
import hub.top.petrinet.PetriNetIO;
import hub.top.petrinet.Place;
import hub.top.petrinet.Transition;
import hub.top.petrinet.util.Complexity;
import hub.top.uma.DNode;
import hub.top.uma.DNodeBP;
import hub.top.uma.DNodeRefold;
import hub.top.uma.DNodeSys;
import hub.top.uma.InvalidModelException;
import hub.top.uma.Uma;
import hub.top.uma.synthesis.NetSynthesis;
import hub.top.uma.synthesis.TransitiveDependencies;
import hub.top.uma.view.MineSimplify;
import hub.top.uma.view.ViewGeneration;
import hub.top.uma.view.ViewGeneration2;

import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.LinkedList;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.SubMonitor;
import org.eclipse.core.runtime.jobs.Job;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.dialogs.IInputValidator;
import org.eclipse.jface.dialogs.InputDialog;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.IWorkbenchWindowActionDelegate;

import com.google.gwt.dev.util.collect.HashSet;

public class Action_Views_UnfoldingView_Block implements IWorkbenchWindowActionDelegate {

	public static final String ID = "hub.top.uma.ui.view.UnfoldingView_Block";
	
	private IWorkbenchWindow workbenchWindow;

	// fields for tracking the selection in the explorer 
	private IFile  selectedFile_system = null;
	private IFile  selectedFile_log = null;
	
	public void dispose() {
	}

	public void init(IWorkbenchWindow window) {
		workbenchWindow = window;
	}
	

	public void run(IAction action) {
		if (!action.getId().equals(ID))
			return;
		if (!action.isEnabled())
		  return;
		
    final String fileName_system_sysPath = selectedFile_system.getRawLocation().toOSString();
    final IPath fileName_system_wsPath = selectedFile_system.getFullPath();
    final String fileName_trace = selectedFile_log.getRawLocation().toOSString();
    
    try {
      
      IInputValidator float_0_1_Validator = new IInputValidator() {
        
        public String isValid(String newText) {
          if (newText == null || newText.length() == 0)
            return " ";
          try {
            float val = Float.parseFloat(newText);
            if (val < 0) return "Number out of bounds.";
            if (val > 1) return "Number out of bounds.";
            return null;
          } catch (NumberFormatException e) {
            return "Please enter a number between 0.0 and 1.0";
          }
        }
      };
      
      InputDialog minTraceCoverageDiag = new InputDialog(null,
          "Generate view for "+selectedFile_system.getName(),
          "fraction of traces to be represented by the view between 0.0 and 1.0", "1", float_0_1_Validator);
      if (minTraceCoverageDiag.open() == InputDialog.CANCEL)
        return;
      final float coverTracePercentage = Float.parseFloat(minTraceCoverageDiag.getValue());
      
      DNode.idGen = 0;
      final ISystemModel sysModel = Uma.readSystemFromFile(fileName_system_sysPath);
      if (sysModel instanceof PetriNet) {
        ((PetriNet)sysModel).makeNormalNet();
        ((PetriNet)sysModel).turnIntoLabeledNet();
        
        for (Transition t : ((PetriNet)sysModel).getTransitions())
          System.out.println(t+" "+t.getPostSet());
      }
      
      Job bpBuildJob = new Job("Constructing view") 
      {
        @Override
        protected IStatus run(IProgressMonitor monitor) {
          
          UmaUI.getUI().switchOutStream_uiConsole();
          SubMonitor mainMonitor = SubMonitor.convert(monitor, "Constructing view", 2);
          
          ArrayList<String[]> allTraces = new ArrayList<String[]>();
          try {
            allTraces.addAll(ViewGeneration2.readTraces(fileName_trace));
          } catch (FileNotFoundException e) {
            Activator.getPluginHelper().logError("Could not read log file "+fileName_trace+".", e);
            return Status.OK_STATUS;
          }
          
          try {
            

            DNodeSys sys = Uma.getBehavioralSystemModel(sysModel);
            DNodeRefold build = Uma.initBuildPrefix_View(sys, 0);
            ViewGeneration2 viewGen = new ViewGeneration2(build);

            SubMonitor subMonitor = mainMonitor.newChild(1);
            subMonitor.beginTask(null, allTraces.size());
            for (String[] trace : allTraces) {
              viewGen.extendByTrace(trace, null);
              if (monitor.isCanceled() || subMonitor.isCanceled()) {
                break;
              }
              subMonitor.subTask(build.getBranchingProcess().allConditions.size()+" / "+build.getBranchingProcess().allEvents.size());
              subMonitor.worked(1);
              
              //if (build.getBranchingProcess().allEvents.size() > 1000) break;
            }
            
            Uma.out.println("built "+build.getBranchingProcess().allConditions.size()+" / "+build.getBranchingProcess().allEvents.size());

            LinkedList<String[]>[] traceClasses = viewGen.accumulateEquivalentTraces(10);
            for (int i=0; i< traceClasses.length; i++) {
              Uma.out.print(traceClasses[i].size()+" ");
            }
            Uma.out.println();
            
            boolean[] traceConfigCounter = new boolean[traceClasses.length+1];
            int doneTrace = traceConfigCounter.length-1;
            int total = (1 << doneTrace) - 1;
            int explored = 0;
            
            PetriNet simplestNet = null;
            PetriNet simplestNet_struct = null;
            int bestConfigSize = 0;
            float simplestComplexity = Float.MAX_VALUE;
            float simplestComplexity_struct = Float.MAX_VALUE;
            boolean simplestTraceConfig[] = null;
            boolean simplestEventConfig[];
            //int simplestFail = 0;
            
            //DNodeBP simplestBP = null;
            
            SubMonitor subMonitor2 = mainMonitor.newChild(1);
            subMonitor2.beginTask(null, total);
            
            UmaUI.getUI().monitorHook = subMonitor2;

            while (!traceConfigCounter[doneTrace]) {
              if (monitor.isCanceled() || subMonitor2.isCanceled())
                break;
              
              UmaUI.getUI().monitorHook_subTask = "explored "+explored+"/"+total+", best complexity: "+simplestComplexity+" covering "+((float)bestConfigSize/((float)allTraces.size())); 
              subMonitor2.subTask(UmaUI.getUI().monitorHook_subTask);
              explored++;
              
              for (int i=0;i<traceConfigCounter.length;i++) {
                if (!traceConfigCounter[i]) {
                  traceConfigCounter[i] = true;
                  break;
                } else {
                  traceConfigCounter[i] = false;
                }
              }
              if (traceConfigCounter[doneTrace]) break;
                            
              LinkedList<String[]> traceConfig = new LinkedList<String[]>();
              for (int i=0; i < traceClasses.length; i++) {
                if (traceConfigCounter[i]) traceConfig.addAll(traceClasses[i]);
              }
              
              // skip all configurations having not enough traces
              if (((float)traceConfig.size())/((float)allTraces.size()) < coverTracePercentage) {
                subMonitor2.worked(1);
                continue;
              }
              
              //PetriNet net = generateView2(sysModel, traceConfig, true);
              
              
              MineSimplify simplify = new MineSimplify(sysModel, traceConfig);
              simplify.run();
              PetriNet net = simplify.getSimplifiedNet();
              
              if (net != null) {
                final HashSet<String> traceEvents = new HashSet<String>();
                for (String[] trace : traceConfig)  {
                  for (String event : trace) traceEvents.add(event);
                }

                
                PetriNet netStruct = Action_Views_StructuralView_BlockHide.generateView(sysModel, traceEvents, null);
                
                
                float c = Complexity.complexitySimple(net);
                float cStruct = Complexity.complexitySimple(netStruct);
                
                if (c < simplestComplexity) {
                  simplestNet = net;
                  simplestComplexity = c;
                  simplestTraceConfig = Arrays.copyOf(traceConfigCounter, traceConfigCounter.length-1);
                  
                  simplestComplexity_struct = cStruct;
                  simplestNet_struct = netStruct;
                  
                  //simplestBP = lastViewBuild;
                  //simplestFail = fail;
                  //simplestColorMap = colorMap2;
                  
                  bestConfigSize = traceConfig.size();
                  
                  Uma.out.println("complexity: "+c+" for "+ViewGeneration.toString(traceConfigCounter));
                  //break;
                }
              } else {
                Uma.out.print("-");
              }
  
              subMonitor2.worked(1);
            }
            
            if (simplestNet != null) {
              
              int cov = (int)((float)bestConfigSize*100/((float)allTraces.size()));
              
              //IPath targetPath_dot_bp = new Path(fileName_system_wsPath.toString()+".view_unfold_"+cov+"_bp.dot");
              //ActionHelper.writeFile(targetPath_dot_bp, simplestBP.toDot());
              
              IPath targetPath_dot = new Path(fileName_system_wsPath.toString()+".view_unfold_"+cov+".dot");
              ActionHelper.writeFile(targetPath_dot, simplestNet.toDot());
              
              IPath targetPath_lola = new Path(fileName_system_wsPath.toString()+".view_unfold_"+cov+".lola");
              ActionHelper.writeFile(targetPath_lola, PetriNetIO.toLoLA(simplestNet));
              
              IPath targetPath_dot2 = new Path(fileName_system_wsPath.toString()+".view_unfold_"+cov+"_S.dot");
              ActionHelper.writeFile(targetPath_dot2, simplestNet_struct.toDot());
              /*
              IPath targetPath_lola2 = new Path(fileName_system_wsPath.toString()+".view_unfold_S.lola");
              ActionHelper.writeFile(targetPath_lola2, PetriNetIO.toLoLA(simplestNet_struct));
              */
              IPath targetPath_result = new Path(fileName_system_wsPath.toString()+".view_unfold_"+cov+".result.txt");
              String result_string = "";
              result_string += "best complexity: "+simplestComplexity+" (structural: "+simplestComplexity_struct+")\n";
              result_string += "covering: "+((float)bestConfigSize/((float)allTraces.size()))+"\n";
              //result_string += "failed to execute: "+simplestFail;
              ActionHelper.writeFile(targetPath_result, result_string);
              
              IPath targetPath_covered = new Path(fileName_system_wsPath.toString()+".view_unfold_"+cov+".log.txt");
              LinkedList<String[]> coveredConfig = new LinkedList<String[]>();
              for (int i=0; i < traceClasses.length; i++) {
                if (simplestTraceConfig[i]) coveredConfig.addAll(traceClasses[i]);
              }
              ActionHelper.writeFile(targetPath_covered, ViewGeneration2.generateSimpleLog(coveredConfig).toString());
            }

          } catch (InvalidModelException e) {
            Activator.getPluginHelper().logError("Invalid system model "+fileName_system_sysPath+".", e);
            MessageDialog.openError(null, "Could not read model", ((InvalidModelException)e).getMessage());
            return Status.OK_STATUS;
          }
          
          UmaUI.getUI().switchOutStream_System();
          monitor.done();
          
          return Status.OK_STATUS;
        }
      };
        
      bpBuildJob.setUser(true);
      bpBuildJob.schedule();
      
    } catch (IOException e) {
      Activator.getPluginHelper().logError("Failed to parse "+fileName_system_sysPath, e);
    }
	}
	
	// private static DNodeBP lastViewBuild = null;
	
	/*
	private static PetriNet generateView(ISystemModel sysModel, LinkedList<String[]> traces) throws InvalidModelException {
	  
	  System.out.print("g");
	  
    DNodeSys sys = Uma.getBehavioralSystemModel(sysModel);
    DNodeBP build = Uma.initBuildPrefix(sys, 0);
    
    ViewGeneration2 viewGen = new ViewGeneration2(build);
    for (String[] trace : traces) {
      viewGen.extendByTrace(trace);
    }
    
    viewGen.identifyFoldingRelation();
    
    build.foldingEquivalence();
    build.extendFoldingEquivalence_maximal();
    while (build.extendFoldingEquivalence_backwards());
    build.relaxFoldingEquivalence();
    
    //build.simplifyFoldingEquivalence();
    
    TransitiveDependencies dep = new TransitiveDependencies(build);
    HashSet<DNode> implied = dep.getImpliedConditions_solution();
    
    NetSynthesis synth = new NetSynthesis(build);
    PetriNet net = synth.foldToNet_labeled();
    
    LinkedList<Place> impliedPlaces = new LinkedList<Place>();
    for (Place p : net.getPlaces()) {
      boolean allImplied = true;
      DNode b = synth.n2d.get(p);
      for (DNode bPrime : build.foldingEquivalence().get(build.equivalentNode().get(b))) {
        if (!implied.contains(bPrime)) {
          allImplied = false;
        }
      }
      if (allImplied) {
        impliedPlaces.add(p);
      }
    }
    
    for (Place p : impliedPlaces) {
      net.removePlace(p);
    }

    lastViewBuild = build;
    
    return net;
	}
	*/
	
	//private static HashMap<Object, String> colorMap2;
	
	/*
	private static PetriNet generateView2(ISystemModel sysModel, LinkedList<String[]> traces,
    boolean abstractCycles) throws InvalidModelException {
    
    DNodeSys sys = Uma.getBehavioralSystemModel(sysModel);
    DNodeBP build = Uma.initBuildPrefix_View(sys, 0);
    
    Uma.out.println("generating view for "+traces.size()+" traces...");
    
    ViewGeneration2 viewGen = new ViewGeneration2(build);
    for (String[] trace : traces) {
      viewGen.extendByTrace(trace);
    }
    boolean printDetail = (build.getBranchingProcess().allConditions.size()
        +build.getBranchingProcess().allEvents.size() > 10000) ? true : false;
    
    // viewGen.identifyFoldingRelation();
    
    build.debugPrintCCpairs();
    
    if (printDetail) Uma.out.println("equivalence..");
    build.foldingEquivalence();
    
    build.debug_printFoldingEquivalence();
    
    if (printDetail) Uma.out.println("join maximal..");
    build.extendFoldingEquivalence_maximal();
    if (printDetail) Uma.out.println("fold backwards..");
    while (build.extendFoldingEquivalence_backwards()) {
      if (printDetail) Uma.out.println("fold backwards..");
    }
    if (printDetail) Uma.out.println("relax..");
    build.relaxFoldingEquivalence();
    if (printDetail) Uma.out.println("determinize..");
    //while (build.extendFoldingEquivalence_deterministic()) {
    //  if (printDetail) Uma.out.println("determinize..");
    //}
    
    //build.simplifyFoldingEquivalence();
    
    if (printDetail) Uma.out.println("transitive dependencies..");
    TransitiveDependencies dep = new TransitiveDependencies(build);
    if (printDetail) Uma.out.println("find solution..");
    HashSet<DNode> implied = dep.getImpliedConditions_solution();
    
    if (printDetail) Uma.out.println("fold net..");
    NetSynthesis synth = new NetSynthesis(build);
    PetriNet net = synth.foldToNet_labeled(false);
    
    if (printDetail) Uma.out.println("remove implied places..");
    LinkedList<Place> impliedPlaces = new LinkedList<Place>();
    for (Place p : net.getPlaces()) {
      boolean allImplied = true;
      DNode b = synth.n2d.get(p);
      for (DNode bPrime : build.foldingEquivalence().get(build.equivalentNode().get(b))) {
        
        //if (!implied.contains(bPrime)) {
        //  allImplied = false;
        //}
        
        if (implied.contains(bPrime)) {
          impliedPlaces.add(p);
          break;
        }
      }
      
      //if (allImplied) {
      //  impliedPlaces.add(p);
      //}
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
        //Uma.out.println("removing implied place: "+p);
        net.removePlace(p);
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
    
    lastViewBuild = build;

    if (abstractCycles) {
      while (collapseChains(net));
    }
    
    removeFlowerPlaces(net);
    
    Uma.out.println("done");
    
    return net;
  }
  */
	
	private static HashMap<Object, String> _debug_cycleAbstraction_coloring;
	
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
	
	private static void removeFlowerPlaces(PetriNet net) {
	  
	  LinkedList<Place> places = new LinkedList<Place>(net.getPlaces());
	  for (Place p : places) {
	    if (   p.getIncoming().size() > net.getTransitions().size() / 10
	        && p.getOutgoing().size() > net.getTransitions().size() / 10) {
	      Uma.out.println("flower place: "+p);
	      
	      HashSet<Transition> prePost = new HashSet<Transition>(p.getPreSet());
	      prePost.retainAll(p.getPostSet());
	      
	      for (Transition t : prePost) {
	        
	        Uma.out.println(t);
	        if (t.getIncoming().size() > 1 /*&& t.getOutgoing().size() > 1*/ ) {
  	        for (Arc a : t.getIncoming()) {
  	          if (a.getSource() == p) { net.removeArc(a); Uma.out.println("in "); break; }
  	        }
            for (Arc a : t.getOutgoing()) {
              if (a.getTarget() == p) { net.removeArc(a); Uma.out.println("out "); break; }
            }
	        }
	      }
	    }
	  }
	}

	/*
	private static void generateAndWriteView(IProgressMonitor monitor, ISystemModel sysModel, LinkedList<String[]> traces, String sourceFileName) throws InvalidModelException {

    DNodeSys sys = Uma.getBehavioralSystemModel(sysModel);
    DNodeBP build = Uma.initBuildPrefix(sys, 0);
    
    ViewGeneration2 viewGen = new ViewGeneration2(build);
    for (String[] trace : traces) {
      viewGen.extendByTrace(trace);
      if (monitor.isCanceled()) {
        break;
      }
    }
    
    viewGen.identifyFoldingRelation();
    
    build.foldingEquivalence();
    build.extendFoldingEquivalence_maximal();
    while (build.extendFoldingEquivalence_backwards());
    build.relaxFoldingEquivalence();
    while (build.extendFoldingEquivalence_deterministic());
    
    TransitiveDependencies dep = new TransitiveDependencies(build);
    HashSet<DNode> implied = dep.getImpliedConditions_solution();
    
    NetSynthesis synth = new NetSynthesis(build);
    PetriNet net = synth.foldToNet_labeled();
    
    LinkedList<Place> someImpliedPlaces = new LinkedList<Place>();
    LinkedList<Place> impliedPlaces = new LinkedList<Place>();
    for (Place p : net.getPlaces()) {
      boolean allImplied = true;
      boolean someImplied = false;
      DNode b = synth.n2d.get(p);
      for (DNode bPrime : build.foldingEquivalence().get(build.equivalentNode().get(b))) {
        if (!implied.contains(bPrime)) {
          allImplied = false;
        } else {
          someImplied = true;
        }
      }
      if (allImplied) {
        impliedPlaces.add(p);
      } else if (someImplied) {
        someImpliedPlaces.add(p);
      }
    }
    
    HashMap<DNode, String> colorMap = new HashMap<DNode, String>();
    HashMap<Object, String> colorMap2 = new HashMap<Object, String>();
    
    for (Place p : impliedPlaces) {
      //net.removePlace(p);
      DNode b = synth.n2d.get(p);
      for (DNode bPrime : build.foldingEquivalence().get(build.equivalentNode().get(b))) {
        colorMap.put(bPrime, "maroon1");
      }
      colorMap2.put(p, "maroon1");
    }
    for (Place p : someImpliedPlaces) {
      colorMap2.put(p, "pink");
    }
    
    monitor.subTask("validating view");
    int failed = validateNet(net, traces);
    if (failed > 0) {
      MessageDialog.openError(null, "Constructing unfolding-based view", "Could not execute "+failed+" of "+traces.size()+" traces");
    }
      
    IPath targetPath_dot = new Path(sourceFileName+".view_unfold_bp.dot");
    ActionHelper.writeFile(targetPath_dot, build.toDot(colorMap));
    
    IPath targetPath_dot2 = new Path(sourceFileName+".view_unfold.dot");
    ActionHelper.writeFile(targetPath_dot2, net.toDot(colorMap2));
    
    IPath targetPath_lola = new Path(sourceFileName+".view_unfold.lola");
    ActionHelper.writeFile(targetPath_lola, PetriNetIO.toLoLA(net));
	}
	
	private static int validateNet(PetriNet net, LinkedList<String[]> traces) {
    ViewGeneration3 validate = new ViewGeneration3(net);
    int failed = 0;
    for (String[] trace : traces) {
      if (!validate.validateTrace(trace)) failed++;
    }
    return failed;
	}
  */
	
  public void selectionChanged(IAction action, ISelection selection) {

    selectedFile_system = null;
    selectedFile_log = null;

    action.setEnabled(false);
    if (selection instanceof IStructuredSelection == false)
    {
      return;
    }
    
    try {
      Object[] selected = ((IStructuredSelection) selection).toArray();
      if (selected.length == 2) {
      
        String[] sysExt = new String[] { "oclets", "lola", "tpn" };
        String[] logExt = new String[] { "txt" };
        
        LinkedList<IFile> systemFiles = ActionHelper.filterFiles(selected, sysExt);
        LinkedList<IFile> logFiles = ActionHelper.filterFiles(selected, logExt);
        
        if (systemFiles.size() == 1 && logFiles.size() == 1) {
          selectedFile_system = systemFiles.getFirst();
          selectedFile_log = logFiles.getFirst();
          action.setEnabled(true);
        }
      }
        
    } catch (ClassCastException e) {
      // just catch, do nothing
    } catch (NullPointerException e) {
      // just catch, do nothing
    }
  }
}
