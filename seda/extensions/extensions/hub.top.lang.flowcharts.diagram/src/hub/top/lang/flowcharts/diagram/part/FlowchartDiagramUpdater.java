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

import hub.top.lang.flowcharts.DiagramArc;
import hub.top.lang.flowcharts.DiagramNode;
import hub.top.lang.flowcharts.Document;
import hub.top.lang.flowcharts.Endnode;
import hub.top.lang.flowcharts.Event;
import hub.top.lang.flowcharts.Flow;
import hub.top.lang.flowcharts.FlowPackage;
import hub.top.lang.flowcharts.MergeNode;
import hub.top.lang.flowcharts.PersistentResource;
import hub.top.lang.flowcharts.SimpleActivity;
import hub.top.lang.flowcharts.SplitNode;
import hub.top.lang.flowcharts.StartNode;
import hub.top.lang.flowcharts.Subprocess;
import hub.top.lang.flowcharts.TransientResource;
import hub.top.lang.flowcharts.WorkflowDiagram;
import hub.top.lang.flowcharts.diagram.edit.parts.DiagramArcEditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.Document2EditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.DocumentEditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.Endnode2EditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.EndnodeEditPart;
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
import hub.top.lang.flowcharts.diagram.edit.parts.SimpleActivity2EditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.SimpleActivityEditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.SplitNode2EditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.SplitNodeEditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.StartNode2EditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.StartNodeEditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.Subprocess2EditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.SubprocessEditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.TransientResource2EditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.TransientResourceEditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.WorkflowDiagramEditPart;
import hub.top.lang.flowcharts.diagram.providers.FlowchartElementTypes;

import java.util.Collection;
import java.util.Collections;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.EStructuralFeature;
import org.eclipse.emf.ecore.util.EcoreUtil;
import org.eclipse.gmf.runtime.notation.View;

/**
 * @generated
 */
public class FlowchartDiagramUpdater {

  /**
   * @generated
   */
  public static List getSemanticChildren(View view) {
    switch (FlowchartVisualIDRegistry.getVisualID(view)) {
    case FlowFlowCompartmentEditPart.VISUAL_ID:
      return getFlowFlowCompartment_5001SemanticChildren(view);
    case FlowFlowCompartment2EditPart.VISUAL_ID:
      return getFlowFlowCompartment_5002SemanticChildren(view);
    case WorkflowDiagramEditPart.VISUAL_ID:
      return getWorkflowDiagram_79SemanticChildren(view);
    }
    return Collections.EMPTY_LIST;
  }

  /**
   * @generated
   */
  public static List getFlowFlowCompartment_5001SemanticChildren(View view) {
    if (false == view.eContainer() instanceof View) {
      return Collections.EMPTY_LIST;
    }
    View containerView = (View) view.eContainer();
    if (!containerView.isSetElement()) {
      return Collections.EMPTY_LIST;
    }
    Flow modelElement = (Flow) containerView.getElement();
    List result = new LinkedList();
    for (Iterator it = modelElement.getChildNodes().iterator(); it.hasNext();) {
      DiagramNode childElement = (DiagramNode) it.next();
      int visualID = FlowchartVisualIDRegistry.getNodeVisualID(view,
          childElement);
      if (visualID == StartNode2EditPart.VISUAL_ID) {
        result.add(new FlowchartNodeDescriptor(childElement, visualID));
        continue;
      }
      if (visualID == Endnode2EditPart.VISUAL_ID) {
        result.add(new FlowchartNodeDescriptor(childElement, visualID));
        continue;
      }
      if (visualID == SimpleActivity2EditPart.VISUAL_ID) {
        result.add(new FlowchartNodeDescriptor(childElement, visualID));
        continue;
      }
      if (visualID == Subprocess2EditPart.VISUAL_ID) {
        result.add(new FlowchartNodeDescriptor(childElement, visualID));
        continue;
      }
      if (visualID == Event2EditPart.VISUAL_ID) {
        result.add(new FlowchartNodeDescriptor(childElement, visualID));
        continue;
      }
      if (visualID == SplitNode2EditPart.VISUAL_ID) {
        result.add(new FlowchartNodeDescriptor(childElement, visualID));
        continue;
      }
      if (visualID == MergeNode2EditPart.VISUAL_ID) {
        result.add(new FlowchartNodeDescriptor(childElement, visualID));
        continue;
      }
      if (visualID == Flow2EditPart.VISUAL_ID) {
        result.add(new FlowchartNodeDescriptor(childElement, visualID));
        continue;
      }
      if (visualID == TransientResource2EditPart.VISUAL_ID) {
        result.add(new FlowchartNodeDescriptor(childElement, visualID));
        continue;
      }
      if (visualID == Document2EditPart.VISUAL_ID) {
        result.add(new FlowchartNodeDescriptor(childElement, visualID));
        continue;
      }
      if (visualID == PersistentResource2EditPart.VISUAL_ID) {
        result.add(new FlowchartNodeDescriptor(childElement, visualID));
        continue;
      }
    }
    return result;
  }

