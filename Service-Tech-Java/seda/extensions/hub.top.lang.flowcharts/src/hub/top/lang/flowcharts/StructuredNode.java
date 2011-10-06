/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package hub.top.lang.flowcharts;

import org.eclipse.emf.common.util.EList;

/**
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Structured Node</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link hub.top.lang.flowcharts.StructuredNode#getChildNodes <em>Child Nodes</em>}</li>
 * </ul>
 * </p>
 *
 * @see hub.top.lang.flowcharts.FlowPackage#getStructuredNode()
 * @model abstract="true"
 * @generated
 */
public interface StructuredNode extends ControlFlowNode {
	/**
	 * Returns the value of the '<em><b>Child Nodes</b></em>' containment reference list.
	 * The list contents are of type {@link hub.top.lang.flowcharts.DiagramNode}.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Child Nodes</em>' containment reference list isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Child Nodes</em>' containment reference list.
	 * @see hub.top.lang.flowcharts.FlowPackage#getStructuredNode_ChildNodes()
	 * @model containment="true" required="true"
	 * @generated
	 */
	EList<DiagramNode> getChildNodes();

} // StructuredNode
