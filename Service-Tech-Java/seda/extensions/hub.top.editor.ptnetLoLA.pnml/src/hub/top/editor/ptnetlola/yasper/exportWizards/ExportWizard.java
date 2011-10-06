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

package hub.top.editor.ptnetlola.yasper.exportWizards;

import hub.top.editor.eclipse.FileIOHelper;
import hub.top.editor.eclipse.PluginHelper;
import hub.top.editor.eclipse.ResourceHelper;
import hub.top.editor.eclipse.ui.UIhelper;
import hub.top.editor.ptnetlola.pnml.Activator;
import hub.top.editor.ptnetlola.pnml.PTtoPNML_Yasper;
import hub.top.lang.ptnet.yasper.YasperEPNML114.Pnml;

import java.io.IOException;

import org.eclipse.core.commands.ExecutionException;
import org.eclipse.core.commands.operations.IOperationHistory;
import org.eclipse.core.commands.operations.OperationHistoryFactory;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IAdaptable;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.NullProgressMonitor;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Status;
import org.eclipse.emf.common.util.URI;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.resource.Resource;
import org.eclipse.emf.transaction.TransactionalEditingDomain;
import org.eclipse.emf.transaction.util.TransactionUtil;
import org.eclipse.emf.workspace.AbstractEMFOperation;
import org.eclipse.gmf.runtime.emf.core.GMFEditingDomainFactory;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.wizard.IWizardPage;
import org.eclipse.jface.wizard.Wizard;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.IExportWizard;
import org.eclipse.ui.IWorkbench;

/*
import fr.lip6.move.pnml.ptnet.presentation.PtnetEditor;
import fr.lip6.move.pnml.utilities.LOGMEDIA;
import fr.lip6.move.pnml.utilities.PNMLPrinter;
import fr.lip6.move.pnml.utilities.PnmlException;
import fr.lip6.move.pnml.utilities.UtilitiesFactory;
import fr.lip6.move.pnml.utilities.UtilitiesPackage;
*/

public class ExportWizard extends Wizard implements IExportWizard {
	
	// the current workbench where this wizard is called
	protected IWorkbench workbench;
	// the workbench selection when the wizard was started.
	protected IStructuredSelection selection;
	// the UI helper supporting this wizard
	protected UIhelper uihelper;
	protected PluginHelper  pluginHelper = Activator.getPluginHelper();
	
	// the current Petri net editor
	//protected PtnetEditor editor;

	
	// the pages of the wizard, stored individually for cross-page checks
	protected ExportWizardPage page1;
	
	/**
	 * create a new wizard
	 */
	public ExportWizard() {
		setWindowTitle("Export to PNML");	
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
		this.uihelper = new UIhelper(workbench, pluginHelper);
		
		// read current selection into UIHelper
		uihelper.getSelectedResource(this.selection);
		// TODO: remove UI helper
		
		/*
		IWorkbenchWindow window = workbench.getActiveWorkbenchWindow();
		IEditorPart _editor = (IEditorPart) window.getActivePage().getActiveEditor();
		
		if (_editor instanceof hub.metrik.lang.petri.diagram.part.PetriDiagramEditor)
		{
			PetriDiagramEditor pde = (PetriDiagramEditor) _editor; 
			System.err.println(pde.getDiagram());
			System.err.println(pde.getDiagramDocument());
			
			Diagram diag = pde.getDiagram();
			hub.metrik.lang.petri.Net net = (hub.metrik.lang.petri.Net) diag.getElement();
		}
		else if (_editor instanceof fr.lip6.move.pnml.ptnet.presentation.PtnetEditor) 
		{
			this.editor = (PtnetEditor) _editor;
			this.resources = this.editor.getEditingDomain().getResourceSet().getResources();
		}
		*/
		
	}
	

	/**
	 * populate the wizard with its pages
	 * 
	 * @see org.eclipse.jface.wizard.Wizard#addPages()
	 */
	public void addPages () {
		// create a new page
		page1 = new ExportWizardPage("Export to PNML");
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

		if (uihelper.getResource() != null)	{
			// save name of the current resource from the editor in the wizard
			page1.setModelName(uihelper.getURI().lastSegment());
		} else {
			page1.setModelName(null);
		}
	}
	
	@Override
	public IWizardPage getStartingPage() {
	  if (uihelper.getResource() == null) {
	     MessageDialog.openError(workbench.getActiveWorkbenchWindow().getShell(), "No model found.",
         "Please select a model or a file that you would like to export.");
	    return null;
	  } else {
	    return super.getStartingPage();
	  }
	}
	
	/**
	 * check whether the wizard may enable the 'finish' button
	 * 
	 * @see org.eclipse.jface.wizard.IWizard#canFinish()
	 */
	public boolean canFinish() {
		return page1.isPageComplete();
	}

	/**
	 * @see org.eclipse.jface.wizard.IWizard#performFinish()
	 */
	public boolean performFinish() {

	  // get model
		Resource res = uihelper.getResource();
		EObject modelObject = res.getContents().get(0);
		
		if (modelObject == null)
		  return false;
		
		// translate model to PNML
		PTtoPNML_Yasper pt2yasper = new PTtoPNML_Yasper(modelObject);
    final Pnml pnmlDoc = pt2yasper.process();
    
    // now we export the model in 3 steps
    // 1) let Eclipse generate the correct XML by writing the PNML
    //    object into a temporary workspace resource
    // 2) read the temporary file and write the result to the target path
    // 3) remove the temporary file
    
    // 1) create temporary Eclipse resource
    // now store the new object in a new resource
    // get the correct editing domain for storing the resource
    TransactionalEditingDomain ed = TransactionUtil.getEditingDomain(pnmlDoc);
    if (ed == null) ed = GMFEditingDomainFactory.INSTANCE.createEditingDomain();

    // create the resource at the given path/URI
    String fileName = res.getURI().trimFileExtension().lastSegment()+"_tmp";
    URI tmpURI = res.getURI().trimSegments(1).appendSegment(fileName).appendFileExtension("pnml");

    Resource pnmlRes = FileIOHelper.writeEObjectToResource(pnmlDoc, ed, tmpURI);
    if (pnmlRes == null)
      return false;
    // done creating the temporary resource

    // 2) read the contents of the written temporary file and write it to the target
    String pnmlString = FileIOHelper.readFile(new Path(tmpURI.toPlatformString(true)));
    try {
      FileIOHelper.writeFile(page1.getTargetFileName(), pnmlString);
    } catch (IOException e) {
      Activator.getPluginHelper().logError("Error writing file "+page1.getTargetFileName()+".", e);
    }
    
    // 3) delete the temporary file
    try {
      ResourcesPlugin.getWorkspace().getRoot().findMember(new Path(tmpURI.toPlatformString(true))).delete(true, null);
    } catch (CoreException e) {
      Activator.getPluginHelper().logError("Error deleting temporary file "+tmpURI+".", e);
    }
    
		return true;
	}
}
