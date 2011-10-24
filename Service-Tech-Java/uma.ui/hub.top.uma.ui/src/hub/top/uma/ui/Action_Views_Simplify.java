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
import hub.top.petrinet.PetriNet;
import hub.top.petrinet.PetriNetIO;
import hub.top.petrinet.Place;
import hub.top.petrinet.Transition;
import hub.top.uma.DNode;
import hub.top.uma.DNodeRefold;
import hub.top.uma.DotColors;
import hub.top.uma.InvalidModelException;
import hub.top.uma.view.MineSimplify;

import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Set;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.jobs.Job;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.IWorkbenchWindowActionDelegate;

public class Action_Views_Simplify implements IWorkbenchWindowActionDelegate {

	public static final String ID = "hub.top.uma.ui.view.UnfoldingView_Simplify";
	
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
    
    Job bpBuildJob = new Job("Simplifying model") 
    {
      @Override
      protected IStatus run(IProgressMonitor monitor) {
        
        UmaUI.getUI().switchOutStream_uiConsole();
        monitor.beginTask("Simplifying model", IProgressMonitor.UNKNOWN);
        
        try {
          MineSimplify simplify = new MineSimplify(fileName_system_sysPath, fileName_trace);
          simplify.prepareModel();
          simplify.run();
          PetriNet simplifiedNet = simplify.getSimplifiedNet();
            
          if (simplifiedNet != null) {
            
            IPath targetPath_dot = new Path(fileName_system_wsPath.toString()+".simplified.dot");
            ActionHelper.writeFile(targetPath_dot, simplifiedNet.toDot(simplify._debug_weakImpliedPlaces));
            
            IPath targetPath_lola = new Path(fileName_system_wsPath.toString()+".simplified.lola");
            ActionHelper.writeFile(targetPath_lola, PetriNetIO.toLoLA(simplifiedNet));
            
            IPath targetPath_result = new Path(fileName_system_wsPath.toString()+".simplified.result.txt");
            ActionHelper.writeFile(targetPath_result, simplify.generateResultsFile());
            
            List<DNode> nodes = new LinkedList<DNode>(simplify.getBuild().getBranchingProcess().allConditions);
            for (DNode b : nodes) {
              if (b.post == null || b.post.length == 0) {
                if (b.pre[0].post != null && b.pre[0].post.length > 1) {
                  simplify.getBuild().getBranchingProcess().remove(b);
                }
              }
            }
            
            IPath targetPath_bp_dot = new Path(fileName_system_wsPath.toString()+".simplify_bp.dot");
            //ActionHelper.writeFile(targetPath_bp_dot, simplify.getBuild().toDot(simplify._getColoringImplied()));
            ActionHelper.writeFile(targetPath_bp_dot, simplify.getBuild().toDot(simplify._debug_weakImpliedConditions));
          }

        } catch (InvalidModelException e) {
          Activator.getPluginHelper().logError("Invalid system model "+fileName_system_sysPath+".", e);
          MessageDialog.openError(null, "Could not read model", ((InvalidModelException)e).getMessage());
          return Status.OK_STATUS;
          
        } catch (FileNotFoundException e) {
          Activator.getPluginHelper().logError("Could not read file. "+e.getMessage(), e);
          return Status.OK_STATUS;
        
        } catch (IOException e) {
          Activator.getPluginHelper().logError("Error reading file. "+e.getMessage(), e);
          return Status.OK_STATUS;
        }

        UmaUI.getUI().switchOutStream_System();
        monitor.done();
        
        return Status.OK_STATUS;
      }
    };
      
    bpBuildJob.setUser(true);
    bpBuildJob.schedule();
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
