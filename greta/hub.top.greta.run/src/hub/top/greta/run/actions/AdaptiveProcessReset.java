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

package hub.top.greta.run.actions;

import hub.top.greta.run.AdaptiveProcessSimulationView;
import hub.top.greta.run.RunConfiguration;

import org.eclipse.jface.action.IAction;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.IWorkbenchWindowActionDelegate;

public class AdaptiveProcessReset implements IWorkbenchWindowActionDelegate {

	public static final String ID = "hub.top.GRETA.run.reset";
	protected IWorkbenchWindow workbenchWindow;
	
	private AdaptiveProcessSimulationView simView = new AdaptiveProcessSimulationView();
	
	public void dispose() {
		// do nothing
	}

	public void init(IWorkbenchWindow window) {
		workbenchWindow = window;
	}

	public void run(IAction action) {
		if (!action.getId().equals(AdaptiveProcessReset.ID))
			return;
		if (simView.processViewEditor == null)
			return;
		
		resetSimulation(workbenchWindow.getShell(), simView);
	}

	public void selectionChanged(IAction action, ISelection selection) {
		boolean validContext = true;

		if (!action.getId().equals(AdaptiveProcessReset.ID))
			validContext = false;
		
		// set and check whether the current editor can handle this action
		simView.setProcessViewEditor_andFields(workbenchWindow.getActivePage().getActiveEditor());
		if (simView.processViewEditor == null)
			validContext = false;
		
		if (validContext)
		{
			if (AdaptiveProcessSimulation.isValidConfigOf(simView.adaptiveSystem)) {
				if (!action.isEnabled()) {
					// we have a running simulation,
					// looking at the right simulation view
					action.setEnabled(true);
					return;
				}
			} else {
				// no simulation running
				if (action.isEnabled()) {
					action.setEnabled(false);
				}
			}
		} else {
			// invalid context
			if (action.isEnabled()) {
				action.setEnabled(false);
			}
		}
	}
	
	/**
	 * Reset the current simulation in the current view. Will display an error
	 * message if the current view does not show the current simulation (in this
	 * case, reset is not possible).
	 * 
	 * @param shell
	 * @param simView
	 */
	protected static void resetSimulation(Shell shell, AdaptiveProcessSimulationView simView) {
		RunConfiguration config = AdaptiveProcessSimulation.getActiveRunConfiguration(simView.adaptiveSystem);
		if (config != null) {
			config.resetToInitial(simView.processViewEditor);
			AdaptiveProcessRun.arrangeAllAdaptiveProcess(simView.processViewEditor.getEditingDomain(), simView.apEditPart, null);
		} else {
			MessageDialog.openError(shell,
					"Could not reset simulation.",
					"Could not reset simulation. This process is not in the " +
					"current simulation.");
		}
	}
}
