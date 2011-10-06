/*****************************************************************************\
 * Copyright (c) 2009 Dirk Fahland. All rights reserved. EPL1.0/GPL3.0/LGPL3.0
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
 * The Original Code is this file as it was released on January 17, 2009.
 * The Initial Developer of the Original Code are
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

import hub.top.editor.eclipse.FileIOHelper;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.Path;
import org.eclipse.emf.common.util.URI;
import org.eclipse.emf.transaction.TransactionalEditingDomain;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.jface.wizard.Wizard;
import org.eclipse.ui.dialogs.WizardNewFileCreationPage;

public class ConvertFileWizard extends Wizard {

	// main wizard objects
	
	// editing domain in which we operate for file creation and manipultion
	protected TransactionalEditingDomain 	myEditingDomain;	
	// location of the source model
	protected URI 	modelURI;
	// object to convert (usually root object at modelURI)
	protected Object	modelObject;
	// extension of target
	protected String 	extension;

	// wizard pages
	protected WizardNewFileCreationPage 	pageFileCreation;
	
	public ConvertFileWizard(URI domainModelURI, Object resourceRoot,
		      TransactionalEditingDomain editingDomain, String targetExtension)
	{
	    assert domainModelURI != null : "Domain model uri must be specified";
	    assert resourceRoot != null : "Model root element must be specified";
	    assert editingDomain != null : "Editing domain must be specified";


	    myEditingDomain = editingDomain;
	    modelObject = resourceRoot;
	    modelURI = domainModelURI;
	    extension = targetExtension;
	    
		preparePages();
	}
	
	/**
	 * Construct wizard pages and store them as objects local to this
	 * wizard object.
	 */
	protected void preparePages() {
	    // create new wizard page for choosing a file name
		pageFileCreation = new WizardNewFileCreationPage("Convert File", StructuredSelection.EMPTY);
		pageFileCreation.setTitle("File name");
		pageFileCreation.setDescription("Convert model file");
		pageFileCreation.setAllowExistingResources(true);

		// and propose a file name
	    IPath filePath;
	    String fileName = modelURI.trimFileExtension().lastSegment();
	    if (modelURI.isPlatformResource()) {
	      filePath = new Path(modelURI.trimSegments(1).toPlatformString(true));
	    } else if (modelURI.isFile()) {
	      filePath = new Path(modelURI.trimSegments(1).toFileString());
	    } else {
	      // TODO : use some default path
	      throw new IllegalArgumentException("Unsupported URI: " + modelURI); //$NON-NLS-1$
	    }
	    pageFileCreation.setContainerFullPath(filePath);
	    pageFileCreation.setFileName(
	    		FileIOHelper.getFileName(filePath, fileName, extension));

	}
	
	public void addPages() {
		addPage(pageFileCreation);
	    //addPage(diagramRootElementSelectionPage);
	}
	
	@Override
	public boolean performFinish() {
	    //List affectedFiles = new LinkedList();
	    IFile targetFile = pageFileCreation.createNewFile();
	    FileIOHelper.setCharset(targetFile);
	    //affectedFiles.add(targetFile);

	    return false;
	}
}
