/**
 * Humboldt Universität zu Berlin, Thoery of Programming
 *
 * $Id$
 */
package hub.top.adaptiveSystem;


/**
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Arc To Condition</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link hub.top.adaptiveSystem.ArcToCondition#getSource <em>Source</em>}</li>
 *   <li>{@link hub.top.adaptiveSystem.ArcToCondition#getDestination <em>Destination</em>}</li>
 * </ul>
 * </p>
 *
 * @see hub.top.adaptiveSystem.AdaptiveSystemPackage#getArcToCondition()
 * @model
 * @generated
 */
public interface ArcToCondition extends Arc {
	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	String copyright = "Humboldt Universit\u00e4t zu Berlin, Thoery of Programming";

	/**
	 * Returns the value of the '<em><b>Source</b></em>' reference.
	 * It is bidirectional and its opposite is '{@link hub.top.adaptiveSystem.Event#getOutgoing <em>Outgoing</em>}'.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Source</em>' reference isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Source</em>' reference.
	 * @see #setSource(Event)
	 * @see hub.top.adaptiveSystem.AdaptiveSystemPackage#getArcToCondition_Source()
	 * @see hub.top.adaptiveSystem.Event#getOutgoing
	 * @model opposite="outgoing"
	 * @generated
	 */
	Event getSource();

	/**
	 * Sets the value of the '{@link hub.top.adaptiveSystem.ArcToCondition#getSource <em>Source</em>}' reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Source</em>' reference.
	 * @see #getSource()
	 * @generated
	 */
	void setSource(Event value);

	/**
	 * Returns the value of the '<em><b>Destination</b></em>' reference.
	 * It is bidirectional and its opposite is '{@link hub.top.adaptiveSystem.Condition#getIncoming <em>Incoming</em>}'.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Destination</em>' reference isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Destination</em>' reference.
	 * @see #setDestination(Condition)
	 * @see hub.top.adaptiveSystem.AdaptiveSystemPackage#getArcToCondition_Destination()
	 * @see hub.top.adaptiveSystem.Condition#getIncoming
	 * @model opposite="incoming"
	 * @generated
	 */
	Condition getDestination();

	/**
	 * Sets the value of the '{@link hub.top.adaptiveSystem.ArcToCondition#getDestination <em>Destination</em>}' reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Destination</em>' reference.
	 * @see #getDestination()
	 * @generated
	 */
	void setDestination(Condition value);

} // ArcToCondition
