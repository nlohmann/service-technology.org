package hub.top.adaptiveSystem.diagram.providers;

import org.eclipse.core.runtime.IAdaptable;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.gmf.runtime.diagram.core.providers.AbstractViewProvider;
import org.eclipse.gmf.runtime.emf.type.core.IElementType;
import org.eclipse.gmf.runtime.emf.type.core.IHintedType;
import org.eclipse.gmf.runtime.notation.View;

/**
 * @generated
 */
public class AdaptiveSystemViewProvider extends AbstractViewProvider {

	/**
	 * @generated
	 */
	protected Class getDiagramViewClass(IAdaptable semanticAdapter,
			String diagramKind) {
		EObject semanticElement = getSemanticElement(semanticAdapter);
		if (hub.top.adaptiveSystem.diagram.edit.parts.AdaptiveSystemEditPart.MODEL_ID
				.equals(diagramKind)
				&& hub.top.adaptiveSystem.diagram.part.AdaptiveSystemVisualIDRegistry
						.getDiagramVisualID(semanticElement) != -1) {
			return hub.top.adaptiveSystem.diagram.view.factories.AdaptiveSystemViewFactory.class;
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
			if (elementType != null || domainElement == null) {
				return null;
			}
			visualID = hub.top.adaptiveSystem.diagram.part.AdaptiveSystemVisualIDRegistry
					.getNodeVisualID(containerView, domainElement);
		} else {
			visualID = hub.top.adaptiveSystem.diagram.part.AdaptiveSystemVisualIDRegistry
					.getVisualID(semanticHint);
			if (elementType != null) {
				if (!hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes
						.isKnownElementType(elementType)
						|| false == elementType instanceof IHintedType) {
					return null;
				}
				String elementTypeHint = ((IHintedType) elementType)
						.getSemanticHint();
				if (!semanticHint.equals(elementTypeHint)) {
					return null;
				}
				if (domainElement != null
						&& visualID != hub.top.adaptiveSystem.diagram.part.AdaptiveSystemVisualIDRegistry
								.getNodeVisualID(containerView, domainElement)) {
					return null;
				}
			} else {
				if (domainElement == null) {
					return null;
				}
				switch (visualID) {
				case hub.top.adaptiveSystem.diagram.edit.parts.AdaptiveSystemEditPart.VISUAL_ID:
				case hub.top.adaptiveSystem.diagram.edit.parts.AdaptiveProcessEditPart.VISUAL_ID:
				case hub.top.adaptiveSystem.diagram.edit.parts.OcletEditPart.VISUAL_ID:
				case hub.top.adaptiveSystem.diagram.edit.parts.ConditionAPEditPart.VISUAL_ID:
				case hub.top.adaptiveSystem.diagram.edit.parts.EventAPEditPart.VISUAL_ID:
				case hub.top.adaptiveSystem.diagram.edit.parts.PreNetEditPart.VISUAL_ID:
				case hub.top.adaptiveSystem.diagram.edit.parts.ConditionPreNetEditPart.VISUAL_ID:
				case hub.top.adaptiveSystem.diagram.edit.parts.EventPreNetEditPart.VISUAL_ID:
				case hub.top.adaptiveSystem.diagram.edit.parts.DoNetEditPart.VISUAL_ID:
				case hub.top.adaptiveSystem.diagram.edit.parts.ConditionDoNetEditPart.VISUAL_ID:
				case hub.top.adaptiveSystem.diagram.edit.parts.EventDoNetEditPart.VISUAL_ID:
				case hub.top.adaptiveSystem.diagram.edit.parts.ArcToConditionEditPart.VISUAL_ID:
				case hub.top.adaptiveSystem.diagram.edit.parts.ArcToEventEditPart.VISUAL_ID:
					if (visualID != hub.top.adaptiveSystem.diagram.part.AdaptiveSystemVisualIDRegistry
							.getNodeVisualID(containerView, domainElement)) {
						return null;
					}
					break;

				case hub.top.adaptiveSystem.diagram.edit.parts.AdaptiveProcessCompartmentEditPart.VISUAL_ID:
					if (hub.top.adaptiveSystem.diagram.edit.parts.AdaptiveProcessEditPart.VISUAL_ID != hub.top.adaptiveSystem.diagram.part.AdaptiveSystemVisualIDRegistry
							.getVisualID(containerView)
							|| containerView.getElement() != domainElement) {
						return null;
					}
					break;
				case hub.top.adaptiveSystem.diagram.edit.parts.OcletNameEditPart.VISUAL_ID:
				case hub.top.adaptiveSystem.diagram.edit.parts.OcletQuantorOrientationEditPart.VISUAL_ID:

					if (hub.top.adaptiveSystem.diagram.edit.parts.OcletEditPart.VISUAL_ID != hub.top.adaptiveSystem.diagram.part.AdaptiveSystemVisualIDRegistry
							.getVisualID(containerView)
							|| containerView.getElement() != domainElement) {
						return null;
					}
					break;
				case hub.top.adaptiveSystem.diagram.edit.parts.ConditionAPNameEditPart.VISUAL_ID:
				case hub.top.adaptiveSystem.diagram.edit.parts.ConditionAPTempEditPart.VISUAL_ID:
				case hub.top.adaptiveSystem.diagram.edit.parts.ConditionAPTokenEditPart.VISUAL_ID:

					if (hub.top.adaptiveSystem.diagram.edit.parts.ConditionAPEditPart.VISUAL_ID != hub.top.adaptiveSystem.diagram.part.AdaptiveSystemVisualIDRegistry
							.getVisualID(containerView)
							|| containerView.getElement() != domainElement) {
						return null;
					}
					break;
				case hub.top.adaptiveSystem.diagram.edit.parts.EventAPNameEditPart.VISUAL_ID:
				case hub.top.adaptiveSystem.diagram.edit.parts.EventAPTempEditPart.VISUAL_ID:

					if (hub.top.adaptiveSystem.diagram.edit.parts.EventAPEditPart.VISUAL_ID != hub.top.adaptiveSystem.diagram.part.AdaptiveSystemVisualIDRegistry
							.getVisualID(containerView)
							|| containerView.getElement() != domainElement) {
						return null;
					}
					break;

				case hub.top.adaptiveSystem.diagram.edit.parts.PreNetCompartmentEditPart.VISUAL_ID:
					if (hub.top.adaptiveSystem.diagram.edit.parts.PreNetEditPart.VISUAL_ID != hub.top.adaptiveSystem.diagram.part.AdaptiveSystemVisualIDRegistry
							.getVisualID(containerView)
							|| containerView.getElement() != domainElement) {
						return null;
					}
					break;
				case hub.top.adaptiveSystem.diagram.edit.parts.ConditionPreNetNameEditPart.VISUAL_ID:
				case hub.top.adaptiveSystem.diagram.edit.parts.ConditionPreNetTempEditPart.VISUAL_ID:
				case hub.top.adaptiveSystem.diagram.edit.parts.ConditionPreNetTokenEditPart.VISUAL_ID:

					if (hub.top.adaptiveSystem.diagram.edit.parts.ConditionPreNetEditPart.VISUAL_ID != hub.top.adaptiveSystem.diagram.part.AdaptiveSystemVisualIDRegistry
							.getVisualID(containerView)
							|| containerView.getElement() != domainElement) {
						return null;
					}
					break;
				case hub.top.adaptiveSystem.diagram.edit.parts.EventPreNetNameEditPart.VISUAL_ID:
				case hub.top.adaptiveSystem.diagram.edit.parts.EventPreNetTempEditPart.VISUAL_ID:

					if (hub.top.adaptiveSystem.diagram.edit.parts.EventPreNetEditPart.VISUAL_ID != hub.top.adaptiveSystem.diagram.part.AdaptiveSystemVisualIDRegistry
							.getVisualID(containerView)
							|| containerView.getElement() != domainElement) {
						return null;
					}
					break;

				case hub.top.adaptiveSystem.diagram.edit.parts.DoNetCompartmentEditPart.VISUAL_ID:
					if (hub.top.adaptiveSystem.diagram.edit.parts.DoNetEditPart.VISUAL_ID != hub.top.adaptiveSystem.diagram.part.AdaptiveSystemVisualIDRegistry
							.getVisualID(containerView)
							|| containerView.getElement() != domainElement) {
						return null;
					}
					break;
				case hub.top.adaptiveSystem.diagram.edit.parts.ConditionDoNetNameEditPart.VISUAL_ID:
				case hub.top.adaptiveSystem.diagram.edit.parts.ConditionDoNetTempEditPart.VISUAL_ID:
				case hub.top.adaptiveSystem.diagram.edit.parts.ConditionDoNetTokenEditPart.VISUAL_ID:

					if (hub.top.adaptiveSystem.diagram.edit.parts.ConditionDoNetEditPart.VISUAL_ID != hub.top.adaptiveSystem.diagram.part.AdaptiveSystemVisualIDRegistry
							.getVisualID(containerView)
							|| containerView.getElement() != domainElement) {
						return null;
					}
					break;
				case hub.top.adaptiveSystem.diagram.edit.parts.EventDoNetNameEditPart.VISUAL_ID:
				case hub.top.adaptiveSystem.diagram.edit.parts.EventDoNetTempEditPart.VISUAL_ID:

					if (hub.top.adaptiveSystem.diagram.edit.parts.EventDoNetEditPart.VISUAL_ID != hub.top.adaptiveSystem.diagram.part.AdaptiveSystemVisualIDRegistry
							.getVisualID(containerView)
							|| containerView.getElement() != domainElement) {
						return null;
					}
					break;
				default:
					return null;
				}
			}
		}
		if (!hub.top.adaptiveSystem.diagram.part.AdaptiveSystemVisualIDRegistry
				.canCreateNode(containerView, visualID)) {
			return null;
		}
		switch (visualID) {
		case hub.top.adaptiveSystem.diagram.edit.parts.AdaptiveProcessEditPart.VISUAL_ID:
			return hub.top.adaptiveSystem.diagram.view.factories.AdaptiveProcessViewFactory.class;
		case hub.top.adaptiveSystem.diagram.edit.parts.OcletEditPart.VISUAL_ID:
			return hub.top.adaptiveSystem.diagram.view.factories.OcletViewFactory.class;
		case hub.top.adaptiveSystem.diagram.edit.parts.OcletNameEditPart.VISUAL_ID:
			return hub.top.adaptiveSystem.diagram.view.factories.OcletNameViewFactory.class;
		case hub.top.adaptiveSystem.diagram.edit.parts.OcletQuantorOrientationEditPart.VISUAL_ID:
			return hub.top.adaptiveSystem.diagram.view.factories.OcletQuantorOrientationViewFactory.class;
		case hub.top.adaptiveSystem.diagram.edit.parts.ConditionAPEditPart.VISUAL_ID:
			return hub.top.adaptiveSystem.diagram.view.factories.ConditionAPViewFactory.class;
		case hub.top.adaptiveSystem.diagram.edit.parts.ConditionAPNameEditPart.VISUAL_ID:
			return hub.top.adaptiveSystem.diagram.view.factories.ConditionAPNameViewFactory.class;
		case hub.top.adaptiveSystem.diagram.edit.parts.ConditionAPTempEditPart.VISUAL_ID:
			return hub.top.adaptiveSystem.diagram.view.factories.ConditionAPTempViewFactory.class;
		case hub.top.adaptiveSystem.diagram.edit.parts.ConditionAPTokenEditPart.VISUAL_ID:
			return hub.top.adaptiveSystem.diagram.view.factories.ConditionAPTokenViewFactory.class;
		case hub.top.adaptiveSystem.diagram.edit.parts.EventAPEditPart.VISUAL_ID:
			return hub.top.adaptiveSystem.diagram.view.factories.EventAPViewFactory.class;
		case hub.top.adaptiveSystem.diagram.edit.parts.EventAPNameEditPart.VISUAL_ID:
			return hub.top.adaptiveSystem.diagram.view.factories.EventAPNameViewFactory.class;
		case hub.top.adaptiveSystem.diagram.edit.parts.EventAPTempEditPart.VISUAL_ID:
			return hub.top.adaptiveSystem.diagram.view.factories.EventAPTempViewFactory.class;
		case hub.top.adaptiveSystem.diagram.edit.parts.PreNetEditPart.VISUAL_ID:
			return hub.top.adaptiveSystem.diagram.view.factories.PreNetViewFactory.class;
		case hub.top.adaptiveSystem.diagram.edit.parts.ConditionPreNetEditPart.VISUAL_ID:
			return hub.top.adaptiveSystem.diagram.view.factories.ConditionPreNetViewFactory.class;
		case hub.top.adaptiveSystem.diagram.edit.parts.ConditionPreNetNameEditPart.VISUAL_ID:
			return hub.top.adaptiveSystem.diagram.view.factories.ConditionPreNetNameViewFactory.class;
		case hub.top.adaptiveSystem.diagram.edit.parts.ConditionPreNetTempEditPart.VISUAL_ID:
			return hub.top.adaptiveSystem.diagram.view.factories.ConditionPreNetTempViewFactory.class;
		case hub.top.adaptiveSystem.diagram.edit.parts.ConditionPreNetTokenEditPart.VISUAL_ID:
			return hub.top.adaptiveSystem.diagram.view.factories.ConditionPreNetTokenViewFactory.class;
		case hub.top.adaptiveSystem.diagram.edit.parts.EventPreNetEditPart.VISUAL_ID:
			return hub.top.adaptiveSystem.diagram.view.factories.EventPreNetViewFactory.class;
		case hub.top.adaptiveSystem.diagram.edit.parts.EventPreNetNameEditPart.VISUAL_ID:
			return hub.top.adaptiveSystem.diagram.view.factories.EventPreNetNameViewFactory.class;
		case hub.top.adaptiveSystem.diagram.edit.parts.EventPreNetTempEditPart.VISUAL_ID:
			return hub.top.adaptiveSystem.diagram.view.factories.EventPreNetTempViewFactory.class;
		case hub.top.adaptiveSystem.diagram.edit.parts.DoNetEditPart.VISUAL_ID:
			return hub.top.adaptiveSystem.diagram.view.factories.DoNetViewFactory.class;
		case hub.top.adaptiveSystem.diagram.edit.parts.ConditionDoNetEditPart.VISUAL_ID:
			return hub.top.adaptiveSystem.diagram.view.factories.ConditionDoNetViewFactory.class;
		case hub.top.adaptiveSystem.diagram.edit.parts.ConditionDoNetNameEditPart.VISUAL_ID:
			return hub.top.adaptiveSystem.diagram.view.factories.ConditionDoNetNameViewFactory.class;
		case hub.top.adaptiveSystem.diagram.edit.parts.ConditionDoNetTempEditPart.VISUAL_ID:
			return hub.top.adaptiveSystem.diagram.view.factories.ConditionDoNetTempViewFactory.class;
		case hub.top.adaptiveSystem.diagram.edit.parts.ConditionDoNetTokenEditPart.VISUAL_ID:
			return hub.top.adaptiveSystem.diagram.view.factories.ConditionDoNetTokenViewFactory.class;
		case hub.top.adaptiveSystem.diagram.edit.parts.EventDoNetEditPart.VISUAL_ID:
			return hub.top.adaptiveSystem.diagram.view.factories.EventDoNetViewFactory.class;
		case hub.top.adaptiveSystem.diagram.edit.parts.EventDoNetNameEditPart.VISUAL_ID:
			return hub.top.adaptiveSystem.diagram.view.factories.EventDoNetNameViewFactory.class;
		case hub.top.adaptiveSystem.diagram.edit.parts.EventDoNetTempEditPart.VISUAL_ID:
			return hub.top.adaptiveSystem.diagram.view.factories.EventDoNetTempViewFactory.class;
		case hub.top.adaptiveSystem.diagram.edit.parts.AdaptiveProcessCompartmentEditPart.VISUAL_ID:
			return hub.top.adaptiveSystem.diagram.view.factories.AdaptiveProcessCompartmentViewFactory.class;
		case hub.top.adaptiveSystem.diagram.edit.parts.PreNetCompartmentEditPart.VISUAL_ID:
			return hub.top.adaptiveSystem.diagram.view.factories.PreNetCompartmentViewFactory.class;
		case hub.top.adaptiveSystem.diagram.edit.parts.DoNetCompartmentEditPart.VISUAL_ID:
			return hub.top.adaptiveSystem.diagram.view.factories.DoNetCompartmentViewFactory.class;
		}
		return null;
	}

