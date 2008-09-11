package hub.top.adaptiveSystem.diagram.part;

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
public class AdaptiveSystemDiagramUpdater {

	/**
	 * @generated
	 */
	public static List getSemanticChildren(View view) {
		switch (hub.top.adaptiveSystem.diagram.part.AdaptiveSystemVisualIDRegistry
				.getVisualID(view)) {
		case hub.top.adaptiveSystem.diagram.edit.parts.OcletEditPart.VISUAL_ID:
			return getOclet_1002SemanticChildren(view);
		case hub.top.adaptiveSystem.diagram.edit.parts.AdaptiveProcessCompartmentEditPart.VISUAL_ID:
			return getAdaptiveProcessAdaptiveProcessCompartment_5001SemanticChildren(view);
		case hub.top.adaptiveSystem.diagram.edit.parts.PreNetCompartmentEditPart.VISUAL_ID:
			return getPreNetPreNetCompartment_5002SemanticChildren(view);
		case hub.top.adaptiveSystem.diagram.edit.parts.DoNetCompartmentEditPart.VISUAL_ID:
			return getDoNetDoNetCompartment_5003SemanticChildren(view);
		case hub.top.adaptiveSystem.diagram.edit.parts.AdaptiveSystemEditPart.VISUAL_ID:
			return getAdaptiveSystem_79SemanticChildren(view);
		}
		return Collections.EMPTY_LIST;
	}

	/**
	 * @generated
	 */
	public static List getOclet_1002SemanticChildren(View view) {
		if (!view.isSetElement()) {
			return Collections.EMPTY_LIST;
		}
		hub.top.adaptiveSystem.Oclet modelElement = (hub.top.adaptiveSystem.Oclet) view
				.getElement();
		List result = new LinkedList();
		{
			hub.top.adaptiveSystem.PreNet childElement = modelElement
					.getPreNet();
			int visualID = hub.top.adaptiveSystem.diagram.part.AdaptiveSystemVisualIDRegistry
					.getNodeVisualID(view, childElement);
			if (visualID == hub.top.adaptiveSystem.diagram.edit.parts.PreNetEditPart.VISUAL_ID) {
				result
						.add(new hub.top.adaptiveSystem.diagram.part.AdaptiveSystemNodeDescriptor(
								childElement, visualID));
			}
		}
		{
			hub.top.adaptiveSystem.DoNet childElement = modelElement.getDoNet();
			int visualID = hub.top.adaptiveSystem.diagram.part.AdaptiveSystemVisualIDRegistry
					.getNodeVisualID(view, childElement);
			if (visualID == hub.top.adaptiveSystem.diagram.edit.parts.DoNetEditPart.VISUAL_ID) {
				result
						.add(new hub.top.adaptiveSystem.diagram.part.AdaptiveSystemNodeDescriptor(
								childElement, visualID));
			}
		}
		return result;
	}

	/**
	 * @generated
	 */
	public static List getAdaptiveProcessAdaptiveProcessCompartment_5001SemanticChildren(
			View view) {
		if (false == view.eContainer() instanceof View) {
			return Collections.EMPTY_LIST;
		}
		View containerView = (View) view.eContainer();
		if (!containerView.isSetElement()) {
			return Collections.EMPTY_LIST;
		}
		hub.top.adaptiveSystem.AdaptiveProcess modelElement = (hub.top.adaptiveSystem.AdaptiveProcess) containerView
				.getElement();
		List result = new LinkedList();
		for (Iterator it = modelElement.getNodes().iterator(); it.hasNext();) {
			hub.top.adaptiveSystem.Node childElement = (hub.top.adaptiveSystem.Node) it
					.next();
			int visualID = hub.top.adaptiveSystem.diagram.part.AdaptiveSystemVisualIDRegistry
					.getNodeVisualID(view, childElement);
			if (visualID == hub.top.adaptiveSystem.diagram.edit.parts.ConditionAPEditPart.VISUAL_ID) {
				result
						.add(new hub.top.adaptiveSystem.diagram.part.AdaptiveSystemNodeDescriptor(
								childElement, visualID));
				continue;
			}
			if (visualID == hub.top.adaptiveSystem.diagram.edit.parts.EventAPEditPart.VISUAL_ID) {
				result
						.add(new hub.top.adaptiveSystem.diagram.part.AdaptiveSystemNodeDescriptor(
								childElement, visualID));
				continue;
			}
		}
		return result;
	}