  /**
   * @generated
   */
  public static List getFlowFlowCompartment_5002SemanticChildren(View view) {
    if (false == view.eContainer() instanceof View) {
      return Collections.EMPTY_LIST;
    }
    View containerView = (View) view.eContainer();
    if (!containerView.isSetElement()) {
      return Collections.EMPTY_LIST;
    }
    Flow modelElement = (Flow) containerView.getElement();
    List result = new LinkedList();
    for (Iterator it = modelElement.getChildNodes().iterator(); it.hasNext();) {
      DiagramNode childElement = (DiagramNode) it.next();
      int visualID = FlowchartVisualIDRegistry.getNodeVisualID(view,
          childElement);
      if (visualID == StartNode2EditPart.VISUAL_ID) {
        result.add(new FlowchartNodeDescriptor(childElement, visualID));
        continue;
      }
      if (visualID == Endnode2EditPart.VISUAL_ID) {
        result.add(new FlowchartNodeDescriptor(childElement, visualID));
        continue;
      }
      if (visualID == SimpleActivity2EditPart.VISUAL_ID) {
        result.add(new FlowchartNodeDescriptor(childElement, visualID));
        continue;
      }
      if (visualID == Subprocess2EditPart.VISUAL_ID) {
        result.add(new FlowchartNodeDescriptor(childElement, visualID));
        continue;
      }
      if (visualID == Event2EditPart.VISUAL_ID) {
        result.add(new FlowchartNodeDescriptor(childElement, visualID));
        continue;
      }
      if (visualID == SplitNode2EditPart.VISUAL_ID) {
        result.add(new FlowchartNodeDescriptor(childElement, visualID));
        continue;
      }
      if (visualID == MergeNode2EditPart.VISUAL_ID) {
        result.add(new FlowchartNodeDescriptor(childElement, visualID));
        continue;
      }
      if (visualID == Flow2EditPart.VISUAL_ID) {
        result.add(new FlowchartNodeDescriptor(childElement, visualID));
        continue;
      }
      if (visualID == TransientResource2EditPart.VISUAL_ID) {
        result.add(new FlowchartNodeDescriptor(childElement, visualID));
        continue;
      }
      if (visualID == Document2EditPart.VISUAL_ID) {
        result.add(new FlowchartNodeDescriptor(childElement, visualID));
        continue;
      }
      if (visualID == PersistentResource2EditPart.VISUAL_ID) {
        result.add(new FlowchartNodeDescriptor(childElement, visualID));
        continue;
      }
    }
    return result;
  }

  /**
   * @generated
   */
  public static List getWorkflowDiagram_79SemanticChildren(View view) {
    if (!view.isSetElement()) {
      return Collections.EMPTY_LIST;
    }
    WorkflowDiagram modelElement = (WorkflowDiagram) view.getElement();
    List result = new LinkedList();
    for (Iterator it = modelElement.getDiagramNodes().iterator(); it.hasNext();) {
      DiagramNode childElement = (DiagramNode) it.next();
      int visualID = FlowchartVisualIDRegistry.getNodeVisualID(view,
          childElement);
      if (visualID == StartNodeEditPart.VISUAL_ID) {
        result.add(new FlowchartNodeDescriptor(childElement, visualID));
        continue;
      }
      if (visualID == EndnodeEditPart.VISUAL_ID) {
        result.add(new FlowchartNodeDescriptor(childElement, visualID));
        continue;
      }
      if (visualID == SimpleActivityEditPart.VISUAL_ID) {
        result.add(new FlowchartNodeDescriptor(childElement, visualID));
        continue;
      }
      if (visualID == SubprocessEditPart.VISUAL_ID) {
        result.add(new FlowchartNodeDescriptor(childElement, visualID));
        continue;
      }
      if (visualID == EventEditPart.VISUAL_ID) {
        result.add(new FlowchartNodeDescriptor(childElement, visualID));
        continue;
      }
      if (visualID == SplitNodeEditPart.VISUAL_ID) {
        result.add(new FlowchartNodeDescriptor(childElement, visualID));
        continue;
      }
      if (visualID == MergeNodeEditPart.VISUAL_ID) {
        result.add(new FlowchartNodeDescriptor(childElement, visualID));
        continue;
      }
      if (visualID == FlowEditPart.VISUAL_ID) {
        result.add(new FlowchartNodeDescriptor(childElement, visualID));
        continue;
      }
      if (visualID == TransientResourceEditPart.VISUAL_ID) {
        result.add(new FlowchartNodeDescriptor(childElement, visualID));
        continue;
      }
      if (visualID == DocumentEditPart.VISUAL_ID) {
        result.add(new FlowchartNodeDescriptor(childElement, visualID));
        continue;
      }
      if (visualID == PersistentResourceEditPart.VISUAL_ID) {
        result.add(new FlowchartNodeDescriptor(childElement, visualID));
        continue;
      }
    }
    return result;
  }

