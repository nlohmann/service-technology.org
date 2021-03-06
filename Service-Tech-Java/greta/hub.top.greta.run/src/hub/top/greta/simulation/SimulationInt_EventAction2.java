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
import hub.top.greta.cpn.AdaptiveSystemToCPN;
import hub.top.greta.run.actions.ActionHelper;
import hub.top.uma.DNode;
import hub.top.uma.DNodeBP;
import hub.top.uma.DNodeBP_Scenario;
import hub.top.uma.DNodeEmbeddingVisitor;
import hub.top.uma.DNodeSet;
import hub.top.uma.DNodeSys_AdaptiveSystem;
import hub.top.uma.INameProcessor;
import hub.top.uma.InvalidModelException;
import hub.top.uma.Options;

import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.cpntools.accesscpn.engine.highlevel.instance.Binding;
import org.cpntools.accesscpn.engine.highlevel.instance.ValueAssignment;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.Path;
import org.eclipse.emf.common.command.Command;
import org.eclipse.emf.common.command.CommandStack;
import org.eclipse.emf.edit.command.DeleteCommand;
import org.eclipse.emf.edit.domain.EditingDomain;
import org.eclipse.jface.dialogs.MessageDialog;

public class SimulationInt_EventAction2 extends SimulationInteractiveAction {

	public static final String ID = "hub.top.GRETA.run.commands.simulationStep";

