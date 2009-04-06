/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package hub.top.editor.ptnetLoLA;


/**
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Ref Marked Place</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link hub.top.editor.ptnetLoLA.RefMarkedPlace#getToken <em>Token</em>}</li>
 * </ul>
 * </p>
 *
 * @see hub.top.editor.ptnetLoLA.PtnetLoLAPackage#getRefMarkedPlace()
 * @model
 * @generated
 */
public interface RefMarkedPlace extends PlaceReference {
	/**
   * Returns the value of the '<em><b>Token</b></em>' attribute.
   * The default value is <code>"1"</code>.
   * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Token</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
   * @return the value of the '<em>Token</em>' attribute.
   * @see #setToken(int)
   * @see hub.top.editor.ptnetLoLA.PtnetLoLAPackage#getRefMarkedPlace_Token()
   * @model default="1" required="true"
   * @generated
   */
	int getToken();

	/**
   * Sets the value of the '{@link hub.top.editor.ptnetLoLA.RefMarkedPlace#getToken <em>Token</em>}' attribute.
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @param value the new value of the '<em>Token</em>' attribute.
   * @see #getToken()
   * @generated
   */
	void setToken(int value);

} // RefMarkedPlace
