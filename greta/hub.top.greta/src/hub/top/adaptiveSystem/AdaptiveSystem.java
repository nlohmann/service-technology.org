/**
 * Humboldt Universität zu Berlin, Thoery of Programming
 *
 * $Id$
 */
package hub.top.adaptiveSystem;

import org.eclipse.emf.common.util.EList;

import org.eclipse.emf.ecore.EObject;

/**
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Adaptive System</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link hub.top.adaptiveSystem.AdaptiveSystem#getOclets <em>Oclets</em>}</li>
 *   <li>{@link hub.top.adaptiveSystem.AdaptiveSystem#getAdaptiveProcess <em>Adaptive Process</em>}</li>
 *   <li>{@link hub.top.adaptiveSystem.AdaptiveSystem#isSetWellformednessToOclets <em>Set Wellformedness To Oclets</em>}</li>
 * </ul>
 * </p>
 *
 * @see hub.top.adaptiveSystem.AdaptiveSystemPackage#getAdaptiveSystem()
 * @model
 * @generated
 */
public interface AdaptiveSystem extends EObject {
	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	String copyright = "Humboldt Universit\u00e4t zu Berlin, Thoery of Programming";

	/**
	 * Returns the value of the '<em><b>Oclets</b></em>' containment reference list.
	 * The list contents are of type {@link hub.top.adaptiveSystem.Oclet}.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Oclets</em>' containment reference list isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Oclets</em>' containment reference list.
	 * @see hub.top.adaptiveSystem.AdaptiveSystemPackage#getAdaptiveSystem_Oclets()
	 * @model containment="true"
	 * @generated
	 */
	EList<Oclet> getOclets();

	/**
	 * Returns the value of the '<em><b>Adaptive Process</b></em>' containment reference.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Adaptive Process</em>' containment reference isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Adaptive Process</em>' containment reference.
	 * @see #setAdaptiveProcess(AdaptiveProcess)
	 * @see hub.top.adaptiveSystem.AdaptiveSystemPackage#getAdaptiveSystem_AdaptiveProcess()
	 * @model containment="true" required="true"
	 * @generated
	 */
	AdaptiveProcess getAdaptiveProcess();

	/**
	 * Sets the value of the '{@link hub.top.adaptiveSystem.AdaptiveSystem#getAdaptiveProcess <em>Adaptive Process</em>}' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Adaptive Process</em>' containment reference.
	 * @see #getAdaptiveProcess()
	 * @generated
	 */
	void setAdaptiveProcess(AdaptiveProcess value);

	/**
	 * Returns the value of the '<em><b>Set Wellformedness To Oclets</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Set Wellformedness To Oclets</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Set Wellformedness To Oclets</em>' attribute.
	 * @see #setSetWellformednessToOclets(boolean)
	 * @see hub.top.adaptiveSystem.AdaptiveSystemPackage#getAdaptiveSystem_SetWellformednessToOclets()
	 * @model required="true"
	 * @generated
	 */
	boolean isSetWellformednessToOclets();

	/**
	 * Sets the value of the '{@link hub.top.adaptiveSystem.AdaptiveSystem#isSetWellformednessToOclets <em>Set Wellformedness To Oclets</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Set Wellformedness To Oclets</em>' attribute.
	 * @see #isSetWellformednessToOclets()
	 * @generated
	 */
	void setSetWellformednessToOclets(boolean value);

} // AdaptiveSystem
