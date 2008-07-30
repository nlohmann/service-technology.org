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

package hub.top.editor.eclipse.ui;

import hub.top.editor.eclipse.EditorHelper;
import hub.top.editor.eclipse.IEditorUtil;
import hub.top.editor.eclipse.IFrameWorkEditor;
import hub.top.editor.eclipse.PluginHelper;
import hub.top.editor.resource.Activator;

import java.io.PrintStream;

import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.wizard.Wizard;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IExportWizard;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.IWorkbenchWindow;


public class ExportWizard extends Wizard implements IExportWizard {
	
	// the current workbench where this wizard is called
	protected IWorkbench workbench;
	// the workbench selection when the wizard was started.
	protected IStructuredSelection selection;
	// the UI helper supporting this wizard
	protected UIhelper 		uihelper = null;
	protected EditorHelper 	editorHelper = null;
	protected PluginHelper  pluginHelper = Activator.getPluginHelper();

	private boolean invalidEditor;
	
	// the pages of the wizard, stored individually for cross-page checks
	protected ExportWizardPage page1;
	

	
	/**
	 * create a new wizard
	 */
	public ExportWizard() {
		setWindowTitle("Export");	
	}
	
	/**
	 * called when the wizard is initialized
	 * 
	 * @see org.eclipse.ui.IWorkbenchWizard#init(IWorkbench,
	 *      IStructuredSelection)
	 */
	public void init(IWorkbench workbench, IStructuredSelection selection) {
		// just remember workbench and selection
		this.workbench = workbench;
		this.selection = selection;
		IWorkbenchWindow window = workbench.getActiveWorkbenchWindow();

		invalidEditor = false;
		uihelper = new UIhelper(workbench, pluginHelper);
		
		// there was nothing selected, so try the current editor
		if (!selection.isEmpty())
		{
			// try to open the selection in an editor
			if (!uihelper.openSelectionInEditor(selection))
			{
				invalidEditor = true;
				MessageDialog.openInformation(workbench.getActiveWorkbenchWindow().getShell(),
					"File export", "The selected resource cannot be exported with this wizard. Please open the resource in the editor and try again.");
			}
		}
		
		if (!invalidEditor)
		{
			// assume that the object to be exported is display in the
			// active editor window
			IEditorPart _editor = (IEditorPart) window.getActivePage().getActiveEditor();
			if (_editor instanceof IFrameWorkEditor) {
				IEditorUtil editorUtil = ((IFrameWorkEditor)_editor).getEditorUtil();
				editorHelper = new EditorHelper(Activator.getPluginHelper(), editorUtil);
			} else {
				MessageDialog.openInformation(workbench.getActiveWorkbenchWindow().getShell(),
						"File export", "We're sorry, this editor does not support plain text export with this wizard.");
			}
		}
	}
	

	/**
	 * populate the wizard with its pages
	 * 
	 * @see org.eclipse.jface.wizard.Wizard#addPages()
	 */
	public void addPages () {
		// create a new page
		page1 = new ExportWizardPage("Export");
		// and register (via the super-class)
		super.addPage(page1);
	}
	
	/**
	 * create the page controls for all pages of the wizard
	 * 
	 * @see org.eclipse.jface.wizard.Wizard#createPageControls(Composite)
	 */
	public void createPageControls(Composite pageContainer) {
		super.createPageControls(pageContainer);
		// do something additional here?

		// save name of the current resource from the editor in the wizard
		if (editorHelper != null) {
			page1.setOriginalResourceName(editorHelper.getURI(true).lastSegment());
			page1.setSuggestedExtensions(editorHelper.getEditorUtil().compatibleTextFileExtensions());
		//} else if (uihelper != null) {
		//	page1.setOriginalResourceName(uihelper.getURI().lastSegment());
		} else {
			page1.setOriginalResourceName(null);
		}
	}
	
	/**
	 * check whether the wizard may enable the 'finish' button
	 * 
	 * @see org.eclipse.jface.wizard.IWizard#canFinish()
	 */
	public boolean canFinish() {
		return page1.isPageComplete() && !invalidEditor;
	}

	/**
	 * @see org.eclipse.jface.wizard.IWizard#performFinish()
	 */
	public boolean performFinish() {
		//EList<org.eclipse.emf.common.util.URI> seenURIs = new BasicEList<org.eclipse.emf.common.util.URI>();
		//EList<Resource> resources = uihelper.getResources();
		//for (int i=0; i<resources.size(); i++)
		{
			/*
			Resource res = resources.get(i);
			
			EObject modelObject = res.getContents().get(0);
			
			// make sure that we see each model only once
			org.eclipse.emf.common.util.URI uri = modelObject.eResource().getURI();
			if (seenURIs.contains(uri))	continue;
			else seenURIs.add(uri);
			
			// diagram resources always come bundled with their underlying models
			if (modelObject instanceof Diagram) {
				Diagram diag = (Diagram) modelObject;
				uri = diag.getElement().eResource().getURI();
				if (seenURIs.contains(uri))	continue;
				else seenURIs.add(uri);
			}
			*/
			
			String exportText = null;
			String targetFileName = page1.getEnteredTargetFileName();
			
			if (editorHelper != null) {
				exportText = editorHelper.getEditorUtil().getCurrentText();
			}

			if (exportText == null) {
				MessageDialog.openError(workbench.getActiveWorkbenchWindow().getShell(),
						"File export", "There is no support for exporting "+page1.getOriginalResourceName()+".");
				return false;
			}
			// now we are treating an object we haven't seen before
			try {
				// write to external file
				PrintStream outStream = new PrintStream(targetFileName);
				outStream.print(exportText);
				outStream.checkError();
				outStream.close();
			} catch (Exception e) {
				pluginHelper.logError("Failed to write "+targetFileName+".", e);
				MessageDialog.openError(workbench.getActiveWorkbenchWindow().getShell(),
						"File export", "Failed to export currently selected resource.\n\nReason: "+e);
				return false;
			}
		}
		return true;
	}
}
