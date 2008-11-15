/**
 * Humboldt Universität zu Berlin, Theory of Programming
 *
 * $Id$
 */
package hub.top.adaptiveSystem;

import org.eclipse.emf.common.util.EList;

/**
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Event</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link hub.top.adaptiveSystem.Event#getPreConditions <em>Pre Conditions</em>}</li>
 *   <li>{@link hub.top.adaptiveSystem.Event#getPostConditions <em>Post Conditions</em>}</li>
 *   <li>{@link hub.top.adaptiveSystem.Event#isSaturated <em>Saturated</em>}</li>
 *   <li>{@link hub.top.adaptiveSystem.Event#isEnabled <em>Enabled</em>}</li>
 *   <li>{@link hub.top.adaptiveSystem.Event#getIncoming <em>Incoming</em>}</li>
 *   <li>{@link hub.top.adaptiveSystem.Event#getOutgoing <em>Outgoing</em>}</li>
 * </ul>
 * </p>
 *
 * @see hub.top.adaptiveSystem.AdaptiveSystemPackage#getEvent()
 * @model
 * @generated
 */
public interface Event extends Node {
	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	String copyright = "Humboldt Universit\u00e4t zu Berlin, Theory of Programming";

	/**
	 * Returns the value of the '<em><b>Pre Conditions</b></em>' reference list.
	 * The list contents are of type {@link hub.top.adaptiveSystem.Condition}.
	 * It is bidirectional and its opposite is '{@link hub.top.adaptiveSystem.Condition#getPostEvents <em>Post Events</em>}'.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Pre Conditions</em>' reference list isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Pre Conditions</em>' reference list.
	 * @see hub.top.adaptiveSystem.AdaptiveSystemPackage#getEvent_PreConditions()
	 * @see hub.top.adaptiveSystem.Condition#getPostEvents
	 * @model opposite="postEvents" suppressedSetVisibility="true"
	 * @generated
	 */
	EList<Condition> getPreConditions();

	/**
	 * Returns the value of the '<em><b>Post Conditions</b></em>' reference list.
	 * The list contents are of type {@link hub.top.adaptiveSystem.Condition}.
	 * It is bidirectional and its opposite is '{@link hub.top.adaptiveSystem.Condition#getPreEvents <em>Pre Events</em>}'.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Post Conditions</em>' reference list isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Post Conditions</em>' reference list.
	 * @see hub.top.adaptiveSystem.AdaptiveSystemPackage#getEvent_PostConditions()
	 * @see hub.top.adaptiveSystem.Condition#getPreEvents
	 * @model opposite="preEvents" suppressedSetVisibility="true"
	 * @generated
	 */
	EList<Condition> getPostConditions();

	/**
	 * Returns the value of the '<em><b>Saturated</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Saturated</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Saturated</em>' attribute.
	 * @see hub.top.adaptiveSystem.AdaptiveSystemPackage#getEvent_Saturated()
	 * @model required="true" transient="true" changeable="false" volatile="true" derived="true"
	 * @generated
	 */
	boolean isSaturated();

	/**
	 * Returns the value of the '<em><b>Enabled</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Enabled</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Enabled</em>' attribute.
	 * @see #setEnabled(boolean)
	 * @see hub.top.adaptiveSystem.AdaptiveSystemPackage#getEvent_Enabled()
	 * @model required="true"
	 * @generated
	 */
	boolean isEnabled();

	/**
	 * Sets the value of the '{@link hub.top.adaptiveSystem.Event#isEnabled <em>Enabled</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Enabled</em>' attribute.
	 * @see #isEnabled()
	 * @generated
	 */
	void setEnabled(boolean value);

	/**
	 * Returns the value of the '<em><b>Incoming</b></em>' reference list.
	 * The list contents are of type {@link hub.top.adaptiveSystem.ArcToEvent}.
	 * It is bidirectional and its opposite is '{@link hub.top.adaptiveSystem.ArcToEvent#getDestination <em>Destination</em>}'.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Incoming</em>' reference list isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Incoming</em>' reference list.
	 * @see hub.top.adaptiveSystem.AdaptiveSystemPackage#getEvent_Incoming()
	 * @see hub.top.adaptiveSystem.ArcToEvent#getDestination
	 * @model opposite="destination"
	 * @generated
	 */
	EList<ArcToEvent> getIncoming();

	/**
	 * Returns the value of the '<em><b>Outgoing</b></em>' reference list.
	 * The list contents are of type {@link hub.top.adaptiveSystem.ArcToCondition}.
	 * It is bidirectional and its opposite is '{@link hub.top.adaptiveSystem.ArcToCondition#getSource <em>Source</em>}'.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Outgoing</em>' reference list isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Outgoing</em>' reference list.
	 * @see hub.top.adaptiveSystem.AdaptiveSystemPackage#getEvent_Outgoing()
	 * @see hub.top.adaptiveSystem.ArcToCondition#getSource
	 * @model opposite="source"
	 * @generated
	 */
	EList<ArcToCondition> getOutgoing();

} // Event
