/**
 * Humboldt Universität zu Berlin, Theory of Programming
 *
 * $Id$
 */
package hub.top.adaptiveSystem;

import org.eclipse.emf.common.util.EList;

/**
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Condition</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link hub.top.adaptiveSystem.Condition#getPreEvents <em>Pre Events</em>}</li>
 *   <li>{@link hub.top.adaptiveSystem.Condition#getPostEvents <em>Post Events</em>}</li>
 *   <li>{@link hub.top.adaptiveSystem.Condition#getToken <em>Token</em>}</li>
 *   <li>{@link hub.top.adaptiveSystem.Condition#isMarked <em>Marked</em>}</li>
 *   <li>{@link hub.top.adaptiveSystem.Condition#isMaximal <em>Maximal</em>}</li>
 *   <li>{@link hub.top.adaptiveSystem.Condition#isMinimal <em>Minimal</em>}</li>
 *   <li>{@link hub.top.adaptiveSystem.Condition#getIncoming <em>Incoming</em>}</li>
 *   <li>{@link hub.top.adaptiveSystem.Condition#getOutgoing <em>Outgoing</em>}</li>
 * </ul>
 * </p>
 *
 * @see hub.top.adaptiveSystem.AdaptiveSystemPackage#getCondition()
 * @model
 * @generated
 */
public interface Condition extends Node {
	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	String copyright = "Humboldt Universit\u00e4t zu Berlin, Theory of Programming";

	/**
	 * Returns the value of the '<em><b>Pre Events</b></em>' reference list.
	 * The list contents are of type {@link hub.top.adaptiveSystem.Event}.
	 * It is bidirectional and its opposite is '{@link hub.top.adaptiveSystem.Event#getPostConditions <em>Post Conditions</em>}'.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Pre Events</em>' reference list isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Pre Events</em>' reference list.
	 * @see hub.top.adaptiveSystem.AdaptiveSystemPackage#getCondition_PreEvents()
	 * @see hub.top.adaptiveSystem.Event#getPostConditions
	 * @model opposite="postConditions" suppressedSetVisibility="true"
	 * @generated
	 */
	EList<Event> getPreEvents();

	/**
	 * Returns the value of the '<em><b>Post Events</b></em>' reference list.
	 * The list contents are of type {@link hub.top.adaptiveSystem.Event}.
	 * It is bidirectional and its opposite is '{@link hub.top.adaptiveSystem.Event#getPreConditions <em>Pre Conditions</em>}'.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Post Events</em>' reference list isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Post Events</em>' reference list.
	 * @see hub.top.adaptiveSystem.AdaptiveSystemPackage#getCondition_PostEvents()
	 * @see hub.top.adaptiveSystem.Event#getPreConditions
	 * @model opposite="preConditions" suppressedSetVisibility="true"
	 * @generated
	 */
	EList<Event> getPostEvents();

	/**
	 * Returns the value of the '<em><b>Token</b></em>' attribute.
	 * The default value is <code>"0"</code>.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Token</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Token</em>' attribute.
	 * @see #setToken(int)
	 * @see hub.top.adaptiveSystem.AdaptiveSystemPackage#getCondition_Token()
	 * @model default="0" required="true"
	 * @generated
	 */
	int getToken();

	/**
	 * Sets the value of the '{@link hub.top.adaptiveSystem.Condition#getToken <em>Token</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Token</em>' attribute.
	 * @see #getToken()
	 * @generated
	 */
	void setToken(int value);

	/**
	 * Returns the value of the '<em><b>Marked</b></em>' attribute.
	 * The default value is <code>"true"</code>.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Marked</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Marked</em>' attribute.
	 * @see hub.top.adaptiveSystem.AdaptiveSystemPackage#getCondition_Marked()
	 * @model default="true" unique="false" required="true" transient="true" changeable="false" volatile="true" derived="true"
	 * @generated
	 */
	boolean isMarked();

	/**
	 * Returns the value of the '<em><b>Maximal</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Maximal</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Maximal</em>' attribute.
	 * @see #isSetMaximal()
	 * @see hub.top.adaptiveSystem.AdaptiveSystemPackage#getCondition_Maximal()
	 * @model unique="false" unsettable="true" required="true" transient="true" changeable="false" volatile="true" derived="true"
	 * @generated
	 */
	boolean isMaximal();

	/**
	 * Returns whether the value of the '{@link hub.top.adaptiveSystem.Condition#isMaximal <em>Maximal</em>}' attribute is set.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return whether the value of the '<em>Maximal</em>' attribute is set.
	 * @see #isMaximal()
	 * @generated
	 */
	boolean isSetMaximal();

	/**
	 * Returns the value of the '<em><b>Minimal</b></em>' attribute.
	 * The default value is <code>"true"</code>.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Minimal</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Minimal</em>' attribute.
	 * @see hub.top.adaptiveSystem.AdaptiveSystemPackage#getCondition_Minimal()
	 * @model default="true" required="true" transient="true" changeable="false" volatile="true" derived="true"
	 * @generated
	 */
	boolean isMinimal();

	/**
	 * Returns the value of the '<em><b>Incoming</b></em>' reference list.
	 * The list contents are of type {@link hub.top.adaptiveSystem.ArcToCondition}.
	 * It is bidirectional and its opposite is '{@link hub.top.adaptiveSystem.ArcToCondition#getDestination <em>Destination</em>}'.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Incoming</em>' reference list isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Incoming</em>' reference list.
	 * @see hub.top.adaptiveSystem.AdaptiveSystemPackage#getCondition_Incoming()
	 * @see hub.top.adaptiveSystem.ArcToCondition#getDestination
	 * @model opposite="destination"
	 * @generated
	 */
	EList<ArcToCondition> getIncoming();

	/**
	 * Returns the value of the '<em><b>Outgoing</b></em>' reference list.
	 * The list contents are of type {@link hub.top.adaptiveSystem.ArcToEvent}.
	 * It is bidirectional and its opposite is '{@link hub.top.adaptiveSystem.ArcToEvent#getSource <em>Source</em>}'.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Outgoing</em>' reference list isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Outgoing</em>' reference list.
	 * @see hub.top.adaptiveSystem.AdaptiveSystemPackage#getCondition_Outgoing()
	 * @see hub.top.adaptiveSystem.ArcToEvent#getSource
	 * @model opposite="source"
	 * @generated
	 */
	EList<ArcToEvent> getOutgoing();

} // Condition
