package hub.top.greta.run.actions;

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
					
			if(!adaptiveSystem.isSetWellformednessToOclets()) {
				// create a list of commands and add commands with values of attribute wellformed to this list
				EList<Command> cmdList = new BasicEList<Command>();
				
				for(Oclet oclet : ocletList) {
					//check only the oclets which are not wellformed
					if(!oclet.isWellFormed()) {
						System.out.println("Check wellformedness of oclet : " + oclet.getName());
						if(checkWellformedness(oclet)) {
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
	 * @param oclet
	 * @return
	 */
	private boolean checkWellformedness(Oclet oclet) {

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
			if(!isWellformed(ocletNode)) return false;
			
			//8. fill the list of minimal nodes for check of cycles
			if(ocletNode instanceof Condition 
					&& ((Condition) ocletNode).getPreEvents().isEmpty()) {
				minimalNodeList.add(ocletNode);
			}
			
		}
		
		//check each DoNet-Nodes
		for (Node ocletNode : oclet.getDoNet().getNodes()) {
			//check 1. - 6. in a separate method isWellformed(node)
			if(!isWellformed(ocletNode)) return false;
		}
		
		//8. in oclet is no cycle allowed
		for(Node node : minimalNodeList) {
			if(findCycle(node)) {
				System.out.println("  not well formed - there is a cycle in oclet.");
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
			System.out.println("  not well formed - there is a cycle in oclet.");
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
	 * 
	 * 
	 * @param node
	 * @return
	 */
	private boolean isWellformed(Node ocletNode) {
		Shell shell = new Shell();
		String ocletName = ((Oclet) ocletNode.eContainer().eContainer()).getName();
		
		//4. abstract nodes in oclet are not allowed
		if (ocletNode.isAbstract()) {
			System.out.println("  not well formed - first occurrence of an abstract node.");
			MessageDialog.openInformation(
				shell,
				"AdaptiveSystem - check wellformedness of oclets",
				"Oclet " + ocletName + " is not used for execution of a step because it is not wellformed. There is at least one abstract node in oclet.");
			return false;
		}
		
		if(ocletNode.eContainer() instanceof PreNet) {
			if (ocletNode instanceof Condition) {
				//2. all maximal conditions have to be marked			
				if(((Condition) ocletNode).isMaximal()) {
					if(!((Condition) ocletNode).isMarked()) {
					System.out.println("  not well formed - condition " + ocletNode.getName() + " is maximal but not marked.");
					MessageDialog.openInformation(
						shell,
						"AdaptiveSystem - check wellformedness of oclets",
						"Oclet " + ocletName + " is not used for execution of a step because it is not wellformed. There is at least one maximal condition without token in preNet.");
					return false;
					}
				}
				//6. a preNet don't have conflicts, that means a condition has maximal one event in postEvents
				if(((Condition) ocletNode).getPostEvents().size() > 1) {
					System.out.println("  not well formed - preNet has at least one (forward-)conflict.");
					MessageDialog.openInformation(
						shell,
						"AdaptiveSystem - check wellformedness of oclets",
						"Oclet " + ocletName + " is not used for execution of a step because it is not wellformed. There is at least one (forward-)conflict  in preNet.");
					return false;
				}
				//6. an preNet don't have conflicts, that means a condition has maximal one event in preEvents
				if(((Condition) ocletNode).getPreEvents().size() > 1) {
					System.out.println("  not well formed - preNet has at least one (backward-)conflict.");
					MessageDialog.openInformation(
						shell,
						"AdaptiveSystem - check wellformedness of oclets",
						"Oclet " + ocletName + " is not used for execution of a step because it is not wellformed. There is at least one (backward-)conflict  in preNet.");
					return false;
				}
			}
			
			if(ocletNode instanceof Event) {
				//1. an events in preNet is not allowed to be minimal (without preSet)
				if(((Event) ocletNode).getPreConditions().isEmpty()) {
					System.out.println("  not well formed - event " + ocletNode.getName() + " is minimal. Only conditions are allowed to be minimal.");
					MessageDialog.openInformation(
						shell,
						"AdaptiveSystem - check wellformedness of oclets",
						"Oclet " + ocletName + " is not used for execution of a step because it is not wellformed. There is at least one minimal event in preNet. Only conditions are allowed to be minimal.");
					return false;					
				}
				//2. maximal events in preNet are not allowed
				if(!((Event) ocletNode).getPostConditions().isEmpty()) {
					for(Condition postCondition : ((Event) ocletNode).getPostConditions()) {
						if(postCondition.eContainer() instanceof DoNet) {
							System.out.println("  not well formed - event " + ocletNode.getName() + " is maximal. Only conditions are allowed to be maximal.");
							MessageDialog.openInformation(
								shell,
								"AdaptiveSystem - check wellformedness of oclets",
								"Oclet " + ocletName + " is not used for execution of a step because it is not wellformed. There is at least one maximal event in preNet. Only conditions are allowed to be maximal.");
							return false;
						}
					}
				}//END if maximal events in preNet
			}//END if ocletNode is event 
		}
		
		if(ocletNode.eContainer() instanceof DoNet) {
			//5. passive nodes in doNet are not allowed
			if(ocletNode.getTemp().equals(Temp.WITHOUT)) {
				System.out.println("  not well formed - first occurrence of an passive node in doNet.");
				MessageDialog.openInformation(
					shell,
					"AdaptiveSystem - check wellformedness of oclets",
					"Oclet " + ocletName + " is not used for execution of a step because it is not wellformed. There is a passive node in doNet.");
				return false;
			}
			
			if(ocletNode instanceof Condition) {
				//3. conditions in doNet are not allowed to be minimal. A minimal condition has no preEvent.
				if(((Condition) ocletNode).getPreEvents().isEmpty()) {
					System.out.println("  not well formed - condition " + ocletNode.getName() + " from doNet is minimal.");
					MessageDialog.openInformation(
						shell,
						"AdaptiveSystem - check wellformedness of oclets",
						"Oclet " + ocletName + " is not used for execution of a step because it is not wellformed. There is at least one minimal condition in doNet.");					
					return false;
				}
				//6. a doNet don't have conflicts, that means a condition has maximal one event in postEvents
				if(((Condition) ocletNode).getPostEvents().size() > 1) {
					System.out.println("  not well formed - preNet has at least one (forward-)conflict.");
					MessageDialog.openInformation(
						shell,
						"AdaptiveSystem - check wellformedness of oclets",
						"Oclet " + ocletName + " is not not used for execution of a step because it is wellformed. There is at least one (forward-)conflict in doNet.");
					return false;
				}
				//6. a doNet don't have conflicts, that means a condition has maximal one event in postEvents
				if(((Condition) ocletNode).getPreEvents().size() > 1) {
					System.out.println("  not well formed - preNet has at least one (backward-)conflict.");
					MessageDialog.openInformation(
						shell,
						"AdaptiveSystem - check wellformedness of oclets",
						"Oclet " + ocletName + " is not not used for execution of a step because it is wellformed. There is at least one (backward-)conflict in doNet.");
					return false;
				}
			}
			if(ocletNode instanceof Event) {
				//3. events in doNet are not allowed to be minimal. A minimal event has no preConditions.
				if(((Event) ocletNode).getPreConditions().isEmpty()) {
					System.out.println("  not well formed - event" + ocletNode.getName() + " from doNet is minimal.");
					MessageDialog.openInformation(
							shell,
							"AdaptiveSystem - check wellformedness of oclets",
							"Oclet " + ocletName + " is not used for execution of a step because it is not wellformed. There is at least one minimal event in doNet.");					
					return false;
				}
			}			

		}
		return true;
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
