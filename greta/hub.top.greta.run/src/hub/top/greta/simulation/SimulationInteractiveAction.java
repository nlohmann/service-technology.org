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

import java.util.HashSet;
import java.util.ListIterator;
import java.util.Map;

import hub.top.adaptiveSystem.AdaptiveSystemFactory;
import hub.top.adaptiveSystem.AdaptiveSystemPackage;
import hub.top.adaptiveSystem.ArcToCondition;
import hub.top.adaptiveSystem.ArcToEvent;
import hub.top.adaptiveSystem.Condition;
import hub.top.adaptiveSystem.Event;
import hub.top.adaptiveSystem.Node;
import hub.top.adaptiveSystem.Oclet;
import hub.top.adaptiveSystem.diagram.edit.parts.EventAPEditPart;
import hub.top.greta.run.AdaptiveProcessSimulationView;
import hub.top.greta.run.actions.AdaptiveProcessSimulation;

import org.eclipse.emf.common.command.Command;
import org.eclipse.emf.common.command.CommandStack;
import org.eclipse.emf.common.command.CompoundCommand;
import org.eclipse.emf.common.util.BasicEList;
import org.eclipse.emf.common.util.BasicEMap;
import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.common.util.EMap;
import org.eclipse.emf.edit.command.AddCommand;
import org.eclipse.emf.edit.command.SetCommand;
import org.eclipse.emf.edit.domain.EditingDomain;
import org.eclipse.gmf.runtime.diagram.ui.editpolicies.CanonicalConnectionEditPolicy;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.IWorkbenchWindowActionDelegate;

public abstract class SimulationInteractiveAction implements IWorkbenchWindowActionDelegate {
  
  public static boolean option_checkCapacity = true;  // enable to set capacity of places to "1"
  public static boolean option_matchExactPrecondition = false;
  
  protected IWorkbenchWindow workbenchWindow;

  // data structure collecting all information about the current simulation view in
  // the shown editor
  protected AdaptiveProcessSimulationView simView = new AdaptiveProcessSimulationView();
  
  // list of nodes that were newly created in the current step
  protected EList<Node> newNodes = new BasicEList<Node>();
  // all activated events of the AdaptiveProcess in the current step
  protected EList<Event> activatedEvents = new BasicEList<Event>();

  // internal flag of the action that signals whether the action now
  // waits for the user to select an enabled event for firing
  private boolean inWaitForFireEvent = false;
  
  public void init(IWorkbenchWindow window) {
    workbenchWindow = window;
  }
  
  public void dispose() {
    // do nothing

  }
  
  public void run(IAction action) {
    
    if (simView.processViewEditor == null)
      return;
    // adaptiveSystem, adaptiveProcess and processViewEditor are set

    if (!simView.apEditPart.isActive()) {
      return;
    }
    
    extendAdaptiveProcess();
    inWaitForFireEvent = true;
    action.setEnabled(false);
  }

  public void selectionChanged(IAction action, ISelection selection) {

    boolean validContext = true;

    if (!action.getId().equals(getID()))
      validContext = false;
    
    // set and check whether the current editor can handle this action
    simView.setProcessViewEditor_andFields(workbenchWindow.getActivePage().getActiveEditor());
    if (simView.processViewEditor == null)
      validContext = false;
    
    if (validContext)
    {
      if (!AdaptiveProcessSimulation.isValidConfigOf(simView.adaptiveSystem)) {
        // no simulation running or this view is not the current simulation
        if (action.isEnabled()) {
          action.setEnabled(false);
        }
        
        if (!AdaptiveProcessSimulation.isSimuluationRunning()) {
          inWaitForFireEvent = false;  // no simulation running: reset internal configuration
        }

        return;
      }
      
      // changed the editor?
      if (simView.processViewEditor != simView.oldProcessViewEditor) {
        // reset action's internal state
        inWaitForFireEvent = false;
        simView.processViewEditor.getDiagramGraphicalViewer().select(simView.apEditPart);
        return;
      }
      
      // adaptiveSystem, adaptiveProcess, oldAdaptiveSystem, and  
      // processViewEditor are set
      if (simView.adaptiveSystem.isSetWellformednessToOclets()) {

        // the editor changed silently, we have a new model:
        if(simView.oldAdaptiveSystem != null && !simView.oldAdaptiveSystem.equals(simView.adaptiveSystem))
          // reset action
          inWaitForFireEvent = false;
        
        // an event was selected: fire it
        if(inWaitForFireEvent && ((StructuredSelection) selection).getFirstElement() instanceof EventAPEditPart)
        {
          EventAPEditPart eventEP = (EventAPEditPart) ((StructuredSelection) selection).getFirstElement();
          Event event = (Event) ((org.eclipse.gmf.runtime.notation.Node) eventEP.getModel()).getElement();
          if(!activatedEvents.isEmpty() && activatedEvents.contains(event)) {

            fireSelectedEvent(event);
            eventEP.getViewer().deselect(eventEP);  // auto-deselect the fired transition

            inWaitForFireEvent = false;
          }
          return;
        }
        
        if(!inWaitForFireEvent && !action.isEnabled()) {
          action.setEnabled(true);
        }
        
      } else {  // well-formedness of oclets not checked yet 
        if(action.isEnabled()) {
          inWaitForFireEvent = false;
          action.setEnabled(false);
          return;
        }
      }
    } else {  // invalid context

      if (action.isEnabled()) {
        inWaitForFireEvent = false;
        action.setEnabled(false);
      }
    }
  }
  
  /**
   * @return the action ID of this action
   */
  public abstract String getID();

