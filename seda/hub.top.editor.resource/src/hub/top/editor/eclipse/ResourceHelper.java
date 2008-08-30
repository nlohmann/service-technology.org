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
import java.util.Collection;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.Path;
import org.eclipse.emf.common.util.BasicEList;
import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.common.util.URI;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.resource.Resource;
import org.eclipse.emf.ecore.resource.ResourceSet;
import org.eclipse.emf.ecore.util.EcoreUtil;
import org.eclipse.emf.ecore.xmi.XMLResource;
import org.eclipse.emf.transaction.TransactionalEditingDomain;
import org.eclipse.emf.transaction.util.TransactionUtil;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.actions.WorkspaceModifyOperation;

public class ResourceHelper {
	
	/**
	 * convert file to path
	 * @param file
	 * @return
	 */
	public static IPath fileToPath (IFile file) {
		if (file == null) return null;
		return file.getFullPath();
	}
	
	/**
	 * convert path to URI
	 * @param path
	 * @return
	 */
	public static URI pathToUri (IPath path) {
		if (path == null) return null;
		return URI.createPlatformResourceURI(path.toString(), true);
	}
	
	/**
	 * convert path to file handle
	 * @param path
	 * @return
	 */
	public static IFile pathToFile (IPath path) {
		if (path == null) return null;
		return ResourcesPlugin.getWorkspace().getRoot().getFile(path);
	}

	/**
	 * convert uri to file handle
	 * @param uri
	 * @return
	 */
	public static IFile uriToFile (URI uri) {
		if (uri != null && uri.isPlatformResource()) {
			String platformString = uri.toPlatformString(true);
			IResource res = ResourcesPlugin.getWorkspace().getRoot().findMember(platformString);
			if (res instanceof IFile)
				return (IFile) res;
		}
		return null;
	}
	
	/**
	 * convert uri to file handle
	 * @param uri
	 * @return
	 */
	public static IFile uriToFile (java.net.URI uri) {
		return ResourcesPlugin.getWorkspace().getRoot().getFile(new Path(uri.getPath())); 
	}
	
	/**
	 * @param uri
	 * @return true iff the <code>uri</code> denotes a resource in the current workspace
	 */
	public static boolean isWorkspaceResource (URI uri)
	{
		// check whether the uri denotes a valid resource
		if (uri.isPlatformResource()){
			String platformString = uri.toPlatformString(true);
			if (ResourcesPlugin.getWorkspace().getRoot().findMember(platformString, true) == null) {
				return false;
			}
		}
		return true;
	}
	
	/**
	 * convert uri to resource
	 * @param uri
	 * @param resources resource set into which resource shall be loaded,
	 *                  (can be null)
	 * @return
	 */
	public static Resource uriToResource (URI uri, ResourceSet resources) {
		if (uri == null)
			return null;
		
		if (resources == null)
			return null;
		
		// check whether the uri denotes a valid resource
		if (isWorkspaceResource(uri))
			return resources.getResource(uri, true);
		else
			return null;
	}
	
	private final PluginHelper fPluginHelper;
	
	protected TransactionalEditingDomain editingDomain;
	protected ResourceSet resources;
	
	private Resource 	resource;
	private IPath 		resourcePath;
	private IFile		resourceFile;
	private URI			resourceURI;
	
	private int			mostRecentUpdate;
	private static final int MRU_NONE     = 0;
	private static final int MRU_RESOURCE = 1;
	private static final int MRU_PATH     = 2;
	private static final int MRU_FILE     = 3;
	private static final int MRU_URI      = 4;
	
	private String		feasibleExtension;
	
	/**
	 * helper class to modify, store and convert resources
	 * @param pluginHelper
	 */
	public ResourceHelper(PluginHelper pluginHelper) {
		this(pluginHelper, null);
	}
	
	/**
	 * helper class to modify, store and convert resources
	 * @param pluginHelper
	 * @param ext extension of files this helper will work on 
	 */
	public ResourceHelper(PluginHelper pluginHelper, String ext) {
		fPluginHelper = pluginHelper;
		editingDomain = TransactionalEditingDomain.Factory.INSTANCE.createEditingDomain();
		resources = editingDomain.getResourceSet();
		mostRecentUpdate = MRU_NONE;
		setFeasibleExtension(ext);
	}
	
