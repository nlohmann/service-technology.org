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

package hub.top.greta.simulation;

import hub.top.adaptiveSystem.AdaptiveSystem;
import hub.top.adaptiveSystem.diagram.part.AdaptiveSystemDiagramEditor;
import hub.top.greta.run.Activator;

import org.eclipse.jface.action.IAction;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.MessageBox;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.IWorkbenchWindowActionDelegate;

/**
 * Action that controls the simulation mode of the {@link AdaptiveSystemDiagramEditor}.
 * The action toggles between standard editor mode and a simulation mode. Entering
 * the simulation mode takes a snapshot of the current model (that is restored when
 * the simulation is left) and enables all other simulation controls.
 * 
 * @author Dirk Fahland
 *
 */
public class StartAction implements
		IWorkbenchWindowActionDelegate {

	public static final String ID = "hub.top.GRETA.run.simulation";
	
	private IWorkbenchWindow window;
	private AdaptiveProcessSimulationView simView = new AdaptiveProcessSimulationView();

	private boolean startSim;

	public void dispose() {
		// do nothing
	}

	public void init(IWorkbenchWindow window) {
		this.window = window;
		this.startSim = true;
	}

	public void run(IAction action) {
		if (!action.getId().equals(StartAction.ID))
			return;
		
		if (startSim) {
			
			startSimulation();
			switchToStop(action);
			return;

			/*
			InitProcessSimulationView.initializeProcessViewDiagram(
					as.eResource().getURI(), ed.getEditingDomain());
			
			IEditorPart newEditor = window.getActivePage().getActiveEditor();
			window.getActivePage().activate(newEditor);
			*/

		} else if (isValidConfigOf(simView.adaptiveSystem)) {
			
			stopSimulation(/*doReset*/ true);			
			switchToPlay(action);
			
		} else if (isSimuluationRunning()) {
			MessageBox m = new MessageBox(window.getShell(), (SWT.YES|SWT.NO));
			m.setText("Abort current simulation?");
			m.setMessage("There is currently an active simulation. Stop " +
					"this simulation?");
			if (m.open() == SWT.NO) {
				return;
			} else {
				
				stopSimulation(/*doReset*/ false);
				switchToPlay(action);
				return;
			}
		}
	}

	public void selectionChanged(IAction action, ISelection selection) {
		if (!action.getId().equals(StartAction.ID))
			return;
		
		simView.setProcessViewEditor_andFields(window.getActivePage().getActiveEditor());
		if (simView.processViewEditor == null) {
			if (startSim && action.isEnabled())
				// cannot start simulation on an invalid view,
				// but stopping is possible 
				action.setEnabled(false);
		} else {
			
			if (!action.isEnabled())
				action.setEnabled(true);
		}
	}

	/**
	 * Switch action to "start simulation" by changing icons, texts, ...
	 * @param action
	 */
	private void switchToPlay (IAction action) {
		ImageDescriptor img = Activator.imageDescriptorFromPlugin(
				Activator.PLUGIN_ID, "icons/full/etool16/run_exc.gif");
		action.setImageDescriptor(img);
		
		img = Activator.imageDescriptorFromPlugin(
				Activator.PLUGIN_ID, "icons/full/dtool16/run_exc.gif");
		action.setDisabledImageDescriptor(img);

		action.setText("start simulation");
		action.setToolTipText("start simulation of adaptive process");
		
		startSim = true;
		
		if (simView.processViewEditor == null) {
			// disable action if not in the right context
			action.setEnabled(false);
		}
	}

	/**
	 * Switch action to "stop simulation" by changing icons, texts, ...
	 * @param action
	 */
	private void switchToStop (IAction action) {
		ImageDescriptor img = Activator.imageDescriptorFromPlugin(
				Activator.PLUGIN_ID, "icons/full/elcl16/terminate_co.gif");
		action.setImageDescriptor(img);
		
		img = Activator.imageDescriptorFromPlugin(
				Activator.PLUGIN_ID, "icons/full/dlcl16/terminate_co.gif");
		action.setDisabledImageDescriptor(img);
		
		action.setText("stop simulation");
		action.setToolTipText("stop simulation of adaptive process");
		
		startSim = false;
	}
	
	/**
	 * Initiate simulation and notify other actions about this change
	 */
	private void startSimulation () {
		createNewRunConfiguration(simView.adaptiveSystem);
		
		// notify all actions
		simView.processViewEditor.getDiagramGraphicalViewer().deselectAll();
		simView.processViewEditor.getDiagramGraphicalViewer().select(simView.apEditPart);
	}

	/**
	 * Stop simulation and notify other actions about this change
	 */
	private void stopSimulation (boolean doReset) {
		
		if (doReset)
			ResetAction.resetSimulation(window.getShell(), simView);
		
		cancelCurrentRun();
		
		if (simView.processViewEditor == null)
			return;

		// notify all actions
		simView.processViewEditor.getDiagramGraphicalViewer().deselectAll();
		simView.processViewEditor.getDiagramGraphicalViewer().select(simView.apEditPart);

	}
	
	// -----------------------------------------------------------------
	//  static methods for maintaining the run-time state of the
	//  simulation in the current instance of the program
	// -----------------------------------------------------------------
	
	private static RunConfiguration simulationConfiguration = null;

	/**
	 * @return true iff there is an active simulation
	 */
	public static boolean isSimuluationRunning() {
		return (simulationConfiguration != null);
	}
	
	/**
	 * Cancel current simulation. Destroys current run configuration.
	 * 
	 * @return if cancellation succeeded
	 */
	public static boolean cancelCurrentRun() {
		simulationConfiguration = null;
		return true;
	}
	
	/**
	 * @param as
	 * @return
	 *		true iff the current simulation run configuration is valid
	 *		for the given adaptive system {@literal as}
	 */
	public static boolean isValidConfigOf(AdaptiveSystem as) {
		return (	isSimuluationRunning()
				&&	simulationConfiguration.isValidConfigOf(as));
	}

	/**
	 * Return the current active run configuration in the simulation environment
	 * that is valid for the given adaptive system <code>as</code>
	 * @param as
	 * @return null of configuration is invalid
	 */
	public static RunConfiguration getActiveRunConfiguration(AdaptiveSystem as) {
		if (isValidConfigOf(as))
			return simulationConfiguration;
		else
			return null;
	}
	
	/**
	 * Initialize a new run configuration for simulation  
	 * @param as
	 * @return the new configuration
	 */
	public static RunConfiguration createNewRunConfiguration(AdaptiveSystem as) {
		simulationConfiguration = new RunConfiguration(as);
		return simulationConfiguration;
	}

}
