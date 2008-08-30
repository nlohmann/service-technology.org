package hub.top.editor.ptnetLoLA.diagram.part;

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
public class PtnetLoLAVisualIDRegistry {

	/**
	 * @generated
	 */
	private static final String DEBUG_KEY = "hub.top.editor.ptnetLoLA.diagram/debug/visualID"; //$NON-NLS-1$

	/**
	 * @generated
	 */
	public static int getVisualID(View view) {
		if (view instanceof Diagram) {
			if (hub.top.editor.ptnetLoLA.diagram.edit.parts.PtNetEditPart.MODEL_ID
					.equals(view.getType())) {
				return hub.top.editor.ptnetLoLA.diagram.edit.parts.PtNetEditPart.VISUAL_ID;
			} else {
				return -1;
			}
		}
		return hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLAVisualIDRegistry
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
				hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLADiagramEditorPlugin
						.getInstance().logError(
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
		if (hub.top.editor.ptnetLoLA.PtnetLoLAPackage.eINSTANCE.getPtNet()
				.isSuperTypeOf(domainElement.eClass())
				&& isDiagram((hub.top.editor.ptnetLoLA.PtNet) domainElement)) {
			return hub.top.editor.ptnetLoLA.diagram.edit.parts.PtNetEditPart.VISUAL_ID;
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
		String containerModelID = hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLAVisualIDRegistry
				.getModelID(containerView);
		if (!hub.top.editor.ptnetLoLA.diagram.edit.parts.PtNetEditPart.MODEL_ID
				.equals(containerModelID)) {
			return -1;
		}
		int containerVisualID;
		if (hub.top.editor.ptnetLoLA.diagram.edit.parts.PtNetEditPart.MODEL_ID
				.equals(containerModelID)) {
			containerVisualID = hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLAVisualIDRegistry
					.getVisualID(containerView);
		} else {
			if (containerView instanceof Diagram) {
				containerVisualID = hub.top.editor.ptnetLoLA.diagram.edit.parts.PtNetEditPart.VISUAL_ID;
			} else {
				return -1;
			}
		}
		switch (containerVisualID) {
		case hub.top.editor.ptnetLoLA.diagram.edit.parts.PtNetEditPart.VISUAL_ID:
			if (hub.top.editor.ptnetLoLA.PtnetLoLAPackage.eINSTANCE.getTransition()
					.isSuperTypeOf(domainElement.eClass())) {
				return hub.top.editor.ptnetLoLA.diagram.edit.parts.TransitionEditPart.VISUAL_ID;
			}
			if (hub.top.editor.ptnetLoLA.PtnetLoLAPackage.eINSTANCE.getPlace()
					.isSuperTypeOf(domainElement.eClass())) {
				return hub.top.editor.ptnetLoLA.diagram.edit.parts.PlaceEditPart.VISUAL_ID;
			}
			break;
		}
		return -1;
	}

	/**
	 * @generated
	 */
	public static boolean canCreateNode(View containerView, int nodeVisualID) {
		String containerModelID = hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLAVisualIDRegistry
				.getModelID(containerView);
		if (!hub.top.editor.ptnetLoLA.diagram.edit.parts.PtNetEditPart.MODEL_ID
				.equals(containerModelID)) {
			return false;
		}
		int containerVisualID;
		if (hub.top.editor.ptnetLoLA.diagram.edit.parts.PtNetEditPart.MODEL_ID
				.equals(containerModelID)) {
			containerVisualID = hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLAVisualIDRegistry
					.getVisualID(containerView);
		} else {
			if (containerView instanceof Diagram) {
				containerVisualID = hub.top.editor.ptnetLoLA.diagram.edit.parts.PtNetEditPart.VISUAL_ID;
			} else {
				return false;
			}
		}
		switch (containerVisualID) {
		case hub.top.editor.ptnetLoLA.diagram.edit.parts.TransitionEditPart.VISUAL_ID:
			if (hub.top.editor.ptnetLoLA.diagram.edit.parts.TransitionNameEditPart.VISUAL_ID == nodeVisualID) {
				return true;
			}
			break;
		case hub.top.editor.ptnetLoLA.diagram.edit.parts.PlaceEditPart.VISUAL_ID:
			if (hub.top.editor.ptnetLoLA.diagram.edit.parts.PlaceNameEditPart.VISUAL_ID == nodeVisualID) {
				return true;
			}
			if (hub.top.editor.ptnetLoLA.diagram.edit.parts.PlaceTokenEditPart.VISUAL_ID == nodeVisualID) {
				return true;
			}
			break;
		case hub.top.editor.ptnetLoLA.diagram.edit.parts.PtNetEditPart.VISUAL_ID:
			if (hub.top.editor.ptnetLoLA.diagram.edit.parts.TransitionEditPart.VISUAL_ID == nodeVisualID) {
				return true;
			}
			if (hub.top.editor.ptnetLoLA.diagram.edit.parts.PlaceEditPart.VISUAL_ID == nodeVisualID) {
				return true;
			}
			break;
		case hub.top.editor.ptnetLoLA.diagram.edit.parts.ArcToPlaceEditPart.VISUAL_ID:
			if (hub.top.editor.ptnetLoLA.diagram.edit.parts.ArcToPlaceWeightEditPart.VISUAL_ID == nodeVisualID) {
				return true;
			}
			break;
		case hub.top.editor.ptnetLoLA.diagram.edit.parts.ArcToTransitionEditPart.VISUAL_ID:
			if (hub.top.editor.ptnetLoLA.diagram.edit.parts.ArcToTransitionWeightEditPart.VISUAL_ID == nodeVisualID) {
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
		if (hub.top.editor.ptnetLoLA.PtnetLoLAPackage.eINSTANCE.getArcToPlace()
				.isSuperTypeOf(domainElement.eClass())) {
			return hub.top.editor.ptnetLoLA.diagram.edit.parts.ArcToPlaceEditPart.VISUAL_ID;
		}
		if (hub.top.editor.ptnetLoLA.PtnetLoLAPackage.eINSTANCE
				.getArcToTransition().isSuperTypeOf(domainElement.eClass())) {
			return hub.top.editor.ptnetLoLA.diagram.edit.parts.ArcToTransitionEditPart.VISUAL_ID;
		}
		return -1;
	}

	/**
	 * User can change implementation of this method to handle some specific
	 * situations not covered by default logic.
	 * 
	 * @generated
	 */
	private static boolean isDiagram(hub.top.editor.ptnetLoLA.PtNet element) {
		return true;
	}

}
