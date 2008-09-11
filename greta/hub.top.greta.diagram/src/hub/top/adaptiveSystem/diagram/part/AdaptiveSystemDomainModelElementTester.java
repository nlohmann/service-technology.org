package hub.top.adaptiveSystem.diagram.part;

import org.eclipse.core.expressions.PropertyTester;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.EObject;

/**
 * @generated
 */
public class AdaptiveSystemDomainModelElementTester extends PropertyTester {

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
		if (eClass == hub.top.adaptiveSystem.AdaptiveSystemPackage.eINSTANCE
				.getAdaptiveSystem()) {
			return true;
		}
		if (eClass == hub.top.adaptiveSystem.AdaptiveSystemPackage.eINSTANCE
				.getOclet()) {
			return true;
		}
		if (eClass == hub.top.adaptiveSystem.AdaptiveSystemPackage.eINSTANCE
				.getOccurrenceNet()) {
			return true;
		}
		if (eClass == hub.top.adaptiveSystem.AdaptiveSystemPackage.eINSTANCE
				.getDoNet()) {
			return true;
		}
		if (eClass == hub.top.adaptiveSystem.AdaptiveSystemPackage.eINSTANCE
				.getPreNet()) {
			return true;
		}
		if (eClass == hub.top.adaptiveSystem.AdaptiveSystemPackage.eINSTANCE
				.getAdaptiveProcess()) {
			return true;
		}
		if (eClass == hub.top.adaptiveSystem.AdaptiveSystemPackage.eINSTANCE
				.getNode()) {
			return true;
		}
		if (eClass == hub.top.adaptiveSystem.AdaptiveSystemPackage.eINSTANCE
				.getCondition()) {
			return true;
		}
		if (eClass == hub.top.adaptiveSystem.AdaptiveSystemPackage.eINSTANCE
				.getEvent()) {
			return true;
		}
		if (eClass == hub.top.adaptiveSystem.AdaptiveSystemPackage.eINSTANCE
				.getArc()) {
			return true;
		}
		if (eClass == hub.top.adaptiveSystem.AdaptiveSystemPackage.eINSTANCE
				.getArcToEvent()) {
			return true;
		}
		if (eClass == hub.top.adaptiveSystem.AdaptiveSystemPackage.eINSTANCE
				.getArcToCondition()) {
			return true;
		}
		return false;
	}

}