	/**
	 * @return plugin helper for this resource
	 */
	public PluginHelper getPluginHelper() {
		return fPluginHelper;
	}
	
	/*
	 * methods to consistently manipulate the member fields of
	 * this resource helper
	 */
	
	/**
	 * resolve all fields from the most recently updated field
	 */
	public void resolveFields() {
		URI uri;
		if (mostRecentUpdate == MRU_NONE) {
			return;
		} else if (mostRecentUpdate == MRU_FILE) {
			uri = pathToUri(fileToPath(resourceFile));
		} else if (mostRecentUpdate == MRU_PATH) {
			uri = pathToUri(resourcePath);
		} else if (mostRecentUpdate == MRU_URI) {
			uri = resourceURI;
		} else if (mostRecentUpdate == MRU_RESOURCE) {
			if (resource != null)
				uri = resource.getURI();
			else
				uri = null;	// in case it is a fresh editor helper with no data yet
		} else {
			fPluginHelper.logError(new Error("Failed to resolve resource helper, unknown most recent update: "+mostRecentUpdate));
			mostRecentUpdate = MRU_NONE;
			return;
		}
		resolveFields(uri);
		mostRecentUpdate = MRU_NONE;
	}
	
	/**
	 * resolve and update all fields of this helper based on a given URI
	 * @param uri
	 */
	private void resolveFields(URI uri) {
		// derive file name (if necessary)
		if (mostRecentUpdate != MRU_FILE)
			resourceFile = uriToFile(uri);
		else
			resourceFile = null;

		// derive path (if necessary)
		if (mostRecentUpdate != MRU_PATH && resourceFile != null)
			resourcePath = fileToPath(resourceFile);

		// update uri
			resourceURI = uri;
		// create and/or load the resource for this file (if necessary)
		if (mostRecentUpdate != MRU_RESOURCE) {
			resource = uriToResource(uri, resources);
		}
	}
	
	/**
	 * set file handle
	 * @param file
	 * @param resolve update all other fields for the new value
	 */
	public void setFile(IFile file, boolean resolve) {
		resourceFile = file;
		mostRecentUpdate = MRU_FILE;
		if (resolve) resolveFields();
	}
	
	/**
	 * @param smart try all fields of this helper to retrieve info
	 * @return the file that is bound to this resource (if any)
	 */
	public IFile getFile (boolean smart) {
		if (resourceFile != null) return resourceFile;
		if (!smart) return null;
		
		// retrieve file from the other fields
		if (resourcePath == null) return null;
		return pathToFile(resourcePath);
	}
	
	/**
	 * @return list of files affected when working with this helper
	 */
	public List<IFile> affectedFiles() {
		List<IFile> fList = new LinkedList<IFile>();
		if (getFile(true) != null)
			fList.add(getFile(true));
		return fList;
	}
	
	/**
	 * set resource URI
	 * @param file
	 * @param resolve update all other fields for the new value
	 */
	public void setURI(URI uri, boolean resolve) {
		resourceURI = uri;
		mostRecentUpdate = MRU_URI;
		if (resolve) resolveFields();
	}
	
	/**
	 * @param smart try all fields of this helper to retrieve info
	 * @return uri that can be retrieved from this helper
	 * by trying all possible source
	 */
	public URI getURI (boolean smart) {
		if (resourceURI != null) return resourceURI;
		if (!smart) return null;
		
		// retrieve uri from other fields
		if (resource != null) return resource.getURI();
		if (resourcePath != null) return pathToUri(resourcePath);
		if (resourceFile != null) return pathToUri(fileToPath(resourceFile));
		return null;
	}
	
	/**
	 * set path to resource
	 * @param file
	 * @param resolve update all other fields for the new value
	 */
	public void setPath(IPath path, boolean resolve) {
		resourcePath = path;
		mostRecentUpdate = MRU_PATH;
		if (resolve) resolveFields();
	}
	