  /**
   * Extend the {@link hub.top.adaptiveSystem.AdaptiveProcess} by structures from
   * {@link Oclet}s.
   */
  protected abstract void extendAdaptiveProcess();
  
  /**
   * Fire the selected event in the {@link hub.top.adaptiveSystem.AdaptiveProcess}
   * 
   * @param e
   */
  protected abstract void fireSelectedEvent(Event e);
  
  /**
   * Check whether the given list of preConditions have matching {@link Condition}s in
   * the cut and build a mapping from preConditions to matching {@link Condition}s in
   * the cut.
   * 
   * TODO: method works only for safe Petri nets. If the net isn't safe and
   * there are more nodes with same label, this method should be modified.
   * ( -> prüfe alle Cut-Condition (break entfernen) und speichere in der
   *   conditionLinkMap am key (der OcletCondition) eine <b>Liste</b> der
   *   im Main Process gefundenen Condition EMap<Condition, Elist<Condition>> )
   * 
   * @param preConditions
   * @param cut
   * @param matchingConditions
   * @return true iff each preCondition matches with some {@link Condition} in the cut
   */
  protected boolean matchPreConditionsWithCut(EList<Condition> preConditions, EList<Condition> cut, EMap<Condition, Condition> matchingConditions) {
    boolean isValidMatch = false;
    for (Condition ocletCondition : preConditions)
    {
      ////System.out.println("  preSetSatisfied() - check ocletCondition " + ocletCondition.getName());
      for (Condition cutCondition : cut) {
        //call matchingalgorithm
        //System.out.println("  call nodeMatches() with ocletCondition " + ocletCondition.getName() + " and cutCondition " + cutCondition.getName());
        isValidMatch = nodeMatches(ocletCondition, cutCondition, new HashSet<Event>());
        //if cutCondition matches -> fine, check the next ocletCondition
        //ODER-Verknüpfung -> beim ersten Erfolg Schleife beenden
        if(isValidMatch) {
          if(matchingConditions.containsValue(cutCondition)) {
            ////System.out.println("  cutCondition " + cutCondition.getName() + " is in LinkMap yet.");
            isValidMatch = false;
            break;
          }
          ////System.out.println("  ocletCondition " + ocletCondition.getName() + " matched.");
          matchingConditions.put(ocletCondition, cutCondition);
          break;
        } 
      } //END for cutCondition
      //didn't find a condition in cut that matches with ocletCondition
      //OR there are minimum two ocletCondition with same label
      //UND-Verknüpfung -> beim ersten Misserfolg Schleife beenden
      if(!isValidMatch) {
        ////System.out.println("  Oclet " + oclet.getName() + " not activated.");
        return false;
      }
    } //END for ocletCondition
    return isValidMatch;
  }

