/*****************************************************************************\
 * Copyright (c) 2008, 2009, 2010 Manja Wolf, Dirk Fahland. EPL1.0/AGPL3.0
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

import hub.top.adaptiveSystem.Condition;
import hub.top.adaptiveSystem.Event;
import hub.top.adaptiveSystem.Node;
import hub.top.adaptiveSystem.Temp;
import hub.top.greta.run.actions.ActionHelper;
import hub.top.uma.DNode;
import hub.top.uma.DNodeBP;
import hub.top.uma.DNodeBP_Scenario;
import hub.top.uma.DNodeSet;
import hub.top.uma.DNodeSys_AdaptiveSystem;

import java.util.HashSet;
import java.util.LinkedList;

import org.eclipse.core.resources.IFile;
import org.eclipse.emf.common.command.Command;
import org.eclipse.emf.common.command.CommandStack;
import org.eclipse.emf.common.util.BasicEList;
import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.common.util.EMap;
import org.eclipse.emf.edit.command.DeleteCommand;
import org.eclipse.emf.edit.domain.EditingDomain;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IFileEditorInput;

public class SimulationInt_EventAction2 extends SimulationInteractiveAction {

	public static final String ID = "hub.top.GRETA.run.processD";

  // all events that were visited during nodeMatch()
  //protected HashSet<Event> checkedEvents = new HashSet<Event>();
	//saves the activated normal oclets with all marked conditions in oclet, associated with matched adaptive process conditions
	protected EList<EMap<Condition, Condition>> activatedNormalTransitions = new BasicEList<EMap<Condition, Condition>>();
	//saves the activated anti oclets with all marked conditions in oclet, associated with matched adaptive process conditions
	protected EList<EMap<Condition, Condition>> activatedAntiTransitions = new BasicEList<EMap<Condition, Condition>>();
	// list of all currently enabled transitions in an oclet
	protected EList<Event>activatedOcletTransitions = new BasicEList<Event>();

	// list of all nodes to be deleted before the next extension
	// this list is filled in the previous step with nodes that are not
	// part of the executed process instance (AdaptiveProcess), but remain
	// in the visual representation for an improved user experience
	private LinkedList<Node> toDelete = new LinkedList<Node>();
	
	/*
	 * (non-Javadoc)
	 * @see hub.top.greta.run.actions.SimulationInteractive#getID()
	 */
  @Override
  public String getID() {
    return SimulationInt_EventAction2.ID;
  }
	
  //private static int simulatioStepNum = 0;  // for debugging purposes
  
  /**
   * @see hub.top.greta.simulation.SimulationInteractiveAction#extendAdaptiveProcess()
   */
  @Override
	protected void extendAdaptiveProcess() {
    //System.out.println("step!");
    
    // some pre-processing to clear situation from previous step
    
    // delete the nodes that are not part of the process instance
    // and everything that refers to them
    DeleteCommand cmd = new DeleteCommand(simView.processViewEditor.getEditingDomain(), toDelete);
    cmd.canExecute();
    ((CommandStack) ((EditingDomain) simView.processViewEditor.getEditingDomain()).getCommandStack()).execute(cmd);
    toDelete.clear();
    
    // clear the list of new nodes from last step
    newNodes.clear(); 

    // transform current system model and process instance into Uma's input format
    // TODO: compute system when starting simulation, update adaptive process only
    DNodeSys_AdaptiveSystem system = new DNodeSys_AdaptiveSystem(simView.adaptiveSystem);
    DNodeBP bp = new DNodeBP_Scenario(system);
    bp.configure_Scenarios();
    bp.configure_buildOnly();
    
// for debugging
//    IEditorInput in = simView.processViewEditor.getEditorInput();
//    IFile inFile = null;
//    if (in instanceof IFileEditorInput) {
//      IFileEditorInput fileIn = (IFileEditorInput) in;
//      inFile = fileIn.getFile();
//    }
//
//    if (inFile != null) {
//      ActionHelper.writeDotFile(bp, inFile, "_"+Integer.toString(simulatioStepNum)+"_history");
//      simulatioStepNum++;
//
//    }
    
    // let Uma compute one step = fire all events that are enabled at the end
    // of the current process instance
    int eventNum = bp.step();
    
// for debugging
//    if (inFile != null) {
//      ActionHelper.writeDotFile(bp, inFile, "_"+Integer.toString(simulatioStepNum)+"_enabled");
//      simulatioStepNum++;
//    }
    
    //System.out.println("fired "+eventNum+" events");

    // visualize all fired events 
    if (eventNum > 0) {
      DNodeSet bps = bp.getBranchingProcess();
      
      // we identify the fired events as the pre-events of the maximal conditions
      // of the current process instance as computed by Uma
      HashSet<DNode> firedEvents = new HashSet<DNode>();
      for (DNode b : bps.getCurrentMaxNodes()) {
        if (b.pre == null || b.pre.length == 0) continue;
        firedEvents.add(b.pre[0]);
      }
      
      // determine for each fired event where it was fired and which
      // oclet events caused its occurrence
      for (DNode e : firedEvents) {
        //System.out.println("fired event "+e);
        if (e.causedBy == null) {
          // event 'e' was a pre-event of a maximal condition (in bps.maxNodes)
          // that had been created in the previous step, this case arises for
          // instance when not all redundant events from the previous step are
          // cleared from the instance, see 'toDelete'
          continue;
        }
        // determine the originating oclet events
        for (int eOrigId : e.causedBy) {
          Node nOrig = system.getOriginalNode(system.getTransitionForID(eOrigId));
          //System.out.println("   because of "+nOrig);
          if (nOrig instanceof Event)
            activatedOcletTransitions.add((Event)nOrig);
        }

        // display fired event in the editor: create a new event
        Event event = createEvent(system.properNames[e.id], Temp.COLD, e.isAnti);
        
        // create an arc to the event's preconditions in the editor
        for (DNode b : e.pre) {
          Condition cond = (Condition)system.getOriginalNode(b);
          //System.out.println("   at "+cond);
          createArc(cond, event);
        }
        
        // create corresponding post-conditions and arcs in the editor
        for (DNode b : e.post) {
          //System.out.println("   producing "+b);
          Condition cond = createCondition(system.properNames[b.id], Temp.COLD, b.isAnti);
          createArc(event, cond);
        }
      } // finished displaying all fired events in the editor
      
      //arrange all elements in adaptiveProcess
      SimulationHelper.arrangeAllAdaptiveProcess(simView.processViewEditor.getEditingDomain(), simView.apEditPart, /*newNodes*/ null );
    }

    //highlight enabled events
    getActivatedEvents();
    highlightActivatedEvents();
	}
	
  /**
   * @see hub.top.greta.simulation.SimulationInteractiveAction#fireSelectedEvent(hub.top.adaptiveSystem.Event)
   */
  @Override
	protected void fireSelectedEvent(Event e) {
    fireEvent(e);
    removeHighlightActivatedEvents();

    // remove all events that had been added temporarily to the process
    // instance to show enabling information
    for (Node n : newNodes) {
      // keep the event that was fired and its postconditions
      if (n == e) continue;
      if (e.getPostSet().contains(n)) continue;

      // TODO: make configurable, which nodes are preserved
      // keep all events that show alternatives that could not be taken
      //if (n.isDisabledByAntiOclet()) continue;
      //if (n.isDisabledByConflict()) continue;
      
      // delete everything else
      toDelete.add(n);
    }
    // these nodes will be removed prior to the next extension
    // we keep them in the current instance for an improved user experience

    // list of all currently enabled transitions in an oclet
    activatedEvents.clear();
    activatedOcletTransitions.clear();
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
