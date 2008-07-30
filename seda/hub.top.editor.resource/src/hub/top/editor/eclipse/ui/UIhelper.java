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

import hub.top.editor.eclipse.PluginHelper;
import hub.top.editor.eclipse.ResourceHelper;

import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

import org.eclipse.core.resources.IFile;
import org.eclipse.emf.common.util.URI;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.resource.Resource;
import org.eclipse.emf.ecore.resource.ResourceSet;
import org.eclipse.emf.ecore.resource.impl.ResourceSetImpl;
import org.eclipse.emf.ecore.xmi.XMLResource;
import org.eclipse.gmf.runtime.diagram.ui.editparts.DiagramEditPart;
import org.eclipse.gmf.runtime.notation.impl.DiagramImpl;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.jface.viewers.TreeSelection;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.part.FileEditorInput;
import org.xml.sax.SAXParseException;

public class UIhelper {
	// the workbench of this UI helper
	private IWorkbench workbench;
	private PluginHelper plugin;


	// selected resource
	protected Resource resource;
	// the URI of the selected resource
	protected URI selectedURI;	
	
	/**
	 * create a new UI helper for a given workbench
	 * @param workbench
	 */
	public UIhelper (IWorkbench workbench, PluginHelper plugin) {
		this.workbench = workbench;
		this.plugin = plugin;
	}
	
	/**
	 * from the given selection, find the resource that is determined by
	 * the selection and load it into the member fields of this helper
	 * 
	 * @param selection
	 * @param openEditors allow editors to be opened to retrieve resources
	 * @return true if a resource was found and loaded
	 */
	public boolean retrieveSelectedResource (IStructuredSelection selection, boolean openEditors)
	{
		// see what has been selected
		Object selectedObject = null;
		if (selection instanceof TreeSelection)
			selectedObject = ((TreeSelection)selection).getFirstElement();
		else if (selection instanceof StructuredSelection)
			selectedObject = ((StructuredSelection)selection).getFirstElement();
		else
			return false;	// nothing selected
		
		if (selectedObject instanceof IFile) {
			IFile file = (IFile) selectedObject;
			return getSelectedIFileResource(file, openEditors);
			
		} else if (selectedObject instanceof org.eclipse.emf.ecore.resource.Resource) {
			Resource res = (Resource) selectedObject;
			return getSelectedEMFResource(res);
			
		} else if (selectedObject instanceof DiagramEditPart) {
			DiagramEditPart dge = (DiagramEditPart) selectedObject;
			return getSelectedDiagramResource(dge);
			
		} else if (selectedObject instanceof EObject) {
			EObject obj = (EObject) selectedObject;
			Resource res = obj.eResource();
			return getSelectedEMFResource(res);
		}
		
		// System.err.println("no resouce has been selected for exporting");
		plugin.logError("No or unknown resource has been selected.");
		return false;
	}
	
	/**
	 * open the selection in the corresponding editor (if it is not
	 * opened already),
	 * currently opens an editor for the first selected object only
	 * @param selection
	 * @return <code>true</code> iff an editor could be opened
	 */
	public boolean openSelectionInEditor (IStructuredSelection selection) {
		if (!retrieveSelectedResource(selection, false))
			return false;
		IFile file = ResourceHelper.uriToFile(selectedURI);
		if (file == null)
			return false;
		return openFileInEditor(file);
	}
	
