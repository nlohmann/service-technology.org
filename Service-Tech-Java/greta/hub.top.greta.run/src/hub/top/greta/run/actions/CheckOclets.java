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

package hub.top.greta.run.actions;

import java.util.LinkedList;
import java.util.List;

import hub.top.adaptiveSystem.AdaptiveSystem;
import hub.top.adaptiveSystem.AdaptiveSystemPackage;
import hub.top.adaptiveSystem.Condition;
import hub.top.adaptiveSystem.DoNet;
import hub.top.adaptiveSystem.Event;
import hub.top.adaptiveSystem.Node;
import hub.top.adaptiveSystem.Oclet;
import hub.top.adaptiveSystem.PreNet;
import hub.top.adaptiveSystem.Temp;
import hub.top.adaptiveSystem.diagram.part.AdaptiveSystemDiagramEditor;
import hub.top.greta.validation.ModelError;

import org.eclipse.emf.common.command.Command;
import org.eclipse.emf.common.command.CommandStack;
import org.eclipse.emf.common.command.CompoundCommand;
import org.eclipse.emf.common.util.BasicEList;
import org.eclipse.emf.common.util.BasicEMap;
import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.common.util.EMap;
import org.eclipse.emf.edit.command.SetCommand;
import org.eclipse.emf.edit.domain.EditingDomain;
import org.eclipse.jface.action.Action;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.IObjectActionDelegate;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.IWorkbenchWindowActionDelegate;


