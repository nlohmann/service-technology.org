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
import org.eclipse.core.resources.IFile;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.emf.common.util.URI;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.resource.Resource;
import org.eclipse.gmf.runtime.diagram.core.services.ViewService;
import org.eclipse.gmf.runtime.emf.commands.core.command.AbstractTransactionalCommand;
import org.eclipse.gmf.runtime.notation.Diagram;
import org.eclipse.ui.PartInitException;

public interface IDiagramEditorUtil extends IEditorUtil{

	public boolean openDiagram(Resource diagram) throws PartInitException;
	
	public Resource createDiagram(URI diagramURI, URI modelURI,
			IProgressMonitor progressMonitor);
	/**
	 * @return <code>true</code> iff during <code>{@link IDiagramEditorUtil#createDiagram(URI, URI, IProgressMonitor)}</code>
	 * the model resource of the diagram is initialized with the standard (or emtpy) model 
	 */
	public boolean standardCreate_InitializesModel();
	
	public String defaultModelFileExtension();
	
	public void setCharset(IFile file);
	
	public boolean isCorrectRootElement(EObject eObj);
	public String msg_inCorrectRoolElement ();
	
	/**
	 * create a diagram with the given object as root of the diagram model
	 * an implementation may call <code>{@link ViewService#createDiagram(EObject, String, org.eclipse.gmf.runtime.diagram.core.preferences.PreferencesHint)}</code>
	 * with correct <code>MODEL_ID</code> and <code>PreferencesHint</code>
	 * to realize the functionality, the methods {@link IDiagramEditorUtil#isCorrectRootElement(EObject)}
	 * and {@link IDiagramEditorUtil#msg_inCorrectRoolElement()} should be
	 * used to check whether <code>root</code> is a correct root element
	 * and to provide the corresponding error string if necessary
	 * 
	 * this method should be called within an {@link AbstractTransactionalCommand}
	 * as the invoked methods are likely to create or update files
	 * 
	 * @param root
	 * @return the created diagram
	 * @throws ExecutionException if <code>root</code> is not a valid root
	 * object for this diagram
	 */
	public Diagram createDiagram(EObject root) throws ExecutionException;
}
