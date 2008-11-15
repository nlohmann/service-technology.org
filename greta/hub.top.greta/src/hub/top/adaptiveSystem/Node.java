/**
 * Humboldt Universität zu Berlin, Theory of Programming
 *
 * $Id$
 */
package hub.top.adaptiveSystem;

import org.eclipse.emf.ecore.EObject;

/**
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Node</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link hub.top.adaptiveSystem.Node#getName <em>Name</em>}</li>
 *   <li>{@link hub.top.adaptiveSystem.Node#isAbstract <em>Abstract</em>}</li>
 *   <li>{@link hub.top.adaptiveSystem.Node#getTemp <em>Temp</em>}</li>
 *   <li>{@link hub.top.adaptiveSystem.Node#isDisabledByAntiOclet <em>Disabled By Anti Oclet</em>}</li>
 *   <li>{@link hub.top.adaptiveSystem.Node#isDisabledByConflict <em>Disabled By Conflict</em>}</li>
 * </ul>
 * </p>
 *
 * @see hub.top.adaptiveSystem.AdaptiveSystemPackage#getNode()
 * @model abstract="true"
 * @generated
 */
public interface Node extends EObject {
	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	String copyright = "Humboldt Universit\u00e4t zu Berlin, Theory of Programming";

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
	 * @see hub.top.adaptiveSystem.AdaptiveSystemPackage#getNode_Name()
	 * @model required="true"
	 * @generated
	 */
	String getName();

	/**
	 * Sets the value of the '{@link hub.top.adaptiveSystem.Node#getName <em>Name</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Name</em>' attribute.
	 * @see #getName()
	 * @generated
	 */
	void setName(String value);

	/**
	 * Returns the value of the '<em><b>Abstract</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Abstract</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Abstract</em>' attribute.
	 * @see #isSetAbstract()
	 * @see #unsetAbstract()
	 * @see #setAbstract(boolean)
	 * @see hub.top.adaptiveSystem.AdaptiveSystemPackage#getNode_Abstract()
	 * @model unsettable="true" required="true"
	 * @generated
	 */
	boolean isAbstract();

	/**
	 * Sets the value of the '{@link hub.top.adaptiveSystem.Node#isAbstract <em>Abstract</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Abstract</em>' attribute.
	 * @see #isSetAbstract()
	 * @see #unsetAbstract()
	 * @see #isAbstract()
	 * @generated
	 */
	void setAbstract(boolean value);

	/**
	 * Unsets the value of the '{@link hub.top.adaptiveSystem.Node#isAbstract <em>Abstract</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #isSetAbstract()
	 * @see #isAbstract()
	 * @see #setAbstract(boolean)
	 * @generated
	 */
	void unsetAbstract();

	/**
	 * Returns whether the value of the '{@link hub.top.adaptiveSystem.Node#isAbstract <em>Abstract</em>}' attribute is set.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return whether the value of the '<em>Abstract</em>' attribute is set.
	 * @see #unsetAbstract()
	 * @see #isAbstract()
	 * @see #setAbstract(boolean)
	 * @generated
	 */
	boolean isSetAbstract();

	/**
	 * Returns the value of the '<em><b>Temp</b></em>' attribute.
	 * The literals are from the enumeration {@link hub.top.adaptiveSystem.Temp}.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Temp</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Temp</em>' attribute.
	 * @see hub.top.adaptiveSystem.Temp
	 * @see #setTemp(Temp)
	 * @see hub.top.adaptiveSystem.AdaptiveSystemPackage#getNode_Temp()
	 * @model unique="false" required="true"
	 * @generated
	 */
	Temp getTemp();

	/**
	 * Sets the value of the '{@link hub.top.adaptiveSystem.Node#getTemp <em>Temp</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Temp</em>' attribute.
	 * @see hub.top.adaptiveSystem.Temp
	 * @see #getTemp()
	 * @generated
	 */
	void setTemp(Temp value);

