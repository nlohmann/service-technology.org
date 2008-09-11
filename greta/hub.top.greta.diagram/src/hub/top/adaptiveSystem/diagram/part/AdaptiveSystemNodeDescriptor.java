package hub.top.adaptiveSystem.diagram.part;

import org.eclipse.emf.ecore.EObject;

/**
 * @generated
 */
public class AdaptiveSystemNodeDescriptor {

	/**
	 * @generated
	 */
	private EObject myModelElement;

	/**
	 * @generated
	 */
	private int myVisualID;

	/**
	 * @generated
	 */
	private String myType;

	/**
	 * @generated
	 */
	public AdaptiveSystemNodeDescriptor(EObject modelElement, int visualID) {
		myModelElement = modelElement;
		myVisualID = visualID;
	}

	/**
	 * @generated
	 */
	public EObject getModelElement() {
		return myModelElement;
	}

	/**
	 * @generated
	 */
	public int getVisualID() {
		return myVisualID;
	}

	/**
	 * @generated
	 */
	public String getType() {
		if (myType == null) {
			myType = hub.top.adaptiveSystem.diagram.part.AdaptiveSystemVisualIDRegistry
					.getType(getVisualID());
		}
		return myType;
	}

}
