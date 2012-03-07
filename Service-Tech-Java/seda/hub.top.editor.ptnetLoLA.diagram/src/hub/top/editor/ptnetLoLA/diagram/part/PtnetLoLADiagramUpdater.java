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
  public static List<hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLANodeDescriptor> getSemanticChildren(
      View view) {
    switch (hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLAVisualIDRegistry
        .getVisualID(view)) {
    case hub.top.editor.ptnetLoLA.diagram.edit.parts.PtNetEditPart.VISUAL_ID:
      return getPtNet_1000SemanticChildren(view);
    }
    return Collections.emptyList();
  }

  /**
   * @generated
   */
  public static List<hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLANodeDescriptor> getPtNet_1000SemanticChildren(
      View view) {
    if (!view.isSetElement()) {
      return Collections.emptyList();
    }
    hub.top.editor.ptnetLoLA.PtNet modelElement = (hub.top.editor.ptnetLoLA.PtNet) view
        .getElement();
    LinkedList<hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLANodeDescriptor> result = new LinkedList<hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLANodeDescriptor>();
    for (Iterator<?> it = modelElement.getTransitions().iterator(); it
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
    for (Iterator<?> it = modelElement.getPlaces().iterator(); it.hasNext();) {
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
  public static List<hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLALinkDescriptor> getContainedLinks(
      View view) {
    switch (hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLAVisualIDRegistry
        .getVisualID(view)) {
    case hub.top.editor.ptnetLoLA.diagram.edit.parts.PtNetEditPart.VISUAL_ID:
      return getPtNet_1000ContainedLinks(view);
    case hub.top.editor.ptnetLoLA.diagram.edit.parts.TransitionEditPart.VISUAL_ID:
      return getTransition_2001ContainedLinks(view);
    case hub.top.editor.ptnetLoLA.diagram.edit.parts.PlaceEditPart.VISUAL_ID:
      return getPlace_2002ContainedLinks(view);
    case hub.top.editor.ptnetLoLA.diagram.edit.parts.ArcToPlaceEditPart.VISUAL_ID:
      return getArcToPlace_4001ContainedLinks(view);
    case hub.top.editor.ptnetLoLA.diagram.edit.parts.ArcToTransitionEditPart.VISUAL_ID:
      return getArcToTransition_4002ContainedLinks(view);
    }
    return Collections.emptyList();
  }

  /**
   * @generated
   */
  public static List<hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLALinkDescriptor> getIncomingLinks(
      View view) {
    switch (hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLAVisualIDRegistry
        .getVisualID(view)) {
    case hub.top.editor.ptnetLoLA.diagram.edit.parts.TransitionEditPart.VISUAL_ID:
      return getTransition_2001IncomingLinks(view);
    case hub.top.editor.ptnetLoLA.diagram.edit.parts.PlaceEditPart.VISUAL_ID:
      return getPlace_2002IncomingLinks(view);
    case hub.top.editor.ptnetLoLA.diagram.edit.parts.ArcToPlaceEditPart.VISUAL_ID:
      return getArcToPlace_4001IncomingLinks(view);
    case hub.top.editor.ptnetLoLA.diagram.edit.parts.ArcToTransitionEditPart.VISUAL_ID:
      return getArcToTransition_4002IncomingLinks(view);
    }
    return Collections.emptyList();
  }

  /**
   * @generated
   */
  public static List<hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLALinkDescriptor> getOutgoingLinks(
      View view) {
    switch (hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLAVisualIDRegistry
        .getVisualID(view)) {
    case hub.top.editor.ptnetLoLA.diagram.edit.parts.TransitionEditPart.VISUAL_ID:
      return getTransition_2001OutgoingLinks(view);
    case hub.top.editor.ptnetLoLA.diagram.edit.parts.PlaceEditPart.VISUAL_ID:
      return getPlace_2002OutgoingLinks(view);
    case hub.top.editor.ptnetLoLA.diagram.edit.parts.ArcToPlaceEditPart.VISUAL_ID:
      return getArcToPlace_4001OutgoingLinks(view);
    case hub.top.editor.ptnetLoLA.diagram.edit.parts.ArcToTransitionEditPart.VISUAL_ID:
      return getArcToTransition_4002OutgoingLinks(view);
    }
    return Collections.emptyList();
  }

  /**
   * @generated
   */
  public static List<hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLALinkDescriptor> getPtNet_1000ContainedLinks(
      View view) {
    hub.top.editor.ptnetLoLA.PtNet modelElement = (hub.top.editor.ptnetLoLA.PtNet) view
        .getElement();
    LinkedList<hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLALinkDescriptor> result = new LinkedList<hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLALinkDescriptor>();
    result
        .addAll(getContainedTypeModelFacetLinks_ArcToPlace_4001(modelElement));
    result
        .addAll(getContainedTypeModelFacetLinks_ArcToTransition_4002(modelElement));
    return result;
  }

  /**
   * @generated
   */
  public static List<hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLALinkDescriptor> getTransition_2001ContainedLinks(
      View view) {
    return Collections.emptyList();
  }

  /**
   * @generated
   */
  public static List<hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLALinkDescriptor> getPlace_2002ContainedLinks(
      View view) {
    return Collections.emptyList();
  }

  /**
   * @generated
   */
  public static List<hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLALinkDescriptor> getArcToPlace_4001ContainedLinks(
      View view) {
    return Collections.emptyList();
  }

  /**
   * @generated
   */
  public static List<hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLALinkDescriptor> getArcToTransition_4002ContainedLinks(
      View view) {
    return Collections.emptyList();
  }

  /**
   * @generated
   */
  public static List<hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLALinkDescriptor> getTransition_2001IncomingLinks(
      View view) {
    hub.top.editor.ptnetLoLA.Transition modelElement = (hub.top.editor.ptnetLoLA.Transition) view
        .getElement();
    Map<EObject, Collection<EStructuralFeature.Setting>> crossReferences = EcoreUtil.CrossReferencer
        .find(view.eResource().getResourceSet().getResources());
    LinkedList<hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLALinkDescriptor> result = new LinkedList<hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLALinkDescriptor>();
    result.addAll(getIncomingTypeModelFacetLinks_ArcToPlace_4001(modelElement,
        crossReferences));
    result.addAll(getIncomingTypeModelFacetLinks_ArcToTransition_4002(
        modelElement, crossReferences));
    return result;
  }

  /**
   * @generated
   */
  public static List<hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLALinkDescriptor> getPlace_2002IncomingLinks(
      View view) {
    hub.top.editor.ptnetLoLA.Place modelElement = (hub.top.editor.ptnetLoLA.Place) view
        .getElement();
    Map<EObject, Collection<EStructuralFeature.Setting>> crossReferences = EcoreUtil.CrossReferencer
        .find(view.eResource().getResourceSet().getResources());
    LinkedList<hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLALinkDescriptor> result = new LinkedList<hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLALinkDescriptor>();
    result.addAll(getIncomingTypeModelFacetLinks_ArcToPlace_4001(modelElement,
        crossReferences));
    result.addAll(getIncomingTypeModelFacetLinks_ArcToTransition_4002(
        modelElement, crossReferences));
    return result;
  }

  /**
   * @generated
   */
  public static List<hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLALinkDescriptor> getArcToPlace_4001IncomingLinks(
      View view) {
    return Collections.emptyList();
  }

  /**
   * @generated
   */
  public static List<hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLALinkDescriptor> getArcToTransition_4002IncomingLinks(
      View view) {
    return Collections.emptyList();
  }

  /**
   * @generated
   */
  public static List<hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLALinkDescriptor> getTransition_2001OutgoingLinks(
      View view) {
    hub.top.editor.ptnetLoLA.Transition modelElement = (hub.top.editor.ptnetLoLA.Transition) view
        .getElement();
    LinkedList<hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLALinkDescriptor> result = new LinkedList<hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLALinkDescriptor>();
    result.addAll(getOutgoingTypeModelFacetLinks_ArcToPlace_4001(modelElement));
    result
        .addAll(getOutgoingTypeModelFacetLinks_ArcToTransition_4002(modelElement));
    return result;
  }

  /**
   * @generated
   */
  public static List<hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLALinkDescriptor> getPlace_2002OutgoingLinks(
      View view) {
    hub.top.editor.ptnetLoLA.Place modelElement = (hub.top.editor.ptnetLoLA.Place) view
        .getElement();
    LinkedList<hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLALinkDescriptor> result = new LinkedList<hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLALinkDescriptor>();
    result.addAll(getOutgoingTypeModelFacetLinks_ArcToPlace_4001(modelElement));
    result
        .addAll(getOutgoingTypeModelFacetLinks_ArcToTransition_4002(modelElement));
    return result;
  }

  /**
   * @generated
   */
  public static List<hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLALinkDescriptor> getArcToPlace_4001OutgoingLinks(
      View view) {
    return Collections.emptyList();
  }

  /**
   * @generated
   */
  public static List<hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLALinkDescriptor> getArcToTransition_4002OutgoingLinks(
      View view) {
    return Collections.emptyList();
  }

  /**
   * @generated
   */
  private static Collection<hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLALinkDescriptor> getContainedTypeModelFacetLinks_ArcToPlace_4001(
      hub.top.editor.ptnetLoLA.PtNet container) {
    LinkedList<hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLALinkDescriptor> result = new LinkedList<hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLALinkDescriptor>();
    for (Iterator<?> links = container.getArcs().iterator(); links.hasNext();) {
      EObject linkObject = (EObject) links.next();
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
              hub.top.editor.ptnetLoLA.diagram.providers.PtnetLoLAElementTypes.ArcToPlace_4001,
              hub.top.editor.ptnetLoLA.diagram.edit.parts.ArcToPlaceEditPart.VISUAL_ID));
    }
    return result;
  }

  /**
   * @generated
   */
  private static Collection<hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLALinkDescriptor> getContainedTypeModelFacetLinks_ArcToTransition_4002(
      hub.top.editor.ptnetLoLA.PtNet container) {
    LinkedList<hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLALinkDescriptor> result = new LinkedList<hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLALinkDescriptor>();
    for (Iterator<?> links = container.getArcs().iterator(); links.hasNext();) {
      EObject linkObject = (EObject) links.next();
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
              hub.top.editor.ptnetLoLA.diagram.providers.PtnetLoLAElementTypes.ArcToTransition_4002,
              hub.top.editor.ptnetLoLA.diagram.edit.parts.ArcToTransitionEditPart.VISUAL_ID));
    }
    return result;
  }

  /**
   * @generated
   */
  private static Collection<hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLALinkDescriptor> getIncomingTypeModelFacetLinks_ArcToPlace_4001(
      hub.top.editor.ptnetLoLA.Node target,
      Map<EObject, Collection<EStructuralFeature.Setting>> crossReferences) {
    LinkedList<hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLALinkDescriptor> result = new LinkedList<hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLALinkDescriptor>();
    Collection<EStructuralFeature.Setting> settings = crossReferences
        .get(target);
    for (EStructuralFeature.Setting setting : settings) {
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
              hub.top.editor.ptnetLoLA.diagram.providers.PtnetLoLAElementTypes.ArcToPlace_4001,
              hub.top.editor.ptnetLoLA.diagram.edit.parts.ArcToPlaceEditPart.VISUAL_ID));
    }
    return result;
  }

  /**
   * @generated
   */
  private static Collection<hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLALinkDescriptor> getIncomingTypeModelFacetLinks_ArcToTransition_4002(
      hub.top.editor.ptnetLoLA.Node target,
      Map<EObject, Collection<EStructuralFeature.Setting>> crossReferences) {
    LinkedList<hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLALinkDescriptor> result = new LinkedList<hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLALinkDescriptor>();
    Collection<EStructuralFeature.Setting> settings = crossReferences
        .get(target);
    for (EStructuralFeature.Setting setting : settings) {
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
              hub.top.editor.ptnetLoLA.diagram.providers.PtnetLoLAElementTypes.ArcToTransition_4002,
              hub.top.editor.ptnetLoLA.diagram.edit.parts.ArcToTransitionEditPart.VISUAL_ID));
    }
    return result;
  }

  /**
   * @generated
   */
  private static Collection<hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLALinkDescriptor> getOutgoingTypeModelFacetLinks_ArcToPlace_4001(
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
      return Collections.emptyList();
    }
    LinkedList<hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLALinkDescriptor> result = new LinkedList<hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLALinkDescriptor>();
    for (Iterator<?> links = container.getArcs().iterator(); links.hasNext();) {
      EObject linkObject = (EObject) links.next();
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
              hub.top.editor.ptnetLoLA.diagram.providers.PtnetLoLAElementTypes.ArcToPlace_4001,
              hub.top.editor.ptnetLoLA.diagram.edit.parts.ArcToPlaceEditPart.VISUAL_ID));
    }
    return result;
  }

  /**
   * @generated
   */
  private static Collection<hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLALinkDescriptor> getOutgoingTypeModelFacetLinks_ArcToTransition_4002(
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
      return Collections.emptyList();
    }
    LinkedList<hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLALinkDescriptor> result = new LinkedList<hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLALinkDescriptor>();
    for (Iterator<?> links = container.getArcs().iterator(); links.hasNext();) {
      EObject linkObject = (EObject) links.next();
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
              hub.top.editor.ptnetLoLA.diagram.providers.PtnetLoLAElementTypes.ArcToTransition_4002,
              hub.top.editor.ptnetLoLA.diagram.edit.parts.ArcToTransitionEditPart.VISUAL_ID));
    }
    return result;
  }

}