  /**
   * @generated
   */
  public static List getContainedLinks(View view) {
    switch (FlowchartVisualIDRegistry.getVisualID(view)) {
    case WorkflowDiagramEditPart.VISUAL_ID:
      return getWorkflowDiagram_79ContainedLinks(view);
    case StartNodeEditPart.VISUAL_ID:
      return getStartNode_1001ContainedLinks(view);
    case EndnodeEditPart.VISUAL_ID:
      return getEndnode_1002ContainedLinks(view);
    case SimpleActivityEditPart.VISUAL_ID:
      return getSimpleActivity_1003ContainedLinks(view);
    case SubprocessEditPart.VISUAL_ID:
      return getSubprocess_1004ContainedLinks(view);
    case EventEditPart.VISUAL_ID:
      return getEvent_1005ContainedLinks(view);
    case SplitNodeEditPart.VISUAL_ID:
      return getSplitNode_1006ContainedLinks(view);
    case MergeNodeEditPart.VISUAL_ID:
      return getMergeNode_1007ContainedLinks(view);
    case FlowEditPart.VISUAL_ID:
      return getFlow_1008ContainedLinks(view);
    case TransientResourceEditPart.VISUAL_ID:
      return getTransientResource_1009ContainedLinks(view);
    case DocumentEditPart.VISUAL_ID:
      return getDocument_1010ContainedLinks(view);
    case PersistentResourceEditPart.VISUAL_ID:
      return getPersistentResource_1011ContainedLinks(view);
    case StartNode2EditPart.VISUAL_ID:
      return getStartNode_2001ContainedLinks(view);
    case Endnode2EditPart.VISUAL_ID:
      return getEndnode_2002ContainedLinks(view);
    case SimpleActivity2EditPart.VISUAL_ID:
      return getSimpleActivity_2003ContainedLinks(view);
    case Subprocess2EditPart.VISUAL_ID:
      return getSubprocess_2004ContainedLinks(view);
    case Event2EditPart.VISUAL_ID:
      return getEvent_2005ContainedLinks(view);
    case SplitNode2EditPart.VISUAL_ID:
      return getSplitNode_2006ContainedLinks(view);
    case MergeNode2EditPart.VISUAL_ID:
      return getMergeNode_2007ContainedLinks(view);
    case Flow2EditPart.VISUAL_ID:
      return getFlow_2008ContainedLinks(view);
    case TransientResource2EditPart.VISUAL_ID:
      return getTransientResource_2009ContainedLinks(view);
    case Document2EditPart.VISUAL_ID:
      return getDocument_2010ContainedLinks(view);
    case PersistentResource2EditPart.VISUAL_ID:
      return getPersistentResource_2011ContainedLinks(view);
    case DiagramArcEditPart.VISUAL_ID:
      return getDiagramArc_3001ContainedLinks(view);
    }
    return Collections.EMPTY_LIST;
  }

  /**
   * @generated
   */
  public static List getIncomingLinks(View view) {
    switch (FlowchartVisualIDRegistry.getVisualID(view)) {
    case StartNodeEditPart.VISUAL_ID:
      return getStartNode_1001IncomingLinks(view);
    case EndnodeEditPart.VISUAL_ID:
      return getEndnode_1002IncomingLinks(view);
    case SimpleActivityEditPart.VISUAL_ID:
      return getSimpleActivity_1003IncomingLinks(view);
    case SubprocessEditPart.VISUAL_ID:
      return getSubprocess_1004IncomingLinks(view);
    case EventEditPart.VISUAL_ID:
      return getEvent_1005IncomingLinks(view);
    case SplitNodeEditPart.VISUAL_ID:
      return getSplitNode_1006IncomingLinks(view);
    case MergeNodeEditPart.VISUAL_ID:
      return getMergeNode_1007IncomingLinks(view);
    case FlowEditPart.VISUAL_ID:
      return getFlow_1008IncomingLinks(view);
    case TransientResourceEditPart.VISUAL_ID:
      return getTransientResource_1009IncomingLinks(view);
    case DocumentEditPart.VISUAL_ID:
      return getDocument_1010IncomingLinks(view);
    case PersistentResourceEditPart.VISUAL_ID:
      return getPersistentResource_1011IncomingLinks(view);
    case StartNode2EditPart.VISUAL_ID:
      return getStartNode_2001IncomingLinks(view);
    case Endnode2EditPart.VISUAL_ID:
      return getEndnode_2002IncomingLinks(view);
    case SimpleActivity2EditPart.VISUAL_ID:
      return getSimpleActivity_2003IncomingLinks(view);
    case Subprocess2EditPart.VISUAL_ID:
      return getSubprocess_2004IncomingLinks(view);
    case Event2EditPart.VISUAL_ID:
      return getEvent_2005IncomingLinks(view);
    case SplitNode2EditPart.VISUAL_ID:
      return getSplitNode_2006IncomingLinks(view);
    case MergeNode2EditPart.VISUAL_ID:
      return getMergeNode_2007IncomingLinks(view);
    case Flow2EditPart.VISUAL_ID:
      return getFlow_2008IncomingLinks(view);
    case TransientResource2EditPart.VISUAL_ID:
      return getTransientResource_2009IncomingLinks(view);
    case Document2EditPart.VISUAL_ID:
      return getDocument_2010IncomingLinks(view);
    case PersistentResource2EditPart.VISUAL_ID:
      return getPersistentResource_2011IncomingLinks(view);
    case DiagramArcEditPart.VISUAL_ID:
      return getDiagramArc_3001IncomingLinks(view);
    }
    return Collections.EMPTY_LIST;
  }

