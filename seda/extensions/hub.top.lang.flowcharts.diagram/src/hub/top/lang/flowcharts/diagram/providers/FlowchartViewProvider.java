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
package hub.top.lang.flowcharts.diagram.providers;

import hub.top.lang.flowcharts.diagram.edit.parts.DiagramArcEditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.DiagramArcLabelEditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.DiagramArcProbabilityEditPart;
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
import hub.top.lang.flowcharts.diagram.part.FlowchartVisualIDRegistry;
import hub.top.lang.flowcharts.diagram.view.factories.DiagramArcLabelViewFactory;
import hub.top.lang.flowcharts.diagram.view.factories.DiagramArcProbabilityViewFactory;
import hub.top.lang.flowcharts.diagram.view.factories.DiagramArcViewFactory;
import hub.top.lang.flowcharts.diagram.view.factories.Document2ViewFactory;
import hub.top.lang.flowcharts.diagram.view.factories.DocumentLabel2ViewFactory;
import hub.top.lang.flowcharts.diagram.view.factories.DocumentLabelViewFactory;
import hub.top.lang.flowcharts.diagram.view.factories.DocumentViewFactory;
import hub.top.lang.flowcharts.diagram.view.factories.Endnode2ViewFactory;
import hub.top.lang.flowcharts.diagram.view.factories.EndnodeLabel2ViewFactory;
import hub.top.lang.flowcharts.diagram.view.factories.EndnodeLabelViewFactory;
import hub.top.lang.flowcharts.diagram.view.factories.EndnodeViewFactory;
import hub.top.lang.flowcharts.diagram.view.factories.Event2ViewFactory;
import hub.top.lang.flowcharts.diagram.view.factories.EventViewFactory;
import hub.top.lang.flowcharts.diagram.view.factories.Flow2ViewFactory;
import hub.top.lang.flowcharts.diagram.view.factories.FlowFlowCompartment2ViewFactory;
import hub.top.lang.flowcharts.diagram.view.factories.FlowFlowCompartmentViewFactory;
import hub.top.lang.flowcharts.diagram.view.factories.FlowViewFactory;
import hub.top.lang.flowcharts.diagram.view.factories.MergeNode2ViewFactory;
import hub.top.lang.flowcharts.diagram.view.factories.MergeNodeViewFactory;
import hub.top.lang.flowcharts.diagram.view.factories.PersistentResource2ViewFactory;
import hub.top.lang.flowcharts.diagram.view.factories.PersistentResourceLabel2ViewFactory;
import hub.top.lang.flowcharts.diagram.view.factories.PersistentResourceLabelViewFactory;
import hub.top.lang.flowcharts.diagram.view.factories.PersistentResourceViewFactory;
import hub.top.lang.flowcharts.diagram.view.factories.SimpleActivity2ViewFactory;
import hub.top.lang.flowcharts.diagram.view.factories.SimpleActivityLabel2ViewFactory;
import hub.top.lang.flowcharts.diagram.view.factories.SimpleActivityLabelViewFactory;
import hub.top.lang.flowcharts.diagram.view.factories.SimpleActivityViewFactory;
import hub.top.lang.flowcharts.diagram.view.factories.SplitNode2ViewFactory;
import hub.top.lang.flowcharts.diagram.view.factories.SplitNodeLabel2ViewFactory;
import hub.top.lang.flowcharts.diagram.view.factories.SplitNodeLabelViewFactory;
import hub.top.lang.flowcharts.diagram.view.factories.SplitNodeViewFactory;
import hub.top.lang.flowcharts.diagram.view.factories.StartNode2ViewFactory;
import hub.top.lang.flowcharts.diagram.view.factories.StartNodeLabel2ViewFactory;
import hub.top.lang.flowcharts.diagram.view.factories.StartNodeLabelViewFactory;
import hub.top.lang.flowcharts.diagram.view.factories.StartNodeViewFactory;
import hub.top.lang.flowcharts.diagram.view.factories.Subprocess2ViewFactory;
import hub.top.lang.flowcharts.diagram.view.factories.SubprocessLabel2ViewFactory;
import hub.top.lang.flowcharts.diagram.view.factories.SubprocessLabelViewFactory;
import hub.top.lang.flowcharts.diagram.view.factories.SubprocessViewFactory;
import hub.top.lang.flowcharts.diagram.view.factories.TransientResource2ViewFactory;
import hub.top.lang.flowcharts.diagram.view.factories.TransientResourceLabel2ViewFactory;
import hub.top.lang.flowcharts.diagram.view.factories.TransientResourceLabelViewFactory;
import hub.top.lang.flowcharts.diagram.view.factories.TransientResourceViewFactory;
import hub.top.lang.flowcharts.diagram.view.factories.WorkflowDiagramViewFactory;