	/**
	 * @generated
	 */
	protected Class getEdgeViewClass(IAdaptable semanticAdapter,
			View containerView, String semanticHint) {
		IElementType elementType = getSemanticElementType(semanticAdapter);
		if (elementType == null) {
			return null;
		}
		if (!hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes
				.isKnownElementType(elementType)
				|| false == elementType instanceof IHintedType) {
			return null;
		}
		String elementTypeHint = ((IHintedType) elementType).getSemanticHint();
		if (elementTypeHint == null) {
			return null;
		}
		if (semanticHint != null && !semanticHint.equals(elementTypeHint)) {
			return null;
		}
		int visualID = hub.top.adaptiveSystem.diagram.part.AdaptiveSystemVisualIDRegistry
				.getVisualID(elementTypeHint);
		EObject domainElement = getSemanticElement(semanticAdapter);
		if (domainElement != null
				&& visualID != hub.top.adaptiveSystem.diagram.part.AdaptiveSystemVisualIDRegistry
						.getLinkWithClassVisualID(domainElement)) {
			return null;
		}
		switch (visualID) {
		case hub.top.adaptiveSystem.diagram.edit.parts.ArcToConditionEditPart.VISUAL_ID:
			return hub.top.adaptiveSystem.diagram.view.factories.ArcToConditionViewFactory.class;
		case hub.top.adaptiveSystem.diagram.edit.parts.ArcToEventEditPart.VISUAL_ID:
			return hub.top.adaptiveSystem.diagram.view.factories.ArcToEventViewFactory.class;
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
