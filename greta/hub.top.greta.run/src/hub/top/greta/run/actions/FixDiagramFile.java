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

import hub.top.adaptiveSystem.diagram.part.AdaptiveSystemDiagramEditorPlugin;
import hub.top.adaptiveSystem.diagram.part.AdaptiveSystemDiagramEditorUtil;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import org.eclipse.core.commands.ExecutionException;
import org.eclipse.core.commands.operations.OperationHistoryFactory;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.runtime.IAdaptable;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.NullProgressMonitor;
import org.eclipse.emf.common.util.TreeIterator;
import org.eclipse.emf.common.util.URI;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.resource.Resource;
import org.eclipse.emf.transaction.TransactionalEditingDomain;
import org.eclipse.gmf.runtime.common.core.command.CommandResult;
import org.eclipse.gmf.runtime.emf.commands.core.command.AbstractTransactionalCommand;
import org.eclipse.gmf.runtime.emf.core.GMFEditingDomainFactory;
import org.eclipse.gmf.runtime.notation.Diagram;
import org.eclipse.gmf.runtime.notation.Node;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.IObjectActionDelegate;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.PlatformUI;

/**
 * A helper action that establishes backwards compatibility with old
 * versions of this package. The action allows to refactor a .ptnetlola
 * model file and corresponding diagram into a .ptnet model file
 * and corresponding diagram. Thereby, all cross-references from the
 * diagram to the model file are updated as well.
 * 
 * @author Dirk Fahland
 */
public class FixDiagramFile implements IObjectActionDelegate {

  private IWorkbenchPart targetPart;
  private URI diagramModelURI;
  
  private IFile selectedFile;

  public void setActivePart(IAction action, IWorkbenchPart targetPart) {
    this.targetPart = targetPart;
  }

  /**
   * track selection changes in the workbench
   * @param action
   * @param selection
   */
  public void selectionChanged(IAction action, ISelection selection) {
    diagramModelURI = null;
    action.setEnabled(false);
    if (selection instanceof IStructuredSelection == false
        || selection.isEmpty()) {
      return;
    }
    // store selection and URI, re-enable action
    selectedFile = (IFile) ((IStructuredSelection) selection).getFirstElement();
    diagramModelURI = URI.createPlatformResourceURI(selectedFile.getFullPath()
        .toString(), true);
    action.setEnabled(true);
  }
  
  /**
   * Execute the action, rename the selected file (by copying and deleting the old file),
   * and update name and contents of the corresponding diagram file.
   * 
   * @param action
   */
  public void run(IAction action) {
    // get the shell to pop up message boxes
    Shell shell = PlatformUI.getWorkbench().getActiveWorkbenchWindow().getShell();

	final TransactionalEditingDomain editingDomain =
		GMFEditingDomainFactory.INSTANCE.createEditingDomain();
	
	List<IFile> affectedFiles = new LinkedList<IFile>();
	affectedFiles.add(selectedFile);
	
    AbstractTransactionalCommand cmd = new AbstractTransactionalCommand(
    		editingDomain, "fix diagram file", affectedFiles)
    {
	
		@Override
		protected CommandResult doExecuteWithResult(IProgressMonitor monitor,
				IAdaptable info) throws ExecutionException {
			Resource r = editingDomain.getResourceSet().getResource(diagramModelURI, true);
			
			Diagram d = (Diagram)r.getContents().get(0);
			TreeIterator<EObject> it = d.eAllContents();
			while (it.hasNext()) {
				EObject o = it.next();
				if (o instanceof Node) {
					Integer type = new Integer(((Node)o).getType());
					//System.out.println(type);
					if (   type == hub.top.adaptiveSystem.diagram.edit.parts.ConditionPreNetNameEditPart.VISUAL_ID
						|| type == hub.top.adaptiveSystem.diagram.edit.parts.ConditionPreNetTempEditPart.VISUAL_ID
						|| type == hub.top.adaptiveSystem.diagram.edit.parts.ConditionDoNetNameEditPart.VISUAL_ID
						|| type == hub.top.adaptiveSystem.diagram.edit.parts.ConditionDoNetTempEditPart.VISUAL_ID
						|| type == hub.top.adaptiveSystem.diagram.edit.parts.ConditionAPNameEditPart.VISUAL_ID
						|| type == hub.top.adaptiveSystem.diagram.edit.parts.ConditionAPTempEditPart.VISUAL_ID
						
						|| type == hub.top.adaptiveSystem.diagram.edit.parts.EventPreNetNameEditPart.VISUAL_ID
						|| type == hub.top.adaptiveSystem.diagram.edit.parts.EventPreNetTempEditPart.VISUAL_ID
						|| type == hub.top.adaptiveSystem.diagram.edit.parts.EventDoNetNameEditPart.VISUAL_ID
						|| type == hub.top.adaptiveSystem.diagram.edit.parts.EventDoNetTempEditPart.VISUAL_ID
						|| type == hub.top.adaptiveSystem.diagram.edit.parts.EventAPNameEditPart.VISUAL_ID
						|| type == hub.top.adaptiveSystem.diagram.edit.parts.EventAPTempEditPart.VISUAL_ID
						)
					{
						//System.out.println("fix");
						((Node)o).setLayoutConstraint(null);
					}
				}
			}
			
			try {
				r.save(AdaptiveSystemDiagramEditorUtil.getSaveOptions());
			} catch (IOException e) {
				AdaptiveSystemDiagramEditorPlugin.getInstance().logError(
						"Unable to store diagram resource", e); //$NON-NLS-1$
			}
			return null;
		}
	};
    
	try {
		OperationHistoryFactory.getOperationHistory().execute(cmd,
				new NullProgressMonitor(), null);
	} catch (ExecutionException e) {
	      MessageDialog.openError(shell,
						"Unable to fix diagram", e.getMessage()); //$NON-NLS-1$
	}
  }
}