import org.eclipse.core.runtime.IAdaptable;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.gmf.runtime.diagram.core.providers.AbstractViewProvider;
import org.eclipse.gmf.runtime.emf.type.core.IElementType;
import org.eclipse.gmf.runtime.emf.type.core.IHintedType;
import org.eclipse.gmf.runtime.notation.View;

/**
 * @generated
 */
public class FlowchartViewProvider extends AbstractViewProvider {

  /**
   * @generated
   */
  protected Class getDiagramViewClass(IAdaptable semanticAdapter,
      String diagramKind) {
    EObject semanticElement = getSemanticElement(semanticAdapter);
    if (WorkflowDiagramEditPart.MODEL_ID.equals(diagramKind)
        && FlowchartVisualIDRegistry.getDiagramVisualID(semanticElement) != -1) {
      return WorkflowDiagramViewFactory.class;
    }
    return null;
  }

  /**
   * @generated
   */
  protected Class getNodeViewClass(IAdaptable semanticAdapter,
      View containerView, String semanticHint) {
    if (containerView == null) {
      return null;
    }
    IElementType elementType = getSemanticElementType(semanticAdapter);
    EObject domainElement = getSemanticElement(semanticAdapter);
    int visualID;
    if (semanticHint == null) {
      // Semantic hint is not specified. Can be a result of call from CanonicalEditPolicy.
      // In this situation there should be NO elementType, visualID will be determined
      // by VisualIDRegistry.getNodeVisualID() for domainElement.
      if (elementType != null || domainElement == null) {
        return null;
      }
      visualID = FlowchartVisualIDRegistry.getNodeVisualID(containerView,
          domainElement);
    } else {
      visualID = FlowchartVisualIDRegistry.getVisualID(semanticHint);
      if (elementType != null) {
        // Semantic hint is specified together with element type.
        // Both parameters should describe exactly the same diagram element.
        // In addition we check that visualID returned by VisualIDRegistry.getNodeVisualID() for
        // domainElement (if specified) is the same as in element type.
        if (!FlowchartElementTypes.isKnownElementType(elementType)
            || (!(elementType instanceof IHintedType))) {
          return null; // foreign element type
        }
        String elementTypeHint = ((IHintedType) elementType).getSemanticHint();
        if (!semanticHint.equals(elementTypeHint)) {
          return null; // if semantic hint is specified it should be the same as in element type
        }
        if (domainElement != null
            && visualID != FlowchartVisualIDRegistry.getNodeVisualID(
                containerView, domainElement)) {
          return null; // visual id for node EClass should match visual id from element type
        }
      } else {
        // Element type is not specified. Domain element should be present (except pure design elements).
        // This method is called with EObjectAdapter as parameter from:
        //   - ViewService.createNode(View container, EObject eObject, String type, PreferencesHint preferencesHint) 
        //   - generated ViewFactory.decorateView() for parent element
        if (!WorkflowDiagramEditPart.MODEL_ID.equals(FlowchartVisualIDRegistry
            .getModelID(containerView))) {
          return null; // foreign diagram
        }
        switch (visualID) {
        case StartNodeEditPart.VISUAL_ID:
        case EndnodeEditPart.VISUAL_ID:
        case SimpleActivityEditPart.VISUAL_ID:
        case SubprocessEditPart.VISUAL_ID:
        case EventEditPart.VISUAL_ID:
        case SplitNodeEditPart.VISUAL_ID:
        case MergeNodeEditPart.VISUAL_ID:
        case FlowEditPart.VISUAL_ID:
        case TransientResource2EditPart.VISUAL_ID:
        case Document2EditPart.VISUAL_ID:
        case PersistentResource2EditPart.VISUAL_ID:
        case TransientResourceEditPart.VISUAL_ID:
        case DocumentEditPart.VISUAL_ID:
        case PersistentResourceEditPart.VISUAL_ID:
        case StartNode2EditPart.VISUAL_ID:
        case Endnode2EditPart.VISUAL_ID:
        case SimpleActivity2EditPart.VISUAL_ID:
        case Subprocess2EditPart.VISUAL_ID:
        case Event2EditPart.VISUAL_ID:
        case SplitNode2EditPart.VISUAL_ID:
        case MergeNode2EditPart.VISUAL_ID:
        case Flow2EditPart.VISUAL_ID:
          if (domainElement == null
              || visualID != FlowchartVisualIDRegistry.getNodeVisualID(
                  containerView, domainElement)) {
            return null; // visual id in semantic hint should match visual id for domain element
          }
          break;
        case StartNodeLabelEditPart.VISUAL_ID:
          if (StartNodeEditPart.VISUAL_ID != FlowchartVisualIDRegistry
              .getVisualID(containerView)
              || containerView.getElement() != domainElement) {
            return null; // wrong container
          }
          break;
        case EndnodeLabelEditPart.VISUAL_ID:
          if (EndnodeEditPart.VISUAL_ID != FlowchartVisualIDRegistry
              .getVisualID(containerView)
              || containerView.getElement() != domainElement) {
            return null; // wrong container
          }
          break;
        case SimpleActivityLabelEditPart.VISUAL_ID:
          if (SimpleActivityEditPart.VISUAL_ID != FlowchartVisualIDRegistry
              .getVisualID(containerView)
              || containerView.getElement() != domainElement) {
            return null; // wrong container
          }
          break;
        case SubprocessLabelEditPart.VISUAL_ID:
          if (SubprocessEditPart.VISUAL_ID != FlowchartVisualIDRegistry
              .getVisualID(containerView)
              || containerView.getElement() != domainElement) {
            return null; // wrong container
          }
          break;
        case SplitNodeLabelEditPart.VISUAL_ID:
          if (SplitNodeEditPart.VISUAL_ID != FlowchartVisualIDRegistry
              .getVisualID(containerView)
              || containerView.getElement() != domainElement) {
            return null; // wrong container
          }
          break;
        case FlowFlowCompartmentEditPart.VISUAL_ID:
          if (FlowEditPart.VISUAL_ID != FlowchartVisualIDRegistry
              .getVisualID(containerView)
              || containerView.getElement() != domainElement) {
            return null; // wrong container
          }
          break;
        case TransientResourceLabelEditPart.VISUAL_ID:
          if (TransientResourceEditPart.VISUAL_ID != FlowchartVisualIDRegistry
              .getVisualID(containerView)
              || containerView.getElement() != domainElement) {
            return null; // wrong container
          }
          break;
        case DocumentLabelEditPart.VISUAL_ID:
          if (DocumentEditPart.VISUAL_ID != FlowchartVisualIDRegistry
              .getVisualID(containerView)
              || containerView.getElement() != domainElement) {
            return null; // wrong container
          }
          break;
        case PersistentResourceLabelEditPart.VISUAL_ID:
          if (PersistentResourceEditPart.VISUAL_ID != FlowchartVisualIDRegistry
              .getVisualID(containerView)
              || containerView.getElement() != domainElement) {
            return null; // wrong container
          }
          break;
        case StartNodeLabel2EditPart.VISUAL_ID:
          if (StartNode2EditPart.VISUAL_ID != FlowchartVisualIDRegistry
              .getVisualID(containerView)
              || containerView.getElement() != domainElement) {
            return null; // wrong container
          }
          break;
        case EndnodeLabel2EditPart.VISUAL_ID:
          if (Endnode2EditPart.VISUAL_ID != FlowchartVisualIDRegistry
              .getVisualID(containerView)
              || containerView.getElement() != domainElement) {
            return null; // wrong container
          }
          break;
        case SimpleActivityLabel2EditPart.VISUAL_ID:
          if (SimpleActivity2EditPart.VISUAL_ID != FlowchartVisualIDRegistry
              .getVisualID(containerView)
              || containerView.getElement() != domainElement) {
            return null; // wrong container
          }
          break;
        case SubprocessLabel2EditPart.VISUAL_ID:
          if (Subprocess2EditPart.VISUAL_ID != FlowchartVisualIDRegistry
              .getVisualID(containerView)
              || containerView.getElement() != domainElement) {
            return null; // wrong container
          }
          break;
        case SplitNodeLabel2EditPart.VISUAL_ID:
          if (SplitNode2EditPart.VISUAL_ID != FlowchartVisualIDRegistry
              .getVisualID(containerView)
              || containerView.getElement() != domainElement) {
            return null; // wrong container
          }
          break;
        case FlowFlowCompartment2EditPart.VISUAL_ID:
          if (Flow2EditPart.VISUAL_ID != FlowchartVisualIDRegistry
              .getVisualID(containerView)
              || containerView.getElement() != domainElement) {
            return null; // wrong container
          }
          break;
        case TransientResourceLabel2EditPart.VISUAL_ID:
          if (TransientResource2EditPart.VISUAL_ID != FlowchartVisualIDRegistry
              .getVisualID(containerView)
              || containerView.getElement() != domainElement) {
            return null; // wrong container
          }
          break;
        case DocumentLabel2EditPart.VISUAL_ID:
          if (Document2EditPart.VISUAL_ID != FlowchartVisualIDRegistry
              .getVisualID(containerView)
              || containerView.getElement() != domainElement) {
            return null; // wrong container
          }
          break;
        case PersistentResourceLabel2EditPart.VISUAL_ID:
          if (PersistentResource2EditPart.VISUAL_ID != FlowchartVisualIDRegistry
              .getVisualID(containerView)
              || containerView.getElement() != domainElement) {
            return null; // wrong container
          }
          break;
        case DiagramArcLabelEditPart.VISUAL_ID:
        case DiagramArcProbabilityEditPart.VISUAL_ID:
          if (DiagramArcEditPart.VISUAL_ID != FlowchartVisualIDRegistry
              .getVisualID(containerView)
              || containerView.getElement() != domainElement) {
            return null; // wrong container
          }
          break;
        default:
          return null;
        }
      }
    }
    return getNodeViewClass(containerView, visualID);
  }

