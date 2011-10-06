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

package hub.top.editor.ui.actions;

import hub.top.editor.eclipse.DiagramEditorHelper;
import hub.top.editor.eclipse.EditorHelper;
import hub.top.editor.eclipse.EditorUtil;
import hub.top.editor.eclipse.IFrameWorkEditor;
import hub.top.editor.eclipse.PluginHelper;
import hub.top.editor.eclipse.ResourceHandling;
import hub.top.editor.eclipse.ResourceHelper;

import org.eclipse.core.commands.AbstractHandler;
import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.commands.ExecutionException;
import org.eclipse.core.commands.IHandler;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.handlers.HandlerUtil;

public class ConvertToDiagram extends AbstractHandler implements IHandler {
	
	public Object execute(ExecutionEvent event) throws ExecutionException {
		IWorkbenchWindow window = HandlerUtil.getActiveWorkbenchWindowChecked(event);
		PluginHelper plugin = hub.top.editor.Activator.getPluginHelper();
		try
		{
			if (!(window.getActivePage().getActiveEditor() instanceof
					hub.top.editor.eclipse.IFrameWorkEditor ))
				throw new Exception("Convert to Diagram failed: action works in ServiceTechnology.org framework editors only.");

			// retrieve and prepare editor utils 
			EditorUtil sourceEditor = ((IFrameWorkEditor)window.getActivePage().getActiveEditor()).getEditorUtil();
			hub.top.editor.eclipse.DiagramEditorUtil targetDiagramEditor =
				new hub.top.editor.petrinets.diagram.eclipse.PtnetLoLADiagramEditorUtil(null);
			
			// retrieve and prepare specific resource helpers
			EditorHelper sourceHelper = new EditorHelper(plugin, sourceEditor, sourceEditor.defaultFileExtension());
			EditorHelper targetModelHelper;
			// check whether the source contains a valid model for the target diagram (by file extension)
			if (sourceEditor.defaultFileExtension().equals(targetDiagramEditor.defaultModelFileExtension()))
				targetModelHelper = sourceHelper;
			else
				targetModelHelper = new EditorHelper(plugin, null, targetDiagramEditor.defaultModelFileExtension());
			DiagramEditorHelper targetDiagramHelper = new DiagramEditorHelper(plugin, targetDiagramEditor, targetModelHelper, targetDiagramEditor.defaultFileExtension());
			
			// check whether operation will succeed
			if (!ResourceHelper.isWorkspaceResource(sourceHelper.getURI(true))) {
				//System.err.println("is not a workspace resource");
				MessageDialog.openInformation(window.getShell(),
						"Convert to diagram", "We're sorry, we cannot convert files which are stored outside the workspace.\nPlease import your file into the workspace and try again.");
				return null;
			}
			
			// now everything is fine, invoke transformation
			//ResourceHandling.createModelResourceFromResource(window, sourceHelper, targetModelHelper);
			ResourceHandling.createDiagramResourceFromResource(window, sourceHelper, targetModelHelper, targetDiagramHelper);
			//targetModelHelper.openFileInEditor(window);
			// FIXME do refresh only if file contents was touched
			if (targetModelHelper.getEditorUtil() != null)
				targetModelHelper.getEditorUtil().refreshEditorView();
			targetDiagramHelper.openFileInEditor(window);
			return targetDiagramHelper.getResource(false);
		}
		catch (Exception exception) {
			plugin.logError(exception);
			MessageDialog.openInformation(window.getShell(),
					"Convert to diagram", "We're sorry, we could not convert the diagram. See the error log for details.");
		}
		return null;
	}
}
