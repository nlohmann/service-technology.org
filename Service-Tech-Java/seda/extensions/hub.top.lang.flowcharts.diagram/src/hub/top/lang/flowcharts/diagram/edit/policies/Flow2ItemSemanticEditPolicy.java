/*
 * Copyright (c) 2008 Dirk Fahland. All rights reserved. EPL1.0/GPL3.0/LGPL3.0
 * 
 * ServiceTechnolog.org - FlowChart Editors
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
 * The Original Code is this file as it was released on July 30, 2008.
 * The Initial Developer of the Original Code is
 *  		Dirk Fahland
 *  
 * Portions created by the Initial Developer are Copyright (c) 2008
 * the Initial Developer. All Rights Reserved.
 * 
 * Contributor(s):
 * 
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 3 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 3 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the EPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the EPL, the GPL or the LGPL.
 */
package hub.top.lang.flowcharts.diagram.edit.policies;

import hub.top.lang.flowcharts.diagram.edit.commands.DiagramArcCreateCommand;
import hub.top.lang.flowcharts.diagram.edit.commands.DiagramArcReorientCommand;
import hub.top.lang.flowcharts.diagram.edit.parts.DiagramArcEditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.Document2EditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.Endnode2EditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.Event2EditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.Flow2EditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.FlowFlowCompartment2EditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.MergeNode2EditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.PersistentResource2EditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.SimpleActivity2EditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.SplitNode2EditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.StartNode2EditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.Subprocess2EditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.TransientResource2EditPart;
import hub.top.lang.flowcharts.diagram.part.FlowchartVisualIDRegistry;
import hub.top.lang.flowcharts.diagram.providers.FlowchartElementTypes;

import java.util.Iterator;

import org.eclipse.emf.ecore.EAnnotation;
import org.eclipse.gef.commands.Command;
import org.eclipse.gef.commands.CompoundCommand;
import org.eclipse.gmf.runtime.emf.type.core.commands.DestroyElementCommand;
import org.eclipse.gmf.runtime.emf.type.core.requests.CreateRelationshipRequest;
import org.eclipse.gmf.runtime.emf.type.core.requests.DestroyElementRequest;
import org.eclipse.gmf.runtime.emf.type.core.requests.ReorientRelationshipRequest;
import org.eclipse.gmf.runtime.notation.Node;
import org.eclipse.gmf.runtime.notation.View;

/**
 * @generated
 */
public class Flow2ItemSemanticEditPolicy extends
    FlowchartBaseItemSemanticEditPolicy {

  /**
   * @generated
   */
  protected Command getDestroyElementCommand(DestroyElementRequest req) {
    CompoundCommand cc = getDestroyEdgesCommand();
    addDestroyChildNodesCommand(cc);
    addDestroyShortcutsCommand(cc);
    cc.add(getGEFWrapper(new DestroyElementCommand(req)));
    return cc.unwrap();
  }

  /**
   * @generated
   */
  protected void addDestroyChildNodesCommand(CompoundCommand cmd) {
    View view = (View) getHost().getModel();
    EAnnotation annotation = view.getEAnnotation("Shortcut"); //$NON-NLS-1$
    if (annotation != null) {
      return;
    }
    for (Iterator it = view.getChildren().iterator(); it.hasNext();) {
      Node node = (Node) it.next();
      switch (FlowchartVisualIDRegistry.getVisualID(node)) {
      case FlowFlowCompartment2EditPart.VISUAL_ID:
        for (Iterator cit = node.getChildren().iterator(); cit.hasNext();) {
          Node cnode = (Node) cit.next();
          switch (FlowchartVisualIDRegistry.getVisualID(cnode)) {
          case StartNode2EditPart.VISUAL_ID:
            cmd.add(getDestroyElementCommand(cnode));
            break;
          case Endnode2EditPart.VISUAL_ID:
            cmd.add(getDestroyElementCommand(cnode));
            break;
          case SimpleActivity2EditPart.VISUAL_ID:
            cmd.add(getDestroyElementCommand(cnode));
            break;
          case Subprocess2EditPart.VISUAL_ID:
            cmd.add(getDestroyElementCommand(cnode));
            break;
          case Event2EditPart.VISUAL_ID:
            cmd.add(getDestroyElementCommand(cnode));
            break;
          case SplitNode2EditPart.VISUAL_ID:
            cmd.add(getDestroyElementCommand(cnode));
            break;
          case MergeNode2EditPart.VISUAL_ID:
            cmd.add(getDestroyElementCommand(cnode));
            break;
          case Flow2EditPart.VISUAL_ID:
            cmd.add(getDestroyElementCommand(cnode));
            break;
          case TransientResource2EditPart.VISUAL_ID:
            cmd.add(getDestroyElementCommand(cnode));
            break;
          case Document2EditPart.VISUAL_ID:
            cmd.add(getDestroyElementCommand(cnode));
            break;
          case PersistentResource2EditPart.VISUAL_ID:
            cmd.add(getDestroyElementCommand(cnode));
            break;
          }
        }
        break;
      }
    }
  }

  /**
   * @generated
   */
  protected Command getCreateRelationshipCommand(CreateRelationshipRequest req) {
    Command command = req.getTarget() == null ? getStartCreateRelationshipCommand(req)
        : getCompleteCreateRelationshipCommand(req);
    return command != null ? command : super.getCreateRelationshipCommand(req);
  }

  /**
   * @generated
   */
  protected Command getStartCreateRelationshipCommand(
      CreateRelationshipRequest req) {
    if (FlowchartElementTypes.DiagramArc_4001 == req.getElementType()) {
      return getGEFWrapper(new DiagramArcCreateCommand(req, req.getSource(),
          req.getTarget()));
    }
    return null;
  }

  /**
   * @generated
   */
  protected Command getCompleteCreateRelationshipCommand(
      CreateRelationshipRequest req) {
    if (FlowchartElementTypes.DiagramArc_4001 == req.getElementType()) {
      return getGEFWrapper(new DiagramArcCreateCommand(req, req.getSource(),
          req.getTarget()));
    }
    return null;
  }

  /**
   * Returns command to reorient EClass based link. New link target or source
   * should be the domain model element associated with this node.
   * 
   * @generated
   */
  protected Command getReorientRelationshipCommand(
      ReorientRelationshipRequest req) {
    switch (getVisualID(req)) {
    case DiagramArcEditPart.VISUAL_ID:
      return getGEFWrapper(new DiagramArcReorientCommand(req));
    }
    return super.getReorientRelationshipCommand(req);
  }

}