  /**
   * @generated
   */
  public static List getOutgoingLinks(View view) {
    switch (FlowchartVisualIDRegistry.getVisualID(view)) {
    case StartNodeEditPart.VISUAL_ID:
      return getStartNode_1001OutgoingLinks(view);
    case EndnodeEditPart.VISUAL_ID:
      return getEndnode_1002OutgoingLinks(view);
    case SimpleActivityEditPart.VISUAL_ID:
      return getSimpleActivity_1003OutgoingLinks(view);
    case SubprocessEditPart.VISUAL_ID:
      return getSubprocess_1004OutgoingLinks(view);
    case EventEditPart.VISUAL_ID:
      return getEvent_1005OutgoingLinks(view);
    case SplitNodeEditPart.VISUAL_ID:
      return getSplitNode_1006OutgoingLinks(view);
    case MergeNodeEditPart.VISUAL_ID:
      return getMergeNode_1007OutgoingLinks(view);
    case FlowEditPart.VISUAL_ID:
      return getFlow_1008OutgoingLinks(view);
    case TransientResourceEditPart.VISUAL_ID:
      return getTransientResource_1009OutgoingLinks(view);
    case DocumentEditPart.VISUAL_ID:
      return getDocument_1010OutgoingLinks(view);
    case PersistentResourceEditPart.VISUAL_ID:
      return getPersistentResource_1011OutgoingLinks(view);
    case StartNode2EditPart.VISUAL_ID:
      return getStartNode_2001OutgoingLinks(view);
    case Endnode2EditPart.VISUAL_ID:
      return getEndnode_2002OutgoingLinks(view);
    case SimpleActivity2EditPart.VISUAL_ID:
      return getSimpleActivity_2003OutgoingLinks(view);
    case Subprocess2EditPart.VISUAL_ID:
      return getSubprocess_2004OutgoingLinks(view);
    case Event2EditPart.VISUAL_ID:
      return getEvent_2005OutgoingLinks(view);
    case SplitNode2EditPart.VISUAL_ID:
      return getSplitNode_2006OutgoingLinks(view);
    case MergeNode2EditPart.VISUAL_ID:
      return getMergeNode_2007OutgoingLinks(view);
    case Flow2EditPart.VISUAL_ID:
      return getFlow_2008OutgoingLinks(view);
    case TransientResource2EditPart.VISUAL_ID:
      return getTransientResource_2009OutgoingLinks(view);
    case Document2EditPart.VISUAL_ID:
      return getDocument_2010OutgoingLinks(view);
    case PersistentResource2EditPart.VISUAL_ID:
      return getPersistentResource_2011OutgoingLinks(view);
    case DiagramArcEditPart.VISUAL_ID:
      return getDiagramArc_3001OutgoingLinks(view);
    }
    return Collections.EMPTY_LIST;
  }

  /**
   * @generated
   */
  public static List getWorkflowDiagram_79ContainedLinks(View view) {
    WorkflowDiagram modelElement = (WorkflowDiagram) view.getElement();
    List result = new LinkedList();
    result
        .addAll(getContainedTypeModelFacetLinks_DiagramArc_3001(modelElement));
    return result;
  }

  /**
   * @generated
   */
  public static List getStartNode_1001ContainedLinks(View view) {
    return Collections.EMPTY_LIST;
  }

  /**
   * @generated
   */
  public static List getEndnode_1002ContainedLinks(View view) {
    return Collections.EMPTY_LIST;
  }

  /**
   * @generated
   */
  public static List getSimpleActivity_1003ContainedLinks(View view) {
    return Collections.EMPTY_LIST;
  }

  /**
   * @generated
   */
  public static List getSubprocess_1004ContainedLinks(View view) {
    return Collections.EMPTY_LIST;
  }

  /**
   * @generated
   */
  public static List getEvent_1005ContainedLinks(View view) {
    return Collections.EMPTY_LIST;
  }

  /**
   * @generated
   */
  public static List getSplitNode_1006ContainedLinks(View view) {
    return Collections.EMPTY_LIST;
  }

  /**
   * @generated
   */
  public static List getMergeNode_1007ContainedLinks(View view) {
    return Collections.EMPTY_LIST;
  }

  /**
   * @generated
   */
  public static List getFlow_1008ContainedLinks(View view) {
    return Collections.EMPTY_LIST;
  }

  /**
   * @generated
   */
  public static List getTransientResource_1009ContainedLinks(View view) {
    return Collections.EMPTY_LIST;
  }

  /**
   * @generated
   */
  public static List getDocument_1010ContainedLinks(View view) {
    return Collections.EMPTY_LIST;
  }

  /**
   * @generated
   */
  public static List getPersistentResource_1011ContainedLinks(View view) {
    return Collections.EMPTY_LIST;
  }

  /**
   * @generated
   */
  public static List getStartNode_2001ContainedLinks(View view) {
    return Collections.EMPTY_LIST;
  }

  /**
   * @generated
   */
  public static List getEndnode_2002ContainedLinks(View view) {
    return Collections.EMPTY_LIST;
  }

  /**
   * @generated
   */
  public static List getSimpleActivity_2003ContainedLinks(View view) {
    return Collections.EMPTY_LIST;
  }

  /**
   * @generated
   */
  public static List getSubprocess_2004ContainedLinks(View view) {
    return Collections.EMPTY_LIST;
  }

  /**
   * @generated
   */
  public static List getEvent_2005ContainedLinks(View view) {
    return Collections.EMPTY_LIST;
  }

  /**
   * @generated
   */
  public static List getSplitNode_2006ContainedLinks(View view) {
    return Collections.EMPTY_LIST;
  }

