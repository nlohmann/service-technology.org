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
import hub.top.petrinet.Transition;
import hub.top.uma.DNode;
import hub.top.uma.DNodeBP;
import hub.top.uma.DNodeSet.DNodeSetElement;
import hub.top.uma.DNodeSys;
import hub.top.uma.InvalidModelException;
import hub.top.uma.Uma;
import hub.top.uma.synthesis.NetSynthesis;
import hub.top.uma.view.DNodeBP_View;
import hub.top.uma.view.Precision;
import hub.top.uma.view.Precision_ETC;
import hub.top.uma.view.ViewGeneration;
import hub.top.uma.view.ViewGeneration2;
import hub.top.uma.view.ViewGeneration3;

import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.Map.Entry;

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
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.IWorkbenchWindowActionDelegate;

public class Action_ComputePrecision implements IWorkbenchWindowActionDelegate {

	public static final String ID = "hub.top.uma.ui.ComputePrecision";
	
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
    final IPath fileName_system_wsPath = selectedFile_system.getFullPath();
    final String fileName_trace = selectedFile_log.getRawLocation().toOSString();
    
    try {
      
      DNode.idGen = 0;
      ISystemModel sysModel = Uma.readSystemFromFile(fileName_system);
      if (sysModel instanceof PetriNet == false) return;
      final PetriNet net = (PetriNet)sysModel;
      net.makeNormalNet();
      net.turnIntoLabeledNet();
      final DNodeSys sys = Uma.getBehavioralSystemModel(net);
      
      ArrayList<Transition> ts = new ArrayList<Transition>();
      for (int i=0; i < ts.size(); i++) {
        for (int j=i+1; j < ts.size(); j++) {
          if (net.nondeterministicTransitions(ts.get(i), ts.get(j))) {
            System.out.println("non-deterministic transitions "+ts.get(i)+" and "+ts.get(j));
          }
        }
      }
      
      final LinkedList<String[]> traces = ViewGeneration2.readTraces(fileName_trace);
      
      Job bpBuildJob = new Job("Calculate Precision") 
      {
        @Override
        protected IStatus run(IProgressMonitor monitor) {
          
          UmaUI.getUI().switchOutStream_uiConsole();
          monitor.beginTask("Calculate Precision", traces.size());
          

          DNodeBP build = Uma.initBuildPrefix_View(sys, 0);
          
          Precision precision = new Precision(build);
          Precision_ETC precision2 = new Precision_ETC(net);
          
          int failed = 0;
          int traceNum = 0;
          for (String[] trace : traces) {
            if (monitor.isCanceled())
              break;
            if (!precision.extendByTrace(trace)) {
              System.out.println("failed "+traceNum+": "+ViewGeneration2.toString(trace));
              failed++;
            }
            precision2.extendByTrace(trace);
            
            traceNum++;
            monitor.subTask("failed for: "+failed);
          }
          
          precision.computePrecision();
          precision2.computePrecision();
          
          /*
          Uma.out.println("Precision result for "+fileName_system);
          String message = 
            "Global precision: "+precision.getPrecisionGlobal()
           +", Trace precision: "+precision.getPrecisionTrace()
           +"\nGlobal precision (Trans): "+precision.getPrecisionGlobalTrans()
           +", Trace precision (Trans): "+precision.getPrecisionTraceTrans()
           +"\nUnused alternatives: "+precision.getUnusedAlternatives()
           +", average deviation: "+precision.getAverageDeviation()
           +"\nCould not execute "+failed+"/"+traceNum+" traces";
          Uma.out.println(message);

          String message2 = 
            "Global precision: "+precision2.getPrecisionGlobal()
           +", Trace precision: "+precision2.getPrecisionTrace()
           +"\nUnused alternatives: "+precision2.getUnusedAlternatives()
           +", average deviation: "+precision2.getAverageDeviation();
          Uma.out.println(message2);
          */
          
          
          Uma.out.println("Precision result for "+fileName_system);
          Uma.out.println(" Global (BP): "+precision.getPrecisionGlobalTrans()
              +"  Unused Alt: "+precision.getUnusedAlternatives()
              +"  Global (Tree): "+precision2.getPrecisionGlobal()
              +"  ETC: "+precision2.getPrecisionTrace());

          /*
          Display.getDefault().syncExec(new Runnable() {
            public void run() {         
              MessageDialog.openError(null, "Precision", message);
            }
          });
          */
          monitor.done();
          UmaUI.getUI().switchOutStream_System();
          
          return Status.OK_STATUS;
        }
      };
        
      bpBuildJob.setUser(true);
      bpBuildJob.schedule();
      
    } catch (IOException e) {
      Activator.getPluginHelper().logError("Failed to parse "+fileName_system, e);
    } catch (InvalidModelException e) {
      Activator.getPluginHelper().logError("Invalid model in  "+fileName_system, e);
    }
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
      
        String[] sysExt = new String[] { "lola", "tpn" };
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
