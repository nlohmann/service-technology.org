package hub.top.adaptiveSystem.diagram.providers;

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
public class AdaptiveSystemElementTypes extends ElementInitializers {

	/**
	 * @generated
	 */
	private AdaptiveSystemElementTypes() {
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
	public static final IElementType AdaptiveSystem_79 = getElementType("hub.top.GRETA.diagram.AdaptiveSystem_79"); //$NON-NLS-1$
	/**
	 * @generated
	 */
	public static final IElementType AdaptiveProcess_1001 = getElementType("hub.top.GRETA.diagram.AdaptiveProcess_1001"); //$NON-NLS-1$
	/**
	 * @generated
	 */
	public static final IElementType Oclet_1002 = getElementType("hub.top.GRETA.diagram.Oclet_1002"); //$NON-NLS-1$
	/**
	 * @generated
	 */
	public static final IElementType Condition_2001 = getElementType("hub.top.GRETA.diagram.Condition_2001"); //$NON-NLS-1$
	/**
	 * @generated
	 */
	public static final IElementType Event_2002 = getElementType("hub.top.GRETA.diagram.Event_2002"); //$NON-NLS-1$
	/**
	 * @generated
	 */
	public static final IElementType PreNet_2003 = getElementType("hub.top.GRETA.diagram.PreNet_2003"); //$NON-NLS-1$
	/**
	 * @generated
	 */
	public static final IElementType Condition_2004 = getElementType("hub.top.GRETA.diagram.Condition_2004"); //$NON-NLS-1$
	/**
	 * @generated
	 */
	public static final IElementType Event_2005 = getElementType("hub.top.GRETA.diagram.Event_2005"); //$NON-NLS-1$
	/**
	 * @generated
	 */
	public static final IElementType DoNet_2006 = getElementType("hub.top.GRETA.diagram.DoNet_2006"); //$NON-NLS-1$
	/**
	 * @generated
	 */
	public static final IElementType Condition_2007 = getElementType("hub.top.GRETA.diagram.Condition_2007"); //$NON-NLS-1$
	/**
	 * @generated
	 */
	public static final IElementType Event_2008 = getElementType("hub.top.GRETA.diagram.Event_2008"); //$NON-NLS-1$
	/**
	 * @generated
	 */
	public static final IElementType ArcToCondition_3001 = getElementType("hub.top.GRETA.diagram.ArcToCondition_3001"); //$NON-NLS-1$
	/**
	 * @generated
	 */
	public static final IElementType ArcToEvent_3002 = getElementType("hub.top.GRETA.diagram.ArcToEvent_3002"); //$NON-NLS-1$

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
			} else if (eType instanceof EClass
					&& !((EClass) eType).isAbstract()) {
				element = eType;
			}
		}
		if (element instanceof EClass) {
			EClass eClass = (EClass) element;
			if (!eClass.isAbstract()) {
				return hub.top.adaptiveSystem.diagram.part.AdaptiveSystemDiagramEditorPlugin
						.getInstance().getItemImageDescriptor(
								eClass.getEPackage().getEFactoryInstance()
										.create(eClass));
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

			elements.put(AdaptiveSystem_79,
					hub.top.adaptiveSystem.AdaptiveSystemPackage.eINSTANCE
							.getAdaptiveSystem());

			elements.put(AdaptiveProcess_1001,
					hub.top.adaptiveSystem.AdaptiveSystemPackage.eINSTANCE
							.getAdaptiveProcess());

			elements.put(Oclet_1002,
					hub.top.adaptiveSystem.AdaptiveSystemPackage.eINSTANCE
							.getOclet());

			elements.put(Condition_2001,
					hub.top.adaptiveSystem.AdaptiveSystemPackage.eINSTANCE
							.getCondition());

			elements.put(Event_2002,
					hub.top.adaptiveSystem.AdaptiveSystemPackage.eINSTANCE
							.getEvent());

			elements.put(PreNet_2003,
					hub.top.adaptiveSystem.AdaptiveSystemPackage.eINSTANCE
							.getPreNet());

			elements.put(Condition_2004,
					hub.top.adaptiveSystem.AdaptiveSystemPackage.eINSTANCE
							.getCondition());

			elements.put(Event_2005,
					hub.top.adaptiveSystem.AdaptiveSystemPackage.eINSTANCE
							.getEvent());

			elements.put(DoNet_2006,
					hub.top.adaptiveSystem.AdaptiveSystemPackage.eINSTANCE
							.getDoNet());

			elements.put(Condition_2007,
					hub.top.adaptiveSystem.AdaptiveSystemPackage.eINSTANCE
							.getCondition());

			elements.put(Event_2008,
					hub.top.adaptiveSystem.AdaptiveSystemPackage.eINSTANCE
							.getEvent());

			elements.put(ArcToCondition_3001,
					hub.top.adaptiveSystem.AdaptiveSystemPackage.eINSTANCE
							.getArcToCondition());

			elements.put(ArcToEvent_3002,
					hub.top.adaptiveSystem.AdaptiveSystemPackage.eINSTANCE
							.getArcToEvent());
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
			KNOWN_ELEMENT_TYPES.add(AdaptiveSystem_79);
			KNOWN_ELEMENT_TYPES.add(AdaptiveProcess_1001);
			KNOWN_ELEMENT_TYPES.add(Oclet_1002);
			KNOWN_ELEMENT_TYPES.add(Condition_2001);
			KNOWN_ELEMENT_TYPES.add(Event_2002);
			KNOWN_ELEMENT_TYPES.add(PreNet_2003);
			KNOWN_ELEMENT_TYPES.add(Condition_2004);
			KNOWN_ELEMENT_TYPES.add(Event_2005);
			KNOWN_ELEMENT_TYPES.add(DoNet_2006);
			KNOWN_ELEMENT_TYPES.add(Condition_2007);
			KNOWN_ELEMENT_TYPES.add(Event_2008);
			KNOWN_ELEMENT_TYPES.add(ArcToCondition_3001);
			KNOWN_ELEMENT_TYPES.add(ArcToEvent_3002);
		}
		return KNOWN_ELEMENT_TYPES.contains(elementType);
	}

}
