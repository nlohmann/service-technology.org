/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package hub.top.editor.ptnetLoLA;

import org.eclipse.emf.common.util.EList;

import org.eclipse.emf.ecore.EObject;

/**
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Node</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link hub.top.editor.ptnetLoLA.Node#getName <em>Name</em>}</li>
 *   <li>{@link hub.top.editor.ptnetLoLA.Node#getAnnotation <em>Annotation</em>}</li>
 *   <li>{@link hub.top.editor.ptnetLoLA.Node#getIncoming <em>Incoming</em>}</li>
 *   <li>{@link hub.top.editor.ptnetLoLA.Node#getOutgoing <em>Outgoing</em>}</li>
 *   <li>{@link hub.top.editor.ptnetLoLA.Node#getType <em>Type</em>}</li>
 * </ul>
 * </p>
 *
 * @see hub.top.editor.ptnetLoLA.PtnetLoLAPackage#getNode()
 * @model
 * @generated
 */
public interface Node extends EObject {
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
   * @see hub.top.editor.ptnetLoLA.PtnetLoLAPackage#getNode_Name()
   * @model
   * @generated
   */
	String getName();

	/**
   * Sets the value of the '{@link hub.top.editor.ptnetLoLA.Node#getName <em>Name</em>}' attribute.
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @param value the new value of the '<em>Name</em>' attribute.
   * @see #getName()
   * @generated
   */
	void setName(String value);

	/**
   * Returns the value of the '<em><b>Annotation</b></em>' containment reference.
   * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Annotation</em>' containment reference isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
   * @return the value of the '<em>Annotation</em>' containment reference.
   * @see #setAnnotation(Annotation)
   * @see hub.top.editor.ptnetLoLA.PtnetLoLAPackage#getNode_Annotation()
   * @model containment="true"
   * @generated
   */
	Annotation getAnnotation();

	/**
   * Sets the value of the '{@link hub.top.editor.ptnetLoLA.Node#getAnnotation <em>Annotation</em>}' containment reference.
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @param value the new value of the '<em>Annotation</em>' containment reference.
   * @see #getAnnotation()
   * @generated
   */
	void setAnnotation(Annotation value);

	/**
   * Returns the value of the '<em><b>Incoming</b></em>' reference list.
   * The list contents are of type {@link hub.top.editor.ptnetLoLA.Arc}.
   * It is bidirectional and its opposite is '{@link hub.top.editor.ptnetLoLA.Arc#getTarget <em>Target</em>}'.
   * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Incoming</em>' reference list isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
   * @return the value of the '<em>Incoming</em>' reference list.
   * @see hub.top.editor.ptnetLoLA.PtnetLoLAPackage#getNode_Incoming()
   * @see hub.top.editor.ptnetLoLA.Arc#getTarget
   * @model opposite="target" changeable="false" derived="true"
   * @generated
   */
	EList<Arc> getIncoming();

	/**
   * Returns the value of the '<em><b>Outgoing</b></em>' reference list.
   * The list contents are of type {@link hub.top.editor.ptnetLoLA.Arc}.
   * It is bidirectional and its opposite is '{@link hub.top.editor.ptnetLoLA.Arc#getSource <em>Source</em>}'.
   * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Outgoing</em>' reference list isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
   * @return the value of the '<em>Outgoing</em>' reference list.
   * @see hub.top.editor.ptnetLoLA.PtnetLoLAPackage#getNode_Outgoing()
   * @see hub.top.editor.ptnetLoLA.Arc#getSource
   * @model opposite="source" changeable="false" derived="true"
   * @generated
   */
	EList<Arc> getOutgoing();

	/**
   * Returns the value of the '<em><b>Type</b></em>' attribute.
   * The default value is <code>"0"</code>.
   * The literals are from the enumeration {@link hub.top.editor.ptnetLoLA.NodeType}.
   * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Type</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
   * @return the value of the '<em>Type</em>' attribute.
   * @see hub.top.editor.ptnetLoLA.NodeType
   * @see #setType(NodeType)
   * @see hub.top.editor.ptnetLoLA.PtnetLoLAPackage#getNode_Type()
   * @model default="0" required="true"
   * @generated
   */
	NodeType getType();

	/**
   * Sets the value of the '{@link hub.top.editor.ptnetLoLA.Node#getType <em>Type</em>}' attribute.
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @param value the new value of the '<em>Type</em>' attribute.
   * @see hub.top.editor.ptnetLoLA.NodeType
   * @see #getType()
   * @generated
   */
	void setType(NodeType value);

} // Node
