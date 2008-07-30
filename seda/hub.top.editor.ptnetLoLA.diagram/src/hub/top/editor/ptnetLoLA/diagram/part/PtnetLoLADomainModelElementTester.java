package hub.top.editor.ptnetLoLA.diagram.part;

import org.eclipse.core.expressions.PropertyTester;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.EObject;

/**
 * @generated
 */
public class PtnetLoLADomainModelElementTester extends PropertyTester {

	/**
	 * @generated
	 */
	public boolean test(Object receiver, String method, Object[] args,
			Object expectedValue) {
		if (false == receiver instanceof EObject) {
			return false;
		}
		EObject eObject = (EObject) receiver;
		EClass eClass = eObject.eClass();
		if (eClass == hub.top.editor.ptnetLoLA.PtnetLoLAPackage.eINSTANCE
				.getPlace()) {
			return true;
		}
		if (eClass == hub.top.editor.ptnetLoLA.PtnetLoLAPackage.eINSTANCE
				.getPtNet()) {
			return true;
		}
		if (eClass == hub.top.editor.ptnetLoLA.PtnetLoLAPackage.eINSTANCE
				.getTransition()) {
			return true;
		}
		if (eClass == hub.top.editor.ptnetLoLA.PtnetLoLAPackage.eINSTANCE
				.getNode()) {
			return true;
		}
		if (eClass == hub.top.editor.ptnetLoLA.PtnetLoLAPackage.eINSTANCE
				.getMarking()) {
			return true;
		}
		if (eClass == hub.top.editor.ptnetLoLA.PtnetLoLAPackage.eINSTANCE
				.getPlaceReference()) {
			return true;
		}
		if (eClass == hub.top.editor.ptnetLoLA.PtnetLoLAPackage.eINSTANCE
				.getRefMarkedPlace()) {
			return true;
		}
		if (eClass == hub.top.editor.ptnetLoLA.PtnetLoLAPackage.eINSTANCE
				.getAnnotation()) {
			return true;
		}
		if (eClass == hub.top.editor.ptnetLoLA.PtnetLoLAPackage.eINSTANCE
				.getArc()) {
			return true;
		}
		if (eClass == hub.top.editor.ptnetLoLA.PtnetLoLAPackage.eINSTANCE
				.getArcToPlace()) {
			return true;
		}
		if (eClass == hub.top.editor.ptnetLoLA.PtnetLoLAPackage.eINSTANCE
				.getArcToTransition()) {
			return true;
		}
		return false;
	}

}
