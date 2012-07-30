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

import hub.top.adaptiveSystem.AdaptiveSystem;
import hub.top.adaptiveSystem.AdaptiveSystemPackage;
import hub.top.adaptiveSystem.Condition;
import hub.top.adaptiveSystem.DoNet;
import hub.top.adaptiveSystem.Event;
import hub.top.adaptiveSystem.Node;
import hub.top.adaptiveSystem.Oclet;
import hub.top.adaptiveSystem.PreNet;
import hub.top.adaptiveSystem.diagram.part.AdaptiveSystemDiagramEditor;
import hub.top.greta.cpn.AdaptiveSystemToCPN;
import hub.top.greta.run.Activator;
import hub.top.greta.validation.ModelError;

import java.util.HashSet;
import java.util.LinkedList;
import java.util.List;
import java.util.Set;

import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.commands.ExecutionException;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.jobs.Job;
import org.eclipse.emf.common.command.Command;
import org.eclipse.emf.common.command.CommandStack;
import org.eclipse.emf.common.command.CompoundCommand;
import org.eclipse.emf.common.util.BasicEList;
import org.eclipse.emf.common.util.BasicEMap;
import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.common.util.EMap;
import org.eclipse.emf.edit.command.SetCommand;
import org.eclipse.emf.edit.domain.EditingDomain;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.IWorkbenchPart;


public class CheckOclets extends SelectionAwareCommandHandler {
	
	public static String ID = "hub.top.GRETA.run.commands.checkOclets";
	
	@Override
	protected String getID() {
		return ID;
	}

	protected AdaptiveSystemDiagramEditor adaptiveSystemDiagramEditor;
	protected AdaptiveSystem adaptiveSystem;
	
	protected EMap<Node, Boolean> cycleCheckMap = new BasicEMap<Node, Boolean>();
		
