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
import hub.top.lang.flowcharts.diagram.edit.parts.DiagramArcEditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.Document2EditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.DocumentEditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.Endnode2EditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.EndnodeEditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.Event2EditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.EventEditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.Flow2EditPart;
import hub.top.lang.flowcharts.diagram.edit.parts.FlowEditPart;
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
import hub.top.lang.flowcharts.diagram.part.FlowchartDiagramUpdater;
import hub.top.lang.flowcharts.diagram.part.FlowchartLinkDescriptor;
import hub.top.lang.flowcharts.diagram.part.FlowchartNodeDescriptor;
import hub.top.lang.flowcharts.diagram.part.FlowchartVisualIDRegistry;

import java.util.Collection;
import java.util.Collections;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.eclipse.core.runtime.IAdaptable;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.gef.EditPart;
import org.eclipse.gef.commands.Command;
import org.eclipse.gmf.runtime.diagram.core.util.ViewUtil;
import org.eclipse.gmf.runtime.diagram.ui.commands.DeferredLayoutCommand;
import org.eclipse.gmf.runtime.diagram.ui.commands.ICommandProxy;
import org.eclipse.gmf.runtime.diagram.ui.editparts.IGraphicalEditPart;
import org.eclipse.gmf.runtime.diagram.ui.editpolicies.CanonicalConnectionEditPolicy;
import org.eclipse.gmf.runtime.diagram.ui.requests.CreateConnectionViewRequest;
import org.eclipse.gmf.runtime.diagram.ui.requests.RequestConstants;
import org.eclipse.gmf.runtime.notation.Diagram;
import org.eclipse.gmf.runtime.notation.Edge;
import org.eclipse.gmf.runtime.notation.View;

/**
 * @generated
 */
