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
package hub.top.lang.flowcharts.diagram.providers;

import hub.top.lang.flowcharts.FlowPackage;
import hub.top.lang.flowcharts.diagram.part.FlowchartDiagramEditorPlugin;

import java.util.HashSet;
import java.util.IdentityHashMap;
import java.util.Map;
import java.util.Set;

import org.eclipse.core.runtime.IAdaptable;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.EClassifier;
import org.eclipse.emf.ecore.ENamedElement;
import org.eclipse.emf.ecore.EStructuralFeature;
import org.eclipse.gmf.runtime.emf.type.core.ElementTypeRegistry;
import org.eclipse.gmf.runtime.emf.type.core.IElementType;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.jface.resource.ImageRegistry;
import org.eclipse.swt.graphics.Image;

/**
 * @generated
 */
public class FlowchartElementTypes extends ElementInitializers {

  /**
   * @generated
   */
  private FlowchartElementTypes() {
  }

  /**
   * @generated
   */
  private static Map elements;

  /**
   * @generated
   */
  private static ImageRegistry imageRegistry;

  /**
   * @generated
   */
  private static Set KNOWN_ELEMENT_TYPES;

  /**
   * @generated
   */
  public static final IElementType WorkflowDiagram_1000 = getElementType("hub.top.lang.flowcharts.diagram.WorkflowDiagram_1000"); //$NON-NLS-1$

  /**
   * @generated
   */
  public static final IElementType StartNode_2001 = getElementType("hub.top.lang.flowcharts.diagram.StartNode_2001"); //$NON-NLS-1$
  /**
   * @generated
   */
  public static final IElementType Endnode_2002 = getElementType("hub.top.lang.flowcharts.diagram.Endnode_2002"); //$NON-NLS-1$
  /**
   * @generated
   */
  public static final IElementType SimpleActivity_2003 = getElementType("hub.top.lang.flowcharts.diagram.SimpleActivity_2003"); //$NON-NLS-1$
  /**
   * @generated
   */
  public static final IElementType Subprocess_2004 = getElementType("hub.top.lang.flowcharts.diagram.Subprocess_2004"); //$NON-NLS-1$
  /**
   * @generated
   */
  public static final IElementType Event_2005 = getElementType("hub.top.lang.flowcharts.diagram.Event_2005"); //$NON-NLS-1$
  /**
   * @generated
   */
  public static final IElementType SplitNode_2006 = getElementType("hub.top.lang.flowcharts.diagram.SplitNode_2006"); //$NON-NLS-1$
  /**
   * @generated
   */
  public static final IElementType MergeNode_2007 = getElementType("hub.top.lang.flowcharts.diagram.MergeNode_2007"); //$NON-NLS-1$
  /**
   * @generated
   */
  public static final IElementType Flow_2008 = getElementType("hub.top.lang.flowcharts.diagram.Flow_2008"); //$NON-NLS-1$
  /**
   * @generated
   */
  public static final IElementType TransientResource_2009 = getElementType("hub.top.lang.flowcharts.diagram.TransientResource_2009"); //$NON-NLS-1$
  /**
   * @generated
   */
  public static final IElementType Document_2010 = getElementType("hub.top.lang.flowcharts.diagram.Document_2010"); //$NON-NLS-1$
  /**
   * @generated
   */
  public static final IElementType PersistentResource_2011 = getElementType("hub.top.lang.flowcharts.diagram.PersistentResource_2011"); //$NON-NLS-1$
  /**
   * @generated
   */
  public static final IElementType StartNode_3001 = getElementType("hub.top.lang.flowcharts.diagram.StartNode_3001"); //$NON-NLS-1$

  /**
   * @generated
   */
  public static final IElementType Endnode_3002 = getElementType("hub.top.lang.flowcharts.diagram.Endnode_3002"); //$NON-NLS-1$

  /**
   * @generated
   */
  public static final IElementType SimpleActivity_3003 = getElementType("hub.top.lang.flowcharts.diagram.SimpleActivity_3003"); //$NON-NLS-1$

  /**
   * @generated
   */
  public static final IElementType Subprocess_3004 = getElementType("hub.top.lang.flowcharts.diagram.Subprocess_3004"); //$NON-NLS-1$

  /**
   * @generated
   */
  public static final IElementType Event_3005 = getElementType("hub.top.lang.flowcharts.diagram.Event_3005"); //$NON-NLS-1$