  /**
   * @generated
   */
  protected Class getNodeViewClass(View containerView, int visualID) {
    if (containerView == null
        || !FlowchartVisualIDRegistry.canCreateNode(containerView, visualID)) {
      return null;
    }
    switch (visualID) {
    case StartNodeEditPart.VISUAL_ID:
      return StartNodeViewFactory.class;
    case StartNodeLabelEditPart.VISUAL_ID:
      return StartNodeLabelViewFactory.class;
    case EndnodeEditPart.VISUAL_ID:
      return EndnodeViewFactory.class;
    case EndnodeLabelEditPart.VISUAL_ID:
      return EndnodeLabelViewFactory.class;
    case SimpleActivityEditPart.VISUAL_ID:
      return SimpleActivityViewFactory.class;
    case SimpleActivityLabelEditPart.VISUAL_ID:
      return SimpleActivityLabelViewFactory.class;
    case SubprocessEditPart.VISUAL_ID:
      return SubprocessViewFactory.class;
    case SubprocessLabelEditPart.VISUAL_ID:
      return SubprocessLabelViewFactory.class;
    case EventEditPart.VISUAL_ID:
      return EventViewFactory.class;
    case SplitNodeEditPart.VISUAL_ID:
      return SplitNodeViewFactory.class;
    case SplitNodeLabelEditPart.VISUAL_ID:
      return SplitNodeLabelViewFactory.class;
    case MergeNodeEditPart.VISUAL_ID:
      return MergeNodeViewFactory.class;
    case FlowEditPart.VISUAL_ID:
      return FlowViewFactory.class;
    case TransientResourceEditPart.VISUAL_ID:
      return TransientResourceViewFactory.class;
    case TransientResourceLabelEditPart.VISUAL_ID:
      return TransientResourceLabelViewFactory.class;
    case DocumentEditPart.VISUAL_ID:
      return DocumentViewFactory.class;
    case DocumentLabelEditPart.VISUAL_ID:
      return DocumentLabelViewFactory.class;
    case PersistentResourceEditPart.VISUAL_ID:
      return PersistentResourceViewFactory.class;
    case PersistentResourceLabelEditPart.VISUAL_ID:
      return PersistentResourceLabelViewFactory.class;
    case StartNode2EditPart.VISUAL_ID:
      return StartNode2ViewFactory.class;
    case StartNodeLabel2EditPart.VISUAL_ID:
      return StartNodeLabel2ViewFactory.class;
    case Endnode2EditPart.VISUAL_ID:
      return Endnode2ViewFactory.class;
    case EndnodeLabel2EditPart.VISUAL_ID:
      return EndnodeLabel2ViewFactory.class;
    case SimpleActivity2EditPart.VISUAL_ID:
      return SimpleActivity2ViewFactory.class;
    case SimpleActivityLabel2EditPart.VISUAL_ID:
      return SimpleActivityLabel2ViewFactory.class;
    case Subprocess2EditPart.VISUAL_ID:
      return Subprocess2ViewFactory.class;
    case SubprocessLabel2EditPart.VISUAL_ID:
      return SubprocessLabel2ViewFactory.class;
    case Event2EditPart.VISUAL_ID:
      return Event2ViewFactory.class;
    case SplitNode2EditPart.VISUAL_ID:
      return SplitNode2ViewFactory.class;
    case SplitNodeLabel2EditPart.VISUAL_ID:
      return SplitNodeLabel2ViewFactory.class;
    case MergeNode2EditPart.VISUAL_ID:
      return MergeNode2ViewFactory.class;
    case Flow2EditPart.VISUAL_ID:
      return Flow2ViewFactory.class;
    case TransientResource2EditPart.VISUAL_ID:
      return TransientResource2ViewFactory.class;
    case TransientResourceLabel2EditPart.VISUAL_ID:
      return TransientResourceLabel2ViewFactory.class;
    case Document2EditPart.VISUAL_ID:
      return Document2ViewFactory.class;
    case DocumentLabel2EditPart.VISUAL_ID:
      return DocumentLabel2ViewFactory.class;
    case PersistentResource2EditPart.VISUAL_ID:
      return PersistentResource2ViewFactory.class;
    case PersistentResourceLabel2EditPart.VISUAL_ID:
      return PersistentResourceLabel2ViewFactory.class;
    case FlowFlowCompartmentEditPart.VISUAL_ID:
      return FlowFlowCompartmentViewFactory.class;
    case FlowFlowCompartment2EditPart.VISUAL_ID:
      return FlowFlowCompartment2ViewFactory.class;
    case DiagramArcLabelEditPart.VISUAL_ID:
      return DiagramArcLabelViewFactory.class;
    case DiagramArcProbabilityEditPart.VISUAL_ID:
      return DiagramArcProbabilityViewFactory.class;
    }
    return null;
  }