  /**
   * Checks whether the given node of an {@link hub.top.adaptiveSystem.Oclet} matches with the given node of
   * an {@link hub.top.adaptiveSystem.AdaptiveProcess}.
   * 
   * @param ocletNode
   * @param adaptiveprocessNode
   * @return
   */
  private boolean nodeMatches(Node ocletNode, Node adaptiveprocessNode, HashSet<Event> checkedEvents) {
    //System.out.println("  start nodeMatches. ocletNode " + ocletNode.getName() + " adaptiveprocessNode " + adaptiveprocessNode.getName());
    
    //check for conditions
    if(ocletNode instanceof Condition) {
      ////System.out.println("    ocletNode " + ocletNode.getName() + " is condition.");
      //if ocletNode and adaptiveprocessNode from different kind it doesn't match
      if(!(adaptiveprocessNode instanceof Condition)) {
        ////System.out.println("    no match. - different kind of nodes. ocletNode " + ocletNode.getName() + " adaptiveprocessNode " + adaptiveprocessNode.getName());
        return false;
      }
      else {
        
        Condition ocletCondition = (Condition) ocletNode;
        Condition adaptiveprocessCondition = (Condition) adaptiveprocessNode;
        
        //if the ocletNode and adaptiveprocessNode have the same name check the preSet
        if(ocletCondition.getName().equals(adaptiveprocessCondition.getName())) {
          //if there isn't a preEvent return true because the nodes match
          //if there are preEvents check the preSets
          if(ocletCondition.getPreEvents().isEmpty()) {
            ////System.out.println("    matches ocletCondition " + ocletCondition.getName() + "! - no preSet of ocletCondition.");
            return true;
          }
          else {
            //System.out.println("    matches! check the preSet of ocletCondition " + ocletCondition.getName());
            boolean isMatch = false;
            //check all nodes of the preSet of the ocletNode
            for (Node ocletPreNode : ocletCondition.getPreEvents()) {
              if(checkedEvents.contains(ocletPreNode)) {
                ////System.out.println("    event " + ocletPreNode.getName() + (" was successfully checked further."));
                return true; //I checked this event further and it matches
              }
              for (Node adaptiveprocessPreNode : adaptiveprocessCondition.getPreEvents()) {
                //recursive call of nodeMatches(node, node)
                //System.out.println("    nodeMatches( ocletPreNode " + ocletPreNode.getName() + ", adaptiveprocessPreNode " + adaptiveprocessPreNode.getName() + ")");
                isMatch = nodeMatches(ocletPreNode, adaptiveprocessPreNode, checkedEvents);
                if(isMatch) {
                  //System.out.println("    matches! - isMatch = true - ocletEvent " + ocletPreNode.getName() + " put to checkedEvents mit true");
                  checkedEvents.add((Event) ocletPreNode);
                  break;
                }
              } //END for adaptiveprocessPreNode
              if(!isMatch) return false; //for this oclet node there is no adaptive process node - no match
            } //END for ocletPreNode
            if(isMatch) {
              ////System.out.println("    match ocletCondition " + ocletCondition.getName() + "! return true.");
              return true;
            } 
          } //END if preEvents empty
        } //END if ocletCondition equals adaptiveprocessCondition
      } //END if adaptiveprocessNode is Condition
    } //END if ocletNode is Condition
    
    //check for events
    if(ocletNode instanceof Event) {
      //System.out.println("    ocletNode " + ocletNode.getName() + " is event.");
      //if ocletNode and adaptiveprocessNode from different kind it doesn't match
      if(!(adaptiveprocessNode instanceof Event)) {
        ////System.out.println("    no match. - different kind of nodes. ocletNode " + ocletNode.getName() + " adaptiveprocessNode " + adaptiveprocessNode.getName());
        return false;
      }
      else {
        Event ocletEvent = (Event) ocletNode;
        Event adaptiveprocessEvent = (Event) adaptiveprocessNode;
        
        //if the ocletNode and adaptiveprocessNode have the same name check the preSet
        if(ocletEvent.getName().equals(adaptiveprocessEvent.getName())) {
          //if the preConditions of ocletEvent are empty return true because the node matches
          //otherwise check the preSet
          if(ocletEvent.getPreConditions().isEmpty()) {
            ////System.out.println("    matches! - no preSet of ocletEvent " + ocletEvent.getName() + " put to checkedEvents mit true");
            checkedEvents.add(ocletEvent);
            return true;
          }
          else {
            //System.out.println("    matches! check the preSet of ocletEvent " + ocletEvent.getName());
            
            if (option_matchExactPrecondition) {
              // the preSet of adaptiveProcessEvent must have the same number of
              // conditions like the corresponding OcletPreNetEvent
              if (adaptiveprocessEvent.getPreConditions().size() != ocletEvent.getPreConditions().size())
                return false;           
            }

            // check all nodes of preSet of the ocletNode
            boolean isMatch = false;
            for (Node ocletPreNode : ocletEvent.getPreConditions()) {
              for (Node adaptiveprocessPreNode : adaptiveprocessEvent.getPreConditions()) {
                //recursive call of nodeMatches(node, node)
                //System.out.println("    nodeMatches( ocletPreNode " + ocletPreNode.getName() + ", adaptiveprocessPreNode " + adaptiveprocessPreNode.getName() + ")");
                isMatch = nodeMatches(ocletPreNode, adaptiveprocessPreNode, checkedEvents);
                if(isMatch) break; //found this node in adaptive process
              } //END for adaptiveprocessPreNode
              if(!isMatch) return false; //for this oclet node there is no adaptive process node - no match
            } //END for ocletPreNode
            if(isMatch) {
              ////System.out.println("    match ocletEvent " + ocletEvent.getName() + "! return true.");
              return true;
            }
          } //END if preConditions empty
        } //END if ocletEvent equals adaptiveprocessEvent
      } //END if adaptiveprocessNode is Event
    } //END if ocletNode is Event
  
    //System.out.println("      no match - ocletNode " + ocletNode.getName() + " adaptiveprocessNode " + adaptiveprocessNode.getName());
    return false;
  }

