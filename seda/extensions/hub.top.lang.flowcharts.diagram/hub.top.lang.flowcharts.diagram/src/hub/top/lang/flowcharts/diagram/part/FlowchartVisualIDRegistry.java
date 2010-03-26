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
package hub.top.lang.flowcharts.diagram.part;

import hub.top.lang.flowcharts.FlowPackage;
import hub.top.lang.flowcharts.WorkflowDiagram;
import hub.top.lang.flowcharts.diagram.edit.parts.DiagramArcEditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.DiagramArcLabelEditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.Document2EditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.DocumentEditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.DocumentLabel2EditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.DocumentLabelEditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.Endnode2EditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.EndnodeEditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.EndnodeLabel2EditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.EndnodeLabelEditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.Event2EditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.EventEditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.Flow2EditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.FlowEditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.FlowFlowCompartment2EditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.FlowFlowCompartmentEditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.MergeNode2EditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.MergeNodeEditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.PersistentResource2EditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.PersistentResourceEditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.PersistentResourceLabel2EditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.PersistentResourceLabelEditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.SimpleActivity2EditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.SimpleActivityEditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.SimpleActivityLabel2EditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.SimpleActivityLabelEditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.SplitNode2EditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.SplitNodeEditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.SplitNodeLabel2EditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.SplitNodeLabelEditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.StartNode2EditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.StartNodeEditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.StartNodeLabel2EditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.StartNodeLabelEditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.Subprocess2EditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.SubprocessEditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.SubprocessLabel2EditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.SubprocessLabelEditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.TransientResource2EditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.TransientResourceEditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.TransientResourceLabel2EditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.TransientResourceLabelEditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.WorkflowDiagramEditPart;

import org.eclipse.core.runtime.Platform;
import org.eclipse.emf.ecore.EAnnotation;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.gmf.runtime.notation.Diagram;
import org.eclipse.gmf.runtime.notation.View;

/**
 * This registry is used to determine which type of visual object should be
 * created for the corresponding Diagram, Node, ChildNode or Link represented
 * by a domain model object.
 * 
 * @generated
 */
public class FlowchartVisualIDRegistry {

  /**
   * @generated
   */
  private static final String DEBUG_KEY = "hub.top.lang.flowcharts.diagram/debug/visualID"; //$NON-NLS-1$

  /**
   * @generated
   */
  public static int getVisualID(View view) {
    if (view instanceof Diagram) {
      if (WorkflowDiagramEditPart.MODEL_ID.equals(view.getType())) {
        return WorkflowDiagramEditPart.VISUAL_ID;
      } else {
        return -1;
      }
    }
    return hub.top.lang.flowcharts.diagram.part.FlowchartVisualIDRegistry
        .getVisualID(view.getType());
  }

  /**
   * @generated
   */
  public static String getModelID(View view) {
    View diagram = view.getDiagram();
    while (view != diagram) {
      EAnnotation annotation = view.getEAnnotation("Shortcut"); //$NON-NLS-1$
      if (annotation != null) {
        return (String) annotation.getDetails().get("modelID"); //$NON-NLS-1$
      }
      view = (View) view.eContainer();
    }
    return diagram != null ? diagram.getType() : null;
  }

  /**
   * @generated
   */
  public static int getVisualID(String type) {
    try {
      return Integer.parseInt(type);
    } catch (NumberFormatException e) {
      if (Boolean.TRUE.toString().equalsIgnoreCase(
          Platform.getDebugOption(DEBUG_KEY))) {
        FlowchartDiagramEditorPlugin.getInstance().logError(
            "Unable to parse view type as a visualID number: " + type);
      }
    }
    return -1;
  }

  /**
   * @generated
   */
  public static String getType(int visualID) {
    return String.valueOf(visualID);
  }