	/**
	 * @generated
	 */
	public static List getPreNetPreNetCompartment_5002SemanticChildren(View view) {
		if (false == view.eContainer() instanceof View) {
			return Collections.EMPTY_LIST;
		}
		View containerView = (View) view.eContainer();
		if (!containerView.isSetElement()) {
			return Collections.EMPTY_LIST;
		}
		hub.top.adaptiveSystem.PreNet modelElement = (hub.top.adaptiveSystem.PreNet) containerView
				.getElement();
		List result = new LinkedList();
		for (Iterator it = modelElement.getNodes().iterator(); it.hasNext();) {
			hub.top.adaptiveSystem.Node childElement = (hub.top.adaptiveSystem.Node) it
					.next();
			int visualID = hub.top.adaptiveSystem.diagram.part.AdaptiveSystemVisualIDRegistry
					.getNodeVisualID(view, childElement);
			if (visualID == hub.top.adaptiveSystem.diagram.edit.parts.ConditionPreNetEditPart.VISUAL_ID) {
				result
						.add(new hub.top.adaptiveSystem.diagram.part.AdaptiveSystemNodeDescriptor(
								childElement, visualID));
				continue;
			}
			if (visualID == hub.top.adaptiveSystem.diagram.edit.parts.EventPreNetEditPart.VISUAL_ID) {
				result
						.add(new hub.top.adaptiveSystem.diagram.part.AdaptiveSystemNodeDescriptor(
								childElement, visualID));
				continue;
			}
		}
		return result;
	}

	/**
	 * @generated
	 */
	public static List getDoNetDoNetCompartment_5003SemanticChildren(View view) {
		if (false == view.eContainer() instanceof View) {
			return Collections.EMPTY_LIST;
		}
		View containerView = (View) view.eContainer();
		if (!containerView.isSetElement()) {
			return Collections.EMPTY_LIST;
		}
		hub.top.adaptiveSystem.DoNet modelElement = (hub.top.adaptiveSystem.DoNet) containerView
				.getElement();
		List result = new LinkedList();
		for (Iterator it = modelElement.getNodes().iterator(); it.hasNext();) {
			hub.top.adaptiveSystem.Node childElement = (hub.top.adaptiveSystem.Node) it
					.next();
			int visualID = hub.top.adaptiveSystem.diagram.part.AdaptiveSystemVisualIDRegistry
					.getNodeVisualID(view, childElement);
			if (visualID == hub.top.adaptiveSystem.diagram.edit.parts.ConditionDoNetEditPart.VISUAL_ID) {
				result
						.add(new hub.top.adaptiveSystem.diagram.part.AdaptiveSystemNodeDescriptor(
								childElement, visualID));
				continue;
			}
			if (visualID == hub.top.adaptiveSystem.diagram.edit.parts.EventDoNetEditPart.VISUAL_ID) {
				result
						.add(new hub.top.adaptiveSystem.diagram.part.AdaptiveSystemNodeDescriptor(
								childElement, visualID));
				continue;
			}
		}
		return result;
	}