  /**
   * The method inserts all events and their postConditions in AdaptiveProcess if they don't exit.
   * If the event and its preSet exist in adaptiveProcess than merge the postSet of the inserted
   * event with the postSet of the existing adaptiveProcessEvent. The method is called
   * recursively.
   * 
   * @param currentInsertMap
   * @param depth
   *   depth of nodes to be inserted beginning from currentInsertMap, set to
   *   <code>-1</code> for unbounded depth
   */
  public void insertNodes(EMap<Condition, Condition> currentInsertMap, int depth) {
    
    if (depth == 0) {
      ////System.out.println("   finished insertNode, reached depth 0");
      return;

    } else if (depth < -20) {
      if (!MessageDialog.openQuestion(workbenchWindow.getShell(), "Possibly unsafe process?",
          "It might be that you are executing an unsafe process. Continue " +
          "with the operation or cancel?"))
      {
        return;
      }
    }
    
    ////System.out.println("   START - next level of insertNode("+depth+")");
    EMap<Node , Node> insertedMap = new BasicEMap<Node, Node>(); 
    EMap<Condition, Condition> nextInsertMap = new BasicEMap<Condition, Condition>();
    
    //iterate over marked preNet conditions
    for(ListIterator<Map.Entry<Condition, Condition>> li = currentInsertMap.listIterator(); li.hasNext();) {
      Map.Entry<Condition, Condition> ctupel = li.next();
      Condition oCondition = ctupel.getKey(); //oclet condition
      Condition mpCondition = ctupel.getValue(); //adaptive process condition

      ////System.out.println("   next condition from currentInsertMap : " + oCondition.getName());
      
      if(!oCondition.getPostEvents().isEmpty()) {
        //there is only one event in postSet, because of wellformedness of oclet (it is an occurrence net)
        Event oPostEvent = oCondition.getPostEvents().get(0);
        if(!mpCondition.getPostEvents().isEmpty()) {
          //is on postSet of mpCondition an event with same label than on oPostEvent?
          Event mpPostEvent = null;
          ////System.out.println("   is there an event in adaptive process with same name than oclet event? mpCondition " + mpCondition.getName());
          for(Event event : mpCondition.getPostEvents()) {
            if(event.getName().equals(oPostEvent.getName())) {
              mpPostEvent = event;
              break;
            }
          }
          //YES, this event is in adaptive process
          if(mpPostEvent != null) {
            ////System.out.println("   YES. event " + oPostEvent.getName() + " is in adaptiveProcess.");
            // look for additional conditions in preSet of event (in oclet)
            if(oPostEvent.getPreConditions().size() != mpPostEvent.getPreConditions().size()) {
              ////System.out.println("   the number of preConditions of mpEvent differs from the number of preConditions of oPostEvent");
              //INSERT CASE 2
              //the number of preConditions of mpEvent differs from the number of preConditions of oPostEvent
              //therefore create a new event and connect with preSet
              insertedMap.put(oPostEvent, createNode(oPostEvent));
              for(Condition oPreCondition : oPostEvent.getPreConditions()) {
                //create an arc between adaptive process event and every associated adaptiveProcessPreCondition of oclet
                if(currentInsertMap.containsKey(oPreCondition)) {
                  createArc(currentInsertMap.get(oPreCondition), insertedMap.get(oPostEvent));
                } else
                if(insertedMap.containsKey(oPreCondition)) {
                  createArc(insertedMap.get(oPreCondition), insertedMap.get(oPostEvent));
                } else {
                  //the condition does not exist - should not occur
                  ////System.out.println("the oclet preCondition " + oPreCondition.getName() + " of event " + oPostEvent.getName() + " don't exist in adaptive process. Should not occur!!");
                }
              } //END for - connect all preConditions with new adaptive process event
              // create all postConditions on adaptive process event and connect them
              for(Condition oPostCondition : oPostEvent.getPostConditions()) {
                insertedMap.put(oPostCondition, createNode(oPostCondition));
                createArc(insertedMap.get(oPostEvent), insertedMap.get(oPostCondition));
                nextInsertMap.put(oPostCondition, (Condition) insertedMap.get(oPostCondition));
              } //END Insert actual level - CASE 2
            } else {
              //if the preConditions of the both events (oclet and adaptive process) are same
              //than the event exists yet (maybe from insert of another oclet)
              boolean isSamePreSet = true;
              for(Condition oPreCondition : oPostEvent.getPreConditions()) {
                boolean foundNode = false;
                for(Condition mpPreCondition : mpPostEvent.getPreConditions()) {
                  //found different condition in preSets
                  if(oPreCondition.getName().equals(mpPreCondition.getName())) {
                    //System.out.println("   equal preSet node.");
                    foundNode = true;
                  }
                }
                if(!foundNode) {
                  isSamePreSet = false;
                  break;
                }
              }
              if(isSamePreSet) {
                ////System.out.println("   is same preSet. Don't create event.");
                //INSERT CASE 3
                //associate the ocletEvent with the adaptiveprocessEvent
                insertedMap.put(oPostEvent, mpPostEvent);
                //look for postConditions on adaptive process event
                if(!mpPostEvent.getPostConditions().isEmpty()) {
                  //there are postConditions on adaptive process event
                  //look whether all postConditions of oclet event have associated postConditions on adaptive process event
                  for(Condition oPostCondition : oPostEvent.getPostConditions()) {
                    Condition mpPostCondition = null;
                    for(Condition transientCondition : mpPostEvent.getPostConditions()) {
                      if(oPostCondition.getName().equals(transientCondition.getName())) {
                        //System.out.println("   the associated postCondition " + oPostCondition.getName() + " of oclet exists on adaptive process event.");
                        mpPostCondition = transientCondition;
                        break;
                      }
                    }
                    //if oPostCondition does not exist, than create it and connect it to event in adaptive process
                    if(mpPostCondition == null) {
                      insertedMap.put(oPostCondition, createNode(oPostCondition));
                      createArc(mpPostEvent, insertedMap.get(oPostCondition));
                    } else {
                      insertedMap.put(oPostCondition, mpPostCondition);
                    }
                    nextInsertMap.put(oPostCondition, (Condition) insertedMap.get(oPostCondition)); //END actual insert level - CASE 3
                  } //END oPostCondition in mpPostEvent.postCondition? 
                } //END are there postConditions on adaptive process event?
              } //END isSamePreSet
              else {
                //INSERT CASE 2
                //is not the same preSet
                //therefore create a new event and connect with preSet
                insertedMap.put(oPostEvent, createNode(oPostEvent));
                for(Condition oPreCondition : oPostEvent.getPreConditions()) {
                  //create an arc between adaptive process event and every associated adaptiveProcessPreCondition of oclet
                  if(currentInsertMap.containsKey(oPreCondition)) {
                    createArc(currentInsertMap.get(oPreCondition), insertedMap.get(oPostEvent));
                  } else
                  if(insertedMap.containsKey(oPreCondition)) {
                    createArc(insertedMap.get(oPreCondition), insertedMap.get(oPostEvent));
                  } else {
                    //the condition does not exist - should not occur
                    ////System.out.println("the oclet preCondition " + oPreCondition.getName() + " of event " + oPostEvent.getName() + " don't exist in adaptive process. Should not occur!!");
                  }
                } //END for - connect all preConditions with new adaptive process event
                // create all postConditions on adaptive process event and connect them
                for(Condition oPostCondition : oPostEvent.getPostConditions()) {
                  insertedMap.put(oPostCondition, createNode(oPostCondition));
                  createArc(insertedMap.get(oPostEvent), insertedMap.get(oPostCondition));
                  nextInsertMap.put(oPostCondition, (Condition) insertedMap.get(oPostCondition));
                } //END Insert actual level - CASE 2
                break;
              }
            } //END else - means: oPostEvent had only 1 preCondition 
          } else {
          //INSERT CASE 2
          //NO, this event is not in adaptive process
            //create an event with attribute values of the oPostEvent
            insertedMap.put(oPostEvent, createNode(oPostEvent));
            
            //look for preConditions of the oclet event and create connections if they exist
            for(Condition oPreCondition : oPostEvent.getPreConditions()) {
              //is the oCondition in initial Map?
              if(currentInsertMap.containsKey(oPreCondition)) {
                //the condition exists in adaptive Process (is in cut) 
                createArc(currentInsertMap.get(oPreCondition), insertedMap.get(oPostEvent));
              } else if(insertedMap.containsKey(oPreCondition)) {
                //the condition exists in adaptive process (was created during this step)
                createArc(insertedMap.get(oPreCondition), insertedMap.get(oPostEvent));
              } else {
                //the condition doesn't exist in adaptive process
                //should not occur !!!
                ////System.out.println("the oclet preCondition " + oPreCondition.getName() + " of event " + oPostEvent.getName() + " don't exist in adaptive process. Should not occur!!");
              }
            }
            //if the oPostEvent has postConditions create those conditions in adaptive process
            //and connect them with associated adaptive process event
            for(Condition oPostCondition : oPostEvent.getPostConditions()) {
              insertedMap.put(oPostCondition, createNode(oPostCondition));
              createArc(insertedMap.get(oPostEvent), insertedMap.get(oPostCondition));
              nextInsertMap.put(oPostCondition, (Condition) insertedMap.get(oPostCondition)); //END actual insert level - CASE 2
            }
          }
        } else {
          //mpCondition has no postEvent -> create an event with attribute values of oPostEvent
          //has the oPostEvent more than one preConditions?
          if(oPostEvent.getPreConditions().size() > 1) {
            boolean containsAllPreCondition = true;
            for(Condition oPreCondition : oPostEvent.getPreConditions()) {
              if(!currentInsertMap.containsKey(oPreCondition)) {
                containsAllPreCondition = false;
                break;
              }
            }
            //event is in this insertLevel
            if(containsAllPreCondition) {
              //INSERT CASE 2
              insertedMap.put(oPostEvent, createNode(oPostEvent));
              
              //look for all preConditions on oclet postEvent whether they have associated condition in adaptive process
              //than connect new adaptive process event with preCondition
              for(Condition oPreCondition : oPostEvent.getPreConditions()) {
                //is the oCondition in initial Map?
                if(currentInsertMap.containsKey(oPreCondition)) {
                  //the condition exists in adaptive Process (is in cut) 
                  createArc(currentInsertMap.get(oPreCondition), insertedMap.get(oPostEvent));
                } else if(insertedMap.containsKey(oPreCondition)) {
                  //the condition exists in adaptive process (was created during this step)
                  createArc(insertedMap.get(oPreCondition), insertedMap.get(oPostEvent));
                } else {
                  //the condition doesn't exist in adaptive process
                  //should not occur !!!
                  ////System.out.println("the oclet preCondition " + oPreCondition.getName() + " of event " + oPostEvent.getName() + " don't exist in adaptive process. Should not occur!!");
                }
              }
              //if the oPostEvent has postConditions create those conditions in adaptive process
              //and connect them with associated adaptive process event
              for(Condition oPostCondition : oPostEvent.getPostConditions()) {
                insertedMap.put(oPostCondition, createNode(oPostCondition));
                createArc(insertedMap.get(oPostEvent), insertedMap.get(oPostCondition));
                nextInsertMap.put(oPostCondition, (Condition) insertedMap.get(oPostCondition)); //END actual insert level - CASE 2
              }
            } else {
              //event is in a future insert level
              //put the condition in the nextLevelMap
              ////System.out.println("      insert postEvent of oclet condition " + oCondition.getName() + " in next Level.");
              nextInsertMap.put(oCondition, mpCondition);
            }
          } else {
            //oPostEvent has only 1 preCondition
            //INSERT CASE 2
            insertedMap.put(oPostEvent, createNode(oPostEvent));
            
            //look for all preConditions on oclet postEvent whether they have associated condition in adaptive process
            //than connect new adaptive process event with preCondition
            for(Condition oPreCondition : oPostEvent.getPreConditions()) {
              //is the oCondition in initial Map?
              if(currentInsertMap.containsKey(oPreCondition)) {
                //the condition exists in adaptive Process (is in cut) 
                createArc(currentInsertMap.get(oPreCondition), insertedMap.get(oPostEvent));
              } else if(insertedMap.containsKey(oPreCondition)) {
                //the condition exists in adaptive process (was created during this step)
                createArc(insertedMap.get(oPreCondition), insertedMap.get(oPostEvent));
              } else {
                //the condition doesn't exist in adaptive process
                //should not occur !!!
                ////System.out.println("the oclet preCondition " + oPreCondition.getName() + " of event " + oPostEvent.getName() + " don't exist in adaptive process. Should not occur!!");
              }
            }
            //if the oPostEvent has postConditions create those conditions in adaptive process
            //and connect them with associated adaptive process event
            for(Condition oPostCondition : oPostEvent.getPostConditions()) {
              insertedMap.put(oPostCondition, createNode(oPostCondition));
              createArc(insertedMap.get(oPostEvent), insertedMap.get(oPostCondition));
              nextInsertMap.put(oPostCondition, (Condition) insertedMap.get(oPostCondition)); //END actual insert level - CASE 2
            }
          }
          
          
        }
      } else {
        ////System.out.println("   no postEvent on oclet condition " + oCondition.getName() + " - nothing to do.");
        //END actual insert level - CASE 1
      }
      
    }//END for - iterate the currentInsertMap
    //after iteration over all elements of currentInsertMap set 
    currentInsertMap.clear();
    //recursive call of insertNodes()
    if(nextInsertMap != null && !nextInsertMap.isEmpty())
      // done with adding nodes of depth d, proceed with nodes of depth d-1
      insertNodes(nextInsertMap, depth-1);
    
    //clean the nextInsertMap and insertedMap for next insertLevel
    nextInsertMap.clear();
    insertedMap.clear();
    ////System.out.println("   END - next level of insertNode()");
  }
  
