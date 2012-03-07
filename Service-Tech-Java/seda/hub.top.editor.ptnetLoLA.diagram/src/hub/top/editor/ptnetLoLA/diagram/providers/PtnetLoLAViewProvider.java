package hub.top.editor.ptnetLoLA.diagram.providers;

import java.util.ArrayList;
import org.eclipse.core.runtime.IAdaptable;
import org.eclipse.emf.ecore.EAnnotation;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.EcoreFactory;
import org.eclipse.emf.transaction.util.TransactionUtil;
import org.eclipse.gmf.runtime.common.core.service.AbstractProvider;
import org.eclipse.gmf.runtime.common.core.service.IOperation;
import org.eclipse.gmf.runtime.diagram.core.preferences.PreferencesHint;
import org.eclipse.gmf.runtime.diagram.core.providers.AbstractViewProvider;
import org.eclipse.gmf.runtime.diagram.core.providers.IViewProvider;
import org.eclipse.gmf.runtime.diagram.core.services.view.CreateDiagramViewOperation;
import org.eclipse.gmf.runtime.diagram.core.services.view.CreateEdgeViewOperation;
import org.eclipse.gmf.runtime.diagram.core.services.view.CreateNodeViewOperation;
import org.eclipse.gmf.runtime.diagram.core.services.view.CreateViewForKindOperation;
import org.eclipse.gmf.runtime.diagram.core.services.view.CreateViewOperation;
import org.eclipse.gmf.runtime.diagram.core.util.ViewUtil;
import org.eclipse.gmf.runtime.diagram.ui.preferences.IPreferenceConstants;
import org.eclipse.gmf.runtime.draw2d.ui.figures.FigureUtilities;
import org.eclipse.gmf.runtime.emf.core.util.EMFCoreUtil;
import org.eclipse.gmf.runtime.emf.type.core.IElementType;
import org.eclipse.gmf.runtime.emf.type.core.IHintedType;
import org.eclipse.gmf.runtime.notation.DecorationNode;
import org.eclipse.gmf.runtime.notation.Diagram;
import org.eclipse.gmf.runtime.notation.Edge;
import org.eclipse.gmf.runtime.notation.FontStyle;
import org.eclipse.gmf.runtime.notation.Location;
import org.eclipse.gmf.runtime.notation.MeasurementUnit;
import org.eclipse.gmf.runtime.notation.Node;
import org.eclipse.gmf.runtime.notation.NotationFactory;
import org.eclipse.gmf.runtime.notation.NotationPackage;
import org.eclipse.gmf.runtime.notation.RelativeBendpoints;
import org.eclipse.gmf.runtime.notation.Routing;
import org.eclipse.gmf.runtime.notation.View;
import org.eclipse.gmf.runtime.notation.datatype.RelativeBendpoint;
import org.eclipse.jface.preference.IPreferenceStore;
import org.eclipse.jface.preference.PreferenceConverter;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.FontData;

/**
 * @generated
 */