  /**
   * @generated
   */
  public static final IElementType SplitNode_3006 = getElementType("hub.top.lang.flowcharts.diagram.SplitNode_3006"); //$NON-NLS-1$

  /**
   * @generated
   */
  public static final IElementType MergeNode_3007 = getElementType("hub.top.lang.flowcharts.diagram.MergeNode_3007"); //$NON-NLS-1$

  /**
   * @generated
   */
  public static final IElementType Flow_3008 = getElementType("hub.top.lang.flowcharts.diagram.Flow_3008"); //$NON-NLS-1$

  /**
   * @generated
   */
  public static final IElementType TransientResource_3009 = getElementType("hub.top.lang.flowcharts.diagram.TransientResource_3009"); //$NON-NLS-1$

  /**
   * @generated
   */
  public static final IElementType Document_3010 = getElementType("hub.top.lang.flowcharts.diagram.Document_3010"); //$NON-NLS-1$

  /**
   * @generated
   */
  public static final IElementType PersistentResource_3011 = getElementType("hub.top.lang.flowcharts.diagram.PersistentResource_3011"); //$NON-NLS-1$

  /**
   * @generated
   */
  public static final IElementType DiagramArc_4001 = getElementType("hub.top.lang.flowcharts.diagram.DiagramArc_4001"); //$NON-NLS-1$

  /**
   * @generated
   */
  private static ImageRegistry getImageRegistry() {
    if (imageRegistry == null) {
      imageRegistry = new ImageRegistry();
    }
    return imageRegistry;
  }

  /**
   * @generated
   */
  private static String getImageRegistryKey(ENamedElement element) {
    return element.getName();
  }

  /**
   * @generated
   */
  private static ImageDescriptor getProvidedImageDescriptor(
      ENamedElement element) {
    if (element instanceof EStructuralFeature) {
      EStructuralFeature feature = ((EStructuralFeature) element);
      EClass eContainingClass = feature.getEContainingClass();
      EClassifier eType = feature.getEType();
      if (eContainingClass != null && !eContainingClass.isAbstract()) {
        element = eContainingClass;
      } else if (eType instanceof EClass && !((EClass) eType).isAbstract()) {
        element = eType;
      }
    }
    if (element instanceof EClass) {
      EClass eClass = (EClass) element;
      if (!eClass.isAbstract()) {
        return FlowchartDiagramEditorPlugin.getInstance()
            .getItemImageDescriptor(
                eClass.getEPackage().getEFactoryInstance().create(eClass));
      }
    }
    // TODO : support structural features
    return null;
  }

  /**
   * @generated
   */
  public static ImageDescriptor getImageDescriptor(ENamedElement element) {
    String key = getImageRegistryKey(element);
    ImageDescriptor imageDescriptor = getImageRegistry().getDescriptor(key);
    if (imageDescriptor == null) {
      imageDescriptor = getProvidedImageDescriptor(element);
      if (imageDescriptor == null) {
        imageDescriptor = ImageDescriptor.getMissingImageDescriptor();
      }
      getImageRegistry().put(key, imageDescriptor);
    }
    return imageDescriptor;
  }

  /**
   * @generated
   */
  public static Image getImage(ENamedElement element) {
    String key = getImageRegistryKey(element);
    Image image = getImageRegistry().get(key);
    if (image == null) {
      ImageDescriptor imageDescriptor = getProvidedImageDescriptor(element);
      if (imageDescriptor == null) {
        imageDescriptor = ImageDescriptor.getMissingImageDescriptor();
      }
      getImageRegistry().put(key, imageDescriptor);
      image = getImageRegistry().get(key);
    }
    return image;
  }

  /**
   * @generated
   */
  public static ImageDescriptor getImageDescriptor(IAdaptable hint) {
    ENamedElement element = getElement(hint);
    if (element == null) {
      return null;
    }
    return getImageDescriptor(element);
  }

  /**
   * @generated
   */
  public static Image getImage(IAdaptable hint) {
    ENamedElement element = getElement(hint);
    if (element == null) {
      return null;
    }
    return getImage(element);
  }