	/**
	 * retrieve path of this resource
	 * @param smart
	 */
	public IPath getPath (boolean smart) {
		if (resourcePath != null) return resourcePath;
		if (smart) {
			String pathStr = null;
			// retrieve uri from other fields
			if (resource != null) pathStr = resource.getURI().path();
			if (resourceURI != null) pathStr = resourceURI.path();
			if (resourceFile != null) pathStr = resourceFile.toString();
			return new Path(pathStr);
		}
		return null;
	}
	
	/**
	 * @return the feasible file extension
	 */
	public String getFeasibleExtension () {
		return feasibleExtension;
	}
	
	/**
	 * @return the feasible file extension
	 */
	public void setFeasibleExtension (String ext) {
		feasibleExtension = ext;
	}

	/**
	 * @return the editing domain that manages the resources
	 * of this helper
	 */
	public TransactionalEditingDomain getEditingDomain() {
		assert(editingDomain != null);
		assert(editingDomain instanceof TransactionalEditingDomain);
		return editingDomain;
	}
	
	/**
	 * replace resource set of this helper with a different
	 * resource set, reloads all member fields of this helper
	 * @param res new resource set
	 */
	public void refreshResourceSet(ResourceSet res) {
		URI uri = getURI(true);
		// disconnect current resource from old editing domain
		if (resource != null)
			TransactionUtil.disconnectFromEditingDomain(resource);
		// install new editing domain
		this.resources = res;	// replace resource set with external
		setURI(uri, true);		// refresh all fields in the new resource set
	}
	
	/**
	 * set resource in this helper
	 * @param res
	 */
	public void setResource (Resource res, boolean resolve) {
		resource = res;
		mostRecentUpdate = MRU_RESOURCE;
		if (resolve) resolveFields();
	}
	
	/**
	 * @param smart try all fields of this helper to retrieve info
	 * @return the resource that is wrapped in this helper
	 */
	public Resource getResource (boolean smart) {
		if (resource != null) return resource;
		if (!smart) return null;
		
		// retrieve resource from other fields
		if (resourceURI != null)
			return uriToResource(resourceURI, resources);
		if (resourcePath != null)
			return uriToResource(pathToUri(resourcePath), resources);
		if (resourceFile != null)
			return uriToResource(pathToUri(fileToPath(resourceFile)),resources);
		return null;
	}
	
	/**
	 * 
	 * @return
	 */
	public Resource createResource () {
		// check whether resource already exists
		if (resource != null) return resource;
		
		// no resource available, create new resource from uri
		URI uri = getURI(true);
		if (uri == null) return null;
		setResource(resources.createResource(uri), true);
		return resource;
	}
	
	/**
	 * @return <code>true</code> iff stored resource holds an object
	 */
	public boolean hasModel() {
		 return (getResource(true) != null && !getResource(true).getContents().isEmpty());
	}
	
	/**
	 * @return the model of this resource
	 */
	public EList<EObject> getModel () {
		Resource res = getResource(true);
		if (res == null) return null;
		return res.getContents();
	}
	
	/**
	 * @return the root object of this resource
	 */
	public EObject getModelRoot() {
		Resource res = getResource(true);
		if (res == null) return null;
		return res.getContents().get(0);
	}
	
	/*
	 * methods to store the resource (with new contents)
	 */
	
	/**
	 * @return standard UTF-8 encoding save options
	 */
	public Map<String, String> getSaveOptions() {
		// Save the contents of the resource to the file system.
		Map<String, String> options = new HashMap<String, String>();
		options.put(XMLResource.OPTION_ENCODING, "UTF-8");
		return options;
	}

	private EList<EObject> newModelContents = null;
	/**
	 * remember model to be stored in this resource
	 * @param eObj
	 */
	public void addToModelContents(EObject eObj) {
		if (newModelContents == null)
			newModelContents = new BasicEList<EObject>();
		newModelContents.add(eObj);
	}
	
