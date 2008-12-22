/*****************************************************************************\
 * Copyright (c) 2008 Dirk Fahland. All rights reserved. EPL1.0/GPL3.0/LGPL3.0
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
 * 		Dirk Fahland
 * 
 * Portions created by the Initial Developer are Copyright (c) 2008
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 3 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 3 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the EPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the EPL, the GPL or the LGPL.
\*****************************************************************************/

package hub.top.greta.run.actions;

import hub.top.adaptiveSystem.AdaptiveSystem;
import hub.top.adaptiveSystem.diagram.part.AdaptiveSystemDiagramEditor;
import hub.top.greta.oclets.canonical.CNodeSet;
import hub.top.greta.oclets.ts.AdaptiveSystemTS;
import hub.top.greta.run.Activator;

import java.io.ByteArrayInputStream;
import java.io.InputStream;
import java.lang.reflect.InvocationTargetException;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.NullProgressMonitor;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.jobs.Job;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.dialogs.ProgressMonitorDialog;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IFileEditorInput;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.IWorkbenchWindowActionDelegate;
import org.eclipse.ui.actions.WorkspaceModifyOperation;

public class BuildStateSpace implements IWorkbenchWindowActionDelegate {

	public static final String ID = "hub.top.GRETA.run.buildStateSpace";
	
	private IWorkbenchWindow workbenchWindow;

	private AdaptiveSystemDiagramEditor adaptiveSystemDiagramEditor;
	private AdaptiveSystem adaptiveSystem;

	
	@Override
	public void dispose() {
		// TODO Auto-generated method stub

	}

	@Override
	public void init(IWorkbenchWindow window) {
		workbenchWindow = window;
	}
	
	private void writeFile (IPath targetPath, InputStream contents) {
		IWorkspaceRoot root = ResourcesPlugin.getWorkspace().getRoot();
		IFile targetFile = root.getFile(targetPath);
		try {
			if (targetFile.exists())
				targetFile.setContents(contents, true, true, null);
			else
				targetFile.create(contents, true, null);
			
		} catch (CoreException e) {
			ResourcesPlugin.getPlugin().getLog().log(
				new Status(Status.ERROR, Activator.PLUGIN_ID, "Could not save transition system graph.", e));
		}
	}

	private void writeDotFile (AdaptiveSystemTS ts, IFile inputFile) {
		ByteArrayInputStream contents = new ByteArrayInputStream(ts.toDot().getBytes());
		
		String targetPathStr = inputFile.getFullPath().removeFileExtension().toString();
		IPath targetPath = new Path(targetPathStr+"_ts.dot");

		writeFile (targetPath, contents);
	}
	
	private void writeGenetFile (AdaptiveSystemTS ts, IFile inputFile) {
		ByteArrayInputStream contents = new ByteArrayInputStream(ts.toGenet().getBytes());

		String targetPathStr = inputFile.getFullPath().removeFileExtension().toString();
		IPath targetPath = new Path(targetPathStr+"_ts.sg");

		writeFile (targetPath, contents);
	}
	
	@Override
	public void run(IAction action) {
		if(workbenchWindow.getActivePage().getActiveEditor() instanceof AdaptiveSystemDiagramEditor 
				&& action.getId().equals(BuildStateSpace.ID)) {
			
			adaptiveSystemDiagramEditor = (AdaptiveSystemDiagramEditor) workbenchWindow.getActivePage().getActiveEditor();
			adaptiveSystem = (AdaptiveSystem) adaptiveSystemDiagramEditor.getDiagram().getElement();
			
			final AdaptiveSystemTS ts = new AdaptiveSystemTS(adaptiveSystem);
			ts.withOcletSteps = false;
			
			Job tsExploreJob = new Job("Exploring state space") {

				@Override
				protected IStatus run(IProgressMonitor monitor) {
				
					monitor.beginTask("Exploring state space", IProgressMonitor.UNKNOWN);

					System.out.println("------- constructing state-space -------");
					int steps = 0;
					boolean interrupted = false;
					long tStart = System.currentTimeMillis();
					while (ts.explore()) {
						
						steps++;
						if ((steps % 10) == 0) {
							//System.out.print("\n"+steps+" ");
							monitor.subTask("explored "+steps+" states");
						}
						//System.out.print(".");

						if (monitor.isCanceled()) {
							interrupted = true;
							break;
						}
					}
					long tEnd = System.currentTimeMillis();
					
					if (interrupted) System.out.print("\ninterrupted ");
					else System.out.print("\ndone ");
					System.out.println("after "+steps+" steps, "+(tEnd-tStart)+"ms");
					System.out.println(ts.getStatistics());
					
					if (ts.hasDeadlock())
						System.out.println("has a deadlock");
					else
						System.out.println("is deadlock-free");

					// write result to files
					monitor.beginTask("Writing result files", 2);
					IEditorInput in = adaptiveSystemDiagramEditor.getEditorInput();
					if (in instanceof IFileEditorInput) {
						IFile inputFile = ((IFileEditorInput)in).getFile();
						
						monitor.subTask("writing dot file");
						writeDotFile(ts, inputFile);
						monitor.worked(1);
						
						monitor.subTask("writing genet file");
						writeGenetFile(ts, inputFile);
						monitor.worked(1);
						/*
						for (CNodeSet state : ts.states) {
							ByteArrayInputStream contents = new ByteArrayInputStream(state.toDot().getBytes());

							String targetPathStr = inputFile.getFullPath().removeFileExtension().toString();
							String nullStr = "";
							if (state.num < 10) nullStr += "0";
							if (state.num < 100) nullStr += "0";
								
							IPath targetPath = new Path(targetPathStr+"_ts_"+nullStr+state.num+".dot");
							writeFile(targetPath, contents);
						}
						*/
					}
					monitor.done();
					
					if (interrupted)
						return Status.CANCEL_STATUS;
					else
						return Status.OK_STATUS;
				}
			};

			tsExploreJob.setUser(true);
			tsExploreJob.schedule();
		}
	}

	@Override
	public void selectionChanged(IAction action, ISelection selection) {
		// TODO Auto-generated method stub

	}

}