  /**
   * Returns 'type' of the ecore object associated with the hint.
   * 
   * @generated
   */
  public static ENamedElement getElement(IAdaptable hint) {
    Object type = hint.getAdapter(IElementType.class);
    if (elements == null) {
      elements = new IdentityHashMap();

      elements.put(WorkflowDiagram_1000, FlowPackage.eINSTANCE
          .getWorkflowDiagram());

      elements.put(StartNode_2001, FlowPackage.eINSTANCE.getStartNode());

      elements.put(Endnode_2002, FlowPackage.eINSTANCE.getEndnode());

      elements.put(SimpleActivity_2003, FlowPackage.eINSTANCE
          .getSimpleActivity());

      elements.put(Subprocess_2004, FlowPackage.eINSTANCE.getSubprocess());

      elements.put(Event_2005, FlowPackage.eINSTANCE.getEvent());

      elements.put(SplitNode_2006, FlowPackage.eINSTANCE.getSplitNode());

      elements.put(MergeNode_2007, FlowPackage.eINSTANCE.getMergeNode());

      elements.put(Flow_2008, FlowPackage.eINSTANCE.getFlow());

      elements.put(TransientResource_2009, FlowPackage.eINSTANCE
          .getTransientResource());

      elements.put(Document_2010, FlowPackage.eINSTANCE.getDocument());

      elements.put(PersistentResource_2011, FlowPackage.eINSTANCE
          .getPersistentResource());

      elements.put(StartNode_3001, FlowPackage.eINSTANCE.getStartNode());

      elements.put(Endnode_3002, FlowPackage.eINSTANCE.getEndnode());

      elements.put(SimpleActivity_3003, FlowPackage.eINSTANCE
          .getSimpleActivity());

      elements.put(Subprocess_3004, FlowPackage.eINSTANCE.getSubprocess());

      elements.put(Event_3005, FlowPackage.eINSTANCE.getEvent());

      elements.put(SplitNode_3006, FlowPackage.eINSTANCE.getSplitNode());

      elements.put(MergeNode_3007, FlowPackage.eINSTANCE.getMergeNode());

      elements.put(Flow_3008, FlowPackage.eINSTANCE.getFlow());

      elements.put(TransientResource_3009, FlowPackage.eINSTANCE
          .getTransientResource());

      elements.put(Document_3010, FlowPackage.eINSTANCE.getDocument());

      elements.put(PersistentResource_3011, FlowPackage.eINSTANCE
          .getPersistentResource());

      elements.put(DiagramArc_4001, FlowPackage.eINSTANCE.getDiagramArc());
    }
    return (ENamedElement) elements.get(type);
  }

  /**
   * @generated
   */
  private static IElementType getElementType(String id) {
    return ElementTypeRegistry.getInstance().getType(id);
  }

  /**
   * @generated
   */
  public static boolean isKnownElementType(IElementType elementType) {
    if (KNOWN_ELEMENT_TYPES == null) {
      KNOWN_ELEMENT_TYPES = new HashSet();
      KNOWN_ELEMENT_TYPES.add(WorkflowDiagram_1000);
      KNOWN_ELEMENT_TYPES.add(StartNode_2001);
      KNOWN_ELEMENT_TYPES.add(Endnode_2002);
      KNOWN_ELEMENT_TYPES.add(SimpleActivity_2003);
      KNOWN_ELEMENT_TYPES.add(Subprocess_2004);
      KNOWN_ELEMENT_TYPES.add(Event_2005);
      KNOWN_ELEMENT_TYPES.add(SplitNode_2006);
      KNOWN_ELEMENT_TYPES.add(MergeNode_2007);
      KNOWN_ELEMENT_TYPES.add(Flow_2008);
      KNOWN_ELEMENT_TYPES.add(TransientResource_2009);
      KNOWN_ELEMENT_TYPES.add(Document_2010);
      KNOWN_ELEMENT_TYPES.add(PersistentResource_2011);
      KNOWN_ELEMENT_TYPES.add(StartNode_3001);
      KNOWN_ELEMENT_TYPES.add(Endnode_3002);
      KNOWN_ELEMENT_TYPES.add(SimpleActivity_3003);
      KNOWN_ELEMENT_TYPES.add(Subprocess_3004);
      KNOWN_ELEMENT_TYPES.add(Event_3005);
      KNOWN_ELEMENT_TYPES.add(SplitNode_3006);
      KNOWN_ELEMENT_TYPES.add(MergeNode_3007);
      KNOWN_ELEMENT_TYPES.add(Flow_3008);
      KNOWN_ELEMENT_TYPES.add(TransientResource_3009);
      KNOWN_ELEMENT_TYPES.add(Document_3010);
      KNOWN_ELEMENT_TYPES.add(PersistentResource_3011);
      KNOWN_ELEMENT_TYPES.add(DiagramArc_4001);
    }
    return KNOWN_ELEMENT_TYPES.contains(elementType);
  }

}
