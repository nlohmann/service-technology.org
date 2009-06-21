/*****************************************************************************\
 * Copyright (c) 2008, 2009. Manja Wolf, Dirk Fahland. EPL1.0/AGPL3.0
 * All rights reserved.
 * 
 * ServiceTechnolog.org - Greta
 *                       (Graphical Runtime Environment for Adaptive Processes) 
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
 * 		Manja Wolf, Dirk Fahland
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

package hub.top.greta.run;

import hub.top.adaptiveProcess.diagram.part.AdaptiveProcessDiagramViewer;
import hub.top.adaptiveSystem.AdaptiveProcess;
import hub.top.adaptiveSystem.AdaptiveSystem;
import hub.top.adaptiveSystem.diagram.edit.parts.AdaptiveProcessEditPart;
import hub.top.adaptiveSystem.diagram.edit.parts.AdaptiveSystemEditPart;
import hub.top.adaptiveSystem.diagram.part.AdaptiveSystemDiagramEditor;
import hub.top.greta.run.editors.AdaptiveSystemMultiEditor;

import org.eclipse.gef.EditPart;
import org.eclipse.gmf.runtime.diagram.ui.editpolicies.CanonicalConnectionEditPolicy;
import org.eclipse.gmf.runtime.diagram.ui.resources.editor.parts.DiagramDocumentEditor;
import org.eclipse.ui.IEditorPart;

public class AdaptiveProcessSimulationView {
	public DiagramDocumentEditor oldProcessViewEditor;
	public DiagramDocumentEditor processViewEditor;
	public AdaptiveSystem adaptiveSystem;
	public AdaptiveSystem oldAdaptiveSystem;
	public AdaptiveProcess adaptiveProcess;
	public EditPart apEditPart;

	public AdaptiveProcessSimulationView() {
		this.oldProcessViewEditor = null;
		this.processViewEditor = null;
		this.adaptiveSystem = null;
		this.oldAdaptiveSystem = null;
		this.adaptiveProcess = null;
		this.apEditPart = null;
	}
	
	/**
	 * Set this action's current processViewEditor, only set those
	 * editors which are capable of handling this action's operations
	 * 
	 * set corresponding action-wide fields based on that editor
	 * 
	 * @param editor
	 */
	public void setProcessViewEditor_andFields(IEditorPart editor) {
		
		DiagramDocumentEditor ed = null;
		AdaptiveSystem 	as = null;
		AdaptiveProcess ap = null;
		EditPart apE = null;
		
		// unwrap multipage editor
		if (editor instanceof AdaptiveSystemMultiEditor) {
			AdaptiveSystemMultiEditor me = (AdaptiveSystemMultiEditor)editor;
			editor = me.getActiveEditor();
		}
		
		if (editor instanceof AdaptiveSystemDiagramEditor) {
			ed = (DiagramDocumentEditor)editor;
			
			// displays an entire adaptive system, get diagram's root element
			as = (AdaptiveSystem) ed.getDiagram().getElement();
			
			ap = as.getAdaptiveProcess();
			
			AdaptiveSystemEditPart asEditPart = (AdaptiveSystemEditPart) ed.getDiagramEditPart();
			apE = null;
			for(Object object : asEditPart.getChildren()) {
				if(object instanceof AdaptiveProcessEditPart) { 
					apE = (AdaptiveProcessEditPart) object;
					break;
				}
			}
			
		} else if (editor instanceof AdaptiveProcessDiagramViewer) {
			ed = (DiagramDocumentEditor)editor;
			
			// displays only the adaptive process, contained in an adaptive system
			ap = (AdaptiveProcess) ed.getDiagram().getElement();
			as = (AdaptiveSystem) ap.eContainer();
			apE = ed.getDiagramEditPart();
			
		} else {
			ed = null; ap = null; as = null; apE = null;
		}
		
		// validation step
		if (ed != null && !(ed.getDiagramEditPart().getEditPolicy("Canonical")
					instanceof CanonicalConnectionEditPolicy))
		{
			// current editor does not implement the required canonical edit policy
			ed = null; ap = null; as = null; apE = null;
		}
		
		if (apE == null || apE.isActive() == false) {
			ed = null; ap = null; as = null; apE = null;
		}
		
		// store old adaptive system for selectionChanged()
		oldAdaptiveSystem = as;
		oldProcessViewEditor = processViewEditor;
		
		// set new values
		processViewEditor = ed;
		adaptiveProcess = ap;
		adaptiveSystem = as;
		apEditPart = apE;
	}
	
	public void reset() {
		oldProcessViewEditor = processViewEditor;
		processViewEditor = null;
		adaptiveProcess = null;
		adaptiveSystem = null;
		apEditPart = null;
	}
}