  /**
   * @generated
   */
  public static int getDiagramVisualID(EObject domainElement) {
    if (domainElement == null) {
      return -1;
    }
    if (FlowPackage.eINSTANCE.getWorkflowDiagram().isSuperTypeOf(
        domainElement.eClass())
        && isDiagram((WorkflowDiagram) domainElement)) {
      return WorkflowDiagramEditPart.VISUAL_ID;
    }
    return -1;
  }

  /**
   * @generated
   */
  public static int getNodeVisualID(View containerView, EObject domainElement) {
    if (domainElement == null) {
      return -1;
    }
    String containerModelID = hub.top.lang.flowcharts.diagram.part.FlowchartVisualIDRegistry
        .getModelID(containerView);
    if (!WorkflowDiagramEditPart.MODEL_ID.equals(containerModelID)) {
      return -1;
    }
    int containerVisualID;
    if (WorkflowDiagramEditPart.MODEL_ID.equals(containerModelID)) {
      containerVisualID = hub.top.lang.flowcharts.diagram.part.FlowchartVisualIDRegistry
          .getVisualID(containerView);
    } else {
      if (containerView instanceof Diagram) {
        containerVisualID = WorkflowDiagramEditPart.VISUAL_ID;
      } else {
        return -1;
      }
    }
    switch (containerVisualID) {
    case FlowFlowCompartmentEditPart.VISUAL_ID:
      if (FlowPackage.eINSTANCE.getStartNode().isSuperTypeOf(
          domainElement.eClass())) {
        return StartNode2EditPart.VISUAL_ID;
      }
      if (FlowPackage.eINSTANCE.getEndnode().isSuperTypeOf(
          domainElement.eClass())) {
        return Endnode2EditPart.VISUAL_ID;
      }
      if (FlowPackage.eINSTANCE.getSimpleActivity().isSuperTypeOf(
          domainElement.eClass())) {
        return SimpleActivity2EditPart.VISUAL_ID;
      }
      if (FlowPackage.eINSTANCE.getSubprocess().isSuperTypeOf(
          domainElement.eClass())) {
        return Subprocess2EditPart.VISUAL_ID;
      }
      if (FlowPackage.eINSTANCE.getEvent()
          .isSuperTypeOf(domainElement.eClass())) {
        return Event2EditPart.VISUAL_ID;
      }
      if (FlowPackage.eINSTANCE.getSplitNode().isSuperTypeOf(
          domainElement.eClass())) {
        return SplitNode2EditPart.VISUAL_ID;
      }
      if (FlowPackage.eINSTANCE.getMergeNode().isSuperTypeOf(
          domainElement.eClass())) {
        return MergeNode2EditPart.VISUAL_ID;
      }
      if (FlowPackage.eINSTANCE.getFlow().isSuperTypeOf(domainElement.eClass())) {
        return Flow2EditPart.VISUAL_ID;
      }
      if (FlowPackage.eINSTANCE.getTransientResource().isSuperTypeOf(
          domainElement.eClass())) {
        return TransientResource2EditPart.VISUAL_ID;
      }
      if (FlowPackage.eINSTANCE.getDocument().isSuperTypeOf(
          domainElement.eClass())) {
        return Document2EditPart.VISUAL_ID;
      }
      if (FlowPackage.eINSTANCE.getPersistentResource().isSuperTypeOf(
          domainElement.eClass())) {
        return PersistentResource2EditPart.VISUAL_ID;
      }
      break;
    case FlowFlowCompartment2EditPart.VISUAL_ID:
      if (FlowPackage.eINSTANCE.getStartNode().isSuperTypeOf(
          domainElement.eClass())) {
        return StartNode2EditPart.VISUAL_ID;
      }
      if (FlowPackage.eINSTANCE.getEndnode().isSuperTypeOf(
          domainElement.eClass())) {
        return Endnode2EditPart.VISUAL_ID;
      }
      if (FlowPackage.eINSTANCE.getSimpleActivity().isSuperTypeOf(
          domainElement.eClass())) {
        return SimpleActivity2EditPart.VISUAL_ID;
      }
      if (FlowPackage.eINSTANCE.getSubprocess().isSuperTypeOf(
          domainElement.eClass())) {
        return Subprocess2EditPart.VISUAL_ID;
      }
      if (FlowPackage.eINSTANCE.getEvent()
          .isSuperTypeOf(domainElement.eClass())) {
        return Event2EditPart.VISUAL_ID;
      }
      if (FlowPackage.eINSTANCE.getSplitNode().isSuperTypeOf(
          domainElement.eClass())) {
        return SplitNode2EditPart.VISUAL_ID;
      }
      if (FlowPackage.eINSTANCE.getMergeNode().isSuperTypeOf(
          domainElement.eClass())) {
        return MergeNode2EditPart.VISUAL_ID;
      }
      if (FlowPackage.eINSTANCE.getFlow().isSuperTypeOf(domainElement.eClass())) {
        return Flow2EditPart.VISUAL_ID;
      }
      if (FlowPackage.eINSTANCE.getTransientResource().isSuperTypeOf(
          domainElement.eClass())) {
        return TransientResource2EditPart.VISUAL_ID;
      }
      if (FlowPackage.eINSTANCE.getDocument().isSuperTypeOf(
          domainElement.eClass())) {
        return Document2EditPart.VISUAL_ID;
      }
      if (FlowPackage.eINSTANCE.getPersistentResource().isSuperTypeOf(
          domainElement.eClass())) {
        return PersistentResource2EditPart.VISUAL_ID;
      }
      break;
    case WorkflowDiagramEditPart.VISUAL_ID:
      if (FlowPackage.eINSTANCE.getStartNode().isSuperTypeOf(
          domainElement.eClass())) {
        return StartNodeEditPart.VISUAL_ID;
      }
      if (FlowPackage.eINSTANCE.getEndnode().isSuperTypeOf(
          domainElement.eClass())) {
        return EndnodeEditPart.VISUAL_ID;
      }
      if (FlowPackage.eINSTANCE.getSimpleActivity().isSuperTypeOf(
          domainElement.eClass())) {
        return SimpleActivityEditPart.VISUAL_ID;
      }
      if (FlowPackage.eINSTANCE.getSubprocess().isSuperTypeOf(
          domainElement.eClass())) {
        return SubprocessEditPart.VISUAL_ID;
      }
      if (FlowPackage.eINSTANCE.getEvent()
          .isSuperTypeOf(domainElement.eClass())) {
        return EventEditPart.VISUAL_ID;
      }
      if (FlowPackage.eINSTANCE.getSplitNode().isSuperTypeOf(
          domainElement.eClass())) {
        return SplitNodeEditPart.VISUAL_ID;
      }
      if (FlowPackage.eINSTANCE.getMergeNode().isSuperTypeOf(
          domainElement.eClass())) {
        return MergeNodeEditPart.VISUAL_ID;
      }
      if (FlowPackage.eINSTANCE.getFlow().isSuperTypeOf(domainElement.eClass())) {
        return FlowEditPart.VISUAL_ID;
      }
      if (FlowPackage.eINSTANCE.getTransientResource().isSuperTypeOf(
          domainElement.eClass())) {
        return TransientResourceEditPart.VISUAL_ID;
      }
      if (FlowPackage.eINSTANCE.getDocument().isSuperTypeOf(
          domainElement.eClass())) {
        return DocumentEditPart.VISUAL_ID;
      }
      if (FlowPackage.eINSTANCE.getPersistentResource().isSuperTypeOf(
          domainElement.eClass())) {
        return PersistentResourceEditPart.VISUAL_ID;
      }
      break;
    }
    return -1;
  }