public class WorkflowDiagramCanonicalEditPolicy extends
    CanonicalConnectionEditPolicy {

  /**
   * @generated
   */
  Set myFeaturesToSynchronize;

  /**
   * @generated
   */
  protected List getSemanticChildrenList() {
    View viewObject = (View) getHost().getModel();
    List result = new LinkedList();
    for (Iterator it = FlowchartDiagramUpdater
        .getWorkflowDiagram_1000SemanticChildren(viewObject).iterator(); it
        .hasNext();) {
      result.add(((FlowchartNodeDescriptor) it.next()).getModelElement());
    }
    return result;
  }

  /**
   * @generated
   */
  protected boolean shouldDeleteView(View view) {
    return true;
  }

  /**
   * @generated
   */
  protected boolean isOrphaned(Collection semanticChildren, final View view) {
    int visualID = FlowchartVisualIDRegistry.getVisualID(view);
    switch (visualID) {
    case StartNodeEditPart.VISUAL_ID:
    case EndnodeEditPart.VISUAL_ID:
    case SimpleActivityEditPart.VISUAL_ID:
    case SubprocessEditPart.VISUAL_ID:
    case EventEditPart.VISUAL_ID:
    case SplitNodeEditPart.VISUAL_ID:
    case MergeNodeEditPart.VISUAL_ID:
    case FlowEditPart.VISUAL_ID:
    case TransientResourceEditPart.VISUAL_ID:
    case DocumentEditPart.VISUAL_ID:
    case PersistentResourceEditPart.VISUAL_ID:
      if (!semanticChildren.contains(view.getElement())) {
        return true;
      }
    }
    return false;
  }

  /**
   * @generated
   */
  protected String getDefaultFactoryHint() {
    return null;
  }

  /**
   * @generated
   */
  protected Set getFeaturesToSynchronize() {
    if (myFeaturesToSynchronize == null) {
      myFeaturesToSynchronize = new HashSet();
      myFeaturesToSynchronize.add(FlowPackage.eINSTANCE
          .getWorkflowDiagram_DiagramNodes());
    }
    return myFeaturesToSynchronize;
  }

  /**
   * @generated
   */
  protected List getSemanticConnectionsList() {
    return Collections.EMPTY_LIST;
  }

  /**
   * @generated
   */
  protected EObject getSourceElement(EObject relationship) {
    return null;
  }

  /**
   * @generated
   */
  protected EObject getTargetElement(EObject relationship) {
    return null;
  }

  /**
   * @generated
   */
  protected boolean shouldIncludeConnection(Edge connector, Collection children) {
    return false;
  }

  /**
   * @generated
   */
  protected void refreshSemantic() {
    List createdViews = new LinkedList();
    createdViews.addAll(refreshSemanticChildren());
    List createdConnectionViews = new LinkedList();
    createdConnectionViews.addAll(refreshSemanticConnections());
    createdConnectionViews.addAll(refreshConnections());

    if (createdViews.size() > 1) {
      // perform a layout of the container
      DeferredLayoutCommand layoutCmd = new DeferredLayoutCommand(host()
          .getEditingDomain(), createdViews, host());
      executeCommand(new ICommandProxy(layoutCmd));
    }

    createdViews.addAll(createdConnectionViews);
    makeViewsImmutable(createdViews);
  }

  /**
   * @generated
   */
  private Diagram getDiagram() {
    return ((View) getHost().getModel()).getDiagram();
  }

  /**
   * @generated
   */
  private Collection refreshConnections() {
    Map domain2NotationMap = new HashMap();
    Collection linkDescriptors = collectAllLinks(getDiagram(),
        domain2NotationMap);
    Collection existingLinks = new LinkedList(getDiagram().getEdges());
    for (Iterator linksIterator = existingLinks.iterator(); linksIterator
        .hasNext();) {
      Edge nextDiagramLink = (Edge) linksIterator.next();
      int diagramLinkVisualID = FlowchartVisualIDRegistry
          .getVisualID(nextDiagramLink);
      if (diagramLinkVisualID == -1) {
        if (nextDiagramLink.getSource() != null
            && nextDiagramLink.getTarget() != null) {
          linksIterator.remove();
        }
        continue;
      }
      EObject diagramLinkObject = nextDiagramLink.getElement();
      EObject diagramLinkSrc = nextDiagramLink.getSource().getElement();
      EObject diagramLinkDst = nextDiagramLink.getTarget().getElement();
      for (Iterator LinkDescriptorsIterator = linkDescriptors.iterator(); LinkDescriptorsIterator
          .hasNext();) {
        FlowchartLinkDescriptor nextLinkDescriptor = (FlowchartLinkDescriptor) LinkDescriptorsIterator
            .next();
        if (diagramLinkObject == nextLinkDescriptor.getModelElement()
            && diagramLinkSrc == nextLinkDescriptor.getSource()
            && diagramLinkDst == nextLinkDescriptor.getDestination()
            && diagramLinkVisualID == nextLinkDescriptor.getVisualID()) {
          linksIterator.remove();
          LinkDescriptorsIterator.remove();
        }
      }
    }
    deleteViews(existingLinks.iterator());
    return createConnections(linkDescriptors, domain2NotationMap);
  }

  /**
   * @generated
   */
  private Collection collectAllLinks(View view, Map domain2NotationMap) {
    if (!WorkflowDiagramEditPart.MODEL_ID.equals(FlowchartVisualIDRegistry
        .getModelID(view))) {
      return Collections.EMPTY_LIST;
    }
    Collection result = new LinkedList();
    switch (FlowchartVisualIDRegistry.getVisualID(view)) {
    case WorkflowDiagramEditPart.VISUAL_ID: {
      if (!domain2NotationMap.containsKey(view.getElement())) {
        result.addAll(FlowchartDiagramUpdater
            .getWorkflowDiagram_1000ContainedLinks(view));
      }
      if (!domain2NotationMap.containsKey(view.getElement())
          || view.getEAnnotation("Shortcut") == null) { //$NON-NLS-1$
        domain2NotationMap.put(view.getElement(), view);
      }
      break;
    }
    case StartNodeEditPart.VISUAL_ID: {
      if (!domain2NotationMap.containsKey(view.getElement())) {
        result.addAll(FlowchartDiagramUpdater
            .getStartNode_2001ContainedLinks(view));
      }
      if (!domain2NotationMap.containsKey(view.getElement())
          || view.getEAnnotation("Shortcut") == null) { //$NON-NLS-1$
        domain2NotationMap.put(view.getElement(), view);
      }
      break;
    }
    case EndnodeEditPart.VISUAL_ID: {
      if (!domain2NotationMap.containsKey(view.getElement())) {
        result.addAll(FlowchartDiagramUpdater
            .getEndnode_2002ContainedLinks(view));
      }
      if (!domain2NotationMap.containsKey(view.getElement())
          || view.getEAnnotation("Shortcut") == null) { //$NON-NLS-1$
        domain2NotationMap.put(view.getElement(), view);
      }
      break;
    }
    case SimpleActivityEditPart.VISUAL_ID: {
      if (!domain2NotationMap.containsKey(view.getElement())) {
        result.addAll(FlowchartDiagramUpdater
            .getSimpleActivity_2003ContainedLinks(view));
      }
      if (!domain2NotationMap.containsKey(view.getElement())
          || view.getEAnnotation("Shortcut") == null) { //$NON-NLS-1$
        domain2NotationMap.put(view.getElement(), view);
      }
      break;
    }
    case SubprocessEditPart.VISUAL_ID: {
      if (!domain2NotationMap.containsKey(view.getElement())) {
        result.addAll(FlowchartDiagramUpdater
            .getSubprocess_2004ContainedLinks(view));
      }
      if (!domain2NotationMap.containsKey(view.getElement())
          || view.getEAnnotation("Shortcut") == null) { //$NON-NLS-1$
        domain2NotationMap.put(view.getElement(), view);
      }
      break;
    }
    case EventEditPart.VISUAL_ID: {
      if (!domain2NotationMap.containsKey(view.getElement())) {
        result
            .addAll(FlowchartDiagramUpdater.getEvent_2005ContainedLinks(view));
      }
      if (!domain2NotationMap.containsKey(view.getElement())
          || view.getEAnnotation("Shortcut") == null) { //$NON-NLS-1$
        domain2NotationMap.put(view.getElement(), view);
      }
      break;
    }
    case SplitNodeEditPart.VISUAL_ID: {
      if (!domain2NotationMap.containsKey(view.getElement())) {
        result.addAll(FlowchartDiagramUpdater
            .getSplitNode_2006ContainedLinks(view));
      }
      if (!domain2NotationMap.containsKey(view.getElement())
          || view.getEAnnotation("Shortcut") == null) { //$NON-NLS-1$
        domain2NotationMap.put(view.getElement(), view);
      }
      break;
    }
    case MergeNodeEditPart.VISUAL_ID: {
      if (!domain2NotationMap.containsKey(view.getElement())) {
        result.addAll(FlowchartDiagramUpdater
            .getMergeNode_2007ContainedLinks(view));
      }
      if (!domain2NotationMap.containsKey(view.getElement())
          || view.getEAnnotation("Shortcut") == null) { //$NON-NLS-1$
        domain2NotationMap.put(view.getElement(), view);
      }
      break;
    }
    case FlowEditPart.VISUAL_ID: {
      if (!domain2NotationMap.containsKey(view.getElement())) {
        result.addAll(FlowchartDiagramUpdater.getFlow_2008ContainedLinks(view));
      }
      if (!domain2NotationMap.containsKey(view.getElement())
          || view.getEAnnotation("Shortcut") == null) { //$NON-NLS-1$
        domain2NotationMap.put(view.getElement(), view);
      }
      break;
    }
    case TransientResourceEditPart.VISUAL_ID: {
      if (!domain2NotationMap.containsKey(view.getElement())) {
        result.addAll(FlowchartDiagramUpdater
            .getTransientResource_2009ContainedLinks(view));
      }
      if (!domain2NotationMap.containsKey(view.getElement())
          || view.getEAnnotation("Shortcut") == null) { //$NON-NLS-1$
        domain2NotationMap.put(view.getElement(), view);
      }
      break;
    }
    case DocumentEditPart.VISUAL_ID: {
      if (!domain2NotationMap.containsKey(view.getElement())) {
        result.addAll(FlowchartDiagramUpdater
            .getDocument_2010ContainedLinks(view));
      }
      if (!domain2NotationMap.containsKey(view.getElement())
          || view.getEAnnotation("Shortcut") == null) { //$NON-NLS-1$
        domain2NotationMap.put(view.getElement(), view);
      }
      break;
    }
    case PersistentResourceEditPart.VISUAL_ID: {
      if (!domain2NotationMap.containsKey(view.getElement())) {
        result.addAll(FlowchartDiagramUpdater
            .getPersistentResource_2011ContainedLinks(view));
      }
      if (!domain2NotationMap.containsKey(view.getElement())
          || view.getEAnnotation("Shortcut") == null) { //$NON-NLS-1$
        domain2NotationMap.put(view.getElement(), view);
      }
      break;
    }
    case StartNode2EditPart.VISUAL_ID: {
      if (!domain2NotationMap.containsKey(view.getElement())) {
        result.addAll(FlowchartDiagramUpdater
            .getStartNode_3001ContainedLinks(view));
      }
      if (!domain2NotationMap.containsKey(view.getElement())
          || view.getEAnnotation("Shortcut") == null) { //$NON-NLS-1$
        domain2NotationMap.put(view.getElement(), view);
      }
      break;
    }
    case Endnode2EditPart.VISUAL_ID: {
      if (!domain2NotationMap.containsKey(view.getElement())) {
        result.addAll(FlowchartDiagramUpdater
            .getEndnode_3002ContainedLinks(view));
      }
      if (!domain2NotationMap.containsKey(view.getElement())
          || view.getEAnnotation("Shortcut") == null) { //$NON-NLS-1$
        domain2NotationMap.put(view.getElement(), view);
      }
      break;
    }
    case SimpleActivity2EditPart.VISUAL_ID: {
      if (!domain2NotationMap.containsKey(view.getElement())) {
        result.addAll(FlowchartDiagramUpdater
            .getSimpleActivity_3003ContainedLinks(view));
      }
      if (!domain2NotationMap.containsKey(view.getElement())
          || view.getEAnnotation("Shortcut") == null) { //$NON-NLS-1$
        domain2NotationMap.put(view.getElement(), view);
      }
      break;
    }
    case Subprocess2EditPart.VISUAL_ID: {
      if (!domain2NotationMap.containsKey(view.getElement())) {
        result.addAll(FlowchartDiagramUpdater
            .getSubprocess_3004ContainedLinks(view));
      }
      if (!domain2NotationMap.containsKey(view.getElement())
          || view.getEAnnotation("Shortcut") == null) { //$NON-NLS-1$
        domain2NotationMap.put(view.getElement(), view);
      }
      break;
    }
    case Event2EditPart.VISUAL_ID: {
      if (!domain2NotationMap.containsKey(view.getElement())) {
        result
            .addAll(FlowchartDiagramUpdater.getEvent_3005ContainedLinks(view));
      }
      if (!domain2NotationMap.containsKey(view.getElement())
          || view.getEAnnotation("Shortcut") == null) { //$NON-NLS-1$
        domain2NotationMap.put(view.getElement(), view);
      }
      break;
    }
    case SplitNode2EditPart.VISUAL_ID: {
      if (!domain2NotationMap.containsKey(view.getElement())) {
        result.addAll(FlowchartDiagramUpdater
            .getSplitNode_3006ContainedLinks(view));
      }
      if (!domain2NotationMap.containsKey(view.getElement())
          || view.getEAnnotation("Shortcut") == null) { //$NON-NLS-1$
        domain2NotationMap.put(view.getElement(), view);
      }
      break;
    }
    case MergeNode2EditPart.VISUAL_ID: {
      if (!domain2NotationMap.containsKey(view.getElement())) {
        result.addAll(FlowchartDiagramUpdater
            .getMergeNode_3007ContainedLinks(view));
      }
      if (!domain2NotationMap.containsKey(view.getElement())
          || view.getEAnnotation("Shortcut") == null) { //$NON-NLS-1$
        domain2NotationMap.put(view.getElement(), view);
      }
      break;
    }
    case Flow2EditPart.VISUAL_ID: {
      if (!domain2NotationMap.containsKey(view.getElement())) {
        result.addAll(FlowchartDiagramUpdater.getFlow_3008ContainedLinks(view));
      }
      if (!domain2NotationMap.containsKey(view.getElement())
          || view.getEAnnotation("Shortcut") == null) { //$NON-NLS-1$
        domain2NotationMap.put(view.getElement(), view);
      }
      break;
    }
    case TransientResource2EditPart.VISUAL_ID: {
      if (!domain2NotationMap.containsKey(view.getElement())) {
        result.addAll(FlowchartDiagramUpdater
            .getTransientResource_3009ContainedLinks(view));
      }
      if (!domain2NotationMap.containsKey(view.getElement())
          || view.getEAnnotation("Shortcut") == null) { //$NON-NLS-1$
        domain2NotationMap.put(view.getElement(), view);
      }
      break;
    }
    case Document2EditPart.VISUAL_ID: {
      if (!domain2NotationMap.containsKey(view.getElement())) {
        result.addAll(FlowchartDiagramUpdater
            .getDocument_3010ContainedLinks(view));
      }
      if (!domain2NotationMap.containsKey(view.getElement())
          || view.getEAnnotation("Shortcut") == null) { //$NON-NLS-1$
        domain2NotationMap.put(view.getElement(), view);
      }
      break;
    }
    case PersistentResource2EditPart.VISUAL_ID: {
      if (!domain2NotationMap.containsKey(view.getElement())) {
        result.addAll(FlowchartDiagramUpdater
            .getPersistentResource_3011ContainedLinks(view));
      }
      if (!domain2NotationMap.containsKey(view.getElement())
          || view.getEAnnotation("Shortcut") == null) { //$NON-NLS-1$
        domain2NotationMap.put(view.getElement(), view);
      }
      break;
    }
    case DiagramArcEditPart.VISUAL_ID: {
      if (!domain2NotationMap.containsKey(view.getElement())) {
        result.addAll(FlowchartDiagramUpdater
            .getDiagramArc_4001ContainedLinks(view));
      }
      if (!domain2NotationMap.containsKey(view.getElement())
          || view.getEAnnotation("Shortcut") == null) { //$NON-NLS-1$
        domain2NotationMap.put(view.getElement(), view);
      }
      break;
    }
    }
    for (Iterator children = view.getChildren().iterator(); children.hasNext();) {
      result
          .addAll(collectAllLinks((View) children.next(), domain2NotationMap));
    }
    for (Iterator edges = view.getSourceEdges().iterator(); edges.hasNext();) {
      result.addAll(collectAllLinks((View) edges.next(), domain2NotationMap));
    }
    return result;
  }

  /**
   * @generated
   */
  private Collection createConnections(Collection linkDescriptors,
      Map domain2NotationMap) {
    List adapters = new LinkedList();
    for (Iterator linkDescriptorsIterator = linkDescriptors.iterator(); linkDescriptorsIterator
        .hasNext();) {
      final FlowchartLinkDescriptor nextLinkDescriptor = (FlowchartLinkDescriptor) linkDescriptorsIterator
          .next();
      EditPart sourceEditPart = getEditPart(nextLinkDescriptor.getSource(),
          domain2NotationMap);
      EditPart targetEditPart = getEditPart(
          nextLinkDescriptor.getDestination(), domain2NotationMap);
      if (sourceEditPart == null || targetEditPart == null) {
        continue;
      }
      CreateConnectionViewRequest.ConnectionViewDescriptor descriptor = new CreateConnectionViewRequest.ConnectionViewDescriptor(
          nextLinkDescriptor.getSemanticAdapter(), null, ViewUtil.APPEND,
          false, ((IGraphicalEditPart) getHost()).getDiagramPreferencesHint());
      CreateConnectionViewRequest ccr = new CreateConnectionViewRequest(
          descriptor);
      ccr.setType(RequestConstants.REQ_CONNECTION_START);
      ccr.setSourceEditPart(sourceEditPart);
      sourceEditPart.getCommand(ccr);
      ccr.setTargetEditPart(targetEditPart);
      ccr.setType(RequestConstants.REQ_CONNECTION_END);
      Command cmd = targetEditPart.getCommand(ccr);
      if (cmd != null && cmd.canExecute()) {
        executeCommand(cmd);
        IAdaptable viewAdapter = (IAdaptable) ccr.getNewObject();
        if (viewAdapter != null) {
          adapters.add(viewAdapter);
        }
      }
    }
    return adapters;
  }

  /**
   * @generated
   */
  private EditPart getEditPart(EObject domainModelElement,
      Map domain2NotationMap) {
    View view = (View) domain2NotationMap.get(domainModelElement);
    if (view != null) {
      return (EditPart) getHost().getViewer().getEditPartRegistry().get(view);
    }
    return null;
  }
}