public class CheckOclets extends Action implements
		IWorkbenchWindowActionDelegate,IObjectActionDelegate {

	protected IWorkbenchWindow workbenchWindow;
	protected AdaptiveSystemDiagramEditor adaptiveSystemDiagramEditor;
	protected AdaptiveSystem adaptiveSystem;
	
	protected EMap<Node, Boolean> cycleCheckMap = new BasicEMap<Node, Boolean>();
		
	public void setActivePart(IAction action, IWorkbenchPart targetPart) {
		//do nothing
		
	}

	
	public void dispose() {
		//do nothing
	}

	public void init(IWorkbenchWindow window) {
		workbenchWindow = window;	
	}

	public void run(IAction action) {
		if(workbenchWindow.getActivePage().getActiveEditor() instanceof AdaptiveSystemDiagramEditor 
				&& action.getId().equals("hub.top.GRETA.run.checkOclets")) {
			//System.out.println("Check wellformedness of oclets : START run() ");
			adaptiveSystemDiagramEditor = (AdaptiveSystemDiagramEditor) workbenchWindow.getActivePage().getActiveEditor();
			adaptiveSystem = (AdaptiveSystem) adaptiveSystemDiagramEditor.getDiagram().getElement();
			EList<Oclet> ocletList = adaptiveSystem.getOclets();
					
			List<ModelError> errors = new LinkedList<ModelError>();
	    
			if(!adaptiveSystem.isSetWellformednessToOclets()) {
				// create a list of commands and add commands with values of attribute wellformed to this list
				EList<Command> cmdList = new BasicEList<Command>();
				
				for(Oclet oclet : ocletList) {
					//check only the oclets which are not wellformed
					if(!oclet.isWellFormed()) {
						System.out.println("Check wellformedness of oclet : " + oclet.getName());
						if(checkWellformedness(oclet, errors)) {
							SetCommand cmd = new SetCommand(
									adaptiveSystemDiagramEditor.getEditingDomain(), oclet, AdaptiveSystemPackage.eINSTANCE.getOclet_WellFormed(), true);
							cmd.setLabel("set oclet attribute " + AdaptiveSystemPackage.eINSTANCE.getOclet_WellFormed().getName());
							cmd.canExecute();
							cmdList.add(cmd);
						}
						else {
							SetCommand cmd = new SetCommand(
									adaptiveSystemDiagramEditor.getEditingDomain(), oclet, AdaptiveSystemPackage.eINSTANCE.getOclet_WellFormed(), false);
							cmd.setLabel("set oclet attribute " + AdaptiveSystemPackage.eINSTANCE.getOclet_WellFormed().getName());
							cmd.canExecute();
							cmdList.add(cmd);				
						}
					}
				}
				//set wellformednessOfOclets to true
				SetCommand cmd = new SetCommand(
						adaptiveSystemDiagramEditor.getEditingDomain(), adaptiveSystem, AdaptiveSystemPackage.eINSTANCE.getAdaptiveSystem_SetWellformednessToOclets(), true);
				cmd.setLabel("set adaptive process attribute " + AdaptiveSystemPackage.eINSTANCE.getAdaptiveSystem_SetWellformednessToOclets().getName());
				cmd.canExecute();
				cmdList.add(cmd);
				// and join all wellformed commands in a compound transition fire command
				CompoundCommand fireCmd = new CompoundCommand(cmdList);
				fireCmd.setLabel("proof oclets and set wellformed to each oclet.");
				fireCmd.setDescription("proof oclets and set wellformed to each oclet.");
				// and execute it in a transactional editing domain (for undo/redo)
				fireCmd.canExecute();
				((CommandStack) ((EditingDomain) adaptiveSystemDiagramEditor.getEditingDomain()).getCommandStack()).execute(fireCmd);
				action.setEnabled(false);
			} 
		} else {
			/*
			Shell shell = new Shell();
			MessageDialog.openInformation(
				shell,
				"AdaptiveSystem - check wellformedness of oclets run()", 
				"This button is only enabled for *.adaptiveSystem_diagram files.");
				*/
			action.setEnabled(false);
		}
	}

	/**
	 * track selections in editor and reset button proofWellformednessOfOclets if an oclets preNet could be changed
	 * 
	 * @author Manja Wolf
	 */
	public void selectionChanged(IAction action, ISelection selection) {
		if(workbenchWindow.getActivePage().getActiveEditor() instanceof AdaptiveSystemDiagramEditor 
				&& action.getId().equals("hub.top.GRETA.run.checkOclets")) {
			
			adaptiveSystemDiagramEditor = (AdaptiveSystemDiagramEditor) workbenchWindow.getActivePage().getActiveEditor();
			adaptiveSystem = (AdaptiveSystem) adaptiveSystemDiagramEditor.getDiagram().getElement();
			
			if(adaptiveSystem.isSetWellformednessToOclets() ) {
				//if SetWellformdnessToOclets is true and action is enabled, than was restart of editor and initial values should be set
				//(action disabled) ==> synchronize setWellformednessToOclets with action.isEnabled()
				if(action.isEnabled()) {
					//System.out.println("ap: " + adaptiveSystem.getOclets() + " initial ap.true und action.enabled.");
					action.setEnabled(false);
					return;
				}
			} //END if isSetWellformedToOClets
			else {
				if(!action.isEnabled()) {
					//System.out.println("ap: " + adaptiveSystem.getOclets() + "initial ap.false und action.disabled.");
					action.setEnabled(true);
				}
			}
		} else if((!(workbenchWindow.getActivePage().getActiveEditor() instanceof AdaptiveSystemDiagramEditor)
				|| !action.getId().equals("hub.top.GRETA.run.checkOclets"))
				&& action.isEnabled()) {
			/*
			Shell shell = new Shell();
			MessageDialog.openInformation(
				shell,
				"AdaptiveSystem - check wellformedness of oclets selctionChanged()",
				"This button is only enabled for *.adaptiveSystem_diagram files.");
			*/
			action.setEnabled(false);
		}
	}
	
	
	
	
	/**
	 * At this time an oclet is well-formed if
	 * 1.) minimal nodes in preNet are conditions (nothing else) 
	 * 2.) maximal nodes in preNet are marked conditions (nothing else)
	 * 3.) minimal nodes (nodes without preSet) not allowed in doNet (neither conditions nor events) 
	 * 4.) abstract nodes not allowed in whole oclet 
	 * 5.) passive nodes not allowed in doNet
	 * 6.) conflicts not allowed in whole oclet (no forward and no backward)
	 * 7.) a non-empty preNet is required
	 * 8.) in oclet is no cycle
	 * 9.) no two conditions in PostSet of an event are same labeled
	 * @param oclet
	 * @return
	 */
	private boolean checkWellformedness(Oclet oclet, List<ModelError> errors) {

		Shell shell = new Shell();
		EList<Node> minimalNodeList = new BasicEList<Node>();
		
		//7. a non-empty preNet is required (e.g. a maximal, marked condition in preNet is required)
		if(oclet.getPreNet().getNodes().isEmpty()) {
			System.out.println("  not well formed - preNet is empty.");
			MessageDialog.openInformation(
				shell,
				"AdaptiveSystem - check wellformedness of oclets",
				"Oclet " + oclet.getName() + " is not used for execution of a step because it is not wellformed. PreNet is empty.");
			return false;
		}
		
		//check each preNet-Node
		for (Node ocletNode : oclet.getPreNet().getNodes()) {
			//check 1. - 6. in a separate method isWellformed(node)
			if(!isWellformed(ocletNode, errors)) break;
			
			//8. fill the list of minimal nodes for check of cycles
			if( (  ocletNode instanceof Condition
				   && ((Condition) ocletNode).getPreEvents().isEmpty())
				||
				(  ocletNode instanceof Event
				   && ((Event) ocletNode).getPreConditions().isEmpty()))
			{
				minimalNodeList.add(ocletNode);
			}
			
		}
		
		//check each DoNet-Nodes
		for (Node ocletNode : oclet.getDoNet().getNodes()) {
			//check 1. - 6. in a separate method isWellformed(node)
			if(!isWellformed(ocletNode, errors)) return false;
		}
		
		//8. in oclet is no cycle allowed
		for(Node node : minimalNodeList) {
			if(findCycle(node)) {
				System.out.println("  not well formed - there is a cycle in oclet (start: "+node.getName()+")");
				MessageDialog.openInformation(
						shell,
						"AdaptiveSystem - check wellformedness of oclets",
						"Oclet " + oclet.getName() + " is not used for execution of a step because it is not wellformed. There is at least one cycle in oclet.");
				cycleCheckMap.clear();
				return false;
			}
		}
		//8. in oclet is no cycle 
		//if number of checked nodes is smaller as the number of oclet nodes 
		//than there are not minimal nodes i can't reach from minimal nodes hence they are in a cycle.
		if(cycleCheckMap.size() != oclet.getPreNet().getNodes().size() + oclet.getDoNet().getNodes().size()) {
			System.out.println("  not well formed - there is a cycle in oclet (wrong number of nodes).");
			MessageDialog.openInformation(
					shell,
					"AdaptiveSystem - check wellformedness of oclets",
					"Oclet " + oclet.getName() + " is not used for execution of a step because it is not wellformed. There is at least one cycle in oclet.");
			cycleCheckMap.clear();
			return false;
		}
		cycleCheckMap.clear();
		
		System.out.println("oclet " + oclet.getName() + " is wellformed.");
		return true;
	}

	
	/**
	 * This method checks that
	 * 1.) minimal nodes in preNet are conditions (nothing else) 
	 * 2.) maximal nodes in preNet are marked conditions (nothing else)
	 * 3.) minimal nodes (nodes without preSet) not allowed in doNet (neither conditions nor events) 
	 * 4.) abstract nodes not allowed in whole oclet 
	 * 5.) passive nodes not allowed in doNet
	 * 6.) conflicts not allowed in whole oclet (no forward and no backward)
	 * 9.) no two conditions in PostSet of an event are equally labeled
	 * 
	 * 
	 * @param node
	 * @param errors list of errors found in the oclet
	 * @return
	 */
	private boolean isWellformed(Node ocletNode, List<ModelError> errors) {

	  int oldErrors = errors.size();
	  
		String ocletName = ((Oclet) ocletNode.eContainer().eContainer()).getName();
		
    if (ocletNode instanceof Condition) {
      
      Condition c = (Condition)ocletNode;
      
      //6. a preNet don't have conflicts, that means a condition has maximal one event in postEvents
      if(c.getPostEvents().size() > 1) {
        String postEvents = "";
        for (Event e : c.getPostEvents()) postEvents += "'"+e.getName()+"' ";
        errors.add(new ModelError(ocletNode, ocletName, "Condition '" + ocletNode.getName() + "' has more than one post-event: "+postEvents));
      }
      //6. an preNet don't have conflicts, that means a condition has maximal one event in preEvents
      if(c.getPreEvents().size() > 1) {
        String preEvents = "";
        for (Event e : c.getPreEvents()) preEvents += "'"+e.getName()+"' ";
        errors.add(new ModelError(ocletNode, ocletName, "Condition '" + ocletNode.getName() + "' has more than one pre-event: "+preEvents));
      }
      
      if(ocletNode.eContainer() instanceof PreNet) {
        
        //2. all maximal conditions have to be marked     
        if(c.isMaximal() && !c.isMarked()) {
          errors.add(new ModelError(ocletNode, ocletName, "Condition '" + ocletNode.getName() + "' is maximal but not marked"));
        }
      }
      
      if(ocletNode.eContainer() instanceof DoNet) {
        //3. conditions in doNet are not allowed to be minimal. A minimal condition has no preEvent.
        if(c.getPreEvents().isEmpty()) {
          errors.add(new ModelError(ocletNode, ocletName, "Condition '" + ocletNode.getName() + "' of the contribution has no pre-event."));
        }
      }
      
    }
		
		if(ocletNode instanceof Event) {
		  Event e = (Event)ocletNode;
		  
      //2. maximal events in preNet are not allowed
      for(Condition postCondition : e.getPostConditions()) {
        if (postCondition.isAbstract()) continue;
        for(Condition postCondition2 : e.getPostConditions()) {
          if(postCondition != postCondition2 && postCondition.getName().equals(postCondition2.getName())) {
            errors.add(new ModelError(ocletNode, ocletName, "Event '" + ocletNode.getName() + "' has two post-condition with the same label: "+postCondition.getName()));
          }
        }
      }
		  
	    if(ocletNode.eContainer() instanceof PreNet) {

				//2. maximal events in preNet are not allowed
	      for(Condition postCondition : e.getPostConditions()) {
          if (postCondition.isAbstract()) continue;
					if(postCondition.eContainer() instanceof DoNet) {
					  errors.add(new ModelError(ocletNode, ocletName, "Event '" + ocletNode.getName() + "' has post-condition in the contribution: "+postCondition.getName()));
					}
				}//END if maximal events in preNet (2.) and 9.) 
			}
	    
	    if(ocletNode.eContainer() instanceof DoNet) {
	      
  			//3. events in doNet are not allowed to be minimal. A minimal event has no preConditions.
  			if(e.getPreConditions().isEmpty()) {
  			  errors.add(new ModelError(ocletNode, ocletName, "Event '" + ocletNode.getName() + "' has no pre-condition"));
  			}
	    }

		}
		return oldErrors == errors.size(); // no new errors
	}
	
	/**
	 * checks the oclet wether there is a cycle in it
	 * @author Manja Wolf
	 */
	private boolean findCycle(Node node) {
		//is not in cycleCheckMap => was not checked until now therefore check the node
		if(!cycleCheckMap.containsKey(node)) {

			if(node instanceof Condition) {
				Condition condition = (Condition) node;
				//condition has empty postSet => there is no cycle
				if(condition.getPostEvents().isEmpty()) {
					//put condition which is not in check and has no cycle in cycleCheckMap (cycleCheckMap.put(condition, false))
					cycleCheckMap.put(condition, false);
					return false;
				} else {
					//Node is in check now
					cycleCheckMap.put(condition, true);
					for(Event postEvent : condition.getPostEvents()) {
						if(!cycleCheckMap.containsKey(postEvent)) {
							//recursive call of findCycle
							if(findCycle(postEvent)) return true;
							else {
								cycleCheckMap.put(postEvent, false);
							}
						} else {
							if(cycleCheckMap.get(postEvent)) {
								return true;
							} 						
						}
					}
					return false;
				}
			} else
			if(node instanceof Event) {
				Event event = (Event) node;
				//event has empty postSet => there is no cycle
				if(event.getPostConditions().isEmpty()) {
					cycleCheckMap.put(event, false);
					return false;
				} else {
					//Node is in check now
					cycleCheckMap.put(event, true);
					for(Condition postCondition : event.getPostConditions()) {
						if(!cycleCheckMap.containsKey(postCondition)) {
							//recursive call of findCycle
							if(findCycle(postCondition)) return true;
							else {
								cycleCheckMap.put(postCondition, false);
							}
						} else {
							if(cycleCheckMap.get(postCondition)) {
								return true;
							} 						
						}
					}
					return false;
				}
			}
		} else {
			//is in cycleCheckMap && isNoCycle = false => no cycle
			if(!cycleCheckMap.get(node)) {
				System.out.println("Node " + node.getName() + " is not in a cycle.");
				return false;
			}
			//is in cycleCheckMap && isNoCycle = true => cycle
			if(cycleCheckMap.get(node)) {
				System.out.println("Node " + node.getName() + " is in cycle.");
				return true;
			}
		}
		
		return false;
	}
	
}