  /**
   * @generated
   */
  public static List getMergeNode_2007ContainedLinks(View view) {
    return Collections.EMPTY_LIST;
  }

  /**
   * @generated
   */
  public static List getFlow_2008ContainedLinks(View view) {
    return Collections.EMPTY_LIST;
  }

  /**
   * @generated
   */
  public static List getTransientResource_2009ContainedLinks(View view) {
    return Collections.EMPTY_LIST;
  }

  /**
   * @generated
   */
  public static List getDocument_2010ContainedLinks(View view) {
    return Collections.EMPTY_LIST;
  }

  /**
   * @generated
   */
  public static List getPersistentResource_2011ContainedLinks(View view) {
    return Collections.EMPTY_LIST;
  }

  /**
   * @generated
   */
  public static List getDiagramArc_3001ContainedLinks(View view) {
    return Collections.EMPTY_LIST;
  }

  /**
   * @generated
   */
  public static List getStartNode_1001IncomingLinks(View view) {
    StartNode modelElement = (StartNode) view.getElement();
    Map crossReferences = EcoreUtil.CrossReferencer.find(view.eResource()
        .getResourceSet().getResources());
    List result = new LinkedList();
    result.addAll(getIncomingTypeModelFacetLinks_DiagramArc_3001(modelElement,
        crossReferences));
    return result;
  }

  /**
   * @generated
   */
  public static List getEndnode_1002IncomingLinks(View view) {
    Endnode modelElement = (Endnode) view.getElement();
    Map crossReferences = EcoreUtil.CrossReferencer.find(view.eResource()
        .getResourceSet().getResources());
    List result = new LinkedList();
    result.addAll(getIncomingTypeModelFacetLinks_DiagramArc_3001(modelElement,
        crossReferences));
    return result;
  }

  /**
   * @generated
   */
  public static List getSimpleActivity_1003IncomingLinks(View view) {
    SimpleActivity modelElement = (SimpleActivity) view.getElement();
    Map crossReferences = EcoreUtil.CrossReferencer.find(view.eResource()
        .getResourceSet().getResources());
    List result = new LinkedList();
    result.addAll(getIncomingTypeModelFacetLinks_DiagramArc_3001(modelElement,
        crossReferences));
    return result;
  }

  /**
   * @generated
   */
  public static List getSubprocess_1004IncomingLinks(View view) {
    Subprocess modelElement = (Subprocess) view.getElement();
    Map crossReferences = EcoreUtil.CrossReferencer.find(view.eResource()
        .getResourceSet().getResources());
    List result = new LinkedList();
    result.addAll(getIncomingTypeModelFacetLinks_DiagramArc_3001(modelElement,
        crossReferences));
    return result;
  }

  /**
   * @generated
   */
  public static List getEvent_1005IncomingLinks(View view) {
    Event modelElement = (Event) view.getElement();
    Map crossReferences = EcoreUtil.CrossReferencer.find(view.eResource()
        .getResourceSet().getResources());
    List result = new LinkedList();
    result.addAll(getIncomingTypeModelFacetLinks_DiagramArc_3001(modelElement,
        crossReferences));
    return result;
  }

  /**
   * @generated
   */
  public static List getSplitNode_1006IncomingLinks(View view) {
    SplitNode modelElement = (SplitNode) view.getElement();
    Map crossReferences = EcoreUtil.CrossReferencer.find(view.eResource()
        .getResourceSet().getResources());
    List result = new LinkedList();
    result.addAll(getIncomingTypeModelFacetLinks_DiagramArc_3001(modelElement,
        crossReferences));
    return result;
  }

  /**
   * @generated
   */
  public static List getMergeNode_1007IncomingLinks(View view) {
    MergeNode modelElement = (MergeNode) view.getElement();
    Map crossReferences = EcoreUtil.CrossReferencer.find(view.eResource()
        .getResourceSet().getResources());
    List result = new LinkedList();
    result.addAll(getIncomingTypeModelFacetLinks_DiagramArc_3001(modelElement,
        crossReferences));
    return result;
  }

  /**
   * @generated
   */
  public static List getFlow_1008IncomingLinks(View view) {
    Flow modelElement = (Flow) view.getElement();
    Map crossReferences = EcoreUtil.CrossReferencer.find(view.eResource()
        .getResourceSet().getResources());
    List result = new LinkedList();
    result.addAll(getIncomingTypeModelFacetLinks_DiagramArc_3001(modelElement,
        crossReferences));
    return result;
  }

  /**
   * @generated
   */
  public static List getTransientResource_1009IncomingLinks(View view) {
    TransientResource modelElement = (TransientResource) view.getElement();
    Map crossReferences = EcoreUtil.CrossReferencer.find(view.eResource()
        .getResourceSet().getResources());
    List result = new LinkedList();
    result.addAll(getIncomingTypeModelFacetLinks_DiagramArc_3001(modelElement,
        crossReferences));
    return result;
  }

  /**
   * @generated
   */
  public static List getDocument_1010IncomingLinks(View view) {
    Document modelElement = (Document) view.getElement();
    Map crossReferences = EcoreUtil.CrossReferencer.find(view.eResource()
        .getResourceSet().getResources());
    List result = new LinkedList();
    result.addAll(getIncomingTypeModelFacetLinks_DiagramArc_3001(modelElement,
        crossReferences));
    return result;
  }

