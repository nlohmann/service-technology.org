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
import hub.top.petrinet.util.StructuralReduction;
import hub.top.uma.DNode;
import hub.top.uma.DNodeRefold;
import hub.top.uma.DNodeSet.DNodeSetElement;
import hub.top.uma.DNodeSys;
import hub.top.uma.InvalidModelException;
import hub.top.uma.Uma;
import hub.top.uma.synthesis.NetSynthesis;
import hub.top.uma.synthesis.TransitiveDependencies;
import hub.top.uma.view.ViewGeneration;
import hub.top.uma.view.ViewGeneration2;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.LinkedList;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.jobs.Job;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.dialogs.IInputValidator;
import org.eclipse.jface.dialogs.InputDialog;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.IWorkbenchWindowActionDelegate;

import com.google.gwt.dev.util.collect.HashSet;

public class Action_Views_GenerateView_BlockHide implements IWorkbenchWindowActionDelegate {

	public static final String ID = "hub.top.uma.ui.view.GenerateView_BlockHide";
	
	private IWorkbenchWindow workbenchWindow;

	// fields for tracking the selection in the explorer 
	private IFile  selectedFile_system = null;
	private LinkedList<IFile> selectedFile_log = null;
	private LinkedList<IFile> selectedFile_events = null;
	
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
		
    String fileName_system = selectedFile_system.getRawLocation().toOSString();
    
