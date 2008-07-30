package hub.top.editor.ptnetLoLA.diagram.providers;

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
public class PtnetLoLAElementTypes extends ElementInitializers {

	/**
	 * @generated
	 */
	private PtnetLoLAElementTypes() {
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
	public static final IElementType PtNet_79 = getElementType("hub.top.editor.ptnetLoLA.diagram.PtNet_79"); //$NON-NLS-1$
	/**
	 * @generated
	 */
	public static final IElementType Transition_1001 = getElementType("hub.top.editor.ptnetLoLA.diagram.Transition_1001"); //$NON-NLS-1$
	/**
	 * @generated
	 */
	public static final IElementType Place_1002 = getElementType("hub.top.editor.ptnetLoLA.diagram.Place_1002"); //$NON-NLS-1$
	/**
	 * @generated
	 */
	public static final IElementType ArcToPlace_3001 = getElementType("hub.top.editor.ptnetLoLA.diagram.ArcToPlace_3001"); //$NON-NLS-1$
	/**
	 * @generated
	 */
	public static final IElementType ArcToTransition_3002 = getElementType("hub.top.editor.ptnetLoLA.diagram.ArcToTransition_3002"); //$NON-NLS-1$

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
				return hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLADiagramEditorPlugin
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

			elements.put(PtNet_79,
					hub.top.editor.ptnetLoLA.PtnetLoLAPackage.eINSTANCE
							.getPtNet());

			elements.put(Transition_1001,
					hub.top.editor.ptnetLoLA.PtnetLoLAPackage.eINSTANCE
							.getTransition());

			elements.put(Place_1002,
					hub.top.editor.ptnetLoLA.PtnetLoLAPackage.eINSTANCE
							.getPlace());

			elements.put(ArcToPlace_3001,
					hub.top.editor.ptnetLoLA.PtnetLoLAPackage.eINSTANCE
							.getArcToPlace());

			elements.put(ArcToTransition_3002,
					hub.top.editor.ptnetLoLA.PtnetLoLAPackage.eINSTANCE
							.getArcToTransition());
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
			KNOWN_ELEMENT_TYPES.add(PtNet_79);
			KNOWN_ELEMENT_TYPES.add(Transition_1001);
			KNOWN_ELEMENT_TYPES.add(Place_1002);
			KNOWN_ELEMENT_TYPES.add(ArcToPlace_3001);
			KNOWN_ELEMENT_TYPES.add(ArcToTransition_3002);
		}
		return KNOWN_ELEMENT_TYPES.contains(elementType);
	}

}
