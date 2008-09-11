/**
 * Humboldt Universität zu Berlin, Thoery of Programming
 *
 * $Id$
 */
package hub.top.adaptiveSystem;

import org.eclipse.emf.ecore.EObject;

/**
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Oclet</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link hub.top.adaptiveSystem.Oclet#getName <em>Name</em>}</li>
 *   <li>{@link hub.top.adaptiveSystem.Oclet#getOrientation <em>Orientation</em>}</li>
 *   <li>{@link hub.top.adaptiveSystem.Oclet#getQuantor <em>Quantor</em>}</li>
 *   <li>{@link hub.top.adaptiveSystem.Oclet#getPreNet <em>Pre Net</em>}</li>
 *   <li>{@link hub.top.adaptiveSystem.Oclet#getDoNet <em>Do Net</em>}</li>
 *   <li>{@link hub.top.adaptiveSystem.Oclet#isWellFormed <em>Well Formed</em>}</li>
 * </ul>
 * </p>
 *
 * @see hub.top.adaptiveSystem.AdaptiveSystemPackage#getOclet()
 * @model
 * @generated
 */
public interface Oclet extends EObject {
	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	String copyright = "Humboldt Universit\u00e4t zu Berlin, Thoery of Programming";

	/**
	 * Returns the value of the '<em><b>Name</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Name</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Name</em>' attribute.
	 * @see #setName(String)
	 * @see hub.top.adaptiveSystem.AdaptiveSystemPackage#getOclet_Name()
	 * @model required="true"
	 * @generated
	 */
	String getName();

	/**
	 * Sets the value of the '{@link hub.top.adaptiveSystem.Oclet#getName <em>Name</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Name</em>' attribute.
	 * @see #getName()
	 * @generated
	 */
	void setName(String value);

	/**
	 * Returns the value of the '<em><b>Orientation</b></em>' attribute.
	 * The literals are from the enumeration {@link hub.top.adaptiveSystem.Orientation}.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Orientation</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Orientation</em>' attribute.
	 * @see hub.top.adaptiveSystem.Orientation
	 * @see #setOrientation(Orientation)
	 * @see hub.top.adaptiveSystem.AdaptiveSystemPackage#getOclet_Orientation()
	 * @model required="true"
	 * @generated
	 */
	Orientation getOrientation();

	/**
	 * Sets the value of the '{@link hub.top.adaptiveSystem.Oclet#getOrientation <em>Orientation</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Orientation</em>' attribute.
	 * @see hub.top.adaptiveSystem.Orientation
	 * @see #getOrientation()
	 * @generated
	 */
	void setOrientation(Orientation value);

	/**
	 * Returns the value of the '<em><b>Quantor</b></em>' attribute.
	 * The literals are from the enumeration {@link hub.top.adaptiveSystem.Quantor}.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Quantor</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Quantor</em>' attribute.
	 * @see hub.top.adaptiveSystem.Quantor
	 * @see #setQuantor(Quantor)
	 * @see hub.top.adaptiveSystem.AdaptiveSystemPackage#getOclet_Quantor()
	 * @model required="true"
	 * @generated
	 */
	Quantor getQuantor();

	/**
	 * Sets the value of the '{@link hub.top.adaptiveSystem.Oclet#getQuantor <em>Quantor</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Quantor</em>' attribute.
	 * @see hub.top.adaptiveSystem.Quantor
	 * @see #getQuantor()
	 * @generated
	 */
	void setQuantor(Quantor value);

	/**
	 * Returns the value of the '<em><b>Pre Net</b></em>' containment reference.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Pre Net</em>' containment reference isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Pre Net</em>' containment reference.
	 * @see #setPreNet(PreNet)
	 * @see hub.top.adaptiveSystem.AdaptiveSystemPackage#getOclet_PreNet()
	 * @model containment="true" required="true"
	 * @generated
	 */
	PreNet getPreNet();

	/**
	 * Sets the value of the '{@link hub.top.adaptiveSystem.Oclet#getPreNet <em>Pre Net</em>}' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Pre Net</em>' containment reference.
	 * @see #getPreNet()
	 * @generated
	 */
	void setPreNet(PreNet value);

	/**
	 * Returns the value of the '<em><b>Do Net</b></em>' containment reference.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Do Net</em>' containment reference isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Do Net</em>' containment reference.
	 * @see #setDoNet(DoNet)
	 * @see hub.top.adaptiveSystem.AdaptiveSystemPackage#getOclet_DoNet()
	 * @model containment="true" required="true"
	 * @generated
	 */
	DoNet getDoNet();

	/**
	 * Sets the value of the '{@link hub.top.adaptiveSystem.Oclet#getDoNet <em>Do Net</em>}' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Do Net</em>' containment reference.
	 * @see #getDoNet()
	 * @generated
	 */
	void setDoNet(DoNet value);

	/**
	 * Returns the value of the '<em><b>Well Formed</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Well Formed</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Well Formed</em>' attribute.
	 * @see #setWellFormed(boolean)
	 * @see hub.top.adaptiveSystem.AdaptiveSystemPackage#getOclet_WellFormed()
	 * @model required="true"
	 * @generated
	 */
	boolean isWellFormed();

	/**
	 * Sets the value of the '{@link hub.top.adaptiveSystem.Oclet#isWellFormed <em>Well Formed</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Well Formed</em>' attribute.
	 * @see #isWellFormed()
	 * @generated
	 */
	void setWellFormed(boolean value);

} // Oclet