	/**
	 * Returns the value of the '<em><b>Disabled By Anti Oclet</b></em>' attribute.
	 * The default value is <code>"false"</code>.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Disabled By Anti Oclet</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Disabled By Anti Oclet</em>' attribute.
	 * @see #isSetDisabledByAntiOclet()
	 * @see #unsetDisabledByAntiOclet()
	 * @see #setDisabledByAntiOclet(boolean)
	 * @see hub.top.adaptiveSystem.AdaptiveSystemPackage#getNode_DisabledByAntiOclet()
	 * @model default="false" unsettable="true" required="true"
	 * @generated
	 */
	boolean isDisabledByAntiOclet();

	/**
	 * Sets the value of the '{@link hub.top.adaptiveSystem.Node#isDisabledByAntiOclet <em>Disabled By Anti Oclet</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Disabled By Anti Oclet</em>' attribute.
	 * @see #isSetDisabledByAntiOclet()
	 * @see #unsetDisabledByAntiOclet()
	 * @see #isDisabledByAntiOclet()
	 * @generated
	 */
	void setDisabledByAntiOclet(boolean value);

	/**
	 * Unsets the value of the '{@link hub.top.adaptiveSystem.Node#isDisabledByAntiOclet <em>Disabled By Anti Oclet</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #isSetDisabledByAntiOclet()
	 * @see #isDisabledByAntiOclet()
	 * @see #setDisabledByAntiOclet(boolean)
	 * @generated
	 */
	void unsetDisabledByAntiOclet();

	/**
	 * Returns whether the value of the '{@link hub.top.adaptiveSystem.Node#isDisabledByAntiOclet <em>Disabled By Anti Oclet</em>}' attribute is set.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return whether the value of the '<em>Disabled By Anti Oclet</em>' attribute is set.
	 * @see #unsetDisabledByAntiOclet()
	 * @see #isDisabledByAntiOclet()
	 * @see #setDisabledByAntiOclet(boolean)
	 * @generated
	 */
	boolean isSetDisabledByAntiOclet();

	/**
	 * Returns the value of the '<em><b>Disabled By Conflict</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Disabled By Conflict</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Disabled By Conflict</em>' attribute.
	 * @see #isSetDisabledByConflict()
	 * @see #unsetDisabledByConflict()
	 * @see #setDisabledByConflict(boolean)
	 * @see hub.top.adaptiveSystem.AdaptiveSystemPackage#getNode_DisabledByConflict()
	 * @model unsettable="true" required="true"
	 * @generated
	 */
	boolean isDisabledByConflict();

	/**
	 * Sets the value of the '{@link hub.top.adaptiveSystem.Node#isDisabledByConflict <em>Disabled By Conflict</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Disabled By Conflict</em>' attribute.
	 * @see #isSetDisabledByConflict()
	 * @see #unsetDisabledByConflict()
	 * @see #isDisabledByConflict()
	 * @generated
	 */
	void setDisabledByConflict(boolean value);

	/**
	 * Unsets the value of the '{@link hub.top.adaptiveSystem.Node#isDisabledByConflict <em>Disabled By Conflict</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #isSetDisabledByConflict()
	 * @see #isDisabledByConflict()
	 * @see #setDisabledByConflict(boolean)
	 * @generated
	 */
	void unsetDisabledByConflict();

	/**
	 * Returns whether the value of the '{@link hub.top.adaptiveSystem.Node#isDisabledByConflict <em>Disabled By Conflict</em>}' attribute is set.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return whether the value of the '<em>Disabled By Conflict</em>' attribute is set.
	 * @see #unsetDisabledByConflict()
	 * @see #isDisabledByConflict()
	 * @see #setDisabledByConflict(boolean)
	 * @generated
	 */
	boolean isSetDisabledByConflict();
	
	/**
	 * @author Manja Wolf
	 */
	public EObject eOldContainer();

} // Node
