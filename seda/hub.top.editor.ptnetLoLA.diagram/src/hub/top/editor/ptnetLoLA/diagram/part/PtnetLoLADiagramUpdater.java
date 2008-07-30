package hub.top.editor.ptnetLoLA.diagram.part;

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
public class PtnetLoLADiagramUpdater {

	/**
	 * @generated
	 */
	public static List getSemanticChildren(View view) {
		switch (hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLAVisualIDRegistry
				.getVisualID(view)) {
		case hub.top.editor.ptnetLoLA.diagram.edit.parts.PtNetEditPart.VISUAL_ID:
			return getPtNet_79SemanticChildren(view);
		}
		return Collections.EMPTY_LIST;
	}

	/**
	 * @generated
	 */
	public static List getPtNet_79SemanticChildren(View view) {
		if (!view.isSetElement()) {
			return Collections.EMPTY_LIST;
		}
		hub.top.editor.ptnetLoLA.PtNet modelElement = (hub.top.editor.ptnetLoLA.PtNet) view
				.getElement();
		List result = new LinkedList();
		for (Iterator it = modelElement.getTransitions().iterator(); it
				.hasNext();) {
			hub.top.editor.ptnetLoLA.Transition childElement = (hub.top.editor.ptnetLoLA.Transition) it
					.next();
			int visualID = hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLAVisualIDRegistry
					.getNodeVisualID(view, childElement);
			if (visualID == hub.top.editor.ptnetLoLA.diagram.edit.parts.TransitionEditPart.VISUAL_ID) {
				result
						.add(new hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLANodeDescriptor(
								childElement, visualID));
				continue;
			}
		}
		for (Iterator it = modelElement.getPlaces().iterator(); it.hasNext();) {
			hub.top.editor.ptnetLoLA.Place childElement = (hub.top.editor.ptnetLoLA.Place) it
					.next();
			int visualID = hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLAVisualIDRegistry
					.getNodeVisualID(view, childElement);
			if (visualID == hub.top.editor.ptnetLoLA.diagram.edit.parts.PlaceEditPart.VISUAL_ID) {
				result
						.add(new hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLANodeDescriptor(
								childElement, visualID));
				continue;
			}
		}
		return result;
	}

	/**
	 * @generated
	 */
	public static List getContainedLinks(View view) {
		switch (hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLAVisualIDRegistry
				.getVisualID(view)) {
		case hub.top.editor.ptnetLoLA.diagram.edit.parts.PtNetEditPart.VISUAL_ID:
			return getPtNet_79ContainedLinks(view);
		case hub.top.editor.ptnetLoLA.diagram.edit.parts.TransitionEditPart.VISUAL_ID:
			return getTransition_1001ContainedLinks(view);
		case hub.top.editor.ptnetLoLA.diagram.edit.parts.PlaceEditPart.VISUAL_ID:
			return getPlace_1002ContainedLinks(view);
		case hub.top.editor.ptnetLoLA.diagram.edit.parts.ArcToPlaceEditPart.VISUAL_ID:
			return getArcToPlace_3001ContainedLinks(view);
		case hub.top.editor.ptnetLoLA.diagram.edit.parts.ArcToTransitionEditPart.VISUAL_ID:
			return getArcToTransition_3002ContainedLinks(view);
		}
		return Collections.EMPTY_LIST;
	}

	/**
	 * @generated
	 */
	public static List getIncomingLinks(View view) {
		switch (hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLAVisualIDRegistry
				.getVisualID(view)) {
		case hub.top.editor.ptnetLoLA.diagram.edit.parts.TransitionEditPart.VISUAL_ID:
			return getTransition_1001IncomingLinks(view);
		case hub.top.editor.ptnetLoLA.diagram.edit.parts.PlaceEditPart.VISUAL_ID:
			return getPlace_1002IncomingLinks(view);
		case hub.top.editor.ptnetLoLA.diagram.edit.parts.ArcToPlaceEditPart.VISUAL_ID:
			return getArcToPlace_3001IncomingLinks(view);
		case hub.top.editor.ptnetLoLA.diagram.edit.parts.ArcToTransitionEditPart.VISUAL_ID:
			return getArcToTransition_3002IncomingLinks(view);
		}
		return Collections.EMPTY_LIST;
	}

	/**
	 * @generated
	 */
	public static List getOutgoingLinks(View view) {
		switch (hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLAVisualIDRegistry
				.getVisualID(view)) {
		case hub.top.editor.ptnetLoLA.diagram.edit.parts.TransitionEditPart.VISUAL_ID:
			return getTransition_1001OutgoingLinks(view);
		case hub.top.editor.ptnetLoLA.diagram.edit.parts.PlaceEditPart.VISUAL_ID:
			return getPlace_1002OutgoingLinks(view);
		case hub.top.editor.ptnetLoLA.diagram.edit.parts.ArcToPlaceEditPart.VISUAL_ID:
			return getArcToPlace_3001OutgoingLinks(view);
		case hub.top.editor.ptnetLoLA.diagram.edit.parts.ArcToTransitionEditPart.VISUAL_ID:
			return getArcToTransition_3002OutgoingLinks(view);
		}
		return Collections.EMPTY_LIST;
	}

