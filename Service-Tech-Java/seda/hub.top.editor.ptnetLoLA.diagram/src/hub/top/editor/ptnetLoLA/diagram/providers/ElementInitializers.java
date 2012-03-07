package hub.top.editor.ptnetLoLA.diagram.providers;

import java.util.ArrayList;
import java.util.Collection;
import java.util.Iterator;
import java.util.List;

import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.EStructuralFeature;

/**
 * @generated
 */
public class ElementInitializers {

  protected ElementInitializers() {
    // use #getInstance to access cached instance
  }

  /**
   * @generated
   */
  public static ElementInitializers getInstance() {
    ElementInitializers cached = hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLADiagramEditorPlugin
        .getInstance().getElementInitializers();
    if (cached == null) {
      hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLADiagramEditorPlugin
          .getInstance().setElementInitializers(
              cached = new ElementInitializers());
    }
    return cached;
  }
}
