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
import hub.top.petrinet.ISystemModel;
import hub.top.petrinet.PetriNet;
import hub.top.petrinet.PetriNetIO_Out;
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
import hub.top.uma.view.ViewGeneration2;
import hub.top.uma.view.ViewGeneration3;

import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.ArrayList;
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

public class Action_Views_UnfoldingView_Modular implements IWorkbenchWindowActionDelegate {

	public static final String ID = "hub.top.uma.ui.view.UnfoldingView_Modular";
	
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
		
    final String fileName_system = selectedFile_system.getRawLocation().toOSString();
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
      final ISystemModel sysModel = Uma.readSystemFromFile(fileName_system);
      if (sysModel instanceof PetriNet)
        ((PetriNet)sysModel).makeNormalNet();
      
      Job bpBuildJob = new Job("Constructing view") 
      {
        @Override
        protected IStatus run(IProgressMonitor monitor) {

          SubMonitor mainMonitor = SubMonitor.convert(monitor, "Constructing view", 1);
          
          ArrayList<String[]> allTraces = new ArrayList<String[]>();
          try {
            allTraces.addAll(ViewGeneration2.readTraces(fileName_trace));
          } catch (FileNotFoundException e) {
            Activator.getPluginHelper().logError("Could not read log file "+fileName_trace+".", e);
            return Status.OK_STATUS;
          }
          
          try {
            
            SubMonitor subMonitor2 = mainMonitor.newChild(1);
            subMonitor2.beginTask(null, allTraces.size());
            
            PetriNet simplestNet = null;
            PetriNet simplestNet_struct = null;
            int bestConfigSize = 0;
            float simplestComplexity = Float.MAX_VALUE;
            float simplestComplexity_struct = Float.MAX_VALUE;
            int simplestTrace = -1;
            
            for (int i=0;i<allTraces.size(); i++) {
              
              if (subMonitor2.isCanceled() || monitor.isCanceled())
                break;
              
              LinkedList<String[]> traceConfig = new LinkedList<String[]>();
              for (int j=0; j<=i;j++)
                traceConfig.add(allTraces.get(j));
              
              PetriNet net = generateView(sysModel, traceConfig);
              if (net != null) {
                
                int fail = validateNet(net, traceConfig);
                if (fail > 0) System.out.println("failed to execute "+fail);
                
                HashSet<String> traceEvents = new HashSet<String>();
                for (String[] trace : traceConfig)  {
                  for (String event : trace) traceEvents.add(event);
                }
                
                PetriNet netStruct = Action_Views_StructuralView_BlockHide.generateView(sysModel, traceEvents, null);
                
                float c = Complexity.complexitySimple(net);
                float cStruct = Complexity.complexitySimple(netStruct);
                
                if (c < simplestComplexity) {
                  simplestNet = net;
                  simplestComplexity = c;
                  simplestTrace = i;
                  
                  simplestComplexity_struct = cStruct;
                  simplestNet_struct = netStruct;
                  
                  bestConfigSize = traceConfig.size();
                  
                  System.out.println("complexity: "+simplestComplexity+" for "+simplestTrace);
                }
                
                System.out.println("complexity: "+c+" for "+i);

                /*
                Collection<Short> complexIDs = lastViewBuild.simplifyFoldingEquivalence();
                HashMap<DNode, String> colorMap = new HashMap<DNode, String>();
                for (DNode d : lastViewBuild.getBranchingProcess().getAllNodes()) {
                  if (complexIDs.contains(d.id)) {
                    colorMap.put(d, "red");
                  }
                }
                */
                
                /*
                IPath targetPath_dot_bp = new Path(selectedFile_system.getFullPath().toString()+".view_unfold_bp_"+i+".dot");
                ActionHelper.writeFile(targetPath_dot_bp, lastViewBuild.toDot());
                */
                IPath targetPath_dot = new Path(selectedFile_system.getFullPath().toString()+".view_unfold_"+i+".dot");
                ActionHelper.writeFile(targetPath_dot, net.toDot());
                /*
                IPath targetPath_lola = new Path(selectedFile_system.getFullPath().toString()+".view_unfold_"+i+".lola");
                ActionHelper.writeFile(targetPath_lola, PetriNetIO.toLoLA(net));
                
                IPath targetPath_dot2 = new Path(selectedFile_system.getFullPath().toString()+".view_unfold_S_"+i+".dot");
                ActionHelper.writeFile(targetPath_dot2, netStruct.toDot());
                
                IPath targetPath_lola2 = new Path(selectedFile_system.getFullPath().toString()+".view_unfold_S_"+i+".lola");
                ActionHelper.writeFile(targetPath_lola2, PetriNetIO.toLoLA(netStruct));
                */
              } else {
                System.out.print("-");
              }
  
              subMonitor2.worked(1);
            }
            
            if (simplestNet != null) {
              System.out.println("best complexity: "+simplestComplexity+" ("+simplestComplexity_struct+") for "+simplestTrace);
            }

          } catch (InvalidModelException e) {
            Activator.getPluginHelper().logError("Invalid system model "+fileName_system+".", e);
            MessageDialog.openError(null, "Could not read model", ((InvalidModelException)e).getMessage());
            return Status.OK_STATUS;
          }

          monitor.done();
          
          return Status.OK_STATUS;
        }
      };
        
