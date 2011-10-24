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
import hub.top.editor.eclipse.FileIOHelper;
import hub.top.petrinet.ISystemModel;
import hub.top.uma.DNode;
import hub.top.uma.DNodeBP;
import hub.top.uma.DNodeRefold;
import hub.top.uma.DNodeSys;
import hub.top.uma.InvalidModelException;
import hub.top.uma.Uma;
import hub.top.uma.view.ViewGeneration2;

import java.io.IOException;
import java.io.PrintStream;
import java.util.LinkedList;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.jobs.Job;
import org.eclipse.emf.common.util.URI;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.dialogs.IInputValidator;
import org.eclipse.jface.dialogs.InputDialog;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.IWorkbenchWindowActionDelegate;

public class Action_Views_GenerateTraces implements IWorkbenchWindowActionDelegate {

	public static final String ID = "hub.top.uma.ui.view.GenerateTraces";
	
	private IWorkbenchWindow workbenchWindow;

	// fields for tracking the selection in the explorer 
	private IFile 	selectedFile_system = null;
	
	public void dispose() {
	}

	public void init(IWorkbenchWindow window) {
		workbenchWindow = window;
	}
	

	public void run(IAction action) {
		if (!action.getId().equals(ID))
			return;
		
    String fileName = selectedFile_system.getRawLocation().toOSString();
    
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
      
      InputDialog traceNumDiag = new InputDialog(null,
          "Generate log traces for "+selectedFile_system.getName(),
          "How many traces shall be generated?", "10", intValidator);
      if (traceNumDiag.open() == InputDialog.CANCEL)
        return;

      InputDialog traceLengthDiag = new InputDialog(null,
          "Generate log traces for "+selectedFile_system.getName(),
          "Maximum length of traces?", "30", intValidator);
      if (traceLengthDiag.open() == InputDialog.CANCEL)
        return;
      
      InputDialog boundDiag = new InputDialog(null,
          "Build branching process for "+selectedFile_system.getName(),
          "bound k = ..", "1", intValidator);
      if (boundDiag.open() == InputDialog.CANCEL)
        return;
      int bound = Integer.parseInt(boundDiag.getValue());

      DNode.idGen = 0;
      ISystemModel sysModel = Uma.readSystemFromFile(fileName);
      DNodeSys sys = Uma.getBehavioralSystemModel(sysModel);

      final DNodeRefold build = Uma.initBuildPrefix_View(sys, bound);
      final PrintStream out = System.out;
      final int traceNum = Integer.parseInt(traceNumDiag.getValue());
      final int traceLength = Integer.parseInt(traceLengthDiag.getValue());
      
      Job bpBuildJob = new Job("constructing branching process") 
      {
        @Override
        protected IStatus run(IProgressMonitor monitor) {
          
          monitor.beginTask("constructing branching process", IProgressMonitor.UNKNOWN);
          
          // console output
          if (out != null) out.println("------- constructing branching process -------");

          int current_steps, steps = 0;
          int num = 0;
          
          boolean interrupted = false;
          // branching process construction loop
          // repeat step() as long as an event can be fired
          long tStart = System.currentTimeMillis();
          while ((current_steps = build.step()) > 0) {
            steps += current_steps;

            // graphical output and UI interaction
            if (monitor != null) {
              monitor.subTask("BP: explored "+steps+" events");
              
              if (monitor.isCanceled()) {
                interrupted = true;
                break;
              }
            }
            
            // console output
            if (out != null) {
              out.print(steps+".. ");
              if (num++ > 10) { System.out.print("\n"); num = 0; }
            }
          } // end of BP construction loop
          long tEnd = System.currentTimeMillis();
          long analysisTime = (tEnd - tStart);
          
          // console output
          if (out != null) { 
            out.println();
            if (interrupted) out.print("\ninterrupted ");
            else out.print("\ndone ");
          }
          
          if (interrupted) return Status.OK_STATUS;
          
          monitor.subTask("generating traces");
          
          build.futureEquivalence();

          ViewGeneration2 viewGen = new ViewGeneration2(build);
          LinkedList<String[]> traces = viewGen.generateRandomTraces(traceNum, traceLength);

          IPath targetPath = new Path(selectedFile_system.getFullPath().toString()+".log.txt");
          ActionHelper.writeFile(targetPath, ViewGeneration2.generateSimpleLog(traces).toString());

          monitor.done();
          
          return Status.OK_STATUS;
        }
      };
        
      bpBuildJob.setUser(true);
      bpBuildJob.schedule();
      
    } catch (IOException e) {
      Activator.getPluginHelper().logError("Failed to parse "+fileName, e);
    } catch (InvalidModelException e) {
      Activator.getPluginHelper().logError("Failed to parse "+fileName, e);
      MessageDialog.openError(null, "Could not read model", ((InvalidModelException)e).getMessage());
    }
	}

  public void selectionChanged(IAction action, ISelection selection) {
    selectedFile_system = null;

    action.setEnabled(false);
    if (selection instanceof IStructuredSelection == false)
    {
      return;
    }
    
    try {
      Object[] selected = ((IStructuredSelection) selection).toArray();
      
      if (selected.length == 1) {
      
        String[] sysExt = new String[] { "oclets", "lola", "tpn" };
        LinkedList<IFile> systemFiles = ActionHelper.filterFiles(selected, sysExt);
        
        if (systemFiles.size() == 1) {
          selectedFile_system = systemFiles.getFirst();
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
