package hub.top.adaptiveSystem.diagram.navigator;

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
public class AdaptiveSystemNavigatorContentProvider implements
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
	public AdaptiveSystemNavigatorContentProvider() {
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
						for (Iterator it = myEditingDomain.getResourceSet()
								.getResources().iterator(); it.hasNext();) {
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
						for (Iterator it = myEditingDomain.getResourceSet()
								.getResources().iterator(); it.hasNext();) {
							Resource nextResource = (Resource) it.next();
							nextResource.unload();
						}
						if (myViewer != null) {
							myViewer.getControl().getDisplay().asyncExec(
									myViewerRefreshRunnable);
						}
						return true;
					}

					public boolean handleResourceMoved(Resource resource,
							final URI newURI) {
						for (Iterator it = myEditingDomain.getResourceSet()
								.getResources().iterator(); it.hasNext();) {
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
			URI fileURI = URI.createPlatformResourceURI(file.getFullPath()
					.toString(), true);
			Resource resource = myEditingDomain.getResourceSet().getResource(
					fileURI, true);
			Collection result = new ArrayList();
			result
					.addAll(createNavigatorItems(
							selectViewsByType(
									resource.getContents(),
									hub.top.adaptiveSystem.diagram.edit.parts.AdaptiveSystemEditPart.MODEL_ID),
							file, false));
			return result.toArray();
		}

		if (parentElement instanceof hub.top.adaptiveSystem.diagram.navigator.AdaptiveSystemNavigatorGroup) {
			hub.top.adaptiveSystem.diagram.navigator.AdaptiveSystemNavigatorGroup group = (hub.top.adaptiveSystem.diagram.navigator.AdaptiveSystemNavigatorGroup) parentElement;
			return group.getChildren();
		}

		if (parentElement instanceof hub.top.adaptiveSystem.diagram.navigator.AdaptiveSystemNavigatorItem) {
			hub.top.adaptiveSystem.diagram.navigator.AdaptiveSystemNavigatorItem navigatorItem = (hub.top.adaptiveSystem.diagram.navigator.AdaptiveSystemNavigatorItem) parentElement;
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
		switch (hub.top.adaptiveSystem.diagram.part.AdaptiveSystemVisualIDRegistry
				.getVisualID(view)) {

		case hub.top.adaptiveSystem.diagram.edit.parts.AdaptiveSystemEditPart.VISUAL_ID: {
			Collection result = new ArrayList();
			hub.top.adaptiveSystem.diagram.navigator.AdaptiveSystemNavigatorGroup links = new hub.top.adaptiveSystem.diagram.navigator.AdaptiveSystemNavigatorGroup(
					hub.top.adaptiveSystem.diagram.part.Messages.NavigatorGroupName_AdaptiveSystem_79_links,
					"icons/linksNavigatorGroup.gif", parentElement); //$NON-NLS-1$
			Collection connectedViews = getChildrenByType(
					Collections.singleton(view),
					hub.top.adaptiveSystem.diagram.edit.parts.AdaptiveProcessEditPart.VISUAL_ID);
			result.addAll(createNavigatorItems(connectedViews, parentElement,
					false));
			connectedViews = getChildrenByType(
					Collections.singleton(view),
					hub.top.adaptiveSystem.diagram.edit.parts.OcletEditPart.VISUAL_ID);
			result.addAll(createNavigatorItems(connectedViews, parentElement,
					false));
			connectedViews = getDiagramLinksByType(
					Collections.singleton(view),
					hub.top.adaptiveSystem.diagram.edit.parts.ArcToConditionEditPart.VISUAL_ID);
			links
					.addChildren(createNavigatorItems(connectedViews, links,
							false));
			connectedViews = getDiagramLinksByType(
					Collections.singleton(view),
					hub.top.adaptiveSystem.diagram.edit.parts.ArcToEventEditPart.VISUAL_ID);
			links
					.addChildren(createNavigatorItems(connectedViews, links,
							false));
			if (!links.isEmpty()) {
				result.add(links);
			}
			return result.toArray();
		}

		case hub.top.adaptiveSystem.diagram.edit.parts.AdaptiveProcessEditPart.VISUAL_ID: {
			Collection result = new ArrayList();
			Collection connectedViews = getChildrenByType(
					Collections.singleton(view),
					hub.top.adaptiveSystem.diagram.edit.parts.AdaptiveProcessCompartmentEditPart.VISUAL_ID);
			connectedViews = getChildrenByType(
					connectedViews,
					hub.top.adaptiveSystem.diagram.edit.parts.ConditionAPEditPart.VISUAL_ID);
			result.addAll(createNavigatorItems(connectedViews, parentElement,
					false));
			connectedViews = getChildrenByType(
					Collections.singleton(view),
					hub.top.adaptiveSystem.diagram.edit.parts.AdaptiveProcessCompartmentEditPart.VISUAL_ID);
			connectedViews = getChildrenByType(
					connectedViews,
					hub.top.adaptiveSystem.diagram.edit.parts.EventAPEditPart.VISUAL_ID);
			result.addAll(createNavigatorItems(connectedViews, parentElement,
					false));
			return result.toArray();
		}

		case hub.top.adaptiveSystem.diagram.edit.parts.OcletEditPart.VISUAL_ID: {
			Collection result = new ArrayList();
			Collection connectedViews = getChildrenByType(
					Collections.singleton(view),
					hub.top.adaptiveSystem.diagram.edit.parts.PreNetEditPart.VISUAL_ID);
			result.addAll(createNavigatorItems(connectedViews, parentElement,
					false));
			connectedViews = getChildrenByType(
					Collections.singleton(view),
					hub.top.adaptiveSystem.diagram.edit.parts.DoNetEditPart.VISUAL_ID);
			result.addAll(createNavigatorItems(connectedViews, parentElement,
					false));
			return result.toArray();
		}

		case hub.top.adaptiveSystem.diagram.edit.parts.ConditionAPEditPart.VISUAL_ID: {
			Collection result = new ArrayList();
			hub.top.adaptiveSystem.diagram.navigator.AdaptiveSystemNavigatorGroup incominglinks = new hub.top.adaptiveSystem.diagram.navigator.AdaptiveSystemNavigatorGroup(
					hub.top.adaptiveSystem.diagram.part.Messages.NavigatorGroupName_Condition_2001_incominglinks,
					"icons/incomingLinksNavigatorGroup.gif", parentElement); //$NON-NLS-1$
			hub.top.adaptiveSystem.diagram.navigator.AdaptiveSystemNavigatorGroup outgoinglinks = new hub.top.adaptiveSystem.diagram.navigator.AdaptiveSystemNavigatorGroup(
					hub.top.adaptiveSystem.diagram.part.Messages.NavigatorGroupName_Condition_2001_outgoinglinks,
					"icons/outgoingLinksNavigatorGroup.gif", parentElement); //$NON-NLS-1$
			Collection connectedViews = getIncomingLinksByType(
					Collections.singleton(view),
					hub.top.adaptiveSystem.diagram.edit.parts.ArcToConditionEditPart.VISUAL_ID);
			incominglinks.addChildren(createNavigatorItems(connectedViews,
					incominglinks, true));
			connectedViews = getOutgoingLinksByType(
					Collections.singleton(view),
					hub.top.adaptiveSystem.diagram.edit.parts.ArcToEventEditPart.VISUAL_ID);
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

		case hub.top.adaptiveSystem.diagram.edit.parts.EventAPEditPart.VISUAL_ID: {
			Collection result = new ArrayList();
			hub.top.adaptiveSystem.diagram.navigator.AdaptiveSystemNavigatorGroup outgoinglinks = new hub.top.adaptiveSystem.diagram.navigator.AdaptiveSystemNavigatorGroup(
					hub.top.adaptiveSystem.diagram.part.Messages.NavigatorGroupName_Event_2002_outgoinglinks,
					"icons/outgoingLinksNavigatorGroup.gif", parentElement); //$NON-NLS-1$
			hub.top.adaptiveSystem.diagram.navigator.AdaptiveSystemNavigatorGroup incominglinks = new hub.top.adaptiveSystem.diagram.navigator.AdaptiveSystemNavigatorGroup(
					hub.top.adaptiveSystem.diagram.part.Messages.NavigatorGroupName_Event_2002_incominglinks,
					"icons/incomingLinksNavigatorGroup.gif", parentElement); //$NON-NLS-1$
			Collection connectedViews = getOutgoingLinksByType(
					Collections.singleton(view),
					hub.top.adaptiveSystem.diagram.edit.parts.ArcToConditionEditPart.VISUAL_ID);
			outgoinglinks.addChildren(createNavigatorItems(connectedViews,
					outgoinglinks, true));
			connectedViews = getIncomingLinksByType(
					Collections.singleton(view),
					hub.top.adaptiveSystem.diagram.edit.parts.ArcToEventEditPart.VISUAL_ID);
			incominglinks.addChildren(createNavigatorItems(connectedViews,
					incominglinks, true));
			if (!outgoinglinks.isEmpty()) {
				result.add(outgoinglinks);
			}
			if (!incominglinks.isEmpty()) {
				result.add(incominglinks);
			}
			return result.toArray();
		}

		case hub.top.adaptiveSystem.diagram.edit.parts.PreNetEditPart.VISUAL_ID: {
			Collection result = new ArrayList();
			Collection connectedViews = getChildrenByType(
					Collections.singleton(view),
					hub.top.adaptiveSystem.diagram.edit.parts.PreNetCompartmentEditPart.VISUAL_ID);
			connectedViews = getChildrenByType(
					connectedViews,
					hub.top.adaptiveSystem.diagram.edit.parts.ConditionPreNetEditPart.VISUAL_ID);
			result.addAll(createNavigatorItems(connectedViews, parentElement,
					false));
			connectedViews = getChildrenByType(
					Collections.singleton(view),
					hub.top.adaptiveSystem.diagram.edit.parts.PreNetCompartmentEditPart.VISUAL_ID);
			connectedViews = getChildrenByType(
					connectedViews,
					hub.top.adaptiveSystem.diagram.edit.parts.EventPreNetEditPart.VISUAL_ID);
			result.addAll(createNavigatorItems(connectedViews, parentElement,
					false));
			return result.toArray();
		}

		case hub.top.adaptiveSystem.diagram.edit.parts.ConditionPreNetEditPart.VISUAL_ID: {
			Collection result = new ArrayList();
			hub.top.adaptiveSystem.diagram.navigator.AdaptiveSystemNavigatorGroup incominglinks = new hub.top.adaptiveSystem.diagram.navigator.AdaptiveSystemNavigatorGroup(
					hub.top.adaptiveSystem.diagram.part.Messages.NavigatorGroupName_Condition_2004_incominglinks,
					"icons/incomingLinksNavigatorGroup.gif", parentElement); //$NON-NLS-1$
			hub.top.adaptiveSystem.diagram.navigator.AdaptiveSystemNavigatorGroup outgoinglinks = new hub.top.adaptiveSystem.diagram.navigator.AdaptiveSystemNavigatorGroup(
					hub.top.adaptiveSystem.diagram.part.Messages.NavigatorGroupName_Condition_2004_outgoinglinks,
					"icons/outgoingLinksNavigatorGroup.gif", parentElement); //$NON-NLS-1$
			Collection connectedViews = getIncomingLinksByType(
					Collections.singleton(view),
					hub.top.adaptiveSystem.diagram.edit.parts.ArcToConditionEditPart.VISUAL_ID);
			incominglinks.addChildren(createNavigatorItems(connectedViews,
					incominglinks, true));
			connectedViews = getOutgoingLinksByType(
					Collections.singleton(view),
					hub.top.adaptiveSystem.diagram.edit.parts.ArcToEventEditPart.VISUAL_ID);
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

		case hub.top.adaptiveSystem.diagram.edit.parts.EventPreNetEditPart.VISUAL_ID: {
			Collection result = new ArrayList();
			hub.top.adaptiveSystem.diagram.navigator.AdaptiveSystemNavigatorGroup outgoinglinks = new hub.top.adaptiveSystem.diagram.navigator.AdaptiveSystemNavigatorGroup(
					hub.top.adaptiveSystem.diagram.part.Messages.NavigatorGroupName_Event_2005_outgoinglinks,
					"icons/outgoingLinksNavigatorGroup.gif", parentElement); //$NON-NLS-1$
			hub.top.adaptiveSystem.diagram.navigator.AdaptiveSystemNavigatorGroup incominglinks = new hub.top.adaptiveSystem.diagram.navigator.AdaptiveSystemNavigatorGroup(
					hub.top.adaptiveSystem.diagram.part.Messages.NavigatorGroupName_Event_2005_incominglinks,
					"icons/incomingLinksNavigatorGroup.gif", parentElement); //$NON-NLS-1$
			Collection connectedViews = getOutgoingLinksByType(
					Collections.singleton(view),
					hub.top.adaptiveSystem.diagram.edit.parts.ArcToConditionEditPart.VISUAL_ID);
			outgoinglinks.addChildren(createNavigatorItems(connectedViews,
					outgoinglinks, true));
			connectedViews = getIncomingLinksByType(
					Collections.singleton(view),
					hub.top.adaptiveSystem.diagram.edit.parts.ArcToEventEditPart.VISUAL_ID);
			incominglinks.addChildren(createNavigatorItems(connectedViews,
					incominglinks, true));
			if (!outgoinglinks.isEmpty()) {
				result.add(outgoinglinks);
			}
			if (!incominglinks.isEmpty()) {
				result.add(incominglinks);
			}
			return result.toArray();
		}

		case hub.top.adaptiveSystem.diagram.edit.parts.DoNetEditPart.VISUAL_ID: {
			Collection result = new ArrayList();
			Collection connectedViews = getChildrenByType(
					Collections.singleton(view),
					hub.top.adaptiveSystem.diagram.edit.parts.DoNetCompartmentEditPart.VISUAL_ID);
			connectedViews = getChildrenByType(
					connectedViews,
					hub.top.adaptiveSystem.diagram.edit.parts.ConditionDoNetEditPart.VISUAL_ID);
			result.addAll(createNavigatorItems(connectedViews, parentElement,
					false));
			connectedViews = getChildrenByType(
					Collections.singleton(view),
					hub.top.adaptiveSystem.diagram.edit.parts.DoNetCompartmentEditPart.VISUAL_ID);
			connectedViews = getChildrenByType(
					connectedViews,
					hub.top.adaptiveSystem.diagram.edit.parts.EventDoNetEditPart.VISUAL_ID);
			result.addAll(createNavigatorItems(connectedViews, parentElement,
					false));
			return result.toArray();
		}

		case hub.top.adaptiveSystem.diagram.edit.parts.ConditionDoNetEditPart.VISUAL_ID: {
			Collection result = new ArrayList();
			hub.top.adaptiveSystem.diagram.navigator.AdaptiveSystemNavigatorGroup incominglinks = new hub.top.adaptiveSystem.diagram.navigator.AdaptiveSystemNavigatorGroup(
					hub.top.adaptiveSystem.diagram.part.Messages.NavigatorGroupName_Condition_2007_incominglinks,
					"icons/incomingLinksNavigatorGroup.gif", parentElement); //$NON-NLS-1$
			hub.top.adaptiveSystem.diagram.navigator.AdaptiveSystemNavigatorGroup outgoinglinks = new hub.top.adaptiveSystem.diagram.navigator.AdaptiveSystemNavigatorGroup(
					hub.top.adaptiveSystem.diagram.part.Messages.NavigatorGroupName_Condition_2007_outgoinglinks,
					"icons/outgoingLinksNavigatorGroup.gif", parentElement); //$NON-NLS-1$
			Collection connectedViews = getIncomingLinksByType(
					Collections.singleton(view),
					hub.top.adaptiveSystem.diagram.edit.parts.ArcToConditionEditPart.VISUAL_ID);
			incominglinks.addChildren(createNavigatorItems(connectedViews,
					incominglinks, true));
			connectedViews = getOutgoingLinksByType(
					Collections.singleton(view),
					hub.top.adaptiveSystem.diagram.edit.parts.ArcToEventEditPart.VISUAL_ID);
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

		case hub.top.adaptiveSystem.diagram.edit.parts.EventDoNetEditPart.VISUAL_ID: {
			Collection result = new ArrayList();
			hub.top.adaptiveSystem.diagram.navigator.AdaptiveSystemNavigatorGroup outgoinglinks = new hub.top.adaptiveSystem.diagram.navigator.AdaptiveSystemNavigatorGroup(
					hub.top.adaptiveSystem.diagram.part.Messages.NavigatorGroupName_Event_2008_outgoinglinks,
					"icons/outgoingLinksNavigatorGroup.gif", parentElement); //$NON-NLS-1$
			hub.top.adaptiveSystem.diagram.navigator.AdaptiveSystemNavigatorGroup incominglinks = new hub.top.adaptiveSystem.diagram.navigator.AdaptiveSystemNavigatorGroup(
					hub.top.adaptiveSystem.diagram.part.Messages.NavigatorGroupName_Event_2008_incominglinks,
					"icons/incomingLinksNavigatorGroup.gif", parentElement); //$NON-NLS-1$
			Collection connectedViews = getOutgoingLinksByType(
					Collections.singleton(view),
					hub.top.adaptiveSystem.diagram.edit.parts.ArcToConditionEditPart.VISUAL_ID);
			outgoinglinks.addChildren(createNavigatorItems(connectedViews,
					outgoinglinks, true));
			connectedViews = getIncomingLinksByType(
					Collections.singleton(view),
					hub.top.adaptiveSystem.diagram.edit.parts.ArcToEventEditPart.VISUAL_ID);
			incominglinks.addChildren(createNavigatorItems(connectedViews,
					incominglinks, true));
			if (!outgoinglinks.isEmpty()) {
				result.add(outgoinglinks);
			}
			if (!incominglinks.isEmpty()) {
				result.add(incominglinks);
			}
			return result.toArray();
		}

		case hub.top.adaptiveSystem.diagram.edit.parts.ArcToConditionEditPart.VISUAL_ID: {
			Collection result = new ArrayList();
			hub.top.adaptiveSystem.diagram.navigator.AdaptiveSystemNavigatorGroup target = new hub.top.adaptiveSystem.diagram.navigator.AdaptiveSystemNavigatorGroup(
					hub.top.adaptiveSystem.diagram.part.Messages.NavigatorGroupName_ArcToCondition_3001_target,
					"icons/linkTargetNavigatorGroup.gif", parentElement); //$NON-NLS-1$
			hub.top.adaptiveSystem.diagram.navigator.AdaptiveSystemNavigatorGroup source = new hub.top.adaptiveSystem.diagram.navigator.AdaptiveSystemNavigatorGroup(
					hub.top.adaptiveSystem.diagram.part.Messages.NavigatorGroupName_ArcToCondition_3001_source,
					"icons/linkSourceNavigatorGroup.gif", parentElement); //$NON-NLS-1$
			Collection connectedViews = getLinksTargetByType(
					Collections.singleton(view),
					hub.top.adaptiveSystem.diagram.edit.parts.ConditionAPEditPart.VISUAL_ID);
			target.addChildren(createNavigatorItems(connectedViews, target,
					true));
			connectedViews = getLinksTargetByType(
					Collections.singleton(view),
					hub.top.adaptiveSystem.diagram.edit.parts.ConditionPreNetEditPart.VISUAL_ID);
			target.addChildren(createNavigatorItems(connectedViews, target,
					true));
			connectedViews = getLinksTargetByType(
					Collections.singleton(view),
					hub.top.adaptiveSystem.diagram.edit.parts.ConditionDoNetEditPart.VISUAL_ID);
			target.addChildren(createNavigatorItems(connectedViews, target,
					true));
			connectedViews = getLinksSourceByType(
					Collections.singleton(view),
					hub.top.adaptiveSystem.diagram.edit.parts.EventAPEditPart.VISUAL_ID);
			source.addChildren(createNavigatorItems(connectedViews, source,
					true));
			connectedViews = getLinksSourceByType(
					Collections.singleton(view),
					hub.top.adaptiveSystem.diagram.edit.parts.EventPreNetEditPart.VISUAL_ID);
			source.addChildren(createNavigatorItems(connectedViews, source,
					true));
			connectedViews = getLinksSourceByType(
					Collections.singleton(view),
					hub.top.adaptiveSystem.diagram.edit.parts.EventDoNetEditPart.VISUAL_ID);
			source.addChildren(createNavigatorItems(connectedViews, source,
					true));
			if (!target.isEmpty()) {
				result.add(target);
			}
			if (!source.isEmpty()) {
				result.add(source);
			}
			return result.toArray();
		}

		case hub.top.adaptiveSystem.diagram.edit.parts.ArcToEventEditPart.VISUAL_ID: {
			Collection result = new ArrayList();
			hub.top.adaptiveSystem.diagram.navigator.AdaptiveSystemNavigatorGroup target = new hub.top.adaptiveSystem.diagram.navigator.AdaptiveSystemNavigatorGroup(
					hub.top.adaptiveSystem.diagram.part.Messages.NavigatorGroupName_ArcToEvent_3002_target,
					"icons/linkTargetNavigatorGroup.gif", parentElement); //$NON-NLS-1$
			hub.top.adaptiveSystem.diagram.navigator.AdaptiveSystemNavigatorGroup source = new hub.top.adaptiveSystem.diagram.navigator.AdaptiveSystemNavigatorGroup(
					hub.top.adaptiveSystem.diagram.part.Messages.NavigatorGroupName_ArcToEvent_3002_source,
					"icons/linkSourceNavigatorGroup.gif", parentElement); //$NON-NLS-1$
			Collection connectedViews = getLinksTargetByType(
					Collections.singleton(view),
					hub.top.adaptiveSystem.diagram.edit.parts.EventAPEditPart.VISUAL_ID);
			target.addChildren(createNavigatorItems(connectedViews, target,
					true));
			connectedViews = getLinksTargetByType(
					Collections.singleton(view),
					hub.top.adaptiveSystem.diagram.edit.parts.EventPreNetEditPart.VISUAL_ID);
			target.addChildren(createNavigatorItems(connectedViews, target,
					true));
			connectedViews = getLinksTargetByType(
					Collections.singleton(view),
					hub.top.adaptiveSystem.diagram.edit.parts.EventDoNetEditPart.VISUAL_ID);
			target.addChildren(createNavigatorItems(connectedViews, target,
					true));
			connectedViews = getLinksSourceByType(
					Collections.singleton(view),
					hub.top.adaptiveSystem.diagram.edit.parts.ConditionAPEditPart.VISUAL_ID);
			source.addChildren(createNavigatorItems(connectedViews, source,
					true));
			connectedViews = getLinksSourceByType(
					Collections.singleton(view),
					hub.top.adaptiveSystem.diagram.edit.parts.ConditionPreNetEditPart.VISUAL_ID);
			source.addChildren(createNavigatorItems(connectedViews, source,
					true));
			connectedViews = getLinksSourceByType(
					Collections.singleton(view),
					hub.top.adaptiveSystem.diagram.edit.parts.ConditionDoNetEditPart.VISUAL_ID);
			source.addChildren(createNavigatorItems(connectedViews, source,
					true));
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
		String type = hub.top.adaptiveSystem.diagram.part.AdaptiveSystemVisualIDRegistry
				.getType(visualID);
		for (Iterator it = edges.iterator(); it.hasNext();) {
			Edge nextEdge = (Edge) it.next();
			View nextEdgeSource = nextEdge.getSource();
			if (type.equals(nextEdgeSource.getType())
					&& isOwnView(nextEdgeSource)) {
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
		String type = hub.top.adaptiveSystem.diagram.part.AdaptiveSystemVisualIDRegistry
				.getType(visualID);
		for (Iterator it = edges.iterator(); it.hasNext();) {
			Edge nextEdge = (Edge) it.next();
			View nextEdgeTarget = nextEdge.getTarget();
			if (type.equals(nextEdgeTarget.getType())
					&& isOwnView(nextEdgeTarget)) {
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
		String type = hub.top.adaptiveSystem.diagram.part.AdaptiveSystemVisualIDRegistry
				.getType(visualID);
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
		String type = hub.top.adaptiveSystem.diagram.part.AdaptiveSystemVisualIDRegistry
				.getType(visualID);
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
		String type = hub.top.adaptiveSystem.diagram.part.AdaptiveSystemVisualIDRegistry
				.getType(visualID);
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
		String type = hub.top.adaptiveSystem.diagram.part.AdaptiveSystemVisualIDRegistry
				.getType(visualID);
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
		return hub.top.adaptiveSystem.diagram.edit.parts.AdaptiveSystemEditPart.MODEL_ID
				.equals(hub.top.adaptiveSystem.diagram.part.AdaptiveSystemVisualIDRegistry
						.getModelID(view));
	}

	/**
	 * @generated
	 */
	private Collection createNavigatorItems(Collection views, Object parent,
			boolean isLeafs) {
		Collection result = new ArrayList();
		for (Iterator it = views.iterator(); it.hasNext();) {
			result
					.add(new hub.top.adaptiveSystem.diagram.navigator.AdaptiveSystemNavigatorItem(
							(View) it.next(), parent, isLeafs));
		}
		return result;
	}

	/**
	 * @generated
	 */
	public Object getParent(Object element) {
		if (element instanceof hub.top.adaptiveSystem.diagram.navigator.AdaptiveSystemAbstractNavigatorItem) {
			hub.top.adaptiveSystem.diagram.navigator.AdaptiveSystemAbstractNavigatorItem abstractNavigatorItem = (hub.top.adaptiveSystem.diagram.navigator.AdaptiveSystemAbstractNavigatorItem) element;
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