  /**
   * Create a copy of ocletNode and add it to adaptiveProcess nodeList
   * @author Manja Wolf
   * @param ocletNode
   * @param adaptiveProcessNode
   * @return
   */
  private Node createNode(Node ocletNode) {
    
    Node copiedNode = null;
    
    if(ocletNode instanceof Condition) {
      ////System.out.println("      create condition " + ocletNode.getName());
      Condition ocletCondition = (Condition) ocletNode;
      Condition copiedCondition = AdaptiveSystemFactory.eINSTANCE.createCondition();
      copiedCondition.unsetAbstract(); //TODO: fixme
      copiedCondition.setName(ocletCondition.getName());
      copiedCondition.setTemp(ocletCondition.getTemp());
      copiedCondition.setDisabledByAntiOclet(false);
      copiedCondition.setDisabledByConflict(false);
      
      AddCommand cmd = new AddCommand(simView.processViewEditor.getEditingDomain(), simView.adaptiveProcess, AdaptiveSystemPackage.eINSTANCE.getOccurrenceNet_Nodes(), copiedCondition);
      cmd.canExecute();
      ((CommandStack) ((EditingDomain) simView.processViewEditor.getEditingDomain()).getCommandStack()).execute(cmd);     
      
      //adaptiveProcess.getNodes().add(copiedCondition);
      copiedNode = copiedCondition;
    } else
    if(ocletNode instanceof Event) {
      ////System.out.println("      create event " + ocletNode.getName());
      Event ocletEvent = (Event) ocletNode;
      Event copiedEvent = AdaptiveSystemFactory.eINSTANCE.createEvent();
      copiedEvent.unsetAbstract(); //TODO: fixme
      copiedEvent.setName(ocletEvent.getName());
      copiedEvent.setTemp(ocletEvent.getTemp());
      copiedEvent.setDisabledByAntiOclet(false);
      copiedEvent.setDisabledByConflict(false);
      
      AddCommand cmd = new AddCommand(simView.processViewEditor.getEditingDomain(), simView.adaptiveProcess, AdaptiveSystemPackage.eINSTANCE.getOccurrenceNet_Nodes(), copiedEvent);
      cmd.canExecute();
      ((CommandStack) ((EditingDomain) simView.processViewEditor.getEditingDomain()).getCommandStack()).execute(cmd);
      
      copiedNode = copiedEvent;
    }   
    
    if (copiedNode != null)
      newNodes.add(copiedNode); // remember all new nodes
    
    return copiedNode;
  }
  
