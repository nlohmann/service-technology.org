/*****************************************************************************\
 * Copyright (c) 2008, 2009. All rights reserved. Dirk Fahland. EPL1.0/AGPL3.0
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

package hub.top.greta.run.actions;

import hub.top.adaptiveSystem.AdaptiveSystem;
import hub.top.adaptiveSystem.Oclet;
import hub.top.adaptiveSystem.diagram.part.AdaptiveSystemDiagramEditor;
import hub.top.greta.oclets.canonical.CNodeSet;

import org.eclipse.emf.common.util.EList;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.IWorkbenchWindowActionDelegate;

public class OcletDependencies implements IWorkbenchWindowActionDelegate {

	public static final String ID = "hub.top.GRETA.run.ocletDependencies";
	
	private IWorkbenchWindow workbenchWindow;

	private AdaptiveSystemDiagramEditor adaptiveSystemDiagramEditor;
	private AdaptiveSystem adaptiveSystem;

	/**
	 * @see org.eclipse.ui.IWorkbenchWindowActionDelegate#dispose()
	 */
	public void dispose() {
		// TODO Auto-generated method stub
	}

	/**
	 * @see org.eclipse.ui.IWorkbenchWindowActionDelegate#init(org.eclipse.ui.IWorkbenchWindow)
	 */
	public void init(IWorkbenchWindow window) {
		workbenchWindow = window;
	}

	
	
	/**
	 * @see org.eclipse.ui.IActionDelegate#run(org.eclipse.jface.action.IAction)
	 */
	public void run(IAction action) {
		if(workbenchWindow.getActivePage().getActiveEditor() instanceof AdaptiveSystemDiagramEditor 
				&& action.getId().equals(OcletDependencies.ID)) {
			
			adaptiveSystemDiagramEditor = (AdaptiveSystemDiagramEditor) workbenchWindow.getActivePage().getActiveEditor();
			adaptiveSystem = (AdaptiveSystem) adaptiveSystemDiagramEditor.getDiagram().getElement();
			EList<Oclet> ocletList = adaptiveSystem.getOclets();
			
			String result = "";
			
			for (int i=0; i < ocletList.size(); i++) {
				for (int j=0; j < ocletList.size(); j++) {
					System.err.println(ocletList.get(i).getName() +"->"+ ocletList.get(j).getName());
					CNodeSet si = CNodeSet.fromPreNet(ocletList.get(i));
					CNodeSet sj = CNodeSet.fromPreNet(ocletList.get(j));
					if (i != j && sj.equals(si)) {
						result += "[enabled] "+ocletList.get(i).getName()+" iff "+ocletList.get(j).getName()+"\n";
					}
					if (i != j && sj.maxContainedIn(si)) {
						result += "[enabled] if "+ocletList.get(i).getName()+" then "+ocletList.get(j).getName()+"\n";
					}
					
					si = CNodeSet.fromOclet(ocletList.get(i));
					if (sj.enabledBy(si)) {
						result += "[run] "+ocletList.get(i).getName()+" enables "+ocletList.get(j).getName()+"\n";
					}
				}
			}
			
			MessageDialog.openInformation(
					workbenchWindow.getShell(),
					"AdaptiveSystem - check oclet dependencies",
					result);
		}
	}

	/**
	 * @see org.eclipse.ui.IActionDelegate#selectionChanged(org.eclipse.jface.action.IAction, org.eclipse.jface.viewers.ISelection)
	 */
	public void selectionChanged(IAction action, ISelection selection) {
		// TODO Auto-generated method stub

	}

}