  /**
   * @generated
   */
  public static List getPersistentResource_1011IncomingLinks(View view) {
    PersistentResource modelElement = (PersistentResource) view.getElement();
    Map crossReferences = EcoreUtil.CrossReferencer.find(view.eResource()
        .getResourceSet().getResources());
    List result = new LinkedList();
    result.addAll(getIncomingTypeModelFacetLinks_DiagramArc_3001(modelElement,
        crossReferences));
    return result;
  }

  /**
   * @generated
   */
  public static List getStartNode_2001IncomingLinks(View view) {
    StartNode modelElement = (StartNode) view.getElement();
    Map crossReferences = EcoreUtil.CrossReferencer.find(view.eResource()
        .getResourceSet().getResources());
    List result = new LinkedList();
    result.addAll(getIncomingTypeModelFacetLinks_DiagramArc_3001(modelElement,
        crossReferences));
    return result;
  }

  /**
   * @generated
   */
  public static List getEndnode_2002IncomingLinks(View view) {
    Endnode modelElement = (Endnode) view.getElement();
    Map crossReferences = EcoreUtil.CrossReferencer.find(view.eResource()
        .getResourceSet().getResources());
    List result = new LinkedList();
    result.addAll(getIncomingTypeModelFacetLinks_DiagramArc_3001(modelElement,
        crossReferences));
    return result;
  }

  /**
   * @generated
   */
  public static List getSimpleActivity_2003IncomingLinks(View view) {
    SimpleActivity modelElement = (SimpleActivity) view.getElement();
    Map crossReferences = EcoreUtil.CrossReferencer.find(view.eResource()
        .getResourceSet().getResources());
    List result = new LinkedList();
    result.addAll(getIncomingTypeModelFacetLinks_DiagramArc_3001(modelElement,
        crossReferences));
    return result;
  }

  /**
   * @generated
   */
  public static List getSubprocess_2004IncomingLinks(View view) {
    Subprocess modelElement = (Subprocess) view.getElement();
    Map crossReferences = EcoreUtil.CrossReferencer.find(view.eResource()
        .getResourceSet().getResources());
    List result = new LinkedList();
    result.addAll(getIncomingTypeModelFacetLinks_DiagramArc_3001(modelElement,
        crossReferences));
    return result;
  }

  /**
   * @generated
   */
  public static List getEvent_2005IncomingLinks(View view) {
    Event modelElement = (Event) view.getElement();
    Map crossReferences = EcoreUtil.CrossReferencer.find(view.eResource()
        .getResourceSet().getResources());
    List result = new LinkedList();
    result.addAll(getIncomingTypeModelFacetLinks_DiagramArc_3001(modelElement,
        crossReferences));
    return result;
  }

  /**
   * @generated
   */
  public static List getSplitNode_2006IncomingLinks(View view) {
    SplitNode modelElement = (SplitNode) view.getElement();
    Map crossReferences = EcoreUtil.CrossReferencer.find(view.eResource()
        .getResourceSet().getResources());
    List result = new LinkedList();
    result.addAll(getIncomingTypeModelFacetLinks_DiagramArc_3001(modelElement,
        crossReferences));
    return result;
  }

  /**
   * @generated
   */
  public static List getMergeNode_2007IncomingLinks(View view) {
    MergeNode modelElement = (MergeNode) view.getElement();
    Map crossReferences = EcoreUtil.CrossReferencer.find(view.eResource()
        .getResourceSet().getResources());
    List result = new LinkedList();
    result.addAll(getIncomingTypeModelFacetLinks_DiagramArc_3001(modelElement,
        crossReferences));
    return result;
  }

  /**
   * @generated
   */
  public static List getFlow_2008IncomingLinks(View view) {
    Flow modelElement = (Flow) view.getElement();
    Map crossReferences = EcoreUtil.CrossReferencer.find(view.eResource()
        .getResourceSet().getResources());
    List result = new LinkedList();
    result.addAll(getIncomingTypeModelFacetLinks_DiagramArc_3001(modelElement,
        crossReferences));
    return result;
  }

  /**
   * @generated
   */
  public static List getTransientResource_2009IncomingLinks(View view) {
    TransientResource modelElement = (TransientResource) view.getElement();
    Map crossReferences = EcoreUtil.CrossReferencer.find(view.eResource()
        .getResourceSet().getResources());
    List result = new LinkedList();
    result.addAll(getIncomingTypeModelFacetLinks_DiagramArc_3001(modelElement,
        crossReferences));
    return result;
  }

  /**
   * @generated
   */
  public static List getDocument_2010IncomingLinks(View view) {
    Document modelElement = (Document) view.getElement();
    Map crossReferences = EcoreUtil.CrossReferencer.find(view.eResource()
        .getResourceSet().getResources());
    List result = new LinkedList();
    result.addAll(getIncomingTypeModelFacetLinks_DiagramArc_3001(modelElement,
        crossReferences));
    return result;
  }

  /**
   * @generated
   */
  public static List getPersistentResource_2011IncomingLinks(View view) {
    PersistentResource modelElement = (PersistentResource) view.getElement();
    Map crossReferences = EcoreUtil.CrossReferencer.find(view.eResource()
        .getResourceSet().getResources());
    List result = new LinkedList();
    result.addAll(getIncomingTypeModelFacetLinks_DiagramArc_3001(modelElement,
        crossReferences));
    return result;
  }

