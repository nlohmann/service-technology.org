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

package hub.top.editor.ptnetlola.pnml.exportWizards;

import fr.lip6.move.pnml.framework.utils.exception.InvalidIDException;
import fr.lip6.move.pnml.framework.utils.exception.VoidRepositoryException;
import fr.lip6.move.pnml.ptnet.PetriNetDoc;
import hub.top.editor.eclipse.FileIOHelper;
import hub.top.editor.eclipse.PluginHelper;
import hub.top.editor.eclipse.ui.UIhelper;
import hub.top.editor.ptnetlola.pnml.Activator;
import hub.top.editor.ptnetlola.pnml.PTtoPNML;

import java.io.IOException;

import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.resource.Resource;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.wizard.IWizardPage;
import org.eclipse.jface.wizard.Wizard;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.IExportWizard;
import org.eclipse.ui.IWorkbench;


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
		
		// TODO: remove UIHelper
		
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

	  // get the model
		Resource res = uihelper.getResource();
		EObject modelObject = res.getContents().get(0);
		
    try {
      // initialize PNML-Framework
      fr.lip6.move.pnml.framework.utils.ModelRepository.getInstance().createDocumentWorkspace("void");
      // translate the model to PNML
      PetriNetDoc doc = PTtoPNML.transformModel(modelObject);      
      
      // prepare PNML document export
      final boolean dropNameSpace = false;  // set to true for dropping all namespace references in the output
      final String ns_str = " xmlns=\""+doc.getXmlns()+"\"";
      PnmlExport pex = new PnmlExport () {
        @Override
        public String postProcess(String output) {
          if (!dropNameSpace) return output;
          
          // clear all name space references in the output string to make it parseable by Yasper (or other tools)
          int pos = output.indexOf(ns_str);
          while (pos != -1) {
            output = output.substring(0, pos)+output.substring(pos+ns_str.length());
            pos = output.indexOf(ns_str);
          }
          return output;
        }
      };
      String result = pex.postProcess(doc.toPNML());
      
      // write the result to a file
      // we are not using the PNML framework functionality because we are not using the HL-API
      FileIOHelper.writeFile(page1.getTargetFileName(), result);
      
    } catch (InvalidIDException ex) {
      Activator.getPluginHelper().logError("Error creating PNML document from "+uihelper.getURI()+".", ex);
    } catch (IOException ex) {
      Activator.getPluginHelper().logError("Error creating PNML document from "+uihelper.getURI()+".", ex);
    }

    // finalize PNML-Framework
    try {
      fr.lip6.move.pnml.framework.utils.ModelRepository.getInstance().destroyCurrentWorkspace();
    } catch (VoidRepositoryException ex) {
      Activator.getPluginHelper().logError("Error creating PNML document from "+uihelper.getURI()+".", ex);
    }

		return true;
	}
}