	/**
	 * @return the contents that shall be added to the resource,
	 * this contents can set using <code>{@link ResourceHelper#addToModelContents(EObject)}</code>
	 */
	public EList<EObject> getNewModelContents () {
		return newModelContents;
	}
	
	/**
	 * @return the root object of the new model contents,
	 * this contents can set using <code>{@link ResourceHelper#addToModelContents(EObject)}</code>
	 */
	public EObject getNewModelRoot () {
		return newModelContents.get(0);
	}
	
	/**
	 * @return <code>true</code> iff the additional contents of
	 * this helper {@link ResourceHelper#addToModelContents(EObject)}
	 * is contained in the resource
	 */
	public boolean isInSync() {
		if (newModelContents == null) return true;
		if (getResource(true) != null
			&& getResource(true).getContents().containsAll(newModelContents))
		{
			return true;
		}
		return false;
	}
	
	/**
	 * create operation to store model contents in the resource,
	 * the operation needs to be executed, for instance using
	 * <code>{@link IWorkbenchWindow#run(boolean, boolean, org.eclipse.jface.operation.IRunnableWithProgress)}</code>
	 * @return 
	 */
	public NewModelFileOperation storeModelContentsInResource(boolean overwrite) {
		NewModelFileOperation op = new NewModelFileOperation(this, overwrite);
		return op;
	}
	
	/**
	 * Create a new model file in the current workspace based on
	 * the fields of this helper, this method must be called
	 * with an <code>{@link WorkspaceModifyOperation}</code>. This
	 * method is called from <code>{@link NewModelFileOperation}</code>
	 * which can be generated for this helper with
	 * <code>{@link ResourceHelper#storeModelContentsInResource()}</code>
	 * 
	 * @throws IOException
	 */
	protected void createNewModelFile () throws IOException {
		// resolve fields from the helper
		resolveFields();
		// if the resource is in sync, don't do anything
		if (isInSync()) return;
		
		Resource resource = createResource();
		assert (resource != null);
		
		// copy the new model contents and
		Collection<EObject>modelContents = EcoreUtil.copyAll(getNewModelContents());
		// put all model objects into the file resource
		if (modelContents != null && modelContents.size() > 0) {
			resource.getContents().addAll(modelContents);
		}

		// Save the contents of the resource to the file system.
		resource.save(getSaveOptions());
	}
	
	/**
	 * Update contents of an existing model file in the current
	 * workspace based on the fields of this helper, this method will
	 * replace all contents of the resource with the
	 * <code>{@link ResourceHelper#getModelContents(boolean)}</code> of
	 * this helper.
	 * 
	 * This method must be called with an 
	 * <code>{@link WorkspaceModifyOperation}</code>. This
	 * method is called from <code>{@link UpdateDiagramFileOperation}</code>
	 * which is generated for this helper from
	 * <code>{@link DiagramEditorHelper#updateDiagramContentsInResource()}</code>
	 * 
	 * @param doSave invoke the save method on the resource
	 * @param overwrite whether the contents of the resource will be cleared before the update
	 * 
	 * @throws IOException
	 */
	protected void updateModelFile (boolean doSave, boolean overwrite) throws IOException {
		// resolve fields and empty existing resource
		resolveFields();
		// if the resource is in sync, don't do anything
		if (isInSync()) return;

		Resource resource = getResource(false);
		// clear resource if required
		if (overwrite && resource.getContents().size() > 0) {
			resource.getContents().clear();
		}
		
		// copy model contents and 
		Collection<EObject>modelContents = EcoreUtil.copyAll(getNewModelContents());
		// put all model objects into file
		if (modelContents != null && modelContents.size() > 0) {
			resource.getContents().addAll(modelContents);
		}
		
		// save changed resource if this is desired
		if (doSave) {
			resource.save(getSaveOptions());
		}
	}
	
	/**
	 * returns a string representation of this object
	 * @see java.lang.Object#toString()
	 */
	@Override
	public String toString() {
		return this.getClass().getSimpleName()
			+" ["+resource+", "+resourcePath+", "+resourceFile+", "+resourceURI+"]";
	}
}
