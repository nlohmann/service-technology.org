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

import hub.top.adaptiveProcess.diagram.part.AdaptiveProcessDiagramViewer;
import hub.top.adaptiveSystem.AdaptiveProcess;
import hub.top.adaptiveSystem.Node;
import hub.top.adaptiveSystem.diagram.part.AdaptiveSystemDiagramEditor;

import java.util.LinkedList;
import java.util.List;

import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.emf.common.command.Command;
import org.eclipse.emf.common.util.BasicEMap;
import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.common.util.EMap;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.transaction.RecordingCommand;
import org.eclipse.emf.transaction.TransactionalEditingDomain;
import org.eclipse.gef.ConnectionEditPart;
import org.eclipse.gef.EditPart;
import org.eclipse.gef.GraphicalEditPart;
import org.eclipse.gmf.runtime.diagram.ui.actions.ActionIds;
import org.eclipse.gmf.runtime.diagram.ui.commands.ICommandProxy;
import org.eclipse.gmf.runtime.diagram.ui.commands.SetBoundsCommand;
import org.eclipse.gmf.runtime.diagram.ui.l10n.DiagramUIMessages;
import org.eclipse.gmf.runtime.diagram.ui.requests.ArrangeRequest;
import org.eclipse.gmf.runtime.notation.Bounds;

/**
 * This class provides shared core functionality for the simulation view of
 * the {@link AdaptiveSystemDiagramEditor} and {@link AdaptiveProcessDiagramViewer}.
 * 
 * @author Dirk Fahland
 */
public class SimulationHelper {
  
  
  /**
   * Arrange all elements in a graphical representation of an {@link AdaptiveProcess}.
   * 
   * @param apEditPart
   * @param nodesToArrange 
   * 				A list of Nodes. Only diagram nodes that have one of these
   *        nodes as model object will be arranged. The list can be
   *        <code>null</code>; then all nodes of the diagram will be arranged.
   *        
   * @author Manja Wolf
   *       Dirk Fahland
   * 
   */
  @SuppressWarnings("unchecked")
  public static void arrangeAllAdaptiveProcess(TransactionalEditingDomain editingDomain, EditPart apEditPart, final EList<Node> nodesToArrange) {
  	
    // determine the EditPart in which arranging shall take place
    // - either in the AdaptiveProcessEditPart of the editor
    // - or in the separate Process Viewer
  	GraphicalEditPart	apCanvasEditPart = null;
  	if (apEditPart instanceof hub.top.adaptiveSystem.diagram.edit.parts.AdaptiveProcessEditPart)
  		// the AdaptiveProcessEditPart has its canvas inside a compartment
  		apCanvasEditPart = (GraphicalEditPart) apEditPart.getChildren().get(0);
  	else if (apEditPart instanceof hub.top.adaptiveProcess.diagram.edit.parts.AP_ProcessEditPart) {
  		// the AP_ProcessEditPart IS the canvas
  		apCanvasEditPart = (GraphicalEditPart) apEditPart;
  	}
  
  	List<EditPart> editPartsToArrange = null;
  	ArrangeRequest request = null;
  	// arrange ALL nodes in the canvas
  	if (nodesToArrange == null) {
  		request = new ArrangeRequest(ActionIds.ACTION_ARRANGE_ALL);
  		// arrange all nodes on the current canvas
  		editPartsToArrange = (List<EditPart>)apCanvasEditPart.getChildren();
  		
  	// arrange only the SELECTED nodes in the canvas
  	} else {
      request =  new ArrangeRequest(ActionIds.ACTION_ARRANGE_SELECTION);
  
  		// find the corresponding editpart for each of the given nodes
  		// on the current canvas
      EMap<EObject, EditPart> nodeEPmapping = getEditPartsForObjects(nodesToArrange, apCanvasEditPart);
  		editPartsToArrange = new LinkedList<EditPart>(nodeEPmapping.values());
  		
  	  /*
      Command cmd2 = SimulationHelper.createCommand_ArrangeNodes(editingDomain, nodesToArrange, nodeEPmapping);
      cmd2.canExecute();
      editingDomain.getCommandStack().execute(cmd2);
      */
  	}
  	request.setPartsToArrange(editPartsToArrange);		
  	org.eclipse.gef.commands.Command cmd = apCanvasEditPart.getCommand(request);
  	try {
  		cmd.canExecute();
  		cmd.execute();
  	} catch (Exception e) {
  		
  	}
  }
  
