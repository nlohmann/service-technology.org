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
import hub.top.uma.DNode;
import hub.top.uma.DNodeRefold;
import hub.top.uma.DNodeSys;
import hub.top.uma.InvalidModelException;
import hub.top.uma.Uma;
import hub.top.uma.view.ViewGeneration2;

import java.io.IOException;
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
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.IWorkbenchWindowActionDelegate;

public class Action_Views_GenerateView_Block implements IWorkbenchWindowActionDelegate {

	public static final String ID = "hub.top.uma.ui.view.GenerateView_Block";
	
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
		
    String fileName_system = selectedFile_system.getRawLocation().toOSString();
    final String fileName_trace = selectedFile_log.getRawLocation().toOSString();
    
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
      
      InputDialog boundDiag = new InputDialog(null,
          "Generate view for "+selectedFile_system.getName(),
          "bound k = ..", "1", intValidator);
      if (boundDiag.open() == InputDialog.CANCEL)
        return;
      int bound = Integer.parseInt(boundDiag.getValue());
      
      DNode.idGen = 0;
      ISystemModel sysModel = Uma.readSystemFromFile(fileName_system);
      DNodeSys sys = Uma.getBehavioralSystemModel(sysModel);
      final LinkedList<String[]> traces = ViewGeneration2.readTraces(fileName_trace);
      final DNodeRefold build = Uma.initBuildPrefix_View(sys, bound);
      
      Job bpBuildJob = new Job("constructing view") 
      {
        @Override
        protected IStatus run(IProgressMonitor monitor) {
          
          monitor.beginTask("constructing view", IProgressMonitor.UNKNOWN);
          
          while (build.step() > 0);
          ViewGeneration2 viewGen = new ViewGeneration2(build);
          PetriNet net = viewGen.generateViewBlocked(traces, 0.0f);

          IPath targetPath_lola = new Path(selectedFile_system.getFullPath().toString()+".view_block.lola");
          IPath targetPath_dot = new Path(selectedFile_system.getFullPath().toString()+".view_block.dot");
          ActionHelper.writeFile(targetPath_lola, PetriNetIO_Out.toLoLA(net));
          ActionHelper.writeFile(targetPath_dot, net.toDot());

          monitor.done();
          
          return Status.OK_STATUS;
        }
      };
        
      bpBuildJob.setUser(true);
      bpBuildJob.schedule();
      
    } catch (IOException e) {
      Activator.getPluginHelper().logError("Failed to parse "+fileName_system, e);
    } catch (InvalidModelException e) {
      Activator.getPluginHelper().logError("Failed to parse "+fileName_system, e);
      MessageDialog.openError(null, "Could not read model", ((InvalidModelException)e).getMessage());
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
