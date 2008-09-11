/**
 * Humboldt Universität zu Berlin, Thoery of Programming
 *
 * $Id$
 */
package hub.top.adaptiveSystem;

import org.eclipse.emf.ecore.EFactory;

/**
 * <!-- begin-user-doc -->
 * The <b>Factory</b> for the model.
 * It provides a create method for each non-abstract class of the model.
 * <!-- end-user-doc -->
 * @see hub.top.adaptiveSystem.AdaptiveSystemPackage
 * @generated
 */
public interface AdaptiveSystemFactory extends EFactory {
	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	String copyright = "Humboldt Universit\u00e4t zu Berlin, Thoery of Programming";

	/**
	 * The singleton instance of the factory.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	AdaptiveSystemFactory eINSTANCE = hub.top.adaptiveSystem.impl.AdaptiveSystemFactoryImpl.init();

	/**
	 * Returns a new object of class '<em>Adaptive System</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>Adaptive System</em>'.
	 * @generated
	 */
	AdaptiveSystem createAdaptiveSystem();

	/**
	 * Returns a new object of class '<em>Oclet</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>Oclet</em>'.
	 * @generated
	 */
	Oclet createOclet();

	/**
	 * Returns a new object of class '<em>Do Net</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>Do Net</em>'.
	 * @generated
	 */
	DoNet createDoNet();

	/**
	 * Returns a new object of class '<em>Pre Net</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>Pre Net</em>'.
	 * @generated
	 */
	PreNet createPreNet();

	/**
	 * Returns a new object of class '<em>Adaptive Process</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>Adaptive Process</em>'.
	 * @generated
	 */
	AdaptiveProcess createAdaptiveProcess();

	/**
	 * Returns a new object of class '<em>Condition</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>Condition</em>'.
	 * @generated
	 */
	Condition createCondition();

	/**
	 * Returns a new object of class '<em>Event</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>Event</em>'.
	 * @generated
	 */
	Event createEvent();

	/**
	 * Returns a new object of class '<em>Arc</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>Arc</em>'.
	 * @generated
	 */
	Arc createArc();

	/**
	 * Returns a new object of class '<em>Arc To Event</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>Arc To Event</em>'.
	 * @generated
	 */
	ArcToEvent createArcToEvent();

	/**
	 * Returns a new object of class '<em>Arc To Condition</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>Arc To Condition</em>'.
	 * @generated
	 */
	ArcToCondition createArcToCondition();

	/**
	 * Returns the package supported by this factory.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the package supported by this factory.
	 * @generated
	 */
	AdaptiveSystemPackage getAdaptiveSystemPackage();

} //AdaptiveSystemFactory