  /**
   * @generated
   */
  public static boolean canCreateNode(View containerView, int nodeVisualID) {
    String containerModelID = hub.top.lang.flowcharts.diagram.part.FlowchartVisualIDRegistry
        .getModelID(containerView);
    if (!WorkflowDiagramEditPart.MODEL_ID.equals(containerModelID)) {
      return false;
    }
    int containerVisualID;
    if (WorkflowDiagramEditPart.MODEL_ID.equals(containerModelID)) {
      containerVisualID = hub.top.lang.flowcharts.diagram.part.FlowchartVisualIDRegistry
          .getVisualID(containerView);
    } else {
      if (containerView instanceof Diagram) {
        containerVisualID = WorkflowDiagramEditPart.VISUAL_ID;
      } else {
        return false;
      }
    }
    switch (containerVisualID) {
    case StartNodeEditPart.VISUAL_ID:
      if (StartNodeLabelEditPart.VISUAL_ID == nodeVisualID) {
        return true;
      }
      break;
    case EndnodeEditPart.VISUAL_ID:
      if (EndnodeLabelEditPart.VISUAL_ID == nodeVisualID) {
        return true;
      }
      break;
    case SimpleActivityEditPart.VISUAL_ID:
      if (SimpleActivityLabelEditPart.VISUAL_ID == nodeVisualID) {
        return true;
      }
      break;
    case SubprocessEditPart.VISUAL_ID:
      if (SubprocessLabelEditPart.VISUAL_ID == nodeVisualID) {
        return true;
      }
      break;
    case SplitNodeEditPart.VISUAL_ID:
      if (SplitNodeLabelEditPart.VISUAL_ID == nodeVisualID) {
        return true;
      }
      break;
    case FlowEditPart.VISUAL_ID:
      if (FlowFlowCompartmentEditPart.VISUAL_ID == nodeVisualID) {
        return true;
      }
      break;
    case TransientResourceEditPart.VISUAL_ID:
      if (TransientResourceLabelEditPart.VISUAL_ID == nodeVisualID) {
        return true;
      }
      break;
    case DocumentEditPart.VISUAL_ID:
      if (DocumentLabelEditPart.VISUAL_ID == nodeVisualID) {
        return true;
      }
      break;
    case PersistentResourceEditPart.VISUAL_ID:
      if (PersistentResourceLabelEditPart.VISUAL_ID == nodeVisualID) {
        return true;
      }
      break;
    case StartNode2EditPart.VISUAL_ID:
      if (StartNodeLabel2EditPart.VISUAL_ID == nodeVisualID) {
        return true;
      }
      break;
    case Endnode2EditPart.VISUAL_ID:
      if (EndnodeLabel2EditPart.VISUAL_ID == nodeVisualID) {
        return true;
      }
      break;
    case SimpleActivity2EditPart.VISUAL_ID:
      if (SimpleActivityLabel2EditPart.VISUAL_ID == nodeVisualID) {
        return true;
      }
      break;
    case Subprocess2EditPart.VISUAL_ID:
      if (SubprocessLabel2EditPart.VISUAL_ID == nodeVisualID) {
        return true;
      }
      break;
    case SplitNode2EditPart.VISUAL_ID:
      if (SplitNodeLabel2EditPart.VISUAL_ID == nodeVisualID) {
        return true;
      }
      break;
    case Flow2EditPart.VISUAL_ID:
      if (FlowFlowCompartment2EditPart.VISUAL_ID == nodeVisualID) {
        return true;
      }
      break;
    case TransientResource2EditPart.VISUAL_ID:
      if (TransientResourceLabel2EditPart.VISUAL_ID == nodeVisualID) {
        return true;
      }
      break;
    case Document2EditPart.VISUAL_ID:
      if (DocumentLabel2EditPart.VISUAL_ID == nodeVisualID) {
        return true;
      }
      break;
    case PersistentResource2EditPart.VISUAL_ID:
      if (PersistentResourceLabel2EditPart.VISUAL_ID == nodeVisualID) {
        return true;
      }
      break;
    case FlowFlowCompartmentEditPart.VISUAL_ID:
      if (StartNode2EditPart.VISUAL_ID == nodeVisualID) {
        return true;
      }
      if (Endnode2EditPart.VISUAL_ID == nodeVisualID) {
        return true;
      }
      if (SimpleActivity2EditPart.VISUAL_ID == nodeVisualID) {
        return true;
      }
      if (Subprocess2EditPart.VISUAL_ID == nodeVisualID) {
        return true;
      }
      if (Event2EditPart.VISUAL_ID == nodeVisualID) {
        return true;
      }
      if (SplitNode2EditPart.VISUAL_ID == nodeVisualID) {
        return true;
      }
      if (MergeNode2EditPart.VISUAL_ID == nodeVisualID) {
        return true;
      }
      if (Flow2EditPart.VISUAL_ID == nodeVisualID) {
        return true;
      }
      if (TransientResource2EditPart.VISUAL_ID == nodeVisualID) {
        return true;
      }
      if (Document2EditPart.VISUAL_ID == nodeVisualID) {
        return true;
      }
      if (PersistentResource2EditPart.VISUAL_ID == nodeVisualID) {
        return true;
      }
      break;
    case FlowFlowCompartment2EditPart.VISUAL_ID:
      if (StartNode2EditPart.VISUAL_ID == nodeVisualID) {
        return true;
      }
      if (Endnode2EditPart.VISUAL_ID == nodeVisualID) {
        return true;
      }
      if (SimpleActivity2EditPart.VISUAL_ID == nodeVisualID) {
        return true;
      }
      if (Subprocess2EditPart.VISUAL_ID == nodeVisualID) {
        return true;
      }
      if (Event2EditPart.VISUAL_ID == nodeVisualID) {
        return true;
      }
      if (SplitNode2EditPart.VISUAL_ID == nodeVisualID) {
        return true;
      }
      if (MergeNode2EditPart.VISUAL_ID == nodeVisualID) {
        return true;
      }
      if (Flow2EditPart.VISUAL_ID == nodeVisualID) {
        return true;
      }
      if (TransientResource2EditPart.VISUAL_ID == nodeVisualID) {
        return true;
      }
      if (Document2EditPart.VISUAL_ID == nodeVisualID) {
        return true;
      }
      if (PersistentResource2EditPart.VISUAL_ID == nodeVisualID) {
        return true;
      }
      break;
    case WorkflowDiagramEditPart.VISUAL_ID:
      if (StartNodeEditPart.VISUAL_ID == nodeVisualID) {
        return true;
      }
      if (EndnodeEditPart.VISUAL_ID == nodeVisualID) {
        return true;
      }
      if (SimpleActivityEditPart.VISUAL_ID == nodeVisualID) {
        return true;
      }
      if (SubprocessEditPart.VISUAL_ID == nodeVisualID) {
        return true;
      }
      if (EventEditPart.VISUAL_ID == nodeVisualID) {
        return true;
      }
      if (SplitNodeEditPart.VISUAL_ID == nodeVisualID) {
        return true;
      }
      if (MergeNodeEditPart.VISUAL_ID == nodeVisualID) {
        return true;
      }
      if (FlowEditPart.VISUAL_ID == nodeVisualID) {
        return true;
      }
      if (TransientResourceEditPart.VISUAL_ID == nodeVisualID) {
        return true;
      }
      if (DocumentEditPart.VISUAL_ID == nodeVisualID) {
        return true;
      }
      if (PersistentResourceEditPart.VISUAL_ID == nodeVisualID) {
        return true;
      }
      break;
    case DiagramArcEditPart.VISUAL_ID:
      if (DiagramArcLabelEditPart.VISUAL_ID == nodeVisualID) {
        return true;
      }
      break;
    }
    return false;
  }

  /**
   * @generated
   */
  public static int getLinkWithClassVisualID(EObject domainElement) {
    if (domainElement == null) {
      return -1;
    }
    if (FlowPackage.eINSTANCE.getDiagramArc().isSuperTypeOf(
        domainElement.eClass())) {
      return DiagramArcEditPart.VISUAL_ID;
    }
    return -1;
  }

  /**
   * User can change implementation of this method to handle some specific
   * situations not covered by default logic.
   * 
   * @generated
   */
  private static boolean isDiagram(WorkflowDiagram element) {
    return true;
  }

}