	@Override
	public Object execute(ExecutionEvent event) throws ExecutionException {

		if(getWorkbenchWindow().getActivePage().getActiveEditor() instanceof AdaptiveSystemDiagramEditor) {
			//System.out.println("Check wellformedness of oclets : START run() ");
			adaptiveSystemDiagramEditor = (AdaptiveSystemDiagramEditor) getWorkbenchWindow().getActivePage().getActiveEditor();
			adaptiveSystem = (AdaptiveSystem) adaptiveSystemDiagramEditor.getDiagram().getElement();
			EList<Oclet> ocletList = adaptiveSystem.getOclets();
					
			List<ModelError> errors = new LinkedList<ModelError>();
	    
			//if(!adaptiveSystem.isSetWellformednessToOclets())
			{
				// create a list of commands and add commands with values of attribute wellformed to this list
				EList<Command> cmdList = new BasicEList<Command>();
				
				boolean wellFormedSpec = true;
				
				for(Oclet oclet : ocletList) {
					//check only the oclets which are not wellformed
					if(!oclet.isWellFormed()) {
						System.out.println("Check wellformedness of oclet : " + oclet.getName());
						
						boolean isWellFormed = checkWellformedness(oclet, errors);
						SetCommand cmd = new SetCommand(
								adaptiveSystemDiagramEditor.getEditingDomain(), oclet, AdaptiveSystemPackage.eINSTANCE.getOclet_WellFormed(), isWellFormed);
						cmd.setLabel("set oclet attribute " + AdaptiveSystemPackage.eINSTANCE.getOclet_WellFormed().getName());
						cmd.canExecute();
						cmdList.add(cmd);
						
						wellFormedSpec &= isWellFormed;
					}
				}
				
        ActionHelper.showMarkers(errors, adaptiveSystemDiagramEditor);
	        
				if (wellFormedSpec) {
				  if (AdaptiveSystemToCPN.isHighLevelSpecification(adaptiveSystem)) {
				    
				    final AdaptiveSystemDiagramEditor ed = adaptiveSystemDiagramEditor;
				    
				    //final Shell jobShell = new Shell(getWorkbenchWindow().getShell());
				    Job checkJob = new Job("Checking Data in Scenarios") {
				      @Override
				      protected IStatus run(IProgressMonitor monitor) {
		            AdaptiveSystemToCPN a2c = null;
		            try {
		              a2c = new AdaptiveSystemToCPN(adaptiveSystem);
		              // now read from the underlying .cpn file
		              //a2c.loadFunctionDefinitions(adaptiveSystem);
		              a2c.loadPlaceTypes(adaptiveSystem);
		              a2c.declareVariables(adaptiveSystem);
		              a2c.buildDependencies(adaptiveSystem);
		              
		              a2c.convertInitialRunToMarking(adaptiveSystem);
		              a2c.convertEventsToTransitions(adaptiveSystem);
		              a2c.check();
		              a2c.exportNet();
		              
                  a2c.destroy();
		              
		              ActionHelper.showMarkers(a2c.errors, ed);

		              // finally invalidate all erroneous oclets
		              EList<Command> cmdList = new BasicEList<Command>();
		              
		              Set<Oclet> errorOclets = new HashSet<Oclet>();
		              for (ModelError e : a2c.errors) {
		                if (e.modelObject instanceof Oclet) errorOclets.add((Oclet)e);
		                if (e.modelObject instanceof Node) errorOclets.add((Oclet)e.modelObject.eContainer().eContainer());
		              }
		              
		              for (Oclet oclet : errorOclets) {
  		              SetCommand cmd = new SetCommand(
  		                  adaptiveSystemDiagramEditor.getEditingDomain(), oclet, AdaptiveSystemPackage.eINSTANCE.getOclet_WellFormed(), false);
  		              cmd.setLabel("set oclet attribute " + AdaptiveSystemPackage.eINSTANCE.getOclet_WellFormed().getName());
  		              cmd.canExecute();
  		              cmdList.add(cmd);
		              }
		              

		              //set wellformednessOfOclets to true
		              SetCommand cmd = new SetCommand(
		                  adaptiveSystemDiagramEditor.getEditingDomain(), adaptiveSystem, AdaptiveSystemPackage.eINSTANCE.getAdaptiveSystem_SetWellformednessToOclets(), true);
		              cmd.setLabel("set adaptive process attribute " + AdaptiveSystemPackage.eINSTANCE.getAdaptiveSystem_SetWellformednessToOclets().getName());
		              cmd.canExecute();
		              cmdList.add(cmd);
		              
		              CompoundCommand fireCmd = new CompoundCommand(cmdList);
		              fireCmd.setLabel("Checking specification.");
		              fireCmd.setDescription("Checking specification.");
		              // and execute it in a transactional editing domain (for undo/redo)
		              fireCmd.canExecute();
		              ((CommandStack) ((EditingDomain) adaptiveSystemDiagramEditor.getEditingDomain()).getCommandStack()).execute(fireCmd);
		              
		            } catch (Exception e) {
		              Activator.getPluginHelper().showErrorToUser("Checking Data in Scenarios", "Failed to check data in scenarios:\n"+e.getMessage(), e);
		            }
		            return Status.OK_STATUS;
				      }
				    };
				    checkJob.setUser(true);
				    // FIXME: lock resource of the file only
				    checkJob.setRule(ResourcesPlugin.getWorkspace().getRoot());
				    checkJob.schedule();
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
        
        //action.setEnabled(false);

			} 
		}
		/*
		  else {
			action.setEnabled(false);
		}*/
		
		return null;
	}

	
	@Override
	public void selectionChanged(IWorkbenchPart part, ISelection selection) {
		if(getWorkbenchWindow().getActivePage().getActiveEditor() instanceof AdaptiveSystemDiagramEditor
				&& !this.isEnabled())
		{
			this.setBaseEnabled(true);
		} else {
			this.setBaseEnabled(false);
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
	 * @param errors
	 * @return
	 */
	private boolean checkWellformedness(Oclet oclet, List<ModelError> errors) {

	  int oldErrors = errors.size();
	  
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
			  errors.add(new ModelError(oclet, oclet.getName(), "'"+oclet.getName()+"' contains a cycle"));
			}
		}
		//8. in oclet is no cycle 
		//if number of checked nodes is smaller as the number of oclet nodes 
		//than there are not minimal nodes i can't reach from minimal nodes hence they are in a cycle.
		if(cycleCheckMap.size() != oclet.getPreNet().getNodes().size() + oclet.getDoNet().getNodes().size()) {
      errors.add(new ModelError(oclet, oclet.getName(), "'"+oclet.getName()+"' contains a cycle"));
		}
		cycleCheckMap.clear();

		return oldErrors == errors.size();
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