  /**
   * @generated
   */
  public static List getDiagramArc_3001IncomingLinks(View view) {
    return Collections.EMPTY_LIST;
  }

  /**
   * @generated
   */
  public static List getStartNode_1001OutgoingLinks(View view) {
    StartNode modelElement = (StartNode) view.getElement();
    List result = new LinkedList();
    result.addAll(getOutgoingTypeModelFacetLinks_DiagramArc_3001(modelElement));
    return result;
  }

  /**
   * @generated
   */
  public static List getEndnode_1002OutgoingLinks(View view) {
    Endnode modelElement = (Endnode) view.getElement();
    List result = new LinkedList();
    result.addAll(getOutgoingTypeModelFacetLinks_DiagramArc_3001(modelElement));
    return result;
  }

  /**
   * @generated
   */
  public static List getSimpleActivity_1003OutgoingLinks(View view) {
    SimpleActivity modelElement = (SimpleActivity) view.getElement();
    List result = new LinkedList();
    result.addAll(getOutgoingTypeModelFacetLinks_DiagramArc_3001(modelElement));
    return result;
  }

  /**
   * @generated
   */
  public static List getSubprocess_1004OutgoingLinks(View view) {
    Subprocess modelElement = (Subprocess) view.getElement();
    List result = new LinkedList();
    result.addAll(getOutgoingTypeModelFacetLinks_DiagramArc_3001(modelElement));
    return result;
  }

  /**
   * @generated
   */
  public static List getEvent_1005OutgoingLinks(View view) {
    Event modelElement = (Event) view.getElement();
    List result = new LinkedList();
    result.addAll(getOutgoingTypeModelFacetLinks_DiagramArc_3001(modelElement));
    return result;
  }

  /**
   * @generated
   */
  public static List getSplitNode_1006OutgoingLinks(View view) {
    SplitNode modelElement = (SplitNode) view.getElement();
    List result = new LinkedList();
    result.addAll(getOutgoingTypeModelFacetLinks_DiagramArc_3001(modelElement));
    return result;
  }

  /**
   * @generated
   */
  public static List getMergeNode_1007OutgoingLinks(View view) {
    MergeNode modelElement = (MergeNode) view.getElement();
    List result = new LinkedList();
    result.addAll(getOutgoingTypeModelFacetLinks_DiagramArc_3001(modelElement));
    return result;
  }

  /**
   * @generated
   */
  public static List getFlow_1008OutgoingLinks(View view) {
    Flow modelElement = (Flow) view.getElement();
    List result = new LinkedList();
    result.addAll(getOutgoingTypeModelFacetLinks_DiagramArc_3001(modelElement));
    return result;
  }

  /**
   * @generated
   */
  public static List getTransientResource_1009OutgoingLinks(View view) {
    TransientResource modelElement = (TransientResource) view.getElement();
    List result = new LinkedList();
    result.addAll(getOutgoingTypeModelFacetLinks_DiagramArc_3001(modelElement));
    return result;
  }

  /**
   * @generated
   */
  public static List getDocument_1010OutgoingLinks(View view) {
    Document modelElement = (Document) view.getElement();
    List result = new LinkedList();
    result.addAll(getOutgoingTypeModelFacetLinks_DiagramArc_3001(modelElement));
    return result;
  }

  /**
   * @generated
   */
  public static List getPersistentResource_1011OutgoingLinks(View view) {
    PersistentResource modelElement = (PersistentResource) view.getElement();
    List result = new LinkedList();
    result.addAll(getOutgoingTypeModelFacetLinks_DiagramArc_3001(modelElement));
    return result;
  }

  /**
   * @generated
   */
  public static List getStartNode_2001OutgoingLinks(View view) {
    StartNode modelElement = (StartNode) view.getElement();
    List result = new LinkedList();
    result.addAll(getOutgoingTypeModelFacetLinks_DiagramArc_3001(modelElement));
    return result;
  }

  /**
   * @generated
   */
  public static List getEndnode_2002OutgoingLinks(View view) {
    Endnode modelElement = (Endnode) view.getElement();
    List result = new LinkedList();
    result.addAll(getOutgoingTypeModelFacetLinks_DiagramArc_3001(modelElement));
    return result;
  }

  /**
   * @generated
   */
  public static List getSimpleActivity_2003OutgoingLinks(View view) {
    SimpleActivity modelElement = (SimpleActivity) view.getElement();
    List result = new LinkedList();
    result.addAll(getOutgoingTypeModelFacetLinks_DiagramArc_3001(modelElement));
    return result;
  }

  /**
   * @generated
   */
  public static List getSubprocess_2004OutgoingLinks(View view) {
    Subprocess modelElement = (Subprocess) view.getElement();
    List result = new LinkedList();
    result.addAll(getOutgoingTypeModelFacetLinks_DiagramArc_3001(modelElement));
    return result;
  }

  /**
   * @generated
   */
  public static List getEvent_2005OutgoingLinks(View view) {
    Event modelElement = (Event) view.getElement();
    List result = new LinkedList();
    result.addAll(getOutgoingTypeModelFacetLinks_DiagramArc_3001(modelElement));
    return result;
  }

