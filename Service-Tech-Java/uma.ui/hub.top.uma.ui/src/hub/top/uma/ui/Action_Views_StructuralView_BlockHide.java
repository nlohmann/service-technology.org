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
import hub.top.petrinet.PetriNetIO;
import hub.top.petrinet.Transition;
import hub.top.petrinet.util.Complexity;
import hub.top.petrinet.util.StructuralReduction;
import hub.top.uma.InvalidModelException;
import hub.top.uma.Uma;
import hub.top.uma.view.ViewGeneration;
import hub.top.uma.view.ViewGeneration2;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.HashSet;
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

public class Action_Views_StructuralView_BlockHide implements IWorkbenchWindowActionDelegate {

	public static final String ID = "hub.top.uma.ui.view.StructuralView_BlockHide";
	
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
      
      HashSet<String> hideEvents = new HashSet<String>();
      for (IFile file : selectedFile_events) {
        String fileName = file.getRawLocation().toOSString();
        hideEvents.addAll(ViewGeneration2.readEvents(fileName));
      }
      final ArrayList<String> hideEventSets = new ArrayList<String>(hideEvents);

      String systemFileName = selectedFile_system.getRawLocation().toOSString();
      final ISystemModel sysModel = Uma.readSystemFromFile(systemFileName);
      
      Job bpBuildJob = new Job("constructing view") 
      {
        @Override
        protected IStatus run(IProgressMonitor monitor) {
          
          try {
            int traceCounter = 0;
            int doneTrace = traces.size();
            int total = doneTrace;
            
            monitor.beginTask("exploring "+total+" views", total);
            
            PetriNet simplestNet = null;
            float simplestComplexity = Float.MAX_VALUE;
            int simplestTraceConfig;
            boolean simplestEventConfig[];
            
            while (traceCounter < doneTrace) {
              if (monitor.isCanceled())
                break;
              
              traceCounter++;
              if (traceCounter == doneTrace) break;
                            
              HashSet<String> keepEventConfig = new HashSet<String>();
              for (int i=0; i < traceCounter; i++) {
                for (String event : traces.get(i))
                  keepEventConfig.add(event);
              }
              
              // skip all configurations having not enough traces
              if (((float)traceCounter)/((float)traces.size()) < coverTracePercentage) {
                monitor.worked(1);
                continue;
              }
              
              if (hideEventSets.size() > 0) {
                boolean[] eventConfigCounter = new boolean[hideEventSets.size()+1];
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
                  for (int i=0; i < hideEventSets.size(); i++) {
                    if (eventConfigCounter[i]) eventConfig.add(hideEventSets.get(i));
                  }
                  
                  // skip all configurations having not enough traces
                  if (((float)eventConfig.size())/((float)hideEventSets.size()) < coverEventPercentage) {
                    stepEvent++;
                    continue;
                  }
                  
                  monitor.subTask("event view "+stepEvent+"/"+totalEvent);
                  
                  PetriNet net = generateView(sysModel, new LinkedList<String>(keepEventConfig), eventConfig);
                  float c = Complexity.complexitySimple(net);
                  
                  if (c < simplestComplexity) {
                    simplestNet = net;
                    simplestComplexity = c;
                    simplestTraceConfig = traceCounter;
                    simplestEventConfig = Arrays.copyOf(eventConfigCounter, eventConfigCounter.length-1);
                    
                    System.out.println("complexity: "+c+" for "+simplestTraceConfig+" "+ViewGeneration.toString(simplestEventConfig));
                  }
                  stepEvent++;
                }
              } else {
                PetriNet net = generateView(sysModel, new LinkedList<String>(keepEventConfig), null);
                float c = Complexity.complexitySimple(net);
                
                if (c < simplestComplexity) {
                  simplestNet = net;
                  simplestComplexity = c;
                  simplestTraceConfig = traceCounter;
                  simplestEventConfig = null;
                  
                  System.out.println("complexity: "+c+" for "+simplestTraceConfig);
                }
              }
  
              monitor.worked(1);
            }
  
            if (simplestNet != null) {
              IPath targetPath_lola = new Path(selectedFile_system.getFullPath().toString()+".view_struct.lola");
              IPath targetPath_dot = new Path(selectedFile_system.getFullPath().toString()+".view_struct.dot");
              ActionHelper.writeFile(targetPath_lola, PetriNetIO.toLoLA(simplestNet));
              ActionHelper.writeFile(targetPath_dot, simplestNet.toDot());
            }
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
	
  public static PetriNet generateView(ISystemModel sysModel,
      Collection<String> keepEvents, Collection<String> showEvents) throws InvalidModelException
  {
    if (sysModel instanceof PetriNet == false) return new PetriNet();
    PetriNet net = new PetriNet((PetriNet)sysModel);
    
    LinkedList<Transition> removeTransition = new LinkedList<Transition>();
    for (Transition t : net.getTransitions()) {
      boolean found = false;
      for (String event : keepEvents) {
        if (t.getName().equals(event)) {
          found = true; break;
        }
      }
      if (!found) removeTransition.add(t);
    }
    
    for (Transition t : removeTransition)
      net.removeTransition(t);

    if (showEvents != null) {
      for (Transition t : net.getTransitions()) {
        if (!showEvents.contains(t.getName())) {
          t.setTau(true);
          t.setName("tau");
        }
      }
    }
    
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
      if (selected.length >= 2) {
      
        String[] sysExt = new String[] { "lola", "tpn" };
        String[] logExt = new String[] { "txt" };
        
        LinkedList<IFile> systemFiles = ActionHelper.filterFiles(selected, sysExt);
        LinkedList<IFile> logFiles = ActionHelper.filterFiles(selected, logExt);
        
        if (systemFiles.size() == 1 && logFiles.size() >= 1) {
          selectedFile_system = systemFiles.getFirst();
          for (IFile file : logFiles) {
            if (file.getFullPath().toString().endsWith(".log.txt"))
              selectedFile_log.add(file);
            if (file.getFullPath().toString().endsWith(".events.txt"))
              selectedFile_events.add(file);
          }
          if (selectedFile_log.size() == 0)
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
