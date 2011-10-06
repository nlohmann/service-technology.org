/*****************************************************************************\
 * Copyright (c) 2009 Konstanze Swist, Dirk Fahland. EPL1.0/AGPL3.0
 * All rights reserved.
 * 
 * ServiceTechnolog.org - Modeling Languages
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
 * The Original Code is this file as it was released on December 12, 2009.
 * The Initial Developer of the Original Code are
 *    Konstanze Swist
 *    Dirk Fahland
 * 
 * Portions created by the Initial Developer are Copyright (c) 2009
 * the Initial Developer. All Rights Reserved.
 * 
 * Contributor(s):
 * 
 * Alternatively, the contents of this file may be used under the terms of
 * the GNU Affero General Public License Version 3 or later (the "GPL") in
 * which case the provisions of the AGPL are applicable instead of those above.
 * If you wish to allow use of your version of this file only under the terms
 * of the AGPL and not to allow others to use your version of this file under
 * the terms of the EPL, indicate your decision by deleting the provisions
 * above and replace them with the notice and other provisions required by the 
 * AGPL. If you do not delete the provisions above, a recipient may use your
 * version of this file under the terms of any one of the EPL or the AGPL.
\*****************************************************************************/

package hub.top.editor.eclipse.ui;

import hub.top.editor.eclipse.FileIOHelper;
import hub.top.editor.eclipse.ui.ConvertFileWizard;
import hub.top.lang.flowcharts.ptnet.FCtoPTnet;

import org.eclipse.core.resources.IFile;
import org.eclipse.emf.common.util.URI;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.transaction.TransactionalEditingDomain;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PlatformUI;

public class ConvertFileWizard_FCtoINA extends ConvertFileWizard {

	public ConvertFileWizard_FCtoINA(URI domainModelURI, Object resourceRoot,
			TransactionalEditingDomain editingDomain, String targetExtension)
	{
		super(domainModelURI, resourceRoot, editingDomain, targetExtension);
	}

	@Override
	public boolean performFinish() {
    IFile targetFile = pageFileCreation.createNewFile();
    
    FileIOHelper.setCharset(targetFile);
    IWorkbenchWindow win = PlatformUI.getWorkbench().getActiveWorkbenchWindow();
    
    URI targetURI = URI.createPlatformResourceURI(targetFile.getFullPath().toString(), true);

    if (modelObject instanceof org.eclipse.gmf.runtime.notation.Diagram) {
    	org.eclipse.gmf.runtime.notation.Diagram d = (org.eclipse.gmf.runtime.notation.Diagram)modelObject;
    	if (d.getElement() instanceof hub.top.lang.flowcharts.WorkflowDiagram) {
    		// we understand workflow diagrams, invoke translation to P/T-Net
    	  FCtoPTnet toPTnet = new FCtoPTnet(d.getElement());
    	  //FileIOHelper.writeFile(targetFile, toPTnet.getText());
    	  FileIOHelper.writeEcoreResourceToFile(win, targetURI, toPTnet.getModel());
    	  return true;
    	}
    } else if (modelObject instanceof hub.top.lang.flowcharts.WorkflowDiagram) {
  		// we understand workflow diagrams, invoke translation to PNML
      FCtoPTnet toPTnet = new FCtoPTnet((EObject)modelObject);
      //FileIOHelper.writeFile(targetFile, toPTnet.getText());
      FileIOHelper.writeEcoreResourceToFile(win, targetURI, toPTnet.getModel());
 	    return true;
    }
    return false;
	}
}