  /**
   * Create an arc and add it to adaptiveProcess arcList
   * 
   * @author Manja Wolf
   * @param source
   * @param target
   */
  private boolean createArc(Node source, Node target) {
    
    //create an arcToEvent
    if(source instanceof Condition && target instanceof Event) {
      ////System.out.println("      create arcToEvent with source " + source.getName() + " and target " + target.getName());
      
      ArcToEvent arc = AdaptiveSystemFactory.eINSTANCE.createArcToEvent();
      EList<Command> cmdList = new BasicEList<Command>();
      SetCommand cmdSource = new SetCommand(
          simView.processViewEditor.getEditingDomain(), arc, AdaptiveSystemPackage.eINSTANCE.getArcToEvent_Source(), (Condition) source);
      cmdSource.canExecute();
      cmdList.add(cmdSource);
      SetCommand cmdDestination = new SetCommand(
          simView.processViewEditor.getEditingDomain(), arc, AdaptiveSystemPackage.eINSTANCE.getArcToEvent_Destination(), (Event) target);
      cmdDestination.canExecute();
      cmdList.add(cmdDestination);
      
      AddCommand cmdAdd = new AddCommand(simView.processViewEditor.getEditingDomain(), simView.adaptiveProcess, AdaptiveSystemPackage.eINSTANCE.getOccurrenceNet_Arcs(), arc);
      cmdAdd.canExecute();
      cmdList.add(cmdAdd);
      
      CompoundCommand fireCmd = new CompoundCommand(cmdList);
      // and execute it in a transactional editing domain (for undo/redo)
      fireCmd.canExecute();
      ((CommandStack) ((EditingDomain) simView.processViewEditor.getEditingDomain()).getCommandStack()).execute(fireCmd);

      ((CanonicalConnectionEditPolicy) simView.processViewEditor
          .getDiagramEditPart().getEditPolicy("Canonical")).refresh();
      
      return true;
    } else
    //create an arcToCondition
    if(source instanceof Event && target instanceof Condition) {
      ////System.out.println("      create arcToCondition with source " + source.getName() + " and target " + target.getName());
      ArcToCondition arc = AdaptiveSystemFactory.eINSTANCE.createArcToCondition();
      EList<Command> cmdList = new BasicEList<Command>();
      SetCommand cmdSource = new SetCommand(
          simView.processViewEditor.getEditingDomain(), arc, AdaptiveSystemPackage.eINSTANCE.getArcToCondition_Source(), (Event) source);
      cmdSource.canExecute();
      cmdList.add(cmdSource);
      SetCommand cmdDestination = new SetCommand(
          simView.processViewEditor.getEditingDomain(), arc, AdaptiveSystemPackage.eINSTANCE.getArcToCondition_Destination(), (Condition) target);
      cmdDestination.canExecute();
      cmdList.add(cmdDestination);
      
      AddCommand cmdAdd = new AddCommand(simView.processViewEditor.getEditingDomain(), simView.adaptiveProcess, AdaptiveSystemPackage.eINSTANCE.getOccurrenceNet_Arcs(), arc);
      cmdAdd.canExecute();
      cmdList.add(cmdAdd);
      
      CompoundCommand fireCmd = new CompoundCommand(cmdList);
      // and execute it in a transactional editing domain (for undo/redo)
      fireCmd.canExecute();
      ((CommandStack) ((EditingDomain) simView.processViewEditor.getEditingDomain()).getCommandStack()).execute(fireCmd);
      
      ((CanonicalConnectionEditPolicy) simView.processViewEditor
          .getDiagramEditPart().getEditPolicy("Canonical")).refresh();
      
      
      return true;
    }
    ////if(source != null && target != null)
    ////  System.out.print("      Arc couldn't create. Shouldn't occur!!! (source " + source.getName() + ", target " + target.getName());
    ////else
    ////  System.out.print("      Arc couldn't create. Shouldn't occur!!! source or/and target == null.");
    
    return false;
  }
  
