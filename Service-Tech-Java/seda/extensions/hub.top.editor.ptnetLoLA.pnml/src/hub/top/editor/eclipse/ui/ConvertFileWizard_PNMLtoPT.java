/*****************************************************************************\
 * Copyright (c) 2008, 2009. All rights reserved. Dirk Fahland. EPL1.0/AGPL3.0
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
 * The Original Code is this file as it was released on June 6, 2009.
 * The Initial Developer of the Original Code are
 *    Dirk Fahland
 * 
 * Portions created by the Initial Developer are Copyright (c) 2008, 2009
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

import fr.lip6.move.pnml.ptnet.hlapi.PetriNetDocHLAPI;
import hub.top.editor.eclipse.ResourceHelper;
import hub.top.editor.ptnetLoLA.PtNet;
import hub.top.editor.ptnetlola.pnml.Activator;
import hub.top.editor.ptnetlola.pnml.PNMLtoPT;

import java.io.IOException;

import org.eclipse.core.commands.ExecutionException;
import org.eclipse.core.commands.operations.IOperationHistory;
import org.eclipse.core.commands.operations.OperationHistoryFactory;
import org.eclipse.core.runtime.IAdaptable;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.NullProgressMonitor;
import org.eclipse.core.runtime.Status;
import org.eclipse.emf.common.util.URI;
import org.eclipse.emf.ecore.resource.Resource;
import org.eclipse.emf.transaction.TransactionalEditingDomain;
import org.eclipse.emf.transaction.util.TransactionUtil;
import org.eclipse.emf.workspace.AbstractEMFOperation;


public class ConvertFileWizard_PNMLtoPT extends ConvertFileWizard {

	public ConvertFileWizard_PNMLtoPT(URI domainModelURI, Object resourceRoot,
			TransactionalEditingDomain editingDomain, String targetExtension)
	{
		super(domainModelURI, resourceRoot, editingDomain, targetExtension);
	}

	@Override
	public boolean performFinish() {
	   
	  
	    if (modelObject instanceof PetriNetDocHLAPI) {
	      PetriNetDocHLAPI root = (PetriNetDocHLAPI)modelObject;
	      
	    	// we understand PNML documents
   	    PNMLtoPT processor = new PNMLtoPT();
   	    processor.process(root);
   	    
   	    // this is the extracted Petri net (only consider the first)
   	    final PtNet net = processor.getNets().get(0); 
   	    
   	    // now store the new object in a new resource
   	    // get the correct editing domain for storing the resource
   	    TransactionalEditingDomain ed = TransactionUtil.getEditingDomain(net);
   	    if (ed == null) ed = myEditingDomain;

   	    // create the resource at the given path/URI
        final IPath filePath = pageFileCreation.getContainerFullPath().append(pageFileCreation.getFileName());
   	    URI targetURI = URI.createPlatformResourceURI(filePath.toString(), true);
   	    final Resource modelResource = ed.getResourceSet().createResource(targetURI);
   	    
   	    // create the transaction-compatible command for storing the object in the created resource
   	    AbstractEMFOperation op = new AbstractEMFOperation (ed, "create P/T net model") {

          @Override
          protected IStatus doExecute(IProgressMonitor arg0, IAdaptable arg1)
              throws ExecutionException {
            
            // add net to resource
            modelResource.getContents().add(net);
            // and save
            try {
              modelResource.save(ResourceHelper.getSaveOptions());
            } catch (IOException e) {
              Activator.getPluginHelper().logError("Unable to store modelresource", e); //$NON-NLS-1$
            }
            
            // great success!
            return Status.OK_STATUS;
          }
   	    };
   	    
   	    // execute the "create" command, we need an operation history for this 
   	    IOperationHistory myHistory = OperationHistoryFactory.getOperationHistory();
   	    try {
   	      myHistory.execute(op, new NullProgressMonitor(), null);
   	    } catch (ExecutionException e) {
   	      Activator.getPluginHelper().logError("Could not store model "+targetURI, e);
   	    }
        
   	    return true;
	    }
	    
	    return false;
	}
}
