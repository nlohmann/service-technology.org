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
package hub.top.lang.flowcharts.diagram.navigator;

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
import hub.top.lang.flowcharts.diagram.part.FlowchartVisualIDRegistry;
import hub.top.lang.flowcharts.diagram.part.Messages;

import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.HashMap;
import java.util.Iterator;

import org.eclipse.core.resources.IFile;
import org.eclipse.emf.common.util.URI;
import org.eclipse.emf.ecore.resource.Resource;
import org.eclipse.emf.edit.domain.AdapterFactoryEditingDomain;
import org.eclipse.emf.transaction.TransactionalEditingDomain;
import org.eclipse.emf.workspace.util.WorkspaceSynchronizer;
import org.eclipse.gmf.runtime.emf.core.GMFEditingDomainFactory;
import org.eclipse.gmf.runtime.notation.Diagram;
import org.eclipse.gmf.runtime.notation.Edge;
import org.eclipse.gmf.runtime.notation.View;
import org.eclipse.jface.viewers.Viewer;
import org.eclipse.ui.IMemento;
import org.eclipse.ui.navigator.ICommonContentExtensionSite;
import org.eclipse.ui.navigator.ICommonContentProvider;

/**
 * @generated
 */
public class FlowchartNavigatorContentProvider implements
    ICommonContentProvider {

  /**
   * @generated
   */
  private static final Object[] EMPTY_ARRAY = new Object[0];

  /**
   * @generated
   */
  private Viewer myViewer;

  /**
   * @generated
   */
  private AdapterFactoryEditingDomain myEditingDomain;

  /**
   * @generated
   */
  private WorkspaceSynchronizer myWorkspaceSynchronizer;

  /**
   * @generated
   */
  private Runnable myViewerRefreshRunnable;

  /**
   * @generated
   */
  public FlowchartNavigatorContentProvider() {
    TransactionalEditingDomain editingDomain = GMFEditingDomainFactory.INSTANCE
        .createEditingDomain();
    myEditingDomain = (AdapterFactoryEditingDomain) editingDomain;
    myEditingDomain.setResourceToReadOnlyMap(new HashMap() {
      public Object get(Object key) {
        if (!containsKey(key)) {
          put(key, Boolean.TRUE);
        }
        return super.get(key);
      }
    });
    myViewerRefreshRunnable = new Runnable() {
      public void run() {
        if (myViewer != null) {
          myViewer.refresh();
        }
      }
    };
    myWorkspaceSynchronizer = new WorkspaceSynchronizer(editingDomain,
        new WorkspaceSynchronizer.Delegate() {
          public void dispose() {
          }

          public boolean handleResourceChanged(final Resource resource) {
            for (Iterator it = myEditingDomain.getResourceSet().getResources()
                .iterator(); it.hasNext();) {
              Resource nextResource = (Resource) it.next();
              nextResource.unload();
            }
            if (myViewer != null) {
              myViewer.getControl().getDisplay().asyncExec(
                  myViewerRefreshRunnable);
            }
            return true;
          }

          public boolean handleResourceDeleted(Resource resource) {
            for (Iterator it = myEditingDomain.getResourceSet().getResources()
                .iterator(); it.hasNext();) {
              Resource nextResource = (Resource) it.next();
              nextResource.unload();
            }
            if (myViewer != null) {
              myViewer.getControl().getDisplay().asyncExec(
                  myViewerRefreshRunnable);
            }
            return true;
          }

          public boolean handleResourceMoved(Resource resource, final URI newURI) {
            for (Iterator it = myEditingDomain.getResourceSet().getResources()
                .iterator(); it.hasNext();) {
              Resource nextResource = (Resource) it.next();
              nextResource.unload();
            }
            if (myViewer != null) {
              myViewer.getControl().getDisplay().asyncExec(
                  myViewerRefreshRunnable);
            }
            return true;
          }
        });
  }

  /**
   * @generated
   */
  public void dispose() {
    myWorkspaceSynchronizer.dispose();
    myWorkspaceSynchronizer = null;
    myViewerRefreshRunnable = null;
    for (Iterator it = myEditingDomain.getResourceSet().getResources()
        .iterator(); it.hasNext();) {
      Resource resource = (Resource) it.next();
      resource.unload();
    }
    ((TransactionalEditingDomain) myEditingDomain).dispose();
    myEditingDomain = null;
  }

  /**
   * @generated
   */
  public void inputChanged(Viewer viewer, Object oldInput, Object newInput) {
    myViewer = viewer;
  }

  /**
   * @generated
   */
  public Object[] getElements(Object inputElement) {
    return getChildren(inputElement);
  }

  /**
   * @generated
   */
  public void restoreState(IMemento aMemento) {
  }

  /**
   * @generated
   */
  public void saveState(IMemento aMemento) {
  }

  /**
   * @generated
   */
  public void init(ICommonContentExtensionSite aConfig) {
  }

  /**
   * @generated
   */
  public Object[] getChildren(Object parentElement) {
    if (parentElement instanceof IFile) {
      IFile file = (IFile) parentElement;
      URI fileURI = URI.createPlatformResourceURI(
          file.getFullPath().toString(), true);
      Resource resource = myEditingDomain.getResourceSet().getResource(fileURI,
          true);
      Collection result = new ArrayList();
      result.addAll(createNavigatorItems(selectViewsByType(resource
          .getContents(), WorkflowDiagramEditPart.MODEL_ID), file, false));
      return result.toArray();
    }

    if (parentElement instanceof FlowchartNavigatorGroup) {
      FlowchartNavigatorGroup group = (FlowchartNavigatorGroup) parentElement;
      return group.getChildren();
    }

    if (parentElement instanceof FlowchartNavigatorItem) {
      FlowchartNavigatorItem navigatorItem = (FlowchartNavigatorItem) parentElement;
      if (navigatorItem.isLeaf() || !isOwnView(navigatorItem.getView())) {
        return EMPTY_ARRAY;
      }
      return getViewChildren(navigatorItem.getView(), parentElement);
    }

    return EMPTY_ARRAY;
  }

  /**
   * @generated
   */
  private Object[] getViewChildren(View view, Object parentElement) {
    switch (FlowchartVisualIDRegistry.getVisualID(view)) {

    case WorkflowDiagramEditPart.VISUAL_ID: {
      Collection result = new ArrayList();
      FlowchartNavigatorGroup links = new FlowchartNavigatorGroup(
          Messages.NavigatorGroupName_WorkflowDiagram_1000_links,
          "icons/linksNavigatorGroup.gif", parentElement); //$NON-NLS-1$
      Collection connectedViews = getChildrenByType(
          Collections.singleton(view), StartNodeEditPart.VISUAL_ID);
      result.addAll(createNavigatorItems(connectedViews, parentElement, false));
      connectedViews = getChildrenByType(Collections.singleton(view),
          EndnodeEditPart.VISUAL_ID);
      result.addAll(createNavigatorItems(connectedViews, parentElement, false));
      connectedViews = getChildrenByType(Collections.singleton(view),
          SimpleActivityEditPart.VISUAL_ID);
      result.addAll(createNavigatorItems(connectedViews, parentElement, false));
      connectedViews = getChildrenByType(Collections.singleton(view),
          SubprocessEditPart.VISUAL_ID);
      result.addAll(createNavigatorItems(connectedViews, parentElement, false));
      connectedViews = getChildrenByType(Collections.singleton(view),
          EventEditPart.VISUAL_ID);
      result.addAll(createNavigatorItems(connectedViews, parentElement, false));
      connectedViews = getChildrenByType(Collections.singleton(view),
          SplitNodeEditPart.VISUAL_ID);
      result.addAll(createNavigatorItems(connectedViews, parentElement, false));
      connectedViews = getChildrenByType(Collections.singleton(view),
          MergeNodeEditPart.VISUAL_ID);
      result.addAll(createNavigatorItems(connectedViews, parentElement, false));
      connectedViews = getChildrenByType(Collections.singleton(view),
          FlowEditPart.VISUAL_ID);
      result.addAll(createNavigatorItems(connectedViews, parentElement, false));
      connectedViews = getChildrenByType(Collections.singleton(view),
          TransientResourceEditPart.VISUAL_ID);
      result.addAll(createNavigatorItems(connectedViews, parentElement, false));
      connectedViews = getChildrenByType(Collections.singleton(view),
          DocumentEditPart.VISUAL_ID);
      result.addAll(createNavigatorItems(connectedViews, parentElement, false));
      connectedViews = getChildrenByType(Collections.singleton(view),
          PersistentResourceEditPart.VISUAL_ID);
      result.addAll(createNavigatorItems(connectedViews, parentElement, false));
      connectedViews = getDiagramLinksByType(Collections.singleton(view),
          DiagramArcEditPart.VISUAL_ID);
      links.addChildren(createNavigatorItems(connectedViews, links, false));
      if (!links.isEmpty()) {
        result.add(links);
      }
      return result.toArray();
    }

    case StartNodeEditPart.VISUAL_ID: {
      Collection result = new ArrayList();
      FlowchartNavigatorGroup incominglinks = new FlowchartNavigatorGroup(
          Messages.NavigatorGroupName_StartNode_2001_incominglinks,
          "icons/incomingLinksNavigatorGroup.gif", parentElement); //$NON-NLS-1$
      FlowchartNavigatorGroup outgoinglinks = new FlowchartNavigatorGroup(
          Messages.NavigatorGroupName_StartNode_2001_outgoinglinks,
          "icons/outgoingLinksNavigatorGroup.gif", parentElement); //$NON-NLS-1$
      Collection connectedViews = getIncomingLinksByType(Collections
          .singleton(view), DiagramArcEditPart.VISUAL_ID);
      incominglinks.addChildren(createNavigatorItems(connectedViews,
          incominglinks, true));
      connectedViews = getOutgoingLinksByType(Collections.singleton(view),
          DiagramArcEditPart.VISUAL_ID);
      outgoinglinks.addChildren(createNavigatorItems(connectedViews,
          outgoinglinks, true));
      if (!incominglinks.isEmpty()) {
        result.add(incominglinks);
      }
      if (!outgoinglinks.isEmpty()) {
        result.add(outgoinglinks);
      }
      return result.toArray();
    }

    case EndnodeEditPart.VISUAL_ID: {
      Collection result = new ArrayList();
      FlowchartNavigatorGroup incominglinks = new FlowchartNavigatorGroup(
          Messages.NavigatorGroupName_Endnode_2002_incominglinks,
          "icons/incomingLinksNavigatorGroup.gif", parentElement); //$NON-NLS-1$
      FlowchartNavigatorGroup outgoinglinks = new FlowchartNavigatorGroup(
          Messages.NavigatorGroupName_Endnode_2002_outgoinglinks,
          "icons/outgoingLinksNavigatorGroup.gif", parentElement); //$NON-NLS-1$
      Collection connectedViews = getIncomingLinksByType(Collections
          .singleton(view), DiagramArcEditPart.VISUAL_ID);
      incominglinks.addChildren(createNavigatorItems(connectedViews,
          incominglinks, true));
      connectedViews = getOutgoingLinksByType(Collections.singleton(view),
          DiagramArcEditPart.VISUAL_ID);
      outgoinglinks.addChildren(createNavigatorItems(connectedViews,
          outgoinglinks, true));
      if (!incominglinks.isEmpty()) {
        result.add(incominglinks);
      }
      if (!outgoinglinks.isEmpty()) {
        result.add(outgoinglinks);
      }
      return result.toArray();
    }

    case SimpleActivityEditPart.VISUAL_ID: {
      Collection result = new ArrayList();
      FlowchartNavigatorGroup incominglinks = new FlowchartNavigatorGroup(
          Messages.NavigatorGroupName_SimpleActivity_2003_incominglinks,
          "icons/incomingLinksNavigatorGroup.gif", parentElement); //$NON-NLS-1$
      FlowchartNavigatorGroup outgoinglinks = new FlowchartNavigatorGroup(
          Messages.NavigatorGroupName_SimpleActivity_2003_outgoinglinks,
          "icons/outgoingLinksNavigatorGroup.gif", parentElement); //$NON-NLS-1$
      Collection connectedViews = getIncomingLinksByType(Collections
          .singleton(view), DiagramArcEditPart.VISUAL_ID);
      incominglinks.addChildren(createNavigatorItems(connectedViews,
          incominglinks, true));
      connectedViews = getOutgoingLinksByType(Collections.singleton(view),
          DiagramArcEditPart.VISUAL_ID);
      outgoinglinks.addChildren(createNavigatorItems(connectedViews,
          outgoinglinks, true));
      if (!incominglinks.isEmpty()) {
        result.add(incominglinks);
      }
      if (!outgoinglinks.isEmpty()) {
        result.add(outgoinglinks);
      }
      return result.toArray();
    }

    case SubprocessEditPart.VISUAL_ID: {
      Collection result = new ArrayList();
      FlowchartNavigatorGroup incominglinks = new FlowchartNavigatorGroup(
          Messages.NavigatorGroupName_Subprocess_2004_incominglinks,
          "icons/incomingLinksNavigatorGroup.gif", parentElement); //$NON-NLS-1$
      FlowchartNavigatorGroup outgoinglinks = new FlowchartNavigatorGroup(
          Messages.NavigatorGroupName_Subprocess_2004_outgoinglinks,
          "icons/outgoingLinksNavigatorGroup.gif", parentElement); //$NON-NLS-1$
      Collection connectedViews = getIncomingLinksByType(Collections
          .singleton(view), DiagramArcEditPart.VISUAL_ID);
      incominglinks.addChildren(createNavigatorItems(connectedViews,
          incominglinks, true));
      connectedViews = getOutgoingLinksByType(Collections.singleton(view),
          DiagramArcEditPart.VISUAL_ID);
      outgoinglinks.addChildren(createNavigatorItems(connectedViews,
          outgoinglinks, true));
      if (!incominglinks.isEmpty()) {
        result.add(incominglinks);
      }
      if (!outgoinglinks.isEmpty()) {
        result.add(outgoinglinks);
      }
      return result.toArray();
    }

    case EventEditPart.VISUAL_ID: {
      Collection result = new ArrayList();
      FlowchartNavigatorGroup incominglinks = new FlowchartNavigatorGroup(
          Messages.NavigatorGroupName_Event_2005_incominglinks,
          "icons/incomingLinksNavigatorGroup.gif", parentElement); //$NON-NLS-1$
      FlowchartNavigatorGroup outgoinglinks = new FlowchartNavigatorGroup(
          Messages.NavigatorGroupName_Event_2005_outgoinglinks,
          "icons/outgoingLinksNavigatorGroup.gif", parentElement); //$NON-NLS-1$
      Collection connectedViews = getIncomingLinksByType(Collections
          .singleton(view), DiagramArcEditPart.VISUAL_ID);
      incominglinks.addChildren(createNavigatorItems(connectedViews,
          incominglinks, true));
      connectedViews = getOutgoingLinksByType(Collections.singleton(view),
          DiagramArcEditPart.VISUAL_ID);
      outgoinglinks.addChildren(createNavigatorItems(connectedViews,
          outgoinglinks, true));
      if (!incominglinks.isEmpty()) {
        result.add(incominglinks);
      }
      if (!outgoinglinks.isEmpty()) {
        result.add(outgoinglinks);
      }
      return result.toArray();
    }

    case SplitNodeEditPart.VISUAL_ID: {
      Collection result = new ArrayList();
      FlowchartNavigatorGroup incominglinks = new FlowchartNavigatorGroup(
          Messages.NavigatorGroupName_SplitNode_2006_incominglinks,
          "icons/incomingLinksNavigatorGroup.gif", parentElement); //$NON-NLS-1$
      FlowchartNavigatorGroup outgoinglinks = new FlowchartNavigatorGroup(
          Messages.NavigatorGroupName_SplitNode_2006_outgoinglinks,
          "icons/outgoingLinksNavigatorGroup.gif", parentElement); //$NON-NLS-1$
      Collection connectedViews = getIncomingLinksByType(Collections
          .singleton(view), DiagramArcEditPart.VISUAL_ID);
      incominglinks.addChildren(createNavigatorItems(connectedViews,
          incominglinks, true));
      connectedViews = getOutgoingLinksByType(Collections.singleton(view),
          DiagramArcEditPart.VISUAL_ID);
      outgoinglinks.addChildren(createNavigatorItems(connectedViews,
          outgoinglinks, true));
      if (!incominglinks.isEmpty()) {
        result.add(incominglinks);
      }
      if (!outgoinglinks.isEmpty()) {
        result.add(outgoinglinks);
      }
      return result.toArray();
    }

    case MergeNodeEditPart.VISUAL_ID: {
      Collection result = new ArrayList();
      FlowchartNavigatorGroup incominglinks = new FlowchartNavigatorGroup(
          Messages.NavigatorGroupName_MergeNode_2007_incominglinks,
          "icons/incomingLinksNavigatorGroup.gif", parentElement); //$NON-NLS-1$
      FlowchartNavigatorGroup outgoinglinks = new FlowchartNavigatorGroup(
          Messages.NavigatorGroupName_MergeNode_2007_outgoinglinks,
          "icons/outgoingLinksNavigatorGroup.gif", parentElement); //$NON-NLS-1$
      Collection connectedViews = getIncomingLinksByType(Collections
          .singleton(view), DiagramArcEditPart.VISUAL_ID);
      incominglinks.addChildren(createNavigatorItems(connectedViews,
          incominglinks, true));
      connectedViews = getOutgoingLinksByType(Collections.singleton(view),
          DiagramArcEditPart.VISUAL_ID);
      outgoinglinks.addChildren(createNavigatorItems(connectedViews,
          outgoinglinks, true));
      if (!incominglinks.isEmpty()) {
        result.add(incominglinks);
      }
      if (!outgoinglinks.isEmpty()) {
        result.add(outgoinglinks);
      }
      return result.toArray();
    }

    case FlowEditPart.VISUAL_ID: {
      Collection result = new ArrayList();
      FlowchartNavigatorGroup incominglinks = new FlowchartNavigatorGroup(
          Messages.NavigatorGroupName_Flow_2008_incominglinks,
          "icons/incomingLinksNavigatorGroup.gif", parentElement); //$NON-NLS-1$
      FlowchartNavigatorGroup outgoinglinks = new FlowchartNavigatorGroup(
          Messages.NavigatorGroupName_Flow_2008_outgoinglinks,
          "icons/outgoingLinksNavigatorGroup.gif", parentElement); //$NON-NLS-1$
      Collection connectedViews = getChildrenByType(
          Collections.singleton(view), FlowFlowCompartmentEditPart.VISUAL_ID);
      connectedViews = getChildrenByType(connectedViews,
          StartNode2EditPart.VISUAL_ID);
      result.addAll(createNavigatorItems(connectedViews, parentElement, false));
      connectedViews = getChildrenByType(Collections.singleton(view),
          FlowFlowCompartmentEditPart.VISUAL_ID);
      connectedViews = getChildrenByType(connectedViews,
          Endnode2EditPart.VISUAL_ID);
      result.addAll(createNavigatorItems(connectedViews, parentElement, false));
      connectedViews = getChildrenByType(Collections.singleton(view),
          FlowFlowCompartmentEditPart.VISUAL_ID);
      connectedViews = getChildrenByType(connectedViews,
          SimpleActivity2EditPart.VISUAL_ID);
      result.addAll(createNavigatorItems(connectedViews, parentElement, false));
      connectedViews = getChildrenByType(Collections.singleton(view),
          FlowFlowCompartmentEditPart.VISUAL_ID);
      connectedViews = getChildrenByType(connectedViews,
          Subprocess2EditPart.VISUAL_ID);
      result.addAll(createNavigatorItems(connectedViews, parentElement, false));
      connectedViews = getChildrenByType(Collections.singleton(view),
          FlowFlowCompartmentEditPart.VISUAL_ID);
      connectedViews = getChildrenByType(connectedViews,
          Event2EditPart.VISUAL_ID);
      result.addAll(createNavigatorItems(connectedViews, parentElement, false));
      connectedViews = getChildrenByType(Collections.singleton(view),
          FlowFlowCompartmentEditPart.VISUAL_ID);
      connectedViews = getChildrenByType(connectedViews,
          SplitNode2EditPart.VISUAL_ID);
      result.addAll(createNavigatorItems(connectedViews, parentElement, false));
      connectedViews = getChildrenByType(Collections.singleton(view),
          FlowFlowCompartmentEditPart.VISUAL_ID);
      connectedViews = getChildrenByType(connectedViews,
          MergeNode2EditPart.VISUAL_ID);
      result.addAll(createNavigatorItems(connectedViews, parentElement, false));
      connectedViews = getChildrenByType(Collections.singleton(view),
          FlowFlowCompartmentEditPart.VISUAL_ID);
      connectedViews = getChildrenByType(connectedViews,
          Flow2EditPart.VISUAL_ID);
      result.addAll(createNavigatorItems(connectedViews, parentElement, false));
      connectedViews = getChildrenByType(Collections.singleton(view),
          FlowFlowCompartmentEditPart.VISUAL_ID);
      connectedViews = getChildrenByType(connectedViews,
          TransientResource2EditPart.VISUAL_ID);
      result.addAll(createNavigatorItems(connectedViews, parentElement, false));
      connectedViews = getChildrenByType(Collections.singleton(view),
          FlowFlowCompartmentEditPart.VISUAL_ID);
      connectedViews = getChildrenByType(connectedViews,
          Document2EditPart.VISUAL_ID);
      result.addAll(createNavigatorItems(connectedViews, parentElement, false));
      connectedViews = getChildrenByType(Collections.singleton(view),
          FlowFlowCompartmentEditPart.VISUAL_ID);
      connectedViews = getChildrenByType(connectedViews,
          PersistentResource2EditPart.VISUAL_ID);
      result.addAll(createNavigatorItems(connectedViews, parentElement, false));
      connectedViews = getIncomingLinksByType(Collections.singleton(view),
          DiagramArcEditPart.VISUAL_ID);
      incominglinks.addChildren(createNavigatorItems(connectedViews,
          incominglinks, true));
      connectedViews = getOutgoingLinksByType(Collections.singleton(view),
          DiagramArcEditPart.VISUAL_ID);
      outgoinglinks.addChildren(createNavigatorItems(connectedViews,
          outgoinglinks, true));
      if (!incominglinks.isEmpty()) {
        result.add(incominglinks);
      }
      if (!outgoinglinks.isEmpty()) {
        result.add(outgoinglinks);
      }
      return result.toArray();
    }

    case TransientResourceEditPart.VISUAL_ID: {
      Collection result = new ArrayList();
      FlowchartNavigatorGroup incominglinks = new FlowchartNavigatorGroup(
          Messages.NavigatorGroupName_TransientResource_2009_incominglinks,
          "icons/incomingLinksNavigatorGroup.gif", parentElement); //$NON-NLS-1$
      FlowchartNavigatorGroup outgoinglinks = new FlowchartNavigatorGroup(
          Messages.NavigatorGroupName_TransientResource_2009_outgoinglinks,
          "icons/outgoingLinksNavigatorGroup.gif", parentElement); //$NON-NLS-1$
      Collection connectedViews = getIncomingLinksByType(Collections
          .singleton(view), DiagramArcEditPart.VISUAL_ID);
      incominglinks.addChildren(createNavigatorItems(connectedViews,
          incominglinks, true));
      connectedViews = getOutgoingLinksByType(Collections.singleton(view),
          DiagramArcEditPart.VISUAL_ID);
      outgoinglinks.addChildren(createNavigatorItems(connectedViews,
          outgoinglinks, true));
      if (!incominglinks.isEmpty()) {
        result.add(incominglinks);
      }
      if (!outgoinglinks.isEmpty()) {
        result.add(outgoinglinks);
      }
      return result.toArray();
    }

    case DocumentEditPart.VISUAL_ID: {
      Collection result = new ArrayList();
      FlowchartNavigatorGroup incominglinks = new FlowchartNavigatorGroup(
          Messages.NavigatorGroupName_Document_2010_incominglinks,
          "icons/incomingLinksNavigatorGroup.gif", parentElement); //$NON-NLS-1$
      FlowchartNavigatorGroup outgoinglinks = new FlowchartNavigatorGroup(
          Messages.NavigatorGroupName_Document_2010_outgoinglinks,
          "icons/outgoingLinksNavigatorGroup.gif", parentElement); //$NON-NLS-1$
      Collection connectedViews = getIncomingLinksByType(Collections
          .singleton(view), DiagramArcEditPart.VISUAL_ID);
      incominglinks.addChildren(createNavigatorItems(connectedViews,
          incominglinks, true));
      connectedViews = getOutgoingLinksByType(Collections.singleton(view),
          DiagramArcEditPart.VISUAL_ID);
      outgoinglinks.addChildren(createNavigatorItems(connectedViews,
          outgoinglinks, true));
      if (!incominglinks.isEmpty()) {
        result.add(incominglinks);
      }
      if (!outgoinglinks.isEmpty()) {
        result.add(outgoinglinks);
      }
      return result.toArray();
    }

    case PersistentResourceEditPart.VISUAL_ID: {
      Collection result = new ArrayList();
      FlowchartNavigatorGroup incominglinks = new FlowchartNavigatorGroup(
          Messages.NavigatorGroupName_PersistentResource_2011_incominglinks,
          "icons/incomingLinksNavigatorGroup.gif", parentElement); //$NON-NLS-1$
      FlowchartNavigatorGroup outgoinglinks = new FlowchartNavigatorGroup(
          Messages.NavigatorGroupName_PersistentResource_2011_outgoinglinks,
          "icons/outgoingLinksNavigatorGroup.gif", parentElement); //$NON-NLS-1$
      Collection connectedViews = getIncomingLinksByType(Collections
          .singleton(view), DiagramArcEditPart.VISUAL_ID);
      incominglinks.addChildren(createNavigatorItems(connectedViews,
          incominglinks, true));
      connectedViews = getOutgoingLinksByType(Collections.singleton(view),
          DiagramArcEditPart.VISUAL_ID);
      outgoinglinks.addChildren(createNavigatorItems(connectedViews,
          outgoinglinks, true));
      if (!incominglinks.isEmpty()) {
        result.add(incominglinks);
      }
      if (!outgoinglinks.isEmpty()) {
        result.add(outgoinglinks);
      }
      return result.toArray();
    }

    case StartNode2EditPart.VISUAL_ID: {
      Collection result = new ArrayList();
      FlowchartNavigatorGroup incominglinks = new FlowchartNavigatorGroup(
          Messages.NavigatorGroupName_StartNode_3001_incominglinks,
          "icons/incomingLinksNavigatorGroup.gif", parentElement); //$NON-NLS-1$
      FlowchartNavigatorGroup outgoinglinks = new FlowchartNavigatorGroup(
          Messages.NavigatorGroupName_StartNode_3001_outgoinglinks,
          "icons/outgoingLinksNavigatorGroup.gif", parentElement); //$NON-NLS-1$
      Collection connectedViews = getIncomingLinksByType(Collections
          .singleton(view), DiagramArcEditPart.VISUAL_ID);
      incominglinks.addChildren(createNavigatorItems(connectedViews,
          incominglinks, true));
      connectedViews = getOutgoingLinksByType(Collections.singleton(view),
          DiagramArcEditPart.VISUAL_ID);
      outgoinglinks.addChildren(createNavigatorItems(connectedViews,
          outgoinglinks, true));
      if (!incominglinks.isEmpty()) {
        result.add(incominglinks);
      }
      if (!outgoinglinks.isEmpty()) {
        result.add(outgoinglinks);
      }
      return result.toArray();
    }

    case Endnode2EditPart.VISUAL_ID: {
      Collection result = new ArrayList();
      FlowchartNavigatorGroup incominglinks = new FlowchartNavigatorGroup(
          Messages.NavigatorGroupName_Endnode_3002_incominglinks,
          "icons/incomingLinksNavigatorGroup.gif", parentElement); //$NON-NLS-1$
      FlowchartNavigatorGroup outgoinglinks = new FlowchartNavigatorGroup(
          Messages.NavigatorGroupName_Endnode_3002_outgoinglinks,
          "icons/outgoingLinksNavigatorGroup.gif", parentElement); //$NON-NLS-1$
      Collection connectedViews = getIncomingLinksByType(Collections
          .singleton(view), DiagramArcEditPart.VISUAL_ID);
      incominglinks.addChildren(createNavigatorItems(connectedViews,
          incominglinks, true));
      connectedViews = getOutgoingLinksByType(Collections.singleton(view),
          DiagramArcEditPart.VISUAL_ID);
      outgoinglinks.addChildren(createNavigatorItems(connectedViews,
          outgoinglinks, true));
      if (!incominglinks.isEmpty()) {
        result.add(incominglinks);
      }
      if (!outgoinglinks.isEmpty()) {
        result.add(outgoinglinks);
      }
      return result.toArray();
    }

    case SimpleActivity2EditPart.VISUAL_ID: {
      Collection result = new ArrayList();
      FlowchartNavigatorGroup incominglinks = new FlowchartNavigatorGroup(
          Messages.NavigatorGroupName_SimpleActivity_3003_incominglinks,
          "icons/incomingLinksNavigatorGroup.gif", parentElement); //$NON-NLS-1$
      FlowchartNavigatorGroup outgoinglinks = new FlowchartNavigatorGroup(
          Messages.NavigatorGroupName_SimpleActivity_3003_outgoinglinks,
          "icons/outgoingLinksNavigatorGroup.gif", parentElement); //$NON-NLS-1$
      Collection connectedViews = getIncomingLinksByType(Collections
          .singleton(view), DiagramArcEditPart.VISUAL_ID);
      incominglinks.addChildren(createNavigatorItems(connectedViews,
          incominglinks, true));
      connectedViews = getOutgoingLinksByType(Collections.singleton(view),
          DiagramArcEditPart.VISUAL_ID);
      outgoinglinks.addChildren(createNavigatorItems(connectedViews,
          outgoinglinks, true));
      if (!incominglinks.isEmpty()) {
        result.add(incominglinks);
      }
      if (!outgoinglinks.isEmpty()) {
        result.add(outgoinglinks);
      }
      return result.toArray();
    }

    case Subprocess2EditPart.VISUAL_ID: {
      Collection result = new ArrayList();
      FlowchartNavigatorGroup incominglinks = new FlowchartNavigatorGroup(
          Messages.NavigatorGroupName_Subprocess_3004_incominglinks,
          "icons/incomingLinksNavigatorGroup.gif", parentElement); //$NON-NLS-1$
      FlowchartNavigatorGroup outgoinglinks = new FlowchartNavigatorGroup(
          Messages.NavigatorGroupName_Subprocess_3004_outgoinglinks,
          "icons/outgoingLinksNavigatorGroup.gif", parentElement); //$NON-NLS-1$
      Collection connectedViews = getIncomingLinksByType(Collections
          .singleton(view), DiagramArcEditPart.VISUAL_ID);
      incominglinks.addChildren(createNavigatorItems(connectedViews,
          incominglinks, true));
      connectedViews = getOutgoingLinksByType(Collections.singleton(view),
          DiagramArcEditPart.VISUAL_ID);
      outgoinglinks.addChildren(createNavigatorItems(connectedViews,
          outgoinglinks, true));
      if (!incominglinks.isEmpty()) {
        result.add(incominglinks);
      }
      if (!outgoinglinks.isEmpty()) {
        result.add(outgoinglinks);
      }
      return result.toArray();
    }

    case Event2EditPart.VISUAL_ID: {
      Collection result = new ArrayList();
      FlowchartNavigatorGroup incominglinks = new FlowchartNavigatorGroup(
          Messages.NavigatorGroupName_Event_3005_incominglinks,
          "icons/incomingLinksNavigatorGroup.gif", parentElement); //$NON-NLS-1$
      FlowchartNavigatorGroup outgoinglinks = new FlowchartNavigatorGroup(
          Messages.NavigatorGroupName_Event_3005_outgoinglinks,
          "icons/outgoingLinksNavigatorGroup.gif", parentElement); //$NON-NLS-1$
      Collection connectedViews = getIncomingLinksByType(Collections
          .singleton(view), DiagramArcEditPart.VISUAL_ID);
      incominglinks.addChildren(createNavigatorItems(connectedViews,
          incominglinks, true));
      connectedViews = getOutgoingLinksByType(Collections.singleton(view),
          DiagramArcEditPart.VISUAL_ID);
      outgoinglinks.addChildren(createNavigatorItems(connectedViews,
          outgoinglinks, true));
      if (!incominglinks.isEmpty()) {
        result.add(incominglinks);
      }
      if (!outgoinglinks.isEmpty()) {
        result.add(outgoinglinks);
      }
      return result.toArray();
    }

    case SplitNode2EditPart.VISUAL_ID: {
      Collection result = new ArrayList();
      FlowchartNavigatorGroup incominglinks = new FlowchartNavigatorGroup(
          Messages.NavigatorGroupName_SplitNode_3006_incominglinks,
          "icons/incomingLinksNavigatorGroup.gif", parentElement); //$NON-NLS-1$
      FlowchartNavigatorGroup outgoinglinks = new FlowchartNavigatorGroup(
          Messages.NavigatorGroupName_SplitNode_3006_outgoinglinks,
          "icons/outgoingLinksNavigatorGroup.gif", parentElement); //$NON-NLS-1$
      Collection connectedViews = getIncomingLinksByType(Collections
          .singleton(view), DiagramArcEditPart.VISUAL_ID);
      incominglinks.addChildren(createNavigatorItems(connectedViews,
          incominglinks, true));
      connectedViews = getOutgoingLinksByType(Collections.singleton(view),
          DiagramArcEditPart.VISUAL_ID);
      outgoinglinks.addChildren(createNavigatorItems(connectedViews,
          outgoinglinks, true));
      if (!incominglinks.isEmpty()) {
        result.add(incominglinks);
      }
      if (!outgoinglinks.isEmpty()) {
        result.add(outgoinglinks);
      }
      return result.toArray();
    }

    case MergeNode2EditPart.VISUAL_ID: {
      Collection result = new ArrayList();
      FlowchartNavigatorGroup incominglinks = new FlowchartNavigatorGroup(
          Messages.NavigatorGroupName_MergeNode_3007_incominglinks,
          "icons/incomingLinksNavigatorGroup.gif", parentElement); //$NON-NLS-1$
      FlowchartNavigatorGroup outgoinglinks = new FlowchartNavigatorGroup(
          Messages.NavigatorGroupName_MergeNode_3007_outgoinglinks,
          "icons/outgoingLinksNavigatorGroup.gif", parentElement); //$NON-NLS-1$
      Collection connectedViews = getIncomingLinksByType(Collections
          .singleton(view), DiagramArcEditPart.VISUAL_ID);
      incominglinks.addChildren(createNavigatorItems(connectedViews,
          incominglinks, true));
      connectedViews = getOutgoingLinksByType(Collections.singleton(view),
          DiagramArcEditPart.VISUAL_ID);
      outgoinglinks.addChildren(createNavigatorItems(connectedViews,
          outgoinglinks, true));
      if (!incominglinks.isEmpty()) {
        result.add(incominglinks);
      }
      if (!outgoinglinks.isEmpty()) {
        result.add(outgoinglinks);
      }
      return result.toArray();
    }

    case Flow2EditPart.VISUAL_ID: {
      Collection result = new ArrayList();
      FlowchartNavigatorGroup incominglinks = new FlowchartNavigatorGroup(
          Messages.NavigatorGroupName_Flow_3008_incominglinks,
          "icons/incomingLinksNavigatorGroup.gif", parentElement); //$NON-NLS-1$
      FlowchartNavigatorGroup outgoinglinks = new FlowchartNavigatorGroup(
          Messages.NavigatorGroupName_Flow_3008_outgoinglinks,
          "icons/outgoingLinksNavigatorGroup.gif", parentElement); //$NON-NLS-1$
      Collection connectedViews = getChildrenByType(
          Collections.singleton(view), FlowFlowCompartment2EditPart.VISUAL_ID);
      connectedViews = getChildrenByType(connectedViews,
          StartNode2EditPart.VISUAL_ID);
      result.addAll(createNavigatorItems(connectedViews, parentElement, false));
      connectedViews = getChildrenByType(Collections.singleton(view),
          FlowFlowCompartment2EditPart.VISUAL_ID);
      connectedViews = getChildrenByType(connectedViews,
          Endnode2EditPart.VISUAL_ID);
      result.addAll(createNavigatorItems(connectedViews, parentElement, false));
      connectedViews = getChildrenByType(Collections.singleton(view),
          FlowFlowCompartment2EditPart.VISUAL_ID);
      connectedViews = getChildrenByType(connectedViews,
          SimpleActivity2EditPart.VISUAL_ID);
      result.addAll(createNavigatorItems(connectedViews, parentElement, false));
      connectedViews = getChildrenByType(Collections.singleton(view),
          FlowFlowCompartment2EditPart.VISUAL_ID);
      connectedViews = getChildrenByType(connectedViews,
          Subprocess2EditPart.VISUAL_ID);
      result.addAll(createNavigatorItems(connectedViews, parentElement, false));
      connectedViews = getChildrenByType(Collections.singleton(view),
          FlowFlowCompartment2EditPart.VISUAL_ID);
      connectedViews = getChildrenByType(connectedViews,
          Event2EditPart.VISUAL_ID);
      result.addAll(createNavigatorItems(connectedViews, parentElement, false));
      connectedViews = getChildrenByType(Collections.singleton(view),
          FlowFlowCompartment2EditPart.VISUAL_ID);
      connectedViews = getChildrenByType(connectedViews,
          SplitNode2EditPart.VISUAL_ID);
      result.addAll(createNavigatorItems(connectedViews, parentElement, false));
      connectedViews = getChildrenByType(Collections.singleton(view),
          FlowFlowCompartment2EditPart.VISUAL_ID);
      connectedViews = getChildrenByType(connectedViews,
          MergeNode2EditPart.VISUAL_ID);
      result.addAll(createNavigatorItems(connectedViews, parentElement, false));
      connectedViews = getChildrenByType(Collections.singleton(view),
          FlowFlowCompartment2EditPart.VISUAL_ID);
      connectedViews = getChildrenByType(connectedViews,
          Flow2EditPart.VISUAL_ID);
      result.addAll(createNavigatorItems(connectedViews, parentElement, false));
      connectedViews = getChildrenByType(Collections.singleton(view),
          FlowFlowCompartment2EditPart.VISUAL_ID);
      connectedViews = getChildrenByType(connectedViews,
          TransientResource2EditPart.VISUAL_ID);
      result.addAll(createNavigatorItems(connectedViews, parentElement, false));
      connectedViews = getChildrenByType(Collections.singleton(view),
          FlowFlowCompartment2EditPart.VISUAL_ID);
      connectedViews = getChildrenByType(connectedViews,
          Document2EditPart.VISUAL_ID);
      result.addAll(createNavigatorItems(connectedViews, parentElement, false));
      connectedViews = getChildrenByType(Collections.singleton(view),
          FlowFlowCompartment2EditPart.VISUAL_ID);
      connectedViews = getChildrenByType(connectedViews,
          PersistentResource2EditPart.VISUAL_ID);
      result.addAll(createNavigatorItems(connectedViews, parentElement, false));
      connectedViews = getIncomingLinksByType(Collections.singleton(view),
          DiagramArcEditPart.VISUAL_ID);
      incominglinks.addChildren(createNavigatorItems(connectedViews,
          incominglinks, true));
      connectedViews = getOutgoingLinksByType(Collections.singleton(view),
          DiagramArcEditPart.VISUAL_ID);
      outgoinglinks.addChildren(createNavigatorItems(connectedViews,
          outgoinglinks, true));
      if (!incominglinks.isEmpty()) {
        result.add(incominglinks);
      }
      if (!outgoinglinks.isEmpty()) {
        result.add(outgoinglinks);
      }
      return result.toArray();
    }

    case TransientResource2EditPart.VISUAL_ID: {
      Collection result = new ArrayList();
      FlowchartNavigatorGroup incominglinks = new FlowchartNavigatorGroup(
          Messages.NavigatorGroupName_TransientResource_3009_incominglinks,
          "icons/incomingLinksNavigatorGroup.gif", parentElement); //$NON-NLS-1$
      FlowchartNavigatorGroup outgoinglinks = new FlowchartNavigatorGroup(
          Messages.NavigatorGroupName_TransientResource_3009_outgoinglinks,
          "icons/outgoingLinksNavigatorGroup.gif", parentElement); //$NON-NLS-1$
      Collection connectedViews = getIncomingLinksByType(Collections
          .singleton(view), DiagramArcEditPart.VISUAL_ID);
      incominglinks.addChildren(createNavigatorItems(connectedViews,
          incominglinks, true));
      connectedViews = getOutgoingLinksByType(Collections.singleton(view),
          DiagramArcEditPart.VISUAL_ID);
      outgoinglinks.addChildren(createNavigatorItems(connectedViews,
          outgoinglinks, true));
      if (!incominglinks.isEmpty()) {
        result.add(incominglinks);
      }
      if (!outgoinglinks.isEmpty()) {
        result.add(outgoinglinks);
      }
      return result.toArray();
    }

    case Document2EditPart.VISUAL_ID: {
      Collection result = new ArrayList();
      FlowchartNavigatorGroup incominglinks = new FlowchartNavigatorGroup(
          Messages.NavigatorGroupName_Document_3010_incominglinks,
          "icons/incomingLinksNavigatorGroup.gif", parentElement); //$NON-NLS-1$
      FlowchartNavigatorGroup outgoinglinks = new FlowchartNavigatorGroup(
          Messages.NavigatorGroupName_Document_3010_outgoinglinks,
          "icons/outgoingLinksNavigatorGroup.gif", parentElement); //$NON-NLS-1$
      Collection connectedViews = getIncomingLinksByType(Collections
          .singleton(view), DiagramArcEditPart.VISUAL_ID);
      incominglinks.addChildren(createNavigatorItems(connectedViews,
          incominglinks, true));
      connectedViews = getOutgoingLinksByType(Collections.singleton(view),
          DiagramArcEditPart.VISUAL_ID);
      outgoinglinks.addChildren(createNavigatorItems(connectedViews,
          outgoinglinks, true));
      if (!incominglinks.isEmpty()) {
        result.add(incominglinks);
      }
      if (!outgoinglinks.isEmpty()) {
        result.add(outgoinglinks);
      }
      return result.toArray();
    }

    case PersistentResource2EditPart.VISUAL_ID: {
      Collection result = new ArrayList();
      FlowchartNavigatorGroup incominglinks = new FlowchartNavigatorGroup(
          Messages.NavigatorGroupName_PersistentResource_3011_incominglinks,
          "icons/incomingLinksNavigatorGroup.gif", parentElement); //$NON-NLS-1$
      FlowchartNavigatorGroup outgoinglinks = new FlowchartNavigatorGroup(
          Messages.NavigatorGroupName_PersistentResource_3011_outgoinglinks,
          "icons/outgoingLinksNavigatorGroup.gif", parentElement); //$NON-NLS-1$
      Collection connectedViews = getIncomingLinksByType(Collections
          .singleton(view), DiagramArcEditPart.VISUAL_ID);
      incominglinks.addChildren(createNavigatorItems(connectedViews,
          incominglinks, true));
      connectedViews = getOutgoingLinksByType(Collections.singleton(view),
          DiagramArcEditPart.VISUAL_ID);
      outgoinglinks.addChildren(createNavigatorItems(connectedViews,
          outgoinglinks, true));
      if (!incominglinks.isEmpty()) {
        result.add(incominglinks);
      }
      if (!outgoinglinks.isEmpty()) {
        result.add(outgoinglinks);
      }
      return result.toArray();
    }

    case DiagramArcEditPart.VISUAL_ID: {
      Collection result = new ArrayList();
      FlowchartNavigatorGroup target = new FlowchartNavigatorGroup(
          Messages.NavigatorGroupName_DiagramArc_4001_target,
          "icons/linkTargetNavigatorGroup.gif", parentElement); //$NON-NLS-1$
      FlowchartNavigatorGroup source = new FlowchartNavigatorGroup(
          Messages.NavigatorGroupName_DiagramArc_4001_source,
          "icons/linkSourceNavigatorGroup.gif", parentElement); //$NON-NLS-1$
      Collection connectedViews = getLinksTargetByType(Collections
          .singleton(view), StartNodeEditPart.VISUAL_ID);
      target.addChildren(createNavigatorItems(connectedViews, target, true));
      connectedViews = getLinksTargetByType(Collections.singleton(view),
          EndnodeEditPart.VISUAL_ID);
      target.addChildren(createNavigatorItems(connectedViews, target, true));
      connectedViews = getLinksTargetByType(Collections.singleton(view),
          SimpleActivityEditPart.VISUAL_ID);
      target.addChildren(createNavigatorItems(connectedViews, target, true));
      connectedViews = getLinksTargetByType(Collections.singleton(view),
          SubprocessEditPart.VISUAL_ID);
      target.addChildren(createNavigatorItems(connectedViews, target, true));
      connectedViews = getLinksTargetByType(Collections.singleton(view),
          EventEditPart.VISUAL_ID);
      target.addChildren(createNavigatorItems(connectedViews, target, true));
      connectedViews = getLinksTargetByType(Collections.singleton(view),
          SplitNodeEditPart.VISUAL_ID);
      target.addChildren(createNavigatorItems(connectedViews, target, true));
      connectedViews = getLinksTargetByType(Collections.singleton(view),
          MergeNodeEditPart.VISUAL_ID);
      target.addChildren(createNavigatorItems(connectedViews, target, true));
      connectedViews = getLinksTargetByType(Collections.singleton(view),
          FlowEditPart.VISUAL_ID);
      target.addChildren(createNavigatorItems(connectedViews, target, true));
      connectedViews = getLinksTargetByType(Collections.singleton(view),
          TransientResourceEditPart.VISUAL_ID);
      target.addChildren(createNavigatorItems(connectedViews, target, true));
      connectedViews = getLinksTargetByType(Collections.singleton(view),
          DocumentEditPart.VISUAL_ID);
      target.addChildren(createNavigatorItems(connectedViews, target, true));
      connectedViews = getLinksTargetByType(Collections.singleton(view),
          PersistentResourceEditPart.VISUAL_ID);
      target.addChildren(createNavigatorItems(connectedViews, target, true));
      connectedViews = getLinksTargetByType(Collections.singleton(view),
          StartNode2EditPart.VISUAL_ID);
      target.addChildren(createNavigatorItems(connectedViews, target, true));
      connectedViews = getLinksTargetByType(Collections.singleton(view),
          Endnode2EditPart.VISUAL_ID);
      target.addChildren(createNavigatorItems(connectedViews, target, true));
      connectedViews = getLinksTargetByType(Collections.singleton(view),
          SimpleActivity2EditPart.VISUAL_ID);
      target.addChildren(createNavigatorItems(connectedViews, target, true));
      connectedViews = getLinksTargetByType(Collections.singleton(view),
          Subprocess2EditPart.VISUAL_ID);
      target.addChildren(createNavigatorItems(connectedViews, target, true));
      connectedViews = getLinksTargetByType(Collections.singleton(view),
          Event2EditPart.VISUAL_ID);
      target.addChildren(createNavigatorItems(connectedViews, target, true));
      connectedViews = getLinksTargetByType(Collections.singleton(view),
          SplitNode2EditPart.VISUAL_ID);
      target.addChildren(createNavigatorItems(connectedViews, target, true));
      connectedViews = getLinksTargetByType(Collections.singleton(view),
          MergeNode2EditPart.VISUAL_ID);
      target.addChildren(createNavigatorItems(connectedViews, target, true));
      connectedViews = getLinksTargetByType(Collections.singleton(view),
          Flow2EditPart.VISUAL_ID);
      target.addChildren(createNavigatorItems(connectedViews, target, true));
      connectedViews = getLinksTargetByType(Collections.singleton(view),
          TransientResource2EditPart.VISUAL_ID);
      target.addChildren(createNavigatorItems(connectedViews, target, true));
      connectedViews = getLinksTargetByType(Collections.singleton(view),
          Document2EditPart.VISUAL_ID);
      target.addChildren(createNavigatorItems(connectedViews, target, true));
      connectedViews = getLinksTargetByType(Collections.singleton(view),
          PersistentResource2EditPart.VISUAL_ID);
      target.addChildren(createNavigatorItems(connectedViews, target, true));
      connectedViews = getLinksSourceByType(Collections.singleton(view),
          StartNodeEditPart.VISUAL_ID);
      source.addChildren(createNavigatorItems(connectedViews, source, true));
      connectedViews = getLinksSourceByType(Collections.singleton(view),
          EndnodeEditPart.VISUAL_ID);
      source.addChildren(createNavigatorItems(connectedViews, source, true));
      connectedViews = getLinksSourceByType(Collections.singleton(view),
          SimpleActivityEditPart.VISUAL_ID);
      source.addChildren(createNavigatorItems(connectedViews, source, true));
      connectedViews = getLinksSourceByType(Collections.singleton(view),
          SubprocessEditPart.VISUAL_ID);
      source.addChildren(createNavigatorItems(connectedViews, source, true));
      connectedViews = getLinksSourceByType(Collections.singleton(view),
          EventEditPart.VISUAL_ID);
      source.addChildren(createNavigatorItems(connectedViews, source, true));
      connectedViews = getLinksSourceByType(Collections.singleton(view),
          SplitNodeEditPart.VISUAL_ID);
      source.addChildren(createNavigatorItems(connectedViews, source, true));
      connectedViews = getLinksSourceByType(Collections.singleton(view),
          MergeNodeEditPart.VISUAL_ID);
      source.addChildren(createNavigatorItems(connectedViews, source, true));
      connectedViews = getLinksSourceByType(Collections.singleton(view),
          FlowEditPart.VISUAL_ID);
      source.addChildren(createNavigatorItems(connectedViews, source, true));
      connectedViews = getLinksSourceByType(Collections.singleton(view),
          TransientResourceEditPart.VISUAL_ID);
      source.addChildren(createNavigatorItems(connectedViews, source, true));
      connectedViews = getLinksSourceByType(Collections.singleton(view),
          DocumentEditPart.VISUAL_ID);
      source.addChildren(createNavigatorItems(connectedViews, source, true));
      connectedViews = getLinksSourceByType(Collections.singleton(view),
          PersistentResourceEditPart.VISUAL_ID);
      source.addChildren(createNavigatorItems(connectedViews, source, true));
      connectedViews = getLinksSourceByType(Collections.singleton(view),
          StartNode2EditPart.VISUAL_ID);
      source.addChildren(createNavigatorItems(connectedViews, source, true));
      connectedViews = getLinksSourceByType(Collections.singleton(view),
          Endnode2EditPart.VISUAL_ID);
      source.addChildren(createNavigatorItems(connectedViews, source, true));
      connectedViews = getLinksSourceByType(Collections.singleton(view),
          SimpleActivity2EditPart.VISUAL_ID);
      source.addChildren(createNavigatorItems(connectedViews, source, true));
      connectedViews = getLinksSourceByType(Collections.singleton(view),
          Subprocess2EditPart.VISUAL_ID);
      source.addChildren(createNavigatorItems(connectedViews, source, true));
      connectedViews = getLinksSourceByType(Collections.singleton(view),
          Event2EditPart.VISUAL_ID);
      source.addChildren(createNavigatorItems(connectedViews, source, true));
      connectedViews = getLinksSourceByType(Collections.singleton(view),
          SplitNode2EditPart.VISUAL_ID);
      source.addChildren(createNavigatorItems(connectedViews, source, true));
      connectedViews = getLinksSourceByType(Collections.singleton(view),
          MergeNode2EditPart.VISUAL_ID);
      source.addChildren(createNavigatorItems(connectedViews, source, true));
      connectedViews = getLinksSourceByType(Collections.singleton(view),
          Flow2EditPart.VISUAL_ID);
      source.addChildren(createNavigatorItems(connectedViews, source, true));
      connectedViews = getLinksSourceByType(Collections.singleton(view),
          TransientResource2EditPart.VISUAL_ID);
      source.addChildren(createNavigatorItems(connectedViews, source, true));
      connectedViews = getLinksSourceByType(Collections.singleton(view),
          Document2EditPart.VISUAL_ID);
      source.addChildren(createNavigatorItems(connectedViews, source, true));
      connectedViews = getLinksSourceByType(Collections.singleton(view),
          PersistentResource2EditPart.VISUAL_ID);
      source.addChildren(createNavigatorItems(connectedViews, source, true));
      if (!target.isEmpty()) {
        result.add(target);
      }
      if (!source.isEmpty()) {
        result.add(source);
      }
      return result.toArray();
    }
    }
    return EMPTY_ARRAY;
  }

  /**
   * @generated
   */
  private Collection getLinksSourceByType(Collection edges, int visualID) {
    Collection result = new ArrayList();
    String type = FlowchartVisualIDRegistry.getType(visualID);
    for (Iterator it = edges.iterator(); it.hasNext();) {
      Edge nextEdge = (Edge) it.next();
      View nextEdgeSource = nextEdge.getSource();
      if (type.equals(nextEdgeSource.getType()) && isOwnView(nextEdgeSource)) {
        result.add(nextEdgeSource);
      }
    }
    return result;
  }

  /**
   * @generated
   */
  private Collection getLinksTargetByType(Collection edges, int visualID) {
    Collection result = new ArrayList();
    String type = FlowchartVisualIDRegistry.getType(visualID);
    for (Iterator it = edges.iterator(); it.hasNext();) {
      Edge nextEdge = (Edge) it.next();
      View nextEdgeTarget = nextEdge.getTarget();
      if (type.equals(nextEdgeTarget.getType()) && isOwnView(nextEdgeTarget)) {
        result.add(nextEdgeTarget);
      }
    }
    return result;
  }

  /**
   * @generated
   */
  private Collection getOutgoingLinksByType(Collection nodes, int visualID) {
    Collection result = new ArrayList();
    String type = FlowchartVisualIDRegistry.getType(visualID);
    for (Iterator it = nodes.iterator(); it.hasNext();) {
      View nextNode = (View) it.next();
      result.addAll(selectViewsByType(nextNode.getSourceEdges(), type));
    }
    return result;
  }

  /**
   * @generated
   */
  private Collection getIncomingLinksByType(Collection nodes, int visualID) {
    Collection result = new ArrayList();
    String type = FlowchartVisualIDRegistry.getType(visualID);
    for (Iterator it = nodes.iterator(); it.hasNext();) {
      View nextNode = (View) it.next();
      result.addAll(selectViewsByType(nextNode.getTargetEdges(), type));
    }
    return result;
  }

  /**
   * @generated
   */
  private Collection getChildrenByType(Collection nodes, int visualID) {
    Collection result = new ArrayList();
    String type = FlowchartVisualIDRegistry.getType(visualID);
    for (Iterator it = nodes.iterator(); it.hasNext();) {
      View nextNode = (View) it.next();
      result.addAll(selectViewsByType(nextNode.getChildren(), type));
    }
    return result;
  }

  /**
   * @generated
   */
  private Collection getDiagramLinksByType(Collection diagrams, int visualID) {
    Collection result = new ArrayList();
    String type = FlowchartVisualIDRegistry.getType(visualID);
    for (Iterator it = diagrams.iterator(); it.hasNext();) {
      Diagram nextDiagram = (Diagram) it.next();
      result.addAll(selectViewsByType(nextDiagram.getEdges(), type));
    }
    return result;
  }

  /**
   * @generated
   */
  private Collection selectViewsByType(Collection views, String type) {
    Collection result = new ArrayList();
    for (Iterator it = views.iterator(); it.hasNext();) {
      View nextView = (View) it.next();
      if (type.equals(nextView.getType()) && isOwnView(nextView)) {
        result.add(nextView);
      }
    }
    return result;
  }

  /**
   * @generated
   */
  private boolean isOwnView(View view) {
    return WorkflowDiagramEditPart.MODEL_ID.equals(FlowchartVisualIDRegistry
        .getModelID(view));
  }

  /**
   * @generated
   */
  private Collection createNavigatorItems(Collection views, Object parent,
      boolean isLeafs) {
    Collection result = new ArrayList();
    for (Iterator it = views.iterator(); it.hasNext();) {
      result.add(new FlowchartNavigatorItem((View) it.next(), parent, isLeafs));
    }
    return result;
  }

  /**
   * @generated
   */
  public Object getParent(Object element) {
    if (element instanceof FlowchartAbstractNavigatorItem) {
      FlowchartAbstractNavigatorItem abstractNavigatorItem = (FlowchartAbstractNavigatorItem) element;
      return abstractNavigatorItem.getParent();
    }
    return null;
  }

  /**
   * @generated
   */
  public boolean hasChildren(Object element) {
    return element instanceof IFile || getChildren(element).length > 0;
  }

}