  /**
   * Set all nodes that follow after the conditions in disableMap to
   * 'disabled by anti-oclet'.
   * 
   * @param disableMap
   *   mapping that matches the preconditions of an anti-oclet with
   *   corresponding conditions in the cut of the adaptive process, this
   *   mapping is generated by {@link SimulationInteractiveAction#matchPreConditionsWithCut(EList, EList, EMap)}
   *   
   * 
   * @author Manja Wolf
   */
  public void disableNodes_anti(EMap<Condition, Condition> disableMap) {
    if(disableMap == null || disableMap.isEmpty())
      return;

    ////System.out.println("apply anti oclet " + ((Oclet) ((Condition) disableMap.get(0).getKey()).eContainer().eContainer()).getName());
    
    //iterate over marked preNet conditions
    for(ListIterator<Map.Entry<Condition, Condition>> li = disableMap.listIterator(); li.hasNext();) {
      Map.Entry<Condition, Condition> ctupel = li.next();
      Condition oCondition = ctupel.getKey(); //oclet condition
      Condition mpCondition = ctupel.getValue(); //adaptive process condition
      
      if(!oCondition.getPostEvents().isEmpty()) {
        //There is only one postEvent
        Event oPostEvent = oCondition.getPostEvents().get(0);
        
        //if oPostEvent has one preCondition in a deeper level (it is not in disableMap) 
        //than do nothing, event will be disabled by further event
        boolean preSetIsInMap = true;
        for(Condition oPreCondition : oPostEvent.getPreConditions()) {
          if(!disableMap.containsKey(oPreCondition)) {
            preSetIsInMap = false;
            break;
          }
        }
        if(preSetIsInMap) {
          //all preConditions in disabledMap => look for the same labeled event in adaptive process with same preSet
          Event mpEventToDisable = null;
          //is there a same labeled event with same preConditions like oPostEvent on mpCondition 
          for(Event mpPostEvent : mpCondition.getPostEvents()) {
            //there is an event with same name like the event of oclet on mpCondition
            if(mpPostEvent.getName().equals(oPostEvent.getName()) 
              && mpPostEvent.getPreConditions().size() == oPostEvent.getPreConditions().size()) {
              //the preSets have same cardinality
              //check the label of preConditions
              boolean isSamePreSet = true;
              for(Condition mpPreCondition : mpPostEvent.getPreConditions()) {
                boolean foundCondition = false;
                for(Condition oPreCondition : oPostEvent.getPreConditions()) {
                  if(disableMap.containsValue(mpPreCondition) 
                      && disableMap.get(oPreCondition).equals(mpPreCondition)) {
                    //is the same preCondition
                    foundCondition = true;
                    break;
                  }
                }
                if(!foundCondition) {
                  isSamePreSet = false;
                  break;
                }
              } //END for check every preCondition of adaptive process event
              if(isSamePreSet) {
                mpEventToDisable = mpPostEvent;
                break;
              }
            } //else - name is different or preSet has other size - check next mpPostEvent
          }
          if(mpEventToDisable != null) {
            disableEvent(mpEventToDisable);
          }
        } else {
          ////System.out.println("Do nothing. The Event is deeper in path and will be disabled by another event.");
        }
      } //END if postSet of oCondition is not empty
      else {
        ////System.out.println("Do nothing. Oclet condition " + oCondition.getName() + " has no postEvent.");
      }
    } //END for disableMap iterieren
  }
  