	/**
	 * @generated
	 */
	public static List getAdaptiveSystem_79SemanticChildren(View view) {
		if (!view.isSetElement()) {
			return Collections.EMPTY_LIST;
		}
		hub.top.adaptiveSystem.AdaptiveSystem modelElement = (hub.top.adaptiveSystem.AdaptiveSystem) view
				.getElement();
		List result = new LinkedList();
		{
			hub.top.adaptiveSystem.AdaptiveProcess childElement = modelElement
					.getAdaptiveProcess();
			int visualID = hub.top.adaptiveSystem.diagram.part.AdaptiveSystemVisualIDRegistry
					.getNodeVisualID(view, childElement);
			if (visualID == hub.top.adaptiveSystem.diagram.edit.parts.AdaptiveProcessEditPart.VISUAL_ID) {
				result
						.add(new hub.top.adaptiveSystem.diagram.part.AdaptiveSystemNodeDescriptor(
								childElement, visualID));
			}
		}
		for (Iterator it = modelElement.getOclets().iterator(); it.hasNext();) {
			hub.top.adaptiveSystem.Oclet childElement = (hub.top.adaptiveSystem.Oclet) it
					.next();
			int visualID = hub.top.adaptiveSystem.diagram.part.AdaptiveSystemVisualIDRegistry
					.getNodeVisualID(view, childElement);
			if (visualID == hub.top.adaptiveSystem.diagram.edit.parts.OcletEditPart.VISUAL_ID) {
				result
						.add(new hub.top.adaptiveSystem.diagram.part.AdaptiveSystemNodeDescriptor(
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
		switch (hub.top.adaptiveSystem.diagram.part.AdaptiveSystemVisualIDRegistry
				.getVisualID(view)) {
		case hub.top.adaptiveSystem.diagram.edit.parts.AdaptiveSystemEditPart.VISUAL_ID:
			return getAdaptiveSystem_79ContainedLinks(view);
		case hub.top.adaptiveSystem.diagram.edit.parts.AdaptiveProcessEditPart.VISUAL_ID:
			return getAdaptiveProcess_1001ContainedLinks(view);
		case hub.top.adaptiveSystem.diagram.edit.parts.OcletEditPart.VISUAL_ID:
			return getOclet_1002ContainedLinks(view);
		case hub.top.adaptiveSystem.diagram.edit.parts.ConditionAPEditPart.VISUAL_ID:
			return getCondition_2001ContainedLinks(view);
		case hub.top.adaptiveSystem.diagram.edit.parts.EventAPEditPart.VISUAL_ID:
			return getEvent_2002ContainedLinks(view);
		case hub.top.adaptiveSystem.diagram.edit.parts.PreNetEditPart.VISUAL_ID:
			return getPreNet_2003ContainedLinks(view);
		case hub.top.adaptiveSystem.diagram.edit.parts.ConditionPreNetEditPart.VISUAL_ID:
			return getCondition_2004ContainedLinks(view);
		case hub.top.adaptiveSystem.diagram.edit.parts.EventPreNetEditPart.VISUAL_ID:
			return getEvent_2005ContainedLinks(view);
		case hub.top.adaptiveSystem.diagram.edit.parts.DoNetEditPart.VISUAL_ID:
			return getDoNet_2006ContainedLinks(view);
		case hub.top.adaptiveSystem.diagram.edit.parts.ConditionDoNetEditPart.VISUAL_ID:
			return getCondition_2007ContainedLinks(view);
		case hub.top.adaptiveSystem.diagram.edit.parts.EventDoNetEditPart.VISUAL_ID:
			return getEvent_2008ContainedLinks(view);
		case hub.top.adaptiveSystem.diagram.edit.parts.ArcToConditionEditPart.VISUAL_ID:
			return getArcToCondition_3001ContainedLinks(view);
		case hub.top.adaptiveSystem.diagram.edit.parts.ArcToEventEditPart.VISUAL_ID:
			return getArcToEvent_3002ContainedLinks(view);
		}
		return Collections.EMPTY_LIST;
	}

	/**
	 * @generated
	 */
	public static List getIncomingLinks(View view) {
		switch (hub.top.adaptiveSystem.diagram.part.AdaptiveSystemVisualIDRegistry
				.getVisualID(view)) {
		case hub.top.adaptiveSystem.diagram.edit.parts.AdaptiveProcessEditPart.VISUAL_ID:
			return getAdaptiveProcess_1001IncomingLinks(view);
		case hub.top.adaptiveSystem.diagram.edit.parts.OcletEditPart.VISUAL_ID:
			return getOclet_1002IncomingLinks(view);
		case hub.top.adaptiveSystem.diagram.edit.parts.ConditionAPEditPart.VISUAL_ID:
			return getCondition_2001IncomingLinks(view);
		case hub.top.adaptiveSystem.diagram.edit.parts.EventAPEditPart.VISUAL_ID:
			return getEvent_2002IncomingLinks(view);
		case hub.top.adaptiveSystem.diagram.edit.parts.PreNetEditPart.VISUAL_ID:
			return getPreNet_2003IncomingLinks(view);
		case hub.top.adaptiveSystem.diagram.edit.parts.ConditionPreNetEditPart.VISUAL_ID:
			return getCondition_2004IncomingLinks(view);
		case hub.top.adaptiveSystem.diagram.edit.parts.EventPreNetEditPart.VISUAL_ID:
			return getEvent_2005IncomingLinks(view);
		case hub.top.adaptiveSystem.diagram.edit.parts.DoNetEditPart.VISUAL_ID:
			return getDoNet_2006IncomingLinks(view);
		case hub.top.adaptiveSystem.diagram.edit.parts.ConditionDoNetEditPart.VISUAL_ID:
			return getCondition_2007IncomingLinks(view);
		case hub.top.adaptiveSystem.diagram.edit.parts.EventDoNetEditPart.VISUAL_ID:
			return getEvent_2008IncomingLinks(view);
		case hub.top.adaptiveSystem.diagram.edit.parts.ArcToConditionEditPart.VISUAL_ID:
			return getArcToCondition_3001IncomingLinks(view);
		case hub.top.adaptiveSystem.diagram.edit.parts.ArcToEventEditPart.VISUAL_ID:
			return getArcToEvent_3002IncomingLinks(view);
		}
		return Collections.EMPTY_LIST;
	}

	/**
	 * @generated
	 */
	public static List getOutgoingLinks(View view) {
		switch (hub.top.adaptiveSystem.diagram.part.AdaptiveSystemVisualIDRegistry
				.getVisualID(view)) {
		case hub.top.adaptiveSystem.diagram.edit.parts.AdaptiveProcessEditPart.VISUAL_ID:
			return getAdaptiveProcess_1001OutgoingLinks(view);
		case hub.top.adaptiveSystem.diagram.edit.parts.OcletEditPart.VISUAL_ID:
			return getOclet_1002OutgoingLinks(view);
		case hub.top.adaptiveSystem.diagram.edit.parts.ConditionAPEditPart.VISUAL_ID:
			return getCondition_2001OutgoingLinks(view);
		case hub.top.adaptiveSystem.diagram.edit.parts.EventAPEditPart.VISUAL_ID:
			return getEvent_2002OutgoingLinks(view);
		case hub.top.adaptiveSystem.diagram.edit.parts.PreNetEditPart.VISUAL_ID:
			return getPreNet_2003OutgoingLinks(view);
		case hub.top.adaptiveSystem.diagram.edit.parts.ConditionPreNetEditPart.VISUAL_ID:
			return getCondition_2004OutgoingLinks(view);
		case hub.top.adaptiveSystem.diagram.edit.parts.EventPreNetEditPart.VISUAL_ID:
			return getEvent_2005OutgoingLinks(view);
		case hub.top.adaptiveSystem.diagram.edit.parts.DoNetEditPart.VISUAL_ID:
			return getDoNet_2006OutgoingLinks(view);
		case hub.top.adaptiveSystem.diagram.edit.parts.ConditionDoNetEditPart.VISUAL_ID:
			return getCondition_2007OutgoingLinks(view);
		case hub.top.adaptiveSystem.diagram.edit.parts.EventDoNetEditPart.VISUAL_ID:
			return getEvent_2008OutgoingLinks(view);
		case hub.top.adaptiveSystem.diagram.edit.parts.ArcToConditionEditPart.VISUAL_ID:
			return getArcToCondition_3001OutgoingLinks(view);
		case hub.top.adaptiveSystem.diagram.edit.parts.ArcToEventEditPart.VISUAL_ID:
			return getArcToEvent_3002OutgoingLinks(view);
		}
		return Collections.EMPTY_LIST;
	}

	/**
	 * @generated
	 */
	public static List getAdaptiveSystem_79ContainedLinks(View view) {
		return Collections.EMPTY_LIST;
	}

	/**
	 * @generated
	 */
	public static List getAdaptiveProcess_1001ContainedLinks(View view) {
		hub.top.adaptiveSystem.AdaptiveProcess modelElement = (hub.top.adaptiveSystem.AdaptiveProcess) view
				.getElement();
		List result = new LinkedList();
		result
				.addAll(getContainedTypeModelFacetLinks_ArcToCondition_3001(modelElement));
		result
				.addAll(getContainedTypeModelFacetLinks_ArcToEvent_3002(modelElement));
		return result;
	}

	/**
	 * @generated
	 */
	public static List getOclet_1002ContainedLinks(View view) {
		return Collections.EMPTY_LIST;
	}

	/**
	 * @generated
	 */
	public static List getCondition_2001ContainedLinks(View view) {
		return Collections.EMPTY_LIST;
	}

	/**
	 * @generated
	 */
	public static List getEvent_2002ContainedLinks(View view) {
		return Collections.EMPTY_LIST;
	}

	/**
	 * @generated
	 */
	public static List getPreNet_2003ContainedLinks(View view) {
		hub.top.adaptiveSystem.PreNet modelElement = (hub.top.adaptiveSystem.PreNet) view
				.getElement();
		List result = new LinkedList();
		result
				.addAll(getContainedTypeModelFacetLinks_ArcToCondition_3001(modelElement));
		result
				.addAll(getContainedTypeModelFacetLinks_ArcToEvent_3002(modelElement));
		return result;
	}

	/**
	 * @generated
	 */
	public static List getCondition_2004ContainedLinks(View view) {
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
	public static List getDoNet_2006ContainedLinks(View view) {
		hub.top.adaptiveSystem.DoNet modelElement = (hub.top.adaptiveSystem.DoNet) view
				.getElement();
		List result = new LinkedList();
		result
				.addAll(getContainedTypeModelFacetLinks_ArcToCondition_3001(modelElement));
		result
				.addAll(getContainedTypeModelFacetLinks_ArcToEvent_3002(modelElement));
		return result;
	}

	/**
	 * @generated
	 */
	public static List getCondition_2007ContainedLinks(View view) {
		return Collections.EMPTY_LIST;
	}

	/**
	 * @generated
	 */
	public static List getEvent_2008ContainedLinks(View view) {
		return Collections.EMPTY_LIST;
	}

	/**
	 * @generated
	 */
	public static List getArcToCondition_3001ContainedLinks(View view) {
		return Collections.EMPTY_LIST;
	}

	/**
	 * @generated
	 */
	public static List getArcToEvent_3002ContainedLinks(View view) {
		return Collections.EMPTY_LIST;
	}

	/**
	 * @generated
	 */
	public static List getAdaptiveProcess_1001IncomingLinks(View view) {
		return Collections.EMPTY_LIST;
	}

	/**
	 * @generated
	 */
	public static List getOclet_1002IncomingLinks(View view) {
		return Collections.EMPTY_LIST;
	}

	/**
	 * @generated
	 */
	public static List getCondition_2001IncomingLinks(View view) {
		hub.top.adaptiveSystem.Condition modelElement = (hub.top.adaptiveSystem.Condition) view
				.getElement();
		Map crossReferences = EcoreUtil.CrossReferencer.find(view.eResource()
				.getResourceSet().getResources());
		List result = new LinkedList();
		result.addAll(getIncomingTypeModelFacetLinks_ArcToCondition_3001(
				modelElement, crossReferences));
		return result;
	}

	/**
	 * @generated
	 */
	public static List getEvent_2002IncomingLinks(View view) {
		hub.top.adaptiveSystem.Event modelElement = (hub.top.adaptiveSystem.Event) view
				.getElement();
		Map crossReferences = EcoreUtil.CrossReferencer.find(view.eResource()
				.getResourceSet().getResources());
		List result = new LinkedList();
		result.addAll(getIncomingTypeModelFacetLinks_ArcToEvent_3002(
				modelElement, crossReferences));
		return result;
	}

	/**
	 * @generated
	 */
	public static List getPreNet_2003IncomingLinks(View view) {
		return Collections.EMPTY_LIST;
	}

	/**
	 * @generated
	 */
	public static List getCondition_2004IncomingLinks(View view) {
		hub.top.adaptiveSystem.Condition modelElement = (hub.top.adaptiveSystem.Condition) view
				.getElement();
		Map crossReferences = EcoreUtil.CrossReferencer.find(view.eResource()
				.getResourceSet().getResources());
		List result = new LinkedList();
		result.addAll(getIncomingTypeModelFacetLinks_ArcToCondition_3001(
				modelElement, crossReferences));
		return result;
	}

	/**
	 * @generated
	 */
	public static List getEvent_2005IncomingLinks(View view) {
		hub.top.adaptiveSystem.Event modelElement = (hub.top.adaptiveSystem.Event) view
				.getElement();
		Map crossReferences = EcoreUtil.CrossReferencer.find(view.eResource()
				.getResourceSet().getResources());
		List result = new LinkedList();
		result.addAll(getIncomingTypeModelFacetLinks_ArcToEvent_3002(
				modelElement, crossReferences));
		return result;
	}

	/**
	 * @generated
	 */
	public static List getDoNet_2006IncomingLinks(View view) {
		return Collections.EMPTY_LIST;
	}

	/**
	 * @generated
	 */
	public static List getCondition_2007IncomingLinks(View view) {
		hub.top.adaptiveSystem.Condition modelElement = (hub.top.adaptiveSystem.Condition) view
				.getElement();
		Map crossReferences = EcoreUtil.CrossReferencer.find(view.eResource()
				.getResourceSet().getResources());
		List result = new LinkedList();
		result.addAll(getIncomingTypeModelFacetLinks_ArcToCondition_3001(
				modelElement, crossReferences));
		return result;
	}

	/**
	 * @generated
	 */
	public static List getEvent_2008IncomingLinks(View view) {
		hub.top.adaptiveSystem.Event modelElement = (hub.top.adaptiveSystem.Event) view
				.getElement();
		Map crossReferences = EcoreUtil.CrossReferencer.find(view.eResource()
				.getResourceSet().getResources());
		List result = new LinkedList();
		result.addAll(getIncomingTypeModelFacetLinks_ArcToEvent_3002(
				modelElement, crossReferences));
		return result;
	}

	/**
	 * @generated
	 */
	public static List getArcToCondition_3001IncomingLinks(View view) {
		return Collections.EMPTY_LIST;
	}

	/**
	 * @generated
	 */
	public static List getArcToEvent_3002IncomingLinks(View view) {
		return Collections.EMPTY_LIST;
	}

	/**
	 * @generated
	 */
	public static List getAdaptiveProcess_1001OutgoingLinks(View view) {
		return Collections.EMPTY_LIST;
	}

	/**
	 * @generated
	 */
	public static List getOclet_1002OutgoingLinks(View view) {
		return Collections.EMPTY_LIST;
	}

	/**
	 * @generated
	 */
	public static List getCondition_2001OutgoingLinks(View view) {
		hub.top.adaptiveSystem.Condition modelElement = (hub.top.adaptiveSystem.Condition) view
				.getElement();
		List result = new LinkedList();
		result
				.addAll(getOutgoingTypeModelFacetLinks_ArcToEvent_3002(modelElement));
		return result;
	}

	/**
	 * @generated
	 */
	public static List getEvent_2002OutgoingLinks(View view) {
		hub.top.adaptiveSystem.Event modelElement = (hub.top.adaptiveSystem.Event) view
				.getElement();
		List result = new LinkedList();
		result
				.addAll(getOutgoingTypeModelFacetLinks_ArcToCondition_3001(modelElement));
		return result;
	}

	/**
	 * @generated
	 */
	public static List getPreNet_2003OutgoingLinks(View view) {
		return Collections.EMPTY_LIST;
	}

	/**
	 * @generated
	 */
	public static List getCondition_2004OutgoingLinks(View view) {
		hub.top.adaptiveSystem.Condition modelElement = (hub.top.adaptiveSystem.Condition) view
				.getElement();
		List result = new LinkedList();
		result
				.addAll(getOutgoingTypeModelFacetLinks_ArcToEvent_3002(modelElement));
		return result;
	}

	/**
	 * @generated
	 */
	public static List getEvent_2005OutgoingLinks(View view) {
		hub.top.adaptiveSystem.Event modelElement = (hub.top.adaptiveSystem.Event) view
				.getElement();
		List result = new LinkedList();
		result
				.addAll(getOutgoingTypeModelFacetLinks_ArcToCondition_3001(modelElement));
		return result;
	}

	/**
	 * @generated
	 */
	public static List getDoNet_2006OutgoingLinks(View view) {
		return Collections.EMPTY_LIST;
	}

	/**
	 * @generated
	 */
	public static List getCondition_2007OutgoingLinks(View view) {
		hub.top.adaptiveSystem.Condition modelElement = (hub.top.adaptiveSystem.Condition) view
				.getElement();
		List result = new LinkedList();
		result
				.addAll(getOutgoingTypeModelFacetLinks_ArcToEvent_3002(modelElement));
		return result;
	}

	/**
	 * @generated
	 */
	public static List getEvent_2008OutgoingLinks(View view) {
		hub.top.adaptiveSystem.Event modelElement = (hub.top.adaptiveSystem.Event) view
				.getElement();
		List result = new LinkedList();
		result
				.addAll(getOutgoingTypeModelFacetLinks_ArcToCondition_3001(modelElement));
		return result;
	}

	/**
	 * @generated
	 */
	public static List getArcToCondition_3001OutgoingLinks(View view) {
		return Collections.EMPTY_LIST;
	}

	/**
	 * @generated
	 */
	public static List getArcToEvent_3002OutgoingLinks(View view) {
		return Collections.EMPTY_LIST;
	}

	/**
	 * @generated
	 */
	private static Collection getContainedTypeModelFacetLinks_ArcToCondition_3001(
			hub.top.adaptiveSystem.OccurrenceNet container) {
		Collection result = new LinkedList();
		for (Iterator links = container.getArcs().iterator(); links.hasNext();) {
			Object linkObject = links.next();
			if (false == linkObject instanceof hub.top.adaptiveSystem.ArcToCondition) {
				continue;
			}
			hub.top.adaptiveSystem.ArcToCondition link = (hub.top.adaptiveSystem.ArcToCondition) linkObject;
			if (hub.top.adaptiveSystem.diagram.edit.parts.ArcToConditionEditPart.VISUAL_ID != hub.top.adaptiveSystem.diagram.part.AdaptiveSystemVisualIDRegistry
					.getLinkWithClassVisualID(link)) {
				continue;
			}
			hub.top.adaptiveSystem.Condition dst = link.getDestination();
			hub.top.adaptiveSystem.Event src = link.getSource();
			result
					.add(new hub.top.adaptiveSystem.diagram.part.AdaptiveSystemLinkDescriptor(
							src,
							dst,
							link,
							hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToCondition_3001,
							hub.top.adaptiveSystem.diagram.edit.parts.ArcToConditionEditPart.VISUAL_ID));
		}
		return result;
	}

	/**
	 * @generated
	 */
	private static Collection getContainedTypeModelFacetLinks_ArcToEvent_3002(
			hub.top.adaptiveSystem.OccurrenceNet container) {
		Collection result = new LinkedList();
		for (Iterator links = container.getArcs().iterator(); links.hasNext();) {
			Object linkObject = links.next();
			if (false == linkObject instanceof hub.top.adaptiveSystem.ArcToEvent) {
				continue;
			}
			hub.top.adaptiveSystem.ArcToEvent link = (hub.top.adaptiveSystem.ArcToEvent) linkObject;
			if (hub.top.adaptiveSystem.diagram.edit.parts.ArcToEventEditPart.VISUAL_ID != hub.top.adaptiveSystem.diagram.part.AdaptiveSystemVisualIDRegistry
					.getLinkWithClassVisualID(link)) {
				continue;
			}
			hub.top.adaptiveSystem.Event dst = link.getDestination();
			hub.top.adaptiveSystem.Condition src = link.getSource();
			result
					.add(new hub.top.adaptiveSystem.diagram.part.AdaptiveSystemLinkDescriptor(
							src,
							dst,
							link,
							hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToEvent_3002,
							hub.top.adaptiveSystem.diagram.edit.parts.ArcToEventEditPart.VISUAL_ID));
		}
		return result;
	}

	/**
	 * @generated
	 */
	private static Collection getIncomingTypeModelFacetLinks_ArcToCondition_3001(
			hub.top.adaptiveSystem.Condition target, Map crossReferences) {
		Collection result = new LinkedList();
		Collection settings = (Collection) crossReferences.get(target);
		for (Iterator it = settings.iterator(); it.hasNext();) {
			EStructuralFeature.Setting setting = (EStructuralFeature.Setting) it
					.next();
			if (setting.getEStructuralFeature() != hub.top.adaptiveSystem.AdaptiveSystemPackage.eINSTANCE
					.getArcToCondition_Destination()
					|| false == setting.getEObject() instanceof hub.top.adaptiveSystem.ArcToCondition) {
				continue;
			}
			hub.top.adaptiveSystem.ArcToCondition link = (hub.top.adaptiveSystem.ArcToCondition) setting
					.getEObject();
			if (hub.top.adaptiveSystem.diagram.edit.parts.ArcToConditionEditPart.VISUAL_ID != hub.top.adaptiveSystem.diagram.part.AdaptiveSystemVisualIDRegistry
					.getLinkWithClassVisualID(link)) {
				continue;
			}
			hub.top.adaptiveSystem.Event src = link.getSource();
			result
					.add(new hub.top.adaptiveSystem.diagram.part.AdaptiveSystemLinkDescriptor(
							src,
							target,
							link,
							hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToCondition_3001,
							hub.top.adaptiveSystem.diagram.edit.parts.ArcToConditionEditPart.VISUAL_ID));
		}
		return result;
	}

	/**
	 * @generated
	 */
	private static Collection getIncomingTypeModelFacetLinks_ArcToEvent_3002(
			hub.top.adaptiveSystem.Event target, Map crossReferences) {
		Collection result = new LinkedList();
		Collection settings = (Collection) crossReferences.get(target);
		for (Iterator it = settings.iterator(); it.hasNext();) {
			EStructuralFeature.Setting setting = (EStructuralFeature.Setting) it
					.next();
			if (setting.getEStructuralFeature() != hub.top.adaptiveSystem.AdaptiveSystemPackage.eINSTANCE
					.getArcToEvent_Destination()
					|| false == setting.getEObject() instanceof hub.top.adaptiveSystem.ArcToEvent) {
				continue;
			}
			hub.top.adaptiveSystem.ArcToEvent link = (hub.top.adaptiveSystem.ArcToEvent) setting
					.getEObject();
			if (hub.top.adaptiveSystem.diagram.edit.parts.ArcToEventEditPart.VISUAL_ID != hub.top.adaptiveSystem.diagram.part.AdaptiveSystemVisualIDRegistry
					.getLinkWithClassVisualID(link)) {
				continue;
			}
			hub.top.adaptiveSystem.Condition src = link.getSource();
			result
					.add(new hub.top.adaptiveSystem.diagram.part.AdaptiveSystemLinkDescriptor(
							src,
							target,
							link,
							hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToEvent_3002,
							hub.top.adaptiveSystem.diagram.edit.parts.ArcToEventEditPart.VISUAL_ID));
		}
		return result;
	}

	/**
	 * @generated
	 */
	private static Collection getOutgoingTypeModelFacetLinks_ArcToCondition_3001(
			hub.top.adaptiveSystem.Event source) {
		hub.top.adaptiveSystem.OccurrenceNet container = null;
		// Find container element for the link.
		// Climb up by containment hierarchy starting from the source
		// and return the first element that is instance of the container class.
		for (EObject element = source; element != null && container == null; element = element
				.eContainer()) {
			if (element instanceof hub.top.adaptiveSystem.OccurrenceNet) {
				container = (hub.top.adaptiveSystem.OccurrenceNet) element;
			}
		}
		if (container == null) {
			return Collections.EMPTY_LIST;
		}
		Collection result = new LinkedList();
		for (Iterator links = container.getArcs().iterator(); links.hasNext();) {
			Object linkObject = links.next();
			if (false == linkObject instanceof hub.top.adaptiveSystem.ArcToCondition) {
				continue;
			}
			hub.top.adaptiveSystem.ArcToCondition link = (hub.top.adaptiveSystem.ArcToCondition) linkObject;
			if (hub.top.adaptiveSystem.diagram.edit.parts.ArcToConditionEditPart.VISUAL_ID != hub.top.adaptiveSystem.diagram.part.AdaptiveSystemVisualIDRegistry
					.getLinkWithClassVisualID(link)) {
				continue;
			}
			hub.top.adaptiveSystem.Condition dst = link.getDestination();
			hub.top.adaptiveSystem.Event src = link.getSource();
			if (src != source) {
				continue;
			}
			result
					.add(new hub.top.adaptiveSystem.diagram.part.AdaptiveSystemLinkDescriptor(
							src,
							dst,
							link,
							hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToCondition_3001,
							hub.top.adaptiveSystem.diagram.edit.parts.ArcToConditionEditPart.VISUAL_ID));
		}
		return result;
	}

	/**
	 * @generated
	 */
	private static Collection getOutgoingTypeModelFacetLinks_ArcToEvent_3002(
			hub.top.adaptiveSystem.Condition source) {
		hub.top.adaptiveSystem.OccurrenceNet container = null;
		// Find container element for the link.
		// Climb up by containment hierarchy starting from the source
		// and return the first element that is instance of the container class.
		for (EObject element = source; element != null && container == null; element = element
				.eContainer()) {
			if (element instanceof hub.top.adaptiveSystem.OccurrenceNet) {
				container = (hub.top.adaptiveSystem.OccurrenceNet) element;
			}
		}
		if (container == null) {
			return Collections.EMPTY_LIST;
		}
		Collection result = new LinkedList();
		for (Iterator links = container.getArcs().iterator(); links.hasNext();) {
			Object linkObject = links.next();
			if (false == linkObject instanceof hub.top.adaptiveSystem.ArcToEvent) {
				continue;
			}
			hub.top.adaptiveSystem.ArcToEvent link = (hub.top.adaptiveSystem.ArcToEvent) linkObject;
			if (hub.top.adaptiveSystem.diagram.edit.parts.ArcToEventEditPart.VISUAL_ID != hub.top.adaptiveSystem.diagram.part.AdaptiveSystemVisualIDRegistry
					.getLinkWithClassVisualID(link)) {
				continue;
			}
			hub.top.adaptiveSystem.Event dst = link.getDestination();
			hub.top.adaptiveSystem.Condition src = link.getSource();
			if (src != source) {
				continue;
			}
			result
					.add(new hub.top.adaptiveSystem.diagram.part.AdaptiveSystemLinkDescriptor(
							src,
							dst,
							link,
							hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToEvent_3002,
							hub.top.adaptiveSystem.diagram.edit.parts.ArcToEventEditPart.VISUAL_ID));
		}
		return result;
	}

}
