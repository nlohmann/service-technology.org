/*****************************************************************************\
 * Copyright (c) 2008, 2009. All rights reserved. Dirk Fahland. EPL1.0/AGPL3.0
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
 * The Original Code is this file as it was released on June 6, 2009.
 * The Initial Developer of the Original Code are
 * 		Dirk Fahland
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

package hub.top.greta.verification;

import hub.top.editor.eclipse.FileIOHelper;
import hub.top.editor.lola.text.ModelEditor;
import hub.top.editor.lola.text.TextEditor;
import hub.top.editor.ptnetLoLA.PtNet;
import hub.top.greta.oclets.canonical.DNodeBP;
import hub.top.greta.oclets.canonical.InvalidModelException;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.jobs.Job;
import org.eclipse.emf.common.util.URI;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.IWorkbenchWindowActionDelegate;

public class BuildBPLolaAction implements IWorkbenchWindowActionDelegate {
	
	public static final String ID = "hub.top.GRETA.run.buildBranchingProcessLoLA";

	private IWorkbenchWindow workbenchWindow;

	// fields for tracking the selection in the explorer 
	private IFile 	selectedFile = null;
	private URI 	  selectedURI = null;

	public void dispose() {
		// TODO Auto-generated method stub

	}
	
	public void init(IWorkbenchWindow window) {
		workbenchWindow = window;
	}


	public void run(IAction action) {
		if (!action.getId().equals(ID))
			return;
	
		String netText = FileIOHelper.readFile(selectedFile);
		final PtNet ptnet = ModelEditor.getModel(netText);
		if (ptnet == null)
			return;
		
		try {
  		final BuildBP_LoLA buildBP = new BuildBP_LoLA(selectedFile);
  		
  		Job bpBuildJob = new Job("Constructing branching process II") 
  		{
  			@Override
  			protected IStatus run(IProgressMonitor monitor) {
  				
  		    monitor.beginTask("Constructing branching process", IProgressMonitor.UNKNOWN);
  			  boolean result = buildBP.run(monitor, System.out);
  			  monitor.done();
  			  
          DNodeBP bp = buildBP.getBranchingProcess();
          boolean hasDeadlock = bp.hasDeadCondition();
          boolean isUnsafe = !bp.isSafe();
          if (hasDeadlock)
            System.out.println("has a deadlock.");
          if (isUnsafe)
            System.out.println("is unsafe.");
          
  			  if (result)
  			    return Status.OK_STATUS;
  			  else
  			    return Status.CANCEL_STATUS;
  			}
  		};
  			
  		bpBuildJob.setUser(true);
  		bpBuildJob.schedule();
		} catch (InvalidModelException e) {
		  MessageDialog.openError(workbenchWindow.getShell(),
		      "Build Branching Process",
		      "The input model '"+selectedFile.getName()+"' contains an error. "+e.getMessage());
		}
	}
	
	public void selectionChanged(IAction action, ISelection selection) {
	    selectedURI = null;
	    selectedFile = null;
	    action.setEnabled(false);
	    if (selection instanceof IStructuredSelection == false
	        || selection.isEmpty()) {
	      return;
	    }
	    
	    // store selection and URI, re-enable action
	    try {
	    	selectedFile = (IFile) ((IStructuredSelection) selection).getFirstElement();
		    selectedURI = URI.createPlatformResourceURI(selectedFile.getFullPath()
			        .toString(), true);
			    action.setEnabled(true);
	    } catch (ClassCastException e) {
	    	// just catch, do nothing
	    }
	}

}
