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

import java.util.HashSet;
import java.util.LinkedList;
import java.util.Map;

import org.eclipse.emf.common.command.CommandStack;
import org.eclipse.emf.common.command.CompoundCommand;
import org.eclipse.emf.common.util.BasicEMap;
import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.common.util.EMap;
import org.eclipse.emf.ecore.util.EcoreUtil;
import org.eclipse.emf.edit.command.RemoveCommand;
import org.eclipse.emf.edit.command.SetCommand;
import org.eclipse.emf.edit.domain.EditingDomain;
import org.eclipse.emf.transaction.RecordingCommand;
import org.eclipse.gmf.runtime.diagram.ui.editpolicies.CanonicalConnectionEditPolicy;
import org.eclipse.gmf.runtime.diagram.ui.resources.editor.parts.DiagramDocumentEditor;

import hub.top.adaptiveSystem.AdaptiveProcess;
import hub.top.adaptiveSystem.AdaptiveSystem;
import hub.top.adaptiveSystem.AdaptiveSystemPackage;
import hub.top.adaptiveSystem.Arc;
import hub.top.adaptiveSystem.Condition;
import hub.top.adaptiveSystem.Event;
import hub.top.adaptiveSystem.Node;

public class RunConfiguration {

	protected AdaptiveSystem 	as;
	protected AdaptiveProcess	initialState;

	public RunConfiguration(AdaptiveSystem as) {
		this.as = as;
		this.initialState = (AdaptiveProcess)EcoreUtil.copy(as.getAdaptiveProcess());
	}
	
	
	private static EList<? extends Node> getPreSet(Node n) {
		if (n instanceof Event) {
			return ((Event)n).getPreConditions();
		} else if (n instanceof Condition) {
			return ((Condition)n).getPreEvents();
		}
		return null;
	}
	
	private static EList<? extends Node> getPostSet(Node n) {
		if (n instanceof Event) {
			return ((Event)n).getPostConditions();
		} else if (n instanceof Condition) {
			return ((Condition)n).getPostEvents();
		}
		return null;
	}
	
	public void resetToInitial(DiagramDocumentEditor editor) {
		

		final EMap<Node, Node> matchingNodes = new BasicEMap<Node, Node>();
		LinkedList<Node> unmatchedNodes = new LinkedList<Node>(initialState.getNodes());
		
		// get all minimal nodes of the current adaptive process
		LinkedList<Node> queue = new LinkedList<Node>();
		for (Node n : as.getAdaptiveProcess().getNodes()) {
			if (getPreSet(n).size() == 0)
				queue.add(n);
		}
		
		// do a breadth-first search on the current process to find all
		// nodes that have also been in the initial configuration
		while (!queue.isEmpty()) {
			Node n = queue.removeFirst();
			
			for (Node m : unmatchedNodes) {
				if (!n.getName().equals(m.getName()))
					continue;	// no match
				if (getPreSet(n).size() != getPreSet(m).size())
					continue;
				
				boolean unmatchedPredecessor = false;
				// compare predecessors
				for (Node preN : getPreSet(n)) {
					if (!getPreSet(m).contains(matchingNodes.get(preN))) {
						unmatchedPredecessor = true;
					}
				}
				
				// this node of the current run is also contained in the
				// initial run
				if (!unmatchedPredecessor) {
					matchingNodes.put(n, m);
					// add all successors of n for matching as well
					queue.addAll(getPostSet(n));
				}
			}
		}
		
		// now remove all new nodes from the model
		final LinkedList<Node> toRemove = new LinkedList<Node>();
		for (Node n : as.getAdaptiveProcess().getNodes()) {
			if (!matchingNodes.keySet().contains(n))
				toRemove.add(n);
		}
		
		RecordingCommand clearCmd = new RecordingCommand(editor.getEditingDomain(), "reset process") {
		
			@Override
			protected void doExecute() {
				// remove all additional nodes
				as.getAdaptiveProcess().getNodes().removeAll(toRemove);
				
				HashSet<Arc> arcsToRemove = new HashSet<Arc>();
				
				for (Node n : toRemove) {
					if (n instanceof Event) {
						((Event) n).getPreConditions().clear();
						((Event) n).getPostConditions().clear();
						
						((Event) n).getIncoming().clear();
						((Event) n).getOutgoing().clear();


						// remember arcs to remove
						arcsToRemove.addAll(((Event) n).getIncoming());
						arcsToRemove.addAll(((Event) n).getOutgoing());
					} else if (n instanceof Condition) {
						((Condition) n).getPreEvents().clear();
						((Condition) n).getPostEvents().clear();
						
						((Condition) n).getIncoming().clear();
						((Condition) n).getOutgoing().clear();
						
						// remember arcs to remove
						arcsToRemove.addAll(((Condition) n).getIncoming());
						arcsToRemove.addAll(((Condition) n).getOutgoing());

					}
				}
				
				// remove all arcs adjacent to additional nodes
				as.getAdaptiveProcess().getArcs().removeAll(arcsToRemove);
				
				// restore marking
				for (Map.Entry<Node,Node> e : matchingNodes) {
					if (e.getValue() instanceof Condition) {
						Condition cInit = (Condition)e.getValue();
						Condition cCurrent = (Condition)e.getKey();
						// see if marking matches in the kept conditions
						if (cCurrent.getToken() != cInit.getToken()) {
							cCurrent.setToken(cInit.getToken());
						}
					}
				}
			}
		};
		
		clearCmd.canExecute();
		editor.getEditingDomain().getCommandStack().execute(clearCmd);

		// refresh the diagram to get rid of nodes and edges
		((CanonicalConnectionEditPolicy) editor
				.getDiagramEditPart().getEditPolicy("Canonical")).refresh();
	}
	
	/**
	 * @param as
	 * @return true
	 * 		iff this configuration is a valid configuration for the
	 * 		given adaptive system as
	 */
	public boolean isValidConfigOf (AdaptiveSystem as) {
		if (this.as != null && this.as.equals(as))
			return true;
		else
			return false;
	}
}