  /**
   * @generated
   */
  public static List getSplitNode_2006OutgoingLinks(View view) {
    SplitNode modelElement = (SplitNode) view.getElement();
    List result = new LinkedList();
    result.addAll(getOutgoingTypeModelFacetLinks_DiagramArc_3001(modelElement));
    return result;
  }

  /**
   * @generated
   */
  public static List getMergeNode_2007OutgoingLinks(View view) {
    MergeNode modelElement = (MergeNode) view.getElement();
    List result = new LinkedList();
    result.addAll(getOutgoingTypeModelFacetLinks_DiagramArc_3001(modelElement));
    return result;
  }

  /**
   * @generated
   */
  public static List getFlow_2008OutgoingLinks(View view) {
    Flow modelElement = (Flow) view.getElement();
    List result = new LinkedList();
    result.addAll(getOutgoingTypeModelFacetLinks_DiagramArc_3001(modelElement));
    return result;
  }

  /**
   * @generated
   */
  public static List getTransientResource_2009OutgoingLinks(View view) {
    TransientResource modelElement = (TransientResource) view.getElement();
    List result = new LinkedList();
    result.addAll(getOutgoingTypeModelFacetLinks_DiagramArc_3001(modelElement));
    return result;
  }

  /**
   * @generated
   */
  public static List getDocument_2010OutgoingLinks(View view) {
    Document modelElement = (Document) view.getElement();
    List result = new LinkedList();
    result.addAll(getOutgoingTypeModelFacetLinks_DiagramArc_3001(modelElement));
    return result;
  }

  /**
   * @generated
   */
  public static List getPersistentResource_2011OutgoingLinks(View view) {
    PersistentResource modelElement = (PersistentResource) view.getElement();
    List result = new LinkedList();
    result.addAll(getOutgoingTypeModelFacetLinks_DiagramArc_3001(modelElement));
    return result;
  }

  /**
   * @generated
   */
  public static List getDiagramArc_3001OutgoingLinks(View view) {
    return Collections.EMPTY_LIST;
  }

  /**
   * @generated
   */
  private static Collection getContainedTypeModelFacetLinks_DiagramArc_3001(
      WorkflowDiagram container) {
    Collection result = new LinkedList();
    for (Iterator links = container.getDiagramArcs().iterator(); links
        .hasNext();) {
      EObject linkObject = (EObject) links.next();
      if (false == linkObject instanceof DiagramArc) {
        continue;
      }
      DiagramArc link = (DiagramArc) linkObject;
      if (DiagramArcEditPart.VISUAL_ID != FlowchartVisualIDRegistry
          .getLinkWithClassVisualID(link)) {
        continue;
      }
      DiagramNode dst = link.getTarget();
      DiagramNode src = link.getSrc();
      result.add(new FlowchartLinkDescriptor(src, dst, link,
          FlowchartElementTypes.DiagramArc_3001, DiagramArcEditPart.VISUAL_ID));
    }
    return result;
  }

  /**
   * @generated
   */
  private static Collection getIncomingTypeModelFacetLinks_DiagramArc_3001(
      DiagramNode target, Map crossReferences) {
    Collection result = new LinkedList();
    Collection settings = (Collection) crossReferences.get(target);
    for (Iterator it = settings.iterator(); it.hasNext();) {
      EStructuralFeature.Setting setting = (EStructuralFeature.Setting) it
          .next();
      if (setting.getEStructuralFeature() != FlowPackage.eINSTANCE
          .getDiagramArc_Target()
          || false == setting.getEObject() instanceof DiagramArc) {
        continue;
      }
      DiagramArc link = (DiagramArc) setting.getEObject();
      if (DiagramArcEditPart.VISUAL_ID != FlowchartVisualIDRegistry
          .getLinkWithClassVisualID(link)) {
        continue;
      }
      DiagramNode src = link.getSrc();
      result.add(new FlowchartLinkDescriptor(src, target, link,
          FlowchartElementTypes.DiagramArc_3001, DiagramArcEditPart.VISUAL_ID));
    }
    return result;
  }

  /**
   * @generated
   */
  private static Collection getOutgoingTypeModelFacetLinks_DiagramArc_3001(
      DiagramNode source) {
    WorkflowDiagram container = null;
    // Find container element for the link.
    // Climb up by containment hierarchy starting from the source
    // and return the first element that is instance of the container class.
    for (EObject element = source; element != null && container == null; element = element
        .eContainer()) {
      if (element instanceof WorkflowDiagram) {
        container = (WorkflowDiagram) element;
      }
    }
    if (container == null) {
      return Collections.EMPTY_LIST;
    }
    Collection result = new LinkedList();
    for (Iterator links = container.getDiagramArcs().iterator(); links
        .hasNext();) {
      EObject linkObject = (EObject) links.next();
      if (false == linkObject instanceof DiagramArc) {
        continue;
      }
      DiagramArc link = (DiagramArc) linkObject;
      if (DiagramArcEditPart.VISUAL_ID != FlowchartVisualIDRegistry
          .getLinkWithClassVisualID(link)) {
        continue;
      }
      DiagramNode dst = link.getTarget();
      DiagramNode src = link.getSrc();
      if (src != source) {
        continue;
      }
      result.add(new FlowchartLinkDescriptor(src, dst, link,
          FlowchartElementTypes.DiagramArc_3001, DiagramArcEditPart.VISUAL_ID));
    }
    return result;
  }

}