	/**
	 * retrieve selected file resource and store it
	 * @param res
	 * @param openEditors allow editors to be opened to retrieve file resource
	 * @return true if retrieval was successful
	 */
	private boolean getSelectedIFileResource (IFile file, boolean openEditors) {
		URI fileURI = URI.createPlatformResourceURI(file.getFullPath().toString(), true);
		
		// register URI handler for the current file
		if (!registerResourceURI(file, openEditors)) {
			selectedURI = fileURI;
			if (openEditors) return false;
			else return true;
		}

		try {
			// load the contents of the resource from the file system.
			// requires that the URI handler for this file is in memory
			ResourceSet resourceSet = new ResourceSetImpl();
			resourceSet.createResource(fileURI);

			Resource res = resourceSet.getResource(fileURI, true);
			
			Map<Object, Object> options = new HashMap<Object, Object>();
			options.put(XMLResource.OPTION_ENCODING, "UTF-8");
			// TODO fix encoding from UTF-8 to a real choice
			res.load(options);
			
			if (res == null)
				return false;
			
			resource = res;
			selectedURI = res.getURI();
			//System.err.println("export file from uri: "+fileURI);
			//System.err.println(this.resources.get(0).getContents().get(0).getClass());
		} catch (IOException e) {
			plugin.logError(e);
			return false;
		} catch (Exception e) {
			if (e.getCause() instanceof SAXParseException) {
				// tried to load a non XML resource
				selectedURI = fileURI;
			} else {
				plugin.logError(e);
				return false;
			}
		}
		return true;
	}
	
	/**
	 * register resource URI to have the XMI parser in memory to parse
	 * the resource from file,
	 * @param file
	 * @param openEditors allow editors to be opened to retrieve file resource
	 * @return true if registering was successful
	 */
	private boolean registerResourceURI (IFile file, boolean openEditors) {
		// open the file in an editor to register the plugin-id
		// TODO this should go without opening a window...
		if (openEditors)
			return openFileInEditor(file);
		return false;
	}

	/**
	 * open file in the corresponding workbench editor
	 * @param file
	 * @return <code>true</code> iff opening the editor was successful
	 */
	private boolean openFileInEditor (IFile file) {
		try
		{
			FileEditorInput input = new FileEditorInput(file);
			IEditorPart editor = workbench.getActiveWorkbenchWindow().getActivePage().findEditor(input);
			// file is not opened yet, open corresponding editor
			if (editor == null)	{
				IWorkbenchPage page = workbench.getActiveWorkbenchWindow().getActivePage();
				editor = page.openEditor
					(new FileEditorInput(file),
					 workbench.getEditorRegistry().getDefaultEditor(file.getFullPath().toString()).getId());
			}
			// bring editor to front
			workbench.getActiveWorkbenchWindow().getActivePage().activate(editor);
			return true;
		} catch (PartInitException e) {
			plugin.logError(e);
			MessageDialog.openError(workbench.getActiveWorkbenchWindow().getShell(),
					"Error opening editor",	e.getMessage());
			return false;
		}
	}
	
	/**
	 * retrieve selected EMF resource and store it
	 * @param res
	 * @return true if retrieval was successful
	 */
	private boolean getSelectedEMFResource (Resource res) {
		try {
			this.resource = res;
			this.selectedURI = res.getURI();
			//System.err.println("export file from editor resource at "+this.selectedURI);
			return true;
		} catch (Exception e) {
			plugin.logError(e);
			return false;
		}
	}
	
	/**
	 * retrieve selected resource from a diagram selection and store it
	 * @param dge
	 * @return true if retrieval was successful
	 */
	private boolean getSelectedDiagramResource (DiagramEditPart dge) {
		try {
			if (dge.getModel() instanceof org.eclipse.gmf.runtime.notation.impl.DiagramImpl)
			{
				DiagramImpl diag = (DiagramImpl) dge.getModel();
				this.resource = diag.getElement().eResource();
				this.selectedURI = diag.getElement().eResource().getURI();
				//System.err.println("export file from visual editor resource at "+this.selectedURI);
				return true;
			}
		} catch (Exception e) {
			plugin.logError(e);
			return false;
		}
		return false;
	}

	/**
	 * @return the selected resources that were found by this helper
	 */
	public Resource getResource() {
		return resource;
	}

	/**
	 * @return the URI of the selected resources that were found by this helper
	 */
	public URI getURI() {
		return selectedURI;
	}
}