  /**
   * @generated
   */
  protected Class getEdgeViewClass(IAdaptable semanticAdapter,
      View containerView, String semanticHint) {
    IElementType elementType = getSemanticElementType(semanticAdapter);
    if (!FlowchartElementTypes.isKnownElementType(elementType)
        || (!(elementType instanceof IHintedType))) {
      return null; // foreign element type
    }
    String elementTypeHint = ((IHintedType) elementType).getSemanticHint();
    if (elementTypeHint == null) {
      return null; // our hint is visual id and must be specified
    }
    if (semanticHint != null && !semanticHint.equals(elementTypeHint)) {
      return null; // if semantic hint is specified it should be the same as in element type
    }
    int visualID = FlowchartVisualIDRegistry.getVisualID(elementTypeHint);
    EObject domainElement = getSemanticElement(semanticAdapter);
    if (domainElement != null
        && visualID != FlowchartVisualIDRegistry
            .getLinkWithClassVisualID(domainElement)) {
      return null; // visual id for link EClass should match visual id from element type
    }
    return getEdgeViewClass(visualID);
  }

  /**
   * @generated
   */
  protected Class getEdgeViewClass(int visualID) {
    switch (visualID) {
    case DiagramArcEditPart.VISUAL_ID:
      return DiagramArcViewFactory.class;
    }
    return null;
  }

  /**
   * @generated
   */
  private IElementType getSemanticElementType(IAdaptable semanticAdapter) {
    if (semanticAdapter == null) {
      return null;
    }
    return (IElementType) semanticAdapter.getAdapter(IElementType.class);
  }
}