  /**
   * @param objects
   * @param canvasEditPart
   * @return
   *  a mapping that associates each model object with its {@link EditPart} on the canvasEditPart
   */
  @SuppressWarnings("unchecked")
  protected static EMap<EObject, EditPart> getEditPartsForObjects(EList objects, GraphicalEditPart canvasEditPart) {
    EMap<EObject, EditPart> map = new BasicEMap<EObject, EditPart>();
    
    // iterate over all childs of the given canvas EditPart
    // to find those that represent an object from the given set
    for (EditPart ep : (List<EditPart>)canvasEditPart.getChildren()) {
      if (ep.getModel() instanceof org.eclipse.gmf.runtime.notation.Node) {
        org.eclipse.gmf.runtime.notation.Node diagramNode =
          (org.eclipse.gmf.runtime.notation.Node)ep.getModel();
        if (objects.contains(diagramNode.getElement())) {
          // remember which node is represented by which editPart
          map.put(diagramNode.getElement(), ep);
        }
      }
    }
    return map;
  }


  
  /**
   * Return a command for automatically arranging the given nodes as a directed
   * acyclic graph. This command is meant for managing the layout of the
   * {@link AdaptiveProcess} during simulation.
   * 
   * @param editingDomain
   * @param nodesToArrange  the nodes that shall be arranged
   * @param nodeEPmapping   a mapping from the nodes to their editParts,
   *                        the editParts contain the graphical information
   *                        this mapping can be obtained with {@link #getEditPartsForObjects(EList, GraphicalEditPart)}
   * @return
   */
  @SuppressWarnings("unused")
  private static Command createCommand_ArrangeNodes(
      TransactionalEditingDomain editingDomain,
      final EList<Node> nodesToArrange,
      final EMap<EObject, EditPart> nodeEPmapping)
  {
    // attempt for manual layout of the given nodesToArrange 
    if (nodesToArrange == null) return null;

    final TransactionalEditingDomain ed = editingDomain;
    RecordingCommand cmd2 = new RecordingCommand(editingDomain, "set nodes") {
    
      @Override
      protected void doExecute() {

        // find the minimal node in the nodes that shall be arranged 
        LinkedList<Node> minimalNodes = new LinkedList<Node>();
        for (Node n : nodesToArrange) {
          boolean preInArrange = false;
          for (Node pre : n.getPreSet()) {
            if (nodesToArrange.contains(pre)) {
              preInArrange = true; break;
            }
          }
          if (!preInArrange) {
            minimalNodes.add(n);
          }
        }

        for (Node minimalNode : minimalNodes)
        {
          
          GraphicalEditPart ep = (GraphicalEditPart)nodeEPmapping.get(minimalNode);
          if (ep != null 
            && ep.getTargetConnections().size() > 0
            && ep.getTargetConnections().get(0) instanceof ConnectionEditPart)
          {
            ConnectionEditPart cep = (ConnectionEditPart)ep.getTargetConnections().get(0);
            if (cep.getSource() instanceof GraphicalEditPart) {
              GraphicalEditPart predEP = (GraphicalEditPart)cep.getSource();
              Point predLocation = predEP.getFigure().getBounds().getLocation();
              Point newLocation = new Point(predLocation.x, predLocation.y+50);
              
              SetBoundsCommand cmd = new SetBoundsCommand(ed,
                  DiagramUIMessages.ArrangeAction_toolbar_ArrangeSelection_ActionLabelText,
                  ep,
                  newLocation);
              
              ep.getViewer().getEditDomain().getCommandStack()
                .execute(new ICommandProxy(cmd));

              Dimension move = newLocation.getDifference(ep.getFigure().getBounds().getLocation());
              System.out.println("delta :"+move);

              for (Node succ : minimalNode.getPostSet()) {
                EditPart epSucc = nodeEPmapping.get(succ);
                if (epSucc != null && epSucc instanceof GraphicalEditPart) {
                  GraphicalEditPart toMove = (GraphicalEditPart)epSucc;
                  Point sourceSucc = toMove.getFigure().getBounds().getLocation();
                  
                  if (sourceSucc.x == 0 && sourceSucc.y == 0 &&
                    toMove.getModel() instanceof org.eclipse.gmf.runtime.notation.Node)
                  {
                    org.eclipse.gmf.runtime.notation.Node graphicalNode =
                      (org.eclipse.gmf.runtime.notation.Node)toMove.getModel();
                    if (graphicalNode.getLayoutConstraint() instanceof Bounds) {
                      sourceSucc = new Point(
                          ((Bounds)graphicalNode.getLayoutConstraint()).getX(),
                          ((Bounds)graphicalNode.getLayoutConstraint()).getY());
                    }
                  }
                  
                  Point targetSucc = new Point(sourceSucc.x + move.width, sourceSucc.y + move.height);
                  
                  SetBoundsCommand cmd2 = new SetBoundsCommand(ed,
                      DiagramUIMessages.ArrangeAction_toolbar_ArrangeSelection_ActionLabelText,
                      toMove,
                      targetSucc);
                  
                  System.out.println(succ.getName() + ": " + sourceSucc + " -> " + targetSucc);

                  
                  toMove.getViewer().getEditDomain().getCommandStack()
                    .execute(new ICommandProxy(cmd2));
                }
              }
            }
          }
        }
      }
    };
    return cmd2;
  }
  

}