    try {
      
      IInputValidator intValidator = new IInputValidator() {
        
        public String isValid(String newText) {
          if (newText == null || newText.length() == 0)
            return " ";
          try {
            int val = Integer.parseInt(newText);
            if (val < Integer.MIN_VALUE) return "Number out of bounds.";
            if (val > Integer.MAX_VALUE) return "Number out of bounds.";
            return null;
          } catch (NumberFormatException e) {
            return "Please enter an integer number.";
          }
        }
      };
      
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
      
      InputDialog boundDiag = new InputDialog(null,
          "Generate view for "+selectedFile_system.getName(),
          "bound k = ..", "1", intValidator);
      if (boundDiag.open() == InputDialog.CANCEL)
        return;
      final int bound = Integer.parseInt(boundDiag.getValue());
      
      InputDialog minTraceCoverageDiag = new InputDialog(null,
          "Generate view for "+selectedFile_system.getName(),
          "fraction of traces to be represented by the view between 0.0 and 1.0", "1", float_0_1_Validator);
      if (minTraceCoverageDiag.open() == InputDialog.CANCEL)
        return;
      final float coverTracePercentage = Float.parseFloat(minTraceCoverageDiag.getValue());
      
      InputDialog minEventCoverageDiag = new InputDialog(null,
          "Generate view for "+selectedFile_system.getName(),
          "fraction of events to be represented by the view between 0.0 and 1.0", "1", float_0_1_Validator);
      if (minEventCoverageDiag.open() == InputDialog.CANCEL)
        return;
      final float coverEventPercentage = Float.parseFloat(minEventCoverageDiag.getValue());

      
      final ArrayList<String[]> traces = new ArrayList<String[]>();
      int fileCount = 0;
      for (IFile file : selectedFile_log) {
        String fileName = file.getRawLocation().toOSString();
        traces.addAll(ViewGeneration2.readTraces(fileName));
      }
      
      HashSet<String> events = new HashSet<String>();
      fileCount = 0;
      for (IFile file : selectedFile_events) {
        String fileName = file.getRawLocation().toOSString();
        events.addAll(ViewGeneration2.readEvents(fileName));
      }
      final ArrayList<String> eventSets = new ArrayList<String>(events);

      String systemFileName = selectedFile_system.getRawLocation().toOSString();
      final ISystemModel sysModel = Uma.readSystemFromFile(systemFileName);
      
      Job bpBuildJob = new Job("constructing view") 
      {
        @Override
        protected IStatus run(IProgressMonitor monitor) {
          
          try {
            boolean[] traceConfigCounter = new boolean[traces.size()+1];
            int doneTrace = traceConfigCounter.length-1;
            int total = 1 << doneTrace;
            
            monitor.beginTask("exploring "+total+" views", total);
            
            PetriNet simplestNet = null;
            float simplestComplexity = Float.MAX_VALUE;
            boolean simplestTraceConfig[];
            boolean simplestEventConfig[];
            
            while (!traceConfigCounter[doneTrace]) {
              if (monitor.isCanceled())
                break;
              
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
              for (int i=0; i < traces.size(); i++) {
                if (traceConfigCounter[i]) traceConfig.add(traces.get(i));
              }
              
              // skip all configurations having not enough traces
              if (((float)traceConfig.size())/((float)traces.size()) < coverTracePercentage) {
                monitor.worked(1);
                continue;
              }
              
              boolean[] eventConfigCounter = new boolean[eventSets.size()+1];
              int doneEvent = eventConfigCounter.length-1;
              int totalEvent = 1 << doneEvent;
              int stepEvent = 0;
              
              while (!eventConfigCounter[doneEvent]) {
                if (monitor.isCanceled())
                  break;
                
                for (int i=0;i<eventConfigCounter.length;i++) {
                  if (!eventConfigCounter[i]) {
                    eventConfigCounter[i] = true;
                    break;
                  } else {
                    eventConfigCounter[i] = false;
                  }
                }
                if (eventConfigCounter[doneEvent]) break;
                              
                LinkedList<String> eventConfig = new LinkedList<String>();
                for (int i=0; i < eventSets.size(); i++) {
                  if (eventConfigCounter[i]) eventConfig.add(eventSets.get(i));
                }
                
                // skip all configurations having not enough traces
                if (((float)eventConfig.size())/((float)eventSets.size()) < coverEventPercentage) {
                  stepEvent++;
                  continue;
                }
                
                monitor.subTask("event view "+stepEvent+"/"+totalEvent);
                
                PetriNet net = generateView(sysModel, bound, traceConfig, eventConfig);
                float c = Complexity.complexitySimple(net);
                
                if (c < simplestComplexity) {
                  simplestNet = net;
                  simplestComplexity = c;
                  simplestTraceConfig = Arrays.copyOf(traceConfigCounter, traceConfigCounter.length-1);
                  simplestEventConfig = Arrays.copyOf(eventConfigCounter, eventConfigCounter.length-1);
                  
                  System.out.println("complexity: "+c+" for "+ViewGeneration.toString(traceConfigCounter)+" "+ViewGeneration.toString(eventConfigCounter));
                }
                stepEvent++;
              }
  
              monitor.worked(1);
            }
  
            IPath targetPath_lola = new Path(selectedFile_system.getFullPath().toString()+".view_block_hide.lola");
            IPath targetPath_dot = new Path(selectedFile_system.getFullPath().toString()+".view_block_hide.dot");
            ActionHelper.writeFile(targetPath_lola, PetriNetIO_Out.toLoLA(simplestNet));
            ActionHelper.writeFile(targetPath_dot, simplestNet.toDot());
          } catch (IOException e) {
            Activator.getPluginHelper().logError("Could not read model file: "+selectedFile_system, e);
          } catch (InvalidModelException e) {
            Activator.getPluginHelper().logError("Invalid model in "+selectedFile_system, e);
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
    /*
    catch (InvalidModelException e) {
      Activator.getPluginHelper().logError("Failed to parse "+fileName_system, e);
      MessageDialog.openError(null, "Could not read model", ((InvalidModelException)e).getMessage());
    }
    */
	}
	
	private static PetriNet generateView(ISystemModel sysModel, int bound,
	    LinkedList<String[]> traces, LinkedList<String> events) throws IOException, InvalidModelException
	{
    DNode.idGen = 0;
	  
	  DNodeSys sys = Uma.getBehavioralSystemModel(sysModel);
	  DNodeRefold build = Uma.initBuildPrefix_View(sys, bound);
	  
    while (build.step() > 0);
    ViewGeneration2 viewGen = new ViewGeneration2(build);
    DNodeSetElement nonBlocked = viewGen.getNonBlockedNodes(traces, 0.0f);
    
    TransitiveDependencies dep = new TransitiveDependencies(build);
    HashSet<DNode> implied = dep.getImpliedConditions_solutionLocal();
    
    NetSynthesis synth = new NetSynthesis(build);
    PetriNet net = synth.foldToNet_labeled(nonBlocked, true);
    
    for (Transition t : net.getTransitions()) {
      if (!events.contains(t.getName())) {
        t.setTau(true);
        t.setName("tau");
      }
    }
    
    LinkedList<Place> impliedPlaces = new LinkedList<Place>();
    for (Place p : net.getPlaces()) {
      boolean allImplied = true;
      DNode b = synth.n2d.get(p);
      for (DNode bPrime : build.futureEquivalence().get(build.equivalentNode().get(b))) {
        if (!implied.contains(bPrime)) {
          allImplied = false;
          break;
        }
      }
      if (allImplied) {
        impliedPlaces.add(p);
      }
    }
    
    for (Place p : impliedPlaces)
      net.removePlace(p);
    
    StructuralReduction red = new StructuralReduction(net);
    red.reduce();
    
    return net;
	}
	
  public void selectionChanged(IAction action, ISelection selection) {

    selectedFile_system = null;
    selectedFile_log = new LinkedList<IFile>();
    selectedFile_events = new LinkedList<IFile>();

    action.setEnabled(false);
    if (selection instanceof IStructuredSelection == false)
    {
      return;
    }
    
    try {
      Object[] selected = ((IStructuredSelection) selection).toArray();
      if (selected.length >= 3) {
      
        String[] sysExt = new String[] { "oclets", "lola", "tpn" };
        String[] logExt = new String[] { "txt" };
        
        LinkedList<IFile> systemFiles = ActionHelper.filterFiles(selected, sysExt);
        LinkedList<IFile> logFiles = ActionHelper.filterFiles(selected, logExt);
        
        if (systemFiles.size() == 1 && logFiles.size() >= 2) {
          selectedFile_system = systemFiles.getFirst();
          for (IFile file : logFiles) {
            if (file.getFullPath().toString().endsWith(".log.txt"))
              selectedFile_log.add(file);
            if (file.getFullPath().toString().endsWith(".events.txt"))
              selectedFile_events.add(file);
          }
          if (selectedFile_events.size() == 0 || selectedFile_log.size() == 0)
            return;
          
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
