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
import java.util.LinkedList;
import java.util.List;

import org.eclipse.core.resources.IFile;
import org.eclipse.emf.ecore.resource.Resource;
import org.eclipse.emf.transaction.TransactionalEditingDomain;
import org.eclipse.gmf.runtime.emf.core.GMFEditingDomainFactory;
import org.eclipse.gmf.runtime.notation.Diagram;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.actions.WorkspaceModifyOperation;

/**
 * a helper for diagram editor resources and files, manages resources
 * via a {@link TransactionalEditingDomain}. 
 * 
 * @author Dirk Fahland
 */
public class DiagramEditorHelper extends EditorHelper {
	
	/** the <code>{@link EditorHelper}</code> that underlies this diagram */
	private ResourceHelper modelHelper;
	
	/**
	 * initialize helper with a new transactional GMF editing domain
	 * @param pluginHelper
	 * @param editorUtil
	 * @param modelHelper
	 */
	public DiagramEditorHelper(PluginHelper pluginHelper, IDiagramEditorUtil editorUtil,
			ResourceHelper modelHelper)
	{
		this(pluginHelper, editorUtil, modelHelper, null);
	}

	/**
	 * initialize helper with a new transactional GMF editing domain
	 * @param pluginHelper
	 * @param editorUtil
	 * @param modelHelper
	 * @param ext
	 */
	public DiagramEditorHelper(PluginHelper pluginHelper, IDiagramEditorUtil editorUtil,
			ResourceHelper modelHelper, String ext)
	{
		super(pluginHelper, editorUtil, ext);
		this.modelHelper = modelHelper;
		this.editingDomain = GMFEditingDomainFactory.INSTANCE.createEditingDomain();
		this.resources = editingDomain.getResourceSet();
		
		modelHelper.refreshResourceSet(this.resources);
	}
	
	public IDiagramEditorUtil getEditorUtil() {
		return (IDiagramEditorUtil)super.getEditorUtil();
	}
	
	/**
	 * @return list of files affected when working with this helper
	 */
	@Override
	public List<IFile> affectedFiles() {
		List<IFile> fList = super.affectedFiles();
		// extend list with model helper files 
		fList.addAll(modelHelper.affectedFiles());
		return fList;
	}

	@Override
	public void openFileInEditor(IWorkbenchWindow window) {
		try {
			getEditorUtil().openDiagram(getResource(true));
		} catch (PartInitException e) {
			getPluginHelper().logError("Could not open diagram.", e);
		}
	}

	public NewDiagramFileOperation createDiagramResource() {
		
		if (getEditorUtil().standardCreate_InitializesModel()
			&& modelHelper.hasModel())
		{
			// in case the standard create operation overwrites existing models
			// and if the current helper has contents, create the model
			// from the model file
			return new NewDiagramFromModelOperation(modelHelper, this);
		}
		else
		{
			return new NewDiagramFileOperation(modelHelper, this);
		}
	}

	public UpdateDiagramFileOperation updateDiagramContentsInResource(boolean overwrite) {
		return new UpdateDiagramFileOperation(modelHelper, this, overwrite);
	}

	/**
	 * @return <code>true</code> iff diagram of this helper display the
	 * contents of its model helper
	 */
	public boolean diagramInSync () {
		Diagram diagram = (Diagram)getResource(true).getContents().get(0);
		return (modelHelper.isInSync() &&
				diagram.getElement() == modelHelper.getResource(true).getContents().get(0));
	}
	
	/**
	 * Update contents of an existing diagram file in the current
	 * workspace based on the fields of this helper, this method will
	 * replace all contents of the resource with the
	 * <code>{@link ResourceHelper#getModelContents(boolean)}</code> of
	 * its model helper (that contains the model that is shown in this
	 * diagram).
	 * 
	 * This method must be called within an 
	 * <code>{@link WorkspaceModifyOperation}</code>. This
	 * method is called from <code>{@link UpdateDiagramFileOperation}</code>
	 * which is generated for this helper from
	 * <code>{@link DiagramEditorHelper#updateDiagramContentsInResource()}</code>
	 * 
	 * @throws IOException
	 * 
	 * @see {@link ResourceHelper#updateModelFile(boolean)
	 */
	@Override
	protected void updateModelFile(boolean doSave, boolean overwrite) throws IOException {
		// resolve fields and overwrite existing resource
		resolveFields();
		Resource resource = getResource(false);
		
		// this method works on the assumption that the resource in
		// this helper is a diagram
		assert(resource.getContents().get(0) != null);
		assert(resource.getContents().get(0) instanceof Diagram);
		
		// and that the model helper contains a model object of the
		// underlying meta-model of this diagram, set its root object
		// as the new diagram element of this diagram
		Diagram diagram = (Diagram)getModelRoot();
		if (overwrite)
			diagram.setElement(modelHelper.getNewModelRoot());
		else
			diagram.setElement(modelHelper.getModelRoot());
		
		if (doSave) {
			resource.save(getSaveOptions());
		}
	}
	
}