  /**
   * During apply of anti oclets:
   * Sets the isDisabledByAntiOclet to the Event and automatically the
   * whole PostSet of it.
   * 
   * @author Manja Wolf
   * @param event
   */
  private void disableEvent(Event event) {
    
    if(!event.isSetDisabledByAntiOclet() || (event.isSetDisabledByAntiOclet() && !event.isDisabledByAntiOclet())) {
      ////System.out.println("disable Event " + event.getName());
      SetCommand cmd = new SetCommand(
          simView.processViewEditor.getEditingDomain(), event, AdaptiveSystemPackage.eINSTANCE.getNode_DisabledByAntiOclet(), true);
      cmd.setLabel("set attribute isDisabledByAntiOclet on event");
      cmd.canExecute();
      ((CommandStack) ((EditingDomain) simView.processViewEditor.getEditingDomain()).getCommandStack()).execute(cmd); 
    }
  }
  
  
  /**
   * Fill the list of {@link #activatedEvents}.
   * 
   * @author Manja Wolf
   */
  public void getActivatedEvents() {

    //find the saturated events - start with cut (all marked conditions)
    for(Condition cutCondition : simView.adaptiveProcess.getMarkedConditions()) {
      //check every postEvent of the cutCondition whether it is saturated and not disabled
      for(Event event : cutCondition.getPostEvents()) {
        if(!this.activatedEvents.contains(event)
            && event.isSaturated() 
            && (!event.isSetDisabledByAntiOclet() || (event.isSetDisabledByAntiOclet() && !event.isDisabledByAntiOclet())) 
            && (!event.isSetDisabledByConflict() || (event.isSetDisabledByConflict() && !event.isDisabledByConflict())))
        {
          boolean isActivatedEvent = true;

          if (option_checkCapacity) {
            // check the postSet of the saturated, not disabled event
            // whether the firing of this event makes the process unsafe
            for(Condition postCondition : event.getPostConditions()) {
              
              //if event makes net unsave -> not allowed to fire
              for(Condition cutCondition2 : simView.adaptiveProcess.getMarkedConditions()){
                if(    cutCondition2.getName().equals(postCondition.getName())
                  && !event.getPreConditions().contains(cutCondition2))
                {
                  ////System.out.println("Event " + event.getName() +  " makes the net unsafe.");
                  isActivatedEvent = false;
                  break;
                }
              }
            } //END for check postSet of event
          }

          if(isActivatedEvent) {
            if(!activatedEvents.contains(event)) activatedEvents.add(event);
          } //END if isActivatedEvents

        } //END if event is saturated and not disabled
      } //END for check every postEvent of the cutCondition
    } //END for check every cutCondition
  }
  
  /**
   * 
   * 
   * @author Manja Wolf
   * @param event
   */
  protected void fireEvent(Event firedEvent) {

    EList<Command> cmdList = new BasicEList<Command>();
    
    for(ArcToEvent arcToEvent : firedEvent.getIncoming()) {
      Condition source = arcToEvent.getSource();
      SetCommand cmdConsume = new SetCommand(
          simView.processViewEditor.getEditingDomain(), source, AdaptiveSystemPackage.eINSTANCE.getCondition_Token(), source.getToken() - arcToEvent.getWeight());
      cmdConsume.canExecute();
      cmdList.add(cmdConsume);
    }
    for(ArcToCondition arcToCondition : firedEvent.getOutgoing()) {
      Condition destination = arcToCondition.getDestination();
      SetCommand cmdProduce = new SetCommand(
          simView.processViewEditor.getEditingDomain(), destination, AdaptiveSystemPackage.eINSTANCE.getCondition_Token(), destination.getToken() + arcToCondition.getWeight());
      cmdProduce.canExecute();
      cmdList.add(cmdProduce);
    }
    
    //Set the events which are in conflict with the fired event to disabledByConflict
    for(Event event : activatedEvents) {
      // do not disable the event that is being fired
      if (event == firedEvent) continue;
      
      //if(event.isSetDisabledByConflict() && event.isDisabledByAntiOclet()) continue;
      for(Condition preCondition : firedEvent.getPreConditions()) {
        if(event.getPreConditions().contains(preCondition)) {
          //set the event disabledByConflict
          if(!event.isSetDisabledByConflict() || (event.isSetDisabledByConflict() && !event.isDisabledByConflict())) {
            SetCommand cmd = new SetCommand(
                simView.processViewEditor.getEditingDomain(), event, AdaptiveSystemPackage.eINSTANCE.getNode_DisabledByConflict(), true);
            cmd.setLabel("set attribute disabledByConflict on event");
            cmd.canExecute();
            cmdList.add(cmd);
            break;
          }
        }
      }
    }
    executeCommands(cmdList);
  }
  
  /**
   * Set {@link Event#isEnabled()} to <code>true</code>. The diagram viewer
   * will then highlight the event in a different color.
   * 
   * @param event
   */
  protected Command highlightEvent_enabled(Event event) {
    SetCommand cmd = new SetCommand(
        simView.processViewEditor.getEditingDomain(), event, AdaptiveSystemPackage.eINSTANCE.getEvent_Enabled(), true);
    cmd.setLabel("set attribute enabled on event");
    cmd.canExecute();
    return cmd;
  }
  
  /**
   * Set {@link Event#isEnabled()} to <code>true</code>. The diagram viewer
   * will then highlight the event in a different color.
   * 
   * @param event
   */
  protected Command highlightEvent_disabled(Event event) {
    SetCommand cmd = new SetCommand(
        simView.processViewEditor.getEditingDomain(), event, AdaptiveSystemPackage.eINSTANCE.getEvent_Enabled(), false);
    cmd.setLabel("set attribute enabled on event");
    cmd.canExecute();
    return cmd;
  }

  /**
   * 
   * @param cmdList
   */
  protected void executeCommands(EList<Command> cmdList) {
    CompoundCommand fireCmd = new CompoundCommand(cmdList);
    // and execute it in a transactional editing domain (for undo/redo)
    fireCmd.canExecute();
    ((CommandStack) ((EditingDomain) simView.processViewEditor.getEditingDomain()).getCommandStack()).execute(fireCmd);
  }
}
