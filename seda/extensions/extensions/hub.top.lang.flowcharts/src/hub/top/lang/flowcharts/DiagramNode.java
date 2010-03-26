/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package hub.top.lang.flowcharts;

import org.eclipse.emf.common.util.EList;

import org.eclipse.emf.ecore.EObject;

/**
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Diagram Node</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link hub.top.lang.flowcharts.DiagramNode#getLabel <em>Label</em>}</li>
 *   <li>{@link hub.top.lang.flowcharts.DiagramNode#getIncoming <em>Incoming</em>}</li>
 *   <li>{@link hub.top.lang.flowcharts.DiagramNode#getOutgoing <em>Outgoing</em>}</li>
 * </ul>
 * </p>
 *
 * @see hub.top.lang.flowcharts.FlowPackage#getDiagramNode()
 * @model abstract="true"
 * @generated
 */
public interface DiagramNode extends EObject {
	/**
	 * Returns the value of the '<em><b>Label</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Label</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Label</em>' attribute.
	 * @see #setLabel(String)
	 * @see hub.top.lang.flowcharts.FlowPackage#getDiagramNode_Label()
	 * @model
	 * @generated
	 */
	String getLabel();

	/**
	 * Sets the value of the '{@link hub.top.lang.flowcharts.DiagramNode#getLabel <em>Label</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Label</em>' attribute.
	 * @see #getLabel()
	 * @generated
	 */
	void setLabel(String value);

	/**
	 * Returns the value of the '<em><b>Incoming</b></em>' reference list.
	 * The list contents are of type {@link hub.top.lang.flowcharts.DiagramArc}.
	 * It is bidirectional and its opposite is '{@link hub.top.lang.flowcharts.DiagramArc#getTarget <em>Target</em>}'.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Incoming</em>' reference list isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Incoming</em>' reference list.
	 * @see hub.top.lang.flowcharts.FlowPackage#getDiagramNode_Incoming()
	 * @see hub.top.lang.flowcharts.DiagramArc#getTarget
	 * @model opposite="target"
	 * @generated
	 */
	EList<DiagramArc> getIncoming();

	/**
	 * Returns the value of the '<em><b>Outgoing</b></em>' reference list.
	 * The list contents are of type {@link hub.top.lang.flowcharts.DiagramArc}.
	 * It is bidirectional and its opposite is '{@link hub.top.lang.flowcharts.DiagramArc#getSrc <em>Src</em>}'.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Outgoing</em>' reference list isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Outgoing</em>' reference list.
	 * @see hub.top.lang.flowcharts.FlowPackage#getDiagramNode_Outgoing()
	 * @see hub.top.lang.flowcharts.DiagramArc#getSrc
	 * @model opposite="src"
	 * @generated
	 */
	EList<DiagramArc> getOutgoing();

} // DiagramNode