	/**
	 * @generated
	 */
	public static List getPtNet_79ContainedLinks(View view) {
		hub.top.editor.ptnetLoLA.PtNet modelElement = (hub.top.editor.ptnetLoLA.PtNet) view
				.getElement();
		List result = new LinkedList();
		result
				.addAll(getContainedTypeModelFacetLinks_ArcToPlace_3001(modelElement));
		result
				.addAll(getContainedTypeModelFacetLinks_ArcToTransition_3002(modelElement));
		return result;
	}

	/**
	 * @generated
	 */
	public static List getTransition_1001ContainedLinks(View view) {
		return Collections.EMPTY_LIST;
	}

	/**
	 * @generated
	 */
	public static List getPlace_1002ContainedLinks(View view) {
		return Collections.EMPTY_LIST;
	}

	/**
	 * @generated
	 */
	public static List getArcToPlace_3001ContainedLinks(View view) {
		return Collections.EMPTY_LIST;
	}

	/**
	 * @generated
	 */
	public static List getArcToTransition_3002ContainedLinks(View view) {
		return Collections.EMPTY_LIST;
	}

	/**
	 * @generated
	 */
	public static List getTransition_1001IncomingLinks(View view) {
		hub.top.editor.ptnetLoLA.Transition modelElement = (hub.top.editor.ptnetLoLA.Transition) view
				.getElement();
		Map crossReferences = EcoreUtil.CrossReferencer.find(view.eResource()
				.getResourceSet().getResources());
		List result = new LinkedList();
		result.addAll(getIncomingTypeModelFacetLinks_ArcToPlace_3001(
				modelElement, crossReferences));
		result.addAll(getIncomingTypeModelFacetLinks_ArcToTransition_3002(
				modelElement, crossReferences));
		return result;
	}

	/**
	 * @generated
	 */
	public static List getPlace_1002IncomingLinks(View view) {
		hub.top.editor.ptnetLoLA.Place modelElement = (hub.top.editor.ptnetLoLA.Place) view
				.getElement();
		Map crossReferences = EcoreUtil.CrossReferencer.find(view.eResource()
				.getResourceSet().getResources());
		List result = new LinkedList();
		result.addAll(getIncomingTypeModelFacetLinks_ArcToPlace_3001(
				modelElement, crossReferences));
		result.addAll(getIncomingTypeModelFacetLinks_ArcToTransition_3002(
				modelElement, crossReferences));
		return result;
	}

	/**
	 * @generated
	 */
	public static List getArcToPlace_3001IncomingLinks(View view) {
		return Collections.EMPTY_LIST;
	}

	/**
	 * @generated
	 */
	public static List getArcToTransition_3002IncomingLinks(View view) {
		return Collections.EMPTY_LIST;
	}

	/**
	 * @generated
	 */
	public static List getTransition_1001OutgoingLinks(View view) {
		hub.top.editor.ptnetLoLA.Transition modelElement = (hub.top.editor.ptnetLoLA.Transition) view
				.getElement();
		List result = new LinkedList();
		result
				.addAll(getOutgoingTypeModelFacetLinks_ArcToPlace_3001(modelElement));
		result
				.addAll(getOutgoingTypeModelFacetLinks_ArcToTransition_3002(modelElement));
		return result;
	}

	/**
	 * @generated
	 */
	public static List getPlace_1002OutgoingLinks(View view) {
		hub.top.editor.ptnetLoLA.Place modelElement = (hub.top.editor.ptnetLoLA.Place) view
				.getElement();
		List result = new LinkedList();
		result
				.addAll(getOutgoingTypeModelFacetLinks_ArcToPlace_3001(modelElement));
		result
				.addAll(getOutgoingTypeModelFacetLinks_ArcToTransition_3002(modelElement));
		return result;
	}

	/**
	 * @generated
	 */
	public static List getArcToPlace_3001OutgoingLinks(View view) {
		return Collections.EMPTY_LIST;
	}

	/**
	 * @generated
	 */
	public static List getArcToTransition_3002OutgoingLinks(View view) {
		return Collections.EMPTY_LIST;
	}