      bpBuildJob.setUser(true);
      bpBuildJob.schedule();
      
    } catch (IOException e) {
      Activator.getPluginHelper().logError("Failed to parse "+fileName_system, e);
    }
	}
	
	private static DNodeBP lastViewBuild = null;
	
	private static PetriNet generateView(ISystemModel sysModel, LinkedList<String[]> traces) throws InvalidModelException {
	  
	  System.out.print("g");
	  
    DNodeSys sys = Uma.getBehavioralSystemModel(sysModel);
    DNodeRefold build = Uma.initBuildPrefix_View(sys, 0);
    
    ViewGeneration2 viewGen = new ViewGeneration2(build);
    for (String[] trace : traces) {
      viewGen.extendByTrace(trace, null);
    }
    
    // viewGen.identifyFoldingRelation();
    
    build.futureEquivalence();
    build.extendFutureEquivalence_maximal();
    while (build.extendFutureEquivalence_backwards());
    build.relaxFutureEquivalence();
    
    //build.simplifyFoldingEquivalence();
    
    TransitiveDependencies dep = new TransitiveDependencies(build);
    HashSet<DNode> implied = dep.getImpliedConditions_solutionLocal();
    
    NetSynthesis synth = new NetSynthesis(build);
    PetriNet net = synth.foldToNet_labeled();
    
    LinkedList<Place> impliedPlaces = new LinkedList<Place>();
    for (Place p : net.getPlaces()) {
      boolean allImplied = true;
      DNode b = synth.n2d.get(p);
      for (DNode bPrime : build.futureEquivalence().get(build.equivalentNode().get(b))) {
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
      for (Transition t : p.getPreSet()) {
        if (t.getOutgoing().size() == 1) {
          isSinglePost = true;
          break;
        }
      }
      if (!isSinglePost)
        net.removePlace(p);
    }
    
    for (Transition t : net.getTransitions()) {
      LinkedList<Place> maxPlaces = new LinkedList<Place>();
      for (Place p : t.getPostSet()) {
        if (p.getOutgoing().isEmpty())
          maxPlaces.add(p);
      }
      if (!maxPlaces.isEmpty() && maxPlaces.size() == t.getPostSet().size())
        maxPlaces.removeLast();
      for (Place p : maxPlaces)
        net.removePlace(p);
    }

    lastViewBuild = build;
    
    return net;
	}
	
	private static void generateAndWriteView(IProgressMonitor monitor, ISystemModel sysModel, LinkedList<String[]> traces, String sourceFileName) throws InvalidModelException {

    DNodeSys sys = Uma.getBehavioralSystemModel(sysModel);
    DNodeRefold build = Uma.initBuildPrefix_View(sys, 0);
    
    ViewGeneration2 viewGen = new ViewGeneration2(build);
    for (String[] trace : traces) {
      viewGen.extendByTrace(trace, null);
      if (monitor.isCanceled()) {
        break;
      }
    }
    
    viewGen.identifyFoldingRelation();
    
    build.futureEquivalence();
    build.extendFutureEquivalence_maximal();
    while (build.extendFutureEquivalence_backwards());
    build.relaxFutureEquivalence();
    
    TransitiveDependencies dep = new TransitiveDependencies(build);
    HashSet<DNode> implied = dep.getImpliedConditions_solutionLocal();
    
    NetSynthesis synth = new NetSynthesis(build);
    PetriNet net = synth.foldToNet_labeled();
    
    LinkedList<Place> someImpliedPlaces = new LinkedList<Place>();
    LinkedList<Place> impliedPlaces = new LinkedList<Place>();
    for (Place p : net.getPlaces()) {
      boolean allImplied = true;
      boolean someImplied = false;
      DNode b = synth.n2d.get(p);
      for (DNode bPrime : build.futureEquivalence().get(build.equivalentNode().get(b))) {
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
      for (DNode bPrime : build.futureEquivalence().get(build.equivalentNode().get(b))) {
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
    ActionHelper.writeFile(targetPath_lola, PetriNetIO_Out.toLoLA(net));
	}
	
	private static int validateNet(PetriNet net, LinkedList<String[]> traces) {
    ViewGeneration3 validate = new ViewGeneration3(net);
    int failed = 0;
    for (String[] trace : traces) {
      if (!validate.validateTrace(trace)) failed++;
    }
    return failed;
	}
	
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
