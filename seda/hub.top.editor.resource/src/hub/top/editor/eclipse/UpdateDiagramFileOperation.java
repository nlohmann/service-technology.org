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
import java.util.Map;

import org.eclipse.core.commands.ExecutionException;
import org.eclipse.core.runtime.IAdaptable;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.gmf.runtime.common.core.command.CommandResult;
import org.eclipse.gmf.runtime.emf.commands.core.command.AbstractTransactionalCommand;

public class UpdateDiagramFileOperation extends AbstractTransactionalCommand {
//public class UpdateDiagramFileOperation extends WorkspaceModifyOperation {

	private ResourceHelper modelHelper;
	private DiagramEditorHelper diagramHelper;
	
	private Map<String, String> saveOptions = null;
	private boolean overwrite;

	/**
	 * An operation to update the contents of an existing diagram file,
	 * the diagram file and the corresponding model file have to be create
	 * beforehand. Use {@link NewDiagramFileOperation} to create these resources,
	 * the method {@link DiagramEditorHelper#createDiagramResource()} generates
	 * a correctly instantiated operation to do so.
	 * 
	 * @param modelHelper helper of the underlying model 
	 * @param diagramHelper helper of the diagram
	 * @param overwrite whether to overwrite model and diagram resources
	 */
	public UpdateDiagramFileOperation(ResourceHelper modelHelper, DiagramEditorHelper diagramHelper, boolean overwrite) {
		super(diagramHelper.getEditingDomain(),
				"UpdateDiagramFile",
				diagramHelper.affectedFiles());
				
		this.modelHelper = modelHelper;
		this.diagramHelper = diagramHelper;
		this.overwrite = overwrite;
	}
	
	/**
	 * update the save options that are used when saving resources, if none
	 * is set the default save options @see {@link UpdateDiagramFileOperation#getDefaultSaveOptions()}
	 * will be used
	 * 
	 * @param saveOptions
	 */
	public void setSaveOptions(Map<String, String> saveOptions) {
		this.saveOptions = saveOptions;
	}
	
	@Override
	//protected void execute(IProgressMonitor monitor) throws CoreException,
	//			InvocationTargetException, InterruptedException
	protected CommandResult doExecuteWithResult(IProgressMonitor monitor,
			IAdaptable arg1) throws ExecutionException
	{
		try {
			// update resource with overwriting the contents, but do not write yet
			modelHelper.updateModelFile(false, overwrite);
			// update diagram, but do not write yet
			//diagramHelper.updateModelFile(false, overwrite);
		
			if (saveOptions == null)
				saveOptions = diagramHelper.getSaveOptions();

			// now save both resources together
			modelHelper.getResource(false).save(saveOptions);
			//diagramHelper.getResource(false).save(saveOptions);
		} catch (IOException exception) {
			diagramHelper.getPluginHelper().logError("Save operation failed for: "+modelHelper+" and/or "+diagramHelper+".", exception);
			return CommandResult.newErrorCommandResult(exception);
		} finally {
			monitor.done();
		}
		return CommandResult.newOKCommandResult();
	}
}