  // all events that were visited during nodeMatch()
  //protected HashSet<Event> checkedEvents = new HashSet<Event>();
	//saves the activated normal oclets with all marked conditions in oclet, associated with matched adaptive process conditions
	protected List<Map<Condition, Condition>> activatedNormalTransitions = new LinkedList<Map<Condition, Condition>>();
	//saves the activated anti oclets with all marked conditions in oclet, associated with matched adaptive process conditions
	protected List<Map<Condition, Condition>> activatedAntiTransitions = new LinkedList<Map<Condition, Condition>>();
	// list of all currently enabled transitions in an oclet
	protected List<Event>activatedOcletTransitions = new LinkedList<Event>();
	// HL enabling bindings for events
	protected Map<Event, Binding> enablingBinding = new HashMap<Event, Binding>();

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
	protected void extendAdaptiveProcess(RunConfiguration rc) {
    System.out.println("step!");
    
    // some pre-processing to clear situation from previous step
    
    // delete the nodes that are not part of the process instance
    // and everything that refers to them
    DeleteCommand cmd = new DeleteCommand(simView.processViewEditor.getEditingDomain(), toDelete);
    cmd.canExecute();
    ((CommandStack) ((EditingDomain) simView.processViewEditor.getEditingDomain()).getCommandStack()).execute(cmd);
    toDelete.clear();
    
    // clear the list of new nodes from last step
    newNodes.clear(); 

    DNodeSys_AdaptiveSystem system;
    // transform current system model and process instance into Uma's input format
    // TODO: compute system when starting simulation, update adaptive process only
    try {
      boolean coloredSpec = (rc.a2c != null) ? true : false; 
      system = new DNodeSys_AdaptiveSystem(simView.adaptiveSystem, INameProcessor.HLtoLL, coloredSpec);
    } catch (InvalidModelException e) {
      MessageDialog.openError(getWorkbenchWindow().getShell(), "Animated one step.", "Failed to animate. "+e.getMessage());
      return;
    }
    
    Options o = new Options(system);
    o.configure_buildOnly();
    DNodeBP bp = new DNodeBP_Scenario(system, o);
    
    // print system for debugging purposes
    //IPath dotPath = new Path(simView.adaptiveProcess.eResource().getURI().trimFileExtension().appendFileExtension("dot").toPlatformString(true));
    //ActionHelper.writeFile(dotPath, system.toDot());
    
    //System.out.println(system.fireableEvents);
    //System.out.println(bp.getBranchingProcess().initialConditions);
    
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
    for (DNode e : bp.getBranchingProcess().allEvents) e._isNew = false;
    int eventNum = bp.step();
    
// for debugging
//    if (inFile != null) {
//      ActionHelper.writeDotFile(bp, inFile, "_"+Integer.toString(simulatioStepNum)+"_enabled");
//      simulatioStepNum++;
//    }
    
    System.out.println("fired "+eventNum+" events");

    // visualize all fired events 
    if (eventNum > 0) {
      DNodeSet bps = bp.getBranchingProcess();
      
      
      HashSet<DNode> newEvents = new HashSet<DNode>();
      /*
      nextmax: for (DNode b : bps.getCurrentMaxNodes()) {
        if (b.pre == null || b.pre.length == 0) continue;
        
        DNode e = b.pre[0];
        
        // consider only those events that were just added
        for (DNode c : e.post) {
          if (c.post != null && c.post.length > 0) continue nextmax;
        }
        
        newEvents.add(e);
      }*/
      for (DNode e : bps.allEvents) if (e._isNew) newEvents.add(e);
        
      if (rc.a2c != null) {
        
        Map<org.cpntools.accesscpn.model.Place, Set<Condition>> extraTokens = new HashMap<org.cpntools.accesscpn.model.Place, Set<Condition>>();
        
        for (DNode e : newEvents) {
          System.out.println("new event "+e);
          
          for (int i = 0; i<e.causedBy.length; i++) {
            for (DNode e_system : bp.getSystem().fireableEvents) {
              if (e_system.globalId == e.causedBy[i]) {
                
                System.out.println("causedBy "+e_system);

                DNodeEmbeddingVisitor embedding = system.getEmbeddingVisitor();
                if (!e_system.suffixOf(e, embedding)) {
                  System.err.println("ERROR. Could not embed causing event "+e_system+" at "+e);
                }
                
                List<AdaptiveSystemToCPN.ExtraPlace> extraPlaces = new LinkedList<AdaptiveSystemToCPN.ExtraPlace>();
                for (AdaptiveSystemToCPN.ExtraPlace ep : rc.a2c.extraPlaces) {
                  if (ep.event == (Event)system.getOriginalNode(e_system)) {
                    extraPlaces.add(ep);
                  }
                }
                
                for (AdaptiveSystemToCPN.ExtraPlace ep : extraPlaces) {
                  System.out.println("looking for "+ep.condition+" in "+embedding);
                  
                  for (Map.Entry<DNode, DNode> m : embedding.getEmbedding().entrySet()) {
                    if (m.getKey().isEvent) continue;
                    
                    // system condition that defines an extra place for the CPN Tools transition
                    if (ep.condition == (Condition)system.getOriginalNode(m.getKey())) {
                      // get label of original condition of the run
                      Condition runCondition = (Condition)system.getOriginalNode(m.getValue());
                      System.out.println(runCondition.getName()+" provides an extra value for "+rc.a2c.eventToTransition.get(ep.event));
                      
                      if (!extraTokens.containsKey(ep.place)) extraTokens.put(ep.place, new HashSet<Condition>());
                      extraTokens.get(ep.place).add(runCondition);
                    }
                  }
                }
                
                // collect the tokens of the preconditions of enabled event 'e'
                // to update the marking in the net to the current marking of the run (wrt. 'e')
                for (DNode b : e.pre) {
                  Condition runCondition = (Condition)system.getOriginalNode(b);
                  if (runCondition.isAbstract()) continue;
                  
                  int b_system = 0;
                  for (;b_system<e_system.pre.length;b_system++) {
                    if (e_system.pre[b_system].id == b.id) break;
                  }
                  org.cpntools.accesscpn.model.Place p = rc.a2c.conditionToPlace.get(system.getOriginalNode(e_system.pre[b_system]));
                  
                  if (!extraTokens.containsKey(p)) extraTokens.put(p, new HashSet<Condition>());
                  extraTokens.get(p).add(runCondition);
                }
              }
            } // for all bp.getSystem().fireableEvents
          } // for all causedBy
        }
        
        // add additional tokens to the CPN
        rc.a2c.setMarkingOfExtraPlaces(extraTokens);
      }
      
      // we identify the fired events as the pre-events of the maximal conditions
      // of the current process instance as computed by Uma
      HashSet<DNode> firedEvents = new HashSet<DNode>();
      Map<DNode, Binding> bp_enablingBinding = new HashMap<DNode, Binding>();
      List<Binding> bindings = (rc.a2c != null) ? rc.a2c.enabledTransitions() : null;
      
      for (DNode e : newEvents) {

        // collect all systems events that contributed to firing this event
        Event[] e_cause = new Event[e.causedBy.length];
        for (int i = 0; i<e.causedBy.length; i++) {
          for (DNode e_system : bp.getSystem().fireableEvents) {
            if (e_system.globalId == e.causedBy[i]) {
              e_cause[i] = (Event)system.getOriginalNode(e_system);
            }
          }
        }
        
        boolean isHLenabled;
        
        if (rc.a2c == null) {
          // this is a low-level specification, event is enabled
          isHLenabled = true;
        } else {
          // this is a high-level specification, check enabling wrt data
          isHLenabled = false;
          
          System.out.println("find enabling binding for "+e);
          
          for (Binding bind : bindings) {
            
            Map<String, String> toConsume = rc.a2c.getSourceTokens(bind);
            
            boolean is_binding_for_event = false;
            for (Event e_system : e_cause) {
              if (rc.a2c.eventToTransition.get(e_system) == bind.getTransitionInstance().getNode())
                is_binding_for_event = true;
            }
            

            
            //if (bind.getTransitionInstance().getNode().getName().getText().equals(system.properNames[e.id])) {
            if (is_binding_for_event) {

              System.out.println(bind+" is binding for "+e);
              
              boolean allBindingsMatch = true;
              for (DNode c : e.pre) {
                String place = system.getOriginalNode(c).getName();
                String placeName = AdaptiveSystemToCPN.getPlaceName(place);
                String token = AdaptiveSystemToCPN.getToken(place);
                
                if (!token.equals(toConsume.get(placeName))) {
                  // in some cases, Strings are formatted differently, ask CPN Tools for equality
                  if (!rc.a2c.compareValues(token, toConsume.get(placeName))) {
                    System.out.println(token+" of "+place+" does not match "+toConsume+" in "+bind);
                    allBindingsMatch = false;
                  }
                }
              }
              if (allBindingsMatch) {
                
                boolean alreadyBound = false;
                for (DNode ePrime : bp_enablingBinding.keySet()) {
                  if (e.id == ePrime.id) {
                    if (bp_enablingBinding.get(ePrime) == bind) {
                      System.out.println(" is already bound to "+ePrime);
                      alreadyBound = true;
                    }
                  }
                }
                
                if (!alreadyBound) {
                  bp_enablingBinding.put(e, bind); // remember the binding
                  isHLenabled = true;
                  break;
                }
              }
            }
          }
        }
        
        if (isHLenabled)
          firedEvents.add(e);
      }
      
      System.out.println("Enabling bindings are: ");
      for (Map.Entry<DNode, Binding> bind : bp_enablingBinding.entrySet())  {
        System.out.println(bind.getKey()+" -> "+bind.getValue());
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
        
        // store the binding for this event
        enablingBinding.put(event, bp_enablingBinding.get(e));

        Map<String, String> tokens = null;
        if (rc.a2c != null) {
          // HL-specification: get resulting data values
          tokens = rc.a2c.getResultingTokens(bp_enablingBinding.get(e));
        }
          
        // create corresponding post-conditions and arcs in the editor
        for (DNode b : e.post) {
          
          String conditionName = system.properNames[b.id];
          
          if (rc.a2c != null) { // HL-spec: put resulting conditions in right format
            String token = tokens.get(conditionName);
            conditionName = "("+conditionName+","+token+")";
          }

          //System.out.println("   producing "+b);
          Condition cond = createCondition(conditionName, Temp.COLD, b.isAnti);
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
	protected void fireSelectedEvent(RunConfiguration rc, Event e) {
    fireEvent(e);
    removeHighlightActivatedEvents();
    
    /*
    if (rc.a2c != null) {
      System.out.println("firing: "+enablingBinding.get(e));
      rc.a2c.execute(enablingBinding.get(e));
    }
    */

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
    enablingBinding.clear();
	}
	
  /**
   * Highlight the activated events in the current simulation view.
   * 
   * This will change the appearance (color, size) of activated events
   * in the AdaptiveProcess and in the oclets. 
   */
  private void highlightActivatedEvents() {
    
    List<Command> cmdList = new LinkedList<Command>();
    
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
    
    List<Command> cmdList = new LinkedList<Command>();
    
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
