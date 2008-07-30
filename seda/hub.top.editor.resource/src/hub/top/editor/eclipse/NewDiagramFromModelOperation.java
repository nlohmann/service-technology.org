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

import java.io.IOException;

import org.eclipse.core.commands.ExecutionException;
import org.eclipse.core.runtime.IAdaptable;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.resource.Resource;
import org.eclipse.gmf.runtime.common.core.command.CommandResult;
import org.eclipse.gmf.runtime.notation.Diagram;
import org.eclipse.ui.PartInitException;

public class NewDiagramFromModelOperation extends NewDiagramFileOperation {

	/**
	 * An operation to create a new diagram file from an existing
	 * model file
	 * 
	 * @param modelHelper helper of the underlying model 
	 * @param diagramHelper helper of the diagram
	 */
	public NewDiagramFromModelOperation(ResourceHelper modelHelper, DiagramEditorHelper diagramHelper) {
		super(modelHelper, diagramHelper);
	}
	
	public static final String InitDiagramCommand = "Creating diagram";
	
	/*
	 * (non-Javadoc)
	 * @see hub.top.editor.eclipse.NewDiagramFileOperation#execute(org.eclipse.core.runtime.IProgressMonitor)
	 */
	@Override
	//protected void execute(IProgressMonitor monitor) throws CoreException,
	//			InvocationTargetException, InterruptedException
	protected CommandResult doExecuteWithResult(IProgressMonitor monitor,
			IAdaptable arg1) throws ExecutionException
	{
		final Resource diagramResource = diagramHelper.createResource();
		final EObject diagramRoot = modelHelper.getResource(true).getContents().get(0);
		final IDiagramEditorUtil editorUtil = diagramHelper.getEditorUtil();
		
		try {
			Diagram diagram = editorUtil.createDiagram(diagramRoot); 
			diagramResource.getContents().add(diagram);
			
			diagramResource.save(diagramHelper.getSaveOptions());
			diagramHelper.getEditorUtil().setCharset(diagramHelper.getFile(true));
			if (openDiagram)
				diagramHelper.getEditorUtil().openDiagram(diagramResource);
			
		} catch (ExecutionException ex) {
			diagramHelper.getPluginHelper().logError("Unable to create diagram", ex); //$NON-NLS-1$
			return CommandResult.newErrorCommandResult(ex);
		} catch (IOException ex) {
			diagramHelper.getPluginHelper().logError("Save operation failed for: " + diagramHelper.getURI(false), ex); //$NON-NLS-1$
			return CommandResult.newErrorCommandResult(ex);
		} catch (PartInitException ex) {
			diagramHelper.getPluginHelper().logError("Unable to open editor", ex); //$NON-NLS-1$
			return CommandResult.newErrorCommandResult(ex);
		}
		return CommandResult.newOKCommandResult();
	}
}
