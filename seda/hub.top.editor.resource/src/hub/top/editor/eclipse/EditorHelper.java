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


import org.eclipse.core.resources.IFile;
import org.eclipse.emf.ecore.resource.Resource;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.ui.IEditorDescriptor;
import org.eclipse.ui.IFileEditorInput;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.part.FileEditorInput;
import org.eclipse.ui.part.ISetSelectionTarget;

public class EditorHelper extends ResourceHelper {
	
	private final IEditorUtil fEditorUtil;
	
	/**
	 * @param pluginHelper helper of the plugin that runs this editor helper
	 * @param editorUtil wrapping a specific editor that is linked to the resource
	 *                   that will be wrapped in this helper (can be <code>null</code>)
	 */
	public EditorHelper (PluginHelper pluginHelper, IEditorUtil editorUtil) {
		super(pluginHelper);
		fEditorUtil = editorUtil;
		updateResourceFromEditor();
	}

	/**
	 * @param pluginHelper helper of the plugin that runs this editor helper
	 * @param editorUtil wrapping a specific editor that is linked to the resource
	 *                   that will be wrapped in this helper (can be <code>null</code>)
	 * @param ext file extension that will be handled by this helper
	 */
	public EditorHelper (PluginHelper pluginHelper, IEditorUtil editorUtil, String ext) {
		super(pluginHelper, ext);
		fEditorUtil = editorUtil;
		updateResourceFromEditor();
	}
	
	public IEditorUtil getEditorUtil() {
		return fEditorUtil;
	}

	/**
	 * send commands to the Eclipse workbench to open the editor that
	 * corresponds to the given file, will create an error dialog if
	 * there is no such editor
	 * @param window the workbench window where to perform the commands
	 * @param file the file to open
	 */
	public void openFileInEditor (IWorkbenchWindow window) {
		IWorkbenchWindow workbenchWindow = window.getWorkbench().getActiveWorkbenchWindow();
		
		IFile file = getFile(true);	// retrieve file of this helper
		if (file == null) {
			MessageDialog.openError(workbenchWindow.getShell(), "Error opening editor", "No input file given.");
			return;
		}
		
		// Select the file resource in the current view.
		IWorkbenchPage page = workbenchWindow.getActivePage();
		final IWorkbenchPart activePart = page.getActivePart();
		if (activePart instanceof ISetSelectionTarget) {
			final ISelection targetSelection = new StructuredSelection(file);
			window.getShell().getDisplay().asyncExec
				(new Runnable() {
					 public void run() {
						 ((ISetSelectionTarget)activePart).selectReveal(targetSelection);
					 }
				 });
		}
		// Open an editor on the new file.
		try {
			IEditorDescriptor desc = window.getWorkbench().getEditorRegistry().getDefaultEditor(file.getFullPath().toString());
			if (desc == null)
				MessageDialog.openError(workbenchWindow.getShell(), "Error opening editor.", "Unknown file extension "+file.getFileExtension()+" of file "+file);
			else
				page.openEditor(new FileEditorInput(file), desc.getId());
		}
		catch (PartInitException exception) {
			MessageDialog.openError(workbenchWindow.getShell(), "Error opening editor.", exception.getMessage());
		}
	}
	
	/**
	 * store resource from currently opened editor in this wrapper
	 */
	public void updateResourceFromEditor () {
		if (fEditorUtil == null)
			return;
		setResource(fEditorUtil.getCurrentResource(), true);
	}

	/**
	 * @return resource that is currently contained in the editor
	 * @see hub.top.editor.eclipse.ResourceHelper#getResource()
	 */
	@Override
	public Resource getResource(boolean smart) {
		Resource res = super.getResource(smart);
		if (res != null)
			return res;
		if (smart && fEditorUtil != null)
			return fEditorUtil.getCurrentResource();
		return null;
	}
	
	/**
	 * @return file handle of the file that is currently opened in
	 *         the editor (if it is a file editor) or that is linked
	 *         to the current resource otherwise
	 * @see EditorHelper#getFile()
	 */
	@Override
	public IFile getFile(boolean smart) {
		IFile file = null;
		// retrieve file from editor input
		if (smart) {
			IEditorUtil editorUtil = this.getEditorUtil();
			if (editorUtil != null
				&& editorUtil.getEditor() != null
				&& editorUtil.getEditor().getEditorInput() != null
				&& editorUtil.getEditor().getEditorInput() instanceof IFileEditorInput)
			{
				// if the editor has opened a file, retrieve it
				file = ((IFileEditorInput)editorUtil.getEditor().getEditorInput()).getFile();
			}
		}
		// if none found, retrieve from underlying handler
		if (file == null)	
			file = super.getFile(smart);
		return file;
	}
}
