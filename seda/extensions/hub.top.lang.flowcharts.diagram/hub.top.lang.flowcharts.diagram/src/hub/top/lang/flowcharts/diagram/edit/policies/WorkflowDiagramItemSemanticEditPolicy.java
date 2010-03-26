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

import hub.top.lang.flowcharts.FlowPackage;
import hub.top.lang.flowcharts.diagram.edit.commands.DocumentCreateCommand;
import hub.top.lang.flowcharts.diagram.edit.commands.EndnodeCreateCommand;
import hub.top.lang.flowcharts.diagram.edit.commands.EventCreateCommand;
import hub.top.lang.flowcharts.diagram.edit.commands.FlowCreateCommand;
import hub.top.lang.flowcharts.diagram.edit.commands.MergeNodeCreateCommand;
import hub.top.lang.flowcharts.diagram.edit.commands.PersistentResourceCreateCommand;
import hub.top.lang.flowcharts.diagram.edit.commands.SimpleActivityCreateCommand;
import hub.top.lang.flowcharts.diagram.edit.commands.SplitNodeCreateCommand;
import hub.top.lang.flowcharts.diagram.edit.commands.StartNodeCreateCommand;
import hub.top.lang.flowcharts.diagram.edit.commands.SubprocessCreateCommand;
import hub.top.lang.flowcharts.diagram.edit.commands.TransientResourceCreateCommand;
import hub.top.lang.flowcharts.diagram.providers.FlowchartElementTypes;

import org.eclipse.emf.transaction.TransactionalEditingDomain;
import org.eclipse.gef.commands.Command;
import org.eclipse.gmf.runtime.diagram.ui.editparts.IGraphicalEditPart;
import org.eclipse.gmf.runtime.emf.commands.core.commands.DuplicateEObjectsCommand;
import org.eclipse.gmf.runtime.emf.type.core.requests.CreateElementRequest;
import org.eclipse.gmf.runtime.emf.type.core.requests.DuplicateElementsRequest;

/**
 * @generated
 */
public class WorkflowDiagramItemSemanticEditPolicy extends
    FlowchartBaseItemSemanticEditPolicy {

  /**
   * @generated
   */
  protected Command getCreateCommand(CreateElementRequest req) {
    if (FlowchartElementTypes.StartNode_1001 == req.getElementType()) {
      if (req.getContainmentFeature() == null) {
        req.setContainmentFeature(FlowPackage.eINSTANCE
            .getWorkflowDiagram_DiagramNodes());
      }
      return getGEFWrapper(new StartNodeCreateCommand(req));
    }
    if (FlowchartElementTypes.Endnode_1002 == req.getElementType()) {
      if (req.getContainmentFeature() == null) {
        req.setContainmentFeature(FlowPackage.eINSTANCE
            .getWorkflowDiagram_DiagramNodes());
      }
      return getGEFWrapper(new EndnodeCreateCommand(req));
    }
    if (FlowchartElementTypes.SimpleActivity_1003 == req.getElementType()) {
      if (req.getContainmentFeature() == null) {
        req.setContainmentFeature(FlowPackage.eINSTANCE
            .getWorkflowDiagram_DiagramNodes());
      }
      return getGEFWrapper(new SimpleActivityCreateCommand(req));
    }
    if (FlowchartElementTypes.Subprocess_1004 == req.getElementType()) {
      if (req.getContainmentFeature() == null) {
        req.setContainmentFeature(FlowPackage.eINSTANCE
            .getWorkflowDiagram_DiagramNodes());
      }
      return getGEFWrapper(new SubprocessCreateCommand(req));
    }
    if (FlowchartElementTypes.Event_1005 == req.getElementType()) {
      if (req.getContainmentFeature() == null) {
        req.setContainmentFeature(FlowPackage.eINSTANCE
            .getWorkflowDiagram_DiagramNodes());
      }
      return getGEFWrapper(new EventCreateCommand(req));
    }
    if (FlowchartElementTypes.SplitNode_1006 == req.getElementType()) {
      if (req.getContainmentFeature() == null) {
        req.setContainmentFeature(FlowPackage.eINSTANCE
            .getWorkflowDiagram_DiagramNodes());
      }
      return getGEFWrapper(new SplitNodeCreateCommand(req));
    }
    if (FlowchartElementTypes.MergeNode_1007 == req.getElementType()) {
      if (req.getContainmentFeature() == null) {
        req.setContainmentFeature(FlowPackage.eINSTANCE
            .getWorkflowDiagram_DiagramNodes());
      }
      return getGEFWrapper(new MergeNodeCreateCommand(req));
    }
    if (FlowchartElementTypes.Flow_1008 == req.getElementType()) {
      if (req.getContainmentFeature() == null) {
        req.setContainmentFeature(FlowPackage.eINSTANCE
            .getWorkflowDiagram_DiagramNodes());
      }
      return getGEFWrapper(new FlowCreateCommand(req));
    }
    if (FlowchartElementTypes.TransientResource_1009 == req.getElementType()) {
      if (req.getContainmentFeature() == null) {
        req.setContainmentFeature(FlowPackage.eINSTANCE
            .getWorkflowDiagram_DiagramNodes());
      }
      return getGEFWrapper(new TransientResourceCreateCommand(req));
    }
    if (FlowchartElementTypes.Document_1010 == req.getElementType()) {
      if (req.getContainmentFeature() == null) {
        req.setContainmentFeature(FlowPackage.eINSTANCE
            .getWorkflowDiagram_DiagramNodes());
      }
      return getGEFWrapper(new DocumentCreateCommand(req));
    }
    if (FlowchartElementTypes.PersistentResource_1011 == req.getElementType()) {
      if (req.getContainmentFeature() == null) {
        req.setContainmentFeature(FlowPackage.eINSTANCE
            .getWorkflowDiagram_DiagramNodes());
      }
      return getGEFWrapper(new PersistentResourceCreateCommand(req));
    }
    return super.getCreateCommand(req);
  }

  /**
   * @generated
   */
  protected Command getDuplicateCommand(DuplicateElementsRequest req) {
    TransactionalEditingDomain editingDomain = ((IGraphicalEditPart) getHost())
        .getEditingDomain();
    return getGEFWrapper(new DuplicateAnythingCommand(editingDomain, req));
  }

  /**
   * @generated
   */
  private static class DuplicateAnythingCommand extends
      DuplicateEObjectsCommand {

    /**
     * @generated
     */
    public DuplicateAnythingCommand(TransactionalEditingDomain editingDomain,
        DuplicateElementsRequest req) {
      super(editingDomain, req.getLabel(), req.getElementsToBeDuplicated(), req
          .getAllDuplicatedElementsMap());
    }

  }

}