public class PtnetLoLAViewProvider extends AbstractProvider implements
    IViewProvider {

  /**
   * @generated
   */
  public final boolean provides(IOperation operation) {
    if (operation instanceof CreateViewForKindOperation) {
      return provides((CreateViewForKindOperation) operation);
    }
    assert operation instanceof CreateViewOperation;
    if (operation instanceof CreateDiagramViewOperation) {
      return provides((CreateDiagramViewOperation) operation);
    } else if (operation instanceof CreateEdgeViewOperation) {
      return provides((CreateEdgeViewOperation) operation);
    } else if (operation instanceof CreateNodeViewOperation) {
      return provides((CreateNodeViewOperation) operation);
    }
    return false;
  }

  /**
   * @generated
   */
  protected boolean provides(CreateViewForKindOperation op) {
    /*
     if (op.getViewKind() == Node.class)
     return getNodeViewClass(op.getSemanticAdapter(), op.getContainerView(), op.getSemanticHint()) != null;
     if (op.getViewKind() == Edge.class)
     return getEdgeViewClass(op.getSemanticAdapter(), op.getContainerView(), op.getSemanticHint()) != null;
     */
    return true;
  }

  /**
   * @generated
   */
  protected boolean provides(CreateDiagramViewOperation op) {
    return hub.top.editor.ptnetLoLA.diagram.edit.parts.PtNetEditPart.MODEL_ID
        .equals(op.getSemanticHint())
        && hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLAVisualIDRegistry
            .getDiagramVisualID(getSemanticElement(op.getSemanticAdapter())) != -1;
  }

  /**
   * @generated
   */
  protected boolean provides(CreateNodeViewOperation op) {
    if (op.getContainerView() == null) {
      return false;
    }
    IElementType elementType = getSemanticElementType(op.getSemanticAdapter());
    EObject domainElement = getSemanticElement(op.getSemanticAdapter());
    int visualID;
    if (op.getSemanticHint() == null) {
      // Semantic hint is not specified. Can be a result of call from CanonicalEditPolicy.
      // In this situation there should be NO elementType, visualID will be determined
      // by VisualIDRegistry.getNodeVisualID() for domainElement.
      if (elementType != null || domainElement == null) {
        return false;
      }
      visualID = hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLAVisualIDRegistry
          .getNodeVisualID(op.getContainerView(), domainElement);
    } else {
      visualID = hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLAVisualIDRegistry
          .getVisualID(op.getSemanticHint());
      if (elementType != null) {
        if (!hub.top.editor.ptnetLoLA.diagram.providers.PtnetLoLAElementTypes
            .isKnownElementType(elementType)
            || (!(elementType instanceof IHintedType))) {
          return false; // foreign element type
        }
        String elementTypeHint = ((IHintedType) elementType).getSemanticHint();
        if (!op.getSemanticHint().equals(elementTypeHint)) {
          return false; // if semantic hint is specified it should be the same as in element type
        }
        if (domainElement != null
            && visualID != hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLAVisualIDRegistry
                .getNodeVisualID(op.getContainerView(), domainElement)) {
          return false; // visual id for node EClass should match visual id from element type
        }
      } else {
        if (!hub.top.editor.ptnetLoLA.diagram.edit.parts.PtNetEditPart.MODEL_ID
            .equals(hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLAVisualIDRegistry
                .getModelID(op.getContainerView()))) {
          return false; // foreign diagram
        }
        switch (visualID) {
        case hub.top.editor.ptnetLoLA.diagram.edit.parts.TransitionEditPart.VISUAL_ID:
        case hub.top.editor.ptnetLoLA.diagram.edit.parts.PlaceEditPart.VISUAL_ID:
          if (domainElement == null
              || visualID != hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLAVisualIDRegistry
                  .getNodeVisualID(op.getContainerView(), domainElement)) {
            return false; // visual id in semantic hint should match visual id for domain element
          }
          break;
        default:
          return false;
        }
      }
    }
    return hub.top.editor.ptnetLoLA.diagram.edit.parts.TransitionEditPart.VISUAL_ID == visualID
        || hub.top.editor.ptnetLoLA.diagram.edit.parts.PlaceEditPart.VISUAL_ID == visualID;
  }

  /**
   * @generated
   */
  protected boolean provides(CreateEdgeViewOperation op) {
    IElementType elementType = getSemanticElementType(op.getSemanticAdapter());
    if (!hub.top.editor.ptnetLoLA.diagram.providers.PtnetLoLAElementTypes
        .isKnownElementType(elementType)
        || (!(elementType instanceof IHintedType))) {
      return false; // foreign element type
    }
    String elementTypeHint = ((IHintedType) elementType).getSemanticHint();
    if (elementTypeHint == null
        || (op.getSemanticHint() != null && !elementTypeHint.equals(op
            .getSemanticHint()))) {
      return false; // our hint is visual id and must be specified, and it should be the same as in element type
    }
    int visualID = hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLAVisualIDRegistry
        .getVisualID(elementTypeHint);
    EObject domainElement = getSemanticElement(op.getSemanticAdapter());
    if (domainElement != null
        && visualID != hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLAVisualIDRegistry
            .getLinkWithClassVisualID(domainElement)) {
      return false; // visual id for link EClass should match visual id from element type
    }
    return true;
  }

  /**
   * @generated
   */
  public Diagram createDiagram(IAdaptable semanticAdapter, String diagramKind,
      PreferencesHint preferencesHint) {
    Diagram diagram = NotationFactory.eINSTANCE.createDiagram();
    diagram.getStyles().add(NotationFactory.eINSTANCE.createDiagramStyle());
    diagram
        .setType(hub.top.editor.ptnetLoLA.diagram.edit.parts.PtNetEditPart.MODEL_ID);
    diagram.setElement(getSemanticElement(semanticAdapter));
    diagram.setMeasurementUnit(MeasurementUnit.PIXEL_LITERAL);
    return diagram;
  }

  /**
   * @generated
   */
  public Node createNode(IAdaptable semanticAdapter, View containerView,
      String semanticHint, int index, boolean persisted,
      PreferencesHint preferencesHint) {
    final EObject domainElement = getSemanticElement(semanticAdapter);
    final int visualID;
    if (semanticHint == null) {
      visualID = hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLAVisualIDRegistry
          .getNodeVisualID(containerView, domainElement);
    } else {
      visualID = hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLAVisualIDRegistry
          .getVisualID(semanticHint);
    }
    switch (visualID) {
    case hub.top.editor.ptnetLoLA.diagram.edit.parts.TransitionEditPart.VISUAL_ID:
      return createTransition_2001(domainElement, containerView, index,
          persisted, preferencesHint);
    case hub.top.editor.ptnetLoLA.diagram.edit.parts.PlaceEditPart.VISUAL_ID:
      return createPlace_2002(domainElement, containerView, index, persisted,
          preferencesHint);
    }
    // can't happen, provided #provides(CreateNodeViewOperation) is correct
    return null;
  }

  /**
   * @generated
   */
  public Edge createEdge(IAdaptable semanticAdapter, View containerView,
      String semanticHint, int index, boolean persisted,
      PreferencesHint preferencesHint) {
    IElementType elementType = getSemanticElementType(semanticAdapter);
    String elementTypeHint = ((IHintedType) elementType).getSemanticHint();
    switch (hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLAVisualIDRegistry
        .getVisualID(elementTypeHint)) {
    case hub.top.editor.ptnetLoLA.diagram.edit.parts.ArcToPlaceEditPart.VISUAL_ID:
      return createArcToPlace_4001(getSemanticElement(semanticAdapter),
          containerView, index, persisted, preferencesHint);
    case hub.top.editor.ptnetLoLA.diagram.edit.parts.ArcToTransitionEditPart.VISUAL_ID:
      return createArcToTransition_4002(getSemanticElement(semanticAdapter),
          containerView, index, persisted, preferencesHint);
    }
    // can never happen, provided #provides(CreateEdgeViewOperation) is correct
    return null;
  }

  /**
   * @generated
   */
  public Node createTransition_2001(EObject domainElement, View containerView,
      int index, boolean persisted, PreferencesHint preferencesHint) {
    Node node = NotationFactory.eINSTANCE.createNode();
    node.getStyles().add(NotationFactory.eINSTANCE.createDescriptionStyle());
    node.getStyles().add(NotationFactory.eINSTANCE.createFontStyle());
    node.setLayoutConstraint(NotationFactory.eINSTANCE.createBounds());
    node.setType(hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLAVisualIDRegistry
        .getType(hub.top.editor.ptnetLoLA.diagram.edit.parts.TransitionEditPart.VISUAL_ID));
    ViewUtil.insertChildView(containerView, node, index, persisted);
    node.setElement(domainElement);
    stampShortcut(containerView, node);
    // initializeFromPreferences 
    final IPreferenceStore prefStore = (IPreferenceStore) preferencesHint
        .getPreferenceStore();
    FontStyle nodeFontStyle = (FontStyle) node
        .getStyle(NotationPackage.Literals.FONT_STYLE);
    if (nodeFontStyle != null) {
      FontData fontData = PreferenceConverter.getFontData(prefStore,
          IPreferenceConstants.PREF_DEFAULT_FONT);
      nodeFontStyle.setFontName(fontData.getName());
      nodeFontStyle.setFontHeight(fontData.getHeight());
      nodeFontStyle.setBold((fontData.getStyle() & SWT.BOLD) != 0);
      nodeFontStyle.setItalic((fontData.getStyle() & SWT.ITALIC) != 0);
      org.eclipse.swt.graphics.RGB fontRGB = PreferenceConverter.getColor(
          prefStore, IPreferenceConstants.PREF_FONT_COLOR);
      nodeFontStyle.setFontColor(FigureUtilities.RGBToInteger(fontRGB)
          .intValue());
    }
    Node label5001 = createLabel(
        node,
        hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLAVisualIDRegistry
            .getType(hub.top.editor.ptnetLoLA.diagram.edit.parts.TransitionNameEditPart.VISUAL_ID));
    label5001.setLayoutConstraint(NotationFactory.eINSTANCE.createLocation());
    Location location5001 = (Location) label5001.getLayoutConstraint();
    location5001.setX(0);
    location5001.setY(5);
    return node;
  }

  /**
   * @generated
   */
  public Node createPlace_2002(EObject domainElement, View containerView,
      int index, boolean persisted, PreferencesHint preferencesHint) {
    Node node = NotationFactory.eINSTANCE.createNode();
    node.getStyles().add(NotationFactory.eINSTANCE.createDescriptionStyle());
    node.getStyles().add(NotationFactory.eINSTANCE.createFontStyle());
    node.setLayoutConstraint(NotationFactory.eINSTANCE.createBounds());
    node.setType(hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLAVisualIDRegistry
        .getType(hub.top.editor.ptnetLoLA.diagram.edit.parts.PlaceEditPart.VISUAL_ID));
    ViewUtil.insertChildView(containerView, node, index, persisted);
    node.setElement(domainElement);
    stampShortcut(containerView, node);
    // initializeFromPreferences 
    final IPreferenceStore prefStore = (IPreferenceStore) preferencesHint
        .getPreferenceStore();
    FontStyle nodeFontStyle = (FontStyle) node
        .getStyle(NotationPackage.Literals.FONT_STYLE);
    if (nodeFontStyle != null) {
      FontData fontData = PreferenceConverter.getFontData(prefStore,
          IPreferenceConstants.PREF_DEFAULT_FONT);
      nodeFontStyle.setFontName(fontData.getName());
      nodeFontStyle.setFontHeight(fontData.getHeight());
      nodeFontStyle.setBold((fontData.getStyle() & SWT.BOLD) != 0);
      nodeFontStyle.setItalic((fontData.getStyle() & SWT.ITALIC) != 0);
      org.eclipse.swt.graphics.RGB fontRGB = PreferenceConverter.getColor(
          prefStore, IPreferenceConstants.PREF_FONT_COLOR);
      nodeFontStyle.setFontColor(FigureUtilities.RGBToInteger(fontRGB)
          .intValue());
    }
    Node label5002 = createLabel(
        node,
        hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLAVisualIDRegistry
            .getType(hub.top.editor.ptnetLoLA.diagram.edit.parts.PlaceNameEditPart.VISUAL_ID));
    label5002.setLayoutConstraint(NotationFactory.eINSTANCE.createLocation());
    Location location5002 = (Location) label5002.getLayoutConstraint();
    location5002.setX(0);
    location5002.setY(5);
    Node label5003 = createLabel(
        node,
        hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLAVisualIDRegistry
            .getType(hub.top.editor.ptnetLoLA.diagram.edit.parts.PlaceTokenEditPart.VISUAL_ID));
    return node;
  }

  /**
   * @generated
   */
  public Edge createArcToPlace_4001(EObject domainElement, View containerView,
      int index, boolean persisted, PreferencesHint preferencesHint) {
    Edge edge = NotationFactory.eINSTANCE.createEdge();
    edge.getStyles().add(NotationFactory.eINSTANCE.createRoutingStyle());
    edge.getStyles().add(NotationFactory.eINSTANCE.createFontStyle());
    RelativeBendpoints bendpoints = NotationFactory.eINSTANCE
        .createRelativeBendpoints();
    ArrayList<RelativeBendpoint> points = new ArrayList<RelativeBendpoint>(2);
    points.add(new RelativeBendpoint());
    points.add(new RelativeBendpoint());
    bendpoints.setPoints(points);
    edge.setBendpoints(bendpoints);
    ViewUtil.insertChildView(containerView, edge, index, persisted);
    edge.setType(hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLAVisualIDRegistry
        .getType(hub.top.editor.ptnetLoLA.diagram.edit.parts.ArcToPlaceEditPart.VISUAL_ID));
    edge.setElement(domainElement);
    // initializePreferences
    final IPreferenceStore prefStore = (IPreferenceStore) preferencesHint
        .getPreferenceStore();
    FontStyle edgeFontStyle = (FontStyle) edge
        .getStyle(NotationPackage.Literals.FONT_STYLE);
    if (edgeFontStyle != null) {
      FontData fontData = PreferenceConverter.getFontData(prefStore,
          IPreferenceConstants.PREF_DEFAULT_FONT);
      edgeFontStyle.setFontName(fontData.getName());
      edgeFontStyle.setFontHeight(fontData.getHeight());
      edgeFontStyle.setBold((fontData.getStyle() & SWT.BOLD) != 0);
      edgeFontStyle.setItalic((fontData.getStyle() & SWT.ITALIC) != 0);
      org.eclipse.swt.graphics.RGB fontRGB = PreferenceConverter.getColor(
          prefStore, IPreferenceConstants.PREF_FONT_COLOR);
      edgeFontStyle.setFontColor(FigureUtilities.RGBToInteger(fontRGB)
          .intValue());
    }
    Routing routing = Routing.get(prefStore
        .getInt(IPreferenceConstants.PREF_LINE_STYLE));
    if (routing != null) {
      ViewUtil.setStructuralFeatureValue(edge,
          NotationPackage.eINSTANCE.getRoutingStyle_Routing(), routing);
    }
    Node label6001 = createLabel(
        edge,
        hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLAVisualIDRegistry
            .getType(hub.top.editor.ptnetLoLA.diagram.edit.parts.ArcToPlaceWeightEditPart.VISUAL_ID));
    label6001.setLayoutConstraint(NotationFactory.eINSTANCE.createLocation());
    Location location6001 = (Location) label6001.getLayoutConstraint();
    location6001.setX(5);
    location6001.setY(5);
    return edge;
  }

  /**
   * @generated
   */
  public Edge createArcToTransition_4002(EObject domainElement,
      View containerView, int index, boolean persisted,
      PreferencesHint preferencesHint) {
    Edge edge = NotationFactory.eINSTANCE.createEdge();
    edge.getStyles().add(NotationFactory.eINSTANCE.createRoutingStyle());
    edge.getStyles().add(NotationFactory.eINSTANCE.createFontStyle());
    RelativeBendpoints bendpoints = NotationFactory.eINSTANCE
        .createRelativeBendpoints();
    ArrayList<RelativeBendpoint> points = new ArrayList<RelativeBendpoint>(2);
    points.add(new RelativeBendpoint());
    points.add(new RelativeBendpoint());
    bendpoints.setPoints(points);
    edge.setBendpoints(bendpoints);
    ViewUtil.insertChildView(containerView, edge, index, persisted);
    edge.setType(hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLAVisualIDRegistry
        .getType(hub.top.editor.ptnetLoLA.diagram.edit.parts.ArcToTransitionEditPart.VISUAL_ID));
    edge.setElement(domainElement);
    // initializePreferences
    final IPreferenceStore prefStore = (IPreferenceStore) preferencesHint
        .getPreferenceStore();
    FontStyle edgeFontStyle = (FontStyle) edge
        .getStyle(NotationPackage.Literals.FONT_STYLE);
    if (edgeFontStyle != null) {
      FontData fontData = PreferenceConverter.getFontData(prefStore,
          IPreferenceConstants.PREF_DEFAULT_FONT);
      edgeFontStyle.setFontName(fontData.getName());
      edgeFontStyle.setFontHeight(fontData.getHeight());
      edgeFontStyle.setBold((fontData.getStyle() & SWT.BOLD) != 0);
      edgeFontStyle.setItalic((fontData.getStyle() & SWT.ITALIC) != 0);
      org.eclipse.swt.graphics.RGB fontRGB = PreferenceConverter.getColor(
          prefStore, IPreferenceConstants.PREF_FONT_COLOR);
      edgeFontStyle.setFontColor(FigureUtilities.RGBToInteger(fontRGB)
          .intValue());
    }
    Routing routing = Routing.get(prefStore
        .getInt(IPreferenceConstants.PREF_LINE_STYLE));
    if (routing != null) {
      ViewUtil.setStructuralFeatureValue(edge,
          NotationPackage.eINSTANCE.getRoutingStyle_Routing(), routing);
    }
    Node label6002 = createLabel(
        edge,
        hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLAVisualIDRegistry
            .getType(hub.top.editor.ptnetLoLA.diagram.edit.parts.ArcToTransitionWeightEditPart.VISUAL_ID));
    label6002.setLayoutConstraint(NotationFactory.eINSTANCE.createLocation());
    Location location6002 = (Location) label6002.getLayoutConstraint();
    location6002.setX(5);
    location6002.setY(5);
    return edge;
  }

  /**
   * @generated
   */
  private void stampShortcut(View containerView, Node target) {
    if (!hub.top.editor.ptnetLoLA.diagram.edit.parts.PtNetEditPart.MODEL_ID
        .equals(hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLAVisualIDRegistry
            .getModelID(containerView))) {
      EAnnotation shortcutAnnotation = EcoreFactory.eINSTANCE
          .createEAnnotation();
      shortcutAnnotation.setSource("Shortcut"); //$NON-NLS-1$
      shortcutAnnotation
          .getDetails()
          .put(
              "modelID", hub.top.editor.ptnetLoLA.diagram.edit.parts.PtNetEditPart.MODEL_ID); //$NON-NLS-1$
      target.getEAnnotations().add(shortcutAnnotation);
    }
  }

  /**
   * @generated
   */
  private Node createLabel(View owner, String hint) {
    DecorationNode rv = NotationFactory.eINSTANCE.createDecorationNode();
    rv.setType(hint);
    ViewUtil.insertChildView(owner, rv, ViewUtil.APPEND, true);
    return rv;
  }

  /**
   * @generated
   */
  private EObject getSemanticElement(IAdaptable semanticAdapter) {
    if (semanticAdapter == null) {
      return null;
    }
    EObject eObject = (EObject) semanticAdapter.getAdapter(EObject.class);
    if (eObject != null) {
      return EMFCoreUtil.resolve(TransactionUtil.getEditingDomain(eObject),
          eObject);
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
