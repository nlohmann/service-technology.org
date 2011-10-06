/*****************************************************************************\
 * Copyright (c) 2008 Dirk Fahland. All rights reserved. EPL1.0/GPL3.0/LGPL3.0
 * 
 * ServiceTechnolog.org - PetriNet Editor Framework
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
 * The Original Code is this file as it was released on July 30, 2008.
 * The Initial Developer of the Original Code is
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
package hub.top.editor.eclipse;


import org.eclipse.core.commands.ExecutionException;
import org.eclipse.core.runtime.IAdaptable;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.emf.ecore.resource.Resource;
import org.eclipse.gmf.runtime.common.core.command.CommandResult;
import org.eclipse.gmf.runtime.emf.commands.core.command.AbstractTransactionalCommand;
import org.eclipse.ui.PartInitException;

public class NewDiagramFileOperation extends AbstractTransactionalCommand {
	
	protected boolean openDiagram = false;
	protected ResourceHelper modelHelper;
	protected DiagramEditorHelper diagramHelper;
	
	/**
	 * An operation to create a new diagram file, the contents of the
	 * diagram can be set with a {@link UpdateDiagramFileOperation}, that
	 * is generated in {@link DiagramEditorHelper#updateDiagramContentsInResource()}
	 * 
	 * @param modelHelper helper of the underlying model 
	 * @param diagramHelper helper of the diagram
	 */
	public NewDiagramFileOperation(ResourceHelper modelHelper, DiagramEditorHelper diagramHelper)
	{
		super(diagramHelper.getEditingDomain(),
				"NewDiagramFile",
				diagramHelper.affectedFiles());
		
		this.modelHelper = modelHelper;
		this.diagramHelper = diagramHelper;
	}
	
	/**
	 * toggle whether the diagram file should be opened after
	 * the file has been created
	 * @param open
	 */
	public void setOpenDiagram(boolean open) {
		openDiagram = open;
	}
	
	/* (non-Javadoc)
	 * @see org.eclipse.ui.actions.WorkspaceModifyOperation#execute(org.eclipse.core.runtime.IProgressMonitor)
	 */
	@Override
	//protected void execute(IProgressMonitor monitor) throws CoreException,
	//			InvocationTargetException, InterruptedException
	protected CommandResult doExecuteWithResult(IProgressMonitor monitor,
			IAdaptable adapter) throws ExecutionException
	{
		try {
			Resource diagramResource =
				diagramHelper.getEditorUtil().createDiagram(
						diagramHelper.getURI(true),	modelHelper.getURI(true), monitor);
			// resource was created in this operation, just assign it to the helper
			diagramHelper.setResource(diagramResource, true);
			
			if (openDiagram)
				diagramHelper.getEditorUtil().openDiagram(diagramResource);
		} catch (PartInitException ex) {
			diagramHelper.getPluginHelper().logError("Unable to open editor", ex);
			return CommandResult.newErrorCommandResult(ex);
		}
		return CommandResult.newOKCommandResult();
	}
}
