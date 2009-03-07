/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package hub.top.editor.ptnetLoLA;


/**
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Place</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link hub.top.editor.ptnetLoLA.Place#getToken <em>Token</em>}</li>
 *   <li>{@link hub.top.editor.ptnetLoLA.Place#getFinalMarking <em>Final Marking</em>}</li>
 * </ul>
 * </p>
 *
 * @see hub.top.editor.ptnetLoLA.PtnetLoLAPackage#getPlace()
 * @model
 * @generated
 */
public interface Place extends Node {
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
	 * @see hub.top.editor.ptnetLoLA.PtnetLoLAPackage#getPlace_Token()
	 * @model default="0" required="true"
	 * @generated
	 */
	int getToken();

	/**
	 * Sets the value of the '{@link hub.top.editor.ptnetLoLA.Place#getToken <em>Token</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Token</em>' attribute.
	 * @see #getToken()
	 * @generated
	 */
	void setToken(int value);

	/**
	 * Returns the value of the '<em><b>Final Marking</b></em>' attribute.
	 * The default value is <code>"0"</code>.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Final Marking</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Final Marking</em>' attribute.
	 * @see #setFinalMarking(int)
	 * @see hub.top.editor.ptnetLoLA.PtnetLoLAPackage#getPlace_FinalMarking()
	 * @model default="0" required="true" transient="true" volatile="true" derived="true"
	 * @generated
	 */
	int getFinalMarking();

	/**
	 * Sets the value of the '{@link hub.top.editor.ptnetLoLA.Place#getFinalMarking <em>Final Marking</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Final Marking</em>' attribute.
	 * @see #getFinalMarking()
	 * @generated
	 */
	void setFinalMarking(int value);

} // Place
