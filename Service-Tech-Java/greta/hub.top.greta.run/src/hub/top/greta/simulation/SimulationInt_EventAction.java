/*****************************************************************************\
 * Copyright (c) 2008, 2009 Manja Wolf, Dirk Fahland. EPL1.0/AGPL3.0
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
 * The Original Code is this file as it was released on December 04, 2008.
 * The Initial Developer of the Original Code are
 *    Manja Wolf
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
import hub.top.adaptiveSystem.Condition;
import hub.top.adaptiveSystem.Event;
import hub.top.adaptiveSystem.Node;
import hub.top.adaptiveSystem.Oclet;
import hub.top.adaptiveSystem.Orientation;

import org.eclipse.emf.common.command.Command;
import org.eclipse.emf.common.util.BasicEList;
import org.eclipse.emf.common.util.BasicEMap;
import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.common.util.EMap;

@Deprecated
public class SimulationInt_EventAction extends SimulationInteractiveAction {

	public static final String ID = "hub.top.GRETA.run.process";

  // all events that were visited during nodeMatch()
  //protected HashSet<Event> checkedEvents = new HashSet<Event>();
	//saves the activated normal oclets with all marked conditions in oclet, associated with matched adaptive process conditions
	protected EList<EMap<Condition, Condition>> activatedNormalTransitions = new BasicEList<EMap<Condition, Condition>>();
	//saves the activated anti oclets with all marked conditions in oclet, associated with matched adaptive process conditions
	protected EList<EMap<Condition, Condition>> activatedAntiTransitions = new BasicEList<EMap<Condition, Condition>>();
	// list of all currently enabled transitions in an oclet
	protected EList<Event>activatedOcletTransitions = new BasicEList<Event>();
	
	/*
	 * (non-Javadoc)
	 * @see hub.top.greta.run.actions.SimulationInteractive#getID()
	 */
  @Override
  public String getID() {
    return SimulationInt_EventAction.ID;
  }
	
  /**
   * @see hub.top.greta.simulation.SimulationInteractiveAction#extendAdaptiveProcess()
   */
  @Override
	protected void extendAdaptiveProcess(RunConfiguration rc) {
    ////System.out.println("step!");
    
    //cut is the list of markedConditions of adaptive process
    EList<Condition> cut = simView.adaptiveProcess.getMarkedConditions();
    
    //apply all activated oclets
    //fill the two global lists of all activated normal oclets and anti oclets
    getActivatedTransitions(simView.adaptiveSystem, cut);
    
    //insert all normal activated oclets in adaptiveProcess
    insertActivatedNormalTransitions();
    //arrange all elements in adaptiveProcess
    SimulationHelper.arrangeAllAdaptiveProcess(simView.processViewEditor.getEditingDomain(), simView.apEditPart, /*newNodes*/ null );
    newNodes.clear(); // clear the list of new nodes for next step
    
    //apply all activated anti oclets on adaptive process
    applyActivatedAntiTransitions();
    
    //highlight activated events
    getActivatedEvents();
    highlightActivatedEvents();

	}
	
  /**
   * @see hub.top.greta.simulation.SimulationInteractiveAction#fireSelectedEvent(hub.top.adaptiveSystem.Event)
   */
  @Override
	protected void fireSelectedEvent(RunConfiguration rc, Event e) {
    fireEvent(e);
    removeHighlightActivatedEvents();
    activatedEvents.clear();
    activatedOcletTransitions.clear();
	}
	
	/**
   * Fill the global list of all activated normal transition and anti transition
   * @param ap
   * @param cut
   */
	protected void getActivatedTransitions(AdaptiveSystem ap, EList<Condition> cut) {
		////System.out.println("START getActivatedOclet()");
		
		for (Oclet oclet : ap.getOclets()) {
			////System.out.println("checking "+ oclet.getName());
			
			if (oclet.isWellFormed()) {
				////System.out.println("  is well formed oclet.");
				
				for (Node transition : oclet.getDoNet().getNodes()) {
					if (!(transition instanceof Event))	// check enabling of transitions only
						continue;

					////System.out.println("  checking transition "+ transition.getName());
					if(preSetSatfisfied(oclet, (Event)transition, cut)) {
						////System.out.println("    is activated.");
					  // save all activated transitions in a list for highlighting
					  activatedOcletTransitions.add((Event)transition);
					} else {
						////System.out.println("    not activated.");
					}
				}
			}
			////else System.out.println("  isn't well formed oclet.");
			
		}
		////System.out.println("END getActivatedOclet()");
	}
	
 /**
   * Check whether the precondition of an oclet transition matches with the cut
   * and all transitive predecessors of the cut. If it matches, return true and
   * extend {@link SimulationInt_EventAction#activatedNormalTransitions} 
   * or {@link SimulationInt_EventAction#activatedAntiTransitions}
   * with the Map of pre-conditions of the transitions (key) associated
   * with the cut condtions (adaptiveProcess - value).
   * 
   * @param oclet       containing a transition to check activation for
   * @param transition  transition of oclet to be checked for enabling in cut
   * @param cut         set of conditions being the current cut 
   * @return
   */
  private boolean preSetSatfisfied (Oclet oclet, Event transition, EList<Condition> cut)
  {

    ////System.out.println("  start preSetSatisfied()");
    EMap<Condition, Condition> matchingConditions = new BasicEMap<Condition, Condition>();
    
    // check whether preconditions of the given transition are in the cut
    boolean isValidMatch = matchPreConditionsWithCut(transition.getPreConditions(), cut, matchingConditions);

    if(isValidMatch) {
      if(oclet.getOrientation().equals(Orientation.NORMAL)) activatedNormalTransitions.add(matchingConditions);
      if(oclet.getOrientation().equals(Orientation.ANTI)) activatedAntiTransitions.add(matchingConditions);
    } 
    
    //checkedEvents.clear(); //for every check of an oclet use a clear checkedEventMap
    return isValidMatch;
  }

  /**
   * Insert all activated normal transitions in {@link hub.top.adaptiveSystem.AdaptiveProcess}
   */  
  private void insertActivatedNormalTransitions() {
    ////System.out.println("START insertActivatedNormalTransions()");
    //get every activated normal oclet
    for(EMap<Condition, Condition> currentInsertMap : activatedNormalTransitions) {
      if(currentInsertMap != null && !currentInsertMap.isEmpty()) {
        ////System.out.println("insert normal transition "+currentInsertMap.get(0).getKey().getPostSet().get(0).getName()+" from " + ((Oclet) ((Condition) currentInsertMap.get(0).getKey()).eContainer().eContainer()).getName());
        // insert at most one transition (and its post-set)
        insertNodes(currentInsertMap, 1); 
      }
      ////else System.out.println("insert normal transition " + currentInsertMap);
      
    }//END for - iterate all oclets
    
    activatedNormalTransitions.clear();
    
    ////System.out.println("END insertActivatedNormalTransitions()");
  }
  
  /**
   * Set disabledByAntiOclet on every postEvent of every condition (adaptiveProcess)
   * in the list of matching preconditions of anti-oclets.
   * 
   * @author Manja Wolf
   */
  private void applyActivatedAntiTransitions() {
    ////System.out.println("START applyActivatedAntiTransitions()");
    //get every activated anti oclet
    for(EMap<Condition, Condition> disableMap : activatedAntiTransitions) {
      disableNodes_anti(disableMap);
    }//END for - iterate all oclets
    
    activatedAntiTransitions.clear();
     
    ////System.out.println("END applyActivatedAntiTransitions()");
  }
  
  /**
   * Highlight the activated events in the current simulation view.
   * 
   * This will change the appearance (color, size) of activated events
   * in the AdaptiveProcess and in the oclets. 
   */
  private void highlightActivatedEvents() {
    
    EList<Command> cmdList = new BasicEList<Command>();
    
    // highlight all activated events in the adaptive process
    for (Event event : activatedEvents) {
      if(!event.isEnabled()) {
        cmdList.add(highlightEvent_enabled(event));
      }
    }
    
    // highlight all corresponding activated oclet transitions
    for (Event event : activatedOcletTransitions) {
      cmdList.add(highlightEvent_enabled(event));
    }
    
    executeCommands(cmdList);
  }
  
  /**
   * Remove highlight the activated events in the current simulation view.
   * 
   * This will change the appearance (color, size) of activated events
   * in the AdaptiveProcess and in the oclets. 
   */
  private void removeHighlightActivatedEvents() {
    
    EList<Command> cmdList = new BasicEList<Command>();
    
    //after firing event reset values
    for(Event event : activatedEvents) {
      if(event.isEnabled()) {
        //set the event enabled
        cmdList.add(highlightEvent_disabled(event));
      }
    }
    for(Event event : activatedOcletTransitions) {
      if(event.isEnabled()) {
        //set the event enabled
        cmdList.add(highlightEvent_disabled(event));
      }
    }
    
    executeCommands(cmdList);
  }

}
