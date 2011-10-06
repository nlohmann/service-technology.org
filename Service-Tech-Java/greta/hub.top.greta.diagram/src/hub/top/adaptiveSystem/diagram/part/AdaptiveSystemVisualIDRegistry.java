package hub.top.adaptiveSystem.diagram.part;

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
public class AdaptiveSystemVisualIDRegistry {

	/**
	 * @generated
	 */
	private static final String DEBUG_KEY = "hub.top.GRETA.diagram/debug/visualID"; //$NON-NLS-1$

	/**
	 * @generated
	 */
	public static int getVisualID(View view) {
		if (view instanceof Diagram) {
			if (hub.top.adaptiveSystem.diagram.edit.parts.AdaptiveSystemEditPart.MODEL_ID
					.equals(view.getType())) {
				return hub.top.adaptiveSystem.diagram.edit.parts.AdaptiveSystemEditPart.VISUAL_ID;
			} else {
				return -1;
			}
		}
		return hub.top.adaptiveSystem.diagram.part.AdaptiveSystemVisualIDRegistry
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
				hub.top.adaptiveSystem.diagram.part.AdaptiveSystemDiagramEditorPlugin
						.getInstance().logError(
								"Unable to parse view type as a visualID number: "
										+ type);
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
		if (hub.top.adaptiveSystem.AdaptiveSystemPackage.eINSTANCE
				.getAdaptiveSystem().isSuperTypeOf(domainElement.eClass())
				&& isDiagram((hub.top.adaptiveSystem.AdaptiveSystem) domainElement)) {
			return hub.top.adaptiveSystem.diagram.edit.parts.AdaptiveSystemEditPart.VISUAL_ID;
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
		String containerModelID = hub.top.adaptiveSystem.diagram.part.AdaptiveSystemVisualIDRegistry
				.getModelID(containerView);
		if (!hub.top.adaptiveSystem.diagram.edit.parts.AdaptiveSystemEditPart.MODEL_ID
				.equals(containerModelID)) {
			return -1;
		}
		int containerVisualID;
		if (hub.top.adaptiveSystem.diagram.edit.parts.AdaptiveSystemEditPart.MODEL_ID
				.equals(containerModelID)) {
			containerVisualID = hub.top.adaptiveSystem.diagram.part.AdaptiveSystemVisualIDRegistry
					.getVisualID(containerView);
		} else {
			if (containerView instanceof Diagram) {
				containerVisualID = hub.top.adaptiveSystem.diagram.edit.parts.AdaptiveSystemEditPart.VISUAL_ID;
			} else {
				return -1;
			}
		}
		switch (containerVisualID) {
		case hub.top.adaptiveSystem.diagram.edit.parts.OcletEditPart.VISUAL_ID:
			if (hub.top.adaptiveSystem.AdaptiveSystemPackage.eINSTANCE
					.getPreNet().isSuperTypeOf(domainElement.eClass())) {
				return hub.top.adaptiveSystem.diagram.edit.parts.PreNetEditPart.VISUAL_ID;
			}
			if (hub.top.adaptiveSystem.AdaptiveSystemPackage.eINSTANCE
					.getDoNet().isSuperTypeOf(domainElement.eClass())) {
				return hub.top.adaptiveSystem.diagram.edit.parts.DoNetEditPart.VISUAL_ID;
			}
			break;
		case hub.top.adaptiveSystem.diagram.edit.parts.AdaptiveProcessCompartmentEditPart.VISUAL_ID:
			if (hub.top.adaptiveSystem.AdaptiveSystemPackage.eINSTANCE
					.getCondition().isSuperTypeOf(domainElement.eClass())) {
				return hub.top.adaptiveSystem.diagram.edit.parts.ConditionAPEditPart.VISUAL_ID;
			}
			if (hub.top.adaptiveSystem.AdaptiveSystemPackage.eINSTANCE
					.getEvent().isSuperTypeOf(domainElement.eClass())) {
				return hub.top.adaptiveSystem.diagram.edit.parts.EventAPEditPart.VISUAL_ID;
			}
			break;
		case hub.top.adaptiveSystem.diagram.edit.parts.PreNetCompartmentEditPart.VISUAL_ID:
			if (hub.top.adaptiveSystem.AdaptiveSystemPackage.eINSTANCE
					.getCondition().isSuperTypeOf(domainElement.eClass())) {
				return hub.top.adaptiveSystem.diagram.edit.parts.ConditionPreNetEditPart.VISUAL_ID;
			}
			if (hub.top.adaptiveSystem.AdaptiveSystemPackage.eINSTANCE
					.getEvent().isSuperTypeOf(domainElement.eClass())) {
				return hub.top.adaptiveSystem.diagram.edit.parts.EventPreNetEditPart.VISUAL_ID;
			}
			break;
		case hub.top.adaptiveSystem.diagram.edit.parts.DoNetCompartmentEditPart.VISUAL_ID:
			if (hub.top.adaptiveSystem.AdaptiveSystemPackage.eINSTANCE
					.getCondition().isSuperTypeOf(domainElement.eClass())) {
				return hub.top.adaptiveSystem.diagram.edit.parts.ConditionDoNetEditPart.VISUAL_ID;
			}
			if (hub.top.adaptiveSystem.AdaptiveSystemPackage.eINSTANCE
					.getEvent().isSuperTypeOf(domainElement.eClass())) {
				return hub.top.adaptiveSystem.diagram.edit.parts.EventDoNetEditPart.VISUAL_ID;
			}
			break;
		case hub.top.adaptiveSystem.diagram.edit.parts.AdaptiveSystemEditPart.VISUAL_ID:
			if (hub.top.adaptiveSystem.AdaptiveSystemPackage.eINSTANCE
					.getAdaptiveProcess().isSuperTypeOf(domainElement.eClass())) {
				return hub.top.adaptiveSystem.diagram.edit.parts.AdaptiveProcessEditPart.VISUAL_ID;
			}
			if (hub.top.adaptiveSystem.AdaptiveSystemPackage.eINSTANCE
					.getOclet().isSuperTypeOf(domainElement.eClass())) {
				return hub.top.adaptiveSystem.diagram.edit.parts.OcletEditPart.VISUAL_ID;
			}
			break;
		}
		return -1;
	}

	/**
	 * @generated
	 */
	public static boolean canCreateNode(View containerView, int nodeVisualID) {
		String containerModelID = hub.top.adaptiveSystem.diagram.part.AdaptiveSystemVisualIDRegistry
				.getModelID(containerView);
		if (!hub.top.adaptiveSystem.diagram.edit.parts.AdaptiveSystemEditPart.MODEL_ID
				.equals(containerModelID)) {
			return false;
		}
		int containerVisualID;
		if (hub.top.adaptiveSystem.diagram.edit.parts.AdaptiveSystemEditPart.MODEL_ID
				.equals(containerModelID)) {
			containerVisualID = hub.top.adaptiveSystem.diagram.part.AdaptiveSystemVisualIDRegistry
					.getVisualID(containerView);
		} else {
			if (containerView instanceof Diagram) {
				containerVisualID = hub.top.adaptiveSystem.diagram.edit.parts.AdaptiveSystemEditPart.VISUAL_ID;
			} else {
				return false;
			}
		}
		switch (containerVisualID) {
		case hub.top.adaptiveSystem.diagram.edit.parts.AdaptiveProcessEditPart.VISUAL_ID:
			if (hub.top.adaptiveSystem.diagram.edit.parts.AdaptiveProcessCompartmentEditPart.VISUAL_ID == nodeVisualID) {
				return true;
			}
			break;
		case hub.top.adaptiveSystem.diagram.edit.parts.OcletEditPart.VISUAL_ID:
			if (hub.top.adaptiveSystem.diagram.edit.parts.OcletNameEditPart.VISUAL_ID == nodeVisualID) {
				return true;
			}
			if (hub.top.adaptiveSystem.diagram.edit.parts.OcletQuantorOrientationEditPart.VISUAL_ID == nodeVisualID) {
				return true;
			}
			if (hub.top.adaptiveSystem.diagram.edit.parts.PreNetEditPart.VISUAL_ID == nodeVisualID) {
				return true;
			}
			if (hub.top.adaptiveSystem.diagram.edit.parts.DoNetEditPart.VISUAL_ID == nodeVisualID) {
				return true;
			}
			break;
		case hub.top.adaptiveSystem.diagram.edit.parts.ConditionAPEditPart.VISUAL_ID:
			if (hub.top.adaptiveSystem.diagram.edit.parts.ConditionAPNameEditPart.VISUAL_ID == nodeVisualID) {
				return true;
			}
			if (hub.top.adaptiveSystem.diagram.edit.parts.ConditionAPTempEditPart.VISUAL_ID == nodeVisualID) {
				return true;
			}
			if (hub.top.adaptiveSystem.diagram.edit.parts.ConditionAPTokenEditPart.VISUAL_ID == nodeVisualID) {
				return true;
			}
			break;
		case hub.top.adaptiveSystem.diagram.edit.parts.EventAPEditPart.VISUAL_ID:
			if (hub.top.adaptiveSystem.diagram.edit.parts.EventAPNameEditPart.VISUAL_ID == nodeVisualID) {
				return true;
			}
			if (hub.top.adaptiveSystem.diagram.edit.parts.EventAPTempEditPart.VISUAL_ID == nodeVisualID) {
				return true;
			}
			break;
		case hub.top.adaptiveSystem.diagram.edit.parts.PreNetEditPart.VISUAL_ID:
			if (hub.top.adaptiveSystem.diagram.edit.parts.PreNetCompartmentEditPart.VISUAL_ID == nodeVisualID) {
				return true;
			}
			break;
		case hub.top.adaptiveSystem.diagram.edit.parts.ConditionPreNetEditPart.VISUAL_ID:
			if (hub.top.adaptiveSystem.diagram.edit.parts.ConditionPreNetNameEditPart.VISUAL_ID == nodeVisualID) {
				return true;
			}
			if (hub.top.adaptiveSystem.diagram.edit.parts.ConditionPreNetTempEditPart.VISUAL_ID == nodeVisualID) {
				return true;
			}
			if (hub.top.adaptiveSystem.diagram.edit.parts.ConditionPreNetTokenEditPart.VISUAL_ID == nodeVisualID) {
				return true;
			}
			break;
		case hub.top.adaptiveSystem.diagram.edit.parts.EventPreNetEditPart.VISUAL_ID:
			if (hub.top.adaptiveSystem.diagram.edit.parts.EventPreNetNameEditPart.VISUAL_ID == nodeVisualID) {
				return true;
			}
			if (hub.top.adaptiveSystem.diagram.edit.parts.EventPreNetTempEditPart.VISUAL_ID == nodeVisualID) {
				return true;
			}
			break;
		case hub.top.adaptiveSystem.diagram.edit.parts.DoNetEditPart.VISUAL_ID:
			if (hub.top.adaptiveSystem.diagram.edit.parts.DoNetCompartmentEditPart.VISUAL_ID == nodeVisualID) {
				return true;
			}
			break;
		case hub.top.adaptiveSystem.diagram.edit.parts.ConditionDoNetEditPart.VISUAL_ID:
			if (hub.top.adaptiveSystem.diagram.edit.parts.ConditionDoNetNameEditPart.VISUAL_ID == nodeVisualID) {
				return true;
			}
			if (hub.top.adaptiveSystem.diagram.edit.parts.ConditionDoNetTempEditPart.VISUAL_ID == nodeVisualID) {
				return true;
			}
			if (hub.top.adaptiveSystem.diagram.edit.parts.ConditionDoNetTokenEditPart.VISUAL_ID == nodeVisualID) {
				return true;
			}
			break;
		case hub.top.adaptiveSystem.diagram.edit.parts.EventDoNetEditPart.VISUAL_ID:
			if (hub.top.adaptiveSystem.diagram.edit.parts.EventDoNetNameEditPart.VISUAL_ID == nodeVisualID) {
				return true;
			}
			if (hub.top.adaptiveSystem.diagram.edit.parts.EventDoNetTempEditPart.VISUAL_ID == nodeVisualID) {
				return true;
			}
			break;
		case hub.top.adaptiveSystem.diagram.edit.parts.AdaptiveProcessCompartmentEditPart.VISUAL_ID:
			if (hub.top.adaptiveSystem.diagram.edit.parts.ConditionAPEditPart.VISUAL_ID == nodeVisualID) {
				return true;
			}
			if (hub.top.adaptiveSystem.diagram.edit.parts.EventAPEditPart.VISUAL_ID == nodeVisualID) {
				return true;
			}
			break;
		case hub.top.adaptiveSystem.diagram.edit.parts.PreNetCompartmentEditPart.VISUAL_ID:
			if (hub.top.adaptiveSystem.diagram.edit.parts.ConditionPreNetEditPart.VISUAL_ID == nodeVisualID) {
				return true;
			}
			if (hub.top.adaptiveSystem.diagram.edit.parts.EventPreNetEditPart.VISUAL_ID == nodeVisualID) {
				return true;
			}
			break;
		case hub.top.adaptiveSystem.diagram.edit.parts.DoNetCompartmentEditPart.VISUAL_ID:
			if (hub.top.adaptiveSystem.diagram.edit.parts.ConditionDoNetEditPart.VISUAL_ID == nodeVisualID) {
				return true;
			}
			if (hub.top.adaptiveSystem.diagram.edit.parts.EventDoNetEditPart.VISUAL_ID == nodeVisualID) {
				return true;
			}
			break;
		case hub.top.adaptiveSystem.diagram.edit.parts.AdaptiveSystemEditPart.VISUAL_ID:
			if (hub.top.adaptiveSystem.diagram.edit.parts.AdaptiveProcessEditPart.VISUAL_ID == nodeVisualID) {
				return true;
			}
			if (hub.top.adaptiveSystem.diagram.edit.parts.OcletEditPart.VISUAL_ID == nodeVisualID) {
				return true;
			}
			break;
		case hub.top.adaptiveSystem.diagram.edit.parts.ArcToConditionEditPart.VISUAL_ID:
			if (hub.top.adaptiveSystem.diagram.edit.parts.ArcToConditionWeightEditPart.VISUAL_ID == nodeVisualID) {
				return true;
			}
			break;
		case hub.top.adaptiveSystem.diagram.edit.parts.ArcToEventEditPart.VISUAL_ID:
			if (hub.top.adaptiveSystem.diagram.edit.parts.ArcToEventWeightEditPart.VISUAL_ID == nodeVisualID) {
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
		if (hub.top.adaptiveSystem.AdaptiveSystemPackage.eINSTANCE
				.getArcToCondition().isSuperTypeOf(domainElement.eClass())) {
			return hub.top.adaptiveSystem.diagram.edit.parts.ArcToConditionEditPart.VISUAL_ID;
		}
		if (hub.top.adaptiveSystem.AdaptiveSystemPackage.eINSTANCE
				.getArcToEvent().isSuperTypeOf(domainElement.eClass())) {
			return hub.top.adaptiveSystem.diagram.edit.parts.ArcToEventEditPart.VISUAL_ID;
		}
		return -1;
	}

	/**
	 * User can change implementation of this method to handle some specific
	 * situations not covered by default logic.
	 * 
	 * @generated
	 */
	private static boolean isDiagram(
			hub.top.adaptiveSystem.AdaptiveSystem element) {
		return true;
	}

}