	/**
	 * @generated
	 */
	private static Collection getContainedTypeModelFacetLinks_ArcToPlace_3001(
			hub.top.editor.ptnetLoLA.PtNet container) {
		Collection result = new LinkedList();
		for (Iterator links = container.getArcs().iterator(); links.hasNext();) {
			Object linkObject = links.next();
			if (false == linkObject instanceof hub.top.editor.ptnetLoLA.ArcToPlace) {
				continue;
			}
			hub.top.editor.ptnetLoLA.ArcToPlace link = (hub.top.editor.ptnetLoLA.ArcToPlace) linkObject;
			if (hub.top.editor.ptnetLoLA.diagram.edit.parts.ArcToPlaceEditPart.VISUAL_ID != hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLAVisualIDRegistry
					.getLinkWithClassVisualID(link)) {
				continue;
			}
			hub.top.editor.ptnetLoLA.Node dst = link.getTarget();
			hub.top.editor.ptnetLoLA.Node src = link.getSource();
			result
					.add(new hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLALinkDescriptor(
							src,
							dst,
							link,
							hub.top.editor.ptnetLoLA.diagram.providers.PtnetLoLAElementTypes.ArcToPlace_3001,
							hub.top.editor.ptnetLoLA.diagram.edit.parts.ArcToPlaceEditPart.VISUAL_ID));
		}
		return result;
	}

	/**
	 * @generated
	 */
	private static Collection getContainedTypeModelFacetLinks_ArcToTransition_3002(
			hub.top.editor.ptnetLoLA.PtNet container) {
		Collection result = new LinkedList();
		for (Iterator links = container.getArcs().iterator(); links.hasNext();) {
			Object linkObject = links.next();
			if (false == linkObject instanceof hub.top.editor.ptnetLoLA.ArcToTransition) {
				continue;
			}
			hub.top.editor.ptnetLoLA.ArcToTransition link = (hub.top.editor.ptnetLoLA.ArcToTransition) linkObject;
			if (hub.top.editor.ptnetLoLA.diagram.edit.parts.ArcToTransitionEditPart.VISUAL_ID != hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLAVisualIDRegistry
					.getLinkWithClassVisualID(link)) {
				continue;
			}
			hub.top.editor.ptnetLoLA.Node dst = link.getTarget();
			hub.top.editor.ptnetLoLA.Node src = link.getSource();
			result
					.add(new hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLALinkDescriptor(
							src,
							dst,
							link,
							hub.top.editor.ptnetLoLA.diagram.providers.PtnetLoLAElementTypes.ArcToTransition_3002,
							hub.top.editor.ptnetLoLA.diagram.edit.parts.ArcToTransitionEditPart.VISUAL_ID));
		}
		return result;
	}

	/**
	 * @generated
	 */
	private static Collection getIncomingTypeModelFacetLinks_ArcToPlace_3001(
			hub.top.editor.ptnetLoLA.Node target, Map crossReferences) {
		Collection result = new LinkedList();
		Collection settings = (Collection) crossReferences.get(target);
		for (Iterator it = settings.iterator(); it.hasNext();) {
			EStructuralFeature.Setting setting = (EStructuralFeature.Setting) it
					.next();
			if (setting.getEStructuralFeature() != hub.top.editor.ptnetLoLA.PtnetLoLAPackage.eINSTANCE
					.getArc_Target()
					|| false == setting.getEObject() instanceof hub.top.editor.ptnetLoLA.ArcToPlace) {
				continue;
			}
			hub.top.editor.ptnetLoLA.ArcToPlace link = (hub.top.editor.ptnetLoLA.ArcToPlace) setting
					.getEObject();
			if (hub.top.editor.ptnetLoLA.diagram.edit.parts.ArcToPlaceEditPart.VISUAL_ID != hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLAVisualIDRegistry
					.getLinkWithClassVisualID(link)) {
				continue;
			}
			hub.top.editor.ptnetLoLA.Node src = link.getSource();
			result
					.add(new hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLALinkDescriptor(
							src,
							target,
							link,
							hub.top.editor.ptnetLoLA.diagram.providers.PtnetLoLAElementTypes.ArcToPlace_3001,
							hub.top.editor.ptnetLoLA.diagram.edit.parts.ArcToPlaceEditPart.VISUAL_ID));
		}
		return result;
	}

