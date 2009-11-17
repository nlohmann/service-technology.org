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

package hub.top.greta.run.actions;

import hub.top.adaptiveSystem.AdaptiveSystem;
import hub.top.editor.eclipse.ResourceHelper;
import hub.top.greta.oclets.canonical.DNodeBP;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.jobs.Job;
import org.eclipse.emf.common.util.URI;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.gef.EditPart;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IFileEditorInput;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.IWorkbenchWindowActionDelegate;

public class BuildBranchingProcess2 implements IWorkbenchWindowActionDelegate {

	public static final String ID = "hub.top.GRETA.run.buildBranchingProcess2";
	
	private IWorkbenchWindow workbenchWindow;

	private AdaptiveSystem adaptiveSystem;
	
	// fields for tracking the selection in the explorer 
	private IFile 	selectedFile = null;
	private URI 	selectedURI = null;
	
	public void dispose() {
		// TODO Auto-generated method stub

	}

	public void init(IWorkbenchWindow window) {
		workbenchWindow = window;
	}
	

	public void run(IAction action) {
		if (!action.getId().equals(ID))
			return;
		
		// seek system to check from a given URI
		adaptiveSystem = ActionHelper.getAdaptiveSystem(selectedURI);
		
		// if there was no system at the given URI, check the current editor
		if (adaptiveSystem == null) {
			IEditorPart editor = workbenchWindow.getActivePage().getActiveEditor();
			adaptiveSystem = ActionHelper.getAdaptiveSystem(editor);
			
			// if this editor has input file, remember it
			if (editor.getEditorInput() != null
				&& editor.getEditorInput() instanceof IFileEditorInput)
			{
				selectedFile = ((IFileEditorInput)editor.getEditorInput()).getFile();
			}
		}
		
		if (adaptiveSystem == null)
			return;
			
		final DNodeBP bp = new DNodeBP(adaptiveSystem);
		
		Job bpBuildJob = new Job("Constructing branching process II") 
		{
			@Override
			protected IStatus run(IProgressMonitor monitor) {
				
				monitor.beginTask("Constructing branching process II", IProgressMonitor.UNKNOWN);
				
				System.out.println("------- constructing branching process II -------");
				System.out.println("input file: "+adaptiveSystem.eResource().getURI());
				
				int steps = 0;
				int current_steps = 0;
				int num = 0;
				
				boolean interrupted = false;
				long tStart = System.currentTimeMillis();
				while ((current_steps = bp.step()) > 0) {
					steps += current_steps;

					monitor.subTask("explored "+steps+" events");
					
					if (monitor.isCanceled()) {
						interrupted = true;
						break;
					}
					
					System.out.print(steps+"... ");
					if (num++ > 10) { System.out.print("\n"); num = 0; }
				}
				long tEnd = System.currentTimeMillis();
				
				if (interrupted) System.out.print("\ninterrupted ");
				else System.out.print("\ndone ");
				
				System.out.println("after "+steps+" steps, "+(tEnd-tStart)+"ms");
				System.out.println(bp.getStatistics());
				
        boolean hasDeadlock = bp.hasDeadCondition();
        boolean isUnsafe = !bp.isSafe();
        if (hasDeadlock)
          System.out.println("has a deadlock.");
        if (isUnsafe)
          System.out.println("is unsafe.");
				
				try {
				  bp.minimize();
				} catch (NullPointerException e) {
				  
				}
				
				monitor.beginTask("Writing result files", 1);
				if (selectedFile != null) {
					monitor.subTask("writing dot file");
					writeDotFile(bp, selectedFile, "_bp2");
					monitor.worked(1);

				}
				monitor.done();
				
				/*
				if (interrupted)
					return Status.CANCEL_STATUS;
				else*/
					return Status.OK_STATUS;
				
			}
		};
			
		bpBuildJob.setUser(true);
		bpBuildJob.schedule();
	}

	public void selectionChanged(IAction action, ISelection selection) {
	    selectedURI = null;
	    selectedFile = null;
	    action.setEnabled(false);
	    if (selection instanceof IStructuredSelection == false
	        || selection.isEmpty()) {
	      return;
	    }
	    try {
	      Object o = ((IStructuredSelection) selection).getFirstElement();
	      if (o instanceof IFile) {
	        selectedFile = (IFile) ((IStructuredSelection) selection).getFirstElement();
	        selectedURI = URI.createPlatformResourceURI(selectedFile.getFullPath()
	            .toString(), true);
	        
	      } else if (o instanceof EditPart) {
	        EObject e = (EObject)((EditPart)o).getModel();
	        selectedURI = e.eResource().getURI();
          selectedFile = ResourceHelper.uriToFile(selectedURI);
          
	      } else {
	        return;
	      }
	      
		    action.setEnabled(true);
	    } catch (ClassCastException e) {
	    	// just catch, do nothing
	    }
	}

	private void writeDotFile (DNodeBP bp, IFile inputFile, String suffix) {

		String targetPathStr = inputFile.getFullPath().removeFileExtension().toString();
		IPath targetPath = new Path(targetPathStr+suffix+".dot");

		ActionHelper.writeFile (targetPath, bp.toDot());
	}
}
