/*****************************************************************************\
 * Copyright (c) 2009 Dirk Fahland. All rights reserved. EPL1.0/GPL3.0/LGPL3.0
 * 
 * ServiceTechnolog.org - FlowChart Editors
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
 * The Original Code is this file as it was released on March 09, 2009.
 * The Initial Developer of the Original Code are
 * 		Dirk Fahland
 * 
 * Portions created by the Initial Developer are Copyright (c) 2009
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

import fr.lip6.move.pnml.ptnet.hlapi.PetriNetDocHLAPI;
import hub.top.editor.eclipse.FileIOHelper;
import hub.top.editor.ptnetLoLA.PtNet;
import hub.top.editor.ptnetlola.pnml.PNMLtoPT;
import hub.top.lang.flowcharts.ptnet.pnml.FCtoPNML;

import org.eclipse.core.runtime.IPath;
import org.eclipse.emf.common.util.URI;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.resource.Resource;
import org.eclipse.emf.transaction.TransactionalEditingDomain;
import org.eclipse.jface.wizard.Wizard;
import org.eclipse.osgi.util.NLS;

public class ConvertFileWizard_FCtoPTnetDiag extends ConvertFileWizard {

	public ConvertFileWizard_FCtoPTnetDiag(URI domainModelURI, Object resourceRoot,
			TransactionalEditingDomain editingDomain, String targetExtension)
	{
		super(domainModelURI, resourceRoot, editingDomain, targetExtension);
	}

	@Override
	public boolean performFinish() {
	    //IFile targetFile = pageFileCreation.createNewFile();
	    //FileIOHelper.setCharset(targetFile);
	  
      // create the resource at the given path/URI
      final IPath targetPath = pageFileCreation.getContainerFullPath().append(pageFileCreation.getFileName());

	    if (modelObject instanceof org.eclipse.gmf.runtime.notation.Diagram) {
	    	org.eclipse.gmf.runtime.notation.Diagram d = (org.eclipse.gmf.runtime.notation.Diagram)modelObject;
	    	if (d.getElement() instanceof hub.top.lang.flowcharts.WorkflowDiagram) {
	    		// we understand workflow diagrams, invoke translation to PNML
	    	  process(targetPath, d.getElement());
	    	  return true;
	    	}
	    } else if (modelObject instanceof hub.top.lang.flowcharts.WorkflowDiagram) {
    		// we understand workflow diagrams, invoke translation to PNML
	        process(targetPath, (EObject)modelObject);
    	    return true;
	    }
	    return false;
	}
	
	private void process(IPath targetPath, EObject modelObject) {
	  // translate FC to PNML
    FCtoPNML toPNML = new FCtoPNML(modelObject);
    
    FileIOHelper.writeFile(targetPath.addFileExtension("pnml"), toPNML.getText());
    
    PetriNetDocHLAPI doc = toPNML.getPNMLDoc();
    
    // translate PNML to P/T net
    PNMLtoPT toPT = new PNMLtoPT();
    toPT.process(doc);
    PtNet net = toPT.getNets().get(0);
    Resource r = FileIOHelper.writeEObjectToResource(net, myEditingDomain, targetPath);

    // initialize diagram for P/T net with a wizard
    Wizard wizard = new hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLANewDiagramFileWizard(
        r.getURI(), net, myEditingDomain);
    wizard
        .setWindowTitle(NLS
            .bind(
                hub.top.editor.ptnetLoLA.diagram.part.Messages.PtnetLoLAInitDiagramFileAction_InitDiagramFileWizardTitle,
                hub.top.editor.ptnetLoLA.diagram.edit.parts.PtNetEditPart.MODEL_ID));
    hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLADiagramEditorUtil.runWizard(
        getShell(), wizard, "InitDiagramFile"); //$NON-NLS-1$
	}
	

	
}