	/**
	 * @generated
	 */
	private static Collection getIncomingTypeModelFacetLinks_ArcToTransition_3002(
			hub.top.editor.ptnetLoLA.Node target, Map crossReferences) {
		Collection result = new LinkedList();
		Collection settings = (Collection) crossReferences.get(target);
		for (Iterator it = settings.iterator(); it.hasNext();) {
			EStructuralFeature.Setting setting = (EStructuralFeature.Setting) it
					.next();
			if (setting.getEStructuralFeature() != hub.top.editor.ptnetLoLA.PtnetLoLAPackage.eINSTANCE
					.getArc_Target()
					|| false == setting.getEObject() instanceof hub.top.editor.ptnetLoLA.ArcToTransition) {
				continue;
			}
			hub.top.editor.ptnetLoLA.ArcToTransition link = (hub.top.editor.ptnetLoLA.ArcToTransition) setting
					.getEObject();
			if (hub.top.editor.ptnetLoLA.diagram.edit.parts.ArcToTransitionEditPart.VISUAL_ID != hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLAVisualIDRegistry
					.getLinkWithClassVisualID(link)) {
				continue;
			}
			hub.top.editor.ptnetLoLA.Node src = link.getSource();
			result
					.add(new hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLALinkDescriptor(
							src,
							target,
							link,
							hub.top.editor.ptnetLoLA.diagram.providers.PtnetLoLAElementTypes.ArcToTransition_3002,
							hub.top.editor.ptnetLoLA.diagram.edit.parts.ArcToTransitionEditPart.VISUAL_ID));
		}
		return result;
	}

	/**
	 * @generated
	 */
	private static Collection getOutgoingTypeModelFacetLinks_ArcToPlace_3001(
			hub.top.editor.ptnetLoLA.Node source) {
		hub.top.editor.ptnetLoLA.PtNet container = null;
		// Find container element for the link.
		// Climb up by containment hierarchy starting from the source
		// and return the first element that is instance of the container class.
		for (EObject element = source; element != null && container == null; element = element
				.eContainer()) {
			if (element instanceof hub.top.editor.ptnetLoLA.PtNet) {
				container = (hub.top.editor.ptnetLoLA.PtNet) element;
			}
		}
		if (container == null) {
			return Collections.EMPTY_LIST;
		}
		Collection result = new LinkedList();
		for (Iterator links = container.getArcs().iterator(); links.hasNext();) {
			Object linkObject = links.next();
			if (false == linkObject instanceof hub.top.editor.ptnetLoLA.ArcToPlace) {
				continue;
			}
			hub.top.editor.ptnetLoLA.ArcToPlace link = (hub.top.editor.ptnetLoLA.ArcToPlace) linkObject;
			if (hub.top.editor.ptnetLoLA.diagram.edit.parts.ArcToPlaceEditPart.VISUAL_ID != hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLAVisualIDRegistry
					.getLinkWithClassVisualID(link)) {
				continue;
			}
			hub.top.editor.ptnetLoLA.Node dst = link.getTarget();
			hub.top.editor.ptnetLoLA.Node src = link.getSource();
			if (src != source) {
				continue;
			}
			result
					.add(new hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLALinkDescriptor(
							src,
							dst,
							link,
							hub.top.editor.ptnetLoLA.diagram.providers.PtnetLoLAElementTypes.ArcToPlace_3001,
							hub.top.editor.ptnetLoLA.diagram.edit.parts.ArcToPlaceEditPart.VISUAL_ID));
		}
		return result;
	}

	/**
	 * @generated
	 */
	private static Collection getOutgoingTypeModelFacetLinks_ArcToTransition_3002(
			hub.top.editor.ptnetLoLA.Node source) {
		hub.top.editor.ptnetLoLA.PtNet container = null;
		// Find container element for the link.
		// Climb up by containment hierarchy starting from the source
		// and return the first element that is instance of the container class.
		for (EObject element = source; element != null && container == null; element = element
				.eContainer()) {
			if (element instanceof hub.top.editor.ptnetLoLA.PtNet) {
				container = (hub.top.editor.ptnetLoLA.PtNet) element;
			}
		}
		if (container == null) {
			return Collections.EMPTY_LIST;
		}
		Collection result = new LinkedList();
		for (Iterator links = container.getArcs().iterator(); links.hasNext();) {
			Object linkObject = links.next();
			if (false == linkObject instanceof hub.top.editor.ptnetLoLA.ArcToTransition) {
				continue;
			}
			hub.top.editor.ptnetLoLA.ArcToTransition link = (hub.top.editor.ptnetLoLA.ArcToTransition) linkObject;
			if (hub.top.editor.ptnetLoLA.diagram.edit.parts.ArcToTransitionEditPart.VISUAL_ID != hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLAVisualIDRegistry
					.getLinkWithClassVisualID(link)) {
				continue;
			}
			hub.top.editor.ptnetLoLA.Node dst = link.getTarget();
			hub.top.editor.ptnetLoLA.Node src = link.getSource();
			if (src != source) {
				continue;
			}
			result
					.add(new hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLALinkDescriptor(
							src,
							dst,
							link,
							hub.top.editor.ptnetLoLA.diagram.providers.PtnetLoLAElementTypes.ArcToTransition_3002,
							hub.top.editor.ptnetLoLA.diagram.edit.parts.ArcToTransitionEditPart.VISUAL_ID));
		}
		return result;
	}

}
