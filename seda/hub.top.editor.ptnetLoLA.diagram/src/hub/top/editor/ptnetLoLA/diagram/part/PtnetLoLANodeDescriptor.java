package hub.top.editor.ptnetLoLA.diagram.part;

import org.eclipse.emf.ecore.EObject;

/**
 * @generated
 */
public class PtnetLoLANodeDescriptor {

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
	public PtnetLoLANodeDescriptor(EObject modelElement, int visualID) {
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
			myType = hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLAVisualIDRegistry
					.getType(getVisualID());
		}
		return myType;
	}

}
