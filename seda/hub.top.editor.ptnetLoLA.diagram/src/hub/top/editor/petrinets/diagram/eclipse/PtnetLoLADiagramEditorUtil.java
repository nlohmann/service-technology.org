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
package hub.top.editor.petrinets.diagram.eclipse;

import org.eclipse.core.commands.ExecutionException;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.emf.common.util.URI;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.resource.Resource;
import org.eclipse.gmf.runtime.diagram.core.services.ViewService;
import org.eclipse.gmf.runtime.diagram.ui.parts.IDiagramEditorInput;
import org.eclipse.gmf.runtime.notation.Diagram;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.PartInitException;

import hub.top.editor.eclipse.DiagramEditorUtil;
import hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLADiagramEditor;

public class PtnetLoLADiagramEditorUtil extends DiagramEditorUtil {

	public PtnetLoLADiagramEditorUtil (PtnetLoLADiagramEditor editor) {
		super(editor);
	}
	
	/**
	 * create diagram that can be opened by the wrapped editor,
	 * method calls corresponding method from the GMF-generated
	 * GEF sources
	 * @see hub.top.editor.eclipse.DiagramEditorUtil#createDiagram(org.eclipse.emf.common.util.URI, org.eclipse.emf.common.util.URI, org.eclipse.core.runtime.IProgressMonitor)
	 */
	@Override
	public Resource createDiagram(URI diagramURI, URI modelURI,
			IProgressMonitor progressMonitor) {
		return hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLADiagramEditorUtil.createDiagram(diagramURI, modelURI, progressMonitor);
	}
	
	/**
	 * @see hub.top.editor.eclipse.IDiagramEditorUtil#setCharset(org.eclipse.core.resources.IFile)
	 */
	@Override
	public void setCharset(IFile file) {
		hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLADiagramEditorUtil.setCharset(file);
	}

	/**
	 * @return resource of the currently opened diagram
	 * @see hub.top.editor.eclipse.DiagramEditorUtil#getCurrentResource()
	 */
	@Override
	public Resource getCurrentResource() {
		IEditorInput input = getEditorInput();
		if (input != null && input instanceof IDiagramEditorInput) {
			return ((IDiagramEditorInput)input).getDiagram().eResource();
		}
		return null;
	}

	/**
	 * open diagram in the wrapped editor, method calls corresponding
	 * method from the GMF-generated GEF sources
	 * @see hub.top.editor.eclipse.DiagramEditorUtil#openDiagram(org.eclipse.emf.ecore.resource.Resource)
	 */
	@Override
	public boolean openDiagram(Resource diagram)
			throws PartInitException {
		return hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLADiagramEditorUtil.openDiagram(diagram);
	}
	
	@Override
	public String defaultFileExtension() {
		return "ptnetlola_diagram";
	}
	
	@Override
	public String defaultModelFileExtension() {
		return "ptnetlola";
	}
	
	@Override
	public boolean isCorrectRootElement(EObject obj) {
		int vid = hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLAVisualIDRegistry.getDiagramVisualID(obj);
		return vid == hub.top.editor.ptnetLoLA.diagram.edit.parts.PtNetEditPart.VISUAL_ID;
	}
	
	@Override
	public String msg_inCorrectRoolElement () {
		return hub.top.editor.ptnetLoLA.diagram.part.Messages.PtnetLoLANewDiagramFileWizard_IncorrectRootError;
	}
	
	/**
	 * create a diagram with the given object as root of the diagram model 
	 * this method should be called within an {@link AbstractTransactionalCommand}
	 * as the invoked methods are likely to create or update files
	 * 
	 * @param root
	 * @return the created diagram
	 * @throws ExecutionException if <code>root</code> is not a valid root
	 * object for this diagram
	 */
	@Override
	public Diagram createDiagram(EObject root) throws ExecutionException  {
		if (!isCorrectRootElement(root)) {
			throw new ExecutionException(msg_inCorrectRoolElement());
		}
		return ViewService.createDiagram(
				root,
				hub.top.editor.ptnetLoLA.diagram.edit.parts.PtNetEditPart.MODEL_ID,
				hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLADiagramEditorPlugin.DIAGRAM_PREFERENCES_HINT);
	}
}
